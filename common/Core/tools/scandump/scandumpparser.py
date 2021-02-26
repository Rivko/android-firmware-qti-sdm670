"""===============================================================================
 scandumpparser.py

 GENERAL DESCRIPTION
    Scan dump data parser for a given SoC

 Copyright (c) 2016 by QUALCOMM Technologies Incorporated.
 All Rights Reserved.

 QUALCOMM Proprietary

-------------------------------------------------------------------------------

  $Header: //components/rel/core.glue/1.2/tools/scandump/scandumpparser.py#4 $
  $DateTime: 2017/08/29 12:59:18 $
  $Author: pwbldsvc $
  $Change: 14212689 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who         what, where, why
 --------   ---         ---------------------------------------------------------
 08/19/2016 AJCheriyan  Created v2.0. Support for inversions
 12/11/2015 AJCheriyan  Created v1.0
=============================================================================="""



import glob, itertools, os, string, sys, subprocess, re, platform, shutil, fnmatch, time, tempfile
from optparse import OptionParser
from ctypes import *
from helper import *
from xml.etree.ElementTree import Element, SubElement, tostring
import xml.etree.ElementTree as XMLParser
from xml.dom import minidom
import time
import binascii
import importlib

""" @Description: prettify
    @brief : Helper function to generate pretty xml string from xml element
    @param outputfile: Outputfile name.
    @param registers: Register dictionary to be written out
    @return String XML element
"""
def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")


# Function to parse input options
def parseoptions():
    global options
    argparser = OptionParser(usage = 'scandumpparser.py -x <bitfield info xml> -m <mapping> -d <binary>', version='%prog 2.0')

    argparser.add_option('-x', '--xml', dest='xml', help='Bitfield info xml file', metavar='xml', default=None)
    argparser.add_option('-m', '--map', dest='mapping', help='Signal/Register mapping file', metavar='sigcfg', default=None)
    argparser.add_option('-f', '--format', dest='mapfmt', help='Format for mapping file - aarch64 or aarch32', default='armv8')
    argparser.add_option('-d', '--data', dest='data', help='Binary scandump data', metavar='scandump', default=None)
    argparser.add_option('-o', '--output', dest='ofile', help='Output file name', metavar='output', default='register.txt')
    argparser.add_option('-p', '--parse', dest='pslice', help='Slice to parse', metavar='output', default=0)
    argparser.add_option('-c', '--bitfields', dest='bitfields', help='Bitfield count per slice', metavar='bitfield_count', default=32)
    argparser.add_option('-s', '--slices', dest='slices', help='Total number of slices (each of bitfield_count length)', metavar='slices', default=1)
    argparser.add_option('-k', '--skip', dest='skipheader', help='Header bytes to skip in the data binary', default=16)
    argparser.add_option('-g', '--debug', dest='debug', help='Enable additional debug', default=False)
    (options, args) = argparser.parse_args()

    if not options.xml:
        kill('Please provide XML bitfield file')
    if not options.mapping:
        kill('Please provide register/signal mapping file')
    if not options.data:
        kill('Please provide the binary scandump data')

    # Load the signal information
    global mappingdata
    mappingdata = importlib.import_module(options.mapping)


""" @brief : Function to parse the binary scandump data.
    @param datafile : Input scandump data to parse
    @param start_addr: Start of the data in the file
    @param chunksize: Chunk size for file reads (optional)
    @return Returns a one dimensional array of boolean values represented as strings
"""
def getscandump_data(datafile, start_addr = 0, chunksize=64):
    fptr = open(datafile, 'rb+')
    # Move to the actual start of data
    fptr.seek(start_addr, 0)

    # Read the file one chunk at a time
    binrep = []
    for blob in iter(lambda: fptr.read(chunksize), ''):
        hexrep = [binascii.hexlify(list(blob)[i]) for i in range(0, len(list(blob)))]
        binrep_adjust = [bin(int(hexrep[i], 16)).lstrip('0b').zfill(8)[::-1] for i in range(0, len(hexrep))]
        #binrep = binrep + [''.join(binrep_adjust[::-1])]
        binrep = binrep + binrep_adjust

    #for index in range(0, len(binrep)):
    #    binrep[index] = binrep[index][4:] + binrep[index][0:4]
    # Join them all to get the single bit stream of scandump data
    scandump_data = ''.join(binrep)
    return scandump_data

""" @brief : Function to extract a given stream of data from parsed scandump data
    @param scandump_data: Output of the getscandump_data api
    @param start_offset: Start of the first chunk of data within scandump_data.
    @param bitfieldnumber: Offset of the bitfield blob from start_offset
    @param shift_offset: Offset of the each entry in the bitfield within scandump_data
    @return Returns a one dimensional array of boolean values represented as strings
"""

def extract_bitfield(scandump_data, start_offset, bitfield_number, shift_offset):
    debug('Extracting bitfield data: ' + str(bitfield_number + start_offset) + ' with offset: ' + str(shift_offset))
    bitfield_data = [scandump_data[i] for i in range(start_offset + bitfield_number, len(scandump_data), shift_offset)]
    debug('Bitfield Data for bitfield' + str(bitfield_number) + '= ' + str(bitfield_data))
    debug('Total length of bitfield data: ' + str(len(bitfield_data)))
    return ''.join(bitfield_data)

""" @brief : Remap registers as needed
    @param parsed_registers: List of parsed registers
    @param mapping: Mapping to be used if physical registers are renamed
    @return Returns a dictionary of register and values
"""
def remap(parsed_registers, mapping):

    # For every SW register in the mapping, we need to find the hard / physical
    # registers which are part of it
    sw_registers = {}
    for sw_register, reg_decoding in mapping.iteritems():
        # Variable to hold our register value
        reg_val = ''
        # Check if the register is simply a pointer to another register or a raw value
        # It should come along with a formatter function
        if re.match('val(.*?)', reg_decoding['val']):
            matches = re.search('val\((.*?)\)', reg_decoding['val'])
            pointer_val = parsed_registers[matches.group(1)]
            # Format it now after checking if a formatter is present
            if 'formatter' not in reg_decoding:
                kill('Formatter missing in the entry for mapping: ' + reg_decoding['val'])
            pointer_val_int = reg_decoding['formatter'](pointer_val) #str(int(pointer_val, 2)).zfill(2)
            debug('Pointing to register: ' + pointer_val_int, False)
            for reg in reg_decoding['regs']:
                pointer_reg_name = reg % (pointer_val_int)
                debug('Register name: ' + pointer_reg_name, False)
                try:
                    pointer_reg_val = parsed_registers[pointer_reg_name]
                    """ Workaround for a weird bug where if the value is 0x0, the lstrip below returns NULL instead of '0' """
                    if (int(pointer_reg_val,2) == 0):
                        reg_val = reg_val + '00000000'
                    else:
                        reg_val = reg_val + str(hex(int(pointer_reg_val, 2))).lstrip('0x').rstrip('L').zfill(8)
                except:
                    debug('Pointer Register Error: Could not find register in the list: ' + pointer_reg_name, True)
                    continue
        else:

            for reg in reg_decoding['regs']:
                reg_name = reg
                try:
                    reg_val = reg_val + str(hex(int(parsed_registers[reg_name], 2))).lstrip('0x').rstrip('L')
                except:
                    debug('Direct Register Error: Could not find register in the list: ' + reg_name, True)
                    continue



        # Finally append the 0x to the register name
        reg_val = '0x' + reg_val.upper()

        debug( sw_register + ' has value : ' + reg_val, options.debug)
        sw_registers[sw_register] = reg_val

    return sw_registers

""" @brief : Function to parse the XML data and generate the register values
    @param register_info: List of registers to extract from the XML
    @return Returns a dictionary of registers (in the intended usage, these are physical registers)
"""

def xmlparse(inputfile, register_info):
    root = XMLParser.parse(inputfile).getroot()

    # Find the registers we need to extract from the mapping file first
    registers_val = {}
    bitfield_data = {}
    for register, signal in register_info.iteritems():
        registers_val[register] = list()
        for element in root.findall(".//bitfield-entry[register='%s']" % (register)):
            # all of these are unique registers but may have multiple bitfield entries
            # For additional protection, check the signal name as well
            if (not re.search(signal, element.find('signal').text)):
                continue
            # Find the bitfield entry this belongs to
            bitfield = element.find('bitfield').text
            # if we have not extracted this bitfield entry before, do that now and cache it
            if bitfield not in bitfield_data :
                debug('New bitfield encountered: ' + bitfield)
                slice_index = int(options.pslice) * int(options.bitfields)
                extracted_bitfield_data = extract_bitfield(scandump_data, slice_index , int(bitfield), int(options.slices) * int(options.bitfields))
                bitfield_data[bitfield] = extracted_bitfield_data
            # Now get the shift data for that particular entry
            shift = element.find('shift').text
            shift_array = shift.split(',')
            # Now get the inversion data for this entry
            inversion = element.find('inv_final').text
            inversion_array = inversion.split(',')
            # Create a register of size equal to the shift array
            if (len(registers_val[register]) < len(shift_array)):
                resize_arr = ['0'] * (len(shift_array) - len(registers_val[register]))
                registers_val[register] = registers_val[register] + resize_arr
                debug('Resizing %s length by value : %d ' % (register, len(resize_arr)))
            # If the index is not used, we use a 'x' to denote it in the xml format
            for index in range(0, len(shift_array)):
                if shift_array[index] not in ['x']:
                    # combine the inversion data along with the actual value from the bitfield data
                    debug('Bitfield : %s, Index: %s' % (bitfield, index))
                    bitvalue = bitfield_data[bitfield][int(shift_array[int(index)])]
                    bitvalue = bitvalue if (inversion_array[index] == '0') else str(int(bitvalue) ^ 1)
                    registers_val[register][index] = bitvalue
        # Now reverse it since we are printing bit 0 on the left. We need it on the right
        registers_val[register] = registers_val[register][::-1]
        registers_val[register] = ''.join(registers_val[register])

    # Print out the registers for debug
    for register, val in registers_val.iteritems():
        debug('Register Name: %s ' %( register), options.debug)
        debug('Hex Value : %s ' % hex(int(val,2)), options.debug)
        debug('Raw Value: ' + str(val), options.debug)
        debug('Length of register: ' + str(len(val)), options.debug)

    return registers_val


""" @Description: get_output
    @brief : Function to write the output to file
    @param outputfile: Outputfile name.
    @param registers: Register dictionary to be written out
    @return None
"""
def gen_output(outputfile, registers):
    if outputfile is not None:
        (base, extension) = os.path.splitext(os.path.basename(outputfile))
        debug('Writing to file: ' + outputfile)
        ofptr = open(outputfile, 'w+')
        ofptr.write( '\n// Auto-generated by ' + os.path.basename(__file__) + ' on ' + pretty_time())
        ofptr.write('\n// Scandump binary: ' + options.data)
        # Put an error handler in there to make sure we don't die on some terrible formatting.
        if extension in ['.cmm']:
            ofptr.write('\n ON ERROR CONTINUE')
        for name in sorted(registers.keys()):
            if extension in ['.cmm']:
                ofptr.write('\nREGISTER.SET ' + name + ' ' + registers[name].rstrip('x') + '\n')
            else:
                ofptr.write('\n' + name + ' : ' + registers[name])
        ofptr.close()
    else:
        kill('Please provide a valid outputfile. Input: ' + outputfile)
    return



""" @Description: Main
    @brief : Main function of the program
    @param outputfile: Outputfile name.
    @param registers: Register dictionary to be written out
    @return None
"""

if __name__ == '__main__':

    # Parse all the options passed in
    parseoptions()

    # Generate the output file
    script_start_time = get_time()
    scandump_data = getscandump_data(options.data, start_addr = int(options.skipheader))
    debug('Time to extract scandump data: ' + str(timedelta(script_start_time)), True)


    # Retrieve of the list of physical registers
    parsed_registers = xmlparse(options.xml, mappingdata.input_signals)
    if parsed_registers is None:
        kill('XML Parsing returned null list of registers. Check input files')

    # Now translate them to the software understandable registers if we stuff to translate
    default_map = mappingdata.aarch64_map
    if (options.mapfmt == 'armv8'):
        default_map.update(mappingdata.aarch32_map)
    elif (options.mapfmt == 'aarch64'):
        default_map = mappingdata.aarch64_map
    elif (options.mapfmt == 'aarch32'):
        default_map = mappingdata.aarch32_map
    else:
        debug('Time to parse scandump data: ' + str(timedelta(script_start_time)), True)
        kill('Unknown map format ' + options.mapfmt)

    if (options.mapfmt != 'nomap'):
        software_registers = remap(parsed_registers, default_map)
        # Now pretty print the registers
        gen_output(options.ofile, software_registers)

    debug('Time to parse scandump data: ' + str(timedelta(script_start_time)), True)
