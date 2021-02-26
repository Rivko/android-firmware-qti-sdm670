from __future__ import print_function
import struct
import logging
logger = logging.getLogger(__name__)

import os
import itertools
import math
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from pdc_registers import masks, unit_masks, pdc_fields, version, status_val, enable_pdc_val, disable_clk_gate_val, enable_bank_val, irq_status_val, irq_cfg_val
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *
from targ_spec_config import *
from summary import chip_version



def dump(debug_data): 
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
	subs_list= pdc_ss_list [target]
	
	for i in range(len(subs_list)):
		str_addr="pdc_"+subs_list[i]+"_baseaddr"
		exec("%s = %s" % (str_addr,pdc_global[target][str_addr]))
		
		str_mode_addr=subs_list[i]+"_pdc_mode_status_baseaddr"
		exec("%s = %s" % ( str_mode_addr,eval("pdc_"+subs_list[i])[target]["pdc_mode_status_baseaddr"]))
		str_no=subs_list[i]+"_no_drv"
		exec("%s = %d" % ( str_no,eval("pdc_"+subs_list[i])[target]["no_drv"]))
		
		str_mode_st=subs_list[i]+"_pdc_mode_status"
		exec("%s = %d" % ( str_mode_st,pdc_fields[target]["pdc_mode_status"]))

		str_enable_addr=subs_list[i]+"_enable_pdc_baseaddr"
		exec("%s = %s" % ( str_enable_addr,eval("pdc_"+subs_list[i])[target]["enable_pdc_baseaddr"]))
		str_enable=subs_list[i]+"_enable_pdc"
		exec("%s = %d" % ( str_enable,pdc_fields[target]["enable_pdc"]))
		str_enable_clk=subs_list[i]+"_disable_clk_gate"
		exec("%s = %d" % ( str_enable_clk,pdc_fields[target]["disable_clk_gate"]))

		str_lo_addr=subs_list[i]+"_timer_match_value_lo_baseaddr"
		exec("%s = %s" % ( str_lo_addr,eval("pdc_"+subs_list[i])[target]["timer_match_value_lo_baseaddr"]))
		str_lo=subs_list[i]+"_data_lo_upto"
		exec("%s = %d" % ( str_lo,pdc_fields[target]["data_lo_upto"]))

		str_hi_addr=subs_list[i]+"_timer_match_value_hi_baseaddr"
		exec("%s = %s" % ( str_hi_addr,eval("pdc_"+subs_list[i])[target]["timer_match_value_hi_baseaddr"]))
		str_hi=subs_list[i]+"_data_hi_upto"
		exec("%s = %d" % ( str_hi,pdc_fields[target]["data_hi_upto"]))

		if (subs_list[i]!="debug"):
			str_prof_addr=subs_list[i]+"_profiling_output_baseaddr"
			exec("%s = %s" % ( str_prof_addr,eval("pdc_"+subs_list[i])[target]["profiling_output_baseaddr"]))
		
			str_prof_no_unit=subs_list[i]+"_no_units"
			exec("%s = %d" % ( str_prof_no_unit,eval("pdc_"+subs_list[i])[target]["no_units"]))
			
			str_tm_lo_addr=subs_list[i]+"_profiling_timestamp_lo_baseaddr"
			exec("%s = %s" % ( str_tm_lo_addr,eval("pdc_"+subs_list[i])[target]["profiling_timestamp_lo_baseaddr"]))
		
			str_tm_hi_addr=subs_list[i]+"_profiling_timestamp_hi_baseaddr"
			exec("%s = %s" % ( str_tm_hi_addr,eval("pdc_"+subs_list[i])[target]["profiling_timestamp_hi_baseaddr"]))
		
		
		try:
			ver_data = memory.read(pdc_global[target][str("pdc_"+subs_list[i]+"_baseaddr")], 4)
			
		except:
			debug_data['printer'].p("\tPDC registers bin is not recovered...")
			return

	
	if (dump_error != 0):
		debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
	else:
		debug_data['printer'].p("\t...DONE")

	
	if not os.path.exists(dump_path+"\pdc"):
		os.makedirs(dump_path+"\pdc")
	
	pdc_summary_file_name = os.path.join(dump_path+"\pdc", 'pdc_summary.txt')


	with open(pdc_summary_file_name, 'w') as pdc_summary_file:
		
		print("\n\t\t ~~PDC Summary~~", file=pdc_summary_file)
		
		############################################################
		for xx in range(len(subs_list)):
		
			#module
			print("\n\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=pdc_summary_file)
			print ("\t\t {0:20s}".format(str("RPMH_PDC_"+subs_list[xx].upper())),file=pdc_summary_file)
			print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", file=pdc_summary_file)
			
			#drv_ids base
			no_drv = int(eval(subs_list[xx]+"_no_drv"))
			bs_addr = eval("pdc_"+subs_list[xx]+"_baseaddr")
			
			#print ("\t\t ~~~RSC_DRV_ID~~~\n\t\t Base address\t = {0:<20s}".format((hex(bs_addr))), file=pdc_summary_file)
			print ("\t\t ~~~RSC_DRV_ID~~~", file=pdc_summary_file)
			print ("\t\t No. of DRV's\t = {0:<20d}".format((no_drv)), file=pdc_summary_file)
			
			for drv in range(no_drv):
				needed_addr = bs_addr + (0x4*drv)
				reg_data = memory.read(needed_addr, 4) 
				reg_val = struct.unpack('<L', reg_data)[0]
				DRV_ID =  (reg_val) & 0xF
				print("\t\t DRV [%d] \t DRV_ID: %s" % ( drv,  hex(DRV_ID).rstrip("L") ), file=pdc_summary_file)
		
		############################################################
				
			#mode status base
			print("\n", file=pdc_summary_file)
			#print ("\t\t ~~~MODE_STATUS~~~\n\t\t Base address\t = {0:<20s}".format(hex(eval(subs_list[xx]+"_pdc_mode_status_baseaddr" ))), file=pdc_summary_file)
			print ("\t\t ~~~MODE_STATUS~~~", file=pdc_summary_file)
			#drv
			print ("\t\t No. of DRV's\t = {0:<20d}".format((eval(subs_list[xx]+"_no_drv" ))), file=pdc_summary_file)
			#mode status 			
			for d in range(0,int(eval(subs_list[xx]+"_no_drv" ))):
				needed_addr = eval(subs_list[xx]+"_pdc_mode_status_baseaddr") + (0x10000*d) 
				reg_data = memory.read(needed_addr, 4) 
				reg_val = struct.unpack('<L', reg_data)[0]
				if (eval(subs_list[xx]+"_pdc_mode_status")==0):
					param=reg_val&0x1
				else:
					param=-1
				print("\t\t DRV [%d]:\t PDC_MODE_STATUS = %s ( %s )" % ( d, hex(param).rstrip("L"), status_val[param] ), file=pdc_summary_file)	
		
		############################################################
			
			#enable pdc
			print("\n", file=pdc_summary_file)
			#print ("\t\t ~~~ENABLE_PDC~~~\n\t\t Base address\t = {0:<20s}".format(hex(eval(subs_list[xx]+"_enable_pdc_baseaddr" ))), file=pdc_summary_file)
			print ("\t\t ~~~ENABLE_PDC~~~", file=pdc_summary_file)
			#params
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
			
			print("\t\t ENABLE_PDC\t = %s ( %s )" % ( hex(param1).rstrip("L"),enable_pdc_val[param1] ), file=pdc_summary_file)			
			print("\t\t DISABLE_CLK_GATE = %s ( %s )" % ( hex(param2).rstrip("L"),disable_clk_gate_val[param2] ), file=pdc_summary_file)	
			
		############################################################
				 
			#timer lo
			print("\n", file=pdc_summary_file)
			print ("\t\t ~~~NEXT WAKE UP TIME~~~", file=pdc_summary_file)
			reg_data_lo = memory.read(eval(subs_list[xx]+"_timer_match_value_lo_baseaddr"), 4) 
			reg_val_lo = struct.unpack('<L', reg_data_lo)[0]
			if (eval(subs_list[xx]+"_data_lo_upto")==31):
				param_lo = reg_val_lo
			else:
				param_lo= reg_val_lo & (masks[eval(subs_list[xx]+"_data_lo_upto")+1])
			 
			#timer hi
			reg_data_hi = memory.read(eval(subs_list[xx]+"_timer_match_value_hi_baseaddr"), 4) 
			reg_val_hi = struct.unpack('<L', reg_data_hi)[0]
			if (eval(subs_list[xx]+"_data_hi_upto")==31):
				param_hi = reg_val_hi
			else:
				param_hi= reg_val_hi & (masks[eval(subs_list[xx]+"_data_hi_upto")+1])
			#import pdb;pdb.set_trace()
			param_val= str("{0:032b}".format(param_hi)+"{0:032b}".format(param_lo))
			
			print("\t\t Value =  {0:s}".format(hex(int(param_val,2)).rstrip("L")) , file=pdc_summary_file)
	
	############################################################

			print("\n", file=pdc_summary_file)
			print ("\t\t ~~~PDC TIMELINES~~~", file=pdc_summary_file)

			if (subs_list[xx]!="debug"): # no profilng for debug
				#import pdb; pdb.set_trace()
				bs_addr_out = eval(subs_list[xx]+"_profiling_output_baseaddr")
				bs_addr_lo = eval(subs_list[xx]+"_profiling_timestamp_lo_baseaddr") 
				bs_addr_hi = eval(subs_list[xx]+"_profiling_timestamp_hi_baseaddr")
				
				for d in range(eval(subs_list[xx]+"_no_drv")):
				
					for pf in range (0,eval(subs_list[xx]+"_no_units")):
						
						needed_addr_out = bs_addr_out + (0x10000 * d) + (0x14 * pf) 
						reg_data_out = memory.read(needed_addr_out, 4) 
						reg_val = struct.unpack('<L', reg_data_out)[0]
						
						needed_addr_lo = bs_addr_lo + (0x10000*d) + (0x14*pf) 
						reg_data_lo = memory.read(needed_addr_lo, 4) 
						reg_val_lo = struct.unpack('<L', reg_data_lo)[0]
						
						needed_addr_hi = bs_addr_hi + (0x10000*d) + (0x14*pf) 
						reg_data_hi = memory.read(needed_addr_hi, 4) 
						reg_val_hi = struct.unpack('<L', reg_data_hi)[0]
						
						if (pf==0):
							dnStartValid=(int(reg_val)&(1<<0))
							dnStartOverflow=(int(reg_val)&(1<<9))
							dnStart=(int(reg_val_hi)<<32)|(int(reg_val_lo))
					
						elif (pf==1):
							upEndValid=(int(reg_val)&(1<<0))
							upEndOverflow=(int(reg_val)&(1<<8))
							upEnd=(int(reg_val_hi)<<32)|(int(reg_val_lo))

						elif (pf==2):
							dnEndValid=(int(reg_val)&(1<<0))
							dnEndOverflow=(int(reg_val)&(1<<8))
							dnEnd=(int(reg_val_hi)<<32)|(int(reg_val_lo))
							
						elif (pf==3):
							upStartValid=(int(reg_val)&(1<<0))
							upStartOverflow=(int(reg_val)&(1<<9))
							upStart=(int(reg_val_hi)<<32)|(int(reg_val_lo))
				

					print("\t\t DRV [ %d ]:" % (d),file=pdc_summary_file)
					print("\t\t\t      PDC Sleep sequence Start : " + str(dnStart) + " [Valid: "+str(bool(dnStartValid))+", Overflow: "+str(bool(dnStartOverflow))+"]" ,file=pdc_summary_file)
					print("\t\t\t      PDC Sleep sequence End   : " + str(dnEnd) + " [Valid: "+str(bool(dnEndValid))+", Overflow: "+str(bool(dnEndOverflow))+"]"  ,file=pdc_summary_file)
					print("\t\t\t      PDC Wake sequence Start  : " + str(upStart) + " [Valid: "+str(bool(upStartValid))+", Overflow: "+str(bool(upStartOverflow))+"]" ,file=pdc_summary_file)
					print("\t\t\t      PDC Wake sequence End    : " + str(upEnd) + " [Valid: "+str(bool(upEndValid))+", Overflow: "+str(bool(upEndOverflow))+"]" ,file=pdc_summary_file)
					print("\t\t\t      ------------------------------------------------",file=pdc_summary_file)
					print("\t\t\t      PDC Sleep enter time     :"+ str(dnEnd-dnStart) + " ticks == " + str((dnEnd-dnStart)/19.2) + " us",file=pdc_summary_file)
					print("\t\t\t      PDC Sleep exit time      :"+ str(upEnd-upStart) + " ticks == " + str((upEnd-upStart)/19.2) + " us",file=pdc_summary_file)
					print("\n",file=pdc_summary_file)
					
					
	