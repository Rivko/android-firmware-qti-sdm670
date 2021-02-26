# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.elf.header.format import Elf32_Ehdr, Elf64_Ehdr
from sectools.common.parsegen.elf.header.ident import ELFCLASS32, ELFCLASS64, ELFCLASS_DESCRIPTION
from sectools.common.parsegen.elf.section import defines
from sectools.common.parsegen.elf.section.flags import PERM_DESCRIPTION, PERM_VALUE
from sectools.common.parsegen.elf.section.type import ST_STRING, ST_DESCRIPTION, SHT_STRTAB
from sectools.common.utils.c_misc import hex_addr, properties_repr, TablePrinter
from sectools.common.utils.struct_base import StructBase


class Elf32_Shdr(StructBase):
    """ELF 32bit Section Header Class"""

    def _unpack_data_list(self, unpacked):
        self.sh_name = unpacked[0]
        self.sh_type = unpacked[1]
        self.sh_flags = unpacked[2]
        self.sh_addr = unpacked[3]
        self.sh_offset = unpacked[4]
        self.sh_size = unpacked[5]
        self.sh_link = unpacked[6]
        self.sh_info = unpacked[7]
        self.sh_addralign = unpacked[8]
        self.sh_entsize = unpacked[9]
        self.sh_name_str = ""

    def _pack_data_list(self):
        return [self.sh_name,
                self.sh_type,
                self.sh_flags,
                self.sh_addr,
                self.sh_offset,
                self.sh_size,
                self.sh_link,
                self.sh_info,
                self.sh_addralign,
                self.sh_entsize]

    @property
    def flags(self):
        return PERM_VALUE(self.sh_flags)

    @property
    def filesz(self):
        return self.sh_size

    @property
    def offset(self):
        return self.sh_offset

    @offset.setter
    def offset(self, offset):
        self.sh_offset = offset

    @classmethod
    def get_format(cls):
        return 'I'*10

    def _repr_properties(self):
        properties = [
            (defines.SN_STRING, self.sh_name),  # TODO: extract name of section from the string table in section specified by e_shstrndx
            (ST_STRING, ST_DESCRIPTION.get(self.sh_type, self.sh_type)),
            PERM_DESCRIPTION.get(self.flags, self.flags)
            (defines.SA_STRING, hex_addr(self.sh_addr)),
            (defines.SO_STRING, hex_addr(self.sh_offset)),
            (defines.SS_STRING, hex_addr(self.sh_size)),
            (defines.SL_STRING, self.sh_link),
            (defines.SI_STRING, self.sh_info),
            (defines.SAL_STRING, self.sh_addralign),
            (defines.SE_STRING, hex_addr(self.sh_entsize))
        ]
        return [(attr, val) for attr, val in properties]

    def __repr__(self):
        return properties_repr(self._repr_properties())


class Elf64_Shdr(Elf32_Shdr):
    """ELF 64bit Section Header Class"""

    @classmethod
    def get_format(cls):
        return 'IIQQQQIIQQ'


ELF_SHDRS = {
    ELFCLASS32: Elf32_Shdr,
    ELFCLASS64: Elf64_Shdr
}


def get_shdr_class(elf_header):
    # Get the appropriate elf section header class
    header_class = ELF_SHDRS.get(elf_header.e_ident_class, None)
    if header_class is None:
        supported_classes = [str(key) + '-' + str(ELFCLASS_DESCRIPTION[key]) for key in ELF_SHDRS.keys()]
        raise RuntimeError('File contains unsupported section header class: ' + str(elf_header.e_ident_class) + '\n'
                           '    ' + 'Supported classes are: ' + str(supported_classes))
    return header_class


def set_shdr_names(shdrs, e_shstrndx, data):
    if shdrs and e_shstrndx != defines.SHN_UNDEF and e_shstrndx < len(shdrs):
        str_tab_shdr = shdrs[e_shstrndx]
        if str_tab_shdr.sh_type == SHT_STRTAB:
            offset = str_tab_shdr.sh_offset
            str_tab_data = data[offset: offset + str_tab_shdr.sh_size]
            if str_tab_data:
                for shdr in shdrs:
                    if shdr.sh_name < len(str_tab_data):
                        shdr.sh_name_str = str(str_tab_data[shdr.sh_name:]).split("\x00")[0]


def extract_shdrs(data, elf_header):
    assert isinstance(elf_header, (Elf32_Ehdr, Elf64_Ehdr))
    shdrs = []

    header_class = get_shdr_class(elf_header)
    for idx in range(elf_header.e_shnum):
        offset = elf_header.e_shoff + idx*elf_header.e_shentsize
        section_header_data = data[offset: offset + elf_header.e_shentsize]

        # Ensure data is of elf section header length
        if len(section_header_data) < header_class.get_size():
            raise RuntimeError('Data length "' + str(len(section_header_data)) + '" is less than ' +
                               str(ELFCLASS_DESCRIPTION[elf_header.e_ident_class]) + ' Elf Section Header length "' + str(header_class.get_size()) + '"')

        # Create the section header class
        shdrs.append(header_class(section_header_data))

    set_shdr_names(shdrs, elf_header.e_shstrndx, data)

    return shdrs


def repr_shdrs(shdrs, sno_range=None):
    retval = TablePrinter()

    COLUMN_SNO     = 0
    COLUMN_NAME    = 1
    COLUMN_TYPE    = 2
    COLUMN_ADDR    = 3
    COLUMN_OFF     = 4
    COLUMN_SIZE    = 5
    COLUMN_ENTSIZE = 6
    COLUMN_FLAGS   = 7
    COLUMN_LINK    = 8
    COLUMN_INFO    = 9
    COLUMN_ALIGN   = 10

    retval.insert_data(0, COLUMN_SNO, 'Num')
    retval.insert_data(0, COLUMN_NAME, 'Name')
    retval.insert_data(0, COLUMN_TYPE, 'Type')
    retval.insert_data(0, COLUMN_ADDR, 'Addr')
    retval.insert_data(0, COLUMN_OFF, 'Offset')
    retval.insert_data(0, COLUMN_SIZE, 'Size')
    retval.insert_data(0, COLUMN_ENTSIZE, 'EntSize')
    retval.insert_data(0, COLUMN_FLAGS, 'Flags')
    retval.insert_data(0, COLUMN_LINK, 'Link')
    retval.insert_data(0, COLUMN_INFO, 'Info')
    retval.insert_data(0, COLUMN_ALIGN, 'Align')

    stats = [0] * (COLUMN_ALIGN + 1)
    for idx, shdr in enumerate(shdrs):
        stats[COLUMN_ADDR] = shdr.sh_addr if shdr.sh_addr > stats[COLUMN_ADDR] else stats[COLUMN_ADDR]
        stats[COLUMN_OFF] = shdr.sh_offset if shdr.sh_offset > stats[COLUMN_OFF] else stats[COLUMN_OFF]
        stats[COLUMN_SIZE] = shdr.sh_size if shdr.sh_size > stats[COLUMN_SIZE] else stats[COLUMN_SIZE]
        stats[COLUMN_ENTSIZE] = shdr.sh_entsize if shdr.sh_entsize > stats[COLUMN_ENTSIZE] else stats[COLUMN_ENTSIZE]

    for idx in range(COLUMN_ALIGN + 1):
        stats[idx] = len(hex(stats[idx]).rstrip('L'))-2

    if sno_range is None:
        sno_range = range(1, len(shdrs)+1)

    for idx, shdr in enumerate(shdrs):
        retval.insert_data(idx+1, COLUMN_SNO, str(sno_range[idx]))
        retval.insert_data(idx+1, COLUMN_NAME, shdr.sh_name_str, justify=retval.LEFT)
        retval.insert_data(idx+1, COLUMN_TYPE, str(ST_DESCRIPTION.get(shdr.sh_type, shdr.sh_type)), justify=retval.LEFT)
        retval.insert_data(idx+1, COLUMN_ADDR, hex_addr(shdr.sh_addr, stats[COLUMN_ADDR]))
        retval.insert_data(idx+1, COLUMN_OFF, hex_addr(shdr.sh_offset, stats[COLUMN_OFF]))
        retval.insert_data(idx+1, COLUMN_SIZE, hex_addr(shdr.sh_size, stats[COLUMN_SIZE]))
        retval.insert_data(idx+1, COLUMN_ENTSIZE, hex_addr(shdr.sh_entsize, stats[COLUMN_ENTSIZE]))
        retval.insert_data(idx+1, COLUMN_FLAGS, PERM_DESCRIPTION.get(shdr.flags, ""))
        retval.insert_data(idx+1, COLUMN_LINK, shdr.sh_link)
        retval.insert_data(idx+1, COLUMN_INFO, shdr.sh_info)
        retval.insert_data(idx+1, COLUMN_ALIGN, shdr.sh_addralign)
    return retval.get_data()


def pack_shdrs(shdrs):
    retval = ''
    for shdr in shdrs:
        retval += shdr.pack()
    return retval


def _extract_data(data, offset, size):
    subdata = ''
    if offset < len(data) and size > 0:
        subdata = data[offset: offset + size]
    return subdata


def extract_section_data(data, shdr):
    offset = shdr.sh_offset
    size = shdr.sh_size
    return _extract_data(data, offset, size)


def extract_sections(data, shdrs):
    retval = {}
    for shdr in shdrs:
        retval[shdr] = extract_section_data(data, shdr)
    return retval


def zero_out_sections(elf_header):
    assert isinstance(elf_header, (Elf32_Ehdr, Elf64_Ehdr))
    elf_header.e_shoff = 0
    elf_header.e_shnum = 0
    elf_header.e_shstrndx = 0
