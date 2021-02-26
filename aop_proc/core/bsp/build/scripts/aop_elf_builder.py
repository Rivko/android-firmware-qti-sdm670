import os
import subprocess
import string
import re
import shutil
import struct
import sys

class Elf32_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIIIIIHHHHHH') 
   
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
# ELF Program Header Class 
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

class AopElfBuilder:
    valid_attributes = ('segment.paddr', 'segment.vaddr', 'entry_point')
    MI_PBT_FLAG_SEGMENT_TYPE_MASK         = 0x07000000
    MI_PBT_FLAG_SEGMENT_TYPE_SHIFT        = 0x18
    MI_PBT_HASH_SEGMENT                   = 0x2
    ELF_HDR_SIZE              = 52 
    LOAD_TYPE                 = 0x1

    def __init__ (self, env, input, output, attribute_list):
        self.input = input
        self.output = output
        self.attribute_list = attribute_list
        self.env = env

        for attr in self.attribute_list:
          if (attr[0] not in self.valid_attributes):
              raise NotImplementedError

    def MI_PBT_SEGMENT_TYPE_VALUE (self, x):
       return ( ((x) & self.MI_PBT_FLAG_SEGMENT_TYPE_MASK) >> self.MI_PBT_FLAG_SEGMENT_TYPE_SHIFT )

    def preprocess_elf_file (self, elf_file_name):
       # Initialize
       elf_fp = open (elf_file_name, 'rb')
       elf_header = Elf32_Ehdr (elf_fp.read (self.ELF_HDR_SIZE))
       phdr_table = []

       # Verify ELF header information
       #if verify_elf_header(elf_header) is False:
       #   raise RuntimeError, "ELF file failed verification: " + elf_file_name

       # Get program header size
       phdr_size = elf_header.e_phentsize

       # Find the program header offset 
       file_offset = elf_header.e_phoff
       elf_fp.seek(file_offset) 
   
       # Read in the program headers
       for i in range(elf_header.e_phnum):
          phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size))) 

       file_offset = elf_header.e_shoff
       elf_fp.seek (file_offset)

       elf_fp.close()
       return [elf_header, phdr_table]

    def generate (self):
        ifo = open (self.input, "rb")
        ofo = open (self.output, "wb+")
        [elf_header, phdr_table] = self.preprocess_elf_file (self.input)
        shutil.copyfileobj (ifo, ofo, os.path.getsize (self.input))

        for attr in self.attribute_list:
          if 'entry_point' == attr[0]:
            val = 0
            val_str = attr[1]
            old_val = elf_header.e_entry
            if (val_str[0] == "+"):
               offset = int (val_str[1:], 0)
               if (old_val < (0xFFFFFFFF - offset)):
                  val = old_val + offset
               else:
                  print "Entry point:", "orig:", hex (old_val), "cannot apply offset:", hex(offset)
                  val = old_val
            elif (val_str[0] == "-"):
                val = old_val - int (val_str[1:], 0)
            else:
                val = int (val_str, 0)
            elf_header.e_entry = val
            ofo.seek(0)
            ofo.write (elf_header.getPackedData ())

        ofo.seek (elf_header.e_phoff)

        for seg in range (elf_header.e_phnum):
            curr_phdr = phdr_table[seg]
            # Only encrypt segments of LOAD_TYPE. Do not encrypt the hash segment. 
            if (curr_phdr.p_type == self.LOAD_TYPE) and \
                    (self.MI_PBT_SEGMENT_TYPE_VALUE(curr_phdr.p_flags) != self.MI_PBT_HASH_SEGMENT):

                for attr in self.attribute_list:
                  if not attr[0].startswith('segment'):
                    continue

                  #print "adjusting", attr[0], "range", attr[2]
                  segments = [int (i) for i in string.split (attr[2], ",")]
                  if not seg in segments:
                    continue

                  val = 0
                  val_str = attr[1]
                  old_val = curr_phdr.p_paddr
                  if (attr[0] == 'segment.vaddr'):
                    old_val = curr_phdr.p_vaddr

                  if (val_str[0] == "+"):
                     offset = int (val_str[1:], 0)
                     if (old_val < (0xFFFFFFFF - offset)):
                        val = old_val + offset
                     else:
                        print "segment:", seg, "orig:", hex (old_val), "cannot apply offset:", hex(offset)
                        val = old_val
                  elif (val_str[0] == "-"):
                      val = old_val - int (val_str[1:], 0)
                  else:
                      val = int (val_str, 0)
                  
                  if (attr[0] == 'segment.paddr'):
                    curr_phdr.p_paddr = val
                    print "segment paddr:", seg, "orig:", hex (old_val), "new:", hex (val)
                  elif (attr[0] == 'segment.vaddr'):
                    curr_phdr.p_vaddr = val
                    print "segment vaddr:", seg, "orig:", hex (old_val), "new:", hex (val)


            ofo.write (curr_phdr.getPackedData ())

        ifo.close ()
        ofo.close ()

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect ('aop_elf_builder')

def generate(env):
   aop_elf_generate (env)

def aop_elf_generate (env):
   #-------------------------------------------------------------------------------
   # def builder for PBN
   #
   print "RJ: ELF_GENERATE"

   elf_bld = env.Builder(action = env.GetBuilderAction (aop_elf_builder),
                         suffix = '.elf')

   env.Append(BUILDERS = {'AopElfBuilder' : elf_bld})

def aop_elf_emitter (target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)
   return (target, source)

def aop_elf_builder (target, source, env):
   # init and fix variebles
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))

   print "RJ: ", source_str, target_str
   if env['MSM_ID'] in ['8996']:
      re = AopElfBuilder (env, source_str, target_str, ('segment.paddr', '+0x00200000', '0,1,2,3'))
   elif env['MSM_ID'] in ['8998']:
      re = AopElfBuilder (env, source_str, target_str, [('segment.paddr', '+0x00200000', '0,1,2,3'), ('segment.vaddr', '+0x00200000', '0,1,2,3'), ('entry_point', '+0x00200000', '')])
   elif env['MSM_ID'] in ['845', '24', '670', '640']:  #SDM640_CHANGES
      re = AopElfBuilder (env, source_str, target_str, [
            ('segment.paddr', '+0x0B000000', '0,1'), 
            ('segment.vaddr', '+0x0B000000', '0,1'), 
            #('segment.paddr', '-0x60000000', '2'), 
            #('segment.vaddr', '-0x60000000', '2'), 
            ('entry_point', '+0x0B000000', '')
            ])
   elif env['MSM_ID'] in ['855']:        # For standalone, until we have DDR
      re = AopElfBuilder (env, source_str, target_str, [
            ('segment.paddr', '+0x0B000000', '0,1'), 
            ('segment.vaddr', '+0x0B000000', '0,1'), 
            #('segment.paddr', '-0x60000000', '2'), 
            #('segment.vaddr', '-0x60000000', '2'), 
            ('entry_point', '+0x0B000000', '')
            ])
   elif env['MSM_ID'] in ['1000']:        # For standalone, until we have DDR
      re = AopElfBuilder (env, source_str, target_str, [
            ('segment.paddr', '+0x0B000000', '0,1'), 
            ('segment.vaddr', '+0x0B000000', '0,1'), 
            #('segment.paddr', '-0x60000000', '2'), 
            #('segment.vaddr', '-0x60000000', '2'), 
            ('entry_point', '+0x0B000000', '')
            ])
   else:
      re = AopElfBuilder (env, source_str, target_str, ('segment.paddr', '+0xfc000000', '0,1,2,3'))
   re.generate ()

