#! /usr/bin/python
'''
=======================================================================================================
meta_cli.py: Command Line Interface for meta_lib functions

Description:
    This tool/utility provides a command line interface for meta_lib functions. 

    The idea is to provide access of meta_lib functions to third party ( or automation teams) 
    to process information in contents.xml or to make use of different api's to work with Glue build

Usage:

   meta_cli.py <function> <arguments> [--doc]
   function:   Name of the function.
               This function has to exist in meta_lib, it will be created.

   arguments:  These are arguments for the above function. 
               The arguments should be followed after function.
            
   --doc:      Prints the documentation of the function 
   
   --list_all  ??
   
   --return ...??
   
   -- logging ...??

 Copyright (c) 2016-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/app/meta_cli.py#11 $
 $DateTime: 2018/03/26 15:21:09 $
===============================================================================================
'''
def pack_input_args(input_list): 
   
   if all ('=' in str(each)  for each in input_list) : 
      argss = dict()
      for each_input in input_list :
         param = each_input.split('=',1)[0]
         value = each_input.split('=',1)[1].replace('\\','\\\\') # Adjust back slash network path
         # None string data type value check
         bool_none_val = (value == 'True' or value == 'False' or value == 'None')
         if not value.startswith("\'") and not value.startswith('\"') and param not in ['file_types','var_values','cur_elem'] and not bool_none_val:
            value = '\''+value.strip()+ '\''
         argss[each_input.split('=', 1)[0]] = ast.literal_eval(value)
   else : 
      argss = input_list
      
   #print argss
   return argss

def print_documentation(func):
   print str(func.__doc__)
   argss, varargs, keywords, defaults = inspect.getargspec(func)
   print "input arguments = ", argss
   print "varargs = ", varargs
   print "keywords =", keywords 
   print "defaults = ", defaults 
   formated_arg = inspect.formatargspec(argss, varargs, keywords, defaults)
   formated_arg = str.rstrip(str.lstrip(formated_arg, '('), ')')
   f_args = formated_arg.split(',')
   if f_args[0] == 'self' :
      f_args.remove('self')
   for i in range(len(f_args)) : 
      if '=' in f_args[i] : 
         continue
      else : 
         f_args[i] = f_args[i] + ' = <userinput>'
   out_string = os.path.basename(__file__) + ' ' + func.__name__ + ' ' + ' '.join(f_args)
   print "\nsuggested Command-Line-Interface usage:"
   print '\t', out_string
    
def print_dir(ml, mi):
   print ("meta_cli : list of supported functions : \n")
   #for method in dir(mi): 
      #print method
   #for method in dir(ml):
      #print method
   api_list = ["get_version", "get_chipids", "get_product_info", "get_product_name", "get_build_id", 
   "get_hlos_type", "get_build_path", "get_build_list", "get_product_flavors", "get_storage_types", 
   "get_si_build_info_list", "get_build_bid_var_list", "get_meta_build_info", "get_partition_files", 
   "get_files", "get_file_vars", "get_cust_meta_source" ,"get_filenames_from_partitionfile", "get_sparse_image_command",
   "get_tool_build", "get_tool_path", "get_wf_step_filter_list", "get_wf_step_filter_list_by_build",
   "get_attr_values", "gen_minimized_build"]
   for method in api_list:
      print (method)


# ##***    Main
import sys, os, os.path, inspect, json, ast
from optparse import OptionParser

# ##***    process input arguments
input_string = sys.argv[1:]

parser = OptionParser("usage: meta_cli.py <function> <arguments> [--doc]")
parser.add_option("--doc", action="store_true", dest="print_doc", help="For printing function's documentation", default=False)
parser.add_option("--dir", action="store_true", dest="print_dir", help="For printing list of functions supported", default=False)
parser.add_option("--dbg", action="store_true", dest="debug_msg", help="For printing meta_cli debug messages", default=False)
parser.add_option("--contentsxml", action="store", dest="contents_xml_path", type="string", help="Contents XML path for standalone mode execution")
parser.add_option("--fb_nearest", action="store_true", dest="fb_nearest",help="Find nearest build path", default=False)

(options, args) = parser.parse_args()

#If contents XML present argument list, then it suppose to execute standalone mode outside of meta location 
if (options.contents_xml_path !=None):
   # current working directory outside meta location, all dependent package should be included in standalone executable 
   if(os.path.exists(options.contents_xml_path)):
      sys.path.append(os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '../lib')))
      contents_xml_rel_path = options.contents_xml_path
   else:
      print >> sys.stderr, "Failed to validate contents.xml path"
      sys.exit(0)
else:
   if os.path.exists(os.path.join(os.path.dirname(sys.argv[0]), '../lib'))  and os.path.exists(os.path.join(os.path.dirname(sys.argv[0]), '../../../contents.xml')):
      # current working directory should be <root>/common/build/app
      sys.path.append(os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '../lib')))
      contents_xml_rel_path = '../../../contents.xml'
   elif  os.path.exists(os.path.join(os.path.dirname(sys.argv[0]), '../../lib'))  and os.path.exists(os.path.join(os.path.dirname(sys.argv[0]), '../../../../contents.xml')) :
      # current working directory should be <root>/common/build/app/<folder>
      sys.path.append(os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '../../lib')))
      contents_xml_rel_path = '../../../../contents.xml'
   else: 
      print >> sys.stderr, "Failed locating contents.xml"
      sys.exit(0)

import meta_lib as ml
import meta_log as lg

# Create log and meta interface 
lg = lg.Logger(log_to_stdout=options.debug_msg, save_old_log_file=False)
mi = ml.meta_info(file_pfn=os.path.join(os.path.dirname(sys.argv[0]), contents_xml_rel_path), logger=lg, fb_nearest=options.fb_nearest)

if options.debug_msg:
   lg.log("meta_cli : meta_info object created with " + contents_xml_rel_path)

if len(sys.argv[1:]) < 1: 
   parser.error("invalid usage. \nFor help , please try: \nmeta_cli.py -h")

if options.print_doc:
   input_string.remove('--doc')
if options.print_dir:
   input_string.remove('--dir')
   print_dir(ml, mi)
   sys.exit(0)

if options.fb_nearest:
   input_string.remove('--fb_nearest')   
if options.debug_msg:
   input_string.remove('--dbg')
if options.contents_xml_path:
   input_string.remove('--contentsxml='+options.contents_xml_path)
   lg.log("meta_cli : Executing standalone mode")
else:
   lg.log("meta_cli : Executing from meta build location")

if (len(input_string) < 1) and (options.print_dir):
   sys.exit(0)
elif (len(input_string) < 1) :
   parser.error("invalid usage. \nFor help , please try: \nmeta_cli.py -h")

func_string = input_string[0]
args = pack_input_args (input_string[1:])


# ##***    scan for funciton in meta_lib and meta info
found_object = None
if func_string in dir(ml):
   found_object = ml
elif func_string in dir(mi):
   found_object = mi
else: 
   print >> sys.stderr, "Could not find the function : " + func_string
   sys.exit(0)

func = getattr(found_object, func_string)

if options.print_doc :
   print_documentation(func) 
   sys.exit(0)
   
if options.debug_msg:
   lg.log("meta_cli : calling " + func_string + " in " + str(found_object) + "with arguments :" + ''.join(args))

# ##***    Unpack the arguments with * or **  
try:
   if type(args) is list: 
      output = func(*args)
   else : 
      output = func(**args)
   #This section needs to be updated and modified before generate the standalone executable
   #if func_string == 'get_version':
      #comp_list = []
      #comp_version = {}
      #comp_version['component'] = 'build.glue.X.X'
      #comp_version['version'] = '00XXX'
      #comp_list.append(comp_version)
      #output = comp_list
   
except TypeError as err:
   print >> sys.stderr, "Fatal error!! \nTypeError exception :", err.args
   print >> sys.stderr, "please try: \nmeta_cli.py ", func_string, " --doc"
   sys.exit(0)
except Exception as ex:
   err_msg = "{0} exception. \nArguments:{1!r}"
   print >> sys.stderr, err_msg.format(type(ex).__name__, ex.args)
   sys.exit(0)

if options.debug_msg:
   lg.log("meta_cli : function call returned")

# for each_line in output: 
   # print(each_line)
   # print output[each_line]
# print "######\n"
print json.dumps(output, indent=2)

# output = mi.get_files(file_types='download_file')
# print output
# return output

