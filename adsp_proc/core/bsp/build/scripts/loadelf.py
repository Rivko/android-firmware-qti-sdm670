# --------------------------------------------------------------------------- #
# File Name ::  loadelf.py
# Purpose   ::  Module to parse ELF files
# --------------------------------------------------------------------------- #
# $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/loadelf.py#1 $
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
#
# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #
#                              IMPORT MODULES                                 #
# --------------------------------------------------------------------------- #
import sys, os
import logging
import elftools.elf.elffile
import elftools.elf.relocation
import elftools.elf.constants
import elftools.elf.enums

# --------------------------------------------------------------------------- #
#                       LOCAL VARIABLE DEFINITIONS                            #
# --------------------------------------------------------------------------- #
version_number = '1.00'
quietmode = False

# --------------------------------------------------------------------------- #
#                         FUNCTION DEFINITIONS                                #
# --------------------------------------------------------------------------- #
def error(err):
    global quietmode
    if not quietmode:
        logging.error(err)
        print "\nERROR: " + err
    raise RuntimeError(err)
    
def debug(s):
    global quietmode
    if not quietmode:
        logging.debug(s)

def generate_full_binary():
    elfname = sys.argv[1]
    binname = sys.argv[2]
    print "Loading ELF: " + elfname
    print "Writing BIN: " + binname
    inelf = open(elfname, "rb")
    outbin = open(binname, "wb")
    elf = elftools.elf.elffile.ELFFile(inelf)
    # ELF Header
    elfheader = elf.header
    print "There are {0} sections".format(elfheader['e_shnum'])
    entry = elfheader['e_entry']
    print "Entry point: " + hex(entry)
    # Verify 32-bit
    if elf.elfclass != 32:
        print "Unsupported ELF class: " + str(elf.elfclass)
        sys.exit(1)
    # Verify no relocations
    for section in elf.iter_sections():
        if isinstance(section, elftools.elf.relocation.RelocationSection):
            print "ELF file has relocations which are not supported"
            sys.exit(1)
    # Iterate through the sections and write them out
    expected_addr = 0
    prev_addr = 0
    overlay_size = 0
    cur_offset = 0
    for section in sorted(elf.iter_sections(), key=lambda x:x['sh_addr']):
        addr = section['sh_addr']
        offset = addr - entry
        data = section.data()
        size = section['sh_size']
        print "Looking at section '{0:s}' @ offset {1:s}={2:s}".format(section.name, hex(offset), hex(cur_offset))
        if str(section.name) == '.sdata':
            print "  -> analyzing .sdata for content"
            import struct
            firstbyte = struct.unpack('B', data[0])[0]
            secondbyte = struct.unpack('B', data[1])[0]
            if firstbyte == 0:
                print "  -> why is the first byte 0?"
                sys.exit(1)
            else:
                print "  -> we're good: {0:s}, {1:s}".format(hex(firstbyte), hex(secondbyte))
        # Make sure we have an address
        if addr == 0:
            print "  -> skipping due to address of 0"
            continue
        # Make sure we have data
        if len(data) == 0:
            print "  -> skipping due to size of 0"
            continue
        # Sanity check that the section size matches the number of bytes
        if size != len(data):
            print "Section {0:s} has size mismatch ({1:d},{2:d})".format(size, len(data))
            sys.exit(1)
        # Skip non-alloc sections
        if not (section['sh_flags'] & elftools.elf.constants.SH_FLAGS.SHF_ALLOC):
            print "  -> skipping due to non-ALLOC"
            continue
        # Make sure the data is contiguous
        if expected_addr != 0 and expected_addr != addr:
            delta = addr - expected_addr
            if delta > 0:
                print "  -> writing an extra {0:s} bytes prior to this section".format(hex(delta))
                outbin.write(bytearray([0] * delta))
                cur_offset += delta
        expected_addr = addr + size
        # Error checking
        print "  -> Adjusted offsets: {0:s}={1:s}".format(hex(offset),hex(cur_offset))
        if offset != cur_offset and section['sh_type'] != 'SHT_NOBITS':
            print "!!! offsets no longer match"
            sys.exit(1)
        # Handle zero-init (NOBITS)
        if section['sh_type'] == 'SHT_NOBITS':
            if addr == prev_addr:
                # We have an overlay so just keep adding if necessary
                if size > overlay_size:
                    delta = size - overlay_size
                    print "  -> adding {0:s} bytes of zero to the overlay".format(hex(delta))
                    outbin.write(bytearray([0] * delta))
                    overlay_size = size
                    cur_offset += delta
                else:
                    print "  -> skipping since overlay already large enough"
                # Otherwise do nothing
            else:
                # First time, so just write normally
                print "  -> writing {0:s} bytes of zero".format(hex(size))
                outbin.write(bytearray([0] * size))
                overlay_size = size
                cur_offset += size
            # Update previous address
            prev_addr = addr
            expected_addr = addr + overlay_size
            continue
        else:
            overlay_size = 0
        # Write the data!!
        print "  -> writing {0:s} bytes".format(hex(size))
        outbin.write(data)
        cur_offset += len(data)
        # Update previous address
        prev_addr = addr
    inelf.close()
    outbin.close()

def _create_overlay_name(n1, n2):
    pieces = n1.strip('.').split('.') + n2.strip('.').split('.')
    unique_pieces = []
    for piece in pieces:
        if piece not in unique_pieces:
            unique_pieces.append(piece)
    return '.'+'.'.join(unique_pieces)

def load_elf(elffname, quiet):
    global quietmode
    quietmode = quiet

    debug("Loading elf: " + elffname)
    inelf = open(elffname, "rb")
    elf = elftools.elf.elffile.ELFFile(inelf)
    elfinfo = { }
    # ELF Header
    elfheader = elf.header
    debug("There are {0} sections".format(elfheader['e_shnum']))
    elfinfo['entry'] = elfheader['e_entry']
    debug("Entry point: " + hex(elfinfo['entry']))
    # Verify 32-bit
    if elf.elfclass != 32:
        error("Unsupported ELF class: " + str(elf.elfclass))
    # Verify no relocations
    for section in elf.iter_sections():
        if isinstance(section, elftools.elf.relocation.RelocationSection):
            error("ELF file has relocations which are not supported")

    # Iterate through the sections and save pertinent information
    sections = []
    for elfsection in sorted(elf.iter_sections(), key=lambda x:x['sh_addr']):
        if not (elfsection['sh_flags'] & elftools.elf.constants.SH_FLAGS.SHF_ALLOC):
            debug("Ignore section '{0:s}' due to no ALLOC".format(elfsection.name))
            continue
        debug(" - {0:s} ({1:s},{2:s})".format(elfsection.name, hex(elfsection['sh_addr']), hex(elfsection['sh_size'])))
        section = { }
        section['name']   = elfsection.name
        section['addr']   = elfsection['sh_addr']
        section['offset'] = section['addr'] - elfinfo['entry']
        section['size']   = elfsection['sh_size']
        section['data']   = elfsection.data()
        section['write']  = elfsection['sh_flags'] & elftools.elf.constants.SH_FLAGS.SHF_WRITE
        section['exec']   = elfsection['sh_flags'] & elftools.elf.constants.SH_FLAGS.SHF_EXECINSTR

        # Error checks
        if section['addr'] > (2**32 - 1):
            error("Address for " + section['name'] + " is too large: " + hex(section['addr']))
        if section['size'] > (2**32 - 1):
            error("Size for " + section['name'] + " is too large: " + hex(section['size']))
        if len(section['data']) != section['size']:
            error("Size for " + section['name'] + " doesn't match data: " + hex(len(section['data'])) + "!=" + hex(section['size']))

        # Append
        sections.append(section)

    # Collapse overlays using double loop
    debug("Looking for overlays")
    elfinfo['sections'] = [ ]
    i = 0
    while i < len(sections):
        section1 = sections[i]
        isoverlay = False
        overlayname = section1['name'].lstrip('.')
        maxoverlaysize = section1['size']
        overlaydata = section1['data']
        lastoverlayindex = i
        j = i+1
        while j < len(sections):
            section2 = sections[j]
            # Check if starting addresses match
            if section1['addr'] == section2['addr']:
                debug("Sections {0:s} and {1:s} have same address {2:s}".format(section1['name'], section2['name'], hex(section1['addr'])))
                isoverlay = True
                overlayname = _create_overlay_name(overlayname, section2['name'])
                if section2['size'] > maxoverlaysize:
                    debug("Section {0:s} is larger".format(section2['name']))
                    maxoverlaysize = section2['size']
                    overlaydata = section2['data']
                lastoverlayindex = j
            # Increment index
            j += 1

        if isoverlay:
            # Create a new section with the overlay informatino
            section = { }
            section['name']   = '.' + overlayname.strip('.').replace('.','_')
            section['addr']   = section1['addr']
            section['offset'] = section1['offset']
            section['size']   = maxoverlaysize
            section['data']   = overlaydata
            section['write']  = section1['write']
            section['exec']   = section1['exec']
            debug("Overlay section {0:s} start={1:s}, size={2:s}".format(section['name'], hex(section['addr']), hex(section['size'])))
            elfinfo['sections'].append(section)
            # Update section1 index to ignore the entire overlay
            i = lastoverlayindex
        else:
            # No overlay so just use info as is from ELF
            elfinfo['sections'].append(section1)
        # Increment index
        i += 1

    # Iterate through the segments and save pertinent information
    elfinfo['segments'] = []
    i = 0
    for elfsegment in elf.iter_segments():
        # Ignore non-existent segments
        if elfsegment['p_memsz'] == 0:
            debug("Ignore segment #{0:d} due to no memory size".format(i))
            i += 1
            continue
        debug("Segment #{0:d} - ({1:s},{2:s})".format(i, hex(elfsegment['p_paddr']), hex(elfsegment['p_memsz'])))
        # Error check that memsize >= filesize
        if elfsegment['p_filesz'] > elfsegment['p_memsz']:
            error("Segment #{0:d} filesize({1:s}) > memsize({2:s})".format(i, hex(elfsegment['p_filesz']), hex(elfsegment['p_memsz'])))
        # Figure out the sections in this segment
        sections_in_this_segment = []
        for elfsection in elf.iter_sections():
            if not elfsection.is_null() and elfsegment.section_in_segment(elfsection):
                sections_in_this_segment.append(elfsection.name)
        # Create a dictionary to hold our information
        segment = { }
        segment['type']    = elfsegment['p_type']
        segment['index']   = i
        segment['addr']    = elfsegment['p_paddr']
        segment['offset']  = segment['addr'] - elfinfo['entry']
        segment['size']    = elfsegment['p_memsz']
        segment['filesize']  = elfsegment['p_filesz']
        segment['data']    = elfsegment.data()
        segment['write']   = elfsegment['p_flags'] & elftools.elf.constants.P_FLAGS.PF_W
        segment['exec']    = elfsegment['p_flags'] & elftools.elf.constants.P_FLAGS.PF_X
        segment['sections']= sections_in_this_segment
        elfinfo['segments'].append(segment)

        # Iterate our segment index
        i += 1
    # Check for errors (overlaps in segments)
    for i in range(len(elfinfo['segments'])):
        for j in range(i+1, len(elfinfo['segments'])):
            seg1 = elfinfo['segments'][i]
            seg2 = elfinfo['segments'][j]
            if seg1['offset'] < seg2['offset'] and seg1['offset'] + seg1['size'] > seg2['offset']:
                error("Segment #{0:d} overlaps with segment #{1:d}".format(i,j))
            if seg1['offset'] >= seg2['offset'] and seg1['offset'] < seg2['offset'] + seg2['size']:
                error("Segment #{0:d} overlaps with segment #{1:d}".format(i,j))

    # Return the simplified ELF information
    inelf.close()
    return elfinfo

def find_symbols_in_elf(elffname, symbol_names, quiet):
    global quietmode
    quietmode = quiet

    # Convert to list if necessary
    if type(symbol_names) is not list:
        symbol_names = [symbol_names]

    debug("Loading elf: " + elffname)
    inelf = open(elffname, "rb")
    elf = elftools.elf.elffile.ELFFile(inelf)
    elfinfo = { }
    # ELF Header
    elfheader = elf.header
    elfinfo['entry'] = elfheader['e_entry']
    debug("Entry point: " + hex(elfinfo['entry']))
    # Verify 32-bit
    if elf.elfclass != 32:
        error("Unsupported ELF class: " + str(elf.elfclass))

    # Get symbol table sections
    symbol_tables = []
    for section in elf.iter_sections():
        # Ignore sections that are not symbol tables
        if not isinstance(section, elftools.elf.sections.SymbolTableSection):
            continue

        # Ignore zero size tables
        if section['sh_entsize'] == 0:
            continue

        # Store the section
        symbol_tables.append(section)

    # Look for our symbols
    symbols = {}
    for symbol_name in symbol_names:
        debug("Looking for " + symbol_name)
        found = False
        # For each symbol, iterate through the symbol tables to find it
        for symbol_table in symbol_tables:
            # For each symbol table, iterate through the symbols inside
            for nsym, symbol in enumerate(symbol_table.iter_symbols()):
                # See if we have a match of names
                if symbol.name == symbol_name:
                    debug("Found it")
                    # Save the information about the symbol
                    symbols[symbol_name] = {}
                    symbols[symbol_name]['addr'] = symbol['st_value']
                    symbols[symbol_name]['size'] = symbol['st_size']
                    found = True
                    break
            # If we found it, bail
            if found:
                break
        # If we didn't find a symbol, save a None entry
        if not found:
            debug("Not found")
            symbols[symbol_name] = None
                                                       
    # Store the symbol dictionary in the return structure
    elfinfo['symbols'] = symbols

    return elfinfo

