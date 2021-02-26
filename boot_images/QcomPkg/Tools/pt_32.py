#!/usr/bin/python
#============================================================================
#
#/** @file pt_32.py
#
# GENERAL DESCRIPTION
#   Process the 64 bit memorymap table information dumped by the cmm script
#   to represent in easy format to analyze
#
#  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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
# 05/19/14    yg       Fix missing last entry
# 05/15/14    yg       Initial revision
#
#============================================================================

import os
import sys
import re
import string
import pdb

PROCESS_REG_VALUES = 1
PROCESS_L0_TABLES  = 2
PROCESS_L1_TABLES  = 3

# as the debug level increases, more and more things are logged. 0 means no debug
DebugLevel = 4

#
#
#   To keep this script generic and simple, the naming convention might be
#    different from what's in spec. For 4KB page size, L0 tables in this script
#    refer to L1, L1 to L2, L2 to L3 of actual page tables. This holds good
#    for any configuration where only 3 levels of tables suffice
#

TTBR0 = 0;
RegionSizeBits = 0;
ProcessState = PROCESS_REG_VALUES;
PageSizeKB = 0;
PgTblLevelsReqd = 0;
RootLevelEntryCnt = 4 * 1024;
EntryAddress = 0;
PTEntriesPerPage = 256
L0_Table = {}
L1_Table = {}
tbl_entry = []
L0_RegionAddr = 0
L1_RegionAddr = 0
L0_addr_incr = (1024 * 1024)
L1_addr_incr = 4096

if len(sys.argv) < 2:
   print "Usage: pt.py <Page tables dump file>"
   raise RuntimeError, "Usage: pt.py <Page tables dump file>"

source_file = sys.argv[1]

fp = open(source_file)

def get_attr_Str (xn, b, c, ap, tx):
  tex = tx
  if tex > 3:
    tex = tex & 0x3

  if (tex == 0):
    if c == 0:
      if b == 0:
        str = "SO"
      else:
        str = "Shareable Device"
    else:
      if b == 0:
        str = "WT"
      else:
        str = "WB"
  elif (tex == 1):
    if c == 0 and b == 0:
      if tx == 5:
        str = "UC"
      else:
        str = "SO"
    elif c == 1 and b == 1:
      str = "WB"
    elif c == 1 and b == 0:
      str = "WB?"
    else:
      str = "??"
  elif (tex == 2):
    str = "Non Shared Device  "
  else:
    str = "**"
  return str;

print "Map file : %s" % source_file
while 1:
  line = fp.readline()
  if not line:
    break
  line = line.strip()
  #print line
  
  
  wlist = line.split();
  #wlist = line.split(':');
  #pdb.set_trace()

  if len(wlist) == 0:
    continue

  #slist = [item.strip() for item in wlist]
  #print wlist;
  #pdb.set_trace()

  if ProcessState == PROCESS_REG_VALUES:
    if string.find(line, "MMU Table TTBR") != -1:
      TTBR0 = int(wlist[4], 16);
      PageSizeKB = 4
      PageTableSize = 1024
      print ("%-6s : %8X" % ("TTBR0", TTBR0))
      continue

  if string.find (line, "L0 Table") != -1:
    ProcessState = PROCESS_L0_TABLES;
    L0_RegionAddr = 0
    if DebugLevel > 2:
      print "Now process L0 tables"
    if DebugLevel > 3:
      print wlist
    continue

  elif string.find (line, "L1 Table") != -1:
    ProcessState = PROCESS_L1_TABLES;
    if DebugLevel > 3:
      print "Now process L1 tables"
    wlist[9] = wlist[9].strip(')')
    if DebugLevel > 3:
      print "%8s   %8s    %8s" % (wlist[3], wlist[7], wlist[9])
    if DebugLevel > 4:
      print wlist[7]
    PTEntry = int(wlist[7], 16)
    for itm in L0_Table:
      temp_list = L0_Table[itm]
      if PTEntry == temp_list[1]:
        L1_RegionAddr = itm
        if DebugLevel > 3:
          print "Got the item : %08X : %08X   %08X" % (L1_RegionAddr, temp_list[0], temp_list[1])
        L1_Table = {}
        temp_list.append(L1_Table)
        if DebugLevel > 4:
          print L0_Table
    continue

  if ProcessState == PROCESS_L0_TABLES:
    if DebugLevel > 3:
      print wlist
    PTEntryAddr = int(wlist[0], 16)
    for i in range(2, 10):
      PTEntry = int(wlist[i],16)
      tbl_entry = [PTEntryAddr, PTEntry]
      L0_Table[L0_RegionAddr] = tbl_entry
      PTEntryAddr += 4
      L0_RegionAddr += L0_addr_incr

    if DebugLevel > 4:
      print tbl_entry
    if DebugLevel > 4:
      print L0_Table

    if DebugLevel > 3:
      if len(L0_Table) == RootLevelEntryCnt:
        print "All L0 Page table entries in this page"
        for itm in sorted(L0_Table):
          val = L0_Table[itm]
          EntryAddr = val[0]
          RegionAddr  = itm
          PTEntry = val[1]
          if (PTEntry & 0x3) == 0 or (PTEntry & 0x3) == 3:
            stat = 'Invalid'
          if (PTEntry & 0x3) == 2:
            stat = 'Section or Super section'
            PhyAddr = PTEntry & 0xFFF00000
            if RegionAddr != PhyAddr:
              print "ERROR: Wrong address mapping..!! %08X   %08X" % (RegionAddr, PhyAddr)
          if (PTEntry & 0x3) == 1:
            stat = 'Page Table'
          if PTEntry != 0:
            print "%08X : %08X   %08X   %s" % (EntryAddr, RegionAddr, PTEntry, stat)

  if ProcessState == PROCESS_L1_TABLES:
    PTEntryAddr = int(wlist[0], 16)
    for i in range(2,10):
      PTEntry = int(wlist[i],16)
      tbl_entry = [PTEntryAddr, PTEntry]
      L1_Table[L1_RegionAddr] = tbl_entry
      PTEntryAddr += 4
      L1_RegionAddr += L1_addr_incr

    if DebugLevel > 4:
      print wlist
    if DebugLevel > 3:
      if len(L1_Table) == PTEntriesPerPage:
        print "All L1 Page table entries in this page"
        for itm in sorted(L1_Table):
          val = L1_Table[itm]
          EntryAddr = val[0]
          RegionAddr  = itm
          PTEntry = val[1]
          if (PTEntry & 0x3) == 0:
            stat = 'Not mapped'
          if (PTEntry & 0x3) == 1:
            stat = 'Large Page'
            PhyAddr = PTEntry & 0xFFFF0000
          if (PTEntry & 0x3) == 2  or (PTEntry & 0x3) == 3:
            stat = 'Small Page'
            PhyAddr = PTEntry & 0xFFFFF000
          if PTEntry != 0:
            print "%08X : %08X   %08X   %08X   %s" % (EntryAddr, RegionAddr, RegionAddr + L1_addr_incr, PTEntry, stat)
            if (RegionAddr != PhyAddr):
              print "ERROR: Wrong address mapping..!! %08X   %08X" % (RegionAddr, PhyAddr)


if DebugLevel > 3:
  print "Now traversing the page table tree"

print " Start   --   End    : Attributes    XN  B  C AP[2:1]  TEX[2:0] S  nG  $Attr"

start_addr = 0
end_addr = 0
LastAttr = 0
current_mask = 0
current_attr = 0
prev_pt_entry = 0

L0_PT_Mask = 0x1FC
L0_Section_Mask = 0xFFFFC
L1_SmallPage_Mask = 0xFFF

for L0_Item in sorted(L0_Table):
  L0_list = L0_Table[L0_Item]
  L0_RegionAddr  = L0_Item
  L0_PTEntry = L0_list[1]
  #pdb.set_trace()

# L0 Section/Super section
  if (L0_PTEntry & 0x3) == 2:
    current_mask = L0_PTEntry
    b = (current_mask >> 2) & 0x1
    c = (current_mask >> 3) & 0x1
    xn = (current_mask >> 4) & 0x1
    ap = ((current_mask >> 10) & 0x3) | ((current_mask >> 13) & 0x4)
    tex = (current_mask >> 12) & 0x7
    s = (current_mask >> 16) & 0x1
    ng = (current_mask >> 17) & 0x1
    current_attr = xn | (b << 1) | (c << 2) | (ap << 3) | (s << 6) | (ng << 7) | (tex << 8)

    if LastAttr == current_attr:
      end_addr = L0_RegionAddr + L0_addr_incr
    else:
      if LastAttr == 0:
        print "%08X -- %08X :"  % (start_addr, end_addr)
      else:
        xn = (LastAttr >> 0) & 0x1
        b = (LastAttr >> 1) & 0x1
        c = (LastAttr >> 2) & 0x1
        ap = ((LastAttr >> 3) & 0x7)
        s = (LastAttr >> 6) & 0x1
        ng = (LastAttr >> 7) & 0x1
        tex = (LastAttr >> 8) & 0x7
        print "%08X -- %08X : %08X      %d    %d  %d  %d        %d      %d   %d   %s" % (start_addr, end_addr, LastAttr, xn, b, c, ap, tex, s, ng, get_attr_Str(xn, b, c, ap, tex))
      start_addr = L0_RegionAddr
      end_addr = L0_RegionAddr + L0_addr_incr
      LastAttr = current_attr

# L0 Page table
  elif (L0_PTEntry & 0x3) == 1:
    L1_Table = L0_list[2]
    L1_RegionAddr = L0_RegionAddr

    for L1_Item in sorted(L1_Table):
      L1_list = L1_Table[L1_Item]
      L1_RegionAddr  = L1_Item
      L1_PTEntry = L1_list[1]

# L1 Small Page
      if (L1_PTEntry & 0x3) == 2 or (L1_PTEntry & 0x3) == 3:
        current_mask = L1_PTEntry

        xn = (current_mask >> 0) & 0x1
        b = (current_mask >> 2) & 0x1
        c = (current_mask >> 3) & 0x1
        ap = ((current_mask >> 4) & 0x3) | ((current_mask >> 7) & 0x4)
        tex = (current_mask >> 6) & 0x7
        s = (current_mask >> 10) & 0x1
        ng = (current_mask >> 11) & 0x1
        current_attr = xn | (b << 2) | (c << 3) | (ap << 4) | (s << 5) | (ng << 6) | (ap << 7) | (tex << 10)

        if LastAttr == current_attr:
          end_addr = L1_RegionAddr + L1_addr_incr
        else:
          if LastAttr == 0:
            print "%08X -- %08X :"  % (start_addr, end_addr)
          else:
            xn = (LastAttr >> 0) & 0x1
            b = (LastAttr >> 1) & 0x1
            c = (LastAttr >> 2) & 0x1
            ap = ((LastAttr >> 3) & 0x7)
            s = (LastAttr >> 6) & 0x1
            ng = (LastAttr >> 7) & 0x1
            tex = (LastAttr >> 8) & 0x7
            print "%08X -- %08X : %08X      %d    %d  %d  %d        %d      %d   %d   %s" % (start_addr, end_addr, LastAttr, xn, b, c, ap, tex, s, ng, get_attr_Str(xn, b, c, ap, tex))
          start_addr = L1_RegionAddr
          end_addr = L1_RegionAddr + L1_addr_incr
          LastAttr = current_attr


# L1 Large Page
      elif (L1_PTEntry & 0x3) == 1:
        print "Large page not supported : %08X @ %08X  for addr %08X"  % (L1_PTEntry, L1_list[0], L1_RegionAddr)
      else:
        current_attr = 0
        if LastAttr != current_attr:
          xn = (LastAttr >> 0) & 0x1
          b = (LastAttr >> 1) & 0x1
          c = (LastAttr >> 2) & 0x1
          ap = ((LastAttr >> 3) & 0x7)
          s = (LastAttr >> 6) & 0x1
          ng = (LastAttr >> 7) & 0x1
          tex = (LastAttr >> 8) & 0x7
          print "%08X -- %08X : %08X      %d    %d  %d  %d        %d      %d   %d   %s" % (start_addr, end_addr, LastAttr, xn, b, c, ap, tex, s, ng, get_attr_Str(xn, b, c, ap, tex))
          start_addr = L1_RegionAddr
          end_addr = L1_RegionAddr + L1_addr_incr
          LastAttr = current_attr
        else:
          end_addr += L1_addr_incr
  else:
    current_attr = 0
    if LastAttr != current_attr:
      xn = (LastAttr >> 0) & 0x1
      b = (LastAttr >> 1) & 0x1
      c = (LastAttr >> 2) & 0x1
      ap = ((LastAttr >> 3) & 0x7)
      s = (LastAttr >> 6) & 0x1
      ng = (LastAttr >> 7) & 0x1
      tex = (LastAttr >> 8) & 0x7
      print "%08X -- %08X : %08X      %d    %d  %d  %d        %d      %d   %d   %s" % (start_addr, end_addr, LastAttr, xn, b, c, ap, tex, s, ng, get_attr_Str(xn, b, c, ap, tex))
      start_addr = L0_RegionAddr
      end_addr = L0_RegionAddr + L0_addr_incr
      LastAttr = current_attr
    else:
      end_addr += L0_addr_incr

if LastAttr == 0:
  print "%08X -- %08X :"  % (start_addr, end_addr)
else:
  xn = (LastAttr >> 0) & 0x1
  b = (LastAttr >> 1) & 0x1
  c = (LastAttr >> 2) & 0x1
  ap = ((LastAttr >> 3) & 0x7)
  s = (LastAttr >> 6) & 0x1
  ng = (LastAttr >> 7) & 0x1
  tex = (LastAttr >> 8) & 0x7
  print "%08X -- %08X : %08X      %d    %d  %d  %d        %d      %d   %d   %s" % (start_addr, end_addr, LastAttr, xn, b, c, ap, tex, s, ng, get_attr_Str(xn, b, c, ap, tex))


