#============================================================================
#
#/** @file gcc_map.py
#
# GENERAL DESCRIPTION
#   Simplify the memmap output of gnu linker to extract size/location 
#   information for easy processing
#
#  Copyright (c) 2014 - 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
#  Qualcomm Technologies Proprietary and Confidential.
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
# 06/10/16    ml       Added wider column for symbols.  Aligned header.
# 10/26/15    yg       Fix to get appropriate size for all type of mem
# 09/30/15    bh       Merge changes from yg
# 07/09/15    yg       Handle unkown symbols
# 05/30/14    yg       Handle Linux/Windows build variants
# 05/12/14    yg       Collect and print statistics
# 05/07/14    yg       Initial revision
#
#============================================================================
import os
import sys
import re
import string
import pdb

PROCESS_SKIP_TILL_MEM_MAP = 1
PROCESS_MEM_MAP  = 2
TextTotal = 0
RoDataTotal = 0
DataTotal = 0
BssTotal = 0
Addr = 0
Size = 0
PrevAddr = 0
FillerTotal = 0
prevsym = ''
SymType = 0
LineNum = 0

PATH_TYPE_NONE = 0
PATH_TYPE_WIN = 1
PATH_TYPE_LIN = 2
path_type = 0
path_seperator = ""

SYM_TYPE_TEXT  =  1
SYM_TYPE_RODATA  =  2
SYM_TYPE_BSS  =  3
SYM_TYPE_DATA  =  4
SYM_TYPE_UNKNOWN  =  5

TypeStrings = [" ", "Text", "RO", "ZI", "RW", "??"]

if len(sys.argv) < 2:
   print "Usage: gcc_map.py <gcc map file>"
   raise RuntimeError, "Usage: gcc_map.py <gcc map file>" 

source_file = sys.argv[1]

fp = open(source_file)

print "  %5s    %18s   %8s   %8s   %-60s   %s" % ("Size", "Address", "HexSize", "Type", "Symbol", "Path")

process_state = PROCESS_SKIP_TILL_MEM_MAP

while 1:
  line = fp.readline()
  if not line:
    break
  #print line
  line = line.strip()

  LineNum = LineNum + 1

#  if LineNum == 1055:
#  pdb.set_trace()

  if len(line) == 0:
    continue

  
#  Skip everything till we get to interesting point of map
  if string.find(line.upper(), "Linker script and memory map".upper()) != -1:
    process_state = PROCESS_MEM_MAP
    continue

   
#  Stop processing when map contents is finished
  if ((string.find(line, "START GROUP") != -1) or (string.find(line, "/DISCARD/") != -1)) and (TextTotal > 0):
    break

  if path_type == PATH_TYPE_NONE:
    path_seperator = "/AARCH64/"
    if string.find(line, path_seperator) != -1:
      path_type = PATH_TYPE_LIN
    else:
      path_seperator = "\\AARCH64\\"
      if string.find(line, path_seperator) != -1:
        path_type = PATH_TYPE_WIN


# Process the interesting part of the map
  if process_state == PROCESS_MEM_MAP:
    if string.find(line, "*fill*") != -1:   # Skip the stuff we don't need
      fls = line.split()
      sz = int(fls[2], 16)
      FillerTotal = FillerTotal + sz
      continue
    if string.find(line, "size before relaxing") != -1:  # Skip the stuff we don't need
      continue
    if string.find(line, "SYSTEM_DEBUG_DATA") != -1:  # Skip the stuff we don't need
      continue
    

    #print line   
    #pdb.set_trace()

    words = line.split();

    
    if (words[0].find('.text.') != -1):
      SymType = SYM_TYPE_TEXT
    elif  (words[0].find('.bss.') != -1):
      SymType = SYM_TYPE_BSS
    elif (words[0].find('.rodata.') != -1):
      SymType = SYM_TYPE_RODATA
    elif (words[0].find('.data.') != -1):
      SymType = SYM_TYPE_DATA

    # ignore the sections that we are not interested in
    if ((words[0].find('.comment') != -1) or
       (words[0].find('.debug') != -1)  or
       (words[0].find('.stab') != -1)) :
      SymType = SYM_TYPE_UNKNOWN
      continue

    if ((words[0].find('.text.') != -1) or
       (words[0].find('.bss.') != -1)  or
       (words[0].find('.rodata.') != -1) or
       (words[0].find('.data.') != -1)) :
      words[0] = words[0].strip('.')
      syms = words[0].split('.')
      type = syms[0]
      prevsym = syms[1]
      if len(prevsym) < 2:
        try:
          prevsym = syms[2]
        except:
          continue
          pass
        if len(prevsym) < 2:
          prevsym = syms[3]
        prevsym = "         _" + prevsym

      if len(words) > 1:
        try:
          Addr = int(words[1], 16)
        except:
          continue
          pass
        Size = int(words[2], 16)
        paths = words[3].split(path_seperator)
        if len(paths) > 1:
          file = paths[1]
        else:
          file = ''
        if (Size > 0):
          if SymType == SYM_TYPE_TEXT:
            TextTotal = TextTotal + Size
          elif SymType == SYM_TYPE_BSS:
            BssTotal = BssTotal + Size
          elif SymType == SYM_TYPE_RODATA:
            if (Addr > 0):
              RoDataTotal = RoDataTotal + Size
            else:
              SymType = SYM_TYPE_UNKNOWN
          elif SymType == SYM_TYPE_DATA:
            DataTotal = DataTotal + Size
          if SymType != SYM_TYPE_UNKNOWN:
            print "  %5d    0x%016X   %8s   %8s   %-60s   %s" % (Size, Addr, words[2], TypeStrings[SymType], prevsym, file)

      else:
        #print ".%s.%s" % (type, prevsym)
        continue

      continue

    else:
      if len(words) == 4:
        try:
          Addr = int(words[1], 16)
        except:
          Addr = 0xa5a5a5a5
          #print 'Exception : %s' % words[1]
          pass
        try:
          Size = int(words[2], 16)
        except:
          Size = 0xa5a5a5a6
          #print 'Exception : %s' % words[2]
          pass
          continue
        paths = words[3].split(path_seperator)
        if len(paths) > 1:
          file = paths[1]
        else:
          file = ''
        if (Size > 0):
          if SymType == SYM_TYPE_TEXT:
            TextTotal = TextTotal + Size
          elif SymType == SYM_TYPE_BSS:
            BssTotal = BssTotal + Size
          elif SymType == SYM_TYPE_RODATA:
            if (Addr > 0):
              RoDataTotal = RoDataTotal + Size
            else:
              SymType = SYM_TYPE_UNKNOWN
          elif SymType == SYM_TYPE_DATA:
            DataTotal = DataTotal + Size
          if SymType != SYM_TYPE_UNKNOWN:
            print "  %5d    0x%016X   %8s   %8s  %60s    %s" % (Size, Addr, words[2], TypeStrings[SymType], '', file)
          Addr = 0x3434

      elif len(words) == 3:
        try:
          Addr = int(words[0], 16)
        except:
          Addr = 0xa5a5a5a7
          #print 'Exception : %s' % words[0]
          continue
          pass
        Size = int(words[1], 16)
        paths = words[2].split(path_seperator)
        if len(paths) > 1:
          file = paths[1]
        else:
          file = ''

        if (Size > 0):
          if SymType == SYM_TYPE_TEXT:
            TextTotal = TextTotal + Size
          elif SymType == SYM_TYPE_BSS:
            BssTotal = BssTotal + Size
          elif SymType == SYM_TYPE_RODATA:
            if (Addr > 0):
              RoDataTotal = RoDataTotal + Size
            else:
              SymType = SYM_TYPE_UNKNOWN
          elif SymType == SYM_TYPE_DATA:
            DataTotal = DataTotal + Size
          if SymType != SYM_TYPE_UNKNOWN:
            print "  %5d    0x%016X   %8s   %8s   %-60s   %s" % (Size, Addr, words[1], TypeStrings[SymType], prevsym, file)

      elif len(words) == 2:
        try:
          adr = int(words[0], 16)
        except:
          adr = 0xa5a5a5a3
          #print 'Exception : %s' % words[0]
          continue
          pass
        sym = words[1]
        if Addr != adr:
          if SymType != SYM_TYPE_UNKNOWN:
            print "           0x%016X                          %s" % (adr, sym)


  #print words
print "\n\n"
print "           Totals        : Size     in Hex"
print "           Text          : %6d      0x%05X" % (TextTotal, TextTotal)
print "           BSS           : %6d      0x%05X" % (BssTotal, BssTotal)
print "           RO Data       : %6d      0x%05X" % (RoDataTotal, RoDataTotal)
print "           RW Data       : %6d      0x%05X" % (DataTotal, DataTotal)
print "           Filler        : %6d      0x%05X" % (FillerTotal, FillerTotal)

# This size number should match the last address location of the image since the base is 0.
# there is small descrepency, that it doens't match, but its usable
# TODO: fix the leaked size
GTotal = TextTotal + BssTotal + RoDataTotal + DataTotal + FillerTotal

print "           Grand Total   : %6d      0x%05X" % (GTotal, GTotal)

