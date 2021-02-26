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
from cmd_db import vrm_map
tab_size = 8
dump_error = 0 # should only be used for error that indicate dump corruption

# Hardcoding BASE addresses 
vt_base_address =  0
vt_en_base_address = 0
mode_vote = 0
headroom_vt= 0
volt_csr= 0
enb_csr= 0
mode_csr= 0
headroom_csr= 0
volt_wsr= 0
enb_wsr= 0
mode_wsr= 0
headroom_wsr= 0
volt_dsr= 0
enb_dsr= 0
mode_dsr= 0
headroom_dsr= 0
no_regs = 0
no_drv = 0 

arc_vrm_ary = ['vrm.cx','vrm.mx','vrm.ebi','vrm.lcx','vrm.lmx','vrm.gfx','vrm.mss']
arc_vrm_sts = [0,0,0,0,0,0,0]

VRM_votetable = [[]]
vrm_sts_table = [[]]
def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error, vt_base_address, vt_en_base_address, no_regs, no_drv, no_seq, VRM_votetable
    global mode_vote, headroom_vt, volt_csr, enb_csr, mode_csr, headroom_csr,vrm_sts_table, sequence_csr, status_csr
    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target'] 
    target_ver = target+'-'+str(chip_version)
    if target_ver in targ_spec_config:
        target = target_ver
    
    debug_data['printer'].p("Dumping VRM Vote table registers...")
    vt_base_address = vrm[target]['vt_base_address']
    vt_en_base_address = vrm[target]['vt_en_base_address']
    mode_vote       = vrm[target]['mode_vote']
    headroom_vt     = vrm[target]['headroom_vt']
    volt_csr        = vrm[target]['volt_csr']
    enb_csr         = vrm[target]['enb_csr']
    mode_csr        = vrm[target]['mode_csr']
    headroom_csr    = vrm[target]['headroom_csr']
    volt_wsr        = vrm[target]['volt_wsr']
    enb_wsr         = vrm[target]['enb_wsr']
    mode_wsr        = vrm[target]['mode_wsr']
    headroom_wsr    = vrm[target]['headroom_wsr']
    volt_dsr        = vrm[target]['volt_dsr']
    enb_dsr         = vrm[target]['enb_dsr']
    mode_dsr        = vrm[target]['mode_dsr']
    headroom_dsr    = vrm[target]['headroom_dsr']
    sequence_csr    = vrm[target]['sequence_csr'] #0x0B9F2000 
    status_csr      = vrm[target]['status_csr'] #0x0B9F2100

    no_regs = vrm[target]['no_regs']
    no_drv = vrm[target]['no_drv']
    no_seq = vrm[target]['no_seq']
    VRM_votetable = [[0 for i in range(no_drv)] for j in range(no_regs)]
    vrm_sts_table = [[0,0]  for k in range(no_regs)]
    try:
        ver_data = memory.read(vt_base_address, 4)
    except:
        debug_data['printer'].p("\tVRM registers bin is not recovered...")
        return

    dump_vrm_vts(dump_path, memory, target, debug_info, fill_char)
    find_vrm_arc_res();
    #import pdb; pdb.set_trace();
    dump_vrm_summary(dump_path, memory, target, debug_info, fill_char)
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

def dump_vrm_vts(dump_path, memory, target, debug_info, fill_char):
    dump_path = os.path.join(dump_path, 'reqs_for_resources')
    if not os.path.exists(dump_path):
        os.makedirs(dump_path)

    vrm_file_name = os.path.join(dump_path, 'vrm_vt.txt')
    with open(vrm_file_name, 'w') as vrm_file:
        
        print("\n ~~VRM Resource Summary~~", file=vrm_file)
        #import pdb; pdb.set_trace()
        for regs in range(0,no_regs):
            print('\n\n\t  %s VRM[%d] Summary' % (vrm_map[regs], regs), file=vrm_file)
            # volt_vote
            address = volt_csr + (0x100*regs) 
            vrm_data = memory.read(address, 4) 
            vrm_sts = (struct.unpack('<L', vrm_data)[0]) & 0x1fff
            
            # enb_vote
            address = enb_csr +  (0x100*regs) 
            enb_data = memory.read(address, 4) 
            enb_sts = (struct.unpack('<L', enb_data)[0]) & 0x1
            
            # mode_vote
            address = mode_csr + (0x100*regs) 
            mode_data = memory.read(address, 4) 
            mode_sts = (struct.unpack('<L', mode_data)[0]) & 0x7
            
            # headroom_vote
            address = headroom_csr + (0x100*regs) 
            hr_data = memory.read(address, 4) 
            hr_sts = (struct.unpack('<L', hr_data)[0])& 0x1FF
            print('\t VRM_Sts: %d,VRM_ENB: %d, VRM_MODE: %d,Headroom: %d ' % (vrm_sts,enb_sts, mode_sts, hr_sts ), file=vrm_file)
            print('\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~', file=vrm_file)
            print('\t\t|   DRV   | ID | Voltage | ENB | MODE | Headroom |', file=vrm_file)
            print('\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~', file=vrm_file)
            vrm_sts_table[regs]= [vrm_sts, enb_sts]
            for drv in range(0,no_drv):
                # volt_vote
                address = vt_base_address + (0x10000*drv) + (0x100*regs) 
                vrm_data = memory.read(address, 4) 
                vrm_vt = (struct.unpack('<L', vrm_data)[0]) & 0x1fff

                # enb_vote
                address = vt_en_base_address + (0x10000*drv) + (0x100*regs) 
                enb_data = memory.read(address, 4) 
                enb_vt = (struct.unpack('<L', enb_data)[0]) & 0x1

                # mode_vote
                address = mode_vote + (0x10000*drv) + (0x100*regs) 
                mode_data = memory.read(address, 4) 
                mode_vt = (struct.unpack('<L', mode_data)[0]) & 0x7

                # headroom_vote
                address = headroom_vt + (0x10000*drv) + (0x100*regs) 
                hr_data = memory.read(address, 4) 
                hr_vt = (struct.unpack('<L', hr_data)[0])& 0x1FF

                if vrm_vt != 0:
                    print("\t\t|{0:>9s}|{1:>4d}|{2:>9d}|{3:>5d}|{4:>6d}|{5:>10d}|" .format((targ_spec_config[target]['drv'][drv]), drv,  vrm_vt,enb_vt,mode_vt,hr_vt  ), file=vrm_file)
                VRM_votetable[regs][drv] = [vrm_vt,enb_vt, mode_vt,hr_vt ]
            print('\t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~', file=vrm_file)

def dump_vrm_summary(dump_path, memory, target, debug_info, fill_char):
    dump_path = os.path.join(dump_path, 'reqs_for_resources')
    if not os.path.exists(dump_path):
        os.makedirs(dump_path)
    
    summary_file_name = os.path.join(dump_path, 'rpmh_summary.txt')

    with open(summary_file_name, 'w') as summary_file:

        print("\n ~~VRM Status~~", file=summary_file)
        print("\n\t ~~VRM Sequencer Status~~", file=summary_file)
                
        for regs in range(0,no_seq):
    
            address = sequence_csr + (0x4*regs) 
            seq_data = memory.read(address, 4) 
            seq_sts = (struct.unpack('<L', seq_data)[0]) & (0x10000000)
            seq_pc = (struct.unpack('<L', seq_data)[0]) & (0x1FF)

            #print(hex(seq_sts))
            
            if seq_sts:
                print("\t\t Sequencer %d is Busy. Sequencer PC is %s" % (regs, str(hex(seq_pc))) , file=summary_file)
            else:
                print("\t\t Sequencer %d is Idle." % (regs) , file=summary_file)
                
        print("\n\t ~~VRM Status~~", file=summary_file)
    
        address = status_csr 
        vrm_data = memory.read(address, 4) 
        vrm_sts = (struct.unpack('<L', vrm_data)[0])

        #print(hex(vrm_sts))
        
        if vrm_sts != int(hex(0x10111),16):
            print("\t\t VRM is Busy" , file=summary_file)
        else:
            print("\t\t VRM is Idle" , file=summary_file)
               
def find_vrm_arc_res():
    for res in range(0,7):
        for regs in range(0,no_regs):
            if arc_vrm_ary[res] in vrm_map[regs]:
                arc_vrm_sts[res] = vrm_sts_table[regs]
