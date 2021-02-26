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
import binascii
import pdb


def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error

    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target'] 
    
    debug_data['printer'].p("Dumping AOP Logs from MSGRAM...")

    try: 
        aop_log_file_name = os.path.join(dump_path, 'aop_log_msgram6.txt')

        with open(aop_log_file_name, 'w') as aop_log_file:
        
            print('{0: <20}'.format("Timestamp")+ '{0: <20}'.format("Message")+'{0: <20}'.format("Data"), file=aop_log_file)
            print("-------------------------------------------------------------------" , file=aop_log_file)
      
            log_address = debug_info.variables['aop_log_data'].address
            #print(hex(log_address))
            log_type = debug_info.variables['aop_log_data'].die
            log = decode_object('aop_log_data', log_address, None, memory, debug_info, die=log_type)
                
            for log_num in range(0,60):
                 print('{0: <20}'.format(hex(log[log_num].timestamp_lo))+ '{0: <20}'.format(conhexToAscii(hex(log[log_num].message[0])) + conhexToAscii(hex(log[log_num].message[1])))+ "  " + '{0: <20}'.format(hex(log[log_num].data)), file=aop_log_file)
        
    except:
        dump_error = 1
        
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

def conhexToAscii(message):

    message2 = message[2:]
    messageStr = binascii.unhexlify(message2)
    return messageStr[::-1]
    
