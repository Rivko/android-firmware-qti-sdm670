'''
=============================================================================

                  Q S H R I N K E L F . P Y

Copyright (c) 2010 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Elf modification script for debug string optimization.
=============================================================================

=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order
$Header: //components/rel/dspcore.adsp/4.0/qshrink/src/QShrinkElf.py#1 $
$Date: 2017/03/30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13/03/13   sh      Added Qshrink 3.0 support
10/07/11   sh      Updates Kernel vend address from linker generated end SYMBOL
09/26/11   sh      Added Contiguous QSR_STRING section support
07/21/11   sh      QShrink20 commercial version release
03/13/11   sh      added anomalous string handling, removed intermediate ptr,
                   Elf shrink, output mem file list
07/21/10   jw/kf   Initial Version

=============================================================================
'''

import hashlib
import re
import time
import string
import struct
import csv
import os
import shutil

if __debug__:
  from QElfAnomalies import analyzeAnomalies
  #seems to happen whether __debug__ or not

# ======================== start globals ========================

const = {
  # Constants as defined in the ELF specification
  # Nowhere near complete but sufficient for this program.

  # ELF Sizes
  'Elf32_Addr'    : 4,
  'Elf32_Half'    : 2,
  'Elf32_Off'     : 4,
  'Elf32_Sword'   : 4,
  'Elf32_Word'    : 4,
  # ELF Type
  'ET_NONE'       : 0,
  'ET_REL'        : 1,
  'ET_EXEC'       : 2,
  'ET_DYN'        : 3,
  'ET_CORE'       : 4,
  'ET_LOPROC'     : 0xff00, # lower processor-specific range
  'ET_HIPROC'     : 0xffff, # higher processor-specific range
  # ELF Machine
  'EM_NONE'       : 0, # Error?
  'EM_M32'        : 1,
  'EM_SPARC'      : 2,
  'EM_386'        : 3,
  'EM_68K'        : 4,
  'EM_88K'        : 5,
  'EM_860'        : 7,
  'EM_MIPS'       : 8,
  'EM_MIPS_RS4_BE': 10,
  'EM_ARM'        : 40, # Only ones that this script handles
  # ELF Version
  'EV_NONE'       : 0, # Error
  'EV_CURRENT'    : 1, # Good
  # ELF Identifier Index
  'EI_MAG0'       : 0,
  'EI_MAG1'       : 1,
  'EI_MAG2'       : 2,
  'EI_MAG3'       : 3,
  'EI_CLASS'      : 4, # File Class (32/64-bit)
  'EI_DATA'       : 5, # Data Encoding (Little/Big-endian)
  'EI_VERSION'    : 6, # Error/Current
  'EI_PAD'        : 7, # Start of padding.  Should be 0 for 8 bytes.
  'EI_NIDENT'     : 16, # Size of ELF Identifier
  # Magic Numbers.  Identify ELF files
  'ELFMAG0'       : 0x7f,
  'ELFMAG1'       : ord("E"),
  'ELFMAG2'       : ord("L"),
  'ELFMAG3'       : ord("F"),
  # 32-bit or 64-bit
  'ELFCLASSNONE'  : 0, # Error
  'ELFCLASS32'    : 1, # 32-bit
  'ELFCLASS64'    : 2, # 64-bit
  # Endian-ness
  'ELFDATANONE'   : 0, # Error
  'ELFDATA2LSB'   : 1, # Little-endian
  'ELFDATA2MSB'   : 2, # Big-endian
  # Elf Header Offset
  'ELFH_PHOFF_OFFSET'     : 28, # e_phOff
  'ELFH_SHOFF_OFFSET'     : 32, # e_shOff
  'ELFH_PHNUM_OFFSET'     : 44, # e_phNum
  'ELFH_SHNUM_OFFSET'     : 48, # e_phNum
  'ELFH_SHSTRINDEX_OFFSET'     : 50, # e_phNum
  # Section Header Offset
  'SECTIONH_OFFSET'     : 16, #sh_off
  'SECTIONH_SIZE'       : 20, #sh_size
  'SECTIONH_LINK'       : 24, #sh_link
  # Section Header Entry
  'SHN_UNDEF'     : 0, # should ignore
  'SHN_LORESERVE' : 0xff00, # lower reserved range
  'SHN_HIRESERVE' : 0xffff, # higher reserved range
  'SHN_LOPROC'    : 0xff00, # lower processor specific range
  'SHN_HIPROC'    : 0xff1f, # higher processor specific range
  'SHN_ABS'       : 0xfff1, # if symbol is relative to, use fixed address
  'SHN_COMMON'    : 0xfff2, # if symbol is relative to,treat as common symbol
  # Section Header Type
  'SHT_NULL'      : 0, # no section data
  'SHT_PROGBITS'  : 1, # holds data defined by program
  'SHT_SYMTAB'    : 2, # holds symbol table (same as SHT_DYNSYM ?)
  'SHT_STRTAB'    : 3, # holds string table
  'SHT_RELA'      : 4, # holds relocation entries with explicit addends
  'SHT_HASH'      : 5, # holds symbol hash table
  'SHT_DYNAMIC'   : 6, # holds dynamic linking information
  'SHT_NOTE'      : 7, # holds information on the file (labels?)
  'SHT_NOBITS'    : 8, # does not go into file. sh_offset not real
  'SHT_REL'       : 9, # holds relocation entries without explicit addends
  'SHT_SHLIB'     : 10, # reserved
  'SHT_DYNSYM'    : 11, # holds a symbol table (same as SHT_SYMTAB ?)
  'SHT_LOPROC'    : 0x70000000, # lower processor specific semantics range
  'SHT_HIPROC'    : 0x7fffffff, # higher processor specific semantics range
  'SHT_LOUSER'    : 0x80000000, # lower application reserve range
  'SHT_HIUSER'    : 0xffffffff, # higher application reserve range
  # Section Header Flags
  'SHF_WRITE'     : 0x1, # section is writable during execution
  'SHF_ALLOC'     : 0x2, # occupies memory during process execution
  'SHF_EXECINSTR' : 0x4, # executable machine instructions
  'SHF_MASKPROC'  : 0xf0000000 # processor-specific flags (just a mask)
}

header = {
# global ELF header stat.  Set in initializeWithElfHeader 
  'endian'            : 0, # 1 = little endian  :: 2 = big endian
  'headerSize'        : 0, # size of the ELF Header. (always 52?)
  'programHeaderOff'  : 0, # start of program header table
  'programHeaderSize' : 0, # size of each program header entry
  'programHeaderNum'  : 0, # number of program header entries in table
  'sectionHeaderOff'  : 0, # start of section header table
  'sectionHeaderSize' : 0, # size of each section header entry
  'sectionHeaderNum'  : 0, # number of section header entries in table
  'entry'             : 0, # entry address into executable image
  'flags'             : 0,
  'sectionNameTable'  : 0  # Section Header String Table Index
}

class ElfShrinkStateClass:
  '''
  Used to hold most variables for ElfShrinkState.
  '''


REPLACEMENT_MODE = 1
#discovery or replacement mode?

# @TODO Eventually need to make these below dynamically grabbed, somehow.

MSG_V2_STRUCT_SIZE = 0
# size of the xx_msg_v2_const struct
MSG_V3_STRUCT_SIZE = 0
# size of the xx_msg_v3_const struct

MSG_V2_ERR_STRUCT_SIZE = 0
# size of the xx_err_msg_v2_const struct
MSG_V3_ERR_STRUCT_SIZE = 0
# size of the xx_err_msg_v3_const struct

QSR_POINTER_OFFSET = 8

MSG_V3_QSR_POINTER_OFFSET = 4
# offset of the struct's const char* const* pointer
MSG_V3_SS_MASK_OFFSET = 0
# offset of the struct's ss_mask pointer

MSG_V3_SS_INDEX_OFFSET = 0
# offset of the struct's ss_index pointer
MSG_V3_LINE_OFFSET = 2
# offset of the struct's line offset

QSR_STRING_START = 0
# Virtual address of the start of the QSR string section

QSR_STRING_SIZE = 0
# Size of the QSR string section

QSR_STRING_END = 0
# Virtual address of the end of the QSR string section

QSR_STRING_OFFSET = 0
# Elf Offset of QSR_STRING section

QSR_DESC_ENTRY_SIZE = 8
# Size of Description table entry


QSR_STRING_START_OUTPUT = 0
# Virtual address of the start of the QSR string section in the output ELF
# This can be different from QSR_STRING_START if QSR_STRING section is not contiguous to the previous section


# ======================== end globals ========================

#=========================================================================
# Function : checkEIDENT 
#
# Arguments
#  header: (str) Bytes of ELF header
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks the e_ident field for global state that needs to be set.  May exit
#  if flags show an unsupported type.
#
# Return Values:
#  None
#=========================================================================
def checkEIDENT(header, e_ident):
  # formatting print to separate runQshrink info 
  if __debug__:
    print ""
  checkEidentELF(e_ident)
  checkEidentClass(e_ident)
  getEidentData(header, e_ident)
  checkEidentVersion(e_ident)


#=========================================================================
# Function : checkEidentELF 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks to see if the ELF file is actually an ELF file.  Will exit if we are
#  not working with an ELF file.
#
# Return Values:
#  None
#=========================================================================
def checkEidentELF(e_ident):
  if e_ident[0:4] != "".join(['\x7f', "ELF"]) :
    print "NOT AN ELF FILE!"
    exit(1)

#=========================================================================
# Function : checkEidentClass 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks if ELF is 32-bit.  Exits if it is NONE, UNKNOWN, or 64-bit.
#
# Return Values:
#  None
#=========================================================================
def checkEidentClass(e_ident):
  num = ord(e_ident[const['EI_CLASS']])
  if num != const['ELFCLASS32'] :
    print "NEEDS TO BE ELF-32"
    if    num == const['ELFCLASSNONE'] : print "CURRENT: INVALID"
    elif  num == const['ELFCLASS64']   : print "CURRENT: ELF-64"
    else: print "UNKNOWN EI_CLASS:", str(num)
    exit()

#=========================================================================
# Function : getEidentData 
#
# Arguments
#  header: (str) Bytes of the ELF header
#  e_ident: (str) Bytes of the e_ident field.
#
# Description
#    Sets the global variable for endianness.  Modifies behavior of the
#   getWord(str) family of functions.
#
# Return Values:
#  None
#=========================================================================
def getEidentData(header, e_ident):
  num = ord(e_ident[const['EI_DATA']])
  if 1 <= num <= 2:
    if __debug__:
      if    num == 1 : print "LITTLE ENDIAN"
      elif  num == 2 : print "BIG ENDIAN"
    header['endian'] = num
    return
  print "INVALID EI_DATA:", str(num)
  exit()

#=========================================================================
# Function : checkEidentVersion 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field.
#
# Description
#   Checks the ELF file version.  Exits on anything other than a Current one.
#
# Return Values:
#  None
#=========================================================================
def checkEidentVersion(e_ident):
  num = ord(e_ident[const['EI_VERSION']])
  if num == 1:
    return
  elif num == 0:  print "EIDENT VERSION: INVALID"
  else :          print "EIDENT VERSION: UNKNOWN", str(num)
  exit()

#=========================================================================
# Function : getPrettyBinary 
#
# Arguments
#  string: (str) String of bytes
#
# Description
#   Only prints ASCII characters and a '.' for all non-ascii characters.  May 
#  become confusing if there are real '.'s in your strings.
#  Used only for debugging and when you don't want to use fromelf.
#
# Return Values:
#  None
#=========================================================================
def getPrettyBinary(string):
  return ''.join([(x if (32 <= ord(x) <= 126) else '.') for x in string])

#=========================================================================
# Function : getSectionFromVA 
#
# Arguments
#  virtual_Address: used to look up the appropriate section 
#
# Description
#  Looks up the section that a given VA resides in.
#
# Return Values:
#  a section header object
#=========================================================================
def getSectionFromVA(virtual_address):
  dbg_match = re.compile(
      r'(\.debug)|(\.symtab)|(\.strtab)|(\.note)|(\.comment)|(\.shstrtab)')
  for shd in ElfShrinkState.sectionHeaderDict.values():
    if not dbg_match.search(shd.name):
      if(virtual_address >= shd.data['sh_addr'] and
         virtual_address < shd.data['sh_addr'] + shd.data['sh_size']):
        return shd
  print "getSectionFromVA(", hex(virtual_address), "):", hex(virtual_address), "is not in a section" 
  return None

#=========================================================================
# Function : initializeArmElfShrinkState 
#
# Arguments
#  elfFile, outputFile, hashFile,
#  sclFile, logFile, anomalyListFile
#
# Description
#  Initialize the global ElfShrinkState
#
# Return Values:
#  None
#=========================================================================
def initializeArmElfShrinkState(elfFile, outputFile, hashFile,
                                sclFile, logFile, anomalyListFile):
  global ElfShrinkState
  ElfShrinkState = ElfShrinkStateClass()
  #used to hold variables shared between the processing methods

  ElfShrinkState.sectionNameHeader = None
  # Header of the section with the names of the other sections

  ElfShrinkState.sectionNameData = None
  # Data of the section with the names of the other sections.

  ElfShrinkState.path = elfFile
  # ELF file path

  ElfShrinkState.bytesDict = dict()
  #'name' -> bytes, for section caching

  ElfShrinkState.memSizeFnameSizeDict = dict()
  #'filename' -> bytesSaved, total debug ROM per file

  ElfShrinkState.memSizePtrFnameDict = dict()
  #'strPointer' -> filename, for counting the bytes saved per file

  ElfShrinkState.sectionHeaderDict = {}
  # index | name => section header

  ElfShrinkState.strAnomalyDict = {}
  # anomaly_VA_Addr => anomaly 

  ElfShrinkState.strMovedStrDict = {}
  # strPointer => moved string Address

  ElfShrinkState.stringHashDict = {}
  # msg string => big-endian hash

  ElfShrinkState.stringPtrDict = {}
  # msg string => moved pointers

  ElfShrinkState.hashStringDict = {}

  ElfShrinkState.anomalyStructCount = 0

  ElfShrinkState.path = elfFile
  ElfShrinkState.outputPath = outputFile
  ElfShrinkState.hashPath = hashFile
  ElfShrinkState.sclPath = sclFile
  ElfShrinkState.logPath = logFile
  
  ElfShrinkState.image_vend_offset = 0
  ElfShrinkState.linker_endAddr = 0
  # Linker generated "end" address used by Kernel needs to be modified for 9K targets, if present

  ElfShrinkState.diagDescTblEntry = 0
  ElfShrinkState.diagDescTbl = 0
  # DIAG description table entry offsets,  msg_v2_ss_num_table_entries, msg_v2_ss_table

  ElfShrinkState.anomalyListPath = anomalyListFile

#=========================================================================
# Function : runQShrink 
#
# Arguments
#  elfFile: (str) ELF file to read in
#  outputFile: (str) ELF file to outputLog to
#  hashFile: (str) Master Hash file.  hash:filename:string format
#  sclFile: (str) SCL file to modify
#  mapFile: (str) Map file used for memLog
#  logFile: (str) Holds the log of changes.  name:index:address:bytes
#  anomalyListFile: (str) Lists invalid qsr structs found.
#  replace_only_mode: 
#  1 - only replace strings already hashed
#  0 - discover new strings and replace the strings in Elf
#  2 - only discover new strings, do not update Elf file.
#  memLog: Output the memory savings per file based on Map file.
#                    Can be used for memsize utility
#
# Description
#    Main entry function .Runs the ELF editor.  Gets its parameters from
#   Qshrink.py which parses command-line options.
#
# Return Values:
#  None
#=========================================================================
def runQShrink(elfFile, outputFile, hashFile, sclFile, mapFile,
               logFile, anomalyListFile, replace_only_mode, memLog):

  # keep track of total time
  start = time.clock()

  import datetime #can't be at top of file

  timeString = str(datetime.datetime.now())
  timeString = re.sub(r' ', r'_', timeString)
  timeString = re.sub(r'\..*', r'', timeString)
  timeString = re.sub(r':', r'.', timeString)

  memSizeFile = logFile + "_" + "mem_size.csv"
  logFile = logFile + "_" + ".log"
  reportFile = anomalyListFile + "_report_" + ".txt"
  anomalyListFile = anomalyListFile + "_" + ".csv"
  
  #logFile = logFile + ".log"
  #memSizeFile = memSizeFile  + ".csv"
  #reportFile = anomalyListFile + ".txt"
  #anomalyListFile = anomalyListFile + ".csv"


  datetime = 0

  resultStrs = [ \
    " ".join(["Starting Qshrink"]),
    " ".join(["ELF File:", elfFile]),
    " ".join(["Output File:", outputFile]),
    " ".join(["Hash File:", hashFile]),
    " ".join(["Log File:", logFile])
  ]
  if replace_only_mode == 1:
    resultStrs.append("Replacement Mode")
  elif replace_only_mode == 0:
    resultStrs.append("Discover and Replace Mode")
  elif replace_only_mode == 2:
    resultStrs.append("Discover Only Mode")

  try:
    outputLog = open(logFile, 'w')
  except IOError:
    print "Error: Could not open Output log, ",logFile
    exit(1)
  else:
    for strn in resultStrs:
      print(strn)
      outputLog.write(strn)
      outputLog.write("\n")
    outputLog.close()

  if __debug__:
    print "Anomaly List File:", anomalyListFile
    print "Report File:", reportFile

  if memLog == 1:
    print "MemLog File:",memSizeFile
    print "Map File  :",mapFile

  if (replace_only_mode != 2):
    # copy if output is different than input.
    # if input == output, then the original copy will be lost
    if elfFile != outputFile :
      copyStart = time.clock()
      if __debug__:
        print "Copy Start"
      shutil.copyfile(elfFile, outputFile)
      copyTime = time.clock() - copyStart
      if __debug__:
        print "Finished Copying (", copyTime, "seconds)"
      try:
        outputLog = open(logFile, 'a')
      except IOError:
        print "Error: Could not open Output log, ",logFile
        exit(1)
      else:
        outputLog.write(" ".join(["Finished Copying (", str(copyTime),
                                "seconds)\n"]))
        outputLog.close()

  #global to count up anomalies that occur, no matter what the mode
  global ElfShrinkState

  initializeArmElfShrinkState(elfFile, outputFile, hashFile,
                              sclFile, logFile, anomalyListFile )

  global REPLACEMENT_MODE
  REPLACEMENT_MODE = replace_only_mode

  if memLog==1:
    if loadMapFileForMemLog(mapFile) == 0:
      memLog = 0;

  ElfShrinkState.errCount = 0
  # summary counter for number of invalid symbol matches

  ElfShrinkState.matches = 0
  # summary counter for number of symbols which matched

  ElfShrinkState.newHashedStrings = 0
  
  ElfShrinkState.version = 0
  # QSHrink version

  #counters
  ElfShrinkState.hashified = 0
  ElfShrinkState.moved = 0
  ElfShrinkState.uniqueStructMatches = 0

  ElfShrinkState.visitedAnomalies = {}

  ElfShrinkState.outputLog = open(ElfShrinkState.logPath, 'a')
  if REPLACEMENT_MODE == 1:
    ElfShrinkState.outputHash = open(ElfShrinkState.hashPath, 'r')
  else:
    ElfShrinkState.outputHash = open(ElfShrinkState.hashPath, 'a')
        

  #start ElfShrink      
  initializeWithElfHeader()
  loadMasterHash(ElfShrinkState.hashPath)

  ElfShrinkState.bytesDict['QSR_STRING'] = getSectionData('QSR_STRING')

  if (REPLACEMENT_MODE == 2):
    # discover_only mode
    print "Discover_only option is supported for post-QSHrink elf file"
    updateMasterHashfromSection()
    memLog = 0
  else:
    replaceQSRSymbolStructs()
    if __debug__:
      analyzeAnomalies(ElfShrinkState.anomalyListPath, reportFile, ElfShrinkState.errCount)
   
  resultStrs = [ \
    " ".join(["Total Unique Struct Matches:",
              str(ElfShrinkState.uniqueStructMatches)]),
    " ".join(["Total New Hashed Strings:",
              str(ElfShrinkState.newHashedStrings)]),
    " ".join(["Total Strings Replaced by Hashes:",
              str(ElfShrinkState.hashified)]),
    " ".join(["Total Moved Strings in QSR_STR section:",
              str(ElfShrinkState.moved)]),
    " ".join(["Total Anomalies processed :",
              str(ElfShrinkState.anomalyStructCount)]),
    " ".join(["Total Strings in Master Hash File:",
              str(len(ElfShrinkState.stringHashDict))]),
    ]

  for strn in resultStrs:
    if __debug__:
      print(strn)
    ElfShrinkState.outputLog.write(strn)
    ElfShrinkState.outputLog.write("\n")

  ElfShrinkState.outputHash.close()
  ElfShrinkState.outputLog.close()

  if ElfShrinkState.errCount > 0:
    os.remove(ElfShrinkState.outputPath)

  if memLog == 1:
    try:
      memSizeFile = open(memSizeFile, 'w')
    except IOError:
      print "Could not open memSize file,", memSizeFile
      exit(1)
    else:
      iitem={}
      csv.register_dialect('ours',
                         delimiter=',',
                         quoting=csv.QUOTE_MINIMAL,
                         lineterminator='\n')

      memSizeListLabels = ["FileName", "RO_Data_Debug_String"]

      memSizeListCSV = csv.DictWriter(memSizeFile,
                                    memSizeListLabels,
                                    dialect='ours')

      # invalidListCSV.writeheader() #need python 2.7,fix below
      headers = dict((n, n) for n in memSizeListLabels)
      memSizeListCSV.writerow(headers)
      for (iitem['FileName'], iitem['RO_Data_Debug_String']) in ElfShrinkState.memSizeFnameSizeDict.items():
        memSizeListCSV.writerow(iitem)
      iitem['FileName'] = 'Total ROM reduced (includes padding)'
      iitem['RO_Data_Debug_String'] = QSR_STRING_SIZE - ElfShrinkState.newQSR_STRINGlen
      memSizeListCSV.writerow(iitem)
      memSizeFile.close()

  if ElfShrinkState.errCount > 0:
    resultStrs = [ \
      " ".join(["Total Time:", str(time.clock() - start)]),
      " ".join(["Qshrink completed with errors"]),
      " ".join(["Total Errors Found:", str(ElfShrinkState.errCount),
              "[Investigate errors in logFile]"])
      ]
  else:
    resultStrs = [ \
      " ".join(["Qshrink Version :",
              str(ElfShrinkState.version)]),
      " ".join(["Total Time:", str(time.clock() - start)]),
      " ".join(["Qshrink completed "]),
      ]


  try:
    outputLog = open(ElfShrinkState.logPath, 'a')
  except IOError:
    print "Could not open Log File, ElfShrinkState.logPath"
    exit(1)
  else:
    for strn in resultStrs:
      print(strn)
      outputLog.write(strn)
      outputLog.write("\n")
    outputLog.close()

#=========================================================================
# Function : investigateString 
#
# Arguments
#  strPointer: VA of string
#  section header for string's section 
#
# Description
#    Retrieve the string from the string pointer and section.
#
# Return Values:
#  the string
#=========================================================================
def investigateString(strPointer, strSection):

  global ElfSrhinkState
  
  # check for a non-existant encapsulating section
  if strSection is None :
    print "investigateString(", ", ".join([hex(strPointer), str(strSection)]), "):", hex(strPointer), "is not in a section"
    return None

  if strSection.name not in ElfShrinkState.bytesDict:
    ElfShrinkState.bytesDict[strSection.name] = getSectionData(strSection.name)

  strOffset = strPointer - strSection.data['sh_addr']

  string = getString(ElfShrinkState.bytesDict[strSection.name], strOffset)

  return string

#=========================================================================
# Function : processV3Struct 
#
# Arguments
#  struct: MSG_V3 struct to be processed
#
# Description
#    Goes through and hashifies the pointer or moves the string in the output Elf File
#
# Return Values:
#  new QSR_STRING length, new QSR_STRING
#=========================================================================
def processV3Struct(struct):
  
  global REPLACEMENT_MODE, ElfShrinkState
  QSRstringRE = re.compile('(\d+):(\d+):(.*)')

  # find QSR_string_ptr, the intermediate pointer
  strPointer = convertFromElf('Elf32_Addr',
                      ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + MSG_V3_QSR_POINTER_OFFSET :
                          struct['offset'] + MSG_V3_QSR_POINTER_OFFSET +
                          const['Elf32_Addr']])

  ssMask = convertFromElf('Elf32_Addr',
                      ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + MSG_V3_SS_MASK_OFFSET :
                          struct['offset'] + MSG_V3_SS_MASK_OFFSET +
                          const['Elf32_Addr']])

                            
  secHead = getSectionHeader(struct['section'])

  elfOffset = (secHead.data['sh_offset'] + struct['offset'])

  # if strPointer is null, do not count as an error or anomaly, seen in Q6 build
  if (strPointer == 0):    
    anomaly = {}
    anomaly['structName'] = struct['name']
    anomaly['structSection'] = struct['section']
    anomaly['structAddress'] = struct['address']
    anomaly['structOffset'] = struct['offset']
    anomaly['stringAddress'] = strPointer
    anomaly['Anomaly'] = "string pointer is null for msg_v2_struct"
    ElfShrinkState.errors.append(anomaly)
    ElfShrinkState.errCount += 1 
    return

  
  strPointerOffset = strPointer - QSR_STRING_START

  # check if intermediate pointer is outside of QSR_STRING
  # anomaly : QSR_string_ptr outside of QSR_STRING
  if strPointerOffset < 0 or strPointerOffset > QSR_STRING_SIZE:
    strSection = getSectionFromVA(strPointer)
    QSRstring = investigateString(strPointer, strSection)
    if strPointer not in ElfShrinkState.visitedAnomalies:
      ElfShrinkState.visitedAnomalies[strPointer] = 1;
      anomaly = {}
      anomaly['structName'] = struct['name']
      anomaly['Anomaly'] = "qsr string outside QSR_STR section"
      anomaly['structSection'] = struct['section']
      anomaly['structAddress'] = struct['address']
      anomaly['structOffset'] = struct['offset']
      anomaly['stringAddress'] = strPointer
      anomaly['string'] = QSRstring
      anomaly['stringSection'] = strSection.name

      # found the string
      if QSRstring is not None :
        if ElfShrinkState.strAnomalyDict.has_key(strPointer):
          anomaly['stringSymName'] = ElfShrinkState.strAnomalyDict[strPointer]['name']
          del ElfShrinkState.strAnomalyDict[strPointer]
        ElfShrinkState.anomalyStructCount +=1
        ElfShrinkState.anomalies.append(anomaly)
      else :
        anomaly['Anomaly'] = "qsr_str_temp does not point to a valid string"
        ElfShrinkState.errors.append(anomaly)
        ElfShrinkState.errCount += 1 
        return

  # No anomalies  
  else:
    QSRstring = getString(ElfShrinkState.bytesDict['QSR_STRING'], strPointerOffset)

  QSRstring = re.sub('\x0d', r'', QSRstring)
  # Double backslash, need to be replaced in string as 
  # Python does not match double backslashes, if present in string.
  QSRstring = re.sub(r"\\", r" ", QSRstring)

  match = QSRstringRE.match(QSRstring)
  ssid = int(match.group(1))
  line = int(match.group(2))

  #Hash Format string
  string = match.group(3)
  string = re.sub(r'\\n', '\x0a', string)
  hash = ""
  ptrUpdated = 0

  if (string in ElfShrinkState.stringHashDict):
    hash = ElfShrinkState.stringHashDict[string]
  elif REPLACEMENT_MODE == 1:
    # move this string and adjust pointers

    #replace : with null, we do not replace with null, since the code checks for ':'
    #string = re.sub(colonFixRE, '\x00', string, 1)

    # Check whether the string was already added to ElfShrinkState.newQSR_STRINGlist
    # The strPointer Dictionary takes care of anomalies too
    if (strPointer in ElfShrinkState.strMovedStrDict):
      newVAddr = ElfShrinkState.strMovedStrDict[strPointer]
      ptrUpdated = 1
    elif (string in ElfShrinkState.stringPtrDict):
      newVAddr = ElfShrinkState.stringPtrDict[string]
      ptrUpdated = 1    
    else:
      newVAddr = QSR_STRING_START_OUTPUT + ElfShrinkState.newQSR_STRINGlen
      ElfShrinkState.strMovedStrDict[strPointer] = newVAddr
      ElfShrinkState.stringPtrDict[string] = newVAddr 

      #add string to list
      ElfShrinkState.newQSR_STRINGlist.append(string)
      ElfShrinkState.newQSR_STRINGlist.append('\x00')
      strLen = len(string) + 1 #for null terminator

      #update ElfShrinkState.memSizeFnameSizeDict for the file
      # Do not add the padding information below
      if ElfShrinkState.memSizePtrFnameDict.has_key(strPointer):
        filename = ElfShrinkState.memSizePtrFnameDict[strPointer]
        # strLen (+ 1, if considering "/1")
        if ElfShrinkState.memSizeFnameSizeDict.has_key(filename):
          ElfShrinkState.memSizeFnameSizeDict[filename] -= strLen


      # pad to hit word boundaries
      if strLen % 4 == 3:
        ElfShrinkState.newQSR_STRINGlist.append('\x00')
        strLen = strLen + 1
      elif strLen % 4 == 2:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00')
        strLen = strLen + 2
      elif strLen % 4 == 1:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00\x00')
        strLen = strLen + 3
      # update new pointer to end of QSR_STRING
      ElfShrinkState.newQSR_STRINGlen += strLen

    # log it
    string = re.sub('\x0a', r'\\n', string)
    if __debug__:
      ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "moved & updated", hex(strPointer),"\n"]))
      if (ptrUpdated == 1):
        ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "pointer updated", hex(strPointer),"\n"]))

    qsr_str_hash = convertToElf('Elf32_Addr', newVAddr)
    ElfShrinkState.moved += 1
  else:
    # discovery mode
    hash = toHash(string)
    # replace new lines (all unix (0x0a) internally) with special chars to 
    # avoid partial strings in future Master Hash load
    string = re.sub('\x0a', r'\\n', string)
    ElfShrinkState.outputHash.write("".join([
                      str(convertFromElf('Elf32_Word', hash, endian=False)),
                      ":", string, "\n"]))
    ElfShrinkState.newHashedStrings += 1 # increment new strings counter

  if hash != "":
  # replace pointer in QSR struct with hash
    qsr_str_hash = convertToElf('Elf32_Word',
                convertFromElf('Elf32_Word', hash, endian=False),
                 endian=True) #gotta flip direction

    string = re.sub('\x0a', r'\\n', string)
    if __debug__:
      ElfShrinkState.outputLog.write(
                          ", ".join([secHead.name, str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset),
                               hex(convertFromElf('Elf32_Word',
                                              hash, endian=False)),
                               string, "hashified",
                               hex(strPointer),"\n"]))
    # hash is written in log and hash file in big-endian order
    ElfShrinkState.hashified += 1

  descstruct = makeQSRDescObject(elfOffset, ssid, ssMask, line, qsr_str_hash) 
  ElfShrinkState.qsrDescStructs.append(descstruct)
  
#=========================================================================
# Function : processV2Struct 
#
# Arguments
#  struct: MSG_V2 struct to be processed
#  output: the output ELF file
#
# Description
#    Goes through and hashifies the pointer or moves the string in the output Elf File
#
# Return Values:
#  new QSR_STRING length, new QSR_STRING
#=========================================================================
def processV2Struct(struct, output):
  
  global REPLACEMENT_MODE, ElfShrinkState

  # find QSR_string_ptr, the intermediate pointer
  strPointer = convertFromElf('Elf32_Addr',
                      ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + QSR_POINTER_OFFSET :
                          struct['offset'] + QSR_POINTER_OFFSET +
                          const['Elf32_Addr']])
                            
  secHead = getSectionHeader(struct['section'])

  # check if intermediate pointer is outside of QSR_STRING
  # anomaly : QSR_string_ptr outside of QSR_STRING
  newVAddr = strPointer
  elfOffset = (secHead.data['sh_offset'] + struct['offset'] +
               QSR_POINTER_OFFSET)

  strPointerOffset = strPointer - QSR_STRING_START

  # if strPointer is null, do not count as an error or anomaly, seen in Q6 build
  if (strPointer == 0):
    return
    
  #anomaly Ptr inside QSR String QSR_STRING
  if strPointerOffset < 0 or strPointerOffset > QSR_STRING_SIZE:
    strSection = getSectionFromVA(strPointer)
    string = investigateString(strPointer, strSection)
    if strPointer not in ElfShrinkState.visitedAnomalies:
      ElfShrinkState.visitedAnomalies[strPointer] = 1;
      anomaly = {}
      anomaly['structName'] = struct['name']
      anomaly['Anomaly'] = "qsr string outside QSR_STR section"
      anomaly['structSection'] = struct['section']
      anomaly['structAddress'] = struct['address']
      anomaly['structOffset'] = struct['offset']
      anomaly['stringAddress'] = strPointer
      anomaly['lineNo'] = struct['lineNo'] 
      anomaly['string'] = string
      anomaly['stringSection'] = strSection.name

      # found the string
      if string is not None :
        if ElfShrinkState.strAnomalyDict.has_key(strPointer):
          anomaly['stringSymName'] = ElfShrinkState.strAnomalyDict[strPointer]['name']
          del ElfShrinkState.strAnomalyDict[strPointer]
        ElfShrinkState.anomalyStructCount +=1
        ElfShrinkState.anomalies.append(anomaly)
      else :
        anomaly['Anomaly'] = "qsr_str_temp does not point to a valid string"
        ElfShrinkState.errors.append(anomaly)
        ElfShrinkState.errCount += 1 
        return

  # No anomalies  
  else:
    string = getString(ElfShrinkState.bytesDict['QSR_STRING'], strPointerOffset)

  string = re.sub('\x0d', r'', string)
  # Double backslash, need to be replaced in string as 
  # Python does not match double backslashes, if present in string.
  string = re.sub(r"\\", r" ", string)
  hash = ""
  ptrUpdated = 0
  if (string in ElfShrinkState.stringHashDict):
    hash = ElfShrinkState.stringHashDict[string]
  elif REPLACEMENT_MODE == 1:
    # move this string and adjust pointers

    #replace : with null, we do not replace with null, since the code checks for ':'
    #string = re.sub(colonFixRE, '\x00', string, 1)

    # Check whether the string was already added to ElfShrinkState.newQSR_STRINGlist
    # The strPointer Dictionary takes care of anomalies too
    
    if (strPointer in ElfShrinkState.strMovedStrDict):
      newVAddr = ElfShrinkState.strMovedStrDict[strPointer]
      ptrUpdated = 1
    elif (string in ElfShrinkState.stringPtrDict):
      newVAddr = ElfShrinkState.stringPtrDict[string]
      ptrUpdated = 1    
    else:
      newVAddr = QSR_STRING_START_OUTPUT + ElfShrinkState.newQSR_STRINGlen
      ElfShrinkState.strMovedStrDict[strPointer] = newVAddr
      ElfShrinkState.stringPtrDict[string] = newVAddr

      #add string to list
      ElfShrinkState.newQSR_STRINGlist.append(string)
      ElfShrinkState.newQSR_STRINGlist.append('\x00')
      strLen = len(string) + 1 #for null terminator

      #update ElfShrinkState.memSizeFnameSizeDict for the file
      # Do not add the padding information below
      if ElfShrinkState.memSizePtrFnameDict.has_key(strPointer):
        filename = ElfShrinkState.memSizePtrFnameDict[strPointer]
        # strLen (+ 1, if considering "/1")
        if ElfShrinkState.memSizeFnameSizeDict.has_key(filename):
          ElfShrinkState.memSizeFnameSizeDict[filename] -= strLen


      # pad to hit word boundaries
      if strLen % 4 == 3:
        ElfShrinkState.newQSR_STRINGlist.append('\x00')
        strLen = strLen + 1
      elif strLen % 4 == 2:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00')
        strLen = strLen + 2
      elif strLen % 4 == 1:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00\x00')
        strLen = strLen + 3
      # update new pointer to end of QSR_STRING
      ElfShrinkState.newQSR_STRINGlen += strLen

    # replace pointer in QSR struct with new str addr
    output.seek(elfOffset)
    output.write(convertToElf('Elf32_Addr', newVAddr))

    # log it
    string = re.sub('\x0a', r'\\n', string)
    if __debug__:
      ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "moved & updated", hex(strPointer) , "\n"]))
      if (ptrUpdated == 1):
        ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "pointer updated", hex(strPointer) , "\n"]))

      
    ElfShrinkState.moved += 1
  else:
    # discovery mode
    hash = toHash(string)
    # replace new lines (all unix (0x0a) internally) with special chars to 
    # avoid partial strings in future Master Hash load
    string = re.sub('\x0a', r'\\n', string)
    ElfShrinkState.outputHash.write("".join([
                      str(convertFromElf('Elf32_Word', hash, endian=False)),
                      ":", string, "\n"]))
    ElfShrinkState.newHashedStrings += 1 # increment new strings counter

  if hash != "":
  # replace pointer in QSR struct with hash
    elfOffset = (secHead.data['sh_offset'] + struct['offset'] +
                 QSR_POINTER_OFFSET)
    output.seek(elfOffset)
    output.write(convertToElf('Elf32_Word',
                convertFromElf('Elf32_Word', hash, endian=False),
                 endian=True)) #gotta flip direction

    string = re.sub('\x0a', r'\\n', string)
    if __debug__:
      ElfShrinkState.outputLog.write(
                          ", ".join([secHead.name, str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset),
                               hex(convertFromElf('Elf32_Word',
                                              hash, endian=False)),
                               string, "hashified",
                               hex(strPointer), "\n"]))
    # hash is written in log and hash file in big-endian order
    ElfShrinkState.hashified += 1

#=========================================================================
# Function : modifySCL 
#
# Arguments
#  sclFile: Linker Configuration script
#
# Description
#    This method modifies SCL file, if the QSR_STR section is completely removed.
#
# Return Values:
#  None
#=========================================================================
def modifySCL(sclFile):

  global REPLACEMENT_MODE, ElfShrinkState

  if ElfShrinkState.newQSR_STRINGlen == 0:
    if os.path.exists(sclFile):
      print "Removing QSR_NOTPAGED { ... } from", sclFile
      ElfShrinkState.outputLog.write("\n")
      ElfShrinkState.outputLog.write("Removing QSR_NOTPAGED { ... } from" +
                                     sclFile)
      try:
        input = open(sclFile, 'r');
      except IOError:
        print "Error: Could not open Scatter Loader file, ", sclFile
        exit(1)
      else:
        try:
          output = open(sclFile + "_out", 'w')
        except:
          print "Error: Could not open output SCL file, ", sclFile,"_out"
          input.close()
          exit(1)
        else:
          state = "outside"
          braceCount = 0
          line = input.readline()
          while line:
            #react to header
            for m in re.finditer(r'QSR_NOTPAGED', line):
              state = "seen name"
              output.write(line[0:m.start()])
              #output stuff right before section if present
              line = line[m.end():-1] #then process rest

            #count braces
            if (state == "seen name" or state == "in section"):
              for m in re.finditer(r'\{|\}', line):
                if m.group(0) == '{':
                  if braceCount == 0 and state == "seen name":
                    state = "in section"
                  braceCount += 1
                if m.group(0) == '}':
                  if braceCount == 1 and state == "in section":
                    state = "outside"
                    line = line[m.end():-1]
              #will write out stuff right after section if present
                  braceCount -= 1

            if state == "outside":
              output.write(line)
            line = input.readline()

          #end while
          input.close()
          output.close()
      shutil.copyfile(sclFile + "_out",
                      sclFile)
      os.remove(sclFile + "_out")
    #else:
      # should not matter
#=========================================================================
# Function : shrinkElf 
#
# Arguments
#  output: elf output file
#
# Description
#  This method shrinks the QSR_STR section in the ELF, 
#  moves the sections below it,
#  updates the ELF headers (program and section as well).
#
# Return Values:
#  None
#=========================================================================

def shrinkElf(output):

  global ElfShrinkState
  global QSR_STRING_START, QSR_STRING_SIZE, QSR_STRING_END, QSR_STRING_OFFSET
  global QSR_STRING_START_OUTPUT

  #get the bytes offset to move up the sections after QSR_STRING section
  qsrStringHeader = getSectionHeader('QSR_STRING')
  newQsrStringSectionSize = ElfShrinkState.newQSR_STRINGlen
  if ElfShrinkState.version == 3:
    newQsrStringSectionSize +=  ElfShrinkState.qsr_desc_table_num_entries*QSR_DESC_ENTRY_SIZE
  elfBytesOffset = QSR_STRING_SIZE - newQsrStringSectionSize

  # JB start
  #
  # Find the strictest alignment of any of the section headers which come after
  #  QSR_STRING.
  #
  offset_align = 1
  for i in xrange(0, header['sectionHeaderNum']):
    section_hdr = getSectionHeader(i)
    section_offset = section_hdr.data['sh_offset']
    section_size = section_hdr.data['sh_size']
    if section_offset > QSR_STRING_OFFSET:
      offset_align = max(offset_align, section_hdr.data['sh_addralign'])
  #
  # Make sure that we're shrinking by a multiple of offset_align
  #
  if (elfBytesOffset % offset_align) != 0:
    if __debug__:
      ElfShrinkState.outputLog.write('\nWould have shrunk ELF by %u bytes\n' % (elfBytesOffset,))
    elfBytesOffset -= elfBytesOffset % offset_align
    if __debug__:
      ElfShrinkState.outputLog.write('Alignment restricts ELF to shrink by %u bytes\n' % (elfBytesOffset,))
  #
  # If image_vend_offset is getting moved due to ELF shrinkage, adjust it here
  #
  if ElfShrinkState.image_vend_offset > QSR_STRING_OFFSET:
    ElfShrinkState.image_vend_offset -= elfBytesOffset
  # JB end

  removeQSection = 0
  if (ElfShrinkState.newQSR_STRINGlen == 0) and (ElfShrinkState.version == 2):
    removeQSection = 1
  
  strn = "Shrinking QSR_STRING section in output ..."
  if __debug__:
    print strn
  ElfShrinkState.outputLog.write("\n")
  ElfShrinkState.outputLog.write(strn)
  ElfShrinkState.outputLog.write("\n")

  #ElfHeader changes
  # update program header offset (phOff),
  # update section header offset (shOff),
  # if QSR_STRING is empty
  #   IMP : QSR_STRING should be the last region (by elf file offset, 
  #    and last execution region) in the SCl file.
  #   update number of program headers(phNum)
  #   update number of section headers(shNum)
  #   update Section Header String Table Index, if present
  # else
  #   Update QSR_STRING virtual and physical address to be contiguous 
  #   with the previous section. 
  #   Update QSR_STRING section size
  
  
  if (header['programHeaderOff'] > QSR_STRING_OFFSET):
    output.seek(const['ELFH_PHOFF_OFFSET'])
    output.write(convertToElf('Elf32_Off', header['programHeaderOff']- elfBytesOffset))

  if (header['sectionHeaderOff'] > QSR_STRING_OFFSET):
    output.seek(const['ELFH_SHOFF_OFFSET'])
    output.write(convertToElf('Elf32_Off', header['sectionHeaderOff']- elfBytesOffset))
  if __debug__:
    ElfShrinkState.outputLog.write(" ".join(['ElfHeader changed programHeaderOff =',
                               hex(header['programHeaderOff']- elfBytesOffset), 
                               'sectionHeaderOff =',
                               hex(header['sectionHeaderOff']- elfBytesOffset),
                               "\n"]))

  if removeQSection == 1:
    #phNum
    output.seek(const['ELFH_PHNUM_OFFSET'])
    output.write(convertToElf('Elf32_Half', header['programHeaderNum'] - 1))
    
    #shNum
    output.seek(const['ELFH_SHNUM_OFFSET'])
    output.write(convertToElf('Elf32_Half', header['sectionHeaderNum'] - 1))

    #shstrIndx
    if header['sectionNameTable'] !=0:
      output.seek(const['ELFH_SHSTRINDEX_OFFSET'])
      output.write(convertToElf('Elf32_Half', header['sectionNameTable'] - 1))
    if __debug__:
      ElfShrinkState.outputLog.write(" ".join(['ElfHeader changed programHeaderNum =',
                               str(header['programHeaderNum']-1), 
                               'sectionHeaderNum =',
                               str(header['sectionHeaderNum']-1),
                               'sectionNameTable =',
                               str(header['sectionNameTable']-1),"\n","\n"]))

  #Section Header changes
  #Update the ElfOffset of all the sections moved up, when QSR_STR section is shrunk
  for i in xrange(0, header['sectionHeaderNum']) :
    section_hdr = getSectionHeader(i);
    elfoffset = header['sectionHeaderOff']
    elfoffset += header['sectionHeaderSize'] * section_hdr.index
    if section_hdr.data['sh_offset'] >  QSR_STRING_OFFSET:
      # only debug sections are expected here
      output.seek(elfoffset + const['SECTIONH_OFFSET'])
      output.write(convertToElf('Elf32_Off',
                                     section_hdr.data['sh_offset'] - elfBytesOffset))
      #change .symtab section's "sh_link" to point to changed .strtab section
      if (section_hdr.name == ".symtab" and 
          removeQSection == 1):
        output.seek(elfoffset + const['SECTIONH_LINK'])
        output.write(convertToElf('Elf32_Word',
                                     (section_hdr.data['sh_link']-1)))

    elif section_hdr.name == "QSR_STRING":
      qsrIndex = section_hdr.index;
      elfoffset += const['SECTIONH_SIZE'] # section size
      modELF(elfoffset, convertToElf('Elf32_Word',
                                     newQsrStringSectionSize))
      if QSR_STRING_START_OUTPUT < QSR_STRING_START:
        # make the address contiguous to older section
        elfoffset -= const['Elf32_Word'] # section offset
        elfoffset -= const['Elf32_Word'] #section address
        modELF(elfoffset, convertToElf('Elf32_Word', QSR_STRING_START_OUTPUT))

                                     
  #Move the section headers up                                   
  if removeQSection == 1:
    writeElfoffset = header['sectionHeaderOff'] 
    writeElfoffset += header['sectionHeaderSize'] * qsrIndex
    for i in xrange(qsrIndex + 1, header['sectionHeaderNum']) :
      readElfOffset = writeElfoffset + header['sectionHeaderSize']
      output.seek(readElfOffset);
      bytes = output.read(header['sectionHeaderSize']);
      output.seek(writeElfoffset);
      output.write(bytes);
      writeElfoffset = readElfOffset
  
  #Program Header changes
  if removeQSection == 0:
    #update size and virtual address(if necessary) in program header
    for i in range(0, int(header['programHeaderNum'])):
      ph = ProgramHeader(i)
      #identify str section by offset
      if ph.data['p_vaddr'] == QSR_STRING_START:
        oldSize = ph.data['p_memsz']
        newSize = oldSize - elfBytesOffset
                             
        elfoffset = header['programHeaderOff']
        elfoffset += header['programHeaderSize'] * i
        elfoffset += (const['Elf32_Off'] + 1 * const['Elf32_Word'] +
                      2 * const['Elf32_Addr'])
        output.seek(elfoffset)
        output.write(convertToElf('Elf32_Word', newSize))
        if __debug__:
          ElfShrinkState.outputLog.write(", ".join([
                          'program header qsr segment file size changed at',
                                   hex(elfoffset), 'to',
                                   hex(newSize), "\n"]))
        elfoffset += const['Elf32_Word']
        output.seek(elfoffset)
        output.write(convertToElf('Elf32_Word', newSize))
        if __debug__:
          ElfShrinkState.outputLog.write(", ".join([
                           'program header qsr segment mem size changed at',
                                   hex(elfoffset), 'to',
                                   hex(newSize), "\n"]))
        if (QSR_STRING_START_OUTPUT < QSR_STRING_START):
        # make the address contiguous to older section
          elfoffset -= const['Elf32_Word']
          elfoffset -= const['Elf32_Word']
          modELF(elfoffset, convertToElf('Elf32_Word', QSR_STRING_START_OUTPUT))
          elfoffset -= const['Elf32_Word']
          modELF(elfoffset, convertToElf('Elf32_Word', QSR_STRING_START_OUTPUT))        

          
  else:
    # find QSR_STRING's Program Header and remove it
    for i in range(0, header['programHeaderNum']):
      ph = ProgramHeader(i)
      #identify str section by offset
      if ph.data['p_offset'] == QSR_STRING_OFFSET:
        if ph.index != header['programHeaderNum'] - 1 :
          try:
            input = open(ElfShrinkState.path, 'rb')
          except IOError:
            print "Error: Could not open output Elf file, ", ElfShrinkState.path
            exit(1)
          else:
            input.seek(ph.offset + header['programHeaderSize'])
            numHeaders = header['programHeaderNum'] - 1 - ph.index
            bytes = input.read(header['programHeaderSize'] * numHeaders)
            input.close()
          modELF(ph.offset, bytes)
        break

  #update the elfOffset of Program header's after QShrink (, say for RO_Fatal section ) 
  #correctly
  for i in range(0, int(header['programHeaderNum'])):
    ph = ProgramHeader(i)
    if ph.data['p_offset'] > QSR_STRING_OFFSET:
      elfoffset = header['programHeaderOff']
      elfoffset += header['programHeaderSize'] * i
      elfoffset += (const['Elf32_Word'])
      output.seek(elfoffset)
      output.write(convertToElf('Elf32_Off', ph.data['p_offset'] -elfBytesOffset ))
  
  # move the sections after QSR_STRING up by elfBytesOffset
  output.seek(QSR_STRING_OFFSET)
  #then some strings left behind, output new section
  if ElfShrinkState.newQSR_STRINGlen != 0:
    output.write("".join(ElfShrinkState.newQSR_STRINGlist))
  if ElfShrinkState.version == 3:
    if ElfShrinkState.qsr_desc_table_num_entries != 0:
      #Update QSR Description table and the DIAG related pointers
      output.write("".join(ElfShrinkState.qsr_desc_table))



  #Now copy the rest of the data in the elf , truncating the QShrink section.
  # This has to be done in steps to avoid Memoryerror  
  readElfOffset = QSR_STRING_OFFSET + QSR_STRING_SIZE
  output.seek(readElfOffset)
  writeElfOffset = QSR_STRING_OFFSET + newQsrStringSectionSize
  # JB start
  writeElfOffset = readElfOffset - elfBytesOffset
  # JB end
  output.seek(0,2)
  bytestoCopy = output.tell() - readElfOffset
  while bytestoCopy > 1048576:
    output.seek(readElfOffset)
    bytes = output.read(1048576)
    bytestoCopy-=1048576
    readElfOffset = output.tell()
    # write to  current offset
    output.seek(writeElfOffset)
    output.write(bytes);
    writeElfOffset = output.tell()
  if bytestoCopy > 0:
    output.seek(readElfOffset)
    bytes = output.read(bytestoCopy)
    output.seek(writeElfOffset)
    output.write(bytes)
  output.flush()
  output.truncate()

#=========================================================================
# Function : updateDiagDescTableAddress 
#
# Arguments
#    output: elf output file
# Description
#   Updates DIAG MSG_V3_SS_TABLE address and number of entries
#
# Return Values:
#  None
#=========================================================================
def updateDiagDescTableAddress(output):
  global ElfShrinkState

  output.seek(ElfShrinkState.diagDescTblEntry)  
  output.write(convertToElf('Elf32_Word', ElfShrinkState.qsr_desc_table_num_entries));

  TblAddr = QSR_STRING_START + ElfShrinkState.newQSR_STRINGlen
  output.seek(ElfShrinkState.diagDescTbl)  
  output.write(convertToElf('Elf32_Word', TblAddr));
  output.flush()
  
  ElfShrinkState.outputLog.write(" ".join(['DIAG desc table Num entries, ', 
       str(ElfShrinkState.qsr_desc_table_num_entries),"\n"]))
  ElfShrinkState.outputLog.write(" ".join(['DIAG desc table value changed at ',
       hex(ElfShrinkState.diagDescTbl), ' to ',
       hex(TblAddr),"\n"]))
  

#=========================================================================
# Function : updateKernel_VirtEndAddr 
#
# Arguments
#    output: elf output file
# Description
#   Updates Kernel const variable image_vend const data by reducing QSTRING section
#    size from Kernel generated end symbol
#
# Return Values:
#  None
#=========================================================================
def updateKernel_VirtEndAddr(output):
  global ElfShrinkState,QSR_DESC_ENTRY_SIZE

  align = 0;

  #get the align value from QSR Program Header
  for i in range(0, int(header['programHeaderNum'])):
    ph = ProgramHeader(i) 
    if ph.data['p_vaddr'] == QSR_STRING_START:
      align = ph.data['p_align'];

  old_qsrsize = QSR_STRING_SIZE + align
  old_qsrsize -= old_qsrsize % align

  new_qsr_size = ElfShrinkState.newQSR_STRINGlen
  if ElfShrinkState.version == 3:
    new_qsr_size += ElfShrinkState.qsr_desc_table_num_entries*QSR_DESC_ENTRY_SIZE
  new_q_align = new_qsr_size % align
  if new_q_align != 0: 
    new_qsr_size += align - new_q_align
 
  ElfShrinkState.linker_endAddr -= QSR_STRING_START - QSR_STRING_START_OUTPUT
  ElfShrinkState.linker_endAddr -= old_qsrsize - new_qsr_size
     
  output.seek(ElfShrinkState.image_vend_offset)  
  output.write(convertToElf('Elf32_Word', ElfShrinkState.linker_endAddr));
  output.flush()
  
    
#=========================================================================
# Function : processStructs 
#
# Arguments
#  structs: MSG_V2 structs
#  output : output Elf File
#
# Description
#   Goes through the list of structs found,
#    uses the 'QSR_string_ptr' to a string, hashes 
#     the string, and replaces the pointer with the hash.
#
# Return Values:
#  None
#=========================================================================
def processStructs(structs, output):

  global REPLACEMENT_MODE, ElfShrinkState

  ElfShrinkState.anomalies = []

  ElfShrinkState.qsrDescStructs = []

  if __debug__:
    print "Processing all QSR Structs..."

  #sort into sections
  structs.sort(cmp=lambda x, y: cmp(x['address'], y['address']))

  #setup for replacement mode
  ElfShrinkState.newQSR_STRINGlist = []
  ElfShrinkState.newQSR_STRINGlen = 0

  # keeps track of Virtual Addresses which have already been visited
  visited = {}

  #for changing strings that get moved in replacement mode
  
  if __debug__:
    #print header for csv style logging of changes
    ElfShrinkState.outputLog.write(", ".join(["Section Name",
                                            "Section Index",
                                            "Struct VA",
                                            "Symbol Name",
                                            "ELF File Offset",
                                            "Hash",
                                            "Hashed String" ,
                                            "Action Taken",
                                            "Original VA",
                                             "\n"]))

  for struct in structs:
    # loop over structs doing stuff

    # do not visit same struct twice
    if struct['address'] not in visited:
      visited[struct['address']] = 1
      ElfShrinkState.uniqueStructMatches += 1
    else:
      continue

    if struct['section'] not in ElfShrinkState.bytesDict:
      ElfShrinkState.bytesDict[struct['section']] = getSectionData(struct['section'])
      # load new section

#   process Struct even for anomalies    
  # end loop over structs
    if struct['version'] == 3:
      processV3Struct(struct)    
    elif struct['version'] == 2:
      lineNo = convertFromElf('Elf32_Half',
                        ElfShrinkState.bytesDict[struct['section']][struct['offset'] + 0 :
                          struct['offset'] + 0 + 2])

      struct['lineNo'] = lineNo
      processV2Struct(struct, output)

  if ElfShrinkState.version == 3:
    processMsgDescTable(output) 
  
  del ElfShrinkState.qsrDescStructs
#=========================================================================
# Function : processMsgDescTable 
#
# Arguments
#
# Description
# Create the Msg Description Indexed Table with ssHash, SSID, SS_MASK
# sorted on SSID
#
# Return Values:
#=========================================================================
def processMsgDescTable(output):

  global REPLACEMENT_MODE, ElfShrinkState
  
  if not ElfShrinkState.qsrDescStructs:
    print "Error : QSR Descriptors not found"
    ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR Descriptors not found !","\n"]))
    exit(1)

  ElfShrinkState.qsrDescStructs.sort(cmp=lambda x, y: cmp(x['ss_id'], y['ss_id']))

  # all symbol names
  ssidIndexDict = dict()

  ElfShrinkState.qsr_desc_table = []
  ElfShrinkState.qsr_desc_table_num_entries = 0

  indexcount = 0

  #ElfShrinkState.outputLog.write("".join(["Elfoffset , ", "SS_Index , ",
  #                                        "SS_ID:MASK , ","LINE , ","\n"]))
  
  for descstruct in ElfShrinkState.qsrDescStructs:
  # loop over structs to make unique desc entries
    ssidstring = "".join([
                 str(descstruct['ss_id']),
                 ":",
                 str(descstruct['ss_mask'])])   
    if ssidIndexDict.has_key(ssidstring):
      index = ssidIndexDict[ssidstring]
    else:   
      #rt_mask
      ElfShrinkState.qsr_desc_table.append(convertToElf('Elf32_Half', 0))
      ElfShrinkState.qsr_desc_table.append(convertToElf('Elf32_Half', descstruct['ss_id']))
      ElfShrinkState.qsr_desc_table.append(convertToElf('Elf32_Word', descstruct['ss_mask']))
      ssidIndexDict[ssidstring] = ElfShrinkState.qsr_desc_table_num_entries
      index = ElfShrinkState.qsr_desc_table_num_entries
      ElfShrinkState.qsr_desc_table_num_entries+=1
      
    #Write to Output elf MSG_Vx_STRUCTS
    # Values ss_index, line and hash

    # offset of the struct's const char* const* pointer
    output.seek(descstruct['elfoffset']+ MSG_V3_SS_INDEX_OFFSET)
    output.write(convertToElf('Elf32_Half',index))
    output.write(convertToElf('Elf32_Half',descstruct['line']))
    output.write(descstruct['qsr_msg_hash'])
    #ElfShrinkState.outputLog.write("".join([ hex(descstruct['elfoffset'])," , ",
    #                                        str(index)," , ",ssidstring,
    #                                        " ,", str(descstruct['line']), "\n"]))

  del ssidIndexDict

   
#=========================================================================
# Function : processSymbolTable 
#
# Arguments
#
# Description
#   Gets the list of MSG_V2 structs from the symbol table and the string table
#   Creates description mapping for each struct 
#
# Return Values:
#  List of structs
#=========================================================================
def processSymbolTable():

  global ElfShrinkState,MSG_V2_STRUCT_SIZE,MSG_V2_ERR_STRUCT_SIZE,MSG_V3_STRUCT_SIZE,MSG_V3_ERR_STRUCT_SIZE

  # get the symbol table and the string table to find symbol names
  symbolTableData = getSectionData('.symtab')
# all symbol data
  stringTableData = getSectionData('.strtab')
# all symbol names
  qsrRE = re.compile(r'xx_msg_v[23]_const')
  qsr20RE = re.compile(r'xx_msg_v2_const')
  qsr30RE = re.compile(r'xx_msg_v3_const')
  #  msgRE = re.compile(r'xx_msg_const')
  errRE = re.compile(r'xx_err_msg_v[23]_const')
  err20RE = re.compile(r'xx_err_msg_v2_const')
  err30RE = re.compile(r'xx_err_msg_v3_const')
  
  QSRStrTmpRE = re.compile(r'QSR_string_ptr')
  qsr_re = re.compile(r'QSR_')
  realdatare = re.compile(r'\$d.realdata')
  dre = re.compile(r'\$d')
  qsrConstRE = re.compile(r'MSG_V[23]_CONST')
  
  QSRTblEntryRE = re.compile(r'msg_v3_ss_num_table_entries')
  QSRTblRE = re.compile(r'msg_v3_ss_table')
  
  structs = []
  visited_symtab_str = {}  
  
  ElfShrinkState.errors = []
  if __debug__:
    print "Scanning Symbol Table for Structs ..."

  ElfShrinkState.version = 2
  # iterate through entries in the symbol table (uses a generator) to get the 
  # structure size
  for entry in symbolTableEntryGenerator(symbolTableData, stringTableData):
    # check for a QSR struct symbol
     if (MSG_V2_STRUCT_SIZE != 0) and (MSG_V2_ERR_STRUCT_SIZE != 0):
       break
     if (MSG_V3_STRUCT_SIZE != 0) and (MSG_V3_ERR_STRUCT_SIZE != 0):
       ElfShrinkState.version = 3        
       MSG_V2_STRUCT_SIZE = MSG_V3_STRUCT_SIZE + 4
       MSG_V2_ERR_STRUCT_SIZE = MSG_V3_ERR_STRUCT_SIZE + 4
       # There may be V2 structs in V3 build
       break
     if qsrRE.search(entry.name):
       if (MSG_V2_STRUCT_SIZE == 0) and qsr20RE.search(entry.name):
           MSG_V2_STRUCT_SIZE = entry.size
       elif (MSG_V3_STRUCT_SIZE == 0) and qsr30RE.search(entry.name):
           MSG_V3_STRUCT_SIZE = entry.size  
     elif errRE.search(entry.name):
       if (MSG_V2_ERR_STRUCT_SIZE == 0) and err20RE.search(entry.name):
           MSG_V2_ERR_STRUCT_SIZE = entry.size
       elif (MSG_V3_ERR_STRUCT_SIZE == 0) and err30RE.search(entry.name):
           MSG_V3_ERR_STRUCT_SIZE = entry.size


  if (MSG_V2_STRUCT_SIZE == 0) and (MSG_V3_STRUCT_SIZE == 0):
    ElfShrinkState.errCount += 1
    anomaly['Anomaly'] = "Struct xx_msg_v2_const or xx_msg_v3_const not found "
    ElfShrinkState.errors.append(anomaly)
    return
    
  # iterate through all entries in the symbol table (uses a generator)
  for entry in symbolTableEntryGenerator(symbolTableData, stringTableData):
    qsrentry = 0
    if (ElfShrinkState.image_vend_offset  == 0) and (entry.name == "image_vend"):
      secHead = getSectionHeader(entry.sectionIndex)
      new_struct = makeQSRobject(entry, secHead);
      ElfShrinkState.image_vend_offset = new_struct['elf_offset']


    if (ElfShrinkState.linker_endAddr  == 0) and (entry.name == "end"):
      ElfShrinkState.linker_endAddr = entry.value
    
    if ElfShrinkState.version == 3 :
      if (ElfShrinkState.diagDescTblEntry  == 0) and (QSRTblEntryRE.match(entry.name)):
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.diagDescTblEntry = new_struct['elf_offset']

      if (ElfShrinkState.diagDescTbl  == 0) and (QSRTblRE.match(entry.name)):      
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.diagDescTbl = new_struct['elf_offset']

      if ( (entry.size == MSG_V3_STRUCT_SIZE 
         and (qsr30RE.search(entry.name) 
    #        or msgRE.search(entry.name)
          )
          )
         or(entry.size == MSG_V3_ERR_STRUCT_SIZE
           and (err30RE.search(entry.name))
         )
         ):
        ElfShrinkState.matches += 1
        secHead = getSectionHeader(entry.sectionIndex)
        struct = makeQSRobject(entry, secHead)
        struct['version'] = 3
        structs.append(struct)
        qsrentry = 1

    if qsrentry == 0:
      # check for a QSR struct symbol
      if ( (entry.size == MSG_V2_STRUCT_SIZE 
        and (qsr20RE.search(entry.name) 
    #       or msgRE.search(entry.name)
             )
        )
        or(entry.size == MSG_V2_ERR_STRUCT_SIZE
           and (err20RE.search(entry.name))
         )
         ):
        ElfShrinkState.matches += 1
        secHead = getSectionHeader(entry.sectionIndex)
        struct = makeQSRobject(entry, secHead)
        struct['version'] = 2
        structs.append(struct)
        qsrentry = 1

    #check for QSR strings and pointers in QSR section
    if qsrentry == 0:
      if (QSR_STRING_START <= entry.value <= QSR_STRING_END ):
        if qsr_re.search(entry.name):
          if entry.value not in visited_symtab_str :
            visited_symtab_str[entry.value] = 1
            ElfShrinkState.goodStrsPtrsCount_Symtab += 1      
          else:
            continue
        # check for a non-QSR symbol in QSR_STRING
        # (not qsr_re.search(entry.name) 
        # if previous memory section is empty, linker generated _start and _end
        # address could be mapped to same address as QSR_SECTION
        elif ((not realdatare.search(entry.name)) and
            (not dre.search(entry.name)) and 
            (entry.name !='') and 
            (entry.size !=0)):
          ElfShrinkState.errCount += 1
          secHead = getSectionHeader(entry.sectionIndex)
          anomaly = makeQSRobject(entry, secHead)
          anomaly['Anomaly'] = "Non-QSR_ In QSR_STRING "
          ElfShrinkState.errors.append(anomaly)

          
      # test for a QSR_string_ptr or string not in QSR_STRING Section
      # ignore QSR_CONST
      # in 9K build, in a CPP file the string pointer itself was empty, do not count as error
      # (not (QSR_STRING_START <= entry.value <= QSR_STRING_END) 
      elif (not qsrConstRE.search(entry.name) 
         and (qsr_re.search(entry.name))
         and (entry.value != 0x0)) :
        if entry.value not in visited_symtab_str :
          visited_symtab_str[entry.value] = 1
          #=== anomaly - investigate
          secHead = getSectionHeader(entry.sectionIndex)
       
          #test for QSR_string_ptr
          if (QSRStrTmpRE.search(entry.name)):
            # check for string 
            # string with length <= sizeof(Addr)will never be hit
            # not all string anomalies are caught here as some of the strings are named .conststring$x, instead of qsr_
            ElfShrinkState.anomalyStrCount += 1
            anomaly = makeQSRobject(entry, secHead)
            anomaly['Anomaly'] = "qsr string outside QSR_STRING"
            anomaly['string'] = investigateString(entry.value, secHead)
            ElfShrinkState.strAnomalyDict[entry.value] = anomaly;
          
          # not a QSR struct, QSR string, or intermediate pointer
          # no idea what it is
          #else:
            #anomaly = makeQSRobject(entry, secHead)
            #anomaly['Anomaly'] = "Symbol QSR_ outside QSR_STRING "
            #ElfShrinkState.errCount += 1
            #ElfShrinkState.errors.append(anomaly)
           
        else:
          continue

          
      #=== end anomaly investigation
  # end for symbol table iterator
  if ElfShrinkState.version == 3 :
    if (ElfShrinkState.diagDescTbl == 0) or (ElfShrinkState.diagDescTblEntry == 0):
      print "Error : QSR Descriptors not found"
      ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR Descriptors not found !","\n"]))
      exit(1)
  
  return structs

#=========================================================================
# Function : replaceQSRSymbolStructs 
#
# Arguments
#
# Description
#   Iterates through symbol table entries, finding MSG V2 structs.
#   Then it goes through the structs hashify-ing or moving their strings.
#
# Return Values:
#  List of structs
#=========================================================================
def replaceQSRSymbolStructs():

  global REPLACEMENT_MODE, ElfShrinkState
  scl_re = re.compile(r'scl')  

  ElfShrinkState.goodStrsPtrsCount_Symtab = 0
  #summary counter of qsr_strings and QSR_string_ptrs in QSR_STR section derived from SymTab

  #Note
  #more than one xx_msg_vx_const structure point to the same QSR_string_ptr structure

  ElfShrinkState.anomalyStrCount = 0
  #summary counter of qsr_string outside QSR_STR section derived from processSymbolTable and ProcessStructs
  #some of the strings are named as .conststring and not part of processSymbolTable analysis alone
  
  startTime = time.clock()

  structs = processSymbolTable()

  scanTime = time.clock() - startTime
  if __debug__:
    print "Done Scanning Symbol Table (", scanTime, "seconds)"
    print "Total Struct Matches:", ElfShrinkState.matches

    print "Total QSR Strings and Pointers in QSR_STR Section from SYM_TAB:", ElfShrinkState.goodStrsPtrsCount_Symtab
    print "Total QSR Strings outside QSR_STR Section from SYM_TAB:",ElfShrinkState.anomalyStrCount 
  
  ElfShrinkState.outputLog.write(" ".join(
              ["Done Scanning Symbol Table (", str(scanTime),
               "seconds)", "\n"]))
  ElfShrinkState.outputLog.write(" ".join(
              ["Total Struct Matches:", str(ElfShrinkState.matches), "\n"]))

  for key in ElfShrinkState.bytesDict.keys():
    if key != 'QSR_STRING':
      ElfShrinkState.bytesDict.pop(key) #clear all except QSR_STRING from mem

  output = open(ElfShrinkState.outputPath, 'r+b')

  processStructs(structs, output)

  ElfShrinkState.bytesDict.clear() #release the memory

  if __debug__:
    try:
      anomalyListFile = open(ElfShrinkState.anomalyListPath, 'w')
    except IOError:
      print "Error : Could not open Anomaly List File, ", ElfShrinkState.anomalyListPath
      exit(1)
    else:
      csv.register_dialect('ours',
                           delimiter=',',
                           quoting=csv.QUOTE_MINIMAL,
                           lineterminator='\n')

      anomalyListLabels = ["Anomaly", "structName", "structSection", 
                           "structAddress", "structOffset",
                           "stringSymName","stringSection","stringAddress", 
                           "string", "lineNo"]

      invalidListCSV = csv.DictWriter(anomalyListFile,
                                      anomalyListLabels,
                                      dialect='ours')

      # invalidListCSV.writeheader() #need python 2.7,fix below
      headers = dict((n, n) for n in anomalyListLabels)
      invalidListCSV.writerow(headers)

      #clean up output
      for anomaly in ElfShrinkState.anomalies:
        if 'structAddress' in anomaly:
          anomaly['structAddress'] = hex(anomaly['structAddress'])
        if 'stringAddress' in anomaly:
          anomaly['stringAddress'] = hex(anomaly['stringAddress'])
        if 'structOffset' in anomaly:
          anomaly['structOffset'] = hex(anomaly['structOffset'])

      invalidListCSV.writerows(ElfShrinkState.anomalies)
      anomalyListFile.close()

# Write the non processed Symbol table anomalies into errors
  for (p,v) in ElfShrinkState.strAnomalyDict.items():
    ElfShrinkState.errCount += 1
    ElfShrinkState.errors.append(v)

  if ElfShrinkState.errCount > 0:
    print "QShrink failed ! Investigate Error Structs."
    ElfShrinkState.outputLog.write("".join(["\n","ElfShrink failed ! Investigate Error Structs !","\n"]))
    ElfShrinkState.outputLog.write("".join(["Errors: ",str(ElfShrinkState.errCount),"\n"]))
    
  else:
#    shrink the Elf file 
    if (scl_re.search(ElfShrinkState.sclPath)):
      modifySCL(ElfShrinkState.sclPath)
    shrinkElf(output)
    if (ElfShrinkState.image_vend_offset and 
        ElfShrinkState.linker_endAddr):
      ElfShrinkState.outputLog.write(" ".join(['Linker generated IMAGE END Addr changed in elf_offset ', 
              hex(ElfShrinkState.image_vend_offset),'from ',
               hex(ElfShrinkState.linker_endAddr), "to "]))
      updateKernel_VirtEndAddr(output)
      ElfShrinkState.outputLog.write("".join([hex(ElfShrinkState.linker_endAddr),"\n"]))
    if ElfShrinkState.version == 3:
      updateDiagDescTableAddress(output)
 
  for error in ElfShrinkState.errors:
    print("".join([ "Error : " ,error['Anomaly']]))
    ElfShrinkState.outputLog.write("".join([ "Error : " ,error['Anomaly'],"\n"]))
    ElfShrinkState.outputLog.write("".join(["    " , error['Anomaly'],": ",str(error),"\n"]))
    ElfShrinkState.outputLog.write("\n")

  output.close()

#=========================================================================
# Function : modELF 
#
# Arguments
#   offset: (int) Offset into the ELF file to start writing
#   bytes: (str) Bytes to write into the ELF file in the identical order
#         If there is endianness at play, catch it before calling.
#
# Description
#   Does an in-place overwrite of the ELF file at the specified offset with 
#   the given bytes.  Very slow, but it will keep the memory usage down.
#
# Return Values:
#  List of structs
#=========================================================================
def modELF(offset, bytes):
  global ElfShrinkState
  try:
    file = open(ElfShrinkState.outputPath, 'r+b')
  except IOError:
    print "Error: Could not open output Elf File, ",ElfShrinkState.outputPath
    exit(1)
  else:
    file.seek(offset)
    file.write(bytes)
    file.close()

#=========================================================================
# Function : writeHashToFile 
#
# Arguments
#   string: (str) MSG string that was hashed
#   hash: (str) Hash of string in byte form
#   file: (str) Path to the Master Hash File
#
# Description
#   Writes the given hash and string to the Master Hash File.  Assumes that the
#   hash and string only show up once, otherwise bad behavior may follow when
#   the Master Hash File is loaded on the next run.
#
# Return Values:
#  List of structs
#=========================================================================
def writeHashToFile(string, hash, file):
  if (string not in ElfShrinkState.stringHashDict) :
    ElfShrinkState.stringHashDict[string] = hash
    try:
      output = open(file, 'a')
    except IOError:
      print "Error: Could not open Master Hash File ",file," for writing"
      exit(1)
    else:
      output.write(str(convertFromElf('Elf32_Word', hash, False)) +
                    ":" + string + "\n")
      output.close()
    return 1
  return 0

#=========================================================================
# Function : unAlignHash 
#
# Arguments
#   hash: hash to be unAlinend(not divisible by 4)
#
# Description
#   adds one if the last 2 bits are 00
#
# Return Values:
#  the hash, either untouch or unaligned
#=========================================================================
def unAlignHash(hash):
  '''
  @param hash: hash to be unAlinend
  @return: the hash, either untouch or unaligned
  adds one if the last 2 bits are 00
  '''
  if hash & 0x3 == 0:
    hash += 1
  return hash

#=========================================================================
# Function : toHash 
#
# Arguments
#   string: (str) String to hash.  Should be filename:message
#
# Description
#   Translate a string into a hash.  First checks to see if the string already
#   has a hash.  Otherwise, computes one and returns it. 
#   NOTE: Only used in discovery mode.
#  @TODO: Make hash computation faster on collision case
#  @TODO: Make case for full hash
#  @TODO: Handle int wrap-around case
#
# Return Values:
#  (str) Unique hash for the string.
#=========================================================================
def toHash(string):
  global ElfSrhinkState
  hash = hashlib.md5(string).digest()[-4:]
  # lowest 4 bytes of hash

  hash = convertToElf('Elf32_Word',
                      unAlignHash(convertFromElf('Elf32_Word', hash,
                                             endian=False)),
                      endian=False)
 
  # check if hash is unused or if string is already mapped to hash
  if ((hash not in ElfShrinkState.hashStringDict) or (ElfShrinkState.hashStringDict[hash] == string)) :
    ElfShrinkState.hashStringDict[hash] = string
    ElfShrinkState.stringHashDict[string] = hash
    return hash

  # search for an unused hash
  hash = convertToElf('Elf32_Word',
                      unAlignHash(convertFromElf('Elf32_Word', hash,
                                             endian=False) + 1),
                      endian=False) # @TODO: improve collision resolution
  while hash in ElfShrinkState.hashStringDict :
    if ElfShrinkState.hashStringDict[hash] == string :
      if __debug__:
        print "Hash already present but tried to hash again:", \
              hex(convertFromElf('Elf32_Word', hash, endian=False)), string
      return hash
    hash = convertToElf('Elf32_Word',
                        unAlignHash(convertFromElf('Elf32_Word', hash,
                                             endian=False) + 1),
                        endian=False) # @TODO: improve collision resolution

  # cache results
  ElfShrinkState.hashStringDict[hash] = string
  ElfShrinkState.stringHashDict[string] = hash
  return hash

#=========================================================================
# Function : symbolTableEntryGenerator 
#
# Arguments
#   bytes: (str) String of bytes which contains the Symbol Table
#   strTabData: (str) String of bytes which contains the String table
#
# Description
#   Creates a generator over the given bytes (should be the Symbol Table) that 
#   yields the next 16 bytes as a Symbol Table Entry.
#
# Return Values:
#  (generator:SymbolTableEntry) Iterable which has SymbolTableEntry
#          objects according to bytes
#=========================================================================
def symbolTableEntryGenerator(bytes, strTabData):
  assert len(bytes) % 16 == 0
  for i in xrange(0, len(bytes), 16) :
    yield SymbolTableEntry(bytes[i : i + 16], strTabData)

#=========================================================================
# Function : initializeWithElfHeader 
#
# Arguments
#
# Description
#   @warning: May suddenly exit script if bad header information is found 
#   Sets global data according to the ELF header.
#
# Return Values:
#=========================================================================
def initializeWithElfHeader():
  global ElfShrinkState
  global QSR_STRING_START, QSR_STRING_SIZE, QSR_STRING_END, QSR_STRING_OFFSET
  global QSR_STRING_START_OUTPUT

  # set global header's entries according to ELF header
  try: 
    input = open(ElfShrinkState.path, 'rb')
  except IOError:
    print "Error, Could not open input Elf File, ", ElfShrinkState.path
    exit(1)
  else:
    elf_header = input.read(52)
    processHeader(elf_header)
    input.close()

  # get all section headers and map by index and name
  ElfShrinkState.sectionNameHeader = getSectionHeader(header['sectionNameTable'])
  ElfShrinkState.sectionNameData = getSectionData(header['sectionNameTable'])
  if __debug__:
    print "Loading all Section Headers"
  sectionStart = time.clock()
  for i in xrange(0, header['sectionHeaderNum']) :
    ElfShrinkState.sectionHeaderDict[i] = getSectionHeader(i)
    ElfShrinkState.sectionHeaderDict[getSectionHeader(i).name] = ElfShrinkState.sectionHeaderDict[i]
  if __debug__:
    print "Finished Loading Section Headers (", (time.clock()
                                               - sectionStart), "seconds)"
  # print "Number of Sections: ", header['sectionHeaderNum']

  # set global data about the QSR string section
  # mainly used for shorter calculations
  if ('QSR_STRING' in ElfShrinkState.sectionHeaderDict):
    qsrStringHeader = getSectionHeader('QSR_STRING')
    QSR_STRING_START = qsrStringHeader.data['sh_addr']
    QSR_STRING_SIZE = qsrStringHeader.data['sh_size']
    QSR_STRING_END = QSR_STRING_START + QSR_STRING_SIZE
    QSR_STRING_OFFSET = qsrStringHeader.data['sh_offset']

  #find the highest address less than QSR_STRING addr to update Section headers and
  #Program headers, get align value for Program Headers. For 9k targets, section headers
  # do not contain correct align values
  QSR_STRING_START_OUTPUT = QSR_STRING_START
  QSR_vAddr = 0
  for i in range(0, int(header['programHeaderNum'])):
    ph = ProgramHeader(i)
    if ((ph.data['p_vaddr'] + ph.data['p_memsz']) <= QSR_STRING_START and
        (ph.data['p_vaddr'] + ph.data['p_memsz']) > QSR_vAddr):
      QSR_vAddr = ph.data['p_vaddr'] + ph.data['p_memsz'] + ph.data['p_align']
      QSR_vAddr = QSR_vAddr - (QSR_vAddr % ph.data['p_align'])   
  if (QSR_vAddr < QSR_STRING_START):
    QSR_STRING_START_OUTPUT = QSR_vAddr
    
#=========================================================================
# Function : getString 
#
# Arguments
#  bytes: (str) Bytes that contain a null character.
#  begin: (int) Where to start in the given string of bytes.
#  end: (int) Where to end, non-inclusive, in the given string of bytes
#
# Description
#   Gets the first null-terminated string from bytes[begin : end].  If end is
#   not given, then it will search the bytes[begin : ].  If no null is found,
#   then it will return bytes[begin : ]
#
# Return Values:
#   All characters from begin to either end or the first null char.
#=========================================================================
def getString(bytes, begin=0, end= -1):
  index = begin
  if end == -1 : end = len(bytes)
  while (index < end and bytes[index] != '\x00'):
#         and bytes[index] != '\x01'):
    index += 1
  return bytes[begin:index]
  
#=========================================================================
# Function : getStringGenerator 
#
# Arguments
#   bytes: (str) Bytes to get a string from
#   begin: (int) Start offset into bytes
#   end: (int) End offset into bytes.  Non-inclusive.
#
# Description
#   Deprecated.  Requires only a single-null byte between strings and a
#   contiguous string section.  Only yields strings in order.  Not used since
#   strings are not guaranteed to be in order.
#   Returns a generator that yields the next string in bytes. 
#
# Return Values:
#   (generator:str) Iterable which has strings from bytes
#=========================================================================
def getStringGenerator(bytes, begin=0, end= -1):
  index = begin
  if end == -1 : end = len(bytes)
  while index < end :
    while index < end and bytes[index] != '\x00' :
      index += 1
    yield bytes[begin:index]
    index += 1
    begin = index

#=========================================================================
# Function : getSectionData 
#
# Arguments
#   section: ([str|int]) Index of the section whose data you want
#
# Description
#   Reads the input ELF file and grabs the section's data.  Does not cache to 
#   avoid a Memory Error when part of a build.
#
# Return Values:
#   section data
#=========================================================================
def getSectionData(section):
  secheader = getSectionHeader(section)
  offset = secheader.data['sh_offset']
  size = secheader.data['sh_size']
  try:
    input = open(ElfShrinkState.path, 'rb')
  except IOError:
    print "Error: Could not open input ELF file, ",ElfShrinkState.path
    exit(1)
  else:
    input.seek(offset)
    ret = input.read(size)
    input.close()
  return ret


#=========================================================================
# Function : processHeader 
#
# Arguments
#   elf_header: (str) Bytes of the header
#
# Description
#   Goes through the ELF header and sets entries in the global header dict
#   accordingly.
#
# Return Values:
#=========================================================================
def processHeader(elf_header):

  global header
  header_data = header # local name is faster
  e_ident = elf_header[0:16]
  checkEIDENT(header_data, e_ident)
  if __debug__:
    print ""
  checkElfHeaderType    (convertFromElf('Elf32_Half', elf_header[16:18]))
  checkElfHeaderMachine (convertFromElf('Elf32_Half', elf_header[18:20]))
  checkElfHeaderVersion (convertFromElf('Elf32_Word', elf_header[20:24]))
  header_data["entry"] = convertFromElf('Elf32_Addr', elf_header[24:28])
  header_data["programHeaderOff"] = convertFromElf(
                                       'Elf32_Off' , elf_header[28:32])
  header_data["sectionHeaderOff"] = convertFromElf(
                                       'Elf32_Off' , elf_header[32:36])
  getElfHeaderFlags(header_data, convertFromElf(
                                       'Elf32_Word', elf_header[36:40]))
  header_data["headerSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[40:42])
  header_data["programHeaderSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[42:44])
  header_data["programHeaderNum"] = convertFromElf(
                                       'Elf32_Half', elf_header[44:46])
  header_data["sectionHeaderSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[46:48])
  header_data["sectionHeaderNum"] = convertFromElf(
                                       'Elf32_Half', elf_header[48:50])
  header_data["sectionNameTable"] = convertFromElf(
                                       'Elf32_Half', elf_header[50:52])

#=========================================================================
# Function : getElfHeaderFlags 
#
# Arguments
#   header: (str) Global header dictionary
#   e_flags: (str) Bytes of the e_flags field
#
# Description
#   Print out known flags in the ELF file.  No real reason for this to exist
#   as of yet.
#
# Return Values:
#=========================================================================
def getElfHeaderFlags(header, e_flags):

  header['flags'] = e_flags
  #if __debug__:
    #print "Flags:", header['flags'], hex(header['flags'])
    #if header['flags'] & 0x2: print "Flag: EF_ARM_HASENTRY"
    #if header['flags'] & 0x4: print "Flag: EF_ARM_SYMSARESORTED"
    #if header['flags'] & 0x8: print "Flag: EF_ARM_DYNSYMSUSESEGIDX"
    #if header['flags'] & 0x10: print "Flag: EF_ARM_MAPSYMSFIRST"
    #if header['flags'] & 0xff000000 == 0x02000000 :
      #print "Flag: EF_ARM_EABI -> Version 2"
    #elif header['flags'] & 0xff000000 == 0 :
      #print "Flag: EABI Unknown Conformance"
    #print "Flags only relevant to ARM/Thumb Architecture"
    #print ""

#=========================================================================
# Function : checkElfHeaderVersion 
#
# Arguments
#   e_version: number read from ELF header
#
# Description
#   checks version against constants
#
# Return Values:
#=========================================================================
def checkElfHeaderVersion(e_version):
  num = e_version
  if    num == const['EV_NONE']    : print "Invalid Version"
  elif  num == const['EV_CURRENT'] :
    pass
#    print "Current Version"
  else:
    print "Unknown Version: ", num

#=========================================================================
# Function : checkElfHeaderVersion 
#
# Arguments
#   e_machine: number read from ELF header
#
# Description
#   Checks if machine type is compatable with this script.
#
# Return Values:
#=========================================================================
def checkElfHeaderMachine(e_machine):
  num = e_machine
  if    num == const['EM_NONE'] :
    pass
#    print "No Machine", "Cannot Check Flags"
  elif  num == const['EM_M32'] :
    pass
#    print "AT&T WE 32100", "Cannot Check Flags"
  elif  num == const['EM_SPARC'] :
    pass
#    print "SPARC", "Cannot Check Flags"
  elif  num == const['EM_386'] :
    pass
#    print "Intel Architecture", "Cannot Check Flags"
  elif  num == const['EM_68K'] :
    pass
#    print "Motorola 68000", "Cannot Check Flags"
  elif  num == const['EM_88K'] :
    pass
#    print "Motorola 88000", "Cannot Check Flags"
  elif  num == const['EM_860'] :
    pass
#    print "Intel 80860", "Cannot Check Flags"
  elif  num == const['EM_MIPS'] :
    pass
#    print "MIPS RS3000 Big-Endian", "Cannot Check Flags"
  elif  num == const['EM_MIPS_RS4_BE'] :
    pass
#    print "MIPS RS4000 Big-Endian", "Cannot Check Flags"
  elif  num == const['EM_ARM'] :
    pass
#    print "ARM/Thumb Architecture"
  else:
    print "Machine: ", num

#=========================================================================
# Function : checkElfHeaderType 
#
# Arguments
#   e_type: number read from ELF header
#
# Description
#   Checks if machine type is compatable with this script.
#
# Return Values:
#=========================================================================
def checkElfHeaderType(e_type):
  num = e_type
  if num != const['ET_EXEC'] :
    print "ELF Type Must Be Executable to Modify"
    exit()
#  if    num == const['ET_NONE']  : print "No File Type"
#  elif  num == const['ET_REL']   : print "Re-Locatable File"
#  elif  num == const['ET_EXEC']  : print "Executable File"
#  elif  num == const['ET_DYN']   : print "Shared Object File"
#  elif  num == const['ET_CORE']  : print "Core File"
#  elif  const['ET_LOPROC'] <= num <= const['ET_HIPROC']: 
#    print "Processor Specific:", str(num)
#  else:
#    print "Unknown Type:", str(num)
#    exit()

#=========================================================================
# Function : convertFromElf 
#
# Arguments
#   typeOrLen: str|int - either name of Elf32 type or length in bytes
#   byteString: to be converted to an unsigned number
#   endian: should endianness be adjusted from ELF file? 
#
# Description
#   This accounts for endianness.
#
# Return Values:
#   a number based on the given format
#=========================================================================
def convertFromElf(typeOrLen, byteString, endian=True):
  endianness = ">" #big by default, like english, MSB first
  if endian == True:
    if header['endian'] == 1:
      endianness = "<" #little
    else:
      endianness = ">" #big

  size = 0
  if typeOrLen in const:
    size = const[typeOrLen]
  else:
    size = typeOrLen
  assert size == len(byteString)

  if size == 4:
    format = "L"
  elif size == 2:
    format = "H"
  elif size == 1:
    format = "B"
  elif size == 8:
    format = "Q"
  else:
    raise Exception("bad convertFromElf input: " +
                    str(typeOrLen) + " " + byteString)

  return struct.unpack(endianness + format, byteString)[0]

#=========================================================================
# Function : convertToElf 
#
# Arguments
#   typeOrLen: str|int - either name of Elf32 type or length in bytes
#   number: unsigned number to be converted
#   endian: should endianness be adjusted to match ELF file? 
#
# Description
#   This accounts for endianness.
#
# Return Values:
#   a byte string in the correct format
#=========================================================================
def convertToElf(typeOrLen, number, endian=True):

  endianness = ">" #big by default, like english, MSB first
  if endian == True:
    if header['endian'] == 1:
      endianness = "<" #little
    else:
      endianness = ">" #big

  size = 0
  if typeOrLen in const:
    size = const[typeOrLen]
  else:
    size = typeOrLen

  if size == 4:
    format = "L"
  elif size == 2:
    format = "H"
  elif size == 1:
    format = "B"
  elif size == 8:
    format = "Q"
  else:
    raise Exception("bad convertToElf input: " +
                    str(typeOrLen) + " " + str(number))

  return struct.pack(endianness + format, number)

#=========================================================================
# Function : getSectionHeader 
#
# Arguments
#   args: ([str|int]) If empty, then return an empty SectionHeaderClass.
#                           Otherwise, if first arg is a str, return a 
#                           SectionHeaderClass by name.  If first arg is an
#                           int, return a SectionHeaderClass by index.
#                           Otherwise, return an empty SectionHeaderClass.
#
# Description
#   Wrapper around SectionHeaderClass's constructor to manage the
#   ElfShrinkState.sectionHeaderDict caches.
# Return Values:
#   a byte string in the correct format
#=========================================================================
def getSectionHeader(*args):

  global ElfShrinkState
  if len(args) != 0:
    if args[0] in ElfShrinkState.sectionHeaderDict :
      return ElfShrinkState.sectionHeaderDict[args[0]]
    else:
      return SectionHeaderClass(args[0])
  return SectionHeaderClass()

#=========================================================================
# Class : SectionHeaderClass 
#
# Arguments
#
# Description
#   Initializes data dictionary and grabs header data if index or name
#   is given to the constructor.
#
# Return Values:
#   Section Header class dictionary
#=========================================================================
class SectionHeaderClass:
  def __init__(self, *args):
    '''
    @param *args: Eats up optional parameters.  If they are given, there must
                  be two of them.  The first one is used as the path and the 
                  second the index in a call to readIndex right after the 
                  creation of the data dictionary.
                  
    Initializes data dictionary and grabs header data if index or name
    is given to the constructor.
    '''
    self.name = "NO_NAME"
    self.data = {
      'sh_name'      : 0, # index in section header string table with name
      'sh_type'      : 0, # section's contents and semantics type
      'sh_flags'     : 0,
      'sh_addr'      : 0, # address of section's first byte in image
      'sh_offset'    : 0, # offset from beginning of file to section data
      'sh_size'      : 0, # size of section's data
      'sh_link'      : 0, # section header table index link. depends on type
      'sh_info'      : 0, # depends on type
      'sh_addralign' : 0, # alignment of the entire section
      'sh_entsize'   : 0  # size of entries. only for certain types  
    }
    self.index = 0
    if len(args) != 0 :
      if (type(args[0]) is int
        and 0 <= args[0] <= header['sectionHeaderNum']):
        self.readIndex(args[0])
      elif type(args[0]) is str :
        self.getByName(args[0])
      else: assert False


  def getByName(self, name):
    for i in range(header['sectionHeaderNum'] - 1, 0, -1) :
      self.readIndex(i)
      if self.name == name : return
    print "SectionHeaderClass : getByName(", name, "): Bad Name"
    assert False

  def readIndex(self, index):
    '''
    @param path: The path to the file to read through
    @param index: The index of the section header to read.  File offset is 
                  calculated based off of this.  0 is the null header.
    Gets header data by index.  Also assigns index to object.  Not all data
    is needed and so is not read to save time.
    '''
    global ElfShrinkState
    assert 0 <= index <= header['sectionHeaderNum']
    offset = header['sectionHeaderOff'] + index * header['sectionHeaderSize']
    try :
      input = open(ElfShrinkState.path, 'rb')
    except IOError:
      print "Error: Could not open Input, elf file", ElfShrinkState.path
      exit(1)
    else:
      input.seek(offset)
      bytes = input.read(header['sectionHeaderSize'])
      self.data['sh_name'] = convertFromElf('Elf32_Word', bytes[0:4])
#      self.data['sh_type'] = convertFromElf( 'Elf32_Word', bytes[4:8] )
#      self.data['sh_flags'] = convertFromElf( 'Elf32_Word', bytes[8:12] )
      self.data['sh_addr'] = convertFromElf('Elf32_Addr', bytes[12:16])
      self.data['sh_offset'] = convertFromElf('Elf32_Off', bytes[16:20])
      self.data['sh_size'] = convertFromElf('Elf32_Word', bytes[20:24])
      self.data['sh_link'] = convertFromElf( 'Elf32_Word', bytes[24:28] )
#      self.data['sh_info'] = convertFromElf( 'Elf32_Word', bytes[28:32] )
      self.data['sh_addralign'] = convertFromElf('Elf32_Word', bytes[32:36])
#      self.data['sh_entsize'] = getFromElf( 'Elf32_Word', bytes[36:40] )
      input.close()
    self.index = index
    # need this check in case this header is used to find sectionNameData
    if ElfShrinkState.sectionNameData is not None :
      self.name = getString(ElfShrinkState.sectionNameData, self.data['sh_name'])

  def printData(self):
    print "SECTION HEADER:", self.index, self.name
    flags = self.data['sh_flags']
    if flags & 0x1 != 0 :         print "FLAG: SHF_WRITE"
    if flags & 0x2 != 0 :         print "FLAG: SHF_ALLOC"
    if flags & 0x4 != 0 :         print "FLAG: SHF_EXECINSTR"
    if flags & 0xf0000000 != 0 :  print "FLAG: SHF_MASKPROC"

    type = self.data['sh_type']
    if   type == const['SHT_NULL']:     print "TYPE: SHT_NULL"
    elif type == const['SHT_PROGBITS']: print "TYPE: SHT_PROGBITS"
    elif type == const['SHT_SYMTAB']:   print "TYPE: SHT_SYMTAB"
    elif type == const['SHT_STRTAB']:   print "TYPE: SHT_STRTAB"
    elif type == const['SHT_RELA']:     print "TYPE: SHT_RELA"
    elif type == const['SHT_HASH']:     print "TYPE: SHT_HASH"
    elif type == const['SHT_DYNAMIC']:  print "TYPE: SHT_DYNAMIC"
    elif type == const['SHT_NOTE']:     print "TYPE: SHT_NOTE"
    elif type == const['SHT_NOBITS']:   print "TYPE: SHT_NOBITS"
    elif type == const['SHT_REL']:      print "TYPE: SHT_REL"
    elif type == const['SHT_SHLIB']:    print "TYPE: SHT_SHLIB"
    elif type == const['SHT_DYNSYM']:   print "TYPE: SHT_DYNSYM"
    elif const['SHT_LOPROC'] <= type <= const['SHT_HIPROC']:
      print "TYPE: PROCESSOR SPECIFIC"
    elif const['SHT_LOUSER'] <= type <= const['SHT_HIUSER']:
      print "TYPE: RESERVED FOR APPLICATIONS"

    for key in self.data :
      if key != "sh_flags" and key != "sh_type" :
        print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Class : ProgramHeader 
#
# Arguments
#
# Description: Program Header Dictionary
#
# Return Values:
#=========================================================================
class ProgramHeader:
  def __init__(self, *args):
    self.data = {
      'p_type'    : 0,
      'p_offset'  : 0,
      'p_vaddr'   : 0,
      'p_paddr'   : 0,
      'p_filesz'  : 0,
      'p_memsz'   : 0,
      'p_flags'   : 0,
      'p_align'   : 0,
    }
    self.index = 0
    if len(args) != 0:
      self.readIndex(args[0])

  def readIndex(self, index):
    global ElfShrinkState
    assert (index < header['programHeaderNum'])

    self.offset = header['programHeaderOff'] + index * header['programHeaderSize']
    try:
      input = open(ElfShrinkState.path, 'rb')
    except IOError:
      print "Error: Could not open Input, elf file", ElfShrinkState.path
      exit(1)
    else:
      input.seek(self.offset)
      bytes = input.read(header['programHeaderSize'])
      self.data['p_type'] = convertFromElf('Elf32_Word', bytes[0:4])
      self.data['p_offset'] = convertFromElf('Elf32_Off', bytes[4:8])
      self.data['p_vaddr'] = convertFromElf('Elf32_Addr', bytes[8:12])
      self.data['p_paddr'] = convertFromElf('Elf32_Addr', bytes[12:16])
      self.data['p_filesz'] = convertFromElf('Elf32_Word', bytes[16:20])
      self.data['p_memsz'] = convertFromElf('Elf32_Word', bytes[20:24])
      self.data['p_flags'] = convertFromElf('Elf32_Word', bytes[24:28])
      self.data['p_align'] = convertFromElf('Elf32_Word', bytes[28:32])
      input.close()
    self.index = index

  def printData(self):
    print "Program Header:", self.index
    for key in self.data :
      print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Function : makeQSRobject 
#
# Arguments
#   entry: a symbol table entry object
#   args: if available, the section header object 
#
# Description : make an object with the structure data
#
# Return Values:
#   a dictionary for the object with things from entry filled in
#=========================================================================
def makeQSRobject(entry, *args):
  ''' 
  @param entry: a symbol table entry object
  @param args: if available, the section header object 
  @return: a dictionary for the object with things from entry filled in 
  '''
  d = {
       'name' : entry.name,
       'size' : entry.size,
       'address' : entry.value,
       'section' : entry.sectionIndex
      }
  if len(args) > 0:
    d['section'] = args[0].name
    d['offset'] = entry.value - args[0].data['sh_addr']
    d['elf_offset'] = d['offset'] + args[0].data['sh_offset']
  return d

#=========================================================================
# Function : makeQSRDescobject 
#
# Arguments
#   entry: QSR desc bytes
#
# Description : make an object with the Desc structure
#
# Return Values:
#   a dictionary for the object with things from entry filled in
#=========================================================================
def makeQSRDescObject(elfoffset, ssid, ssmask, line, msg_hash):
  ''' 
  @return: a dictionary for the object with things from entry filled in 
  '''
  d = {
       'elfoffset' : elfoffset,
       'ss_id' : ssid,
       'ss_mask' : ssmask,
       'line' : line,
       'qsr_msg_hash' : msg_hash
      }
  return d

#=========================================================================
# Class : SymbolTableEntry 
#
# Arguments
#    bytes: Entry in bytes
#   stringTable : To get the symbol names
#
# Description: Symbol Table Entry 
#
# Return Values:
#=========================================================================
class SymbolTableEntry:
  def __init__(self, bytes, stringTable):
    self.data = {
      'st_name'   : 0,
#      'st_value'  : 0,
#      'st_size'   : 0,
#      'st_info'   : 0,
      'st_other'  : 0,
#      'st_shndx'  : 0
    }
#    self.data['st_name'] = convertFromElf( 'Elf32_Word', bytes[0:4] )
#    self.data['st_value'] = convertFromElf( 'Elf32_Addr', bytes[4:8] )
    self.value = convertFromElf('Elf32_Addr', bytes[4:8])
#    self.data['st_size'] = convertFromElf( 'Elf32_Word', bytes[8:12] )
    self.size = convertFromElf('Elf32_Word', bytes[8:12])
#    self.data['st_info'] = convertFromElf( 1, bytes[12] ,False)
#    self.st_info = convertFromElf( 1, bytes[12] ,False)
#    self.data['st_other'] = convertFromElf( 1, bytes[13] ,False)
#    self.data['st_shndx'] = convertFromElf( 'Elf32_Half', bytes[14:16] )
    self.sectionIndex = convertFromElf('Elf32_Half', bytes[14:16])
    self.name = getString(stringTable, convertFromElf('Elf32_Word', bytes[0:4]))

  def printData(self):
    print "Symbol Table Entry:", self.name
    for key in self.data :
      print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Function : loadMapFileForMemLog 
#
# Arguments
#   file: Map file
#
# Description : Used to print the debug string size per file log
#               NA for GNU build
#
# Return Values:
#=========================================================================
def loadMapFileForMemLog(file):

  global ElfShrinkState
  
  if __debug__:
    print "Loading Map File Info for Memory Log"
  startQSRSection = 0
  entryRE = re.compile('Execution Region QSR_STRING')
  entryREsearch = entryRE.search
          
  re.sub = re.sub
  if os.path.exists(file) :
    try:
      input = open(file, 'r')
    except IOError:
      print "Could not open MAP file for memory logging"
    else:
      for line in input :
        if (startQSRSection == 1):
          if re.search('QSR_STR',line): 
            ERentry = re.split('\s+', line)
            try:
              size = int(ERentry[2],16) #Value in hex
              filename = ERentry[7]
              if ElfShrinkState.memSizeFnameSizeDict.has_key(filename) == 0:
                  ElfShrinkState.memSizeFnameSizeDict[filename] = 0
              ElfShrinkState.memSizeFnameSizeDict[filename] += size

              strPointer = int(ERentry[1],16) #Value in hex
              # Add to strPointer to filename Dict
              ElfShrinkState.memSizePtrFnameDict[strPointer] = filename
            except IndexError: 
              continue
        else:
          if entryREsearch(line):
            startQSRSection = 1
      input.close()
    if (startQSRSection):
      return 1
    else: return 0
     
  else:
    if __debug__:
      print "Warning : Map file does not exist, memory log output is not generated"
    return 0

#=========================================================================
# Function : loadMasterHash 
#
# Arguments
#   file: Master Hash file
#
# Description : Read Master Hash file
#   Ignore entries , whose hash is aligned (divisible by 4)
#
# Return Values:
#=========================================================================
def loadMasterHash(file):
  start = time.clock()
  global ElfShrinkState
  wordAlignLogStrs = []
  redundantLogStrs = []
  if __debug__:
    print "Loading Master Hash File"
  entryRE = re.compile('(\d+):(.*)')
  entryREsearch = entryRE.search
  re.sub = re.sub
  import os
  if os.path.exists(file) :
    openType = ''
    if REPLACEMENT_MODE == 0:
      openType = 'r+'
    else:
      openType = 'r'

    try:
      input = open(file, openType)
    except IOError:
      print "Error: Cannot load Hash file,", file
      exit(1)
    else:
      for line in input :
        match = entryREsearch(line)
        if match is None : continue
        hashNum = int(match.group(1))
        hash = convertToElf(4, hashNum, endian=False)
        assert convertFromElf('Elf32_Word', hash, False) == hashNum #sanity check
        string = match.group(2)

        #test for old hashes that could be word-aligned str*
        if hashNum % 4 == 0:
          wordAlignLogStrs += [ \
               " ".join(["\n"]),
               " ".join(["Word aligned hashes in Master Hash File - Will be hashed again"]),
               " ".join(["String:", string]),
               " ".join(["Hash :", hex(hashNum), str(hashNum)])
               ]

          continue

        #if string in ElfShrinkState.stringHashDict :
        #  redundantLogStrs += [ \
        #       " ".join(["\n"]),
        #       " ".join(["Redundant String in Master Hash File"]),
        #       " ".join(["String:", string]),
        #       " ".join(["Hash 1:", hex(hashNum), str(hashNum)]),
        #       " ".join(["Hash 2:", hex(convertFromElf('Elf32_Word',
        #                                   ElfShrinkState.stringHashDict[string], False)),
        #         str(convertFromElf('Elf32_Word', ElfShrinkState.stringHashDict[string], False))])
        #       ]

        #if REPLACEMENT_MODE == 0:
        #  if hash in ElfShrinkState.hashStringDict :
        #    redundantLogStrs += [ \
        #         " ".join(["\n"]),
        #         " ".join(["Redundant Hash in Master Hash File"]),
        #         " ".join(["Hash:", hex(convertFromElf('Elf32_Word', hash, False)),
        #           str(convertFromElf('Elf32_Word', hash, False))]),
        #         " ".join(["String 1:", string]),
        #         " ".join(["String 2:", str(ElfShrinkState.hashStringDict[hash])])
        #         ]

        # if string has '\n', treat as Windows and Unix (0d 0a and 0a)
        # only save Unix version in ElfShrinkState.hashStringDict
        string = re.sub(r'\\n', '\x0a', string)
        if REPLACEMENT_MODE == 0:
          ElfShrinkState.hashStringDict[hash] = string
        ElfShrinkState.stringHashDict[string] = hash
      input.close()
  else :
    # master hash file does not exist.  create it.
    #with open(file, 'w') as output :
    #  pass
    print("CANNOT LOAD HASH FILE : QShrink Failed")
    exit(1)
  loadTime = time.clock() - start
  if __debug__:
    try:
      logfile = open(ElfShrinkState.logPath, 'a')
    except IOError:
      print "Error: Could not open logfile ",ElfShrinkState.logPath
      exit(1)
    else:
      for strn in wordAlignLogStrs:
        logfile.write(strn)
        logfile.write("\n")
      for strn in redundantLogStrs:
        logfile.write(strn)
        logfile.write("\n")
      logfile.close()
    print "Finished Loading Master Hash File (", loadTime, "seconds)"

#=========================================================================
# Function : updateMasterHashfromSection
#
# Arguments
#   file: Master Hash file
#
# Description : Update Master Hash file with new strings from 
#               QSR_STRING section in Elf file
#
# Return Values:
#=========================================================================
def updateMasterHashfromSection():
  start = time.clock()
  global ElfShrinkState,QSR_STRING_START, QSR_STRING_END

  strOffset = 0
  
  # for QSHrink version 3 , find the SS ID, mask description table
  
  symbolTableData = getSectionData('.symtab')
# all symbol data
  stringTableData = getSectionData('.strtab')
# all symbol names
  
  QSRTblRE = re.compile(r'msg_v3_ss_table')
  
  qsr_string_length = QSR_STRING_SIZE;
  # iterate through entries in the symbol table (uses a generator) to get the 
  # structure size
  for entry in symbolTableEntryGenerator(symbolTableData, stringTableData):
    # check for a QSR struct symbol
    if (QSRTblRE.match(entry.name)):
      secHead = getSectionHeader(entry.sectionIndex)
      new_struct = makeQSRobject(entry, secHead);
      ElfShrinkState.diagDescTbl = new_struct['elf_offset']
      inputfile = open(ElfShrinkState.path, 'r+b')
      inputfile.seek(ElfShrinkState.diagDescTbl)  
      bytes = inputfile.read(4);
      Tableaddr = convertFromElf('Elf32_Word', bytes[0:4])
        
      if (Tableaddr < QSR_STRING_END) and (Tableaddr !=0):
        qsr_string_length = Tableaddr - QSR_STRING_START  
      break
     
  while strOffset < qsr_string_length :
    string = getString(ElfShrinkState.bytesDict['QSR_STRING'], strOffset)
    string = re.sub('\x0d', r'', string)
    string = re.sub(r"\\", r" ", string)
    strOffset += len(string) + 1 #for null terminator
    

    if (len(string) == 0):
      continue
    if (string not in ElfShrinkState.stringHashDict) :
      hash = toHash(string)
      # replace new lines (all unix (0x0a) internally) with special chars to 
      # avoid partial strings in future Master Hash load
      string = re.sub('\x0a', r'\\n', string)

      ElfShrinkState.outputHash.write("".join([
                      str(convertFromElf('Elf32_Word', hash, endian=False)),
                      ":", string, "\n"]))
      ElfShrinkState.newHashedStrings += 1 # increment new strings counter

if __name__ == '__main__':
  # debugging run
  # typical use should use Qshrink.py
  ElfShrinkState.anomalyCount = 0

  ElfShrinkState.outputPath = "SCTUUS.elf.temp"
  ElfShrinkState.hashPath = "hash.txt"
  ElfShrinkState.sclPath = "scl.scl"
  ElfShrinkState.logPath = "log.log"
  ElfShrinkState.anomalyListPath = "anom.log"

  global path
  #path = r"K:\M9600BSCAQDAFD10301110_2\AMSS\products\9X00\build\ms\dsp2.elf"
  
  initializeWithElfHeader()
  # get the symbol table and the string table to find symbol names
  symbolTableData = getSectionData('.symtab')
  # all symbol data
  stringTableData = getSectionData('.strtab')
  print "Finished"
  
  
