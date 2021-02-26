'''
   genuses
'''
#===============================================================================
#
# genuses
#
# GENERAL DESCRIPTION
#    Generates USES_FLAGS imformation from DATA file generate from build/ms. 
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/genuses.py#3 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Author: pwbldsvc $
#  $Change: 7115037 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 04/02/10   sk     Created
#
#===============================================================================
import os
import sys
import re
from optparse import OptionParser
from datetime import datetime

#===============================================================================
# parse_args
# parse command line arguments
#===============================================================================
def parse_args():
    '''
       parse_args

       parse command line arguments
    '''
    usage = "usage: %prog [options]"
    version = "%prog 1.0"
   
    parser = OptionParser(usage=usage, version=version)
    parser.add_option("-f", "--datfile", dest="dat_filename",
                      help="Read preprocess data from FILE", metavar="FILE")
    parser.add_option("-o", "--outfile", dest="output_filename",
                      help="Write output to FILE", metavar="FILE")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose", default=False,
                      help="print status messages to stdout")
          
    (options, args) = parser.parse_args()
   
    if options.dat_filename is None:
        parser.error("--datfile option must be defined")
        sys.exit(2)

    if options.output_filename is None:
        parser.error("--outfile option must be defined")
        sys.exit(2)

                  
    return (options, args)

#===============================================================================
# create_file_banner
# creates a string that can be use as a banner for auto generated files
#===============================================================================
def create_file_banner(fname, description="None", start_comment="#", 
                    end_comment="", start_block="", end_block="", style='none'):
    '''
       create_file_banner

       creates a string that can be use as a banner for auto generated files
    '''
    banner_str = \
'''$SB$SCM=================================================================''' \
'''===========$ECM
$SCM  Name:                                                                ''' \
'''     $ECM
$SCM    $FILE_NAME $ECM
$SCM
$SCM  Description:                                                         ''' \
'''     $ECM
$SCM    $DESCRIPTION $ECM
$SCM                                                                       ''' \
'''     $ECM
$SCM Copyright (c) $YEAR by QUALCOMM, Incorporated.  All Rights Reserved.  ''' \
'''      $ECM
$SCM=======================================================================''' \
'''=====$ECM
$SCM                                                                       ''' \
'''     $ECM
$SCM *** AUTO GENERATED FILE - DO NOT EDIT                                 ''' \
'''     $ECM
$SCM                                                                       ''' \
'''     $ECM
$SCM GENERATED: $DATE $ECM
$SCM=======================================================================''' \
'''=====$ECM$EB
'''   
    if style == 'C':
        start_comment = "#"
        end_comment = ""
        start_block = "/*\n"
        end_block = "\n*/"
   
    elif style == 'C++':
        start_comment = "//"
        end_comment = ""
        start_block = ""
        end_block = ""
      
    elif style == 'asm':
        start_comment = ";"
        end_comment = ""
        start_block = ""
        end_block = ""
    
    elif style == 'make' or style == 'shell':
        start_comment = "#"
        end_comment = ""
        start_block = ""
        end_block = ""
   
    elif style == 'dos':
        start_comment = "REM "
        end_comment = ""
        start_block = ""
        end_block = ""
      
    banner_str = banner_str.replace('$SCM', start_comment)
    banner_str = banner_str.replace('$ECM', end_comment)
    banner_str = banner_str.replace('$SB', start_block)
    banner_str = banner_str.replace('$EB', end_block)
    banner_str = banner_str.replace('$YEAR', str(datetime.now().strftime('%Y')))
    banner_str = banner_str.replace('$DATE', str(datetime.now().ctime()))
    banner_str = banner_str.replace('$FILE_NAME', fname)
    banner_str = banner_str.replace('$DESCRIPTION', description)

    return banner_str

def clean_line(aline):
    '''
       clean_line
    '''
    aline = aline.replace('(','{')
    aline = aline.replace(')','}')
    aline = aline.replace('\n','')
    aline = aline.replace(':=','=')
    aline = aline.replace('?=','=')
    return aline

def clean_var_name(avar_name):
    '''
       clean_var_name
    '''
    avar_name = avar_name.replace('.', '_')
    avar_name = avar_name.replace('export', '')
    avar_name = avar_name.replace('define', '')
    avar_name = re.sub('\s', '', avar_name) #get rid of whitespaces
    return avar_name

def clean_var_value(avar_value):
    '''
       clean_var_value
    '''
    avar_value = avar_value.strip()
    return avar_value
   
def write_data (options, file_handle, data, new_line="\n"):
    '''
       write_data
    '''
    file_handle.write(data + new_line)
   
    if options.verbose:
        print data
   
   
def main():
    '''
       main
    '''
    # get args from cmd line
    (options, args) = parse_args()

    uses = "USES"
    lines = open(options.dat_filename, 'r').readlines()

    banner = create_file_banner(os.path.split(options.output_filename)[1])
   
    out_file = open(options.output_filename, 'w')
   
    write_data(options, out_file, banner, new_line="")
    write_data(options, out_file, "def exists(env):")
    write_data(options, out_file, "   return env.Detect('usesflags')")
    write_data(options, out_file, "")
    write_data(options, out_file, "def generate(env):")

    var_name_dict = {}
    #count = 0
    for line in lines:
        line = line.lstrip()
        if line.find(uses, 0, 4)>-1:
            line = clean_line(line)
            tempstr = line.split("=")
            var_name = tempstr[0]
            var_name = clean_var_name(var_name)
            var_value = tempstr[1]
            var_value = clean_var_value(var_value)
         
            if var_value == "yes":
                var_uses_flag = True
            else:
                var_uses_flag = False
            
            if var_uses_flag == True:
                var_name_dict[var_name] = True
   
    # sort keys and write file
    #import pdb; pdb.set_trace()
    uses_flags = sorted(var_name_dict.iterkeys())

    for uflag in uses_flags:
        write_data(options, out_file, "   env.Replace(%s = True)" % uflag)
               
    write_data(options, out_file, \
              "   env.Replace(USES_FLAGS = %s)" % str(uses_flags))
    write_data(options, out_file, "   return None")
    out_file.close()

#run
main()
