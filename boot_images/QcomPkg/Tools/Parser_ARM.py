import re
import MapParser

################################################################################
# MapScannerTracker
################################################################################
class MapScannerTracker:
    """ Tracking info for scanning .map files

        This class is used to keep track of scanning data across calls of
        get_next_map_token.
    """
    def __init__(self, fh):
        """ Inits MapScannerTracker info """
        self.fh = fh
        self.in_mem_map = False

################################################################################
# get_next_map_token
################################################################################
def get_next_map_token(scanner):
    """ Parses next section/symbol from map file

        Scans through the .map file tracked by input MapScannerTracker looking
        for Symbols defined in .map file. Returns the first symbol encountered.

        Args:
            scanner: MapScannerTracker with current tracking info

        Returns:
            Symbol object representing next symbol found. None when EOF is 
            reached
    """
    for line in scanner.fh:
        # look for memory map section
        m = re.search('^Memory Map of the image$', line)
        if m:
            scanner.in_mem_map = True
            continue

        # look for symbols (ish)
        if scanner.in_mem_map:
            m = re.search('((0x)?[0-9A-Fa-f]+)\s+((0x)?[0-9A-Fa-f]+)\s+' + \
                          '([A-Za-z]+)\s+([A-Za-z]+)\s+([0-9]+)\s+(\*\s*)?' + \
                          '([A-Za-z0-9\._]+)\s+([A-Za-z0-9_]+\.lib)', 
                          line)
            if m:
                sym = MapParser.Symbol(int(m.group(1),0), 
                                       int(m.group(3),0), 
                                       "", 
                                       m.group(10),
                                       get_segment(m.group(5), m.group(6)),
                                       m.group(9))
                return sym

    # indicate done scanning
    return None

################################################################################
# get_segment
################################################################################
def get_segment(t, a):
    if t == "Code":
        return "CODE"
    elif t == "Data":
        return a
    elif t == "Zero":
        return "ZI"
    else:
        print "WARNING: Unrecognized type:", a, t
        return ""

################################################################################
# XMLScannerTracker
################################################################################
class XMLScannerTracker:
    """ Tracking info for scanning .xml files

        This class is used to keep track of scanning data across calls of
        get_next_xml_token.
    """
    def __init__(self, fh):
        """ Inits XMLScannerTracker info """
        self.fh = fh
        self.in_hw = False

################################################################################
# get_next_xml_token
################################################################################
def get_next_xml_token(scanner): 
    """ Parses next memory region from xml file

        Scans through the .xml file tracked by input XMLScannerTracker looking
        for hardware memory regions. Returns the first region found.

        Args:
            scanner: XMLScannerTracker with current tracking info

        Returns:
            None if scanner has finished file, otherwise Section containing
            info for next hardware memory regions encountered
    """
    for line in scanner.fh:
        # look for hardware section
        m = re.search('<Hardware>', line)
        if m:
            scanner.in_hw = True
            continue

        # look for hardware section end
        m = re.search('</Hardware>', line)
        if m:
            scanner.in_hw = False
            continue

        # look for bank in hw section
        if scanner.in_hw:
            m = re.search('<Bank name=\"([^\"]+)\" addr=\"([^\"]+)\" ' + \
                          'size=\"([^\"]+)\"/>', 
                          line)
            if m:
                size = m.group(3)
                size = size.replace("K", "* 1024")
                size = size.replace("M", "* 1024 * 1024")
                size = size.replace("G", "* 1024 * 1024 * 1024")
                section = MapParser.Section(m.group(1), 
                                            int(m.group(2),0), 
                                            eval(size))
                return section

    return None

################################################################################
# LDScannerTracker
################################################################################
class LDScannerTracker:
    """ Tracking info for scanning .ld files

        This class is used to keep track of scanning data across calls of
        get_next_ld_token.
    """
    def __init__(self, fh):
        """ Init LDScannerTracker info """
        self.fh = fh

################################################################################
# get_next_ld_token
################################################################################
def get_next_ld_token(scanner):

    #TODO WRITE THIS

    return None
