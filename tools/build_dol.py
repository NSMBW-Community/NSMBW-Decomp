#!/usr/bin/env python3
# Reimplementation of makedol

from pathlib import Path
from elftools.elf.elffile import ELFFile

from color_term import *
from dolfile import Dol, DolSection
from elfconsts import SHF

def process_file(elf_file: ELFFile, filename: Path) -> None:
    outfile = filename.with_suffix('.dol')
    dol_file = Dol()

    # Find executable sections
    sec_count = 0
    for section in elf_file.iter_sections():
        if bool(section['sh_flags'] & SHF.SHF_EXECINSTR.value):
            sec = DolSection()
            sec.sec_len = section['sh_size']
            sec.virt_addr = section['sh_addr']
            sec.set_data(bytearray(section.data()))
            dol_file.sections.append(sec)
            sec_count += 1

    # Fill up empty sections
    while sec_count < 7:
        dol_file.sections.append(DolSection(unused=True))
        sec_count += 1

    # Find data sections
    for section in elf_file.iter_sections():
        if not bool(section['sh_flags'] & SHF.SHF_EXECINSTR.value) and bool(section['sh_flags'] & SHF.SHF_ALLOC.value):
            if section.name not in ['.bss', '.sbss', '.sbss2', '.PPC.EMB.apuinfo']:
                sec = DolSection()
                sec.sec_len = section['sh_size']
                sec.virt_addr = section['sh_addr']
                sec.set_data(bytearray(section.data()))
                dol_file.sections.append(sec)
                sec_count += 1

            if section.name == '.bss':
                dol_file.bss_addr = section['sh_addr']
                dol_file.bss_len = 0x0de59c # TODO: how is this calculated?

    # Fill up empty sections
    while sec_count < 18:
        dol_file.sections.append(DolSection(unused=True))
        sec_count += 1

    dol_file.entry = elf_file.header['e_entry']

    # Write file
    with open(outfile, 'wb') as f:
        dol_file.write(f)


def build_dol(elf_file: Path) -> None:
    if not elf_file.is_file():
        print_err('Fatal error: File', str(elf_file), 'not found!')
        return

    with open(elf_file, 'rb') as ef:
        elf = ELFFile(ef)
        print('Building', elf_file.name, end='...\n')
        process_file(elf, elf_file)


if __name__ == '__main__':

    # Parse arguments separately so this file can be imported from other ones
    import argparse

    parser = argparse.ArgumentParser(description='Compiles DOL files.')
    parser.add_argument('elf_file', type=Path, help='File to be compiled.')
    args = parser.parse_args()
    build_dol(args.elf_file)
