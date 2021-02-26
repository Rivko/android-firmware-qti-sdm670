#===========================================================================

#  helper script for emergency programmer for blank emmc devices

# REFERENCES


# when          who     what, where, why 
# --------      ---     ------------------------------------------------------- 
# 2015-08-24    tw      Added support for UINT16 and UINT8 types
# 2012-10-08    ah      First Release

# Copyright (c) 2007-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
# ===========================================================================*/
from xml.etree import ElementTree as ET
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom

#from common import *
#import common
from time import sleep, time
import time
from inspect import getargspec
from os.path import getsize
import struct, sys
import logging

port_trace_level        = logging.INFO

c_data_fields = 8

def init_device_log():
    global port_logger

    LOGGINGFORMAT = '%(asctime)s %(filename)s %(lineno)s %(levelname)s: %(message)s'
    logging.basicConfig(filename='port_trace.txt', filemode='w' ,format=LOGGINGFORMAT,level=port_trace_level)   # filemode='w', level=logging.INFO

    #define a 2nd Handler which writes INFO messages or higher to the sys.stderr
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    console.setFormatter( logging.Formatter( LOGGINGFORMAT ) )

    #add the handler to the root logger
    #logging.getLogger('').addHandler(console)
    port_logger = logging.getLogger('zeno')
    port_logger.addHandler(console)


def print_c_formatted(data_list, title, fp):
    fp.write("\t/* %s */\n" % title)
    for i in range(0, len(data_list), 2):
        data_item = data_list[i:i+2]
        if i % c_data_fields == 0:
            fp.write("\n\t")
        #import pdb; pdb.set_trace()
        fp.write("0x%.2X," % int(data_item,16))
        if i+2 % c_data_fields == 0:
            fp.write("\n")
        else:
            fp.write(" ")
    fp.write("\n")
    fp.write("\n")


def write_c_file(filename, header,metadata,blockdata):
    fp = open(filename, 'w')
    fp.write("""
/*==========================================================================	
                        NOTE: This is a generated file!
===========================================================================*/	
/*==========================================================================

                   INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_config_data.h"

/*=============================================================================

        LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/**
* fixed size array that holds the cdt table in memory, it's intialized to 
* CDT v1 with only one data block which is platform id. The platform id type 
* is UNKNOWN as default.
*/
uint8 config_data_table[CONFIG_DATA_TABLE_MAX_SIZE] = 
{
""")

    print_c_formatted(header, "Header", fp)
    print_c_formatted(metadata, "Meta data", fp)
    print_c_formatted(blockdata, "Block data", fp)

    fp.write("""};
""")
    fp.write("""
/**
    cdt table size
*/
""")
    fp.write("uint32 config_data_table_size = %d;\n" % (len("".join(header+metadata+blockdata))/2))
    fp.close()


def write_bin_file(filename, data): 
    fp = open(filename, 'wb')
    # Add padding to make data 4-byte aligned
    padding = (len(data)/2)%4
    if padding!=0:
        data = data + '00'*(4 - padding)
    for i in range(0, len(data), 2):
        data_item = data[i:i+2]
        fp.write(chr(int(data_item, 16)))
    fp.close()


def ReturnSizeString(size):
    if size>(1024*1024*1024):
        return "%.2f GB" % (size/(1024.0*1024.0*1024.0))
    elif size>(1024*1024):
        return "%.2f MB" % (size/(1024.0*1024.0))
    elif size>(1024):
        return "%.2f KB" % (size/(1024.0))
    else:
        return "%i B" % (size)

def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")



if __name__ == "__main__":

    if len(sys.argv) < 3:
    	print "Usage: cdt_parse.py filename.xml binfile.bin"
    	sys.exit(1);

    init_device_log()

    filename = sys.argv[1]
    bin_file_name = sys.argv[2]
    c_file_name = "boot_cdt_array.c"

    port_logger.info("Filename: '%s'"% (filename))

    root = ET.parse( filename )
    NewXML = Element(root.getroot().tag)    ## elem = tree.getroot(), typically it's <data>

    RootTag = root.getroot().tag

    iter = root.getiterator()

    Array       = []
    TempArray   = []
    for element in iter:
        ##print "\n" + element.tag
        if element.tag == 'device':
            if len(TempArray)>0:
                Array.append(TempArray)
                TempArray   = []
                print Array
        if element.keys():
            TEXT = element.text
            for name, value in element.items():
                ##print "%s=%s" % (name,value)
                if name == 'type':
                    if value == 'DALPROP_ATTR_TYPE_UINT32':
                        ## means it looks like 0x0
                        sz = element.text.strip()   ## remove all whitespace,newlines etc
                        ## does it start with 0x ?
                        #import pdb; pdb.set_trace()
                        if sz[0:2].lower()=='0x':
                            ##print "%.8X" % int(sz,16)
                            ## need to keep as big endian if written in hex
                            TempArray.append("%.8X" % int(sz,16))
                        else:
                            ## need to convert to little endian if written as an integer
                            temp = int(sz)
                            TempArray.append("%.2X%.2X%.2X%.2X" % (temp&0xFF,(temp>>8)&0xFF,(temp>>16)&0xFF,(temp>>24)&0xFF))
                    elif value == 'DALPROP_ATTR_TYPE_UINT8':
                        ## means it looks like 0x0
                        sz = element.text.strip()   ## remove all whitespace,newlines etc
                        ## does it start with 0x ?
                        #import pdb; pdb.set_trace()
                        if sz[0:2].lower()=='0x':
                            ##print "%.8X" % int(sz,16)
                            ## need to keep as big endian if written in hex
                            TempArray.append("%.8X" % int(sz,16))
                        else:
                            ## need to convert to little endian if written as an integer
                            temp = int(sz)
                            TempArray.append("%.2X" % (temp&0xFF))
                    elif value == 'DALPROP_ATTR_TYPE_UINT16':
                        ## means it looks like 0x0
                        sz = element.text.strip()   ## remove all whitespace,newlines etc
                        ## does it start with 0x ?
                        #import pdb; pdb.set_trace()
                        if sz[0:2].lower()=='0x':
                            ##print "%.8X" % int(sz,16)
                            ## need to keep as big endian if written in hex
                            TempArray.append("%.8X" % int(sz,16))
                        else:
                            ## need to convert to little endian if written as an integer
                            temp = int(sz)
                            TempArray.append("%.2X%.2X" % (temp&0xFF,(temp>>8)&0xFF))
                    elif value == 'DALPROP_ATTR_TYPE_BYTE_SEQ':
                        ## means it looks like 0x43, 0x44, 0x54, 0x0, end
                        sz = element.text.strip()   ## remove all whitespace,newlines etc
                        temp = sz.split(',')
                        sz = ''
                        for i in range(len(temp)-1):
                            ##print temp[i].strip()
                            sz += "%.2X" % int(temp[i].strip(),16)

                        TempArray.append(sz)
                    else:
                        ## unhandled
                        print "ERROR: Don't know how to handle that"
                        sys.exit(1)

                #import pdb; pdb.set_trace()

    if len(TempArray)>0:
        Array.append(TempArray)
        TempArray   = []
        print Array

metadata = []

## length of Array[0] is the size of the header
##cdb0    = ''.join(Array[1])     ## len is 5 (actually it's 10, but its a string rep of 0xFF, which is 'FF', so it's half
##cdb1    = ''.join(Array[2])     ## len is 392

## metadata is distance from 0 to first cdb, then size of cdb
### so if 2 CDB, then metadata is going to be 2*4bytes = 8, 
## so  14+8=22
## and 14+8=22+size of last cdb, so +5 = 
blockdata = ''
NumCDBs = len(Array)-1  ## i.e. 2
header  = ''.join(Array[0])     ## len is 28, but it's in string form, so 1 byte is FF, so in string form its 2bytes, so len=14
NumHeaderBytes = len(header)/2

SizeOfMetaData = NumCDBs*4

SizeOfLastCDB = 0

MetaData = ''

for i in range(NumCDBs):
    DistanceInBytesToCDB = NumHeaderBytes + SizeOfMetaData + SizeOfLastCDB
    SizeOfLastCDB = len(''.join(Array[(i+1)]))/2
    print "DistanceInBytesToCDB=%d" % DistanceInBytesToCDB
    print "SizeOfLastCDB=%d" % SizeOfLastCDB
    MetaData = MetaData + ''.join( "%.2X%.2X" % ( (int(DistanceInBytesToCDB)>>0)&0xFF , (int(DistanceInBytesToCDB)>>8)&0xFF ) )
    MetaData = MetaData + ''.join( "%.2X%.2X" % ( (int(SizeOfLastCDB)>>0)&0xFF , (int(SizeOfLastCDB)>>8)&0xFF ))
    blockdata = blockdata + ''.join(Array[i+1])

    

##import pdb; pdb.set_trace() 

# "%.2X" % int(header[0:2])


#(Pdb) header
#['43445400', '0100', '00000000', '00000000']

#(Pdb) metadata
#['1600', '0500', '1B00', '8801']

#(Pdb) blockdata
#['0201010000', '01000000', '00524444', 'FFFF0000', '02000000', 'B8000000', '03000000', '01000000', '00000000', '05000000', '14050000', 'A4010000', '60AE0A00', '76020000', '40010000
#', '58980000', '78050000', '4B000000', '4B000000', '0E010000', '64000000', '96000000', '4B000000', 'F0000000', '02000000', '0E000000', '0A000000', '08000000', '0E000000', '0A000000
#', '08000000', '20000000', '08000000', '03000000', 'F0000000', 'F4010000', '4B000000', '08000000', '04000000', '05000000', '80841E00', '10270000', '04000000', '100E0000', '84030000
#', '10270000', '0A000000', '96000000', 'F4010000', '80A20000', '00200000', '19000000', '37000000', '00000000', '00000000', '05000000', '14050000', 'A4010000', '60AE0A00', '76020000
#', '40010000', '58980000', '78050000', '4B000000', '4B000000', '0E010000', '64000000', '96000000', '4B000000', 'F0000000', '02000000', '0E000000', '0A000000', '08000000', '0E000000
#', '0A000000', '08000000', '20000000', '08000000', '03000000', 'F0000000', 'F4010000', '4B000000', '08000000', '04000000', '05000000', '80841E00', '10270000', '04000000', '100E0000
#', '84030000', '10270000', '0A000000', '96000000', 'F4010000', '80A20000', '00200000', '19000000', '37000000']

write_bin_file(bin_file_name, header+MetaData+blockdata)
write_c_file(c_file_name, header,MetaData,blockdata)

print "\n\nCreated '%s'" % bin_file_name
print "Created '%s'" % c_file_name

