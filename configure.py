#!/usr/bin/env python3

# configure.py
# Generates the Ninja build file.

import sys
from pathlib import Path

sys.path.append('tools')

from project_settings import *
from slicelib import *
from utils.ninja_syntax_ex import Writer as NinjaWriter

####################
# Helper Functions #
####################

def get_build_path(slice_file: SliceFile, suffix: str) -> Path:
    return (BUILDDIR / slice_file.meta.fileName).with_suffix(suffix)


def ld_o_files(slice_file: SliceFile) -> list[Path]:
    files = []
    for slice in slice_file.parsed_slices:
        if slice.source and not slice.nonMatching:
            files.append((BUILDDIR_COMPILED / slice_file.unit_name() / slice.source).with_suffix('.o'))
        else:
            files.append((BUILDDIR_SLICED / slice_file.unit_name() / slice.sliceName).with_suffix('.o'))
    return files


def sliced_o_files(slice_file: SliceFile) -> list[Path]:
    files = []
    for slice in slice_file.parsed_slices:
        if not slice.source or slice.nonMatching:
            files.append((BUILDDIR_SLICED / slice_file.unit_name() / slice.sliceName).with_suffix('.o'))
    return files


def files_with_suffix(files: list[Path], suffix: str) -> list[Path]:
    return [f.with_suffix(suffix) for f in files]

##############################
# Step 1: Source Compilation #
##############################

def gen_compile_build_statements(writer: NinjaWriter, slices: list[SliceFile]):
    for slice_file in slices:
        compiled_files = []
        for slice in slice_file.parsed_slices:
            if not slice.source:
                continue

            compiled_o_file = (BUILDDIR_COMPILED / slice_file.unit_name() / slice.source).with_suffix('.o')
            compiled_files.append(compiled_o_file)
            ccflags = slice_file.meta.defaultCompilerFlags if slice.ccFlags == '' else slice.ccFlags
            writer.build('cw',
                         compiled_o_file,
                         (SRCDIR / slice.source).as_posix(),
                         ccflags=ccflags,
                         depfile=compiled_o_file.with_suffix('.d'))

        writer.build('gen_linkerscript',
                     get_build_path(slice_file, '.lcf'),
                     slice_file.path,
                     implicit_inputs=compiled_files)

###########################
# Step 2: DOL build steps #
###########################

def gen_dol_build_statements(writer: NinjaWriter, slices: list[SliceFile]):
    for slice_file in slices:
        if slice_file.meta.type != SliceType.DOL:
            continue

        # Slice DOL
        writer.build('slice_dol',
                     sliced_o_files(slice_file),
                     ORIGDIR / slice_file.meta.fileName,
                     symbols=SYMBOL_FILE,
                     implicit_inputs=[SYMBOL_FILE, slice_file.path],
                     out_dir = BUILDDIR_SLICED)

        # Linked ELF
        writer.build('link',
                     get_build_path(slice_file, '.elf'),
                     ld_o_files(slice_file),
                     lcf=get_build_path(slice_file, '.lcf'),
                     ldflags='-proc gekko -fp hard',
                     implicit_inputs=get_build_path(slice_file, '.lcf'))

        # Build final DOL
        writer.build('build_dol',
                     get_build_path(slice_file, '.dol'),
                     get_build_path(slice_file, '.elf'))

###########################
# Step 3: REL build steps #
###########################

def gen_rel_build_statements(writer: NinjaWriter, slices: list[SliceFile]):
    dol_file = Path()
    rel_files = []
    for slice_file in slices:
        if slice_file.meta.type == SliceType.DOL:
            dol_file = get_build_path(slice_file, '.dol')
        else:
            rel_files.append(get_build_path(slice_file, '.rel'))

    common_lcf = BUILDDIR / 'modules.lcf'
    fake_path = Path('d:/home/Project/WIIMJ2D/EU/PRD/RVL/bin') # Used for the .str file
    str_file = dol_file.with_suffix('.str')

    #################################
    # Step 3.1: Per-REL build steps #
    #################################

    fake_paths = []
    for slice_file in slices:
        if slice_file.meta.type != SliceType.REL:
            continue

        # Slice REL
        writer.build('slice_rel',
                     sliced_o_files(slice_file),
                     ORIGDIR / slice_file.meta.fileName,
                     alias_file=ALIAS_FILE,
                     implicit_inputs=[ALIAS_FILE, slice_file.path],
                     out_dir=BUILDDIR_SLICED)

        # Non-stripped partially-linked file
        writer.build('link',
                     get_build_path(slice_file, '.preplf'),
                     ld_o_files(slice_file),
                     lcf=get_build_path(slice_file, '.lcf'),
                     ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r',
                     implicit_inputs=get_build_path(slice_file, '.lcf'))

        # Stripped partially-linked file
        writer.build('link',
                     get_build_path(slice_file, '.plf'),
                     ld_o_files(slice_file),
                     lcf=common_lcf,
                     ldflags='-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r1 -strip_partial -w off',
                     implicit_inputs=common_lcf)

        # Fake PLF for the .str file
        fake_plf_path = Path(fake_path / slice_file.meta.fileName).with_suffix('.plf')
        fake_paths.append(str(fake_plf_path))

        # Build final REL
        writer.build('build_rel',
                     get_build_path(slice_file, '.rel'),
                     [dol_file.with_suffix('.elf'), *files_with_suffix(rel_files, '.plf'), str_file])

    ####################################
    # Step 3.2: Common REL build steps #
    ####################################

    # Each REL file contains a pointer into a STR file, which contains the names of the modules.
    writer.build('write_str',
                 str_file,
                 paths=fake_paths)

    # RELs need to come together to create the common linker script for the final stripped modules
    # Pick the slice with the largest number of sections as the base
    rel_slices = [x for x in slices if x.meta.type == SliceType.REL]
    best_slice = max(rel_slices, key=lambda x: len(x.meta.sections))
    writer.build('gen_linkerscript',
                 common_lcf,
                 [best_slice.path, *files_with_suffix(rel_files, '.preplf')])

######################
# Build Script Setup #
######################

# Ninja build variables
writer = NinjaWriter()
writer.variable('builddir', BUILDDIR)
writer.newline()
writer.variable('python', f'"{sys.executable}"')
writer.variable('cc', f'$python {CW_WRAPPER} {CC}' if sys.platform != 'win32' else CC)
writer.variable('ld', f'{WINDOWS_WRAPPER} {LD}' if sys.platform != 'win32' else LD)
writer.newline()

# Ninja build rules
inc_dir_str = ' '.join([f'-i {i}' for i in INCDIRS])
writer.rule('cw',
            command=f'$cc -c $ccflags $in -o $out -MD -gccdep -DREVOLUTION -I- {inc_dir_str}',
            deps='gcc',
            depfile='$depfile',
            description='Compile $in')

writer.rule('slice_dol',
            command=f'$python {SLICE_DOL} $in -s $symbols -o $out_dir',
            description='Slice $in')

writer.rule('slice_rel',
            command=f'$python {SLICE_REL} $in -a $alias_file -o $out_dir',
            description='Slice $in')

writer.rule('gen_linkerscript',
            command=f'$python {GEN_LCF} $in -o $out',
            description='Generate $out')

writer.rule('link',
            command='$ld $ldflags $in -lcf $lcf -o $out',
            description='Link $out')

writer.rule('build_dol',
            command=f'$python {BUILD_DOL} $in -o $out',
            description='Build $out')

writer.rule('write_str',
            command='''$python -c "import sys; open(sys.argv[1], 'w').write('\\0'.join([p.replace('/', '\\u005c') for p in sys.argv[2:]])+'\\0')" $out $paths''',
            description='Write $out')

writer.rule('build_rel',
            command=f'$python {BUILD_REL} $in -o $out',
            description='Build $out')

writer.rule('configure',
            command=f'$python {sys.argv[0]}',
            generator=True,
            description='Configure $out')

writer.rule('decomp_context',
            command='$python tools/decompctx.py $in -o $out',
            depfile='$depfile',
            description='Create decomp.me context for $in')

# Load slice files and ensure correct slice order
slices: list[SliceFile] = [load_slice_file(file) for file in SLICEDIR.rglob('*.json')]
slices = sorted(slices, key=lambda x: x.meta.moduleNum)

# Generate build statements
gen_compile_build_statements(writer, slices)
gen_dol_build_statements(writer, slices)
gen_rel_build_statements(writer, slices)

# Decomp.me context generation
for slice_file in slices:
    for slice in slice_file.parsed_slices:
        if slice.source:
            context_file = (BUILDDIR_DECOMPCTX / slice_file.unit_name() / slice.source).with_suffix('.hpp')
            d_path = (BUILDDIR_COMPILED / slice_file.unit_name() / slice.source).with_suffix('.d')
            writer.build('decomp_context',
                         context_file,
                         SRCDIR / slice.source,
                         depfile=d_path)

# Regenerate build.ninja on changes to the slices
writer.build('configure',
             NINJA_BUILD_FILE,
             [x.path for x in slices])

# Default targets (final DOL and RELs)
writer.default([BUILDDIR / slice.meta.fileName for slice in slices])

# Flush the created file
writer.flush(NINJA_BUILD_FILE)
