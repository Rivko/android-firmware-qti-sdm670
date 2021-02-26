#===============================================================================
#
# VIDEO_PROCESSING Library Builder
#
# GENERAL DESCRIPTION
#    This builder will create libraries after incorporating the following process:-
# 				1. Build the lib. 
#  				3. Strip IP protected symbols from Partial link libs
# 				4. Obfuscate(Name Mangle) the IP protected symbols
# 				6. Strip the final .lib & remove the unnecessary lib's.
#
# Copyright (c) 2015 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#===============================================================================

import re
import os
import stat
import shutil
#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Builder function definitions
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('hcp_prop_library_builder')

def generate(env):
   # Generate function for Proprietary Library builder:  Sets up the 
   # pseudo builder, action and builder function to be called by
   # clients to pass library details to the builder.
   
   env.Replace(USES_HCP_LIBRARY_BUILDER = True)

   # Add Obfuscate tool to construction environment
   if env['PLATFORM'] == 'linux':
      env.Replace(OBFUSCATE = '${BUILD_ROOT}/vap/build/sbin/linux64/hexagon-obfuscate')
   else:
      env.Replace(OBFUSCATE ='${BUILD_ROOT}/vap/build/sbin/win32/hexagon-obfuscate.exe')
      
   # Get verbose option
   env.Replace(VERBOSE = int(env.GetOption('verbose')))
   
   # Add pseudo-builder name (wrapper) function
   env.AddMethod(add_hcp_library, 'AddHcpLibrary')
   
   # Add pseudo-builder name (wrapper) function (Shared Object Creation) 
   env.AddMethod(add_hcp_shared_library, 'AddHcpSharedLibrary')
   
   # ----------------------------------
   # Setup the HCP proprietary builders for HCP Static Objects 
   # ----------------------------------

   # HCP Proprietary Symbol Builder
   fastcv_prop_symbol_act = env.GetBuilderAction(fastcv_prop_symbol_builder)
   fastcv_prop_symbol_bld = env.Builder(action = fastcv_prop_symbol_act)
   env.Append(BUILDERS = {'FastCVPropSymbolBuilder' : fastcv_prop_symbol_bld})
   
  
   # HCP Proprietary Link Builder
   fastcv_prop_link_act = env.GetBuilderAction(fastcv_prop_link_builder)
   fastcv_prop_link_bld = env.Builder(action = fastcv_prop_link_act)
   env.Append(BUILDERS = {'FastCVPropLinkBuilder' : fastcv_prop_link_bld})
   
   # HCP Proprietary Obfuscate Builder
   fastcv_prop_obfus_act = env.GetBuilderAction(fastcv_prop_obfus_builder)
   fastcv_prop_obfus_bld = env.Builder(action = fastcv_prop_obfus_act,
                                    emitter= fastcv_prop_obfus_emitter)
   env.Append(BUILDERS = {'FastCVPropObBuilder' : fastcv_prop_obfus_bld})
   
   # HCP Proprietary Strip Builder
   fastcv_prop_strip_act = env.GetBuilderAction(fastcv_prop_strip_builder)
   fastcv_prop_strip_bld = env.Builder(action = fastcv_prop_strip_act)
   env.Append(BUILDERS = {'FastCVPropStripBuilder' : fastcv_prop_strip_bld})
 
 
    # ----------------------------------
   # Setup the HCP proprietary builders for HCP Static Objects 
   # ----------------------------------

   # HCP Shared Proprietary Symbol Builder
   fastcv_shared_prop_symbol_act = env.GetBuilderAction(fastcv_shared_prop_symbol_builder)
   fastcv_shared_prop_symbol_bld = env.Builder(action = fastcv_shared_prop_symbol_act)
   env.Append(BUILDERS = {'FastCVSharedPropSymbolBuilder' : fastcv_shared_prop_symbol_bld})
      
   # HCP Shared Proprietary Obfuscate Builder
   fastcv_shared_prop_obfus_act = env.GetBuilderAction(fastcv_shared_prop_obfus_builder)
   fastcv_shared_prop_obfus_bld = env.Builder(action = fastcv_shared_prop_obfus_act,
                                    emitter= fastcv_shared_prop_obfus_emitter)
   env.Append(BUILDERS = {'FastCVSharedPropObfusBuilder' : fastcv_shared_prop_obfus_bld})
   
   # HCP Shared Proprietary Strip Builder
   fastcv_shared_prop_strip_act = env.GetBuilderAction(fastcv_shared_prop_strip_builder)
   fastcv_shared_prop_strip_bld = env.Builder(action = fastcv_shared_prop_strip_act,
                                               emitter = fastcv_shared_prop_strip_emitter)
   env.Append(BUILDERS = {'FastCVSharedPropStripBuilder' : fastcv_shared_prop_strip_bld})
 
  
   return


def add_hcp_library(env, build_tags, library, sources,build_type='LEAF'):
   # Input parameters:
   #   'build_tags' and 'library' are the same as add_library
   
	library_node = []

	if not env.IsTargetEnable(build_tags):
		return library_node
   
	library_path, library_fn = os.path.split(library)
	cleanpack = env.GetOption('cleanpack')
	
	if ('USES_CLEAN_PACK' in env) or cleanpack:
		library_node = env.AddBinaryLibrary(build_tags, library + '_strip', sources)

	else:
		# Create the first program library using proprietary sources
		object_node = env.Object(sources)		
		library_node = env.Library(library, object_node)
	      
	  	
	  	# Use 'ld' to partial link proprietary libraries to create a single relocatable file
		link_reloc = env.FastCVPropLinkBuilder(library_path + '/reloc', library_node)
 
	  
		# Use 'nm' (Name list) to produce list of proprietary symbols 
		# from the proprietary object files 
		prop_sym_fn = library_path + '/remove_symbols.txt'
		prop_sym_file = env.FastCVPropSymbolBuilder(prop_sym_fn, link_reloc)
		
		# Put symbol map filename into environment so it can be cleaned 
		# later.  SCons doesn't know about the map file, so we have to 
		# explicitly clean with an env.Clean() call.
		env.Replace(PROP_MAP_FN = library_path + '/remove_symbols.map')
		
		
		# Use 'Obfuscate' program (Qualcomm) to encrypt the proprietary 
		# symbols in the relocatable object
		prop_obfus = env.FastCVPropObBuilder(library + '_obfsc', [prop_sym_file, link_reloc])
				
				
		# Use 'strip' to remove all non-global symbols from obfuscated 
		# relocatable object
		prop_strip = env.FastCVPropStripBuilder(library + '_strip', prop_obfus)
		
		# Create strip library file (e.g., Quartet_strip.lib) from the stripped
		# relocatable object.
		library_node = env.AddLibrary(build_tags, library + '_strip', prop_strip)
			
		library_node = env.Library(build_tags, library, prop_obfus)
		
	return library_node


def add_hcp_shared_library(env, build_tags, library, sources, dependencies, idl_files = None):
   # Input parameters:
   #   'build_tags' and 'library' are the same as add_library
   
	library_node = []

	if not env.IsTargetEnable(build_tags):
		return library_node
   
	library_path, library_fn = os.path.split(library)
	# Ensure proper flags for dynamic linking 
	env.Replace(ASM_DFLAGS = '-D__V_DYNAMIC__')
	env.Replace(CC_DFLAGS = '-D__V_DYNAMIC__')
	env.Replace(HEXAGONCC_WARN = '-Wall -Wpointer-arith -Wno-missing-braces -Wno-strict-aliasing -Wstrict-prototypes -Wnested-externs')
	env.Replace(HEXAGON_UNDEF = ' ')
	
	q6V = '${Q6VERSION}'
	env.PrintInfo('Q6 version read = ' + q6V)
	
	if not env.subst("$Q6VERSION") == "v60":
	    env.Replace(HEXAGON_LANIND = '-fno-exceptions -fno-strict-aliasing -fno-zero-initialized-in-bss -fdata-sections -mno-literal-pool-addresses ')  	
	    env.Replace(CFLAGS = ' -G0 ')
	else:
	    env.Replace(HEXAGON_LANIND = '-fno-exceptions -fno-strict-aliasing -fno-zero-initialized-in-bss -fdata-sections ') 
	    env.Replace(CFLAGS = ' -G0 ')
	
	#shared_library_node = env.AddMySharedLibrary(build_tags,library, sources, idl_files)
	library_node = env.AddMySharedLibrary(build_tags,library, sources, idl_files)	
	
	##Specify Dependencies for a given shared-library. Ensures the link of the dependency before the link of the shared object
	#for  dependency in dependencies:
	#    env.Depends(library_node, env.RealPath(dependency, posix=True))
	#    env.Append(SHLINKFLAGS = ' ' + dependency + ' ')
	#
	## Use 'nm' (Name list) to produce list of proprietary symbols 
	## from the proprietary object files 
	#prop_shared_sym_fn = library_path + '/remove_dyn_symbols.txt'
	#prop_shared_sym_file = env.FastCVSharedPropSymbolBuilder(prop_shared_sym_fn, library_node)
	#
	## Put symbol map filename into environment so it can be cleaned 
	## later.  SCons doesn't know about the map file, so we have to 
	## explicitly clean with an env.Clean() call.
	#env.Replace(PROP_SHARED_MAP_FN = library_path + '/remove_dyn_symbols.map')
	#env.Replace(PROP_SHARED_STRIP1_FN = library + '_1')	
	#env.Replace(PROP_SHARED_STRIP2_FN = library + '_2')		
	#env.Replace(PROP_SHARED_UNSTRIPPED_FN = library + env['SHLIBSUFFIX']+ '.unstripped')		
	#
	## Use 'Obfuscate' program (Qualcomm) to encrypt the proprietary 
	## symbols in the relocatable object
	#prop_shared_obfus = env.FastCVSharedPropObfusBuilder(library + '_obfsc', [prop_shared_sym_file, library_node])
    #
	## Use 'strip' to remove all non-global symbols from obfuscated 
	## relocatable object
	#prop_shared_strip = env.FastCVSharedPropStripBuilder(env['BUILDPATH'] + '/ship/' + library_fn + env['SHLIBSUFFIX'], prop_shared_obfus)
    #
	#library_node = env.AddSharedLibrary(build_tags, library + '_intermediate', prop_shared_strip)
	
	return library_node
   
def fastcv_prop_symbol_builder(target, source, env):
   prop_sym_fn = str(target[0])
   prop_libs_str = str(source[0])

   nm_cmd = ''.join([ '${NM} -f posix ', prop_libs_str])
   data, err, rv = env.ExecCmds (nm_cmd, silent=True, target='FastCV Proprietary Symbol File')
       
   
  # grep " [DTrt] " || true) | (grep -v ".CONST_" || true) | (grep -v "fcv" || true) > ', prop_sym_fn ])
   lines = data.split("\n")
   prop_sym_fh = open(prop_sym_fn, 'w')
   for line in lines:
		if not re.search('fcv', line) and not re.search('.CONST_', line) and re.search('\s[DTrt]\s', line) :
			prop_sym_fh.write(line + '\n')			
   prop_sym_fh.close()

   return None
   

def fastcv_shared_prop_symbol_builder(target, source, env):
   prop_shared_sym_fn = str(target[0])
   prop_shared_libs_str = str(source[0])

   symbol_pattern = 'hcp'
   lib_path, library_name = os.path.split(prop_shared_libs_str)
   env.PrintInfo(library_name)

   
   #import pdb; pdb.set_trace()
   shutil.copy(prop_shared_libs_str,prop_shared_libs_str+'.unstripped')
   nm_cmd = ''.join([ '${NM} -f posix ', prop_shared_libs_str])
   data, err, rv = env.ExecCmds (nm_cmd, silent=True, target='FastCV Shared Lib Proprietary Symbol File')
    
   
  # grep " [DTrt] " || true) | (grep -v ".CONST_" || true) | (grep -v "fcv" || true) > ', prop_sym_fn ])
   lines = data.split("\n")
   prop_shared_sym_fh = open(prop_shared_sym_fn, 'w')
   for line in lines:
	  if symbol_pattern == 'hcp' :
	     if not re.search(symbol_pattern, line) and not re.search('.CONST_', line) and re.search('\s[DTrt]\s', line) :
	        prop_shared_sym_fh.write(line.rstrip('\n') + '\n')
            
   prop_shared_sym_fh.close()

   return None
 

def fastcv_prop_link_builder(target, source, env):
   prop_libs_str = str(source[0])
   env.PrintInfo('Creating partial link for Proprietary Lib')
   
   link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive -o ', str(target[0]), " ", prop_libs_str])
   data, err, rv = env.ExecCmds (link_cmd, silent=True, target='FastCV Proprietary Library Relocatable File')

 
   return None
   
   
def fastcv_prop_obfus_builder(target, source, env):
   prop_sym_file = str(source[0])
   prop_link_reloc = str(source[1])
      
   prop_sym_path, prop_sym_ext = os.path.splitext(prop_sym_file)
   prop_map_fn = ''.join([prop_sym_path, '.map'])
   
   obfus_cmd = ''.join([ '${OBFUSCATE} --symbols-file ', prop_sym_file, ' --map ', 
               prop_map_fn, ' -o ', str(target[0]), ' ', prop_link_reloc])
   
     
   data, err, rv = env.ExecCmds (obfus_cmd, silent=True, target='FASTCV Obfuscated Relocatable File')
   
   return None

   
def fastcv_shared_prop_obfus_builder(target, source, env):
   prop_sym_file = str(source[0])
   prop_link_reloc = str(source[1])
      
   prop_sym_path, prop_sym_ext = os.path.splitext(prop_sym_file)
   prop_map_fn = ''.join([prop_sym_path, '.map'])
   
   obfus_cmd = ''.join([ '${OBFUSCATE} --symbols-file ', prop_sym_file, ' --map ', 
               prop_map_fn, ' -o ', str(target[0]), ' ', prop_link_reloc])
   
     
   data, err, rv = env.ExecCmds (obfus_cmd, silent=True, target='FASTCV Obfuscated Relocatable File')
   
   return None

   
def fastcv_prop_obfus_emitter(target, source, env):
   env.Clean(target, env['PROP_MAP_FN'])
   
   return (target, source)

  
def fastcv_shared_prop_obfus_emitter(target, source, env):
   env.Clean(target, env['PROP_SHARED_MAP_FN'])
   
   return (target, source)
   
   
   
def fastcv_prop_strip_builder(target, source, env):
   strip_cmd = ''.join([ '${STRIP} -x ', str(source[0]), ' -o ', str(target[0])])
   data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='FastCV Stripped Relocatable File')
   
   return None


def fastcv_shared_prop_strip_builder(target, source, env):
   
   strip_cmd = ''.join([ '${STRIP} -x ', str(source[0]), ' -o ', str(target[0])])
   data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Stripping Object Level 1')
   
    # Put symbol map filename into environment so it can be cleaned 
    # later.  SCons doesn't know about the map file, so we have to 
    # explicitly clean with an env.Clean() call.
   
   lib_path, lib_name = os.path.split(str(target[0]))
   src_lib_path, src_lib_name = os.path.split(str(source[0]))
   if re.search('dspCV', lib_name) :
         strip_cmd = ''.join([ '${STRIP} -w -K dspCV* -K apps_mem_heap* ', str(target[0]), ' -o ', str(target[0])])
         data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Stripping dspCV Object')   
         shutil.copy(str(target[0]), src_lib_path+'/'+lib_name)
   else :
      if re.search('scve', lib_name) :
         strip_cmd = ''.join([ '${STRIP} -w -K scve* ', str(target[0]), ' -o ', str(target[0])])
         data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Stripping Object Level 2')
      else :
         strip_cmd = ''.join([ '${STRIP} -w -K fcv* ', str(target[0]), ' -o ', str(target[0])])
         data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Stripping Object Level 2')	     
        	  
      strip_cmd = ''.join([ '${STRIP} -w -K !*C  ', str(target[0]), ' -o ', str(target[0])])
      data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Stripping Object Level 3')

      strip_cmd = ''.join([ '${STRIP} -w -K !*_* -K *_v*  ', str(target[0]), ' -o ', str(target[0])])
      data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='Final Stripped Object') 	
	  
      shutil.copy(str(target[0]), src_lib_path+'/'+lib_name)

	  
   return None

   
   
   
def fastcv_shared_prop_strip_emitter(target, source, env):
   env.Clean(target, env['PROP_SHARED_STRIP1_FN'])
   env.Clean(target, env['PROP_SHARED_STRIP2_FN'])
   env.Clean(target, env['PROP_SHARED_UNSTRIPPED_FN'])
   
   return (target, source)
