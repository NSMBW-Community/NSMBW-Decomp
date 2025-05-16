#!/usr/bin/env python3

# configure.py
# Generates the Ninja build file.

import sys

from tools.project_settings import *
from tools.slicelibV2 import *
from tools.utils.ninja_syntax_ex import Writer as NinjaWriter

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
slices: list[SliceFile] = []
for file in SLICEDIR.rglob('*.json'):
    slices.append(load_slice_file(file))

# Ensure correct slice order
slices = sorted(slices, key=lambda x: x.meta.moduleNum)

##############################
# Step 1: Source Compilation #
##############################

# Define CodeWarrior path
writer.variable('cc', f'{sys.executable} {CW_WRAPPER} {CC}' if sys.platform != 'win32' else CC)
writer.newline()

# Define compilation rule
writer.rule('cw',
            command=f'$cc $cflags -c -o $out -MD -gccdep $in -I- -i {INCDIR}',
            depfile='$depfile',
            description='Compile $in')

# Write build command for each source file
for slice_file in slices:
    unit_name = Path(slice_file.meta.fileName).stem
    for slice in slice_file.slices:
        if slice.source:
            o_file = Path(BUILDDIR, 'compiled', unit_name, slice.source).with_suffix('.o')
            ccflags = slice_file.meta.defaultCompilerFlags
            if slice.compilerFlags:
                ccflags = slice.compilerFlags

            writer.build('cw',
                         o_file,
                         SRCDIR.joinpath(slice.source).as_posix(),
                         cflags=ccflags,
                         depfile=o_file.with_suffix('.d'))

writer.flush(Path('build.ninja'))
