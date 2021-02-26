#===============================================================================
#
# Configuration Data
#
# GENERAL DESCRIPTION
#    Database for .builds file.
# 
#    This file contains code for parsing the .builds file into a database
#    and genenerating the cust<bid>.h and targ<bid>.h header files.
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/tcfg/tcfgparser.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/15/09   dlb'     Created from CoreBSP config.py script.
#
#===============================================================================

import os
import shlex
from glob import glob
from datetime import date

#------------------------------------------------------------------------
# Class Printable
# 
# This class is designed to be a parent class for other classes, allowing
# the print function to print out their contents.
#------------------------------------------------------------------------
class Printable:

   #------------------------------------------------------------------
   # DumpObjToStr
   #
   # Description:
   #   Returns a human readable, almost executable, string representation
   #   of an object.
   # 
   # Parameters:
   #   obj: The object to be printed
   #   pad: An optional padding to use at the beginning of each
   #        line for indentation.  Used for increasing the indentation
   #        for nested objects.
   # 
   # Return Value:
   #   String containing formatted contents of the object.
   #------------------------------------------------------------------
   def DumpObjToStr (self, obj, pad=0):
      result = ''
      indent = "   "
      small_types = [int, str, bool]
      ty = type(obj)
      if ty == str:
         result += pad*indent + "'" + obj + "'"
      elif ty in small_types:
         result += pad*indent + str(obj)
      elif ty == dict:
         result += pad*indent + "{\n"
         need_a_comma = False
         for key in obj.keys():
            if need_a_comma:
               result += ',\n'
            need_a_comma = True
            result += self.DumpObjToStr (key, pad+1)
            ty2 = type(obj[key])
            if ty2 in small_types:
               result += ' : ' + self.DumpObjToStr (obj[key])
            else:
               result += ':\n' + self.DumpObjToStr (obj[key], pad+1)
         result += '\n' + pad*indent + "}"
      elif ty in [list, set]:
         result += pad*indent + "[\n"
         need_a_comma = False
         for elem in obj:
            if need_a_comma:
               result += ',\n'
            need_a_comma = True
            ty2 = type(elem)
            result += self.DumpObjToStr (elem, pad+1)
         result += '\n' + pad*indent + "]"
      else:
         # For now, assume this is a class object
         result += pad*indent + "{\n"
         need_a_comma = False
         # The vars() function returns a list of object members
         fields = vars(obj)
         for field in fields:
            if need_a_comma:
               result += ',\n'
            need_a_comma = True
            value = fields[field]
            result += (pad+1)*indent + field
            ty2 = type(value)
            if ty2 in small_types:
               result += ' : ' + self.DumpObjToStr (value)
            else:
               result += ':\n' + self.DumpObjToStr (value, pad+1)
         result += '\n' + pad*indent + "}"
   # Comment this out until we can figure out how to identify class objects
   #   else:
   #      result += pad*indent + "Unknown Type: " + str(ty)

      return result

   #------------------------------------------------------------------
   # __str__
   #
   # Description:
   #   Called by the standard Python print function (and some other
   #   standard Python operations) to convert an object to string
   #   format.
   # 
   # Parameters:
   #   self: self reference
   # 
   # Return Value:
   #   String containing formatted contents of the object.
   #------------------------------------------------------------------
   def __str__ (self):
      result = self.DumpObjToStr(self)
      return result

#-----------------------------------------------------------------------
# Class TargetBidConfigDb
# 
# This class is used to return Build-ID-specific data to the caller.
# It does not include all of the data needed to generate the cust/targ.h
# header files, just some key values, such as the image, and a list of
# USES_* flags
#-----------------------------------------------------------------------
class TargetBidConfigDb(Printable):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables.
   # 
   # Parameters:
   #   self:     self reference
   #   build_id: Build ID
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__(self, build_id):
      self.build_id = build_id
      self.image = ""
      self.uses_flags = list()

#-----------------------------------------------------------------------
# Class TargetConfiguratorDb
# 
# This class defines the main interface to this module, and is responsible for
# parsing the .builds file, generating the cust<bid>.h and targ<bid>.h
# header files, and generating a list of USES_* flags for the rest
# of the build system.
# 
# The methods that comprise the interface for this class are as follows:
# 
#    Boolean ParseBuildFile (build_file_name)
#    List    GenHeaderFiles (build_id)
# 
# The remaining methods in this class are internal.
#-----------------------------------------------------------------------
class TargetConfigurationDb(Printable):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables.
   # 
   # Parameters:
   #   self: self reference
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__(self):

      # Initialize class variables
      self.BuilFilePath = ""
      self.Asic = ""
      self.GlobalDefines = dict()
      self.TagMap = TagMap()
      self.BuildElementTypes = dict()
      self.BuildElementTypes['hardcomponent'] = BuildElementType('h')
      self.BuildElementTypes['softcomponent'] = BuildElementType('s')
      self.AllBuildElements = dict()


   #--------------------------------------------------------------------
   # ParseBuildFile
   # 
   # This method is responsible for parsing the .builds file.
   # 
   # Parameters:
   #   self:            self reference
   #   build_file_name: String name of .builds file.
   #   search_paths:    A list of strings containing search paths acceptable to glob.
   # 
   # Returns:
   #   True if .builds file is successfully parsed.  False otherwise.
   #--------------------------------------------------------------------
   def ParseBuildFile (self, build_file_name, search_paths = None):

      try:
         # Save the path to the .builds file.  We'll use it to
         # locate the cust/targ header files.
         self.BuildFilePath = os.path.dirname(build_file_name)

         # Read and parse the .builds file
         try:
            build_file = Tokenizer(open(build_file_name, 'r'))
         except:
            ReportError (None, "Cannot open " + build_file_name)
   
         token = build_file.get_token()
         while token != build_file.eof:
   
            if token == 'define':
               key = build_file.get_token(preserve_case=True)
               val = build_file.get_multi_token('\n', preserve_case=True)
               self.GlobalDefines[key] = val
   
            elif token == '[':
               newbe = BuildElement(build_file, self.GlobalDefines)
               try:
                  betype = self.BuildElementTypes[newbe.type]
               except:
                  ReportError(build_file, "Unknown Build Element Type: " + newbe.type)
                  return False
   
               betype.AddNewElement(newbe)
               self.AllBuildElements[newbe.name] = newbe
   
            elif token == 'include':
               token = build_file.get_token()
               if search_paths:
                  for path in search_paths:
                     matches = glob(path + token.strip('"'))
                     if matches:
                        token = matches[0]
                        break
               hook = build_file.sourcehook(token)
               build_file.push_source(hook[1])
            elif token == 'tag_map':
               self.TagMap.ParseTagMap(build_file)
               # Add new tag names to list of build element types
               for newtag in self.TagMap.Elements:
                  if not newtag.name in self.BuildElementTypes:
                     self.BuildElementTypes[newtag.name] = BuildElementType(newtag.hs_type)
                  else:
                     ReportError(build_file, "Error: Build Element Type" + newtag.name + "defined twice")
   
            elif token == 'asic':
               self.Asic = build_file.get_token()
   
            token = build_file.get_token()
   
         return True
      except:
         import sys, traceback
         print '-'*60
         print "Exception in Target Configurator:"
         traceback.print_exc(file=sys.stdout)
         print '-'*60
         #import pdb; pdb.set_trace()
         return False

   #--------------------------------------------------------------------
   # GenConfigData
   # 
   # This method is responsible for generating the cust<bid>.h and
   # targ<bid>.h header files used by the build, and returning a list
   # of USES_* flags.
   # 
   # First generate lists of defines, includes and undefs for the targ
   # and cust header files.  Then call methods to generate each of these
   # files.
   # 
   # Parameters:
   #   self:     self reference
   #   build_id: String containing the full build ID.
   # 
   # Returns:
   #   TargetBidConfigDb object containing configuration info for the
   #   specified build ID, from the .builds file.
   #--------------------------------------------------------------------
   def GenConfigData (self, build_id):

      try:
         # Fill in the build ID values in the tag map
         self.TagMap.SetBuildId(build_id.lower())
   
         # Create data structure for holding Build-ID-specific data
         bid_config_data = BidConfigData()

         # Extract the Build ID Config Data from the builds file data base
         self.ExtractBidConfigData(bid_config_data)

         # Write the header files
         self.GenHeader ('cust', build_id, bid_config_data)
         self.GenHeader ('targ', build_id, bid_config_data)

         # Collect return data
         return_data = TargetBidConfigDb(build_id)
         return_data.image = bid_config_data.image
         return_data.uses_flags = self.CollectUsesFlags(bid_config_data)

         return return_data

      except:
         import sys, traceback
         print '-'*60
         print "Exception in Target Configurator:"
         traceback.print_exc(file=sys.stdout)
         print '-'*60
         #import pdb; pdb.set_trace()
         return None


   #--------------------------------------------------------------------
   # ExtractBidConfigData
   # 
   # Local method called to extract info from the .builds file for the
   # specified build ID.
   # 
   # Parameters:
   #   self:            self reference
   #   bid_config_data: Object used to contain the info from the .builds file.
   # 
   # Returns:
   #   No return value.
   #--------------------------------------------------------------------
   def ExtractBidConfigData (self, bid_config_data):

      #------------------------------#
      # First pass to find the image #
      #------------------------------#

      # Scan through top-level build elements pointed to by tag map
      # and take the last 'image' attribute we find.
      #
      # We can't scan through lower-level build elements because the
      # 'use' and 'unuse' statements can take an image conditional,
      # and we don't know what image we're building yet.
      bid_string = ''
      bid_error = False
      for tag in self.TagMap.Elements:
         BEType = self.BuildElementTypes[tag.name]
         if tag.bid_val in BEType.Elements:
            bid_string += tag.bid_val
            BE = BEType.Elements[tag.bid_val]
            if BE.images != []:
               last_image_found = BE.images[-1]
         else:
            bid_error = True
            bid_string += tag.bid_val.upper()

      if bid_error:
         raise RuntimeError, "Invalid Build ID (upper-case letters are in error): " + bid_string

      # Save the last image found
      bid_config_data.image = last_image_found.value
      bid_config_data.images_allowed.append(last_image_found.value)

      #-------------------------------------------#
      # Second pass to find the unuses attributes #
      #-------------------------------------------#

      for tag in self.TagMap.Elements:
         BEType = self.BuildElementTypes[tag.name]
         BE     = BEType.Elements[tag.bid_val]
         self.RecurseBEsForUnusesAttrs(BE, bid_config_data)

      #------------------------------------#
      # Third pass to find everything else #
      #------------------------------------#

      for tag in self.TagMap.Elements:
         BEType = self.BuildElementTypes[tag.name]
         BE     = BEType.Elements[tag.bid_val]
         self.RecurseBEsForConfigData(BE, bid_config_data)

   #-------------------------------------------------------------------
   # Method RecurseBEsForUnusesAttrs
   # 
   # This method looks at the specified build element and populates
   # the bid_config_data.unuses field.
   # 
   # Parameters:
   #   self:            self reference
   #   curbe:           Reference to current build element.
   #   bid_config_data: Object used to contain the info from the .builds file.
   # 
   # Returns:
   #   No return value.
   #-------------------------------------------------------------------
   def RecurseBEsForUnusesAttrs (self, curbe, bid_config_data):
      # Get the 'unuses' attributes for this Build Element
      for attr in curbe.unuses:
         if attr.image in bid_config_data.images_allowed:
            bid_config_data.unuses.add(attr.value)

      # Now recurse through the 'use' BEs.
      for attr in curbe.attributes:
         if (attr.type == 'use' and
             attr.image in bid_config_data.images_allowed and
             attr.value not in bid_config_data.unuses and
             attr.value in self.AllBuildElements):
            BE = self.AllBuildElements[attr.value]
            self.RecurseBEsForUnusesAttrs(BE, bid_config_data)
         if attr.value in bid_config_data.unuses:
            # I'm not sure if this could be a problem, but I want to know
            # about it if it happens.
            ReportWarning(None, "Build Element " + attr.value + " used after unused")


   #-------------------------------------------------------------------
   # RecurseBEsForConfigData
   # 
   # Local method recurses through the build element tree collecting
   # info from the various statements.
   # 
   # Parameters:
   #   self:            self reference
   #   curbe:           Reference to current build element.
   #   bid_config_data: Object used to contain the info from the .builds file.
   # 
   # Returns:
   #   No return value.
   #-------------------------------------------------------------------
   def RecurseBEsForConfigData (self, curbe, bid_config_data):

      if curbe.hs_type == 'h':
         AttrLists = bid_config_data.hattrs
      else:
         AttrLists = bid_config_data.sattrs

      for attr in curbe.attributes:
         if attr.image in bid_config_data.images_allowed:
            if attr.type == 'define':
               AttrLists.defines.append(attr)
            elif attr.type == 'undef':
               AttrLists.undefs.append(attr)
            elif attr.type == 'undef_first':
               AttrLists.undef_firsts.append(attr)
            elif attr.type == 'module':
               if attr.value in ['custonly', 'make']:
                  AttrLists.includes.append(attr)
               # This is not an elif because modules with the 'make'
               # keyword go in both lists.
               if attr.value in ['make', 'makeonly']:
                  bid_config_data.mod_uses.add(attr)
            elif (attr.type == 'use' and
                  attr.value not in bid_config_data.unuses and
                  attr.value in self.AllBuildElements):
               BE = self.AllBuildElements[attr.value]
               self.RecurseBEsForConfigData(BE, bid_config_data)


   #-------------------------------------------------------------------
   # GenHeader
   # 
   # Generate cust<bid>.h or targ<bid>.h header files.
   # 
   # Parameters:
   #   self:            self reference
   #   type:            String containing prefix for header file, either
   #                    'cust' or 'targ'.
   #   build_id:        String containing the build ID, used to form part of
   #                    the file name for the header file.
   #   bid_config_data: Object used to contain the info from the .builds file.
   # 
   # Returns:
   #   No return value.
   #-------------------------------------------------------------------
   def GenHeader (self, type, build_id, bid_config_data):
      header_file_name = self.BuildFilePath + "/" + type.lower() + build_id.lower() + ".h"
      header_file_guard = type.upper() + build_id.upper() + "_H"
      try:
         header_file = open(header_file_name, 'w')
      except:
         ReportError(None, "Can't open " + header_file_name)

      header_file.write("#ifndef " + header_file_guard + "\n")
      header_file.write("#define " + header_file_guard + "\n")

      header_file.write("""/* ========================================================================
FILE: %s

Copyright (c) 2001-%s by QUALCOMM Incorporated.  All Rights Reserved.        
=========================================================================== */

""" % (type.upper() + build_id.upper(), date.today().year))

      if type == 'cust':
         header_file.write("#ifndef TARG" + build_id.upper() + "_H\n")
         header_file.write('   #include "targ' + build_id.lower() + '.h"\n')
         header_file.write("#endif\n\n")

      if type == 'cust':
         attrs = bid_config_data.sattrs
      else:
         attrs = bid_config_data.hattrs

      # Write defines
      for attr in attrs.defines:
         header_file.write("#define " + attr.name + attr.value + "\n")

      header_file.write("\n")

      # Write undef_firsts
      for attr in attrs.undef_firsts:
         header_file.write("#ifdef " + attr.name + "\n")
         header_file.write("   #undef " + attr.name + "\n")
         header_file.write("#endif\n")

      header_file.write("\n")

      # Write includes
      for attr in attrs.includes:
         header_file.write('#include "cust' + attr.name + '.h"\n')

      header_file.write("\n")

      # Write undefs
      for attr in attrs.undefs:
         header_file.write("#ifdef " + attr.name + "\n")
         header_file.write("   #undef " + attr.name + "\n")
         header_file.write("#endif\n")

      header_file.write("\n")

      # Ship it!
      header_file.write("#endif /* " + header_file_guard + " */\n")
      header_file.close()

   #-------------------------------------------------------------------
   # CollectUsesFlags
   # 
   # Local method to create a list of USES_* flags for the build id to
   # return to the caller.
   # 
   # Parameters:
   #   self:            self reference
   #   bid_config_data: Object used to contain the info from the .builds file.
   # 
   # Returns:
   #   List of strings containing the names of the USES_* flags.
   #-------------------------------------------------------------------
   def CollectUsesFlags (self, bid_config_data):
      uses_flags = []
      for attr in bid_config_data.mod_uses:
         uses_flags.append("USES_" + attr.name.upper())
      return uses_flags


#-----------------------------------------------------------------------
# Class TagMap
# 
# This class defines an object that can contain info from the tag map
# from the .builds file.  The tag map maps each build ID letter to a
# string that is used as the type for the build elements for that build
# ID letter.
#-----------------------------------------------------------------------
class TagMap(Printable):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables.
   # 
   # Parameters:
   #   self: self reference
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self):
      self.Elements = list()
      TagElement.cur_bid_pos = 0

   #-------------------------------------------------------------------
   # ParseTagMap
   # 
   # Parse the information in the tag map, and store the info.
   # 
   # This method is called during the parsing of the .builds file.
   # 
   # Parameters:
   #   self:       self reference
   #   build_file: A tokenizer object that can return tokens from the
   #               .builds file.
   # 
   # Returns:
   #   No return value.
   #-------------------------------------------------------------------
   def ParseTagMap (self, build_file):

      if build_file.get_token() != '{':
         ReportError(build_file, "Error: missing '{' for tag map")

      token = build_file.get_token()
      while token != '}':

         if token != '\n':
            newtag = TagElement(token, build_file)
            self.Elements.append(newtag)

         token = build_file.get_token()

   #-------------------------------------------------------------------
   # SetBuildId
   # 
   # Apply the build ID to each element type defined in the tag map.
   # 
   # This method is called when extracting build ID info from the database
   # created from the .builds file.
   # 
   # Parameters:
   #   self:     self reference
   #   build_id: A string containing the build ID.
   # 
   # Returns:
   #   No return value.
   #-------------------------------------------------------------------
   def SetBuildId (self, build_id):
      for element in self.Elements:
         element.SetBid(build_id)


#-----------------------------------------------------------------------
# Class TagElement
# 
# This class defines individual elements for the Tag Map.
#-----------------------------------------------------------------------
class TagElement(Printable):

   cur_bid_pos = 0

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the statement in the .build file that defines the tag element.
   # 
   # Parameters:
   #   self:       Self reference
   #   token:      First token of tag element.  Was read by caller.
   #   build_file: Token stream from .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, token, build_file):

      # Get tag format
      self.hs_type  = token[0]   # First letter of tag format
      self.start = TagElement.cur_bid_pos  # 0-based index of first letter
      self.end   = self.start+1 # 0-based index of pos after last letter
      self.name  = ""

      # Build ID Value is filled in when working with a specific build ID
      self.bid_val = "?"

      token = build_file.get_token()
      while token != ':':
         if token.isdigit():
            self.end += 1
         token = build_file.get_token()
      TagElement.cur_bid_pos = self.end   # Set to start of next element

      # Get the tag name
      self.name = build_file.get_multi_token(['}', '\n'])

   #------------------------------------------------------------------
   # SetBid
   #
   # Description:
   #   Extracts the build ID letter for this class and saves it.  This
   #   is later used to identify the top level build element for this
   #   tag element.
   # 
   # Parameters:
   #   self:     Self reference
   #   build_id: Complete build ID, from which the relevant letters are taken.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def SetBid (self, build_id):
      self.bid_val = build_id[self.start : self.end]



#-----------------------------------------------------------------------
# Class BuildElementType
# 
# This class defines the build element types that are supported in the
# .builds file.  The default types are HardComponent and SoftComponent.
# Additional types are defined by the tag map.
#-----------------------------------------------------------------------
class BuildElementType(Printable):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables.
   # 
   # Parameters:
   #   self:    Self reference
   #   hs_type: 's' for software type, 'h' for hardware type.  This
   #            parameter determines whether symbols from build elements
   #            of this type go into cust<bid>.h or targ<bid>.h.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, hs_type):
      self.hs_type  = hs_type   # Hardware/Software type
      self.Elements = dict()

   #------------------------------------------------------------------
   # AddNewElement
   #
   # Description:
   #   BuildElementType objects maintain a list of which build elements
   #   are of this type.  This method adds new build elements to this
   #   list.
   # 
   # Parameters:
   #   self:     Self reference
   #   new_elem: New build element to add to the list for this element type.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def AddNewElement (self, new_elem):
      # new_elem should be of type BuildElement
      new_elem.hs_type = self.hs_type
      self.Elements[new_elem.name] = new_elem


#-----------------------------------------------------------------------
# Class BuildElement
# 
# This class defines an individual build element from the .builds file.
#-----------------------------------------------------------------------
class BuildElement(Printable):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the element definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   #   GlobalDefines: A dictionary of symbol/value pairs that were defined
   #                  at the top-level of the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file, GlobalDefines):

      # Initialize Class Variables
      self.type = ""
      self.hs_type = '?'  # Assigned by BuildElementType object.
      self.name = ""
      self.ename = ""
      self.images = list()
      self.unuses = list()

      # All other attributes go in this list because their relative order
      # with respect to the 'uses' statements must be preserved.
      self.attributes = list()  # All other attributes


      # Build Element starts with one of the following:
      #   [type: name]
      #   [type: name, ename]
      # 
      # ('ename' is "Element Name".  Was saved in build.pl, but not
      # sure if it was ever used.
      # 

      # Parse the build element type name
      self.type = build_file.get_multi_token([':'])

      # Throw away the ':'
      build_file.get_token()

      # Parse the build element name
      self.name = build_file.get_token()

      token = build_file.get_token()

      # Not sure if this is used, but build.pl parsed it,
      # so we will too.
      if token == ',':
         self.ename = build_file.get_token()
         token = build_file.get_token()

      # Parse the build element attributes
      # ('[' indicates the start of a new element)
      while token.lower() not in ('end_block', '[', build_file.eof):
         if token.lower() == 'define':
            attr = AttributeDefine(build_file, GlobalDefines)
            self.attributes.append(attr)
         elif token.lower() == 'undef':
            attr = AttributeUndef(build_file)
            self.attributes.append(attr)
         elif token.lower() == 'undef_first':
            attr = AttributeUndefFirst(build_file)
            self.attributes.append(attr)
         # build.pl also parses 'redef' and 'condef', but I didn't see
         # any examples of this in the 7x30 .builds file, so I'm
         # not sure if they're used.  If so, they'll have to be added here.
         elif token.lower() == 'use':
            attr = AttributeUse(build_file)
            self.attributes.append(attr)
         elif token.lower() == 'unuse':
            attr = AttributeUnuse(build_file)
            self.unuses.append(attr)
         elif token.lower() == 'image':
            attr = AttributeImage(build_file)
            self.images.append(attr)
         elif build_file.IsName(token):
            # This block handles the custxxx.h/USES_XXX cases,
            # which have no keyword at the start of the line.

            # Need to push this symbol back so that
            # the attribute initializer can read it.
            build_file.push_token(token)
            attr = AttributeModule(build_file)
            self.attributes.append(attr)

         # We need to preserve case because we'll sometimes push
         # this back onto the stream (e.g. for module statements)
         token = build_file.get_token(preserve_case = True)

      # Push the '[' back on the stack for the caller to see.
      if token == '[':
         build_file.push_token(token)
      

#-----------------------------------------------------------------------
# Class AttributeParent
# 
# This class defines a base class that is used by all attributes.  Each
# attribute represents one statement from a build element from the .builds
# file.
# 
# This base class defines the common parameters used by each class, allowing
# them to be processed similarly.  It also provides a common method for
# parsing the "if apps_proc" or "if modem_proc" that can used to identify a
# particular attribute statement as only applying to either the APPS Proc or
# the Modem Proc.
#-----------------------------------------------------------------------
class AttributeParent(Printable):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables.
   # 
   # Parameters:
   #   self:          Self reference
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self):

      # Initialize class variables
      self.type = ""
      self.name = ""
      self.value = ""
      self.image = "all"

   #------------------------------------------------------------------
   # ParseIfImage
   #
   # Description:
   #   parsing the "if apps_proc" or "if modem_proc" that can used to identify a
   #   particular attribute statement as only applying to either the APPS Proc or
   #   the Modem Proc.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def ParseIfImage (self, build_file):
      token = build_file.get_token()
      if token == 'if':
         self.image = build_file.get_token()
      else:
         self.image = "all"
         build_file.push_token(token)
      # Eat everything to the end of the current line
      throw_away = build_file.get_multi_token(['\n'])

#-----------------------------------------------------------------------
# Class AttributeDefine
# 
# A sub-class of AttributeParent that represents the 'define' statements
# from the .builds file.
#-----------------------------------------------------------------------
class AttributeDefine(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the 'define' attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   #   GlobalDefines: A dictionary of symbol/value pairs that were defined
   #                  at the top-level of the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file, GlobalDefines):
      AttributeParent.__init__(self)
      self.type = 'define'
      # Symbols are case-sensitive in 'C'
      self.name = build_file.get_token(preserve_case = True)
      self.value = build_file.get_multi_token(['if', '\n'],
                                              preserve_case = True,
                                              see_whitespace = True,
                                              expand_dict = GlobalDefines)
      AttributeParent.ParseIfImage(self, build_file)

#-----------------------------------------------------------------------
# Class AttributeUndef
# 
# A sub-class of AttributeParent that represents the 'undef' statements
# from the .builds file.
#-----------------------------------------------------------------------
class AttributeUndef(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the 'undef' attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file):
      AttributeParent.__init__(self)
      self.type = 'undef'
      # Symbols are case-sensitive in 'C'
      self.name = build_file.get_token(preserve_case = True)
      AttributeParent.ParseIfImage(self, build_file)

#-----------------------------------------------------------------------
# Class AttributeUndefFirst
# 
# A sub-class of AttributeParent that represents the 'undef_first' statements
# from the .builds file.
#-----------------------------------------------------------------------
class AttributeUndefFirst(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the 'undef_first' attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file):
      AttributeParent.__init__(self)
      self.type = 'undef_first'
      # Symbols are case-sensitive in 'C'
      self.name = build_file.get_token(preserve_case = True)
      AttributeParent.ParseIfImage(self, build_file)

#-----------------------------------------------------------------------
# Class AttributeUse
# 
# A sub-class of AttributeParent that represents the 'use' statements
# from the .builds file.
#-----------------------------------------------------------------------
class AttributeUse(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the 'use' attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file):
      AttributeParent.__init__(self)
      self.type = 'use'
      self.value = build_file.get_token()
      AttributeParent.ParseIfImage(self, build_file)

#-----------------------------------------------------------------------
# Class AttributeUnuse
# 
# A sub-class of AttributeParent that represents the 'unuse' statements
# from the .builds file.
#-----------------------------------------------------------------------
class AttributeUnuse(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the 'unuse' attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file):
      AttributeParent.__init__(self)
      self.type = 'unuse'
      self.value = build_file.get_token()
      AttributeParent.ParseIfImage(self, build_file)

#-----------------------------------------------------------------------
# Class AttributeImage
# 
# A sub-class of AttributeParent that represents the 'image' statements
# from the .builds file.
#-----------------------------------------------------------------------
class AttributeImage(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the 'image' attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file):
      AttributeParent.__init__(self)
      self.type = 'image'
      self.value = build_file.get_token()

#-----------------------------------------------------------------------
# Class AttributeModule
# 
# A sub-class of AttributeParent that represents the module statements
# from the .builds file.  Module statements don't have a keyword identifying
# them, but they are the statements that identify which cust<module>.h files
# need to be included, and/or which USES_<module> flags need to be defined.
# The 'make' and 'makeonly' keywords are used to make this distinction.
#-----------------------------------------------------------------------
class AttributeModule(AttributeParent):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for class.  Initializes member variables and parses
   #   the module attribute definition in the .builds file.
   # 
   # Parameters:
   #   self:          Self reference
   #   build_file:    A token stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, build_file):
      AttributeParent.__init__(self)
      self.type = 'module'
      # Preserve case for header file names for Linux
      self.name = build_file.get_token(preserve_case = True)
      token = build_file.get_token()
      if token == 'make' or token == 'makeonly':
         self.value = token
      else:
         self.value = 'custonly'
         build_file.push_token(token)
      AttributeParent.ParseIfImage(self, build_file)

#---------------------------------------------------------------------
# Class BidConfigData
# 
# This class serves as a storage place for info gathered while extracting
# configuration data for a particular build ID from the main configuration
# database.
#---------------------------------------------------------------------
class BidConfigData(Printable):

   #---------------------------------------------------------------------
   # Class BidAttrLists
   # 
   # This class serves as a storage place for info gathered while extracting
   # configuration data for a particular build ID from the main configuration
   # database.
   #---------------------------------------------------------------------
   class BidAttrLists:

      #------------------------------------------------------------------
      # __init__
      #
      # Description:
      #   Constructor for BidAttrLists class.  Initializes member variables.
      # 
      # Parameters:
      #   self:          Self reference
      # 
      # Return Value:
      #   No return value.
      #------------------------------------------------------------------
      def __init__ (self):
         self.defines      = list()   # Hardware define list
         self.undefs       = list()   # Hardware undef list
         self.undef_firsts = list()   # Hardware undef_first list
         self.includes     = list()   # Hardware include list

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for BidConfigData class.  Initializes member variables.
   # 
   # Parameters:
   #   self:          Self reference
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self):

      # First pass info
      self.image = ""
      self.images_allowed = ['all'] # List of names of allowed images

      # Second pass info
      self.unuses = set()  # Set of unuses element names

      # Third pass info
      # Hardware definitions (for targ<bid>.h)
      self.hattrs = self.BidAttrLists()
      # Software definitions (for cust<bid>.h)
      self.sattrs = self.BidAttrLists()
      self.mod_uses = set()

#------------------------------------------------------------------------
# Class Tokenizer
#
# Uses shlex as a parent class, but with the following differences
# 
# - Returns multiple newlines as a single newline
# 
#      By default, shlex doesn't return newlines.  You can configure
#      it to do so, but then you get a bunch of newlines, which we
#      really don't need, so we consolidate multiple consecutive
#      newlines into a single newline.  This makes it easier to step
#      through the callers code in the debugger.
# 
# - Allow '.' and '-' as characters in a name.
# 
#      The .builds allows a '-' in build element names.
# 
#      The '.' was needed for parsing makefile names, but this has
#      since been removed.  Not sure if it's needed anymore, but it
#      doesn't hurt.
# 
# - Do our own comment parsing
# 
#      The default shlex comment parsing is almost great, but we
#      have one small problem when enable returning newlines.  The
#      shlex comment parsing still drops the newline at the end of
#      a comment.
# 
#      For example, in the following line:
# 
#          define T_DLOAD    1  # Download support
# 
#      shlex returns a '1' and then the first token from the following
#      line.  We never see the newline, so we process the next line
#      as if it's a continuation of the current line.
# 
#      We need to modify the comment parsing so that we return the
#      newline at the end.
# 
# - Return all tokens in lower case by default
# 
#      This helps keep the .builds file case-insensitive.  However,
#      we also need to preserve case for 'C' symbols, such as for
#      the define and undef statements, and for filenames (for Linux)
#      such as for the makefile attributes.
# 
# - Expands key words
# 
#      The .builds file contains external (global) define statements
#      that can be referenced in internal (in a build element) define
#      statements.  We need to resolve these at some level.  This is
#      probably more appropriately done in the AttributeDefine init
#      method, but we're doing it here because we want AttributeDefine
#      to leverage the get_multi_token method.
# 
# - Adds the get_multi_token() method
# 
#      The .builds file allows spaces in the name of an element
#      type (e.g. "Application Features").  For this reason, we
#      need a method that will return multiple tokens (e.g.
#      "Application" and "Features") as a single token.  We do
#      this by concatenating all tokens we see before we see
#      one of the terminator tokens.  Each token is separated
#      by a single space during the concatenation process.
# 
#      Since we use the same algorithm when parsing both the
#      element type name in the tag map, and in the header for
#      the build element, we'll still get a match, even if one
#      or the other contains additional whitespace in the name.
# 
# - Adds the IsName() method
# 
#      Since the lines that define modules to use (i.e. the lines
#      that tell us to include a certain cust*.h file, or to define
#      a USES_* flag for the build system) don't have a leading
#      label, we need a way to identify these lines.
# 
#      The lines are basically any line in the element definition
#      that begins with a word that isn't one of the predefined
#      keywords ('define', 'undef', 'use', 'unuse', etc.).
# 
#      Unfortunately, the standard Python 'string'.isalnum() method
#      returns false if the string contains '_' characters.  Therefore,
#      we need to write our own.  Our method will compare only the
#      first character of the token with self.wordchars to see if the
#      first character is there.  If it is, and assuming the string
#      was returned by our get_token method, then the string must
#      be a name by our definition.
#------------------------------------------------------------------------

class Tokenizer(shlex.shlex):

   #------------------------------------------------------------------
   # __init__
   #
   # Description:
   #   Constructor for Tokenizer class.  Initializes member variables.
   # 
   # Parameters:
   #   self:     Self reference
   #   instream: File input stream for the .builds file.
   # 
   # Return Value:
   #   No return value.
   #------------------------------------------------------------------
   def __init__ (self, instream):
      shlex.shlex.__init__(self, instream)
      self.whitespace = ''
      self.my_whitespace = ' \t\r'
      self.wordchars = self.wordchars + '-.'
      self.commenters = ''
      #self.debug = 3

   #------------------------------------------------------------------
   # get_token_skip_comments
   #
   # Description:
   #   This is an internal function, called by get_token, and should
   #   not be called from outside this class.
   # 
   # Parameters:
   #   self:     Self reference
   # 
   # Return Value:
   #   String containing the next non-comment token
   #------------------------------------------------------------------
   def get_token_skip_comments (self):
      token = shlex.shlex.get_token(self)
      #print "First  shlex.shlex.get_token returned <" + token + ">"
      if token == '#':
         while token not in ['\n', self.eof]:
            token = shlex.shlex.get_token(self)
            #print "Second shlex.shlex.get_token returned <" + token + ">"
      return token

   #------------------------------------------------------------------
   # get_token
   #
   # Description:
   #   This method returns the next token in the input stream.
   # 
   # Parameters:
   #   self:           Self reference
   #   preserve_case:  Boolean indicating whether or not to convert the token
   #                   to lower case.  False by default.
   #   expand_dict:    Dictionary of symbol/value pairs from which tokens can be
   #                   translated, e.g. "47MB":"0x2F00000".  This is primarily
   #                   used by the 'define' attributes.  By default, this is
   #                   None, which means no translation is done.
   #   see_whitespace: Boolean indicating whether or not whitespaces should be
   #                   skipped or returned as tokens.  If True, multiple
   #                   sequential whitespaces, of any type, will be returned
   #                   as a single whitespace token (' ').
   # 
   # Return Value:
   #   String containing the next token
   #------------------------------------------------------------------
   def get_token (self, preserve_case=False, expand_dict=None, see_whitespace = False):
      token = self.get_token_skip_comments()
      newline_seen = False
      # Group multiple newlines into one newline
      while token == '\n':
         newline_seen = True
         # Skip all '\n'
         token = self.get_token_skip_comments()
      if newline_seen:
         # Interpret multiple '\n' as a single '\n'
         self.push_token(token)
         return '\n'

      whitespace_seen = False
      while token != self.eof and token in self.my_whitespace:
         whitespace_seen = True
         # Skip all whitespace
         token = self.get_token_skip_comments()
      if whitespace_seen:
         if see_whitespace:
            # Interpret multiple whitespace as a single whitespace
            self.push_token(token)
            return ' '

      # Expansion is done before changing case, because currently
      # expansion is done for defines, and defines need to preserve case.
      if expand_dict is not None:
         while token in expand_dict:
            token = expand_dict[token]
      if not preserve_case:
         token = token.lower()
      return token

   #------------------------------------------------------------------
   # get_multi_token
   #
   # Description:
   #   This method returns all tokens, up to but not including the specified
   #   terminator tokens, as a single token.  All sub-tokens will be separated
   #   by a single whitespace token (' '), regardless of the actual whitespace
   #   characters that they were separated by.
   # 
   # Parameters:
   #   self:           Self reference
   #   terminators:    A list of string tokens that will indicate the end of
   #                   the multi-token sequence.
   #   (The remaining parameters are passed through to get_token() and are
   #   documented for that method.)
   # 
   # Return Value:
   #   String containing the next multi-token sequence.
   #------------------------------------------------------------------
   def get_multi_token (self, terminators, preserve_case=False, expand_dict=None, see_whitespace = False):
      token = self.get_token(preserve_case, expand_dict, see_whitespace)
      result = ""
      add_space = False
      while token not in terminators:
         if add_space and not see_whitespace:
            # Only add a space if we're not getting spaces from the .builds file
            result += ' '
         result += token
         add_space = True
         token = self.get_token(preserve_case, expand_dict, see_whitespace)
      # Push the terminator token back on, so caller will find it.
      self.push_token(token)
      return result

   #------------------------------------------------------------------
   # IsName
   #
   # Description:
   #   This method tests the specified token to see whether or not it is
   #   an alpha-numeric symbol, as opposed to punctuation, whitespace, or EOF.
   # 
   # Parameters:
   #   self:  Self reference
   #   token: The token to test.
   # 
   # Return Value:
   #   True if the token is a symbol, False otherwise.
   #------------------------------------------------------------------
   def IsName (self, token):
      try:
         return token[0] in self.wordchars
      except:
         # Most likely token is empty string.
         return False

#=======================================================================
# Global Functions
#=======================================================================

#------------------------------------------------------------------
# ReportError
#
# Description:
#   Raise a Runtime exception with the specified error message.
# 
#   If the error is detected in the .builds file, include the shlex
#   error_leader string in the error message.  This string includes
#   the file name / line number where the error was detected, along
#   with some other data, in a standard error format that many editors
#   can understand.
# 
# Parameters:
#   build_file:    Token stream for .builds file.
#   error_message: Main text of the error message.
# 
# Return Value:
#   No return value.
#------------------------------------------------------------------
def ReportError (build_file, error_message):
   if build_file:
      error_message = build_file.error_leader() + error_message
   raise RuntimeError, error_message

#------------------------------------------------------------------
# ReportError
#
# Description:
#   Report a warning message to the user.
# 
#   If the error is detected in the .builds file, include the shlex
#   error_leader string in the error message.  This string includes
#   the file name / line number where the error was detected, along
#   with some other data, in a standard error format that many editors
#   can understand.
# 
# Parameters:
#   build_file:      Token stream for .builds file.
#   warning_message: Main text of the warning message.
# 
# Return Value:
#   No return value.
#------------------------------------------------------------------
def ReportWarning (build_file, warning_message):
   if build_file:
      print build_file.error_leader(), "Warning:", warning_message
   else:
      print "Warning:", warning_message

