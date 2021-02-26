#!/usr/bin/python
#============================================================================
#
#/** @file pt_64.py
#
# GENERAL DESCRIPTION
#   Process the 64 bit memorymap table information dumped by the cmm script
#   to represent in easy format to analyze
#
#  Copyright (c) 2014,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
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
# 05/09/17    vk       Merge updates
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
PROCESS_L2_TABLES  = 4
PROCESS_L3_TABLES  = 5

# as the debug level increases, more and more things are logged. 0 means no debug
DebugLevel = 2

#
#
#   To keep this script generic and simple, the naming convention might be
#    different from what's in spec. For 4KB page size, L0 tables in this script
#    refer to L1, L1 to L2, L2 to L3 of actual page tables. This holds good
#    for any configuration where only 3 levels of tables suffice
#

CPSR = 0;
SCTLR = 0;
TCR = 0;
TTBR0 = 0;
TTBR1 = 0;
MAIR = 0;
RegionSizeBits = 0;
ProcessState = PROCESS_REG_VALUES;
PageSizeKB = 0;
PgTblLevelsReqd = 0;
RootLevelEntryCnt = 0;
EntryAddress = 0;
PTEntriesPerPage = 0
L0_cnt = 0
L0_Table = {}
L1_Table = {}
L2_Table = {}
L3_Table = {}
tbl_entry = []
L0_RegionAddr = 0
L1_RegionAddr = 0
L2_RegionAddr = 0
L3_RegionAddr = 0
L0_addr_inc_bits = 0
L1_addr_inc_bits = 0
L2_addr_inc_bits = 0
L3_addr_inc_bits = 0
L0_addr_incr = 0
L1_addr_incr = 0
L2_addr_incr = 0
L3_addr_incr = 0
AttrIdx = ['', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']

def RegisterValues():
  print ("%-6s : %8X" % ("CPSR", CPSR)) 
  print ("  %-6s : %d" % ("EL", (CPSR >> 2) & 0x3)) 

  print ("%-6s : %8X" % ("SCTLR", SCTLR)) 
  print ("  %-6s : %4X" % ("WXN", ((SCTLR >> 19) & 0x1) )) 
  print ("  %-6s : %4X" % ("I", ((SCTLR >> 12) & 0x1) )) 
  print ("  %-6s : %4X" % ("CAM", ((SCTLR) & 0x7) )) 
  
  print ("%-6s : %8X" % ("TCR", TCR)) 
  print ("  %-6s : %8X  %s" % ("IPS", IPS, "Bits: 0=>32, 1=>36, 2=>40, 3=>42, 4=>44, 5=>48")) 
  print ("  %-6s : %8X" % ("TG1", ((TCR >> 30) & 0x7))) 
  print ("  %-6s : %8X" % ("T1SZ", ((TCR >> 16) & 0x7))) 
  print ("  %-6s : %8X (Page size %d K)" % ("TG0", TG0, PageSizeKB))
  print ("  %-6s : %8X (Region Size 2 ^ %d)" % ("T0SZ", T0SZ, RegionSizeBits)) 
  
  print ("%-6s : %8X" % ("TTBR0", TTBR0)) 
  
  print ("%-6s : %8X" % ("TTBR1", TTBR1)) 
  
  print ("%-6s : %8X" % ("MAIR", MAIR)) 
  tmp = MAIR
  for i in range(0,7):
    ls = tmp & 0xF
    ms = (tmp & 0xF0) >> 4
    if (ls != ms):
      print "Attribs different found for inner outer %d : %X  %X" % (i, ms, ls)
    if ls == 0:
      AttrIdx[i] = "Dev"
    elif ls == 4:
      AttrIdx[i] = "UC"
    elif (ls & 0xC) == 4:
      AttrIdx[i] = "WB"
    elif (ls & 0xC) == 8:
      AttrIdx[i] = "WT"
    elif (ls & 0xC) == 0xC:
      AttrIdx[i] = "WB"
    tmp = tmp >> 8

  if DebugLevel > 3:
    print ("   %-6s : %s" % ("Attr[7:4]", "Meaning")) 
    print ("   %-6s : %s" % ("0000", "Device Memory")) 
    print ("   %-6s : %s" % ("00RW", "Normal Memory, Outer Write-through transient (RW non zero)")) 
    print ("   %-6s : %s" % ("0100", "Normal Memory, Outer Non-Cacheable")) 
    print ("   %-6s : %s" % ("01RW", "Normal Memory, Outer Write-back transient (RW non zero)")) 
    print ("   %-6s : %s" % ("10RW", "Normal Memory, Outer Write-through non-transient")) 
    print ("   %-6s : %s" % ("11RW", "Normal Memory, Outer Write-back non-transient")) 
    print ("   %-6s : %s" % ("Attr[3:0]", "Meaning")) 
    print ("   %-6s : %s" % ("0000", "Refer to spec for differnt types when device memory")) 
    print ("   %-6s : %s" % ("00RW", "Normal Memory, Inner Write-through transient (RW non zero)")) 
    print ("   %-6s : %s" % ("0100", "Normal Memory, Inner Non-Cacheable")) 
    print ("   %-6s : %s" % ("01RW", "Normal Memory, Inner Write-back transient (RW non zero)")) 
    print ("   %-6s : %s" % ("10RW", "Normal Memory, Inner Write-through non-transient")) 
    print ("   %-6s : %s" % ("11RW", "Normal Memory, Inner Write-back non-transient")) 


if len(sys.argv) < 2:
   print "Usage: pt.py <Page tables dump file>"
   raise RuntimeError, "Usage: pt.py <Page tables dump file>"

source_file = sys.argv[1]

fp = open(source_file)

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
    if wlist[0]=='CPSR':
      CPSR = int(wlist[2], 16);
    elif wlist[0]=='SCTLR':
      SCTLR = int(wlist[2], 16);
    elif wlist[0]=='TCR':
      TCR = int(wlist[2], 16);
    elif wlist[0]=='TTBR0':
      TTBR0 = int(wlist[2], 16);
    elif wlist[0]=='TTBR1':
      TTBR1 = int(wlist[2], 16);
    elif wlist[0]=='MAIR':
      MAIR = int(wlist[2], 16)

    elif string.find(line, "MMU Table TTBR") != -1:
      IPS = ((TCR >> 32) & 0x7);
      TG0 = ((TCR >> 14) & 0x3)
      if TG0 == 0x0:
        PageSizeKB = 4
      elif TG0 == 0x01:
        PageSizeKB = 64
      elif TG0 == 0x10:
        PageSizeKB = 16

      T0SZ = ((TCR >> 0) & 0x3F);
      RegionSizeBits = 64 - T0SZ;
      RegisterValues()

      AddrDirectMap = 0;
      AddrBitsPerLevel = 0;
      if PageSizeKB == 4:
        AddrDirectMap = 12;
        AddrBitsPerLevel = 9; # Page size / 8 byte per entry / bits covered by total entries in a page
      elif PageSizeKB == 16:
        AddrDirectMap = 14;
        AddrBitsPerLevel = 11;
      elif PageSizeKB == 64:
        AddrDirectMap = 16;
        AddrBitsPerLevel = 13;

      PTEntriesPerPage = (1024 * PageSizeKB) / 8
      RootLevelEntryCnt = RegionSizeBits;

      PgTblLevelsReqd = 1
      RootLevelEntryCnt -= AddrDirectMap;

      if RootLevelEntryCnt > AddrBitsPerLevel:
        PgTblLevelsReqd += 1;
        RootLevelEntryCnt -= AddrBitsPerLevel;

      if RootLevelEntryCnt > AddrBitsPerLevel:
        PgTblLevelsReqd += 1;
        RootLevelEntryCnt -= AddrBitsPerLevel;

      if RootLevelEntryCnt > AddrBitsPerLevel:
        PgTblLevelsReqd += 1;
        RootLevelEntryCnt -= AddrBitsPerLevel;

      if RootLevelEntryCnt > AddrBitsPerLevel:
        PgTblLevelsReqd += 1;
        RootLevelEntryCnt -= AddrBitsPerLevel;

      L0_addr_inc_bits = RegionSizeBits - RootLevelEntryCnt;
      L0_addr_incr = 2 ** L0_addr_inc_bits
      if L0_addr_inc_bits > AddrDirectMap:
        L1_addr_inc_bits = L0_addr_inc_bits - AddrBitsPerLevel;
        L1_addr_incr = 2 ** L1_addr_inc_bits

      if L1_addr_inc_bits > AddrDirectMap:
        L2_addr_inc_bits = L1_addr_inc_bits - AddrBitsPerLevel;
        L2_addr_incr = 2 ** L2_addr_inc_bits

      if L2_addr_inc_bits > AddrDirectMap:
        L3_addr_inc_bits = L2_addr_inc_bits - AddrBitsPerLevel;
        L3_addr_incr = 2 ** L3_addr_inc_bits

      if DebugLevel > 2:
        print "L0 Addr incr : %2d bits  %8X" % (L0_addr_inc_bits, L0_addr_incr)
        print "L1 Addr incr : %2d bits  %8X" % (L1_addr_inc_bits, L1_addr_incr)
        print "L2 Addr incr : %2d bits  %8X" % (L2_addr_inc_bits, L2_addr_incr)
        print "L3 Addr incr : %2d bits  %8X" % (L3_addr_inc_bits, L3_addr_incr)
# Convert to entries from address bits
      RootLevelEntryCnt = 2 ** RootLevelEntryCnt

      if DebugLevel > 2:
        print "Page Table levels required %2d, Root level entry count %d" % (PgTblLevelsReqd, RootLevelEntryCnt)

      Root = line.split('@')
      RootDescriptorAddress = int(Root[1], 16)
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
    if DebugLevel > 3:
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
        if DebugLevel > 3:
          print L0_Table
    continue

  elif string.find (line, "L2 Table") != -1:
# Search only in the last active L1 tables, if we don't find the entry then we need to walk through
# the full tree searching for L1 entries and this entry in those L1 tables. This rides on the 
# expectation of the CMM script behaving this way (ie dump L1 table, then dump the L3 tables listed in
# L2 Tables)
    ProcessState = PROCESS_L2_TABLES;
    if DebugLevel > 3:
      print "Now process L2 tables"
    wlist[9] = wlist[9].strip(')')
    if DebugLevel > 3:
      print wlist[7]
    PTEntry = int(wlist[7], 16)
    for itm in L1_Table:
      temp_list = L1_Table[itm]
      if PTEntry == temp_list[1]:
        L2_RegionAddr = itm
        if DebugLevel > 3:
          print "Got the item : %08X : %08X   %08X" % (L2_RegionAddr, temp_list[0], temp_list[1])
        L2_Table = {}
        temp_list.append(L2_Table)
        if DebugLevel > 3:
          print L0_Table
    continue

  elif string.find (line, "L3 Table") != -1:
    ProcessState = PROCESS_L3_TABLES;
    if DebugLevel > 3:
      print "Now process L3 tables"
    wlist[9] = wlist[9].strip(')')
    if DebugLevel > 3:
      print "%8X   %8X    %8X" % int(wlist[3], 16), int(wlist[7], 16), int(wlist[9],16)
    if DebugLevel > 3:
      print wlist[7]
    PTEntry = int(wlist[7], 16)
    for itm in L2_Table:
      temp_list = L2_Table[itm]
      if PTEntry == temp_list[1]:
        L3_RegionAddr = itm
        if DebugLevel > 3:
          print "Got the item : %08X : %08X   %08X" % (L3_RegionAddr, temp_list[0], temp_list[1])
        L3_Table = {}
        temp_list.append(L3_Table)
        if DebugLevel > 3:
          print L0_Table
    continue

  if ProcessState == PROCESS_L0_TABLES:
    if DebugLevel > 3:
      print wlist

    L0_cnt = L0_cnt + 1

    #  Ignore excess entries, this simplifies cmm to have const count
    if L0_cnt <= RootLevelEntryCnt:
      PTEntryAddr = int(wlist[0], 16)
      #PTEntry = int(wlist[2],16) | (int(wlist[3],16) << 32)
      PTEntry = int(wlist[2],16)
      tbl_entry = [PTEntryAddr, PTEntry]

      L0_Table[L0_RegionAddr] = tbl_entry
      if DebugLevel > 3:
        print tbl_entry
      if DebugLevel > 3:
        print L0_Table

      L0_RegionAddr += L0_addr_incr

      if DebugLevel > 3:
        if len(L0_Table) == RootLevelEntryCnt:
          print "All L0 Page table entries in this page"
          for itm in sorted(L0_Table):
            val = L0_Table[itm]
            EntryAddr = val[0]
            RegionAddr  = itm
            PTEntry = val[1]
            if (PTEntry & 0x3) == 0 or (PTEntry & 0x3) == 2:
              stat = 'Invalid'
            if (PTEntry & 0x3) == 1:
              stat = 'Block Entry'
            if (PTEntry & 0x3) == 3:
              stat = 'Page Table'
            print "%08X : %08X   %08X   %s" % (EntryAddr, RegionAddr, PTEntry, stat)

  if ProcessState == PROCESS_L1_TABLES:
    #pdb.set_trace()
    PTEntryAddr = int(wlist[0], 16)
    #PTEntry = int(wlist[2],16) | (int(wlist[3],16) << 32)
    PTEntry = int(wlist[2],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L1_Table[L1_RegionAddr] = tbl_entry
    L1_RegionAddr += L1_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[4],16) | (int(wlist[5],16) << 32)
    PTEntry = int(wlist[3],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L1_Table[L1_RegionAddr] = tbl_entry
    L1_RegionAddr += L1_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[6],16) | (int(wlist[7],16) << 32)
    PTEntry = int(wlist[4],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L1_Table[L1_RegionAddr] = tbl_entry
    L1_RegionAddr += L1_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[8],16) | (int(wlist[9],16) << 32)
    PTEntry = int(wlist[5],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L1_Table[L1_RegionAddr] = tbl_entry
    L1_RegionAddr += L1_addr_incr

    if DebugLevel > 3:
      print wlist
    if DebugLevel > 3:
      if len(L1_Table) == PTEntriesPerPage:
        print "All L1 Page table entries in this page"
        for itm in sorted(L1_Table):
          val = L1_Table[itm]
          EntryAddr = val[0]
          RegionAddr  = itm
          PTEntry = val[1]
          if (PTEntry & 0x3) == 0 or (PTEntry & 0x3) == 2:
            stat = 'Not mapped'
          if (PTEntry & 0x3) == 1:
            stat = 'Block Entry'
          if (PTEntry & 0x3) == 3:
            stat = 'Page Table'
          print "%08X : %08X   %08X   %08X   %s" % (EntryAddr, RegionAddr, RegionAddr + L1_addr_incr, PTEntry, stat)

  if ProcessState == PROCESS_L2_TABLES:
    PTEntryAddr = int(wlist[0], 16)
    #PTEntry = int(wlist[2],16) | (int(wlist[3],16) << 32)
    PTEntry = int(wlist[2],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L2_Table[L2_RegionAddr] = tbl_entry
    L2_RegionAddr += L2_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[4],16) | (int(wlist[5],16) << 32)
    PTEntry = int(wlist[3],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L2_Table[L2_RegionAddr] = tbl_entry
    L2_RegionAddr += L2_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[6],16) | (int(wlist[7],16) << 32)
    PTEntry = int(wlist[4],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L2_Table[L2_RegionAddr] = tbl_entry
    L2_RegionAddr += L2_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[8],16) | (int(wlist[9],16) << 32)
    PTEntry = int(wlist[5],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L2_Table[L2_RegionAddr] = tbl_entry
    L2_RegionAddr += L2_addr_incr

    if DebugLevel > 3:
      print wlist
    if DebugLevel > 3:
      if len(L2_Table) == PTEntriesPerPage:
        print "All L2 Page table entries in this page"
        for itm in sorted(L2_Table):
          val = L2_Table[itm]
          EntryAddr = val[0]
          RegionAddr  = itm
          PTEntry = val[1]
          if (PTEntry & 0x3) == 0 or (PTEntry & 0x3) == 2:
            stat = 'Not mapped'
          if (PTEntry & 0x3) == 1:
            stat = 'Block Entry'
          if (PTEntry & 0x3) == 3:
            stat = 'Page Table'
          print "%08X : %08X   %08X   %08X   %s" % (EntryAddr, RegionAddr, RegionAddr + L2_addr_incr, PTEntry, stat)

  if ProcessState == PROCESS_L3_TABLES:
    PTEntryAddr = int(wlist[0], 16)
    #PTEntry = int(wlist[2],16) | (int(wlist[3],16) << 32)
    PTEntry = int(wlist[2],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L3_Table[L3_RegionAddr] = tbl_entry
    L3_RegionAddr += L3_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[4],16) | (int(wlist[5],16) << 32)
    PTEntry = int(wlist[3],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L3_Table[L3_RegionAddr] = tbl_entry
    L3_RegionAddr += L3_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[6],16) | (int(wlist[7],16) << 32)
    PTEntry = int(wlist[4],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L3_Table[L3_RegionAddr] = tbl_entry
    L3_RegionAddr += L3_addr_incr

    PTEntryAddr += 8
    #PTEntry = int(wlist[8],16) | (int(wlist[9],16) << 32)
    PTEntry = int(wlist[5],16)
    tbl_entry = [PTEntryAddr, PTEntry]
    L3_Table[L3_RegionAddr] = tbl_entry
    L3_RegionAddr += L3_addr_incr

    if DebugLevel > 3:
      print wlist
    if DebugLevel > 3:
      if len(L3_Table) == PTEntriesPerPage:
        print "All L3 Page table entries in this page"
        for itm in sorted(L3_Table):
          val = L3_Table[itm]
          EntryAddr = val[0]
          RegionAddr  = itm
          PTEntry = val[1]
          if (PTEntry & 0x3) == 0 or (PTEntry & 0x3) == 2:
            stat = 'Not mapped'
          if (PTEntry & 0x3) == 1:
            stat = 'Block Entry'
          if (PTEntry & 0x3) == 3:
            stat = 'Page Table'
          print "%08X : %08X   %08X   %08X   %s" % (EntryAddr, RegionAddr, RegionAddr + L3_addr_incr, PTEntry, stat)

if DebugLevel > 4:
  print "Now traversing the page table tree"

AttribMask = (0xF << 52) | (0x3FF << 2)

print " Start   --   End    :   Attributes          XN  PXN Cont  nG  AF  SH  AP[2:1] NS AttrIdx[2:0]"

start_addr = 0
end_addr = 0
current_mask = 0

for L0_Item in sorted(L0_Table):
  L0_list = L0_Table[L0_Item]
  L0_RegionAddr  = L0_Item
  L0_PTEntry = L0_list[1]

  if (L0_PTEntry & 0x3) == 3:
    L1_Table = L0_list[2]
    L1_RegionAddr = L0_RegionAddr

    for L1_Item in sorted(L1_Table):
      L1_list = L1_Table[L1_Item]
      L1_RegionAddr  = L1_Item
      L1_PTEntry = L1_list[1]

      if (L1_PTEntry & 0x3) == 0:
        if (L1_PTEntry & AttribMask) == current_mask:
          end_addr = L1_RegionAddr + L1_addr_incr
        else:
          if current_mask == 0:
            print "%08X -- %08X :"  % (start_addr, end_addr)
          else:
            x = (current_mask >> 54) & 0x1
            p = (current_mask >> 53) & 0x1
            c = (current_mask >> 52) & 0x1
            ng = (current_mask >> 11) & 0x1
            af = (current_mask >> 10) & 0x1
            sh = (current_mask >> 8) & 0x3
            ap = (current_mask >> 6) & 0x3
            ns = (current_mask >> 5) & 0x1
            aidx = (current_mask >> 2) & 0x7
            print "%08X -- %08X : %16X      %d    %d   %d     %d  %d    %d    %d      %d    %d  (%s)"  % (start_addr, end_addr, current_mask, x, p, c, ng, af, sh, ap, ns, aidx, AttrIdx[aidx])
          start_addr = L1_RegionAddr
          end_addr = L1_RegionAddr + L1_addr_incr
          current_mask = L1_PTEntry & AttribMask

      elif (L1_PTEntry & 0x3) == 1:
        if (L1_PTEntry & AttribMask) == current_mask:
          end_addr = L1_RegionAddr + L1_addr_incr
        else:
          if current_mask == 0:
            print "%08X -- %08X :"  % (start_addr, end_addr)
          else:
            x = (current_mask >> 54) & 0x1
            p = (current_mask >> 53) & 0x1
            c = (current_mask >> 52) & 0x1
            ng = (current_mask >> 11) & 0x1
            af = (current_mask >> 10) & 0x1
            sh = (current_mask >> 8) & 0x3
            ap = (current_mask >> 6) & 0x3
            ns = (current_mask >> 5) & 0x1
            aidx = (current_mask >> 2) & 0x7
            print "%08X -- %08X : %16X      %d    %d   %d     %d  %d    %d    %d      %d    %d  (%s)"  % (start_addr, end_addr, current_mask, x, p, c, ng, af, sh, ap, ns, aidx, AttrIdx[aidx])
          start_addr = L1_RegionAddr
          end_addr = L1_RegionAddr + L1_addr_incr
          current_mask = L1_PTEntry & AttribMask


      elif (L1_PTEntry & 0x3) == 3:
        #pdb.set_trace()
        L2_Table = L1_list[2]
        L2_RegionAddr = L1_RegionAddr

        for L2_Item in sorted(L2_Table):
          L2_list = L2_Table[L2_Item]
          L2_RegionAddr  = L2_Item
          L2_PTEntry = L2_list[1]

          if (L2_PTEntry & AttribMask) == current_mask:
            end_addr = L2_RegionAddr + L2_addr_incr
          else:
            if current_mask == 0:
              print "%08X -- %08X :"  % (start_addr, end_addr)
            else:
              x = (current_mask >> 54) & 0x1
              p = (current_mask >> 53) & 0x1
              c = (current_mask >> 52) & 0x1
              ng = (current_mask >> 11) & 0x1
              af = (current_mask >> 10) & 0x1
              sh = (current_mask >> 8) & 0x3
              ap = (current_mask >> 6) & 0x3
              ns = (current_mask >> 5) & 0x1
              aidx = (current_mask >> 2) & 0x7
              print "%08X -- %08X : %16X      %d    %d   %d     %d  %d    %d    %d      %d    %d  (%s)"  % (start_addr, end_addr, current_mask, x, p, c, ng, af, sh, ap, ns, aidx, AttrIdx[aidx])
            start_addr = L2_RegionAddr
            end_addr = L2_RegionAddr + L2_addr_incr
            current_mask = L2_PTEntry & AttribMask

      else:
        current_mask = L2_PTEntry & AttribMask
          #if (L2_PTEntry & 0x3) == 1:
          #  print "%08X -- %08X : %08X"  % (L2_RegionAddr, L2_RegionAddr + L2_addr_incr, L2_PTEntry)

          #if (L2_PTEntry & 0x3) == 3:
          #  print "%08X -- %08X : %08X"  % (L2_RegionAddr, L2_RegionAddr + L2_addr_incr, L2_PTEntry)


if current_mask == 0:
  print "%08X -- %08X :"  % (start_addr, end_addr)
else:
  x = (current_mask >> 54) & 0x1
  p = (current_mask >> 53) & 0x1
  c = (current_mask >> 52) & 0x1
  ng = (current_mask >> 11) & 0x1
  af = (current_mask >> 10) & 0x1
  sh = (current_mask >> 8) & 0x3
  ap = (current_mask >> 6) & 0x3
  ns = (current_mask >> 5) & 0x1
  aidx = (current_mask >> 2) & 0x7
  print "%08X -- %08X : %16X      %d    %d   %d     %d  %d    %d    %d      %d    %d  (%s)"  % (start_addr, end_addr, current_mask, x, p, c, ng, af, sh, ap, ns, aidx, AttrIdx[aidx])



