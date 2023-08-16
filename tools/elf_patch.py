#!/usr/bin/env python3
# Script to patch functions in an object file

from math import ceil
import re
import struct
import sys

sys.path.append('tools')

from color_term import *
from project_settings import *
from elffile import ElfFile, ElfSection, ElfSymtab, ElfRelaSec

class ElfPatcher:
    def __init__(self, elf_file: ElfFile, orig_elf_file: ElfFile = None):
        self.elf_file = elf_file
        self.orig_elf_file = orig_elf_file
        self.elf_symtab: ElfSymtab = elf_file.get_section('.symtab')
        self.orig_elf_symtab: ElfSymtab = orig_elf_file.get_section('.symtab')

    def remove_relocs_within(self, section: ElfSection, start_offs: int, end_offs: int):
        rela_sec: ElfRelaSec = self.elf_file.get_section(f'.rela{section.name}')
        rela_sec.relocs = [x for x in rela_sec.relocs if x.r_offset < start_offs or x.r_offset >= end_offs]
    
    def shift_relocs_after(self, section: ElfSection, start_offs: int, shift_amount: int):
        sec_index = self.elf_file.get_section_index(section.name)

        # Shift symbols
        for sym in self.elf_symtab.syms:
            if sym.st_shndx == sec_index and sym.st_value > start_offs:
                sym.st_value += shift_amount

        # Shift relocations
        rela_sec: ElfRelaSec = self.elf_file.get_section(f'.rela{section.name}')
        if not rela_sec:
            return

        for rela in rela_sec.relocs:
            if rela.r_offset >= start_offs:
                rela.r_offset += shift_amount


    def replace_with_original(self, sym_name: str):
        print(f'Patching {sym_name}...')

        sym = self.elf_symtab.get_symbols(sym_name)
        orig_sym = self.orig_elf_symtab.get_symbols(sym_name)

        if not sym:
            print_err(f'Symbol {sym_name} not in ELF to be patched!')
            return
        if not orig_sym:
            print_err(f'Symbol {sym_name} not in original ELF!')
            return
        if orig_sym[0].st_size == 0:
            print_err(f'Symbol {sym_name} in original ELF has size 0!')
            return
        
        sym = sym[0]
        orig_sym = orig_sym[0]

        old_sec = self.elf_file.sections[sym.st_shndx]
        new_sec = self.orig_elf_file.sections[orig_sym.st_shndx]

        alignment = self.elf_file.sections[sym.st_shndx].header.sh_addralign
        true_old_size = ceil(sym.st_size / alignment) * alignment
        true_new_size = ceil(orig_sym.st_size / alignment) * alignment

        new_code = new_sec.data[orig_sym.st_value:orig_sym.st_value + true_new_size]

        # Patch code data
        self.elf_file.sections[sym.st_shndx].data = \
            old_sec.data[:sym.st_value] + \
            new_code + \
            old_sec.data[sym.st_value + true_old_size:]
        
        # Patch symbol
        sym.st_size = orig_sym.st_size
        
        # Patch .mwcats
        mwcats_sec = self.elf_file.get_section('.mwcats' + old_sec.name)
        mwcats_rela_sec: ElfRelaSec = self.elf_file.get_section('.rela.mwcats' + old_sec.name)

        for rela in mwcats_rela_sec.relocs:
            if rela.sym == sym:
                sym_size_offs = rela.r_offset - 2
                mwcats_sec.data = \
                    mwcats_sec.data[:sym_size_offs] + \
                    struct.pack('>H', orig_sym.st_size) + \
                    mwcats_sec.data[sym_size_offs + 2:]
                
        # Remove obsolete relocations within function
        self.remove_relocs_within(old_sec, sym.st_value, sym.st_value + true_old_size)
        
        # Shift relocations after function
        self.shift_relocs_after(old_sec, sym.st_value, true_new_size - true_old_size)

def patch_elf(nm_file: Path, corr_file: Path, out_file: Path):
    elf_file = ElfFile.read(open(nm_file, 'rb').read())
    corr_elf_file = ElfFile.read(open(corr_file, 'rb').read())

    patch_section: ElfSymtab = elf_file.get_section('.patches')

    if not patch_section:
        return

    cmds = [x.decode() for x in patch_section.data.split(b'\x00') if x]

    elf_patcher = ElfPatcher(elf_file, corr_elf_file)

    for cmd in cmds:
        cmd_name, sym_name = re.match(r'(\w+)\((\w+)\)', cmd).groups()

        if cmd_name == 'replace':
            elf_patcher.replace_with_original(sym_name)
        else:
            print_err(f'Unknown command {cmd_name}')

    # Delete .patches
    elf_file.sections = [x for x in elf_file.sections if x.name != '.patches']

    # Write patched file
    with open(out_file, 'wb') as f:
        f.write(bytes(elf_file))
        print_success('Patched', nm_file)

if __name__ == '__main__':
    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Patches non-matching object files.')
    parser.add_argument('nm_file', type=Path, help='The non-matching object file.')
    parser.add_argument('corr_file', type=Path, help='The non-matching object file.')
    parser.add_argument('--out_path', '-o', default=None, type=Path, help='Where the patched object will be written to.')
    args = parser.parse_args()
    patch_elf(args.nm_file, args.corr_file, args.nm_file if args.out_path is None else args.out_path)
