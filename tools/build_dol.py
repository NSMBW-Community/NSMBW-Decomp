#!/usr/bin/env python3
# Reimplementation of makerel by RootCubed

import re
from pathlib import Path
from elftools.elf.elffile import ELFFile

from dolfile import Dol
from color_term import *
from dolfile import DolSection
from elfconsts import SHF

def process_file(elf_file: ELFFile, filename: Path):
    outfile = filename.with_suffix('.dol')

    dol_file = Dol()

    # find executable sections
    sec_count = 0
    for section in elf_file.iter_sections():
        if bool(section['sh_flags'] & SHF.SHF_EXECINSTR.value):
            sec = DolSection()
            sec.sec_len = section['sh_size']
            sec.virt_addr = section['sh_addr']
            sec.set_data(bytearray(section.data()))
            dol_file.sections.append(sec)
            sec_count += 1
    while sec_count < 7:
        # fill up empty sections
        dol_file.sections.append(DolSection(unused=True))
        sec_count += 1
    
    # find data sections
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

                # TODO: how is this calculated?
                dol_file.bss_len = 0x0de59c
                
    while sec_count < 18:
        # fill up empty sections
        dol_file.sections.append(DolSection(unused=True))
        sec_count += 1
        
    dol_file.entry = elf_file.header['e_entry']

    # Write file
    with open(outfile, 'wb') as f:
        dol_file.write(f)


if __name__ == '__main__':
    import sys

    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Compiles DOL files')
    parser.add_argument('elf_file', type=Path)
    args = parser.parse_args()

    if args.elf_file.is_file():
        with open(args.elf_file, 'rb') as ef:
            elf = ELFFile(ef)

            print(f'Building {str(args.elf_file)}...')
            process_file(elf, args.elf_file)
    else:
        print_err(f'Fatal error: File {args.elf_file} not found!')
