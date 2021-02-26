#===============================================================================
#
# ELF PARSER UTIL TOOLS
#
# GENERAL DESCRIPTION
#    Contains all ELF Parser Utilities for ELF
#
# Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#-------------------------------------------------------------------------------
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/10/14   aa      Initial Version.
#===============================================================================
import struct
import os
import shutil

#----------------------------------------------------------------------------
# CLASS DEFINITIONS BEGIN
#----------------------------------------------------------------------------

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

#----------------------------------------------------------------------------
# ELF32 Header Class
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

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
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
# ELF64 Header Class
#----------------------------------------------------------------------------
class Elf64_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIQQQIHHHHHH') 
   
   # ELF Definitions
   ELF_HDR_SIZE              = 64
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
      unpacked_data       = (Elf64_Ehdr.s).unpack(data)
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

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
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

      return (Elf64_Ehdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF32 Program Header Class
#----------------------------------------------------------------------------
class Elf32_Phdr:

   # Structure object to align and package the ELF Program Header
   s = struct.Struct('I' * 8)

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

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
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
# ELF64 Program Header Class
#----------------------------------------------------------------------------
class Elf64_Phdr:

   # Structure object to align and package the ELF Program Header
   s = struct.Struct('IIQQQQQQ')

   def __init__(self, data):
      unpacked_data       = (Elf64_Phdr.s).unpack(data)
      self.unpacked_data  = unpacked_data
      self.p_type         = unpacked_data[0]
      self.p_flags        = unpacked_data[1]
      self.p_offset       = unpacked_data[2]
      self.p_vaddr        = unpacked_data[3]
      self.p_paddr        = unpacked_data[4]
      self.p_filesz       = unpacked_data[5]
      self.p_memsz        = unpacked_data[6]
      self.p_align        = unpacked_data[7]

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
      values = [self.p_type,
                self.p_flags,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_align
               ]

      return (Elf64_Phdr.s).pack(*values)

#----------------------------------------------------------------------------
# ELF32 Section Header Class
#----------------------------------------------------------------------------

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
# ELF64 Section Header Class
#----------------------------------------------------------------------------
class Elf64_Shdr:
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

    s = struct.Struct('IIQQQQIIQQ') 

    def __init__(self, data, index, is_shstrtab): 
        unpacked_data       = (Elf64_Shdr.s).unpack(data)
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

        return (Elf64_Shdr.s).pack(*values)

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
# ELF32 SYMBOLS CLASS
#----------------------------------------------------------------------------
class Elf32_Symbols:
      def __init__(self, elf_fp, shdr_table): 
          self.elf_fp      = elf_fp
          self.shdr_table  = shdr_table
          self.symtab_sh   = None
          self.strtab_sh   = None
          self.symbol_dict = {}
    
          for sh in shdr_table:
             if sh.str_name == ".symtab":
                self.symtab_sh = sh
             if sh.str_name == ".strtab":
                self.strtab_sh = sh
          if self.symtab_sh == None or self.strtab_sh == None:
             print 'Missing Symbol Tab or String Tab in ELF file'
             return 0
          #seek to the symtab_offset    
          elf_fp.seek(self.symtab_sh.sh_offset)
          
          #each entry is 16 bytes, so num_entries = size/16
          num_entries = self.symtab_sh.sh_size/16
          symbol_list = []
          for _ in range(num_entries): 
              s = struct.Struct('IIIccH' ) 
              unpacked_data = s.unpack(elf_fp.read(16))
              name          = unpacked_data[0]
              value         = unpacked_data[1]
              size          = unpacked_data[2]
              #only add none zero size entries
              if size and name:
                 symbol_list.append({"name":name,"value":value})
                                      
          for symbol in symbol_list:
              self.elf_fp.seek(self.strtab_sh.sh_offset+symbol["name"])
              symbol_name = ""
              ch = elf_fp.read(1)
              while ('\x00' != ch):
                 symbol_name += ch
                 ch = elf_fp.read(1)                            
              value = symbol["value"]   
              file_offset = 0
              for sh in self.shdr_table:
                  if value>= sh.sh_addr and value<(sh.sh_addr+sh.sh_size):
                      file_offset = value-sh.sh_addr+sh.sh_offset        
              self.symbol_dict[symbol_name] = {"file_offset": file_offset, "vaddr":value}
              
      def get_offset(self,name):
          try:
            return self.symbol_dict[name]["file_offset"]
          except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0
            
      def get_vaddr(self,name):
          try:
            return self.symbol_dict[name]["vaddr"]
          except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0 

#----------------------------------------------------------------------------
# ELF64 SYMBOLS CLASS
#----------------------------------------------------------------------------
class Elf64_Symbols:
      def __init__(self, elf_fp, shdr_table): 
          self.elf_fp      = elf_fp
          self.shdr_table  = shdr_table
          self.symtab_sh   = None
          self.strtab_sh   = None
          self.symbol_dict = {}
    
          for sh in shdr_table:
             if sh.str_name == ".symtab":
                self.symtab_sh = sh
             if sh.str_name == ".strtab":
                self.strtab_sh = sh
          if self.symtab_sh == None or self.strtab_sh == None:
             print 'Missing Symbol Tab or String Tab in ELF file'
             return 0
          #seek to the symtab_offset    
          elf_fp.seek(self.symtab_sh.sh_offset)
          
          #each entry is 16 bytes, so num_entries = size/16
          num_entries = self.symtab_sh.sh_size/24
          symbol_list = []
          for _ in range(num_entries): 
              s = struct.Struct('IccHQQ') 
              unpacked_data = s.unpack(elf_fp.read(24))
              name          = unpacked_data[0]
              value         = unpacked_data[4]
              size          = unpacked_data[5]
              #only add none zero size entries
              if size and name:
                 symbol_list.append({"name":name,"value":value})
                                      
          for symbol in symbol_list:
              self.elf_fp.seek(self.strtab_sh.sh_offset+symbol["name"])
              symbol_name = ""
              ch = elf_fp.read(1)
              while ('\x00' != ch):
                 symbol_name += ch
                 ch = elf_fp.read(1)                            
              value = symbol["value"]   
              file_offset = 0
              for sh in self.shdr_table:
                  if value>= sh.sh_addr and value<(sh.sh_addr+sh.sh_size):
                      file_offset = value-sh.sh_addr+sh.sh_offset        
              self.symbol_dict[symbol_name] = {"file_offset": file_offset, "vaddr":value}
              
      def get_offset(self,name):
          try:
            return self.symbol_dict[name]["file_offset"]
          except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0
            
      def get_vaddr(self,name):
          try:
            return self.symbol_dict[name]["vaddr"]
          except KeyError:
            print 'ERROR! Symbol '+name+' not found'
            return 0 
  
#----------------------------------------------------------------------------
# CLASS DEFINITIONS END
#----------------------------------------------------------------------------