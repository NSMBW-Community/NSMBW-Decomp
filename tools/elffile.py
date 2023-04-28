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
    def read(data: bytes, offset: int) -> 'ElfHeader':
        eh_res = ElfHeader()
        (
            eh_res.e_ident,
            e_type,
            e_machine,
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
        eh_res.e_type = ET(e_type)
        eh_res.e_machine = EM(e_machine)
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

    def __init__(self, sh_type=SHT.SHT_NULL, sh_flags: set[SHF]=None, sh_addr=0, sh_link=0, sh_info=0, sh_addralign=0) -> None:
        self.sh_type: SHT = sh_type
        self.sh_flags: set[SHF] = sh_flags if sh_flags else set()
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
    def read(data: bytes, offset: int) -> 'ElfSectionHeader':
        sh_res = ElfSectionHeader()
        (
            sh_res.sh_name,
            sh_res.sh_type,
            _sh_flags,
            sh_res.sh_addr,
            sh_res.sh_offset,
            sh_res.sh_size,
            sh_res.sh_link,
            sh_res.sh_info,
            sh_res.sh_addralign,
            sh_res.sh_entsize
        ) = ElfSectionHeader._struct.unpack(data[offset:offset+ElfSectionHeader._struct.size])
        sh_res.sh_flags = set([x for x in SHF if x.value & _sh_flags])
        sh_res.sh_type = SHT(sh_res.sh_type)
        return sh_res

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
        self.shndx: int = None

    @staticmethod
    def read(data: bytes, header: ElfSectionHeader) -> 'ElfSection':
        assert len(data) == header.sh_size
        return ElfSection('', data, header)

    def _freeze_shndx(self, idx: int) -> None:
        self.shndx = idx

    def _unfreeze_shndx(self) -> None:
        self.shndx = None

    def _prepare_for_write(self) -> None:
        if self.data:
            self.header.sh_size = len(self.data)

    def link_strtab(self, shstrtab: 'ElfStrtab', strtab: 'ElfStrtab', strtabndx: int) -> None:
        self.header.sh_name = shstrtab.add_string(self.name)

    # Note: returns only data bytes, not header bytes
    def __bytes__(self) -> bytes:
        self._prepare_for_write()
        return self.data if self.data else bytes()

    # Always use section.size() instead of len(section.data),
    # since NOBITS sections can still have a length, but no initialized data
    def size(self) -> int:
        if self.data:
            return len(self.data)
        if self.header.sh_size:
            # Needed for .bss section
            return self.header.sh_size
        return 0

    def __repr__(self) -> str:
        return f'elffile.ElfSection({self.name}, data=[{self.size()} bytes])'

class ElfStrtab(ElfSection):
    def __init__(self, name='.strtab', strs: list[str]=None) -> None:
        super().__init__(name)
        self.strs = strs if strs else []

        # For faster reads
        self.pos = 1
        self.lookup = {}
        for s in self.strs:
            self.lookup[self.pos] = s
            self.pos += len(s) + 1

    @staticmethod
    def read(data: bytes, header: ElfSectionHeader) -> 'ElfStrtab':
        assert len(data) == header.sh_size
        strs = [s for s in data.decode('utf-8').split('\0') if s != '']
        sec = ElfStrtab('', strs)
        sec.header = header
        sec.data = data
        return sec

    def clear(self):
        self.strs = []
        self.pos = 1
        self.lookup = {}

    def _prepare_for_write(self) -> None:
        self.data = b'\0' + '\0'.join([f'{str}' for str in self.strs]).encode('utf-8') + b'\0'
        return super()._prepare_for_write()

    def get_at_index(self, index: int) -> str:
        if index == 0:
            return ''
        return self.strs[index - 1]
    def get_at_offset(self, index: int) -> str:
        if index == 0:
            return ''
        return self.lookup[index] if index in self.lookup else ''

    def add_string(self, s: str) -> int:
        if s == '':
            return 0
        curr_pos = self.pos
        self.strs.append(s)
        self.lookup[curr_pos] = s
        self.pos += len(s) + 1
        return curr_pos


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
        self.owning_symtab: ElfSymtab = None
        self.locked_index: int = None

    @staticmethod
    def read(data: bytes, offset: int, strtab: ElfStrtab) -> 'ElfSymbol':
        sym = ElfSymbol('')
        (
            st_name,
            sym.st_value,
            sym.st_size,
            st_info,
            st_other,
            sym.st_shndx
        ) = ElfSymbol._struct.unpack(data[offset:offset+ElfSymbol._struct.size])
        sym.name = strtab.get_at_offset(st_name)
        sym.st_info_bind = STB(st_info >> 4)
        sym.st_info_type = STT(st_info & 0xF)
        sym.st_other = STV(st_other)
        return sym

    def _set_owner(self, symtab: 'ElfSymtab') -> None:
        self.owning_symtab = symtab

    def _set_locked_index(self, idx: int) -> None:
        self.locked_index = idx
        
    def _clear_locked_index(self) -> None:
        self.locked_index = None

    def get_idx(self) -> int:
        if self.locked_index != None:
            return self.locked_index
        assert self.owning_symtab is not None
        return self.owning_symtab.get_index_of_symbol(self)

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
    
    def __repr__(self) -> str:
        return f'elffile.ElfSymbol({self.name}, shndx={self.st_shndx}, value={self.st_value})'


class ElfSymtab(ElfSection):
    def __init__(self, name='.symtab', syms: list[ElfSymbol]=None, strtab: ElfStrtab=None) -> None:
        super().__init__(name)
        self.syms = syms if syms else []
        self.linked_sec = strtab

    @staticmethod
    def read(data: bytes, header: ElfSectionHeader, strtab: ElfStrtab) -> 'ElfSymtab':
        assert len(data) == header.sh_size
        sec = ElfSymtab('', [], strtab)
        # Skip NULL symbol
        for i in range(ElfSymbol._struct.size, len(data), ElfSymbol._struct.size):
            sym = ElfSymbol.read(data, i, strtab)
            sec.add_symbol(sym)
        sec.header = header
        return sec
    
    def _freeze_indices(self) -> None:
        for i, sym in enumerate(self.syms):
            sym._set_locked_index(i + 1)

    def _unfreeze_indices(self) -> None:
        for sym in self.syms:
            sym._clear_locked_index()

    def _prepare_for_write(self) -> None:
        assert self.linked_sec is not None
        null_sym = ElfSymbol('')
        null_sym.st_name = 0
        self.data = bytes(null_sym) + b''.join([bytes(sym) for sym in self.syms])
        self.header.sh_entsize = ElfSymbol._struct.size

        local_syms = [idx + 1 for idx, sym in enumerate(self.syms) if sym.st_info_bind == STB.STB_LOCAL]
        last_local = local_syms[-1] if len(local_syms) > 0 else 0
        self.header.sh_info = last_local + 1
        self.header.sh_link = self.linked_sec.shndx

        super()._prepare_for_write()

    def link_strtab(self, shstrtab: 'ElfStrtab', strtab: 'ElfStrtab', strtabndx: int) -> None:
        for sym in self.syms:
            sym.st_name = strtab.add_string(sym.name)
        self.linked_sec = strtab
        super().link_strtab(shstrtab, strtab, strtabndx)

    def add_symbol(self, sym: ElfSymbol) -> int:
        self.syms.append(sym)
        sym._set_owner(self)
        return len(self.syms) # - 1 not needed as we don't keep track of the initial NULL section

    def get_symbol(self, idx: int) -> ElfSymbol:
        return self.syms[idx - 1] # Accounting for initial NULL section
    
    def remove_symbol(self, sym: ElfSymbol) -> None:
        self.syms.remove(sym)

    def get_index_of_symbol(self, sym: ElfSymbol) -> int:
        return self.syms.index(sym) + 1

    def get_symbols(self, name: str) -> list[ElfSymbol]:
        return [i for i in self.syms if i.name == name]


class ElfRela:
    _struct = struct.Struct('>IIi')

    def __init__(self, sym: ElfSymbol, r_offset=0, r_info_type: PPC_RELOC_TYPE=PPC_RELOC_TYPE.R_PPC_NONE, r_addend=0) -> None:
        self.sym: ElfSymbol = sym
        self.r_offset = r_offset
        self._r_info_sym: int = -1 # Set later
        self.r_info_type: PPC_RELOC_TYPE = r_info_type
        self.r_addend: int = r_addend

    @staticmethod
    def read(data: bytes, offset: int, symtab: ElfSymtab) -> 'ElfRela':
        r_offset, r_info, r_addend = ElfRela._struct.unpack(data[offset:offset+ElfRela._struct.size])
        r_info_sym = r_info >> 8
        sym = symtab.get_symbol(r_info_sym)
        r_info_type = PPC_RELOC_TYPE(r_info & 0xFF)
        return ElfRela(sym, r_offset, r_info_type, r_addend)

    def __bytes__(self) -> bytes:
        return bytes(ElfRela._struct.pack(
            self.r_offset,
            (self.sym.get_idx() << 8) | self.r_info_type.value,
            self.r_addend
        ))
        
    def __repr__(self) -> str:
        return f'elffile.ElfRela({self.sym}, offset={self.r_offset})'


class ElfRelaSec(ElfSection):
    def __init__(self, name='.rela', relocs: list[ElfRela]=None, symtab: ElfSymtab=None) -> None:
        super().__init__(name)
        self.relocs = relocs if relocs else []
        self.linked_symtab = symtab

    @staticmethod
    def read(data: bytes, header: ElfSectionHeader, symtab: ElfSymtab) -> None:
        assert len(data) == header.sh_size
        relocs = []
        for i in range(0, len(data), ElfRela._struct.size):
            relocs.append(ElfRela.read(data, i, symtab))
        sec = ElfRelaSec('', relocs, symtab)
        sec.header = header
        return sec

    def add_reloc(self, reloc: ElfRela) -> None:
        self.relocs.append(reloc)

    def _prepare_for_write(self) -> None:
        assert self.linked_symtab is not None
        self.data = b''.join([bytes(reloc) for reloc in self.relocs])
        self.header.sh_entsize = ElfRela._struct.size
        self.header.sh_link = self.linked_symtab.shndx
        super()._prepare_for_write()


class ElfFile:
    def __init__(self, e_type: ET=ET.ET_NONE, e_machine=EM.EM_NONE) -> None:
        self.header = ElfHeader(e_type, e_machine)
        self.sections: list[ElfSection] = [ElfSection()] # NULL section

    def _read_section(self, data: bytes, sec_index: int) -> tuple[ElfSectionHeader, bytes]:
        sec_hdr_offs = self.header.e_shoff
        sec_hdr_size = self.header.e_shentsize
        hdr = ElfSectionHeader.read(data, sec_hdr_offs + sec_index * sec_hdr_size)
        res_data = None
        if hdr.has_data():
            offs = hdr.sh_offset
            size = hdr.sh_size
            res_data = data[offs:offs+size]
        return (hdr, res_data)

    def read(data: bytes) -> 'ElfFile':
        elf_file = ElfFile()
        elf_file.header = ElfHeader.read(data, 0)

        # Preload .shstrtab
        shstrtab_idx = elf_file.header.e_shstrndx
        strtab_hdr, strtab_data = elf_file._read_section(data, shstrtab_idx)
        shstrtab = ElfStrtab.read(strtab_data, strtab_hdr)

        # Preload .strtab
        strtab_idx = next(i for i in range(elf_file.header.e_shnum) if shstrtab.get_at_index(i) == '.strtab')
        strtab_hdr, strtab_data = elf_file._read_section(data, strtab_idx)
        strtab = ElfStrtab.read(strtab_data, strtab_hdr)

        elf_file.sections = [None] * (elf_file.header.e_shnum)
        # NULL section
        elf_file.sections[0] = ElfSection()

        sections_to_process = list(range(1, elf_file.header.e_shnum))
        while len(sections_to_process) > 0:
            for i in sections_to_process:
                sec_name = shstrtab.get_at_index(i)
                sec_hdr, sec_data = elf_file._read_section(data, i)

                linked_sec = elf_file.sections[sec_hdr.sh_link]
                if linked_sec is None:
                    # Defer to next loop
                    continue

                match sec_hdr.sh_type:
                    case SHT.SHT_PROGBITS | SHT.SHT_MW_CATS:
                        elf_sec = ElfSection.read(sec_data, sec_hdr)
                    case SHT.SHT_RELA:
                        elf_sec = ElfRelaSec.read(sec_data, sec_hdr, linked_sec)
                    case SHT.SHT_STRTAB:
                        elf_sec = ElfStrtab.read(sec_data, sec_hdr)
                    case SHT.SHT_SYMTAB:
                        elf_sec = ElfSymtab.read(sec_data, sec_hdr, linked_sec)
                    case _:
                        elf_sec = ElfSection(sec_name, bytes(), sec_hdr)

                elf_sec.name = shstrtab.get_at_index(i)
                elf_file.sections[i] = elf_sec
                sections_to_process.remove(i)
        return elf_file

    def add_section(self, sect: ElfSection) -> int:
        self.sections.append(sect)
        return len(self.sections) - 1

    def has_section(self, name: str) -> bool:
        return bool([x for x in self.sections if x.name == name])

    def get_section(self, name: str) -> ElfSection:
        if not self.has_section(name):
            return None
        return next((x for x in self.sections if x.name == name), None)

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
        strtabndx = self.get_section_index('.strtab')
        shstrtab: ElfStrtab = self.get_section('.shstrtab')
        strtab: ElfStrtab = self.get_section('.strtab')

        shstrtab.clear()
        strtab.clear()

        self.header.e_phoff = 0
        self.header.e_phnum = 0
        self.header.e_shstrndx = shstrtabndx

        for i, sec in enumerate(self.sections):
            sec.link_strtab(shstrtab, strtab, strtabndx)
            sec._freeze_shndx(i)
            if sec.header.sh_type == SHT.SHT_SYMTAB:
                sec._freeze_indices()

        data = bytearray(b'\0'*ElfHeader._struct.size) # Filled out at the end
        for sec in self.sections:
            sec_data = bytes(sec)
            sec.header.sh_offset = len(data) if sec.header.sh_type != SHT.SHT_NULL else 0
            data.extend(sec_data)

        self.header.e_shnum = len(self.sections)
        self.header.e_shoff = len(data)

        for sec in self.sections:
            sec_hdr_data = bytes(sec.header)
            data.extend(sec_hdr_data)

        header = bytes(self.header)
        data[0:len(header)] = header

        for sec in self.sections:
            sec._unfreeze_shndx()
            if sec.header.sh_type == SHT.SHT_SYMTAB:
                sec._unfreeze_indices()

        return bytes(data)
