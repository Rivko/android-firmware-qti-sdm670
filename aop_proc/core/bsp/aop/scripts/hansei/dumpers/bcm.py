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
import json
from pprint import pprint
import os
import itertools
import math
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *
from targ_spec_config import *
import csv
from summary import chip_version
from cmd_db import bcm_reg_map, bcm_vcd_map



tab_size = 8
dump_error = 0 # should only be used for error that indicate dump corruption

# Hardcoding BASE addresses 
vt_base_address =  0 
bcm_status_fe = 0
bcm_status_be = 0
bcm_status_be_seq = 0

no_regs = 0
no_drv = 0 
no_vcds = 0

BCM_votetable = [[]]
bcm_map_table = bcm_reg_map
vcd_map_table= bcm_vcd_map
def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error, vt_base_address, bcm_status_fe, bcm_status_be
    global bcm_status_be_seq, no_regs, no_drv, no_vcds
    global BCM_votetable, bcm_map_table, vcd_map_table
    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target'] 
    bcm_json_dir = debug_data['bcm_json_dir']
    debug_data['printer'].p("Dumping BCM Vote table registers...")
    
    
    
    target_ver = target+'-'+str(chip_version)
    if target_ver in targ_spec_config:
        target = target_ver

    vt_base_address = bcm[target]['vt_base_address']
    bcm_status_fe = bcm[target]['bcm_status_fe']
    bcm_status_be = bcm[target]['bcm_status_be']
    bcm_status_be_seq = bcm[target]['bcm_status_be_seq']
    no_regs = bcm[target]['no_regs']
    no_drv = bcm[target]['no_drv']
    no_vcds = bcm[target]['no_vcds']
    BCM_votetable = [[0 for i in range(no_drv)] for j in range(no_regs)] 
    
    
    try:
        ver_data = memory.read(bcm_status_fe, 4)
    except:
        debug_data['printer'].p("\tBCM registers bin is not recovered...")
        return

    dump_bcm_vts(dump_path, memory, target, debug_info, fill_char,bcm_json_dir)
    
    
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

def dump_bcm_vts(dump_path, memory, target, debug_info, fill_char,bcm_json_dir):

    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)
    json_file_name = os.path.join(bcm_json_dir, str(target)+"/VCD.json")

    with open(json_file_name,'r') as data_file:    
        data = json.load(data_file)
####################################################################################################################        
    
    if (check_member('global_ddr_cfg ', aop_core_dump)):
        ddr_cfg = aop_core_dump.global_ddr_cfg 
        ddr_cfg = cast(ddr_cfg, 'DDRCfgType', memory, debug_info)
    else:
        try:
            ddr_cfg_address = debug_info.variables['global_ddr_cfg'].die
        except:
            ddr_file_name = os.path.join(dump_path, 'ddr_stats.txt')
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_CFG DOES NOT EXIST !!! \n", file=ddr_file)
            
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            ddr_gbl_address = debug_info.variables['global_ddr_cfg'].address # FIXME: get right address by magic
            ddr_cfg = decode_object('global_ddr_cfg', ddr_gbl_address, None, memory, debug_info, die=ddr_cfg_address)
        except:
            #not correct in elf, find absolute address (very hacky)
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_cfg DOES NOT EXIST !!! \n", file=ddr_file)
    ####################################################################################################################    
    if (check_member('ddr_status', aop_core_dump)):
        #ddr_status in in core dump
        ddr_status = aop_core_dump.ddr_status
        ddr_status = cast(ddr_status, 'ddr_state', memory, debug_info)###################################
    else:
        try:
            freq_switch_address = debug_info.variables['ddr_status'].die###################################
        except:
            ddr_file_name = os.path.join(dump_path, 'ddr_stats.txt')
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_STATUS DOES NOT EXIST !!! \n", file=ddr_file)
            return
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            ddr_status_address = debug_info.variables['ddr_status'].address # FIXME: get right address by magic
            ddr_status = decode_object('ddr_status', ddr_status_address, None, memory, debug_info, die=freq_switch_address)
        except:
            #not correct in elf, find absolute address (very hacky)
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_STATUS DOES NOT EXIST !!! \n", file=ddr_file)
            ####################################################################################################################    
    
    dump_path = os.path.join(dump_path, 'reqs_for_resources')
    if not os.path.exists(dump_path):
        os.makedirs(dump_path)        
    bcm_file_name = os.path.join(dump_path, 'bcm_vt.txt')
    summary_file_name = os.path.join(dump_path, 'rpmh_summary.txt')

    with open(bcm_file_name, 'w') as bcm_file, open(summary_file_name, 'a') as summary_file:

        print("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        print("\t| VCD |                  Nodes                  |", file=bcm_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        for x in range(0, no_vcds):
            print("\t|{0:>5d}|{1:>41s}|" .format( x, vcd_map_table[x] ), file=bcm_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)

        print("\n ~~BCM Status Front End~~", file=bcm_file)
        print("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        print("\t|       VCD             | AGG_BW | Final_CP | AGG_CP | DDR MGR Override AGG_CP |                   Freq_khz                     |", file=bcm_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        for x in range(0, no_vcds):
            address = bcm_status_fe +  (0x4*x)
            fe_data = memory.read(address, 4)
            fe_status = struct.unpack('<L', fe_data)[0]
            agg_cp = fe_status & 0xF
            final_cp = ((fe_status >> 4 ) & 0xF)
            agg_bw = ((fe_status >> 16 ) & 0x3FFF)

            # if x==0: #mc
                # for x in range(0, ddr_cfg.nNumMCStates):
                    # if (ddr_cfg.pMCCfg[x].clk_idx==agg_cp):
                        # frq=ddr_cfg.pMCCfg[x].freq_khz

            # elif x==1: #shub
                # for y in range(0, ddr_cfg.nNumSHUBStates):
                    # if (ddr_cfg.pSHUBCfg[y].clk_idx==agg_cp):
                        # frq=ddr_cfg.pSHUBCfg[y].freq_khz
            
            
            ddr_mc_cp=hex(ddr_status.current_mc_cp) 
            ddr_shub_cp=hex(ddr_status.current_shub_cp) 
                
                
            
            
            flag=0
            for key, value in data.items() :
                if (data[key]["VCD Index"]==x) :
                    if (x==0):
                        v5=str(ddr_mc_cp)
                        if (ddr_mc_cp!=0x0):
                            flag=1
                            var_print=key
                            v1=hex(agg_bw)
                            v2=hex(final_cp)
                            v3=hex(agg_cp)  
                            v4=str(x)+":"
                            for y in data[key]["Clock Domains"]:
                                print("\t|{0:3s}{1:>20s}|{2:>8s}|{3:>10s}|{4:>8s}|{5:>25s}|{6:30s}: {7:>17s}|" .format(v4,var_print,v1,v2,v3,v5,y["domain"],(str(y["freq"][int(ddr_mc_cp,16)]).split(":")[1])[3:-2]), file=bcm_file)
                                var_print=""  
                                v1=""
                                v2=""
                                v3=""
                                v4=""   
                                v5=""
                                
                    elif (x==1):
                        v5=str(ddr_shub_cp)
                        if (ddr_shub_cp!=0x0):
                            flag=1
                            var_print=key
                            v1=hex(agg_bw)
                            v2=hex(final_cp)
                            v3=hex(agg_cp)  
                            v4=str(x)+":"
                            for y in data[key]["Clock Domains"]:
                                print("\t|{0:3s}{1:>20s}|{2:>8s}|{3:>10s}|{4:>8s}|{5:>25s}|{6:30s}: {7:>17s}|" .format(v4,var_print,v1,v2,v3,v5,y["domain"],(str(y["freq"][int(ddr_shub_cp,16)]).split(":")[1])[3:-2]), file=bcm_file)
                                var_print=""  
                                v1=""
                                v2=""
                                v3=""
                                v4=""   
                                v5=""   
                    else:
                        v5="NA"
                        if (agg_cp!=0):
                            flag=1
                            var_print=key
                            v1=hex(agg_bw)
                            v2=hex(final_cp)
                            v3=hex(agg_cp)  
                            v4=str(x)+":"
                            for y in data[key]["Clock Domains"]:
                                print("\t|{0:3s}{1:>20s}|{2:>8s}|{3:>10s}|{4:>8s}|{5:>25s}|{6:30s}: {7:>17s}|" .format(v4,var_print,v1,v2,v3,v5,y["domain"],(str(y["freq"][agg_cp]).split(":")[1])[3:-2]), file=bcm_file)
                                var_print=""  
                                v1=""
                                v2=""
                                v3=""
                                v4=""   
                                v5=""                           
            
            if(flag==0):
                v6=str(x)+":"
                print("\t|{0:<23s}|{1:>8s}|{2:>10s}|{3:>8s}|{4:>25s}|{5:50s}|" .format( v6, hex(agg_bw), hex(final_cp), hex(agg_cp), "NA","") , file=bcm_file)
            print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
       
            
        
        print("\n ~~BCM Status Back End~~", file=bcm_file)
        print("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        print("\t| VCD | Clk_dest_state | Combined_CP | SW_CP_SNAP | Written_CP | CURR_CP |", file=bcm_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        for x in range(0, no_vcds):
            address = bcm_status_be +  (0x4*x)
            fe_data = memory.read(address, 4)
            fe_status = struct.unpack('<L', fe_data)[0]
            curr_cp = fe_status & 0xF
            written_cp = ((fe_status >> 4 ) & 0xF)
            sw_cp = ((fe_status >> 8 ) & 0xF)
            combined_cp = ((fe_status >> 12 ) & 0xF)
            clk_dest = ((fe_status >> 16 ) & 0xF)
            print("\t|{0:>5d}|{1:>16s}|{2:>13s}|{3:>12s}|{4:>12s}|{5:>9s}|" .format( x, hex(clk_dest), hex(combined_cp), hex(sw_cp), hex(written_cp), hex(curr_cp)), file=bcm_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)

        print("\n ~~BCM Back End Sequencer~~", file=bcm_file)
        print("\n ~~BCM Status~~", file=summary_file)
        print("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        print("\t| VCD | SEQ_CURR_PC | IDLE |", file=bcm_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
        for x in range(0, no_vcds):
            address = bcm_status_be_seq +  (0x4*x)
            fe_data = memory.read(address, 4)
            fe_status = struct.unpack('<L', fe_data)[0]
            seq_state = fe_status & 0x1
            curr_pc = ((fe_status >> 8 ) & 0xFF)
            print("\t|{0:>5d}|{1:>8s}|{2:>10s}|" .format( x, hex(curr_pc), hex(seq_state)), file=bcm_file)
            if seq_state:
                print("\tVCD %d is Idle" % (x) ,file=summary_file)
            else:
                print("\tVCD %d is Busy" % (x) ,file=summary_file)
        print("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)

        print("\n\n ~~BCM Resource Summary~~", file=bcm_file)
        #import pdb; pdb.set_trace()
        for regs in range(0,no_regs):
            if bcm_map_table[regs] == ' ':
                continue
            print('\n\n\t BCM [%d] : %s ' % (regs,bcm_map_table[regs]), file=bcm_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
            print("\t\t|    DRV   | ID | Valid |   AB   |   IB   |", file=bcm_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=bcm_file)
            
            for drv in range(0,no_drv):
                address = vt_base_address + (0x10000*drv) + (0x4*regs) 
                bcm_data = memory.read(address, 4) 
                bcm_vt = struct.unpack('<L', bcm_data)[0]
                vote_valid = ((bcm_vt >> 29) & 0x1 )
                vote_x = ((bcm_vt >> 14) & 0x3FFF )
                vote_y = (bcm_vt & 0x3FFF)
                if bcm_vt != 0:
                    print("\t\t|{0:>10s}|{1:>4d}|{2:>7d}|{3:>8s}|{4:>8s}|" .format((targ_spec_config[target]['drv'][drv]), drv, vote_valid, hex(vote_x),hex(vote_y) ), file=bcm_file)
                BCM_votetable[regs][drv] = [vote_valid, vote_x, vote_y]
