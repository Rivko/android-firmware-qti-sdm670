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
#  Copyright (c) 2011-2016 QUALCOMM Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.glue/1.2/tools/tools/systemdrivers/pmic/hoya/HWIOParser.py#1 $
#  $DateTime: 2017/02/17 13:50:58 $
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


# ============================================================================
# Pattern definitions for parsing the address file
# ============================================================================

base_pattern = re.compile('^\s*(\S+)\s+BASE\s+(\S+)\s+(.*)\s+(\d+):(\d+).*$')
base_size_pattern = re.compile('SIZE=(\S+)')
path_pattern = re.compile('\s*--\s*(MODULE|BASE)\s+(\S+)\s*\(level\s+(\d+)\).*$')
module_pattern = re.compile('^\s*(\S+)\s+MODULE\s+OFFSET=(\S+)\s*\+\s*(\S+)\s*(.*)$')
module_max_pattern = re.compile('MAX=(\S+)\s*\+\s*(\S+)')
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
areg_2dim_formula = re.compile('(.+)\*(.)\s*\+\s*(.+)\*(.)(.*)')
reset_value_pattern = re.compile('^\s*(\S+)\s+RESET_VALUE\s+(\S+).*$')
field_reset_value_pattern = re.compile('^\s*(\S+)\s+FLD_RESET_VALUE\s+(\S+).*$')
mem_pattern = re.compile('^\s*(\S+)\s+MEM\s+START=(\S+)\s+END=(\S+)\s*(.*)$')
submem_pattern = re.compile('^\s*(\S+)\s+SUBMEM\s+OFFSET=(\S+)\s*\+\s*(\S+)\s+MAX=(\S+)\s*\+\s*(\S+)\s*(.*)$')

# HWIO line types supported
HWIO_LINE_BLANK          = 0
HWIO_LINE_COMMENT        = 1
HWIO_LINE_BASE           = 2
HWIO_LINE_MODULE         = 3
HWIO_LINE_REGISTER       = 4
HWIO_LINE_REGISTER_ARRAY = 5
HWIO_LINE_FIELD          = 6
HWIO_LINE_RESET          = 7
HWIO_LINE_VALUE          = 8
HWIO_LINE_FIELD_RESET    = 9
HWIO_LINE_PATH           = 10
HWIO_LINE_MEM            = 11
HWIO_LINE_SUBMEM         = 12
HWIO_LINE_UNKNOWN        = 100

hwio_line_patterns = [
  (HWIO_LINE_BLANK, re.compile('^\s*$')),
  #(HWIO_LINE_PATH, path_pattern), # Must come before COMMENT
  (HWIO_LINE_COMMENT, re.compile('^\s*--')),
  (HWIO_LINE_BASE, base_pattern),
  (HWIO_LINE_MODULE, module_pattern),
  (HWIO_LINE_REGISTER, register_pattern),
  (HWIO_LINE_FIELD, field_pattern),
  (HWIO_LINE_VALUE, value_pattern),
  (HWIO_LINE_REGISTER_ARRAY, areg_pattern),
  (HWIO_LINE_RESET, reset_value_pattern),
  (HWIO_LINE_FIELD_RESET, field_reset_value_pattern),
  (HWIO_LINE_MEM, mem_pattern),
  (HWIO_LINE_SUBMEM, submem_pattern),
]


# ============================================================================
# Globals
# ============================================================================

silent = False
warnings = 0
saved_warnings = None
lock = threading.RLock()


# ============================================================================

def LogInfo (log):
  '''
  Prints a message unless "silent" mode is in use.
  '''
  if not silent:
    print log

  
# ============================================================================

def LogWarning (log, line_number=None):
  '''
  Prints a message unless "silent" mode is in use.
  '''
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

class HWIOSymbolChecker:
  '''
  Class to handle checking for duplicate HWIO symbols.
  '''

  def __init__ (self, allow=False):
    self.symbols = {} if not allow else None

  def CheckForDuplicates (self, symbol, fix=False):
    '''
    Validates that the new symbol does not conflict with an existing one.
    Also checks for unexpected case.
    '''

    if self.symbols is None:
      return

    # Get any duplicate symbol
    dup = self.symbols.get(symbol.Symbol(), None)

    # We handle one special case of duplicate symbols here.  If the two
    # conflicting symbols are both modules, and the already existing
    # one has no registers in it, then we delete that one.  This is a hack
    # to handle the not-uncommon case of a container module having the same
    # name as one of its children.  Older versions of AutoReg did not include
    # these modules at all, but starting with 2.0.3 they are listed.
    if isinstance(dup, HWIOModule) and isinstance(symbol, HWIOModule):
      if len(dup.registers) == 0:
        #self.LogWarning("Removing empty duplicate module (line %d): %s" % (dup.line_number, symbol.name), symbol.line_number)
        dup.base.modules.remove(dup)
        del self.symbols[dup.Symbol()]
        dup = None
  
    # If we find a duplicate symbol we append an index to it like '_2'.  Loop
    # until we find an unused index.
    if dup:
      self.LogWarning("Duplicate symbol (line %d): %s" % (self.symbols[symbol.Symbol()].line_number, symbol.name), symbol.line_number)
      if not fix:
        raise SyntaxError

      suffix = ''
      index = 2
      while symbol.Symbol() in self.symbols:
        if not suffix == '':
          symbol.name = re.sub(suffix + '$', '', symbol.name)
        suffix = '_' + str(index)
        symbol.name = symbol.name + "_" + str(index)
        index += 1
  
    # We allow a single lower-case letter as this is not uncommon and used for
    # register arrays for example.  Multiple lower-case in a row generally
    # means a mis-named symbol.
    if re.search('[a-z][a-z]', symbol.name):
      self.LogWarning("Lower case symbol: %s" % (symbol.name), symbol.line_number)
  
    self.symbols[symbol.Symbol()] = symbol


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
    self.line_number = line_number
    try:
      self.value = int(m.group(2), 0)
    except ValueError:
      LogWarning('Invalid field value: %s' % (m.group(2)), line_number)
      raise SyntaxError


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

    # Determine bit range, group(3) will be <high>:<low> or <bit>
    bits = m.group(3)

    bits_m = re.search('(\d+):(\d+)', bits)
    try:
      if bits_m != None:
        self.bit_high = int(bits_m.group(1))
        self.bit_low = int(bits_m.group(2))
        # Sometimes they are reversed
        if self.bit_high < self.bit_low:
          temp = self.bit_high
          self.bit_high = self.bit_low
          self.bit_low = temp
      else:
        self.bit_high = int(bits)
        self.bit_low = self.bit_high
    except ValueError:
      LogWarning('Invalid field bit range: %s' % (m.group(3)), line_number)
      raise SyntaxError

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

  def AddValue (self, value):
    # Check the value is within range for the field
    if value.value >= 1 << self.Width():
      LogWarning('Field value out of range (max %d): %d\n' % ((1 << self.Width()) - 1, value.value) +
                 '     %s:%s:%s' % (self.register.name, self.name, value.name), value.line_number)
      return
    # Check if this is a duplicate value - turns out this is semi-valid
    #for existing_value in self.values:
    #  if existing_value.value == value.value:
    #    LogWarning('Duplicate field value (%d): %s, %s\n' % (value.value, value.name, existing_value.name) +
    #               '     %s:%s' % (self.register.name, self.name), value.line_number)
    #  return
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

  def AddField (self, field):
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
    return ((self.access.find('W') != -1 or self.access.find('C') != -1) and self.access.find('R') == -1)

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
    self.multipliers = []
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
      self.multipliers = [int(formula_m.group(1), 0)]

      # Check that the arry only contains a single occurrence of the
      # index charater, then replace with %d.
      if self.name.count(self.arg[0]) != 1:
        LogWarning('Array argument error for %s: %s' % (self.name, self.arg[0]), line_number)
        raise SyntaxError

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

      mult0 = formula_m.group(1)
      arg0  = formula_m.group(2)
      mult1 = formula_m.group(3)
      arg1  = formula_m.group(4)

      self.formula = "%s * (%s) + %s * (%s)%s" % (mult0, arg0, mult1, arg1, formula_m.group(5))
      self.multipliers = [int(mult0, 0), int(mult1, 0)]

      # Check which order the formula arguments are in to make sure when 
      # evaluating the parameters go in the right place.
      if [arg0, arg1] == self.arg:
        self.eval_formula = "%s * (%%d) + %s * (%%d)%s" % (mult0, mult1, formula_m.group(5))
      elif [arg1, arg0] == self.arg:
        self.eval_formula = "%s * (%%d) + %s * (%%d)%s" % (mult1, mult0, formula_m.group(5))
      else:
        LogWarning('Invalid formula for %s: %s' % (self.name, m.group(3)), line_number)
        raise SyntaxError

      # Check that the array only contains a single occurrence of the
      # index charaters.
      if self.name.count(self.arg[0]) != 1:
        LogWarning('Array argument error for %s: %s' % (self.name, self.arg[0]), line_number)
        raise SyntaxError
      if self.name.count(self.arg[1]) != 1:
        LogWarning('Array argument error for %s: %s' % (self.name, self.arg[1]), line_number)
        raise SyntaxError

      return

    LogWarning('Invalid array formula: %s' % (m.group(1)), line_number)
    raise SyntaxError

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

    eval_name = re.sub(self.arg[0], '%d', self.name, count=1) % index1
    if self.dimensions == 2 and index2 != -1:
        eval_name = re.sub(self.arg[1], '%d', eval_name, count=1) % index2

    return eval_name

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
    array.fields = base_register.fields
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
    else:
      array.min_index.extend(base_register.min_index)
      array.max_index.extend(base_register.max_index)
      array.arg.extend(base_register.arg)
      array.arg_list = '%s,%s' % (array.arg[0], base_register.arg_list)
      array.formula = base_register.formula + ' + %#x * (%s)' % (array_offset, array.arg[0])
      array.eval_formula = base_register.eval_formula + ' + %#x * %%d' % (array_offset)

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

  def __init__(self, m, line_number, use_apre=False, *args, **kwargs):
    self.registers = []
    self.line_number = line_number
    self.base_name = m.group(2)
    self.offset = int(m.group(3), 16)
    self.base = None
    self.longest_field_name = 0
    self.text = ''
    self.max_offset = None
    self.prefix_override = None

    name = m.group(1).upper()

    # Check for generics in the module name and replace with base name
    if name == 'REGS' or name == 'REG':
      name = re.sub('_BASE$', '', self.base_name)

    # Calculate the size of the module, we are provided the MAX offset
    # so just need to +1 to that.
    max_m = re.search(module_max_pattern, m.group(4))
    if max_m:
      self.size = int(max_m.group(2), 16) + 1 - self.offset
    else:
      self.size = None

    if use_apre:
      spre_m = re.search(module_apre_pattern, m.group(4))
    else:
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
    if not use_apre:
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
      self.name = self.prefix[0:-1]
      self.prefix_module = re.sub(name + '_$', '', self.prefix)
    # If the module name already starts with the prefix, do not add it.
    elif re.search('^' + self.prefix, name + '_'):
      self.name = name
      self.prefix_module = ''
    else:
      self.name = self.prefix + name
      self.prefix_module = self.prefix

    # Add the postfix unless it is already present
    if not re.search(self.postfix + '$', '_' + self.name):
      self.name = self.name + self.postfix

  def Symbol (self):
    return self.name + '_REG_BASE'

  def Address (self):
    return self.base.address + self.offset

  def MaxOffset (self):
    if self.max_offset != None:
      return self.max_offset

    last_offset = 0

    if len(self.registers) == 0:
      self.max_offset = self.offset
      return self.max_offset

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

    self.max_offset = last_offset + self.offset
    return self.max_offset

  def Size (self):
    if self.size:
      return self.size
    else:
      return self.MaxOffset() - self.offset

  def SetPrefix (self, new_prefix):
    if self.prefix_override != None:
      prefix_len = len(self.prefix_override)
      prefix_module_len = len(self.prefix_override)
    else:
      prefix_len = len(self.prefix)
      prefix_module_len = len(self.prefix_module)

    self.name = new_prefix + self.name[prefix_module_len:]

    for register in self.registers:
      register.name = new_prefix + register.name[prefix_len:]

    self.prefix_override = new_prefix

  def RestorePrefix (self):
    if self.prefix_override == None:
      return
    prefix_len = len(self.prefix_override)

    self.name = self.prefix_module + self.name[prefix_len:]

    for register in self.registers:
      register.name = self.prefix + register.name[prefix_len:]

    self.prefix_override = None


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
      size = re.search(base_size_pattern, m.group(3))
      if size != None:
        self.size = int(size.group(1), 16)
      else:
        self.size = 2 ** (int(m.group(4)) + 1);
    self.modules = []
    self.line_number = line_number
    self.text = ''

  def Symbol (self):
    return self.name + '_BASE'

  def AddModule (self, module):
    module_end = module.offset + module.size
    #if module_end > self.size:
    #  LogWarning('Module %s too large for base %s' % (module.name, self.name), module.line_number)
    self.modules.append(module)
    module.base = self

  def MaxOffset (self):
    last_offset = 0
 
    for module in self.modules:

      mod_last_offset = module.MaxOffset()

      if mod_last_offset > last_offset:
        last_offset = mod_last_offset

    return last_offset


# ============================================================================
# HWIOSubMem
#
# Class for a sub-memory region in HWIO.
# ============================================================================

class HWIOSubMem:

  # --------------------------------------------------------------------------
  # HWIOSubMem:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # --------------------------------------------------------------------------

  def __init__(self, m, line_number, *args, **kwargs):
    self.name = m.group(1)
    self.mem_name = m.group(2)
    self.memory = None
    self.offset = int(m.group(3), 16)
    self.size = int(m.group(5), 16) - self.offset + 1
    self.line_number = line_number

  def Symbol (self):
    return self.name

  def Address(self):
    return self.memory.address + self.offset


# ============================================================================
# HWIOMem
#
# Class for a memory region in HWIO.
# ============================================================================

class HWIOMem:

  # --------------------------------------------------------------------------
  # HWIOMem:__init__
  #
  # Constructor function that takes the regex match result from the
  # address file.
  # --------------------------------------------------------------------------

  def __init__(self, m, line_number, *args, **kwargs):
    self.name = m.group(1)
    self.address = int(m.group(2), 16)
    self.size = int(m.group(3), 16) - self.address + 1
    self.submems = []
    self.line_number = line_number

  def Symbol (self):
    return self.name

# ============================================================================

def PrintAddressFile (bases, memories, print_arg):
  '''
  Prints data about the address file.
  '''

  # Sort the bases if requested
  if re.search('sortname', print_arg) != None:
    bases_sorted = sorted(bases, key=lambda base: base.name)
  elif re.search('sortaddr', print_arg) != None:
    bases_sorted = sorted(bases, key=lambda base: base.address)
  else:
    bases_sorted = bases

  #if re.search('path', print_arg) != None:
  #  for base in bases_sorted:
  #    print "0x%08X: %s / %s" % (base.address, base.name, base.path)
  #    for module in base.modules:
  #      if module.path:
  #        level = module.path.count('.') - 1
  #        print "0x%08X--0x%08X: %s / %s" % (module.Address(), module.Address()+module.MaxOffset(), module.name, module.path)

  for base in bases_sorted:
    if re.search('bases', print_arg) != None:
      print "Base:         %s at 0x%x (0x%x bytes)" % (base.name, base.address, base.size)
    for module in base.modules:
      if re.search('modules', print_arg) != None:
        print "  Module:     %s at %s+0x%x" % (module.name, module.base_name, module.offset)
      for register in module.registers:
        if re.search('registers', print_arg) != None or (re.search('writeonly', print_arg) != None and register.WriteOnly()):
          print "    Register: %s (%s) at %s+0x%x" % (register.name, register.access, module.name, register.offset)

  if re.search('memories', print_arg) != None:
    for mem in memories:
      print "Memory:         %s at 0x%x (0x%x bytes)" % (mem.name, mem.address, mem.size)
      for submem in mem.submems:
        print "  Sub-Memory:   %s at %s+0x%x" % (submem.name, submem.memory.name, submem.offset)


# ============================================================================

def ValidateFieldCount (bases):
  '''
  Checks that all registers have at least one field defined.
  '''

  # If we are not outputting anything, then we can skip this
  if silent:
    return

  for base in bases:
    for module in base.modules:
      for register in module.registers:
        if len(register.fields) == 0:
          LogWarning("Register with no fields defined: '%s'" % (register.Symbol()), module.line_number)


# ============================================================================

def MatchLine (line):
  '''
  Search the HWIO line patterns for a match to the given line.  Returns
  the line type and the regex match object.
  '''
  for pattern in hwio_line_patterns:
    m = re.match(pattern[1], line)
    if m:
      return pattern[0], m

  return HWIO_LINE_UNKNOWN, None


# ============================================================================

def LoadAddressFile (addr_file_name, load_silently=False, save_text=False,
                     save_warnings=None, register_filter=None, use_apre=False,
                     register_dict=None, include_memories=False,
                     allow_duplicates=False):
  '''
  Parses and loads the data from the given address file.
  '''

  global silent
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
  #path = None
  #path_dict = {}
  memories = []
  mem_dict = {}

  sc = HWIOSymbolChecker(allow=allow_duplicates)

  LogInfo("-- Loading " + addr_file_name + "...")
  if type(addr_file_name) in [str, unicode]:
    try:
      addr_file = open(addr_file_name, 'r')
    except:
      LogWarning("Unable to open " + addr_file_name)
      return [] if not include_memories else [], []
  else:
    addr_file = addr_file_name

  for line in addr_file: 

    line_number += 1

    try:
      # Find the line type this is
      line_type, m = MatchLine(line)

      # Check for path (which is part of a comment)
      #if line_type == HWIO_LINE_PATH:
      #  path = m.group(2)
      #  level = int(m.group(3))
      #  #print "%s%s (%d)" % (("  " * (level-1)), path, level)
      #  continue

      # Check for comments and blank lines
      if line_type == HWIO_LINE_COMMENT or line_type == HWIO_LINE_BLANK:
        continue

      # If we are saving the text, pack out extra whitespace to ensure
      # consistent formatting between different files.
      if save_text:
        text = re.sub(r'\s+', ' ', line).strip() + '\n'

      # Check for memory definition
      if line_type == HWIO_LINE_MEM:
        if not include_memories:
          continue
        mem = HWIOMem(m=m, line_number=line_number)
        #print "Memory: %s at 0x%x (0x%x bytes)" % (mem.name, mem.address, mem.size)
        memories.append(mem)
        mem_dict[mem.name] = mem
        continue

      # Check for submemory definition
      elif line_type == HWIO_LINE_SUBMEM:
        if not include_memories:
          continue
        submem = HWIOSubMem(m=m, line_number=line_number)
        #print "SubMemory: %s at 0x%x (0x%x bytes)" % (submem.name, submem.offset, submem.size)
        if not submem.mem_name in mem_dict:
          LogWarning('SubMem container not found: %s' % (submem.mem_name), line_number)
          continue
        mem = mem_dict[submem.mem_name]
        mem.submems.append(submem)
        submem.memory = mem
        continue

      # --- Check for base definitions
      elif line_type == HWIO_LINE_BASE:
        base = HWIOBase(m=m, line_number=line_number)
        sc.CheckForDuplicates(base, fix=True)
        if save_text:
          base.text = text
        #base.path = path
        #path_dict[path] = base
        #print "Base: %s at 0x%x (0x%x bytes)" % (base.name, base.address, base.size)
        bases.append(base)
        continue

      # --- Check for module definitions
      elif line_type == HWIO_LINE_MODULE:
        module = HWIOModule(m=m, line_number=line_number, use_apre=use_apre)
        sc.CheckForDuplicates(module, fix=True)
        #module.path = path
        #path_dict[path] = module
        if save_text:
          module.text = text
        #print "  Found module: %s (%s) at %s:0x%x" % (module.name, module.Symbol(), module.base_name, module.offset)
        if base != None and base.raw_name == module.base_name:
          base.AddModule(module)
        else:
          unassigned_modules.append(module)
        modules_count += 1
        register = None
        continue

      # --- Check for register definitions
      if line_type == HWIO_LINE_REGISTER:
        # Check if the register definition is part of an array.  If so we
        # already added the register and just need to update the access type.
        if next_register_is_array:
          next_register_is_array = False
          if register:
            comp = register.CompareNames()
            if not m.group(1) in comp:
              LogWarning('Array register mismatch: expected %s, found %s' % (comp[0], m.group(1)), line_number)
            register.access = m.group(3)
        else:
          try:
            register = HWIORegister(m=m, line_number=line_number, module=module)
            sc.CheckForDuplicates(register)
          except SyntaxError:
            register = None

          if register and register_filter:
            if not any(re.match(filt[1], register.Symbol()) for filt in register_filter):
              register = None

          if register:
            module.registers.append(register)
            if register_dict != None:
              register_dict[register.Address()] = register
            registers_count += 1

      # --- Check for register array definition
      elif line_type == HWIO_LINE_REGISTER_ARRAY:
        next_register_is_array = True
        try:
          register = HWIORegisterArray(m=m, line_number=line_number, module=module)
          sc.CheckForDuplicates(register)
        except SyntaxError:
          register = None

        if register and register_filter:
          if not any(re.search(filt[1], register.Symbol()) for filt in register_filter):
            register = None

        if register:
          module.registers.append(register)
          if register_dict != None:
            register_dict[register.Address()] = register
          registers_count += 1

      # --- Check for field definition
      elif line_type == HWIO_LINE_FIELD:
        if save_text:
          text = '\t' + text
        if register:
          try:
            field = HWIOField(m=m, line_number=line_number, register=register)
            sc.CheckForDuplicates(field)
            register.AddField(field)
          except SyntaxError:
            field = None

      # --- Check for value definition
      elif line_type == HWIO_LINE_VALUE:
        if save_text:
          text = '\t\t' + text
        if register and field:
          try:
            value = HWIOFieldValue(m=m, line_number=line_number)
            field.AddValue(value)
          except SyntaxError:
            value = None

      # --- Field reset value pattern 
      elif line_type == HWIO_LINE_FIELD_RESET:
        if save_text:
          text = '\t' + text

      # --- Register reset value pattern 
      elif line_type == HWIO_LINE_RESET:
        if register:
          register.SetResetValue(m, line_number)

      # If we get here, there was a syntax error.
      else:
        LogWarning("Syntax error: \n%s" % (line.rstrip('\n')), line_number)

      # Save the text.  We save the text to the current module unless there is 
      # register filter being used that did not match a register.
      if save_text and module and (not register_filter or register):
        module.text += text

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

  if include_memories:
    return bases, memories
  else:
    return bases


# ============================================================================
# ParseCommandLine
#
# Parse command line arguments.
# ============================================================================

def ParseCommandLine (argv):
  args = {'flat': 'ARM_ADDRESS_FILE.FLAT',
          'use_apre': False }

  try:                                
    opts, _args = getopt.getopt(argv, "h", ["flat=", "print=", "apre"])
  except getopt.GetoptError:          
    print "Invalid command line."
    sys.exit(2)                     
  for opt, arg in opts:
    if opt == '-h':
      sys.exit()                  
    elif opt == "--flat":
      args['flat'] = arg
    elif opt == "--print":
      args['print'] = arg
    elif opt == "--apre":
      args['use_apre'] = True

  return args


# ============================================================================
# Main
#
# Entry point.
# ============================================================================

if __name__ == "__main__":
  args = ParseCommandLine(sys.argv[1:])
  bases, mems = LoadAddressFile(args['flat'], use_apre=args['use_apre'],
                                include_memories=True)

  if 'print' in args:
    PrintAddressFile(bases, mems, args['print'])


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


