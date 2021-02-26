'''
=======================================================================================================
gen_cmm_data3.py:

Usage:
   gen_cmm_data <targ_root> <temp_cmm_script_file_name> <-paths_only> 

   Examples:
      gen_cmm_data3 ..\\..\\.. C:\TEMP\temp_fname.cmm -paths_only
      gen_cmm_data3 ..\\..\\.. C:\ADB\temp_fname.cmm 
      gen_cmm_data3 ..\\..\\.. \\waverider-30\Dropbox\temp_fname.cmm -paths_only
Description:
   gen_cmm_data3.py can be called from a CMM script to generate a temporary CMM
   script to initialize path information to the component builds.

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/app/gen_cmm_data3.py#7 $
 $DateTime: 2017/12/06 08:16:19 $
===============================================================================================
'''
import sys
import os
import os.path
import string
sys.path.append(os.path.abspath(os.path.join(__file__, '../../lib')))
import meta_lib as ml
import meta_log as lg

#--------------------------------------------------------------------------------#
# creating gen_buildinfo.cmm file in TEMP directory#
#--------------------------------------------------------------------------------#

def writepathsonly(cmm_script, mi, cmm_tag_template):
   cmm_script.write("GLOBAL &HLOS\n&HLOS=\"" + mi.get_product_info('hlos_type') + "\"\n\n")
   flavors_list = mi.get_product_flavors()
   if (len(flavors_list) == 0):
      flavors_list.append("None")
   cmm_script.write("GLOBAL &PRODUCT_FLAVORS\n&PRODUCT_FLAVORS=\"" + ','.join(flavors_list) + "\"\n\n")
   for tag in mi.get_build_list():
      # Don't process the current builds
      if tag != 'common':
         print "   Processing " + tag
         path, attrs = mi.get_build_path(tag, get_attrs=True)
         path = string.rstrip(path, os.sep)
         if attrs and 'cmm_root_path_var' in attrs:
            cmm_var_name = attrs['cmm_root_path_var']
            if path and cmm_var_name:
               image_dir = mi.get_image_dir(tag)
               if image_dir and os.path.exists(os.path.join(path, image_dir)):
                  if tag == 'apps' :
                     if os.path.exists(os.path.join(path, 'LINUX', 'android')) and mi.get_hlos_type().upper() in ['LA','LW','LF','LATV']:
                        path = os.path.join(path, 'LINUX', 'android')
                  cmm_script.write(cmm_tag_template % (cmm_var_name, cmm_var_name, path))


#--------------------------------------------------------------------------------#
# Creating gen_buildinfo.cmm file in common/tools/cmm directory
#--------------------------------------------------------------------------------#

def write_cmm_var(cmm_script, mi):
   cmm_script.write("ENTRY &ARG0\n\n")
   
   # Define all "cmm_var"s and "cmm_file_var"s as GLOBAL variables
   #--------------------------------------------------------------
   
   print "Getting all cmm_var values:"
   var_values = mi.get_var_values('cmm_var')
   for each_var in var_values:
      print "  Writing " + each_var
      cmm_script.write("GLOBAL &" + each_var + "\n")
   
   print "Getting all cmm_file_var values:"
   file_vars = mi.get_file_vars(attr='cmm_file_var', abs=False) 
   for var_name in file_vars:
      print "  Writing " + var_name
      cmm_script.write("GLOBAL &" + var_name + "\n")
   
   # Loop through each flavor and write the values for defined variables
   #--------------------------------------------------------------------
   
   print "Looping through each flavor:"
   flavors_list = mi.get_product_flavors()
   if (len(flavors_list) == 0):
      print "No flavors found, taking 'None' as default flavor."
      flavors_list.append("None")
   
   for flavor1 in flavors_list:
      print "  Processing flavor - " + flavor1
      cmm_script.write("\nIF (\"&ARG0\"==\"" + flavor1 + "\")\n(\n")
      cmm_script.write("\t&PRODUCT_FLAVOR=\"" + flavor1 + "\"\n")
      # Look for cmm_var attributes in current flavor:
      var_values = mi.get_var_values('cmm_var', flavor=flavor1)
      for each_var in var_values:
         cmm_script.write("\t&" + each_var + "=\"" + var_values[each_var] + "\"\n")
      # Look for cmm_file_var attributes in current flavor:
      file_vars = mi.get_file_vars(attr='cmm_file_var', flavor=flavor1, abs=False)
      for var_name in file_vars:
         print "    Writing file list for " + var_name
         file_list = file_vars[var_name]
         for file in file_list:
            if string.find(file, 'LINUX') != -1 and string.find(file, 'android') != -1 :
               dirs = string.split(file, os.sep)
               file = string.join(dirs[2:], os.sep)
            cmm_file_array = ''
            cmm_file_array += file + ';'
            cmm_file_array = cmm_file_array[:-1]
            cmm_script.write("\t&" + var_name + "=\"" + cmm_file_array + "\"\n")      
      cmm_script.write(")") 

      
def create_cmm(targ_root, temp_file, paths_only):
   dst_path = os.path.dirname(temp_file)
   if (dst_path != '' and  # '' implies current directory, and is allowed
       not os.path.isdir(dst_path)):
      print "Can't find destination directory:", dst_path
      print __doc__
      sys.exit(1)

   print "Platform is:" + sys.platform
   print "Python Version is:" + sys.version
   print "Current directory is:" + os.getcwd()
   print "Args are:" + str(sys.argv)

   cmm_script_header = ''
   cmm_tag_template = '''GLOBAL &%s
   &%s="%s"\n
   '''
   cmm_script_trailer = '''\nENDDO'''
   exception = False

   if not targ_root.endswith('/') and not targ_root.endswith('\\'):
      targ_root += '/'
   mi = ml.meta_info()

   cmm_script = open(temp_file, 'w')

   cmm_script.write(cmm_script_header)
   # lg.log("Processing meta info")
   print "Processing meta info"

   if paths_only:
      print "Calling function to create cmm with root paths."
      writepathsonly(cmm_script, mi, cmm_tag_template)  
   else:
      print "Calling function to create cmm with relative paths."
      write_cmm_var(cmm_script, mi)
      cmm_script.write(cmm_script_trailer)

   print "Saving cmm script."
   cmm_script.close()

   if os.path.exists(os.path.join(os.path.dirname(temp_file), 'EXCEPTION.txt')) and exception == True:
      exception_FILE.close()
   else:
      # Create a file TEMP_SUCCESS.txt to indicate the successful completion of TEMP.cmm
      success_txt = os.path.join(os.path.dirname(temp_file), 'SUCCESS.txt')
      success_FILE = open(success_txt, 'w')
      success_FILE.close()




#--------------------------------------------------------------------------------#
#                                      MAIN
#--------------------------------------------------------------------------------#

if __name__ == '__main__':
   paths_only = 0
   if len(sys.argv) == 3:
      targ_root = sys.argv[1]
      temp_file = sys.argv[2]
   elif len(sys.argv) == 4 and sys.argv[3] == "-paths_only":
      targ_root = sys.argv[1]
      temp_file = sys.argv[2]
      paths_only = 1
   else:
      print "Error: Invalid number of parameters."
      print __doc__
      sys.exit()
    
   # Call main function create_cmm
   create_cmm(targ_root, temp_file, paths_only)

