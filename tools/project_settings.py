#!/usr/bin/env python3
# Project settings

from pathlib import Path

#######################
# Project directories #
#######################

# This project's directory
PROJECTDIR = Path()

# Directory for decompiled code
SRCDIR = PROJECTDIR.joinpath('source')

# Include directory
INCDIR = PROJECTDIR.joinpath('include')

# Slice directory
SLICEDIR = PROJECTDIR.joinpath('slices')

# Build artifacts directory
BUILDDIR = PROJECTDIR.joinpath('bin')

# Original binaries directory
ORIGDIR = PROJECTDIR.joinpath('original')

# Compiler directory
COMPILERDIR = PROJECTDIR.joinpath('compilers')

# Tool directory
TOOLDIR = PROJECTDIR.joinpath('tools')

#########
# Files #
#########

# Codewarrior
CODEWARRIOR = COMPILERDIR.joinpath('Wii', '1.1')
CC = CODEWARRIOR.joinpath('mwcceppc.exe')
LD = CODEWARRIOR.joinpath('mwldeppc.exe')

# Codewarrior wrapper scripts
CW_WRAPPER = Path(TOOLDIR, 'mwcceppc_wine_wrapper.py')

# LCF templates
LCF_TEMPLATE_DOL = PROJECTDIR.joinpath('template_dol.lcf')
LCF_TEMPLATE_REL = PROJECTDIR.joinpath('template_rel.lcf')

# Relocation aliases
ALIAS_FILE = PROJECTDIR.joinpath('alias_db.txt')

# Symbols
SYMBOL_FILE = PROJECTDIR.joinpath('syms.txt')
