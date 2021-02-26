# --------------------------------------------------------------------------- #
# File Name ::  mbntobin.py
# Purpose   ::  Script to generate BIN file from MBN for DDR backloading on RUMI
# --------------------------------------------------------------------------- #
# $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/mbntobin.py#1 $
# --------------------------------------------------------------------------- #
# Copyright (c) 2015 Qualcomm Technologies Incorporated.
#
# All Rights Reserved. Qualcomm Confidential and Proprietary
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #
#                              IMPORT MODULES                                 #
# --------------------------------------------------------------------------- #
import sys, os
import optparse
import loadelf
import utils

#--------------------------------------------------------------------------- #
#                       LOCAL VARIABLE DEFINITIONS                            #
# --------------------------------------------------------------------------- #
version_number = '1.00'
logger = None
logfile_pre = "mbntobin_"
quietmode = False
presil_email = 'asw.mcs.presil'
logfile_name = None
logfile = None

# --------------------------------------------------------------------------- #
#                         FUNCTION DEFINITIONS                                #
# --------------------------------------------------------------------------- #
       
def main():
    # Parse command-line arguments
    usage = ("Usage: %prog <mbn> <bin.\n" +
             "Version: " + str(version_number) + "\n" +
             "  <" + presil_email + ">\n" +
             "\n"
             )
    parser = optparse.OptionParser(usage=usage)
    parser.add_option("-q", "--quiet",
                      action="store_true", dest="quiet",
                      help="disable all output")
    parser.add_option("-d", "--decorator",
                      action="store", dest="decorator",
                      help="specify a decorator for all files created by the script")
                      
    (options, args) = parser.parse_args()
    
    utils.setup_logging(options.quiet, options.decorator, logfile_pre)
    
    # Print version number
    utils.info("mbntobin.py version " + str(version_number))
    utils.info("Python version " + sys.version)
    utils.info("  <" + presil_email + ">")
    utils.info("")

    # Print out args
    utils.debug("Args: " + str(args))

    # Get the positional arguments
    try:
        mbn   = args[0]
        bin   = args[1]
    except IndexError, e:
        err  = "Must specify MBN and BIN file.  See help for more info\n\n"
        utils.error(err)

    # Debug logging
    # Log the location from where the script is run, the action and the ELF used
    utils.debug("location:" + sys.argv[0])
    utils.debug("MBN File: " + mbn)
    utils.debug("BIN File: " + bin)
    
    # Read the sections from the ELF into a minimal data structure
    elf = loadelf.load_elf(mbn, quietmode)
    
    if os.path.isfile(bin):
        utils.info("Already exists.  Overwriting...")
       
    cur_offset = 0
    binary = open(bin, "wb")
    for segment in elf['segments']:
        #Ignore NULL type segments
        if segment['type'] is 'PT_NULL':
            continue
        utils.debug("Looking at segment #{0:d}".format(segment['index']))
        utils.debug("    Start: {0:s}".format(hex(segment['addr']).rstrip('L')))
        seg_end = segment['addr'] + segment['size']
        utils.debug("      End: {0:s}".format(hex(seg_end).rstrip('L')))
        utils.debug("     Size: {0:s} bytes".format(hex(segment['size']).rstrip('L')))
        if segment['offset'] < cur_offset:
            utils.error("Current offset {0:s} is before current location {1:s}"
                .format(hex(segment['offset']), hex(cur_offset)))
        # Check whether we need some padding bytes
        if segment['offset'] > cur_offset:
            delta = segment['offset'] - cur_offset
            binary.write(bytearray([0] * delta))
            utils.debug("Wrote {0:s} bytes of padding".format(hex(delta).rstrip('L')))
            cur_offset += delta        
        binary.write(segment['data'])
        # For some RW segments, the file size is smaller than memory size
        # due to zero initialized data that happens after the MBN is loaded.
        # In this case, python only writes the actual amount of data, 
        # therefore we need to pad the delta between memory and file sizes with zeros.
        if segment['filesize'] < segment['size']:
            seg_delta = segment['size'] - segment['filesize']
            binary.write(bytearray([0] * seg_delta))
            utils.debug("Wrote {0:s} bytes of padding for difference between filesize & size"
                .format(hex(seg_delta).rstrip('L')))
        # Update the offset
        cur_offset += segment['size']
        utils.debug("Actual bytes written  : {0:s}"
            .format(hex(cur_offset).rstrip('L')))
        utils.debug("Expected bytes written: {0:s}"
            .format(hex(segment['addr']+segment['size']-elf['entry']).rstrip('L')))
    binary.close()
    utils.info("Total bytes written to BIN: {0:s} = {1:.2f}KB"
        .format(hex(cur_offset).rstrip('L'), float(cur_offset)/(1024)))
    sys.exit(0)
    
#-------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
