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

import os
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

    debug_data['printer'].p("Dumping sleep stats...")

    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)

    rpm = 0#cast(aop_core_dump.rpmserver_state, 'SystemData', memory, debug_info)

    if (check_member('sleep_stats', aop_core_dump)):
        #sleep_stats in in core dump
        sleep = aop_core_dump.sleep_stats
        sleep = cast(sleep, 'sleep_stats_type', memory, debug_info)
    else:
        try:
            sleep_type = debug_info.variables['sleep_stats'].die
        except:
            sleep_file_name = os.path.join(dump_path, 'sleep_stats.txt')
            with open(sleep_file_name, 'w') as sleep_file:
                print("\n !!! SLEEP_STATS DOES NOT EXIST, SLEEP IS LIKELY NOT ENABLED !!! \n", file=sleep_file)
            return
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            sleep_address = debug_info.variables['sleep_stats'].address # FIXME: get right address by magic
            sleep = decode_object('sleep_stats', sleep_address, None, memory, debug_info, die=sleep_type)
        except:
            sleep_file_name = os.path.join(dump_path, 'sleep_stats.txt')
            with open(sleep_file_name, 'w') as sleep_file:
                print("\n !!! SLEEP_STATS DOES NOT EXIST, SLEEP IS LIKELY NOT ENABLED !!! \n", file=sleep_file)
            return
    dump_sleep(dump_path, rpm, sleep, memory, debug_info)

def dump_sleep(dump_path, rpm, sleep, memory, debug_info):
    sleep_file_name = os.path.join(dump_path, 'sleep_stats.txt')
    with open(sleep_file_name, 'w') as sleep_file:

        print("\n ~~Sleep Stats Dump~~", file=sleep_file)
        print('\n\tTYPE:            %s' % convert_hex_to_ascii(sleep[0].stat_type), file=sleep_file)
        try:
            print('\t\tsleep_count:            %d' % sleep[0].count, file=sleep_file)
            print('\t\tlast_entered_at:        %s' % hex(sleep[0].last_entered), file=sleep_file)
            print('\t\tlast_exited_at:         %s' % hex(sleep[0].last_exited), file=sleep_file)
            print('\t\taccumulated_duration:   %s' % hex(sleep[0].total_duration), file=sleep_file)
            print('', file=sleep_file)
        except:
            print('\t\tERROR READING sleep_stats[0]', file=sleep_file)

        print('\n\tTYPE:            %s' % convert_hex_to_ascii(sleep[1].stat_type), file=sleep_file)
        try:
            print('\t\tsleep_count:            %d' % sleep[1].count, file=sleep_file)
            print('\t\tlast_entered_at:        %s' % hex(sleep[1].last_entered), file=sleep_file)
            print('\t\tlast_exited_at:         %s' % hex(sleep[1].last_exited), file=sleep_file)
            print('\t\taccumulated_duration:   %s' % hex(sleep[1].total_duration), file=sleep_file)
        except:
            print('\t\tERROR READING sleep_stats[1]', file=sleep_file)

def convert_hex_to_ascii(h):
    chars_in_reverse = []
    while h != 0x0:
        chars_in_reverse.append(chr(h & 0xFF))
        h = h >> 8

    #chars_in_reverse.reverse()
    return ''.join(chars_in_reverse)

