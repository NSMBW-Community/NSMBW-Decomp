#!/usr/bin/env python3

# build_dol.py
# Reimplementation of makedol

from pathlib import Path

from dolfile import Dol, DolSection
from elffile import ElfFile
from elfconsts import SHF

def process_file(elf_file: ElfFile, output_file: Path) -> None:
    dol_file = Dol()

    # Find executable sections
    sec_count = 0
    for section in elf_file.sections:
        if SHF.SHF_EXECINSTR in section.header.sh_flags:
            dol_file.add_section(DolSection.from_elf_section(section))
            sec_count += 1

    # Fill up empty executable sections
    while sec_count < 7:
        dol_file.add_section(DolSection(unused=True))
        sec_count += 1

    # Find data sections
    for section in elf_file.sections:
        if (SHF.SHF_EXECINSTR not in section.header.sh_flags) and (SHF.SHF_ALLOC in section.header.sh_flags):
            if section.name not in ['.bss', '.sbss', '.sbss2', '.PPC.EMB.apuinfo']:
                dol_file.add_section(DolSection.from_elf_section(section))
                sec_count += 1

            if section.name == '.bss':
                dol_file.bss_addr = section.header.sh_addr

    bss_hdr = elf_file.get_section('.bss').header
    sbss2_hdr = elf_file.get_section('.sbss2').header

    bss_begin = bss_hdr.sh_addr
    bss_end = sbss2_hdr.sh_addr + sbss2_hdr.sh_size

    # DOL bss size is actually (end of last bss section) - (start of first bss section)
    dol_file.bss_len = bss_end - bss_begin

    # Fill up empty sections
    while sec_count < 18:
        dol_file.add_section(DolSection(unused=True))
        sec_count += 1

    dol_file.entry = elf_file.header.e_entry

    # Write file
    with open(output_file, 'wb') as f:
        dol_file.write(f)


def build_dol(elf_file: Path, output_file: Path) -> None:
    assert elf_file.is_file()
    elf = ElfFile.read(elf_file.read_bytes())
    process_file(elf, output_file)


if __name__ == '__main__':

    # Parse arguments separately so this file can be imported from other ones
    import argparse

    parser = argparse.ArgumentParser(description='Compiles DOL files.')
    parser.add_argument('elf_file', type=Path, help='File to be compiled.')
    parser.add_argument('-o', '--output', type=Path, required=True, help='Output file name.')
    args = parser.parse_args()
    build_dol(args.elf_file, args.output)
