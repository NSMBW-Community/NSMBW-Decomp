#!/usr/bin/env python3
# Reimplementation of makerel

import re
from pathlib import Path

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import Relocation
from elftools.elf.sections import Symbol

from color_term import *
from elfconsts import PPC_RELOC_TYPE
from relfile import Rel, RelRelocation, RelSection


def find_symbol(mod: ELFFile, i: int, symbol: Symbol, module_classify: dict[int, list], reloc: Relocation) -> bool:
    symtab = mod.get_section_by_name('.symtab')
    try_found = symtab.get_symbol_by_name(symbol.name)
    if try_found and try_found[0]['st_shndx'] != 'SHN_UNDEF':
        if i not in module_classify:
            module_classify[i] = [(reloc, try_found[0])]
        else:
            module_classify[i].append((reloc, try_found[0]))
        return True
    return False


def process_file(modules: list[ELFFile], idx: int, filename: Path, alias_db: dict[str, str], fakedir: str, str_file: str) -> int:
    unresolved_symbol_count: int = 0

    # Generate .str file and output filename
    str_file_offset = len(str_file)
    if fakedir:
        path_str = f'{fakedir}{filename.name}\0'
    else:
        path_str = f'{filename.resolve()}\0'
    str_file += path_str

    outfile = filename.with_suffix('.rel')
    unit_name = filename.stem

    # Create REL
    rel_file = Rel(idx, path_offset=str_file_offset, path_size=len(path_str))
    elffile = modules[idx]
    module_relocations: dict[int, list[RelRelocation]] = {}

    for sec_idx, section in enumerate(elffile.iter_sections()):
        if section['sh_type'] != 'SHT_PROGBITS' or section.name == '.comment':

            # Non-text/data sections are added to the REL as empty sections
            empty_sec = RelSection()

            # Special case: .bss section does get length field set
            if section.name == '.bss':
                empty_sec.is_bss = True
                empty_sec._sec_len = section.data_size
                rel_file.bss_size = section.data_size
                rel_file.bss_align = section['sh_addralign']

            rel_file.add_section(empty_sec)
            continue

        rel_sec = RelSection()
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

            # Try to look up symbol in alias database
            symname = alias_db.get(f'{unit_name}: {symbol.name}', alias_db.get(symbol.name, ''))
            symbol.name = symname if symname else symbol.name

            # TODO: nice hardcode lol
            if symbol.name == '__destroy_global_chain':
                print_warn('Warning: __destroy_global_chain not found, using hardcoded address.')
                symbol.name = f'R_{rel_file.index}_1_90'

            # First check if hardcoded
            m = re.search('R_([0-9a-fA-F]+)_([0-9a-fA-F]+)_([0-9a-fA-F]+)', symbol.name)
            if m:
                # print_warn('Relocation symbol', symbol.name, 'found...')
                mod_num = int(m.group(1), 16)
                out_reloc = RelRelocation()
                out_reloc.section = int(m.group(2), 16)
                out_reloc.addend = int(m.group(3), 16)
                out_reloc.reloc_type = PPC_RELOC_TYPE(reloc['r_info_type'])
                if mod_num not in module_classify:
                    module_classify[mod_num] = [(reloc, out_reloc)]
                else:
                    module_classify[mod_num].append((reloc, out_reloc))
                continue

            # Search own module
            if find_symbol(modules[idx], idx, symbol, module_classify, reloc):
                continue

            # Search other modules
            found_sym = False
            for i, mod in enumerate(modules):
                if i != idx and find_symbol(mod, i, symbol, module_classify, reloc):
                    found_sym = True
                    break

            # Still not found? Bail
            if not found_sym:
                print_err('Error: Symbol', symbol.name, 'not found!')
                unresolved_symbol_count += 1

        for mod in module_classify:
            if mod not in module_relocations:
                module_relocations[mod] = []

            change_section_reloc = RelRelocation()
            change_section_reloc.reloc_type = PPC_RELOC_TYPE.R_RVL_SECT
            change_section_reloc.offset = 0
            change_section_reloc.section = sec_idx
            change_section_reloc.addend = 0
            module_relocations[mod].append(change_section_reloc)
            pos = 0

            for rel, sym in module_classify[mod]:
                if type(sym) is RelRelocation:
                    st_shndx = sym.section
                    st_value = sym.addend
                else:
                    st_shndx = sym['st_shndx']
                    st_value = sym['st_value']

                assert rel['r_offset'] >= pos
                offset = rel['r_offset'] - pos

                # Create R_RVL_NONE relocations until we get close enough to the address we want
                while offset > 0xFFFF:
                    reloc_none = RelRelocation()
                    reloc_none.offset = 0xFFFF
                    reloc_none.reloc_type = PPC_RELOC_TYPE.R_RVL_NONE
                    pos += 0xFFFF
                    module_relocations[mod].append(reloc_none)
                    offset = rel['r_offset'] - pos

                # Make the actual relocation entry
                out_reloc = RelRelocation()
                out_reloc.offset = offset
                pos = rel['r_offset']
                out_reloc.reloc_type = PPC_RELOC_TYPE(rel['r_info_type'])
                out_reloc.section = st_shndx
                out_reloc.addend = st_value
                module_relocations[mod].append(out_reloc)

    # Append final relocation
    for mod in module_relocations:
        stop_reloc = RelRelocation()
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

    # Hardcoded, only needed if this part hasn't been decompiled
    if not prolog or not epilog or not unresolved:
        print_warn('Warning: _prolog, _epilog and/or _unresolved not found, using hardcoded addresses...')
        rel_file.prolog_section = 1
        rel_file.epilog_section = 1
        rel_file.unresolved_section = 1
        rel_file.prolog = 0x0
        rel_file.epilog = 0x30
        rel_file.unresolved = 0x60

    # PLFs usually contain this section and also .rela.mwcats.text, so add those as dummy sections
    if not elffile.get_section_by_name('.mwcats.text'):
        rel_file.add_section(RelSection())
        rel_file.add_section(RelSection())

    # Write file
    with open(outfile, 'wb') as f:
        rel_file.write(f)

    # Return unresolved symbols
    return unresolved_symbol_count


def build_rel(elf_file: Path, plf_files: list[Path], alias_file: Path, fake_path: str) -> None:

    # Check if ELF file exists
    if not elf_file.is_file():
        print_err('Fatal error: File', str(elf_file), 'not found!')
        return

    # Check if PLF files exist
    for plf in plf_files:
        if not plf.is_file():
            print_err('Fatal error: File', str(plf), 'not found!')
            return

    # Load aliases if present
    alias_db: dict[str, str] = {}
    if alias_file.is_file():
        with open(alias_file) as f:
            for line in f:
                from_sym, to_sym = line.split('=')
                if from_sym not in alias_db:
                    alias_db[from_sym] = to_sym

    # Initialize str file
    str_file = ''

    # Open files and parse them
    files = [open(elf_file, 'rb')] + [open(plf, 'rb') for plf in plf_files]
    modules = [ELFFile(f) for f in files]

    # Process them
    for idx in range(1, len(modules)):
        print('Building', str(plf_files[idx-1].with_suffix('.rel')), end='...\n')
        unresolved_symbol_count = process_file(modules, idx, plf_files[idx-1], alias_db, fake_path, str_file)
        if unresolved_symbol_count > 0:
            print_warn('Processed', str(plf_files[idx-1]), 'with', unresolved_symbol_count, 'unresolved symbol(s).')
        else:
            print_success('Processed', str(plf_files[idx-1]), end='.\n')

    # Close them
    for file in files:
        file.close()

    # Write str file
    with open(elf_file.with_suffix('.str'), 'w') as f:
        f.write(str_file)
    print_success('Wrote', f.name, end='.\n')


if __name__ == '__main__':

    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Compiles REL files.')
    parser.add_argument('elf_file', type=Path, help='DOL object file.')
    parser.add_argument('plf_files', type=Path, nargs='+', help='REL object file(s).')
    parser.add_argument('--alias_file', type=Path, help='File that stores aliases for relocation symbols.')
    parser.add_argument('--fake_path', '-p', type=str, help='The fake directory name to be used for the .str file.')
    args = parser.parse_args()
    build_rel(args.elf_file, args.plf_files, args.alias_file, args.fake_path)
