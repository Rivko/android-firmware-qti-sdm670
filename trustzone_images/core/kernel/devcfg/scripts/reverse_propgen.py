#===============================================================================
# R E V E R S E     P R O P G E N 
# Copyright (c) 2011,2014-16 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#===============================================================================
# 
#!/usr/bin/env python
import struct
import os
import shutil
import sys
from optparse import OptionParser
import xml.dom.minidom
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

class StringDevice:
    s = struct.Struct('I'*6)
    def __init__(self, data, base_addr): 
        unpacked_data              = (StringDevice.s).unpack(data)
        self.unpacked_data         = unpacked_data
        self.device_name_addr      = unpacked_data[0]
        self.device_name_offset    = self.device_name_addr - base_addr
        self.hash                  = unpacked_data[1]
        self.prop_offset           = unpacked_data[2]
        self.entry_func_addr       = unpacked_data[3]
        self.num_collision         = unpacked_data[4]
        self.collision_list_addr   = unpacked_data[5]
        self.collision_list_offset = 0
        if self.collision_list_addr:
           self.collision_list_offset = self.collision_list_addr - base_addr
        self.device_name           =""
        self.collision_list        =[]
                
    def print_string_device(self, print_header=False):
        if print_header:
           print 'StringDevice (Name,Hash,Property Offset, Num Collisions, Collision index array)'
        print '\t( {0:}, {1:}, {2:}, {3:}, {4:} )'.format(self.device_name,self.hash,self.prop_offset,self.num_collision,self.collision_list)        
    def populate_device_name_and_collision_entries(self, elf_fp, devcfg_data_segment_addr):
        elf_fp.seek(devcfg_data_segment_addr+self.device_name_offset)
        ch = elf_fp.read(1)
        while ('\x00' != ch):
            self.device_name += ch
            ch = elf_fp.read(1)
        if self.num_collision:    
           elf_fp.seek(devcfg_data_segment_addr+self.collision_list_offset)
           for i in range(self.num_collision):
               s = struct.Struct('I')
               entry = s.unpack(elf_fp.read(4))
               self.collision_list.append(entry[0])   
            
    
class DALPROP_PropsInfo:
    s = struct.Struct('I'*4) 
    def __init__(self, data,base_addr): 
        unpacked_data            = (DALPROP_PropsInfo.s).unpack(data)
        self.unpacked_data       = unpacked_data
        self.prop_bin_addr       = unpacked_data[0]
        self.prop_bin_offset     = self.prop_bin_addr - base_addr
        self.struct_ptr_addr     = unpacked_data[1]
        self.struct_ptr_offset   = self.struct_ptr_addr - base_addr
        self.num_devices         = unpacked_data[2]
        self.string_device_addr  = unpacked_data[3]
        self.string_device_offset= self.string_device_addr - base_addr
        self.base_addr           = base_addr
    def print_props_info(self):   
        print 'DALPROP_PropsInfo (Addr {0:#010x})'.format(self.base_addr)
        print '\tDALPROP_PropBin = (Addr {0:#010x}'.format(self.prop_bin_addr),
        print 'Offset {0:#010x})'. format(self.prop_bin_offset)
        print '\tDALPROP_StructPtrs = (Addr {0:#010x}'.format(self.struct_ptr_addr),
        print 'Offset {0:#010x})'. format(self.struct_ptr_offset)
        print '\tnum_devices = {0:} '.format(self.num_devices)
        print '\tStringDevice = (Addr {0:#010x}'.format(self.string_device_addr),
        print 'Offset {0:#010x})'. format(self.string_device_offset)
        
class DALPropsDir:
    s = struct.Struct('I'*6) 
    def __init__(self, data,dal_props_offset): 
        unpacked_data              = (DALPropsDir.s).unpack(data)
        self.unpacked_data         = unpacked_data
        self.len                   = unpacked_data[0]
        self.name_section_offset   = unpacked_data[1]
        self.string_section_offset = unpacked_data[2]
        self.byte_section_offset   = unpacked_data[3]
        self.uint32_section_offset = unpacked_data[4]
        self.num_devices           = unpacked_data[5]
        self.dal_props_offset      = dal_props_offset
        self.device_offset         = {}
        self.name_section          = []
        self.string_section        = []
        self.byte_section          = []
        self.uint32_section        = []
        
    def print_dalprops_dir(self):   
        print 'DALPropsDir '
        print '\tLen                     = {0:#010x}'.format(self.len)
        print '\tName Section Offset     = {0:#010x}'.format(self.name_section_offset)
        print '\tString Section Offset   = {0:#010x}'.format(self.string_section_offset)
        print '\tByte Section Offset     = {0:#010x}'.format(self.byte_section_offset)
        print '\tUint32 Section Offset   = {0:#010x}'.format(self.uint32_section_offset)
        print '\tNum Devices             = {0:#010x}'.format(self.num_devices)
        print '\t+------------+------------+'
        print '\t| Device  id | offset     | '
        print '\t+------------+------------+'
        for keys in self.device_offset.iterkeys():
            print '\t| {0:#010x} | {1:#010x} |'.format(keys,self.device_offset[keys])
            print '\t+------------+------------+'
            
    def populate_dal_props_dir(self,elf_fp,first_string_device_offset):
        first_device_offset = first_string_device_offset
        elf_fp.seek(self.dal_props_offset+24) #skip 24 bytes of header already scanned by constructor  
        for i in range(self.num_devices):
            s = struct.Struct('I'*2) #device_id, device_offset tuple
            entry = s.unpack(elf_fp.read(8)) #read 8 bytes total
            self.device_offset[entry[0]] = entry[1] # device_id will be the key and offset will point to the start of properties    
            if first_device_offset>entry[1]:
               first_device_offset = entry[1];
        elf_fp.seek(self.dal_props_offset+self.name_section_offset) #skip to name section
        for i in range(self.string_section_offset - self.name_section_offset):
            self.name_section.append(elf_fp.read(1))

        elf_fp.seek(self.dal_props_offset+self.string_section_offset) #skip to string section
        for i in range(self.byte_section_offset - self.string_section_offset):
            self.string_section.append(elf_fp.read(1))

        elf_fp.seek(self.dal_props_offset+self.byte_section_offset) #skip to byte section
        for i in range(self.uint32_section_offset - self.byte_section_offset):
            self.byte_section.append(elf_fp.read(1))
            
        elf_fp.seek(self.dal_props_offset+self.uint32_section_offset) #skip to uint32 section
        for i in range(first_device_offset - self.uint32_section_offset):
            s = struct.Struct('I') 
            entry = s.unpack(elf_fp.read(4)) #read each uint32 
            self.uint32_section.append(entry[0])
            
class Props:    
    #prop types
    DALPROP_ATTR_TYPE_STRING_PTR        =  0x00000011
    DALPROP_ATTR_TYPE_UINT32            =  0x00000002
    DALPROP_ATTR_TYPE_BYTE_SEQ          =  0x00000008
    DALPROP_ATTR_TYPE_BYTE_SEQ_PTR      =  0x00000018
    DALPROP_ATTR_TYPE_UINT32_SEQ_PTR    =  0x00000014
    DALPROP_ATTR_TYPE_STRUCT_PTR        =  0x00000012
    
    type_dict = { 0x00000011 : "DALPROP_ATTR_TYPE_STRING_PTR" ,  
                  0x00000002 : "DALPROP_ATTR_TYPE_UINT32", 
                  0x00000008 : "DALPROP_ATTR_TYPE_BYTE_SEQ",
                  0x00000018 : "DALPROP_ATTR_TYPE_BYTE_SEQ_PTR",
                  0x00000014 : "DALPROP_ATTR_TYPE_UINT32_SEQ_PTR",
                  0x00000012 : "DALPROP_ATTR_TYPE_STRUCT_PTR" }
                  
    s = struct.Struct('I'*3) #Type, Name | Id, value  
    def __init__(self, data,props_offset): 
        unpacked_data              = (Props.s).unpack(data)
        self.unpacked_data         = unpacked_data
        self.type                  = unpacked_data[0]
        self.name_offset           = unpacked_data[1]
        self.value_offset          = unpacked_data[2] #intialize it to the value field intially, but later it will point to the file offset where the property is stored
        self.props_offset          = props_offset 
        
        if self.type == Props.DALPROP_ATTR_TYPE_UINT32 or self.type == Props.DALPROP_ATTR_TYPE_STRUCT_PTR: 
           self.value              = self.value_offset 
        elif self.type == Props.DALPROP_ATTR_TYPE_BYTE_SEQ or self.type == Props.DALPROP_ATTR_TYPE_BYTE_SEQ_PTR or self.type == Props.DALPROP_ATTR_TYPE_UINT32_SEQ_PTR: 
           self.value              = []
        elif self.type == Props.DALPROP_ATTR_TYPE_STRING_PTR:   
           self.value              = ""
           
        self.name                  = ""
        
    def populate_props(self,elf_fp,dal_props_dir):
        prop_len_bytes = 12 # at minimum property will have type,name,value 3 words i.e. 12 bytes
        elf_fp.seek(dal_props_dir.dal_props_offset + dal_props_dir.name_section_offset + self.name_offset) #skip to where prop name is stored
        ch = elf_fp.read(1)
        while ('\x00' != ch):
            self.name += ch
            ch = elf_fp.read(1)              
        #read property value depending on its type    
        if self.type == Props.DALPROP_ATTR_TYPE_UINT32  or self.type == Props.DALPROP_ATTR_TYPE_STRUCT_PTR: #scalar uint32 prop or struct ptr - IGNORE(struct ptr will contain offset in struct ptr table)
           self.value = self.value_offset 
           self.value_offset = self.props_offset+8 #in this case the value is store 8 bytes from the props_offset
           
        elif self.type == Props.DALPROP_ATTR_TYPE_BYTE_SEQ: #inline byte sequence 
           len = self.populate_byte_seq_value(elf_fp,self.props_offset+8)#skip to where prop value is stored ( type,name,value) hence +8
           self.value_offset = self.props_offset+8 #in this case the value is store 8 bytes from the props_offset
           prop_len_bytes += (len-4) # -4 since we have the prop_len_bytes assigned to 12, assuming we will have 4 bytes used for inline byte sequence
           
        elif self.type == Props.DALPROP_ATTR_TYPE_BYTE_SEQ_PTR: # byte sequence pointer value will contain offset relative to byte sequence section in prop bin
           self.populate_byte_seq_value(elf_fp,dal_props_dir.dal_props_offset+dal_props_dir.byte_section_offset+self.value_offset)
           self.value_offset = dal_props_dir.dal_props_offset+dal_props_dir.byte_section_offset+self.value_offset #in this case the value is stored in byte sequence section
           
        elif self.type == Props.DALPROP_ATTR_TYPE_UINT32_SEQ_PTR: #uint32 sequence pointer value will contain offset relative to uint32 sequence section in prop bin  
           self.populate_uint32_seq_value(elf_fp,dal_props_dir.dal_props_offset+dal_props_dir.uint32_section_offset+self.value_offset)
           self.value_offset = dal_props_dir.dal_props_offset+dal_props_dir.uint32_section_offset+self.value_offset # value is stored in uint32 sequence section
           
        elif self.type == Props.DALPROP_ATTR_TYPE_STRING_PTR:   #string sequence pointer value will contain offset relative to sring section in prop bin
           self.populate_string_value(elf_fp,dal_props_dir.dal_props_offset+dal_props_dir.string_section_offset+self.value_offset)
           self.value_offset = dal_props_dir.dal_props_offset+dal_props_dir.string_section_offset+self.value_offset # value is stored in string sequence section
           
        return prop_len_bytes   
           
    def populate_uint32_seq_value(self,elf_fp,value_offset):
        elf_fp.seek(value_offset) 
        s = struct.Struct('I') #read the length of uint32 sequence this is a zero index length
        data = s.unpack(elf_fp.read(4)) #read 4 bytes for len
        len = data[0] + 1 #zero index length, hence +1
        s = struct.Struct('I'*len) 
        entry = s.unpack(elf_fp.read(4*len)) #read 8 bytes total
        for i in range(len):
           self.value.append(entry[i])
        return 4*(len+1) #we will return number of bytes used for this sequence, adjust for length stored upfront
                   
    def populate_byte_seq_value(self,elf_fp,value_offset):
        elf_fp.seek(value_offset) 
        s = struct.Struct('B') #read the length of byte sequence this is a zero index length
        data = s.unpack(elf_fp.read(1)) #read 1 bytes for len
        len = data[0] + 1 #zero index length, hence +1
        s = struct.Struct('B'*len) 
        entry = s.unpack(elf_fp.read(len)) #read len bytes total
        for i in range(len):
           self.value.append(entry[i])
        #we will return number of bytes used for this sequence
        len += 1 #adjust for the length element store upfront
        while(len%4): #pad till word aligned
            len+=1
        return len

    def populate_string_value(self,elf_fp,value_offset):
        elf_fp.seek(value_offset) 
        ch = elf_fp.read(1)
        while ('\x00' != ch):
            self.value += ch
            ch = elf_fp.read(1)     
            
    def modify_props(self,modified_value):
        self.value = modified_value 
        
    def write_modified_props_to_elf(self,elf_fp): 
        elf_fp.seek(self.value_offset) #seek to the location where property is stored
        
        if self.type == Props.DALPROP_ATTR_TYPE_STRUCT_PTR:
            print 'ERROR! Props ' + self.name +'cannot modify structure pointer '
            sys.exit()
       
        elif self.type == Props.DALPROP_ATTR_TYPE_UINT32:  #scalar uint32 prop 
           s = struct.Struct('I')
           data = s.pack(self.value)
           elf_fp.write(data)
                 
        elif self.type == Props.DALPROP_ATTR_TYPE_BYTE_SEQ_PTR or self.type == Props.DALPROP_ATTR_TYPE_BYTE_SEQ: #inline byte sequence 
           elf_fp.seek(1,1) #move over the len byte
           s = struct.Struct('B')
           data = ""
           for i in range(len(self.value)):
               data += s.pack(self.value[i])
           elf_fp.write(data)
           
        elif self.type == Props.DALPROP_ATTR_TYPE_UINT32_SEQ_PTR: 
           elf_fp.seek(4,1) #move over the len byte
           s = struct.Struct('I')
           data = ""
           for i in range(len(self.value)):
               data += s.pack(self.value[i])  
           elf_fp.write(data)         
        elif self.type == Props.DALPROP_ATTR_TYPE_STRING_PTR:  
           ascii = self.value.encode('ascii','ignore')
           for i in range(len(ascii)):
               elf_fp.write(ascii[i])         
           elf_fp.write('\0')    
           
           
    def print_props_xml(self):
        xml = '<props name="{0:}" type= "{1:}"> '.format(self.name,self.type_dict[self.type])
        if self.type == Props.DALPROP_ATTR_TYPE_UINT32 or self.type == Props.DALPROP_ATTR_TYPE_STRUCT_PTR:
            xml += '{0:#0x}'.format(self.value)
        elif self.type == Props.DALPROP_ATTR_TYPE_STRING_PTR:
            xml += '{0:}'.format(self.value)
        else:
            for i in range(len(self.value)):
                xml += ' {0:#0x},'.format(self.value[i])
            xml +=' end' 
        xml += ' </props>'    
        return xml

class Device:                      
    def __init__(self, id, dal_props_dir,string_device_dict): 
        self.id                  = id
        self.dal_props_dir       = dal_props_dir
        self.string_device_dict  = string_device_dict
        self.props_dict          = {}
        self.modified_props_dict = {} #this will contain the modified props for device
        
    def parse_device_props(self, elf_fp):
        if type(self.id) is str:
            #string device Id 
            device_prop_offset = self.dal_props_dir.dal_props_offset + self.string_device_dict[self.id].prop_offset
        else:
            #legacy 32 bit device
            device_prop_offset = self.dal_props_dir.dal_props_offset + self.dal_props_dir.device_offset[self.id]   
        while(1):
            elf_fp.seek(device_prop_offset) #skip to where props for the devices are stored
            s = struct.Struct('I') #read the length of uint32 sequence this is a zero index length
            prop_end_marker = s.unpack(elf_fp.read(4)) #read 4 bytes for len
            if prop_end_marker[0] == 0xff00ff00: #if prop end marker is found bail out
               return
            else:
               elf_fp.seek(-4,1) # restore the file pointer to the start of props   
            props  = Props(elf_fp.read(3*4),device_prop_offset)
            device_prop_offset += props.populate_props(elf_fp,self.dal_props_dir)
            self.props_dict[props.name] = props                   
    
    def modify_props(self,props_name,props):
        self.modified_props_dict[props_name] = props
        
    def write_all_modified_props_to_elf(self,elf_fp):
        for props_name in  self.modified_props_dict.keys():
            props = self.modified_props_dict[props_name]
            props.write_modified_props_to_elf(elf_fp)
            
    def get_device_xml(self):
        if type(self.id) is str:
           xml = '<device id = "{0:}">\n'.format(self.id) 
        else:
           xml = '<device id = "{0:#010x}">\n'.format(self.id)
        for keys in self.props_dict.keys():
           xml += '\t' + self.props_dict[keys].print_props_xml() + '\n'
        xml += '</device>\n'    
        return xml
        
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
def OPEN(file_name, mode):
    try:
        fp = open(file_name, mode)
    except IOError:
        raise RuntimeError, "The file could not be opened: " + file_name

    # File open has succeeded with the given mode, return the file object
    return fp
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
        
class ElfSymbols:
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

class Props_XML:
    DALPROP_ATTR_TYPE_STRING_PTR        =  0x00000011
    DALPROP_ATTR_TYPE_UINT32            =  0x00000002
    DALPROP_ATTR_TYPE_BYTE_SEQ          =  0x00000008
    DALPROP_ATTR_TYPE_BYTE_SEQ_PTR      =  0x00000018
    DALPROP_ATTR_TYPE_UINT32_SEQ_PTR    =  0x00000014
    DALPROP_ATTR_TYPE_STRUCT_PTR        =  0x00000012
    type_dict = { "DALPROP_ATTR_TYPE_STRING_PTR"    : 0x00000011,  
                  "DALPROP_ATTR_TYPE_UINT32"        : 0x00000002,
                  "DALPROP_ATTR_TYPE_BYTE_SEQ"      : 0x00000008,
                  "DALPROP_ATTR_TYPE_BYTE_SEQ_PTR"  : 0x00000018,
                  "DALPROP_ATTR_TYPE_UINT32_SEQ_PTR": 0x00000014,
                  "DALPROP_ATTR_TYPE_STRUCT_PTR"    : 0x00000012 }                      
    def __init__(self, name,type, value): 
        self.name                = name
        value = value.lstrip()
        value = value.rstrip()
        try :
            self.type            = Props_XML.type_dict[type]
            self.type_str        = type
        except KeyError:
            print 'Unknown prop type in modified XML file'
            sys.exit()       
        if self.type == Props_XML.DALPROP_ATTR_TYPE_UINT32 or self.type  == Props_XML.DALPROP_ATTR_TYPE_STRUCT_PTR:
            self.value = eval(value)
        elif self.type == Props_XML.DALPROP_ATTR_TYPE_STRING_PTR:        
            self.value = value
        elif self.type == Props_XML.DALPROP_ATTR_TYPE_BYTE_SEQ or self.type  == Props_XML.DALPROP_ATTR_TYPE_BYTE_SEQ_PTR or self.type == Props_XML.DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
            self.value = []
            _value = value.split(',')
            for index in range(len(_value) - 1):
                _value[index] = _value[index].lstrip()
                self.value.append(eval(_value[index]))
    def is_modified(self,Props):
        # compare name and type, if they dont match bail
        if self.name != Props.name:
           print 'ERROR: Props "'+self.name+'" name doesnt match with ELF file'
           return False
        if self.type != Props.type:
           print 'ERROR: Props "'+self.name+'" type = '+self.type+' doesnt match with ELF file'
           return False
        
        # we dont allow modification of C structure 
        if self.type  == Props_XML.DALPROP_ATTR_TYPE_STRUCT_PTR:
           if self.value != Props.value:
              print 'WARNING: Tool doesnt allow modification of structure pointers, check property "'+self.name+'"'
           return False
        elif self.type == Props_XML.DALPROP_ATTR_TYPE_UINT32:
           if self.value != Props.value:
              return True 
        elif self.type == Props_XML.DALPROP_ATTR_TYPE_STRING_PTR:
           if len(self.value) > len(Props.value):
              print 'Warning: Tool doesnt allow increasing length of strings check prop "'+self.name+'" '
              return False
           elif len(self.value) != len(Props.value): 
              return True
           else: # same length string check content  
              for i in range (len(self.value)):
                  if self.value[i] != Props.value[i]:
                     return True
                    
        # if type is a sequence or pointer we wont allow addition of extra elements, check for this   
        elif self.type == Props_XML.DALPROP_ATTR_TYPE_BYTE_SEQ or self.type  == Props_XML.DALPROP_ATTR_TYPE_BYTE_SEQ_PTR or self.type == Props_XML.DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:             
           if len (self.value) != len(Props.value):
              print 'Warning: Tool doesnt allow modification of sequence length check property "'+self.name+'"'
              return False
           else:
              for i in range (len(Props.value)):
                  if self.value[i] != Props.value[i]:
                     return True
              #if we reach here we are identical
              return False                               
        return False 
            
    def print_props_xml(self):
        xml = '<props name="{0:}" type= "{1:}"> '.format(self.name,self.type_str)
        if self.type == Props_XML.DALPROP_ATTR_TYPE_UINT32 or self.type == Props_XML.DALPROP_ATTR_TYPE_STRUCT_PTR or self.type == Props_XML.DALPROP_ATTR_TYPE_STRING_PTR:
            xml += '{0:}'.format(self.value)
        else:
            for i in range(len(self.value)):
                xml += ' {0:#0x},'.format(self.value[i])
            xml +=' end' 
        xml += ' </props>'    
        return xml            
         
        
class Device_XML:                      
    def __init__(self, id): 
        self.id                  = id
        self.props_dict          = {}
        self.num_props           = 0
        
    def add_xml_props(self, props_xml):
        self.props_dict[props_xml.name] = props_xml

    def get_device_xml(self):
        xml = '<device id = "{0:}"> \n'.format(self.id) 
        for keys in self.props_dict.keys():
           xml += '\t' + self.props_dict[keys].print_props_xml() + '\n'
        xml += '</device>'    
        return xml    
#----------------------------------------------------------------------------
#parse input xml file which contains the modified properties
#schema for input xml is as below
# <dal>
#    <device>
#      <props>
#      </props>
#       ...
#    </device>
#    ...
# </dal>      
#----------------------------------------------------------------------------
def parse_xml_file(xml_file_name):
    modified_device_dict = {}
    device_xml_list = xml.dom.minidom.parse(xml_file_name)
    for device in device_xml_list.getElementsByTagName("device"):
        device_xml = Device_XML(device.getAttribute("id"))
        for props in device.getElementsByTagName("props"):
            props_xml = Props_XML(props.getAttribute("name"),props.getAttribute("type"),props.firstChild.data)
            device_xml.add_xml_props(props_xml)   
        modified_device_dict[device_xml.id] = device_xml
    return modified_device_dict        
    
#----------------------------------------------------------------------------
#Determine if properties have been update, if so update the new values to ELF file    
#----------------------------------------------------------------------------
def modify_props(orig_device_dict, modified_device_dict,elf_fp):
    for device_id in modified_device_dict.keys():
        try: 
            modified_device = modified_device_dict[device_id]
            orig_device     = orig_device_dict[device_id]
            for props_name in modified_device.props_dict.keys():
                try :
                   orig_props     = orig_device.props_dict[props_name]
                   modified_props = modified_device.props_dict[props_name]                  
                   if modified_props.is_modified(orig_props):
                       #update modified props in the orig props
                       orig_props.modify_props(modified_props.value)
                       #add props in device modified props dict, this way it will be easier to modify only update props
                       orig_device.modify_props(props_name,orig_props)
                       print modified_props.print_props_xml()
                except KeyError:
                   print 'ERROR! Props name = '+props_name+ ' not found in Input ELF'                   
            #now we have the modified props (if any) stored in the orig_device, time to write it back to file     
            orig_device.write_all_modified_props_to_elf(elf_fp)           
        except KeyError:
               print 'ERROR! Device id '+device_id+' not found in Input ELF' 
               sys.exit()
#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an 
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def parse_elf_file(elf_fp, elf_file_name, chipsetid, platform):
    # Initialize
    shdr_table = []
    elf_fp.seek(0)
    elf_header = Elf32_Ehdr(elf_fp.read(Elf32_Ehdr.ELF_HDR_SIZE))

    # Verify ELF header information
    if verify_elf_header(elf_header) is False:
        raise RuntimeError, "ELF file failed verification: " + elf_file_name
    
    elf_fp.seek(elf_header.e_phoff)

    # Get section header details (size, file offset, number of headers)
    shdr_size = elf_header.e_shentsize
    shdr_file_offset = elf_header.e_shoff
    shdr_count = elf_header.e_shnum

    elf_fp.seek(shdr_file_offset)
    shdr_shstrtab_file_offset = -1
    for i in range(shdr_count):
        is_shstrtab = 0
        if i == elf_header.e_shstrndx:
            is_shstrtab = 1    
        data = Elf32_Shdr(elf_fp.read(shdr_size), i,is_shstrtab)
        shdr_table.append(data)
        if is_shstrtab == 1:
            shdr_shstrtab_file_offset = data.sh_offset
   
    devcfg_data_file_offset = 0   
    devcfg_data_vaddr       = 0    
    for i in range(shdr_count):
        elf_fp.seek(shdr_shstrtab_file_offset + shdr_table[i].sh_name)
        ch = elf_fp.read(1)
        while ('\x00' != ch):
            shdr_table[i].str_name += ch
            ch = elf_fp.read(1)
        if (shdr_table[i].str_name == ".DEVCFG_DATA") or (shdr_table[i].str_name == "DEVCFG_DATA") or ((shdr_table[i].str_name == "." + chipsetid + "_DEVCFG_DATA") and platform is "none") or (shdr_table[i].str_name.startswith( "." +  chipsetid + "_PLATFROM_" + platform.upper(), 0, 17)):
            devcfg_data_file_offset = shdr_table[i].sh_offset
            devcfg_data_vaddr       = shdr_table[i].sh_addr
            break;
    
    if devcfg_data_file_offset == 0:
       #maybe this is non merged elf - eg. RPM, BOOT
       print 'Non Merged ELF - Attempting DALPROP_PropsInfo lookup via ELF Symbols'
       elf_symbols = ElfSymbols(elf_fp, shdr_table)   
       devcfg_data_file_offset = elf_symbols.get_offset("DALPROP_PropsInfo")
       devcfg_data_vaddr = elf_symbols.get_vaddr("DALPROP_PropsInfo")        
       
    if devcfg_data_file_offset == 0 or devcfg_data_vaddr == 0:
       print 'devcfg_data_file_offset = {0:#010x}'.format(devcfg_data_file_offset)
       print 'devcfg_data_vaddr       = {0:#010x}'.format(devcfg_data_vaddr)
       print 'FATAL-ERROR! Couldn\'t determine devcfg_data_offset via DEVCFG_DATA section or ELF Symbol lookup'
       sys.exit() # couldnt find devcfg_data_offset either via DEVCFG_DATA section or via Symbol lookup 
       
    # seek to the DEVCFG_DATA offset and read the entire offset 
    elf_fp.seek(devcfg_data_file_offset)
    # DALPROP_PropsInfo struct has 4 32bit members hence 16 bytes are read
    props_info = DALPROP_PropsInfo(elf_fp.read(4*4),devcfg_data_vaddr)
    if options.verbose: 
      props_info.print_props_info()
      
    string_device_dict = {}
    device_dict = {}
    if options.verbose or options.print_props_dir: 
       print 'String Devices'     

    elf_fp.seek(devcfg_data_file_offset + props_info.prop_bin_offset)   
    #header is 24 bytes, hence read for 24 bytes
    dal_props_dir = DALPropsDir(elf_fp.read(6*4),devcfg_data_file_offset + props_info.prop_bin_offset)
   
    first_string_device_offset = 0
    # Read all string device entries
    for idx in range(props_info.num_devices):
       #All offsets in props_info are from the base of DEVCFG_DATA segment with each device taking up 24 bytes
       elf_fp.seek(devcfg_data_file_offset + props_info.string_device_offset + idx*24)
       # DALPROP_PropsInfo struct has 6 32bit members hence 24 bytes are read
       string_device = StringDevice(elf_fp.read(6*4),devcfg_data_vaddr)
       if idx == 0:
          first_string_device_offset = string_device.prop_offset
       string_device.populate_device_name_and_collision_entries(elf_fp, devcfg_data_file_offset)
       string_device_dict[string_device.device_name] = string_device
       device = Device(string_device.device_name,dal_props_dir,string_device_dict)
       device.parse_device_props(elf_fp)
       device_dict[device.id] = device
       if options.verbose or options.print_props_dir:             
          string_device.print_string_device()
          
    dal_props_dir.populate_dal_props_dir(elf_fp,first_string_device_offset)
    if options.verbose or options.print_props_dir: 
      dal_props_dir.print_dalprops_dir() 
    
    # Read all uint32 legacy device entries
    for idx in dal_props_dir.device_offset.keys():
        device_id = '{0:#010x}'.format(idx)
        device = Device(idx,dal_props_dir,string_device_dict)
        device.parse_device_props(elf_fp)
        device_dict[device_id] = device
    return device_dict

def write_props_to_xml_file(device_dict):    
    #open file in which to store read properties
    if options.output_xml != "none":
       output_xml_fp = OPEN(options.output_xml, "w") 
       #adding the root tag <dal>
       output_xml_fp.write('<dal>\n') 
       # finished parsing all properties. Check if the user wishes to read and display properties
       if options.device_id == "all":
           for keys in device_dict.keys():
               output_xml_fp.write(device_dict[keys].get_device_xml())
       elif options.device_id != "none":
           try: 
               for i in range(len(options.device_id)): 
                   output_xml_fp.write(device_dict[options.device_id[i]].get_device_xml())
           except KeyError:
               print 'ERROR! Device: "' + options.device_id[i] + '" doesnt exist in ELF'
       output_xml_fp.write('</dal>\n')        
       output_xml_fp.close()         
       
def modify_and_store_props(elf_file_name,elf_fp,device_dict):     
    # If user has supplied a modified xml file scan it and make necessary edits in the ELF           
    if options.edit_xml != "none":
       #save the input elf file as name.elf.orig
       shutil.copy2(elf_file_name, elf_file_name+'.orig')
       #parse the modified property xml file
       modified_device_dict = parse_xml_file(options.edit_xml)
       #modify the props in the orig file
       modify_props(device_dict,modified_device_dict,elf_fp)
    

def print_args_and_options():
   print "Input ELF file   : ",args
   print "Verbose          : ",options.verbose
   print "Print Props Dir  : ",options.print_props_dir
   print "Device Id        : ",options.device_id
   print "Edit XML         : ",options.edit_xml
   print "Output XML       : ",options.output_xml
   print "CHIPSET ID       : ",options.chipsetid
   print "PLATFORM TYPE    : ",options.platform
   
def main():
   global options, args
   parser = OptionParser(usage="usage: %prog [options] ELFfile", version= "%prog 1.0")
   parser.add_option("-v", "--verbose",     dest="verbose",         action="store_true", default=False,      help="print debug logs")
   parser.add_option("-p", "--printhdr",    dest="print_props_dir", action="store_true", default=False,      help="print props dir header/list of device Ids")
   parser.add_option("-d", "--device",      dest="device_id",       type="string",       default="none",     help="specific device id/name to retrieve/modify properties")
   parser.add_option("-e", "--edit",        dest="edit_xml",        type="string",       default="none",     help="Edited xml file containing modified properties")
   parser.add_option("-o", "--output",      dest="output_xml",      type="string",       default="none",     help="output xml file to place read properties")
   parser.add_option("-c", "--chipsetid",   dest="chipsetid",       type="string",       default="none",     help="specify the chipsetid")
   parser.add_option("--pt", "--platform",  dest="platform",        type="string",       default="none",     help="specify the platform type")
   options, args = parser.parse_args()
   
   if options.verbose: 
      print_args_and_options()
      
   if options.device_id != "none" and options.output_xml == "none" :
      #if user doesnt provide xml output file prompt for one 
      print 'ERROR: Must provide output xml file via -o or --output option to store read properties'
      sys.exit()   
   
   if options.chipsetid == "none" :
      #if user doesnt provide chipset id to look for property into right section
      print 'INFO : No Chipset Id Provided, searching properties into default section'   
   
   if options.platform != "none" and options.chipsetid == "none" :
      #if user doesnt provide chipset id to look for property into right section
      print 'ERROR : Must provide Chipset Id if platform type is specified'
      sys.exit()
      
   #if multiple device ids are passed then seperate them out as a list    
   if options.device_id != "none" and options.device_id != "all":
      options.device_id = options.device_id.split(',')   
      for i in range(len(options.device_id)):
          options.device_id[i].lstrip()
          options.device_id[i].rstrip()          
   try:     
      elf_file_name = args[0]
   except IndexError:
      print 'Input ELF file is mandatory, please provide input ELF file'
      sys.exit()       
   elf_fp = OPEN(elf_file_name, "rb+")
   device_dict = parse_elf_file(elf_fp, elf_file_name, options.chipsetid, options.platform)
   write_props_to_xml_file(device_dict)
   modify_and_store_props(elf_file_name,elf_fp,device_dict)
   elf_fp.close()
   
if __name__ == "__main__":
   main()