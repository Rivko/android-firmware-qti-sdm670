# Copyright (c) 2011
# Qualcomm Technologies Incorporated
#!/usr/bin/env python

import sys
import os
import struct

#----------------------------------------------------------------------------
# Header Class Notes:
# In order to properly read and write the header structures as binary data,
# the Python Struct library is used to align and package up the header objects
# All Struct objects are initialized by a special string with the following
# notation. These structure objects are then used to decode binary data in order
# to fill out the appropriate class in Python, or they are used to package up
# the Python class so that we may write the binary data out. 
#----------------------------------------------------------------------------
"""
    Format | C Type         | Python Type | Standard Size
    -----------------------------------------------------
    1) 'X's  | char *         | string      | 'X' bytes
    2) H     | unsigned short | integer     | 2 bytes
    3) I     | unsigned int   | integer     | 4 bytes

"""

VERBOSE_ELFPARSEUTIL = False

def DEBUG_PRINT(dbg_str):
    global VERBOSE_ELFPARSEUTIL
    if True == VERBOSE_ELFPARSEUTIL:
        print dbg_str

def IS_DEBUG():
    global VERBOSE_ELFPARSEUTIL
    return VERBOSE_ELFPARSEUTIL

def OPEN(file_name, mode):
    try:
        fp = open(file_name, mode)
    except IOError:
        raise RuntimeError, "The file could not be opened: " + file_name

    # File open has succeeded with the given mode, return the file object
    return fp

#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:
    # Structure object to align and package the ELF Header
    s = struct.Struct('16sHHIIIIIHHHHHH') 

    # ELF Definitions
    ELF_HDR_SIZE              = 52
    ELFINFO_MAG0_INDEX        = 0
    ELFINFO_MAG1_INDEX        = 1
    ELFINFO_MAG2_INDEX        = 2
    ELFINFO_MAG3_INDEX        = 3
    ELFINFO_MAG0              = '\x7f'
    ELFINFO_MAG1              = 'E'
    ELFINFO_MAG2              = 'L'
    ELFINFO_MAG3              = 'F'
    ELFINFO_CLASS_INDEX       = 4
    ELFINFO_DATA_INDEX        = 5
    ELFINFO_VERSION_INDEX     = 6
    ELFINFO_VERSION_CURRENT   = '\x01'

    #ELF Type Codes
    ET_NONE         = 0
    ET_REL          = 1
    ET_EXEC         = 2
    ET_DYN          = 3
    ET_CORE         = 4
    ET_LOPROC       = 0xff00
    ET_HIPROC       = 0xffff

    #EI_CLASS codes
    ELFINFO_CLASS_NONE  = '\x00'
    ELFINFO_CLASS_32    = '\x01'
    ELFINFO_CLASS_64    = '\x02'

    #EI_DATA codes
    ELFDATANONE     = '\x00'     #Invalid data encoding
    ELFDATA2LSB     = '\x01'     #2's complement LSB endianess
    ELFDATA2MSB     = '\x02'     #2's complement MSB endianess

    #Machine Codes
    EM_NONE         = 0
    EM_M32          = 1
    EM_SPARC        = 2
    EM_386          = 3
    EM_68K          = 4
    EM_88K          = 5
    EM_860          = 7
    EM_MIPS         = 8
    EM_ARM          = 40

    def __init__(self, data): 
        unpacked_data       = (Elf32_Ehdr.s).unpack(data)
        self.unpacked_data  = unpacked_data
        self.e_ident        = unpacked_data[0]
        self.e_type         = unpacked_data[1]
        self.e_machine      = unpacked_data[2]
        self.e_version      = unpacked_data[3]
        self.e_entry        = unpacked_data[4]
        self.e_phoff        = unpacked_data[5]
        self.e_shoff        = unpacked_data[6]
        self.e_flags        = unpacked_data[7]
        self.e_ehsize       = unpacked_data[8]
        self.e_phentsize    = unpacked_data[9]
        self.e_phnum        = unpacked_data[10]
        self.e_shentsize    = unpacked_data[11]
        self.e_shnum        = unpacked_data[12]
        self.e_shstrndx     = unpacked_data[13]

    def get_ident_class_str(self):
        code = self.e_ident[self.ELFINFO_CLASS_INDEX]
        if (self.ELFINFO_CLASS_32 == code):
            return "ELF32"
        elif (self.ELFINFO_CLASS_64 == code):
            return "ELF64"
        else:
            return "ERROR"

    def get_ident_data_str(self):
        code = self.e_ident[self.ELFINFO_DATA_INDEX]
        if (self.ELFDATA2LSB == code):
            return "2's complement, little endian"
        elif (self.ELFDATA2MSB == code):
            return "2's complement, big endian"
        else:
            return "ERROR " + str(ord(code))

    def get_type_str(self):
        code = self.e_type
        if (self.ET_NONE == code):
            return "NONE"
        elif (self.ET_REL == code):
            return "REL"
        elif (self.ET_EXEC == code):
            return "EXEC"
        elif (self.ET_DYN == code):
            return "DYN"
        elif (self.ET_CORE == code):
            return "CORE"
        elif (self.ET_LOPROC == code):
            return "LOPROC"
        elif (self.ET_HIPROC == code):
            return "HIPROC"
        else:
            return "ERROR"

    def get_machine_str(self):
        code = self.e_machine
        if (self.EM_NONE == code):
            return "NONE"
        elif (self.EM_ARM == code):
            return "ARM"
        elif (self.EM_M32 == code):
            return "AT&T WE 32100"
        elif (self.EM_SPARC == code):
            return "SPARC"
        elif (self.EM_386 == code):
            return "Intel 80386"
        elif (self.EM_68K == code):
            return "Motorola 68000"
        elif (self.EM_88K == code):
            return "Motorola 88000"
        elif (self.EM_860 == code):
            return "Intel 80860"
        elif (self.EM_MIPS == code):
            return "MIPS RS300"
        else:
            return "<unknown>"

    def print_values(self):
        print "ATTRIBUTE / VALUE"
        for attr, value in self.__dict__.iteritems():
            print attr, value

    def readelf_print_values(self):
        print '   {0:<34}'.format("Magic:"),
        for ch in str(self.e_ident):
            print '{0:02x}'.format(ord(ch)),
        print ''
        print '   {0:<34} {1}'.format("Class:", self.get_ident_class_str())
        print '   {0:<34} {1}'.format("Data:", self.get_ident_data_str())
        print '   {0:<34} {1}'.format("Version:", ord(self.e_ident[Elf32_Ehdr.ELFINFO_VERSION_INDEX]))
        #print '   {0:<34} {1}'.format("OS/ABI:", "TBD")
        #print '   {0:<34} {1}'.format("ABI Version:", "TBD")
        print '   {0:<34} {1} ({2:#0x})'.format("Type:", self.get_type_str(), self.e_type)
        print '   {0:<34} {1} ({2:#0x})'.format("Machine:", self.get_machine_str(), self.e_machine)
        print '   {0:<34} {1:#0x}'.format("Version:", self.e_version)
        print '   {0:<34} {1:#0x}'.format("Entry point address:", self.e_entry)
        print '   {0:<34} {1:#d} {2}' \
                    .format("Start of program headers:", self.e_phoff, "(bytes into file)")
        print '   {0:<34} {1:#d} {2}'   \
                    .format("Start of section headers:", self.e_shoff, "(bytes into file)")
        print '   {0:<34} {1:#0x}'.format("Flags:", self.e_flags)
        print '   {0:<34} {1:#d} {2}'.format("Size of this header:", self.e_ehsize, "(bytes)")
        print '   {0:<34} {1:#d} {2}'   \
                    .format("Size of program headers:", self.e_phentsize, "(bytes)")
        print '   {0:<34} {1:#0d}'.format("Number of program headers:", self.e_phnum)
        print '   {0:<34} {1:#d} {2}'   \
                    .format("Size of section headers:", self.e_shentsize, "(bytes)")
        print '   {0:<34} {1}'.format("Number of section headers:", self.e_shnum)
        print '   {0:<34} {1}'.format("Section header string table index:", self.e_shstrndx)

    def readelf_elf_header_preamble(self):
        print "\n"
        print "ELF Header:"        

    def readelf_program_header_preamble(self):
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

class Elf32_Phdr:
    # Structure object to align and package the ELF Program Header 
    s = struct.Struct('I' * 8) 

    # ELF Program Header Type
    PT_NULL         = 0
    PT_LOAD         = 1
    PT_DYNAMIC      = 2
    PT_INTERP       = 3
    PT_NOTE         = 4
    PT_SHLIB        = 5
    PT_PHDR         = 6
    PT_LOPROC       = 0x70000000
    PT_HIPROC       = 0x7ffffff

    def __init__(self, data): 
        unpacked_data       = (Elf32_Phdr.s).unpack(data)
        self.unpacked_data  = unpacked_data
        self.p_type         = unpacked_data[0]
        self.p_offset       = unpacked_data[1]
        self.p_vaddr        = unpacked_data[2]
        self.p_paddr        = unpacked_data[3]
        self.p_filesz       = unpacked_data[4]
        self.p_memsz        = unpacked_data[5]
        self.p_flags        = unpacked_data[6]
        self.p_align        = unpacked_data[7]

    def get_type_str(self):
        code = self.p_type
        if (self.PT_NULL == code):
            return "NULL"
        elif (self.PT_LOAD == code):
            return "LOAD"
        elif (self.PT_DYNAMIC == code):
            return "DYNAMIC"
        elif (self.PT_INTERP == code):
            return "INTERP"
        elif (self.PT_NOTE == code):
            return "NOTE"
        elif (self.PT_SHLIB == code):
            return "SHLIB"
        elif (self.PT_PHDR == code):
            return "PHDR"
        elif (self.PT_LOPROC == code):
            return "LOPROC"
        elif (self.PT_HIPROC == code):
            return "HIPROC"
        else:
            return "ERROR"
            
    def print_values(self):
        print "ATTRIBUTE / VALUE"
        for attr, value in self.__dict__.iteritems():
            print attr, value

    def readelf_print_values(self):
        print '{0:8}'.format(self.get_type_str()),                #Type        
        print '{0:#010x}'.format(self.p_offset),                  #Addr
        print '{0:#010x}'.format(self.p_vaddr),                   #Vaddr
        print '{0:#010x}'.format(self.p_paddr),                   #Paddr
        print '{0:#010x}'.format(self.p_filesz),                  #FS
        print '{0:#010x}'.format(self.p_memsz),                   #MS
        print '{0:#04x}'.format(self.p_flags),                    #Flg
        print '{0:#04x}'.format(self.p_align)                     #Al
        
    def get_packed_data(self):
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

class Elf32_Shdr:
    # Structure object to align and package the ELF Section Header
    SHT_NULL        = 0
    SHT_PROGBITS    = 1
    SHT_SYMTAB      = 2
    SHT_STRTAB      = 3
    SHT_RELA        = 4
    SHT_HASH        = 5
    SHT_DYNAMIC     = 6
    SHT_NOTE        = 7
    SHT_NOBITS      = 8
    SHT_REL         = 9
    SHT_SHLIB       = 10
    SHT_DYNSYM      = 11
    SHT_LOPROC      = 0x70000000
    SHT_HIPROC      = 0x7fffffff
    SHT_LOUSER      = 0x80000000
    SHT_HIUSER      = 0xffffffff

    #Section Header Flags
    SHF_WRITE       = 0x1
    SHF_ALLOC       = 0x2
    SHF_EXECINSTR   = 0x4
    SHF_MASKPROC    = 0xf000000

    s = struct.Struct('I' * 10) 

    def __init__(self, data, index, is_shstrtab): 
        unpacked_data       = (Elf32_Shdr.s).unpack(data)
        self.unpacked_data  = unpacked_data
        self.sh_name        = unpacked_data[0]
        self.sh_type        = unpacked_data[1]
        self.sh_flags       = unpacked_data[2]
        self.sh_addr        = unpacked_data[3]
        self.sh_offset      = unpacked_data[4]
        self.sh_size        = unpacked_data[5]
        self.sh_link        = unpacked_data[6]
        self.sh_info        = unpacked_data[7]
        self.sh_addralign   = unpacked_data[8]
        self.sh_entsize     = unpacked_data[9]
        self.index          = index
        self.is_shstrtab    = is_shstrtab
        self.str_name       = ""

    def get_type_str(self):
        code = self.sh_type
        if (self.SHT_NULL == code):
            return "NULL"
        elif (self.SHT_PROGBITS == code):
            return "PROGBITS"
        elif (self.SHT_SYMTAB == code):
            return "SYMTAB"
        elif (self.SHT_STRTAB == code):
            return "STRTAB"
        elif (self.SHT_RELA == code):
            return "RELA"
        elif (self.SHT_HASH == code):
            return "HASH"
        elif (self.SHT_DYNAMIC == code):
            return "DYNAMIC"
        elif (self.SHT_NOTE == code):
            return "NOTE"
        elif (self.SHT_NOBITS == code):
            return "NOBITS"
        elif (self.SHT_REL == code):
            return "REL"
        elif (self.SHT_SHLIB == code):
            return "SHLIB"
        elif (self.SHT_DYNSYM == code):
            return "DYNSYM"
        elif (self.SHT_LOPROC == code):
            return "LOPROC"
        elif (self.SHT_HIPROC == code):
            return "HIPROC"
        elif (self.SHT_LOUSER == code):
            return "LOUSER"
        elif (self.SHT_HIUSER == code):
            return "HIUSER"
        else:
            return "ERROR"

    def print_values(self):
        print "ATTRIBUTE / VALUE"
        for attr, value in self.__dict__.iteritems():
            print attr, value
    
    def readelf_print_values(self):
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
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
    if (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG0_INDEX] != Elf32_Ehdr.ELFINFO_MAG0) or    \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG1_INDEX] != Elf32_Ehdr.ELFINFO_MAG1) or   \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG2_INDEX] != Elf32_Ehdr.ELFINFO_MAG2) or   \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_MAG3_INDEX] != Elf32_Ehdr.ELFINFO_MAG3) or   \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_CLASS_INDEX] != Elf32_Ehdr.ELFINFO_CLASS_32) or \
        (elf_header.e_ident[Elf32_Ehdr.ELFINFO_VERSION_INDEX] != Elf32_Ehdr.ELFINFO_VERSION_CURRENT):

        return False
    else:
        return True

#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an 
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def preprocess_elf_file(elf_fp, elf_file_name):
    # Initialize
    elf_fp.seek(0)
    elf_header = Elf32_Ehdr(elf_fp.read(Elf32_Ehdr.ELF_HDR_SIZE))
    phdr_table = []
    shdr_table = []

    # Verify ELF header information
    if verify_elf_header(elf_header) is False:
        raise RuntimeError, "ELF file failed verification: " + elf_file_name

    # Get program header details (size, file offset, number of headers)
    phdr_size = elf_header.e_phentsize
    phdr_file_offset = elf_header.e_phoff
    phdr_count = elf_header.e_phnum

    # Read all the program headers into phdr table
    elf_fp.seek(phdr_file_offset)
    for i in range(phdr_count):
        phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size))) 

    # Get program header details (size, file offset, number of headers)
    shdr_size = elf_header.e_shentsize
    shdr_file_offset = elf_header.e_shoff
    shdr_count = elf_header.e_shnum

    # Read all the section headers into shdr table
    shdr_shstrtab_file_offset = -1
    elf_fp.seek(shdr_file_offset)
    for i in range(shdr_count):
        is_shstrtab = 0
        if i == elf_header.e_shstrndx:
            is_shstrtab = 1
        data = Elf32_Shdr(elf_fp.read(shdr_size), i, is_shstrtab)
        shdr_table.append(data)
        if is_shstrtab == 1:
            shdr_shstrtab_file_offset = data.sh_offset
            
    for i in range(shdr_count):
        elf_fp.seek(shdr_shstrtab_file_offset + shdr_table[i].sh_name)
        ch = elf_fp.read(1)
        while ('\x00' != ch):
            shdr_table[i].str_name += ch
            ch = elf_fp.read(1)

    return dict([('elf_fp', elf_fp), 
                    ('elf_file_name', elf_file_name), 
                    ('elf_header', elf_header),
                    ('phdr_table', phdr_table),
                    ('shdr_table', shdr_table)])

def readelf(elf_dict):
    print "\nReading ELF File: " + elf_dict['elf_file_name']
    elf_header = elf_dict['elf_header']
    phdr_table = elf_dict['phdr_table']
    shdr_table = elf_dict['shdr_table']

    elf_header.readelf_elf_header_preamble()
    elf_header.readelf_print_values()

    elf_header.readelf_program_header_preamble()
    for phdr in phdr_table:
        phdr.readelf_print_values()

    elf_header.readelf_section_header_preamble()
    for shdr in shdr_table:
        shdr.readelf_print_values()

    return None

def query_section_by_name(elf_dict, section_name, compare_full_sect_str=False):
    shdr_table = elf_dict['shdr_table']
    out_shdr = []
    #import pdb;pdb.set_trace()
    #Note: We can have multiple sections with the same name            
    for shdr in shdr_table:
        if compare_full_sect_str is True:
            if section_name == shdr.str_name:
                out_shdr.append(shdr.get_dict())
        else:
            if section_name in shdr.str_name:
                out_shdr.append(shdr.get_dict())
    
    return out_shdr

def query_section_by_name_raw(elf_dict, section_name):
    shdr_table = elf_dict['shdr_table']
    out_shdr = []

    #Note: We can have multiple sections with the same name            
    for shdr in shdr_table:        
        if section_name == shdr.str_name:          
            #print "Section Header Name: " + shdr.str_name + " Section Name: " + section_name
            out_shdr.append(shdr)

    return out_shdr

def merge_sections_by_name(out_elf_dict, sec_elf_dict, pri_elf_file_name, 
                                pri_section_name, sec_section_name):
    out_elf_fp     = out_elf_dict['elf_fp']
    out_elf_header = out_elf_dict['elf_header']
    out_phdr_table = out_elf_dict['phdr_table']
    out_shdr_table = out_elf_dict['shdr_table']

    sec_elf_fp     = sec_elf_dict['elf_fp']
    sec_elf_header = sec_elf_dict['elf_header']
    sec_phdr_table = sec_elf_dict['phdr_table']
    sec_shdr_table = sec_elf_dict['shdr_table']
    sec_elf_file_name = sec_elf_dict['elf_file_name']

    #Validation checks for both output ELF (i.e. primary ELF) and the secondary ELF

    #Check flags for each, 2's complement, arch etc.
    if out_elf_header.e_machine != sec_elf_header.e_machine:
        err_str = 'Error: ELF {0} and ELF {1} machine types do not match.'  \
                        .format(pri_elf_file_name, sec_elf_file_name)
        raise RuntimeError, err_str
    if out_elf_header.e_type != sec_elf_header.e_type:
        err_str = 'Error: ELF {0} and ELF {1} types do not match.'          \
                        .format(pri_elf_file_name, sec_elf_file_name)
        raise RuntimeError, err_str
    if out_elf_header.e_ident[Elf32_Ehdr.ELFINFO_DATA_INDEX] !=             \
            sec_elf_header.e_ident[Elf32_Ehdr.ELFINFO_DATA_INDEX]:
        err_str = 'Error: ELF {0} and ELF {1} Data types do not match'      \
                        .format(pri_elf_file_name, sec_elf_file_name)
        raise RuntimeError, err_str

    #Check if the section is present in output ELF
    out_shdr_list = query_section_by_name_raw(out_elf_dict, pri_section_name)
    if not out_shdr_list:
        err_str = 'Error: Section containing name "{0}" not found in {1}'   \
                    .format(pri_section_name, pri_elf_file_name)
        raise RuntimeError, err_str

    #Check if multiple sections containing the same name are present in output ELF
    if 1 != len(out_shdr_list):
        err_str = 'Multiple sections containing name "{0}" found in {1}. Cannot merge.' \
                    .format(pri_section_name, pri_elf_file_name)
        raise RuntimeError, err_str

    #Check if the section is present in secondary ELF
    sec_shdr_list = query_section_by_name_raw(sec_elf_dict, sec_section_name)
    if not sec_shdr_list:
        err_str = 'Error: Section containing name "{0}" not found in {1}'   \
                        .format(sec_section_name, sec_elf_file_name)
        raise RuntimeError, err_str

    #Variable to denote output section header
    out_shdr = out_shdr_list[0]

    #Check for PROGBITS for each of the sections and
    #Obtain total size for all sections    
    total_sec_size = 0
    for shdr in sec_shdr_list:
        if shdr.sh_type != Elf32_Shdr.SHT_PROGBITS:
            err_str = 'Error: ELF file {0}\'s Section: "{1}" not marked as PROGBITS'    \
                        .format(sec_elf_file_name, shdr.str_name)
            raise RuntimeError, err_str
        # Disabling the section permission check as main elf  devcfg section is readonly
        # if (shdr.sh_flags & out_shdr.sh_flags) != shdr.sh_flags:
            # err_str = 'Error: ELF file {0} Section:"{1}"\'s permissions do not match '  \
                        # 'ELF file {2} Section:"{3}"\' permissions.'                     \
                        # .format(sec_elf_file_name, shdr.str_name,                       \
                                    # pri_elf_file_name, out_shdr.str_name)
            # raise RuntimeError, err_str

        total_sec_size += shdr.sh_size

    #Check if the output elf section size can fully accomodate the secondary ELF's section
    if out_shdr.sh_size < total_sec_size:
        err_str = 'Error: ELF file {0}\'s Section: "{1}" not big enough to contain the secondary' \
                  'ELF\'s section(s).'.format(pri_elf_file_name, out_shdr.str_name)
        raise RuntimeError, err_str

    if out_shdr.sh_type != Elf32_Shdr.SHT_PROGBITS:
        err_str = 'Error: ELF file {0}\'s Section: "{1}" not marked as PROGBITS'    \
                    .format(pri_elf_file_name, out_shdr.str_name)
        raise RuntimeError, err_str

    #Check if the virtual address range of all secondary sections fits in the output ELF
    #If validations clear copy contents into output ELF section
    min_out_vaddr = out_shdr.sh_addr
    max_out_vaddr = out_shdr.sh_addr + out_shdr.sh_size

    #Set a flag to check if this is the first iteration
    first_iter = True

    #Set the seek pointer to the section offset start in the output ELF
    out_seek_offset = out_shdr.sh_offset    

    print '\n'    
    for shdr in sec_shdr_list:
        sec_min_vaddr = shdr.sh_addr
        sec_max_vaddr = shdr.sh_addr + shdr.sh_size
        sec_seek_offset = shdr.sh_offset
        sec_size = shdr.sh_size

        if ((sec_min_vaddr < min_out_vaddr) or (sec_min_vaddr > max_out_vaddr))                   \
                and ((sec_max_vaddr < min_out_vaddr) or (sec_max_vaddr > max_out_vaddr)):
            err_str = 'Error: ELF file {0}\'s section {1} address not in range:[{2:#0x}, {3:#0x}]'\
                        .format(sec_elf_file_name, shdr.str_name, min_out_vaddr, max_out_vaddr)
            raise RuntimeError, err_str

        if (True == first_iter):
            first_iter = False
            prev_sec_addr = sec_min_vaddr
            if (sec_min_vaddr != min_out_vaddr):
                err_str = 'Error: ELF file {0}\'s section {1} start address not equal to ELF file'\
                        '{2} section {3} start address:{4:#0x}' \
                        .format(sec_elf_file_name, shdr.str_name, pri_elf_file_name, \
                                out_shdr.str_name, sec_min_vaddr)
                raise RuntimeError, err_str
        #Merge in contents of all secondary ELF sections' in the output ELF        
        gap_in_file_offset = sec_min_vaddr - prev_sec_addr
        out_seek_offset = out_shdr.sh_offset + gap_in_file_offset
        print 'Elfparser Merge: Copying {0} bytes from ELF file {1} section "{2}"' \
                .format(sec_size, sec_elf_file_name, shdr.str_name),
        print 'at offset {0:#0x} into {1} at offset {2:#0x}'    \
            .format(sec_seek_offset, pri_elf_file_name, out_seek_offset)
        out_elf_fp.seek(out_seek_offset)
        sec_elf_fp.seek(sec_seek_offset)
        out_elf_fp.write(sec_elf_fp.read(sec_size))
        prev_sec_addr = sec_min_vaddr
        out_elf_fp.flush()

    return 0
