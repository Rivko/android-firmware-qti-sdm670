#===========================================================================
#
#  @file HWIOGenRegs.py
#  @brief Python script to generate register definitions from an MSM
#         address file.  This script depends on the HWIOParser module
#         to load the address file.
#
#  This file can be invoked via HWIOGen.py, or directly by calling:
#
#    HWIOGenRegs.py --flat=<address_file> --cfg=<config_file>
#
#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/hwio/build/scripts/HWIOGenRegs.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

# ============================================================================
# Imports
# ============================================================================

import re
import sys
import getopt
import os
import imp
from datetime import date
from HWIOParser import HWIORegisterArray


# ============================================================================
# Globals
# ============================================================================

silent = False
address_file = "ARM_ADDRESS_FILE.FLAT"
config_files = []


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

def LogWarning (log):
  if not silent:
    print '** [Warning] ' + log

  
# ============================================================================
# SymbolExclude
#
# Check if we should exclude the given symbol using the filters from the
# given dictionary.
# ============================================================================

def IsSymbolExcluded (symbol, filter_dict):
  # Check for exclude filter
  if 'filter-exclude' in filter_dict:
    for filt in filter_dict['filter-exclude']:
      if re.search(filt, symbol.Symbol()):
        return True

  # Check for a module specific exclude filter
  if 'module-filter-exclude' in filter_dict:
    module_filter_dict = filter_dict['module-filter-exclude']
    if symbol.Module().name in module_filter_dict:
      for filt in module_filter_dict[symbol.Module().name]:
        if re.search(filt, symbol.Symbol()):
          return True

  pending_exclusion = False

  # Check for a module specific include filter, which means to exclude
  # everything else unless it shows up in the global include filter below
  if 'module-filter-include' in filter_dict:
    module_filter_dict = filter_dict['module-filter-include']
    if symbol.Module().name in module_filter_dict:
      for filt in module_filter_dict[symbol.Module().name]:
        if re.search(filt, symbol.Symbol()):
          return False
      pending_exclusion = True

  # Check for an include filter, which means to exclude everything else
  if 'filter-include' in filter_dict:
    for filt in filter_dict['filter-include']:
      if re.search(filt, symbol.Symbol()):
        return False
    return True

  return pending_exclusion


# ============================================================================
# ProcessField
#
# Parses a register field.
# ============================================================================

def ProcessField (field, regfile):

  if IsSymbolExcluded(field, regfile):
    return ''

  # Define generic format string to allow aligning all definitions
  form   = '#define %-' + str(field.register.module.longest_field_name + 20) + 's %s\n'
  output = ''

  define = 'HWIO_%s_BMSK' % field.Symbol()
  value  = '%#10x' % field.Mask()
  output += form % (define, value)

  define = 'HWIO_%s_SHFT' % field.Symbol()
  value  = '%#10x' % field.Shift()
  output += form % (define, value)

  if regfile['output-fvals']:
    for fval in field.values:
      define = 'HWIO_%s_%s_FVAL' % (field.Symbol(), fval.name)
      value  = '%#10x' % fval.value
      output += form % (define, value)

  return output


# ============================================================================
# IsRegisterShadowed
#
# Check if the given register should be "shadowed", meaning an in-memory 
# copy is kept and used to simulate reading from write-only registers.
# ============================================================================

def IsRegisterShadowed (register, regfile):
  if 'shadow' in regfile and register.Writeable():
    for reg in regfile['shadow']:
      if re.match(reg + '$', register.name):
        return True

  return False
  

# ============================================================================
# ProcessRegister
#
# Parses a register
# ============================================================================

def ProcessRegister (register, regfiles):

  # Define generic format string to allow aligning all definitions
  form   = '#define %-' + str(register.module.longest_field_name + 20) + 's %s\n'

  for regfile in regfiles:

    if IsSymbolExcluded(register, regfile):
      continue

    register.IgnorePrefix(regfile['ignore-prefixes'])

    if regfile['explicit-addressing']:
      define = 'HWIO_%s_ADDR(x)' % register.Symbol()
      value  = '((x) + %#010x)' % (register.offset)
    else:
      define = 'HWIO_%s_ADDR' % register.Symbol()
      value  = '(%s      + %#010x)' % (register.module.Symbol(), register.offset)
    output = form % (define, value)

    if regfile['output-phys']:
      if regfile['explicit-addressing']:
        define = 'HWIO_%s_PHYS(x)' % register.Symbol()
        value  = '((x) + %#010x)' % (register.offset)
      else:
        define = 'HWIO_%s_PHYS' % register.Symbol()
        value  = '(%s_PHYS + %#010x)' % (register.module.Symbol(), register.offset)
      output += form % (define, value)

    if regfile['output-offsets']:
      if regfile['explicit-addressing']:
        define = 'HWIO_%s_OFFS' % register.Symbol()
        value  = '(%#010x)' % (register.offset)
      else:
        define = 'HWIO_%s_OFFS' % register.Symbol()
        value  = '(%s_OFFS + %#010x)' % (register.module.Symbol(), register.offset)
      output += form % (define, value)

    define = 'HWIO_%s_RMSK' % register.Symbol()
    value  = '%#10x' % register.mask
    output += form % (define, value)

    if regfile['output-resets'] and register.reset_value != None:
      define = 'HWIO_%s_POR' % (register.Symbol())
      value  = '%#010x' % (register.reset_value)
      output += form % (define, value)

    if IsRegisterShadowed(register, regfile):
      if regfile['explicit-addressing']:
        output += '#define HWIO_%s_IN(x)       \\\n' % register.Symbol()
        output += '        (HWIO_%s_shadow & HWIO_%s_RMSK)\n' % (register.Symbol(), register.Symbol())
        output += '#define HWIO_%s_INM(x,m)    \\\n' % register.Symbol()
        output += '        (HWIO_%s_shadow & (m))\n' % register.Symbol()
        output += '#define HWIO_%s_OUT(x,v)    \\\n' % register.Symbol()
        output += '        out_dword_masked(HWIO_%s_ADDR(x),0xFFFFFFFF,v,HWIO_%s_shadow)\n' % (register.Symbol(), register.Symbol())
        output += '#define HWIO_%s_OUTM(x,m,v) \\\n' % register.Symbol()
        output += '        out_dword_masked(HWIO_%s_ADDR(x),m,v,HWIO_%s_shadow)\n' % (register.Symbol(), register.Symbol())
      else:
        output += '#define HWIO_%s_IN          \\\n' % register.Symbol()
        output += '        (HWIO_%s_shadow & HWIO_%s_RMSK)\n' % (register.Symbol(), register.Symbol())
        output += '#define HWIO_%s_INM(m)      \\\n' % register.Symbol()
        output += '        (HWIO_%s_shadow & (m))\n' % register.Symbol()
        output += '#define HWIO_%s_OUT(v)      \\\n' % register.Symbol()
        output += '        out_dword_masked(HWIO_%s_ADDR,0xFFFFFFFF,v,HWIO_%s_shadow)\n' % (register.Symbol(), register.Symbol())
        output += '#define HWIO_%s_OUTM(m,v) \\\n' % register.Symbol()
        output += '        out_dword_masked(HWIO_%s_ADDR,m,v,HWIO_%s_shadow)\n' % (register.Symbol(), register.Symbol())

      if 'shadow-file' in regfile:
        regfile['shadow-file'].write(
          '  #define HWIO_%s_shadow %s->HWIO_%s\n' % (register.Symbol(), regfile['shadow-ptr'], register.Symbol()) +
          '  uint32  HWIO_%s;\n\n' % register.Symbol())

    else:
      if register.Readable():
        if regfile['explicit-addressing']:
          output += '#define HWIO_%s_IN(x)      \\\n' % register.Symbol()
          output += '        in_dword_masked(HWIO_%s_ADDR(x), HWIO_%s_RMSK)\n' % (register.Symbol(), register.Symbol())
          output += '#define HWIO_%s_INM(x, m)      \\\n' % register.Symbol()
          output += '        in_dword_masked(HWIO_%s_ADDR(x), m)\n' % register.Symbol()
        else:
          output += '#define HWIO_%s_IN          \\\n' % register.Symbol()
          output += '        in_dword_masked(HWIO_%s_ADDR, HWIO_%s_RMSK)\n' % (register.Symbol(), register.Symbol())
          output += '#define HWIO_%s_INM(m)      \\\n' % register.Symbol()
          output += '        in_dword_masked(HWIO_%s_ADDR, m)\n' % register.Symbol()

      if register.Writeable():
        if regfile['explicit-addressing']:
          output += '#define HWIO_%s_OUT(x, v)      \\\n' % register.Symbol()
          output += '        out_dword(HWIO_%s_ADDR(x),v)\n' % register.Symbol()
          if register.Readable():
            output += '#define HWIO_%s_OUTM(x,m,v) \\\n' % register.Symbol()
            output += '        out_dword_masked_ns(HWIO_%s_ADDR(x),m,v,HWIO_%s_IN(x))\n' % (register.Symbol(), register.Symbol())
        else:
          output += '#define HWIO_%s_OUT(v)      \\\n' % register.Symbol()
          output += '        out_dword(HWIO_%s_ADDR,v)\n' % register.Symbol()
          if register.Readable():
            output += '#define HWIO_%s_OUTM(m,v) \\\n' % register.Symbol()
            output += '        out_dword_masked_ns(HWIO_%s_ADDR,m,v,HWIO_%s_IN)\n' % (register.Symbol(), register.Symbol())

    for field in register.fields:
      output += ProcessField(field, regfile)

    output += '\n'

    f = regfile['file']
    f.write(output)

    register.IgnorePrefix(False)


# ============================================================================
# ProcessRegisterArray
#
# Parses an array register
# ============================================================================

def ProcessRegisterArray (register, regfiles):

  # Define generic format string to allow aligning all definitions
  form   = '#define %-' + str(register.module.longest_field_name + 20) + 's %s\n'

  for regfile in regfiles:

    if IsSymbolExcluded(register, regfile):
      continue

    register.IgnorePrefix(regfile['ignore-prefixes'])

    if regfile['explicit-addressing']:
      arg_list = 'base,' + register.arg_list
    else:
      arg_list = register.arg_list

    hwio_addr = 'HWIO_%s_ADDR(%s)' % (register.Symbol(), arg_list)

    define = hwio_addr
    if regfile['explicit-addressing']:
      value  = '((base) + %#010x + %s)' % (register.offset, register.formula)
    else:
      value  = '(%s      + %#010x + %s)' % (register.module.Symbol(), register.offset, register.formula)
    output = form % (define, value)

    if regfile['output-phys']:
      define = 'HWIO_%s_PHYS(%s)' % (register.Symbol(), arg_list)
      if regfile['explicit-addressing']:
        value  = '((base) + %#010x + %s)' % (register.offset, register.formula)
      else:
        value  = '(%s_PHYS + %#010x + %s)' % (register.module.Symbol(), register.offset, register.formula)
      output += form % (define, value)

    if regfile['output-offsets']:
      define = 'HWIO_%s_OFFS(%s)' % (register.Symbol(), arg_list)
      if regfile['explicit-addressing']:
        value  = '(%#010x + %s)' % (register.offset, register.formula)
      else:
        value  = '(%s_OFFS + %#010x + %s)' % (register.module.Symbol(), register.offset, register.formula)
      output += form % (define, value)

    define = 'HWIO_%s_RMSK' % register.Symbol()
    value  = '%#10x' % register.mask
    output += form % (define, value)

    define = 'HWIO_%s_MAX%s' % (register.Symbol(), register.arg[0])
    value  = '%10d' % register.max_index[0]
    output += form % (define, value)
    if register.dimensions == 2:
      define = 'HWIO_%s_MAX%s' % (register.Symbol(), register.arg[1])
      value  = '%10d' % register.max_index[1]
      output += form % (define, value)

    if regfile['output-resets'] and register.reset_value != None:
      define = 'HWIO_%s_POR' % (register.Symbol())
      value  = '%#010x' % (register.reset_value)
      output += form % (define, value)


    if register.dimensions == 1:
      index = ''
    elif register.dimensions == 2:
      index = '2'

    if IsRegisterShadowed(register, regfile):
      output += '#define HWIO_%s_INI%s(%s)        \\\n' % (register.Symbol(), index, arg_list)
      output += '        (HWIO_%s_shadow[%s] & HWIO_%s_RMSK)\n' % (register.Symbol(), register.ArgList(']['), register.Symbol())
      output += '#define HWIO_%s_INMI%s(%s,mask)    \\\n' % (register.Symbol(), index, arg_list)
      output += '        (HWIO_%s_shadow[%s] & (mask))\n' % (register.Symbol(), register.ArgList(']['))
      output += '#define HWIO_%s_OUTI%s(%s,val)    \\\n' % (register.Symbol(), index, arg_list)
      output += '        out_dword_masked(%s,0xFFFFFFFF,val,HWIO_%s_shadow[%s])\n' % (hwio_addr, register.Symbol(), register.ArgList(']['))
      output += '#define HWIO_%s_OUTMI%s(%s,mask,val) \\\n' % (register.Symbol(), index, arg_list)
      output += '        out_dword_masked(%s,mask,val,HWIO_%s_shadow[%s])\n' % (hwio_addr, register.Symbol(), register.ArgList(']['))

      if 'shadow-file' in regfile:
        if register.dimensions == 1:
          regfile['shadow-file'].write(
            '  #define HWIO_%s_shadow %s->HWIO_%s\n' % (register.Symbol(), regfile['shadow-ptr'], register.Symbol()) +
            '  uint32  HWIO_%s[%d];\n\n' % (register.Symbol(), register.max_index[0] + 1))
        elif register.dimensions == 2:
          regfile['shadow-file'].write(
            '  #define HWIO_%s_shadow hwio_shadow_ptr->HWIO_%s\n' % (register.Symbol(), register.Symbol()) +
            '  uint32  HWIO_%s[%d][%d];\n\n' % (register.Symbol(), register.max_index[0] + 1, register.max_index[1] + 1))
    else:
      if register.Readable():
        output += '#define HWIO_%s_INI%s(%s)        \\\n' % (register.Symbol(), index, arg_list)
        output += '        in_dword_masked(%s, HWIO_%s_RMSK)\n' % (hwio_addr, register.Symbol())
        output += '#define HWIO_%s_INMI%s(%s,mask)    \\\n' % (register.Symbol(), index, arg_list)
        output += '        in_dword_masked(%s, mask)\n' % hwio_addr
      if register.Writeable():
        output += '#define HWIO_%s_OUTI%s(%s,val)    \\\n' % (register.Symbol(), index, arg_list)
        output += '        out_dword(%s,val)\n' % hwio_addr
        if register.Readable():
          output += '#define HWIO_%s_OUTMI%s(%s,mask,val) \\\n' % (register.Symbol(), index, arg_list)
          output += '        out_dword_masked_ns(%s,mask,val,HWIO_%s_INI%s(%s))\n' % (hwio_addr, register.Symbol(), index, arg_list)

    for field in register.fields:
      output += ProcessField(field, regfile)

    output += '\n'

    f = regfile['file']
    f.write(output)

    register.IgnorePrefix(False)


# ============================================================================
# InitRegisterFile
#
# Opens and initializes a register file.
# ============================================================================

def InitRegisterFile (regfile):
  LogInfo('-- Creating: ' + regfile['filename'])

  if 'stdout' in regfile and regfile['stdout']:
    regfile['file'] = sys.stdout
  else:
    filename = regfile['filename']
    filename = re.sub('\\\\', '/', filename)
    m = re.search('(.*)/(.*)', filename)
    if m != None:
      path = m.group(1)
      filename = m.group(2)
      if not os.path.exists(path):
        os.makedirs(path)

    try:
      regfile['file'] = open(regfile['filename'], 'w')
    except:
      print ('** [Warning] Unable to open "%s"' % regfile['filename'])
      regfile['file'] = open(os.devnull, 'w')
  
  guard = re.sub('\.', '_', filename.upper())
  regfile['guard'] = guard

  f = regfile['file']
  f.write('#ifndef __%s__\n' % guard)
  f.write('#define __%s__\n' % guard)

  f.write('/*\n')
  f.write('===========================================================================\n')
  f.write('*/\n')
  f.write('/**\n')
  f.write('  @file %s\n' % filename)
  f.write('  @brief Auto-generated HWIO interface include file.\n')
  if len(regfile['bases']) > 0:
    f.write('\n')
    f.write('  This file contains HWIO register definitions for the following bases:\n')
    for base in regfile['bases']:
      f.write('    %s\n' % base)
  if len(regfile['modules']) > 0:
    f.write('\n')
    f.write('  This file contains HWIO register definitions for the following modules:\n')
    for module in regfile['modules']:
      f.write('    %s\n' % module)
  f.write('\n')
  f.write("  'Include' filters applied: ")
  if 'filter-include' in regfile:
    for filt in regfile['filter-include']:
      f.write(filt + ' ');
    f.write('\n')
  else:
    f.write('<none>\n')
  f.write("  'Exclude' filters applied: ")
  if 'filter-exclude' in regfile:
    for filt in regfile['filter-exclude']:
      f.write(filt + ' ');
    f.write('\n')
  else:
    f.write('<none>\n')
  f.write('*/')
  f.write('''
/*
  ===========================================================================

  Copyright (c) %d QUALCOMM Incorporated.''' % date.today().year + '''
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $''' + '''Header: $
  $''' + '''DateTime: $
  $''' + '''Author: $

  ===========================================================================
*/

''')

  if 'header' in regfile:
    f.write(regfile['header'] + '\n\n')


# ============================================================================
# CloseRegisterFile
#
# Finishes and closes a register file.
# ============================================================================

def CloseRegisterFile(regfile):
  if not 'file' in regfile:
    return

  f = regfile['file']

  if 'trailer' in regfile:
    f.write('\n' + regfile['trailer'] + '\n\n')

  f.write('\n#endif /* __%s__ */\n' % regfile['guard'])
  f.close()


# ============================================================================
# InitShadowFile
#
# Opens and initializes a register shadow file.
# ============================================================================

def InitShadowFile(regfile):
  LogInfo('-- Creating: ' + regfile['shadow-filename'])

  filename = regfile['shadow-filename']
  filename = re.sub('\\\\', '/', filename)
  m = re.search('(.*)/(.*)', filename)
  if m != None:
    path = m.group(1)
    filename = m.group(2)
    if not os.path.exists(path):
      os.makedirs(path)

  try:
    regfile['shadow-file'] = open(regfile['shadow-filename'], 'w')
  except:
    print ('** [Warning] Unable to open "%s"' % regfile['shadow-filename'])
    regfile['shadow-file'] = open(os.devnull, 'w')

  guard = re.sub('\.', '_', filename.upper())
  regfile['shadow-guard'] = guard

  f = regfile['shadow-file']
  f.write('#ifndef __%s__\n' % guard)
  f.write('#define __%s__\n' % guard)

  f.write('''
/*
===========================================================================
*/
/**
  @file %s''' % filename + '''
  @brief Auto-generated HWIO interface shadow include file.

  This file contains HWIO shadow register definitions.
*/
/*
  ===========================================================================

  Copyright (c) %d QUALCOMM Incorporated.''' % date.today().year + '''
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $''' + '''Header: $
  $''' + '''DateTime:  $
  $''' + '''Author: $

  ===========================================================================
*/

typedef struct
{
''')

  # Create a prefix for the shadow structure type that is the filename
  # with any extension removed.
  prefix = re.sub('\..$', '', filename)

  if not 'shadow-type' in regfile:
    regfile['shadow-type'] = prefix + '_type'
    
  if not 'shadow-ptr' in regfile:
    regfile['shadow-ptr'] = prefix + '_ptr'


# ============================================================================
# CloseShadowFile
#
# Finishes and closes a register file.
# ============================================================================

def CloseShadowFile(regfile):
  if not 'shadow-file' in regfile:
    return

  f = regfile['shadow-file']

  f.write(
'''  uint32 dummy;  /* Ensure at least one element */
} %s;

extern %s *%s;

#endif /* __%s__ */

''' % (regfile['shadow-type'], regfile['shadow-type'], regfile['shadow-ptr'], regfile['shadow-guard']))

  f.close()


# ============================================================================
# ProcessModule
#
# Parses a module
# ============================================================================

def ProcessModule (module, regfiles):

  for regfile in regfiles:
    f = regfile['file']

    f.write( '/*----------------------------------------------------------------------------\n' )
    f.write( ' * MODULE: %s\n' % module.name )
    f.write( ' *--------------------------------------------------------------------------*/\n' )
    f.write( '\n' )

    # Define generic format string to allow aligning all definitions
    form   = '#define %-' + str(module.longest_field_name + 20) + 's %s\n'

    define = '%s' % module.Symbol()
    value  = '(%s      + %#010x)' % (module.base.Symbol(), module.offset)
    f.write( form % (define, value) )

    if regfile['output-phys']:
      define = '%s_PHYS' % module.Symbol()
      value  = '(%s_PHYS + %#010x)' % (module.base.Symbol(), module.offset)
      f.write( form % (define, value) )

    if regfile['output-offsets']:
      define = '%s_OFFS' % module.Symbol()
      value  = '%#010x' % module.offset
      f.write( form % (define, value) )

    f.write( '\n' )

  for register in module.registers:
    if register.Array():
      ProcessRegisterArray(register, regfiles)
    else:
      ProcessRegister(register, regfiles)

    # Check if we have any definitions for creating array macros from a
    # register definition.
    for regfile in regfiles:
      if 'create-array' in regfile:
        for array_def in regfile['create-array']:
          if re.search(array_def[0], register.name):
            new_reg = HWIORegisterArray.CreateFromRegister(register, **array_def[1])
            ProcessRegisterArray(new_reg, regfiles)


# ============================================================================
# ProcessBase
#
# Parses and loads the data from the given address file.
# ============================================================================

def ProcessBase (base, base_regfiles, register_files):

  for module in base.modules:
    
    # Create the list of register files that want this module
    regfiles = []
    regfiles.extend(base_regfiles)
    for regfile in register_files:
      for module_re in regfile['modules']:
        if re.match(module_re + '$', module.name, re.I):
          regfile['__modules-matched'][module_re] = True
          if not (regfile in regfiles):
            regfiles.append(regfile)

    ProcessModule(module, regfiles)


# ============================================================================
# GenerateRegisters
#
# Generates output files.
# ============================================================================

def GenerateRegisters (bases, register_files):

  # Sanitize the register dictionaries
  for regfile in register_files:
    if not 'filename' in regfile:
      LogWarning("Missing 'filename' parameter in register output file.")
      register_files.remove(regfile)
      continue

    if not 'output-phys' in regfile:
      regfile['output-phys'] = False
    if not 'output-offsets' in regfile:
      regfile['output-offsets'] = False
    if not 'output-fvals' in regfile:
      regfile['output-fvals'] = False
    if not 'output-resets' in regfile:
      regfile['output-resets'] = False
    if not 'explicit-addressing' in regfile:
      regfile['explicit-addressing'] = False
    if not 'ignore-prefixes' in regfile:
      regfile['ignore-prefixes'] = regfile['explicit-addressing']
    if not 'bases' in regfile:
      regfile['bases'] = []
    if not 'modules' in regfile:
      regfile['modules'] = []
    if not 'filter-exclude' in regfile:
      regfile['filter-exclude'] = ['RESERVED', 'DUMMY']

    regfile['__bases-matched'] = {}
    regfile['__modules-matched'] = {}

    # Go through any module filters and ensure the dictionary keys are in
    # upper case since that is how module names are stored.
    if 'module-filter-include' in regfile:
      module_filt_dict = regfile['module-filter-include']
      for key in module_filt_dict.keys():
        if key.islower():
          module_filt_dict[key.upper()] = module_filt_dict[key]
    if 'module-filter-exclude' in regfile:
      module_filt_dict = regfile['module-filter-exclude']
      for key in module_filt_dict.keys():
        if key.islower():
          module_filt_dict[key.upper()] = module_filt_dict[key]

    # We allow '-' delimiters in the 'create-array' dictionary for
    # consistency with other parameters, but these need to be
    # converted to '_' for use as arguments to the CreateFromRegister
    # function.
    if 'create-array' in regfile:
      for array_def in regfile['create-array']:
        for key, value in array_def[1].items():
          if re.search('-', key):
            array_def[1][re.sub('-', '_', key)] = value

        if not 'array_offset' in array_def[1]:
          LogWarning("Missing 'array-offset' field in 'create-array' for %s." % array_def[0])
          array_def[1]['array_offset'] = 0

        if not 'max_index' in array_def[1]:
          LogWarning("Missing 'max-index' field in 'create-array' for %s." % array_def[0])
          array_def[1]['max_index'] = 0

  LogInfo("-- Generating register files...")

  for regfile in register_files:
    InitRegisterFile(regfile)
    if 'shadow-filename' in regfile:
      InitShadowFile(regfile)

  for base in bases:
    # Create the list of register files that want this base
    base_regfiles = []
    for regfile in register_files:
      for base_re in regfile['bases']:
        if re.match(base_re + '$', base.name, re.I):
          regfile['__bases-matched'][base_re] = True
          base_regfiles.append(regfile)

    ProcessBase(base, base_regfiles, register_files)

  # Close out the register files
  for regfile in register_files:
    CloseRegisterFile(regfile)
    CloseShadowFile(regfile)

  # Check for unmatched bases or modules
  for regfile in register_files:
    if 'modules' in regfile:
      for module_re in regfile['modules']:
        if not module_re in regfile['__modules-matched']:
          LogWarning("Module '%s' not found for '%s'" % (module_re, regfile['filename']))
    if 'bases' in regfile:
      for base_re in regfile['bases']:
        if not base_re in regfile['__bases-matched']:
          LogWarning("Base '%s' not found for '%s'" % (base_re, regfile['filename']))
      

# ============================================================================
# GetConfigHelp
#
# Return a string containing information on the configuration parameters.
# ============================================================================

def GetConfigHelp ():
  return '''

------------------------------------------------------------------------------
  REQUIRED PARAMETERS
------------------------------------------------------------------------------

filename (string): The filename to create, may include a path.
  Example:
    'filename': 'ClockHWIO.h'

modules (list of strings): List of modules to include.  Note the strings are
  regular expressions, and are not case sensitive.
  Example:
    'modules': ['CLK.*', 'LCC']

bases (list of strings): List of bases to include.  Note the strings are
  regular expressions.  Either or both of 'modules' and 'bases' should be
  listed.
  Example:
    'bases': ['MMSS_CC']

------------------------------------------------------------------------------
  FILTER OPTIONS
------------------------------------------------------------------------------

filter-exclude (list of strings): Define a list of symbols to exclude.  This
  filter is applied to register and field names.  Any that match are
  excluded from the output.  Note these are regular expressions.
  Example:
    # This example will remove all fields containing 'RESERVED', as
    # well as all registers like SDCn_ACR or GCC_RPU_CR.
    'filter-exclude': ['ACR', 'RPU', 'RESERVED']

filter-include (list of strings): Define a list of symbols to include.  This
  filter is applied to register and field names.  Only symbols that match
  are included in the output.  Note these are regular expressions.
  Example:
    'filter-include': ['HW_REVISION_NUMBER']

module-filter-include (dictionary of lists of strings key'ed by module name):
  Apply a filter to include only certain symbols on a per-module basis.
  Note the format with module name followed by array of symbols to include.
  Note that 'module-filter-exclude' is also supported similarly.
  Example:
    # Here we will only include HW_REVISION_NUMBER from the TLMM module, and
    # the two registers GSBI_nSIM_CLK_CTL and DFAB_A1_HCLK_CTL from the CLK_CTL
    # module.
    'module-filter-include': {
      'tlmm': ['HW_REVISION_NUMBER'],
      'clk_ctl': ['GSBIn_SIM_CLK_CTL', 'DFAB_A2_HCLK_CTL']
    }

module-filter-exclude (dictionary of lists of strings key'ed by module name):
  Same as module-filter-include but provide a module based set of symbols
  to exclude.

------------------------------------------------------------------------------
  ADDITIONAL OUTPUT GENERATION
------------------------------------------------------------------------------

output-phy (boolean): Set 'output-phys' to generate HWIO_<sym>_PHYS symbols
  showing the physical address of the register.  Generally not used.
  Example:
   'output-phys': True

output-offsets (boolean): Set 'output-offsets' to generate HWIO_<sym>_OFFS
  symbols showing the register offset from the base.
  Example:
    'output-offsets': True

output-fvals (boolean): Set 'output-fvals' to generate HWIO_<sym>_FVAL
  symbols showing the register field values for each field.  This enormously
  increases the size of the register file.
  Example:
    'output-fvals': True

output-resets (boolean): Set 'output-resets' to generate HWIO_<sym>_POR
  symbols showing the register reset value if defined.
  Example:
    'output-resets': True

explicit-addressing (boolean): Set this to generate HWIO macros that take
  the module address as an initial parameter.  This allows "explicitly"
  specifying the hardware instance.  To use this style of macro switch from
  HWIO_OUT(SYM, value) to HWIO_OUTX(base, SYM, value).
  Note when this option is set then register prefixes will automatically
  be removed from generated definitions.  If this behavior is undesired
  you may override by setting the 'ignore-prefixes' option to False.
  Example:
    'explicit-addressing': True

ignore-prefixes (boolean): Set this parameter to True ignore prefixes
  when generating register definitions.  Generally not required as it
  is set automatically when 'explicit-addressing' is enabled.
  Example:
    'ignore-prefixes': True
 
------------------------------------------------------------------------------
  EXTRA LESSER USED OPTIONS
------------------------------------------------------------------------------

header (string): Define header text to be included at the top of the generated
  file.  This can be used to define the dynamic base address for example.
  Example:
    'header':
      '/* HWIO base definitions */\\n' +
      'extern  uint32               HAL_clk_nHWIOBase;\\n' +
      '#define CLK_CTL_BASE         HAL_clk_nHWIOBase\\n',

trailer (string): Define trailer text to come after the register definitions.
  Here you can add a file that will override definitions if required.
  Example:
    'trailer': '#include "ClockHWIO_port.h"'

shadow (list of strings):  Marks some registers to be shadow'ed, which
  will generate read and read/mask/write macros for write-only
  registers using an in-memory shadow copy, named HWIO_<symbol>_shadow.
  Note it is up to the user of the register file to declare and define the
  shadow data.
  Example:
    # Shadow two registers
    #  HWIO_RTC_INT_ENABLE_shadow
    #  HWIO_MODEM_SSBIc_ENABLE_shadow[x] (where "x" is the size of the array)
    # The shadow data in this case would be something like:
    #  extern uint32 HWIO_RTC_INT_ENABLE_shadow
    # or
    #  #define HWIO_RTC_INT_ENABLE_shadow my_data.rtc_int_enable
    'shadow': ['RTC_INT_ENABLE', 'MODEM_SSBIc_ENABLE']

shadow-filename (string): Name of header file to create with the structure
  definition containing registers marked as shadowed in the 'shadow' option.
  Example:
    'shadow-filename': 'mydriver_shadow.h'
  This example will create a file called mydriver_shadow.h containing
  something like this (based on what registers are in the 'shadow' list:
    typedef struct
    {
      #define HWIO_RTC_INT_ENABLE_shadow mydriver_shadow_ptr->HWIO_RTC_INT_ENABLE
      uint32  HWIO_RTC_INT_ENABLE;
    } mydriver_shadow_type;
    extern mydriver_shadow_type *mydriver_shadow_ptr;

create-array (list of lists): Causes array-style macro definitions
  to be created using a given register.  This is used if the hardware has
  un-rolled the registers into something like XXX_REG0, XXX_REG1, XXX_REG2
  etc and we want to generate macros called XXX_REGn.
  Each list entry is a size-two list that defines a register (regular 
  expression) and the array parameters (dictionary).
  Example:
    # Here we create array access macros for all registers matching the
    # RXFE_ADC_\w+_ADC0 format (\w+ represents any string of non-space
    # characters).  The spacing between elements of the array will be set
    # to 0x100, the max index is 2 (meaning array size of 3), the "argument"
    # character to use is 'a', and the position in the string to replace with
    # the argument character is the last one (negative position means from
    # the end of the register name).  I.e. if we found a register:
    #   RXFE_ADC_CONTROL_ADC0
    # we would create an array definition called:
    #   RXFE_ADC_CONTROL_ADCa(0)-(2)
    'create-array': [
      [ 'RXFE_ADC_\w+_ADC0',
        { 'array_offset': 0x100, 'max_index': 2, 'arg': 'a', 'arg_pos': -1 } ],

'''


# ============================================================================
# PrintHelp
#
# Print command line help.
# ============================================================================

def PrintHelp():
  print '''
-- HWIOGenRegs Script --
Generate the HWIO register definitions files using the hardware address file.
Takes a configuration file input containing various python dictionaries.

HWIOGenRegs.py --cfg=<cfg_file> [ --flat=<address_file> ]
      [ --silent ] [ --help ]

  --cfg   : Configuration file containing what to generate.
  --flat  : Default is ARM_ADDRESS_FILE.FLAT in current directory.
  --help  : Complete help describing the config file format.
  --silent: Do not output anything to stdout.
'''


# ============================================================================
# ParseCommandLine
#
# Parse command line arguments.
# ============================================================================

def ParseCommandLine (argv):
  global address_file
  global config_files
  global silent

  try:                                
    opts, args = getopt.getopt(argv, "h", ["flat=", "cfg=", "silent",  "help"])
  except getopt.GetoptError:          
    LogWarning("Invalid command line.")
    PrintHelp()
    sys.exit(2)                     
  for opt, arg in opts:
    if opt == '-h':
      sys.exit()
    elif opt == "--flat":
      address_file = arg
    elif opt == "--cfg":
      config_files.append(arg)
    elif opt == "--silent":
      silent = True
    elif opt == "--help":
      PrintHelp()
      print "Config Format Help for the HWIO_REGISTER_FILES dictionary:"
      print GetConfigHelp()
      sys.exit()


# ============================================================================
# Main
#
# Entry point.
# ============================================================================

def Main (argv):
  from HWIOParser import LoadAddressFile

  config_mods = []

  ParseCommandLine(argv)

  # Disable compiling modules when importing settings
  dont_write_bytecode = sys.dont_write_bytecode
  sys.dont_write_bytecode = True

  for f in config_files:
    LogInfo("-- Importing config from '" + f + "'")
    config_mods.append(imp.load_source(f, f))

  # Restore setting
  sys.dont_write_bytecode = dont_write_bytecode

  bases = LoadAddressFile(address_file, silent)

  for mod in config_mods:
    if 'HWIO_REGISTER_FILES' in dir(mod):
      GenerateRegisters(bases, mod.HWIO_REGISTER_FILES)

if __name__ == "__main__":
  Main(sys.argv[1:])


# ============================================================================
# SCons Hooks
#
# These functions are used when loading this module as an SCons "tool".
# ============================================================================

def SConsGenerateRegisters (env, bases, register_files):
  global silent
  for regfile in register_files:
    regfile['filename'] = env.NormPath(regfile['filename'])
    if 'shadow-filename' in regfile:
      regfile['shadow-filename'] = env.NormPath(regfile['shadow-filename'])
  #silent = True
  GenerateRegisters(bases, register_files)

def generate (env):
  env.AddMethod(SConsGenerateRegisters, "HWIOGenerateRegisters")

def exists (env):
  return env.Detect('hwio_tools')


