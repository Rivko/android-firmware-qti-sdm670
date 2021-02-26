from optparse import OptionParser
import MapParser
import sys, re

################################################################################
# main
################################################################################
def main():
    # data containers
    symbols = []    # list of symbols parsed from map file
    regions = {}    # 2-level map of memory region to data segments to symbols
    segments = {}   # 2-level map of data segments to memery regions to symbols
    lib_sizes = {}  # 2-level map of lib name to segment type to size in image

    # handle command line
    usage = "usage: Mapit.py [options] <.map file>"
    parser = OptionParser(usage)

    parser.add_option("--ld", 
                      action="store", type="string", dest="ld_file",
                      help=".ld file to determine named section types from")

    parser.add_option("-x", "--xml",
                      action="store", type="string", dest="xml_file",
                      help=".xml file with HW memory regions")

    parser.add_option("-c", "--compiler",
                      action="store", type="string", dest="compiler",
                      default="GNU",
                      help="Name of compiler used to generate .map file" + \
                           "[ARM, GNU, CLANG]")

    parser.add_option("-r", "--range", action="append",
                      help="Add a memory range of interest. Format=" + \
                            "<start_address>:<size>")

    parser.add_option("-l", "--lib", action="append",
                      help="Add a lib of interest")
                      
    parser.add_option("-t", "--target",
                      action="store", type="string", dest="target",
                      help="Name of the target, e.g. 8953")

    (options, args) = parser.parse_args()

    # check that .map file has been provided
    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    # determine compiler that generated files
    compiler = get_compiler(options.compiler)
    if (compiler == None):
        sys.exit(2)

    # add any ranges added to regions
    if (add_ranges(options.range, regions)):
        sys.exit(3)

    # pull information from .map file
    MapParser.parse_map(args[0], symbols, compiler)

    # if ld file provided, extract named section types
    if (options.ld_file):
        MapParser.parse_ld(options.ld_file, None, compiler)

    # if xml file is provided, extract memory regions
    if (options.xml_file):
        MapParser.parse_xml(options.xml_file, regions, compiler)

    # fill region map with symbols from .map file
    build_maps(symbols, regions, segments, options.lib)

    # calculate data segment sizes for each lib
    calc_lib_sizes(symbols, regions, lib_sizes)
 
    # DEBUG PRINT
    #print_totals(lib_sizes)
    if (options.target) :
      print_summary(options.target, lib_sizes)
    print_libs(lib_sizes)
    print_map(regions, print_region_header)
    print_map(segments, print_segment_header)
   
################################################################################
# get_compiler
################################################################################
def get_compiler(c):
    if (c == "ARM"):
        import Parser_ARM
        return Parser_ARM
    elif c == "GNU":
        import Parser_GNU
        return Parser_GNU
    elif c == "CLANG":
        import Parser_CLANG
        return Parser_CLANG
    else:
        print "Invalid Compiler"
        return None

################################################################################
# add_ranges
################################################################################
def add_ranges(ranges, regions):
    # exit if no ranges defined
    if ranges == None: 
        return 0

    for i, opt in enumerate(ranges):
        m = re.search('^((0x)?[0-9a-fA-F]+):((0x)?[0-9a-fA-F]+)$', opt)
        if m:
            region = MapParser.Section(opt, 
                                       int(m.group(1),0), 
                                       int(m.group(3),0))
            regions[region] = {}
        else:
            print "Invalid range format:", opt
            return -1

    return 0

################################################################################
# calc_lib_sizes
################################################################################
def calc_lib_sizes(symbols, regions, lib_sizes):
    # check each symbol and bin it appropriately
    for symbol in symbols:

        # check if valid
        if len(regions):
            in_range = False
            for region in regions:
                if (symbol.address < (region.address + region.size) and
                    region.address < (symbol.address + symbol.size)):
                        in_range = True

            # skip symbol if it doesnt fall under any memory regions
            if not in_range:
                continue

        if symbol.section not in lib_sizes:
            lib_sizes[symbol.section] = {}
        # init new entry for lib if not existant
        if symbol.lib not in lib_sizes[symbol.section]:
            lib_sizes[symbol.section][symbol.lib] = {}

        # add new array for segment if not existant
        if symbol.segment not in lib_sizes[symbol.section][symbol.lib]:
            lib_sizes[symbol.section][symbol.lib][symbol.segment] = 0

        # update size count to segment
        lib_sizes[symbol.section][symbol.lib][symbol.segment] += symbol.size

################################################################################
# build_maps               
################################################################################
def build_maps(symbols, regions, segments, libs):
    # fill out regions map (region->segment->symbol)
    for symbol in symbols:
        
        # check if in library of interest
        if (libs and symbol.lib not in libs):
            continue
        
        # add symbol to appropriate region and segment
        for region in regions:
            if (symbol.address < (region.address + region.size) and
                region.address < (symbol.address + symbol.size)):
                if symbol.segment in regions[region]:
                    regions[region][symbol.segment].append(symbol)
                else:
                    regions[region][symbol.segment] = [symbol,]
                    segments[symbol.segment] = {}

    # fill out segments map (segment->region->symbol)
    for region in regions:
        for segment in regions[region]:
            for symbol in regions[region][segment]:
                if region not in segments[segment]:
                    segments[segment][region] = []
                segments[segment][region].append(symbol)
     

################################################################################
# print_totals
################################################################################
def print_totals(lib_sizes):
    # calculate totals
    segments = {}
    for lib in lib_sizes:
        for seg in lib_sizes[lib]:

            if seg not in segments:
                segments[seg] = 0

            segments[seg] += lib_sizes[lib][seg]

    # print totals
    print "Memory Usage:"
    for seg in segments:
        print seg, ":", segments[seg]
    print

################################################################################
# print_libs
################################################################################
def print_libs(lib_sizes):
    # total counts
    
    
    # print out each lib
    for sec in lib_sizes:
      print sec
      segments = {
                 "CODE" : 0,
                 "RO" : 0,
                 "RW" : 0,
                 "ZI" : 0,
                 "OTHER" : 0,
               }

      # column width setting
      col_width = 10

      # print header
      for key in sorted(segments):
          print key, column_padding(key, col_width),
      print "Total", column_padding("Total", col_width),
      print  "ROM", column_padding("ROM", col_width), "Library"
      for lib in sorted(lib_sizes[sec]):
          total = 0
          rom = 0
          for seg in sorted(segments):
              if seg in lib_sizes[sec][lib]:
                  total += lib_sizes[sec][lib][seg]
                  segments[seg] += lib_sizes[sec][lib][seg]
                  if (seg == "CODE" or seg == "RO" or seg == "RW"):
                      rom += segments[seg]
                  print lib_sizes[sec][lib][seg], 
                  print column_padding(str(lib_sizes[sec][lib][seg]), col_width),
              else: 
                  print 0, column_padding('0', col_width),
          print total, column_padding(str(total), col_width),
          print rom, column_padding(str(rom), col_width), lib
    
      # print totals
      total = 0
      rom = 0
      for seg in sorted(segments):
          print segments[seg], column_padding(str(segments[seg]), col_width),
          total += segments[seg]
          if (seg == "CODE" or seg == "RO" or seg == "RW"):
              rom += segments[seg]
      print total, column_padding(str(total), col_width), 
      print rom, column_padding(str(rom), col_width), "Overall\n"

################################################################################
# print_summary
################################################################################
def print_summary(filename, lib_sizes):
  col_width = 30
  mem_reg_used = {}
  print "\n---------------------------------------------------------------------------" + \
    "Summary " + \
    "---------------------------------------------------------------------------"
  with open(filename + ".txt") as fh:
    for line in fh:
      m = re.search('([0-9A-Z_]*)\s*(0x[0-9a-fA-F]*)\s*(0x[0-9a-fA-F]*)', line)
      if m :
        mem_reg_name = m.group(1)
        mem_addr_start = long(m.group(2), 0)
        mem_addr_end = long(m.group(3), 0)
        mem_reg_size = mem_addr_end - mem_addr_start
        mem_reg_used[mem_reg_name] = 0
        
        # Print Memory Region Information
        print "Memory Region : " + mem_reg_name, column_padding("Memory Region : " + mem_reg_name, col_width),
        print "Start Addr : " + hex(mem_addr_start), column_padding("Start Addr : " + hex(mem_addr_start), col_width),
        print "Total Size : ", mem_reg_size, column_padding("Total Size : " + hex(mem_addr_end - mem_addr_start), col_width)
        print "*******************************************************************************" + \
          "*******************************************************************************"
        # Print sections in the Memory Region
        print_sec_summary(lib_sizes, mem_addr_start, mem_addr_end, mem_reg_used, m.group(1))
        print "*******************************************************************************" + \
          "*******************************************************************************"
        # print summary of memory region
        print "Memory Used : ", mem_reg_used[mem_reg_name], " Memory Free : ", mem_addr_end - mem_reg_used[mem_reg_name] - mem_addr_start
        print "*******************************************************************************" + \
          "*******************************************************************************\n\n\n"
  print "-------------------------------------------------------------------------------" + \
    "-------------------------------------------------------------------------------"
  

################################################################################
# print_sec_summary
################################################################################
def print_sec_summary(lib_sizes, mem_addr_start, mem_addr_end, mem_reg_used, mem_reg_name):
  col_width = 10
  all_out = 0
  segments = {
                 "CODE" : 0,
                 "RO" : 0,
                 "RW" : 0,
                 "ZI" : 0,
                 "OTHER" : 0,
  }
    
  # print headers
  print "Section", column_padding("Section", 30),
  print "Address", column_padding("Address", col_width),
  print "Size", column_padding("Size", col_width),
  for key in sorted(segments):
    print key, column_padding(key, col_width),
  print "Total", column_padding("Total", col_width)
  
  # For all the sections
  for sec in lib_sizes:
    sec_addr = long(hex(sec.address), 0)
    sec_size = long(hex(sec.size), 0)
    
    if (not((sec_addr > mem_addr_start) and ((sec_addr + sec_size) < (mem_addr_end)))):
      continue;
    
    print sec.name, column_padding(sec.name, 30),
    print hex(sec.address), column_padding(hex(sec.address), col_width),
    print sec.size, column_padding(str(sec.size), col_width),

    # Reset segment sizes
    for key in sorted(segments):
      segments[key] = 0

    # calculate segment size
    for lib in sorted(lib_sizes[sec]):
      for seg in sorted(segments):
        if seg in lib_sizes[sec][lib]:
          segments[seg] += lib_sizes[sec][lib][seg]
          
    # print total size
    total = 0
    for seg in sorted(segments):
        print segments[seg], column_padding(str(segments[seg]), col_width),
        total += segments[seg]
    print total, column_padding(str(total), col_width)
    mem_reg_used[mem_reg_name] += total
    


################################################################################
# column_padding
################################################################################
def column_padding(string, width):
    return " " * (width - len(string)) 

################################################################################
# print_map
################################################################################
def print_map(m, header):
    empty = True
    for key in m:
        if len(m[key]):
            empty = False

    if not empty:
        header()

    for key1 in m:
        if len(m[key1]):
            print key1
            for key2 in m[key1]:
                print "\t", key2
                for sym in m[key1][key2]:
                    print "\t\t", sym
    print

################################################################################
# print_region_header
################################################################################
def print_region_header():
    print "####################################################################"
    print "# Map of selected memory ranges"
    print "# Outer Level: HW Memory Regions"
    print "# Inner Level: Memory Type"
    print "####################################################################"

################################################################################
# print_segment_header
################################################################################
def print_segment_header():
    print "####################################################################"
    print "# Map of selected memory ranges"
    print "# Outer Level: Memory Type"
    print "# Inner Level: HW Memory Regions"
    print "####################################################################"

################################################################################
main()
