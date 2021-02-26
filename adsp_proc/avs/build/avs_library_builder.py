#===============================================================================
#
# AVS Library Builder
#
# GENERAL DESCRIPTION
#    This builder will create one of three library types based on USES flags
#    set in the AVS image scons script file.
#       1) A normal library with symbols (env.AddLibrary)
#       2) A stub library (env.AddLibrary)
#       3) A stripped library (obfuscated and symbols stripped)jj
#
# Copyright (c) 2009-2012 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Technologies Proprietary.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/avs.build/1.0/avs_library_builder.py#7 $
#  $DateTime: 2018/04/27 05:15:34 $
#  $Author: pwbldsvc $
#  $Change: 16037238 $
#                      EDIT HISTORY FOR FILE
#model
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/01/12   ps      Create Builder
#
#===============================================================================

import re
import os
import sys
import datetime
import shutil
import glob
import stat
#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Builder function definitions
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('avs_prop_library_builder')


def generate(env):
   # Generate function for Proprietary Library builder:  Sets up the
   # pseudo builder, action and builder function to be called by
   # clients to pass library details to the builder.

   env.Replace(USES_AVS_LIBRARY_BUILDER = True)

   # Add Obfuscate tool to construction environment
   if env['PLATFORM'] == 'linux':
      env.Replace(OBFUSCATE = '${AVS_FOLDER_ROOT}/build/sbin/linux64/hexagon-obfuscate')
      env.Replace(H2XML_EXE = '${AVS_FOLDER_ROOT}/build/h2xml/h2xml')
   else:
      env.Replace(OBFUSCATE ='${AVS_FOLDER_ROOT}/build/sbin/win32/hexagon-obfuscate.exe')
      env.Replace(H2XML_EXE = '${AVS_FOLDER_ROOT}/build/h2xml/h2xml.exe')

   # Get verbose option
   env.Replace(VERBOSE = int(env.GetOption('verbose')))

   # Add pseudo-builder name (wrapper) function
   env.AddMethod(add_avs_library, 'AddAvsLibrary')
   #Method for generating the dummy shared objects at scons read time
   env.AddMethod(avs_so_generate, 'AvsSoGenerator')
   #Method for shared library generations
   env.AddMethod(add_avs_shared_library, 'AddAvsSharedLibrary')
   #Method to get the list of files recursivley inside the folders.
   env.AddMethod(get_list_files, 'GetFilesList')
   env.AddMethod(h2xml_file_generate, 'H2xmlFileGenerate')
   env.AddMethod(autogen_elite, 'Autogenelite')

   # ----------------------------------
   # Setup the AVS proprietary builders
   # ----------------------------------


   # AVS Proprietary Symbol Builder
   avs_prop_symbol_act = env.GetBuilderAction(avs_prop_symbol_builder)
   avs_prop_symbol_bld = env.Builder(action = avs_prop_symbol_act)
   env.Append(BUILDERS = {'AvsPropSymbolBuilder' : avs_prop_symbol_bld})

   # AVS Proprietary Link Builder
   avs_prop_link_act = env.GetBuilderAction(avs_prop_link_builder)
   avs_prop_link_bld = env.Builder(action = avs_prop_link_act,
                                   emitter= avs_prop_link_emitter)
   env.Append(BUILDERS = {'AvsLinkBuilder' : avs_prop_link_bld})

   # AVS Proprietary Link Builder
   avs_prop_strip_link_act = env.GetBuilderAction(avs_prop_strip_link_builder)
   avs_prop_strip_link_bld = env.Builder(action = avs_prop_strip_link_act)
   env.Append(BUILDERS = {'AvsPropLinkBuilder' : avs_prop_strip_link_bld})

   # AVS so encryption builder
   avs_sectools_encrypt_link_act = env.GetBuilderAction(avs_sectools_encrypt_link_builder)
   avs_sectools_encrypt_link_bld = env.Builder(action = avs_sectools_encrypt_link_act,
                                               emitter= avs_sectools_encrypt_emitter)
   env.Append(BUILDERS = {'AvsSoEncryptBuilder' : avs_sectools_encrypt_link_bld})
   
   # AVS Proprietary Obfuscate Builder
   avs_prop_obfus_act = env.GetBuilderAction(avs_prop_obfus_builder)
   avs_prop_obfus_bld = env.Builder(action = avs_prop_obfus_act,
                                    emitter= avs_prop_obfus_emitter)
   env.Append(BUILDERS = {'AvsPropObBuilder' : avs_prop_obfus_bld})

   # AVS Proprietary Strip Builder
   avs_prop_strip_act = env.GetBuilderAction(avs_prop_strip_builder)
   avs_prop_strip_bld = env.Builder(action = avs_prop_strip_act)
   env.Append(BUILDERS = {'AvsPropStripBuilder' : avs_prop_strip_bld})


   return

def sign_encrypt_so(env,library_node,dict_value,library_path,build_tags,library,ext):
       
       
       if len(env["SHARED_LIB_DICT"][dict_value])> 3:
        if env["SHARED_LIB_DICT"][dict_value][3]:
            print "generating signed so for {}".format(str(library_node))
            #Always generate encrypted dynamic modules in this location
            shared_lib_install_dir_encrypt=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules_encrypted')

            #Needed env for run time builders should be added here
            env["sec_encrypt_lib_id"]=env["SHARED_LIB_DICT"][dict_value][3]["lib_id"]

            #As of now client_id is hardcoded, remove the client_id hardcoding when needed
            #env["sec_encrypt_client_id"]=env["SHARED_LIB_DICT"][dict_value][3]["client_id"]
            env["DYNL_ENCRYPT_PATH"]=shared_lib_install_dir_encrypt
            target=env["INC_ROOT"]+"/build/ms/"+os.path.split(library)[1]+ext

            #Make sure artifact is a dependency for so generation. 
            #target should have something else(dummy), it won't generate the so
            encrypt_so = env.AvsSoEncryptBuilder(target, library_node, env)
            env.AddArtifact(build_tags, [encrypt_so])
            
# When prop sources are empty:-
#if <libname>_strip_so in env then strip the shared library but not obfuscate it
#if <libname>_so in env then generate the shared library directly.
def prop_list_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip,uses_lib_strip_only,pass_stub_by_default=False):


      if build_type == 'LEAF':
         # Generate object files from regular source files
         reg_objs = env.AddObject(build_tags, reg_source_list)

         # Generate object files from stub source files
         stub_objs = env.AddObject(build_tags, stub_source_list)

      if build_type == 'MOD':
         reg_objs = list(reg_source_list)
         stub_objs = list(stub_source_list)

      #If any shared library is mentioned in the build config then corresponding
      #Static library is stubbed by default.
      if pass_stub_by_default == True:
         env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
         # Use 'ld' to partially link stub sources.
         link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
                    [stub_objs],build=build_type)
         library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)
         #Create additional static strip lib and do not link. Copied code from below under <if uses_lib_strip in env:>
         # Use 'ld' to partially link all proprietary objects and regular
         # objects.  This will resolve all local symbols and produce a
         # single relocatable file.
         link_reloc_strip = env.AvsPropLinkBuilder(library + '_link_strip.reloc',
            [reg_objs])


         # Use 'strip' to remove all non-global symbols from obfuscated
         # relocatable object
         prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', link_reloc_strip)

         # Create strip library file (e.g., Quartet_strip.lib) from the stripped
         # relocatable object.
         library_node_strip = env.AddLibrary(build_tags, library + "_strip", prop_strip, add_to_list=False)
         library_node_internal = env.AddLibrary(build_tags, library + "_internal", link_reloc_strip, add_to_list=False)
         #Mention dependency on internal as well as normal lib
         env.Depends(library_node, library_node_strip)
         env.Depends(library_node, library_node_internal)
      else:

          if uses_lib_strip in env:

             # Use 'ld' to partially link all proprietary objects and regular
             # objects.  This will resolve all local symbols and produce a
             # single relocatable file.
             link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc',
                [reg_objs])


             # Use 'strip' to remove all non-global symbols from obfuscated
             # relocatable object
             prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', link_reloc)

             # Create strip library file (e.g., Quartet_strip.lib) from the stripped
             # relocatable object.
             library_node = env.AddLibrary(build_tags, library + "_strip", prop_strip)
             library_node_internal = env.AddLibrary(build_tags, library + "_internal", link_reloc, add_to_list=False)
             #Mention dependency on internal as well as normal lib
             env.Depends(library_node, library_node_internal)


          elif uses_lib_stub in env:
              # Use 'ld' to partially link stub sources.
             env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
             link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
                    [stub_objs],build=build_type)
             library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)

          elif uses_lib_strip_only in env:

             # Use 'ld' to partially link all proprietary objects and regular
             # objects.  This will resolve all local symbols and produce a
             # single relocatable file.
             link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc',
                [reg_objs])


             # Use 'strip' to remove all non-global symbols from obfuscated
             # relocatable object
             prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', link_reloc)

             # Create strip library file (e.g., Quartet_strip.lib) from the stripped
             # relocatable object.
             library_node = env.AddLibrary(build_tags, library + "_strip_only", prop_strip)
             library_node_internal = env.AddLibrary(build_tags, library + "_internal", link_reloc, add_to_list=False)
             #Mention dependency on internal as well as normal lib
             env.Depends(library_node, library_node_internal)

          elif uses_lib_stub in env:
              # Use 'ld' to partially link stub sources.
             env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
             link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
                [stub_objs],build=build_type)

             library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)
             
          else:
              # Use 'ld' to partially link all proprietary objects and regular
              # objects.  This will resolve all local symbols and produce a
              # single relocatable file.
             env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
             link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
                 [reg_objs],build=build_type)

             library_node = env.AddLibrary(build_tags, library, link_reloc)

#When prop list is not empty obfuscate them
def prop_list_not_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip,uses_lib_strip_only,prop_source_list,library_path,pass_stub_by_default=False):

      if build_type == 'LEAF':
         # Generate object files from regular source files
         reg_objs = env.AddObject(build_tags, reg_source_list)

         # Generate object files from proprietary source files
         prop_objs = env.AddObject(build_tags, prop_source_list)

         # Generate object files from stub source files
         stub_objs = env.AddObject(build_tags, stub_source_list)

      if build_type == 'MOD':
         #import pdb; pdb.set_trace()
         reg_objs = list(reg_source_list)
         prop_objs = list(prop_source_list)
         stub_objs = list(stub_source_list)

      #If any shared library is mentioned in the build config then corresponding
      #Static library is stubbed by default.
      if pass_stub_by_default == True:
         # Use 'ld' to partially link stub sources.
         env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
         link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
            [stub_objs],build=build_type)
         library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)
         #### Create additional static strip lib and do not link. Copied code from below under <if uses_lib_strip in env:>
         # Use 'nm' (Name list) to produce list of proprietary symbols
         # from the proprietary object files (*.o)
         prop_sym_fn = library_path + '/proprietary_symbols.txt'
         prop_sym_file = env.AvsPropSymbolBuilder(prop_sym_fn, prop_objs)

         # Put symbol map filename into environment so it can be cleaned
         # later.  SCons doesn't know about the map file, so we have to
         # explicitly clean with an env.Clean() call.
         env.Replace(PROP_MAP_FN = library_path + '/proprietary_symbols.map')

         #Use 'ld' to partially link all proprietary objects and regular
         # objects.  This will resolve all local symbols and produce a
         # single relocatable file.
         link_reloc_strip = env.AvsPropLinkBuilder(library + '_link_strip.reloc',
            [prop_objs, reg_objs])

         # Use 'Obfuscate' program (Qualcomm) to encrypt the proprietary
         # symbols in the relocatable object
         prop_obfus = env.AvsPropObBuilder(library + '_obfuscate.reloc',
            [prop_sym_file, link_reloc_strip])

         # Use 'strip' to remove all non-global symbols from obfuscated
         # relocatable object
         prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', prop_obfus)

         # Create strip library file (e.g., Quartet_strip.lib) from the stripped
         # relocatable object.
         library_node_strip = env.AddLibrary(build_tags, library + "_strip", prop_strip, add_to_list=False)
         library_node_internal = env.AddLibrary(build_tags, library + "_internal", link_reloc_strip, add_to_list=False)
         #Mention dependency on internal as well as normal lib
         env.Depends(library_node, library_node_strip)
         env.Depends(library_node, library_node_internal)
      else:
          if uses_lib_strip in env:

             # Use 'nm' (Name list) to produce list of proprietary symbols
             # from the proprietary object files (*.o)
             prop_sym_fn = library_path + '/proprietary_symbols.txt'
             prop_sym_file = env.AvsPropSymbolBuilder(prop_sym_fn, prop_objs)

             # Put symbol map filename into environment so it can be cleaned
             # later.  SCons doesn't know about the map file, so we have to
             # explicitly clean with an env.Clean() call.
             env.Replace(PROP_MAP_FN = library_path + '/proprietary_symbols.map')

             #Use 'ld' to partially link all proprietary objects and regular
             # objects.  This will resolve all local symbols and produce a
             # single relocatable file.
             link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc',
                [prop_objs, reg_objs])

             # Use 'Obfuscate' program (Qualcomm) to encrypt the proprietary
             # symbols in the relocatable object
             prop_obfus = env.AvsPropObBuilder(library + '_obfuscate.reloc',
                [prop_sym_file, link_reloc])

             # Use 'strip' to remove all non-global symbols from obfuscated
             # relocatable object
             prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', prop_obfus)

             # Create strip library file (e.g., Quartet_strip.lib) from the stripped
             # relocatable object.
             library_node = env.AddLibrary(build_tags, library + "_strip", prop_strip)
             library_node_internal = env.AddLibrary(build_tags, library + "_internal", link_reloc, add_to_list=False)
             #Mention dependency on internal as well as normal lib
             env.Depends(library_node, library_node_internal)

          elif uses_lib_strip_only in env:

             # Use 'ld' to partially link all proprietary objects and regular
             # objects.  This will resolve all local symbols and produce a
             # single relocatable file.
             link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc',
                [prop_objs, reg_objs])


             # Use 'strip' to remove all non-global symbols from obfuscated
             # relocatable object
             prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', link_reloc)

             # Create strip library file (e.g., Quartet_strip.lib) from the stripped
             # relocatable object.
             library_node = env.AddLibrary(build_tags, library + "_strip_only", prop_strip)
             library_node_internal = env.AddLibrary(build_tags, library + "_internal", link_reloc, add_to_list=False)
             #Mention dependency on internal as well as normal lib
             env.Depends(library_node, library_node_internal)

          elif uses_lib_stub in env:
              # Use 'ld' to partially link stub sources.
             env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
             link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
                [stub_objs],build=build_type)

             library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)

          else:
              # Use 'ld' to partially link all proprietary objects and regular
              # objects.  This will resolve all local symbols and produce a
              # single relocatable file.
             env.Replace(PROP_MAP_FN_TXT = library + '_link.reloc_temp.txt')
             link_reloc = env.AvsLinkBuilder(library + '_link.reloc',
                [prop_objs, reg_objs],build=build_type)

             library_node = env.AddLibrary(build_tags, library, link_reloc)

#Functiong for getting the list of files recursively.
def get_list_files(env,path,filter=[]):
    if filter != []:
       images = filter
    else:
       images=['.cpp','.c','.S']
    matches = []

    env_subst = env.subst
    root_path=env_subst(path)
    for root, dirs, files in os.walk(root_path):
        for file in files:
            for filter in images:
               if file.endswith(filter):
                   matches.append(os.path.join(root, file))
    #print matches
    return matches

#Function for generation of shared libraries.
def add_avs_shared_library(env,build_tags,library,sources,build_type='LEAF',):
    #import pdb;pdb.set_trace()
    #Always generate the "auto_generated" folder in parallel to build folder.
    #Go back one directory always to generate the auto_Generated folder.
    path='../auto_generated'
    library_path, library_fn = os.path.split(library) #Get the library_path and library name

    # Get library name and create USES_<library>_<type> strings
    uses_lib_strip_so  = ''.join(['USES_', library_fn.upper(), '_STRIP_SO'])
    uses_lib_so=''.join(['USES_', library_fn.upper(), '_SO'])

    #Before we start to parse the shared libraries dictionary, validate all the pre-requistes
    #Check1: Check if both strip_so and _so is mentioned for the same library, if so bail out
    if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True and env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:
       print "Error: Shared library is mentioned both for strip and stub in build config"

    #Shared library with _strip_so or _so are only taken in to consideration
    if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True or env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:


       if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True:
         dict_value=uses_lib_strip_so
       if env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:
         dict_value=uses_lib_so
       #Parse the dict to get versions info
       major_version=env['SHARED_LIB_DICT'][dict_value][0][0]
       minor_version=env['SHARED_LIB_DICT'][dict_value][0][1]
       #Get is_qurt version
       is_qurt_used=env['SHARED_LIB_DICT'][dict_value][1][0]

       #This will re-create the header files for every time we compile.
       shared_lib_api= env.subst('${AVS_ROOT}')
       shared_lib_cpp=os.path.join(shared_lib_api,'elite','module_interfaces','shared_lib_api','src','shared_lib_api.cpp')
       default_loc_for_dsp_version=os.path.join(shared_lib_api,'elite','module_interfaces','shared_lib_api','build')

       #So and dsp version file names.
       so_version='so_version.h'
       dsp_version='dsp_version.h'

       #so_version family names.
       header_path=os.getcwd()+'/'+path
       so_full_path=header_path+'/'+so_version


       #Full path for the shared_lib_api.cpp
       shared_lib_api_cpp= os.path.join(header_path,'shared_lib_api.cpp')


       HEXAGON_TOOL_VERSION= env.subst('$HEXAGON_RTOS_RELEASE')
       #Get the tools version and format it
       #Only keep alpha numeric and .
       HEXAGON_TOOL_VERSION2 = re.sub("[^0-9a-fA-f.]", "", HEXAGON_TOOL_VERSION)
       tool_vers_split=HEXAGON_TOOL_VERSION2.split('.')

       #tool_vers_split=HEXAGON_TOOL_VERSION.split('.')
       if len(tool_vers_split)==4:
          format_hex_tool_version=""
       else:
          format_hex_tool_version="00"
       for each_tool in tool_vers_split:
          if len(each_tool) == 1:
              each_tool="%02d" % int(each_tool)
          if len(each_tool) > 2:
              each_tool=each_tool[:1]+each_tool[-1:] # if there are more than 2 characters, use the first and last

          format_hex_tool_version+=each_tool
       if len(tool_vers_split)==1:
          format_hex_tool_version+="0000"
       elif len(tool_vers_split)==2:
          format_hex_tool_version+="00"

       #Template for the so_version.h
       #Any changes to so_version.h should go in here.
       write_major_version='#define SHARED_LIB_API__LIB_MAJOR_VERSION %s' % major_version
       write_minor_version='#define SHARED_LIB_API__LIB_MINOR_VERSION %s' % minor_version
       write_is_qurt='#define SHARED_LIB_API__IS_QURT_USED_IN_LIB %s' % is_qurt_used   #Grab from the env
       write_hex_tool_version = "#define SHARED_LIB_API__HEXAGON_TOOLS_VERSION 0x%s" % format_hex_tool_version;
       #write_buildid = "#define SHARED_LIB_API__SHARED_LIB_BUILD_ID "+"\"LIB_BUILD_ID_"+str(major_version)+"."+str(minor_version)+"_"+datetime.datetime.now().strftime("%a_%b_%d_%H:%M:%S_PST_%Y")+"\""
       write_buildid_eng = "#define ENGG_TIMESTAMP \"Q6_BUILD_TS_"+str(env.subst('${ENGG_TIME_STAMP}'))+"_"+str(library_fn)+"\""
       write_buildid_qcom = "#define QCOM_TIMESTAMP \"Q6_BUILD_TS_"+str(env.subst('${QCOM_TIME_STAMP}'))+"_"+str(major_version)+"."+str(minor_version)+"_"+str(library_fn)+"\""
       write_so_version='\n'.join([write_major_version,write_minor_version,write_is_qurt,write_hex_tool_version,write_buildid_eng,write_buildid_qcom,'\n'])
       str_timestamp = "#define SHARED_LIB_API__HEXAGON_TOOLS_VERSION 0x%s" % format_hex_tool_version;


       #Generate auto_generated folder, if doesnot exist
       if not os.path.exists(path):
          os.makedirs(path)


       if path != False:


           if os.path.exists(header_path):

              #Adding a method for the clean
              if env.GetOption('clean'):
                 #Add all the files to the clean list
                 list_files=[so_full_path]
                 for file in list_files:
                     if os.path.exists(file):
                        os.remove(file)

              #Generate so_version.h and  copy the shared_lib_api.cpp to autogen folder
              #Copying of shared_lib_api.cpp is neeeded as to include this .cpp file in every shared library being compiled
              if not env.GetOption('clean'):
                 gen_so_version =open(env.RealPath(so_full_path), 'w')
                 gen_so_version.write(write_so_version)
                 #copy the file to auto_generated file as this should be part of the every so
                 if os.path.exists(shared_lib_api_cpp):
                    os.chmod(shared_lib_api_cpp, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)
                    os.remove(shared_lib_api_cpp)
                 shutil.copy(shared_lib_cpp,shared_lib_api_cpp)
                 os.chmod(shared_lib_api_cpp, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

           else:
              print "Specified path does not exist %s" % header_path #Error check

       #Any changes to dsp_version.h autogeneration should go in here...
       dsp_ver=True #If there are any changes in mere future use this flag
       if dsp_ver != False:
         dsp_auto_gen_folder=os.path.join(default_loc_for_dsp_version,path)
         dsp_auto_header_path=os.path.join(dsp_auto_gen_folder,dsp_version)
         if os.path.exists(default_loc_for_dsp_version):
           if not os.path.exists(dsp_auto_gen_folder):
               os.makedirs(dsp_auto_gen_folder)
           #Adding a method for the clean
           #TODO: Emitter type of builder is needed, this will address the neeeds as of now.
           if env.GetOption('clean'):
              if os.path.exists(dsp_auto_header_path):
                 os.remove(dsp_auto_header_path)

          #Generate dsp_version.h with the following content

           if not env.GetOption('clean'):
              gen_dsp_version =open(env.RealPath(dsp_auto_header_path), 'w')
              gen_dsp_version.write(str_timestamp)

    reg_source_list = sources[0]
    stub_source_list = sources[1]
    prop_source_list = sources[2]
    library_node = [] #Make sure this is empty at the start for every shared library

    #By default for every shared library being compiled, shared_lib_api.cpp should be part of it
    sources_default = env.GlobSourceFiles(['auto_generated/*.cpp'], '..',posix=True)

	#Depends libs are always to be linked from this location,as all the dependent libraries are generated here
    so_lib_path=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','autogenerated')

    if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True or env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:

	   #useful for debugging when issues in the dict parser.
       '''
       get_dict=env['SHARED_LIB_DICT']
       for key,value in get_dict.iteritems():
            print key
            print len(value)
            if len(value) >2:
               if value[2] != []:
                  for lib in value[2]:
                     dep_so_list.append(lib)

       '''

    depends_libs_with_Versions=[] #Making sure that no depend libs are added before we fill this

    #Fill in uses_lib_strip_sl( used for getting the strip sharedlibrary)
    uses_lib_strip_sl=env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so)

    #Fill in uses_lib_strip_sl( used for getting the normal sharedlibrary)
    uses_lib_sl=env['SHARED_LIB_DICT'].has_key(uses_lib_so)

    if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True or env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:
       #Get the value or libname for _strip_so case
       if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True:
         dict_value=uses_lib_strip_so
         if len(env['SHARED_LIB_DICT'][dict_value]) > 2:
            print "Marking dependencies for %s" % env['SHARED_LIB_DICT'][dict_value]
            #Fill the dependencies needed for the strip shared library
            depends_libs_with_Versions=env['SHARED_LIB_DICT'][dict_value][2]
       #Get the value or libname for _so case
       if env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:
         dict_value=uses_lib_so
         if len(env['SHARED_LIB_DICT'][dict_value]) > 2:
            print "Marking dependencies for normal lib %s" % env['SHARED_LIB_DICT'][dict_value]
            #Fill the dependencies needed for the strip shared library
            depends_libs_with_Versions=env['SHARED_LIB_DICT'][dict_value][2]

       #If proprietary sources are empty(Donot obfuscate but strip for "_strip_so" case)
       #Check with team for this as concept is cloned from static libs
       
       if prop_source_list ==[]:
          prop_list_empty_so(env,build_tags,build_type,reg_source_list,stub_source_list,library,uses_lib_strip_sl,sources_default,library_path,uses_lib_sl,so_lib_path,dict_value,depends_libs_with_Versions)
       else: #For proprietary sources not empty case(Obfuscate and then strip for _Strip_so case)
          prop_list_not_empty_so(env,build_tags,build_type,reg_source_list,stub_source_list,prop_source_list,library,uses_lib_strip_sl,sources_default,library_path,uses_lib_sl,so_lib_path,dict_value,depends_libs_with_Versions)

	#Cleaning of autogenerated files are only to be done once the builder, is able to
	#identify the auto generated cpp file. Else it will not clean the generated binaries.
    #If we have the emitter builder in place this has to be marked as dependency.
    if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True or env['SHARED_LIB_DICT'].has_key(uses_lib_so) == True:
       if env.GetOption('clean'):
          if os.path.exists(shared_lib_api_cpp):
             os.remove(shared_lib_api_cpp)

    return library_node #Must return the library node else the image script will not mark in the scons memory to compile
    #it should go here.
def prop_list_empty_so(env,build_tags,build_type,reg_source_list,stub_source_list,library,uses_lib_strip_sl,sources_default,library_path,uses_lib_sl,so_lib_path,dict_value,depends):
    library_path, library_fn = os.path.split(library) #Get the library_path and library name
    #By default every shared library must get compiled with "AVS_BUILD_SOS" macro
    env.Append(CFLAGS = '-DAVS_BUILD_SOS')

    #For the leaf level libs generate the objects
    if build_type == 'LEAF':
       reg_objs = env.SharedObject(reg_source_list+sources_default)
       stub_objs = env.SharedObject(stub_source_list)

    #If it is module level then collect the libs donot compile again
    #As the compiled libs/objs are passed from the leaf level
    if build_type == 'MOD':
       #Make sure we add the sources_defualt list after compiling
       #Must use scons default builder for building and not QCT scons shared object builder
	   #If we use QCT scons default method then change is needed in image scripts.

       reg_objs = list(reg_source_list+env.SharedObject(sources_default))
       stub_objs = list(stub_source_list)

    if uses_lib_strip_sl == True:
       # Use 'ld' to partially link all proprietary objects and regular
       # objects.  This will resolve all local symbols and produce a
       # single relocatable file.

       link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc',
             [reg_objs])


       # Use 'strip' to remove all non-global symbols from partially linked regular objects
       prop_strip = env.AvsPropStripBuilder(library + '_strip_so.reloc', link_reloc)

	   #Every shared library should be generated with <.so><.majorversion><.minorversion>
       #EXTENSION_CHANGE=".so"+'.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])+'.'+str(env['SHARED_LIB_DICT'][dict_value][0][1])
       EXTENSION_CHANGE=".so"+'.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       EMBED_IN_SO="-soname"+" "+library_fn+'.so.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       #By default every shared library uses ".so" as extension.
       #Replacing "SHLIBSUFFIX" will overwrite the default ".so" extension as per our needs.
       env.Replace(SHLIBSUFFIX = EXTENSION_CHANGE)
       env.Replace(EMBED_IN_SO_ENV = EMBED_IN_SO)

       #This will make sure to generate the all the static libs in the "avs/build/dynamic_modules"
       #Controlled from here as if the user forgets to mention this path in the leaf/mod level scons
       #then the shared library will not get generated in install_dir & hence we control from here.
       shared_lib_install_dir=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules',library)
       shared_lib_install_dir_unstrip=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules',library+'_unstrip')

       if 'HAP_AVS_ADSP' in build_tags :
         shared_lib_install_dir=os.path.join(env.get('OEM_ROOT'),'build','dynamic_modules_hash',library_fn)
         shared_lib_install_dir_unstrip=os.path.join(env.get('OEM_ROOT'),'build','dynamic_modules_hash',library_fn+'_unstrip')
       #Get all the flags under our environment control, such that we can define our own rules for the shared libraries.
       #If capi concept is in place, we assume it is for "module" level linking
       #special way of linking for mod level linking by marking all the dependecies.
       #SHLINKFLAGS= Used to control LINK FLAGS.
       #SHLINKCOM=Used to control the shared library compiler.

       if build_type=='MOD':
           #Pass "-shared" option while linking the sharedlibraries.
           env.Replace(SHLINKFLAGS = "-m${Q6VERSION}  -Bdynamic -nostdlib -shared --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=memalign $SHLINK_FLAG ")
           if depends:
                #Get the dependent libs and add to linking
                depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
                DEPEND_LIBS="-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs
                env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)

                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
           else:
                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $EMBED_IN_SO_ENV')}")
       else: #For the leaf level linking(if there is no capi conecpt)
           EMBED_IN_SO="-soname"+" "+library_fn+'.so.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
           env.Replace(EMBED_IN_SO_ENV = EMBED_IN_SO)
           if depends:
                depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
                DEPEND_LIBS="-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs
                env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)
                env.Replace(SHLINKFLAGS = "-m${Q6VERSION}  -Bdynamic -nostdlib -shared --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=memalign $SHLINK_FLAG ")
                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS $SOURCES.posix $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
           else:
                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS $SOURCES.posix $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
       library_node=env.AddSharedLibrary(build_tags,shared_lib_install_dir, prop_strip)
       library_node_unstrip=env.AddSharedLibrary(build_tags,shared_lib_install_dir_unstrip, link_reloc, add_to_list=False)
       env.Depends(library_node, library_node_unstrip)

    #For normal shared library without any stripping
    if uses_lib_sl == True:

       #This will make sure to generate the all the static libs in the "avs/build/dynamic_modules"
       #Controlled from here as if the user forgets to mention this path in the leaf/mod level scons
       #then the shared library will not get generated in install_dir & hence we control from here.
       shared_lib_install_dir=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules',library)

       if 'HAP_AVS_ADSP' in build_tags :
          shared_lib_install_dir=os.path.join(env.get('OEM_ROOT'),'build','dynamic_modules_hash',library_fn)
       #Every shared library should be generated with <.so><.majorversion><.minorversion>
       EXTENSION_CHANGE=".so"+'.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       #Replacing "SHLIBSUFFIX" will overwrite the default ".so" extension as per our needs.
       env.Replace(SHLIBSUFFIX = EXTENSION_CHANGE)
       EMBED_IN_SO="-soname"+" "+library_fn+'.so.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       env.Replace(EMBED_IN_SO_ENV = EMBED_IN_SO)
       if build_type == 'MOD':
          env.Replace(SHLINKFLAGS = "-m${Q6VERSION}  -Bdynamic -nostdlib -shared --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=memalign $SHLINK_FLAG ")
          if depends:
             #import pdb; pdb.set_trace()
             depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
             DEPEND_LIBS="-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs
             env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)
             env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
          else:
             env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $EMBED_IN_SO_ENV')}")
       else:
          if depends:

             env.Replace(SHLINKFLAGS = "-m${Q6VERSION}  -Bdynamic -nostdlib -shared --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=memalign $SHLINK_FLAG ")
             depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
             env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)
             env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS $SOURCES.posix $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")

       library_node=env.AddSharedLibrary(build_tags,shared_lib_install_dir, [reg_objs])
    #Signed so encryption takes place only if security_info is added in json
    sign_encrypt_so(env,library_node,dict_value,library_path,build_tags,library,EXTENSION_CHANGE)

def prop_list_not_empty_so(env,build_tags,build_type,reg_source_list,stub_source_list,prop_source_list,library,uses_lib_strip_sl,sources_default,library_path,uses_lib_sl,so_lib_path,dict_value,depends):
    library_path, library_fn = os.path.split(library) #Get the library_path and library name
    #By default every shared library must get compiled with "AVS_BUILD_SOS" macro
    env.Append(CFLAGS = '-DAVS_BUILD_SOS')

    #For the leaf level libs generate the objects
    if build_type == 'LEAF':
       reg_objs = env.SharedObject(reg_source_list+sources_default)
       stub_objs = env.SharedObject(stub_source_list)
       prop_objs = env.SharedObject(prop_source_list)

    #If it is module level then collect the libs donot compile again
    #As the compiled libs/objs are passed from the leaf level
    if build_type == 'MOD':
       reg_objs = list(reg_source_list+env.SharedObject(sources_default))
       stub_objs = list(stub_source_list)
       prop_objs = list(prop_source_list)

    if uses_lib_strip_sl == True:

       # Use 'nm' (Name list) to produce list of proprietary symbols
       # from the proprietary object files (*.os)
       prop_sym_fn = [library_path + '/proprietary_symbols_so.txt']
       prop_sym_file = env.AvsPropSymbolBuilder(prop_sym_fn, prop_objs)
       env.Replace(PROP_MAP_FN = library_path + '/proprietary_symbols_so.map')

       # Use 'ld' to partially link all proprietary objects and regular
       # objects.  This will resolve all local symbols and produce a
       # single relocatable file.
       link_reloc = env.AvsPropLinkBuilder(library + '_link_so.reloc',
                [prop_objs,reg_objs])

       #Run obfuscation on the sharedobjects
       prop_obfus = env.AvsPropObBuilder(library + '_obfuscate_so.reloc',
                [prop_sym_file, link_reloc])

       #Strip the local symbols
       #Make sure to get the unique output name as not to collide with regular static libs.
       prop_strip = env.AvsPropStripBuilder(library + '_strip_so.reloc', prop_obfus)

       #Every shared library should be generated with <.so><.majorversion><.minorversion>
       EXTENSION_CHANGE=".so"+'.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       EMBED_IN_SO="-soname"+" "+library_fn+'.so.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       env.Replace(EMBED_IN_SO_ENV = EMBED_IN_SO)
       #Extension change as needed for Linux.
       env.Replace(SHLIBSUFFIX = EXTENSION_CHANGE)

       #This will make sure to generate the all the static libs in the "avs/build/dynamic_modules"
       #Controlled from here as if the user forgets to mention this path in the leaf/mod level scons
       #then the shared library will not get generated in install_dir & hence we control from here.
       shared_lib_install_dir=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules',library)
       shared_lib_install_dir_unstrip=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules',library+'_unstrip')

       if 'HAP_AVS_ADSP' in build_tags :
         shared_lib_install_dir=os.path.join(env.get('OEM_ROOT'),'build','dynamic_modules_hash',library_fn)
         shared_lib_install_dir_unstrip=os.path.join(env.get('OEM_ROOT'),'build','dynamic_modules_hash',library_fn+'_unstrip')
               

       if build_type=='MOD':
           env.Replace(SHLINKFLAGS = "-m${Q6VERSION} -Bdynamic -nostdlib -shared --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=memalign $SHLINK_FLAG ")
           if  depends:
                depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
                print "check for the libs here %s" %depends_libs
                env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)
                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
           else:
                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $EMBED_IN_SO_ENV')}")
       else:
           if  depends:
                depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
                env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)
                env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS $SOURCES.posix $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
       library_node=env.AddSharedLibrary(build_tags,shared_lib_install_dir, prop_strip)
       library_node_unstrip=env.AddSharedLibrary(build_tags,shared_lib_install_dir_unstrip, link_reloc, add_to_list=False)
       env.Depends(library_node, library_node_unstrip)


    if uses_lib_sl == True:

       #This will make sure to generate the all the static libs in the "avs/build/dynamic_modules"
       #Controlled from here as if the user forgets to mention this path in the leaf/mod level scons
       #then the shared library will not get generated in install_dir & hence we control from here.
       shared_lib_install_dir=os.path.join(env.subst('${AVS_FOLDER_ROOT}'),'build','dynamic_modules',library)

       if 'HAP_AVS_ADSP' in build_tags :
          shared_lib_install_dir=os.path.join(env.get('OEM_ROOT'),'build','dynamic_modules_hash',library_fn)
               
       #Every shared library should be generated with <.so><.majorversion><.minorversion>
       EXTENSION_CHANGE=".so"+'.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       EMBED_IN_SO="-soname"+" "+library_fn+'.so.'+str(env['SHARED_LIB_DICT'][dict_value][0][0])
       env.Replace(EMBED_IN_SO_ENV = EMBED_IN_SO)
       #Replacing "SHLIBSUFFIX" will overwrite the default ".so" extension as per our needs.
       env.Replace(SHLIBSUFFIX = EXTENSION_CHANGE)

       if build_type == 'MOD':
          env.Replace(SHLINKFLAGS = "-m${Q6VERSION}  -nostdlib -shared --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=memalign $SHLINK_FLAG ")

          if depends:

             env.Replace(SO_LINK_PATH = so_lib_path)
             depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
             env.Replace(DEPENDS_LIBS=depends_libs)
             env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS -Bdynamic -L$SO_LINK_PATH $DEPENDS_LIBS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $EMBED_IN_SO_ENV')}")
          else:
             env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS --whole-archive $SOURCES.posix --no-whole-archive $HEXAGON_OUTPUT_CMD $TARGET.posix $EMBED_IN_SO_ENV')}")

       else:
          if depends:
             depends_libs=''.join(['-l'+''.join([str(lib),' ']) for lib in depends])
             env.Replace(DEPEND_LIB= "-L${AVS_FOLDER_ROOT}/build/autogenerated %s" %depends_libs)
             env.Replace(SHLINKCOM = "${TEMPFILE('$SHLINK $SHLINKFLAGS $SOURCES.posix $HEXAGON_OUTPUT_CMD $TARGET.posix $DEPEND_LIB $EMBED_IN_SO_ENV')}")
       library_node=env.AddSharedLibrary(build_tags,shared_lib_install_dir, [prop_objs, reg_objs])

    
    #Signed so encryption takes place only if security_info is added in json
    sign_encrypt_so(env,library_node,dict_value,library_path,build_tags,library,EXTENSION_CHANGE)
   

	
#dsp_version.h file should get autogenerated even for the static libraries too
#calling this method will generated dsp_version.h file.
def auto_dsp_gen(env):

    path='../auto_generated'
    dsp_version='dsp_version.h'
    shared_lib_api= env.subst('${AVS_ROOT}')
    dsp_version_full_path=os.path.join(os.getcwd(),path,dsp_version)
    default_loc_for_dsp_version=os.path.join(shared_lib_api,'elite','module_interfaces','shared_lib_api','build')
    dsp_auto_gen_folder=os.path.join(default_loc_for_dsp_version,path)
    dsp_auto_header_path=os.path.join(dsp_auto_gen_folder,dsp_version)
    HEXAGON_TOOL_VERSION= env.subst('$HEXAGON_RTOS_RELEASE')
    #Get the tools version and format it
    #Only keep alpha numeric and .
    HEXAGON_TOOL_VERSION2 = re.sub("[^0-9a-fA-f.]", "", HEXAGON_TOOL_VERSION)
    tool_vers_split=HEXAGON_TOOL_VERSION2.split('.')
    #tool_vers_split=HEXAGON_TOOL_VERSION.split('.')
	
    if len(tool_vers_split)==4:
       format_hex_tool_version=""
    else:
       format_hex_tool_version="00"
    for each_tool in tool_vers_split:
       if len(each_tool) == 1:
           each_tool="%02d" % int(each_tool)
       if len(each_tool) > 2:
	   each_tool=each_tool[:1]+each_tool[-1:] # if there are more than 2 characters, use the first and last
		   
       format_hex_tool_version+=each_tool
    if len(tool_vers_split)==1:
       format_hex_tool_version+="0000"
    elif len(tool_vers_split)==2:
       format_hex_tool_version+="00"
    str_timestamp = "#define SHARED_LIB_API__HEXAGON_TOOLS_VERSION 0x%s" % format_hex_tool_version;

    if os.path.exists(default_loc_for_dsp_version):
      if not os.path.exists(dsp_auto_gen_folder):
         os.makedirs(dsp_auto_gen_folder)
         #Adding a method for the clean
         #TODO: EMitter type of builder is needed, this will address the neeeds as of now.
      if env.GetOption('clean'):
        if os.path.exists(dsp_auto_header_path):
           os.remove(dsp_auto_header_path)
      if not env.GetOption('clean'):
        if os.path.exists(dsp_auto_gen_folder):
            gen_dsp_version =open(env.RealPath(dsp_auto_header_path), 'w')
            gen_dsp_version.write(str_timestamp)






def add_avs_library(env, build_tags, library, sources,build_type='LEAF', PACK_SRC=False):
   # Input parameters:
   #   'build_tags' and 'library' are the same as add_library
   #   'sources' is a list of three (3) source file lists:
   #      sources[0] --> list of regular source files
   #      sources[1] --> list of stub source files
   #      sources[2] --> list of proprietary source files
   #   'PACK_SRC' decides whether a modules needs to be shipped as source or binary. (Defaulted to ship only Binary)
   #
   # This function checks USES flags as follows and builds the correct libraries:
   #
   #    USES_<Library>_STRIP  --
   #       - Compile and build library using 'strip' and regular source lists
   #       - Use NM to create a list of proprietary symbols
   #       - Use 'ld' to partially link all proprietary objects and regular objects
   #       - Use 'Obfuscate' program to encrypt the proprietary symbols in the
   #         relocatable object
   #       - Use 'strip' to remove all non-global symbols from the obfuscated
   #         relocatable object
   #       - Create strip library file (e.g., Quartet_strip.lib) from the stripped
   #         relocatable object.
   #
   #    USES_<Library>_STUB --
   #       - Compile and build library using 'stub' source list
   #
   #    No USES Flags set -- (default)
   #       - Compile and build library using 'regular' source list

   library_node = []



   # Unpack regular, stub and propriety source lists from 'sources'
   reg_source_list = sources[0]
   stub_source_list = sources[1]
   prop_source_list = sources[2]

   # Get library name and create USES_<library>_<type> strings
   library_path, library_fn = os.path.split(library)
   uses_lib_stub  = ''.join(['USES_', library_fn.upper(), '_STUB'])
   uses_lib_strip = ''.join(['USES_', library_fn.upper(), '_STRIP'])
   uses_lib_strip_only = ''.join(['USES_', library_fn.upper(), '_STRIP_ONLY'])

   #Need to generate dsp_version.h for static lib generation
   #Needs only for elite folder.
   #Based on the lib name it will identify this
   if library_fn == 'SharedLibApi':
      auto_dsp_gen(env) #Method to generate dsp_version.h with its valid params
      #By the time dsp_version is generated the scons include paths are published first
      #If the published paths are not physically present then that include path is not published
      #Jumping this, after generating the auto gen folder, publish the path from the builder.
      #This path is only affective for "SharedLibApi" lib alone and will not affect others
      env.PublishPrivateApi('AVS',[
                     '../auto_generated',
      ])

   #Stub the static lib, if the shared library for that lib is mentioned in the build config
   library_path, library_fn = os.path.split(library)
   uses_lib_strip_so  = ''.join(['USES_', library_fn.upper(), '_STRIP_SO'])
   uses_lib_normal_so = ''.join(['USES_', library_fn.upper(), '_SO'])

   #'GEN_SHARED_LIBS' tell me if need to build for shared libraries.
   #Wrap the default stubbing of static libs while compiling for shared libraries under 'GEN_SHARED_LIBS'
   #If the user donot want to build for shared libraries and wants only static libs then it should not affect normal behaviour
   #Wrapping under this flag will not affect the normal behaviour
   #If building for shared libraries then only enable this feature.
   pass_stub_by_default=False
   if 'GEN_SHARED_LIBS' in env:

      if env['SHARED_LIB_DICT'].has_key(uses_lib_strip_so) == True or env['SHARED_LIB_DICT'].has_key(uses_lib_normal_so) == True:
         #import pdb;pdb.set_trace();
         print library_fn
         pass_stub_by_default = True
      else:
         pass_stub_by_default=False

   #Scan the build config if the same lib name is mentioned for two LIB opetations.
   count=0
   if uses_lib_stub in env:
      count=count+1
   if uses_lib_strip in env:
      count=count+1
   if uses_lib_strip_only in env:
      count=count+1

   #Mark the lib as failure, if the count is > 1.
   if count > 1:
      print "Sending error status to environment and fail the build"
      print "ERROR::LIB--%s is mentioned twice for two lib operations in build_config...!!!" % library_fn.upper()
      sys.exit(-1)
   if not env.IsTargetEnable(build_tags):
      return library_node

   # if (in pack builds) and (PACK_SRC not set) use AddBinaryLibrary
   #    i.e delete 'sources' during pack creation and link library directly during pack build.
   if not env.IsBuildInternal() and (PACK_SRC == False):
      if uses_lib_strip in env:
         library += "_strip"
      elif uses_lib_strip_only in env:
         library += "_strip_only"
      elif uses_lib_stub in env or pass_stub_by_default == True:
         library += "_stub"
      env.AddBinaryLibrary(build_tags, library, sources)
      CLEAN_LIST = env.FindFiles(['*.reloc', '*_internal.lib', 'proprietary_symbols*'])
      if(build_type == 'MOD'):
         libs_to_remove = env.Flatten(sources)
         for each_lib in libs_to_remove:
            lib_to_del = env.RealPath(str(each_lib))
            if '_cleanpack.lib' in lib_to_del:
               lib_to_del_path, lib_name = os.path.split(lib_to_del)
               CLEAN_LIST.extend(env.FindFiles('*.lib', lib_to_del_path))
         pwd = os.getcwd()+'/..'
         list = os.listdir(pwd)
         list.remove('build')
         for leaf_libs in list:
            CLEAN_LIST.extend(env.FindFiles('*.scons', pwd+'/'+leaf_libs))
      env.CleanPack(build_tags, CLEAN_LIST)

   else:
      if prop_source_list == []:
         prop_list_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip,uses_lib_strip_only,pass_stub_by_default)
      else:
         prop_list_not_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip,uses_lib_strip_only,prop_source_list,library_path,pass_stub_by_default)
   return library_node


def avs_prop_symbol_builder(target, source, env):
   prop_sym_fn = str(target[0])
   prop_objs_str = ''.join([ ''.join([str(src), ' ' ]) for src in source ])

   nm_cmd = ''.join([ '${NM} -f posix ', prop_objs_str ])
   #import pdb; pdb.set_trace()
   data, err, rv = env.ExecCmds (nm_cmd, silent=True, target='AVS Proprietary Symbol File')

   if env['VERBOSE'] == 2:
     env.PrintInfo('Removing lines matching regex - " \.CONST_"')
     env.PrintInfo('Finding lines matching regex - " [DdTt] "')

   # Remove '\r' chars from nm output
   nm_data1 = re.sub('\r', '', data)

   #import pdb; pdb.set_trace()

   m = re.search('.*\.CONST_.*',nm_data1,re.M)
   print m
   while True:
       if m==None:
           break;
       nm_data = re.sub('.*\.CONST_.*', '', nm_data1, re.M);
       m = re.search('.*\.CONST_.*',nm_data,re.M);
       nm_data1 = nm_data;

   #import pdb; pdb.set_trace()
   # Find all lines with regex of: ' [DdTt] '
   nm_list = re.findall('.* [DdTt] .*', nm_data1, re.M)

   if env['VERBOSE'] == 2:
     env.PrintInfo('Creating Symbol File - %s' % prop_sym_fn)

     # Create the proprietary symbol file
   try:
      prop_sym_fh = open(prop_sym_fn, 'w')
   except IOError:
      env.PrintError ('Error Could not create Proprietary Symbol file - ' + prop_sym_fn)

   for nm_line in nm_list:
      prop_sym_fh.write(nm_line + '\n')

   # Close proprietary symbol file
   prop_sym_fh.close()
   #import pdb; pdb.set_trace()
   return None


def autogen_elite(env,source,target,ext):
	if env.IsTargetEnable(["AVS_ADSP"]):
		print "Creating elite autogen from avs"
		#import pdb;pdb.set_trace()
		if "AcquireMPMutex" in dir(env):
			print "Trying to acquire mutex"
			env.AcquireMPMutex("autogenelite")
			print "Acquired mutex"
		if os.path.exists(target):
			shutil.rmtree(target)
			print "removed the targets"
		#if not env.GetOption('clean'):
		if 1:
			os.makedirs(target)
			print "Created dir structure"
			if not os.access(target, os.X_OK):
				print "Setting up necessary permissions"
				curr_st = os.stat(target)
				os.chmod(target, curr_st.st_mode | stat.S_IEXEC)
				print "Setting up necessary permissions completed"
			#Copy only linker files
			source_files = "/".join([source,ext])
			print "Copying necessary files"
			get_files_to_copy = env.GlobFiles(glob_patterns=[source_files])
			[shutil.copy(each,target) for each in get_files_to_copy]
			#print os.path.join(target,os.path.basename(each))
			[os.chmod(os.path.join(target,os.path.basename(each)), stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO) for each in get_files_to_copy if os.path.exists(os.path.join(target,os.path.basename(each)))]
			print "Copying necessary files completed"
			#[print os.path.join(target,os.path.basename(each)) for each in get_files_to_copy if os.path.exists(os.path.join(target,os.path.basename(each)))]
		if "ReleaseMPMutex" in dir(env):
			print "Trying to release lock"
			env.ReleaseMPMutex("autogenelite")
			print "Released lock"
			#for each in get_files_to_copy:
		#	shutil.copy(each,target)

def avs_prop_link_builder(target, source, env):

   prop_objs_str = ''.join([ ''.join([str(src), ' ' ]) for src in source ])

   #print prop_objs_str
   #print source
   #This is to be called only after creating the object files.
   #In the other means it should only be called when env.addobject is called and not when build_tag='MOD'
   #
   if env['build'] == 'LEAF':
      path_to_txt=env.subst('${BUILDPATH}')
      #gen_txt_cmd = 'echo %s > %s.txt' % (os.path.join(prop_objs_str).replace("\\","/"),str(target[0]))
      f=open(str(target[0])+'_temp.txt','w')
      for each in source:
         #print type(str(each))
         #print str(each)
         each_temp=str(each)
         if os.name == 'nt': #if it is non-linux then replace the paths
            string_temp=str(os.path.join(str(each_temp))).replace("\\","/")
            string=str(string_temp)
         else:
            string=str(each_temp)
         f.write(str(string))
         f.write(' ')
      gen_txt_cmd = 'cat %s > %s.txt' % (str(target[0])+'_temp.txt',str(target[0]))
      f.close()
      #Genearate a temp file to have the files in it...

      #data, err, rv = env.ExecCmds (gen_txt_cmd, silent=True, target='AVS intermediate txt file generation...')
      link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive -o ', str(target[0]), " ", '@'+str(target[0])+'_temp.txt'])
      data_temp, err, rv = env.ExecCmds (link_cmd, silent=True, target='AVS Proprietary Linked Relocatable File')
      print data_temp
      #import pdb;pdb.set_trace();
   else:
     #print "running in MOD mode"
     link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive -o ', str(target[0]), " ", prop_objs_str])
     data_temp, err, rv = env.ExecCmds (link_cmd, silent=True, target='AVS Proprietary Linked Relocatable File')
   print data_temp
   print err

   return None

def avs_prop_strip_link_builder(target, source, env):
   prop_objs_str = ''.join([ ''.join([str(src), ' ' ]) for src in source ])
   if 'MDSPMODE' in env:
      link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive -o ', str(target[0]), " ", prop_objs_str])
   else:
      link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive --script ${BUILD_ROOT}/qdsp6/Partial_Linking/plink.linker -o ', str(target[0]), " ", prop_objs_str])
   data, err, rv = env.ExecCmds (link_cmd, silent=True, target='AVS Proprietary Linked Relocatable File')

   return None
   
def avs_sectools_encrypt_link_builder(target, source, env):
   
   
   #Load sectools builder.
   sectools_path=os.path.join(env['INC_ROOT'],"sectools","sectools_builder.py")
   if os.path.exists(sectools_path):
      
      print "avs so encryption generation in progress for {}".format(target[0])
      lookup_libid = {"0x1":"dolby","0x2":"dolby"} #Since the lib_id is unique, so use lib id as keys
      if lookup_libid.has_key(env["sec_encrypt_lib_id"]):
         print "Fetched lookup as {} for libid {}".format(lookup_libid[env["sec_encrypt_lib_id"]],env["sec_encrypt_lib_id"])
         lookedup_name=str(lookup_libid[env["sec_encrypt_lib_id"]])
      else:
         raise ValueError('could not find lookup info for libid'.format(env["sec_encrypt_lib_id"]))

      ### Build testkey based for internal testing. 
      shared_lib_install_dir_encrypt=os.path.join(env["DYNL_ENCRYPT_PATH"], 'test')
      
      if not os.path.exists(shared_lib_install_dir_encrypt):
          print "Encryption folder structure created "+shared_lib_install_dir_encrypt
          os.makedirs(shared_lib_install_dir_encrypt)
      
      temp_dir=os.path.join(shared_lib_install_dir_encrypt,'sectools_output');
      #sec tools command formation.
      cfg_path = os.path.join(env['INC_ROOT'],"sectools", "config", "integration", "secimagev2.xml")
      link_cmd = ''.join([ "python"," ",sectools_path," -i"," ",str(source[0])," -t ", temp_dir, " -g ",lookedup_name, " -o ",shared_lib_install_dir_encrypt," ","--lib_id=",env["sec_encrypt_lib_id"]," ","--client_id=0x1 --soc_hw_version=0x30020000 --soc_vers=0x3005 --build_policy_id=MULTIPLE_DEFAULT_SIGN", " --config ", str(cfg_path)])
      print link_cmd
     
      data, err, rv = env.ExecCmds (link_cmd, silent=True, target='Sectools encrypted so')
      if env['VERBOSE'] == 2:
         print data
      #We are validating so encrypted correctly or not with the belos pass and fail strings.
      #If any of the pass or fail strings are hit then a failure is treated.
      pass_strings=["is encrypted","signature is valid"] #input all pass validations
      fail_strings=["ERROR:"] #input all failure strings
      passed_or_not=[ data.find(s) for s in pass_strings if data.find(s) >= 0 ]
      failed_or_not=[ data.find(f) for f in fail_strings if data.find(f) >= 0 ]

      if rv or err or len(failed_or_not) or (not len(passed_or_not)):
         print "So signing or encryption or validation failed"
         sys.exit(-1)
      ##################################################################
      ### build prod_key based one for shipping to customers.
      #Always generate encrypted dynamic modules in this location
      shared_lib_install_dir_encrypt=os.path.join(env["DYNL_ENCRYPT_PATH"], 'prod')
      
      if not os.path.exists(shared_lib_install_dir_encrypt):
          print "Encryption folder structure created"+shared_lib_install_dir_encrypt
          os.makedirs(shared_lib_install_dir_encrypt)
      
      temp_dir=os.path.join(shared_lib_install_dir_encrypt,'sectools_output');
      #sec tools command formation.
      cfg_path = os.path.join(env['INC_ROOT'],"avs", "tpa", "dolby", "dolby_security", "prod_key", "Sectools_Prod_UIE", "secimagev2_prod_uie.xml")
      link_cmd = ''.join([ "python"," ",sectools_path," -i"," ",str(source[0])," -t ", temp_dir, " -g ",lookedup_name, " -o ",shared_lib_install_dir_encrypt," ","--lib_id=",env["sec_encrypt_lib_id"]," ","--client_id=0x1 --soc_hw_version=0x30020000 --soc_vers=0x3005 --build_policy_id=MULTIPLE_DEFAULT_SIGN", " --config ", str(cfg_path)])
      print link_cmd
     
      data, err, rv = env.ExecCmds (link_cmd, silent=True, target='Sectools encrypted so')
      if env['VERBOSE'] == 2:
         print data
      #We are validating so encrypted correctly or not with the belos pass and fail strings.
      #If any of the pass or fail strings are hit then a failure is treated.
      pass_strings=["is encrypted","signature is valid"] #input all pass validations
      fail_strings=["ERROR:"] #input all failure strings
      passed_or_not=[ data.find(s) for s in pass_strings if data.find(s) >= 0 ]
      failed_or_not=[ data.find(f) for f in fail_strings if data.find(f) >= 0 ]

      if rv or err or len(failed_or_not) or (not len(passed_or_not)):
         print "So signing or encryption or validation failed"
         sys.exit(-1)

   else:
      print "Error: {} not found".format(sectools_path)
      raise #Leave the truthworthiness to python
   
   return None


def avs_prop_obfus_builder(target, source, env):
   prop_sym_file = str(source[0])
   prop_link_reloc = str(source[1])

   # Derive map filename from symbol filename
   prop_sym_path, prop_sym_ext = os.path.splitext(prop_sym_file)
   prop_map_fn = ''.join([prop_sym_path, '.map'])

   obfus_cmd = ''.join([ '${OBFUSCATE} --symbols-file ', prop_sym_file, ' --map ',
               prop_map_fn, ' -o ', str(target[0]), ' ', prop_link_reloc])

   data, err, rv = env.ExecCmds (obfus_cmd, silent=True, target='AVS Obfuscated Relocatable File')


   return None


def avs_prop_obfus_emitter(target, source, env):
   env.Clean(target, env['PROP_MAP_FN'])

   return (target, source)
def avs_prop_link_emitter(target,source,env):
   env.Clean(target, env['PROP_MAP_FN_TXT'])

   return (target, source)

def avs_sectools_encrypt_emitter(target,source,env):
   env.Clean(target, env['DYNL_ENCRYPT_PATH'])

   return (target, source)
   
def avs_prop_strip_builder(target, source, env):
   #import pdb;pdb.set_trace()
   if 'MDSPMODE' in env:
      strip_cmd = ''.join([ '${STRIP} -x -w -K *xx_msg_* -K *xx_err_msg_*  ', str(source[0]), ' -o ', str(target[0])])
   else:
      strip_cmd = ''.join([ '${STRIP} -x ', str(source[0]), ' -o ', str(target[0])])
   data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='AVS Stripped Relocatable File')

   return None


#Physically shared libraries should exist to mark it as dependent shared libraries on the other so's
#This function will generate all the needed dependent shared libraries at the time of scons reading itself
#This should be called first from to component top level scons file(Say no it should be called from avs/build/avs.scons)
def avs_so_generate(env,target,source):

   AVS_ROOT=env.subst('${AVS_FOLDER_ROOT}')

   path=os.path.join(AVS_ROOT,'build','autogenerated')
   #Delete the prev artifacts.
   if os.path.exists(path):
         shutil.rmtree(path)

   if not env.GetOption('clean'):
      if not os.path.exists(path):
          os.makedirs(path)
      #Copy shared lib api path and its header file from
      #Create a dummy header file and its own cpp file and form a dummy so
      header_path=os.path.join(path,'dummy_so.h')
      cpp_path=os.path.join(path,'dummy_so.cpp')
      shared_binaries_path_os=os.path.join(path,'dummy_so.os')
      shared_binaries_path_so=os.path.join(path,'dummy_so.so')

      gen_so_header =open(env.RealPath(header_path), 'w')
      gen_so_cpp =open(env.RealPath(cpp_path), 'w')
      str_timestamp = "#define DUMMY_SO_TIMESTAMP 0x%s" % datetime.datetime.now().strftime("%a_%b_%d_%H:%M:%S_PST_%Y");
      gen_so_header.write(str_timestamp)
      content_to_write='''#include "dummy_so.h"
      #define DUMMY_DEFINE DUMMY_SO_TIMESTAMP '''
      gen_so_cpp.write(content_to_write)

      #Try to create a shared library as of now
      get_compiler=env.subst('${SHCXX}')
      get_fpic=env.subst('${HEXAGONLD_SH_FPIC}')
      q6version='-m'+os.environ.get('HEXAGON_Q6VERSION')
      #Form the compile command to generate the shared library.
      compile_cmd=''.join([get_compiler," ",get_fpic," ",q6version," ","-o"," ",shared_binaries_path_os," ","-c"," ",cpp_path])
      data, err, rv = env.ExecCmds (compile_cmd, silent=True, target='Generating the needed shared files')
      print err



      sos_to_form=[] #Basket to hold dependent so's list

      dict=env['SHARED_LIB_DICT']
      err_flag=False

      for key,value in dict.iteritems():
         #print key
         #print len(value)
         if len(value) >2:
            if value[2] != []:
               for each in value[2]:

                  #Get the lib dependent lib version now.
                  #Convert the lib to upper and search in the dict
                  strip_so=''.join(['USES_', each.upper(), '_STRIP_SO'])
                  norm_so=''.join(['USES_', each.upper(), '_SO'])
                  if dict.has_key(strip_so) == True:

                     major_ver=dict[strip_so][0][0]
                     minor_ver=dict[strip_so][0][1]

                     get_so_path=os.path.join(path,'lib'+each+'.so')
                     form_so_names=''.join([get_so_path," ",'-soname'," ",each+'.so.'+str(major_ver)])

                     if not os.path.exists(get_so_path):
                        sos_to_form.append(form_so_names)
                  elif dict.has_key(norm_so) == True:

                     major_ver=dict[norm_so][0][0]
                     minor_ver=dict[norm_so][0][1]

                     get_so_path=os.path.join(path,'lib'+each+'.so')
                     form_so_names=''.join([get_so_path," ",'-soname'," ",each+'.so.'+str(major_ver)])
                     sos_to_form.append(form_so_names)
                     if not os.path.exists(get_so_path):
                        sos_to_form.append(form_so_names)
                  else:
                     print "Error: These libs are mentioned as dependency but not specified in the build config %s" %each
                     err_flag=True


      if err_flag== True:
         sys.exit(-1)
      sos_to_form=list(set(sos_to_form))

      for sos in sos_to_form:
          #Generate the dependent shared libraries by embedding the actual shared library name including the version number using "-soname" gcc flag.
          form_link_command=''.join([env.subst('${SHLINK}')," "," -nostdlib -shared"," ",shared_binaries_path_os," ","-o"," ",sos])
          data, err, rv = env.ExecCmds (form_link_command, silent=True, target='Generating the needed dependt shared files')
          if err:
            print "Error from the dependencies generation:"+err
   else:
      if os.path.exists(path):
         shutil.rmtree(path)

   return None

def h2xml_file_generate(env,scan_path):
   
   # Only build for AVS flavor
   if not env.IsTargetEnable(["AVS_ADSP_USER","AVS_ADSP"]):
      return None

   #Define all builder variables
   if type(scan_path) is not list:
      scan_path = [scan_path]

   #Set needed environment
   env.Replace(H2XML_WCD_XML = '${AVS_FOLDER_ROOT}/build/h2xml/config_adsp.xml')
   env.Replace(H2XML_WCD_RTF = '${AVS_FOLDER_ROOT}/build/h2xml/h2xml_doc_template.rtf')
   env.Replace(H2XML_OUTPUT_PATH = '${AVS_FOLDER_ROOT}/build/h2xml_autogen/${BUILD_ID}')
   env.Replace(H2XML_INCLUDE_PATH = "${H2XML_INC_PATH}")
   
   #Include path logic
   H2XML_INCLUDE_PATHS_TEMP=env.subst('${H2XML_INCLUDE_PATH}').split(' ')
   get_paths_list = ''.join([ ''.join(["-I"+str(path), ' ' ]) for path in H2XML_INCLUDE_PATHS_TEMP ])
   env.Replace(H2XML_INCLUDE_PATHS=get_paths_list)
   if env['VERBOSE'] == 2:
      print "H2xml include path list {}".format(env.subst("${H2XML_INCLUDE_PATHS}"))
   
  
   #This will take care of flavor based h2xml generation
   if not os.path.exists(env.subst('${H2XML_OUTPUT_PATH}')):
      os.makedirs(env.subst('${H2XML_OUTPUT_PATH}'))
   if not os.access(env.subst('${H2XML_EXE}'), os.X_OK):
      curr_st = os.stat(env.subst('${H2XML_EXE}'))
      os.chmod(env.subst('${H2XML_EXE}'), curr_st.st_mode | stat.S_IEXEC)
   
   for each_scan in scan_path:
      print "Scanning folders in %s ..." %each_scan
      each_scan = r"%s" %each_scan
      
      for root,dirs,files in os.walk(each_scan):
         #Scan all headers under specified folder
         get_hfiles=r'%s/*.h' %root
         hfile_xml=glob.glob(get_hfiles)
         
         #remove server header files (starting with adsp_)
         get_servicehfiles=r'%s/adsp_*.h' %root
         serivcehfiles = glob.glob(get_servicehfiles)
         hfile_xml  = [files for files in hfile_xml if files not in serivcehfiles]

         for each_file in hfile_xml:
            each_file = os.path.normpath(each_file)
            #h2xml_cmd = ''.join([ '${H2XML_EXE} -rtf ${H2XML_WCD_RTF} -conf ${H2XML_WCD_XML} -D __H2XML__ -o ${H2XML_OUTPUT_PATH} ${H2XML_INCLUDE_PATHS}',' ', each_file])
            h2xml_cmd = ''.join([ '${H2XML_EXE} -conf ${H2XML_WCD_XML} -D __H2XML__ -o ${H2XML_OUTPUT_PATH} ${H2XML_INCLUDE_PATHS}',' ', each_file])
            print env.subst(h2xml_cmd)
            data, err, rv = env.ExecCmds (h2xml_cmd, silent=True, target='h2xml generation in progress')
            if err or rv:
               print 'Error: ', data
               sys.exit(-1)
