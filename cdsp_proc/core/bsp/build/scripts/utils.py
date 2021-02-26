# --------------------------------------------------------------------------- #
# File Name ::  utils.py
# Purpose   ::  Helper functions for crashdump and bin file generation
# --------------------------------------------------------------------------- #
# $Header: //components/rel/core.qdsp6/2.1.c4/bsp/build/scripts/utils.py#1 $
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
import logging
import shutil
import datetime

#--------------------------------------------------------------------------- #
#                       LOCAL VARIABLE DEFINITIONS                            #
# --------------------------------------------------------------------------- #
version_number = '1.00'
quietmode = False
logfile_ext = ".log"
logfile = None
logfile_path = None
logfile_name = None

# --------------------------------------------------------------------------- #
#                         FUNCTION DEFINITIONS                                #
# --------------------------------------------------------------------------- #
def error(err):
    global quietmode
    if not quietmode:
        logging.error(err)
        print "\nERROR: " + err
    sys.exit(1)
    
def info(s):
    global quietmode
    if not quietmode:
        logging.info(s)
        print s

def debug(s):
    global quietmode
    if not quietmode:
        logging.debug(s)
        
def setup_logging(quiet, decorator, logfile_pre):
    # Set quiet mode if applicable
    global quietmode
    if quiet:
        quietmode = quiet
    
    # Setup logging, add timestamp to the log file name in the format YYYYMMDD_HHMMSS
    if not quietmode:
        global logfile
        global logfile_name
        global logfile_path
        logfile_name = logfile_pre
        if decorator:
            logfile_name +=  "." + decorator
        logfile_name += str(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d_%H%M%S")) + logfile_ext
        logfile_path = os.getcwd()
        
        logfile = os.path.join(logfile_path, logfile_name)
        logging.basicConfig(filename=logfile, filemode='w',
                            format='%(levelname)s:%(message)s', level=logging.DEBUG)
        # Make dure the log file exists
        info("Logfile: " + logfile)
        if os.path.isfile(logfile) == False:
            error("Could not create log file" + logfile)
            sys.exit(0xFF) 

def shutdown_logging(dumppath):
    # Force python to use the global vars logfile & logfile_path instead of creating new limited scope ones that are initialized to None.
    global logfile
    global logfile_path
    global logfile_name
    if dumppath:
        info("Copying " + logfile_name + " from " + logfile_path + " to " + dumppath)
    # Close the log file so that we can move it to the dump folder
    logging.shutdown()

    if dumppath:
        #import pdb; pdb.set_trace();
        shutil.move(logfile, str(os.path.join(dumppath, logfile_name)))            
