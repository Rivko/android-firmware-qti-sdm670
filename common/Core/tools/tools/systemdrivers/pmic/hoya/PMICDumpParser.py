#!/usr/bin/env python
#===========================================================================
#
#  @file PMICDumpParser.py
#  @brief Python script for parsing a PMIC register dump into a human-
#         readable format.
#
#  This file can be invoked directly by calling:
#
#    PMICDumpParser.py --flat=<address_file> --file=<dump_file> [ --output=<output_file> ]  [ --devidx=<pmic_index> ] [ --adb ] [ --sid=<slave_id> ]
#
#    --output argument is optional and will default to stdout
#    --devidx argument is optional and will default to 0
#    --adb argument is only needed if the dump file is generated through adb
#    --sid argument is optional and will be considered only with --adb argument.
#
#    Usage examples:   
#      python PMICDumpParser.py --flat=CORE_ADDRESS_FILE_CUSTOMER.FLAT --file=pmicdump.xml --output=parsed_dump.txt --devidx=0
#
#      For adb pmic dumps:
#      python PMICDumpParser.py --flat=CORE_ADDRESS_FILE_CUSTOMER.FLAT --file=pmicdump.xml --output=parsed_dump.txt --adb --sid=2
#
#  ===========================================================================
#
#  Copyright (c) 2011-2016 QUALCOMM Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.glue/1.2/tools/tools/systemdrivers/pmic/hoya/PMICDumpParser.py#2 $
#  $DateTime: 2017/11/15 17:28:44 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

# ============================================================================
# Imports
# ============================================================================

import re
import csv
import sys
import getopt
import xml.etree.ElementTree as ET

g_silent = False


# ============================================================================
# LogInfo
#
# Prints a message unless "silent" mode is in use.
# ============================================================================

def LogInfo (log):
  global g_silent
  if not g_silent:
    print log

  
# ============================================================================
# LogWarning
#
# Prints a message unless "silent" mode is in use.
# ============================================================================

def LogWarning (log, line_number=None):
  global g_silent
  if not g_silent:
    print '** [Warning] ' + log

  
# ============================================================================
# LogError
#
# Prints a message unless "silent" mode is in use.
# ============================================================================

def LogError(log, line_number=None):
  global g_silent
  if not g_silent:
    print '** [Error] ' + log

  
# ============================================================================
# LoadDumpXML
#
# Parses and loads the data from the given PMIC register dump file.
# ============================================================================

def LoadDumpXML (file_name, pmic_index=0, silent=False):
  global g_silent
  g_silent = silent

  # Init variables
  register_values = {}
  metadata = {}

  LogInfo("-- Loading " + str(file_name) + "...")
  try:
    tree = ET.parse(file_name)
  except Exception as e:
    error = "Unable to parse XML file: %s" % str(e)
    metadata['error'] = error
    LogWarning(error)
    return None, metadata

  root = tree.getroot()

  # Get some metadata
  data = root.find('timestamp')
  if data != None:
    metadata['timestamp'] = data.text
  data = root.find('generator')
  if data != None:
    metadata['generator'] = data.text

  # Find the registers
  target = root.find('target')
  if target == None:
    error = "No <target> element found."
    metadata['error'] = error
    LogWarning(error)
    return None, metadata

  pmics = target.findall('pmic')
  if len(pmics) <= pmic_index:
    return register_values, metadata
  pmic = pmics[pmic_index]
  registers = pmic.findall('register')

  target_name = target.attrib.get('name', None)
  if target_name:
    metadata['target__name'] = target_name
    LogInfo("Target: %s" % target_name)

  LogInfo("PMICs Found: %d" % len(pmics))

  pmic_model = pmic.attrib.get('model', None)
  if pmic_model:
    metadata['pmic__model'] = pmic_model
    LogInfo("PMIC Model: %s" % pmic_model)

  for reg in registers:
    addr = reg.attrib.get('address', None)
    if addr == None:
      LogWarning("No address attribute for register")
      continue
    try:
      addr = int(addr, 16)
    except:
      continue

    val = reg.attrib.get('value', None)
    if val == None:
      LogWarning("No value attribute for register 0x%05x" % addr)
      continue
    try:
      val = int(val, 16)
    except:
      continue

    #print "Register: 0x%05x = 0x%04x" % (addr, val)
    register_values[addr] = val

  return register_values, metadata


# ============================================================================
# LoadDumpADB
#
# Parses and loads the data from the given PMIC register ADB dump file.
# ============================================================================

def LoadDumpADB (file_name, pmic_index=0, slave_id=0, silent=False):
  global g_silent
  g_silent = silent

  # Init variables
  register_values = {}
  adb_new_format_flag = False

  if type(file_name) in [str, unicode]:
    LogInfo("-- Loading " + str(file_name) + "...")
    try:
      fd = open(file_name, 'r')
    except:
      LogWarning("Unable to open " + str(file_name))
      return register_values
  else:
    fd = file_name

  for line in fd.readlines():
    try:
      if line.find(':') >= 0:
        # adb new format is "addr: data" and every line has data for only that specific address
        fields = line.strip().split(':')
        adb_new_format_flag = True
      else:
        # adb old format is "addr data1 data2 data3..." and every line has multiple address datas
        fields = line.split()

      if len(fields) == 0:
        continue

      addr = int(fields[0], 16)

      if adb_new_format_flag == True:
        sid = (slave_id % 2)
      else:
        sid = (addr >> 16) % 2

      addr = (sid << 16) | (addr & 0xFFFF)

      for data in fields[1:]:
          data = int(data, 16)
          register_values[addr] = data
          addr += 1
    except:
      pass

  return register_values
 

# ============================================================================
# LoadDumpDragonfly
#
# Parses and loads the data from the given PMIC Dragonfly register dump file.
# Sample input:
#
#  Channel,Frame,Time,Cycles,Status,OpCode,Command,Source,Dest,Arbitration,CBit,CmdFrame,Address,Data0,Data1,Data2,Data3,Data4,Data5,Data6,Data7,ParityLO,ParityHI,FrameParity
#  0,"1","24/07 14:09:51","08368382","_ _ _ _ _ _ _ _","38","RD_LONG","03","00","SecMas","","38","C013","01","00","00","00","00","00","00","00","00","00","_ _ _ _ _"
# ============================================================================

def LoadDumpDragonfly (file_name, sid0=0, sid1=1, silent=False):
  global g_silent
  g_silent = silent

  # Init variables
  register_values = {}

  # Load the file
  LogInfo("-- Loading " + str(file_name) + "...")
  if type(file_name) in [str, unicode]:
    try:
      reader = csv.reader(open(file_name, 'r'), dialect='excel')
    except:
      LogWarning("Unable to open " + str(file_name))
      return register_values
  else:
    reader = csv.reader(file_name, dialect='excel')

  row_index = 0
  for row in reader:
    row_index += 1

    # Skip the header row
    if row_index == 1:
      continue

    # Make sure the column count matches what we expected.
    if len(row) != 24:
      continue

    # Skip non read log lines.
    if row[6] != 'RD_LONG':
      #print "Unexpected command on line %d: %s\n" % (row_index, row[6])
      continue

    # Check for a read parity error and ignore the line if so.
    if row[21] != '00' or row[22] != '00':
      LogInfo("Failed read command on line %d: '%s'" % (row_index, row[23]))
      continue

    # Get the SID
    try:
      sid = int(row[8], 16)
    except:
      LogInfo("Invalid SID on line %d: '%s'" % (row_index, row[8]))
      continue

    # Get the address
    try:
      addr = int(row[12], 16)
    except:
      LogInfo("Invalid address on line %d: '%s'" % (row_index, row[12]))
      continue

    # Ignore unexpected SIDs.  Note this is not necessarily an error in 
    # the log as it may capture reads to multiple PMICs.
    if sid == sid1:
      addr |= 0x10000
    elif sid != sid0:
      continue

    try:
      register_values[addr] = int(row[13], 16)
    except:
      LogInfo("Invalid data byte on line %d: '%s'" % (row_index, row[13]))

  return register_values


# ============================================================================

class PMICDumpField:
  '''
  Container for a parsed PMIC Dump field.
  '''

  def __init__ (self, field, field_value):
    self.field = field
    self.field_value = field_value
    self.value = None

    for value in field.values:
      if value.value == field_value:
        self.value = value
        break


# ============================================================================

class PMICDumpRegister:
  '''
  Container for a parsed PMIC Dump register.
  '''

  def __init__ (self, register, register_value):
    self.register = register
    self.register_value = register_value
    self.fields = []

    # Parse out the field and value information
    for field in register.fields:
      field_value = (register_value & field.Mask()) >> field.Shift()
      self.fields.append(PMICDumpField(field, field_value))


# ============================================================================

class PMICDumpModule:
  '''
  Container for a parsed PMIC Dump module.
  '''

  def __init__ (self, module):
    self.module = module
    self.registers = []


# ============================================================================

def ParseDump (register_values, bases):
  '''
  Parses the data from the given PMIC register dump file.  Returns an array
  of PMICDumpModule types.
  '''

  modules = []
  for base in bases:
    for module in base.modules:
      dump_module = None
      for register in module.registers:
        if register.Address() in register_values:
          if not dump_module:
            dump_module = PMICDumpModule(module)
            modules.append(dump_module)

          value = register_values[register.Address()]
          dump_module.registers.append(PMICDumpRegister(register, value))

  return modules


# ============================================================================

def PrintDump (register_values, bases, output=sys.stdout):
  '''
  Prints the register dump into text.
  '''

  modules = ParseDump(register_values, bases)

  for module_dump in modules:
    module = module_dump.module

    output.write('-----------------------------------------------------------------\n')
    output.write('PERIPHERAL: %s @ 0x%05x\n' % (module.name, module.Address()))
    output.write('-----------------------------------------------------------------\n')

    for register_dump in module_dump.registers:
      register = register_dump.register
      output.write('%s (%s+%#x) = 0x%04x\n' % (register.Symbol(), module.name, register.Offset(), register_dump.register_value))
      for field_dump in register_dump.fields:
        field = field_dump.field
        if field.Width() > 1:
          range_str = "%2d:%2d" % (field.bit_high, field.bit_low)
        else:
          range_str = "   %2d" % field.bit_low

        if field_dump.value is not None:
          info = ' (%s)' % field_dump.value.name
        else:
          info = ''

        output.write('    [%s]  %-30s = 0x%x%s\n' % (range_str, field.name, field_dump.field_value, info))

    output.write('\n')
 

# ============================================================================
#  Globals used by the dump analysis code
# ============================================================================

# Peripheral types
PERPH_TYPE__HF_BUCK = 3
PERPH_TYPE__LDO = 4
PERPH_TYPE__VS = 5
PERPH_TYPE__FTS = 28
PERPH_TYPE__ULT_LDO = 33
PERPH_TYPE__ULT_BUCK = 34
PERPH_TYPE__GPIO = 16
PERPH_TYPE__MPP = 17
PERPH_TYPE__CLK = 6

# Default input clock frequency
INPUT_CLK_FREQ_KHZ = 19200

VRAIL_TYPES = [
  PERPH_TYPE__LDO,
  PERPH_TYPE__VS,
  PERPH_TYPE__HF_BUCK,
  PERPH_TYPE__FTS,
  PERPH_TYPE__ULT_LDO,
  PERPH_TYPE__ULT_BUCK
]

PERPH_TYPES = [
  PERPH_TYPE__LDO,
  PERPH_TYPE__VS,
  PERPH_TYPE__HF_BUCK,
  PERPH_TYPE__FTS,
  PERPH_TYPE__ULT_LDO,
  PERPH_TYPE__ULT_BUCK,
  PERPH_TYPE__GPIO,
  PERPH_TYPE__MPP,
  PERPH_TYPE__CLK
]


# ============================================================================

class RailStatus:
  '''
  Class capturing the status of a voltage rail.
  '''

  DIG_MAJOR = 0x1
  STATUS1 = 0x8
  STATUS2 = 0x9
  VOLTAGE_CTL1 = 0x40
  VOLTAGE_CTL2 = 0x41
  VSET_VALID_LB = 0x42
  VSET_VALID_UB = 0x43
  MODE_CTL_PRIM = 0x45
  MODE_CTL_SEC = 0x49
  EN_CTL = 0x46
  PIN_CTL = 0x47
  PD_CTL = 0x48
  CLK_DIV = 0x200 + 0x50

  VRAIL_RANGES = {
    # TYPE       VMin      VStep
    # HFS
    3:     { 0: [  375000,  12500 ],
             1: [ 1550000,  25000 ] },
    # LDO
    4:     { 0: [  375000,  12500 ],
             1: [  375000,  12500 ],
             2: [  750000,  12500 ],
             3: [ 1500000,  25000 ],
             4: [ 1750000,  50000 ] },
    # LN_LDO
    4.16:  { 0: [ 1380000, 120000 ],
             1: [  690000,  60000 ] },
    # FTS
    28:    { 0: [       0,   5000 ],
             1: [       0,  10000 ] },
    # FTS 2.5
    28.09: { 0: [   80000,   5000 ],
             1: [  160000,  10000 ] },
    # ULT_LDO PMOS
    33:    { 0: [ 1750000,  12500 ] },
    # ULT_LDO NMOS
    33.21: { 0: [  375000,  12500 ] },
    # ULT_BUCK CTL1,2,3
    34:    { 0: [  375000,  12500 ],
             1: [  750000,  25000 ] },
    # ULT_BUCK CTL4
    34.16: { 0: [ 1550000,  25000 ] },
  }

  def __init__ (self, base, perph_type, perph_subtype, register_values):

    self.base = base
    self.perph_type = perph_type
    self.perph_subtype = perph_subtype

    self.is_perph_unified_reg = False
    self.perph_new_mode_flag = False
    self.dig_major = 0

    base_addr = base.address

    if (self.perph_type == PERPH_TYPE__LDO and self.perph_subtype >= 48) or \
       (self.perph_type == PERPH_TYPE__HF_BUCK and self.perph_subtype >= 10) or \
       (self.perph_type == PERPH_TYPE__FTS and self.perph_subtype >= 10):
      self.is_perph_unified_reg = True

      if (self.perph_type == PERPH_TYPE__LDO and self.perph_subtype >= 83) or \
         (self.perph_type == PERPH_TYPE__HF_BUCK and self.perph_subtype >= 10) or \
         (self.perph_type == PERPH_TYPE__FTS and self.perph_subtype >= 11):
        self.perph_new_mode_flag = True
        if (self.perph_type == PERPH_TYPE__HF_BUCK and self.perph_subtype == 10):
          self.dig_major = register_values.get(base_addr + RailStatus.DIG_MAJOR, None)
          if (self.dig_major < 3):
            self.perph_new_mode_flag = False

    # Determine the vreg-ok status
    self.vreg_ok = register_values.get(base_addr + RailStatus.STATUS1, None)
    if self.vreg_ok != None:
      self.vreg_ok = (self.vreg_ok >> 7) & 0x1

    if self.is_perph_unified_reg:
      # vrange not applicable on new peripheral register interface
      vrange = 0
      vsetlb = register_values.get(base_addr + RailStatus.VOLTAGE_CTL1, None)
      vsetub = register_values.get(base_addr + RailStatus.VOLTAGE_CTL2, None)
      if vsetlb == None or vsetub == None:
        vset = None
      else:
        vset = ((vsetub << 8)|(vsetlb))

      vsetvalidlb = register_values.get(base_addr + RailStatus.VSET_VALID_LB, None)
      vsetvalidub = register_values.get(base_addr + RailStatus.VSET_VALID_UB, None)
      if vsetvalidlb == None or vsetvalidub == None:
        vsetvalid = None
      else:
        vsetvalid = ((vsetvalidub << 8)|(vsetvalidlb))
    # Default the range to 0 for ULT peripheral types since there is no VOLT_CTL1 reg.
    elif self.perph_type == PERPH_TYPE__ULT_BUCK or self.perph_type == PERPH_TYPE__ULT_LDO:
      vrange = 0
      vset = register_values.get(base_addr + RailStatus.VOLTAGE_CTL2, None)
      # For ULT_BUCK_CTL1,2,3 peripheral subtype(13) alone, the range is part of VOLT_CTL2 reg.
      # VOLT_CTL2 mask 0x60 (bit<6>:bit<5>)=(1:1) implies range = 1 else range = 0
      if perph_type == PERPH_TYPE__ULT_BUCK and perph_subtype == 13:
        if (vset & 0x60) == 0x60:
          vrange = 1
          vset = vset & 0x1F
      # For NMOS ULT LDOs, default the perph_subtype to 21 to pick the assigned correct subtype_key value.
      # ULT LDO perph_subtype value 21 implies N300_STEPPER which is the only NMOS ULT LDO having subtype value >=8.
      if perph_type == PERPH_TYPE__ULT_LDO and perph_subtype < 8:
        perph_subtype = 21
    else:
      vrange = register_values.get(base_addr + RailStatus.VOLTAGE_CTL1, None)
      vset = register_values.get(base_addr + RailStatus.VOLTAGE_CTL2, None)

    vtable = None

    subtype_key = (perph_type + perph_subtype / 100.0)
    if self.is_perph_unified_reg:
      # vmin is 0 and vstep is 1000 mV for the peripherals with new unified registers 
      vtable = [0, 1000]
    elif subtype_key in RailStatus.VRAIL_RANGES:
      vtable = RailStatus.VRAIL_RANGES[subtype_key].get(vrange, None)
    elif perph_type in RailStatus.VRAIL_RANGES:
      vtable = RailStatus.VRAIL_RANGES[perph_type].get(vrange, None)

    if vtable == None or vset == None:
      self.vlevel = None
    else:
      vmin =  vtable[0]
      vstep = vtable[1]
      self.vlevel = vmin + vset * vstep
      # Log the vset_valid value on the new peripheral rails if the rail is ON
      if self.is_perph_unified_reg and vsetvalid != None and self.vreg_ok:
        self.vlevel = vmin + vsetvalid * vstep

    # Determine the vreg-on status
    if self.is_perph_unified_reg == False:
      self.vreg_on = register_values.get(base_addr + RailStatus.STATUS2, None)
      if self.vreg_on != None:
        self.vreg_on = (self.vreg_on >> 5) & 0x1

    # Determine the perph_en status
    self.en_ctl = register_values.get(base_addr + RailStatus.EN_CTL, None)
    if self.en_ctl != None:
      self.perph_en = (self.en_ctl >> 7) & 0x1
    else:
      self.perph_en = None

    if self.is_perph_unified_reg:
      self.pin_ctl = register_values.get(base_addr + RailStatus.PIN_CTL, None)
    else:
      self.pin_ctl = None

    # Determine the mode status
    self.mode_ctl = register_values.get(base_addr + RailStatus.MODE_CTL_PRIM, None)
    if self.is_perph_unified_reg:
      self.mode_ctl_sec = register_values.get(base_addr + RailStatus.MODE_CTL_SEC, None)
    else:
      self.mode_ctl_sec = None

    # Determine the pulldown status
    self.pd_en = register_values.get(base_addr + RailStatus.PD_CTL, None)
    if self.pd_en != None:
      self.pd_en = (self.pd_en >> 7) & 0x1

    # Find the frequency.  It is actually in a different peripheral, +200 from the base.
    if self.perph_type is PERPH_TYPE__HF_BUCK or self.perph_type is PERPH_TYPE__FTS:
      self.clk_div = register_values.get(base_addr + RailStatus.CLK_DIV, None)
    else:
      self.clk_div = None

  def VLevelVolts (self):
    '''
    Return the voltage level in volts instead of the default microvolts.
    '''

    return self.vlevel / 1000000.0

  def FrequencyMHz (self):
    '''
    Return the rail frequency in MHz.  Note only applies to SMPS.
    '''

    if self.clk_div is None:
      return None

    clk_div = self.clk_div & 0xF
    clk_prediv = (self.clk_div >> 4) & 0x1

    return (INPUT_CLK_FREQ_KHZ / ((clk_prediv + 1) * (clk_div + 1))) / 1000.0

  def Frequency (self):
    '''
    Return the rail frequency as a string in MHz.  Note only applies to SMPS.
    '''

    if self.clk_div is None:
      return '-'

    clk_div = self.clk_div & 0xF
    clk_prediv = (self.clk_div >> 4) & 0x1

    return "%.3f MHz" % ((INPUT_CLK_FREQ_KHZ / ((clk_prediv + 1) * (clk_div + 1))) / 1000.0)

  def Mode (self):
    '''
    Return a string representing the decoded mode.
    '''
    mode = ''
    mode_sec = ''
    if self.is_perph_unified_reg:
      if self.mode_ctl == 7:
        mode = 'NPM'
      elif self.mode_ctl == 6 and self.perph_type != PERPH_TYPE__LDO:
        mode = 'AUTO'
      elif self.mode_ctl == 5:
        mode = 'LPM'
      elif self.mode_ctl == 4:
        mode = 'RETENTION'
      elif self.mode_ctl == 3 and self.perph_type == PERPH_TYPE__LDO:
        mode = 'BYPASS'
      elif self.mode_ctl == 2 and self.perph_type == PERPH_TYPE__LDO:
        mode = 'ACTIVE_BYPASS_NPM'
      elif self.mode_ctl == 1 and self.perph_type == PERPH_TYPE__LDO:
        mode = 'ACTIVE_BYPASS_LPM'
      else:
        mode = 'NPM(U)'
      if self.perph_new_mode_flag:
        if self.mode_ctl == 7:
          mode = 'NPM'
        elif self.mode_ctl == 6 and self.perph_type != PERPH_TYPE__LDO:
          mode = 'AUTO'
        elif self.mode_ctl == 5 and self.perph_type != PERPH_TYPE__LDO:
          mode = 'AUTO_RET'
        elif self.mode_ctl == 4:
          mode = 'LPM'
        elif self.mode_ctl == 3:
          mode = 'RETENTION'
        elif self.mode_ctl == 2 and self.perph_type == PERPH_TYPE__LDO:
          mode = 'BYPASS'
        elif self.mode_ctl == 1 and self.perph_type == PERPH_TYPE__LDO:
          mode = 'ACTIVE_BYPASS_NPM'
        elif self.mode_ctl == 0 and self.perph_type == PERPH_TYPE__LDO:
          mode = 'ACTIVE_BYPASS_LPM'
        else:
          mode = 'NPM(U)'

      if (self.pin_ctl & 0xF0) == 0:
        return mode

      if self.mode_ctl_sec == 7:
        mode_sec = 'NPM'
      elif self.mode_ctl_sec == 6 and self.perph_type != PERPH_TYPE__LDO:
        mode_sec = 'AUTO'
      elif self.mode_ctl_sec == 5:
        mode_sec = 'LPM'
      elif self.mode_ctl_sec == 4:
        mode_sec = 'RETENTION'
      elif self.mode_ctl_sec == 3 and self.perph_type == PERPH_TYPE__LDO:
        mode_sec = 'BYPASS'
      elif self.mode_ctl_sec == 2 and self.perph_type == PERPH_TYPE__LDO:
        mode_sec = 'ACTBYPNPM'
      elif self.mode_ctl_sec == 1 and self.perph_type == PERPH_TYPE__LDO:
        mode_sec = 'ACTBYPLPM'
      else:
        mode_sec = 'NPM'

      if self.perph_new_mode_flag:
        if self.mode_ctl == 7:
          mode_sec = 'NPM'
        elif self.mode_ctl == 6 and self.perph_type != PERPH_TYPE__LDO:
          mode_sec = 'AUTO'
        elif self.mode_ctl == 5 and self.perph_type != PERPH_TYPE__LDO:
          mode_sec = 'AUTO_RET'
        elif self.mode_ctl == 4:
          mode_sec = 'LPM'
        elif self.mode_ctl == 3:
          mode_sec = 'RETENTION'
        elif self.mode_ctl == 2 and self.perph_type == PERPH_TYPE__LDO:
          mode_sec = 'BYPASS'
        elif self.mode_ctl == 1 and self.perph_type == PERPH_TYPE__LDO:
          mode_sec = 'ACTBYPNPM'
        elif self.mode_ctl == 0 and self.perph_type == PERPH_TYPE__LDO:
          mode_sec = 'ACTBYPLPM'
        else:
          mode_sec = 'NPM(U)'

      mode += ' '
      if self.pin_ctl & (1 << 7):
        mode += mode_sec + '.FOLLOW_AWAKE '
      if self.pin_ctl & (1 << 6):
        mode += mode_sec + '.FOLLOW_HWEN2 '
      if self.pin_ctl & (1 << 5):
        mode += mode_sec + '.FOLLOW_HWEN1 '
      if self.pin_ctl & (1 << 4):
        mode += mode_sec + '.FOLLOW_HWEN0 '

      return mode[:-1]

    # Note the order of checking is important as certain bits have priority.
    if self.mode_ctl is None:
      return ''
    elif self.perph_type is PERPH_TYPE__LDO and self.mode_ctl & (1 << 5):
      return 'BYPASS'
    elif self.mode_ctl & (1 << 7):
      return 'NPM'

    mode = ''
    if self.mode_ctl & (1 << 6):
      mode += 'AUTO '
    if self.mode_ctl & (1 << 4):
      mode += 'NPM.FOLLOW_AWAKE '
    if self.perph_type != PERPH_TYPE__VS and self.mode_ctl & 0xF:
      if self.mode_ctl & (1 << 3):
        mode += 'NPM.FOLLOW_HWEN3 '
      if self.mode_ctl & (1 << 2):
        mode += 'NPM.FOLLOW_HWEN2 '
      if self.mode_ctl & (1 << 1):
        mode += 'NPM.FOLLOW_HWEN1 '
      if self.mode_ctl & (1 << 0):
        mode += 'NPM.FOLLOW_HWEN0 '

    if mode == '':
      return 'LPM'
    else:
      return mode[:-1]

  def Enabled (self):
    '''
    Return a string representing the decoded enabled status.
    '''

    if self.en_ctl is None:
      return ''
    elif (self.en_ctl >> 7) & 0x1:
      return 'On'
    elif self.is_perph_unified_reg:
      if (self.pin_ctl != None) and (self.pin_ctl & 0xF):
        en = ''
        if self.pin_ctl & (1 << 3):
          en += 'FOLLOW_AWAKE '
        if self.pin_ctl & (1 << 2):
          en += 'FOLLOW_HWEN2 '
        if self.pin_ctl & (1 << 1):
          en += 'FOLLOW_HWEN1 '
        if self.pin_ctl & (1 << 0):
          en += 'FOLLOW_HWEN0 '
        return en[:-1]
    elif self.en_ctl & 0xF:
      en = ''
      if self.en_ctl & (1 << 3):
        en += 'FOLLOW_HWEN3 '
      if self.en_ctl & (1 << 2):
        en += 'FOLLOW_HWEN2 '
      if self.en_ctl & (1 << 1):
        en += 'FOLLOW_HWEN1 '
      if self.en_ctl & (1 << 0):
        en += 'FOLLOW_HWEN0 '
      return en[:-1]

    return 'Off'
    

# ============================================================================

class GPIOStatus:
  '''
  Class capturing the status of a PMIC GPIO
  '''

  GPIO_MODE_CTL = 0x40
  GPIO_DIG_VIN_CTL = 0x41
  GPIO_DIG_PULL_CTL = 0x42
  GPIO_DIG_OUT_SRC_CTL = 0x44
  GPIO_DIG_OUT_CTL = 0x45
  GPIO_EN_CTL = 0x46
  GPIO_STATUS1 = 0x8

  def __init__ (self, base, perph_type, perph_subtype, register_values):

    self.base = base
    self.perph_type = perph_type
    self.perph_subtype = perph_subtype

    base_addr = base.address

    # Determine the perph_en status
    self.perph_en = register_values.get(base_addr + GPIOStatus.GPIO_EN_CTL, None)
    if self.perph_en != None:
      self.perph_en = (self.perph_en >> 7) & 0x1

    # Determine the mode:
    self.mode = register_values.get(base_addr + GPIOStatus.GPIO_MODE_CTL, None)
    if self.mode != None:
      # new GPIO register interface starts from subtype GPIO_LV (16)
      if self.perph_subtype < 16:
        self.mode = (self.mode >> 4) & 0x3

    # Determine the voltage selection:
    self.voltage_sel = register_values.get(base_addr + GPIOStatus.GPIO_DIG_VIN_CTL, None)
    if self.voltage_sel != None:
      self.voltage_sel = (self.voltage_sel >> 0) & 0x7

    # Determine the output type
    # 0: CMOS
    # 1: Open drain NMOS (only drive low)
    # 2: Open drain PMOS (only drive high)
    self.output_type = register_values.get(base_addr + GPIOStatus.GPIO_DIG_OUT_CTL, None)
    if self.output_type != None:
      self.output_type = (self.output_type >> 4) & 0x3

    # Determine the output drive strength
    self.output_drv_sel = register_values.get(base_addr + GPIOStatus.GPIO_DIG_OUT_CTL, None)
    if self.output_drv_sel != None:
      self.output_drv_sel = (self.output_drv_sel >> 0) & 0x3

    # Determine the pull:
    self.pull = register_values.get(base_addr + GPIOStatus.GPIO_DIG_PULL_CTL, None)
    if self.pull != None:
      self.pull = (self.pull >> 0) & 0x7

    # Determine the source:
    # new GPIO register interface starts from subtype GPIO_LV (16)
    if self.perph_subtype < 16:
      self.source = register_values.get(base_addr + GPIOStatus.GPIO_MODE_CTL, None)
      if self.source != None:
        self.source = (self.source >> 0) & 0xF
    else:
      self.source = register_values.get(base_addr + GPIOStatus.GPIO_DIG_OUT_SRC_CTL, None)

    # Determine the value:
    self.value = register_values.get(base_addr + GPIOStatus.GPIO_STATUS1, None)
    if self.value != None:
      self.value = (self.value >> 0) & 0x1

  def Source (self):
    if self.source is None or self.mode is 0:
      return ''
    # new GPIO register interface starts from subtype GPIO_LV (16)
    elif self.perph_subtype >= 16:
      if (self.source & 0xF) == 0:
        return '%s' % ('High' if (self.source & 0x80) else 'Low')
      elif (self.source & 0xF) == 1:
        return '%sPairedGPIO' % ('Not' if (self.source & 0x80) else '')
      elif (self.source & 0xF) == 2:
        return '%sFunction1' % ('Not' if (self.source & 0x80) else '')
      elif (self.source & 0xF) == 3:
        return '%sFunction2' % ('Not' if (self.source & 0x80) else '')
      elif (self.source & 0xF) == 4:
        return '%sFunction3' % ('Not' if (self.source & 0x80) else '')
      elif (self.source & 0xF) == 5:
        return '%sFunction4' % ('Not' if (self.source & 0x80) else '')
      elif (self.source & 0xF) > 5:
        return '%sDTEST%d' % ('Not' if (self.source & 0x80) else '', ((self.source & 0xF) - 5))
    #elif self.source is 0 or self.source is 1:
    #  return '%d' % self.source
    elif self.source is 0:
      return 'Low'
    elif self.source is 1:
      return 'High'
    elif self.source is 2:
      return 'PairedGPIO'
    elif self.source is 3:
      return 'NotPairedGPIO'
    elif self.source is 4:
      return 'Function1'
    elif self.source is 5:
      return 'NotFunction1'
    elif self.source is 6:
      return 'Function2'
    elif self.source is 7:
      return 'NotFunction2'
    elif self.source > 7:
      return '%sDTEST%d' % ('Not' if (self.source & 0x1) else '', ((self.source - 8) / 2) + 1)

  def Pull (self):
    # 000 = pull-up 30uA constant
    # 001 = pull-up 1.5uA
    # 010 = pull-up 31.5uA constant
    # 011 = pull-up 1.5uA + 30uA boost
    # 100 = pull-down 10uA
    # 101 = no pull
    # 110 = Reserved
    # 111 = Reserved
    # (HW disables pulls for modes other than input and open-drain output)
    if self.pull is None or \
       (self.mode != 0 and self.output_type != 1 and self.output_type != 2):
      return ''
    elif self.pull == 0:
      return 'PU 30uA'
    elif self.pull == 1:
      return 'PU 1.5uA'
    elif self.pull == 2:
      return 'PU 31.5uA'
    elif self.pull == 3:
      return 'Active PU 1.5uA'
    elif self.pull == 4:
      return 'PD 10uA'
    else:
      return ''

  def Mode (self):
    if self.mode is None:
      return ''
    elif self.mode is 0:
      return 'Input'
    elif self.mode is 1:
      return 'Output'
    elif self.mode is 2:
      return 'DIO'
    elif self.mode is 3:
      return 'Analog'
    else:
      return ''

  def Value (self):
    # Value only applies for input or input/output mode.
    if self.mode is 0 or self.mode is 2:
      if self.value is 0:
        return 'Low'
      else:
        return 'High'
      #return '%d' % self.value
    else:
      return ''

  def OpenDrain (self):
    return (self.output_type is 1 or self.output_type is 2)
  
  def OutputType (self):
    if self.mode is 0:
      return ''
    elif self.output_type is 0:
      return 'CMOS'
    elif self.output_type is 1:
      return 'Open Drain NMOS'
    elif self.output_type is 2:
      return 'Open Drain PMOS'
    else:
      return ''

  def DriveStrength (self):
    if self.mode is 0:
      return ''
    elif self.output_drv_sel is 1:
      return 'Low'
    elif self.output_drv_sel is 2:
      return 'Medium'
    elif self.output_drv_sel is 3:
      return 'High'
    else:
      return ''

  def Supply (self):
    if self.voltage_sel != None:
      return 'VIN%d' % self.voltage_sel
    else:
      return ''

 
# ============================================================================

class ClockStatus:
  '''
  Class capturing the status of a PMIC Clock
  '''

  CLK_EN_CTL = 0x46
  CLK_STATUS1 = 0x8

  def __init__ (self, base, perph_type, perph_subtype, register_values):

    self.base = base
    self.perph_type = perph_type
    self.perph_subtype = perph_subtype

    base_addr = base.address

    # Determine the perph_en status
    self.perph_en = register_values.get(base_addr + ClockStatus.CLK_EN_CTL, None)
    if self.perph_en != None:
      self.perph_en = (self.perph_en >> 7) & 0x1

    # Determine the value:
    self.clk_ok = register_values.get(base_addr + ClockStatus.CLK_STATUS1, None)
    if self.clk_ok != None:
      self.clk_ok = (self.clk_ok >> 7) & 0x1


# ============================================================================

class MPPStatus:
  '''
  Class capturing the status of a PMIC MPP
  '''

  MPP_MODE_CTL = 0x40
  MPP_DIG_VIN_CTL = 0x41
  MPP_DIG_PULL_CTL = 0x42
  MPP_EN_CTL = 0x46
  MPP_STATUS1 = 0x8

  def __init__ (self, base, perph_type, perph_subtype, register_values):

    self.base = base
    self.perph_type = perph_type
    self.perph_subtype = perph_subtype

    base_addr = base.address

    # Determine the perph_en status
    self.perph_en = register_values.get(base_addr + MPPStatus.MPP_EN_CTL, None)
    if self.perph_en != None:
      self.perph_en = (self.perph_en >> 7) & 0x1

    # Determine the mode:
    self.mode = register_values.get(base_addr + MPPStatus.MPP_MODE_CTL, None)
    if self.mode != None:
      self.mode = (self.mode >> 4) & 0x7

    # Determine the input voltage selection:
    self.voltage_sel = register_values.get(base_addr + MPPStatus.MPP_DIG_VIN_CTL, None)
    if self.voltage_sel != None:
      self.voltage_sel = (self.voltage_sel >> 0) & 0x7

    # Determine the pull:
    self.pull = register_values.get(base_addr + MPPStatus.MPP_DIG_PULL_CTL, None)
    if self.pull != None:
      self.pull = (self.pull >> 0) & 0x7

    # Determine the source:
    self.source = register_values.get(base_addr + MPPStatus.MPP_MODE_CTL, None)
    if self.source != None:
      self.source = (self.source >> 0) & 0xF

    # Determine the value:
    self.value = register_values.get(base_addr + MPPStatus.MPP_STATUS1, None)
    if self.value != None:
      self.value = (self.value >> 0) & 0x1

  def Source (self):
    if self.source is None or self.mode is 0:
      return ''
    elif self.source is 0:
      return 'Low'
    elif self.source is 1:
      return 'High'
    #elif self.source is 0 or self.source is 1:
    #  return '%d' % self.source
    elif self.source is 2:
      return 'Paired MPP'
    elif self.source is 3:
      return 'Not Paired MPP'
    elif self.source > 7:
      return '%sDTEST%d' % ('Not ' if self.source & 0x1 else '', (self.source / 2) + 1)

  def Pull (self):
    # Pull applies for bidirectional mode only.
    if self.pull is None or self.mode != 3:
      return ''
    elif self.pull == 0:
      return 'R600ohm'
    elif self.pull == 1:
      return 'R10kohm'
    elif self.pull == 2:
      return 'R30kohm'
    elif self.pull == 3:
      return 'Open'
    else:
      return ''

  def Mode (self):
    if self.mode is None:
      return ''
    elif self.mode is 0:
      return 'Digital Input'
    elif self.mode is 1:
      return 'Digital Output'
    elif self.mode is 2:
      return 'Digital Input/Output'
    elif self.mode is 3:
      return 'Bidirectional'
    elif self.mode is 4:
      return 'Analog Input'
    elif self.mode is 5:
      return 'Analog Output'
    elif self.mode is 6:
      return 'Current Sink'
    else:
      return ''

  def Value (self):
    # Value only applies for input or input/output mode.
    if self.mode is 0 or self.mode is 2:
      if self.value is 0:
        return 'Low'
      else:
        return 'High'
      #return '%d' % self.value
    else:
      return ''

  def Supply (self):
    # Supply only matters in digital modes
    if self.mode <= 3:
      return 'VIN%d' % self.voltage_sel
    else:
      return ''

 
# ============================================================================

def AnalyzeDump (register_values, bases):
  '''
  Perform an analysis of the voltage regulators found in the given
  register dump.  Returns an array of RailStatus elements.
  '''

  # Register offsets
  PERPH_TYPE = 0x4
  PERPH_SUBTYPE = 0x5

  perph_bases = range(0, 0x20000, 0x100)

  vrail_status = []
  gpio_status = []
  mpp_status = []
  clk_status = []

  for base_addr in perph_bases:
    perph_type_addr = base_addr + PERPH_TYPE
    perph_type = register_values.get(perph_type_addr, None)
    if perph_type not in PERPH_TYPES:
      continue

    perph_subtype_addr = base_addr + PERPH_SUBTYPE
    perph_subtype = register_values.get(perph_subtype_addr, None)

    # Ignoring the SMPS PS peripherals while displaying the status.
    if perph_subtype == None:
      continue
    if perph_type == PERPH_TYPE__HF_BUCK and (perph_subtype < 8 or perph_subtype > 10):
      continue
    if perph_type == PERPH_TYPE__FTS and perph_subtype < 8:
      continue
    if perph_type == PERPH_TYPE__ULT_BUCK and perph_subtype < 13:
      continue

    base = None
    for b in bases:
      if b.address == base_addr:
        base = b
        break

    if base is None:
      continue

    if perph_type in VRAIL_TYPES:
      vrail_status.append(RailStatus(base, perph_type, perph_subtype, register_values))
    elif perph_type is PERPH_TYPE__GPIO:
      gpio_status.append(GPIOStatus(base, perph_type, perph_subtype, register_values))
    elif perph_type is PERPH_TYPE__MPP:
      mpp_status.append(MPPStatus(base, perph_type, perph_subtype, register_values))
    elif perph_type is PERPH_TYPE__CLK:
      clk_status.append(ClockStatus(base, perph_type, perph_subtype, register_values))

  return {'rails': vrail_status, 'gpios': gpio_status, 'mpps': mpp_status,
          'clks': clk_status }


# ============================================================================

def PrintRailStatus (vrail_status, output=sys.stdout):
  '''
  Print out the voltage rail analysis information.
  '''

  if len(vrail_status) == 0:
    return

  output.write('-----------------------------------------------------------------\n')
  output.write('Power Rail Analysis:\n')
  output.write('-----------------------------------------------------------------\n')

  output.write("%-10s %-10s %-17s %-10s %-10s %-3s %-11s %-10s\n" % ("Rail", "Level", "Enabled", "VREG_OK", "VREG_ON", "PD", "Frequency", "Mode"))
  for rail in vrail_status:
    # Voltage switches do not have a voltage "level", just on/off
    if rail.perph_type == PERPH_TYPE__VS:
      out = "%-10s %-10s" % (rail.base.name, 'N/A')
    elif rail.vlevel == None:
      out = "%-10s %-10s" % (rail.base.name, '???')
    else:
      out = "%-10s %-10.5f" % (rail.base.name, rail.VLevelVolts())

    out += " %-17s" % rail.Enabled()
    out += " %-10s" % ('Yes' if rail.vreg_ok else 'No ')

    if rail.perph_type == PERPH_TYPE__LDO and rail.is_perph_unified_reg == False:
      out += " %-10s" % ('Yes' if rail.vreg_on else 'No ')
    else:
      out += " %-10s" % (' - ')

    if rail.pd_en != None:
      out += " %-3s" % ('On ' if rail.pd_en else 'Off')
    else:
      out += "  - "

    freq = rail.FrequencyMHz()
    if freq is None:
      out += " %-11s" % ('   -')
    else:
      out += " %6.3f MHz " % (freq)

    out += " %-10s" % rail.Mode()

    output.write(out + '\n')
  output.write('\n')


# ============================================================================

def PrintGPIOStatus (gpio_status, output=sys.stdout):
  '''
  Print out the GPIO analysis information.
  '''

  if len(gpio_status) == 0:
    return

  output.write('-----------------------------------------------------------------\n')
  output.write('GPIO Analysis:\n')
  output.write('-----------------------------------------------------------------\n')

  output.write("%-10s %-6s %-8s %-8s %-10s %-17s %-10s %-10s\n" % ("GPIO", "EN", "Mode", "Supply", "Pull", "Output", "Input", "DrvSel"))
  for gpio in gpio_status:
    output.write("%-10s %-6s %-8s %-8s %-10s %-17s %-10s %-10s\n" % \
      (gpio.base.name,
       'On ' if gpio.perph_en else 'Off',
       gpio.Mode() + (' (OpenDrain)' if gpio.OpenDrain() else ''),
       gpio.Supply(),
       gpio.Pull(),
       gpio.Source(),
       gpio.Value(),
       gpio.DriveStrength()))
  output.write('\n')


# ============================================================================

def PrintMPPStatus (mpp_status, output=sys.stdout):
  '''
  Print out the MPP analysis information.
  '''

  if len(mpp_status) == 0:
    return

  output.write('-----------------------------------------------------------------\n')
  output.write('MPP Analysis:\n')
  output.write('-----------------------------------------------------------------\n')

  output.write("%-10s %-6s %-16s %-8s %-10s %-10s %-10s\n" % ("MPP", "EN", "Mode", "Supply", "Pull", "Output", "Input"))
  for mpp in mpp_status:
    output.write("%-10s %-6s %-16s %-8s %-10s %-10s %-10s\n" % \
      (mpp.base.name,
       'On ' if mpp.perph_en else 'Off',
       mpp.Mode(),
       mpp.Supply(),
       mpp.Pull(),
       mpp.Source(),
       mpp.Value()))
  output.write('\n')


# ============================================================================

def PrintClockStatus (clk_status, output=sys.stdout):
  '''
  Print out the Clock analysis information.
  '''

  if len(clk_status) == 0:
    return

  output.write('-----------------------------------------------------------------\n')
  output.write('Clock Analysis:\n')
  output.write('-----------------------------------------------------------------\n')

  output.write("%-10s %-6s %-6s\n" % ("Clock", "EN", "OK"))
  for clk in clk_status:
    output.write("%-10s %-6s %-6s\n" % \
      (clk.base.name,
       'On ' if clk.perph_en else 'Off',
       'On ' if clk.clk_ok else 'Off'))
  output.write('\n')


# ============================================================================

def PrintHelp():
  '''
  Print command line help.
  '''

  print '''
-- PMICDumpParser Script --
Parses a PMIC register dump (XML or dragonfly format) into text.  Register
dumps can be captured using the PMICDump.cmm script.

PMICDumpParser.py --flat=<address_file> --file=<xml_dump_file>
                  [ --devidx=x ] [ --dragonfly ] 

  --flat    : PMIC address file.  Default is CORE_ADDRESS_FILE.FLAT in
              current directory.
  --file    : Input register dump file (XML).
  --output  : A file to output the results to.  WIll use stdout if no
              file name provided.
  --dragonfly : Indicates the log file specified by "--file" is a 
              Dragonfly log.
  --devidx  : Which device index to analyze (0=first PMIC, 1=second).
'''



# ============================================================================
# ParseCommandLine
#
# Parse command line arguments.
# ============================================================================

def ParseCommandLine (argv, config):
  try:                                
    opts, args = getopt.getopt(argv, "h", ["help", "file=", "flat=", "devidx=", "dragonfly", "output=", "adb", "sid="])
  except getopt.GetoptError:          
    print "Invalid command line."
    PrintHelp()
    sys.exit(2)                     
  for opt, arg in opts:
    if opt == '-h' or opt == '--help':
      PrintHelp()
      sys.exit()                  
    elif opt == "--file":
      config['dump_file'] = arg
    elif opt == "--flat":
      config['flat_file'] = arg
    elif opt == "--devidx":
      config['dev_index'] = int(arg)
    elif opt == "--dragonfly":
      config['dragonfly'] = True
    elif opt == "--output":
      config['output_file'] = arg
    elif opt == "--adb":
      config['adb'] = True
    elif opt == "--sid":
      config['slave_id'] = int(arg)


# ============================================================================
# Main
#
# Entry point.
# ============================================================================

if __name__ == "__main__":

  # Import the HWIOParser script
  try:
    from HWIOParser import LoadAddressFile
  except:
    LogError('Unable to import HWIOParser.py.')
    sys.exit(1)

  config = {
    'dump_file': 'pmicdump.xml',
    'flat_file': 'CORE_ADDRESS_FILE.FLAT',
    'dev_index' : 0,
    'dragonfly': False,
    'adb': False,
    'slave_id' : 0
  }

  # Parse the command line
  ParseCommandLine(sys.argv[1:], config)

  # Load the dump
  if config['dragonfly']:
    register_values = LoadDumpDragonfly(config['dump_file'])
    metadata = None
  elif config['adb']:
    register_values = LoadDumpADB(config['dump_file'], config['dev_index'], config['slave_id'])
    metadata = {}
    metadata['pmic__model'] = 'Unknown'
  else:
    register_values, metadata = LoadDumpXML(config['dump_file'], config['dev_index'])

  # If we could not load any values, error out.
  if register_values is None:
    sys.exit(1)

  # Load the address file
  bases = LoadAddressFile(config['flat_file'], use_apre=True)

  # Create an output file if requested.
  if 'output_file' in config:
    try:
      output = open(config['output_file'], 'w')
    except:
      LogError('Unable to open "%s"' % config['output_file'])
      sys.exit(1)
  else:
    output = sys.stdout

  # Attempt to get the PMIC version
  revid_all_layer = register_values.get(0x105, None)
  revid_metal = register_values.get(0x102, None)

  # Print header
  output.write('=================================================================\n')
  output.write('PMIC Register Dump Analysis\n\n')
  output.write('Filename:  %s\n' % config['dump_file'])

  output.write('PMIC:      ') 
  if 'pmic__model' in metadata:
    output.write('%s\n' % metadata['pmic__model'])
  else:
    output.write('Unknown\n')

  output.write('Version:   ') 
  if revid_all_layer != None and revid_metal != None:
    output.write('%d.%d\n' % (revid_all_layer, revid_metal))
  else:
    output.write('Unknown\n')

  if 'timestamp' in metadata:
    output.write('Timestamp: %s\n' % metadata['timestamp'])
  if 'generator' in metadata:
    output.write('Generator: %s\n' % metadata['generator'])
  output.write('=================================================================\n\n')

  # Analyze the dump
  analysis = AnalyzeDump(register_values, bases)
  PrintRailStatus(analysis['rails'], output=output)
  PrintGPIOStatus(analysis['gpios'], output=output)
  PrintMPPStatus(analysis['mpps'], output=output)
  PrintClockStatus(analysis['clks'], output=output)

  # Parse the full dump
  PrintDump(register_values, bases, output=output)

  if 'output_file' in config:
    output.close()

