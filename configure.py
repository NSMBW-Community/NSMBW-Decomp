#!/usr/bin/env python3

# configure.py
# Generates the Ninja build file.

import sys

sys.path.append('tools')

from project_settings import *
from slicelibV2 import *
from utils.ninja_syntax_ex import Writer as NinjaWriter

###################
# Write Variables #
###################

writer = NinjaWriter()
writer.variable('builddir', BUILDDIR)
writer.newline()

########################
# Step 0: Slice Import #
########################

# Load slice files
slices: list[tuple[Path, SliceFile]] = []
for file in SLICEDIR.rglob('*.json'):
    slices.append((file, load_slice_file(file)))

# Ensure correct slice order
slices = sorted(slices, key=lambda x: x[1].meta.moduleNum)

##############################
# Step 1: Source Compilation #
##############################

# Define CodeWarrior path
writer.variable('cc', f'{sys.executable} {CW_WRAPPER} {CC}' if sys.platform != 'win32' else CC)
writer.variable('ld', f'{sys.executable} {CW_WRAPPER} {LD}' if sys.platform != 'win32' else LD)
writer.variable('python', sys.executable)
writer.newline()

# Define compilation rule
writer.rule('cw',
            command=f'$cc -c $cflags $in -o $out -MD -gccdep -I- -i {INCDIR}',
            deps='gcc',
            depfile='$depfile',
            description='Compile $in')

writer.rule('slice_dol',
            command=f'$python tools/slice_dol.py $in',
            description='Slice $in')

writer.rule('slice_rel',
            command=f'$python tools/slice_rel.py $in',
            description='Slice $in')

writer.rule('gen_linkerscript',
            command=f'$python tools/gen_lcf.py $in -o $out',
            description='Generate linker script for $in')

writer.rule('link',
            command=f'$ld $ldflags $in -lcf $lcf -o $out')

writer.rule('build_dol',
            command=f'$python tools/build_dol.py $in -o $out',
            description='Build $out')

writer.rule('build_rel',
            command=f'$python tools/build_rel.py $in -a $alias_file -p $fakepath',
            description='Build $out')

# Write build command for each source file
dol_file = Path()
rel_files: list[Path] = []
for slice_file_path, slice_file in slices:
    sliced_files = []
    compiled_files = []
    ld_o_files = []

    unit_name = Path(slice_file.meta.fileName).stem

    for slice in slice_file.parsed_slices:
        sliced_o_file = (BUILDDIR / 'sliced' / unit_name / slice.sliceName).with_suffix('.o')
        compiled_o_file = (BUILDDIR / 'compiled' / unit_name / slice.source).with_suffix('.o')
        sliced_files.append(sliced_o_file)
        if slice.source:
            compiled_files.append(compiled_o_file)

            ccflags = slice_file.meta.defaultCompilerFlags
            if slice.ccFlags != '':
                ccflags = slice.ccFlags

            writer.build('cw',
                         compiled_o_file,
                         SRCDIR.joinpath(slice.source).as_posix(),
                         cflags=ccflags,
                         depfile=compiled_o_file.with_suffix('.d'))

        if slice.source and not slice.nonMatching:
            ld_o_files.append(compiled_o_file)
        else:
            ld_o_files.append(sliced_o_file)

    lcf_path = (BUILDDIR / unit_name).with_suffix('.lcf')

    writer.build('gen_linkerscript',
                 lcf_path,
                 slice_file_path,
                 implicit_inputs=[str(x) for x in compiled_files])

    if slice_file.meta.type == SliceType.DOL:
        dol_file = BUILDDIR / f'{unit_name}.dol'
        writer.build('slice_dol',
                    sliced_files,
                    ORIGDIR / slice_file.meta.fileName)
        writer.build('link',
                     dol_file.with_suffix('.elf'),
                     ld_o_files,
                     lcf=lcf_path,
                     ldflags='-proc gekko -fp hard',
                     implicit_inputs=[lcf_path])
    else:
        rel_file = BUILDDIR / f'{unit_name}.rel'
        rel_files.append(rel_file)
        writer.build('slice_rel',
                     sliced_files,
                     ORIGDIR / slice_file.meta.fileName)
        writer.build('link',
                     rel_file.with_suffix('.preplf'),
                     ld_o_files,
                     lcf=lcf_path,
                     ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r',
                     implicit_inputs=[lcf_path])
        writer.build('link',
                     rel_file.with_suffix('.plf'),
                     ld_o_files,
                     lcf=BUILDDIR / 'modules.lcf',
                     ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r1 -strip_partial -w off',
                     implicit_inputs=[BUILDDIR / 'modules.lcf'])

# RELs need to come together to create the common linker script for the final stripped modules
# Pick the slice with the largest number of sections as the base
rel_slices = [x for x in slices if x[1].meta.type == SliceType.REL]
best_slice = max(rel_slices, key=lambda x: len(x[1].meta.sections))
preplf_files = [x.with_suffix('.preplf') for x in rel_files]
writer.build('gen_linkerscript',
             BUILDDIR / 'modules.lcf',
             [best_slice[0], *preplf_files])

writer.build('build_dol',
            dol_file,
            dol_file.with_suffix('.elf'))

plf_files = [x.with_suffix('.plf') for x in rel_files]
writer.build('build_rel',
            rel_files,
            [dol_file.with_suffix('.elf'), *plf_files],
            fakepath='d:\\home\\Project\\WIIMJ2D\\EU\\PRD\\RVL\\bin\\',
            alias_file='alias_db.txt')

writer.flush(Path('build.ninja'))
