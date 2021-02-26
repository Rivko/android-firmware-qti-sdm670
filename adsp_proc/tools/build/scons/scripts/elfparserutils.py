'''
   ELF Parser Utils
'''
#===============================================================================
#
# ELF Parser Utils
#
# GENERAL DESCRIPTION
#    ELF parser utilities used across the build system
#
# Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/elfparserutils.py#5 $
#  $DateTime: 2015/01/29 20:21:26 $
#  $Author: pwbldsvc $
#  $Change: 7374326 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 11/11/13   aa      Create
#
#===============================================================================
import struct
#import sys
#import os
#import shutil

#----------------------------------------------------------------------------
# ELF Header Class
#----------------------------------------------------------------------------
class Elf32_Ehdr(object):
    '''
       class Elf32_Ehdr

       ELF Header Class
    '''
    # Structure object to align and package the ELF Header
    s = struct.Struct('16sHHIIIIIHHHHHH')

    # ELF Definitions
    ELF_HDR_SIZE = 52
    ELFINFO_MAG0_INDEX = 0
    ELFINFO_MAG1_INDEX = 1
    ELFINFO_MAG2_INDEX = 2
    ELFINFO_MAG3_INDEX = 3
    ELFINFO_MAG0 = '\x7f'
    ELFINFO_MAG1 = 'E'
    ELFINFO_MAG2 = 'L'
    ELFINFO_MAG3 = 'F'
    ELFINFO_CLASS_INDEX = 4
    ELFINFO_DATA_INDEX = 5
    ELFINFO_VERSION_INDEX = 6
    ELFINFO_VERSION_CURRENT = '\x01'

    #ELF Type Codes
    ET_NONE = 0
    ET_REL = 1
    ET_EXEC = 2
    ET_DYN = 3
    ET_CORE = 4
    ET_LOPROC = 0xff00
    ET_HIPROC = 0xffff

    #EI_CLASS codes
    ELFINFO_CLASS_NONE = '\x00'
    ELFINFO_CLASS_32 = '\x01'
    ELFINFO_CLASS_64 = '\x02'

    #EI_DATA codes
    ELFDATANONE = '\x00'     #Invalid data encoding
    ELFDATA2LSB = '\x01'     #2's complement LSB endianess
    ELFDATA2MSB = '\x02'     #2's complement MSB endianess

    #Machine Codes
    EM_NONE = 0
    EM_M32 = 1
    EM_SPARC = 2
    EM_386 = 3
    EM_68K = 4
    EM_88K = 5
    EM_860 = 7
    EM_MIPS = 8
    EM_ARM = 40

    def __init__(self, data):
        '''
           __init__
        '''
        unpacked_data = (Elf32_Ehdr.s).unpack(data)
        self.unpacked_data = unpacked_data
        self.e_ident = unpacked_data[0]
        self.e_type = unpacked_data[1]
        self.e_machine = unpacked_data[2]
        self.e_version = unpacked_data[3]
        self.e_entry = unpacked_data[4]
        self.e_phoff = unpacked_data[5]
        self.e_shoff = unpacked_data[6]
        self.e_flags = unpacked_data[7]
        self.e_ehsize = unpacked_data[8]
        self.e_phentsize = unpacked_data[9]
        self.e_phnum = unpacked_data[10]
        self.e_shentsize = unpacked_data[11]
        self.e_shnum = unpacked_data[12]
        self.e_shstrndx = unpacked_data[13]

    def get_ident_class_str(self):
        '''
           get_ident_class_str
        '''
        code = self.e_ident[self.ELFINFO_CLASS_INDEX]
        if self.ELFINFO_CLASS_32 == code:
            return "ELF32"
        elif self.ELFINFO_CLASS_64 == code:
            return "ELF64"
        else:
            return "ERROR"

    def get_ident_data_str(self):
        '''
           get_ident_data_str
        '''
        code = self.e_ident[self.ELFINFO_DATA_INDEX]
        if self.ELFDATA2LSB == code:
            return "2's complement, little endian"
        elif self.ELFDATA2MSB == code:
            return "2's complement, big endian"
        else:
            return "ERROR " + str(ord(code))

    def get_type_str(self):
        '''
           get_type_str
        '''
        code = self.e_type
        if self.ET_NONE == code:
            return "NONE"
        elif self.ET_REL == code:
            return "REL"
        elif self.ET_EXEC == code:
            return "EXEC"
        elif self.ET_DYN == code:
            return "DYN"
        elif self.ET_CORE == code:
            return "CORE"
        elif self.ET_LOPROC == code:
            return "LOPROC"
        elif self.ET_HIPROC == code:
            return "HIPROC"
        else:
            return "ERROR"

    def get_machine_str(self):
        '''
           get_machine_str
        '''
        code = self.e_machine
        if self.EM_NONE == code:
            return "NONE"
        elif self.EM_ARM == code:
            return "ARM"
        elif self.EM_M32 == code:
            return "AT&T WE 32100"
        elif self.EM_SPARC == code:
            return "SPARC"
        elif self.EM_386 == code:
            return "Intel 80386"
        elif self.EM_68K == code:
            return "Motorola 68000"
        elif self.EM_88K == code:
            return "Motorola 88000"
        elif self.EM_860 == code:
            return "Intel 80860"
        elif self.EM_MIPS == code:
            return "MIPS RS300"
        else:
            return "<unknown>"

    def print_values(self):
        '''
           print_values
        '''
        print "ATTRIBUTE / VALUE"
        for attr, value in self.__dict__.iteritems():
            print attr, value

    def readelf_print_values(self):
        '''
           readelf_print_values
        '''
        print '   {0:<34}'.format("Magic:"),
        for char in str(self.e_ident):
            print '{0:02x}'.format(ord(char)),
        print ''
        print '   {0:<34} {1}'.format("Class:", self.get_ident_class_str())
        print '   {0:<34} {1}'.format("Data:", self.get_ident_data_str())
        print '   {0:<34} {1}'.format \
            ("Version:", ord(self.e_ident[Elf32_Ehdr.ELFINFO_VERSION_INDEX]))
        #print '   {0:<34} {1}'.format("OS/ABI:", "TBD")
        #print '   {0:<34} {1}'.format("ABI Version:", "TBD")
        print '   {0:<34} {1} ({2:#0x})'.format \
            ("Type:", self.get_type_str(), self.e_type)
        print '   {0:<34} {1} ({2:#0x})'.format \
            ("Machine:", self.get_machine_str(), self.e_machine)
        print '   {0:<34} {1:#0x}'.format("Version:", self.e_version)
        print '   {0:<34} {1:#0x}'.format("Entry point address:", self.e_entry)
        print '   {0:<34} {1:#d} {2}'.format \
            ("Start of program headers:", self.e_phoff, "(bytes into file)")
        print '   {0:<34} {1:#d} {2}'.format \
            ("Start of section headers:", self.e_shoff, "(bytes into file)")
        print '   {0:<34} {1:#0x}'.format("Flags:", self.e_flags)
        print '   {0:<34} {1:#d} {2}'.format \
            ("Size of this header:", self.e_ehsize, "(bytes)")
        print '   {0:<34} {1:#d} {2}'.format \
            ("Size of program headers:", self.e_phentsize, "(bytes)")
        print '   {0:<34} {1:#0d}'.format \
            ("Number of program headers:", self.e_phnum)
        print '   {0:<34} {1:#d} {2}'.format \
            ("Size of section headers:", self.e_shentsize, "(bytes)")
        print '   {0:<34} {1}'.format \
            ("Number of section headers:", self.e_shnum)
        print '   {0:<34} {1}'.format \
            ("Section header string table index:", self.e_shstrndx)

    def readelf_elf_header_preamble(self):
        '''
           readelf_elf_header_preamble
        '''
        print "\n"
        print "ELF Header:"

    def readelf_program_header_preamble(self):
        '''
           readelf_program_header_preamble
        '''
        print "\n"
        print "Program Headers:"
        print '{0:8}'.format("Type"),
        print '{0:10}'.format("Offset"),
        print '{0:10}'.format("VirtAddr"),
        print '{0:10}'.format("PhysAddr"),
        print '{0:10}'.format("FileSiz"),
        print '{0:10}'.format("MemSiz"),
        print '{0:4}'.format("Flg"),
        print '{0:4}'.format("Align")

    def readelf_section_header_preamble(self):
        '''
           readelf_section_header_preamble
        '''
        print "\n"
        print "Section Headers:"
        print '{0:6}'.format("[Nr]"),
        print '{0:26}'.format("Name"),
        print '{0:8}'.format("Type"),
        print '{0:10}'.format("Addr"),
        print '{0:10}'.format("Off"),
        print '{0:10}'.format("Size"),
        print '{0:4}'.format("ES"),
        print '{0:4}'.format("Flg"),
        print '{0:4}'.format("Lk"),
        print '{0:4}'.format("Inf"),
        print '{0:4}'.format("Al")

    def get_packed_data(self):
        '''
           get_packed_data
        '''
        values = [self.e_ident,
                  self.e_type,
                  self.e_machine,
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
                 ]

        return (Elf32_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# Program Header Class
#----------------------------------------------------------------------------
class Elf32_Phdr(object):
    '''
       class Elf32_Phdr

       Program Header Class
    '''
    # Structure object to align and package the ELF Program Header
    s = struct.Struct('I' * 8)

    # ELF Program Header Type
    PT_NULL = 0
    PT_LOAD = 1
    PT_DYNAMIC = 2
    PT_INTERP = 3
    PT_NOTE = 4
    PT_SHLIB = 5
    PT_PHDR = 6
    PT_LOPROC = 0x70000000
    PT_HIPROC = 0x7ffffff

    def __init__(self, data):
        '''
           __init__
        '''
        unpacked_data = (Elf32_Phdr.s).unpack(data)
        self.unpacked_data = unpacked_data
        self.p_type = unpacked_data[0]
        self.p_offset = unpacked_data[1]
        self.p_vaddr = unpacked_data[2]
        self.p_paddr = unpacked_data[3]
        self.p_filesz = unpacked_data[4]
        self.p_memsz = unpacked_data[5]
        self.p_flags = unpacked_data[6]
        self.p_align = unpacked_data[7]

    def get_type_str(self):
        '''
           get_type_str
        '''
        code = self.p_type
        if self.PT_NULL == code:
            return "NULL"
        elif self.PT_LOAD == code:
            return "LOAD"
        elif self.PT_DYNAMIC == code:
            return "DYNAMIC"
        elif self.PT_INTERP == code:
            return "INTERP"
        elif self.PT_NOTE == code:
            return "NOTE"
        elif self.PT_SHLIB == code:
            return "SHLIB"
        elif self.PT_PHDR == code:
            return "PHDR"
        elif self.PT_LOPROC == code:
            return "LOPROC"
        elif self.PT_HIPROC == code:
            return "HIPROC"
        else:
            return "ERROR"

    def print_values(self):
        '''
           print_values
        '''
        print "ATTRIBUTE / VALUE"
        for attr, value in self.__dict__.iteritems():
            print attr, value

    def readelf_print_values(self):
        '''
           readelf_print_values
        '''
        print '{0:8}'.format(self.get_type_str()),                #Type
        print '{0:#010x}'.format(self.p_offset),                  #Addr
        print '{0:#010x}'.format(self.p_vaddr),                   #Vaddr
        print '{0:#010x}'.format(self.p_paddr),                   #Paddr
        print '{0:#010x}'.format(self.p_filesz),                  #FS
        print '{0:#010x}'.format(self.p_memsz),                   #MS
        print '{0:#04x}'.format(self.p_flags),                    #Flg
        print '{0:#04x}'.format(self.p_align)                     #Al

    def get_packed_data(self):
        '''
           get_packed_data
        '''
        values = [self.p_type,
                  self.p_offset,
                  self.p_vaddr,
                  self.p_paddr,
                  self.p_filesz,
                  self.p_memsz,
                  self.p_flags,
                  self.p_align
                 ]

        return (Elf32_Phdr.s).pack(*values)

#----------------------------------------------------------------------------
# Section Header Class
#----------------------------------------------------------------------------
class Elf32_Shdr(object):
    '''
       class Elf32_Shdr

       Section Header Class
    '''
    # Structure object to align and package the ELF Section Header
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
    SHT_LOPROC = 0x70000000
    SHT_HIPROC = 0x7fffffff
    SHT_LOUSER = 0x80000000
    SHT_HIUSER = 0xffffffff

    #Section Header Flags
    SHF_WRITE = 0x1
    SHF_ALLOC = 0x2
    SHF_EXECINSTR = 0x4
    SHF_MASKPROC = 0xf000000

    s = struct.Struct('I' * 10)

    def __init__(self, data, index, is_shstrtab):
        '''
           __init__
        '''
        unpacked_data = (Elf32_Shdr.s).unpack(data)
        self.unpacked_data = unpacked_data
        self.sh_name = unpacked_data[0]
        self.sh_type = unpacked_data[1]
        self.sh_flags = unpacked_data[2]
        self.sh_addr = unpacked_data[3]
        self.sh_offset = unpacked_data[4]
        self.sh_size = unpacked_data[5]
        self.sh_link = unpacked_data[6]
        self.sh_info = unpacked_data[7]
        self.sh_addralign = unpacked_data[8]
        self.sh_entsize = unpacked_data[9]
        self.index = index
        self.is_shstrtab = is_shstrtab
        self.str_name = ""

    def get_type_str(self):
        '''
           get_type_str
        '''
        code = self.sh_type
        if self.SHT_NULL == code:
            return "NULL"
        elif self.SHT_PROGBITS == code:
            return "PROGBITS"
        elif self.SHT_SYMTAB == code:
            return "SYMTAB"
        elif self.SHT_STRTAB == code:
            return "STRTAB"
        elif self.SHT_RELA == code:
            return "RELA"
        elif self.SHT_HASH == code:
            return "HASH"
        elif self.SHT_DYNAMIC == code:
            return "DYNAMIC"
        elif self.SHT_NOTE == code:
            return "NOTE"
        elif self.SHT_NOBITS == code:
            return "NOBITS"
        elif self.SHT_REL == code:
            return "REL"
        elif self.SHT_SHLIB == code:
            return "SHLIB"
        elif self.SHT_DYNSYM == code:
            return "DYNSYM"
        elif self.SHT_LOPROC == code:
            return "LOPROC"
        elif self.SHT_HIPROC == code:
            return "HIPROC"
        elif self.SHT_LOUSER == code:
            return "LOUSER"
        elif self.SHT_HIUSER == code:
            return "HIUSER"
        else:
            return "ERROR"

    def print_values(self):
        '''
           print_values
        '''
        print "ATTRIBUTE / VALUE"
        for attr, value in self.__dict__.iteritems():
            print attr, value

    def readelf_print_values(self):
        '''
           readelf_print_values
        '''
        print '[{0:4d}]'.format(self.index),
        print '{0:18} ({1:#05x})'.format(self.str_name, self.sh_name),  #Name
        print '{0:8}'.format(self.get_type_str()),                #Type
        print '{0:#010x}'.format(self.sh_addr),                   #Addr
        print '{0:#010x}'.format(self.sh_offset),                 #Off
        print '{0:#010x}'.format(self.sh_size),                   #Size
        print '{0:#04x}'.format(self.sh_entsize),                 #ES
        print '{0:#04x}'.format(self.sh_flags),                   #Flg
        print '{0:#04x}'.format(self.sh_link),                    #Lk
        print '{0:#4d}'.format(self.sh_info),                     #Inf
        print '{0:#04x}'.format(self.sh_addralign)                #Al

    def get_packed_data(self):
        '''
           get_packed_data
        '''
        values = [self.sh_name,
                  self.sh_type,
                  self.sh_flags,
                  self.sh_addr,
                  self.sh_offset,
                  self.sh_size,
                  self.sh_link,
                  self.sh_info,
                  self.sh_addralign,
                  self.sh_entsize
                 ]

        return (Elf32_Shdr.s).pack(*values)

    def get_dict(self):
        '''
           get_dict
        '''
        return dict([('nr', self.index),
                     ('name', self.str_name),
                     ('type', self.get_type_str()),
                     ('addr', self.sh_addr),
                     ('off', self.sh_offset),
                     ('size', self.sh_size),
                     ('es', self.sh_entsize),
                     ('flg', self.sh_flags),
                     ('lk', self.sh_link),
                     ('inf', self.sh_info),
                     ('al', self.sh_addralign)
                    ])

#----------------------------------------------------------------------------
# Symbol Table Class
#----------------------------------------------------------------------------
class ElfSymbols(object):
    '''
       class ElfSymbols

       Symbol Table Class
    '''
    def __init__(self, elf_fp, shdr_table):
        '''
           __init__
        '''
        self.elf_fp = elf_fp
        self.shdr_table = shdr_table
        self.symtab_sh = None
        self.strtab_sh = None
        self.symbol_dict = {}

        for shdr in shdr_table:
            if shdr.str_name == ".symtab":
                self.symtab_sh = shdr
            if shdr.str_name == ".strtab":
                self.strtab_sh = shdr
        if self.symtab_sh == None or self.strtab_sh == None:
            err_str = 'Missing Symbol Tab or String Tab in ELF file'
            raise Exception(err_str)

        #seek to the symtab_offset
        elf_fp.seek(self.symtab_sh.sh_offset)
        #each entry is 16 bytes, so num_entries = size/16
        num_entries = self.symtab_sh.sh_size/16
        symbol_list = []
        for idx in range(num_entries):
            string = struct.Struct('IIIccH')
            #offset = elf_fp.tell()
            buf = elf_fp.read(16)
            if len(buf) != 16:
                err_str = \
                    'Read for 16 bytes failed, read data of size ' + len(buf)
                raise Exception(err_str)

            unpacked_data = string.unpack(buf)
            name = unpacked_data[0]
            value = unpacked_data[1]
            size = unpacked_data[2]
            info = unpacked_data[3]
            #other = unpacked_data[4]
            shndx = unpacked_data[5]

            symbol_list.append({"name":name, "value":value, "size": size, \
                                "type": info, "shndx":shndx})

        for symbol in symbol_list:
            self.elf_fp.seek(self.strtab_sh.sh_offset+symbol["name"])
            symbol_name = ""
            char = elf_fp.read(1)
            while '\x00' != char:
                symbol_name += char
                char = elf_fp.read(1)
            value = symbol["value"]
            size = symbol["size"]
            types = symbol["type"]
            shndx = symbol["shndx"]

            file_offset = 0
            for shdr in self.shdr_table:
                if value >= shdr.sh_addr and \
                    value < (shdr.sh_addr + shdr.sh_size):
                    file_offset = value - shdr.sh_addr + shdr.sh_offset

            # if its a symbol with valid symbol name
            if len(symbol_name) != 0:
                self.symbol_dict[symbol_name] = {"file_offset": file_offset, \
                                                 "vaddr":value, "size": size, \
                                                 "type":types, "shndx":shndx}

    def get_offset(self, name):
        '''
           get_offset
        '''
        try:
            return self.symbol_dict[name]["file_offset"]
        except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0

    def get_vaddr(self, name):
        '''
           get_vaddr
        '''
        try:
            return self.symbol_dict[name]["vaddr"]
        except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0

#----------------------------------------------------------------------------
# Dynamic Symbol Table Class
#----------------------------------------------------------------------------
class ElfDynSymbols(object):
    '''
        class ElfDynSymbols

        Dynamic Symbol Table Class
    '''
    def __init__(self, elf_fp, shdr_table):
        '''
           __init__
        '''
        self.elf_fp = elf_fp
        self.shdr_table = shdr_table
        self.symtab_sh = None
        self.strtab_sh = None
        self.symbol_dict = {}

        for shdr in shdr_table:
            if shdr.str_name == ".dynsym":
                self.symtab_sh = shdr
            if shdr.str_name == ".dynstr":
                self.strtab_sh = shdr
        if self.symtab_sh == None or self.strtab_sh == None:
            err_str = 'Missing Dynamic Symbol Tab or String Tab in ELF file'
            raise Exception(err_str)

        #seek to the symtab_offset
        elf_fp.seek(self.symtab_sh.sh_offset)
        #each entry is 16 bytes, so num_entries = size/16
        num_entries = self.symtab_sh.sh_size/16
        symbol_list = []
        for idx in range(num_entries):
            strng = struct.Struct('IIIccH')
            #offset = elf_fp.tell()
            buf = elf_fp.read(16)
            if len(buf) != 16:
                err_str = \
                    'Read for 16 bytes failed, read data of size ' + len(buf)
                raise Exception(err_str)

            unpacked_data = strng.unpack(buf)
            name = unpacked_data[0]
            value = unpacked_data[1]
            size = unpacked_data[2]
            info = unpacked_data[3]
            #other = unpacked_data[4]
            shndx = unpacked_data[5]

            symbol_list.append({"name":name, "value":value, "size": size, \
                "type": info, "shndx":shndx})

        for symbol in symbol_list:
            self.elf_fp.seek(self.strtab_sh.sh_offset+symbol["name"])
            symbol_name = ""
            char = elf_fp.read(1)
            while '\x00' != char:
                symbol_name += char
                char = elf_fp.read(1)
            value = symbol["value"]
            size = symbol["size"]
            types = symbol["type"]
            shndx = symbol["shndx"]

            file_offset = 0
            for shdr in self.shdr_table:
                if value >= shdr.sh_addr and \
                    value < (shdr.sh_addr + shdr.sh_size):
                    file_offset = value - shdr.sh_addr + shdr.sh_offset

            # if its a symbol with valid symbol name
            if len(symbol_name) != 0:
                self.symbol_dict[symbol_name] = {"file_offset": file_offset, \
                    "vaddr":value, "size": size, "type":types, "shndx":shndx}

    def get_offset(self, name):
        '''
           get_offset
        '''
        try:
            return self.symbol_dict[name]["file_offset"]
        except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0

    def get_vaddr(self, name):
        '''
           get_vaddr
        '''
        try:
            return self.symbol_dict[name]["vaddr"]
        except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0

#----------------------------------------------------------------------------
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
    '''
       verify_elf_header

       Verify ELF header contents from an input ELF file
    '''
    if (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG0_INDEX] != \
    	 Elf32_Ehdr.ELFINFO_MAG0) or    \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG1_INDEX] != \
         Elf32_Ehdr.ELFINFO_MAG1) or   \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG2_INDEX] != \
         Elf32_Ehdr.ELFINFO_MAG2) or   \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG3_INDEX] != \
         Elf32_Ehdr.ELFINFO_MAG3) or   \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_CLASS_INDEX] != \
         Elf32_Ehdr.ELFINFO_CLASS_32) or \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_VERSION_INDEX] != \
         Elf32_Ehdr.ELFINFO_VERSION_CURRENT):

        return False
    else:
        return True

