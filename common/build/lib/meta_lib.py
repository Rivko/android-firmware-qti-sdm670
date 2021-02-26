'''
===============================================================================

 Meta Library

 GENERAL DESCRIPTION
    This script contains code for extracting meta-information from the
    contents.xml file.

 Copyright (c) 2011-2018 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR

-------------------------------------------------------------------------------

 $Header: //components/rel/build.glue/1.0/lib/meta_lib.py#52 $
 $DateTime: 2018/06/11 17:14:13 $
 
===============================================================================
'''
import fnmatch
import sys
import os
import subprocess
from xml.etree import ElementTree as et
from xml.dom import minidom
import re
import shutil
from glob import glob
import meta_log
import attr_evaluation as ae
import time
import datetime

# Fix for minidom prettyprint function from:
#   http://ronrothman.com/public/leftbraned/xml-dom-minidom-toprettyxml-and-silly-whitespace/
# (with some editing to suit out purposes.)
def fixed_writexml(self, writer, indent="", addindent="", newl=""):
   # indent = current indentation
   # addindent = indentation to add to higher levels
   # newl = newline string
   writer.write(indent + "<" + self.tagName)

   attrs = self._get_attributes()
   a_names = attrs.keys()
   # donb: a_names.sort()

   for a_name in a_names:
      writer.write(" %s=\"" % a_name)
      minidom._write_data(writer, attrs[a_name].value)
      writer.write("\"")
   if self.childNodes:
      if len(self.childNodes) == 1 \
         and self.childNodes[0].nodeType == minidom.Node.TEXT_NODE:
         writer.write(">")
         self.childNodes[0].writexml(writer, "", "", "")
         writer.write("</%s>%s" % (self.tagName, newl))
         return
      writer.write(">%s" % (newl))
      for node in self.childNodes:
         if node.nodeType != minidom.Node.TEXT_NODE:
            node.writexml(writer, indent + addindent, addindent, newl)
      writer.write("%s</%s>%s" % (indent, self.tagName, newl))
   else:
      writer.write("/>%s" % (newl))
# replace minidom's function with ours
minidom.Element.writexml = fixed_writexml

class meta_info:

   #----------------------------------------------------------#
   # Public Methods                                           #
   #----------------------------------------------------------#
   # Updating file list for backward compatibility. In future dload_boot_image, dir_ref, device_programmer and flash_programmer will be deprecated.  
   default_file_types = ['file_ref', 'download_file', 'dload_boot_image', 'partition_file', 'partition_patch_file', 'dir_ref', 'device_programmer', 'flash_programmer']

   def __init__ (self, logger=None, file_pfn=None, fb_nearest=False):
      '''Initialize by reading from the meta_file.'''

      # Set the path for contents.xml
      meta_file_pfn = ''
      root_dir = os.path.join(os.path.dirname(__file__), '../../../')
      if not file_pfn:
         meta_file_pfn = os.path.join(root_dir , 'contents.xml')
      else:
         meta_file_pfn = file_pfn
      
      # Set path for contents_oem.xml
      path_oem_file = os.getenv('SCONS_BUILD_SCRIPTS_OEM_ROOT')
      if not path_oem_file:
         if os.path.exists(os.path.join(os.path.dirname(__file__), '../../../contents_oem.xml')):
            path_oem_file = os.path.join(os.path.dirname(__file__), '../../../')  # if oem variable is not se, looking for contents_oem.xml in root directory
      self.meta_info_pfn = os.path.abspath(meta_file_pfn)  # 'pfn' => path/file-name
      self.meta_info_fn = os.path.basename(meta_file_pfn)  # 'fn' => file name
      self.logger = logger
      self.meta_oem_file_pfn = ''  # path to OEM customer's contents_oem.xml, "necessary" to initialize it to empty string

      # Initialize the XML tree
      if os.path.exists(self.meta_info_pfn):
         self.XmlTree = et.ElementTree(file=self.meta_info_pfn)
         self.XmlRoot = self.XmlTree.getroot()

         if path_oem_file:  # check if the environment variable SCONS_BUILD_SCRIPTS_OEM_ROOT is defined ( usually it is only defined in OEM customer's environment )
            self.log('SCONS_BUILD_SCRIPTS_OEM_ROOT is defined as: ' + path_oem_file)
            self.meta_oem_file_pfn = os.path.abspath(os.path.join(path_oem_file , 'contents_oem.xml'))
         
            if os.path.exists(self.meta_oem_file_pfn):  # check if contents_oem.xml exists at the path specified by the system variable SCONS_BUILD_SCRIPTS_OEM_ROOT
               self.log('contents_oem.xml found at path: ' + self.meta_oem_file_pfn)
               self.OemXmlTree = et.ElementTree(file=self.meta_oem_file_pfn)
               self.OemXmlRoot = self.OemXmlTree.getroot()
               
               # check if OEM merge is already done, if not, merge the OEM flavor information
               oem_merge_already_done = self.__is_oem_merge_alreadydone()
               if not oem_merge_already_done:
                  self.log('Performing OEM flavors merge...')
                  self.__merge_oem()
                  self.save(os.path.join(root_dir , 'contents.xml'))
                  self.log('contents.xml saved with OEM customer information')
               else:
                  self.log('OEM flavors are already merged with contents.xml...')
      else:
         # Generate default structure
         self.XmlRoot = et.Element('contents')
         self.XmlTree = et.ElementTree(element=self.XmlRoot)
         et.SubElement(self.XmlRoot, 'builds_flat')

      # Set platform flag
      self.on_linux = sys.platform.startswith("linux")
      
      # Build an internal flavor dictionary for easier access
      self.flavor_data = self.__build_flavor_data()
      self.__do_integrity_check()

      # Define variables for findbuild object
      self.fb = None
      self.FINDABLE = True
      self.fb_nearest = fb_nearest  
      
   def save (self , fName=None):

      # Do stuff here that we only want to do once we know that the
      # contents data is fully updated.
      self.flavor_data = self.__build_flavor_data()

      # -- This method doesn't insert whitespace for readability
      # self.XmlTree.write(self.meta_info_pfn)
      # -- Minidom pretty printing is inserting white space in the strings,
      # -- so commenting it out for now.
      txt = et.tostring(self.XmlRoot)
      txt = minidom.parseString(txt).toprettyxml(indent='  ')
#      txt  = '<?xml version="1.0" encoding="utf-8"?>\n'
#      txt += XmlPrettyPrint(self.XmlRoot)
      
      if not fName:
         fName = self.meta_info_pfn

      meta_file = open(fName, 'w')
      meta_file.write(txt)
      meta_file.close()

   def remove_files (self, rm_list):
      # Get default file types from definition
      file_type_list = self.default_file_types
      # Start with builds_flat element
      builds_flat = self.XmlRoot.find('builds_flat')
      # Loop through each build
      for build in builds_flat.getchildren():
         for ftl in file_type_list:
            # Find all elements of each type
            file_types = build.findall(ftl)
            for file_type in file_types:
               file_name_elem = file_type.find('file_name')
               # Get the file name string
               if file_name_elem.text:
                  file_name = file_name_elem.text.strip()
                  for rm_str in rm_list:
                     # Remove element if file name string is present in rm_list
                     if fnmatch.fnmatch(file_name, rm_str):
                        self.log('File type element was removed for ' + file_name)
                        build.remove(file_type)
      self.save(self.meta_info_pfn)
   
   def remove_elements_with_attrs (self, attr_string, root_elem=None, elem_types=None):
      """
      Remove xml <element>s of elem_types with attributes matching attr_string
      under given <root_elem>
      """
      # Defaults
      if root_elem == None:
         root_elem = self.XmlRoot
      
      attr_tree = ae.parse_attr_string(attr_string)
      child_elems = list(root_elem)
      for each_child_elem in child_elems:
         # Process only the provided elem_types
         # If None, process all types
         if (elem_types is None) or (each_child_elem.tag in elem_types):
            # Evaluate attribute string
            rem_check = ae.eval(attr_tree, each_child_elem)
            if rem_check:
               # Remove the element (along with children)
               self.log("Removing:")
               self.log(et.tostring(each_child_elem))
               root_elem.remove(each_child_elem)
            else:
               # If check fails, go through child elements recursively
               new_root_elem = each_child_elem
               self.remove_elements_with_attrs (attr_string, new_root_elem, elem_types)
      self.save(self.meta_info_pfn)
   # End of remove_elements_with_attrs

   def clean_file_paths(self, si_root_path_rule=None):
      file_types = self.default_file_types
      builds_flat = self.XmlRoot.find('builds_flat')
      for build in builds_flat.getchildren():
         lnx_path = "./"
         win_path = ".\\"
         # Clean SI build root file path based  
         build_name = build.find('name').text
         if (si_root_path_rule is not None and build_name != 'glue' and build_name != 'common'):
            if (si_root_path_rule == 'spf' or si_root_path_rule == 'chipset_sipl'):
               #si_match = re.search('(.+)-([0-9]{5,9})[.,-]', build.find('build_id').text)
               si_match = re.split('-\d+', build.find('build_id').text, 1)
               if si_match is None: 
                  raise Exception('SPF build package error!')
			   # Found Software Image PL
               si_name = si_match[0]
               #si_name = si_match.group(1)
               # Remove remove .c<nn> or .r<nn> extensions from the SP/SI name.
               cmatch = re.search('(.+).[cr][0-9][0-9]*', si_name)
               if cmatch : 
                  # For example: MPSS.TA.2.5.c1.1 will be translated to MPSS.TA.2.5
                  si_name = cmatch.group(1)
               if si_root_path_rule == 'spf':
                  win_path += '..\\'+ si_name
                  lnx_path += '../'+ si_name
               else:
                  lnx_path += (str(build.find('chipset').text) + '_' + si_name).lower()
                  win_path += (str(build.find('chipset').text) + '_' + si_name).lower()
            elif(si_root_path_rule == 'buildtag'):
               lnx_path += build_name.lower()
               win_path += build_name.lower()
            elif(si_root_path_rule == 'chipset_role'):
               lnx_path += (str(build.find('chipset').text) + '_' + str(build.find('role').text)).lower()
               win_path += (str(build.find('chipset').text) + '_' + str(build.find('role').text)).lower()
            #Add trailing slash explicitly to resolve QPST path issue
            if(si_root_path_rule != 'root'):
               lnx_path += '/'
               win_path += '\\'
         
         build.find('linux_root_path').text = lnx_path
         build.find('windows_root_path').text = win_path
         for file in get_all_files_for_build(build, file_types):
            file.find('file_path').text = clean_vars(file.find('file_path').text.strip())                 
      self.save(self.meta_info_pfn)


   def update_pl_from_buildid(self, build_id):
   # For CHIP-ID population:
   #    *If contents_template.xml is empty , CHIP-ID will be obtained from Findbuild
   #    *If contents_template.xml already has a chip-id , then only clean_vars is done on that element
      self.__init_fb_obj()
      if not self.fb:
         self.log('meta_lib.py: Information for build ' + build_id + ' cannot be found.')
         self.log('meta_lib.py: Skipping PL/CHIP update.')
         return
      
      fbInfo = self.fb.get_fbInfo(build_id)
      try:
         product_info = self.XmlRoot.find('product_info')
         pl = fbInfo['ProductLine']
         product_info.find ('product_name').text = pl
         self.log('meta_lib.py: PL is updated to ' + pl)
      except:
         self.log('meta_lib.py: Unable to update PL info.')
         if self.logger:
            self.logger.log_exception()
      try:
         if (product_info.find ('chipid').text is not None):
            chip_id = product_info.find ('chipid').text
            product_info.find ('chipid').text = clean_vars(chip_id)
            self.log('meta_lib.py: pre-populated Chip ID is updated to ' + clean_vars(chip_id))
         elif (product_info.find ('chipid').text is None):
            chip_id = fbInfo['HWPlatform']
            product_info.find ('chipid').text = chip_id
            self.log('meta_lib.py: Chip ID is updated to ' + chip_id)
      except:
         self.log('meta_lib.py: Unable to update Chip info.')
         if self.logger:
            self.logger.log_exception()
   # end of update_pl_from_buildid()
   
   
   def clean_product_name(self):
      product_info = self.XmlRoot.find('product_info')
      product_name_text = product_info.find('product_name').text
      filtered_product_name = re.search (r'.+(?=\.r\d+)', product_name_text)
      if filtered_product_name:
         product_info.find('product_name').text = filtered_product_name.group()
      else: 
         product_info.find('product_name').text = product_name_text
   # end of clean_product_name()

   def update_build_info (self, tag, loc_list, root_dir=None, build_id=None):
      # "loc" must be a list of paths
      if type(loc_list) is not list:
         loc_list = [loc_list]
         
      win_path = None
      lin_path = None
      # Loop through each path and update corresponding element
      for loc in loc_list:
         # Find build IDs, path info, from pair
         if loc.count('/'):
            # It's a Linux path
            self.log('meta_lib.py: Linux path provided for ' + tag + ' - ' + loc)
            lin_path = os.path.join(loc, '').replace('\\', '/')  # Adjust linux path
            real_lin_path = lin_path
            if root_dir is not None:
               real_lin_path = os.path.join(root_dir, loc)
            if (not os.path.exists(real_lin_path)) and self.on_linux:
               self.log('meta_lib.py: ' + real_lin_path + ' is not accesible in linux environment.')
               self.log('Exiting update_build_info() !')
               raise Exception(real_lin_path + " is not accessible in linux environment.")
         elif loc.count('\\'):
            # It's a Windows path
            self.log('meta_lib.py: Windows path provided for ' + tag + ' - ' + loc)
            win_path = os.path.join(loc, '').replace('/', '\\')  # Adjust windows path
            real_win_path = win_path
            if root_dir is not None:
               real_win_path = os.path.join(root_dir, loc)
            if (not os.path.exists(real_win_path)) and (not self.on_linux):
               self.log('meta_lib.py: ' + real_win_path + ' is not accesible in windows environment')
               self.log('Exiting update_build_info() !')
               raise Exception(real_win_path + " is not accessible in windows environment.")
         else:
            # It's a Build ID
            # Assume we're running in QC environment, and
            # findbuild is present.  Customer environments should only specify
            # paths for this function, not build IDs.
            self.log('meta_lib.py: Build ID provided for ' + tag + ' - ' + loc)
            build_id = loc
            if tag != 'common':
               self.log('meta_lib.py: Getting windows and linux paths for ' + build_id)
               mpath = self.__get_new_path(build_id)
               lin_path = mpath.lin_path
               win_path = mpath.win_path
               if not lin_path and not win_path:
                  return 1  # build id not there
            
      # Update build ID, paths in meta data
      # Read each CRM build from 'flat builds' and add it.
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, tag)
      flat_build.find('build_id').text = build_id
      if (flat_build.find('chipset').text is not None):
         self.log('meta_lib.py: Updating Chipset for  ' + tag)
         flat_build.find('chipset').text = clean_vars(flat_build.find('chipset').text)
      if tag =='apps' or tag =='app':#Upgrade content XML file path if 'Linux/android' path invalid
         if self.on_linux:
            if not lin_path.startswith('./') and not os.path.exists(os.path.join(lin_path,'LINUX/android/out')) and os.path.exists(os.path.join(lin_path,'out')): 
               self.replace_string('LINUX/android/', '')
         else:
            if not win_path.startswith('.\\') and not os.path.exists(os.path.join(win_path,'LINUX/android/out')) and os.path.exists(os.path.join(win_path,'out')): 
               self.replace_string('LINUX/android/', '')
      if tag != 'common':
         self.log('meta_lib.py: Updating ' + tag + ' root paths in meta-info object')
         flat_build.find('windows_root_path').text = win_path
         flat_build.find('linux_root_path').text = lin_path
               
         # Get build commands from images and update in contents
         self.log('meta_lib.py: Updating ' + tag + ' make commands in meta-info object')
         if self.on_linux:
            mainmake_string = self.get_image_mainmake(lin_path, build_id)
         else:
            mainmake_string = self.get_image_mainmake(win_path, build_id)
         
         # Split the mainmake into two parts, relative path and build command
         mainmake_split = mainmake_string.split(' ', 1)  # separate the arguments
         mainmake_split[0] = mainmake_split[0].replace('\\', '/')  # change all to forward slashes
         buildfile_path = mainmake_split[0].rsplit('/', 1)[0]  # now separate relative path and build filename
         build_command = mainmake_split[0].rsplit('/', 1)[-1] + " " + mainmake_split[-1]  # -1 works when mainmake_string is empty
         # Save the filepath and build command to contents.xml
         find_element_or_create(flat_build, 'buildfile_path').text = buildfile_path + '/'
         find_element_or_create(flat_build, 'build_command').text = build_command

   def get_build_bid_var_list (self, build=None):
      # Get BID IDs define in contents XML, This value can be modified dynamically
      # pattern = ".*{(.*):.*}.*" 
      # contents_txt = et.tostring(self.XmlRoot)
      # bid_list = re.findall(pattern,contents_txt)
      # bid_list.append('variant')
      # return sorted(list(set(bid_list)))
      bid_list = {}	  
      pattern = ".*{(.*):(.*)}.*"
        
      if build == None:      
         contents_txt = et.tostring(self.XmlRoot)
      else:
         builds_flat = self.XmlRoot.find('builds_flat')
         build = find_named_element(builds_flat, build)
         contents_txt = et.tostring(build)
      # bid_list = sorted(list(set(re.findall(pattern,contents_txt))))
      for var_vals in re.findall(pattern, contents_txt):
         bid_list[var_vals[0]] = var_vals[1]
      return bid_list

   def replace_string (self, strreplace, strval):
      '''Replace string meta-info xml tree'''
      # Search text of every element in the xml tree to look for strreplace string.
      self.__replace_string_recurse(self.XmlRoot, strreplace, strval)

   def __replace_string_recurse (self, cur_elem, strreplace, strval):
      '''Private function called by replace_string()'''
      text = cur_elem.text
      if ((text != None) and (text.count(strreplace) >= 1)):
         # Look for a matching string
         cur_elem.text = re.sub(strreplace, strval, text)
         #self.log('Cur element : '+ text)
         #self.log('New element : '+ cur_elem.text)

      # Now recurse to all children
      for child in cur_elem.getchildren():
         self.__replace_string_recurse(child, strreplace, strval)

   def update_var_values (self, var, val):
      '''Update variable values in meta-data:
   var: String name of variable to update.
   val: String value to update variable with.'''

      # Search text of every element in the tree to look for instances of the
      # variable.  Variables are surrounded by ${var_name}, and may already
      # contain a value, such as: ${var_name:value}.
      # Currently, this pattern can only match one var/value pair per string.
      pattern = "\A(.*\${%s*)(?::[^}]*)?(}.*)\Z" % var
      re_obj = re.compile(pattern)
      self.__update_var_values_recurse(re_obj, self.XmlRoot, var, val)

   def __update_var_values_recurse (self, re_obj, cur_elem, var, val):
      '''Private function called by update_var_values()'''
      text = cur_elem.text
      # Look for "${" first, since this is much faster than a regex search
      if ((text != None) and (text.count('${') >= 1)):
         # Look for a matching variable in our string
         match_obj = re_obj.search(cur_elem.text)
         if match_obj:
            groups = match_obj.groups()
            new_text = groups[0] + ':' + val + groups[1]
            cur_elem.text = new_text

      # Now recurse to all children
      for child in cur_elem.getchildren():
         self.__update_var_values_recurse(re_obj, child, var, val)
   
   def clean_content_vars (self):
      self.__clean_vars_recurse(self.XmlRoot)
   
   def __clean_vars_recurse (self, cur_elem):
      '''Private function called by update_var_values()'''
      text = cur_elem.text
      if text:
         new_text = clean_vars(text)
         cur_elem.text = new_text
      for child in cur_elem.getchildren():
         self.__clean_vars_recurse(child)

   def update_to_release_path(self, tag, attr=None):
      '''Updates the build path with release folder path'''
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, tag) 
      linux_path_elem = flat_build.find('linux_root_path')
      windows_path_elem = flat_build.find('windows_root_path')
      if linux_path_elem.text != None:
         linux_path = linux_path_elem.text.strip()
      else:
         linux_path = None
      
      if windows_path_elem.text:
         windows_path = windows_path_elem.text.strip()
      else:
         windows_path = None
      
      release_path = self.get_release_path(tag, attr)
      self.log('meta_lib.py: updating to release path "' + release_path + '" ')
      if release_path:
         if windows_path:
            flat_build.find('windows_root_path').text = join_paths([windows_path, release_path], '\\') + '\\'
         if linux_path:
            flat_build.find('linux_root_path').text = join_paths([linux_path, release_path], '/') + '/'

   # End of update_to_release_path
   
   def filter_product_flavors(self, selected_pf_list):
      '''
      Removes unselected flavors from the list of all flavors
      Reads the used flavors from <product_flavor> section
      and removes any files in this build which are of other flavors
      '''
      self.log("===== Filtering unused product flavors =====")
      self.log("Selected flavors = " + ', '.join(selected_pf_list))
      product_flavors = self.XmlRoot.find('product_flavors')
      component_flavor_dict = {}
      unwanted_pf_elems = []
      if product_flavors is not None:

         # Remove <pf> sections of unspecified flavors from <product_flavors>
         for pf_elem in product_flavors:
            pf_name = pf_elem.find('name').text.strip()
            if pf_name not in selected_pf_list:
               unwanted_pf_elems.append(pf_elem)
            else:  # If flavor is selected, create a dictionary
               for component_elem in pf_elem.findall('component'):
                  name = component_elem.find('name').text
                  flavor = component_elem.find('flavor').text
                  # Check and add to dictionary as {build : [flavors]}
                  if name in component_flavor_dict:
                     (component_flavor_dict[name]).add(flavor)  # add to created set
                  else:
                     component_flavor_dict[name] = set([flavor])  # create as a set
         if len(unwanted_pf_elems) > 0:
            for pf_elem in unwanted_pf_elems:
               self.log("Deleting " + pf_elem.find('name').text.strip())
               product_flavors.remove(pf_elem)
            
            # After cleaning up <product_flavors> delete files of those removed flavors from <builds_flat>
            self.log("Checking for unused flavor files")
            build_root = self.XmlRoot.find('builds_flat')
            for build_elem in build_root:
               tag = build_elem.find('name').text
               all_file_elems = list(build_elem)  # take list to loop over (otherwise deleting elems jumps the loop)
               for file_elem in all_file_elems:
                  # Loop only through the default file types
                  if (file_elem.tag in self.default_file_types):
                     # Check if flavor is specified in the file element itself
                     if not (file_elem.get('flavor') is None or file_elem.get('flavor') in component_flavor_dict[tag]):
                        self.log("   Deleting " + file_elem.tag + " " + file_elem.find('file_name').text)
                        build_elem.remove(file_elem)
                     else:
                        # Check if flavor is specified in file name or path
                        for file_property in file_elem:
                           if not (file_property.get('flavor') is None or file_property.get('flavor') in component_flavor_dict[tag]):
                              self.log("   Deleting " + file_property.tag + " " + file_property.text + " in " + file_elem.find('file_name').text)
                              file_elem.remove(file_property)
                        # After filtering, check if the file element has atleast one name and one path left
                        if file_elem.find('file_name') is None or file_elem.find('file_path') is None:
                           self.log("   Deleting parent " + file_elem.tag + " " + file_elem.find('file_name').text)
                           build_elem.remove(file_elem)
   # End of filter_product_flavors
   
   def remove_workflow_steps(self, selected_steps_list):
      if len(selected_steps_list):  						# atleast 1
         # Build attribute string from list
         attr_string = "filter!=" + selected_steps_list[0]  # if only 1, no parentheses
         if len(selected_steps_list) > 1:  					# atleast 2
            attr_string = "(" + attr_string + ")"  			# add parentheses for 1st one
            for step in selected_steps_list[1:]:
               attr_string += " && "  						# add AND between filters
               attr_string += "(filter!={0})".format(step)  # filter elems with attrs NOT in selected list
         self.log("Workflow filter string: " + attr_string)
         # Apply filtering only under <workflow> element
         root_elem = self.XmlRoot.find('workflow')
         # Call function to remove unselected elements
         self.remove_elements_with_attrs(attr_string, root_elem, elem_types=["step"])
   # End of remove_workflow_steps
   
   def remove_storage_type_elements(self, slected_storage_list):
      if len(slected_storage_list):  						# atleast 1
         # Build attribute string from list
         attr_string = "storage_type!=" + slected_storage_list[0]   # if only 1, no parentheses
         if len(slected_storage_list) > 1:  					    # atleast 2
            attr_string = "(" + attr_string + ")"  			# add parentheses for 1st one
            for st_type in slected_storage_list[1:]:
               attr_string += " && "  						# add AND between filters
               attr_string += "(storage_type!={0})".format(st_type)  # filter elems with attrs NOT in selected list
         self.log("Storage filter string: " + attr_string)
         # Remove build file elements
         for root_elem in self.XmlRoot.findall('./builds_flat/build'):
            self.remove_elements_with_attrs(attr_string, root_elem, elem_types= self.default_file_types)
         #Remove work flow entry
         root_elem = self.XmlRoot.find('workflow')
         self.remove_elements_with_attrs(attr_string, root_elem, elem_types=["step"])
   # End of remove_storage_type_elements
   
   def remove_images(self, selected_images):
      self.log("selected images are "+str(selected_images)) 
      build_root = self.XmlRoot.find('builds_flat')
      for build_elem in build_root:
         tag = build_elem.find('name').text
         if tag in selected_images:
            self.log("Removing the image:"+tag)		 
            build_root.remove(build_elem)		 
         
   # End of remove_images
   
   # Get Component version 
   def get_version(self, component='build.glue'):
      '''Read component version from PLF file'''
      comp_list = []
      root_dir = None	  
      try:     
         root_dir = self.get_build_path('common')
         #root_dir = None
      except:
          self.log("Build tag not found!!!")		 
      if root_dir is not None:
         glue_plf_list = glob(os.path.join(root_dir, 'GLUE.*.plf'))
         if len(glue_plf_list) > 0 :
            glue_plf_file = os.path.abspath(glue_plf_list[0])
            if component is None:
               component = '@'
            else:
               component = '@' + component
            # print glue_plf_file
            with open(glue_plf_file, "r") as plfcontents:
               for line in plfcontents:
                  if line.startswith(component):
                     comp, version = line[1:].strip('\n').split('-')
                     comp_version = {}
                     comp_version['component'] = comp
                     comp_version['version'] = version
                     comp_list.append(comp_version)
      
      if len(comp_list) <= 0:
         #Version needs to be updated before check-in
         comp_version = {}
         comp_version['component'] = 'build.glue.1.0'
         comp_version['version'] = '00113'
         comp_list.append(comp_version)
      return comp_list
   # End of Component version 

   # Get Chipids 
   def get_chipids(self):
      '''Find the chipids from the contents xml product info section '''
      product_info = self.XmlRoot.find('product_info')
      product_name = product_info.find('product_name').text
      chip_ids = (product_info.find ('chipid').text.upper()).split('_')
      if (product_name.split('.')[0]).find('_') != -1 and len(chip_ids) < 2:  # More than one combine SP
         '''Might be older two SP combination, need to go through all the build elements for chipset ids '''
         for element in self.XmlRoot.findall('./builds_flat/build/chipset') :
            chipid = element.text.strip().upper()
            if chipid not in chip_ids:
               chip_ids.append(chipid)       	
      
      return chip_ids
   # End of Chipids

   def get_additional_chipids(self):
      product_info = self.XmlRoot.find('product_info')
      chipids = product_info.find ('additional_chipid')
      return chipids.text.upper().split(',') if chipids is not None else []
   # End of additional Chipids

   def get_build_path (self, tag, get_attrs=False, xmlpath=False):
      '''Find the current path for the specified build tag'''
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, tag)
      return self.__get_build_path_from_build_elem(flat_build, get_attrs, xmlpath)
   
   # Gets build id from builds_flat   
   # Since nested builds feature is removed, build_base is Unused, but kept to maintain symmetry  
   def get_build_id(self, tag, build_base='Unused', get_attrs=False):
      '''Find the build id from the specified build tag '''
      build_root = self.XmlRoot.find('builds_flat')
      build_elem = find_named_element(build_root, tag)
      build_id = build_elem.find('build_id').text     
                
      if build_id:
         build_id = build_id.strip()

      return build_id
   
   def get_build_field (self, build_name, field):
      ''' Find the build id from the specified build tag '''
      build_root = self.XmlRoot.find('builds_flat')
      build_elem = find_named_element(build_root, build_name)
      ret = build_elem.find(field).text     

      if ret:
         ret = ret.strip()

      return ret

  # Get list of workflow filter
   def get_wf_step_filter_list (self):
      '''Get list of workflow steps'''
      wf_filter = []
      for step_elem in self.XmlRoot.findall('./workflow/step') :
         if 'filter' in step_elem.attrib:
            wf_filter.append(step_elem.attrib['filter'])    
      return list(set(wf_filter))

  # Get list of workflow steps for re-compilation chnage of given SI build
   def get_wf_step_filter_list_by_build (self, build_name):
      '''Get related workflow step need to compile after change of given build'''
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, build_name)
	  # By default pass "--wflow_filter=contentsxml" to update only contents XML 
      step_to_execute = ['contentsxml']  
      step_filter = flat_build.find('wf_step_filter')
      if step_filter != None:
         step_to_execute = step_filter.text.split(",")
      return step_to_execute

   # Get command from step to re-generates sparse image.  
   def get_sparse_image_command(self, flavor=None, storage=None, secondary_boot=False):
      '''Find out list of command to generate sparse image for given 
      flavor type. It will return empty list in case there no such step contents 
      XML to generates sparse image'''
      sparse_step_command = []
      pflavors = self.get_product_flavors()
      if (pflavors is not None and flavor is None) or (pflavors is None and flavor is not None):
         return sparse_step_command  # Return empty sparse command list
      # Find out Workflow step for sparse image
      for element in self.XmlRoot.findall('./workflow/step[@filter="hlos"][@type="exec"]') :
         step_dict = XmlToDict(element)
         # Filter out sparse image steps based on tool name
         if('tool_name' in step_dict and 'checksparse.py' in step_dict['tool_name']): 
            # Check flavor flag and update step accordingly
            if pflavors is not None and flavor in pflavors:
               for key in step_dict:  # Update dictionary based on flavor
                  if ('$[flavor' in step_dict[key]):                  
                     step_dict[key] = update_var_values(step_dict[key], "flavor", flavor) 
               # Find out tool path	 
               tool_name = step_dict['tool_name']               
               tool_build = self.get_tool_build(tool_name, 'workflow')               
               if tool_build:               
                  # If valid build exists, get the full path to tool
                  tool_pfn = os.path.join(self.get_build_path(tool_build), os.path.join(self.get_tool_path(tool_name, 'workflow'), tool_name))
                  step_dict['tool_name'] = tool_pfn
                  # print step_dict['tool_name']
                  
                  # Findout default execution directory for steps 
                  step_exec_dir = os.path.join(self.get_build_path('common'), 'common/build')
                  if ('exec_dir' in step_dict):
                     cur_step_exec_dir = clean_wvars(step_dict['exec_dir'])
                     if cur_step_exec_dir.startswith('./') or cur_step_exec_dir.startswith('.\\'):
                        cur_step_exec_dir = clean_wvars(step_dict['exec_dir'])[2:]
                        step_dict['exec_dir'] = os.path.join(step_exec_dir, cur_step_exec_dir)
                  else:  # Default <step> execution network path
                     step_dict['exec_dir'] = step_exec_dir
                  # Update root path for file entry all child element 
                  for step_elem in step_dict.keys():
                     if step_elem.startswith('root_dir'):
                        build = clean_wvars(step_dict[step_elem])
                        root_dir_si = self.get_build_path(build)
                        step_dict[step_elem] = root_dir_si
                     else:
                        element = step_dict[step_elem]
                        if element.startswith('./') or element.startswith('.\\'):
                           step_dict[step_elem] = os.path.join(step_dict['exec_dir'], element[2:])
                           # print step_dict[step_elem]

                  # Finally evalute param element
                  step_dict['params'] = evaluate_params(step_dict_flavor=step_dict)
                  if storage:
                     if '/'+storage+'/' in  step_dict['params']:
                        sparse_step_command.append(step_dict['params'])
                  else:
                     sparse_step_command.append(step_dict['params']) 
                  # print str(step_dict)
      # Check for secondary_boot 
      if secondary_boot:
         secboot_sparse_step_command = list()
         for command in sparse_step_command:
            command_array = command.split(' ')
            index = command_array.index('-s') 
            command_array.insert(index, os.path.join(command_array[index+1],'secondary-boot/'))
            command_array.insert(index, '-s')
            secboot_sparse_step_command.append(" ".join(command_array))
         return secboot_sparse_step_command
      # List of commad use in step to generates sparse image
      return sparse_step_command 

   def __get_build_path_from_build_elem(self, build, get_attrs=False, xmlpath=False):
      if self.on_linux:
         path_elem = build.find('linux_root_path')
      else:
         path_elem = build.find('windows_root_path')

      path = path_elem.text
      if path:
         path = path.strip()
         # If the path is relative, turn it into an absolute path
         # using the directory of the meta info file as the base.
         if ((self.on_linux and not path.startswith('/'))
              or (not self.on_linux and not(path.startswith('\\') or path.find(':\\', 1, 3) == 1))):
            base_path = os.path.realpath(os.path.dirname(self.meta_info_pfn))
            path = os.path.normpath(os.path.join(base_path, path)) + os.path.sep  # Keeping slash at the end

      if not xmlpath:
         image_dir = build.find('image_dir').text
         build_id = build.find('build_id').text
         if image_dir:
            image_dir = image_dir.strip()
         image_path = os.path.join(path, image_dir)
         if build_id:
            build_id = build_id.strip()
            image_archive_file = build_id + '.7z'
            image_archive_filepath = os.path.join(path, image_archive_file)
      
            if (os.path.exists(image_archive_filepath) and not os.path.exists(image_path)) or not path or not os.path.exists(path) :
               self.log('Build path' + image_path + ' is not accessible!')
               self.log(build_id + ' might be moved, is expired or archived.')
               self.log('Checking for a new path...')
               mpath = self.__get_new_path(build_id)
               if self.on_linux:
                  path = mpath.lin_path
               else:
                  path = mpath.win_path
               image_path = os.path.join(path, image_dir)
               self.log('Image path ' + image_path)
               if not path or not os.path.exists(path):
                  self.log('The build_id ' + build_id + ' is not accessible. Please check if the server on which build is present is not accessible or if the build is expired')
                  raise NameError, "The build_id " + build_id + " is not accessible. Please check if the server on which build is present is not accessible or if the build is expired"
               elif not os.path.exists(image_path):
                  self.log('The build_id ' + build_id + ' is archived. Please de-archive the image build and try again')
                  raise NameError, "The build_id " + build_id + " is archived. Please de-archive the image build and try again"
               elif os.path.exists(image_path):
                  self.log('New path for ' + build_id + ' is accessible!')
                  # save new path to current mi object so that
                  # findbuild doesn't need to be called everytime
                  self.update_build_info(build.find('name').text, [mpath.lin_path, mpath.win_path],None,build_id)

      if get_attrs:
         return [path, path_elem.attrib]
      else:
         return path


   def get_image_dir (self, tag):
      '''Find the image directory for the specified build tag'''
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, tag)
      image_dir = flat_build.find('image_dir')
      if image_dir != None:
         return image_dir.text.strip()
      else:
         return None

   def get_release_path (self, tag, attr=None):

      '''Find the release folder path for the specified build tag''' 
      self.log("meta_lib.py: obtaining release path for " + tag)
      src_search_paths = ['HY11_CompileTest/SRC*',
                          'HY11_CompileTest/HY11',
                          'HY11_CompileTest/HY11_1',
                          'HY11_CompileTest',
                          'PCT/SRC*',
                          'PCT/HY11*']
      bin_search_paths = ['HK11*',
                          'BIN*']

      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, tag)
      build_root_dir = self.__get_build_path_from_build_elem(flat_build)

      release_test_config = self.get_product_info(field='release_test_config')

      release_path_elements = flat_build.findall('release_path')
      release_path = None
      for each_element in release_path_elements:
         if 'type' in each_element.attrib:
            if each_element.attrib['type'].upper() == attr.upper():
               release_path = each_element
               self.log("meta_lib.py: found release_path element with " + attr)
               break
         elif 'type' not in each_element.attrib:
            release_path = each_element  # No type attrib mentioned, considered as default
            self.log("meta_lib.py: default release_path element selected ")

      rel_path_text = None
      if release_path != None:
         if release_path.text != None:
            rel_path_text = clean_vars(release_path.text.strip())
         else:
            rel_path_text = ""

      if release_test_config == None and attr != None :
      # performing auto-detection  (only in the case where release_test_config not defined)
         self.log("meta_lib.py: auto searching release_paths of type " + attr)
         if attr.upper() == 'SRC':
            search_paths = src_search_paths
         elif attr.upper() == 'BIN':
            search_paths = bin_search_paths
         for search_path in search_paths:
            rel_path_texts = sorted(glob(os.path.join(build_root_dir, search_path)))
            if len(rel_path_texts) > 0:
               rel_path_text = os.path.relpath(rel_path_texts[0], build_root_dir)
               break

      if rel_path_text is not None and os.path.isdir(os.path.join(build_root_dir, rel_path_text)):
         return rel_path_text

      if rel_path_text is not None:
         raise InputError('Release directory does not exist or is not a directory: ' + rel_path_text)

      raise InputError('Could not find <release_path> element with type="' + attr + '" for ' + tag + ' build.')
   # End of get_release_path()

   def get_build_list (self, chipset=None):
      '''Return a list of build names'''
      builds_flat = self.XmlRoot.find('builds_flat')
      build_names = list()
      if chipset is None:
         for child in builds_flat.getchildren():
            if (has_element(child, 'build_id') or
                has_element(child, 'windows_root_path') or
                has_element(child, 'linux_root_path')):
                build_names.append(child.find('name').text.strip())
      else:
         for child in builds_flat.getchildren():
            if ((has_element(child, 'build_id') or
                has_element(child, 'windows_root_path') or
                has_element(child, 'linux_root_path')) and
                (child.find('chipset').text.strip().lower() == chipset.lower())):
                build_names.append(child.find('name').text.strip())
      return build_names

   def get_product_flavors(self):
      '''Return a list of product flavors in the same order they appeared in contents.xml.'''
      pf_list = []
      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors is not None:
         for pf_elem in product_flavors.getchildren():
            prod_flav = pf_elem.find('name').text.strip()
            pf_list.append(prod_flav)

      return pf_list
   
   def get_tool_build (self, tool, root='build_tools'):
      '''Look in the tools tag for the specified tool, and return the contents of the 'build' child.'''
      tools_elem = self.XmlRoot.find(root)
      tool_elem = find_named_element(tools_elem, tool)
      if tool_elem is not None:
         build_elem = tool_elem.find('build')
         return build_elem.text.strip()
      else:
         return None

   def get_tool_path (self, tool, root='build_tools'):
      '''Look in the tools tag for the specified tool, and return the contents of the 'path' child.'''
      tools_elem = self.XmlRoot.find(root)
      tool_elem = find_named_element(tools_elem, tool)
      path_elem = tool_elem.find('path')
      return path_elem.text.strip()

   def get_tool_params (self, tool, flavor=None):
      '''Look in the tools tag for the specified tool, and return the contents of the 'params' child.'''
      tools_elem = self.XmlRoot.find('build_tools')
      tool_elem = find_named_element(tools_elem, tool)
      params_elem = tool_elem.findall('params')
      params_list = []
      
      flavors = self.get_product_flavors()
      if flavor != None:
         # Check if its a valid flavor
         if flavor in flavors:
            # Take this flavor as the only flavor required
            flavors = [flavor]
         else:
            # Return empty list
            self.log("Specified flavor is either invalid or not part of this product")
            return []
         for each_param in params_elem:
            if "flavor" in each_param.attrib and each_param.attrib["flavor"] == flavor:
               params_list.append(clean_vars(each_param.text.strip()))
      else:
         # Find only the element with no flavor attribute
         for each_param in params_elem:
            if "flavor" not in each_param.attrib:
               params_list.append(clean_vars(each_param.text.strip()))
      
      return params_list
   
   def get_attr_values (self, attr, build_name=None):
      ''' Getting list vars based on given attribute''' 
      var_list = []
      flat_build_list = self.XmlRoot.findall('./builds_flat/')
      if build_name != None:
         for flat_build in flat_build_list:
            if build_name == flat_build.find('name').text.strip():
               for element in flat_build.getchildren():
                  if element.attrib and attr in  element.attrib:
                     var_list.append(element.attrib[attr])
      else:
         for flat_build in flat_build_list:
            for element in flat_build.getchildren():
               if element.attrib and attr in  element.attrib:
                  var_list.append(element.attrib[attr])
      # Return list of vars
      return list(set(var_list)) 

   def get_var_values (self, attr, cur_elem=None, var_values=None, flavor=None):
      '''Return a dictionary of var/values for the specified attribute.This handles the format
      where an attribute value specifies the name of a variable, and the value of the variable
      is the value of the element containing the attribute.

      This is a recursive function.  It is initially called with only the attribute name.The cur_elem 
      and var_values parameters are used in recursive calls to sub-elements.'''

      # Initialize
      if var_values is None:
         var_values = dict()
      if cur_elem is None:
         cur_elem = self.XmlRoot

      if cur_elem.attrib and attr in cur_elem.attrib:
         if flavor :
            flavor_list = get_flavor_list(cur_elem)
            if (len(flavor_list) == 0) or (flavor in flavor_list) :
               var_values[cur_elem.attrib[attr]] = clean_vars(cur_elem.text).strip()
         else:
            var_values[cur_elem.attrib[attr]] = clean_vars(cur_elem.text).strip()
      # Now recurse to sub-elements
      for elem in cur_elem.getchildren():
         self.get_var_values(attr, elem, var_values, flavor)

      return var_values

   def get_file_vars (self,
                      attr,
                      build=None,
                      file_types=default_file_types,
                      flavor=None,
                      abs=True,
                      critical=False,
                      file_name=None,
                      storage=None):
      '''
      Return a list dictionary of attribute value to a list of relative paths to files.

      The idea here is the attribute values are names that are mapped to files
      listed in contents.xml.  The first usage of this is for cmm variables.

      Parameters:
      
         file_types (optional, defaults to 'file_ref'):
      
            A list of types of file to return.  The default is all file types.
      
            Supported file types are:
      
               default_file_types = ['download_file', 'file_ref','dload_boot_image']

         attr (mandatory):
      
            Filter based on the specified attribute.  An attribute is required because
            the return value contains a mapping of the values of the attributes to the
            list of files paths for that value.
      
         flavor (optional, defaults to None):
      
            Filter based on the specified flavor.  If not specified, all files are returned
            regardless of flavor.
      
         abs (optional, defaults to True):
      
            Specifies whether to return absolute or relative paths.  Relative paths are
            relative to the root directory for the build.  Defaults to absolute paths.
            Set to False to return relative paths.
      '''

      if type(file_types) is not list:
         file_types = [file_types]

      builds_flat = self.XmlRoot.find('builds_flat')
      if build:
         if type(build) is list: # Not supported meta_cli
            build_list = [find_named_element(builds_flat,build_name) for build_name in build ]
         else:			
            build_list = [find_named_element(builds_flat, build)]
      else:
         build_list = builds_flat.getchildren()

      var_list = dict()
      # Add storage type filter
      comb_attr = attr
      if storage != None:
         if storage not in self.get_storage_types():
            return var_list
         storage_attr = "(!(storage_type)) || (storage_type=={0})".format(storage)
         if comb_attr:
            comb_attr = "(" + comb_attr + ")" + " && "
         else:
            comb_attr = ""  # convert to empty string instead of None to append below
         comb_attr += storage_attr
         
      for build in build_list:
         build_flavor = None
         build_file_list = get_all_files_for_build(build, file_types)

         if comb_attr != None:
            # Filter on attribute
            build_file_list = filter_files_by_attr(build_file_list, comb_attr)

         if flavor != None:
            # Filter on flavor
            build_name = build.find('name').text.strip()
            if (flavor in self.flavor_data and
                build_name in self.flavor_data[flavor]) :
               build_flavor = self.flavor_data[flavor][build_name]
            build_file_list = filter_files_by_flavor(build_file_list, build_flavor)

         for file in build_file_list:
            var = file.attrib[attr]
            if var not in var_list:
               var_list[var] = list()
            base_path = self.__get_build_path_from_build_elem(build)
            var_list[var] += build_file_path_name_list_from_flavor(root_path=base_path,
                                                                   file_elem=file,
                                                                   flavor=build_flavor,
                                                                   abs=abs,
                                                                   critical=critical)
      if file_name != None:
         var_dict = dict()
         for k, v in var_list.iteritems():  # add each item to common dict
            for file in v:
               base_file_name = os.path.basename(file)		 
               if fnmatch.fnmatch(base_file_name, file_name):	
                  if k not in var_dict:
                     var_dict[k] = list()				
                  var_dict[k].append(file)
         var_list = dict(var_dict)					

      return var_list
   
   def get_files_detailed (self, attr, file_types=default_file_types):
      '''
        Return a list of files listed in contents.xml as DetailedInfo class, 
        which matches the attr value specified. 
        The DetailedInfo contains the following information for the file element:
         - Base path of the file element 
         - All the attributes of the file element
         - All the names and attributes of the children of the file element
        
        The first usage of this is for sign_id.
        
        Parameters:
        
           file_types (optional, defaults to default_file_types):
        
              A list of types of file to return.  The default is all file types.
        
              Supported file types are:
        
                 default_file_types=['file_ref','download_file', 'dload_boot_image','partition_file','partition_patch_file'] 
        
           attr (mandatory):
        
              Filter based on the specified attribute.  An attribute is required because
              the return value contains list of file path elements that matched the
              specified attribute.
         
   Usage example:
      <file_ref chipset="8084" sign_id="appsbl_mbn" minimized="true">
      <file_name>emmc_appsboot.mbn</file_name>
      <file_path sign_source="true">LINUX/android/out/target/product/apq8084/unsigned/</file_path>
      <file_path sign_dest="true">LINUX/android/out/target/product/apq8084/</file_path>
      </file_ref> 
   
      To retrieve the attribute values in examples above, the following
      code can be used:          
   
      detailed_info_list = meta_info.get_files_detailed('sign_id')
      for detailed_info in detailed_info_list:
         base_path = detailed_info.image_dir_base 
         chipset = detailed_info.chipset
         file_name = detailed_info.file_name[0].value
         for path in detailed_info.file_path:
            file_path = path.value
       isSource = path.sign_source
       isDestination = path.sign_dest
         
        '''
      class DetailedInfo(object):
         def __init__(self):
            pass
         
      if type(file_types) is not list:
         file_types = [file_types]
      builds_flat = self.XmlRoot.find('builds_flat')
      obj_list = []
      
      for build in builds_flat.getchildren():
         build_file_list = get_all_files_for_build(build, file_types)
         
         if attr != None:
            # Filter on attribute
            build_file_list = filter_files_by_attr(build_file_list, attr)
         
         for _file in build_file_list:
            obj = DetailedInfo()
            
            # Save the base path of the image 
            base_path = self.__get_build_path_from_build_elem(build)
            setattr(obj, 'image_dir_base', base_path)
            
            # Save all the attributes in the object
            for e_attr, e_value in _file.attrib.items(): 
               setattr(obj, e_attr, e_value)
            
            # Save all the attributes of the children
            for child in _file.getchildren():
               child_data = getattr(obj, child.tag, None)
               if child_data is None:
                  child_data = []
               this_child = DetailedInfo()
               setattr(this_child, 'value', clean_vars(child.text.strip()))
               for e_attr, e_value in child.attrib.items(): 
                  setattr(this_child, e_attr, e_value)
               child_data.append(this_child)
               setattr(obj, child.tag, child_data)
            
            # Add this object to the list
            obj_list.append(obj)
      return obj_list


   def get_files (self,
                  file_types=default_file_types,
                  build=None,
                  attr=None,
                  flavor=None,
                  abs=True,
                  expand_wildcards=True,
                  critical=False,
                  file_name=None,
                  storage=None,
                  invalid_path=False):
      '''
      Return a list of file path/name strings for the given file_type, optionally filtered
      by build name, attribute or flavor.  The path defaults to an absolute path, but can be
      specified to be relative by setting the abs parameter to False.
      
      Parameters:
      
         file_types (optional, defaults to 'file_ref'):
      
            A list of types of file to return.  The default is all file types.
      
            Supported file types are:
      
               ['download_file', 'file_ref','dload_boot_image']
      
         build (optional, defaults None):

            Filter based on the specified build.  If not specified, files from all builds are
            returned.
      
         attr (optional, defaults to None):
      
            Filter based on the specified attribute.  If not specified, all files are returned
            regardless of attributes.
      
         flavor (optional, defaults to None):

            Filter based on the specified flavor.  If not specified, all files are returned
            regardless of flavor.
      
         abs (optional, defaults to True):
      
            Specifies whether to return absolute or relative paths.  Relative paths are
            relative to the root directory for the build.  Defaults to absolute paths.
            Set to False to return relative paths.
'''
      #self.log("Calling  get_files")
      if type(file_types) is not list:
         file_types = [file_types]
      
      builds_flat = self.XmlRoot.find('builds_flat')
      if build:
         if type(build) is list: # Not supported meta_cli
            build_list = [find_named_element(builds_flat,build_name) for build_name in build ]
         else:			
            build_list = [find_named_element(builds_flat, build)]
      else:
         build_list = builds_flat.getchildren()

      comb_attr = attr
      if storage:
         if storage not in self.get_storage_types():
            # Return empty list
            self.log("Specified storage is either invalid or not part of this product")
            return []
         storage_attr = "(!(storage_type)) || (storage_type=={0})".format(storage)
         if comb_attr:
            comb_attr = "(" + comb_attr + ")" + " && "
         else:
            comb_attr = ""  # convert to empty string instead of None to append below
         comb_attr += storage_attr

      file_list = list()
      for build in build_list:
         build_flavor = None
         build_name = build.find('name').text.strip()
         build_base_path = self.__get_build_path_from_build_elem(build)
         build_file_list = get_all_files_for_build(build, file_types)
         
         # Filter on attribute
         if comb_attr != None:
            build_file_list = filter_files_by_attr(build_file_list, comb_attr)
         
         # Filter on flavor
         flavors = self.get_product_flavors()
         if flavor != None:
            # Check if its a valid flavor
            if flavor in flavors:
               # Take this flavor as the only flavor required
               flavors = [flavor]
            else:
               # Return empty list
               self.log("Specified flavor is either invalid or not part of this product")
               return []
         else:
            # Take all available flavors and also add 'None' as a flavor 
            # to get files with atleast one unflavored child when filtered
            flavors.append(None)
            
         # Find files for each flavor separately and then combine
         for flavor in flavors:               
            # Filter on flavor
            if (flavor in self.flavor_data) and (build_name in self.flavor_data[flavor]) :
               build_flavor = self.flavor_data[flavor][build_name]
            build_file_list_out = filter_files_by_flavor(build_file_list, build_flavor)
            # Get file list for each flavor
            for each_file in build_file_list_out:
               file_list += build_file_path_name_list_from_flavor(root_path=build_base_path,
                                                                  file_elem=each_file,
                                                                  flavor=build_flavor,
                                                                  abs=abs,
                                                                  expand_wildcards=expand_wildcards,
                                                                  critical=critical,
                                                                  invalid_path=invalid_path)
      if file_name != None:
         out_file_list = list()
         for file  in file_list:
            base_file_name = os.path.basename(file)
            if fnmatch.fnmatch(base_file_name, file_name):
               out_file_list.append(file) 
         file_list = list(out_file_list) 
      
      #self.log("End of get_files")      
      #Remove duplicates from final file list and return
      return sorted(list(set(file_list)))

   def set_minimized_build_flag(self , flag_value):
      '''
      This function sets value of the minimized build flag to specified value.
      The <minimized_build_flag> is a child element of <product_info>
      '''
      
      # check if the contents.xml has the element <minimized_build_flag> under the <product_info>
      product_info_el = self.XmlRoot.find('product_info')
      minimized_build_flag_el = product_info_el.find('minimized_build_flag')
      if minimized_build_flag_el == None:
         minimized_build_flag_el = et.SubElement(product_info_el, 'minimized_build_flag')

      # if the <minimized_build_flag> element does not have cmm_var attribute, then add it and set its values as "MINIMIZED_BUILD"
      cmm_var_value = minimized_build_flag_el.get('cmm_var')
      if not cmm_var_value:
         minimized_build_flag_el.set('cmm_var', 'MINIMIZED_BUILD')
         self.log('<minimized_build_flag> element did not have cmm_var so added it')
      else:
         self.log('<minimized_build_flag> element already has cmm_var attribute')

      # set the value of <minimized_build_flag> to the specified value
      if(flag_value):
         minimized_build_flag_el.text = 'TRUE'
         self.log('<minimized_build_flag> element set to TRUE')
      else:
         minimized_build_flag_el.text = 'FALSE'
         self.log('<minimized_build_flag> element set to FALSE')
         
   # end of function set_minimized_build_flag()

   def get_product_name(self):
      '''Return product name'''
      product_info = self.XmlRoot.find('product_info')
      return product_info.find('product_name').text

   def is_minimized_build(self):
      product_info = self.XmlRoot.find('product_info')
      return (product_info.find('minimized_build_flag')!= None and product_info.find('minimized_build_flag').text.upper() == 'TRUE')

   def get_hlos_type(self):
      '''Return high level os type'''
      product_info = self.XmlRoot.find('product_info')
      return product_info.find('hlos_type').text

   def update_product_name(self, product_name):
      product_info = self.XmlRoot.find('product_info')
      product_info.find('product_name').text = product_name

   def get_product_info(self, field=None, flavor=None):
      '''Get complete product information from contents XML'''
      product_info = self.XmlRoot.find('product_info')
      ret = None
      if product_info is not None:
         if flavor:
            pf_elem_list = product_info.getchildren()
            pf_elem_list = filter_files_by_flavor(pf_elem_list, flavor)
            if field : 
               for each_pf_elem_list in pf_elem_list:
                  if field == each_pf_elem_list.tag : 
                     ret = each_pf_elem_list.text.strip()
            else :
               ret = pf_elem_list
         else:
            if field:
               field_el = product_info.find(field)
               if field_el is not None and field_el.text:
                  ret = field_el.text.strip()
            else:
               ret = XmlToDict(product_info)
      return ret
   # End of get_product_info
   
   def get_contents_parti_list(self, attr=None):
      partition_names = []
      partition_info = self.XmlRoot.find('partition_info')
      if partition_info is not None:
         partition_elems = partition_info.getchildren()
         if attr is not None:
            partition_elems = filter_files_by_attr (partition_elems, attr)
         for child in partition_elems:
            partition_names.append(child.text)
      return partition_names
   # End of get_contents_parti_list

   def get_contents_parti_info_var(self, attr=None):
      partition_files = {}
      partition_info = self.XmlRoot.find('partition_info')
      if partition_info is not None:
         partition_elems = partition_info.getchildren()
         if attr is not None:
            partition_elems = filter_files_by_attr (partition_elems, attr)
            for child in partition_elems:
               partition_files[child.text] = child.attrib[attr]
      return partition_files
   # End of get_contents_parti_info_var

   def get_image_mainmake(self, buildLocation, buildID):
      # Remove excess characters from the string
      buildLocation = buildLocation.replace('\r', '')
      
      notes_lines = self.__read_image_notes(buildLocation, buildID)
      for line in notes_lines:
         # Look through the lines for Host
         currLine = line.split(":", 1)
         if currLine[0] == "Make File Name":
            return currLine[1].strip()
      
      # "Make File Name" not found in the notes.txt
      return ""
  
   def gen_minimized_build (self, dest, subfolder=True, compilable=False, robo_copy=False,si_folder='buildtag'): 
     build_si_folder_map = {} 
     build_si_folder_map['common'] = self.get_image_dir('common')	 
     product_name = self.get_product_name()	 
     if subfolder == True:
        build_id = self.get_build_id('common')
        ts = time.time()    
        build_id = build_id + "_" + datetime.datetime.fromtimestamp(ts).strftime('%Y%m%d_%H%M%S')	 
        dest = os.path.join(dest, build_id) 
        os.mkdir(dest)
        self.log("gen_minimized_build: destination  " +dest)		
     				
     for build_name in self.get_build_list():
       build_path = self.get_build_path(build_name)	
       si_root_folder = ""	   
       if build_name != 'common' and build_name != 'glue':
          if si_folder == 'buildtag':
             si_root_folder = build_name	  
             os.makedirs(os.path.join(dest, build_name))
          elif si_folder == 'buildid':
            si_root_folder = self.get_build_id(build_name).strip()
            # If build and buildpath not in sync, then it is engg build
            if si_root_folder is None or  build_path.rstrip('\\').rstrip('/').endswith(si_root_folder) is False:
               si_root_folder = "engg_" + build_name  # Enginnering SI build    
            os.makedirs(os.path.join(dest, si_root_folder))
          build_si_folder_map[build_name] = si_root_folder			
          if compilable == False:		 
             min_build_files = self.get_files(build=build_name,attr='minimized',abs=False)
          else:
             min_build_files = self.get_files(build=build_name,abs=False)	
          self.log("\n")
          self.log("################################################################################")
          self.log("# gen_minimized_build: Copying files from " + si_root_folder)
          self.log("################################################################################")			 
          for file in min_build_files:
             src_fp = os.path.join(build_path, file)
             src_path, src_file = os.path.split(src_fp)
             dst_fp = os.path.join(dest, si_root_folder, file)		 
             dst_path, dst_file = os.path.split(dst_fp) 
             if not os.path.exists(dst_path):
                os.makedirs(dst_path)
             dst_fp = os.path.join(dst_path, dst_fp)
             self.log("gen_minimized_build: CopyFile: " + src_fp + ' to ' + dst_fp)
             shutil.copy(src_fp, dst_fp)

     # Copying meta common contents.xml/meta script/t32 cmm scripts ..etc       
     src_path = self.get_build_path('common')
     common_src_path = os.path.join(src_path, build_si_folder_map['common'])
     if compilable == True:
        common_src_path = os.path.join(self.get_build_path('glue'), build_si_folder_map['common'])
		
     common_dst_path = os.path.join(dest, build_si_folder_map['common'])
     self.log("\n")	 
     self.log("################################################################################")
     self.log("# gen_minimized_build: Copying: " + common_src_path + ' to ' + common_dst_path)
     self.log("################################################################################")	
     
     if robo_copy == False:
        shutil.copytree(common_src_path, common_dst_path)
     else:			
        os.makedirs(common_dst_path)	
        self.robo_copy(common_src_path,common_dst_path)  

     if compilable:		
        for tool_elem in self.XmlRoot.findall('./workflow/tool'):
           tool_build = tool_elem.find('build').text	 
           src_tool_path = os.path.join(self.get_build_path(tool_build),tool_elem.find('path').text)
           dest_tool_path = os.path.join(dest,build_si_folder_map[tool_build],tool_elem.find('path').text)
           self.log ("dest_tool_path:"+ dest_tool_path)
           if not os.path.exists(dest_tool_path):
              os.makedirs(dest_tool_path) 		
           tool_name = tool_elem.find('name').text
           src_tool = os.path.join(src_tool_path,tool_name)
           dest_tool = os.path.join(dest_tool_path,tool_name)		
           self.log("gen_minimized_build: CopyTool: " + src_tool + ' to ' + dest_tool)	
           if os.path.exists(src_tool):		
             shutil.copy(src_tool, dest_tool)
     # updating SI root folder path		  
     for build_name in self.get_build_list():   
       if build_name == 'glue':	 # Map glue to meta common folder
         self.update_build_info(build_name, ["./", ".\\"], dest)	
       elif build_name != 'common':	 
         self.update_build_info(build_name, ["./" + build_si_folder_map[build_name], ".\\" + build_si_folder_map[build_name]], dest)	

     # Update meta info and set the <minimized_build_flag> element
     try:
       self.log("gen_minimized_build: Setting minimized build flag")  
       self.set_minimized_build_flag(True)	   
       self.log("gen_minimized_build: Updating Product Line")
       self.update_product_name(product_name)	   
     except:
       self.log("gen_minimized_build: Unable to set gen_minimized flag / product name!!")
       self.log(et.tostring(self.XmlRoot))
       self.log("gen_minimized_build: Continuing with save")	
	   
     dest_contents = os.path.join(dest, 'contents.xml')
     self.log("gen_minimized_build: saving "+dest_contents + " with the updated paths")
     try:
       self.save(fName=dest_contents)
     except:
       self.log("gen_minimized_build: Trying to make it writeable before saving")
       os.chmod(dest_contents , stat.S_IWRITE)
       self.save(fName=dest_contents)
     return dest
     
   def robo_copy(self,src,dest):
      command = ['robocopy', src, dest,'/MIR']
      self.log(str(command))
      subproc = subprocess.Popen(command, stdout=subprocess.PIPE)
      stdout, stderr = subproc.communicate() 
      if stderr:
        self.log(str(stderr))
        raise Exception(" Copying " + src + " to " + dest + " failed!!") 
   #End of robo copy
   
   def copy_files(self, src, dest, robo_copy=False):
      if robo_copy == True:
         self.robo_copy(src, dest)
      else:
         if os.path.isdir(src):
            shutil.copytree(src, dest)
         else:
            shutil.copy2(src, dest)
   #End of copy files
     
   def get_cust_meta_source (self, dest, params, bid_ids=None, subfolder=True, robo_copy=False, skipdirs=None, relative_path=True):
      '''This function will help to copy meta source(without binaries) to destination folder along with updated 
         contents XML. Users of this API should make sure to have robocopy executable in env in case of robo_copy=True'''
      images = []
      latestargs_images = {}
      latestargs_bid_ids = {}
      other_latest_args = []
      skipdir_list = skipdirs.lower().split(',') if skipdirs else []
      # validate new SI build path
      # TODO: We might have to update latest_args.txt, if user want run full build command
      latestargs_file_fn = os.path.join(self.get_build_path('common'), 'common', 'build', 'latest_args.txt')
      if os.path.exists(latestargs_file_fn):
         latestargs_file = open(latestargs_file_fn, 'r')
         latestargs_contents = latestargs_file.readlines()
         for param in latestargs_contents:
            if (':' in param):
               (build_tag, build_id) = param.strip().split(':', 1)                  
               latestargs_images[build_tag.strip()] = build_id
            elif ('=' in param):
               (bid_id_var, value) = param.strip().split('=', 1)                  
               latestargs_bid_ids[bid_id_var.strip()] = value
            else:
               other_latest_args.append(param.strip())
         latestargs_file.close()      
      has_new_glue = False
      for pair in params.split(','):
         self.log("Processing " + pair)
         try:
            (build_tag, engg_si_loc) = pair.split(':', 1)
            has_new_glue = (has_new_glue or (build_tag.lower() == 'glue'))
         except(ValueError):
            self.log("please specify in params=build tag:build location format  ")
            raise IOError(params)
         # Check build tag define in base contents XML
         if build_tag not in self.get_build_list():
            self.log("   " + build_tag + " is not a valid build tag")
            raise IOError(build_tag)
         # Update build  
         self.update_build_info(build_tag, engg_si_loc)         
         images.append(build_tag)
         if build_tag in latestargs_images:
            latestargs_images[build_tag.strip()] = engg_si_loc
      # Updating rest of the buildpath if path no longer valid
      for build in self.get_build_list():
         if build not in images:
            cur_build_path = self.get_build_path(build, xmlpath=True)
            if not os.path.exists(cur_build_path):           
               cur_build_path = self.get_build_path(build)
               self.log("   Found new path for " + build + ':'+ cur_build_path)
            # Local path in case of minimize and customer build
            if not relative_path:
               self.update_build_info(build, cur_build_path)         
      
      # Checking for new BID value     
      if bid_ids != None:
         try:
            for pair in bid_ids.split(','):
               if ':' in pair: # value pair client BID 
                  (var, value) = pair.split(':')
                  self.update_var_values(var.strip(), value)
                  latestargs_bid_ids[var.strip()] = value
               else:#Other arguments
                  other_latest_args.append(pair.strip())
         except(ValueError):
            self.log("please specify in bid_ids in bid_name=value,bid_name2_value2 format  ")
            raise IOError(bid_ids)
      # Check write permission and create folder
      if subfolder == True:
         build_id = self.get_build_id('common')
         ts = time.time()    
         build_id = build_id + "_" + datetime.datetime.fromtimestamp(ts).strftime('%Y%m%d_%H%M%S')	 
         dest = os.path.join(dest, build_id) 
         os.mkdir(dest)
      # Updating meta buildid contents XML, consider dest folder basename as metaid
      build_id = os.path.basename(dest)	  
      builds_flat = self.XmlRoot.find('builds_flat')
      flat_build = find_named_element(builds_flat, 'common')
      flat_build.find('build_id').text = build_id        	  
      # Saving updated contents XML to dest location
      # TODO: common build path can be updated as base meta
      contents_xml = os.path.join(dest, "contents.xml")
      # Start copy meta source to dest location
      glue_loc = self.get_build_path('glue') if has_new_glue else self.get_build_path('common')
      self.log("Copying " + glue_loc + " to " + dest)
      try:
         # Copy common folder and skip common top level dir based on skipdirs param 
         for dentry in os.listdir(os.path.join(glue_loc,'common')):
            if dentry.lower() in skipdir_list:
               continue
            spath = os.path.join(glue_loc, 'common',dentry)
            dpath = os.path.join(dest, 'common', dentry)
            #In case 'build' copy only script. Handle use case when glue point meta itself
            if dentry.lower() == 'build':
               self.copy_files(os.path.join(spath,'app'), os.path.join(dpath, 'app'), robo_copy) 
               self.copy_files(os.path.join(spath,'lib'), os.path.join(dpath, 'lib'), robo_copy) 
               for build_dentry in os.listdir(spath):
                  build_spath = os.path.join(spath,build_dentry)
                  build_dpath = os.path.join(dpath, build_dentry)
                  if os.path.isfile(build_spath) and os.path.splitext( os.path.basename(build_spath) )[1].lower() in ['.txt','.py','.sh','.cmd']:
                     #self.log("Copying "+ build_spath)
                     self.copy_files(build_spath, build_dpath)
               continue # Done with build script
			#Process other folders like core, sectools ....
            self.copy_files(spath, dpath, robo_copy)
         
         self.log("Copying contents.xml")
         #robo copy with /MIR will copy tree and also deletes the files which are not in SRC and in DEST 
         self.save(contents_xml)
         # copy latest_args.txt from base meta as latest_args~org.txt      
         self.log("Copying latest_args.txt") 
         shutil.copy (latestargs_file_fn, os.path.join(dest, 'common', 'build', 'latest_args~org.txt'))
         # Create updated latest_args.txt
         latestargs_file_fn = os.path.join(dest, 'common', 'build', 'latest_args.txt')
         param_file = open(latestargs_file_fn, 'w')
         for tag, loc in latestargs_images.iteritems():
            p = tag + ':' + loc + '\n'
            param_file.write (p)

         for bid_id, value in latestargs_bid_ids.iteritems():
            p = bid_id + '=' + value + '\n'
            param_file.write (p)

         for p in other_latest_args:
            param_file.write (p + '\n')  

         param_file.close()
         self.log("Copying goto scripts")
         # Start copy goto script to dest folder
         for file in glob(os.path.join(self.get_build_path('common'), 'goto_*.py')):
            shutil.copy(file, dest)
      except:
         raise Exception("Copy failed!!")          

      # Finally return the path where source for cust meta stored   
      return dest 

   # Get basic meta build info 
   def get_meta_build_info(self):
      meta_info = {}
      meta_info['si_build_info'] = self.get_si_build_info_list()
      meta_info['storage_types'] = self.get_storage_types()
      meta_info['flavors'] = self.get_product_flavors()
      return meta_info

   # Fetch list of SI build listed in contents XML under build flat XML entry   
   def get_si_build_info_list(self):
      SI_info = []
      builds_flat = self.XmlRoot.find('builds_flat')
      for build in builds_flat.getchildren():
         build_info = {}            
         build_info['name'] = build.find('name').text
         build_info['role'] = build.find('role').text
         build_info['chipset'] = build.find('chipset').text
         build_info['build_id'] = build.find('build_id').text
         build_info['path'] = self.get_build_path(build_info['name'])
         build_info['bid'] = self.get_build_bid_var_list(build_info['name'])
         SI_info.append(build_info)		  
      
      return SI_info

   def __read_image_notes(self, buildLocation, buildID):
      lines = []  # Holds the lines read in from the file      
      try:
         # If buildID is None, buildID_notes.txt doesn't exist anyway
         if buildID == None:
            raise IOError
         # If buildID is provided, open the file at buildLocation\buildID_notes.txt
         f = open(buildLocation + buildID + "_notes.txt", 'r')
         lines = f.readlines()
         f.close()
         return lines
      except IOError:
         try:
            # File probably didn't exist. Trying with the containing folder name.
            derp = buildLocation.split("\\")
            f = open(buildLocation + derp[len(derp) - 1] + "_notes.txt", 'r')
            lines = f.readlines()
            f.close()
            return lines
         except IOError:
            try:
               # File didn't exist with foldername either. Globbing for any "_notes.txt".
               file_list = glob(buildLocation + "*_notes.txt")
               if len(file_list) == 1:   
                  # Found one notes.txt file, can give it a try
                  f = open(file_list[0], 'r')
                  lines = f.readlines()
                  f.close()
                  return lines
               else:
                  # Found none or more than one notes file. Cannot proceed.
                  raise IOError
            except IOError:
               # Either file IO failed or the files didn't exist
               # If naming conventions change, more try:excepts can be placed here.
               return ""
   

   #----------------------------------------------------------#
   # Private Methods                                          #
   #----------------------------------------------------------#


   def __get_builds_flat (self):
      '''Return the 'builds_flat' element'''
      return self.XmlRoot.find('builds_flat')

   def __get_builds_flat_as_dict (self):
      '''Return a dictionary of builds with all their info.
Attributes, if any, are not returned.'''
      builds_flat = self.XmlRoot.find('builds_flat')
      return XmlToDict(builds_flat)

   class __multi_path:
      def __init__(self):
         self.lin_path = None
         self.win_path = None

   def __get_new_path (self, build_id):
      self.__init_fb_obj()
      if not self.fb:
         raise MyException("Cannot find build information !!")
      
      mpath = self.__multi_path()
            
      fbInfo = self.fb.get_fbInfo(build_id)
      win_path = fbInfo['Location'] + "\\"
      lin_path = fbInfo['LinuxPath'] + "/"
      if not win_path:
         raise MyException("No Windows path found for " + build_id)
      if not lin_path:
         raise MyException("No Linux path found for " + build_id)
         
      mpath.win_path = win_path
      mpath.lin_path = lin_path
              
      return mpath

   def __init_fb_obj(self):
      if self.fb or not self.FINDABLE:
         # self.fb has value means it is FINDABLE and already initialized
         # self.FINDABLE is False means already tried and is not FINDABLE
         return      
      try:
         #try to import findbuild lib current path
         import fb_info_lib
      except:
         # Not present in current folder, add it from meta script root location
         root_lib = os.path.join(os.path.dirname(__file__), '../../../../common/build/lib')
         sys.path.append(root_lib)
         try:
            import fb_info_lib
         except:  # not present in root either
            self.FINDABLE = False
      if self.FINDABLE:
         # Initialize the findbuild object
         self.fb = fb_info_lib.fbInfo(self.on_linux, self.logger, self.fb_nearest)

   def __is_oem_merge_alreadydone(self):
      oem_merge_done = self.XmlRoot.find('oem_merge_done')
      if oem_merge_done == None:
         return False
      else:
         if oem_merge_done.text.strip() in ['true', 'True', 'TRUE', 1]:
            return True
         else:
            return False
      # end of function __is_oem_merge_alreadydone()
      
   def __build_flavor_data (self):
      '''Build dictionary of flavor information for easier access later.'''
      flavor_data = dict()

      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors is not None:
         for pf_elem in product_flavors.getchildren():
            prod_flav = pf_elem.find('name').text.strip()
            flavor_data[prod_flav] = dict()
            for child in pf_elem.getchildren():
               if child.tag == 'component':
                  comp_name = child.find('name').text.strip()
                  comp_flav = child.find('flavor').text.strip()
                  flavor_data[prod_flav][comp_name] = comp_flav

      return flavor_data

   def __merge_oem(self):
      ''' This function merges OEM's product flavor information from contents_oem.xml into the main XML data structure.
      OEM changes are not made persistent in this function.'''

      # merging of top level OEM product flavors and removing the specified Qualcomm flavor
      xml_elem_list_oem_product_flavors = self.__get_oem_product_flavors()
      for flavor in xml_elem_list_oem_product_flavors:
         # if the OEM product flavor element does not have 'oem_remove' attribute or if it exists and has a value 'False'
         # then that flavor is a candidate to be added.
         # else ( OEM product flavor element has attribute 'oem_remove'==True ) then that product flavor is a candidate to be removed.

         if (flavor.get('oem_remove') == None) or (flavor.get('oem_remove') == 'False'):
            self.__add_product_flavor(flavor)
         else:
            self.__remove_product_flavor(flavor)
      
      # ensure that <product_flavors> is at the begining of the XML tree
      product_flavor_element = self.XmlRoot.find('product_flavors')
      if product_flavor_element:
         self.XmlRoot.remove(product_flavor_element)
         self.XmlRoot.insert(0 , product_flavor_element)


      # merging of OEM build flavor into main XML tree
      oem_build_elem_list = self.__get_oem_build_list()
      
      # process each <build> element in contents_oem.xml
      # if the resultant file type element in the main XML tree has a <file_path> without any 'flavor' attribute it will be removed.
      
      for build in oem_build_elem_list:
         list_file_type = self.default_file_types
         for file_type in list_file_type:
            # check if the OEM <build> element has 'flavor' attribute in <file_path> sub-element
            list_oem_build_file_ref_elements = build.findall(file_type)
            for oem_build_filref_elem in list_oem_build_file_ref_elements:
               oem_build_filename_elem = oem_build_filref_elem.find('file_name')
               oem_build_filepath_elem_list = oem_build_filref_elem.findall('file_path')
               
               for oem_build_filepath_elem in oem_build_filepath_elem_list:               
                  # if the OEM <build> element has file-path flavor information
                  if oem_build_filepath_elem.get('flavor') != None:
                     # get <build> element from main XML tree which matches the name of the current OEM <build> element
                     matching_build_elem = self.__get_build_elem_by_name(build.find('name').text.strip())
                     
                     # get the list of all file_ref elements in that matching <build> element
                     file_ref_list = matching_build_elem.findall(file_type)
                     
                     # check each <file_ref> element in the list if its <file_name> matches against OEM <build> element's <file_name>
                     for file_ref_elem in file_ref_list:
                        # if the <file_name> of current OEM <build> element matches against <file_name> of the main <build> element
                        if(file_ref_elem.find('file_name').text.strip() == oem_build_filename_elem.text.strip()):
                           # before adding the oem <file_path> element into main XML tree, check if the current <file_ref> has a <file_path> element pointing to the same flavoered path
                           if not self.__flavored_path_exists(file_ref_elem , oem_build_filepath_elem):
                              file_ref_elem.append(oem_build_filepath_elem)
                              
                           # check if the current main <file_ref> element has a <file_path> sub-element without any flavor, then remove it
                           list_file_path_elements = file_ref_elem.findall('file_path')
                           for file_path_el in list_file_path_elements:
                              # if the file-path element does not have any flavor, remove it
                              if not file_path_el.get('flavor'):
                                 file_ref_elem.remove(file_path_el)

      # check if the contents.xml has the element <oem_merge_done> under the root element
      oem_merge_done_el = self.XmlRoot.find('oem_merge_done')
      if oem_merge_done_el == None:
         oem_merge_done_el = et.SubElement(self.XmlRoot, 'oem_merge_done')
         oem_merge_done_el.text = 'True'
      else:
         oem_merge_done_el.text = 'True'
         
   # end of function __merge_oem()

   def __flavored_path_exists(self, file_ref_elem , oem_build_filepath_elem):
      ''' This function returns True if the given file_ref_elem has a <file_path> element same as oem_build_filepath_elem'''
      list_file_path_elem = file_ref_elem.findall('file_path')
      for file_path_el in list_file_path_elem:
         if (file_path_el.get('flavor') == oem_build_filepath_elem.get('flavor')) and (file_path_el.text.strip() == oem_build_filepath_elem.text.strip()):
            return True

      return False

   def __get_oem_build_list(self):
      '''This function gets a list of all <build> XML elements from contents_oem.xml'''
      builds_flat = self.OemXmlRoot.find('builds_flat')
      return builds_flat.getchildren()
   # end of __get_oem_build_list()

   def __get_build_elem_by_name(self , name):
      '''This function returns a <build> XML element which has the specified name'''
      builds_flat = self.XmlRoot.find('builds_flat')
      for child in builds_flat.getchildren():
         if (child.find('name').text.strip() == name):
            return child
   # end of __get_build_elem_by_name()  

   def __product_flavor_exists(self, product_flavor_element):
      ''' This function checks if there is an already existing product flavor of the same name as the given product flavor element'''
      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors:
         for child in product_flavors.getchildren():
            if self.__compare_product_flavor_elements(child, product_flavor_element):
               return True

      return False

   def __add_product_flavor(self , product_flavor_element):
      '''Adds a product flavor XML element to product_flavors in contents.xml'''
      product_flavors = self.XmlRoot.find('product_flavors')
      product_flavors.attrib['cmm_pf_var'] = "PRODUCT_FLAVORS"
      # before adding product flavor, check if there is all ready a product flavor of the same name, if not, only then add the given product flavor
      if not self.__product_flavor_exists(product_flavor_element):
         product_flavors.append(product_flavor_element)

   def __remove_product_flavor(self , product_flavor_element):
      '''Removes a product flavor XML element to product_flavors in contents.xml'''
      product_flavors = self.XmlRoot.find('product_flavors')
      if product_flavors:
         for child in product_flavors.getchildren():
            if self.__compare_product_flavor_elements(child, product_flavor_element):
               product_flavors.remove(child)

   def __compare_product_flavor_elements(self, product_flavor_element_a, product_flavor_element_b):
      '''Given two product flavor elements, if their 'name' element values are same, this function returns True , else False'''
      if self.__get_product_flavor_name(product_flavor_element_a) == self.__get_product_flavor_name(product_flavor_element_b):
         return True
      else:
         return False

   def __get_oem_product_flavors(self):
      '''Return a list of oem product flavors XML elements'''
      flavor_names = list()
      product_flavors = self.OemXmlRoot.find('product_flavors')
      return product_flavors.getchildren()

   def __get_product_flavor_name(self, product_flavor):
      '''This function returns the name of a given product flavor'''
      return product_flavor.find('name').text.strip()

   def __do_integrity_check (self):
      '''
      Function __do_integrity_check:

      This function run various tests on the database to make sure the
      information is consistent.

      Make sure the __build_flavor_data() function is called before calling
      __do_integrity_check.
      '''

      # Verify that all builds listed in the product flavors also exist
      # in the <builds_flat> element.
      builds_flat = self.XmlRoot.find('builds_flat')
      build_names = [x.find('name').text.strip() for x in builds_flat.getchildren()] 
      for prod_flav in self.flavor_data:
         for component in self.flavor_data[prod_flav]:
            if component not in build_names:
               raise DataIntegrityException(value="Product flavor '" + prod_flav + "' referenced invalid build '" + component + "'",
                                            logger=self.logger)

   def log(self, logstmt):
      if self.logger:
         self.logger.log(logstmt)
      else:
         print logstmt

   def remove_element(self, build):
      builds_flat = self.XmlRoot.find('builds_flat')
      if build :
         build_elem = find_named_element(builds_flat, build)
         if(build_elem) :
            builds_flat.remove(build_elem)
   # End of remove_element() 
   
   def find_root_element (self, tag):
      '''Return the '<tag>' element under xml root'''
      return self.XmlRoot.find(tag)
   
   def get_partition_files(self,
                     file_types=['download_file','partition_file','partition_patch_file', 'device_programmer', 'flash_programmer'],
                     build=None,
                     attr=None,
                     flavor=None,
                     storage=None,
                     abs=True,
                     expand_wildcards=True,
                     critical=False,
					 group=False):
      '''Return list of partition files define in contents XML along with list of binaries define in partition
         & contents XML. if group=True, then it will return dictionary and grouped all the files under partition, partition_patch &
         partition_bin, otherwise it will return all files as list'''

      partition_xmls_combine_list = []
      partition_xmls_list = []
      partition_patch_xmls_list = []
      if group is False:	  
         partition_xmls_combine_list = set (self.get_files(
                     file_types=['partition_file', 'partition_patch_file'],
                     flavor=flavor,
				     critical=critical,
				     storage=storage))
      else:				  
         partition_xmls_list = set (self.get_files(
                     file_types=['partition_file'],
                     flavor=flavor,
				     critical=critical,
				     storage=storage))				  
         partition_xmls_combine_list.extend(partition_xmls_list)
         partition_patch_xmls_list = set (self.get_files(
                     file_types=['partition_patch_file'],
                     flavor=flavor,
				     critical=critical,
				     storage=storage))
         partition_xmls_combine_list.extend(partition_patch_xmls_list)

      # Flashable binaries from all partition files					 
      partition_file_list = []
      for xml_file in partition_xmls_combine_list:
         self.log("Reading partition file: "+ xml_file)
         partition_file_list.extend(get_filenames_from_partitionfile(xml_file))
      # Remove partition file which needs to be skipped 
      parti_info = self.get_contents_parti_info_var(attr="skip_loading")
      skip_loading_file_list = [attr_val for partition, attr_val in parti_info.iteritems()]
      partition_file_list = list (set(partition_file_list) - set(skip_loading_file_list))
      # print partition_file_list
      if attr == None:
         attr = "!(secondary_boot)"
      # Fetch parttion file full path 
      chip_list = self.get_chipids()
      build_list = build
      if len(chip_list)> 1 and build is None: # Do only in case of fusion build
	    build_list = self.get_build_list(chipset=chip_list[0])
      # Get file path contents XML
      contents_file_list = self.get_files(
                  file_types=file_types,
                  build=build_list,
                  attr=attr,
                  flavor=flavor,
                  abs=abs,
                  expand_wildcards=expand_wildcards,
                  critical=critical,
   				  storage=storage)
      # Consider files those exist in partition files 
      contents_bin_list = [f for f in contents_file_list if os.path.basename(f) in partition_file_list] 
      # Check if it is fusion build	  
      if len(chip_list)> 1: 
	     # Updating parttion file list 
         partition_file_found_list = [os.path.basename(f) for f in contents_bin_list ] 
         partition_file_list = list (set(partition_file_list) - set(partition_file_found_list))
         if build is None:
            build_list = self.get_build_list(chipset=chip_list[1])	  
         # Get file path contents XML
         contents_file_list = self.get_files(
                     file_types=file_types,
                     build=build_list,
                     attr=attr,
                     flavor=flavor,
                     abs=abs,
                     expand_wildcards=expand_wildcards,
                     critical=critical,
   				     storage=storage)
         # Consider files those exist in partition files 
         contents_bin_list.extend([f for f in contents_file_list if os.path.basename(f) in partition_file_list])
      # Retrun file list in dictonary based on file types (partion, partition patch and flashable binaries)
      # remove duplicate file entry
      if group is True:	  
         partition_files_as_dict = {}
         partition_files_as_dict['partition'] = sorted(list(set([os.path.normpath(f) for f in partition_xmls_list])))
         partition_files_as_dict['partition_patch'] = sorted(list(set([os.path.normpath(f) for f in partition_patch_xmls_list])))
         partition_files_as_dict['partition_bin'] = sorted(list(set([os.path.normpath(f) for f in contents_bin_list])))
         return partition_files_as_dict

      # Return all files in single list without grouping and remove duplicate file entry 
      final_file_list = list(set(partition_xmls_combine_list).union(set(contents_bin_list)))
      return sorted(list(set([os.path.normpath(f) for f in final_file_list])))
   
   def get_storage_types (self):
      '''Save list of storage types'''
      storage_list = []
      for file_type in self.default_file_types:
         for element in self.XmlRoot.findall('./builds_flat/build/'+file_type):
            if 'storage_type' in  element.attrib:
               storage_list.append(element.attrib['storage_type'])
      #storage_list = [element.attrib['storage_type'] for element in self.XmlRoot.findall('./builds_flat/build/*[@storage_type]') ]
      return list(set(storage_list)) #self.get_file_vars(attr="storage_type").keys()
   
#==============================================================================
# 
# End of Class meta_info
#
#==============================================================================

def find_named_element (parent_elem, tag):
   for child in parent_elem.getchildren():
      name = child.find('name')
      if ((name is not None) and (name.text == tag)):
         return child
   return None

def find_element_or_create (parent_elem, tag):
   '''This method will search and create an element if one doesn't already exist.'''
   new_elem = parent_elem.find(tag)
   if new_elem is None:
      new_elem = et.SubElement(parent_elem, tag)
   return new_elem
   
def has_element (parent_elem, tag):
   '''Return True if the parent contains a name 'tag', and that element
has text or sub-elements.'''
   child = parent_elem.find(tag)
   if child is not None:
      if child.text:
         return True
      if child.getchildren():
         return True
   return False

def merge_builds (dest_builds, src_builds):
   for src_build in src_builds.getchildren():
      dest_build = find_named_element(dest_builds, src_build.find('name').text.strip())
      if dest_build is not None:
         # Delete the old element
         dest_builds.remove(dest_build)
         # Now append the new element to dest_builds
         dest_builds.append(src_build)

# Clean vars XML entry
def clean_vars (text):
   '''Clean variables from the specified text.'''
   if text.count('${'):
      pattern = "\A(.*)\${[^:]*(?::(.*))?}(.*)\Z"
      re_obj = re.compile(pattern)
      match_obj = re_obj.search(text)
      while match_obj:
         text = ''.join(match_obj.groups())
         match_obj = re_obj.search(text)
   return text
# End of function clean_vars()

# Clean vars workflow xml element
def clean_wvars (text):
   # first clean bid vars using meta-lib function
   text = clean_vars(text);
   # now clean workflow variables
   if text.count('$['):
      pattern = "\A(.*)\$\[[^:]*(?::(.*))?\](.*)\Z"
      re_obj = re.compile(pattern)
      match_obj = re_obj.search(text)
      while match_obj:
         text = ''.join(match_obj.groups())
         match_obj = re_obj.search(text)
   return text
# End of function clean_wvars()

# Update var values in given text and  return updated text
def update_var_values (text, var, val):
   # Search text to look for instances of variable.  
   # Variables are surrounded by $[var_name], and may already
   # contain a value, such as: $[var_name:value].
   # Currently, this pattern can only match one var/value pair per string.
   pattern = "\A(.*\$\[%s*)(?::[^}]*)?(\].*)\Z" % var
   re_obj = re.compile(pattern)
   # Look for "${" first, since this is much faster than a regex search
   try:
      if ((text != None) and (text.count('$[') >= 1)):
         # Look for a matching variable in our string
         match_obj = re_obj.search(text)
         if match_obj:
            groups = match_obj.groups()
            # If val is None (eg flavor) replace with empty
            if val:
               new_text = groups[0] + ':' + val + groups[1]
            else:
               new_text = groups[0] + ':' + groups[1]
            return new_text
      return text
   except:
      return text
# End of function update_var_values

# Function to process and get updated params for a tool execution
def evaluate_params (step_dict_flavor, params=None):
   if params is None:
      params = step_dict_flavor['params']  # Used to pass different value in recursion
   params_split = params.split()
   for i in range(len(params_split)):
      if params_split[i].startswith('@'):
         try:
            # Try to replace its value from the dictionary
            # print params_split[i][1:]
            params_split[i] = evaluate_params (step_dict_flavor, params=step_dict_flavor[params_split[i][1:]])
         except KeyError:
            # If not found evaluate for a regular varialbe
            params_split[i] = evaluate_params (step_dict_flavor, params=str(eval(params_split[i][1:])))
   new_params_split = []
   for i in range(len(params_split)):
      if params_split[i] == '+':
         if '/' in params_split[i - 1] or '\\' in params_split[i - 1]:
            new_params_split[-1] = os.path.join(new_params_split[-1], params_split[i + 1])
         else:
            new_params_split[-1] = new_params_split[-1] + params_split[i + 1]
      elif params_split[i - 1] == '+':
         pass
      else:
         new_params_split.append(params_split[i])
   params = ' '.join(new_params_split)
   # Replace the escaped @ and return
   return clean_wvars(params.replace('\\', '/').replace('/@', '@'))  # Backslash missing in log_exec cmd, during split?
# End of function evaluate_params()
 
# Function to read the  partition value from partition.xml
def getPartitionSize(partition_xml, partition_name, size_in='MB'):
   '''This function reads the value of partition size from partition.xml and return part-size KB/MB based on size_in param value
   If partition not found it raises an exception
   '''
   partition_size = 0  
   inputXmlTree = et.parse(partition_xml)
   root_element = inputXmlTree.getroot()
   # get the "physical_partition" element
   list_physical_partition_elements = root_element.findall('physical_partition')
   for physical_partition_el in list_physical_partition_elements:
      list_partition = physical_partition_el.findall('partition')
      for list_partition_el in list_partition:
         if(list_partition_el.get('label') == partition_name):
            partition_size = int(list_partition_el.get('size_in_kb'))
            if (size_in.lower() == 'kb'): return partition_size 
            if (size_in.lower() == 'bytes'): return (partition_size*1024) 
            # Change the size from KB to MB
            if (size_in.lower() == 'mb'):  
               partition_size = round(partition_size / float(1024), 2) 
               # print 'Partition( ' + partition_name + ' ) Size :{0:.2f}MB'.format( partition_size )
               return partition_size  # Partition size in MB
            # Invalid partion size tye
            raise PartitionSizeNotFoundException('Partition size in ' + size_in + ' is not supported!')
   raise PartitionSizeNotFoundException(partition_name)
# End of function getPartitionSize()

# Function to get modem partition size and backward combability 
def getModemPartitionSize(partition_xml):
   return getPartitionSize(partition_xml, 'modem')  

def XmlPrettyPrint (element, indent=0):
   text = indent * '  ' + '<' + element.tag
   # Print attributes on the same line as the tag
   for attr in element.attrib:
      text += ' ' + attr + '=' + element.attrib[attr]
   # Close the opening tag
   text += '>'
   if element.text and element.text.strip():
      text += element.text
   else:
      child_found = False
      for child in element.getchildren():
         if not child_found:
            text += '\n'
            child_found = True
         text += XmlPrettyPrint (child, indent + 1)
      if child_found:
         text += indent * '  '
   text += '</' + element.tag + '>\n'
   return text

def XmlToDict (element):
   '''Return the content of an xml sub-tree in dictionary format.
This method ignores attributes and only returns tags and associated text.'''
   if element.text and element.text.strip():
      # Leaf node
      ret = element.text
   else:
      ret = dict()
      for child in element.getchildren():
         ret[child.tag] = XmlToDict(child)
   return ret

def get_all_files_for_build (build, file_types):
   '''Return a list of all file elements for a given build. 
      File elements that don't have name/path specified will NOT be included.
   '''
   file_list = []
   for ft in file_types:
      for elem in build.findall(ft):
         empty_child = False
         for child in elem.getchildren():
            # Check if all the child elements have value
            if child.text == None or child.text.strip() == '':
               empty_child = True
               break
         if not empty_child:
            # Take only the files that have name/path values
            file_list.append(elem)
   return file_list

def filter_files_by_attr (file_list_in, attr):
   '''From a list of file elements (or any element really), filter out
those file elements that either don't contain the given attribute,
or for which the given attributes value is 'false', 0 or None.  Return
the resulting list.'''
   file_list_out = []
   attr_tree = ae.parse_attr_string(attr)
   # print "Inorder print of an attribute tree"
   # ae.print_tree(attr_tree)
   for file in file_list_in:
      ret_value = ae.eval(attr_tree, file)
      if ret_value == True:
         file_list_out.append(file)
   return file_list_out

def get_flavor_list (element):
   '''Parse a comma-separated list of flavors from the 'flavors' attribute
   of the given element, and return a Python list of these flavors.'''
   if 'flavor' in element.attrib:
      flavors = element.attrib['flavor'].split(',')
      for i in (range(len(flavors))):
         flavors[i] = flavors[i].strip()
   else:
      flavors = []
   return flavors

def filter_files_by_flavor (file_list_in, flavor):
   '''From a list of file elements, filter out those file elements that
   contain flavor information, but the given flavor.  Flavor information
   can eithre be in the file_type element, or in the file name or file
   path elements.  File elements that don't contain any flavor information
   are also returned because these files are considered to be valid for
   any flavor.  Return the resulting list.'''
   file_list_out = []
   for file in file_list_in:
      flavors_found = False
      if 'flavor' in file.attrib:
         flavors_found = True
         if flavor in get_flavor_list(file):
            file_list_out.append(file)
      else:
         # Check name and path elements for flavors attribute
         for child in file.getchildren():
            if 'flavor' in child.attrib:
               flavors_found = True
               if flavor in get_flavor_list(child):
                  file_list_out.append(file)
                  break

      if not flavors_found:
         # If no flavors attributes found, then file is for all flavors.
         file_list_out.append(file)

   return file_list_out

def build_file_path_name_list_from_flavor (root_path, file_elem, flavor, abs, expand_wildcards=True, critical=False, invalid_path=False):
   '''
   Function: build_file_path_name_list_from_flavor()

   Description:

      Build a list of relative path/file name for given file element, using the
      first path and file name that match the given flavor.  If the path contains
      wildcards, expand the wild-cards using glob and the given root path.
      Wild-card expansion includes recursively entering directories.

      If abs is True, the root path will be pre-pended to each of the file paths,
      creating absolute paths for each file.  If abs is False, the resulting list
      will contain relative paths, relative to the root path.

      The clean_vars function is run on each path to resolve any variables before
      running glob.

      If a path or name element doesn't contain flavor information, it is considered
      a match for any flavor.

      This should work, even if flavor is None, since flavor should only
      be None if there is no flavor information in contents.xml, in which
      case, path and file name should not contain 'flavors' attributes.

   Parameters:

      root_path
         Root path from which to resolve wild-cards.  Prepended to resulting paths
         if abs is True.  This path should not contain wild-cards.

      file_elem
         A file element from which file/path information is extracted.

      flavor
         Flavor of path to extract from file_elem.

      abs
         If true, return absolute paths.  Else, return relative paths.

      expand_wildcards
         Optional parameter. Self explanatory, expands wildcards in paths.

      critical
         Optional parameter. When set, will throw an error if
         search doesn't return any files on a particular element.
   '''

   name = None
   path = None
   temp_name = None
   temp_path = None
   
   if flavor != None:
      for child in file_elem.getchildren():
         if (flavor in get_flavor_list(child)):
            if not name and child.tag == 'file_name':
               name = child.text.strip()
            elif not path and child.tag == 'file_path':
               path = child.text.strip()
         elif ('flavor' not in child.attrib):
            if not temp_name and child.tag == 'file_name':
               temp_name = child.text.strip()
            elif not temp_path and child.tag == 'file_path':
               temp_path = child.text.strip()
   else:
      for child in file_elem.getchildren():
         if ('flavor' not in child.attrib):
            if not name and child.tag == 'file_name':
               name = child.text.strip()
            elif not path and child.tag == 'file_path':
               path = child.text.strip()
         else:
            if not temp_name and child.tag == 'file_name':
               temp_name = child.text.strip()
            elif not temp_path and child.tag == 'file_path':
               temp_path = child.text.strip()
   
   if not name:
      name = temp_name
   if not path:
      path = temp_path
   #Raise exception
   if not name or not path:
      raise DataIntegrityException()
   path_name = os.path.join (path, name)
   # Clean any variables from the path_name
   path_name = clean_vars(path_name)
   # Re-set critical flag based on "ignore" attribute
   if critical and 'ignore' in file_elem.attrib and file_elem.attrib['ignore'].upper is "TRUE":
      critical = False
   # Expand wild-cards
   if expand_wildcards:
      # Check if path has wild-card before processing
      if (path_name.find('*') >=0 or  path_name.find('?') >=0): 
         #print("Start globbing : "+ os.path.join(root_path, path_name))
         if file_elem.tag == 'dir_ref':
            path_names = my_glob(root_path, path_name, abs, recursion=False, definite=critical)
         else : 
            path_names = my_glob(root_path, path_name, abs, definite=critical)
         #print("End of globbing")
         return path_names
      else:
         abs_path = os.path.join(root_path, path_name)
         if invalid_path or os.path.exists(abs_path):
            if not abs:
               return [os.path.relpath(abs_path, root_path)]
            return [abs_path]
         if critical:
            raise MyException("Missing files: " + abs_path)
   else:
      abs_path = os.path.join(root_path, path_name)
      if invalid_path or os.path.exists(abs_path):
         if not abs:
            return [os.path.relpath(abs_path, root_path)]
         return [abs_path]
      if critical:
         raise MyException("Missing files: " + abs_path)
   #File not exist
   return []

# End build_file_path_name_list_from_flavor()

def my_glob (root_path, file_path, abs, recursion=True, definite=False):
   '''
   Recursively glob through directory tree returning a list of all
   matching files.

   file_path is a relative path.  Glob is done using root_path as the root
   for the relative path.  If abs is True, return absolute paths, else return
   relative paths.
   '''
   abs_path = os.path.join(root_path, file_path)
   file_list = glob(abs_path)

   # Expand directories
   new_file_list = []
   for file in file_list:
      if os.path.isdir(file) and recursion :
         if not os.path.islink(file):
            new_file_list += my_glob(file, '*', True)
      else:
         new_file_list.append(file)
   file_list = new_file_list
   
   # For definite searches atleast one file should be found
   if definite and len(file_list) < 1:
      raise MyException("Missing files: " + abs_path)
   
   if not abs:
      # Revert back to relative paths
      rel_file_list = []
      for file in file_list:
         rel_file_list.append(os.path.relpath(file, root_path))
      file_list = rel_file_list

   return file_list

def join_paths (path_list, targ_os_sep=None):
   '''Like os.path.join, but let's you specify the target OS, and makes
   sure resulting path separators are all consistent with that OS.'''

   # First, work exclusively with the local OS separator
   if 'linux' in sys.platform:
      alt_sep = '\\'
   else:
      alt_sep = '/'
   path_list = [x.replace(alt_sep, os.sep) for x in path_list]
   
   # Join and normalize path to collapse redundant separators
   new_path = os.path.normpath(os.path.join(*path_list))
   
   # Replace with target separator if provided
   if targ_os_sep:
      new_path = new_path.replace(os.sep, targ_os_sep)

   return new_path

def SaveFilesSnapshot():
   '''Save current files list to filessnapshot.txt file. This function should be called just
   after starting the build to track base file list'''
   snapshot_fname = 'filessnapshot.txt'
   if os.path.exists(snapshot_fname) is False:
      file_hadler = open(snapshot_fname, 'w')
      for rootpath, subdirs, files in os.walk('../../common/'):
         for fname in files:
            file_hadler.write (os.path.join(rootpath, fname).replace('\\', '/') + '\n')
         for subdir in subdirs:
            file_hadler.write (os.path.join(rootpath, subdir).replace('\\', '/') + '\n')
      file_hadler.close()
   return True
   
def CleanBuild():
   '''Clean build generated files if files snapshot exist for basefiles'''
   snapshot_fname = 'filessnapshot.txt'
   if os.path.exists(snapshot_fname) is False:
      print "\n\nWarning: filessnapshot.txt is not present"
      return False

   snap_fh = open(snapshot_fname, 'r')
   base_file_list = [line.strip() for line in snap_fh.readlines()]
   # print base_file_list
   snap_fh.close()
   # Get current file list in build
   current_file_list = []
   for rootpath, subdirs, files in os.walk('../../common/'):
      for fname in files:
         current_file_list.append(os.path.join(rootpath, fname).replace('\\', '/'))
      for subdir in subdirs:
         current_file_list.append(os.path.join(rootpath, subdir).replace('\\', '/'))
   remove_file_list = sorted(list(set(current_file_list) - set(base_file_list)), reverse=True)
   # print (remove_file_list)
   # Delete list of generated binary files
   for file in remove_file_list:
      print 'Removing ' + file
      if os.path.isdir(file):
         shutil.rmtree(file)
      else:
         os.remove(file)
   return True   

# Parse partition files to fetch the list of partition file names   
def get_filenames_from_partitionfile(rawprogram_xml, sparse=None):
   # # does this file exist? 
   if os.path.exists(rawprogram_xml) is False:
      print "\n\nERROR: The XML file '%s' does not exist at\n\t'%s'" % (os.path.basename(rawprogram_xml), os.path.dirname(rawprogram_xml))
      print "\tOr can't be accessed. Is the network down? Can't continue\n"
      return None
   
   root = et.parse(rawprogram_xml)
   # first assume it is flat syntax and get filenames based on that syntax
   # <program file_sector_offset="x" filename="x" label="x"..."/>
   xml_tag = 'program'
   if sparse is not None:
      xml_tag += '[@sparse=\"' + str(sparse) + '\"]'
   rawprogram_parti_nodes = root.findall(xml_tag)
   rawprogram_files_list = [el.attrib.get('filename') for el in rawprogram_parti_nodes]
   # then get filenames based on nested partition.xml syntax (partition_nand)
   xml_tag = 'partitions/partition/img_name'
   if sparse is not None:
      xml_tag += '[@sparse=\"' + str(sparse) + '\"]'
   partition_image_nodes = root.findall(xml_tag)
   partition_files_list = [el.text for el in partition_image_nodes]
   # combine both lists, clean up and return
   rawprogram_files_list.extend(partition_files_list)
   rawprogram_files_list = [f.strip() for f in rawprogram_files_list if len(f.strip()) > 0]
   return rawprogram_files_list

#==============================================================================
# 
# Exceptions
#
#==============================================================================

class DataIntegrityException(Exception):
   '''An exception class for invalid meta data.'''
   def __init__(self, value=None, logger=None):
      if value:
         self.value = "Error: " + value
      else:
         self.value = None
      if logger:
         logger.log(self.value)
      else:
         print self.value
   def __str__(self):
      return repr(self.value)

class InvalidFilePathException(Exception):
   '''An exception class for invalid file path.'''
   def __init__(self, list_invalid_files):
      Exception.__init__(self)
      self.invalid_files_list = list_invalid_files

class InvalidFileElementTypeException(Exception):
   '''An exception class for invalid file element type.'''
   def __init__(self, list_invalid_files):
      Exception.__init__(self)
      self.invalid_files_list = list_invalid_files

class InputError(Exception):
   def __init__(self, value):
      self.value = "meta_lib.py: Input Error: " + value
   def __str__(self):
      return repr(self.value)
# End of class InputError

class MyException(Exception):
   def __str__(self):
      return repr(self.args[0])
# End of class MyException

# Partition exception class
class PartitionSizeNotFoundException(Exception):
   '''An exception class for partition size not found. '''
   def __init__(self, partition_name):
      Exception.__init__(self, partition_name)
      self.partition_name = partition_name
