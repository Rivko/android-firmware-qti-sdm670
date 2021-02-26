#===========================================================================
#
#  @file HWIOParser.py
#  @brief Python script for parsing an MSM address file into bases,
#         modules, and registers.  It is generally used as a module when
#         using one of the HWIOGen*.py scripts, using the LoadAddressFile
#         function.
#
#  This file can be invoked directly by calling:
#
#    HWIOParser.py [ --flat=<address_file> ]
#
#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.qdsp6/2.1.c4/bsp/hwio/build/scripts/HWIOParser.py#1 $
#  $DateTime: 2018/07/30 01:21:36 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

# ============================================================================
# Imports
# ============================================================================

import re
import sys
import getopt
import threading
import copy


# ============================================================================
# Globals
# ============================================================================

# ----------------------------------------------------------------------------
# Pattern definitions for parsing the address file
# ----------------------------------------------------------------------------

comment_pattern = re.compile('^\s*--')
base_pattern = re.compile('^\s*(\S+)\s+BASE\s+(\S+)\s+(\S+)\s+(\S+):(\S+).*$')
module_pattern = re.compile('^\s*(\S+)\s+MODULE\s+OFFSET=(\S+)\s*\+\s*(\S+)\s*(.*)$')
module_spre_pattern = re.compile('SPRE=(\S+)')
module_apre_pattern = re.compile('APRE=(\S+)')
module_spost_pattern = re.compile('SPOST=(\S+)')
register_pattern = re.compile('^\s*(\w+)\s+ADDRESS\s+(\S+)\s+(\S+).*$')
field_pattern = re.compile('^\s*(\w+)\s+BIT(.*)\[\s*(\S+)\s*\].*$')
value_pattern = re.compile('^\s*(\S+)\s+VALUE\s+(\S+).*$')
areg_pattern = re.compile('^\s*(\S+)\s+ARRAY\s+(\S+?)\+(\S+).*$')
areg_1dim_pattern = re.compile('^(\S+?)\((\S)\):\((\S+)\)-\((\S+)\).*$')
areg_2dim_pattern = re.compile('^(\S+?)\((\S),(\S)\):\((\S+),(\S+)\)-\((\S+),(\S+)\).*$')
areg_1dim_formula = re.compile('(.+)\*(.*)([a-z])(.*)')
areg_2dim_formula = re.compile('(.+)\*(.)(.+)\*(.)(.*)')
reset_value_pattern = re.compile('^\s*(\S+)\s+RESET_VALUE\s+(\S+).*$')
field_reset_value_pattern = re.compile('^\s*(\S+)\s+FLD_RESET_VALUE\s+(\S+).*$')

# ----------------------------------------------------------------------------
# General global variables
# ----------------------------------------------------------------------------

symbols = {}
silent = False
warnings = 0
saved_warnings = None
print_arg = ''
allow_duplicate_field_values = True
lock = threading.RLock()


# ============================================================================
# LogInfo
#
# Prints a message unless "silent" mode is in use.
# ============================================================================

def LogInfo (log):
  if not silent:
    print log

  
# ============================================================================
# LogWarning
#
# Prints a message unless "silent" mode is in use.
# ============================================================================

def LogWarning (log, line_number=None):
  global warnings
  global saved_warnings
  warnings += 1
  if not silent:
    if line_number != None:
      print '** [Warning L:%d] ' % (line_number) + log
    else:
      print '** [Warning] ' + log

  if saved_warnings != None:
    saved_warnings.append([line_number, log])

  
# ============================================================================
# SymbolCheck
#
# Validates that the new symbol does not conflict with an existing one.
# Also checks for unexpected case.
# ============================================================================

def SymbolCheck (symbol, fix_duplicates=False):
  global symbols

  # If we find a duplicate symbol we append an index to it like '_2'.  Loop
  # until we find an unused index.
  suffix = ''
  index = 2
  if symbol.Symbol() in symbols:
    LogWarning("Duplicate symbol (line %d): %s" % (symbols[symbol.Symbol()].line_number, symbol.name), symbol.line_number)
    if not fix_duplicates:
      raise SyntaxError
    while symbol.Symbol() in symbols:
      if not suffix == '':
        symbol.name = re.sub(suffix + '$', '', symbol.name)
      suffix = '_' + str(index)
      symbol.name = symbol.name + "_" + str(index)
      index += 1

  # We allow a single lower-case letter as this is not uncommon and used for
  # register arrays for example.  Multiple lower-case in a row generally
  # means a mis-named symbol.
  if re.search('[a-z][a-z]', symbol.name):
    LogWarning("Lower case symbol: %s" % (symbol.name), symbol.line_number)

  symbols[symbol.Symbol()] = symbol


# ============================================================================
# HWIOFieldValue
#
# Class for a field value in HWIO.
# ============================================================================

class HWIOFieldValue:

  # ----------------------------------------------------------------------
  # HWIOFieldValue:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # ----------------------------------------------------------------------

  def __init__(self, m, line_number, *args, **kwargs):
    self.name = m.group(1)
    self.valid = True
    try:
      self.value = int(m.group(2), 0)
    except ValueError:
      LogWarning('Invalid field value: %s' % (m.group(2)), line_number)
      self.value = 0
      self.valid = False
    self.line_number = line_number


# ============================================================================
# HWIOField
#
# Class for a field in HWIO.
# ============================================================================

class HWIOField:

  name = ''
  bit_low = 0
  bit_high = 0
  values = []
  line_number = 0

  # ----------------------------------------------------------------------
  # HWIOField:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # ----------------------------------------------------------------------

  def __init__(self, m, line_number, register, *args, **kwargs):
    self.name = m.group(1).upper()
    self.values = []
    self.register = register
    self.line_number = line_number
    self.valid = True

    # Determine bit range, group(3) will be <high>:<low> or <bit>
    bits = m.group(3)

    bits_m = re.search('(\d+):(\d+)', bits)
    if bits_m != None:
      self.bit_high = int(bits_m.group(1))
      self.bit_low = int(bits_m.group(2))
      # Sometimes they are reversed
      if self.bit_high < self.bit_low:
        temp = self.bit_high
        self.bit_high = self.bit_low
        self.bit_low = temp
    else:
      try:
        self.bit_high = int(bits)
      except ValueError:
        LogWarning('Invalid field bit range: %s' % (m.group(3)), line_number)
        self.valid = False
        self.bit_high = 0
      self.bit_low = self.bit_high

    if self.bit_high > 31:
      LogWarning('Field out of range: [%s]\n' % (bits) +
                 '     %s:%s' % (self.register.name, self.name), line_number)
      self.bit_high = 31

    # If the field name is marked as "RESERVED", add the bit position
    # to ensure uniqueness
    if self.name == "RESERVED" or self.name == "RESERVED_BITS":
      self.name += "_" + str(self.bit_high)
      if self.bit_low != self.bit_high:
        self.name += "_" + str(self.bit_low)

    SymbolCheck(self)

  def AddValue (self, value):
    if not value.valid:
      return
    # Check the value is within range for the field
    if value.value >= 1 << self.Width():
      LogWarning('Field value out of range (max %d): %d\n' % ((1 << self.Width()) - 1, value.value) +
                 '     %s:%s:%s' % (self.register.name, self.name, value.name), value.line_number)
      return
    # Check if this is a duplicate value - turns out this is semi-valid
    if not allow_duplicate_field_values:
      for existing_value in self.values:
        if existing_value.value == value.value:
          LogWarning('Duplicate field value (%d): %s, %s\n' % (value.value, value.name, existing_value.name) +
                     '     %s:%s' % (self.register.name, self.name), value.line_number)
        return
    self.values.append(value)

  def Mask (self):
    return (2 ** (self.bit_high + 1) - 1) ^ (2 ** self.bit_low - 1)

  def Shift (self):
    return self.bit_low

  def Symbol (self):
    return self.register.name + '_' + self.name

  def Module (self):
    return self.register.module

  def Width (self):
    return self.bit_high - self.bit_low + 1


# ============================================================================
# HWIORegister
#
# Class for a register in HWIO.
# ============================================================================

class HWIORegister(object):

  name = ''
  offset = 0
  module = None
  fields = []
  access = ''
  mask = 0
  longest_field_name = 0
  reset_value = None
  no_prefix = False

  # ----------------------------------------------------------------------
  # HWIORegister:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # ----------------------------------------------------------------------

  def __init__(self, m, line_number, module):
    self.fields = []
    self.line_number = line_number
    self.offset = int(m.group(2), 16)
    self.access = m.group(3)
    self.name = module.prefix + m.group(1) + module.postfix
    self.module = module
    self.dimensions = 0
    self.reset_value = None

    SymbolCheck(self)

  def AddField (self, field):
    if not field.valid:
      return
    self.fields.append(field)
    if len(field.name) > self.longest_field_name:
      self.longest_field_name = len(field.name)
      if self.longest_field_name + len(self.name) > self.module.longest_field_name:
       self.module.longest_field_name = self.longest_field_name + len(self.name)
    self.mask |= field.Mask()

  def CompareNames (self):
    m = re.match(self.module.prefix + '(.+)' + self.module.postfix, self.Symbol())
    return [m.group(1)]

  def SetResetValue (self, m, line_number):
    # Validate the name matches the register.  Sometimes the reset
    # name is expanded to the first element when it is a array.
    comp = self.CompareNames()
    if not m.group(1) in comp:
      LogWarning('Reset value mismatch: expected %s, found %s' % (comp[0], m.group(1)), line_number)
      return
    # Replace any 'X's (do-not-cares) with '0's.
    rxs = re.match('^0[xX](.*[xX].*)', m.group(2))
    if rxs:
      reset_value = '0x' + re.sub('[xX]', '0', rxs.group(1))
    else:
      reset_value = m.group(2)
    try:
      self.reset_value = int(reset_value, 0)
    except:
      LogWarning('Invalid reset value for %s: %s' % (self.name, reset_value), line_number)

  def Command (self):
    return (self.access.find('C') != -1)

  def Readable (self):
    return (self.access.find('R') != -1)

  def Writeable (self):
    return (self.access.find('W') != -1 or self.access.find('C') != -1)

  def WriteOnly (self):
    return (self.access.find('W') != -1 and self.access.find('R') == -1)

  def Address (self, *a):
    return self.module.Address() + self.offset

  def Offset (self, *a):
    return self.offset

  def Array (self):
    return False

  def Symbol (self, *a):
    return self.name

  def Module (self):
    return self.module

  def IgnorePrefix (self, ignore_prefix):
    if self.no_prefix == ignore_prefix:
      return
    if ignore_prefix:
      self.name = re.sub('^' + self.module.prefix, '', self.name) 
    else:
      self.name = self.module.prefix + self.name
    self.no_prefix = ignore_prefix


# ============================================================================
# HWIORegisterArray
#
# Class for a register array in HWIO.  Derived from HWIORegister.
# ============================================================================

class HWIORegisterArray (HWIORegister):

  # ----------------------------------------------------------------------
  # HWIORegisterArray:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # ----------------------------------------------------------------------

  def __init__(self, m, line_number, module):
    self.line_number = line_number
    self.formula = ''
    self.offset = 0
    self.fields = []
    self.access = ''
    self.module = module
    self.min_index = []
    self.max_index = []
    self.arg = []
    self.reset_value = None

    if m == None:
      return

    self.offset = int(m.group(2), 16)

    array_m = re.match(areg_1dim_pattern, m.group(1))
    if array_m != None:
      self.name = module.prefix + array_m.group(1) + module.postfix
      self.dimensions = 1
      self.arg.append(array_m.group(2))
      self.arg_list = "%s" % array_m.group(2)
      self.min_index.append(int(array_m.group(3)))
      self.max_index.append(int(array_m.group(4)))

      formula_m = re.match(areg_1dim_formula, m.group(3))
      self.formula = formula_m.group(1) + " * " + formula_m.group(2) + "(" + formula_m.group(3) + ")" + formula_m.group(4)
      self.eval_formula = formula_m.group(1) + " * " + formula_m.group(2) + "%d" + formula_m.group(4)

      # Check that the arry only contains a single occurrence of the
      # index charater, then replace with %d.
      if self.name.count(self.arg[0]) != 1:
        LogWarning('Array argument error for %s: %s' % (self.name, self.arg[0]), line_number)
        raise SyntaxError
      self.eval_name = re.sub(self.arg[0], '%d', self.name, count=1)

      SymbolCheck(self)
      return

    array_m = re.match(areg_2dim_pattern, m.group(1))
    if array_m != None:
      self.name = module.prefix + array_m.group(1) + module.postfix
      self.dimensions = 2
      self.arg.append(array_m.group(2))
      self.arg.append(array_m.group(3))
      self.arg_list = "%s,%s" % (array_m.group(2), array_m.group(3))
      self.min_index.append(int(array_m.group(4)))
      self.min_index.append(int(array_m.group(5)))
      self.max_index.append(int(array_m.group(6)))
      self.max_index.append(int(array_m.group(7)))

      formula_m = re.match(areg_2dim_formula, m.group(3))
      if not formula_m:
        LogWarning('Invalid formula for %s: %s' % (self.name, m.group(3)), line_number)
        raise SyntaxError
      self.formula = formula_m.group(1) + " * (" + formula_m.group(2) + ")" + formula_m.group(3) + " * (" + formula_m.group(4) + ")" + formula_m.group(5)
      self.eval_formula = formula_m.group(1) + " * (%d)" + formula_m.group(3) + " * (%d)" + formula_m.group(5)

      # Check that the arry only contains a single occurrence of the
      # index charaters, then replace with %d.
      if self.name.count(self.arg[0]) != 1:
        LogWarning('Array argument error for %s: %s' % (self.name, self.arg[0]), line_number)
        raise SyntaxError
      if self.name.count(self.arg[1]) != 1:
        LogWarning('Array argument error for %s: %s' % (self.name, self.arg[1]), line_number)
        raise SyntaxError
      self.eval_name = re.sub(self.arg[0], '%d', self.name, count=1)
      self.eval_name = re.sub(self.arg[1], '%d', self.eval_name, count=1)

      SymbolCheck(self)
      return

  def Address (self, index1=0, index2=0):
    if self.dimensions == 1:
      return super(HWIORegisterArray, self).Address() + eval(self.eval_formula % index1)
    elif self.dimensions == 2:
      return super(HWIORegisterArray, self).Address() + eval(self.eval_formula % (index1, index2))

  def Offset (self, index1=0, index2=0):
    if self.dimensions == 1:
      return super(HWIORegisterArray, self).Offset() + eval(self.eval_formula % index1)
    elif self.dimensions == 2:
      return super(HWIORegisterArray, self).Offset() + eval(self.eval_formula % (index1, index2))

  def Array (self):
    return True

  def Symbol (self, index1=-1, index2=-1):
    if index1 == -1:
      return self.name

    if self.dimensions == 1:
      return self.eval_name % index1
    elif self.dimensions == 2:
      return self.eval_name % (index1, index2)

  def ArgList (self, separator=','):
    if self.dimensions == 1:
      return self.arg[0]
    elif self.dimensions == 2:
      return self.arg[0] + separator + self.arg[1]

  # ----------------------------------------------------------------------
  # HWIORegisterArray:CompareNames
  #
  # This function returns an array of values to be compared against
  # the RESET_VALUE line or the second line of an ARRAY definition.
  # These values remove the prefix and postfix, and include both the
  # raw symbol name as well as the expanded first entry in the array.
  # ----------------------------------------------------------------------

  def CompareNames (self):
    i = [self.min_index[0], 0]
    if self.dimensions == 2:
      i[1] = self.min_index[1]

    m = re.match(self.module.prefix + '(.+)' + self.module.postfix, self.Symbol(i[0], i[1]))
    n = re.match(self.module.prefix + '(.+)' + self.module.postfix, self.name)
    return [m.group(1), n.group(1)]

  # ----------------------------------------------------------------------
  # HWIORegisterArray:CreateFromRegister
  #
  # This function creates a register array object out of the register
  # by using the provided offset and indexing arguments.  This is used
  # in some cases when the hardware definitions have been "un-rolled"
  # and some team wants to roll them back up into an array.
  #  base_register: The register to turn into an array
  #  array_offset: The offset between each element of the array
  #  max_index: Maximum index for the array (generally size-1)
  #  arg: The character argument to use for indexing the array
  #  arg_pos: The position in the register name to substitute the arg
  #    into.  A negative value can be used to index from the end of the
  #    register name.
  # ----------------------------------------------------------------------

  @classmethod
  def CreateFromRegister (cls, base_register, array_offset, max_index, arg='a', arg_pos=None, **kwargs):
    array = HWIORegisterArray(m=None, line_number=base_register.line_number, module=base_register.module)
    array.offset = base_register.offset
    array.mask = base_register.mask
    array.longest_field_name = base_register.longest_field_name
    array.fields = copy.deepcopy(base_register.fields)
    for field in array.fields:
      field.register = array
    array.access = base_register.access
    array.min_index = [ 0 ]
    array.max_index = [ max_index ]
    array.arg = [ arg ]
    if arg_pos == None:
      array.name = base_register.name + array.arg[0]
    else:
      s = list(base_register.name)
      s[arg_pos] = array.arg[0]
      array.name = "".join(s)
    array.dimensions = base_register.dimensions + 1

    if base_register.dimensions == 0:
      array.arg_list = '%s' % array.arg[0]
      array.formula = '%#x * (%s)' % (array_offset, array.arg[0])
      array.eval_formula = '%#x * %%d' % (array_offset)
      array.eval_name = re.sub(array.arg[0], '%d', array.name)
    else:
      array.min_index.extend(base_register.min_index)
      array.max_index.extend(base_register.max_index)
      array.arg.extend(base_register.arg)
      array.arg_list = '%s,%s' % (array.arg[0], base_register.arg_list)
      array.formula = base_register.formula + ' + %#x * (%s)' % (array_offset, array.arg[0])
      array.eval_formula = base_register.eval_formula + ' + %#x * %%d' % (array_offset)
      array.eval_name = re.sub(array.arg[0], '%d', array.name)

    return array


# ============================================================================
# HWIOModule
#
# Class for a module in HWIO.
# ============================================================================

class HWIOModule:

  # --------------------------------------------------------------------------
  # HWIOModule:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # --------------------------------------------------------------------------

  def __init__(self, m, line_number, *args, **kwargs):
    self.registers = []
    self.line_number = line_number
    self.base_name = m.group(2)
    self.offset = int(m.group(3), 16)
    self.base = None
    self.longest_field_name = 0
    self.text = ''

    name = m.group(1).upper()

    # Check for generics in the module name and replace with base name
    if name == 'REGS' or name == 'REG':
      name = re.sub('_BASE$', '', self.base_name)

    spre_m = re.search(module_spre_pattern, m.group(4))
    if spre_m != None:
      spre = spre_m.group(1).upper()

      # Clean up some known spre value issues
      if re.match('A_', spre):
        self.prefix = spre[2:]
      else:
        self.prefix = spre

      # Check it ends with an underscore
      if len(self.prefix) > 0 and self.prefix[-1] != '_':
        LogWarning('SPRE missing underscore: %s (%s)' % (spre, name), line_number)

    else:
      spre = None
      self.prefix = ''

    # Check if APRE and SPRE are different
    apre_m = re.search(module_apre_pattern, m.group(4))
    apre = None
    if apre_m != None:
      apre = apre_m.group(1).upper()
    if spre != apre:
      LogWarning('SPRE=%s not equal to APRE=%s for %s' % (spre, apre, name), line_number)

    spost_m = re.search(module_spost_pattern, m.group(4))
    if spost_m != None:
      self.postfix = spost_m.group(1).upper()
    else:
      self.postfix = ''

    # If the module prefix ends with the module name, then we just set the
    # name to the prefix (minus the trailing '_')
    if name + '_' == self.prefix or re.search('_' + name + '_$', self.prefix):
      m = re.search('(.*)_$', self.prefix)
      self.name = m.group(1)
    # If the module name already starts with the prefix, do not add it.
    elif re.search('^' + self.prefix, name + '_'):
      self.name = name
    else:
      self.name = self.prefix + name

    # Add the postfix unless it is already present
    if not re.search(self.postfix + '$', '_' + self.name):
      self.name = self.name + self.postfix

    SymbolCheck(self, fix_duplicates=True)

  def Symbol (self):
    return self.name + '_REG_BASE'

  def Address (self):
    return self.base.address + self.offset

  def MaxOffset (self):
    last_offset = 0

    if len(self.registers) == 0:
      return 0

    # Find the highest offset register
    last_register = max(self.registers, key=lambda reg: reg.offset)

    # Find the last address
    if last_register.Array():
      if last_register.dimensions == 1:
        last_offset = last_register.Offset(last_register.max_index[0])
      else:
        last_offset = last_register.Offset(last_register.max_index[0], last_register.max_index[1])
    else:
      last_offset = last_register.offset

    return last_offset + self.offset


# ============================================================================
# HWIOBase
#
# Class for a base in HWIO.
# ============================================================================

class HWIOBase:

  name = ''
  raw_name = ''
  address = 0
  size = 0
  modules = []

  # --------------------------------------------------------------------------
  # HWIOBase:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.  It can also take a list containing three values:
  # [Name, Address, Size].
  # --------------------------------------------------------------------------

  def __init__(self, m, line_number, *args, **kwargs):
    if type(m) == list:
      self.name = m[0]
      self.address = m[1]
      self.size = m[2]
    else:
      # Remove trailing '_BASE'
      self.raw_name = m.group(1)
      self.name = re.sub('_BASE$', '', self.raw_name)
      self.address = int(m.group(2), 16)
      self.size = 2 ** (int(m.group(4)) + 1);
    self.modules = []
    self.line_number = line_number
    self.text = ''

    SymbolCheck(self, fix_duplicates=True)

  def Symbol (self):
    return self.name + '_BASE'

  def MaxOffset (self):
    last_offset = 0
 
    for module in self.modules:

      mod_last_offset = module.MaxOffset()

      if mod_last_offset > last_offset:
        last_offset = mod_last_offset

    return last_offset


# ============================================================================
# PrintAddressFile
#
# Prints data about the address file
# ============================================================================

def PrintAddressFile (bases):
  # Sort the bases if requested
  if re.search('sortname', print_arg) != None:
    bases_sorted = sorted(bases, key=lambda base: base.name)
  elif re.search('sortaddr', print_arg) != None:
    bases_sorted = sorted(bases, key=lambda base: base.address)
  else:
    bases_sorted = bases

  for base in bases_sorted:
    if re.search('bases', print_arg) != None:
      print "Base:         %s at 0x%x (0x%x bytes)" % (base.name, base.address, base.size)
    for module in base.modules:
      if re.search('modules', print_arg) != None:
        print "  Module:     %s at %s+0x%x" % (module.name, module.base_name, module.offset)
      for register in module.registers:
        if re.search('registers', print_arg) != None or (re.search('writeonly', print_arg) != None and register.WriteOnly()):
          print "    Register: %s (%s) at %s+0x%x" % (register.name, register.access, module.name, register.offset)


# ============================================================================
# ValidateFieldCount
#
# Checks that all registers have at least one field defined.
# ============================================================================

def ValidateFieldCount (bases):
  # If we are not outputting anything, then we can skip this
  if silent:
    return

  for base in bases:
    for module in base.modules:
      for register in module.registers:
        if len(register.fields) == 0:
          LogWarning("Register with no fields defined: '%s'" % (register.Symbol()), module.line_number)


# ============================================================================
# LoadAddressFile
#
# Parses and loads the data from the given address file.
# ============================================================================

def LoadAddressFile (addr_file_name, load_silently=False, save_text=False, save_warnings=None):
  global silent
  global symbols
  global warnings
  global saved_warnings

  # We are not re-entrant due to use of global variables
  lock.acquire() 

  # Init variables
  silent = load_silently
  saved_warnings = save_warnings
  warnings = 0
  base = None
  module = None
  register = None
  field = None
  line_number = 0
  modules_count = 0
  registers_count = 0
  next_register_is_array = False
  bases = []
  unassigned_modules = []
  symbols = {}

  LogInfo("-- Loading " + addr_file_name + "...")
  try:
    addr_file = open(addr_file_name, 'r')
  except:
    LogWarning("Unable to open " + addr_file_name)
    return []

  for line in addr_file: 

    line_number += 1

    try:

      # Check for comments and blank lines
      if re.match(comment_pattern, line) or re.match('^\s*$', line):
        continue

      # Check for base definitions
      m = re.match(base_pattern, line)
      if m != None:
        base = HWIOBase(m=m, line_number=line_number)
        if save_text:
          base.text = line
        #print "Base: %s at 0x%x (0x%x bytes)" % (base.name, base.address, base.size)
        bases.append(base)
        module = None
        continue

      # Check for module definitions
      m = re.match(module_pattern, line)
      if m != None:
        module = HWIOModule(m=m, line_number=line_number)
        if save_text:
          module.text = line
        #print "  Found module: %s (%s) at %s:0x%x" % (module.name, module.Symbol(), module.base_name, module.offset)
        if base != None and base.raw_name == module.base_name:
          base.modules.append(module)
          module.base = base
        else:
          unassigned_modules.append(module)
        modules_count += 1
        continue

      if save_text and module != None:
        module.text += line

      # Check for register definitions
      m = re.match(register_pattern, line)
      if m != None:
        # Check if the register definition is part of an array.  If so we
        # already added the register and just need to update the access type.
        if next_register_is_array:
          next_register_is_array = False
          if not register:
            continue
          comp = register.CompareNames()
          if not m.group(1) in comp:
            LogWarning('Array register mismatch: expected %s, found %s' % (comp[0], m.group(1)), line_number)
          register.access = m.group(3)
          continue

        try:
          register = HWIORegister(m=m, line_number=line_number, module=module)
        except SyntaxError:
          register = None
          continue
        module.registers.append(register)
        registers_count += 1
        continue

      # Check for register array definition
      m = re.match(areg_pattern, line)
      if m != None:
        next_register_is_array = True
        try:
          register = HWIORegisterArray(m=m, line_number=line_number, module=module)
        except SyntaxError:
          register = None
          continue
        module.registers.append(register)
        registers_count += 1
        continue

      # Check for field definition
      m = re.match(field_pattern, line)
      if m != None:
        if not register:
          continue
        try:
          field = HWIOField(m=m, line_number=line_number, register=register)
        except:
          field = None
          continue
        register.AddField(field)
        continue

      # Check for value definition
      m = re.match(value_pattern, line)
      if m != None:
        if not register or not field:
          continue
        value = HWIOFieldValue(m=m, line_number=line_number)
        field.AddValue(value)
        continue

      if re.match(field_reset_value_pattern, line):
        continue

      m = re.match(reset_value_pattern, line)
      if m != None:
        if not register:
          continue
        register.SetResetValue(m, line_number)
        continue

      LogWarning("Syntax error: \n%s" % (line.rstrip('\n')), line_number)

    except KeyboardInterrupt:
      print "Parsing interrupted at line %d." % line_number
      sys.exit(1)
    except:
      print "Failed parsing line %d:" % line_number
      print line
      import traceback
      print traceback.format_exc()

  # Assign modules to bases that were not found at first pass
  for module in unassigned_modules:
    found = False
    for base in bases:
      if base.raw_name == module.base_name:
        base.modules.append(module)
        module.base = base
        found = True
        break
    if not found:
      LogWarning("Could not find base for module '%s': '%s'" % (module.name, module.base_name), module.line_number)

  # Validate all registers have fields - skip for now as there are lots!
  #ValidateFieldCount(bases)

  log = "-- Found %d bases, %d modules, %d registers" % (len(bases), modules_count, registers_count)
  if warnings > 0:
    log += ' (%d warnings)' % warnings
  LogInfo(log)

  lock.release()

  return bases


# ============================================================================
# ParseCommandLine
#
# Parse command line arguments.
# ============================================================================

def ParseCommandLine (argv):
  global print_arg

  address_file = 'ARM_ADDRESS_FILE.FLAT'

  try:                                
    opts, args = getopt.getopt(argv, "h", ["flat=", "print="])
  except getopt.GetoptError:          
    print "Invalid command line."
    sys.exit(2)                     
  for opt, arg in opts:
    if opt == '-h':
      sys.exit()                  
    elif opt == "--flat":
      address_file = arg
    elif opt == "--print":
      print_arg = arg

  return address_file


# ============================================================================
# Main
#
# Entry point.
# ============================================================================

if __name__ == "__main__":
  bases = LoadAddressFile(ParseCommandLine(sys.argv[1:]))

  if len(print_arg) > 0:
    PrintAddressFile(bases)


# ============================================================================
# SCons Hooks
#
# These functions are used when loading this module as an SCons "tool".
# ============================================================================

def SConsLoadAddressFile (env, address_file):
  return LoadAddressFile(env.NormPath(address_file))

def generate (env):
  env.AddMethod(SConsLoadAddressFile, "HWIOLoadAddressFile")

def exists (env):
  return env.Detect('hwio_tools')


