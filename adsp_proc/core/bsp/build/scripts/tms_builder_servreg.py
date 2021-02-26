# vi: tw=128 ts=3 sw=3 et
# @file tms_builder_servreg.py
# @brief This file contains the API for the Service Registry, API 0.1.0
#===============================================================================
# NOTE: The @brief description above does not appear in the PDF.
# The tms_mainpage.dox file contains the group/module descriptions that
# are displayed in the output PDF generated using Doxygen and LaTeX. To
# edit or update any of the group/module text in the PDF, edit the
# tms_mainpage.dox file or contact Tech Pubs.
#===============================================================================
#===============================================================================
# Copyright (c) 2014 - 2017 Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary.
#===============================================================================
#===============================================================================
# Edit History
# $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/tms_builder_servreg.py#2 $
# $DateTime: 2017/08/17 11:25:19 $
# $Change: 14115431 $
# $Author: pwbldsvc $
#===============================================================================

import sys
if sys.hexversion < 0x02070000:
   raise Exception("python version not supported, update to 2.7 or better")

# pywin32 check (SCONS construction environment not available yet)

if 'linux2' == sys.platform:
   pass

elif 'win32' == sys.platform:
   try:
      from win32api import GetComputerName
      name = GetComputerName()
   except:
      print "pywin32 is not available, update to build 219 or better, available at http://sourceforge.net/projects/pywin32/files/pywin32/"
      #raise

import csv
import os
import fnmatch
from SCons.Script import *
import operator
from operator import itemgetter
from collections import OrderedDict
import inspect
import json

# debug instruments exposed by changing command line as so:
# build.cmd build_target BUILD_VER=00000 --debug=stacktrace --debuginfo=err

#===============================================================================
# Description
#===============================================================================

# This SCONS tool is the build time support for SERVICE REGISTRY(SR) build time services.

#===============================================================================
# Examples
#===============================================================================

#if 'USES_SERVICE_REGISTRY' in env:
# env.AddServRegDomain(
#  BUILD_TAGS,
#  {
#   'soc'                  : 'unique_name',
#   'domain'               : 'unique_name',
#   'subdomain'            : 'unique_name'
#  })
#
# env.AddServRegService(
#  BUILD_TAGS,
#  {
#   'provider'              : 'unique_name',
#   'service'               : 'unique_name'
#  })

#===============================================================================
# IMAGE OWNER Installation Notes, Best Practices
#===============================================================================

# 1. Insure build_tags is the variable name that is used to reflect
# the list of build tag aliases associated to the image/library.
# Edit the scons file if necessary; do not attempt to use a different
# variable name.
#
# build_tags = [ ... ]
#
# 2. BUILD TAGS ARE IMPORTANT; if any specific intermediate output
# is not present, or empty (0 bytes size), that is an indication that
# the build_tags list may not match client specified build_tags. Have
# the tech area *add* a specific build_tag alias to the tech area SCONS
# build file. An IMAGE OWNER should never add a tech area build tag to
# the SCONS file of the image/library.
#
# 3. Edit the InitImageVars() parameters to specify 'build_tags = build_tags'
# The IMAGE OWNER needs to own and protect build_tags from arbitrary
# modification. BUILD TAGS ARE IMPORTANT.
#
# 4. Edit the InitImageVars() parameters to specify 'tms_builder_servreg.py' is
# included with the list of other tools.
#
#env.InitImageVars(
#   ...
#   build_tags = build_tags,
#   ...
#   tools = ['buildspec_builder.py',
#            ...,
#            '${BUILD_ROOT}/core/bsp/build/scripts/tms_builder_servreg.py',
#            ...,
#   )
#
# 5. Edit the UsesFlags section to include the following line. It must
# follow the InitImageVars() call, and it must precede all LoadAreaSoftwareUnits()
# calls. When there are no LoadAreaSoftwareUnits() calls, it must precede all
# link steps in the *_img.scons file.
#
# For every IMAGE being created (*_user.scons, *_root.scons) or *_img.scons
#
# 6. Copy, Paste the following fragment into (*_user.scons, *_root.scons) or *_img.scons files.
#
# 7. There should be limited modification to the SCONS scripting below.
# Modify Variable PLAYLISTS only to reference intermediate outputs by path.
# Do not modify or decorate any of the output files; tech areas have expectations
# of existing names. Do not pollute the inherited construction environment with
# changes to CFLAGS, RequiredPublicApi(), or RequireRestrictedApi(). Construction
# environment of the (*_user.scons, *_root.scons) or *_img.scons file is inherited to all 
# dependants loaded.

"""
#--- SERVICE REGISTRY Playbook Extension, Image Specific Details -----------BEGIN

build_tags = ['CORE_QDSP6_SW', 'AVS_ADSP', 'AVS_ADSP_ROOT', 'VIDEO_ADSP', 'QMIMSGS_ADSP',
              'IMAGE_TREE_VERSION_AUTO_GENERATE',
              'IMAGE_TREE_UUID_AUTO_GENERATE',
             ]

# Follows all LoadAreaSoftwareUnits(). Precedes Link Step Details.

# FOR SCONS TOOL EMITTERS TO PLACE OUTPUT PROPERLY
if not os.path.exists(env.RealPath('${SHORT_BUILDPATH}')):
   if Execute(Mkdir(env.RealPath('${SHORT_BUILDPATH}'))):
      raise

# ONLY WHEN tms_builder_servreg SCONS TOOL LOADED
if 'USES_SERVICE_REGISTRY' in env and 'USES_SERVICE_REGISTRY_PLAYBOOK' in env:

   # NEVER POLLUTE ENV CONSTRUCTION ENVIRONMENT WHICH GETS INHERITED
   playbook_servreg_env = env.Clone()

   # EVERY IMAGE OWNER HAS TO DEFINE THE DOMAIN SCOPES USING AddServRegDomain() API
   playbook_servreg_env.AddServRegDomain(
      build_tags,
      {
         'soc'                  : 'mdm',
         'domain'               : 'apps',
         'subdomain'            : 'root_pd',
      })

   # PLAYLIST TO STORE THE SERVICE REGISTRY - DOMAIN INFORMATION
   servreg_out_pl = playbook_servreg_env.RealPath('${SHORT_BUILDPATH}/servreg_playlist.pl')
   playbook_servreg_env.AddServRegPlaylist(build_tags, servreg_out_pl)
   playbook_servreg_env.Depends(build_tags, servreg_out_pl)
   image_units.append(servreg_out_pl)

   # LIST OF ALL PLAYLISTS THAT WILL CONTRIBUTE TO MAKE THE OUTPUT JSON FILE
   SERVREG_PLAYLISTS = [ # IMAGE_OWNER ADDS SPECIFIC SERVREG_PLAYLISTS, ONE PER LIBRARY, EXPLICIT, NOT AUTO_MAGIC
      '${BUILD_ROOT}/build/bsp/apps_proc_img/build/ACINAAAA/servreg_playlist.pl',
   ]

   # CREATION OF OUTPUT JSON FILE
   servreg_out_json = playbook_servreg_env.RealPath('${SHORT_BUILDPATH}/${TARGET_NAME}_servreg.json')
   playbook_servreg_env.AddServRegPlaybook(build_tags, servreg_out_json, SERVREG_PLAYLISTS)
   playbook_servreg_env.Depends(build_tags, servreg_out_json)
   image_units.append(servreg_out_json)

   # CREATEION OF OUTPUT C FILE i.e THE SERVICE REGISTRY LOCAL DATABASE
   servreg_out_c = playbook_servreg_env.RealPath('${SHORT_BUILDPATH}/servreg_local_db_autogen.c')
   playbook_servreg_env.AddServRegPlaybook(build_tags, servreg_out_c, SERVREG_PLAYLISTS)
   servreg_obj = playbook_servreg_env.AddObject(build_tags, servreg_out_c)
   playbook_servreg_env.Depends(build_tags, servreg_out_c)
   image_objs.append(servreg_obj)

#--- SERVICE REGISTRY Playbook Extension, Image Specific Details ----------- END
"""

# For every LIBRARY being created (*_libs.scons)
#
# 8. Copy, Paste the following fragment into *_libs.scons files.
#
# 9. There should be no modification to the SCONS scripting below.
# Do not modify or decorate any of the output files; tech areas have expectations
# of existing names. Do not pollute the inherited construction environment with
# changes to CFLAGS, RequiredPublicApi(), or RequireRestrictedApi(). Construction
# environment of the *_libs.scons file is inherited to all dependants loaded.

"""
#--- SERVICE REGISTRY Playbook Extension, Library Specific Details ------------BEGIN
build_tags = ['CORE_QDSP6_SW', 'BUILD_TEST_MODEM', 'QDSS_EN_IMG','AVS_ADSP','VIDEO_ADSP', 'QMIMSGS_ADSP', 'CORE_ADSP', 'IMAGE_TREE_VERSION_AUTO_GENERATE' ]
SERVREG_PLAYLISTS = [ ] # NONE USED

# FOR SCONS TOOL EMITTERS TO PLACE OUTPUT PROPERLY
if not os.path.exists(env.RealPath('${SHORT_BUILDPATH}')):
   if Execute(Mkdir(env.RealPath('${SHORT_BUILDPATH}'))):
      raise

# ONLY WHEN DNT_BUILDER_SREG SCONS TOOL LOADED
if 'USES_SERVICE_REGISTRY' in env and 'USES_SERVICE_REGISTRY_PLAYBOOK' in env:

   # NEVER POLLUTE ENV CONSTRUCTION ENVIRONMENT WHICH GETS INHERITED
   playbook_servreg_env = env.Clone()

   # PLAYLIST OUTPUT THIS LIBRARY
   servreg_out_pl = playbook_servreg_env.RealPath('${SHORT_BUILDPATH}/servreg_playlist.pl')
   playbook_servreg_env.AddServRegPlaylist(build_tags, servreg_out_pl)
   playbook_servreg_env.Depends(build_tags, servreg_out_pl)
   image_units.append(servreg_out_pl)

#--- SERVICE REGISTRY Playbook Extension, Library Specific Details ------------END
"""
#===============================================================================
# Tool functions
#
# exists
# generate
# tool_update_file_playbook_c
# tool_update_file_playbook_json
# tool_update_file_playlist
# tool_emitter
# tool_target_scanner
# tool_action
# tool_method_add_playlist
# tool_method_add_playbook
# tool_method_add_domain
# tool_method_add_service
# tool_error
# tool_warning
# tool_info
# tool_debug
# tool_hash
# tool_randstr
#===============================================================================

#===============================================================================
# Module Scope Definitions
#===============================================================================

TOOL_USES_FLAG = 'USES_SERVICE_REGISTRY'             # Specific Tool USES_ Flag
TOOL_ENV       = 'ENV_SERVICE_REGISTRY'              # Specific Tool Construction Environment
TOOL_COLLECTOR = 'SERVICE_REGISTRY_COLLECTOR'        # Specific Tool Collector

# Tool Keywords

TOOL_KEY_VERSION_OBJ        = 'sr_version'               # keyword
TOOL_KEY_MAJOR_NAME         = 'major'
TOOL_KEY_MINOR_NAME         = 'minor'
TOOL_KEY_PATCH_NAME         = 'patch'
TOOL_KEY_MAJOR              = 1
TOOL_KEY_MINOR              = 1
TOOL_KEY_PATCH              = 1

TOOL_KEY_DOMAIN_OBJ         = 'sr_domain'
TOOL_KEY_SOC_NAME           = 'soc'
TOOL_KEY_SOC_LIST           = ['msm','apq','mdm']
TOOL_KEY_DOMAIN_NAME        = 'domain'
TOOL_KEY_DOMAIN_LIST        = ['modem','adsp','slpi', 'apps', 'cdsp']
TOOL_KEY_SUBDOMAIN_NAME     = 'subdomain'
TOOL_KEY_SUBDOMAIN_LIST     = ['root_pd','modem_pd','audio_pd','sensor_pd','wlan_pd','test_pd']
TOOL_KEY_QMI_INSTANCE_ID    = 'qmi_instance_id'

TOOL_KEY_SERVICE_OBJ        = 'sr_service'
TOOL_KEY_PROVIDER_NAME      = 'provider'
TOOL_KEY_SERVICE_NAME       = 'service'
TOOL_KEY_SERVICE_DATA_VALID = 'service_data_valid'
TOOL_KEY_SERVICE_DATA       = 'service_data'

TOOL_KEY_DESCRIPTION        = 'description'           # keyword
TOOL_KEY_DEFINED            = 'defined'               # INTERNAL, NOT CLIENT USED

# Tool Playbook, Playlist Output Extensions

PLAYLIST_EXT_PL             = '.pl'                   # Playlist Raw
PLAYBOOK_EXT_C              = '.c'                    # Playbook Source
PLAYBOOK_EXT_JSN            = '.json'                 # Playbook JSON

#===============================================================================
# SCONS Hooks
#===============================================================================

# exists: Determines if the compilers, assemblers, linkers, tools,
# etc.. are available and present. this controls the generation of a
# construction environment associated to env.

def exists(env):
   ''' SCONS Tool, Exists, Check Conditions to Install a Construction Environment '''
   tool_debug(env, 'exists()')

   # Problem in SCONS Configuration
   if env is None or not isinstance(env, Environment):
      raise Exception()

   return True

# generate: This generates a construction environment for use by
# scons; the environment is provided to use.

def generate(env):
   ''' SCONS Tool, Generate, Generate a Construction Environment '''
   tool_debug(env, 'generate()')

   if env is None or not isinstance(env, Environment):
      raise Exception()

   if not env['MSM_ID'] == '9x45':
      env.AddUsesFlags(TOOL_USES_FLAG)

   env[TOOL_ENV] = env        # use first env for construction env task/function dictionary
   env_tool = env[TOOL_ENV]   # construct env task/function dictionary

   env_tool[TOOL_COLLECTOR]= []

   action = env.GetBuilderAction(tool_action, action_source = None)
   scanner = env.Scanner(tool_target_scanner)
   builder = env.Builder(action = action, target_scanner = scanner, emitter = tool_emitter)

   env.Append(BUILDERS = {'ServRegBuilder' : builder}) # internalized

   env.AddMethod(tool_method_add_domain, 'AddServRegDomain')         # method for use by *_img.scons
   env.AddMethod(tool_method_add_service, 'AddServRegService')       # method for use by *_client.scons

   env.AddMethod(tool_method_add_servreg_playlist, 'AddServRegPlaylist')
   env.AddMethod(tool_method_add_servreg_playbook, 'AddServRegPlaybook')

#===============================================================================

def tool_update_file_playbook_c(env, target, content):
   ''' Output TGT File '''
   tool_debug(env, 'tool_update_file_playbook('+target+')')
   try:
      with open(target, 'w') as output:
         output.write(env.CreateFileBanner(os.path.basename(str(target)), style='C'))
         output.write('\n')
         for line in content:
            output.write(line)
   except:
      print "Failed to open service reg c file."
      raise Exception()

def tool_update_file_playbook_json(env, target, content):
   ''' Output TGT File '''
   tool_debug(env, 'tool_update_file_playbook_json('+target+')')
   try:
      with open(target, 'w') as outfile:
         json.dump(content, outfile, sort_keys=False, indent=4, separators=(',', ': '))
   except:
      print "Failed to open service reg json file."
      raise Exception()

def tool_update_file_playlist(env, target): # Always internally an OrderedDict
   ''' Output PL File Intermediate '''
   tool_debug(env, 'tool_update_file_playlist('+target+')')
   sorted_entry = env[TOOL_COLLECTOR]
   try:
      with open(target, 'w') as output:
         for entry in sorted_entry:
            output.write(str(entry) + '\n')
   except:
      print "Failed to open service reg playlist file."
      raise Exception()

#===============================================================================

# emitter: Run while reading scon scripts; used to tweak the targets
# and sources of the build step (eg, library building might also
# create intermediates that need to be added to targets, or cleanup
# artifacts). There is no file system activity here, just the
# specification of dependency.

# Returns: tuple of 2 lists, [target], [source]

def tool_emitter(target, source, env):
   ''' SCONS Tool, Emitter, Sanitize Target/Source Dependency '''
   tool_debug(env, 'tool_emitter('+str(target[0])+')')

   targets = []
   sources = []

   # Nodes from source list

   if source != None:
      for s in source:
         if s != None:
            sources.append(File(env.RealPath(str(s))))

   # Anticipated target dependency information. The associated files
   # are created with the target as intended side effect. Note, each
   # of the targets appended will be fed to the target_scanner later.

   # Pull apart target and source pathing information for utilization
   # by tool_emitter.

   if target != None:
      for t in target:
         if t != None:

            target_full = env.RealPath(str(t))
            target_path = os.path.split(target_full)[0]
            target_stem, target_ext = os.path.splitext(target_full)

            if target_ext == PLAYLIST_EXT_PL:

               output = File(target_stem + PLAYLIST_EXT_PL)
               env.Clean(target, output)
               targets.append(output)

            elif target_ext == PLAYBOOK_EXT_C:

               output = File(target_stem + PLAYBOOK_EXT_C)
               env.Clean(target, output)
               targets.append(output)

            elif target_ext == PLAYBOOK_EXT_JSN:

               output = File(target_stem + PLAYBOOK_EXT_JSN)
               env.Clean(target, output)
               targets.append(output)

   return targets, sources

#===============================================================================

#target_scanner: Run when explicit dependencies are up-to-date and it
#is otherwise possible for the step to be executed to build the file.
#This may add implicit dependency that must exist before the step
#can run. (eg, rebuild libraries before a program is linked). Here is
#where some files called out by tool_emitter will be created if they
#do not exist.

# Realize that this routine is subject to concurrent execution, and
# must not make any changes to the construction environment. Also, it
# should be noted that concurrent calling for a single node potentially
# would overwrite a file being concurrently created by a different
# node.

def tool_target_scanner(node, env, path):
   ''' SCONS Tool, Target Scanner, ... '''
   tool_debug(env, 'tool_target_scanner('+str(node)+')')

   returnList = [node]

   # Ignore output creation on appropriate SCONS options
   if env.GetOption('clean') or env.GetOption('no_exec'):
      tool_debug(env, 'TARGET CLEAN/NO_EXEC during tool scanner')
      return returnList

   # 'globals' information saved in the construction environment
   env_tool = env[TOOL_ENV] # construct env task/function dictionary

   target_full = env.RealPath(str(node))
   target_path = os.path.split(target_full)[0]
   target_stem, target_ext = os.path.splitext(target_full)

   # dependency target supplied by emitter

   if target_ext == PLAYLIST_EXT_PL:
      tool_update_file_playlist(env_tool, target_full)

   return returnList


#===============================================================================

# action: This is the portion of the tool that takes the source list
# and creates a target from it.

def tool_action(target, source, env):
   ''' SCONS Tool, Action Function, ... '''
   tool_debug(env, 'tool_action('+str(target[0])+')')

   # Ignore output creation on appropriate SCONS options
   if env.GetOption('clean') or env.GetOption('no_exec'):
      tool_debug(env, 'TARGET CLEAN/NO_EXEC during tool action')
      return 0 # Indicate SUCCESS back

   # 'globals' information saved in the construction environment
   env_tool = env[TOOL_ENV] # construct env task/function dictionary

   target_full = env.RealPath(str(target[0]))
   target_path = os.path.split(target_full)[0]
   target_stem, target_ext = os.path.splitext(target_full)

   # Loading of Playlist files to create the Playbook; tool_emitter has already
   # sanitized the list to those that exist.

   # Preparing Json objects : version, SR_Domain, SR_Service
   json_version_dict_item = OrderedDict([(TOOL_KEY_MAJOR_NAME, TOOL_KEY_MAJOR), (TOOL_KEY_MINOR_NAME, TOOL_KEY_MINOR), (TOOL_KEY_PATCH_NAME, TOOL_KEY_PATCH)])
   json_domain_dict_item = OrderedDict()
   json_service_array = []
   json_service_count = 0

   # Preparing for 'servreg_local_db_autogen.c' file
   Playbook_C_Output = []
   Playbook_C_Output.append('/*Service Registry local database of "soc/domain/subdomain/provider/service"*/\n')
   Playbook_C_Output.append('/*Service is said to be LOCAL if the client and the service are in the same soc/domain/subdomain*/\n')
   Playbook_C_Output.append('\n')
   Playbook_C_Output.append('#include "string.h"\n\n')
   Playbook_C_Output.append('struct servreg_entry_s\n')
   Playbook_C_Output.append('{\n')
   Playbook_C_Output.append('char * service_name;\n')
   Playbook_C_Output.append('unsigned int service_data_valid;\n')
   Playbook_C_Output.append('unsigned int service_data;\n')
   Playbook_C_Output.append('};\n')
   Playbook_C_Output.append('typedef struct servreg_entry_s servreg_entry_t, * servreg_entry_p;\n\n')

   Playbook_C_Output.append('const servreg_entry_t servreg_local_services[]={\n')

   playlists = map(lambda x: str(x), source)
   for playlist in playlists:
      playlist_stem, playlist_ext = os.path.splitext(env.RealPath(playlist))
      if playlist_ext == PLAYLIST_EXT_PL:
         tool_debug(env, 'Loading Playlist ' + playlist)
         with open(playlist, 'r') as input:
            for line in input:
               entry = eval(line)
               if isinstance(entry, OrderedDict):

                  if entry.has_key(TOOL_KEY_SOC_NAME) and entry.has_key(TOOL_KEY_DOMAIN_NAME) and entry.has_key(TOOL_KEY_SUBDOMAIN_NAME):
                     json_domain_dict_item = OrderedDict([(TOOL_KEY_SOC_NAME, entry[TOOL_KEY_SOC_NAME]), (TOOL_KEY_DOMAIN_NAME, entry[TOOL_KEY_DOMAIN_NAME]), (TOOL_KEY_SUBDOMAIN_NAME, entry[TOOL_KEY_SUBDOMAIN_NAME]), (TOOL_KEY_QMI_INSTANCE_ID, entry[TOOL_KEY_QMI_INSTANCE_ID])])
                     tool_debug(env, 'Specified Playlist has ' + TOOL_KEY_SOC_NAME + ',' + TOOL_KEY_DOMAIN_NAME + ' and ' + TOOL_KEY_SUBDOMAIN_NAME + ' information check file: {:s}'.format(playlist))

                  if entry.has_key(TOOL_KEY_PROVIDER_NAME) and entry.has_key(TOOL_KEY_SERVICE_NAME) and entry.has_key(TOOL_KEY_SERVICE_DATA_VALID) and entry.has_key(TOOL_KEY_SERVICE_DATA):
                     json_service_dict_item = OrderedDict([(TOOL_KEY_PROVIDER_NAME, entry[TOOL_KEY_PROVIDER_NAME]), (TOOL_KEY_SERVICE_NAME, entry[TOOL_KEY_SERVICE_NAME]), (TOOL_KEY_SERVICE_DATA_VALID, entry[TOOL_KEY_SERVICE_DATA_VALID]), (TOOL_KEY_SERVICE_DATA, entry[TOOL_KEY_SERVICE_DATA])])
                     json_service_array.append(json_service_dict_item)
                     json_service_count = json_service_count + 1

                     Playbook_C_Output.append('   {"' + json_service_dict_item[TOOL_KEY_PROVIDER_NAME] + "/" + json_service_dict_item[TOOL_KEY_SERVICE_NAME] + '", {:d} '.format(json_service_dict_item[TOOL_KEY_SERVICE_DATA_VALID]) + ', {:d}'.format(json_service_dict_item[TOOL_KEY_SERVICE_DATA]) + '},\n')
                     tool_debug(env, 'Specified Playlist has ' + TOOL_KEY_PROVIDER_NAME + ' and ' + TOOL_KEY_SERVICE_NAME + ' infomation check file: {:s}'.format(playlist))
               else:
                  tool_error(env, 'Specified Playlist incorrect internal format, no matching instance {:s}'.format(playlist))


   Playbook_C_Output.append('};\n\n')
   #Playbook_C_Output.append('char *servreg_local_domain = "' + c_local_domain_dict + '";\n')

   # Remove duplicate provider, service entries 
   json_service_dict_array = []
   for item in json_service_array:
      if item not in json_service_dict_array:
         json_service_dict_array.append(item)
      else :
         tool_error(env, 'Error: Same ' + TOOL_KEY_PROVIDER_NAME + ' and ' + TOOL_KEY_SERVICE_NAME + ' multiply defined. Remove double definition in tech-area scons file')

   # Intermediate Output
   if target_ext == PLAYLIST_EXT_PL:
      tool_update_file_playlist(env_tool, target_full)

   # Final Output
   elif target_ext == PLAYBOOK_EXT_JSN:

      # Final JSON dictionary
      json_dict = OrderedDict([(TOOL_KEY_VERSION_OBJ, json_version_dict_item), (TOOL_KEY_DOMAIN_OBJ, json_domain_dict_item), (TOOL_KEY_SERVICE_OBJ, json_service_dict_array)])

      if not json_dict[TOOL_KEY_DOMAIN_OBJ].has_key(TOOL_KEY_SOC_NAME):
         tool_error(env, TOOL_KEY_SOC_NAME + ' not defined. Image owners use env.AddServRegDomain() API to define it.')

      if not json_dict[TOOL_KEY_DOMAIN_OBJ].has_key(TOOL_KEY_DOMAIN_NAME):
         tool_error(env, TOOL_KEY_DOMAIN_NAME + ' not defined. Image owners use env.AddServRegDomain() API to define it.')

      if not json_dict[TOOL_KEY_DOMAIN_OBJ].has_key(TOOL_KEY_SUBDOMAIN_NAME):
         tool_error(env, TOOL_KEY_SUBDOMAIN_NAME + ' not defined. Image owners use env.AddServRegDomain() API to define it.')

      # Dump the final JSON dictionary to *.json file
      tool_update_file_playbook_json(env_tool, target_full, json_dict)
      
   elif target_ext == PLAYBOOK_EXT_C:
      Playbook_C_Output.append('const unsigned int servreg_service_count = ' + str(json_service_count) + ';\n')
      Playbook_C_Output.append('const char *servreg_local_domain = "' + json_domain_dict_item[TOOL_KEY_SOC_NAME] + '/' + json_domain_dict_item[TOOL_KEY_DOMAIN_NAME] + '/' + json_domain_dict_item[TOOL_KEY_SUBDOMAIN_NAME] + '";\n')
      Playbook_C_Output.append('const char *servreg_soc_name = "' + json_domain_dict_item[TOOL_KEY_SOC_NAME] + '";\n')
      Playbook_C_Output.append('const char *servreg_domain_name = "' + json_domain_dict_item[TOOL_KEY_DOMAIN_NAME] + '";\n')
      Playbook_C_Output.append('const char *servreg_subdomain_name = "' + json_domain_dict_item[TOOL_KEY_SUBDOMAIN_NAME] + '";\n')
      Playbook_C_Output.append('const unsigned int servreg_qmi_instance_id = {:d} '.format(json_domain_dict_item[TOOL_KEY_QMI_INSTANCE_ID]) + ';\n')
      tool_update_file_playbook_c(env_tool, target_full, Playbook_C_Output)

   return 0 # Indicate SUCCESS back

#===============================================================================

def tool_method_add_servreg_playlist(env, build_ids, target):
   ''' Method Call, Calls pseudoBuilder '''
   tool_debug(env, 'tool_method_add_servreg_playlist('+str(target[0])+')')

   if not isinstance(target, list):
      target = [target]

   target_full = env.RealPath(str(target[0]))
   target_path = os.path.split(target_full)[0]
   target_stem, output_ext = os.path.splitext(target_full)

   if output_ext == '':
      target_full = target_stem + PLAYLIST_EXT_PL
      env.ServRegBuilder(target=target_full, source=None)

   elif output_ext == PLAYLIST_EXT_PL:
      env.ServRegBuilder(target=target_full, source=None)

   else:
      tool_error(env, 'Method call with unrecognised file type "' + output_ext + '"')

   return target

#===============================================================================

def tool_method_add_servreg_playbook(env, build_ids, target, source):
   ''' Method Call, Calls pseudoBuilder '''
   tool_debug(env, 'tool_method_add_servreg_playbook('+str(target[0])+')')

   if not isinstance(source, list):
      source = [source]

   if not isinstance(target, list):
      target = [target]

   target_full = env.RealPath(str(target[0]))
   target_path = os.path.split(target_full)[0]
   target_stem, output_ext = os.path.splitext(target_full)

   if output_ext == PLAYLIST_EXT_PL:
      env.ServRegBuilder(target=target_full, source=source)
      env.RequirePublicApi(['DEBUGTOOLS', 'TMS',]) # Required to Compile

   elif output_ext == PLAYLIST_EXT_PL:
      env.ServRegBuilder(target=target_full, source=source)

   elif output_ext == PLAYBOOK_EXT_C:
      env.ServRegBuilder(target=target_full, source=source)
      env.RequirePublicApi(['DEBUGTOOLS', 'TMS',]) # Required to Compile

   elif output_ext == PLAYBOOK_EXT_JSN:
      env.ServRegBuilder(target=target_full, source=source)

   else:
      tool_error(env, 'Method call with unrecognised file type "' + output_ext + '"')

   return target, source

#===============================================================================

def tool_method_add_domain(env, build_ids, input_dict):
   ''' Method Call, Adds Function
   Validates input and stores to 'globals' construction environment
   Runs before tool_emitter() function '''

   temp = 0
   hash = 0

   # 'globals' information saved in the construction environment
   env_tool = env[TOOL_ENV] # construct env task/function dictionary

   # caller python frame of interest
   frame, filename, line_number, function_name, lines, index = inspect.getouterframes(inspect.currentframe())[2]

   # define the layout of the ordered dictionary, all fields in expected
   # ordering of the fields
   output_dict = OrderedDict()
   output_dict[TOOL_KEY_SOC_NAME] = None
   output_dict[TOOL_KEY_DOMAIN_NAME] = None
   output_dict[TOOL_KEY_SUBDOMAIN_NAME] = None
   output_dict[TOOL_KEY_QMI_INSTANCE_ID] = None

   output_text = '..../'+filename.replace('\\','/').lower().replace(env.get('BUILD_ROOT', None).lower()+'/', '')
   output_dict[TOOL_KEY_DEFINED] = { 'file': output_text, 'line': line_number }

   #only one soc, domain and subdomain entry per image
   for entry in env_tool[TOOL_COLLECTOR]:
      if entry.has_key(TOOL_KEY_SOC_NAME) or entry.has_key(TOOL_KEY_DOMAIN_NAME) or entry.has_key(TOOL_KEY_SUBDOMAIN_NAME) :
         tool_error(env, TOOL_KEY_SOC_NAME + ', ' + TOOL_KEY_DOMAIN_NAME + ' and ' + TOOL_KEY_SUBDOMAIN_NAME + ' already defined {:s}, {:s}, {:s} "{:s}" {:d}'.format(entry[TOOL_KEY_SOC_NAME], entry[TOOL_KEY_DOMAIN_NAME], entry[TOOL_KEY_SUBDOMAIN_NAME], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))

   #read in soc info
   if not input_dict.has_key(TOOL_KEY_SOC_NAME):
      tool_error(env, TOOL_KEY_SOC_NAME + ' required')

   if not input_dict[TOOL_KEY_SOC_NAME] in TOOL_KEY_SOC_LIST:
      tool_error(env, input_dict[TOOL_KEY_SOC_NAME] + ' is not a valid ' + TOOL_KEY_SOC_NAME + ' name. Get name from list = [apq, msm, mdm] ')

   #read in domain info
   if not input_dict.has_key(TOOL_KEY_DOMAIN_NAME):
      tool_error(env, TOOL_KEY_DOMAIN_NAME + ' required')

   if not input_dict[TOOL_KEY_DOMAIN_NAME] in TOOL_KEY_DOMAIN_LIST:
      tool_error(env, input_dict[TOOL_KEY_DOMAIN_NAME] + ' is not a valid ' + TOOL_KEY_DOMAIN_NAME + ' name. Get name from list = [modem, adsp, slpi, apps, cdsp] ')

   #read in sub-domain info
   if not input_dict.has_key(TOOL_KEY_SUBDOMAIN_NAME):
      tool_error(env, TOOL_KEY_SUBDOMAIN_NAME + ' required')

   if not input_dict[TOOL_KEY_SUBDOMAIN_NAME] in TOOL_KEY_SUBDOMAIN_LIST:
      tool_error(env, input_dict[TOOL_KEY_SUBDOMAIN_NAME] + ' is not a valid ' + TOOL_KEY_SUBDOMAIN_NAME + ' name. Get name from list = [modem_pd, audio_pd, sensor_pd, , wlan_pd, root_pd,test_pd ] ')

   # Process input on SCONS Command Options
   if env.GetOption('clean') or env.GetOption('no_exec'):
      tool_debug(env, 'TARGET CLEAN/NO_EXEC {:s}, {:s}, {:s} "{:s}" {:d}'.format(output_dict[TOOL_KEY_SOC_NAME], output_dict[TOOL_KEY_DOMAIN_NAME],output_dict[TOOL_KEY_SUBDOMAIN_NAME], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))
      return None # RETURN, IGNORED

   # Ignore input on specified BUILD_ID not being active
   if build_ids is not None and not env.IsTargetEnable(build_ids):
      tool_debug(env, 'TARGET NOT ACTIVE {:s}, {:s}, {:s} "{:s}" {:d}'.format(output_dict[TOOL_KEY_SOC_NAME],output_dict[TOOL_KEY_DOMAIN_NAME],output_dict[TOOL_KEY_SUBDOMAIN_NAME], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))
      return None # RETURN, IGNORED

   #write soc, domain and subdomain info to output_dict
   output_dict[TOOL_KEY_SOC_NAME] = input_dict[TOOL_KEY_SOC_NAME].lower()
   output_dict[TOOL_KEY_DOMAIN_NAME] = input_dict[TOOL_KEY_DOMAIN_NAME].lower()
   output_dict[TOOL_KEY_SUBDOMAIN_NAME] = input_dict[TOOL_KEY_SUBDOMAIN_NAME].lower()
   temp = tool_hash(output_dict[TOOL_KEY_SOC_NAME] + '/' + output_dict[TOOL_KEY_DOMAIN_NAME] + '/' + 'root_pd')
   hash = divmod(temp, 256)
   output_dict[TOOL_KEY_QMI_INSTANCE_ID] = hash[1]

   #add to environment descriptor
   env_tool[TOOL_COLLECTOR].append(output_dict)
   tool_debug(env, 'tool_method_add_function {:s}, {:s}, {:s} "{:s}" {:d}'.format(output_dict[TOOL_KEY_SOC_NAME], output_dict[TOOL_KEY_DOMAIN_NAME],output_dict[TOOL_KEY_SUBDOMAIN_NAME], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))

   return output_dict

#===============================================================================

def tool_method_add_service(env, build_ids, input_dict):
   ''' Method Call, Adds Function
   Validates input and stores to 'globals' construction environment
   Runs before tool_emitter() function '''

   # 'globals' information saved in the construction environment
   env_tool = env[TOOL_ENV] # construct env task/function dictionary

   # caller python frame of interest
   frame, filename, line_number, function_name, lines, index = inspect.getouterframes(inspect.currentframe())[2]

   # define the layout of the ordered dictionary, all fields in expected
   # ordering of the fields
   output_dict = OrderedDict()
   output_dict[TOOL_KEY_PROVIDER_NAME] = None
   output_dict[TOOL_KEY_SERVICE_NAME] = None
   output_dict[TOOL_KEY_SERVICE_DATA_VALID] = None
   output_dict[TOOL_KEY_SERVICE_DATA] = None

   output_text = '..../'+filename.replace('\\','/').lower().replace(env.get('BUILD_ROOT', None).lower()+'/', '')
   output_dict[TOOL_KEY_DEFINED] = { 'file': output_text, 'line': line_number }

   #read in provider info
   if not input_dict.has_key(TOOL_KEY_PROVIDER_NAME):
      tool_error(env, TOOL_KEY_PROVIDER_NAME + ' required')

   #read in service info
   if not input_dict.has_key(TOOL_KEY_SERVICE_NAME):
      tool_error(env, TOOL_KEY_SERVICE_NAME + ' required')

   # Process input on SCONS Command Options
   if env.GetOption('clean') or env.GetOption('no_exec'):
      tool_debug(env, 'TARGET CLEAN/NO_EXEC {:s}, {:s} "{:s}" {:d}'.format(output_dict[TOOL_KEY_PROVIDER_NAME], output_dict[TOOL_KEY_SERVICE_NAME], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))
      return None # RETURN, IGNORED

   # Ignore input on specified BUILD_ID not being active
   if build_ids is not None and not env.IsTargetEnable(build_ids):
      tool_debug(env, 'TARGET NOT ACTIVE {:s}, {:s} "{:s}" {:d}'.format(output_dict[TOOL_KEY_PROVIDER_NAME],output_dict[TOOL_KEY_SERVICE_NAME], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))
      return None # RETURN, IGNORED

   #write provider and service info to output_dict
   output_dict[TOOL_KEY_PROVIDER_NAME] = input_dict[TOOL_KEY_PROVIDER_NAME].lower()
   output_dict[TOOL_KEY_SERVICE_NAME] = input_dict[TOOL_KEY_SERVICE_NAME].lower()

   if input_dict.has_key(TOOL_KEY_SERVICE_DATA):
      output_dict[TOOL_KEY_SERVICE_DATA_VALID] = 1
      output_dict[TOOL_KEY_SERVICE_DATA] = input_dict[TOOL_KEY_SERVICE_DATA]
   else:
      output_dict[TOOL_KEY_SERVICE_DATA_VALID] = 0
      output_dict[TOOL_KEY_SERVICE_DATA] = 0

   #add to environment descriptor
   env_tool[TOOL_COLLECTOR].append(output_dict)
   tool_debug(env, 'tool_method_add_function {:s}, {:s}, {:d}, {:d}, "{:s}" {:d}'.format(output_dict[TOOL_KEY_PROVIDER_NAME], output_dict[TOOL_KEY_SERVICE_NAME], output_dict[TOOL_KEY_SERVICE_DATA_VALID], output_dict[TOOL_KEY_SERVICE_DATA], output_dict[TOOL_KEY_DEFINED]['file'], output_dict[TOOL_KEY_DEFINED]['line']))

   return output_dict

#===============================================================================
# Support Functions
#===============================================================================

def tool_error(env, Info):
   ''' FATAL Information, Stops Build '''
   env.PrintError('SERVICE REGISTRY ' + Info)
   raise Exception()


def tool_warning(env, Info):
   ''' Non-FATAL Information, Build Continues '''
   env.PrintWarning('SERVICE REGISTRY ' + Info)


def tool_info(env, Info):
   ''' Informative Information, Build Continues '''
   env.PrintInfo('SERVICE REGISTRY ' + Info)


def tool_debug(env, Info):
   ''' Diagnostic Information, Build Continues '''
   env.PrintDebugInfo('SERVICE REGISTRY', 'SERVICE REGISTRY ' + Info)


#def tool_hash(name):
#   ''' Tool Utility Function '''
#   tmp = 0
#   hash = 0
#   for i in range(len(name)):
#      hash = (hash << 4) + ord(name[i])
#      tmp = hash & 0xf0000000
#      if tmp != 0:
#         hash ^= (tmp >> 24)
#      hash &= ~tmp
 #  return hash

def tool_hash(name):
   ''' Tool Utility Function '''
   hash = 0
   for i in range(len(name)):
      hash = hash + ord(name[i])
   return hash

def tool_randstr(size):
   ''' Tool Utility Function '''
   import string
   import random
   allowed = string.ascii_uppercase # add any other allowed characters here
   tool_randstr = ''.join([allowed[random.randint(0, len(allowed) - 1)] for x in xrange(size)])
   return tool_randstr
