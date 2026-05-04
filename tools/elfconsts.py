#!/usr/bin/env python3
# ELF constants

import re
from enum import IntEnum, IntFlag

class ET(IntEnum):
    ET_NONE = 0
    ET_REL = 1
    ET_EXEC = 2
    ET_DYN = 3
    ET_CORE = 4


class EM(IntEnum):
    EM_NONE = 0
    EM_M32 = 1
    EM_SPARC = 2
    EM_386 = 3
    EM_68K = 4
    EM_88K = 5
    EM_860 = 7
    EM_MIPS = 8
    EM_S370 = 9
    EM_MIPS_RS3_LE = 10
    EM_PARISC = 15
    EM_VPP500 = 17
    EM_SPARC32PLUS = 18
    EM_960 = 19
    EM_PPC = 20
    EM_PPC64 = 21
    EM_S390 = 22
    EM_V800 = 36
    EM_FR20 = 37
    EM_RH32 = 38
    EM_RCE = 39
    EM_ARM = 40
    EM_ALPHA = 41
    EM_SH = 42
    EM_SPARCV9 = 43
    EM_TRICORE = 44
    EM_ARC = 45
    EM_H8_300 = 46
    EM_H8_300H = 47
    EM_H8S = 48
    EM_H8_500 = 49
    EM_IA_64 = 50
    EM_MIPS_X = 51
    EM_COLDFIRE = 52
    EM_68HC12 = 53
    EM_MMA = 54
    EM_PCP = 55
    EM_NCPU = 56
    EM_NDR1 = 57
    EM_STARCORE = 58
    EM_ME16 = 59
    EM_ST100 = 60
    EM_TINYJ = 61
    EM_X86_64 = 62
    EM_PDSP = 63
    EM_PDP10 = 64
    EM_PDP11 = 65
    EM_FX66 = 66
    EM_ST9PLUS = 67
    EM_ST7 = 68
    EM_68HC16 = 69
    EM_68HC11 = 70
    EM_68HC08 = 71
    EM_68HC05 = 72
    EM_SVX = 73
    EM_ST19 = 74
    EM_VAX = 75
    EM_CRIS = 76
    EM_JAVELIN = 77
    EM_FIREPATH = 78
    EM_ZSP = 79
    EM_MMIX = 80
    EM_HUANY = 81
    EM_PRISM = 82
    EM_AVR = 83
    EM_FR30 = 84
    EM_D10V = 85
    EM_D30V = 86
    EM_V850 = 87
    EM_M32R = 88
    EM_MN10300 = 89
    EM_MN10200 = 90
    EM_PJ = 91
    EM_OPENRISC = 92
    EM_ARC_A5 = 93
    EM_XTENSA = 94
    EM_VIDEOCORE = 95
    EM_TMM_GPP = 96
    EM_NS32K = 97
    EM_TPC = 98
    EM_SNP1K = 99
    EM_ST200 = 100


class SHN(IntEnum):
    SHN_UNDEF = 0
    SHN_LORESERVE = 0xff00
    SHN_LOPROC = 0xff00
    SHN_HIPROC = 0xff1f
    SHN_LOOS = 0xff20
    SHN_HIOS = 0xff3f
    SHN_ABS = 0xfff1
    SHN_COMMON = 0xfff2
    SHN_XINDEX = 0xffff
    SHN_HIRESERVE = 0xffff


class SHT(IntEnum):
    SHT_NULL = 0
    SHT_PROGBITS = 1
    SHT_SYMTAB = 2
    SHT_STRTAB = 3
    SHT_RELA = 4
    SHT_HASH = 5
    SHT_DYNAMIC = 6
    SHT_NOTE = 7
    SHT_NOBITS = 8
    SHT_REL = 9
    SHT_SHLIB = 10
    SHT_DYNSYM = 11
    SHT_INIT_ARRAY = 14
    SHT_FINI_ARRAY = 15
    SHT_PREINIT_ARRAY = 16
    SHT_GROUP = 17
    SHT_SYMTAB_SHNDX = 18
    SHT_LOOS = 0x60000000
    SHT_HIOS = 0x6fffffff
    SHT_LOPROC = 0x70000000
    SHT_HIPROC = 0x7fffffff
    SHT_LOUSER = 0x80000000
    SHT_MW_CATS = 0xca2a82c2
    SHT_HIUSER = 0xffffffff


class SHF(IntFlag):
    SHF_NONE = 0x0
    SHF_WRITE = 0x1
    SHF_ALLOC = 0x2
    SHF_EXECINSTR = 0x4
    SHF_MERGE = 0x10
    SHF_STRINGS = 0x20
    SHF_INFO_LINK = 0x40
    SHF_LINK_ORDER = 0x80
    SHF_OS_NONCONFORMING = 0x100
    SHF_GROUP = 0x200
    SHF_TLS = 0x400


class STB(IntEnum):
    STB_LOCAL = 0
    STB_GLOBAL = 1
    STB_WEAK = 2
    STB_LOOS = 10
    STB_HIOS = 12
    STB_LOPROC = 13
    STB_HIPROC = 15


class STT(IntEnum):
    STT_NOTYPE = 0
    STT_OBJECT = 1
    STT_FUNC = 2
    STT_SECTION = 3
    STT_FILE = 4
    STT_COMMON = 5
    STT_TLS = 6
    STT_LOOS = 10
    STT_HIOS = 12
    STT_LOPROC = 13
    STT_HIPROC = 15


class STV(IntEnum):
    STV_DEFAULT = 0
    STV_INTERNAL = 1
    STV_HIDDEN = 2
    STV_PROTECTED = 3


class STN(IntEnum):
    STN_UNDEF = 0

# https://dwarfstd.org/doc/dwarf-2.0.0.pdf, figures 14 and 15
class DWARF_TAG(IntEnum):
    DW_TAG_none = 0x00
    DW_TAG_array_type = 0x01
    DW_TAG_class_type = 0x02
    DW_TAG_entry_point = 0x03
    DW_TAG_enumeration_type = 0x04
    DW_TAG_formal_parameter = 0x05
    DW_TAG_imported_declaration = 0x08
    DW_TAG_label = 0x0a
    DW_TAG_lexical_block = 0x0b
    DW_TAG_member = 0x0d
    DW_TAG_pointer_type = 0x0f
    DW_TAG_reference_type = 0x10
    DW_TAG_compile_unit = 0x11
    DW_TAG_string_type = 0x12
    DW_TAG_structure_type = 0x13
    DW_TAG_subroutine_type = 0x15
    DW_TAG_typedef = 0x16
    DW_TAG_union_type = 0x17
    DW_TAG_unspecified_parameters = 0x18
    DW_TAG_variant = 0x19
    DW_TAG_common_block = 0x1a
    DW_TAG_common_inclusion = 0x1b
    DW_TAG_inheritance = 0x1c
    DW_TAG_inlined_subroutine = 0x1d
    DW_TAG_module = 0x1e
    DW_TAG_ptr_to_member_type = 0x1f
    DW_TAG_set_type = 0x20
    DW_TAG_subrange_type = 0x21
    DW_TAG_with_stmt = 0x22
    DW_TAG_access_declaration = 0x23
    DW_TAG_base_type = 0x24
    DW_TAG_catch_block = 0x25
    DW_TAG_const_type = 0x26
    DW_TAG_constant = 0x27
    DW_TAG_enumerator = 0x28
    DW_TAG_file_type = 0x29
    DW_TAG_friend = 0x2a
    DW_TAG_namelist = 0x2b
    DW_TAG_namelist_item = 0x2c
    DW_TAG_packed_type = 0x2d
    DW_TAG_subprogram = 0x2e
    DW_TAG_template_type_param = 0x2f
    DW_TAG_template_value_param = 0x30
    DW_TAG_thrown_type = 0x31
    DW_TAG_try_block = 0x32
    DW_TAG_variant_part = 0x33
    DW_TAG_variable = 0x34
    DW_TAG_volatile_type = 0x35
    DW_TAG_lo_user = 0x4080
    DW_TAG_hi_user = 0xffff

# https://dwarfstd.org/doc/dwarf-2.0.0.pdf, figures 17 and 18
class DWARF_AT(IntEnum):
    DW_AT_none = 0x00
    DW_AT_sibling = 0x01
    DW_AT_location = 0x02
    DW_AT_name = 0x03
    DW_AT_ordering = 0x09
    DW_AT_byte_size = 0x0b
    DW_AT_bit_offset = 0x0c
    DW_AT_bit_size = 0x0d
    DW_AT_stmt_list = 0x10
    DW_AT_low_pc = 0x11
    DW_AT_high_pc = 0x12
    DW_AT_language = 0x13
    DW_AT_discr = 0x15
    DW_AT_discr_value = 0x16
    DW_AT_visibility = 0x17
    DW_AT_import = 0x18
    DW_AT_string_length = 0x19
    DW_AT_common_reference = 0x1a
    DW_AT_comp_dir = 0x1b
    DW_AT_const_value = 0x1c
    DW_AT_containing_type = 0x1d
    DW_AT_default_value = 0x1e
    DW_AT_inline = 0x20
    DW_AT_is_optional = 0x21
    DW_AT_lower_bound = 0x22
    DW_AT_producer = 0x25
    DW_AT_prototyped = 0x27
    DW_AT_return_addr = 0x2a
    DW_AT_start_scope = 0x2c
    DW_AT_stride_size = 0x2e
    DW_AT_upper_bound = 0x2f
    DW_AT_abstract_origin = 0x31
    DW_AT_accessibility = 0x32
    DW_AT_address_class = 0x33
    DW_AT_artificial = 0x34
    DW_AT_base_types = 0x35
    DW_AT_calling_convention = 0x36
    DW_AT_count = 0x37
    DW_AT_data_member_location = 0x38
    DW_AT_decl_column = 0x39
    DW_AT_decl_file = 0x3a
    DW_AT_decl_line = 0x3b
    DW_AT_declaration = 0x3c
    DW_AT_discr_list = 0x3d
    DW_AT_encoding = 0x3e
    DW_AT_external = 0x3f
    DW_AT_frame_base = 0x40
    DW_AT_friend = 0x41
    DW_AT_identifier_case = 0x42
    DW_AT_macro_info = 0x43
    DW_AT_namelist_items = 0x44
    DW_AT_priority = 0x45
    DW_AT_segment = 0x46
    DW_AT_specification = 0x47
    DW_AT_static_link = 0x48
    DW_AT_type = 0x49
    DW_AT_use_location = 0x4a
    DW_AT_variable_parameter = 0x4b
    DW_AT_virtuality = 0x4c
    DW_AT_vtable_elem_location = 0x4d
    DW_AT_lo_user = 0x2000
    DW_AT_hi_user = 0x3fff

# https://dwarfstd.org/doc/dwarf-2.0.0.pdf, figure 19
class DWARF_FORM(IntEnum):
    DW_FORM_none = 0x00
    DW_FORM_addr = 0x01
    DW_FORM_block2 = 0x03
    DW_FORM_block4 = 0x04
    DW_FORM_data2 = 0x05
    DW_FORM_data4 = 0x06
    DW_FORM_data8 = 0x07
    DW_FORM_string = 0x08
    DW_FORM_block = 0x09
    DW_FORM_block1 = 0x0a
    DW_FORM_data1 = 0x0b
    DW_FORM_flag = 0x0c
    DW_FORM_sdata = 0x0d
    DW_FORM_strp = 0x0e
    DW_FORM_udata = 0x0f
    DW_FORM_ref_addr = 0x10
    DW_FORM_ref1 = 0x11
    DW_FORM_ref2 = 0x12
    DW_FORM_ref4 = 0x13
    DW_FORM_ref8 = 0x14
    DW_FORM_ref_udata = 0x15
    DW_FORM_indirect = 0x16

REL_SYM = re.compile(r'R_([0-9a-fA-F]+)_([0-9a-fA-F]+)_([0-9a-fA-F]+)')

class PPC_RELOC_TYPE(IntEnum):
    R_PPC_NONE = 0
    R_PPC_ADDR32 = 1
    R_PPC_ADDR24 = 2
    R_PPC_ADDR16 = 3
    R_PPC_ADDR16_LO = 4
    R_PPC_ADDR16_HI = 5
    R_PPC_ADDR16_HA = 6
    R_PPC_ADDR14 = 7
    R_PPC_ADDR14_BRTAKEN = 8
    R_PPC_ADDR14_BRNTAKEN = 9
    R_PPC_REL24 = 10
    R_PPC_REL14 = 11
    R_PPC_REL14_BRTAKEN = 12
    R_PPC_REL14_BRNTAKEN = 13
    R_PPC_UADDR32 = 24
    R_PPC_EMB_SDA21 = 109
    R_RVL_NONE = 201
    R_RVL_SECT = 202
    R_RVL_STOP = 203


class SpecialSections:

    _lookup: dict[str, tuple[SHT, SHF]] = {
        'extab_': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC),
        'extabindex_': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC),
        '.bss': (SHT.SHT_NOBITS, SHF.SHF_ALLOC | SHF.SHF_WRITE),
        '.comment': (SHT.SHT_PROGBITS, SHF.SHF_NONE),
        '.ctors': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC),
        '.data': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC | SHF.SHF_WRITE),
        '.dtors': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC),
        '.init': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC | SHF.SHF_EXECINSTR),
        '.rodata': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC),
        '.sbss': (SHT.SHT_NOBITS, SHF.SHF_ALLOC | SHF.SHF_WRITE),
        '.sbss2': (SHT.SHT_NOBITS, SHF.SHF_ALLOC | SHF.SHF_WRITE),
        '.sdata': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC | SHF.SHF_WRITE),
        '.sdata2': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC | SHF.SHF_WRITE),
        '.shstrtab': (SHT.SHT_STRTAB, SHF.SHF_NONE),
        '.strtab': (SHT.SHT_STRTAB, SHF.SHF_NONE),
        '.symtab': (SHT.SHT_SYMTAB, SHF.SHF_NONE),
        '.text': (SHT.SHT_PROGBITS, SHF.SHF_ALLOC | SHF.SHF_EXECINSTR)
    }

    @staticmethod
    def get(sec_name: str) -> tuple[SHT, SHF]:
        if sec_name in SpecialSections._lookup:
            return SpecialSections._lookup[sec_name]
        if sec_name.startswith('.rel.'):
            return (SHT.SHT_REL, SHF.SHF_NONE)
        if sec_name.startswith('.rela.'):
            return (SHT.SHT_RELA, SHF.SHF_NONE)
        return (SHT.SHT_NULL, SHF.SHF_NONE)
