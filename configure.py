#!/usr/bin/env python3

# configure.py
# Generates the Ninja build file.

import sys
from pathlib import PureWindowsPath
from typing import NamedTuple

sys.path.append('tools')

from project_settings import *
from slicelib import *
from utils.ninja_syntax_ex import Writer as NinjaWriter

# Helper functions

def make_path(slice_file: SliceFile, suffix: str) -> Path:
    return (BUILDDIR / slice_file.meta.fileName).with_suffix(suffix)

def unit_name(slice_file: SliceFile) -> str:
    return Path(slice_file.meta.fileName).stem

def ld_o_files(slice_file: SliceFile) -> list[Path]:
    files = []
    for slice in slice_file.parsed_slices:
        if slice.source and not slice.nonMatching:
            files.append((BUILDDIR / 'compiled' / unit_name(slice_file) / slice.source).with_suffix('.o'))
        else:
            files.append((BUILDDIR / 'sliced' / unit_name(slice_file) / slice.sliceName).with_suffix('.o'))
    return files

def sliced_o_files(slice_file: SliceFile) -> list[Path]:
    files = []
    for slice in slice_file.parsed_slices:
        if not slice.source or slice.nonMatching:
            files.append((BUILDDIR / 'sliced' / unit_name(slice_file) / slice.sliceName).with_suffix('.o'))
    return files

def files_with_suffix(files: list[Path], suffix: str) -> list[Path]:
    return [f.with_suffix(suffix) for f in files]

##############################
# Step 1: Source Compilation #
##############################
def gen_compile_build_statements(writer: NinjaWriter, slices: list[tuple[Path, SliceFile]]):
    for slice_file_path, slice_file in slices:
        compiled_files = []

        for slice in slice_file.parsed_slices:
            compiled_o_file = (BUILDDIR / 'compiled' / unit_name(slice_file) / slice.source).with_suffix('.o')
            if not slice.source:
                continue

            compiled_files.append(compiled_o_file)

            ccflags = slice_file.meta.defaultCompilerFlags if slice.ccFlags == '' else slice.ccFlags

            writer.build('cw',
                         compiled_o_file,
                         (SRCDIR / slice.source).as_posix(),
                         ccflags=ccflags,
                         depfile=compiled_o_file.with_suffix('.d'))

        writer.build('gen_linkerscript',
                     make_path(slice_file, '.lcf'),
                     slice_file_path,
                     implicit_inputs=[str(x) for x in compiled_files])

###########################
# Step 2: DOL build steps #
###########################
def gen_dol_build_statements(writer: NinjaWriter, slices: list[tuple[Path, SliceFile]]):
    for _, slice_file in slices:
        if slice_file.meta.type != SliceType.DOL:
            continue

        # Slice DOL
        writer.build('slice_dol',
                     sliced_o_files(slice_file),
                     ORIGDIR / slice_file.meta.fileName,
                     symbols=SYMBOL_FILE,
                     implicit_inputs=[SYMBOL_FILE])
        # Linked ELF
        writer.build('link',
                     make_path(slice_file, '.elf'),
                     ld_o_files(slice_file),
                     lcf=make_path(slice_file, '.lcf'),
                     ldflags='-proc gekko -fp hard',
                     implicit_inputs=[make_path(slice_file, '.lcf')])
        # Build final DOL
        writer.build('build_dol',
                     make_path(slice_file, '.dol'),
                     make_path(slice_file, '.elf'))

###########################
# Step 3: REL build steps #
###########################
def gen_rel_build_statements(writer: NinjaWriter, slices: list[tuple[Path, SliceFile]]):
    dol_file = Path()
    rel_files = []
    for _, slice_file in slices:
        if slice_file.meta.type == SliceType.DOL:
            dol_file = make_path(slice_file, '.dol')
        else:
            rel_files.append(make_path(slice_file, '.rel'))

    common_lcf = BUILDDIR / 'modules.lcf'
    fake_path = PureWindowsPath('d:/home/Project/WIIMJ2D/EU/PRD/RVL/bin') # Used for the .str file
    str_file = dol_file.with_suffix('.str')

    #################################
    # Step 3.1: Per-REL build steps #
    #################################

    fake_paths = []
    for _, slice_file in slices:
        if slice_file.meta.type != SliceType.REL:
            continue
        # Slice REL
        writer.build('slice_rel',
                     sliced_o_files(slice_file),
                     ORIGDIR / slice_file.meta.fileName,
                     alias_file=ALIAS_FILE,
                     implicit_inputs=[ALIAS_FILE])
        # Non-stripped partially-linked file
        writer.build('link',
                     make_path(slice_file, '.preplf'),
                     ld_o_files(slice_file),
                     lcf=make_path(slice_file, '.lcf'),
                     ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r',
                     implicit_inputs=[make_path(slice_file, '.lcf')])
        # Stripped partially-linked file
        writer.build('link',
                     make_path(slice_file, '.plf'),
                     ld_o_files(slice_file),
                     lcf=common_lcf,
                     ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r1 -strip_partial -w off',
                     implicit_inputs=[common_lcf])
        # Fake PLF for the .str file
        fake_plf_path = PureWindowsPath(fake_path / slice_file.meta.fileName).with_suffix('.plf')
        writer.build('phony', fake_plf_path, make_path(slice_file, '.plf'))
        fake_paths.append(str(fake_plf_path))
        # Build final REL
        writer.build('build_rel',
                     make_path(slice_file, '.rel'),
                     [dol_file.with_suffix('.elf'), *files_with_suffix(rel_files, '.plf'), str_file])

    ####################################
    # Step 3.2: Common REL build steps #
    ####################################

    # Each REL file contains a pointer into a STR file, which contains the names of the modules.
    writer.build('write_str',
                 str_file,
                 fake_paths)

    # RELs need to come together to create the common linker script for the final stripped modules
    # Pick the slice with the largest number of sections as the base
    rel_slices = [x for x in slices if x[1].meta.type == SliceType.REL]
    best_slice = max(rel_slices, key=lambda x: len(x[1].meta.sections))
    writer.build('gen_linkerscript',
                 common_lcf,
                 [best_slice[0], *files_with_suffix(rel_files, '.preplf')])

#############################
# Set up ninja build script #
#############################

# Ninja build variables
writer = NinjaWriter()
writer.variable('builddir', BUILDDIR)
writer.newline()
writer.variable('cc', f'{sys.executable} {CW_WRAPPER} {CC}' if sys.platform != 'win32' else CC)
writer.variable('ld', f'wine {LD}' if sys.platform != 'win32' else LD)
writer.variable('python', f'"{sys.executable}"')
writer.newline()

# Ninja build rules
writer.rule('cw',
            command=f'$cc -c $ccflags $in -o $out -MD -gccdep -I- -i {INCDIR}',
            deps='gcc',
            depfile='$depfile',
            description='Compile $in')

writer.rule('slice_dol',
            command=f'$python tools/slice_dol.py $in -s $symbols',
            description='Slice $in')

writer.rule('slice_rel',
            command=f'$python tools/slice_rel.py $in -a $alias_file',
            description='Slice $in')

writer.rule('gen_linkerscript',
            command=f'$python tools/gen_lcf.py $in -o $out',
            description='Generate $out')

writer.rule('link',
            command=f'$ld $ldflags $in -lcf $lcf -o $out',
            description='Link $out')

writer.rule('build_dol',
            command=f'$python tools/build_dol.py $in -o $out',
            description='Build $out')

writer.rule('write_str',
            command=f'''$python -c "import sys; open(sys.argv[1], 'w').write('\\0'.join(sys.argv[2:])+'\\0')" $out $in''',
            description='Write $out')

writer.rule('build_rel',
            command=f'$python tools/build_rel.py $in -o $out',
            description='Build $out')

writer.rule('configure',
            command=f'$python {sys.argv[0]}',
            generator=True,
            description='Configure $out')

writer.rule('decomp_context',
            command=f'$python tools/decompctx.py $in -o $out',
            depfile='$depfile',
            description='Create decomp.me context for $in')

# Load slice files
slices: list[tuple[Path, SliceFile]] = []
for file in SLICEDIR.rglob('*.json'):
    slices.append((file, load_slice_file(file)))

# Ensure correct slice order
slices = sorted(slices, key=lambda x: x[1].meta.moduleNum)

# Generate build statements
gen_compile_build_statements(writer, slices)
gen_dol_build_statements(writer, slices)
gen_rel_build_statements(writer, slices)

# Decomp.me context generation
for _, slice_file in slices:
    for slice in slice_file.parsed_slices:
        if slice.source:
            context_file = (BUILDDIR / 'decompctx' / unit_name(slice_file) / slice.source).with_suffix('.hpp')
            d_path = (BUILDDIR / 'compiled' / unit_name(slice_file) / slice.source).with_suffix('.d')
            writer.build('decomp_context',
                         context_file,
                         SRCDIR / slice.source,
                         depfile=d_path)

# Regenerate build.ninja on changes to the slices
writer.build('configure',
             'build.ninja',
             [x[0] for x in slices])

# Default targets (final DOL and RELs)
writer.default([str(BUILDDIR / slice.meta.fileName) for _, slice in slices])

writer.flush(Path('build.ninja'))
