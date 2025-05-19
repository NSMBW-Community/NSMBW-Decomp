#!/usr/bin/env python3
# Project settings

import shutil
from pathlib import Path

#######################
# Project directories #
#######################

# Directory for decompiled code
SRCDIR = Path('source')

# Include directory
INCDIR = Path('include')

# Slice directory
SLICEDIR = Path('slices')

# Build artifact directories
BUILDDIR = Path('bin')
BUILDDIR_COMPILED = Path(BUILDDIR, 'compiled')
BUILDDIR_SLICED = Path(BUILDDIR, 'sliced')
BUILDDIR_DECOMPCTX = Path(BUILDDIR, 'decompctx')

# Original binaries directory
ORIGDIR = Path('original')

# Compiler directory
COMPILERDIR = Path('compilers')

# Tool directory
TOOLDIR = Path('tools')

#########
# Files #
#########

# Codewarrior
CODEWARRIOR = Path(COMPILERDIR, 'Wii', '1.1')
CC = Path(CODEWARRIOR, 'mwcceppc.exe')
LD = Path(CODEWARRIOR, 'mwldeppc.exe')

# Tools and scripts
BUILD_DOL = Path(TOOLDIR, 'build_dol.py')
BUILD_REL = Path(TOOLDIR, 'build_rel.py')
CW_WRAPPER = Path(TOOLDIR, 'mwcceppc_wrapper.py')
GEN_LCF = Path(TOOLDIR, 'gen_lcf.py')
SLICE_DOL = Path(TOOLDIR, 'slice_dol.py')
SLICE_REL = Path(TOOLDIR, 'slice_rel.py')

# Relocation aliases
ALIAS_FILE = Path('alias_db.txt')

# Symbols
SYMBOL_FILE = Path('syms.txt')

# Ninja build script
NINJA_BUILD_FILE = Path('build.ninja')

# Windows wrapper for non-Windows OSes
WINDOWS_WRAPPER = 'wibo' if shutil.which('wibo') else 'wine'
