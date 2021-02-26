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
from arc import *
from vrm_vt import *
from bcm import *
from summary import chip_version
from cmd_db import vrm_map

tab_size = 8
dump_error = 0 # should only be used for error that indicate dump corruption

no_arc_drv = 0
no_arc_rm = 0

no_vrm_drv = 0
no_vrm_reg = 0

no_bcm_drv = 0
no_bcm_reg = 0

def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error
    global no_arc_rm, no_vrm_drv, no_vrm_reg
    global no_arc_drv, no_bcm_drv, no_bcm_reg
    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target'] 
    target_ver = target+'-'+str(chip_version)
    if target_ver in targ_spec_config:
		target = target_ver
    
    debug_data['printer'].p("Dumping Subsystem Votes on Resources...")

    try:
        ver_data = memory.read(0xb200010, 4)
    except:
        debug_data['printer'].p("\tRPMH registers bin is not recovered...")
        return

    no_arc_rm = targ_spec_config[target]['arc']['no_rm']
    no_arc_drv = targ_spec_config[target]['arc']['no_drv']
    no_vrm_drv = vrm[target]['no_drv']
    no_vrm_reg = vrm[target]['no_regs']
    no_bcm_drv = bcm[target]['no_drv']
    no_bcm_reg = bcm[target]['no_regs']

    dump_drv_vts(dump_path, memory, target, debug_info, fill_char)
    
    
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

def dump_drv_vts(dump_path, memory, target, debug_info, fill_char):
    dump_path = os.path.join(dump_path, 'reqs_by_master')
    if not os.path.exists(dump_path):
        os.makedirs(dump_path)
    
    for drv in range(0,no_arc_drv):
        ss_file_name = os.path.join(dump_path, '%s_drv%d.txt'% (targ_spec_config[target]['drv'][drv],drv))
        with open(ss_file_name, 'w') as ss_vt_file:
            print("\n\n\t ARC Resource Summary", file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)
            print("\t\t|Resource| RM | Vote | Level |", file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)
	  
            for rm in range(0,no_arc_rm):
	            arc_vt = ARC_votetable[rm][drv]
	            arc_lvl = ARC_votetable2[rm][drv]
	            if arc_vt != 0:
	                print("\t\t|{0:>8s}|{1:>4d}|{2:>6s}|{3:>7s}|" .format( targ_spec_config[target]['arc']['rm'][rm], rm, hex(arc_vt), arc_lvl), file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)

    for vrm_drv in range(0,no_vrm_drv):
        ss_file_name = os.path.join(dump_path, '%s_drv%d.txt'% (targ_spec_config[target]['drv'][vrm_drv], vrm_drv))
        with open(ss_file_name, 'a') as ss_vt_file:
            print("\n\n\t VRM resource Summary", file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)
            print("\t\t|      Resource      | VRM# | Vote | ENB | MODE |Headroom|", file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)	  
            for reg in range(0,no_vrm_reg):
	            vrm = VRM_votetable[reg][vrm_drv]
	            if vrm[0] != 0:
	                print("\t\t|{0:>20s}|{1:>6d}|{2:>6d}|{3:>5d}|{4:>6d}|{5:>8d}| ".format( vrm_map[reg], reg, vrm[0], vrm[1], vrm[2], vrm[3]), file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)	  

    for bcm_drv in range(0,no_bcm_drv):
        ss_file_name = os.path.join(dump_path, '%s_drv%d.txt'% (targ_spec_config[target]['drv'][bcm_drv], bcm_drv))
        with open(ss_file_name, 'a') as ss_vt_file:
            print("\n\n\t BCM resource Summary", file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)
            print("\t\t|Resource| BCM# | Valid |   AB   |   IB   |", file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)
	  
            for reg in range(0,no_bcm_reg):
	            
	            if bcm_map_table[reg] != ' ':
	                [vote_valid, vote_x, vote_y] = BCM_votetable[reg][bcm_drv]
	                if vote_valid != 0:
	                    print("\t\t|{0:>8s}|{1:>6d}|{2:>7d}|{3:>8s}|{4:>8s}| " .format( bcm_map_table[reg],reg, vote_valid, hex(vote_x),hex(vote_y)), file=ss_vt_file)
            print("\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", file=ss_vt_file)
