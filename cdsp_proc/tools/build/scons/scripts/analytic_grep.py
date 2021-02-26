'''
   Analytic Grep

   GENERAL DESCRIPTION
      Helper script to analyse information in *.json analytic files.
'''
#============================================================================
#
# Analytic Grep
#
# GENERAL DESCRIPTION
#    Helper script to analyse information in *.json analytic files.
#
# Copyright (c) 2009-2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/analytic_grep.py#4 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Author: pwbldsvc $
#  $Change: 7115037 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# YYYY-MM-DD   who     what, where, why
# ----------   ---     ---------------------------------------------------------
#
#
#
# Usage:
# 	analytic-grep.py requires 2 paramters:
#		--json-file:		the *.json file to grep (provide full path)
#		--search-string:	what to grep for in the *.json file
#============================================================================
import json
import sys

DEFINES = {}
try:
    import argparse
except ImportError:
    print "You need to upgrade to python to 2.7.6 before you can continue."
    sys.exit(1)

PARSER = argparse.ArgumentParser()
PARSER.add_argument('--json-file', \
    help='path to json file with analytic data.' \
    'Please be aware that multiple *.json files can be generated per image.')
PARSER.add_argument('--search-string', help='string to search for')
PARSER.add_argument('--list-defines', \
    help='list DEFINE macros', action='store_true')

ARGS = PARSER.parse_args()

if ARGS.search_string is None and ARGS.list_defines is False:
    print "Please supply a value for either --search-string or --list-defines" \
        " - it is mandatory to suplly one of these parameters"
    PARSER.print_help()
    sys.exit(1)

if ARGS.json_file is None:
    print "Please supply a value for --json-file - it is a mandatory parameter"
    PARSER.print_help()
    sys.exit(1)

DATA = ""
with open(ARGS.json_file) as infile:
    DATA = json.load(infile)

def build_defines_list(command):
    '''
       build_defines_list

       Build list of define macros
    '''
    end = 0
    while True:
        begin = command.find("-D", end) + 2
        end = command.find(" ", begin)
        if begin > 1:
            define = command[begin:end]
            if not DEFINES.has_key(define):
                DEFINES[define] = ""
        else:
            return

for item in DATA["Commands"]:
    if ARGS.search_string is None and ARGS.list_defines:
        build_defines_list(item["cmd"])
    else:
        printIt = False
        if item["source"].find(ARGS.search_string) >= 0:
            printIt = True
        elif item["target"].find(ARGS.search_string) >= 0:
            printIt = True
        elif item["scons_cmd"].find(ARGS.search_string) >= 0:
            printIt = True
        elif item["cmd"].find(ARGS.search_string) >= 0:
            printIt = True
        if printIt:
            build_defines_list(item["cmd"])
            print "\tsource: " + item["source"]
            print "\ttarget: " + item["target"]
            print "\tscons_cmd: " + item["scons_cmd"]
            print "\tcmd: " + item["cmd"]
            print "\t=======================================================" \
                "======\n"

if ARGS.list_defines:
    for key in DEFINES.keys():
        print key
