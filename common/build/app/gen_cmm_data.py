'''
=======================================================================================================
 
gen_cmm_data.py:

 GENERAL DESCRIPTION
    This script is deprecated in favor if gen_cmm_data1.py and gen_cmm_data2.py.
    It is maintained here for backwards compatibility with older versions of the
    Trace32 scripts.
    
Usage:
   gen_cmm_data <targ_root> <temp_cmm_script_file_name>
   Examples:
      gen_cmm_data ..\\..\\.. temp_fname.cmm

Description:
   gen_cmm_data.py can be called from a CMM script to generate a temporary CMM
   script to initialize path information to the component builds.

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 
-----------------------------------------------------------------------------------------------
  $Header: //components/rel/build.glue/1.0/app/gen_cmm_data.py#3 $
  $DateTime: 2017/09/20 18:48:25 $
===============================================================================================
'''
import sys
import os
import os.path
sys.path.append('./lib')
import meta_log as lg

if len(sys.argv) == 3:
   targ_root = sys.argv[1]
   temp_file = sys.argv[2]
else:
   print "Error: Invalid number of parameters."
   print __doc__
   sys.exit()

dst_path = os.path.dirname(temp_file)
if (dst_path != '' and  # '' implies current directory, and is allowed
    not os.path.isdir(dst_path)):
   print "Can't find destination directory:", dst_path
   print __doc__
   sys.exit(1)
logfile = os.path.join(dst_path, 'gen_cmm_data')

lg = lg.Logger(logfile, save_old_log_file=False)

lg.log("Platform is:" + sys.platform)
lg.log("Python Version is:" + sys.version)
lg.log("Current directory is:" + os.getcwd())
lg.log("Args are:" + str(sys.argv))

cmm_script_header = ''
cmm_tag_template = '''GLOBAL &%s
&%s="%s"
'''
cmm_script_trailer = '''ENDDO'''
exception = False
sys.path.append(os.path.dirname(__file__))
import meta_lib as ml

if not targ_root.endswith('/') and not targ_root.endswith('\\'):
   targ_root += '/'
mi = ml.meta_info(logger=lg)

cmm_script = open(temp_file, 'w')

cmm_script.write(cmm_script_header)
lg.log("Processing meta info")

# Look for generic cmm_var attributes
var_values = mi.get_var_values('cmm_var')
for var in var_values:
   cmm_script.write(cmm_tag_template % (var, var, var_values[var]))

flavors_list = mi.get_product_flavors()
if flavors_list:
   default_flavor = flavors_list[0]
else:
   default_flavor = None

# Look for cmm_file_var attributes:
file_vars = mi.get_file_vars(attr='cmm_file_var', flavor=default_flavor, abs=False)
for var_name in file_vars:
   file_list = file_vars[var_name]
   cmm_file_array = ''
   for file in file_list:
      cmm_file_array += file + ';'
   cmm_file_array = cmm_file_array[:-1]
   cmm_script.write(cmm_tag_template % (var_name, var_name, cmm_file_array))

# Look for special cmm_root_path_var attributes.  These require special
# processing to verify that it's actually pointing to a valid build.
for tag in mi.get_build_list():
   # Don't process the current build
   if tag != 'common':
      lg.log("   Processing " + tag)
      path, attrs = mi.get_build_path(tag, get_attrs=True)
      if attrs and 'cmm_root_path_var' in attrs:
         cmm_var_name = attrs['cmm_root_path_var']
         if path and cmm_var_name:
            image_dir = mi.get_image_dir(tag)
            if image_dir and os.path.exists(path + image_dir):
               cmm_script.write(cmm_tag_template % (cmm_var_name, cmm_var_name, path))
            elif image_dir and (path != mi.get_build_path('common')):
               if not os.path.exists(os.path.join(os.path.dirname(temp_file), 'EXCEPTION.txt')):
                  exception_txt = os.path.join(os.path.dirname(temp_file), 'EXCEPTION.txt')
                  exception_FILE = open(exception_txt, 'w')
                  # Temporary hack until ravi kumar delivers a cmm script which removes the exception.txt if it already exists.
                  exception = True
                  exception_FILE.write("The following build paths are missing LOGIN rights or have been zipped : \n")
               exception_FILE.write(cmm_var_name + " : " + path + "\n")
            else:
               cmm_script.write(cmm_tag_template % (cmm_var_name, cmm_var_name, ''))

     
# Write the trailer for the file
cmm_script.write(cmm_script_trailer)
cmm_script.close()

if os.path.exists(os.path.join(os.path.dirname(temp_file), 'EXCEPTION.txt')) and exception == True:
   exception_FILE.close()
else:
   # Create a file TEMP_SUCCESS.txt to indicate the successful completion of TEMP.cmm
   success_txt = os.path.join(os.path.dirname(temp_file), 'SUCCESS.txt')
   success_FILE = open(success_txt, 'w')
   success_FILE.close()


