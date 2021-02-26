#===============================================================================
#
# Builder file to generate shared libraries
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0/sharedlib_builder.py#4 $
#  $DateTime: 2017/10/18 00:47:21 $
#  $Change: 14636412 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================

import re
import os
import stat
import shutil
import datetime
#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Builder function definitions
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sharedlib_builder')

   
def generate(env):
   # Generate function for Shared Library Builder:  Sets up the 
   # pseudo builder, action and builder function to be called by
   # clients to pass library details to the builder.
   
   env.Replace(USES_DYNAMIC_LIBRARY_BUILDER = True)

   # Add QAIC tool to construction environment
   env.PrintInfo("SHARED_LIB_BUILDER: searching for qaic tool")
   env.Append(CPPPATH=['${BUILD_ROOT}/platform/qaic'])
   env.FindQaicExe()

   # Get verbose option
   env.Replace(VERBOSE = int(env.GetOption('verbose')))
   
   # Add pseudo-builder name (wrapper) function
   env.AddMethod(add_my_shared_library, 'AddMySharedLibrary')
   env.AddMethod(install_my_shared_lib, 'InstallMySharedLib')
   
   # Skel Builder
   skel_act = env.GetBuilderAction(skel_builder)
   skel_bld = env.Builder(action = skel_act,
                         emitter= skel_emitter)

   env.Append(BUILDERS = {'SkelBuilder' : skel_bld})
   
   # Header Builder
   header_act = env.GetBuilderAction(header_builder)
   header_bld = env.Builder(action = header_act,
                         emitter= header_emitter)

   env.Append(BUILDERS = {'HeaderBuilder' : header_bld})
   
   # Copy Builder
   copy_act = env.GetBuilderAction(install_so_builder)
   copy_bld = env.Builder(action = copy_act,
                         emitter= installed_so_emitter)

   env.Append(BUILDERS = {'InstallBld' : copy_bld})
   
   # Time Stamp Builder
   ts_act = env.GetBuilderAction(time_stamp_builder)
   ts_bld = env.Builder(action = ts_act,
                         emitter= ts_emitter)

   env.Append(BUILDERS = {'TimeStampBuilder' : ts_bld})

   return

#------------------------------------------------------------------------------
# Builder to create the sharedlibrary
#------------------------------------------------------------------------------
def add_my_shared_library(env, build_tags, library, sources, idl_files = None):
    # Input parameters:
    #   'build_tags' and 'library' are the same as add_library
    library_node = []
    skel_sources = []
    if not env.IsTargetEnable(build_tags):
        return library_node
    library_path, library_fn = os.path.split(library)
    ts_env = env.Clone()
    # Append the time-stamp info to the SHlink flags. So that the new timestamp 
    # gets embedded only when the sources/dependencies gets updated on incremental 
    # build. By appending to link flags rather than having a timestamp source file,
    # we are preventing unnecessary rebuild of SO, when there is no change in sources.
    build_version= ts_env.RealPath('${BUILD_ROOT}/build/manifest.xml')
    timestamp_c= ts_env.RealPath('${BUILDPATH}/timestamp_%s.c'%library_fn)
    # Create timestamp source file 
    timestamp_c = ts_env.TimeStampBuilder(timestamp_c, build_version)
    timestamp_o = ts_env.SharedObject(timestamp_c)
    ts_env.Append(SHLINKFLAGS = ts_env.RealPath(str(timestamp_o[0])).replace('\\', '/'))
    #if the IDL file is not passed, bypass creation of skel files and skel_sources is empty
    if(idl_files != None):
        if type(idl_files) is str:
            idl_files = [idl_files]
        for idl_file in idl_files:
            idl_file_path, idl_file_name = os.path.split(idl_file)
            idl_file_name = idl_file_name.split('.')[0]

            #get the target file locations
            skel_c= ts_env.RealPath('${BUILDPATH}/'+idl_file_name+'_skel.c')
            stub_c= ts_env.RealPath('${BUILDPATH}/'+idl_file_name+'_stub.c')
            idl_header= ts_env.RealPath(idl_file_path+'/'+idl_file_name+'.h')
                
            # Create the header file from IDL
            idl_header = ts_env.HeaderBuilder(idl_header, idl_file)
            # Create the skel file from IDL
            skel_c = ts_env.SkelBuilder(skel_c, idl_file)
            ts_env.Depends(skel_c,idl_header)
            sources.append(skel_c)
        
    # Create the shared library
    library_node = ts_env.AddSharedLibrary(build_tags, library, sources)
    # Adding dependency information
    ts_env.Requires(library_node, timestamp_o)

    return library_node
   
#------------------------------------------------------------------------------
# Wrapper function for shared lib installer
#------------------------------------------------------------------------------
def install_my_shared_lib(env, list_files):
    target_path, fn = os.path.split(env.RealPath(list_files))
    installed_list = '${BUILD_ROOT}/build/dynamic_modules/${SHORT_BUILDPATH}/map_'+fn
    if(list_files != None):
        installed_list = env.InstallBld(installed_list, list_files)

    return installed_list
   
#------------------------------------------------------------------------------
# Builder to install list of libraries to common path
#------------------------------------------------------------------------------
def install_so_builder(target, source, env):
    target_list = env.RealPath(str(target[0]))
    target_path, target_fn = os.path.split(target_list)
    source_list = env.RealPath(str(source[0]))
    source_path, source_fn = os.path.split(source_list)
    if not env.PathExists(source_list):
        env.PrintInfo(source_list+" doesn't exist")
        return None
    if not env.PathExists(target_path):
        os.mkdir(target_path)
    WriteFile = open(target_list, 'w')
    with open(source_list) as f:
        for line in f:
            if line[0]!='#':
                lib = env.RealPath('${BUILD_MS_ROOT}/'+line)
                lib_path, lib_fn = os.path.split(lib)
                shutil.copy2(lib, target_path+'/'+lib_fn)
                WriteFile.write(line+'\n') 
    WriteFile.close()
    
    return None

#------------------------------------------------------------------------------
# Emmiter to remove installed .so files 
#------------------------------------------------------------------------------
def installed_so_emitter(target, source, env):
    target_list = str(target[0])
    target_path, fn = os.path.split(target_list)
    if env.PathExists(target_path) and env.IsBuildInternal():
        shutil.rmtree(target_path)
    return (target, source)

   
#------------------------------------------------------------------------------
# Builder to create skel file from IDL
#------------------------------------------------------------------------------
def skel_builder(target, source, env):
    target_path, fn = os.path.split(str(target[0]))
    exec_cmd = ''.join([ '${QAIC_EXE} ', env.subst('$_CPPINCFLAGS'),' ', str(source[0]), ' --remoting-only ', ' -o ', target_path])
    data, err, rv = env.ExecCmds (exec_cmd, silent=True, target='Generating Skel.c file from IDL')
    env.PrintInfo('Running QAIC command - %s' % exec_cmd)
    print err
    return None

   
#------------------------------------------------------------------------------
# Builder to create header file from IDL
#------------------------------------------------------------------------------
def header_builder(target, source, env):
    target_path, fn = os.path.split(str(target[0]))
    exec_cmd = ''.join([ '${QAIC_EXE} ', env.subst('$_CPPINCFLAGS'),' ', str(source[0]), ' --header-only ', ' -o ', target_path])
    data, err, rv = env.ExecCmds (exec_cmd, silent=True, target='Generating header file from IDL')
    env.PrintInfo('Running QAIC command - %s' % exec_cmd)
    print err
    return None

#------------------------------------------------------------------------------
# Builder to create timestamp source
#------------------------------------------------------------------------------
def time_stamp_builder(target, source, env):
    ts_file = str(target[0])
    library_fn =  os.path.basename(ts_file)[10:-2]
    manifest_file = open(env.RealPath(str(source[0])), 'r')
    import xml.etree.ElementTree as ET
    config = ET.parse(manifest_file).getroot()
    im_tree = config.find('image_tree')
    build_ver = im_tree.find('name').text+'-'+im_tree.find('revision').text+'-'+im_tree.find('client').text+'-'+im_tree.find('spin').text
    current_time = datetime.datetime.now()
    timestamp_value = current_time.strftime("Q6_BUILD_TS_%a_%b_%d_%H:%M:%S_PST_%Y")

    content = 'const char '+library_fn+'_engg_ts[] __attribute__((visibility(\"default\"))) =\"'+timestamp_value+'_'+build_ver+'_'+library_fn.upper()+'\";\n'
    WriteFile = open(env.RealPath(str(target[0])), 'w')
    WriteFile.write(content) 
    WriteFile.close()
    manifest_file.close()
    
    return None

#------------------------------------------------------------------------------
# Emmiter to remove skel/stub files 
#------------------------------------------------------------------------------
def skel_emitter(target, source, env):
   env.Clean(target, str(target[0]))
   env.Clean(target, str(target[0]).replace('skel.c', 'stub.c'))
   return (target, source)

#------------------------------------------------------------------------------
# Emmiter to remove idl header file
#------------------------------------------------------------------------------
def header_emitter(target, source, env):
   env.Clean(target, str(target[0]))
   return (target, source)

#------------------------------------------------------------------------------
# Emmiter to remove timestamp source
#------------------------------------------------------------------------------
def ts_emitter(target, source, env):
   env.Clean(target, str(target[0]))
   return (target, source)

