"""===============================================================================
 scandumpwrapper.py

 GENERAL DESCRIPTION
    Scan dump data parser for a given SoC

 Copyright (c) 2016 by QUALCOMM Technologies Incorporated.
 All Rights Reserved.

 QUALCOMM Proprietary

-------------------------------------------------------------------------------

  $Header: //components/rel/core.glue/1.2/tools/scandump/scandumpwrapper.py#9 $
  $DateTime: 2018/02/14 04:45:11 $
  $Author: pwbldsvc $
  $Change: 15466219 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who         what, where, why
 --------   ---         ---------------------------------------------------------
 04/28/2017 AJCheriyan  Added support for SDM845. Signature check
 10/17/2016 AJCheriyan  Fixed issues with Unix
 12/11/2015 AJCheriyan  Created v1.0 for Nazgul
=============================================================================="""



import glob, itertools, os, string, sys, subprocess, re, platform, shutil, fnmatch, time, tempfile
from optparse import OptionParser
from ctypes import *
from helper import *
import binascii
import struct

# Function to parse input options
def parseoptions():
    global options
    argparser = OptionParser(usage = 'scandumpwrapper.py -d <binary>', version='%prog 1.0')
    argparser.add_option('-d', '--data', dest='data', help='scandump data with header info', metavar='bin', default=None)
    argparser.add_option('-o', '--outpfile', dest='ofile', help='output filename prefix', metavar='file', default='registers')
    argparser.add_option('-f', '--mapfmt', dest='mapfmt', help='Output architecture format. aarch64, aarch32, armv8 (both)', default='armv8')

    (options, args) = argparser.parse_args()

    if not options.data:
        kill('Please provide binary data file')

""" @brief : Function to retrieve the binary header data.
    @param datafile : Input scandump file to parser
    @param start_addr: Start of the header data
    @param chunksize: Chunk size for file reads (optional)
    @return Returns a one dimensional array of boolean values represented as strings
"""
def getscandump_header(datafile, start_addr = 0, chunksize = 64):
    fptr = open(datafile, 'rb+')

    if (chunksize%4 !=0):
        kill('Error: Integral number of 32 bit entries expected for chunksize. Provided: ' + str(chunksize))

    fptr.seek(start_addr, 0)
    header = fptr.read(chunksize)
    fptr.close()
    intrep = struct.unpack('<%dL' % int(chunksize/4), header)
    return [hex(intrep[i]).rstrip('L').lstrip('0x').upper().zfill(8) for i in range(0, len(intrep))]

""" @brief : Function to parse the header and determine chipset
    @param header: Extracted header from the binary data
    @return Returns a dictionary equivalent of the header structure with parsed information appended
"""
def parse_header(header):
    header_attrib = {'chipid' : header[0], 'jtagid' : header[1], 'ipinfo' : header[2], 'instances' : header[3], 'signature' : header[4:8] }

    if (header_attrib['chipid'] == '30020100'):
        header_attrib['chip'] = 'msm8998_v1'
    elif (header_attrib['chipid'] in ['30020200', '30020201'] ):
        header_attrib['chip'] = 'msm8998_v2'
    elif (header_attrib['chipid'] == '30050100'):
        header_attrib['chip'] = 'msm8997_v1'
    elif (header_attrib['chipid'] in ['60000100']):
        header_attrib['chip'] = 'sdm845_v1'
        if header_attrib['ipinfo'] in ['00000000']:
           header_attrib['cputype'] = 'kryo280s'
        elif header_attrib['ipinfo'] in ['00000002']:
           header_attrib['cputype'] = 'kryo280g'
        else:
           kill('Error: Could not identify CPU information %s' % (header_attrib['ipinfo']))
    elif (header_attrib['chipid'] in ['60000200', '60000201']):
        header_attrib['chip'] = 'sdm845_v2'
        if header_attrib['ipinfo'] in ['00000000']:
           header_attrib['cputype'] = 'kryo280s'
        elif header_attrib['ipinfo'] in ['00000002']:
           header_attrib['cputype'] = 'kryo280g'
        else:
           kill('Error: Could not identify CPU information %s' % (header_attrib['ipinfo']))
    elif (header_attrib['chipid'] in ['60040100' , '60040101']):
        header_attrib['chip'] = 'sdm670'
        if header_attrib['ipinfo'] in ['00000000']:
           header_attrib['cputype'] = 'kryo280s'
        elif header_attrib['ipinfo'] in ['00000002']:
           header_attrib['cputype'] = 'kryo280g'
        else:
           kill('Error: Could not identify CPU information %s' % (header_attrib['ipinfo']))
    elif (header_attrib['chipid'] in ['60050100']):
        header_attrib['chip'] = 'qcs605'
        if header_attrib['ipinfo'] in ['00000000']:
           header_attrib['cputype'] = 'kryo280s'
        elif header_attrib['ipinfo'] in ['00000002']:
           header_attrib['cputype'] = 'kryo280g'
        else:
           kill('Error: Could not identify CPU information %s' % (header_attrib['ipinfo']))       
    else:
        kill('Error: Coud not find chipset information for chipid ' + header_attrib['chipid'])
    return header_attrib

""" @brief : Function to parse the signature and verify if the data is consistent
    @param chip, signature: Chipset information and signature data
    @return Returns a number to the signature offset within the header
"""
def parse_signature(chipset, signature):
    if chipset in ['sdm845_v1', 'sdm845_v2','sdm670','qcs605']:
        signature1 = ''.join(signature[0:])
        signature2 = ''.join(signature[0:3])
        offset = 0
        if signature1 in ['0000000000CCAAAA00F0CCCA00C0F0EA']:
            offset = 4
        elif signature2 in ['00CCAAAA00F0CCCA00C0F0EA']:
            offset = 0
        else:
            kill('Error: Could not detect signature flops in the binary data. Signature: %s %s' % (signature1, signature2))
        debug('Detected signature at offset : %s ' % (offset), True)

    return offset




""" @brief : Function to parse the header and determine chipset
    @param header: Extracted header from the binary data
    @return Returns a dictionary equivalent of the header structure with parsed information appended
"""

def exec_scandumpparser(parsed_header):
    #debug('Header Information : ' + str(parsed_header))
    # MSM8998 specific parsing
    if parsed_header['chip'] in ['msm8998_v1', 'msm8998_v2']:
        debug('Detected MSM8998: ' + parsed_header['chip'], True)
        if (parsed_header['ipinfo'] in ['00000002', '00000003']):
            for instance in range(0, int(parsed_header['instances'])):
                registerxml = ('_').join(['register', parsed_header['chip']]) + '.xml'
                registerout = ('_').join([options.ofile, 'core%s.cmm' % (instance)])
                logfile = ('_').join([options.ofile, 'core%s.log' % (instance)])
                logfptr = open(logfile, 'w')
                curr_dir = os.path.abspath(__file__).rstrip(os.path.basename(__file__))
                script = '/'.join([curr_dir, 'scandumpparser.py'])
                registerxml = '/'.join([curr_dir, registerxml])
                signalsfile = 'signals'
                cmdline = 'python %s -x %s -m %s -d %s -p %s -o %s -f %s' % (script, registerxml, signalsfile, options.data, instance, registerout, options.mapfmt)
                scandump_proc = subprocess.Popen(cmdline.split(" "), stdout=logfptr, stderr=subprocess.PIPE)
                (output, error) = scandump_proc.communicate()
                logfptr.close()
                if error:
                    kill('Encountered error in call ' + cmdline + ' Error: ' + error)
                else:
                    debug('Scandump output generated for Core %s.' % (instance))


    # SDM845 specific parsing
    if parsed_header['chip'] in ['sdm845_v1', 'sdm845_v2']:
        debug('Detected SDM845: %s, CPU Type: %s' %( parsed_header['chip'], parsed_header['cputype']), True)
        registerxml = ''
        signalsfile = ''
        """ the default offset is the size of the header information """
        default_offset = 16
        signature_offset = parse_signature(parsed_header['chip'], parsed_header['signature'])
        skip_header = default_offset + signature_offset
        if (parsed_header['cputype'] in ['kryo280s']):
            registerxml = os.sep.join(['target', parsed_header['chip']]) + os.sep + ('_').join(['register', 'silver', 'sdm845']) + '.xml'
            signalsfile = '.'.join(['target',parsed_header['chip'],'signals_silver'])
        elif (parsed_header['cputype'] in ['kryo280g']):
            registerxml = os.sep.join(['target', parsed_header['chip']]) + os.sep + ('_').join(['register', 'gold', 'sdm845']) + '.xml'
            signalsfile = '.'.join(['target',parsed_header['chip'],'signals_gold'])
        for instance in range(0, int(parsed_header['instances'])):
                registerout = ('').join([options.ofile, '.cmm'])
                logfile = ('').join([options.ofile, '.log'])
                logfptr = open(logfile, 'w')
                curr_dir = os.path.abspath(__file__).rstrip(os.path.basename(__file__))
                script = os.sep.join([curr_dir, 'scandumpparser.py'])
                registerxml = os.sep.join([curr_dir, registerxml])
                cmdline = 'python %s -x %s -m %s -d %s -p %s -k %s -o %s -f %s' % (script, registerxml, signalsfile, options.data, instance, skip_header, registerout, options.mapfmt)
                debug('Commandline : %s ' % cmdline)
                scandump_proc = subprocess.Popen(cmdline.split(" "), stdout=logfptr, stderr=subprocess.PIPE)
                (output, error) = scandump_proc.communicate()
                logfptr.close()
                if error:
                    kill('Encountered error in call ' + cmdline + ' Error: ' + error)
                else:
                    debug('Scandump output generated.', True)


	# SDM670 and QCS605 specific parsing
    if parsed_header['chip'] in ['sdm670' , 'qcs605']:
        debug('Detected SDM670: %s, CPU Type: %s' %( parsed_header['chip'], parsed_header['cputype']), True)
        registerxml = ''
        signalsfile = ''
        """ the default offset is the size of the header information """
        default_offset = 16
        signature_offset = parse_signature(parsed_header['chip'], parsed_header['signature'])
        skip_header = default_offset + signature_offset
        if (parsed_header['cputype'] in ['kryo280s']):
            registerxml = os.sep.join(['target', parsed_header['chip']]) + os.sep + ('_').join(['register', 'silver', parsed_header['chip']]) + '.xml'
            signalsfile = '.'.join(['target',parsed_header['chip'],'signals_silver'])
        elif (parsed_header['cputype'] in ['kryo280g']):
            registerxml = os.sep.join(['target', parsed_header['chip']]) + os.sep + ('_').join(['register', 'gold',parsed_header['chip']]) + '.xml'
            signalsfile = '.'.join(['target',parsed_header['chip'],'signals_gold'])
        for instance in range(0, int(parsed_header['instances'])):
                registerout = ('').join([options.ofile, '.cmm'])
                logfile = ('').join([options.ofile, '.log'])
                logfptr = open(logfile, 'w')
                curr_dir = os.path.abspath(__file__).rstrip(os.path.basename(__file__))
                script = os.sep.join([curr_dir, 'scandumpparser.py'])
                registerxml = os.sep.join([curr_dir, registerxml])
                cmdline = 'python %s -x %s -m %s -d %s -p %s -k %s -o %s -f %s' % (script, registerxml, signalsfile, options.data, instance, skip_header, registerout, options.mapfmt)
                debug('Commandline : %s ' % cmdline)
                scandump_proc = subprocess.Popen(cmdline.split(" "), stdout=logfptr, stderr=subprocess.PIPE)
                (output, error) = scandump_proc.communicate()
                logfptr.close()
                if error:
                    kill('Encountered error in call ' + cmdline + ' Error: ' + error)
                else:
                    debug('Scandump output generated.', True)

if __name__ == '__main__':

    # Parse all the options passed in
    parseoptions()

    # Extract the header
    header = getscandump_header(options.data, chunksize = 64)
    parsed_header = parse_header(header)

    # Parse the scandumps
    exec_scandumpparser(parsed_header)







