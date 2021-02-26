#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import os
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object
from hansei_utils import *
import re

from sys import stdout

chips = {
    # family '6' -> Hoya family
    6 : {
        4 : {
            'MSM_ID' : 'SDM670',
            'num'    : '670',
            'name'   : 'Warlock',
            },      
    },
}
chip_version = 0;
def find_version_info(str_name, mem_start, mem_end, memory):
    output = ""
    #import pdb; pdb.set_trace()
    for addr in range(mem_start, mem_end, 1):
        if (memory.read(addr, len(str_name)) == str_name):
            offset = len(str_name)
            while (memory.read(addr+offset, 1) != "\0" and (addr+offset) < mem_end):
                output+=memory.read(addr+offset, 1)
                offset+=1
    if (output):
        return output
    else:
        return "NOT FOUND"

def dump(debug_data): #dump_path, memory, debug_info, base_address):
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    dump_path = debug_data['dump_path']
    base_address = debug_data['image_strings_addr']
    global chip_version 
    dump = open(os.path.join(dump_path, 'aop-summary.txt'), 'w')

    try:
        chipFamilyNum_die = debug_info.variables['chipFamilyNum'].die
        chipDeviceNum_die = debug_info.variables['chipDeviceNum'].die
        chipMinor_die = debug_info.variables['chipMinor'].die
        chipMajor_die = debug_info.variables['chipMajor'].die		
    except:
        print("\n !!! ChipInfoCtxt DOES NOT EXIST !!! \n", file=ddr_file)
        return
    try:
        #not in core dump, but elf is close enough to provide a correct pointer
        chipFamilyNum_address = debug_info.variables['chipFamilyNum'].address # FIXME: get right address by magic
        chipFamilyNum = decode_object('chipFamilyNum', chipFamilyNum_address, None, memory, debug_info, die=chipFamilyNum_die)

        chipDeviceNum_address = debug_info.variables['chipDeviceNum'].address # FIXME: get right address by magic
        chipDeviceNum = decode_object('chipDeviceNum', chipDeviceNum_address, None, memory, debug_info, die=chipDeviceNum_die)

        chipMinor_address = debug_info.variables['chipMinor'].address # FIXME: get right address by magic
        chipMinor = decode_object('chipMinor', chipMinor_address, None, memory, debug_info, die=chipMinor_die)

        chipMajor_address = debug_info.variables['chipMajor'].address # FIXME: get right address by magic
        chipMajor = decode_object('chipMajor', chipMajor_address, None, memory, debug_info, die=chipMajor_die)
    except:
        print("\n !!! ChipInfoCtxt DOES NOT EXIST !!! \n", file=ddr_file)
        return
    chip_version = chipMajor
	
    version_address_start = base_address + 0x0
    version_address_end   = version_address_start + 0x1000
    
    #regex to match the RPM branch name:
    regex = '^AOP\.HO\.\d\.\d.*(?=-)'
    


    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)

    
    chip_name = '<unknown device -> family %d device %d>' % (chipFamilyNum, chipDeviceNum)
    target_chip = chips.get(chipFamilyNum, {}).get(chipDeviceNum, {})
    chip_name = target_chip.get('MSM_ID', chip_name)
    print('\nCollected from %s %d.%d\n' % (chip_name, chipMajor, chipMinor), file=dump)
    
    rpm_ver_type = debug_info.variables['aop_core_dump'].vartype
    
    #rpm_build_date_type = debug_info.variables['gBuildDate'].die
    #rpm_build_date_address = debug_info.variables['gBuildDate'].address
    #rpm_build_time_type = debug_info.variables['gBuildTime'].die
    #rpm_build_time_address = debug_info.variables['gBuildTime'].address
    #rpm_build_date = decode_object('gBuildDate', rpm_build_date_address, None, memory, debug_info, die=rpm_build_date_type)
    #rpm_build_time = decode_object('gBuildTime', rpm_build_time_address, None, memory, debug_info, die=rpm_build_time_type)
    #
    #QC_IMAGE_VERSION = find_version_info("QC_IMAGE_VERSION_STRING=",version_address_start, version_address_end, memory)
    #print("QC_IMAGE_VERSION_STRING:  %s" % QC_IMAGE_VERSION, file=dump)
    #print("IMAGE_VARIANT_STRING:     %s" % find_version_info("IMAGE_VARIANT_STRING=",version_address_start, version_address_end, memory), file=dump)
    #print("OEM_IMAGE_VERSION_STRING: %s" % find_version_info("OEM_IMAGE_VERSION_STRING=",version_address_start, version_address_end, memory), file=dump)
    #
    #print("\nBuild Date: %s" % pointer_to_str(rpm_build_date), file=dump)
    #print("Build Time: %s" % pointer_to_str(rpm_build_time), file=dump)
    
    if aop_core_dump.dumped_at == 0:
        print('\nThe AOP is ok.', file=dump)
        # return chip name and RPM branch
        return [target_chip.get('num', '????')]#, re.compile(regex).match(QC_IMAGE_VERSION).group()]
    
    print('\nThe AOP crashed (@ time = 0x%x)' % aop_core_dump.dumped_at, file=dump)
    xpsr = aop_core_dump.registers.xPSR
    
    ipsr = xpsr & 0xff
    if ipsr == 0:
        print('\tNot in an exception context; vanilla fatal error scenario.', file=dump)
    elif ipsr < 16:
        fault_types = {
            2  : 'NMI',
            3  : 'Hard Fault',
            4  : 'Memory Management Fault',
            5  : 'Bus Fault',
            6  : 'Usage Fault',
            11 : 'SVCall',
            12 : 'Debug Monitor',
            14 : 'PendSV',
            15 : 'SysTick',
        }
        print('\tIn a fault context -> %s' % fault_types[ipsr], file=dump)
    else:
        print('\tFatal error inside ISR #%d' % (ipsr - 16), file=dump)
    
    
    def decode_bitfield(name, bit_definitions, data, joiner = ' | '):
        known_bits = 0
        for id in bit_definitions:
            known_bits |= (1 << id)
        unknown_data = data - (data & known_bits)
        string = joiner.join(['[' + bit_definitions[x] + ']' for x in bit_definitions if (1 << x) & data])
        if unknown_data:
            if string:
                string += ' ...and '
            multi_or_single = ''
            if log(unknown_data, 2) != int(log(unknown_data, 2)):
                multi_or_single = 's'
            string += 'unknown %s%s 0x%0.8x' % (name, multi_or_single, unknown_data)
        return string
    
    
    fault_regs = aop_core_dump.fault_detail
    
    shcas_bits = {
        0  : 'Memory fault in progress',
        1  : 'Bus fault in progress',
        3  : 'Usage fault in progress',
        7  : 'SVCall in progress',
        8  : 'Debug Monitor in progress',
        10 : 'PendSV in progress',
        11 : 'SysTick in progress',
        12 : 'Usage fault pended',
        13 : 'Memory management fault pended',
        14 : 'Bus fault pended',
        15 : 'SVCall pended',
        16 : 'Memory management fault enabled (this is not a problem)',
        17 : 'Bus fault enabled (this is not a problem)',
        18 : 'Usage fault enabled (this is not a problem)',
    }
    
    print('\nSystem handler status {\n\t%s\n}' % decode_bitfield('bits', shcas_bits, fault_regs.SystemHandlerCtrlAndState, '\n\t'), file=dump)
    
    cfs_bits = {
        0  : 'Attempt to fetch an instruction from a non-executable region.',
        1  : 'Attempt to load or store a location that does not permit it.',
        3  : 'Unstacking from an exception return has caused one or more access violations.',
        4  : 'Stacking for an exception has caused one or more access violations.',
        7  : 'Memory manage address register (MMAR) is valid.',
        8  : 'Instruction bus error.',
        9  : 'Precise data bus error.',
        10 : 'Imprecise data bus error.',
        11 : 'Unstack from exception has caused one or more bus errors.',
        12 : 'Stacking for exception has caused one or more bus errors.',
        15 : 'Bus fault address register (BFAR) is valid.',
        16 : 'Undefined instruction exception.',
        17 : 'Invalid state exception.',
        18 : 'Illegal attempt to load EXC_RETURN into PC.',
        19 : 'Attempt to use a coprocessor instruction.',
        24 : 'Unaligned memory access.',
        25 : 'Divide by zero.',
    }
    
    print('\nConfigurable fault status {\n\t%s\n}' % decode_bitfield('bits', cfs_bits, fault_regs.ConfigurableFaultStatus, '\n\t'), file=dump)
    
    hfs_bits = {
        0  : 'Fault on vector table read during exception processing.',
        30 : 'Hard Fault activated because a configurable fault was received and cannot activate because of priority or it is disabled.',
        31 : 'Fault related to debug.',
    }
    
    print('\nHard fault status {\n\t%s\n}' % decode_bitfield('bits', hfs_bits, fault_regs.HardFaultStatus, '\n\t'), file=dump)
        
    dfs_bits = {
        0  : 'Halt requested by NVIC.',
        1  : 'BKPT instruction execution.',
        2  : 'DWT match.',
        3  : 'Vector fetch occurred.',
        4  : 'EDBGRQ signal asserted.',
    }
    
    print('\nDebug fault status {\n\t%s\n}' % decode_bitfield('bits', hfs_bits, fault_regs.DebugFaultStatus, '\n\t'), file=dump)
    
    if 0 != (fault_regs.ConfigurableFaultStatus & (1 << 7)):
        print('\nMemory manage fault address: 0x%0.8x' % fault_regs.MemManageAddress, file=dump)
    
    if 0 != (fault_regs.ConfigurableFaultStatus & (1 << 15)):
        print('\nMemory manage fault address: 0x%0.8x' % fault_regs.BusFaultAddress, file=dump)
    
    print('\nAuxilary fault address register: 0x%0.8x' % fault_regs.AuxFaultStatus, file=dump)
    dump.close()
    
    # return chip name and RPM branch
    return [target_chip.get('num', '????')]#, re.compile(regex).match(QC_IMAGE_VERSION).group()]
    
