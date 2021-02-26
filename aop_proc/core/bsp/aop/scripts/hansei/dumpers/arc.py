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
from vrm_vt import arc_vrm_sts
#masks for length of bit region
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
    20: 0x00FFFFFF,
}
    
# bitfields for commond types
arc_vt = {
    'ARC_VOTE_TABLE' : {
        0 : {
            'name' : 'OL',
            'end'  : 3,
        },
        31 : {
            'name' : 'RR',
            'end'  : 31,
        },
    },
}

rm_ol_type = {
    'ARC_OL_STATUS' : {
        12 : {
            'name' : 'CURRENT_OL',
            'end'  : 15,
        },
        16 : {
            'name' : 'SOLVED_OL',
            'end'  : 19,
        },
        20 : {
            'name' : 'AGGREGATED_OL',
            'end'  : 23,
        },
        24 : {
            'name' : 'SEQUENCE_OL',
            'end'  : 27,
        },
        28 : {
            'name' : 'DESTINATION_OL',
            'end'  : 31,
        },
    },
}
tab_size = 8
dump_error = 0 # should only be used for error that indicate dump corruption

# Hardcoding BASE addresses 
vt_base_address = 0
rm_blk_enb_base_addr = 0
rm_ol_base_addr = 0
rm_seq_base_addr = 0 

 
no_rm = 0
no_drv = 0
ARC_votetable = [[]]#[[0 for i in range(no_drv)] for j in range(no_rm)] 
ARC_votetable2 = [[]]
g_res = 0
def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error
    global ARC_votetable, ARC_votetable2
    global no_rm
    global no_drv,g_res
    global vt_base_address, rm_blk_enb_base_addr, rm_ol_base_addr, rm_seq_base_addr 
    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target']
    target_ver = target+'-'+str(chip_version)
    if target_ver in targ_spec_config:
        target = target_ver
    debug_data['printer'].p("Dumping ARC Vote table registers...")

    vt_base_address = targ_spec_config[target]['arc']['vt_base_address']
    rm_blk_enb_base_addr = targ_spec_config[target]['arc']['rm_enb_base_addr']
    rm_ol_base_addr = targ_spec_config[target]['arc']['rm_ol_base_addr'] 
    rm_seq_base_addr = targ_spec_config[target]['arc']['rm_seq_base_addr'] 
    no_rm = targ_spec_config[target]['arc']['no_rm']
    no_drv = targ_spec_config[target]['arc']['no_drv']
    ARC_votetable = [[0 for i in range(no_drv)] for j in range(no_rm)]
    ARC_votetable2 = [[0 for i in range(no_drv)] for j in range(no_rm)]

    try:
        ver_data = memory.read(rm_ol_base_addr, 4)
    except:
        debug_data['printer'].p("\tARC registers bin is not recovered...")
        return

    g_res_type = debug_info.variables['g_res'].die
    g_res_address = debug_info.variables['g_res'].address
    g_res = decode_object('g_res', g_res_address, None, memory, debug_info, die=g_res_type)
    #import pdb; pdb.set_trace()
    dump_arc_summary(dump_path, memory, target, g_res, debug_info, fill_char)
    
    
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

def dump_arc_summary(dump_path, memory, target, g_res, debug_info, fill_char):
    dump_path = os.path.join(dump_path, 'reqs_for_resources')
    if not os.path.exists(dump_path):
        os.makedirs(dump_path)

    arc_file_name = os.path.join(dump_path, 'arc_vt.txt')
    summary_file_name = os.path.join(dump_path, 'rpmh_summary.txt')

    with open(arc_file_name, 'w') as arc_file, open(summary_file_name, 'a') as summary_file:
        
        print("\n ~~ARC Resource Summary~~", file=arc_file)
        #import pdb; pdb.set_trace()

        for rm in range(0,no_rm):
            
            # Reading whether RM Operational Levels
            ol_addr = rm_ol_base_addr + (0x4*rm)
            ol_data = memory.read(ol_addr, 4) 
            ol = struct.unpack('<L', ol_data)[0]
            CU_OL =  (ol >> 12 ) & 0x0000000F
            print("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=arc_file)
            try:
                print('\t %s : RM [%d] Current OL: %s  (%s), mv: %d, vrm_enb_sts: %d' % (targ_spec_config[target]['arc']['rm'][rm], rm, hex(CU_OL), get_level(CU_OL, rm, g_res,target ), arc_vrm_sts[rm][0],arc_vrm_sts[rm][1]), file=arc_file)
            except:
                print('\t %s : RM [%d] Current OL: %s  (%s)' % (targ_spec_config[target]['arc']['rm'][rm], rm, hex(CU_OL), get_level(CU_OL, rm, g_res,target )), file=arc_file)
            print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=arc_file)

            for drv in range(0,no_drv):
                address = vt_base_address + (0x10000*drv) + (0x10*rm) 
                arc_data = memory.read(address, 4) 
                arc_vt = struct.unpack('<L', arc_data)[0] & 0xF
                drv_name = targ_spec_config[target]['drv'][drv]
                level = get_level(arc_vt, rm, g_res,target )
                if arc_vt != 0:
                    print("\t\t{0:>15s} : DRV[{1:>2d}] Vote : {2:>1s} ({3:>7s})" .format(drv_name, drv, hex(arc_vt), level), file=arc_file)

        print("#################################################################################################", file=arc_file)
        print("\n ~~ARC VOTE TABLE Dump~~", file=arc_file)
        print("\n ~~ARC Status~~", file=summary_file)
        
        for rm in range(0,no_rm):
            # Reading whether RM is Enabled or not
            enb_addr = rm_blk_enb_base_addr + (0x4*rm)
            enb_data = memory.read(enb_addr, 4) 
            rm_enb = struct.unpack('<L', enb_data)[0]
            ol_addr = rm_ol_base_addr + (0x4*rm)
            ol_data = memory.read(ol_addr, 4) 
            ol = struct.unpack('<L', ol_data)[0]
            print('\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~', file=arc_file)
            print('\t %s : RM[%d] Vote Table' % (targ_spec_config[target]['arc']['rm'][rm],rm), file=arc_file)
            print('~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~', file=arc_file)
            print('\t\t RM Enabled = %d' % (rm_enb & 0x00000001), file=arc_file)
            
            bitfield = rm_ol_type['ARC_OL_STATUS']
            print("\t\tRegister Decode:".expandtabs(tab_size), file=arc_file)
            index = 12
            while index < 32 :
                region = bitfield[index]
                data = ol
                data = data >> index
                mask = 1
                for i in range(index, region['end']):
                    mask = (mask << 1) + 1
                data = data & mask
                out_str = "\t\t {0:>25s} = {1:>8s} ({2:>7s})".format(region['name'], hex(data), get_level(data, rm, g_res,target))            
                print(out_str.expandtabs(tab_size), file=arc_file)
                index = region['end']+1
            
            # Reading whether RM Sequencer status
            seq_addr = rm_seq_base_addr + (0x40*rm)
            seq_data = memory.read(seq_addr, 4) 
            seq = struct.unpack('<L', seq_data)[0]
            print('\t\t RM Sequencer Status' , file=arc_file)
            print('\t\t\t Busy = %d' % ((seq >>31) & 0x00000001), file=arc_file)
            
            if (seq >>31) & 0x00000001:
                print('\t RM %d is Busy' % (rm), file=summary_file)
            else:
                print('\t RM %d is Idle' % (rm), file=summary_file)
                
            print('\t\t\t PC = %s' % (hex(seq  & 0x00000fff)), file=arc_file)

            for drv in range(0,no_drv):
                address = vt_base_address + (0x10000*drv) + (0x10*rm) 
                arc_data = memory.read(address, 4) 
                arc_vt = struct.unpack('<L', arc_data)[0] & 0xF
                level = get_level(arc_vt, rm, g_res,target )
                ARC_votetable[rm][drv] = arc_vt
                ARC_votetable2[rm][drv] = level
                print("\t\t{0:>15s} : DRV[{1:>2d}] Vote : {2:>1s}  ({3:>7s}) " .format((targ_spec_config[target]['drv'][drv]), drv, hex(arc_vt), level), file=arc_file)

def get_level (arc_vt, rm, g_res, target):
    if targ_spec_config[target]['arc']['rm'][rm] in ['XO']:
        level = xo_level[g_res[rm].vlvls[arc_vt]]
    elif targ_spec_config[target]['arc']['rm'][rm] in ['DDR_SS']:
        level = ddrss_level[g_res[rm].vlvls[arc_vt]]
    else:
        level = rail_level[g_res[rm].vlvls[arc_vt]]
    return level
