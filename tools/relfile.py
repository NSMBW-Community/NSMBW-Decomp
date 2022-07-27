#!/usr/bin/env python3
# REL definitions and helper functions
# See https://wiki.tockdom.com/wiki/REL_(File_Format)

from math import ceil
import struct
import typing

from elfconsts import PPC_RELOC_TYPE

class RelRelocation:
    struct = struct.Struct('>HBBI')

    def __init__(self, file: bytearray = None, offset: int = 0) -> None:
        if file:
            self._read(file, offset)
        else:
            self.offset: int = 0
            self.reloc_type: PPC_RELOC_TYPE = PPC_RELOC_TYPE.R_RVL_NONE
            self.section: int = 0
            self.addend: int = 0

    def __repr__(self) -> str:
        return f'<Relocation: offset={self.offset} reloc_type={self.reloc_type} section={self.section} addend={self.addend}>'

    @staticmethod
    def entry_size() -> int:
        return 8

    def _read(self, file: bytearray, offset: int) -> None:
        self.offset, _reloc_type, self.section, self.addend = self.struct.unpack(file[offset:offset+8])
        self.reloc_type = PPC_RELOC_TYPE(_reloc_type)

    def write(self, file: typing.BinaryIO) -> None:
        assert self.offset < 0x10000
        file.write(self.struct.pack(self.offset, self.reloc_type.value, self.section, self.addend))


class RelSection:
    struct = struct.Struct('>II')

    def __init__(self, file: typing.BinaryIO = None, info_offset: int = 0) -> None:
        self.is_bss: bool = False

        if file:
            self._read(file, info_offset)
        else:
            self.executable: bool = False
            self._sec_len: int = 0
            self._data: bytearray = bytearray()
            self.alignment: int = 4 # Used for alignment of the section within the file, but not directly written to the file

    def __repr__(self) -> str:
        return f'<Section: {"" if self.executable else "non-"}executable, length {self._sec_len}>'

    def set_data(self, data: bytearray) -> None:
        self._data = data
        self._sec_len = len(data)

    def data_length(self) -> int:
        return self._sec_len

    def get_data(self) -> bytearray:
        return self._data if not self.is_bss else bytearray()

    @staticmethod
    def header_size() -> int:
        return 8

    def write_info(self, file: typing.BinaryIO, data_offs: int) -> None:
        assert data_offs % 4 == 0, 'Sections must be 4-byte aligned!'
        i_data = self.struct.pack(data_offs | self.executable if not self.is_bss else 0, self._sec_len)
        file.write(i_data)

    def write_data(self, file: typing.BinaryIO) -> None:
        if not self.is_bss:
            file.write(self._data)

    def _read(self, file: bytearray, info_offset: int) -> None:
        data_offs_flags, self._sec_len = self.struct.unpack(file[info_offset:info_offset+8])
        self.executable = bool(data_offs_flags & 1)
        data_offs = data_offs_flags & ~0b11
        if data_offs != 0:
            self._data = file[data_offs:data_offs+self._sec_len]
        else:
            self.is_bss = True
            self.bss_size = self._sec_len


class Rel:
    imp_struct = header2_struct = struct.Struct('>II')
    header_struct = struct.Struct('>12I4B3I')

    def __init__(self, id: int, version: int = 3, align: int = 4, bss_align: int = 8, path_offset: int = 0, path_size: int = 0, file: typing.BinaryIO = None) -> None:
        self.index = id
        self.path_offset = path_offset
        self.path_size = path_size
        self.version = version if version in range(0, 4) else 3
        self.align: int = align
        self.bss_align: int = bss_align

        self.sections: list[RelSection] = []
        self.relocations: dict[int, list[RelRelocation]] = {}

        # To be filled out later
        self.section_info_offset: int = 0
        self.bss_size: int = 0
        self.rel_offset: int = 0
        self.imp_offset: int = 0
        self.imp_size: int = 0
        self.prolog_section: int = 0
        self.epilog_section: int = 0
        self.unresolved_section: int = 0
        self.prolog: int = 0
        self.epilog: int = 0
        self.unresolved: int = 0
        self.fix_size: int = 0

        if version >= 2:
            self.align = align
            self.bss_align = bss_align

        if file:
            self.read(file)

    def __repr__(self) -> str:
        return str(vars(self))

    def read(self, file: typing.BinaryIO) -> None:
        bytes = bytearray(file.read())
        (
            self.index,
            _,
            _,
            section_count,
            self.section_info_offset,
            self.path_offset,
            self.path_size,
            self.version,
            self.bss_size,
            self.rel_offset,
            self.imp_offset,
            self.imp_size,
            self.prolog_section,
            self.epilog_section,
            self.unresolved_section,
            _,
            self.prolog,
            self.epilog,
            self.unresolved
        ) = self.header_struct.unpack(bytes[0:0x40])

        if self.version >= 2:
            self.align, self.bss_align = self.header2_struct.unpack(bytes[0x40:0x48])

        if self.version >= 3:
            self.fix_size = int.from_bytes(bytes[0x48:0x4c], 'big')

        # Sections
        for i in range(section_count):
            self.sections.append(RelSection(bytes, self.section_info_offset + i*8))

        # Relocations
        for i in range(0, self.imp_size, 8):
            module_num, table_offs = self.imp_struct.unpack(bytes[self.imp_offset+i:self.imp_offset+i+8])
            assert module_num not in self.relocations, 'Module numbers must be unique!'

            self.relocations[module_num] = []
            pos = table_offs
            while pos < len(bytes):
                reloc = RelRelocation(bytes, pos)
                self.relocations[module_num].append(reloc)
                pos += 8
                if reloc.reloc_type == PPC_RELOC_TYPE.R_RVL_STOP:
                    break

    def add_section(self, section: RelSection) -> None:
        self.sections.append(section)

    def _write_header(self, file: typing.BinaryIO) -> None:
        header = self.header_struct.pack(
            self.index,
            0,
            0,
            len(self.sections),
            self.section_info_offset,
            self.path_offset,
            self.path_size,
            self.version,
            self.bss_size,
            self.rel_offset,
            self.imp_offset,
            self.imp_size,
            self.prolog_section,
            self.epilog_section,
            self.unresolved_section,
            0,
            self.prolog,
            self.epilog,
            self.unresolved)
        file.write(header)

        if self.version >= 2:
            header = self.header2_struct.pack(self.align, self.bss_align)
            file.write(header)

        if self.version >= 3:
            header = self.fix_size.to_bytes(4, 'big')
            file.write(header)

    def header_size(self) -> int:
        return 0x4c if self.version == 3 else 0x48 if self.version == 2 else 0x40

    def _try_relocate_rel24(self, section_data_locs: list[int]) -> None:
        for module in self.relocations:
            curr_pos = 0
            curr_section = 0
            idx = 0

            while idx < len(self.relocations[module]):
                reloc = self.relocations[module][idx]

                if reloc.reloc_type == PPC_RELOC_TYPE.R_RVL_SECT:
                    curr_pos = 0
                    curr_section = reloc.section
                curr_pos += reloc.offset

                if reloc.reloc_type == PPC_RELOC_TYPE.R_PPC_REL24:

                    # In own module, directly relocate and remove the relocation entry
                    # Otherwise, jump to _unresolved
                    should_remove_reloc = module == self.index
                    if should_remove_reloc:
                        goal_addr = section_data_locs[reloc.section] + reloc.addend
                    else:
                        goal_addr = section_data_locs[self.unresolved_section] + self.unresolved

                    own_pos = section_data_locs[curr_section] + curr_pos
                    offset = (goal_addr - own_pos) & 0xFFFFFFFF
                    mask = 0xFC000003
                    curr_data = int.from_bytes(self.sections[curr_section]._data[curr_pos:curr_pos+4], 'big')
                    curr_data = (curr_data & mask) | (offset & ~mask)
                    self.sections[curr_section]._data[curr_pos:curr_pos+4] = curr_data.to_bytes(4, 'big')

                    # Remove the relocation and fix up the next one
                    if should_remove_reloc:
                        old_offset = reloc.offset
                        curr_pos -= reloc.offset
                        self.relocations[module].remove(reloc)
                        if self.relocations[module][idx].reloc_type.value < 201:
                            self.relocations[module][idx].offset += old_offset
                        idx -= 1

                idx += 1

    def write(self, file: typing.BinaryIO) -> None:
        self.section_info_offset = self.header_size()

        section_data_locs = []
        pos = self.section_info_offset + len(self.sections) * RelSection.header_size()
        for sec in self.sections:
            if sec.data_length() == 0 or sec.is_bss:
                section_data_locs.append(0)
            else:
                pos = ceil(pos / sec.alignment) * sec.alignment
                section_data_locs.append(pos)
                pos += sec.data_length()

        # We have to apply REL24 to point to the symbol if found, and to _unresolved if in other module
        # This is done after we know the location of all sections, but before the size of the relocation table is calculated
        self._try_relocate_rel24(section_data_locs)

        self.imp_offset = pos
        self.imp_size = len(self.relocations) * 8
        pos += self.imp_size
        self.rel_offset = pos

        reloc_locs = {}

        # Sort relocation tables
        relocs_module_nums: list[int] = sorted(
            [key for key in self.relocations if key != self.index and key != 0]
        )
        if self.index in self.relocations:
            relocs_module_nums.append(self.index)
        if 0 in self.relocations:
            relocs_module_nums.append(0)

        for module_num in relocs_module_nums:
            pos = ceil(pos / 4) * 4
            reloc_locs[module_num] = pos
            pos += len(self.relocations[module_num]) * RelRelocation.entry_size()

        # Not sure why this location is chosen by makerel
        if self.version >= 3:
            self.fix_size = reloc_locs[self.index]

        # Write data
        self._write_header(file)
        file.seek(self.section_info_offset)
        for i, sec in enumerate(self.sections):
            sec.write_info(file, section_data_locs[i])

        for i, sec in enumerate(self.sections):
            if sec.data_length() == 0 or sec.is_bss:
                continue
            file.seek(section_data_locs[i])
            sec.write_data(file)

        file.seek(self.imp_offset)
        for module_num in relocs_module_nums:
            file.write(self.imp_struct.pack(module_num, reloc_locs[module_num]))

        for module_num in relocs_module_nums:
            file.seek(reloc_locs[module_num])
            for reloc in self.relocations[module_num]:
                reloc.write(file)
