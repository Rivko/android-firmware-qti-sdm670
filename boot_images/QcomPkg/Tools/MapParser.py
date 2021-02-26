from __future__ import with_statement
import re, sys

class Section:
    """ Basic class for tracking linker sections
    
        This class if used for tracking any instances created in the SECTIONS
        portion of a linker script (.ld). Also used for tracking hardware
        memory regions.
    """
    def __init__(self, name, address, size):
        """ Inits Section info"""
        self.name = name
        self.address = address
        self.size = size
    def __str__(self):
        return self.name + "\tAddr: " + hex(self.address) + \
               "\tSize: " + hex(self.size) + "(" + str(self.size) + ")"

class Symbol:
    """ Basic class for tracking linker symbols

        This class is used for tracking member symbols of sections defined in
        the SECTIONS portion of a linker script (.ld).
    """
    def __init__(self, address, size, obj, lib, seg, name,section):
        """ Inits Symbol info """
        self.address = address
        self.size = size
        self.obj = obj
        self.lib = lib
        self.segment = seg
        self.name = name
        self.section = section
    def __str__(self):
        return self.name + "\tAddr: " + hex(self.address) + \
               "\tSize:" + hex(self.size) + "\tObj: " + self.obj + "\tLib: " \
               + self.lib + "\tSeg: " + self.segment

################################################################################
# parse_map
################################################################################
def parse_map(filename, symbols, compiler):
    """ Parses sections and symbols from .map file

        Scans a chosen .map file and extracts symbol information.
        This information is stored in a list of symbols

        Args:
            filename: Name of .map file to parse
            sections: list to fill with symbol info
            compiler: Library of parsing functions specific to compiler that
                      generated the .map file
    """
    with open(filename) as fh:    
        st = compiler.MapScannerTracker(fh)
        while(1):
            ret = compiler.get_next_map_token(st)
            if ret == None:
                break
            else:
                symbols.append(ret)

    symbols.sort(key=lambda sym: sym.address)

################################################################################
# parse_xml
################################################################################
def parse_xml(filename, regions, compiler):
    """ Parses memory regions from .xml file (IPCAT)

        Scans a chosen .xml file from IPCAT and extracts hardware memory 
        regions. The regions are stored as keys in an input map. Values
        are set to empty dicts which are assumed to initalized later
        with symbols extracted from a .map file

        Args:
            filename: Name of .xml file to parse
            regions: Map to fill with hardware memory regions info
            compiler: Library of parsing functions specific to .xml file
    """
    with open(filename) as fh:
        st = compiler.XMLScannerTracker(fh)
        while(1):
            ret = compiler.get_next_xml_token(st)
            if ret == None:
                break
            else:
                regions[ret] = {}
 
################################################################################
# parse_ld
################################################################################
def parse_ld(filename, types, compiler):
    """ Parses PHDR types of .ld file

        Scan a chosen .ld file and extracts PHDR types of each entity defined
        in the SECTIONS area. This is stored in an input map as section to
        PHDR type.

        Args:
            filename: Name of .ld file to parse
            types: Map to fill with .ld type information
            compiler:
    """
    with open(filename) as fh:
        st = compiler.LDScannerTracker(fh)
        while(1):
            ret = compiler.get_next_ld_token(st)
            if ret == None:
                break
            else:
                types[ret[0]] = ret[1]              

