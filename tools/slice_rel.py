#!/usr/bin/env python3
# Slices a REL file into .o files

from pathlib import Path

from color_term import print_err
from elffile import *
from elfconsts import *
from project_settings import *
from relfile import Rel
from slicelibV2 import *
import re

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


def read_reloc_refs(rel_file: Rel, idx: int, reloc_syms: set[RelocSym], module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]]) -> None:
    assert idx not in module_relocs
    module_relocs[idx] = {}

    for mod_num in rel_file.relocations:
        curr_pos = 0
        curr_section = 0
        for reloc in rel_file.relocations[mod_num]:
            curr_pos += reloc.offset
            if reloc.reloc_type == PPC_RELOC_TYPE.R_RVL_SECT:
                curr_pos = 0
                curr_section = reloc.section
                continue
            if reloc.reloc_type.value < 201:
                this_loc = RelocSym(idx, curr_section, curr_pos)
                dest_sym = RelocSym(mod_num, reloc.section, reloc.addend)
                reloc_syms.add(dest_sym)
                module_relocs[idx][this_loc] = (dest_sym, reloc.reloc_type)


def extract_slice(rel_file: Rel, slice: Slice, alias_file: Path, module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]]) -> ElfFile:
    elf_file = ElfFile(ET.ET_REL, EM.EM_PPC)

    reloc_secs: list[ElfRelaSec] = []

    symtab_sec = ElfSymtab('.symtab')

    symbols: dict[str, str] = {}
    if alias_file.is_file():
        with open(alias_file) as f:
            for line in f:
                addr, sym = line.strip().replace(' ', '').split('=')
                symbols[addr.upper()] = sym

    shndx_mapping = dict()
    for sec in slice.sliceSecs:
        if sec.sec_name == '.bss':
            elf_sec = ElfSection(sec.sec_name)
            elf_sec.header.sh_type = SHT.SHT_NOBITS
            elf_sec.header.sh_size = sec.end_offs - sec.start_offs
        else:
            sec_data = rel_file.sections[sec.sec_idx].get_data()[sec.start_offs:sec.end_offs]
            elf_sec = ElfSection(sec.sec_name, bytes(sec_data))
            elf_sec.header.sh_type = SHT.SHT_PROGBITS

        elf_sec.header.sh_addralign = sec.alignment
        shndx_mapping[sec.sec_idx] = elf_file.add_section(elf_sec)

    added_syms = dict()
    for sec in slice.sliceSecs:
        for sym in symbols:
            m = re.search('R_([0-9a-fA-F]+)_([0-9a-fA-F]+)_([0-9a-fA-F]+)', sym)
            assert m is not None, f'Invalid symbol name: {sym}'
            reloc_sym = RelocSym(int(m.group(1), 16), int(m.group(2), 16), int(m.group(3), 16))
            sym_name = symbols[sym]
            shndx = reloc_sym.section
            addend = reloc_sym.addend
            if shndx not in shndx_mapping:
                continue
            actual_shndx = shndx_mapping[shndx]
            if reloc_sym.mod_num == rel_file.index and sec.contains(shndx, addend):
                s = ElfSymbol(sym_name, st_shndx=actual_shndx, st_value=addend - sec.start_offs, st_info_bind=STB.STB_GLOBAL)
                added_syms[sym] = symtab_sec.add_symbol(s)

    for sec in slice.sliceSecs:
        relocs_in_section = [x for x in module_relocs[rel_file.index] if sec.contains(x.section, x.addend)]
        if len(relocs_in_section) > 0:
            reloc_sec = ElfRelaSec(f'.rela{sec.sec_name}')
            reloc_secs.append(reloc_sec)
            for reloc_origin in relocs_in_section:
                reloc_sym, rela_type = module_relocs[rel_file.index][reloc_origin]
                sym_name = str(reloc_sym)
                if sym_name in added_syms:
                    sym_idx = added_syms[sym_name]
                else:
                    sym = ElfSymbol(sym_name, st_info_bind=STB.STB_GLOBAL)
                    sym_idx = symtab_sec.add_symbol(sym)
                    added_syms[sym_name] = sym_idx
                r_offset = reloc_origin.addend - sec.start_offs
                elf_reloc = ElfRela(r_offset, sym_idx, rela_type, 0)
                reloc_sec.add_reloc(elf_reloc)

            reloc_sec.header.sh_info = elf_file.get_section_index(sec.sec_name)
            elf_file.add_section(reloc_sec)

    symtab_sec_idx = elf_file.add_section(symtab_sec)

    for rs in reloc_secs:
        rs.header.sh_link = symtab_sec_idx

    return elf_file


def slice_rel(rel_file: Path, out_path: Path, alias_file: Path) -> None:
    if not rel_file.is_file():
        print_err('Fatal error: File', str(rel_file), 'not found!')
        return

    reloc_syms: set[RelocSym] = set()
    module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]] = {}

    # Read slices
    with open(rel_file, 'rb') as f:
        slice_file: SliceFile = load_slice_file((SLICEDIR / rel_file.stem).with_suffix('.json'))
        mod_num = slice_file.meta.moduleNum
        rel = Rel(mod_num, file=f)
        read_reloc_refs(rel, mod_num, reloc_syms, module_relocs)

        for slice in slice_file.parsed_slices:
            elf = extract_slice(rel, slice, alias_file, module_relocs)
            out_filepath = out_path / slice_file.meta.fileName.split('.')[0] / slice.sliceName
            out_filepath.parent.mkdir(parents=True, exist_ok=True)
            with open(out_filepath, 'wb') as ef:
                ef.write(bytes(elf))


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Slices REL files.')
    parser.add_argument('rel_file', type=Path, help='REL file to be sliced.')
    parser.add_argument('-o', '--output', default=Path('bin/sliced'), type=Path, help='Path the slices will be stored to.')
    args = parser.parse_args()
    slice_rel(args.rel_file, args.output, ALIAS_FILE)
