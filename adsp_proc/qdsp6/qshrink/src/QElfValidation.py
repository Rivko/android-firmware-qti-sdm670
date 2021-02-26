'''
=============================================================================

                  Q E L F V A L I D A T I O N . P Y

Copyright (c) 2010 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Elf modification validation.
=============================================================================

=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order
$Header: //components/rel/dspcore.adsp/4.0/qshrink/src/QElfValidation.py#1 $
$Date: 2017/03/30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13/03/13   sh      Added QShrink 3.0 support
07/21/11   sh      QShrink20 commercial version release
08/12/10   kf      initial version: verify structs have good hashes or ptrs


=============================================================================
'''
import os
import shutil

qversion = 0


#=========================================================================
# Function : validateStructs 
#
# Arguments
#   outputFile: Output elf file to be validated
#   hashFile: Master Hash file
#
# Description : Go through all the MSG_V2 structure in output Elf File
#    and verify the hashes and strings
#
# Return Values:
#=========================================================================
def validateStructs(outputFile, hashFile):
  import QShrinkElf
  global qversion
  
  goodHashes = 0
  goodStrings = 0
  badHashes = 0
  badStrings = 0
  goodSSID = 0
  badSSID = 0

  QShrinkElf.initializeArmElfShrinkState(outputFile, "VAL_out", hashFile,
                              "VAL_log", "VAL_anom")
  
  QShrinkElf.REPLACEMENT_MODE = 0 #so hashStringDict is populated

  if __debug__:
    Validate_Log = open("qsr_validate_elf.log", 'w')
    Validate_Log.write("".join(["MSG V2 struct analysis in shrunk Elf File, ", outputFile, "\n", "\n"]))
  
  QShrinkElf.initializeWithElfHeader()
  QShrinkElf.loadMasterHash(hashFile)

  structs = getStructs()
  if qversion == 3:
    DescTable = getDescTable(outputFile)
  
  #sort into sections
  structs.sort(cmp=lambda x, y: cmp(x['address'], y['address']))
  
  for struct in structs:
    # loop over structs doing stuff
    
    if struct['section'] not in QShrinkElf.ElfShrinkState.bytesDict:
      QShrinkElf.ElfShrinkState.bytesDict[struct['section']] = \
        QShrinkElf.getSectionData(struct['section'])
      # load new section
      
    if struct['version']  == 2:
      pointer = QShrinkElf.convertFromElf('Elf32_Addr',
                        QShrinkElf.ElfShrinkState.bytesDict[struct['section']][
                            struct['offset'] + QShrinkElf.QSR_POINTER_OFFSET :
                            struct['offset'] + QShrinkElf.QSR_POINTER_OFFSET +
                            QShrinkElf.const['Elf32_Addr']], endian=True)      
      ss_id = QShrinkElf.convertFromElf('Elf32_Half',
                 QShrinkElf.ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + 2 :
                          struct['offset'] + 4], endian=True)
      ss_mask = QShrinkElf.convertFromElf('Elf32_Word',
                 QShrinkElf.ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + 4 :
                          struct['offset'] + 8], endian=True)
   
    if struct['version'] == 3:
      pointer = QShrinkElf.convertFromElf('Elf32_Addr',
                        QShrinkElf.ElfShrinkState.bytesDict[struct['section']][
                            struct['offset'] + QShrinkElf.MSG_V3_QSR_POINTER_OFFSET :
                            struct['offset'] + QShrinkElf.MSG_V3_QSR_POINTER_OFFSET +
                            QShrinkElf.const['Elf32_Addr']], endian=True)
      ss_index = QShrinkElf.convertFromElf('Elf32_Half',
                   QShrinkElf.ElfShrinkState.bytesDict[struct['section']][
                            struct['offset'] + QShrinkElf.MSG_V3_SS_INDEX_OFFSET :
                            struct['offset'] + QShrinkElf.MSG_V3_SS_INDEX_OFFSET +
                            2], endian=True)
                            
   
    if pointer & 0x3 == 0:
      #test str pointer
      pointerOffset = pointer - QShrinkElf.QSR_STRING_START
      if (pointerOffset >= 0 and 
          pointerOffset < QShrinkElf.QSR_STRING_SIZE):
        #test string
        #@TODO regular expression match to make sure it follows what Diag code expects "filename:string\0"
        strSection = QShrinkElf.getSectionFromVA(pointer)
        string = QShrinkElf.investigateString(pointer, strSection)
        if __debug__:
          if struct['version'] == 3:
            Validate_Log.write("QSR_V3 ")
          elif struct['version'] == 2:
            Validate_Log.write("QSR_V2 ")
          Validate_Log.write("STRING ")
          Validate_Log.write(hex(pointer))
          Validate_Log.write(" ")
          Validate_Log.write(string)
        if string is None:
          print "BADSTRING in " 
          print pointer
          badStrings += 1
        else:
          goodStrings += 1
      else:
        #then its pointing outside qsr section
        #@TODO test to see if it points to an actual string
        #@TODO test to see if it has a valid Vitrual Address
        #@TODO print bad string data to a log for debugging (Virtual Address, offset into ELF file, struct info... everything)
        if __debug__:
          Validate_Log.write("BADSTRING ")
          Validate_Log.write(hex(pointer))
          Validate_Log.write("\n")
          Validate_Log.write("\n")
        badStrings += 1
    else:
      #test hash
      hash = QShrinkElf.convertToElf('Elf32_Word', pointer, endian=False)
      if hash in QShrinkElf.ElfShrinkState.hashStringDict:
        if __debug__:        
          if struct['version'] == 3:
             Validate_Log.write("QSR_V3 ")
          elif struct['version'] == 2:
             Validate_Log.write("QSR_V2 ")
          Validate_Log.write("HASH ")
          Validate_Log.write(hex(pointer))
          Validate_Log.write(" ")
          Validate_Log.write(QShrinkElf.ElfShrinkState.hashStringDict[hash])
        goodHashes += 1
      else:
        #@TODO print bad hash data to a log for debugging (Virtual Address, offset into ELF file, struct info... everything)
        badHashes += 1
        
    if struct['version'] == 3:
        if __debug__:
          Validate_Log.write(" SS_INDEX ")
          Validate_Log.write(str(ss_index))
          Validate_Log.write(" SSID ")
          Validate_Log.write(str(DescTable[ss_index].ssid))    
          Validate_Log.write(" SS_MASK ")
          Validate_Log.write(hex(DescTable[ss_index].ss_mask))    
        if (ss_index not in DescTable) or (DescTable[ss_index].ssid > 20000) or (DescTable[ss_index].rt_mask != 0):
          Validate_Log.write("\n BAD SSID_INDEX ")          
          badSSID += 1        
        else:
          goodSSID += 1
    else:
        if __debug__:
          Validate_Log.write(" SSID ")
          Validate_Log.write(str(ss_id))    
          Validate_Log.write(" SS_MASK ")
          Validate_Log.write(hex(ss_mask))  
    
    if __debug__:
      Validate_Log.write("\n \n ")

  if __debug__:
    Validate_Log.write("".join(["Good Moved Strings: ", str(goodStrings), "\n"]))
    Validate_Log.write("".join(["Good Hashified Structs:", str(goodHashes), "\n"]))
    Validate_Log.write("".join(["Good SSID index:", str(goodSSID), "\n"]))
    Validate_Log.write("".join(["Bad Moved Strings:", str(badStrings), "\n"]))
    Validate_Log.write("".join(["Bad Hashified Structs:", str(badHashes), "\n"]))
    Validate_Log.write("".join(["Bad SSID Index:", str(badSSID), "\n"]))
    Validate_Log.close()
        
  print "Good Moved Strings:", goodStrings
  print "Good Hashified Structs:", goodHashes
  print "Good SSID Index:", goodSSID  
  print "Bad Moved Strings:", badStrings
  print "Bad Hashified Structs:", badHashes
  print "Bad SSID Index:", badSSID  
  
def getStructs():
  import QShrinkElf
  import re
  global qversion
  
  # get the symbol table and the string table to find symbol names
  symbolTableData = QShrinkElf.getSectionData('.symtab')
  # all symbol data
  stringTableData = QShrinkElf.getSectionData('.strtab')
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
  qsrConstRE = re.compile(r'QSR_CONST')
  structs = []
  qversion = 2
  for entry in QShrinkElf.symbolTableEntryGenerator(symbolTableData,
                                                    stringTableData):
  # check for a QSR struct symbol
    if (QShrinkElf.MSG_V3_STRUCT_SIZE != 0) and (QShrinkElf.MSG_V3_ERR_STRUCT_SIZE != 0):
      qversion = 3
    if(qsrRE.search(entry.name)):
       if (entry.size != 0) and (QShrinkElf.MSG_V2_STRUCT_SIZE == 0) and (QShrinkElf.MSG_V3_STRUCT_SIZE == 0):
         if qsr20RE.search(entry.name):           
           QShrinkElf.MSG_V2_STRUCT_SIZE = entry.size
         elif qsr30RE.search(entry.name):
           QShrinkElf.MSG_V3_STRUCT_SIZE = entry.size  
    if(errRE.search(entry.name)):
       if (entry.size != 0) and (QShrinkElf.MSG_V2_ERR_STRUCT_SIZE == 0) and (QShrinkElf.MSG_V3_ERR_STRUCT_SIZE == 0):
         if err20RE.search(entry.name):
           QShrinkElf.MSG_V2_ERR_STRUCT_SIZE = entry.size
         elif err30RE.search(entry.name):
           QShrinkElf.MSG_V3_ERR_STRUCT_SIZE = entry.size

  if (QShrinkElf.MSG_V2_STRUCT_SIZE == 0) and (QShrinkElf.MSG_V3_STRUCT_SIZE == 0):
    print "Error - No MSG_V2 structs in the Elf ! "
    return
  
  # iterate through all entries in the symbol table (uses a generator)
  for entry in QShrinkElf.symbolTableEntryGenerator(symbolTableData, 
                                                      stringTableData):
                                               
    # check for a QSR struct symbol
    if (((entry.size == QShrinkElf.MSG_V2_STRUCT_SIZE) and
      (qsr20RE.search(entry.name)))
      or ((entry.size == QShrinkElf.MSG_V2_ERR_STRUCT_SIZE) and
      (err20RE.search(entry.name)))):
      secHead = QShrinkElf.getSectionHeader(entry.sectionIndex)
      struct = QShrinkElf.makeQSRobject(entry, secHead)
      struct['version'] = 2
      structs.append(struct)

	# check for a QSR struct symbol
    if (((entry.size == QShrinkElf.MSG_V3_STRUCT_SIZE) and
      (qsr30RE.search(entry.name)))
      or ((entry.size == QShrinkElf.MSG_V3_ERR_STRUCT_SIZE) and
      (err30RE.search(entry.name)))):
      secHead = QShrinkElf.getSectionHeader(entry.sectionIndex)
      struct = QShrinkElf.makeQSRobject(entry, secHead)
      struct['version'] = 3
      structs.append(struct)
      
  return structs

def getDescTable(outputFile):
  import QShrinkElf
  import re
  global qversion
  
  # get the symbol table and the string table to find symbol names
  symbolTableData = QShrinkElf.getSectionData('.symtab')
  stringTableData = QShrinkElf.getSectionData('.strtab')
  
  QSRTblRE = re.compile(r'msg_v3_ss_table')
  QSRTblEntryRE = re.compile(r'msg_v3_ss_num_table_entries')
  DescTable = dict()
  Tableaddr = 0
  TableEntries = 0
  file = open(outputFile, 'r')
  for entry in QShrinkElf.symbolTableEntryGenerator(symbolTableData,
                                                    stringTableData):
    # check for a QSR struct symbol
    if (Tableaddr != 0) and (TableEntries !=0):
      break      
    if (QSRTblRE.match(entry.name)):
      secHead = QShrinkElf.getSectionHeader(entry.sectionIndex)
      new_struct = QShrinkElf.makeQSRobject(entry, secHead);
      diagDescTbl = new_struct['elf_offset']
      file.seek(diagDescTbl)  
      bytes = file.read(4);
      Tableaddr = QShrinkElf.convertFromElf('Elf32_Word', bytes[0:4])   
      if (Tableaddr < QShrinkElf.QSR_STRING_END) and (Tableaddr !=0):
        qsr_string_offset = Tableaddr - QShrinkElf.QSR_STRING_START         
      
    if (QSRTblEntryRE.match(entry.name)):
      secHead = QShrinkElf.getSectionHeader(entry.sectionIndex)
      new_struct = QShrinkElf.makeQSRobject(entry, secHead);
      diagDescTblentry = new_struct['elf_offset']
      file.seek(diagDescTblentry)  
      bytes = file.read(4);
      TableEntries = QShrinkElf.convertFromElf('Elf32_Word', bytes[0:4])
      
  if ( QShrinkElf.QSR_STRING_START + qsr_string_offset + TableEntries*8 > QShrinkElf.QSR_STRING_END):
    print "Error - Message Description table beyond QShrink Section End address "
    exit(1)
    
  bytes = QShrinkElf.getSectionData('QSR_STRING')
  i = 0
  for entry in DescTableEntryGenerator(bytes, TableEntries,qsr_string_offset):
    DescTable[i] = entry   
    i +=1

  return DescTable
  
def DescTableEntryGenerator(bytes,TableEntries,qsr_string_offset):
  for i in xrange(0, TableEntries) :
    address = qsr_string_offset + i*8
    yield DescTableEntry(bytes[address : address + 8],i)
    
  
class DescTableEntry:
  def __init__(self, bytes,i):
    import QShrinkElf
    self.index = i
    self.rt_mask = QShrinkElf.convertFromElf('Elf32_Half', bytes[0:2])
    self.ssid = QShrinkElf.convertFromElf('Elf32_Half', bytes[2:4])
    self.ss_mask = QShrinkElf.convertFromElf('Elf32_Word', bytes[4:8])

if __name__ == '__main__':
  # debugging run
  # typical use should use Qshrink.py
  validateStructs("q20.elf.shrunk", "./msg_hash.txt")
