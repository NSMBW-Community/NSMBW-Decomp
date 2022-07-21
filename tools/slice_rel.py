#!/usr/bin/env python3
# Slices a REL file into .o files

import argparse
from pathlib import Path
from relfile import REL

from elffile import *
from elfconsts import *

file = ElfFile(ET.ET_REL, EM.EM_PPC)
with open("rel_init.o", "wb") as f:
    f.write(bytes(file))
    f.close()

parser = argparse.ArgumentParser(description='Slices REL files')
parser.add_argument('rel_files', type=Path, nargs='+')
args = parser.parse_args()

class RelocSym:
    # Models a symbol referenced by a relocation entry
    def __init__(self, mod_num, section, addend):
        self.mod_num = mod_num
        self.section = section
        self.addend = addend
    
    def __repr__(self):
        return f'R_{self.mod_num:0x}_{self.section:0x}_{self.addend:0x}'

    def __str__(self):
        return self.__repr__()

class SliceSection:
    def __init__(self, sec_name: str, sec_idx: int, start_offs: int, end_offs: int):
        self.sec_name = sec_name
        self.sec_idx = sec_idx
        self.start_offs = start_offs
        self.end_offs = end_offs

    def contains(self, reloc_sym: RelocSym):
        return reloc_sym.section == self.sec_idx and self.start_offs <= reloc_sym.addend <= self.end_offs

class Slice:
    def __init__(self, slice_name: str, slice_secs: list[SliceSection]):
        self.slice_name = slice_name
        self.slice_secs = slice_secs
        

reloc_syms: set[RelocSym] = set()
module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]] = dict()

def read_reloc_refs(rel_file: REL, idx: int):
    assert idx not in module_relocs
    module_relocs[idx] = dict()
    for mod_num in rel_file.relocations:
        curr_pos = 0
        curr_section = 0
        for reloc in rel_file.relocations[mod_num]:
            if reloc.reloc_type == PPC_RELOC_TYPE.R_RVL_SECT:
                curr_pos = 0
                curr_section = reloc.section
                continue
            curr_pos += reloc.offset
            if reloc.reloc_type.value < 201:
                this_loc = RelocSym(idx, curr_section, curr_pos)
                dest_sym = RelocSym(mod_num, reloc.section, reloc.addend)
                reloc_syms.add(dest_sym)
                module_relocs[idx][this_loc] = (dest_sym, reloc.reloc_type)

def extract_slice(rel_file: REL, slice: Slice):
    elf_file = ElfFile(ET.ET_REL, EM.EM_PPC)

    reloc_secs: list[ElfRelaSec] = []

    symtab_sec = ElfSymtab('.symtab')
    shstrtab_sec = ElfStrtab('.shstrtab')
    strtab_sec = ElfStrtab('.strtab')

    for sec in slice.slice_secs:
        sec_data = rel_file.sections[sec.sec_idx].get_data()[sec.start_offs:sec.end_offs]
        elf_sec = ElfSection(sec.sec_name, bytes(sec_data))
        elf_sec.header.sh_type = SHT.SHT_PROGBITS
        elf_file.add_section(elf_sec)
        

    for sec in slice.slice_secs:
        relocs_in_section = [x for x in module_relocs[rel_file.index] if sec.contains(x)]
        print(f'{slice.slice_name}: Section {sec.sec_name} contains {len(relocs_in_section)} relocs')
        if len(relocs_in_section) > 0:
            reloc_sec = ElfRelaSec(f'.rela{sec.sec_name}')
            reloc_secs.append(reloc_sec)
            for reloc_origin in relocs_in_section:
                reloc_dest = module_relocs[rel_file.index][reloc_origin]
                sym = ElfSymbol(str(reloc_dest[0]))
                sym_idx = symtab_sec.add_symbol(sym)
                r_offset = reloc_origin.addend
                elf_reloc = ElfRela(r_offset, sym_idx, reloc_dest[1], 0)
                reloc_sec.add_reloc(elf_reloc)

            reloc_sec.header.sh_info = sec.sec_idx
            elf_file.add_section(reloc_sec)
    
    symtab_sec_idx = elf_file.add_section(symtab_sec)
    _shstrtab_sec_idx = elf_file.add_section(shstrtab_sec)
    strtab_sec_idx = elf_file.add_section(strtab_sec)

    symtab_sec.header.sh_link = strtab_sec_idx

    for rs in reloc_secs:
        rs.header.sh_link = symtab_sec_idx

    
    return elf_file

idx = 1
for rel in args.rel_files:
    if not rel.is_file():
        print(f'Invalid file {rel}')
        continue
    with open(rel, 'rb') as f:
        print(f'Processing module {idx} ({f.name})...')
        rel_file = REL(idx, file=f)
        read_reloc_refs(rel_file, idx)
        # TODO: iterate over slices
        demo_slice = Slice("rel_init.o", [
            SliceSection(".text", 1, 0x0, 0x64)
        ])
        elf = extract_slice(rel_file, demo_slice)
        print(elf)
        with open(demo_slice.slice_name, "wb") as ef:
            ef.write(bytes(elf))
        idx += 1

sorted_uniques = sorted(reloc_syms, key=lambda tup: (tup.mod_num, tup.section, tup.addend))
with open('reloc_names.csv', "w") as rf:
    rf.write('\n'.join(list(map(lambda x: str(x), sorted_uniques))))