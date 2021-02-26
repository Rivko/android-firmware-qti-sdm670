#!/usr/bin/env python


#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 05/11/17   kpa     Add command line options for soc hw version for signing
# 05/09/17   kpa     Update image signing
# 03/15/17   dhaval  Added disassembling XBL_Config support.
# 02/28/17   kpa     Add tools path, support standalone image generation
# 01/27/17   kpa     Add signing support, fix elf entry point population, seg type
# 10/31/16   ck      Initial release


from optparse import OptionParser
from subprocess import call
import shutil
import os
import sys
import locale
import re
import time
import datetime
import json
from collections import OrderedDict
import commons
from commons import *

if sys.version_info < (2, 7, 3):
  print "Must use Python 2.7.3 or greater"
  sys.exit(1)

def disassemble_xbl_config_file(config_file_to_be_disassembled, 
                                format, 
                                output_xbl_config_directory, 
                                autogen_directory,
                                tools_path):
  # Create output based on format
  if (format == "ELF"):
    disassemble_elf(config_file_to_be_disassembled,
                    format,
                    output_xbl_config_directory,
                    autogen_directory,
                    tools_path)
  else:
    print "\nBinary output is currently not supported."

def generate_xbl_config_file(json_config_path,
                             format, 
                             elf_address, 
                             output_xbl_config_filename,
                             base_directory, 
                             output_xbl_config_directory,
                             sectools_directory, 
                             autogen_directory,
                             tools_path,
                             soc_hw_version,
                             soc_vers):
  # Read JSON config data
  try:
    with open(json_config_path) as json_file:    
      json_data = json.load(json_file, object_pairs_hook=OrderedDict)
  except:
    print "\nERROR: An error occurred while reading JSON config file.  " + \
          "Please check contents."
    exit(-1)
  # Call XBL config metadata generator to construct the config headers
  call_os_system("python " + tools_path + "/" + XBL_CONFIG_METADATA_SCRIPT + \
                 " -i " + json_config_path + \
                 " -b " + base_directory + \
                 " -o " +  output_xbl_config_directory)
  # Verify that all binary blobs were generated using JSON config entries
  for current_CFG in json_data:
    if not os.path.isfile(output_xbl_config_directory + "/" + current_CFG + ".bin"):
      print "\nERROR: XBLconfig_metadata_generator.py did not generate " + \
            "expected binary files: " + output_xbl_config_directory + "/" + current_CFG + ".bin"
      exit(-1)
    # if elf_address is 0 and if ELF_ADDRESS is captured in 
    # first cfg_meta item's first item then use that as elf_address
    if (elf_address=="0x0") and (FIRST_CFG_FILE_KEY in json_data[current_CFG]) \
                         and (ELF_ADDRESS in json_data[current_CFG][FIRST_CFG_FILE_KEY]):
      elf_address = json_data[current_CFG][FIRST_CFG_FILE_KEY][ELF_ADDRESS]

  # Create output based on format
  if (format == "ELF"):
    try:
      create_elf(base_directory,
                 json_data,
                 autogen_directory,
                 int(elf_address, base=16),
                 output_xbl_config_filename,
                 tools_path,
                 output_xbl_config_directory)
    except:
      print "\nERROR: Exception occurred for create_elf function call."
      exit(-1)
    sign_elf(sectools_directory,
             autogen_directory,
             output_xbl_config_filename,
             output_xbl_config_directory,
             soc_hw_version,
             soc_vers)
      
  else:
    print "\nBinary output is currently not supported."

###############################################################################
# sign_elf
###############################################################################
def sign_elf(sectools_directory,
             autogen_directory,
             output_xbl_config_filename,
             output_xbl_config_directory,
             soc_hw_version,
             soc_vers):
  try:
    if sectools_directory and os.path.isdir(sectools_directory):
      filepath_to_be_signed = autogen_directory + \
                            ELF_GEN_SCRIPT_OUTPUT_RELATIVE_PATH + \
                    output_xbl_config_filename
      print "************* Sign generated Elf ************\n"
        
      call_os_system("python " + sectools_directory + "/" + SECTOOLS_SCRIPT + SECTOOLS_COMMAND
                          + " -i " + filepath_to_be_signed
                          + " -o " + autogen_directory
                          + " -sa "
                          + " -g " + SIGN_ID 
                          +" --config " + sectools_directory + SECTOOLS_CONFIG_XML
                          +" --cfg_soc_hw_version " + soc_hw_version
                          +" --cfg_in_use_soc_hw_version " + SECTOOLS_IN_USE_SOC_HW_VERSION
                          +" --cfg_soc_vers " + "\"" + soc_vers + "\"" )
      
      shutil.copy((autogen_directory + DEFAULT_SIGN_DIR_ROOT + SIGN_ID 
                    + "/" + output_xbl_config_filename)
                   ,output_xbl_config_directory)
      print "\n Signed image: " + (output_xbl_config_directory + "/" + output_xbl_config_filename + "\n")
                      
      print "*************  Signing Successful   ************"
    else:
      print "\n\n    !!!!! No Sectools Folder provided. SKIPPING IMAGE SIGNING !!!!!"
      print "\n    ERROR: Could not generate Signed XBL Config  \n\n"
  except:
    print "\nERROR: An error occurred while Signing the ELF file."
    exit(-1)       
  

###############################################################################
# disassemble_elf
###############################################################################
def disassemble_elf(config_file_to_be_disassembled,
                    format,
                    output_xbl_config_directory,
                    autogen_directory,
                    tools_path):
  disassembled_elf_info_json = os.path.join(output_xbl_config_directory, DISASSEMBLED_ELF_INFO_JSON)
  # Execute the ELF Generator script to disassemable XBL-config elf
  call_os_system("python " + tools_path + "/" +  ELF_GENERATOR_SCRIPT + \
                 " -d " + config_file_to_be_disassembled + \
                 " -o " + autogen_directory + \
                 " -e " + disassembled_elf_info_json)

  out_create_xcfg_json = os.path.join(output_xbl_config_directory, OUT_CREATE_XCFG_JSON)
  # Call XBL config metadata generator to locate and parse xbl-config-metadata from
  #    disassembled elf segments, generate out_create_xcfg_json with 
  #    xbl-config-items' file-name, config_name etc details
  call_os_system("python " + tools_path + "/" + XBL_CONFIG_METADATA_SCRIPT + \
                 " -d " + disassembled_elf_info_json + \
                 " -o " + output_xbl_config_directory + \
                 " -c " + out_create_xcfg_json)

  # prepare genxblcfg_command to display which can be used by user to re-generate
  #   xbl_config.elf using disassembled info "out_create_xcfg_json" as input
  elf_address = "0x[elf_entry_point]" #default
  if not os.path.isfile(disassembled_elf_info_json):
    print_error_exit("\nError: Disassembled elf info " + str(disassembled_elf_info_json) + " file must be valid and exist.")
  with open(disassembled_elf_info_json) as disassembled_elf_info_json_file:    
    disassembled_bins_info = json.load(disassembled_elf_info_json_file, object_pairs_hook=OrderedDict)
    if E_ENTRY in disassembled_bins_info:
      elf_address = str(disassembled_bins_info[E_ENTRY])
    disassembled_elf_info_json_file.close()
  genxblcfg_command = "python " + tools_path + "/" + GEN_XBL_CONFIG_SCRIPT + \
                 " -i " + out_create_xcfg_json + \
                 " -b " + output_xbl_config_directory + \
                 " -f ELF " + \
                 " -o " + str(os.path.join(output_xbl_config_directory,"xbl_config")) + \
                 " --sectools_directory " + str(os.path.join(tools_path,"sectools")) #+ \
#                 " --elf-address " + (elf_address)
  
  print "\nDisassembled XBL config items and .json can be found in directory:\"" + \
         output_xbl_config_directory + "\"." + \
        "\n\nTo create xbl_config.elf from disassembled items, please execute below command:\n" + \
        "\"" + str(genxblcfg_command) + "\""
                 
###############################################################################
# create_elf
###############################################################################
def create_elf(base_directory,
               json_data,
               output_directory,
               elf_address,
               output_filename,
               tools_path,
               output_xbl_config_directory):

  segment_count = 0
  current_offset = 0
  current_address = elf_address

  # Calculate how many segments will be created
  # Main section does not need a metadata segment so start with -1
  for CFG_section in json_data:
    # Add one for metadata segment
    segment_count += 1

    # Add ubjson segments
    segment_count += len(json_data[CFG_section])


  # Construct initial JSON for elf generator
  elf_json = {}
  
  elf_json.update({"config_output_path" : output_directory})
  elf_json.update({"config_output_file" : output_filename})
  
  elf_json.update({"elf_header" : {"e_ident" : {"ei_class" : "0x2",
                                                "ei_version" : "0x0"},
                                   "ph_num" : str(hex(segment_count).rstrip("L")),
                                   "type" : "0x2",
                                   "machine" : "0x1",
                                   "entry_addr" : str(hex(elf_address).rstrip("L")),
                                   "ph_offset" : "0x40",
                                   "sh_offset" : "0x1",
                                   "flags" : "0x5",
                                   "sh_size" : "0x0",
                                   "sh_num" : "0x0"},
                   "program_header" : {"segment_alignment_offset" : "0x1",
                                       "segment" : list()}})

  
  # Add segments to program header dictionary
  for CFG_section in json_data:
    # Add metadata segment
    elf_json["program_header"]\
            ["segment"].append({"type" : "0x0",
                                "offset" : str(hex(current_offset).rstrip("L")),
                                "vaddr" : str(hex(current_address).rstrip("L")),
                                "paddr" : str(hex(current_address).rstrip("L")),
                                "file_size" : str(hex(os.path.getsize(os.path.join(output_xbl_config_directory,
                                                                                   correct_path(CFG_section + ".bin")))).rstrip("L")),
                                "mem_size" : str(hex(os.path.getsize(os.path.join(output_xbl_config_directory,
                                                                                  correct_path(CFG_section + ".bin")))).rstrip("L")),
                                "flags" : "0x7",
                                "access_type" : "0x0",
                                "segment_type" : "0x1",
                                "addr_alignment" : "0x1",
                                "binary" : str(os.path.join(output_xbl_config_directory, correct_path(CFG_section + ".bin"))),
                                "binary_class" : "0x2"})


    # Update the current_offset and address
    current_offset += os.path.getsize(os.path.join(output_xbl_config_directory, correct_path(CFG_section + ".bin")))
    current_address += os.path.getsize(os.path.join(output_xbl_config_directory, correct_path(CFG_section + ".bin")))
    
    # Add all files/segments
    for current_file in json_data[CFG_section]:
      elf_json["program_header"]\
              ["segment"].append({"type" : "0x0",
                                  "offset" : str(hex(current_offset).rstrip("L")),
                                  "vaddr" : str(hex(current_address).rstrip("L")),
                                  "paddr" : str(hex(current_address).rstrip("L")),
                                  "file_size" : str(hex(os.path.getsize(os.path.join(base_directory,
                                                                                     correct_path(json_data[CFG_section][current_file][JSON_CONFIG_FILE_KEY_NAME])))).rstrip("L")),
                                  "mem_size" : str(hex(os.path.getsize(os.path.join(base_directory,
                                                                                    correct_path(json_data[CFG_section][current_file][JSON_CONFIG_FILE_KEY_NAME])))).rstrip("L")),
                                  "flags" : "0x7",
                                  "access_type" : "0x0",
                                  "segment_type" : "0x1",
                                  "addr_alignment" : "0x1",
                                  "binary" : str(os.path.join(base_directory,
                                                              correct_path(json_data[CFG_section][current_file][JSON_CONFIG_FILE_KEY_NAME]))),
                                  "binary_class" : "0x2"})


      # Update the current_offset
      current_offset += \
        os.path.getsize(os.path.join(base_directory,
                                     correct_path(json_data[CFG_section][current_file][JSON_CONFIG_FILE_KEY_NAME])))

      current_address += \
        os.path.getsize(os.path.join(base_directory,
                                     correct_path(json_data[CFG_section][current_file][JSON_CONFIG_FILE_KEY_NAME])))

  ELF_Generator_JSON_File = output_directory + "/" + ELF_GENERATOR_JSON_FILE_NAME
  
  # Write ELF JSON file out
  with open(ELF_Generator_JSON_File, "w") as elf_json_file:
    json.dump(elf_json, elf_json_file, separators=(',', ':'), indent=4)


  # Execute the ELF Generator script
  call_os_system("python " + tools_path + "/" +  ELF_GENERATOR_SCRIPT + " --cfg " + ELF_Generator_JSON_File)

  return

##############################################################################
# main
##############################################################################
if __name__ == "__main__":
  error_count = 0  

  parser = OptionParser()

  parser.add_option("-a", "--autogen_directory",
                    action="store", type="string", dest="autogen_directory",
                    help="Temporary folder, scratch space for auto generated files.")

  parser.add_option("-b", "--config_item_base_directory",
                    action="store", type="string", dest="base_directory",
                    help="Base directory for relative paths of config items in input .json.")

  parser.add_option("-s", "--sectools_directory",
                    action="store", type="string", dest="sectools_directory",
                    help="Needs to point to sectools folder used for signing.")        
        
  parser.add_option("-t", "--tools_path",
                    action="store", type="string", dest="tools_path",
                    help="Path to tools directory containing dependent *.py scripts.")
        
  parser.add_option("-f", "--format",
                    action="store", type="string", dest="format",
                    help="Output Format: ELF or BIN")

  parser.add_option("-e", "--elf-address",
                    action="store", dest="elf_address", default="0x0",
                    help="If format is ELF then this is the beginning " + \
                         "address.  Default is 0x00000000.")

  parser.add_option("-i", "--input-json",
                    action="store", type="string", dest="json_config_path",
                    help="Input JSON config file.")

  parser.add_option("-o", "--output-file-without-extension OR output-directory with -d option used",
                    action="store", dest="output_file", default="XBL_Config",
                    help="Output file name without extention.  " + \
                    "Default is XBL_Config")

  parser.add_option("-d", "--input-config-file-to-disassemble",
                    action="store", dest="config_file_to_be_disassembled",
                    help="Command to disassemble with input config file.")

  parser.add_option("-j", "--soc-hw-version",
                    action="store", dest="soc_hw_version", default="0x60000000",
                    help="soc hw version for xblconfig image signing. " + \
                         "Default is 0x60000000.")
                         
  parser.add_option("-k", "--soc-vers",
                    action="store", dest="soc_vers", default="0x6000 0x6001",
                    help="soc vers for xblconfig image signing " + \
                         "Default is 0x6000 0x6001.")                         

  (options, args) = parser.parse_args()

  # Format must be supplied and be valid
  if not options.format or \
     (options.format != "ELF" and options.format != "BIN"):
    parser.error("Format must be supplied and be ELF or BIN")

  if not options.config_file_to_be_disassembled: #xbl_config generation case
    if options.output_file and \
       os.path.isdir(os.path.split(os.path.abspath(options.output_file))[0]):
      output_xbl_config_directory = os.path.split(os.path.abspath(options.output_file))[0]
    else:
      parser.error("Output filename path without extension must be valid and exist.")
  else: #xbl_config disassemable case
    output_xbl_config_directory = get_abspath_if_exist(options.output_file, parser, "Output directory must be valid and exist.")

  # Auto generated files directory is optional. Process file path level to
  # contain intermediate files
  if options.autogen_directory and os.path.isdir(options.autogen_directory):
    autogen_directory = os.path.join(os.path.abspath(options.autogen_directory), AUTO_GEN_ITEM_FOLDER)
  else:
    autogen_directory = os.path.join(output_xbl_config_directory, AUTO_GEN_ITEM_FOLDER)
  remove(autogen_directory)
  create_directory(autogen_directory)
  # Dependent Tools (XBLconfig_metadata_generator, elf_gen etc.) path directory 
  if (not options.tools_path) or (not os.path.isdir(options.tools_path)):
    options.tools_path = os.getcwd()
  options.tools_path = os.path.abspath(options.tools_path)
  
  # if XBL config disassemble option is not passed.
  if not options.config_file_to_be_disassembled:
    # Base directory must be supplied and exist
    options.base_directory = get_abspath_if_exist(options.base_directory, parser, "Base directory must be supplied and exist.")
    # check directory portion of the output filename path is valid and exist
    if not options.output_file or \
        not os.path.isdir(os.path.split(os.path.abspath(options.output_file))[0]):
      parser.error("Output file name without extension must be valid and exist.")
  # Set Paths
    output_xbl_config_directory = os.path.split(os.path.abspath(options.output_file))[0]
    output_xbl_config_filename = os.path.split(os.path.abspath(options.output_file))[1]  
    output_xbl_config_filename = output_xbl_config_filename + ".elf"  
    remove(output_xbl_config_filename)
  
    # Input JSON path must be supplied and exist
    options.json_config_path = get_abspath_if_exist(options.json_config_path, parser, "JSON config file must be supplied and exist.")

    #generate XBL config file
    generate_xbl_config_file(options.json_config_path,
                             options.format,
                             options.elf_address, 
                             output_xbl_config_filename,
                             options.base_directory,
                             output_xbl_config_directory, 
                             options.sectools_directory, 
                             autogen_directory,
                             options.tools_path,
                             options.soc_hw_version,
                             options.soc_vers)
  else:
    if not os.path.isfile(options.config_file_to_be_disassembled):
      parser.error("XBL config file must be supplied and exist.")        
    disassemble_xbl_config_file(options.config_file_to_be_disassembled, 
                                 options.format,
                                 output_xbl_config_directory, 
                                 autogen_directory,
                                 options.tools_path)

  exit(0)
