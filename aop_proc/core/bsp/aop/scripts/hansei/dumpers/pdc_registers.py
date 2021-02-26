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

masks = {
    0 : 0x00000000,
    1 : 0x00000001,
    2 : 0x00000003,
    3 : 0x00000007,
    4 : 0x0000000F,
    5 : 0x0000001F,
    6 : 0x0000003F,
    7 : 0x0000007F,
    8 : 0x000000FF,
    9 : 0x000001FF,
    10: 0x000003FF,
    11: 0x000007FF,
    12: 0x00000FFF,
    13: 0x00001FFF,
    14: 0x00003FFF,
    15: 0x00007FFF,
    16: 0x0000FFFF,
    17: 0x0001FFFF,
    18: 0x0003FFFF,
    19: 0x0007FFFF,
    20: 0x000FFFFF,
    21: 0x001FFFFF,
    22: 0x003FFFFF,
    23: 0x007FFFFF,
    24: 0x00FFFFFF,
}

unit_masks = {
0:0x1,
1:0x2,
2:0x4,
3:0x8,
4:0x10,
5:0x20,
6:0x40,
7:0x80,
8:0x100,
9:0x200,
10:0x400,
11:0x800,
12:0x1000,
13:0x2000,
14:0x4000,
15:0x8000,
16:0x10000,
17:0x20000,
18:0x40000,
19:0x80000,
20:0x100000,
21:0x200000,
22:0x400000,
23:0x800000,
24:0x1000000,
25:0x2000000,
26:0x4000000,
27:0x8000000,
28:0x10000000,
29:0x20000000,
30:0x40000000,
31:0x80000000,
}

# pdc_fields ---> bit-index for the field values 
# 1. single bit field name ---> mentioned as-is 
# 2. the fields covering a range of bits starting from zero to a particular bit index ---> mentioned using "_upto" suffix
pdc_fields={
	'670' :{ 
		'irq_enable_bank_upto'				: 31,
		'irq_cfg_upto'						: 2,
		'irq_status_bank_upto'				: 31,
		'pdc_mode_status'					: 0,
		'enable_pdc'						: 0,
		'disable_clk_gate'					: 31,
		'data_lo_upto'						: 31,
		'data_hi_upto'						: 23,
				
				
	},
}

version = {
	'sensor_pdc_version' : {
		0 : {
			'name' : 'STEP_VER',
			'end'  : 7,
		},
		8 : {
			'name' : 'MINOR_VER',
			'end'  : 15,
		},
		16 : {
			'name' : 'MAJOR_VER',
			'end'  : 23,
		},
		
	},
}

status_val = {
	0: "PDC_IN_PASS_THROUGH_MODE",
	1: "PDC_RUNNING_SEQUENCE",
}

enable_pdc_val = {
	0: "PDC_DISABLE",
	1: "PDC_ENABLE",
}
disable_clk_gate_val = {
	0: "AUTO_CLK_GATE",
	1: "DISABLE_CLK_GATE",
}	

enable_bank_val = {
	0: "DISABLE_EVENT_FROM_WAKING_UP_PDC",
	1: "ENABLE_INTERRUPT_TO_WAKEUP_PDC",
}

irq_status_val = {
	0: "INTERRUPT_NOT_OCCURED",
	1: "INTERRUPT_OCCURED",
}

irq_cfg_val = {
	0: "LEVEL_SENSITIVE_ACTIVE_LOW",
	1: "RISING_EDGE_SENSITIVE_ACTIVE_LOW",
	2: "FALLING_EDGE_SENSITIVE_ACTIVE_LOW",
	3: "DUAL_EDGE_SENSITIVE_ACTIVE_LOW",
	4: "LEVEL_SENSITIVE_ACTIVE_HIGH",
	5: "FALLING_EDGE_SENSITIVE_ACTIVE_HIGH",
	6: "RISING_EDGE_SENSITIVE_ACTIVE_HIGH",
	7: "DUAL_EDGE_SENSITIVE_ACTIVE_HIGH",
					
}



def dump(debug_data): #dump_path, memory, debug_info, fill_char):
	global dump_error
	dump_error = 0
	memory = debug_data['rpm_memory']
	debug_info = debug_data['debug_info']
	fill_char = debug_data['args'].fill_char
	dump_path = debug_data['dump_path']
	target = debug_data['target']
	target_ver = target+'-'+str(chip_version)
	if target_ver in targ_spec_config:
		target = target_ver
	debug_data['printer'].p("Dumping pdc_sensor registers...")
	#import pdb; pdb.set_trace()
	subs_list= pdc_ss_list [target]
	
	for i in range(len(subs_list)):
			
		str_mode_addr=subs_list[i]+"_pdc_mode_status_baseaddr"
		exec("%s = %s" % ( str_mode_addr,eval("pdc_"+subs_list[i])[target]["pdc_mode_status_baseaddr"]))
		str_mode_no=subs_list[i]+"_no_drv"
		exec("%s = %d" % ( str_mode_no,eval("pdc_"+subs_list[i])[target]["no_drv"]))
		str_mode_st=subs_list[i]+"_pdc_mode_status"
		exec("%s = %d" % ( str_mode_st,pdc_fields[target]["pdc_mode_status"]))

		str_enable_addr=subs_list[i]+"_enable_pdc_baseaddr"
		exec("%s = %s" % ( str_enable_addr,eval("pdc_"+subs_list[i])[target]["enable_pdc_baseaddr"]))
		str_enable=subs_list[i]+"_enable_pdc"
		exec("%s = %d" % ( str_enable,pdc_fields[target]["enable_pdc"]))
		str_enable_clk=subs_list[i]+"_disable_clk_gate"
		exec("%s = %d" % ( str_enable_clk,pdc_fields[target]["disable_clk_gate"]))
		
		str_seq_addr=subs_list[i]+"_seq_busy_drv_baseaddr"
		exec("%s = %s" % ( str_seq_addr,eval("pdc_"+subs_list[i])[target]["seq_busy_drv_baseaddr"]))
		
		str_lo_addr=subs_list[i]+"_timer_match_value_lo_baseaddr"
		exec("%s = %s" % ( str_lo_addr,eval("pdc_"+subs_list[i])[target]["timer_match_value_lo_baseaddr"]))
		str_lo=subs_list[i]+"_data_lo_upto"
		exec("%s = %d" % ( str_lo,pdc_fields[target]["data_lo_upto"]))

		str_hi_addr=subs_list[i]+"_timer_match_value_hi_baseaddr"
		exec("%s = %s" % ( str_hi_addr,eval("pdc_"+subs_list[i])[target]["timer_match_value_hi_baseaddr"]))
		str_hi=subs_list[i]+"_data_hi_upto"
		exec("%s = %d" % ( str_hi,pdc_fields[target]["data_hi_upto"]))
		
		str_ver_addr=subs_list[i]+"_pdc_version_drv_baseaddr"
		exec("%s = %s" % ( str_ver_addr,eval("pdc_"+subs_list[i])[target]["pdc_version_drv_baseaddr"]))
		
		if ((subs_list[i]!="gpu") & (subs_list[i]!="display")):
			
			str_irq_enable_addr=subs_list[i]+"_irq_enable_bank_baseaddr"
			exec("%s = %s" % ( str_irq_enable_addr,eval("pdc_"+subs_list[i])[target]["irq_enable_bank_baseaddr"]))
			str_irq_enable_no_bank=subs_list[i]+"_no_bank"
			exec("%s = %d" % ( str_irq_enable_no_bank,eval("pdc_"+subs_list[i])[target]["no_bank"]))
			str_irq_enable_upto=subs_list[i]+"_irq_enable_bank_upto"
			exec("%s = %d" % ( str_irq_enable_upto,pdc_fields[target]["irq_enable_bank_upto"]))
			
			str_irq_cfg_addr=subs_list[i]+"_irq_cfg_baseaddr"
			exec("%s = %s" % ( str_irq_cfg_addr,eval("pdc_"+subs_list[i])[target]["irq_cfg_baseaddr"]))
			str_irq_cfg_no_i=subs_list[i]+"_no_irq"
			exec("%s = %d" % ( str_irq_cfg_no_i,eval("pdc_"+subs_list[i])[target]["no_irq"]))
			str_irq_cfg_upto=subs_list[i]+"_irq_cfg_upto"
			exec("%s = %d" % ( str_irq_cfg_upto,pdc_fields[target]["irq_cfg_upto"]))
			
			str_irq_st_addr=subs_list[i]+"_irq_status_bank_baseaddr"
			exec("%s = %s" % ( str_irq_st_addr,eval("pdc_"+subs_list[i])[target]["irq_status_bank_baseaddr"]))
			
			str_irq_st_upto=subs_list[i]+"_irq_status_bank_upto"
			exec("%s = %d" % ( str_irq_st_upto,pdc_fields[target]["irq_status_bank_upto"]))
			

	
	if (dump_error != 0):
		debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
	else:
		debug_data['printer'].p("\t...DONE")
	if not os.path.exists(dump_path+"\pdc"):
		os.makedirs(dump_path+"\pdc")
	
	for xx in range (len(subs_list)):
		pdc_mod_file_name = os.path.join(dump_path+"\pdc", "pdc_"+subs_list[xx]+".txt")
		
		with open(pdc_mod_file_name, 'w') as pdc_module_file:

			print("\n ~~PDC "+subs_list[xx].upper()+" Summary~~", file=pdc_module_file)

			############################################################
			
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_PDC_VERSION_DRV")), file=pdc_module_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print("\t\t No. of DRV's = %d" % (eval(subs_list[xx]+"_no_drv")), file=pdc_module_file)
			print("", file=pdc_module_file)
			
			for d in range(eval(subs_list[xx]+"_no_drv")):
				needed_addr = eval(subs_list[xx]+"_pdc_version_drv_baseaddr") + (0x10000*d) 
				reg_data = memory.read(needed_addr, 4) 
				reg_val = struct.unpack('<L', reg_data)[0]
				bitfield = version['sensor_pdc_version']
				print("\t\t DRV [%d]:" % ( d ), file=pdc_module_file)			
				
				index = 0
				while index < 23:
					region = bitfield[index]
					data = reg_val
					data = data >> index
					mask = 1
					for i in range(index, region['end']):
						mask = (mask << 1) + 1
					data = data & mask
					#out_str = "\t\t\t %s : %s "%(region['name'], hex(data))            
					print("\t\t {0:11s}{1:<10s} = {2:10s}".format( "",region['name'], hex(data).rstrip("L") ), file=pdc_module_file)		
					#print(out_str, file=pdc_module_file)
					index = region['end']+1
	
			############################################################
		
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_PDC_MODE_STATUS_DRV")), file=pdc_module_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print("\t\t No. of DRV's = %d" % (eval(subs_list[xx]+"_no_drv")), file=pdc_module_file)
			print("", file=pdc_module_file)
			
			for d in range(eval(subs_list[xx]+"_no_drv")):
				needed_addr = eval(subs_list[xx]+"_pdc_mode_status_baseaddr") + (0x10000*d) 
				reg_data = memory.read(needed_addr, 4) 
				reg_val = struct.unpack('<L', reg_data)[0]
				if (eval(subs_list[xx]+"_pdc_mode_status")==0):
					param=reg_val&0x1
				else:
					param=-1
				
				print("\t\t DRV [%d]:"% ( d), file=pdc_module_file)			
				print("\t\t {0:11s}{1:<16s} = {2:s} ({3:s})".format( "","PDC_MODE_STATUS", hex(param).rstrip("L"), status_val[param]  ), file=pdc_module_file)		
					 
			############################################################
			
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_ENABLE_PDC")), file=pdc_module_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print("", file=pdc_module_file)
			
			reg_data = memory.read(eval(subs_list[xx]+"_enable_pdc_baseaddr"), 4) 
			reg_val = struct.unpack('<L', reg_data)[0]
			if (eval(subs_list[xx]+"_enable_pdc")==0):
				param1=reg_val&0x1
			else:
				param1=-1
			if (eval(subs_list[xx]+"_disable_clk_gate")==31):
				param2=reg_val >> eval(subs_list[xx]+"_disable_clk_gate")
			else:
				param2=-1
						
			print("\t\t ENABLE_PDC: \t\t%s ( %s )" % ( hex(param1).rstrip("L"),enable_pdc_val[param1] ), file=pdc_module_file)			
			print("\t\t DISABLE_CLK_GATE: \t%s ( %s )" % ( hex(param2).rstrip("L"),disable_clk_gate_val[param2] ), file=pdc_module_file)			
							  
			############################################################
			
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_TIMER_MATCH_VALUE_LO")), file=pdc_module_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print("", file=pdc_module_file)
			
			reg_data = memory.read(eval(subs_list[xx]+"_timer_match_value_lo_baseaddr"), 4) 
			reg_val = struct.unpack('<L', reg_data)[0]
			if (eval(subs_list[xx]+"_data_lo_upto")==31):
				param = reg_val
			else:
				param= reg_val & (masks[eval(subs_list[xx]+"_data_lo_upto")+1])
			print("\t\t DATA_LO: %s " % ( hex(param).rstrip("L") ), file=pdc_module_file)			
			 
			############################################################
						 
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_TIMER_MATCH_VALUE_HI")), file=pdc_module_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print("", file=pdc_module_file)
			
			reg_data = memory.read(eval(subs_list[xx]+"_timer_match_value_hi_baseaddr"), 4) 
			reg_val = struct.unpack('<L', reg_data)[0]
			if (eval(subs_list[xx]+"_data_hi_upto")==31):
				param = reg_val
			else:
				param= reg_val & (masks[eval(subs_list[xx]+"_data_hi_upto")+1])
			print("\t\t DATA_HI: %s " % ( hex(param).rstrip("L") ), file=pdc_module_file)	
			
			############################################################
			
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_SEQ_BUSY_DRV")), file=pdc_module_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
			print("\t\t No. of DRV's = %d" % (eval(subs_list[xx]+"_no_drv")), file=pdc_module_file)
			print("", file=pdc_module_file)
			
			for d in range(eval(subs_list[xx]+"_no_drv")):
				needed_addr = eval(subs_list[xx]+"_seq_busy_drv_baseaddr") + (0x10000*d) 
				reg_data = memory.read(needed_addr, 4) 
				reg_val = struct.unpack('<L', reg_data)[0]
				param=reg_val&0x1
				print("\t\t DRV [%d]:" % ( d) , file=pdc_module_file)			
				print("\t\t {0:11s}{1:<10s} = {2:10s}".format( "","BUSY_STAT", hex(param).rstrip("L") ), file=pdc_module_file)		
					
			############################################################
			
			if ((subs_list[xx]!="gpu") & (subs_list[xx]!="display")):
			
				print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
				print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_IRQ_ENABLE_BANK_DRV")), file=pdc_module_file)
				print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
				print("\t\t No. of DRV's = %d" % (eval(subs_list[xx]+"_no_drv")), file=pdc_module_file)
				print("\t\t No. of Banks = %d" % (eval(subs_list[xx]+"_no_bank")), file=pdc_module_file)
				print("", file=pdc_module_file)
	
				for d in range(0,eval(subs_list[xx]+"_no_drv")):
					val_cntr=-32
					for b in range(0,eval(subs_list[xx]+"_no_bank")):
						val_cntr=val_cntr+32
						var_str="none"
						print("\t\t DRV [%d]   BANK [%d]:" % ( d, b), file=pdc_module_file)
						needed_addr = eval(subs_list[xx]+"_irq_enable_bank_baseaddr") + (0x10000*d) + (0x4*b)
						reg_data = memory.read(needed_addr, 4) 
						reg_val = struct.unpack('<L', reg_data)[0]
						for yy in range (0,32):
							if (reg_val & unit_masks[yy]!=0x0):
								var_str="not_none"
								print("\t\t {0:22s}{1:>7s}{2:>3d}{3:<100s}".format("", "IRQ [",yy+val_cntr,"] ( ENABLE_INTERRUPT_TO_WAKEUP_PDC )" ), file=pdc_module_file)
						if (var_str=="none"):
							print("\t\t {0:24s}{1:<10s}".format ( "","NONE") , file=pdc_module_file)
					print ("\n", file=pdc_module_file)
							
				############################################################
				
				print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
				print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_IRQ_STATUS_BANK_DRV")), file=pdc_module_file)
				print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
				print("\t\t No. of DRV's = %d" % (eval(subs_list[xx]+"_no_drv")), file=pdc_module_file)
				print("\t\t No. of Banks = %d" % (eval(subs_list[xx]+"_no_bank")), file=pdc_module_file)
				print("", file=pdc_module_file)
				
				for d in range(0,eval(subs_list[xx]+"_no_drv")):
					val_cntr=-32
					for b in range(0,eval(subs_list[xx]+"_no_bank")):
						val_cntr=val_cntr+32
						var_str="none"
						print("\t\t DRV [%d]   BANK [%d]:" % ( d, b), file=pdc_module_file)
						needed_addr = eval(subs_list[xx]+"_irq_status_bank_baseaddr") + (0x10000*d) + (0x4*b)
						reg_data = memory.read(needed_addr, 4) 
						reg_val = struct.unpack('<L', reg_data)[0]
						for yy in range (0,32):
							if (reg_val & unit_masks[yy]!=0x0):
								var_str="not_none"
								print("\t\t {0:22s}{1:>7s}{2:>3d}{3:<100s}".format("", "IRQ [",yy+val_cntr,"] ( INTERRUPT_OCCURED )" ), file=pdc_module_file)
						if (var_str=="none"):
							print("\t\t {0:24s}{1:<10s}".format ( "","NONE") , file=pdc_module_file)
					print ("\n", file=pdc_module_file)		
				
				############################################################
							 
				print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
				print('\t\t %s ' % (("RPMH_PDC_"+subs_list[xx].upper()+"_IRQ_CFG_DRV")), file=pdc_module_file)
				print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_module_file)
				print("\t\t No. of DRV's = %d" % (eval(subs_list[xx]+"_no_drv")), file=pdc_module_file)
				print("\t\t No. IRQ's = %d" % (eval(subs_list[xx]+"_no_irq")), file=pdc_module_file)
				print("", file=pdc_module_file)
			
				for d in range(eval(subs_list[xx]+"_no_drv")):
					for i in range(eval(subs_list[xx]+"_no_irq")):
					
						needed_addr = eval(subs_list[xx]+"_irq_cfg_baseaddr") + (0x10000*d) + (0x4*i)
						reg_data = memory.read(needed_addr, 4) 
						reg_val = struct.unpack('<L', reg_data)[0]
						if (eval(subs_list[xx]+"_irq_cfg_upto")==31):
							param = reg_val
						else:
							param= reg_val & (masks[eval(subs_list[xx]+"_irq_cfg_upto")+1])
				
						print("\t\t  DRV [%d]   IRQ [%03d]:     IRQ_CFG = %s ( %s )" % ( d, i, hex(param).rstrip("L"), irq_cfg_val[param] ), file=pdc_module_file)			
					print ("\n", file=pdc_module_file)	 
				############################################################
							 
