#!/usr/bin/env python3
# Reimplementation of makerel by RootCubed

import re
from pathlib import Path
from elftools.elf.elffile import ELFFile

from relfile import REL, Relocation, Section
from elfconsts import PPC_RELOC_TYPE
from color_term import *

str_file = ""
id = 1
unresolved_symbol_count = 0

def process_file(modules: list[ELFFile], idx: int, filename: Path, alias_db: dict[str, str], fakedir: str):
    global id, str_file, unresolved_symbol_count

    # Generate .str file and output filename
    str_file_offset = len(str_file)
    if fakedir:
        path_str = f'{fakedir}{filename.name}\0'
    else:
        path_str = f'{filename.resolve()}\0'
    str_file += path_str

    outfile = filename.with_suffix('.rel')

    # Create REL
    rel_file = REL(id, path_offset=str_file_offset, path_size=len(path_str))
    id += 1

    elffile = modules[idx]
    module_relocations: dict[int, list[Relocation]] = {}

    for (sec_idx, section) in enumerate(elffile.iter_sections()):
        if section['sh_type'] != 'SHT_PROGBITS' or section.name == '.comment':
            # Non-text/data sections are added to the REL as empty sections
            empty_sec = Section()

            # Special case: .bss section does get length field set
            if section.name == '.bss':
                empty_sec.is_bss = True
                empty_sec._sec_len = section.data_size
                rel_file.bss_size = section.data_size
                rel_file.bss_align = section['sh_addralign']

            rel_file.add_section(empty_sec)
            continue

        rel_sec = Section()
        rel_sec.set_data(bytearray(section.data()))
        rel_sec.executable = section.name == '.text'
        rel_sec.alignment = section['sh_addralign'] if section['sh_addralign'] > 0 else 4
        
        if section.name == '.text':
            rel_file.align = section['sh_addralign']

        rel_file.add_section(rel_sec)

        rela_sec = elffile.get_section_by_name('.rela' + section.name)
        if not rela_sec:
            continue

        symbol_table = elffile.get_section(rela_sec['sh_link'])

        module_classify: dict[int, list] = {}
        relocs_sorted = []

        for reloc in rela_sec.iter_relocations():
            relocs_sorted.append(reloc)

        relocs_sorted.sort(key=lambda r: r['r_offset'])
        for reloc in relocs_sorted:
            symbol = symbol_table.get_symbol(reloc['r_info_sym'])

            # Find module which contains the symbol
            found_sym = False

            # Try to look up symbol in alias database
            if symbol.name in alias_db:
                symbol.name = alias_db[symbol.name]

            # First check if hardcoded
            m = re.search('R_([0-9a-fA-F]+)_([0-9a-fA-F]+)_([0-9a-fA-F]+)', symbol.name)
            if m:
                #print_warn(f'Relocation symbol {symbol.name} found...')
                mod_num = int(m.group(1), 16)
                out_reloc = Relocation()
                out_reloc.section = int(m.group(2), 16)
                out_reloc.addend = int(m.group(3), 16)
                out_reloc.reloc_type = PPC_RELOC_TYPE(reloc['r_info_type'])
                if mod_num not in module_classify:
                    module_classify[mod_num] = [(reloc, out_reloc)]
                else:
                    module_classify[mod_num].append((reloc, out_reloc))
                found_sym = True

            if not found_sym:
                for (i, mod) in enumerate(modules):
                    symtab = mod.get_section_by_name('.symtab')
                    try_found = symtab.get_symbol_by_name(symbol.name)
                    if try_found and try_found[0]['st_shndx'] != 'SHN_UNDEF':
                        if i not in module_classify:
                            module_classify[i] = [(reloc, try_found[0])]
                        else:
                            module_classify[i].append((reloc, try_found[0]))
                        found_sym = True
                        break
            if not found_sym:
                print_err(f'Error: Symbol {symbol.name} not found!')
                unresolved_symbol_count += 1

        for mod in module_classify:
            if mod not in module_relocations:
                module_relocations[mod] = []

            change_section_reloc = Relocation()
            change_section_reloc.reloc_type = PPC_RELOC_TYPE.R_RVL_SECT
            change_section_reloc.offset = 0
            change_section_reloc.section = sec_idx
            change_section_reloc.addend = 0
            module_relocations[mod].append(change_section_reloc)
            pos = 0

            for (rel, sym) in module_classify[mod]:
                if type(sym) is Relocation:
                    st_shndx = sym.section
                    st_value = sym.addend
                else:
                    st_shndx = sym['st_shndx']
                    st_value = sym['st_value']

                assert rel['r_offset'] >= pos
                out_reloc = Relocation()
                out_reloc.offset = rel['r_offset'] - pos
                pos = rel['r_offset']
                out_reloc.reloc_type = PPC_RELOC_TYPE(rel['r_info_type'])
                out_reloc.section = st_shndx
                out_reloc.addend = st_value
                module_relocations[mod].append(out_reloc)

    # Append final relocation
    for mod in module_relocations:
        stop_reloc = Relocation()
        stop_reloc.reloc_type = PPC_RELOC_TYPE.R_RVL_STOP
        stop_reloc.offset = 0
        stop_reloc.section = 0
        stop_reloc.addend = 0
        module_relocations[mod].append(stop_reloc)
        rel_file.relocations[mod] = module_relocations[mod]

    # Generate prolog, epilog and unresolved
    symtab = elffile.get_section_by_name('.symtab')
    prolog = symtab.get_symbol_by_name('_prolog')
    if prolog:
        rel_file.prolog_section = prolog[0]['st_shndx']
        rel_file.prolog = prolog[0]['st_value']

    epilog = symtab.get_symbol_by_name('_epilog')
    if epilog:
        rel_file.epilog_section = epilog[0]['st_shndx']
        rel_file.epilog = epilog[0]['st_value']

    unresolved = symtab.get_symbol_by_name('_unresolved')
    if unresolved:
        rel_file.unresolved_section = unresolved[0]['st_shndx']
        rel_file.unresolved = unresolved[0]['st_value']

    # Write file
    with open(outfile, 'wb') as f:
        rel_file.write(f)


if __name__ == '__main__':
    import sys

    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Compiles REL files')
    parser.add_argument('elf_file', type=Path)
    parser.add_argument('plf_files', type=Path, nargs='+')
    parser.add_argument('--alias_file', type=Path, help='file that stores aliases for relocation symbols')
    parser.add_argument('--fake_path', '-p', type=str, help='the fake directory name to be used for the .str file')
    args = parser.parse_args()

    alias_db: dict[str, str] = dict()
    if args.alias_file:   
        with open(args.alias_file, 'r') as f:
            for line in f.readlines():
                from_sym, to_sym = line.split('=')
                assert from_sym not in alias_db
                alias_db[from_sym] = to_sym

    fake_path = args.fake_path

    # Call actual function
    if args.elf_file.is_file():
        for plf in args.plf_files:

            if not plf.is_file():
                print_err(f'Fatal error: Could not file {plf}.')
                sys.exit(1)

            # Open files and parse them
            files = [open(args.elf_file, 'rb')] + [open(plf, 'rb') for plf in args.plf_files]
            modules = [ELFFile(f) for f in files]

            # Process them
            for idx in range(1, len(modules)):
                process_file(modules, idx, args.plf_files[idx-1], alias_db, fake_path)
                if unresolved_symbol_count > 0:
                    print_warn(f'Processed {args.plf_files[idx-1]} with {unresolved_symbol_count} unresolved symbol(s).')
                else:
                    print_success(f'Processed {args.plf_files[idx-1]}.')

            # Close them
            for file in files:
                file.close()

        with open(args.elf_file.with_suffix('.str'), 'w') as f:
            f.write(str_file)
        print_success(f'Wrote {f.name}.')
    else:
        print_err('File', args.elf_file, 'not found!')
