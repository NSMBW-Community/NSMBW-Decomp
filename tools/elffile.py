#!/usr/bin/env python3
# ELF definitions

import struct
from functools import reduce

from elfconsts import *

class ElfHeader:
    _struct = struct.Struct('>16sHHIIIII6H')

    def __init__(self, e_type=ET.ET_NONE, e_machine=EM.EM_NONE, e_entry=0, e_flags=0) -> None:
        self.e_ident: bytes = b'\x7FELF\x01\x02\x01\0\0' + b'\0'*6
        self.e_type: ET = e_type
        self.e_machine: EM = e_machine
        self.e_version: int = 1
        self.e_entry: int = e_entry
        self.e_flags: int = e_flags
        self.e_ehsize: int = ElfHeader._struct.size
        self.e_phentsize: int = 0x20
        self.e_shentsize: int = 0x28

        # Set later
        self.e_phoff: int = None
        self.e_shoff: int = None
        self.e_phnum: int = None
        self.e_shnum: int = None
        self.e_shstrndx: int = None

    @staticmethod
    def read(data: bytes, offset: int=0) -> 'ElfHeader':
        eh_res = ElfHeader()
        (
            eh_res.e_ident,
            eh_res.e_type,
            eh_res.e_machine,
            eh_res.e_version,
            eh_res.e_entry,
            eh_res.e_phoff,
            eh_res.e_shoff,
            eh_res.e_flags,
            eh_res.e_ehsize,
            eh_res.e_phentsize,
            eh_res.e_phnum,
            eh_res.e_shentsize,
            eh_res.e_shnum,
            eh_res.e_shstrndx
        ) = ElfHeader._struct.unpack(data[offset:offset+ElfHeader._struct.size])
        return eh_res

    def __bytes__(self) -> bytes:
        assert self.e_phoff is not None
        assert self.e_shoff is not None
        assert self.e_phnum is not None
        assert self.e_shnum is not None
        assert self.e_shstrndx is not None

        return bytes(ElfHeader._struct.pack(
            self.e_ident,
            self.e_type.value,
            self.e_machine.value,
            self.e_version,
            self.e_entry,
            self.e_phoff,
            self.e_shoff,
            self.e_flags,
            self.e_ehsize,
            self.e_phentsize,
            self.e_phnum,
            self.e_shentsize,
            self.e_shnum,
            self.e_shstrndx
        ))


class ElfSectionHeader:
    _struct = struct.Struct('>10I')

    def __init__(self, sh_type=SHT.SHT_NULL, sh_flags: set[SHF]=set(), sh_addr=0, sh_link=0, sh_info=0, sh_addralign=0) -> None:
        self.sh_type: SHT = sh_type
        self.sh_flags: set[SHF] = sh_flags
        self.sh_addr: int = sh_addr
        self.sh_link: int = sh_link
        self.sh_info: int = sh_info
        self.sh_addralign: int = sh_addralign

        # Set later
        self.sh_name: int = None
        self.sh_offset: int = None
        self.sh_size: int = None
        self.sh_entsize: int = None

    @staticmethod
    def read(data: bytes, offset: int=0) -> 'ElfSectionHeader':
        print(offset, len(data))
        esh_res = ElfSectionHeader()
        (
            esh_res.sh_name,
            esh_res.sh_type,
            _sh_flags,
            esh_res.sh_addr,
            esh_res.sh_offset,
            esh_res.sh_size,
            esh_res.sh_link,
            esh_res.sh_info,
            esh_res.sh_addralign,
            esh_res.sh_entsize
        ) = ElfSectionHeader._struct.unpack(data[offset:offset+ElfSectionHeader._struct.size])
        esh_res.sh_flags = set([x for x in SHF if x.value & _sh_flags])
        esh_res.sh_type = SHT(esh_res.sh_type)
        return esh_res

    def __bytes__(self) -> bytes:
        assert self.sh_name is not None
        assert self.sh_offset is not None
        assert self.sh_size is not None
        assert self.sh_entsize is not None

        return bytes(ElfSectionHeader._struct.pack(
            self.sh_name,
            self.sh_type.value,
            reduce(lambda a, b: a | b.value, self.sh_flags, 0),
            self.sh_addr,
            self.sh_offset,
            self.sh_size,
            self.sh_link,
            self.sh_info,
            self.sh_addralign,
            self.sh_entsize
        ))

    def has_data(self) -> bool:
        return self.sh_type != SHT.SHT_NOBITS

class ElfSection:
    def __init__(self, name='', data: bytes=None, header: ElfSectionHeader=None) -> None:
        if not header:
            h_type, h_flags = SpecialSections.get(name)
            self.header = ElfSectionHeader(h_type, h_flags)
            self.header.sh_size = 0
            self.header.sh_entsize = 0
        else:
            self.header = header

        self.name: str = name
        self.data: bytes = data

        # Set later
        self.header.sh_name: int = None

    def read(self, data: bytes, header: ElfSectionHeader) -> None:
        self.header = header

        if self.header.has_data():
            assert len(data) == self.header.sh_size
            self.data = data

    def _prepare_for_write(self) -> None:
        if self.data:
            self.header.sh_size = len(self.data)

    def link_strtab(self, shstrtab: 'ElfStrtab', strtab: 'ElfStrtab') -> None:
        self.header.sh_name = shstrtab.add_string(self.name)

    # Note: returns only data bytes, not header bytes
    def __bytes__(self) -> bytes:
        self._prepare_for_write()
        return self.data if self.data else bytes()

    def size(self) -> int:
        if self.data:
            return len(self.data)
        if self.header.sh_size:
            return self.header.sh_size
        return 0


class ElfStrtab(ElfSection):
    def __init__(self, name='.strtab', strs: list[str]=[]) -> None:
        super().__init__(name)
        self.strs = strs
        self.offs = 1 # first index is null byte

    def read(self, data: bytes, header: ElfSectionHeader) -> None:
        super().read(data, header)
        self.strs = self.data.decode('utf-8').split('\0')

    def _prepare_for_write(self) -> None:
        self.data = b'\0' + b'\0'.join([str.encode('utf-8') for str in self.strs]) + b'\0'
        return super()._prepare_for_write()

    def add_string(self, str: str) -> int:
        if str == '':
            return 0
        self.strs.append(str)
        ret = self.offs
        self.offs += len(str) + 1
        return ret


class ElfSymbol:
    _struct = struct.Struct('>IIIBBH')

    def __init__(self, name: str, st_value=0, st_size=0, st_info_bind: STB=STB.STB_LOCAL, st_info_type: STT=STT.STT_NOTYPE, st_other: STV=STV.STV_DEFAULT, st_shndx: int = STN.STN_UNDEF.value) -> None:
        self.name: str = name
        self.st_value: int = st_value
        self.st_size: int = st_size
        self.st_info_bind: STB = st_info_bind
        self.st_info_type: STT = st_info_type
        self.st_other: STV = st_other
        self.st_shndx: int = st_shndx

        # Set later
        self.st_name: int = None

    def read(data: bytes, offset: int=0) -> 'ElfSymbol':
        sym = ElfSymbol()
        (
            sym.st_name,
            sym.st_value,
            sym.st_size,
            _st_info,
            sym.st_other,
            sym.st_shndx
        ) = ElfSymbol._struct.unpack(data[offset:offset+ElfSymbol._struct.size])
        sym.st_info_bind = STB(_st_info >> 4)
        sym.st_info_type = STT(_st_info & 0xF)
        return sym

    def __bytes__(self) -> bytes:
        assert self.st_name is not None

        return bytes(ElfSymbol._struct.pack(
            self.st_name,
            self.st_value,
            self.st_size,
            (self.st_info_bind.value << 4) | self.st_info_type.value,
            self.st_other.value,
            self.st_shndx
        ))


class ElfSymtab(ElfSection):
    def __init__(self, name='.symtab', syms: list[ElfSymbol]=[ElfSymbol('')]) -> None:
        super().__init__(name)

        self.syms = syms

    def read(self, data: bytes, header: ElfSectionHeader) -> None:
        super().read(data, header)
        syms = []
        for i in range(0, self.size(), ElfSymbol._struct.size):
            syms.append(ElfSymbol.read(self.data, i))
        self.syms = syms

    def _prepare_for_write(self) -> None:
        self.data = b''.join([bytes(sym) for sym in self.syms])
        self.header.sh_entsize = ElfSymbol._struct.size
        last_local = [idx for idx, sym in enumerate(self.syms) if sym.st_info_bind == STB.STB_LOCAL][-1]
        self.header.sh_info = last_local + 1
        super()._prepare_for_write()

    def link_strtab(self, shstrtab: 'ElfStrtab', strtab: 'ElfStrtab') -> None:
        for sym in self.syms:
            sym.st_name = strtab.add_string(sym.name)
        super().link_strtab(shstrtab, strtab)

    def add_symbol(self, sym: ElfSymbol) -> int:
        self.syms.append(sym)
        return len(self.syms) - 1


class ElfRela:
    _struct = struct.Struct('>IIi')

    def __init__(self, r_offset=0, r_info_sym=0, r_info_type: PPC_RELOC_TYPE=PPC_RELOC_TYPE.R_PPC_NONE, r_addend=0) -> None:
        self.r_offset: int = r_offset
        self.r_info_sym: int = r_info_sym
        self.r_info_type: PPC_RELOC_TYPE = r_info_type
        self.r_addend: int = r_addend

    def read(data: bytes, offset: int=0) -> 'ElfRela':
        rela = ElfRela()
        rela.r_offset, _r_info, rela.r_addend = ElfRela._struct.unpack(data[offset:offset+ElfRela._struct.size])
        rela.r_info_sym = _r_info >> 8
        rela.r_info_type = PPC_RELOC_TYPE(_r_info & 0xF)
        return rela

    def __bytes__(self) -> bytes:
        return bytes(ElfRela._struct.pack(
            self.r_offset,
            (self.r_info_sym << 8) | self.r_info_type.value,
            self.r_addend
        ))


class ElfRelaSec(ElfSection):
    def __init__(self, name='.rela', relocs: list[ElfRela]=[]) -> None:
        super().__init__(name)
        self.relocs = relocs

    def read(self, data: bytes, header: ElfSectionHeader) -> None:
        super().read(data, header)
        relocs = []
        for i in range(0, self.size(), ElfRela._struct.size):
            relocs.append(ElfRela.read(self.data, i))
        self.syms = relocs

    def add_reloc(self, reloc: ElfRela) -> None:
        self.relocs.append(reloc)

    def _prepare_for_write(self) -> None:
        self.data = b''.join([bytes(reloc) for reloc in self.relocs])
        self.header.sh_entsize = ElfRela._struct.size
        super()._prepare_for_write()


class ElfFile:
    def __init__(self, e_type: ET=ET.ET_NONE, e_machine=EM.EM_NONE) -> None:
        self.e_header = ElfHeader(e_type, e_machine)
        self.sections: list[ElfSection] = [ElfSection()] # NULL section

    def read(data: bytes) -> 'ElfFile':
        elf_file = ElfFile()
        elf_file.e_header = ElfHeader.read(data, 0)
        elf_file.sections = []
        sec_hdr_offs = elf_file.e_header.e_shoff
        sec_hdr_size = elf_file.e_header.e_shentsize

        # TODO: load shstrtab first, the strtab and then the rest
        # TODO: add helper functions to read from strtab
        for i in range(elf_file.e_header.e_shnum):
            print(sec_hdr_offs, i*sec_hdr_size)

            sec_hdr = ElfSectionHeader.read(data, sec_hdr_offs + i*sec_hdr_size)
            if sec_hdr.has_data():
                sec_data = data[sec_hdr.sh_offset:sec_hdr.sh_offset+sec_hdr.sh_size]

            if sec_hdr.sh_type == SHT.SHT_PROGBITS:
                elf_sec = ElfSection().read(sec_data, sec_hdr)
            elif sec_hdr.sh_type == SHT.SHT_RELA:
                elf_sec = ElfRelaSec().read(sec_data, sec_hdr)
            elif sec_hdr.sh_type == SHT.SHT_STRTAB:
                elf_sec = ElfStrtab().read(sec_data, sec_hdr)
            elif sec_hdr.sh_type == SHT.SHT_SYMTAB:
                elf_sec = ElfSymtab().read(sec_data, sec_hdr)
            else:
                elf_sec = ElfSection('', bytes(), sec_hdr)
            elf_file.sections.append(elf_sec)

    def add_section(self, sect: ElfSection) -> int:
        self.sections.append(sect)
        return len(self.sections) - 1

    def has_section(self, name: str) -> bool:
        return bool([x for x in self.sections if x.name == name])

    def get_section(self, name: str) -> ElfSection:
        if not self.has_section(name):
            return None
        return next(x for x in self.sections if x.name == name)

    def get_section_index(self, name: str) -> int:
        for i, sec in enumerate(self.sections):
            if sec.name == name:
                return i
        return None

    def __bytes__(self) -> bytes:
        if not self.has_section('.shstrtab'):
            self.sections.append(ElfStrtab('.shstrtab'))
        if not self.has_section('.strtab'):
            self.sections.append(ElfStrtab('.strtab'))

        shstrtabndx = self.get_section_index('.shstrtab')
        shstrtab = self.get_section('.shstrtab')
        strtab = self.get_section('.strtab')

        self.e_header.e_phoff = 0
        self.e_header.e_phnum = 0
        self.e_header.e_shstrndx = shstrtabndx

        for sec in self.sections:
            sec.link_strtab(shstrtab, strtab)

        data = bytearray(b'\0'*ElfHeader._struct.size) # Filled out at the end
        for sec in self.sections:
            sec_data = bytes(sec)
            sec.header.sh_offset = len(data) if sec.header.sh_type != SHT.SHT_NULL else 0
            data.extend(sec_data)

        self.e_header.e_shnum = len(self.sections)
        self.e_header.e_shoff = len(data)

        for sec in self.sections:
            sec_hdr_data = bytes(sec.header)
            data.extend(sec_hdr_data)

        header = bytes(self.e_header)
        data[0:len(header)] = header

        return bytes(data)
