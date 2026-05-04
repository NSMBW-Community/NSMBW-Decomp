#!/usr/bin/env python3
# Generates a linker script from a slice file.

from pathlib import Path
from typing import cast

from elffile import *
from elfdwarfv2 import DWARFTreeNode, ELFDwarfV2, read_leb128
from project_settings import *
from slicelib import *

structure_file_types = {
    'unsigned char': 'u8',
    'unsigned short': 'u16',
    'unsigned int': 'u32',
    'unsigned long': 'u32',
    'char': 's8',
    'short': 's16',
    'int': 's32',
    'long': 's32',
    'long long': 's64',
    'signed char': 's8',
    'signed short': 's16',
    'signed int': 's32',
    'signed long': 's32',
    'void': 'void',
    'bool': 'bool',
    'float': 'float',
    'wchar_t': 'u16'
}

def format_type(node: DWARFTreeNode) -> str:
    match node.tag:
        case DWARF_TAG.DW_TAG_pointer_type | DWARF_TAG.DW_TAG_reference_type:
            fmt_type = format_type(cast(DWARFTreeNode, node.get_attr(DWARF_AT.DW_AT_type)))
            if fmt_type == 'void':
                return 'ptr'
            elif fmt_type == 's8':
                return 'stringJIS'
            return f'*{fmt_type}'
        case DWARF_TAG.DW_TAG_class_type | DWARF_TAG.DW_TAG_structure_type:
            return cast(str, f'{node.get_attr(DWARF_AT.DW_AT_name)}_{node.cu_offset}')
        case DWARF_TAG.DW_TAG_base_type:
            name = cast(str, node.get_attr(DWARF_AT.DW_AT_name))
            if 'long long' in name:
                return 'u32'
            return structure_file_types[name]
        case DWARF_TAG.DW_TAG_subroutine_type | \
                DWARF_TAG.DW_TAG_array_type | \
                DWARF_TAG.DW_TAG_ptr_to_member_type:
            return 'ptr'
        case DWARF_TAG.DW_TAG_enumeration_type:
            return 'u32'
        case _:
            raise ValueError(f'Unsupported type tag: {node.tag.name}')

def gen_structure_file(elf_path: Path, out_path: Path) -> None:
    assert elf_path.is_file()

    elf_file = ElfFile.read(elf_path.read_bytes())
    dbg_info = ELFDwarfV2(elf_file)

    structure_file = ''

    for cu in dbg_info.compile_units:
        for node in cu.children:
            if node.tag == DWARF_TAG.DW_TAG_class_type:
                print(node.dump())
                inherits_from: DWARFTreeNode | None = None
                members = []
                for c in node.children:
                    match c.tag:
                        case DWARF_TAG.DW_TAG_inheritance:
                            if inherits_from is not None:
                                continue
                            inherits_from = cast(DWARFTreeNode, c.get_attr(DWARF_AT.DW_AT_type))
                        case DWARF_TAG.DW_TAG_member:
                            member_name = cast(str, c.get_attr(DWARF_AT.DW_AT_name))
                            member_type = c.get_attr(DWARF_AT.DW_AT_type)
                            member_offset_bytes = cast(bytes, c.get_attr(DWARF_AT.DW_AT_data_member_location))
                            assert member_offset_bytes[0] == 0x23 # DW_OP_plus_uconst
                            member_offset, _ = read_leb128(member_offset_bytes, 1)
                            members.append((member_name, member_type, member_offset))

                name = f'{cast(str, node.get_attr(DWARF_AT.DW_AT_name))}_{node.cu_offset}'
                inherit_str = '-' if inherits_from is None else f'{cast(str, inherits_from.get_attr(DWARF_AT.DW_AT_name))}_{inherits_from.cu_offset}'
                byte_count = cast(int, node.get_attr(DWARF_AT.DW_AT_byte_size))
                structure_file += f'structure {name}:{inherit_str}:{hex(byte_count)} {{'
                for member_name, member_type, member_offset in members:
                    fmt_type = format_type(member_type)
                    structure_file += f'\n  {hex(member_offset)} {member_name}:{fmt_type};'
                structure_file += '\n}\n\n'

    out_path.write_text(structure_file)

# TODO: Also import RELs

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Generates a structures.txt file from an executable.')
    parser.add_argument('elf_file', type=Path, help='Path to the ELF file.')
    parser.add_argument('-o', '--output', type=Path, required=True, help='Path to the structures file.')
    args = parser.parse_args()
    gen_structure_file(args.elf_file, args.output)
