from elffile import ElfFile
from elfconsts import DWARF_TAG, DWARF_AT, DWARF_FORM
import struct
from typing import cast

address_size = -1

def get_address_size() -> int:
    global address_size
    if address_size == -1:
        raise Exception('Address size not set yet')
    return address_size

def read_leb128(data: bytes, offset: int) -> tuple[int, int]:
    result = 0
    shift = 0
    while True:
        byte = data[offset]
        result |= (byte & 0x7F) << shift
        offset += 1
        if (byte & 0x80) == 0:
            break
        shift += 7
    return result, offset

class DWARFAttribute:
    def __init__(self, name: DWARF_AT, form: DWARF_FORM):
        self.name = name
        self.form = form

    @staticmethod
    def parse_int(size: int, data: bytes, pos: int) -> tuple[int, int]:
        return int.from_bytes(data[pos:pos+size], 'big'), pos + size

    @staticmethod
    def parse_bytes(size: int, data: bytes, pos: int) -> tuple[bytes, int]:
        return data[pos:pos+size], pos + size

    def parse(self, data: bytes, pos: int) -> tuple[object, int]:
        match self.form:
            case DWARF_FORM.DW_FORM_addr | DWARF_FORM.DW_FORM_ref_addr:
                return self.parse_int(get_address_size(), data, pos)
            case DWARF_FORM.DW_FORM_data1:
                return self.parse_int(1, data, pos)
            case DWARF_FORM.DW_FORM_data2:
                return self.parse_int(2, data, pos)
            case DWARF_FORM.DW_FORM_data4:
                return self.parse_int(4, data, pos)
            case DWARF_FORM.DW_FORM_udata:
                return read_leb128(data, pos)
            case DWARF_FORM.DW_FORM_string:
                end = data.find(b'\0', pos)
                return data[pos:end].decode('utf-8'), end + 1
            case DWARF_FORM.DW_FORM_block:
                length, pos = read_leb128(data, pos)
                return self.parse_bytes(length, data, pos)
            case DWARF_FORM.DW_FORM_block1:
                length, pos = self.parse_int(1, data, pos)
                return self.parse_bytes(length, data, pos)
            case DWARF_FORM.DW_FORM_block2:
                length, pos = self.parse_int(2, data, pos)
                return self.parse_bytes(length, data, pos)
            case DWARF_FORM.DW_FORM_flag:
                return data[pos] != 0, pos + 1
            case _:
                raise NotImplementedError(f'Form {self.form.name} not implemented')

    def __repr__(self) -> str:
        return f'DWARFAttribute(name={self.name}, form={self.form})'

class DWARFAbbrevEntry:
    def __init__(self, tag: DWARF_TAG, has_children: bool, attributes: list[DWARFAttribute]):
        self.tag = tag
        self.has_children = has_children
        self.attributes = attributes

    def parse(self, data: bytes, pos: int) -> tuple[dict[DWARF_AT, tuple[DWARF_FORM, object]], int]:
        result = dict()
        for attr in self.attributes:
            value, pos = attr.parse(data, pos)
            result[attr.name] = (attr.form, value)
        return result, pos

    def __repr__(self) -> str:
        return f'DWARFAbbrevEntry(tag={self.tag}, has_children={self.has_children}, attributes={self.attributes})'

class DWARFAbbrevSection:
    def __init__(self, entries: dict[int, DWARFAbbrevEntry]):
        self.entries = entries

    @staticmethod
    def read(data: bytes) -> 'DWARFAbbrevSection':
        entries = dict()
        pos = 0
        while pos < len(data):
            code, pos = read_leb128(data, pos)
            if code == 0:
                break

            tag, pos = read_leb128(data, pos)
            tag = DWARF_TAG(tag)

            has_children = data[pos] == 1
            pos += 1

            attributes = []
            while True:
                attribute_name, pos = read_leb128(data, pos)
                attribute_form, pos = read_leb128(data, pos)

                if attribute_name == 0 and attribute_form == 0:
                    break
                if attribute_name == 8224:
                    # Custom MWCC attribute
                    attribute_name = DWARF_AT.DW_AT_lo_user

                attributes.append(DWARFAttribute(DWARF_AT(attribute_name), DWARF_FORM(attribute_form)))
            entries[code] = DWARFAbbrevEntry(tag, has_children, attributes)
        return DWARFAbbrevSection(entries)

class DWARFTreeNode:
    def __init__(self, abbrev_entry: DWARFAbbrevEntry, cu_offset: int):
        self.tag = abbrev_entry.tag
        self.cu_offset = cu_offset
        self.node: dict[DWARF_AT, tuple[DWARF_FORM, object]] = dict()
        self.children: list[DWARFTreeNode] = []

    def get_attr(self, attr: DWARF_AT) -> object:
        return self.node[attr][1]

    @staticmethod
    def read(abbrev_entry_section: DWARFAbbrevSection, data: bytes, pos: int) -> tuple['DWARFTreeNode', dict[int, 'DWARFTreeNode'], int]:
        node_lookup = dict()
        cu_offset = pos
        code, pos = read_leb128(data, pos)
        abbrev_entry = abbrev_entry_section.entries[code]

        # Read main node
        node = DWARFTreeNode(abbrev_entry, cu_offset)
        node.node, pos = abbrev_entry.parse(data, pos)

        # Read children if needed
        if abbrev_entry.has_children:
            while data[pos] != 0:
                child_node, child_lookup, pos = DWARFTreeNode.read(abbrev_entry_section, data, pos)
                node.children.append(child_node)
                node_lookup.update(child_lookup)
            pos += 1 # Skip the null byte that indicates the end of children

        node_lookup[cu_offset] = node

        return node, node_lookup, pos

    def link_references(self, node_lookup: dict[int, 'DWARFTreeNode']) -> None:
        for (attr_name, (attr_form, value)) in self.node.items():
            if attr_name in (DWARF_AT.DW_AT_sibling, DWARF_AT.DW_AT_low_pc, DWARF_AT.DW_AT_high_pc):
                continue
            if attr_form == DWARF_FORM.DW_FORM_ref_addr:
                offset = cast(int, value)
                try:
                    self.node[attr_name] = attr_form, node_lookup[cast(int, value)]
                except KeyError:
                    raise ValueError(f'Failed to link reference for {attr_name.name} at offset {hex(offset)}')
        for child in self.children:
            child.link_references(node_lookup)

    def __repr__(self) -> str:
        return f'DWARFTreeNode(tag={self.tag.name}, node={self.node})'

    def dump(self, indent: int = 0, with_children: bool = True) -> str:
        indent_str = '  ' * indent
        result = f'{indent_str}{self.tag.name} | {hex(self.cu_offset)}:\n'
        for (key, (form, value)) in self.node.items():
            val_str = '?'
            if isinstance(value, int):
                val_str = hex(value)
            elif key == DWARF_AT.DW_AT_data_member_location:
                val_bytes = cast(bytes, value)
                assert val_bytes[0] == 0x23 # DW_OP_plus_uconst
                val_offs, _ = read_leb128(val_bytes, 1)
                val_str = hex(val_offs)
            elif isinstance(value, DWARFTreeNode):
                val_str = f'\n{indent_str}    {value.dump(indent + 2, with_children=False).strip()}'
            elif isinstance(value, bytes):
                val_str = value.hex()
            else:
                val_str = value
            result += f'{indent_str}  {key.name} ({form.name}): {val_str}\n'

        if with_children:
            for child in self.children:
                result += child.dump(indent + 1)
        return result

class ELFDwarfV2:
    def __init__(self, elf_file: ElfFile) -> None:
        self.elf_file = elf_file
        self.debug_info = self.elf_file.get_section('.debug_info')
        self.debug_abbrev = self.elf_file.get_section('.debug_abbrev')
        self.parse_debug_info()

    def read_abbrev(self, offset: int) -> DWARFAbbrevSection:
        return DWARFAbbrevSection.read(self.debug_abbrev.data[offset:])

    def parse_debug_info(self) -> None:
        global address_size

        self.compile_units: list[DWARFTreeNode] = []
        debug_info_header = struct.Struct('>IHIB')
        pos = 0
        node_lookup: dict[int, DWARFTreeNode] = dict()
        while pos < len(self.debug_info.data):
            header = debug_info_header.unpack_from(self.debug_info.data, pos)
            length, _version, abbrev_offset, address_size = header
            start_pos = pos

            data, cu_lookup, pos = DWARFTreeNode.read(self.read_abbrev(abbrev_offset), self.debug_info.data, pos + debug_info_header.size)
            node_lookup.update(cu_lookup)
            self.compile_units.append(data)

            assert(pos == start_pos + length + 4)

        for cu in self.compile_units:
            cu.link_references(node_lookup)
