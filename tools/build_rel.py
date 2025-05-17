#!/usr/bin/env python3
# Reimplementation of makerel

import re
from pathlib import Path
from typing import cast

from color_term import *
from elffile import *
from elfconsts import *
from relfile import Rel, RelRelocation, RelSection

def add_symbol(module_classify: dict[int, list], i: int, reloc: ElfRela, sym: ElfSymbol) -> None:
    if i not in module_classify:
        module_classify[i] = []

    module_classify[i].append((reloc, sym))

def find_symbol(syms: ElfSymtab, i: int, name: str, module_classify: dict[int, list], reloc: ElfRela) -> bool:
    try_found = syms.get_symbols(name)
    if try_found and try_found[0].st_shndx != SHN.SHN_UNDEF.value:
        add_symbol(module_classify, i, reloc, try_found[0])
        return True
    return False

def convert_to_rel_relocations(elf_relocs: list[tuple[ElfRela, ElfSymbol]], sec_idx: int) -> list[RelRelocation]:
    res_relocs: list[RelRelocation] = []

    change_section_reloc = RelRelocation()
    change_section_reloc.reloc_type = PPC_RELOC_TYPE.R_RVL_SECT
    change_section_reloc.offset = 0
    change_section_reloc.section = sec_idx
    change_section_reloc.addend = 0
    res_relocs.append(change_section_reloc)
    pos = 0

    for rel, sym in elf_relocs:
        st_shndx = sym.st_shndx
        st_value = sym.st_value + rel.r_addend

        assert rel.r_offset >= pos
        offset = rel.r_offset - pos

        # Create R_RVL_NONE relocations until we get close enough to the address we want
        while offset > 0xFFFF:
            reloc_none = RelRelocation()
            reloc_none.offset = 0xFFFF
            reloc_none.reloc_type = PPC_RELOC_TYPE.R_RVL_NONE
            pos += 0xFFFF
            res_relocs.append(reloc_none)
            offset = rel.r_offset - pos

        # Make the actual relocation entry
        out_reloc = RelRelocation()
        out_reloc.offset = offset
        pos = rel.r_offset
        out_reloc.reloc_type = rel.r_info_type
        out_reloc.section = st_shndx
        out_reloc.addend = st_value
        res_relocs.append(out_reloc)
    return res_relocs

def process_file(module: ElfFile, symbols: list[ElfSymtab], module_index: int, filename: Path, str_file: Path) -> int:
    unresolved_symbol_count: int = 0

    strs = str_file.read_text()
    str_file_offset = strs.find(filename.name)
    while str_file_offset > 0 and strs[str_file_offset - 1] != '\0':
        str_file_offset -= 1
    str_len = len(strs[str_file_offset:].split('\0')[0]) + 1

    outfile = filename.with_suffix('.rel')

    # Create REL
    rel_file = Rel(module_index, path_offset=str_file_offset, path_size=str_len)
    module_relocations: dict[int, list[RelRelocation]] = {}

    for sec_idx, section in enumerate(module.sections):
        sec_addr_align = section.header.sh_addralign
        if section.header.sh_type != SHT.SHT_PROGBITS or section.name == '.comment':

            # Non-text/data sections are added to the REL as empty sections
            empty_sec = RelSection()

            # Special case: .bss section does get length field set
            if section.name == '.bss':
                empty_sec.is_bss = True
                empty_sec._sec_len = section.size()
                rel_file.bss_size = section.size()
                rel_file.bss_align = sec_addr_align

            rel_file.add_section(empty_sec)
            continue

        rel_sec = RelSection()
        rel_sec.set_data(bytearray(section.data))
        rel_sec.executable = section.name == '.text'
        rel_sec.alignment = sec_addr_align if sec_addr_align > 0 else 4

        if section.name == '.text':
            rel_file.align = sec_addr_align
        rel_file.add_section(rel_sec)

        if not module.has_section('.rela' + section.name):
            continue

        rela_sec = cast(ElfRelaSec, module.get_section('.rela' + section.name))
        symbol_table = cast(ElfSymtab, module.sections[rela_sec.header.sh_link])

        # Group the relocations by destination module and store the corresponding symbol
        module_classify: dict[int, list[tuple[ElfRela, ElfSymbol]]] = {}

        for reloc in rela_sec.relocs:
            symbol = symbol_table.get_symbol(reloc.r_info_sym)
            sym_name = symbol.name

            # Check if in same module
            if symbol.st_shndx != SHN.SHN_UNDEF.value:
                add_symbol(module_classify, module_index, reloc, symbol)
                continue

            # TODO: nice hardcode lol
            if sym_name == '__destroy_global_chain':
                print_warn('Warning: __destroy_global_chain not found, using hardcoded address.')
                sym_name = f'R_{rel_file.index}_1_90'

            # First check if hardcoded
            m = re.search('R_([0-9a-fA-F]+)_([0-9a-fA-F]+)_([0-9a-fA-F]+)', sym_name)
            if m:
                mod_num = int(m.group(1), 16)
                if mod_num not in module_classify:
                    module_classify[mod_num] = []

                sym = ElfSymbol('', st_shndx=int(m.group(2), 16), st_value=int(m.group(3), 16))
                module_classify[mod_num].append((reloc, sym))
                continue

            # Search other modules
            found_sym = False
            for i, symtab in enumerate(symbols):
                if i != module_index and find_symbol(symtab, i, sym_name, module_classify, reloc):
                    found_sym = True
                    break
            if found_sym:
                continue

            print_err('Error: Symbol', sym_name, 'not found!')
            unresolved_symbol_count += 1


        # Convert ELF relocations to REL relocations
        for mod in module_classify:
            if mod not in module_relocations:
                module_relocations[mod] = []
            module_relocations[mod].extend(convert_to_rel_relocations(module_classify[mod], sec_idx))

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
    symtab = cast(ElfSymtab, module.get_section('.symtab'))
    prolog = symtab.get_symbols('_prolog')
    if prolog:
        rel_file.prolog_section = prolog[0].st_shndx
        rel_file.prolog = prolog[0].st_value

    epilog = symtab.get_symbols('_epilog')
    if epilog:
        rel_file.epilog_section = epilog[0].st_shndx
        rel_file.epilog = epilog[0].st_value

    unresolved = symtab.get_symbols('_unresolved')
    if unresolved:
        rel_file.unresolved_section = unresolved[0].st_shndx
        rel_file.unresolved = unresolved[0].st_value

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
    if not module.get_section('.mwcats.text'):
        rel_file.add_section(RelSection())
        rel_file.add_section(RelSection())

    # Write file
    with open(outfile, 'wb') as f:
        rel_file.write(f)

    # Return unresolved symbols
    return unresolved_symbol_count


def build_rel(elf_file: Path, plf_paths: list[Path], output_file: Path, str_file: Path) -> None:
    assert elf_file.is_file()
    assert all(plf.is_file() for plf in plf_paths)

    # Open files and parse them
    files = [elf_file.read_bytes()] + [plf.read_bytes() for plf in plf_paths]
    symbols = [ElfFile.read_symtab(f) for f in files]

    # Process them
    for i in range(1, len(files)):
        plf_path = plf_paths[i - 1]
        if plf_path.name != output_file.with_suffix('.plf').name:
            continue

        plf = ElfFile.read(files[i])

        unresolved_symbol_count = process_file(plf, symbols, i, plf_path, str_file)
        if unresolved_symbol_count > 0:
            print_warn('Processed', str(plf_path), 'with', unresolved_symbol_count, 'unresolved symbol(s).')


if __name__ == '__main__':
    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Builds a REL file.')
    parser.add_argument('elf_file', type=Path, help='DOL object file.')
    parser.add_argument('plf_files', type=Path, nargs='+', help='REL object file(s).')
    parser.add_argument('str_file', type=Path, help='The STR file.')
    parser.add_argument('--output', '-o', type=Path, help='The REL file to output.', required=True)
    args = parser.parse_args()
    build_rel(args.elf_file, args.plf_files, args.output, args.str_file)

