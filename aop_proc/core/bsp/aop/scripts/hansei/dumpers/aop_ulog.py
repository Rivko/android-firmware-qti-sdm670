#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
import subprocess
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object
from hansei_utils import print_p

#
# This algorithm is ripped directly from rpm_ulogdump.cmm.  It is incredibly
# non-pythonic, but it closely matches the CMM algorithm.
#
def dump(debug_data): #dump_path, parse_path, memory, debug_info, target, branch):
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    dump_path = debug_data['dump_path']
    parse_path = debug_data['args'].parser
    target = debug_data['target']
    branch = debug_data['branch']
    parse_tbl = debug_data['parser_tbl']
    debug_data['printer'].p("Dumping ulogs...")
    
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)

    rpm_ulogContext_type = debug_info.variables['rpm_ulogContext'].vartype
    ulog_state = aop_core_dump.ulog_state.address
    rpm_ulogContext = decode_object('rpm_ulogContext', ulog_state, rpm_ulogContext_type, memory, debug_info)

    log = rpm_ulogContext.logHead
    if log.address == 0:
        logger.error('Failed to find any RPM ulogs (rpm_ulogContext.logHead == NULL)!')
        return

    if log.version != 0x1000:
        logger.error('This script only knows how to dump RPM ULog version 0x1000 (Originating July 2012)')
        if log.version in [2, 3, 4]:
            logger.error('It appears your logs are version %d' % log.version)
        else:
            logger.error('Your log version (%d) is unknown to this script.  Is your log corrupted?' % log.version)
        return

    while log.address != 0:
        log_name = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in log.name)))

        log_enabled = (log.logStatus & 0x2) != 0
        if not log_enabled:
            logger.debug('Not dumping ulog (disabled): %d' % log_name)
            continue

        use_64_bit_timestamps = (log.feature_flags1 & 0x1) != 0

        logger.debug('Dumping ulog: %s' % log_name)
        log_file_name = os.path.join(dump_path, '%s.ulog' % log_name)
        with open(log_file_name, 'w') as log_file:
            log_buffer   = log.buffer
            log_size     = log.bufSize
            log_mask     = log.bufSizeMask
            read         = log.readWriter
            readers_read = log.read
            write        = log.write
            bytes_in_log = write - read

            if bytes_in_log > log_size:
                logger.warning('While parsing ulog "%s" -> reported log size %d bigger than the expected log size %d' % (log_name, bytes_in_log, log_size)) 
                logger.warning('The most common cause of this is memory corruption, under-voltage, or writes to unpowered RAM.')
                logger.warning('Will try to continue with this ULog decode, but results may be unpredictable.')

            while read < write:
                # Advance past the message format value
                read = read + 2
                
                # Read the number of bytes in this message
                msg_length = log_buffer[read & log_mask] + (log_buffer[(read + 1) & log_mask] << 8)
                
                # Back up to the format for later
                read = read - 2
                      
                # Handle the current ULog message
                dump_ulog_msg(log_file, log_buffer, log_mask, read, msg_length)
                
                # Move our read pointer past the message we just finished (and 4 byte align it)
                read = read + ((msg_length + 3) & 0xFFFFFFFC)

        log = log.next

    # Now run the second-stage parsing script, if applicable.
    rpm_external_log = os.path.join(dump_path, 'AOP External Log.ulog')
    if os.path.exists(rpm_external_log):
        print_p('\t\tPost-processing the AOP "external" log...')
        parser = parse_path+'aop_log_hfam.py'
        parser2 = parse_tbl+'tracer_event_tbl.h'
        parser_params = ['python', parser, '-f', rpm_external_log, '-t', target, '-tbl', parser2 ]#, '-b', branch]

        try:
            pretty_output = subprocess.check_output(parser_params)
            pretty_output = pretty_output.replace('\r\n', '\n')
        except subprocess.CalledProcessError:
            pretty_output = '<failed to run log parser>\n'
        with open(os.path.join(dump_path, 'aop-rawts.txt'), 'w') as pretty_log:
            pretty_log.write(pretty_output)

        try:
            parser_params = parser_params + ['-p']
            raw_output = subprocess.check_output(parser_params)
            raw_output = raw_output.replace('\r\n', '\n')
        except subprocess.CalledProcessError:
            raw_output = '<failed to run log parser>\n'
        with open(os.path.join(dump_path, 'aop-log.txt'), 'w') as raw_log:
            raw_log.write(raw_output)

    #import pdb; pdb.set_trace()
    # Now run the second-stage parsing script, if applicable.
    ddr_external_log = os.path.join(dump_path, 'AOP DDR Log.ulog')
    if os.path.exists(rpm_external_log):
        print_p('\t\tPost-processing the AOP "DDR" log...')
        parser = parse_path+'aop_log_hfam.py'
        parser2 = parse_tbl+'tracer_event_tbl.h'
        parser_params = ['python', parser, '-f', ddr_external_log, '-t', target, '-tbl', parser2 ]#, '-b', branch]

        try:
            pretty_output = subprocess.check_output(parser_params)
            pretty_output = pretty_output.replace('\r\n', '\n')
        except subprocess.CalledProcessError:
            pretty_output = '<failed to run log parser>\n'
        with open(os.path.join(dump_path, 'ddr-rawts.txt'), 'w') as pretty_log:
            pretty_log.write(pretty_output)

        try:
            parser_params = parser_params + ['-p']
            raw_output = subprocess.check_output(parser_params)
            raw_output = raw_output.replace('\r\n', '\n')
        except subprocess.CalledProcessError:
            raw_output = '<failed to run log parser>\n'
        with open(os.path.join(dump_path, 'ddr-log.txt'), 'w') as raw_log:
            raw_log.write(raw_output)

def dump_ulog_msg(log_file, log_buffer, log_mask, read, msg_length):
    # Read the message format
    fmt = log_buffer[read & log_mask] + (log_buffer[(read + 1) & log_mask] << 8)

    # Advance the read pointer
    read += 4

    if fmt != 0:
        logger.error('These scripts currently only support dumping "raw" ULog types; skipping a message.')
        return

    # Subtract off the length/format word size
    msg_length -= 4

    output_string = '- '
    bytes_out     = 0
    while msg_length != 0:
        out_char = '0x%0.2x' % log_buffer[read & log_mask]
        output_string += out_char + ', '
        bytes_out += 1
        read += 1
        msg_length -= 1

    print(output_string, file=log_file)


