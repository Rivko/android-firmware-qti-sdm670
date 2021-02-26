#!/usr/bin/python

'''
genfmpcapsule.py:

    Usage:
        Run "genfmpcapsule.py -h"

    Description:
        This script is used to generate firmware management protocol capsules
        described in section 32.2 in the UEFI 2.4 Specifications

    Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.

'''

import os
import sys
import struct
import uuid
import xml.etree.ElementTree as ET
from optparse import OptionParser

CapsuleHeader           = None
EmbeddedDrivers         = []
Payloads                = []
PathPrefixes            = []

# Get binary data for a capsule
def get_capsule(body_only):
    global CapsuleHeader

    # Get the CapsuleBody
    CapsuleBody = get_capsule_body()

    HeaderSize = 28
    #This is EFI_FIRMWARE_MANAGEMENT_CAPSULE_ID_GUID
    CapsuleGuid = uuid.UUID("6DCBD5ED-E82D-4C44-BDA1-7194199AD92A")
    Flags = 0
    if CapsuleHeader.attrib["flag_persist_across_reset"] == "true":
        Flags = Flags | 0x00010000
    if CapsuleHeader.attrib["flag_populate_system_table"] == "true":
        Flags = Flags | 0x00020000
    if CapsuleHeader.attrib["flag_initiate_reset"] == "true":
        Flags = Flags | 0x00040000
    CapsuleImageSize = HeaderSize + len(CapsuleBody)

    '''
    typedef struct {
    EFI_GUID CapsuleGuid;
    UINT32 HeaderSize;
    UINT32 Flags;
    UINT32 CapsuleImageSize;
    } EFI_CAPSULE_HEADER;
    '''
    CapsuleHeader = struct.pack('16sIII', CapsuleGuid.bytes_le, HeaderSize, Flags, CapsuleImageSize)
    if not body_only:
      # Append the payload to create the capsule
      Capsule = CapsuleHeader + CapsuleBody
    else:
      Capsule = CapsuleBody
    return Capsule

# Creates the capsule's body
def get_capsule_body():
    global Payloads

    Version = 1
    EmbeddedDriverCount = len(EmbeddedDrivers)
    PayloadItemCount = len(Payloads)

    CapsuleBody = ""
    ItemOffsetList = []
    NumOffsets = EmbeddedDriverCount + PayloadItemCount

    # Set the current offset to the end of EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER
    CurrentOffset = 8*(EmbeddedDriverCount + PayloadItemCount) + 8

    # Loop through embedded drivers, appending them together
    for EmbeddedDriver in EmbeddedDrivers:
        ItemOffsetList.append(CurrentOffset)
        EmbeddedDriverData = get_driver(EmbeddedDriver)
        CurrentOffset = CurrentOffset + len(EmbeddedDriverData)
        CapsuleBody = CapsuleBody + EmbeddedDriverData

    # Loop through payloads, appending them together
    for payload in Payloads:
        ItemOffsetList.append(CurrentOffset)
        PayloadData = get_payload(payload)
        CurrentOffset = CurrentOffset + len(PayloadData)
        CapsuleBody = CapsuleBody + PayloadData

    '''
    #pragma pack(1)
    typedef struct (
    UINT32 Version;
    UINT16 EmbeddedDriverCount;
    UINT16 PayloadItemCount;
     UINT64 ItemOffsetList[];
    ) EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER;
    '''
    CapsuleBodyHeaderData = struct.pack('IHH'+ 'Q'*NumOffsets, Version, EmbeddedDriverCount, PayloadItemCount, *ItemOffsetList)

    # Append the body to the header
    CapsuleBody = CapsuleBodyHeaderData + CapsuleBody

    return CapsuleBody

# Gets binary data for embedded driver
def get_driver(xmlNode):
    FilePath = get_valid_filepath(xmlNode.attrib["filename"])
    DriverImage_file = open(FilePath, 'rb')
    DriverImage_data = DriverImage_file.read()
    DriverImage_file.close()
    return DriverImage_data


# Create payload
def get_payload (xmlNode):
    # Parse the XML node for this payload to get all of the information we need to write
    FilePath = get_valid_filepath(xmlNode.attrib["filename"])
    Version = 2
    UpdateImageIndex = int(xmlNode.attrib["index"])
    UpdateImageTypeId = uuid.UUID(xmlNode.attrib["type"])

    print "Creating payload for " + FilePath + " ..."

    UpdateImage_file = open(FilePath, 'rb')
    BinaryUpdateImage = UpdateImage_file.read()
    UpdateImage_file.close()
    UpdateImageSize = len(BinaryUpdateImage)

    UpdateVendorCode = ""
    UpdateVendorCodeSize = 0
    if xmlNode.attrib["vendorcodefile"]:
        UpdateVendorCodeFilePath = get_valid_filepath(xmlNode.attrib["vendorcodefile"])
        UpdateVendorCode_file = open(UpdateVendorCodeFilePath, 'rb')
        UpdateVendorCode = UpdateVendorCode_file.read()
        UpdateVendorCode_file.close()
        UpdateVendorCodeSize = len(UpdateVendorCode)

    '''
    #pragma pack(1)
    typedef struct (
    UINT32 Version;
    EFI_GUID UpdateImageTypeId;
    UINT8 UpdateImageIndex;
    UINT8 reserved_bytes[3];
    UINT32 UpdateImageSize;
    UINT32 UpdateVendorCodeSize;
    UINT64 UpdateHardwareInstance;
    ) EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER;
    '''
    CapsuleImageHeader = struct.pack('I16sBBBBIIQ', Version, UpdateImageTypeId.bytes_le, UpdateImageIndex, 0,0,0, UpdateImageSize, UpdateVendorCodeSize, 0)

    Payload = CapsuleImageHeader + BinaryUpdateImage + UpdateVendorCode

    return Payload

# Given a filename, checks all paths by traversing PathPrefixes and returns the first valid path
def get_valid_filepath(filename):
    for PathPrefix in PathPrefixes:
        if os.path.isfile(PathPrefix + filename):
            return PathPrefix + filename
    print "ERROR: Unable to find " + filename + " in any of the provided paths"
    sys.exit(1)

# Parses the provided XML file to extract all needed information needed for capsule creation
def parse_xml_file(filename):
    global PathPrefix, CapsuleHeader, Payloads, EmbeddedDrivers
    print "Parsing " + filename + " ..."
    tree = ET.parse(filename)
    root = tree.getroot()
    CapsuleHeader = root.find("./capsule/header")
    Payloads = root.findall("./capsule/body/payload")
    EmbeddedDrivers = root.findall("./capsule/body/driver")
    return

def search_path_callback(option, opt, value, parser):
    global PathPrefixes
    PathPrefixes = value.split(',')
    return

def main():
    usage = "usage: %prog [options] arg1 arg2"
    parser = OptionParser(usage=usage)
    parser.add_option("-s", "--search-paths",
                     type="string",
                     action="callback",
                     callback=search_path_callback,
                     help="Paths this script will look in for files that are referenced in the xml input")
    parser.add_option("-i", "--input",
                     type="string",
                     action="store",
                     dest="input",
                     help="An input xml file which specifies this capsule. See capsule_param.xml for an example")
    parser.add_option("-o", "--output",
                     action="store",
                     dest="output",
                     help="The output path for the capsule file that will be generated by this tool")
    parser.add_option('-b', '--body-only',
                      action="store_true", dest="bodyonly", default=False,
                      help="Only output capsule body")

    (options, args) = parser.parse_args()

    if not options.input:
        parser.error("No input xml provided")
    if not options.output:
        parser.error("No output filepath provided")

    # Parse the xml file
    parse_xml_file(options.input)
    # Get the capsule binary and write it to the specified output
    capsule_data = get_capsule(options.bodyonly)
    capsule_file = open(options.output, 'wb')
    capsule_file.write(capsule_data)
    capsule_file.close()

    print "DONE"

if __name__ == '__main__':
    main()

