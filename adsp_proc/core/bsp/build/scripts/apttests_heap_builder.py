#============================================================================
#
# APTTESTS Heap Builder
#
# GENERAL DESCRIPTION
#    Modifies the customer configuration XML to reduce the heap size.  This 
#    memory reduction is required in order to allow the HCBSP package on 
#    ADSP to fit within the PHYSPOOL.  This script should only be used for
#    the HCBSP package.
#
# Copyright (c) 2009-2014 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/apttests_heap_builder.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#  $Change: 13907704 $
# 
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# YYYY-MM-DD   who     what, where, why
# ----------   ---     ---------------------------------------------------------
# 2014-03-31   SM/YW   Initial revision. 
# 
#============================================================================
import os
import re

#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
HEAP_USES_FLAG = 'USES_HEAP_REDUCTION'

#----------------------------------------------------------------------------
# Hooks for SCons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('apttests_heap_builder')

def generate(env):
   env.AddUsesFlags(HEAP_USES_FLAG)
   apttests_heap_generate(env)

#============================================================================
# Island Build Rules
#============================================================================
def apttests_heap_generate(env):

   heap_act = env.GetBuilderAction(apttests_heap_builder)
   # The generated file from this builder will have a *.xml extension.
   heap_bld = env.Builder(action = heap_act,suffix = '.xml')

   env.Append(BUILDERS = {'ReduceHeapSize' : heap_bld})
   
#-----------------------------------------------------------------------------
# Function:    island_builder
# Description: This is the builder abstraction.  We are expecting the installation
#              location as the target, and the source .xml as the source.
# Inputs:      None.
# Outputs:     None.
#-----------------------------------------------------------------------------
def apttests_heap_builder(target, source, env):
   list_fname = str(target[0])
   input_xml = str(source[0])
   reduce_heap_size(env, list_fname, input_xml)


#-----------------------------------------------------------------------------
# Function:    generic_file_create
# Description: This is the action for this builder.  It will open a new file up for
#              writing, and open up the input file for reading.  It will copy the 
#              read file line by line, unless it finds the heap value.  It will 
#              replace it.  if the size cannot be converted, we will throw  
# Inputs:      list_fname - The filename of the output list file.
# Outputs:     
#-----------------------------------------------------------------------------
def reduce_heap_size(env, list_fname, input_xml):
   # Open file for writing and reading.
   list_file = open(list_fname, "w")
   read_file = open(input_xml, "r")
   for lines in read_file:
     match = re.match(r'(\s+<heap_size\s+name="heap_size"\s+)(value\s+=\s+)"(0x\d+)"\s+/>\s+',lines)
     if match:
      #parse the value
      try:
         #match and parse value
         heap_size = int (match.group(3), 16)
         new_heap_size = heap_size - (1024 * 1024) - (512 * 1024)
         new_hex_str_val = "value = \""+hex(new_heap_size)+"\""
         #replace new line
         new_line = re.sub(r'value\s+=\s+"(0x\d+)"',new_hex_str_val,lines)
         list_file.write(new_line)
      except:
         env.PrintInfo("In ReduceHeapSize, cannot convert to int().")
     else:
      list_file.write(lines)
   
   # Close the files to writing and reading.
   read_file.close()
   list_file.close()



   

