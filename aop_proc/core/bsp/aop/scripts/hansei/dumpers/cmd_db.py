#  ===========================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function
import struct
import logging
logger = logging.getLogger(__name__)

import os
import itertools
import math
from ddr_memory import DDRMemory
from hansei_config import dump_file_types
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *
from targ_spec_config import *
from summary import chip_version
import csv
#import pdb;pdb.set_trace()

tab_size = 8
dump_error = 0 # should only be used for error that indicate dump corruption
vrm_map = []
bcm_reg_map = []
bcm_vcd_map = []
no_regs = 0
no_bcm_regs = 0
no_vcds = 0
other_vrm_rsc = ''
def dump(debug_data): #dump_path, memory, debug_info, fill_char):
	dump_error = 0
	global vrm_map, no_regs,other_vrm_rsc,bcm_reg_map, bcm_vcd_map
	memory = debug_data['rpm_memory']
	debug_info = debug_data['debug_info']
	fill_char = debug_data['args'].fill_char
	dump_path = debug_data['dump_path']
	target = debug_data['target']
	target_ver = target+'-'+str(chip_version)
	if target_ver in targ_spec_config:
		target = target_ver
	debug_data['printer'].p("Dumping CMD_DB data...")
	
	no_regs = vrm[target]['no_regs']
	vrm_map = ['' for i in range(0,no_regs)]
	
	no_vcds = bcm[target]['no_vcds']
	no_bcm_regs = bcm[target]['no_regs']
	bcm_reg_map = ['' for i in range(0,no_bcm_regs)]
	bcm_vcd_map = ['' for i in range(0,no_vcds)]
	

	try:
		ver_data = memory.read(0x65FE0000, 4)
	except:
		debug_data['printer'].p("\tCPRF registers bin is not recovered...")
		return

	#import pdb; pdb.set_trace()
	smem_db_addr_die  = debug_info.variables['smem_db'].die
	smem_db_addr_addr = debug_info.variables['smem_db'].address
	smem_db      = decode_object('smem_db', smem_db_addr_addr, None, memory, debug_info, die=smem_db_addr_die)
	
	cmddb_file_name = os.path.join(dump_path, 'cmd_db.txt')
	with open(cmddb_file_name, 'w') as cmd_db_file:
	
		smem_db_data_addr = 0x65FE0090
		
		for x in range(0,8):
			if smem_db.slv_id_info[x].slv_id != 0:
				if smem_db.slv_id_info[x].slv_id == 3:
					struct_name = 'arc_db_headers'
					slv_name = 'ARC'
					print('\n SLV_TYPE: %s, SLV_ID: %d'% (slv_name,smem_db.slv_id_info[x].slv_id), file=cmd_db_file)
					print("\n\t| Resource | Address |",file=cmd_db_file)
					cmd_db_header_die  = debug_info.variables[struct_name].die
					cmd_db_header_addr = smem_db_data_addr + smem_db.slv_id_info[x].header_offset
					cmd_db_header_data = decode_object(struct_name, cmd_db_header_addr, None, memory, debug_info, die=cmd_db_header_die)
					for cnt in range(0, smem_db.slv_id_info[x].cnt):
						res_id = convert_hex_to_ascii(cmd_db_header_data[cnt].res_id)
						res_addr = hex(cmd_db_header_data[cnt].addr)
						print('\t|{0:>10s}|{1:>9s}|' .format(res_id, res_addr), file=cmd_db_file)
					print("\n\t---------------------",file=cmd_db_file)
					
				elif smem_db.slv_id_info[x].slv_id == 5: 
					struct_name = 'bcm_db_headers'
					slv_name = 'BCM'
					print('\n SLV_TYPE: %s, SLV_ID: %d'% (slv_name,smem_db.slv_id_info[x].slv_id), file=cmd_db_file)
					print("\n\t| Resource | Address |",file=cmd_db_file)
					cmd_db_header_die  = debug_info.variables[struct_name].die
					cmd_db_header_addr = smem_db_data_addr + smem_db.slv_id_info[x].header_offset
					cmd_db_header_data = decode_object(struct_name, cmd_db_header_addr, None, memory, debug_info, die=cmd_db_header_die)
					
					bcm_data_die  = debug_info.variables['bcm_aux'].die
					bcm_data_addr = smem_db_data_addr + smem_db.slv_id_info[x].data_offset
					bcm_data = decode_object(struct_name, bcm_data_addr, None, memory, debug_info, die=bcm_data_die)
					
					for cnt in range(0, smem_db.slv_id_info[x].cnt):
						res_id = convert_hex_to_ascii(cmd_db_header_data[cnt].res_id)
						res_addr = cmd_db_header_data[cnt].addr
						vcd = bcm_data[cnt].clk_id
						bcm_vcd_map[vcd] += res_id +','
						bcm_idx = ((res_addr & 0xffff)/ 0x4)
						bcm_reg_map[bcm_idx] += res_id
						print('\t|{0:>10s}|{1:>9s}|' .format(res_id, hex(res_addr)), file=cmd_db_file)
					print("\n\t---------------------",file=cmd_db_file)
					
				elif smem_db.slv_id_info[x].slv_id == 4: 
					struct_name = 'bcm_db_headers'
					slv_name = 'VRM'
					print('\n SLV_TYPE: %s, SLV_ID: %d'% (slv_name,smem_db.slv_id_info[x].slv_id), file=cmd_db_file)
					print("\n\t| Resource | Address |",file=cmd_db_file)
					cmd_db_header_die  = debug_info.variables[struct_name].die
					for cnt in range(0, smem_db.slv_id_info[x].cnt):
						cmd_db_header_addr = smem_db_data_addr + smem_db.slv_id_info[x].header_offset + 24* cnt
						cmd_db_header_data = decode_object(struct_name, cmd_db_header_addr, None, memory, debug_info, die=cmd_db_header_die)
						res_id = convert_hex_to_ascii(cmd_db_header_data[0].res_id)
						res_addr = cmd_db_header_data[0].addr
						vrm_idx = ((res_addr & 0xffff)/ 0x100)
						try:
							vrm_map[vrm_idx] +=res_id+': '
						except:
							other_vrm_rsc +=res_id+' '
						print('\t|{0:>10s}|{1:>9s}|' .format(res_id, hex(res_addr)), file=cmd_db_file)
	
		if smem_db == 0:
			print("\n !!! Unable to retrive CMD_DB ADDRESS !!! \n", file=cmd_db_file)
			return

def convert_hex_to_ascii(h):
    chars_in_reverse = []
    while h != 0x0:
        chars_in_reverse.append(chr(h & 0xFF))
        h = h >> 8

    #chars_in_reverse.reverse()
    return ''.join(chars_in_reverse)