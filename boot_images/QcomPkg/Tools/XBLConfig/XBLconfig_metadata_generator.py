###########################################################################################################################################################
# This script generates Meta data tables and outputs them as .bin files for Loader and Core Config, from a given input JSON file.
# Script also supports disassembling XBL-config option: 
#             script takes input "disassembled_xcfg_info_json" with disassembled xbl_config binaries, parses  
#             xbl-config-meta-data and creates "out_create_xcfg_json" with disassembled xcfg items listed 
#             which can be re-input to tool to re-generate xbl-config.elf.  
###########################################################################################################################################################
import sys
if sys.version_info < (2, 7, 0):
  print "Must use Python 2.7 or greater"
  sys.exit(1)
import os
import json
import commons
import collections
from commons import *
from collections import OrderedDict
from optparse import OptionParser
#----------------------- Meta Data Table: 12 bytes table-header + config item info for each item -----------------------------
#-----------------------------------------------------------------------------------------------------------------------------
# xcfg_type[4], major_version_number[1], minor_version_number[1], number_of_entries[2], XBLConfig_meta_size[4]        
#-----------------------------------------------------------------------------------------------------------------------------
# attributes[4] # offset_from_meta_start[4] # size[4] # config_name_len[4] # config_name[config_name_len] # align to 8 bytes.#
#               #                           #         #                    #                              #                  #
#               #                           #         #                    #                              #                  #
#-----------------------------------------------------------------------------------------------------------------------------
# order of all key-names like "xbl_cfg_value" etc and their sies must match with expected meta-data-table's header fields in XBL-config library data-structures.
MAJOR_VERSION_CFG,MINOR_VERSION_CFG = 0x01,0x01
XBL_CFG_VALUE,   MAJOR_VERSION,  MINOR_VERSION,   CFG_ITEM_ENTRIES_NUM,  META_DATA_SIZE = \
 "xbl_cfg_value","major_version","minor_version","cfg_item_entries_num","meta_data_size"
meta_header_json = """{ "xbl_cfg_value": 
                                         { "value" : "0x0", "size" : "0x4", "type" : "string" }, 
                        "major_version": 
                                         { "value" : "0x0", "size" : "0x1", "type" : "number" }, 
                        "minor_version": 
                                         { "value" : "0x0", "size" : "0x1", "type" : "number" }, 
                        "cfg_item_entries_num": 
                                         { "value" : "0x0", "size" : "0x2", "type" : "number" }, 
                       "meta_data_size":     
                                         { "value" : "0x0", "size" : "0x4", "type" : "number" } 
                      }"""
# config item key names used in input .json and this file.
RAM_ADDRESS,    ATTRIBUTES, FILE_NAME,  CONFIG_NAME,  OFFSET_FROM_META_START,  ALIGN_BYTES = \
 "ram_address","attributes","file_name","config_name","offset_from_meta_start","align_bytes"
ALIGNMENT, ITEM_SIZE,  ITEM_SIZE_ALIGN, CONFIG_NAME_LEN,  VALUE, SIZE,  TYPE,   STRING, NUMBER,   CFGs = \
    "0x8","item_size", "0x4",          "config_name_len","value","size","type","string","number", {"CFGL"}
# input json template:
# key-names below like "attributes" are exact same key name as in input .json file if exists in input .json.
# order of all key-names like "attributes" etc and their sizes must match with expected meta-data-table's config-entry fields in XBL-config library data-structures.
# sub-keys like "value", "size" etc. are helper attributes for each field to process input
cfg_item_json    = """{ "attributes": 
                                      { "value" : "0x0", "size" : "0x4", "type" : "number" }, 
                        "offset_from_meta_start": 
                                      { "value" : "0x0", "size" : "0x4", "type" : "number" }, 
                        "item_size": 
                                      { "value" : "0x0", "size" : "0x4", "type" : "number" }, 
                        "config_name_len": 
                                      { "value" : "0x0", "size" : "0x4", "type" : "number" }, 
                        "config_name": 
                                      { "value" : "0x0", "size" : "0x0", "type" : "string" },
                        "align_bytes":
                                      { "value" : "0x0", "size" : "0x0", "type" : "number" }
                   }"""

def print_meta_data_table(meta_header, meta_cfg_entries):
  print "\n-----------------------------------------------------XBL Config Meta Header-----------------------------------------------------"
  print('{:>13} {:>17} {:>17} {:>24} {:>18}'.format(*meta_header.keys()))
  print_string=[]
  for header_field in meta_header:
    print_string.append(meta_header[header_field][VALUE])
  print('{:>13} {:>17} {:>17} {:>24} {:>18}'.format(*print_string))
  print "-----------------------------------------------------XBL Config Meta Entries----------------------------------------------------"
  print('{:>10} {:>26} {:>12} {:>18} {:>26} {:>18}'.format(*meta_cfg_entries[0].keys()))
  for cfg_entry in meta_cfg_entries:
    print_string = []
    for item_field in cfg_entry:
      if item_field != ALIGN_BYTES:
        print_string.append(cfg_entry[item_field][VALUE])
      else:
        print_string.append(cfg_entry[item_field][SIZE])
    print('{:>10} {:>26} {:>12} {:>18} {:>26} {:>18}'.format(*print_string))
  print "--------------------------------------------------------------------------------------------------------------------------------"
    
# This is a utility function to write to hex string data in binary to binary file.
def write_num_little_endian(wt_size, bin_fp, data):
  data = int(data,16)
  for i in range (0,wt_size):
    temp = (data>>(i*8))&0xFF
    bin_fp.write(chr(temp))
 
def write_string(length, bin_fp, data):
  data = data.encode("hex")
  data = int(data,16)
  for i in range(length-1, -1, -1):
    temp = (data>>(i*8))&0xFF
    bin_fp.write(chr(temp))

# This is a utility function to generate the required byte alignment and write to the .bin file. Different fields require different alignment such as 2 byte, 4 bytes, 8 bytes etc.
def read_num_little_endian(rd_size, bin_fp):
  data = bin_fp.read(rd_size)
  if len(data) != rd_size:
    print_error_exit("\nERROR: Binary file:" + bin_fp.name + " reached EOF, can't read " + str(rd_size) + " bytes.")
  num = 0
  for i in range(rd_size-1, -1, -1):
    num = num | (ord(data[i])<<(i*8))
  num = hex(num)
  return str(num)
  
def read_string(rd_size, bin_fp):
  data = bin_fp.read(rd_size)
  if data == "":
    print_error_exit("\nERROR: Binary file:" + bin_fp.name + " reached EOF, can't read " + str(rd_size) + " bytes.")
  return data
    
# Utility function to read meta_data_config_file and create meta_header.
def read_meta_header(bin_fp):
  meta_header = json.loads(meta_header_json, object_pairs_hook=OrderedDict)
  #read and create meta-header
  for header_field in meta_header.keys():
    rd_size = int(meta_header[str(header_field)][SIZE],16)
    if meta_header[str(header_field)][TYPE] == STRING:
      meta_header[str(header_field)][VALUE] = read_string(rd_size, bin_fp)
    else:
      meta_header[str(header_field)][VALUE] = read_num_little_endian(rd_size, bin_fp)
  # if read meta-header doesn't have any config_meta signature then print error and exit.
  if not meta_header[XBL_CFG_VALUE][VALUE] in CFGs:
    print_error_exit("\nERROR: " + str(bin_fp.name) + " file doesn't contain " \
                     "any XBL_CFG_Meta signature from " + str(CFGs) + \
                     " at the beginning of the file.")
  return meta_header

# Utility function to read meta_data_config_file and create meta_cfg_entries.
def read_meta_cfg_entries(meta_header, bin_fp):
  meta_cfg_entries = []
  cfg_item_entries_num = int(meta_header[CFG_ITEM_ENTRIES_NUM][VALUE],16)
  for entry_index in range(0, cfg_item_entries_num):
    cfg_entry = json.loads(cfg_item_json, object_pairs_hook=OrderedDict)
    for item_field in cfg_entry:
      if cfg_entry[item_field][TYPE] == STRING:
        cfg_entry[item_field][VALUE] = read_string(int(cfg_entry[item_field][SIZE],16), bin_fp)
      else:
        cfg_entry[item_field][VALUE] = read_num_little_endian(int(cfg_entry[item_field][SIZE],16), bin_fp)
      if str(item_field) == CONFIG_NAME_LEN:
        cfg_entry[CONFIG_NAME][SIZE] = cfg_entry[CONFIG_NAME_LEN][VALUE]
        if (int(cfg_entry[CONFIG_NAME_LEN][VALUE],16) % int(ALIGNMENT,16)):
          cfg_entry[ALIGN_BYTES][SIZE] = hex( int(ALIGNMENT,16) - (int(cfg_entry[CONFIG_NAME_LEN][VALUE],16) % int(ALIGNMENT,16)) )
    meta_cfg_entries.append(cfg_entry)
  return meta_cfg_entries
  
# Utility function to write the meta_header and meta_cfg_entries to file.
def write_bin_file(bin_fp, meta_header, meta_cfg_entries):
  #write meta-header blob.
  for header_field in meta_header:
    if meta_header[str(header_field)][TYPE] == STRING:
      write_string(int(meta_header[str(header_field)][SIZE],16), bin_fp, meta_header[str(header_field)][VALUE])
    else:
      write_num_little_endian(int(meta_header[str(header_field)][SIZE],16), bin_fp, meta_header[str(header_field)][VALUE])
  #write meta_data_table config item entries
  for cfg_entry in meta_cfg_entries:
    for item_field in cfg_entry:
      if cfg_entry[item_field][TYPE] == STRING:
        write_string(int(cfg_entry[item_field][SIZE],16), bin_fp, cfg_entry[item_field][VALUE])
      else:
        write_num_little_endian(int(cfg_entry[item_field][SIZE],16), bin_fp, cfg_entry[item_field][VALUE])

def calc_and_fill_cfg_entry_offset_and_meta_data_size(meta_cfg_entries, meta_header):
  # traverse through each meta_cfg_entry to calculate meta_header size
  for cfg_entry in meta_cfg_entries:
    # calculate meta_data_size by adding current meta-cfg-entry's each field's(key) size 
    for item_key in cfg_entry:
      meta_header[META_DATA_SIZE][VALUE] = hex(int(meta_header[META_DATA_SIZE][VALUE],16) + int(cfg_entry[item_key][SIZE],16))
  if(len(meta_cfg_entries)>0):
    meta_cfg_entries[0][OFFSET_FROM_META_START][VALUE] = meta_header[META_DATA_SIZE][VALUE]
  for entry_index in range(1, len(meta_cfg_entries)):
    meta_cfg_entries[entry_index][OFFSET_FROM_META_START][VALUE] = hex(int(meta_cfg_entries[entry_index-1][OFFSET_FROM_META_START][VALUE],16) + int(meta_cfg_entries[entry_index-1][ITEM_SIZE][VALUE],16))

def check_dup_cfg_name_and_error(meta_cfg_entries, new_cfg_entry):
  for existing_cfg_entry in meta_cfg_entries:
    #if new config item entry's config_name matches already exisitng config_name then error out.
    if str(new_cfg_entry[CONFIG_NAME][VALUE]) == str(existing_cfg_entry[CONFIG_NAME][VALUE]):
      print_error_exit("\nERROR config item with config_name:\"" + new_cfg_entry[CONFIG_NAME][VALUE] + "\" already exist." \
                       "Please update input .json to ensure unique \"config_name\" for each config item.\n")
      
def create_new_cfg_item(input_cfg_item, base_directory):
  config_file = None
  cfg_entry = json.loads(cfg_item_json, object_pairs_hook=OrderedDict)
  # populate ATTRIBUTES using input from .json if it exists
  if ATTRIBUTES in input_cfg_item:
    cfg_entry[ATTRIBUTES][VALUE] = input_cfg_item[ATTRIBUTES]
  # populate file_name and config-item size using input from .json
  if FILE_NAME in input_cfg_item:
    config_file = os.path.join(base_directory, correct_path(str(input_cfg_item[FILE_NAME])))
    cfg_entry[ITEM_SIZE][VALUE] = hex(os.path.getsize(config_file)).rstrip("L")
  else:
    print_error_exit("/nERROR: missing required config item field:\"" + FILE_NAME + "\" in input .json file.")
  # populate config_name and config-name length using input from .json
  if CONFIG_NAME in input_cfg_item:
    cfg_name = input_cfg_item[CONFIG_NAME]
    if (int(cfg_entry[ITEM_SIZE][VALUE],16) % int(ITEM_SIZE_ALIGN,16)):
      print_error_exit("\nERROR: config filename: \"" + str(config_file) + "\" is not "+ ITEM_SIZE_ALIGN +" bytes aligned.")
    cfg_entry[CONFIG_NAME_LEN][VALUE] = hex(len(cfg_name)).rstrip("L")
    cfg_entry[CONFIG_NAME][VALUE] = cfg_name
    cfg_entry[CONFIG_NAME][SIZE] = cfg_entry[CONFIG_NAME_LEN][VALUE]
    if (int(cfg_entry[CONFIG_NAME_LEN][VALUE],16) % int(ALIGNMENT,16)):
      cfg_entry[ALIGN_BYTES][SIZE] = hex( int(ALIGNMENT,16) - (int(cfg_entry[CONFIG_NAME_LEN][VALUE],16) % int(ALIGNMENT,16)) )
  else:
    print_error_exit("/nERROR: missing required config item field:\"" + CONFIG_NAME + "\" in input .json file.")      
  return cfg_entry
  
def fill_meta_header(json_input_cfg_items, xbl_cfg_value, major_version, minor_version):
  header_size = 0
  meta_header = json.loads(meta_header_json, object_pairs_hook=OrderedDict)
  for header_field in meta_header:
    header_size += int(meta_header[header_field][SIZE],16)
  meta_header[XBL_CFG_VALUE][VALUE] = xbl_cfg_value
  meta_header[MAJOR_VERSION][VALUE] = hex(major_version)
  meta_header[MINOR_VERSION][VALUE] = hex(minor_version)
  meta_header[CFG_ITEM_ENTRIES_NUM][VALUE] = hex(len(json_input_cfg_items))
  meta_header[META_DATA_SIZE][VALUE] = hex(header_size)
  return meta_header

def create_meta_data_table(json_input_cfg_items, xbl_cfg_value, fp_cfg_meta_file, major_version, minor_version, base_directory):
  meta_header = fill_meta_header(json_input_cfg_items, xbl_cfg_value, major_version, minor_version)  
  #fill meta-data-table's config item entries
  meta_cfg_entries = []
  # For each config-item in input .json file, create config_item_entry and append/insert it in meta_cfg_entries.
  for input_cfg_item_key in json_input_cfg_items:
    new_cfg_entry = create_new_cfg_item(json_input_cfg_items[input_cfg_item_key], base_directory)
    check_dup_cfg_name_and_error(meta_cfg_entries, new_cfg_entry)
    meta_cfg_entries.append(new_cfg_entry)
  calc_and_fill_cfg_entry_offset_and_meta_data_size(meta_cfg_entries, meta_header)
  write_bin_file(fp_cfg_meta_file, meta_header, meta_cfg_entries)  #Write meta data table in the form of a .bin file
  print_meta_data_table(meta_header, meta_cfg_entries)

def check_duplicate_keys(pairs):
  dict = {}
  for key, val in pairs:
    # if key already exists in dictionary
    if key in dict:
      # duplicate key found, print error and exit.
      print_error_exit("\nERROR Duplicate keys \"" + str(key) + "\" in input .json config file. " \
                          "Please update input .json to ensure unique settings config item.\n")
    else:
      dict[key] = val

  return dict
  
def generate_xbl_config_metadata(json_config_path, base_directory, output_directory):
  print "\n" + XBL_CONFIG_METADATA_SCRIPT + ": Generating XBL CFG meta-data........."
  # Read JSON config data
  try:
    # check for duplicate keys in input json config file
    with open(json_config_path) as json_config_file:    
      input_json_xbl_cfgs = json.load(json_config_file, object_pairs_hook=check_duplicate_keys)
      json_config_file.close()
    # load valid input json as with input dictionary order preserved.
    with open(json_config_path) as json_config_file:    
      input_json_xbl_cfgs = json.load(json_config_file, object_pairs_hook=OrderedDict)
      json_config_file.close()
  except:
    print "\nERROR: An error occured while reading JSON config file. Please check contents."
    sys.exit(1)

  # for every xcfg_meta_item like "CFGL" dictionary, create xcfg_meta binary (CFGL.bin).
  for xcfg_meta_item in input_json_xbl_cfgs:
    xcfg_meta_file = os.path.join(output_directory, correct_path(xcfg_meta_item + ".bin"))
    fp_cfg_meta_file = open(xcfg_meta_file, 'wb')
    create_meta_data_table(input_json_xbl_cfgs[xcfg_meta_item], str(xcfg_meta_item), fp_cfg_meta_file, MAJOR_VERSION_CFG, MINOR_VERSION_CFG, base_directory) 
    fp_cfg_meta_file.close()

def create_xcfg_json(cfg_meta_key,
                     cfg_meta_info_key,
                     meta_cfg_entries, 
                     disassembled_bins_info,
                     disassembled_xcfg_dict,                     
                     output_directory,
                     out_create_xcfg_json):
  disassembled_bins_path = disassembled_bins_info[DISASSEMBLE_PATH]
  if not len(cfg_meta_key)>1:
    print_error_exit("\nERROR: Disassembled bins info input .json must have \"segment_1, segment_2\" etc. \
                        key with value as binary filename.")
  seg_index = int(cfg_meta_info_key[-1]) + 1 #1 is added to move to next key which has <key:value> as <1st_cfg_item_binary_key:binary_filename>
  cfg_meta_info_key = cfg_meta_info_key[:-1]
  item_num = 1
  # For every cfg_entry in meta_table, add cfg_item_key's("file[n]") info in disassembled_xcfg_dictionary
  for cfg_entry in meta_cfg_entries:
    xcfg_dict = disassembled_xcfg_dict
    # create empty cfg_entry:"file1" dictionary if it doesn't exist in overall disassembled_xcfg_dict
    if (ITEM_KEY+str(item_num)) not in xcfg_dict[cfg_meta_key]:
      xcfg_dict[cfg_meta_key][ITEM_KEY+str(item_num)] = OrderedDict({})
    # populate disassembled_xcfg_dict's cfg_item's("file1") fields like "attributes", "filename", "configname" 
    xcfg_dict[cfg_meta_key][ITEM_KEY+str(item_num)][ATTRIBUTES] \
                                = cfg_entry[ATTRIBUTES][VALUE]
    xcfg_dict[cfg_meta_key][ITEM_KEY+str(item_num)][CONFIG_NAME] \
                                = cfg_entry[CONFIG_NAME][VALUE]
    xcfg_dict[cfg_meta_key][ITEM_KEY+str(item_num)][FILE_NAME] \
                                = cfg_item_filename = replace_non_alpha_numeric_chars("_", cfg_entry[CONFIG_NAME][VALUE])
    
    # create key to retrieve current cfg_item's disassembled binary info in disassembled_bins_info dict
    info_key = cfg_meta_info_key + str(seg_index)
    
    if not info_key in disassembled_bins_info:
      print_error_exit("\nERROR: Disassembled info input .json is missing:\"" + info_key + BIN_EXTN + "\".")
    # copy cfg_item's binary file "segment_[n].bin" to cfg_item's config_name itself for 
    #          readability and easily identify cfg_item binary.
    shutil.copyfile(os.path.join(disassembled_bins_path, disassembled_bins_info[info_key]),
                                 os.path.join(output_directory,cfg_item_filename))
    item_num = item_num + 1 # iterate to next cfg_item
    seg_index = seg_index + 1 # iterate to next segment index

def read_populate_cfg_meta_if_exist(cfg_meta_key,
                                    seg_bin_file,
                                    meta_header,
                                    meta_cfg_entries,
                                    disassembled_bins_path):
  cfg_meta_key_found, meta_header, meta_cfg_entries = None, None, None
  # read first len(cfg_meta_key) worth of bytes  
  bin_fp = open(seg_bin_file, 'rb')
  rd_cfg_meta_identifier = read_string(len(cfg_meta_key), bin_fp)
  # if read bytes match cfg_meta_key (signature like CFGL) then cfg_meta binary is found
  if (rd_cfg_meta_identifier == cfg_meta_key):
    cfg_meta_key_found = True
    bin_fp.seek(0) #move file pointer to beginning of binary file.
    # copy cfg_meta_item's binary file "segment_[n].bin" to  
    #          [cfg_meta_key.bin] like CFGL.bin for readability 
    #          and easily identify cfg_meta_item binary.
    shutil.copyfile(seg_bin_file,
                    os.path.join(disassembled_bins_path,
                                 (cfg_meta_key + BIN_EXTN)))
    # read/parse found cfg_meta_key binary and create xcfg meta_data_table
    meta_header = read_meta_header(bin_fp)
    meta_cfg_entries = read_meta_cfg_entries(meta_header, bin_fp)
    print "\nDisassembled XBL Config Meta table."    
    print_meta_data_table(meta_header, meta_cfg_entries)
  bin_fp.close()
  return [cfg_meta_key_found, meta_header, meta_cfg_entries]

def parse_cfg_meta_create_xcfg_dict(cfg_meta_key, 
                                    disassembled_xcfg_info_json, 
                                    disassembled_xcfg_dict, 
                                    output_directory, 
                                    out_create_xcfg_json):
  # Read disassembled xcfg info json
  try:
    with open(disassembled_xcfg_info_json) as disassembled_xcfg_info_json_file:    
      disassembled_bins_info = json.load(disassembled_xcfg_info_json_file, object_pairs_hook=OrderedDict)
  except:
    print "\nERROR: An error occured while reading " + str(xcfg_info_json_file) + " file. Please check the contents."
    sys.exit(-1)
  cfg_meta_key_found, meta_header, meta_cfg_entries = False, None, None
  if not DISASSEMBLE_PATH in disassembled_bins_info:
    print_error_exit("\nERROR Disassemble path with binaries not provided in input .json.")
  # For each "segment_*" key is found in input disassembled_bins_info, process it. 
  for info_key in disassembled_bins_info:
    if ((str(info_key)).find(SEGMENT)) == -1:
      continue
    seg_bin_file = os.path.join(disassembled_bins_info[DISASSEMBLE_PATH], disassembled_bins_info[info_key])
    if not os.path.isfile(seg_bin_file):
      print_error_exit("\nERROR disassembled binary:\"" + seg_bin_file + "\" doesn't exist." \
                       "Please ensure disassembled input .json correctly captures disassembled binaries information.")
    # skip parsing 0 bytes (mostly zi/.bss segments) binary files.
    if ((os.stat(seg_bin_file)).st_size == 0):
      continue

    # Check if key's value("segment_[n].bin") is cfg_meta_data_table binary then
    # populate meta_data_header and meta_cfg_entries data-structures. 
    if (not cfg_meta_key_found):
      [cfg_meta_key_found, meta_header, meta_cfg_entries] \
                         = read_populate_cfg_meta_if_exist(cfg_meta_key,
                                                           seg_bin_file,
                                                           meta_header,
                                                           meta_cfg_entries,
                                                           disassembled_bins_info[DISASSEMBLE_PATH])
      # if cfg_meta_key is found then create out_create_xcfg_json 
      # capturing disassembled cfg items info which can be used to create xbl_config again.
      if cfg_meta_key_found == True:
        if cfg_meta_key not in disassembled_xcfg_dict:
          disassembled_xcfg_dict[cfg_meta_key] = OrderedDict({})
        # CFG_META already found, next (key) in input .json would be first config-item binary info.
        create_xcfg_json(cfg_meta_key,
                         info_key,
                         meta_cfg_entries, 
                         disassembled_bins_info,
                         disassembled_xcfg_dict,
                         output_directory,
                         out_create_xcfg_json)
        break
  
  # if cfg_meta_key was found in disassembled binary then populate elf_entry_point in first cfg_meta item->"file1"->"ram_address"
  if (cfg_meta_key_found==True):
    if (FIRST_CFG_FILE_KEY in disassembled_xcfg_dict[cfg_meta_key]):
      if E_ENTRY in disassembled_bins_info:
        disassembled_xcfg_dict[cfg_meta_key][FIRST_CFG_FILE_KEY][ELF_ADDRESS] = str(disassembled_bins_info[E_ENTRY])
  return
  
def parse_disassemble_xbl_config(disassembled_xcfg_info_json, 
                                 output_directory, 
                                 out_create_xcfg_json):
  print "\n" + XBL_CONFIG_METADATA_SCRIPT + ": Parsing XBL config meta-data and disassembling XBL config items ........."
  disassembled_xcfg_dict = collections.OrderedDict({})
  # for every cfg_meta_key from expected CFGs set.
  for cfg_meta_key in CFGs:
    # parse disassembled_xcfg_info_json, locate bin with cfg_meta_key, parse cfg_meta_bin, 
    #      copy cfg_item .bins as friendly config-item name and disassembled_xcfg_dict dictionary
    parse_cfg_meta_create_xcfg_dict(cfg_meta_key, 
                                    disassembled_xcfg_info_json, 
                                    disassembled_xcfg_dict, 
                                    output_directory, 
                                    out_create_xcfg_json)

  # if xbl_config items found from input disassembled binaries
  if len(disassembled_xcfg_dict) != 0:
    # write disassembled_xcfg_dict as out_create_xcfg_json
    with open(out_create_xcfg_json, "w") as out_create_xcfg_json_fp:
      json.dump(collections.OrderedDict(disassembled_xcfg_dict), out_create_xcfg_json_fp, separators=(',', ':'), indent=4)
    out_create_xcfg_json_fp.close()
    print "\n" + XBL_CONFIG_METADATA_SCRIPT + ": Generated XBL config items and \"" + out_create_xcfg_json + "\""
  else: # input disassembled binaries didn't have any xbl_config items.
    print "\n" + XBL_CONFIG_METADATA_SCRIPT + ": Couldn't find XBL config items from binaries listed in \"" + disassembled_xcfg_info_json + "\""
    exit(1)
  
if __name__ == "__main__":
  error_count = 0  
  parser = OptionParser()
  
  parser.add_option("-b", "--config_item_base_directory",
                    action="store", type="string", dest="base_directory",
                    help="Base directory for relative paths of config items in input .json.")
        
  parser.add_option("-i", "--input-json",
                    action="store", type="string", dest="json_config_path",
                    help="Input JSON config file.")

  parser.add_option("-o", "--output-directory",
                    action="store", dest="output_directory",
                    help="Output directory to generate output files.")

  parser.add_option("-d", "--disassembled_xcfg_info_json",
                    action="store", dest="disassembled_xcfg_info_json",
                    help="Input json filename with disassembled binaries information.")

  parser.add_option("-c", "--output-create-xcfg-json",
                    action="store", type="string", dest="out_create_xcfg_json",
                    help="Output filename including path to create .json post disassembling process.")
  (options, args) = parser.parse_args()

  # XBL config output directory must be supplied and exist
  options.output_directory = get_abspath_if_exist(options.output_directory, parser, 
                                                  "XBL Config output directory must be supplied and exist.")
  # if XBL config disassemble option is not passed. (xbl_config elf generation option is selected)
  if not options.disassembled_xcfg_info_json:
      # Input JSON path must be supplied and exist
    if not options.json_config_path or not os.path.isfile(options.json_config_path):
      parser.error("JSON config file must be supplied and exist.")
    # Base directory must be supplied and exist
    if not options.base_directory or not os.path.isdir(options.base_directory):
      parser.error("Base directory for relative paths of config items must be supplied and exist.")
    # Expand input path/directories to absolute path
    options.base_directory = os.path.abspath(options.base_directory)
    options.json_config_path = os.path.abspath(options.json_config_path)
    generate_xbl_config_metadata(options.json_config_path, options.base_directory, options.output_directory)
  else:
    if not os.path.isfile(options.disassembled_xcfg_info_json):
      parser.error("Input json with disassembled binaries information must be supplied and exist.")
    if not options.out_create_xcfg_json or \
       not (os.path.isdir(os.path.split(os.path.abspath(options.out_create_xcfg_json))[0])):
      parser.error("Output filename including path must be supplied to create " + \
                   ".json as output of disassembling process.")
    parse_disassemble_xbl_config(options.disassembled_xcfg_info_json, options.output_directory, options.out_create_xcfg_json)
  exit(0)