#============================================================================
#
# Island Builder
#
# GENERAL DESCRIPTION
#    Contains a builder that allows developers to include libraries and 
#    object files in the TCM island image for the ADSP image.
#
# Copyright (c) 2009-2014 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/island_builder.py#1 $
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
# 2015-06-08   SM      Addition of whitelist support, to allow developers to pass
#                      symbol information to the list_extref.py builder.
# 2014-08-29   SM      Put a deprecation warning in the code for GenerateIslandLCSFile
#                      since this functionality was moved to the lcs_autogen.py.
# 2014-07-18   SM      Removed the hardcoding from the kernel path, and pointed it to the
#                      generic script location.
# 2014-05-28   SM      Fixed a bug in the AddIsland* APIs.  These APIs were expecting
#                      'SCons.Node.NodeList' sources to be passed in as inputs.  However,
#                      the APIs need to be more generic to deal with other types, such as
#                      'File'.
# 2014-05-23   SM      Fixed a bug in the AddIsland* APIs.  If the same library was
#                      called multiple times, it would cause duplicated lines to show
#                      up in the Island Specification File.
#                      Added the capability to pass in multiple Island Specification
#                      files, or none at all.
# 2014-05-05   SM      Removed relative path option.  Path must be relative.  Also
#                      added the correct formatting of the output file to be picked up
#                      by the linker generator script.
# 2014-04-14   SM      Adding API to add library/object/section combinations.
# 2014-03-31   SM      Updating location of qurt-island-link.py to resolve 
#                      pack compilation failure.
# 2014-02-27   SM      Adding linker script processing.
# 2014-01-29   SM      Updating builder to output lib - obj - section parameters.
# 2014-01-16   SM      Adding emitter to update target when sections are updated.
# 2014-01-09   SM      Initial revision.
# 
#============================================================================
import os
import re

#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
ISLAND_USES_FLAG = 'USES_ISLAND'
island_libs = 'island_libs'
island_objs = 'island_objs'
island_libs_objs = 'island_libs_objs'
island_sect_dict = 'island_sect_dict'
island_libs_objs_dict = 'island_libs_objs_dict'
qurt_linker_script = 'qurt_linker_script'
island_whitelist = 'island_whitelist'
island_whitelist_dict = 'island_whitelist_dict'

#----------------------------------------------------------------------------
# Hooks for SCons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('island_builder')

def generate(env):
   env.AddUsesFlags(ISLAND_USES_FLAG)
   
   # Declare array and dictionary objects local to the image being built.
   rootenv = env.get('IMAGE_ENV')
   rootenv[island_libs] = []
   rootenv[island_objs] = []
   rootenv[island_libs_objs] = []
   rootenv[island_sect_dict] = dict()
   rootenv[island_libs_objs_dict] = dict()
   
   # Create a 'libs_objs_list' key in the dictionary.
   rootenv[island_libs_objs_dict]['libs_objs_list'] = []
   rootenv[island_sect_dict]['libs_list'] = []
   rootenv[island_sect_dict]['objs_list'] = []
   
   # Declare an array and dictionary to store whitelist information.
   rootenv[island_whitelist] = []
   rootenv[island_whitelist_dict] = dict()
   
   # Create a 'whitelist_list' key in the dictionary.
   rootenv[island_whitelist_dict]['whitelist_list'] = []
   
   island_generate(env)
   island_linker_generate(env)
   island_whitelist_generate(env)

#============================================================================
# Island Build Rules
#============================================================================
def island_generate(env):

   # Define required methods for accessing global data array.
   env.AddMethod(island_builder_add_library, "AddIslandLibrary")
   env.AddMethod(island_builder_add_object, "AddIslandObject")
   env.AddMethod(island_builder_add_library_object, "AddIslandLibraryObject")
   env.AddMethod(island_builder_generate_list, "GenerateIslandList")

   # Added on the action_source = None in the case that no source files are 
   # provided.
   island_act = env.GetBuilderAction(island_builder,action_source=None)
   island_bld = env.Builder(action = island_act,suffix = '.txt',emitter=island_emitter)

   env.Append(BUILDERS = {'IslandInternalBuilder' : island_bld})
   
#-----------------------------------------------------------------------------
# Function:    island_builder_add_library
# Description: This method takes a list of SCons nodes as input.  These nodes
#              can be library files (*.lib). An optional list of sections can 
#              also be specified.  The method appends the library files to the
#              island_libs list.  If sections are specified, the file name is
#              added as a key in the island_sect_dict dictionary, and the 
#              sections are stored as the values to that key, in an array.
# Inputs:      source (Required) - List of SCons Nodes
#              sections (Optional) - Array of sections, in characters.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_builder_add_library(env, tags, source, sections=[]):
   # Check if the method should be called for this image.
   if env.IsKeyEnable(tags) is False:
      return None
   
   # Load the image environment.
   rootenv = env.get('IMAGE_ENV')
   
   # If the input is not a list, convert to one.
   if type(source) is not list:
      source = [source]
      source = env.Flatten(source)
   if type(sections) is not list:
      sections = [sections]
      sections = env.Flatten(sections)
      
   # Traverse through list of SCons nodes, and append to island_libs if it
   # hasnt't already been added.  We only append if it is new.
   for nodes in source:
      # If the file has never been added, add it to the dictionary.
      if nodes.name not in rootenv[island_sect_dict].keys():
         rootenv[island_sect_dict][nodes.name] = []
         
         # Add the node to the list of libraries.
         rootenv[island_libs].append(nodes)
      
         # Add the filename to the dictionary also.
         rootenv[island_sect_dict]['libs_list'].append(nodes.name)
      
      # If sections are provided append to existing key/value combination
      for items in sections:
         rootenv[island_sect_dict][nodes.name].append(items)
            
   return None

#-----------------------------------------------------------------------------
# Function:    island_builder_add_object
# Description: This method takes a list of SCons nodes as input.  These nodes
#              can be object files (*.o). An optional list of sections can 
#              also be specified.  The method appends the object files to the
#              island_objs list.  If sections are specified, the file name is
#              added as a key in the island_sect_dict dictionary, and the 
#              sections are stored as the values to that key, in an array.
# Inputs:      source (Required) - List of SCons Nodes
#              sections (Optional) - Array of sections, in characters.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_builder_add_object(env, tags, source, sections=[]):
   # Check if the method should be called for this image.
   if env.IsKeyEnable(tags) is False:
      return None
   
   # Load the image environment.
   rootenv = env.get('IMAGE_ENV')
   
   # If the input is not a list, convert to one.
   if type(source) is not list:
      source = [source]
      source = env.Flatten(source)
   if type(sections) is not list:
      sections = [sections]
      sections = env.Flatten(sections)

   # Traverse through list of SCons nodes, and append to island_objs if it
   # hasn't already been added.  We only append if it is new.
   for nodes in source:
      # If the file has never been added, add it to the dictionary.
      if nodes.name not in rootenv[island_sect_dict].keys():
         rootenv[island_sect_dict][nodes.name] = []
      
         # Add the node to the list of objects.
         rootenv[island_objs].append(nodes)
      
         # Add the filename to the dictionary also.
         rootenv[island_sect_dict]['objs_list'].append(nodes.name)
      
      # If sections are provided append to existing key/value combination
      for items in sections:
         rootenv[island_sect_dict][nodes.name].append(items)
   return None

#-----------------------------------------------------------------------------
# Function:    island_builder_add_library_object
# Description: This method takes a list of SCons nodes as input.  These nodes
#              can be library files (*.lib).  This also requires a object file
#              (*.o) to be specified. An optional list of sections can 
#              also be specified.  The method appends the object files to the
#              island_libs_objs list.  If sections are specified, the file name is
#              added as a key in the island_libs_objs_dict dictionary, and the 
#              sections are stored as the values to that key, in an array.
# Inputs:      libs (Required) - Library node.
#              objs (Required) - List of SCons Object Nodes
#              sections (Optional) - Array of sections, in characters.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_builder_add_library_object(env, tags, libs, objs, sections=[]):
   # Check if the method should be called for this image.
   if env.IsKeyEnable(tags) is False:
      return None
   
   # Load the image environment.
   rootenv = env.get('IMAGE_ENV')
   
   # If the input is not a list, convert to one.
   if type(libs) is not list:
      libs = [libs]
      libs = env.Flatten(libs)
   if type(objs) is not list:
      objs = [objs]
      objs = env.Flatten(objs)
   if type(sections) is not list:
      sections = [sections]
      sections = env.Flatten(sections)

   # We only want to parse one library.  This is to avoid confusion.
   if len(libs) > 1:
      env.PrintError("In env.AddIslandLibraryObject(): Only one library node is allowed to be specified.")
      env.PrintError("Libraries Passed:")
      for nodes in libs:
         env.PrintError(nodes)
      exit(1)
   
   # Traverse through list of SCons nodes, and append to island_libs_objs.  Note
   # we are traversing through the libraries provided only, as these are the 
   # physical files that will be generated in the build environment.
   for nodes in libs:
      
      # If the file has never been added, add it to the dictionary.  We only append if it is new.
      if nodes.name not in rootenv[island_libs_objs_dict].keys():
         rootenv[island_libs_objs_dict][nodes.name] = []
      
         # Add the node to the list of libraries.
         rootenv[island_libs_objs].append(nodes)
      
         # Add the filename to the dictionary also.
         rootenv[island_libs_objs_dict]['libs_objs_list'].append(nodes.name)

      # Now, we need to cycle through the objects provided, and the sections.  We do not check
      # for duplicate sections or objects tagged to a library.
      for subnodes in objs:
         # If there are no sections, append a *.
         if not sections:
            rootenv[island_libs_objs_dict][nodes.name].append([subnodes,'*'])
         else:
            for items in sections:
               rootenv[island_libs_objs_dict][nodes.name].append([subnodes,items])
   return None
   
#-----------------------------------------------------------------------------
# Function:    island_builder_generate_list
# Description: This method invokes the IslandInternalBuilder.  It passes the
#              island_libs, island_objs, and island_sect_dict to the builder.
#              It installs the island object list in the build directory of
#              the image being built.
# Inputs:      None.
# Outputs:     SCons object of generated island list.
#-----------------------------------------------------------------------------
def island_builder_generate_list(env):
   # Load the image environment.
   rootenv = env.get('IMAGE_ENV')
   #Invoke the builder action to generate the output file.
   island_list = env.IslandInternalBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}_island", [rootenv[island_libs],rootenv[island_objs],rootenv[island_libs_objs]], relative_path="${BUILD_ROOT}", posix=True, island_dict=rootenv[island_sect_dict], island_libobj_dict=rootenv[island_libs_objs_dict], add_header=True)
   return island_list

#-----------------------------------------------------------------------------
# Function:    island_builder
# Description: This is the builder abstraction.
# Inputs:      None.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_builder(target, source, env):
   list_fname = str(target[0])
   generic_file_create(env, list_fname, source, env.get('relative_path'), env.get('posix', False), env.get('island_dict', []), env.get('island_libobj_dict', []), env.get('add_header', False))


#-----------------------------------------------------------------------------
# Function:    generic_file_create
# Description: This action generates a text file.  Each line contains the 
#              relative path to the object or library file.  The line is 
#              appended with either the section that was specified by
#              the developer, or an asterick which indicates that all
#              functions within that object file are to be included.
# Inputs:      list_fname - The filename of the output list file.
#              file_list - A list of SCons objects.
#              rel_path_target - The relative directory where this file
#                                should be generated to.
#              posix - Path encoding
#              island_dict - Dictionary of all sections.
# Outputs:     
#-----------------------------------------------------------------------------
def generic_file_create(env, list_fname, file_list, rel_path_target, posix=False, island_dict={}, island_libobj_dict={}, add_header=False):
   # Process header.

   # Open file for writing.  Add optional header.
   list_file = open (list_fname, "w")
   if add_header:
      banner = env.CreateFileBanner(os.path.split(list_fname)[1])
      list_file.write(banner)

   # Convert node objects into strings.
   file_list = [str(fname) for fname in file_list]
   
   # If the filename is present in the dictionary, loop through the array of values and
   # append the section to the end of the line.  If it is not, affix an asterick to the
   # end.  Each line should have one section.
   for fname in file_list:
      # This may occur if a developer accidently uses the same library with both APIs.  If this happens
      # the builder will throw a error and exit.
      if os.path.basename(fname) in island_dict.get('libs_list') and os.path.basename(fname) in island_libobj_dict.get('libs_objs_list'):
         env.PrintError("The island_builder.py Builder has detected that " +
                           fname + " has been declared for both AddIslandLibrary" +
                           " and AddIslandLibraryObject APIs.  Modify the SCons accordingly so it" +
                           " is only declared in one API.")
         exit()
      if os.path.basename(fname) in island_dict.get('libs_list'):
         if not island_dict.get(os.path.basename(fname)):
            list_file.write('*' + env.RelPath(os.path.realpath(fname), rel_path_target, posix) + ' * *\n')
         else:
            for entries in island_dict.get(os.path.basename(fname)):
               list_file.write('*' + env.RelPath(os.path.realpath(fname), rel_path_target, posix) + ' * ' + entries + '\n')
      elif os.path.basename(fname) in island_dict.get('objs_list'):
         if not island_dict.get(os.path.basename(fname)):
            list_file.write('* *' + env.RelPath(os.path.realpath(fname), rel_path_target, posix) + ' *\n')
         else:
            for entries in island_dict.get(os.path.basename(fname)):
               list_file.write('* *' + env.RelPath(os.path.realpath(fname), rel_path_target, posix) + ' ' + entries + '\n')
      elif os.path.basename(fname) in island_libobj_dict.get('libs_objs_list'):
         for entries in island_libobj_dict.get(os.path.basename(fname)):
            list_file.write('*' + env.RelPath(os.path.realpath(fname), rel_path_target, posix) + ' ' + os.path.basename(str(entries[0])) + ' ' + entries[1] + '\n')

   # Close the file to writing.
   list_file.close()

#-----------------------------------------------------------------------------
# Function:    island_emitter
# Description: This function creates a hash value representation of the island
#              dictionary containing all of the sections, and generates a value
#              node from it.  If the dictionary is updated, it will cause the
#              builder to be invoked and the item list to be regenerated.
# Inputs:      None.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_emitter(target, source, env):
   rootenv = env.get('IMAGE_ENV')
   env.Depends(target,env.Value(str(rootenv[island_sect_dict])))
   env.Depends(target,env.Value(str(rootenv[island_libs_objs_dict])))
   return (target, source)   

#============================================================================
# Island Linker Build Rules
#============================================================================
def island_linker_generate(env):
   # Kernel provided linker script generator.
   rootenv = env.get('IMAGE_ENV')
   qurt_script = env.RealPath('${COREBSP_ROOT}/kernel/qurt/scripts/qurt-island-link.py')
   rootenv[qurt_linker_script] = qurt_script

   # Define a method to indirectly invoke the builder.
   env.AddMethod(island_linker_generate_lcs, "GenerateIslandLCSFile")
   
   # Added on the action_source = None in the case that no source files are 
   # provided.  The builder will generate a .lcs file.
   island_link_act = env.GetBuilderAction(island_linker_builder,action_source=None)
   island_link_bld = env.Builder(action = island_link_act,suffix = '.lcs')

   
   # Add this builder to the list of builders.
   env.Append(BUILDERS = {'GenerateIslandLCSInternal' : island_link_bld})


#-----------------------------------------------------------------------------
# Function:    island_linker_generate_lcs
# Description: This method invokes the GenerateIslandLCSInternal.  It passes the
#              linker template path, generated linker output path, the island 
#              specifications, and the kernel script.
#              It installs the generated linker file in the root of the image
#              being build.
# Inputs:      linker_script - The destination location of the generated linker
#              script.
#              template - The linker template file, that is annotated with the 
#              appropriate anchors.
#              island_spec - The island specification file(s), passed as an array.  
#              This is the generated output from the previous call to the 
#              IslandInternalBuilder, or can be passed directly to the builder.  
#              Each line should have three values.  We can pass in multiple files.
#              qurt_script - The kernel script that parses the island specification
#              and the template file.
# Outputs:     island_linker - The autogenerated file.
#-----------------------------------------------------------------------------
def island_linker_generate_lcs(env, linker_script, template, island_spec=[]):
   rootenv = env.get('IMAGE_ENV')
   
   # As this function was moved to lcs_autogen.py, we will throw a warning if this is used.
   if 'USES_LCS_AUTOGEN' in env:
      env.PrintWarning('-------------------------------------------------------------------------')
      env.PrintWarning('lcs_autogen.py is detected in the environment.  Please move the call from')
      env.PrintWarning('GenerateIslandLCSFile to GenerateLCSFileFromTemplate to ensure the code')
      env.PrintWarning('is up-to-date.')
      env.PrintWarning('-------------------------------------------------------------------------')
   
   island_linker = env.GenerateIslandLCSInternal(linker_script, [template, rootenv[qurt_linker_script], island_spec])
   return island_linker

#-----------------------------------------------------------------------------
# Function:    island_linker_builder
# Description: This is the builder abstraction.
# Inputs:      None.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_linker_builder(target, source, env):
   # Declare an array to store the Island Specification Files.
   ispecs = []
   # This is the target file.
   linker_script = str(target[0])
   # This is the source template file.
   linker_script_template = str(source[0])
   # This is the Kernel Script to parse the Island Specification files.
   qurt_script = str(source[1])
   # The remainder of the sources are Island Specification Files.  Save them
   # as their own array.
   for nodes in source[2:len(source)]:
      ispecs += [str(nodes)]
   
   # Generate the LCS file.
   generate_lcs_file(env, linker_script, linker_script_template, qurt_script, ispecs)

   
#-----------------------------------------------------------------------------
# Function:    generate_lcs_file
# Description: This builder will generate the linker file from the template.
#              It installs the generated linker file in the root of the image
#              being build.
# Inputs:      linker_script_template - The linker template file, that is annotated with the 
#              appropriate anchors.
#              ispecs - The island specification file(s) that are potentially saved as an array.  
#              This is the generated output from the previous calls to the IslandInternalBuilder.  
#              Each line should have three values.
#              qurt_script - The kernel script that parses the island specification
#              and the template file.
# Outputs:     linker_script - The autogenerated file.
#-----------------------------------------------------------------------------
def generate_lcs_file(env, linker_script, linker_script_template, qurt_script, ispecs):
   import imp
   # Attempt to load the QuRT linker script generator.
   try:
      qurt_island = imp.load_source('qurtisland', qurt_script)
   except:
      print "Failed to load the kernel linker script generator."
      raise Exception()
   # Call the QuRT supplied linker script generator.  Keep in mind 'ispecs' can potentially
   # be an array!
   qurt_island.create_lcs(linker_script, linker_script_template, None, *ispecs)

#============================================================================
# Island Whitelist Build Rules
#============================================================================

def island_whitelist_generate(env):
   # Define required methods for accessing global data array.
   env.AddMethod(island_whitelist_builder_add_symbol, "AddSymbolToIslandWhitelist")
   env.AddMethod(island_whitelist_builder_create_whitelist, "CreateIslandWhitelist")

   # Added on the action_source = None in the case that no source files are 
   # provided.
   island_whitelist_act = env.GetBuilderAction(island_whitelist_builder,action_source=None)
   island_whitelist_bld = env.Builder(action = island_whitelist_act,suffix = '.txt',emitter=island_whitelist_emitter)

   env.Append(BUILDERS = {'IslandWhitelistInternalBuilder' : island_whitelist_bld})

#-----------------------------------------------------------------------------
# Function:    island_whitelist_builder_add_symbol
# Description: This method takes a list of SCons nodes as input.  These nodes
#              can be library files (*.lib) or object files (*.o). The symbols
#              to be added to the whitelist are also provided in an array.  This 
#              method will append the library files to the island_whitelist list.
#              The symbols will be added as a value to the dictionary, where the key
#              is the name of the library or object.
# Inputs:      source (Required) - List of SCons Nodes
#              sections Required) - Array of sections, of type str.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_whitelist_builder_add_symbol(env, tags, source, symbols=[]):
   # Check if the method should be called for this image.
   if env.IsKeyEnable(tags) is False:
      return None
   
   # If the symbols array is empty, throw an exception.
   if not symbols:
      env.PrintError("No symbols have been provided as an argument to island_builder.py,")
      env.PrintError("through the AddSymbolToIslandWhitelist API.  Please validate the")
      env.PrintError("syntax used.")
      exit()
   
   # Load the image environment.
   rootenv = env.get('IMAGE_ENV')
   
   # If the input is not a list, convert to one.
   if type(source) is not list:
      source = [source]
      source = env.Flatten(source)
   if type(symbols) is not list:
      symbols = [symbols]
      symbols = env.Flatten(symbols)
   
   # Traverse through list of SCons nodes, and append to island_whitelist if it
   # hasnt't already been added.  We only append if it is new.
   for nodes in source:
      # If the file has never been added, add it to the dictionary.
      if nodes.name not in rootenv[island_whitelist_dict].keys():
         rootenv[island_whitelist_dict][nodes.name] = []
         
         # Add the node to the list of libraries.
         rootenv[island_whitelist].append(nodes)
      
         # Add the filename to the dictionary also.
         rootenv[island_whitelist_dict]['whitelist_list'].append(nodes.name)
      
      # Append the provided symbols to existing key/value combination
      for items in symbols:
         rootenv[island_whitelist_dict][nodes.name].append(items)
            
   return None

#-----------------------------------------------------------------------------
# Function:    island_whitelist_builder_create_whitelist
# Description: This method invokes the IslandWhitelistInternalBuilder.  It passes the
#              island_whitelist, and island_whitelist_dict to the builder.
#              It installs the whitelist in the build directory of
#              the image being built.
# Inputs:      None.
# Outputs:     SCons object of generated island list.
#-----------------------------------------------------------------------------
def island_whitelist_builder_create_whitelist(env):
   # Load the image environment.
   rootenv = env.get('IMAGE_ENV')
   #Invoke the builder action to generate the output file.
   island_list = env.IslandWhitelistInternalBuilder("${SHORT_BUILDPATH}/${TARGET_NAME}_island_whitelist", rootenv[island_whitelist], relative_path="${BUILD_ROOT}", posix=True, island_whitelist_dict=rootenv[island_whitelist_dict], add_header=True)
   return island_list

#-----------------------------------------------------------------------------
# Function:    island_whitelist_builder
# Description: This is the builder abstraction.
# Inputs:      None.
# Outputs:     None.
#-----------------------------------------------------------------------------
def island_whitelist_builder(target, source, env):
   list_fname = str(target[0])
   generic_whitelist_create(env, list_fname, source, env.get('relative_path'), env.get('posix', False), env.get('island_whitelist_dict', []), env.get('add_header', False))

#-----------------------------------------------------------------------------
# Function:    generic_whitelist_create
# Description: This action generates a text file.  Each line contains the 
#              relative path to the object or library file.  The line is 
#              appended with either the section that was specified by
#              the developer, or an asterick which indicates that all
#              functions within that object file are to be included.
# Inputs:      list_fname - The filename of the output list file.
#              file_list - A list of SCons objects.
#              rel_path_target - The relative directory where this file
#                                should be generated to.
#              posix - Path encoding
#              island_dict - Dictionary of all sections.
# Outputs:     
#-----------------------------------------------------------------------------
def generic_whitelist_create(env, list_fname, file_list, rel_path_target, posix=False, island_whitelist_dict={}, add_header=False):
   # Process header.

   # Open file for writing.  Add optional header.
   list_file = open (list_fname, "w")
   if add_header:
      banner = env.CreateFileBanner(os.path.split(list_fname)[1])
      list_file.write(banner)
      
   # Convert node objects into strings.
   file_list = [str(fname) for fname in file_list]
   env.PrintInfo(str(file_list))
   
   for fname in file_list:
      for entries in island_whitelist_dict.get(os.path.basename(fname)):
         list_file.write(entries + '\n')
   
   # Close the file to writing.
   list_file.close()
   
#-----------------------------------------------------------------------------
# Function:    island_whitelist_emitter
# Description: This function creates a hash value representation of the island
#              whitelist dictionary containing all of the sections, and generates 
#              a value node from it.  If the dictionary is updated, it will cause the
#              builder to be invoked and the item list to be regenerated.
# Inputs:      None.
# Outputs:     None.
#-----------------------------------------------------------------------------

def island_whitelist_emitter(target, source, env):
   rootenv = env.get('IMAGE_ENV')
   env.Depends(target,env.Value(str(rootenv[island_whitelist_dict])))
   return (target, source)   

