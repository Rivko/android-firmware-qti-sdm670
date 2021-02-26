#!/usr/bin/env python
#-------------------------------------------------------------------------------
# Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#-------------------------------------------------------------------------------

import re
import os
import SCons.Action
import types
import sys
import pickle
import copy

from datetime import datetime
from SCons.Script import *

#-------------------------------------------------------------------------------
#  Hooks for SCons
#-------------------------------------------------------------------------------


# Instead of assembling the metadata file line by line, we now write it out here, and
# use python string formatting, combined with keyword arguments, to fill in the gaps 
# according to the metadata that was passed to SecureAppBuilder.
# See https://pyformat.info/ for proper usage of python's string format()
# Some things to keep in mind:
#   - if adding new metadata fields, be certain that the variable name you want to replace
#     doesn't already exist in the format string
#   - be very sure if you're trying to format in a raw string (like we are doing for uuid_string)
#     or otherwise (like abuf)
#   - if you want an actual brace to appear in the output string, you need to double up the braces 
ENTIRE_FILE_FORMAT_STRING = '''
//-------------------------------------------------------------------------
// Auto-generated file.  Do not edit.
//-------------------------------------------------------------------------
#include <stddef.h>
#include <stdbool.h>

#define TA_SYMBOL_EXPORT __attribute__((visibility("default")))

TA_SYMBOL_EXPORT const char __attribute__((section(".metadata.sentinel"))) TA_METADATA[] = {ta_metadata_str};
TA_SYMBOL_EXPORT const char *TA_APP_NAME = "{appname}";

TA_SYMBOL_EXPORT const unsigned char TA_UUID[] = {uuid_string};
                                  
TA_SYMBOL_EXPORT const size_t ta_acceptBufSize = {abuf};

TA_SYMBOL_EXPORT const size_t ta_stackSize = {stbuf};

TA_SYMBOL_EXPORT const size_t ta_heapSize = {hpbuf};

TA_SYMBOL_EXPORT const bool ta_cryptoSelfTest = {cryptoSelfTest};

TA_SYMBOL_EXPORT const bool ta_multiSession = {multiSession};

TA_SYMBOL_EXPORT const char *ta_version = "{version}";

TA_SYMBOL_EXPORT const char *ta_description = "{description}";

TA_SYMBOL_EXPORT const bool ta_filesNoPersist = {filesNoPersist};

TA_SYMBOL_EXPORT const size_t TA_MAX_STORAGE_FILES = {totalFiles};

TA_SYMBOL_EXPORT const struct TACustomProperty {{
   char const * name;
   char const * value;
}} ta_customProperties[] = {{
  {customProperties}
}};

TA_SYMBOL_EXPORT const unsigned int TA_CUSTOM_PROPERTIES_NUM = {propertiesNum};

'''



def exists(env):
   return env.Detect('secure_apps')

def generate(env):
   bld = env.Builder(action=add_secure_app_metadata)
   env.Append(BUILDERS={'AddSecureAppMetadata' : bld})
   
   if 'IDL_CLASS_MAP' not in env['ROOT_GLOBAL_ENV']:
     sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
     from SecureAppMetadata import IDLScanner
     idl_class_ids = IDLScanner().create_class_id_map()
     env['ROOT_GLOBAL_ENV']['IDL_CLASS_MAP'] = idl_class_ids


def encode_c_str(str):
    str = re.sub(r'[?"\\]', lambda m: '\\' + m.group(0), str)
    str = re.sub(r'[\x00-\x1f]', lambda m: '\\%03o' % ord(m.group(0)), str)
    return '"' + str + '"'

#-------------------------------------------------------------------------------
#  add_secure_app_metadata
#-------------------------------------------------------------------------------

def add_secure_app_metadata(env, target,source):
   sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
   from SecureAppMetadata import SecureAppMetadataClass
   idl_class_ids = copy.deepcopy(env['ROOT_GLOBAL_ENV']['IDL_CLASS_MAP'])
   SecureAppMetadata = SecureAppMetadataClass(idl_class_ids)

   
   md = pickle.loads(str(source[0]))
    
   out_path = target[0].get_abspath()
   print 'Generating module metadata file ' + out_path
   with file(out_path, 'w') as output:
      metadata_str = encode_c_str(SecureAppMetadata.generate_metadata(md))
      metadata_appname = SecureAppMetadata.extractField(md, 'appName')
      
      metadata_uuid = SecureAppMetadata.extractField(md, 'UUID')
      if metadata_uuid != None:
        metadata_uuid_bytes = re.findall('[0-9a-fA-F]{2}', metadata_uuid)
      else:
        metadata_uuid_bytes = ['ff'] * 16
      metadata_uuid_string = '{' + ', '.join(('0x' + x) for x in metadata_uuid_bytes) + '}'
      
      metadata_abuf = SecureAppMetadata.extractField(md, 'acceptBufSize')
      metadata_stbuf = SecureAppMetadata.extractField(md, 'stackSize')
      metadata_hpbuf = SecureAppMetadata.extractField(md, 'heapSize')
      
      metadata_filesnopersist = SecureAppMetadata.extractField(md, 'storageFilesNoPersist')
      metadata_totalfiles = SecureAppMetadata.extractField(md, 'totalStorageFiles')



      metadata_cryptoSelfTest = SecureAppMetadata.extractField(md, 'cryptoSelfTest')
      metadata_multiSession = SecureAppMetadata.extractField(md, 'multiSession')
      metadata_version = SecureAppMetadata.extractField(md, 'version')
      metadata_description = SecureAppMetadata.extractField(md, 'description')
      metadata_properties = SecureAppMetadata.extractField(md, 'properties')
      if metadata_properties is not None:
        metadata_propertiesString = ['{{"{}", "{}"}}'.format(p, metadata_properties[p]) for p in metadata_properties]
        metadata_propertiesString = ', '.join(metadata_propertiesString)
        metadata_propertiesNum = len(metadata_properties)
      else:
        metadata_propertiesString = ""
        metadata_propertiesNum = 0
        
      whole_file = ENTIRE_FILE_FORMAT_STRING.format(
          ta_metadata_str  = metadata_str,
          appname          = metadata_appname,
          uuid_string      = metadata_uuid_string,
          abuf             = metadata_abuf,
          stbuf            = metadata_stbuf,
          hpbuf            = metadata_hpbuf,
          cryptoSelfTest   = metadata_cryptoSelfTest,
          multiSession     = metadata_multiSession,
          version          = metadata_version,
          description      = metadata_description,
          filesNoPersist   = metadata_filesnopersist,
          totalFiles       = metadata_totalfiles,
          customProperties = metadata_propertiesString, 
          propertiesNum    = metadata_propertiesNum,
          )
      output.write(whole_file)