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
        self.split_line_section = False
        self.split_line_symbol = False
        self.curr_section = None
        self.curr_section_name = ''
        self.curr_symbol = None

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
		# look for section header ,support for QSEE.map
        m = re.search('^([0-9a-zA-Z_]+)' + \
                      '(\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+))?\s*$',
					  line)  
        if m:
            if m.group(2) != None:
                section = MapParser.Section(m.group(1), 
                                            int(m.group(3),0), 
                                            int(m.group(4),0))
                scanner.curr_section = section
                #return (section, None) 
            else:
                scanner.curr_section_name = m.group(1)
                scanner.split_line_section = True
            continue

        # handle split line header
        if scanner.split_line_section:
            m = re.search('^\s+(0x[0-9a-fA-F]*)\s+(0x[0-9a-fA-F]+)\s*$', line)
            scanner.split_line_section = False
            if m:
                section = MapParser.Section(scanner.curr_section_name, 
                                            int(m.group(1),0), 
                                            int(m.group(2),0))
                scanner.curr_section = section
                #return (section, None)
            continue

        # look for symbol
        m = re.search('^\s?\\(?(\\.?[0-9a-zA-Z_\.]+)(\s+[0-9a-zA-Z_.]+)?(\s+[a-zA-Z_]+\s?\\))?' + \
                      '(\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+.*?([a-zA-Z_0-9]+.lib)+\\((.*)\\))?\s*$', 
                      line)
        if m and scanner.curr_section != None :
            if re.search('^debug_', m.group(1)):
              continue		
            if m.group(4) != None:        
                symbol = MapParser.Symbol(int(m.group(5),0), 
                                          int(m.group(6),0), 
                                          m.group(8), 
                                          m.group(7), 
                                          extract_segment(m.group(1)),
                                          m.group(1))
                #return (scanner.curr_section, symbol)
                scanner.split_line_symbol = False
                return symbol
            elif not "0x" in m.group(1):
                scanner.curr_symbol = m.group(1)
                scanner.split_line_symbol = True
                continue

        # handle split line symbol
        if scanner.split_line_symbol:
            m = re.search('^\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+.*?' + \
                          '([a-zA-Z_0-9]+.lib)+\\((.*)\\)\s*$', 
                          line)
            #scanner.split_line_symbol = False
            if m:
                symbol = MapParser.Symbol(int(m.group(1),0), 
                                          int(m.group(2),0), 
                                          m.group(4), 
                                          m.group(3),
                                          extract_segment(scanner.curr_symbol),
                                          scanner.curr_symbol)
                #return (scanner.curr_section, symbol)
                return symbol
            continue

        # end section on empty line
        m = re.search('^$', line)
        if m:
            scanner.split_line_section = False
            scanner.split_line_symbol = False
            scanner.curr_section = None
            scanner.curr_section_name = ''
            scanner.curr_symbol = None
            

        # clear split line flags if no matches
        scanner.split_line_section = False
        scanner.split_line_symbol = False

    # indicate done scanning
    #return (None, None)
    return None

################################################################################
section_map = {
    'text' : 'CODE',
	'code' : 'CODE',
    'data' : 'RW',
    'bss' : 'ZI',
    'rodata' : 'RO',
    'COMMON' : 'ZI',
}

################################################################################
# extract_segment
################################################################################
def extract_segment(name):
    # find segment
    m = re.search('^\\.([^\\.]+)', name)
    if m:
        name = m.group(1)
    
    # check that segment is defined
    if name in section_map:
        return section_map[name]

    else:
        return "OTHER"

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
        self.in_sections = False
        self.brace_depth = 0
        self.in_section = False
        self.section_types = []

################################################################################
# get_next_ld_token
################################################################################
def get_next_ld_token(scanner):

    #for line in scanner.fh:
        # look for sections
    #    m = re.search('SECTIONS', line)
    #    if m:
    #        scanner.in_sections = True

        # look for braces
    #    m = re.search('{', line)
    #    if m:
    #        scanner.brace_depth += 1

     #   m = res.search('}', line)
     #   if m:
   #         scanner.brace_depth -= 1

        # check to see if in section
    #    if (scanner.in_sections and scanner.brace_depth == 2):
     #       scanner.in_section = True

        # look for entries in section
      #  if scanner.in_section:
       #     # look for named section
        #    m = re.search('\(([A-Z_]+)\)', line)
         #   if m:
                
        

    return None
