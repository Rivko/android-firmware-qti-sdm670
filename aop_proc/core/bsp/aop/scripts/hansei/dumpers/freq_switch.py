#  ===========================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)
ind=[]
t_stmp=[]
from operator import itemgetter, attrgetter   
import os
import copy
import itertools
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *


def dump(debug_data): #dump_path, memory, debug_info):
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    dump_path = debug_data['dump_path']

    debug_data['printer'].p("Dumping Frequency switch info...")

    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)

    rpm = 0#cast(aop_core_dump.rpmserver_state, 'SystemData', memory, debug_info)

    if (check_member('freq_switch_table_ptr ', aop_core_dump)):
        #freq_switch_table_ptr in in core dump
        freq_switch = aop_core_dump.freq_switch_table_ptr
        freq_switch = cast(freq_switch, 'ddr_stats_type', memory, debug_info)
    else:
        try:
            freq_switch_type = debug_info.variables['freq_switch_table_ptr'].die
        except:
            freq_switch_file_name = os.path.join(dump_path, 'ddr_stats.txt')
            with open(freq_switch_file_name, 'a') as freq_switch_file:
                print("\n !!! FREQ SWITCH INFO DOES NOT EXIST !!! \n", file=freq_switch_file)
            return
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            freq_switch_address = debug_info.variables['freq_switch_table_ptr'].address # FIXME: get right address by magic
            freq_switch = decode_object('freq_switch_table_ptr', freq_switch_address, None, memory, debug_info, die=freq_switch_type)
        except:
            freq_switch_file_name = os.path.join(dump_path, 'ddr_stats.txt')
            with open(freq_switch_file_name, 'a') as freq_switch_file:
                print("\n !!! FREQ SWITCH INFO DOES NOT EXIST !!! \n", file=freq_switch_file)
            return
	
	#import pdb; pdb.set_trace()
	dump_ddr(dump_path, rpm, freq_switch, memory, debug_info)



def dump_ddr(dump_path, rpm, freq_switch, memory, debug_info):
	
	smallest = (freq_switch[0].timestamp)
	smallest_indx=0
	j=0
	while j<50:
		#smallest element in the sublist
		if ( smallest > (freq_switch[j].timestamp)):
			smallest = (freq_switch[j].timestamp)
			smallest_indx=j
		j=j+1
		
	
	
	freq_switch_file_name = os.path.join(dump_path, 'ddr_stats.txt')

	with open(freq_switch_file_name, 'a') as freq_switch_file:
		print("\n\n\n######################################################################################################################################################\n ~~ Freq Switch Info ~~", file=freq_switch_file)
		print("######################################################################################################################################################\n ", file=freq_switch_file)
		
		print("\n\n ------------------------------------------------------------------------------------------------------------------------------------------", file=freq_switch_file)
		print("\n |   TimeStamp   |  Event  | powerstate_request | bcm_acv | bcm_alc | bcm_mc_request | bcm_shub_request | current_mc_cp | current_shub_cp |", file=freq_switch_file)
		print("\n ------------------------------------------------------------------------------------------------------------------------------------------", file=freq_switch_file)
		try:
			for x in range(smallest_indx-1,-1,-1):
				print("|{0:>15s} |{1:>9s}|{2:>20s}|{3:>9s}|{4:>9s}|{5:>16s}|{6:>18s}|{7:>15s}|{8:>17s}|"  .format(\
											hex(freq_switch[x].timestamp),\
											hex(freq_switch[x].event),\
											hex(freq_switch[x].powerstate_request),\
											hex(freq_switch[x].bcm_acv),\
											hex(freq_switch[x].bcm_alc),\
											hex(freq_switch[x].bcm_mc_request),\
											hex(freq_switch[x].bcm_shub_request),\
											hex(freq_switch[x].current_mc_cp),\
											hex(freq_switch[x].current_shub_cp)\
											), file=freq_switch_file)
			for x in range(49,smallest_indx-1,-1):
				print("|{0:>15s} |{1:>9s}|{2:>20s}|{3:>9s}|{4:>9s}|{5:>16s}|{6:>18s}|{7:>15s}|{8:>17s}|"  .format(\
											hex(freq_switch[x].timestamp),\
											hex(freq_switch[x].event),\
											hex(freq_switch[x].powerstate_request),\
											hex(freq_switch[x].bcm_acv),\
											hex(freq_switch[x].bcm_alc),\
											hex(freq_switch[x].bcm_mc_request),\
											hex(freq_switch[x].bcm_shub_request),\
											hex(freq_switch[x].current_mc_cp),\
											hex(freq_switch[x].current_shub_cp)\
											), file=freq_switch_file)
		except:
			print('\t\tERROR READING freq_switch_table_ptr[0]', file=freq_switch_file)


	