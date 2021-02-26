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
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *
from targ_spec_config import *
from summary import chip_version
from arc import g_res

tab_size = 8
dump_error = 0 # should only be used for error that indicate dump corruption

# Hardcoding BASE addresses 
hw_ver_addr         =0
aoss_setting_addr   =0
cprf_cntrl_addr     =0 
cprf_vrm_addr       =0
volt_limits_addr    =0 
volt_safe_addr      =0 
enable_addr         =0
values_addr         =0 
value_1_addr        =0 
value_2_addr        =0 
value_3_addr        =0 
value_4_addr        =0 
value_5_addr        =0 
no_cprf = 0
no_corners = 0

def dump(debug_data): #dump_path, memory, debug_info, fill_char):
	global dump_error
	global hw_ver_addr, aoss_setting_addr, cprf_cntrl_addr
	global cprf_vrm_addr, volt_limits_addr, volt_safe_addr
	global enable_addr, values_addr, value_1_addr, value_3_addr 
	global value_4_addr, value_5_addr, no_cprf, no_corners
	dump_error = 0
	memory = debug_data['rpm_memory']
	debug_info = debug_data['debug_info']
	fill_char = debug_data['args'].fill_char
	dump_path = debug_data['dump_path']
	target = debug_data['target']
	target_ver = target+'-'+str(chip_version)

	if target_ver in targ_spec_config:
		target = target_ver

	debug_data['printer'].p("Dumping CPRF Registers...")

	hw_ver_addr = cprf[target]['hw_ver_addr']
	aoss_setting_addr = cprf[target]['aoss_setting_addr']
	cprf_cntrl_addr = cprf[target]['cprf_cntrl_addr']
	cprf_vrm_addr = cprf[target]['cprf_vrm_addr']
	volt_limits_addr = cprf[target]['volt_limits_addr']
	volt_safe_addr = cprf[target]['volt_safe_addr']
	enable_addr = cprf[target]['enable_addr']
	values_addr = cprf[target]['values_addr']
	value_1_addr = cprf[target]['value_1_addr']
	value_2_addr = cprf[target]['value_2_addr']
	value_3_addr = cprf[target]['value_3_addr']
	value_4_addr = cprf[target]['value_4_addr']
	value_5_addr = cprf[target]['value_5_addr']
	no_cprf = cprf[target]['no_cprf']
	no_corners = cprf[target]['no_corners']


	try:
		ver_data = memory.read(hw_ver_addr, 4)
	except:
		debug_data['printer'].p("\tCPRF registers bin is not recovered...")
		return
	dump_cprf_summary(dump_path, memory, target, debug_info, fill_char)
    
	if dump_error != 0:
		debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
	else:
		debug_data['printer'].p("\t...DONE")

def dump_cprf_summary(dump_path, memory, target, debug_info, fill_char):
	dump_path = os.path.join(dump_path, 'reqs_for_resources')
	if not os.path.exists(dump_path):
		os.makedirs(dump_path)

	cprf_file_name = os.path.join(dump_path, 'cprf.txt')
	with open(cprf_file_name, 'w') as cprf_file:
        
		print("\n ~~CPRF Registers Summary~~", file=cprf_file)
		#import pdb; pdb.set_trace()
		ver_data = memory.read(hw_ver_addr, 4)
		ver_info = struct.unpack('<L', ver_data)[0]
		major = ((ver_info >> 28) & 0xF)
		minor = ((ver_info >> 16) & 0x0FFF)
		step = ((ver_info) & 0xFFFF)
		print('\n\tHW Version  Major: %d, Minor: %d, STEP: %d' %(major,minor,step ), file=cprf_file)

		aoss_data = memory.read(aoss_setting_addr, 4)
		aoss_info = struct.unpack('<L', aoss_data)[0]
		sid = ((aoss_info >> 13) & 0x7)
		drvid = ((aoss_info >> 8) & 0x01F)
		addr = ((aoss_info) & 0xFF)
		print('\n\tAOSS Settings  SID: %d, DRVID: %d, Address: %d'%(sid,drvid,addr ), file=cprf_file)

		print("\nCPRF Settings:" , file=cprf_file)
		print("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)
		print("| Rail |vrm_addr_offset|Resource_ID|volt_vote_en|clk_mgmt_en|ctrl_intr_en|pr_handshake_en|comp_req_dcvs|comp_req_cprc|re_enb_starv|not_vote_eq_volt|" , file=cprf_file)
		print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)

		for m in range(0,no_cprf):
			rail = targ_spec_config[target]['arc']['rm'][m]
			vrm_set_addr = cprf_vrm_addr + (0x1000*m)
			vrm_set_data = memory.read(vrm_set_addr, 4) 
			cprf_vrm_set = struct.unpack('<L', vrm_set_data)[0]
			vrm_addr_offset = hex((cprf_vrm_set >> 13) & 0xffff)
			resource_id     = hex(cprf_vrm_set & 0xff)

			cntrl_set_addr = cprf_cntrl_addr + (0x1000*m)
			ctrl_set_data = memory.read(cntrl_set_addr, 4) 
			cprf_ctrl_set = struct.unpack('<L', ctrl_set_data)[0]
			volt_vote_en = cprf_ctrl_set & 0x1
			clk_mgmt_en      = ((cprf_ctrl_set >> 1) & 0x1)
			ctrl_intr_en     = ((cprf_ctrl_set >> 2) & 0x1)
			pr_handshake_en  = ((cprf_ctrl_set >> 3) & 0x1)
			comp_req_dcvs    = ((cprf_ctrl_set >> 4) & 0x1)
			comp_req_cprc    = ((cprf_ctrl_set >> 5) & 0x1)
			re_enb_starvation = ((cprf_ctrl_set >> 6) & 0x1)
			not_vote_eq_volt = ((cprf_ctrl_set >> 7) & 0x1)
			print("|{0:>6s}|{1:>15s}|{2:>11s}|{3:>12d}|{4:>11d}|{5:>12d}|{6:>15d}|{7:>13d}|{8:>13d}|{9:>12d}|{10:>16d}|" .format( \
                                                rail,\
                                                vrm_addr_offset,\
                                                resource_id,\
                                                volt_vote_en,\
                                                clk_mgmt_en,\
                                                ctrl_intr_en,\
                                                pr_handshake_en,\
                                                comp_req_dcvs,\
                                                comp_req_cprc,\
                                                re_enb_starvation,\
                                                not_vote_eq_volt,\
                                                ), file=cprf_file)
		print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)

		for m in range(0,no_cprf):
			rail = targ_spec_config[target]['arc']['rm'][m]
			print("\n\nRail: %s:"% rail , file=cprf_file)

			enb_addr = enable_addr + (0x1000*m)
			enb_data = memory.read(enb_addr, 4) 
			enb_value = struct.unpack('<L', enb_data)[0]
			print("Enable: %d"% enb_value , file=cprf_file)

			vals_addr = values_addr + (0x1000*m)
			values_data = memory.read(vals_addr, 4) 
			vlaues = struct.unpack('<L', values_data)[0]
			cur_ol = ((vlaues >> 16) & 0x1f)
			tent_vol = vlaues & 0x1fff
			#print("Current OL: %d, TENT_VOL: %d"% (cur_ol,tent_vol) , file=cprf_file)

			val_1_addr = value_1_addr + (0x1000*m) 
			val_1_data = memory.read(val_1_addr, 4)
			val_1      = struct.unpack('<L', val_1_data)[0]
			curr_vol    = ((val_1 >> 16) & 0x1fff)
			vol_vote    = (val_1  & 0x1fff)
			#print("Current Vol: %d, Voltage Vote: %d"% (curr_vol,vol_vote) , file=cprf_file)

			#print("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)
			#print("\t| OL |Enb_count| Vmax | Vmin | Vsafe | Current | Min_vol |Up_count|Down_count|", file=cprf_file)
			#print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)
			print("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)
			print("\t| OL |  Level  | Vmax | Vmin | Vsafe |", file=cprf_file)
			print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)
			for  lvl in range(0,no_corners):
				volt_lmt_addr = volt_limits_addr + (0x1000*m) + (0x4 *lvl)
				volt_lmt_data = memory.read(volt_lmt_addr, 4)
				volt_lmt      = struct.unpack('<L', volt_lmt_data)[0]
				vmax          = ((volt_lmt >> 16) & 0x1fff)
				vmin          = (volt_lmt  & 0x1fff)

				volt_sf_addr = volt_safe_addr + (0x1000*m) + (0x4 *lvl)
				volt_sf_data = memory.read(volt_sf_addr, 4)
				volt_sf      = struct.unpack('<L', volt_sf_data)[0]
				vsafe        = (volt_sf  & 0x1fff)

				val_2_addr = value_2_addr + (0x1000*m) + (0x4 *lvl)
				val_2_data = memory.read(val_2_addr, 4)
				val_2      = struct.unpack('<L', val_2_data)[0]
				max_vol    = ((val_2 >> 16) & 0x1fff)
				min_vol    = (val_2  & 0x1fff)

				val_3_addr = value_3_addr + (0x1000*m) + (0x4 *lvl)
				val_3_data = memory.read(val_3_addr, 4)
				val_3      = struct.unpack('<L', val_3_data)[0]
				last_vol   = (val_3  & 0x1fff)

				val_4_addr = value_4_addr + (0x1000*m) + (0x4 *lvl)
				val_4_data = memory.read(val_4_addr, 4)
				val_4      = struct.unpack('<L', val_4_data)[0]
				ol_count   = (val_4  & 0xff)

				val_5_addr = value_5_addr + (0x1000*m) + (0x4 *lvl)
				val_5_data = memory.read(val_5_addr, 4)
				val_5      = struct.unpack('<L', val_5_data)[0]
				up_count   = (val_5  & 0xff)
				down_count = ((val_5 >>16) & 0xff)

				level = rail_level[g_res[m].vlvls[lvl]]
				if lvl != 0 and level == 'OFF':
					break

				print("\t|{0:>4d}|{1:>9s}|{2:>6d}|{3:>6d}|{4:>7d}|" .format( lvl,level, vmax, vmin, vsafe), file=cprf_file)
                #print("\t|{0:>4d}|{1:>9d}|{2:>6d}|{3:>6d}|{4:>7d}|{5:>9d}|{6:>9d}|{7:>8d}|{8:>10d}|" .format( \
                #                                lvl,\
                #                                ol_count,\
                #                                vmax,\
                #                                vmin,\
                #                                vsafe,\
                #                                last_vol,\
                #                                min_vol,\
                #                                up_count,\
                #                                down_count,\
                #                                ), file=cprf_file)
			print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=cprf_file)
