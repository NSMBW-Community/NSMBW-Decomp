#!/usr/bin/env python3
# Build script

import argparse
import subprocess
import sys
from pathlib import Path
import re

sys.path.append('tools')

from build_dol import build_dol
from build_rel import build_rel
from color_term import *
from project_settings import *
from slice_dol import slice_dol
from slice_rel import slice_rel
from slicelib import SliceFile, Slice, SliceType, load_slice_file
from elffile import ElfFile, ElfSymtab
from elfconsts import STB, STT

def print_cmd(*nargs, **kwargs):
    global args
    if args.v:
        print(*nargs, **kwargs)


parser = argparse.ArgumentParser(description='Builds the game executables.')
parser.add_argument('-v', help='Prints commands for debugging', action='store_true')
parser.add_argument('--objdiff-o', help='Object file to build for objdiff')
args = parser.parse_args()

def create_decomp_context(slice: Slice, out_path: Path):
    context_headers = []
    header_files = dict()

    include_regex = re.compile(r'#include\s+["<](.*)[">]')
    pragma_once_regex = re.compile(r'#pragma\s+once')

    c_file = open(SRCDIR.joinpath(slice.slice_src), 'r').read()
    context_headers = include_regex.findall(c_file)
    # Expand headers until we no expansions are left
    while True:
        no_changes = True
        for i in range(len(context_headers)):
            h_name = context_headers[i]
            if h_name in header_files:
                continue

            h_file = open(INCDIR.joinpath(h_name)).read()
            header_files[h_name] = pragma_once_regex.sub('', include_regex.sub('', h_file))
            referenced_h_files = [x for x in include_regex.findall(h_file) if x not in context_headers[:i]]
            context_headers = \
                context_headers[:i] + \
                referenced_h_files + \
                context_headers[i:]
            no_changes = False

            # Break since indices have changed and we need to start over
            break

        if no_changes:
            break

    # Only keep first instance of each header
    deduped_headers = []
    for h_name in context_headers:
        if h_name not in deduped_headers:
            deduped_headers.append(h_name)

    open(out_path, 'w').write('\n'.join([header_files[x] for x in deduped_headers]))
    print_success('Created decomp context for', slice.slice_src)


slices: list[SliceFile] = []

for file in SLICEDIR.glob('*.json'):
    with open(file) as sf:
        slices.append(load_slice_file(sf))

# Ensure correct order of slices
slices = sorted(slices, key=lambda x: x.meta.mod_num)

# Step 1: compile sources
for slice_file in slices:
    i = 0
    for slice in slice_file.slices:
        if not args.objdiff_o:
            print(f'Compiling sources for {slice_file.meta.name}... {i + 1}/{len(slice_file.slices)}', end='\r', flush=True)
        i += 1
        unit_name = Path(slice_file.meta.name).stem
        if slice.slice_src:
            o_file = Path(f'{BUILDDIR}/compiled/{unit_name}/{slice.slice_name}').with_suffix('.o')
            if args.objdiff_o:
                objdiff_o_path = BUILDDIR.parent.joinpath(Path(args.objdiff_o))
                if objdiff_o_path.suffix == '.hpp':
                    if o_file != objdiff_o_path.with_suffix(''):
                        continue
                    create_decomp_context(slice, objdiff_o_path)
                    sys.exit()
                elif objdiff_o_path.suffix != '.hpp' and o_file != objdiff_o_path:
                    continue
                o_file = objdiff_o_path
                print("Compiling", slice.slice_src + "...")
            ccflags = slice_file.meta.default_compiler_flags
            if slice.cc_flags:
                ccflags = slice.cc_flags

            Path(o_file).parents[0].mkdir(parents=True, exist_ok=True)

            cmd = [] if sys.platform == 'win32' else ['wine']
            cmd.extend([CC, '-c', *ccflags, SRCDIR.joinpath(slice.slice_src).as_posix()])
            cmd.extend(['-o', o_file])
            cmd.extend(['-I-', '-i', f'{INCDIR}'])
            print_cmd(*cmd)
            out = subprocess.run(cmd)
            if out.returncode != 0:
                sys.exit(out.returncode)
    print_success(f'\nCompiled sources for {slice_file.meta.name}.')

if args.objdiff_o:
    sys.exit()

unresolved_rel_symbols = set()

for slice_file in slices:

    # Step 2: slice main.dol and RELs
    slice_name_stem = Path(slice_file.meta.name).stem
    slice_is_rel = slice_file.meta.type == SliceType.REL

    if slice_is_rel:
        slice_rel(Path(f'{ORIGDIR}/{slice_file.meta.name}'), Path(f'{BUILDDIR}/sliced/{slice_name_stem}'))
    else:
        slice_dol(Path(f'{ORIGDIR}/{slice_file.meta.name}'), Path(f'{BUILDDIR}/sliced/{slice_name_stem}'))
    print_success('Sliced', slice_file.meta.name, end='.\n')

    # Step 3: link object files (.preplf for RELs)
    ldflags_dol = '-proc gekko -fp hard'
    ldflags_rel = '-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r'
    out_file = slice_name_stem + ('.preplf' if slice_is_rel else '.elf')

    # Select files
    file_names = [BUILDDIR / o_file for o_file in slice_file.get_o_files()]

    base_lcf_file: Path = LCF_TEMPLATE_REL if slice_is_rel else LCF_TEMPLATE_DOL
    out_lcf_file = f'{BUILDDIR}/{slice_name_stem}' + ('_preplf.lcf' if slice_is_rel else '.lcf')

    with open(base_lcf_file) as f:
        base_lcf_contents = f.read()

    with open(out_lcf_file, 'w') as f:
        f.write(base_lcf_contents)

        if not slice_is_rel:
            force_files = []
            force_actives = set()
            for slice in slice_file.slices:
                if not slice.slice_src or slice.non_matching:
                    continue

                # We load the compiled ELF file and go through the symbols to check if any of them
                # are in the deadstrip list. If not, we add them to the FORCEACTIVE directive.
                compiled_path = Path(f'{BUILDDIR}/compiled/{slice_name_stem}/{slice.slice_name}')
                elf_file = ElfFile.read(open(compiled_path, 'rb').read())
                symtab: ElfSymtab = elf_file.get_section('.symtab')
                if not any([sym.name in slice_file.deadstrip for sym in symtab.syms]):
                    # All symbols should be included in the output file
                    force_files.append(compiled_path)
                    continue

                # A bit more compilated; we need to add all symbols except for the ones
                # we want to deadstrip to the FORCEACTIVE directive.
                for sym in symtab.syms:
                    if sym.name in slice_file.deadstrip:
                        continue
                    if sym.st_info_type in [STT.STT_FUNC, STT.STT_OBJECT] and sym.st_info_bind == STB.STB_GLOBAL:
                        force_actives.add(sym.name)
                    elif sym.name in slice_file.keep_weak:
                        # Some symbols might not be referenced yet, but we still want to keep them
                        force_actives.add(sym.name)

            f.write('FORCEFILES {\n\t')
            f.write('\n\t'.join(['\\'.join(path.relative_to(PROJECTDIR).parts) for path in force_files])) # The linker requires backslashes
            f.write('\n}\n')

            f.write('FORCEACTIVE {\n\t')
            f.write('\n\t'.join(sorted(force_actives)))
            f.write('\n}\n\n')

    ldflags = ldflags_rel if slice_file.meta.type == SliceType.REL else ldflags_dol
    cmd = [] if sys.platform == 'win32' else ['wine']
    cmd.extend([LD, *ldflags.split(' '), *file_names, '-lcf', out_lcf_file, '-o', f'{BUILDDIR}/{out_file}'])
    print_cmd(*cmd)
    out = subprocess.run(cmd)
    if out.returncode != 0:
        sys.exit(out.returncode)

    if slice_is_rel:
        # We need the unresolved symbols from all RELs so that the linker does not deadstrip them.
        # We do this by checking the symbols in the .preplf with an undefined section index.
        preplf_file: ElfFile = ElfFile.read(open(f'{BUILDDIR}/{out_file}', 'rb').read())
        symtab: ElfSymtab = preplf_file.get_section('.symtab')
        for sym in symtab.syms:
            if sym.st_shndx == 0 and not re.match('R_([0-9a-fA-F]+)_([0-9a-fA-F]+)_([0-9a-fA-F]+)', sym.name):
                unresolved_rel_symbols.add(sym.name)
    print_success('Linked', slice_file.meta.name, end='.\n')

# Step 3.5 (only RELs): Create final PLFs
for slice_file in slices:
    if slice_file.meta.type != SliceType.REL:
        continue

    slice_name_stem = Path(slice_file.meta.name).stem

    file_names = [BUILDDIR / o_file for o_file in slice_file.get_o_files()]

    base_lcf_file: Path = LCF_TEMPLATE_REL if slice_is_rel else LCF_TEMPLATE_DOL
    out_lcf_file = f'{BUILDDIR}/{slice_name_stem}.lcf'

    with open(base_lcf_file) as f:
        base_lcf_contents = f.read()

    unresolved_rel_symbols.add('_prolog')
    unresolved_rel_symbols.add('_epilog')
    unresolved_rel_symbols.add('_unresolved')

    with open(out_lcf_file, 'w') as f:
        f.write(base_lcf_contents)

        f.write('FORCEACTIVE {\n\t')
        f.write('\n\t'.join(sorted(unresolved_rel_symbols)))
        f.write('\n}\n\n')

    out_file = f'{BUILDDIR}/{slice_name_stem}.plf'
    cmd = [] if sys.platform == 'win32' else ['wine']
    ldflags = '-w off -proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -r1 -strip_partial'
    cmd.extend([LD, *ldflags.split(' '), *file_names, '-lcf', out_lcf_file, '-o', out_file])
    print_cmd(*cmd)
    out = subprocess.run(cmd)
    if out.returncode != 0:
        sys.exit(out.returncode)

# Step 4: build main.dol
build_dol(Path(f'{BUILDDIR}/wiimj2d.elf'))

# Step 5: build RELs
fake_path = 'd:\\home\\Project\\WIIMJ2D\\EU\\PRD\\RVL\\bin\\'
out_rel_names = [Path(f'{BUILDDIR}/{x.meta.name.replace(".rel", ".plf").replace(".dol", ".elf")}') for x in slices]
build_rel(out_rel_names[0], out_rel_names[1:], ALIAS_FILE, fake_path)

# Done!
print_success('Successfully built binaries!')
