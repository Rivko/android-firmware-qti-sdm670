"""
   Executes dsp tests from provided adsp_proc location
   Script assumes that the test framework has already been compiled and ready to run.
"""
import re
import os
import os.path
import sys
import shutil
from subprocess import Popen, PIPE
"""
Find the test suite name
"""

"""
Some fixed string 
"""
ADSP_TEST_RUN = "dsp_tfw_run.cmd "
SPACE = " "
CMD_OPTION_SIM = " --sim "
CMD_OPTION_CONFIG = " -- -config "
RESULTS_LOG_NAME = "tfw_results.log"
FWK_EXE_PATH = "..\\..\\platform\\testframework\\WinNT_Debug_msvc9"
FWK_EXE_NAME = "libadsprpc.exe"
CONFIG_PATH = "..\\..\\platform\\testframework\\tests\\WinNT_Debug\\"
SLASH =  "\\"
ADSP_SIM_DIR = "\\avs\\aDSPSIM"

"""
Function
   verify_path_exists

Description
   Verifies that specified path exists. Script ends if path not found.

Input
   path        -  Path to verify

Output
   None
"""
def verify_path_exists(path):
   if not if_dir_exists(path):
      sys.exit()

"""
Function
   err_log

Description
   Prints a formated error message.

Input
   msg        -  Error message to print

Output
   None
"""
def err_log(msg):
   print ("\tERROR ==> " + msg + "\n")

"""
Function
   msg_log

Description
   Prints a formated message.

Input
   msg        -  Message to print

Output
   None
"""
def msg_log(msg):
   print ("\t" + msg + "\n")

"""
Function
   get_test_suite_name

Description
   Retrieves a test suite name based on provided test suite description.

Input
   line        -  Line of text to parse
   test_desc   -  Test description text
   suite_desc  -  Suite description text

Output
   Matched string if found, 
   emtpy string otherwise
"""
def get_test_suite_name(line, test_desc, suite_desc):
   line = line.rstrip()
   if re.search(test_desc, line):
      test_suite_name = line.split()[-1]
      match = re.search(suite_desc, line)
      return match.group(0)
   else:
      return ""


"""
Function
   open_file

Description
   Attempts to open a file.

Input
   fileName    -  Name of file
   mode        -  Open mode 

Output
   Handle to opened file, 
   exception on error otherwise
"""
def open_file(fileName, mode):
   try:
      return open(fileName, mode)
   except IOError, e:
      print (e)


"""
Function
   search_test_name

Description
   Searches for a test name in text line.

Input
   line        -  Text line
   substr      -  String to search for

Output
   Match string if found, 
   empty string otherwise
"""
def search_test_name(line, substr):
   match = re.search(substr, line)
   if match:
      return match.group(2)
   else:
      return ""

"""
Function
   get_test_result

Description
   Searches for test result.

Input
   line        -  Text line

Output
   Test result string
"""
def get_test_result(line):
   match = re.search("passed", line)
   if match:
      return "PASS"
   else:
      match = re.search("failed", line)
      if match:
         return "FAILED"
      else:
         return "UNDETERMINED"

"""
Function
   create_suite_file

Description
   Creates a test results file.

Input
   filename    -  File name to create
   fm          -  File mode

Output
   File handle 
"""
def create_suite_file(filename, fm):
   xml_file = open_file(filename, fm)
   xml_file.write('<testsuites>\n')
   return xml_file

"""
Function
   write_test_result

Description
   Writes a test result in test results file.

Input
   fp          -  Test results file pointer
   sn          -  Test suite name
   tn          -  Test name
   res         -  Test result

Output
   None
"""
def write_test_result(fp, sn, tn, res):
   fp.write('\t<testsuite name="' + sn + '">\n')
   line = '\t\t<testcase name="' + tn + '" status="' + res + '" time="1"/>\n'
   fp.write(line)
   fp.write('\t</testsuite>\n')


"""
Function
   if_file_exists

Description
   Verifies file existance.

Input
   fileName    -  File name

Output
   True if file found, 
   false otherwise
"""
def if_file_exists(fileName):
   if not os.path.isfile(fileName):
      err_log("File [ " + fileName + " ] does not exist!")
      return False
   else:
      return True
   
"""
Function
   if_dir_exists

Description
   Verifies directory existance.

Input
   dirName     -  Directory name

Output
   True if file found, 
   false otherwise
"""
def if_dir_exists(dirName):
   if not os.path.isdir(dirName):
      err_log("Directory [ " + dirName + " ] does not exist!")
      return False
   else:
      return True

      
"""
Function
   create_results_file

Description
   Creates a test results file with all test results.

Input
   log_file             -  Test run log file name
   results_file_name    -  Results file name

Output
   None
"""
def create_results_file(log_file, results_file_name):
   fp_xml_created=None
   bool_pass_result = 0
   bool_fail_result = 0
   test_name = ""
   test_desc = "(Test) (\w+) in DLL "
   suite_desc = "\\w+\.dll"

   # Open the results log file
   dsp_log = open_file(log_file, 'r')
   while 1:
      line = dsp_log.readline()
      if not line:
         break

      sn = get_test_suite_name(line, test_desc, suite_desc);
      if "" == sn: 
         continue

      test_name = search_test_name(line, test_desc)
      if "" == test_name: 
         continue

      if (None == fp_xml_created):
         fp_xml_created = create_suite_file(results_file_name, 'w')

      result = get_test_result(line)
      if ("PASS" == result):
         write_test_result(fp_xml_created, sn, test_name, result)
         bool_pass_result = 1
      else:
         write_test_result(fp_xml_created, sn, test_name, result)
         bool_fail_result = 1

   if (fp_xml_created):
      fp_xml_created.write('</testsuites>\n')
      fp_xml_created.close()

   if (1 == bool_pass_result and 1 == bool_fail_result):
      os.rename(xml_file_name, "TEST_FAIL.xml")
   elif (0 == bool_pass_result and 1 == bool_fail_result):
      if (os.path.isfile(xml_file_name)):
         os.remove("TEST_FAIL.xml")
      os.rename(xml_file_name, "TEST_FAIL.xml")

   dsp_log.close()

"""
Function
   compose_full_path

Description
   Verifies provided path and converts it to a full path if path is relative.

Input
   path        -  Test run log file name

Output
   Directory path
"""
def compose_full_path(path):
   # If relative path specified, compose full path here
   string = path
   if (string.startswith(".\\") or 
      string.startswith("..\\") or
      string.startswith(".")):
      fwk_dir = os.getcwd() + SLASH
      fwk_dir = fwk_dir + SLASH + sys.argv[1]
   else:
      fwk_dir = sys.argv[1] 

   return fwk_dir


def copy_file(src, dst):
   file = os.path.basename(xml_file_name)
   if (dst):
      print ("Copying file " + src + " to " + dst)
      try:
         shutil.copy(src, dst)
         #shutil.copy(xml_file_name, "D:\\Path_relative\\testres")
      except IOError, e:
         print ("Unable to copy file" % e)

      print ("File copied")


""" 
Script start
"""

if len(sys.argv) < 2:
   msg_log("Usage: dsp_sanity_cmd.py <adsp_proc_root>")
   sys.exit()


adsp_fwk_dir = compose_full_path(sys.argv[1])
verify_path_exists(adsp_fwk_dir)

adsp_sim_dir = adsp_fwk_dir + ADSP_SIM_DIR
verify_path_exists(adsp_sim_dir)

fwk_exe_full_path = adsp_sim_dir + SLASH + FWK_EXE_PATH 
verify_path_exists(fwk_exe_full_path)
config_full_path =  adsp_sim_dir + SLASH + CONFIG_PATH 
verify_path_exists(config_full_path)

# List of tests to execute
config_files = [
      "rpcversion.cfg",
      "rfstests.cfg",
      "rsatests.cfg",
      "calculator_test.cfg",
      "test_dl_test.cfg",
      ]

# Directory where script resides
script_cwd = os.getcwd() + SLASH 
script_cwd, dummy = os.path.split(os.path.abspath(__file__))
local_log = script_cwd + SLASH + RESULTS_LOG_NAME 
xml_file_name = script_cwd + SLASH + "Test_Pass.xml"
xml_file_fail = script_cwd + SLASH + "Test_Fail.xml"

# Remove old files, if any
if (os.path.isfile(local_log)):
   os.remove(local_log)

if (os.path.isfile(xml_file_name)):
   os.remove(xml_file_name)

if (os.path.isfile(xml_file_fail)):
   os.remove(xml_file_fail)

fwk_exe_cmd = CMD_OPTION_SIM + FWK_EXE_PATH
for cfg in config_files:
   if (if_file_exists(config_full_path + cfg)):
      config_cmd = CMD_OPTION_CONFIG + CONFIG_PATH + SLASH + cfg
      
      run_cmd = script_cwd + SLASH + ADSP_TEST_RUN + adsp_fwk_dir + SPACE + cfg + SPACE + local_log
      msg_log ("Executing test for : " + cfg)
      p = Popen(run_cmd, shell=True) 
      p.communicate()
   else:
      err_log("Config file does not exist : [ " + cfg + " ]")

create_results_file(local_log, xml_file_name)

if (len(sys.argv) >= 3):
   copy_file(xml_file_name, sys.argv[2])

if (len(sys.argv) == 4):
   copy_file(xml_file_name, sys.argv[3])
