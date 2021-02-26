#!/usr/bin/python
#============================================================================
#
#/** @file createversionelf.py
#
# GENERAL DESCRIPTION
#   Creates a elf file that has a note segment with the version information
#   of the build
#
# Copyright 2015,2016 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#**/
#
#----------------------------------------------------------------------------
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 04/16/16   et        Appended path to import correct mbn_tools
# 03/12/15   jn        Add command line options
# 02/26/15   jn        Initial revision
#
#============================================================================
from optparse import OptionParser
import os
import sys
import shutil
from xml.etree import ElementTree as et
import struct
import socket

# Add path to mbn_tools
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                'sectools','sectools','features','isc','parsegen'))
import mbn_tools

PAGE_SIZE       = 4096
SEGMENT_ALIGN   = 16
ELF32_HDR_SIZE  = 52
ELF32_PHDR_SIZE = 32
ELF64_HDR_SIZE  = 64
ELF64_PHDR_SIZE = 56

NOTE_NAME = "Qualcomm Technologies, Incorporated"
NOTE_TYPE = 1

DEFAULT_VARIANT = "UNKNOWN"
DEFAULT_OEM_VER = "Engineering build"

##############################################################################
# GetXMLVersion
##############################################################################
def GetXMLVersion(filename):
  if filename == None:
    return {'name':None, 'revision':None}
  if not os.path.isfile(filename):
    return {'name':None, 'revision':None}

  # Get XML data
  XmlCfg = et.ElementTree(file=filename).getroot()

  XmlImageTree = XmlCfg.find('image_tree')

  if XmlImageTree is not None:
    XmlImageTreeName = XmlImageTree.find('name')
    XmlImageTreeRevision = XmlImageTree.find('revision')
  else:
    PrintError("")
    PrintError("-------------------------------------------------------------------------------")
    PrintError("*** The configuration XML file: %s is missing 'image_tree' tag" % os.path.split(source_full)[1])
    PrintError("-------------------------------------------------------------------------------")
    PrintError("")
    return {'name':None, 'revision':None}

  return {'name':XmlImageTreeName.text, 'revision':XmlImageTreeRevision.text}

##############################################################################
# main
##############################################################################
def main():
  parser = OptionParser(usage='usage: %prog [options] arguments')

  parser.add_option("-x", "--xml_manifest",
                    action="store", type="string", dest="xml_manifest",
                    help="Manifest of the current version")

  parser.add_option("-i", "--image_name",
                    action="store", type="string", dest="image_name",
                    help="Image Name")

  parser.add_option("-r", "--image_revision",
                    action="store", type="string", dest="image_revision",
                    help="Image Revision")

  parser.add_option("-m", "--min_manifest",
                    action="store", type="string", dest="min_manifest",
                    help="Manifest of the minimum version")

  parser.add_option("-n", "--min_revision",
                    action="store", type="string", dest="min_revision",
                    help="Minimum revision")

  parser.add_option("-t", "--type",
                    action="store", type="string", dest="type",
                    help="Image type")

  parser.add_option("-v", "--oem_version",
                    action="store", type="string", dest="oem_version",
                    help="OEM version")

  parser.add_option("-o", "--output_filepath",
                    action="store", type="string", dest="binary_out",
                    help="Merged filename and path.")

  parser.add_option("-a", "--arch",
                    action="store", type="string", dest="arch",
                    help="Output ELF file architecture.  '32' or '64'")


  (options, args) = parser.parse_args()

  if not options.binary_out:
    parser.error('Output filename not given')

  if not options.arch:
    parser.error('ELF architecture not given')

  if (not options.arch == '64') and (not options.arch == '32'):
    parser.error('Invalid ELF architecture given')

  gen_dict = {}

  current_version = GetXMLVersion(options.xml_manifest)
  min_version = GetXMLVersion(options.min_manifest)
  crm_buildid = os.environ.get('CRM_BUILDID')
  if (crm_buildid != None):
    crm_buildid = crm_buildid.split('-')
    if (len(crm_buildid) != 4):
      crm_buildid = None

  if (options.image_name != None):
    current_version['name'] = options.image_name
  if (current_version['name'] == None):
    if (crm_buildid != None):
      current_version['name'] = crm_buildid[0]

  if (options.min_revision != None):
    min_version['revision'] = options.min_revision
  if (min_version['revision'] == None):
    min_version['revision'] = '00000'
  min_version['num'] = int(min_version['revision'])

  if (options.image_revision != None):
    current_version['revision'] = options.image_revision
  if (current_version['revision'] == None):
    current_version['revision'] = os.environ.get('BLD_ENV_VER_NUM_COMPRESSED')
  if (current_version['revision'] == None):
    if (crm_buildid != None):
      current_version['revision'] = crm_buildid[1]
  if (current_version['revision'] == None):
    current_version['revision'] = min_version['revision']
  if (current_version['revision'] == None):
    current_version['revision'] = '00000'
  current_version['num'] = int(current_version['revision'])

  variant = options.type
  if (variant == None):
    variant = os.environ.get('BLD_ENV_BUILD_ID')
  if (variant == None):
    if (crm_buildid != None):
      variant = crm_buildid[2]
  if (variant == None):
    variant = DEFAULT_VARIANT

  oem_version = options.oem_version
  if (oem_version == None):
    oem_version = os.environ.get('SCONS_OEM_BUILD_VER')
  if (oem_version == None):
    oem_version = socket.gethostname()
  if (oem_version == None):
    oem_version = DEFAULT_OEM_VER

  if (current_version['name'] != None):
    current_version['string'] = "".join([current_version['name'].upper(), "-", current_version['revision'].upper()])
  else:
    current_version['string'] = current_version['revision'].upper()

  current_version['string'] += " " + variant + " " + oem_version

  if (min_version['num'] > current_version['num']):
    PrintError("Current version can not be less than min version")
    sys.exit(1)

  binary_out = options.binary_out

  if options.arch == '64':
    is_elf_64_bit = True
  else:
    is_elf_64_bit = False

  write_elf([],
            binary_out,
	          current_version,
            min_version,
            is_elf_64_bit)
  return 0


##############################################################################
# roundup
##############################################################################
def roundup(x, precision):
  return x if x % precision == 0 else (x + precision - (x % precision))

##############################################################################
# pad
##############################################################################
def pad(x, precision):
  return roundup(x, precision) - x

##############################################################################
# write_elf
##############################################################################
def write_elf(env,
              binary_out,
              current_version,
              min_version,
              is_elf_64_bit):

  if binary_out is not None:
    elf_out_fp = mbn_tools.OPEN(binary_out, "wb+")


  # Calculate the new program header size.  This is dependant on the output
  # ELF type and number of program headers going into output.
  if is_elf_64_bit:
    phdr_total_size = ELF64_PHDR_SIZE
    phdr_total_count = 1
  else:
    phdr_total_size = ELF32_PHDR_SIZE
    phdr_total_count = 1


  # Create a new ELF header for the output file
  if is_elf_64_bit:
    out_elf_header = mbn_tools.Elf64_Ehdr('\0' * ELF64_HDR_SIZE)
    out_elf_header.e_phoff     = ELF64_HDR_SIZE
    out_elf_header.e_ehsize    = ELF64_HDR_SIZE
    out_elf_header.e_phentsize = ELF64_PHDR_SIZE
    out_elf_header.e_machine   = 183
    out_elf_header.e_ident     = str('\x7f' + 'E' + 'L' + 'F' + \
                                 '\x02' + \
                                 '\x01' + \
                                 '\x01' + \
                                 '\x00' + \
			         '\x00' + \
                                 ('\x00' * 7))

    out_elf_header.e_entry     = 0
  else:
    out_elf_header = mbn_tools.Elf32_Ehdr('\0' * ELF32_HDR_SIZE)
    out_elf_header.e_phoff     = ELF32_HDR_SIZE
    out_elf_header.e_ehsize    = ELF32_HDR_SIZE
    out_elf_header.e_phentsize = ELF32_PHDR_SIZE
    out_elf_header.e_machine   = 40
    out_elf_header.e_entry       = 0
    out_elf_header.e_ident     = str('\x7f' + 'E' + 'L' + 'F' + \
                                 '\x01' + \
                                 '\x01' + \
                                 '\x01' + \
                                 '\x00' + \
  	                         '\x00' + \
                                 ('\x00' * 7))

  # Common header entries
  out_elf_header.e_type        = 0
  out_elf_header.e_version     = 1
  out_elf_header.e_shoff       = 0
  out_elf_header.e_flags       = 0
  out_elf_header.e_shentsize   = 0
  out_elf_header.e_shnum       = 0
  out_elf_header.e_shstrndx    = 0


  # Now it is ok to populate the ELF header and write it out
  out_elf_header.e_phnum = phdr_total_count

  # write elf header
  if is_elf_64_bit == False:
    elf_out_fp.write(mbn_tools.Elf32_Ehdr.getPackedData(out_elf_header))
  else:
    elf_out_fp.write(mbn_tools.Elf64_Ehdr.getPackedData(out_elf_header))

  phdr_offset = out_elf_header.e_phoff  # offset of where to put next phdr

  # offset the start of the segments just after the program headers
  segment_offset = roundup(out_elf_header.e_phoff + phdr_total_size, PAGE_SIZE)

  name_size = len(NOTE_NAME)
  version_string_size = (len(current_version['string'])+1)*2

  # Copy program header piece by piece to ensure possible conversion success
  if is_elf_64_bit == True:
    new_phdr = mbn_tools.Elf64_Phdr('\0' * ELF64_PHDR_SIZE)
    new_phdr.p_type   = 4 #Note segment
    new_phdr.p_offset = segment_offset
    new_phdr.p_vaddr  = 0
    new_phdr.p_paddr  = 0
    desc_size = 8+version_string_size + pad(version_string_size, 8)
    new_phdr.p_filesz = 24 + name_size + 1 + pad(name_size+1, 8) + desc_size
    new_phdr.p_memsz  = 0
    new_phdr.p_flags  = 0
    new_phdr.p_align  = 0
  else:
    new_phdr = mbn_tools.Elf32_Phdr('\0' * ELF32_PHDR_SIZE)
    new_phdr.p_type   = 4 #Note segment
    new_phdr.p_offset = segment_offset
    new_phdr.p_vaddr  = 0
    new_phdr.p_paddr  = 0
    desc_size = 8+version_string_size + pad(version_string_size, 4)
    new_phdr.p_filesz = 12 + name_size + 1 + pad(name_size + 1, 4) + desc_size
    new_phdr.p_memsz  = 0
    new_phdr.p_flags  = 0
    new_phdr.p_align  = 0

  # update output file location to next phdr location
  elf_out_fp.seek(phdr_offset)
  # increment phdr_offset to next location
  phdr_offset += out_elf_header.e_phentsize

  # output current phdr
  if is_elf_64_bit == False:
    elf_out_fp.write(mbn_tools.Elf32_Phdr.getPackedData(new_phdr))
  else:
    elf_out_fp.write(mbn_tools.Elf64_Phdr.getPackedData(new_phdr))

  elf_out_fp.seek(segment_offset)

  if is_elf_64_bit == True:
    elf_out_fp.write(struct.pack('QQQ',len(NOTE_NAME), desc_size, NOTE_TYPE))
    elf_out_fp.write(NOTE_NAME)
    elf_out_fp.write(struct.pack('c','\0'))
    for x in range(pad(name_size + 1, 8)):
      elf_out_fp.write(struct.pack('x'))
    elf_out_fp.write(struct.pack('II', current_version['num'], min_version['num']))
    elf_out_fp.write(current_version['string'].encode('utf-16-le'))
    elf_out_fp.write(struct.pack('H',0))
    for x in range(pad(version_string_size, 8)):
      elf_out_fp.write(struct.pack('x'))
  else:
    elf_out_fp.write(struct.pack('III', len(NOTE_NAME), desc_size, NOTE_TYPE))
    elf_out_fp.write(NOTE_NAME)
    elf_out_fp.write(struct.pack('c','\0'))
    for x in range(pad(name_size + 1, 4)):
      elf_out_fp.write(struct.pack('x'))
    elf_out_fp.write(struct.pack('II', current_version['num'], min_version['num']))
    elf_out_fp.write(struct.pack('H',0))
    elf_out_fp.write(current_version['string'].encode('utf-16-le'))
    for x in range(pad(version_string_size, 4)):
      elf_out_fp.write(struct.pack('x'))

  elf_out_fp.close()

  return 0


main()
