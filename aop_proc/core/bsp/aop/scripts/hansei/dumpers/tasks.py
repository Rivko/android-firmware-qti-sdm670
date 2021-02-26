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

task_list = ['dog_run_tcb','ddr_fsm_tcb','vrm_task_tcb','main_idle_tcb','aop_services_tcb']

def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error

    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target'] 
    
    debug_data['printer'].p("Dumping Task Details...")

    try:
        
        task_file_name = os.path.join(dump_path, 'task_info.txt')

        with open(task_file_name, 'w') as task_file:
        
            print('{0: <20}'.format("Thread Name")+ '{0: <10}'.format("Priority")+'{0: <10}'.format("Wait")+'{0: <10}'.format("Sigs")+'{0: <5}'.format("Suspended"), file=task_file)
            print("-------------------------------------------------------------------" , file=task_file)
            
            for task_name in task_list:
                tcb_address = debug_info.variables[task_name].address
                tcb_type = debug_info.variables[task_name].die
                tcb = decode_object(task_name, tcb_address, None, memory, debug_info, die=tcb_type)

                print('{0: <20}'.format(task_name)+ '{0: <10}'.format(str(tcb.pri))+'{0: <10}'.format(str(hex(tcb.wait)))+'{0: <10}'.format(str(hex(tcb.sigs)))+'{0: <5}'.format(str(bool(tcb.suspended))), file=task_file)
    except:
        dump_error = 1
        
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

