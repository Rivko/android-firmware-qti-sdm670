#============================================================================
#
#/** @file constants.py
#
# GENERAL DESCRIPTION
#   
#
# Copyright 2017 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#**/
#
#----------------------------------------------------------------------------
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who                   what, where, why
# --------   ---------------       ------------------------------------------------------
# 05/11/17   kpa                   relocate sw hw version defines to genxblconfig
# 05/09/17   kpa                   update SIGN_ID
# 03/13/17   dhaval                common constant defines used across modules
#
#============================================================================
import os
import sys
import shutil
import re

GEN_XBL_CONFIG_SCRIPT = "GenXBLConfig.py"
XBL_CONFIG_METADATA_SCRIPT = "XBLconfig_metadata_generator.py"
GUI_ELF_GENERATOR_SCRIPT = "elf_gen_gui.py"
ELF_GENERATOR_SCRIPT = "elf_gen.py"
DEFAULT_SIGN_DIR_ROOT = "/default/"
ITEM_KEY = "file"
FIRST_CFG_FILE_KEY = ITEM_KEY+"1"
ELF_ADDRESS = "ram_address"
AUTO_GEN_ITEM_FOLDER = "auto_gen"
E_CLASS,    E_ENTRY,   EHDR,         PHDR,   ELF_METADAT,    SEGMENT = \
"E_CLASS", "E_ENTRY", "elf_header", "phdr", "elf_metadata", "segment_"
BIN_EXTN = ".bin"
EHDR_BIN = EHDR+BIN_EXTN
PHDR_BIN = PHDR+BIN_EXTN
ELF_METADATA_BIN = ELF_METADAT+BIN_EXTN
INPUT_ELF, DISASSEMBLE_PATH = "input_elf", "disassemble_path"
PAGE_SIZE       = 0
SEGMENT_ALIGN   = 16
ELF32_HDR_SIZE  = 52
ELF32_PHDR_SIZE = 32
ELF64_HDR_SIZE  = 64
ELF64_PHDR_SIZE = 56
ELFINFO_CLASS_INDEX = 4
JSON_CONFIG_FILE_KEY_NAME = "file_name"
ELF_GENERATOR_JSON_FILE_NAME = "elf_config_auto_generated.json"
ELF_GEN_SCRIPT_OUTPUT_RELATIVE_PATH = "/elf_files/create_cli/"
DISASSEMBLED_ELF_INFO_JSON = "disassembled_elf_info.json"
OUT_CREATE_XCFG_JSON = "create_xbl_config.json"
# Image Signing Info
SECTOOLS_SCRIPT = "sectools.py"
SECTOOLS_COMMAND = " secimage "
SECTOOLS_CONFIG_XML = "/config/integration/secimagev2.xml"
SIGN_ID = "xbl_config"
SECTOOLS_IN_USE_SOC_HW_VERSION = "1"

def get_abspath_if_exist(fname, parser, error_msg="missing"): 
  if fname and (os.path.isdir(fname) or os.path.isfile(fname)):
    return os.path.abspath(fname)
  else:
    if error_msg == "missing":
      error_msg = "Directory/File " + fname + " must be supplied and exist."
    parser.error(error_msg)
    return fname

###############################################################################
# create_directory
###############################################################################
def create_directory(directory_name):
  try:
    os.stat(directory_name)
  except:
    try:
      os.mkdir(directory_name)
    except:
      print "\nERROR: An error occurred while creating directory " + \
            directory_name
      exit(-1)               

def call_os_system(command, error_msg="missing"):
  if error_msg == "missing":
    error_msg =  "\nERROR: An error occurred while executing \"" + command + "\""
  if command:
    try:
      os_call_res = os.system(command)
    except:
      print error_msg
      exit(-1)
      
    if os_call_res != 0:
      print error_msg
      exit(-1)

def get_alpha_numeric_only(input_string):
  return re.sub(r'[\\/*?:"<>|]',"", input_string)

def replace_non_alpha_numeric_chars(rep_char, input_string):
  return re.sub(r'[\\/*?:"<>|]', rep_char, input_string)
  
###############################################################################
# correct_path
###############################################################################
def correct_path(path):
  
  # Strip off initial separator
  if path[0] == "/" or path[0] == "\\":
    path = path[1:]

  # Evaluate Linux
  path = path.replace("/", os.path.sep)

  # Evaluate Windows
  path = path.replace("\\", os.path.sep)

  return path;

def remove(fname):
  if os.path.isfile(fname):
    os.remove(fname)      #delete file
  elif os.path.isdir(fname):
    shutil.rmtree(fname)  #delete directory and contents

def print_error_exit(error_msg="missing"):
  if error_msg == "missing":
    error_msg = "\nERROR: An error occurred."
  print error_msg
  exit(-1)

##############################################################################
# roundup - returns offset 'x' with aligment 'precision'
##############################################################################
def roundup(x, precision):
  if precision == 0:
    return x
  else:
    return x if x % precision == 0 else (x + precision - (x % precision)) 
