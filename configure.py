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
writer.variable('python', 'python')
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

# Write build command for each source file
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
        writer.build('slice_dol',
                    sliced_files,
                    ORIGDIR / slice_file.meta.fileName)
        writer.build('link',
                    f'{BUILDDIR}/{unit_name}.elf',
                    ld_o_files,
                    lcf=lcf_path,
                    ldflags='-proc gekko -fp hard',
                    implicit_inputs=[lcf_path])
        writer.build('build_dol',
                    f'{BUILDDIR}/{unit_name}.dol',
                    f'{BUILDDIR}/{unit_name}.elf')
    else:
        writer.build('slice_rel',
                    sliced_files,
                    ORIGDIR / slice_file.meta.fileName)
        writer.build('link',
                    f'{BUILDDIR}/{unit_name}.elf',
                    ld_o_files,
                    lcf=lcf_path,
                    ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r',
                    implicit_inputs=[lcf_path])


writer.flush(Path('build.ninja'))
