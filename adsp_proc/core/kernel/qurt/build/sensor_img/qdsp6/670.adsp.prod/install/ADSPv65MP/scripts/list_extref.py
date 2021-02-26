#=============================================================================
#
#                                    list_extref.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
import os, sys, re, subprocess, optparse

#globals
libs=[]
undefdict = dict()
defdict_obj = dict()
defdict_arch = dict()
defdict_sect = dict() 
defdict_arch_obj = dict()   
size_obj = dict()
size_sym = dict()
size_arch = dict()
size_data = dict() #(.data .data.*), (.sdata .sdata.*)
size_text = dict() #(.text .text.*)
size_bss = dict() #(.bss .bss.* ) (.sbss .sbss.*)
size_common = dict() #(COMMON) (.scommon .scommon.*)
size_rodata = dict() #(.rodata .rodata.*)
size_other = dict() # Any unaccounted section
symbol_list=[]
csv_list = []

start_addr=0
end_addr=0
Total_size=0
elf_symbol_list=[]
isymbol_list=[]
diff_size=0
section_align_size=0
Symbol_align=0
total_section_size=0
unused_lib_size=[]
aliased_symbol_size=0
pd_summary = ""

#CompareSymbols compares each symbol it got from island_analysis.txt and compares it with the symbol list from elf
def CompareSymbols():
    global elf_symbol_list
    global diff_size
    global unused_lib_size
    prev_lib=''
    prev_size=0
    current_size=0
    lib_size=0
    count=0
    elf_symbol_list=dict(elf_symbol_list)
    print'##############################################################'
    print'                  UNUSED SYMBOLS                              '
    print'##############################################################'
    for isym in symbol_list:
        if (isym[0] not in elf_symbol_list):
            print [isym[0],isym[1],isym[2]]
            diff_size+=isym[1]
            current_size = isym[1]
            if(prev_lib == isym[2]):
                lib_size = lib_size + current_size
            else:
                if count > 0 :
                    unused_lib_size.append([prev_lib,lib_size])
                    lib_size=0
                lib_size = current_size
            count=1
            prev_lib = isym[2]
            prev_size = current_size
    unused_lib_size.append([prev_lib,lib_size])            #for last lib
    print '\n'
    print "Total size of unused symbols: %u" % (diff_size)
    print '\n'

    unused_lib_size.sort(key=lambda x: x[1],reverse=1) #sort by size
    print "*************** Unused symbols per Lib ***************"
    for lib in unused_lib_size:
        if re.search(r'(\w+[.]\w+)', lib[0]) is not None:
            m = re.search(r'(\w+[.]\w+)', lib[0])
            lib[0] = m.group(1)
        print  "Unused symbols in \"%s\": %d bytes" % (lib[0],lib[1])
    print '\n'


# ParseElf is used to extract start and end addresses of island section
# It basically looks for anything ending with island and stores the first and last address
# It also extracts all the symbols from island section
def ParseElf(elfout):
    global start_addr
    global end_addr
    global elf_symbol_list
    global Total_size
    global aliased_symbol_size
    global section_align_size
    global total_section_size
    global Symbol_align
    section_addresses = []
    in_island_sections = 0
    prev_addr=0
    prev_size=0
    current_addr=0
    prev_name=''
    total_sec_symbol_size=0
    sec_symbol_list=[]
    
    for line in elfout.splitlines():
        secpattern = re.compile('\s{2}(.{4})\s+(.*\.\S*)+\s*(.*)\s+\s{7}(\S{8})\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s+(\S*)\s')
        sympattern = re.compile('\s*(.*):\s+(\S{8})\s+\s*(\S*)\s+(\S*)\s+\s*(\S*)\s+\s*(\S*)\s+\s*(\S*)\s+(\S*)')

        secmatch = secpattern.match(line)
        symmatch = sympattern.match(line)
        if(secmatch):
            if('island' in secmatch.group(2)):
                if not in_island_sections:
                    start_addr = secmatch.group(4)
                    in_island_sections=1
                    prev_size=int(start_addr,16) #take care of first section
                else:
                    end_addr = int(secmatch.group(4),16)+int(secmatch.group(6),16)
                    end_addr = hex(end_addr)[2:-1]
                if((prev_addr+prev_size) < int(str(secmatch.group(4)),16)):
                    section_align_size += int(str(secmatch.group(4)),16) - (prev_addr+prev_size)
                prev_addr = int(secmatch.group(4),16)
                prev_size = int(secmatch.group(6),16)
                total_section_size += int(str(secmatch.group(6)),16)
                #store all the section addresses in form - Section start address, section end address, section size
                section_addresses.append([secmatch.group(4),hex(int(secmatch.group(4),16)+int(secmatch.group(6),16))[2:-1],int(secmatch.group(6),16)]) 
        elif(symmatch):
            if(('FILE' not in symmatch.group(4)) and ('SECTION' not in symmatch.group(4))):
                 if((symmatch.group(2) >= start_addr) and (symmatch.group(2) < end_addr)):
                     #store all the island symbols in form - Symbol name, symbol address, symbol size.
                     elf_symbol_list.append((symmatch.group(8),(symmatch.group(2),symmatch.group(3))))
                     if symmatch.group(3).startswith('0x'):
                         Total_size+=int(str(symmatch.group(3)),16)
                     else:
                         Total_size+=int(str(symmatch.group(3)))
    elf_symbol_list.sort(key=lambda x: x[1]) #sort by address
    print "#############################################"
    print "             Aliased Symbols                 "
    print "#############################################"
    for sym in elf_symbol_list:
        current_addr = sym[1][0]
        if(current_addr == prev_addr):
            Total_size -= int(sym[1][1],10)  #adjust the total size by substracting aliazed island symbol size
            if(prev_name == sym[0]): #This is to take care of symbol names that repeat in symbol table.These dont seem to be aliased symbols e.g. qurt_mutex_unlock
                prev_name = sym[0]
                continue
            aliased_symbol_size += int(sym[1][1],10)
            print sym[0]+' '+sym[1][1]  #prinout the aliased symbol name and size
        prev_addr = current_addr
        prev_name = sym[0]
    print '\n'
    print "Total Aliased symbol size: %u" % (aliased_symbol_size)
    print '\n'

    # We can have symbol alignments which can cause the total section size to be higher than sumation of individual symbol sizes within that section.
    # Below code will find out the symbol alignments per section and addup the total alignment for all island sections
    for section in section_addresses:
        Get_SymbolAlignment(section[0],section[1],section[2])

# Get_SymbolAlignment gets the symbol alignments per section and adds it up to give total symbol alignments for all island sections
def Get_SymbolAlignment(start, end, sec_size):
    global Symbol_align
    sec_symbol_list = []
    total_sec_symbol_size = 0
    current_addr = 0
    prev_addr = 0
    prev_name = ''

    #We already have the island symbol list. Use that to generate a list of symbols within the section. Start and end addresses for section are passed to this function
    for sym in elf_symbol_list:
         if((sym[1][0] >= start) and (sym[1][0] < end)):
            sec_symbol_list.append(sym)
            if sym[1][1].startswith('0x'):
                total_sec_symbol_size+=int(str(sym[1][1]),16)
            else:
                total_sec_symbol_size+=int(str(sym[1][1]),10)

    # Now we have a list of symbols for passed section. Find out the aliased symbols, count those out.
    for sym in sec_symbol_list:
        current_addr = sym[1][0]
        if(current_addr == prev_addr):
            if sym[1][1].startswith('0x'):
                total_sec_symbol_size -= int(str(sym[1][1]),16)
            else:
                total_sec_symbol_size -= int(str(sym[1][1]),10)
            if(prev_name == sym[0]): #This is to take care of symbol names that repeat in symbol table.These dont seem to be aliased symbols e.g. qurt_mutex_unlock
                prev_name = sym[0]
                continue
        prev_addr = current_addr
        prev_name = sym[0]
        
    #after removing aliased symbols from section, the total size of symbols within a section could be less than the actual section size that linker reports
    # the difference between two is due to symbol alignment.
    if(total_sec_symbol_size != sec_size):
        Symbol_align += sec_size-total_sec_symbol_size

#Print out island analysis summary in csv format
def IslandCSVout(csv_out):
    import sys
    global pd_summary
    core_root = 0
    user_PD = 0
    pd_print_message = ""
    tools_root = 0
    root_total = 0
    start = 0
    libs = dict(unused_lib_size)
    discrepancy_total =0
    core_discrepancy = 0
    pd_discrepancy = 0
    tools_discrepancy = 0
    check = 0
    alignment = 0
    alignment += Symbol_align
    tools_discrepancy += aliased_symbol_size
    # Open analysis file.
    try:
        csv_fh=open(csv_out[0], 'w+')
        try:                    
            csv_fh.write('Area,Subsystem,Functionality,Object File,Size(KB),Discrepancy(bytes) \n')
            for line in csv_list:
               check = 0
               if re.search(r'.*_proc[/\\]?([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\].*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line) is not None: 
                  m = re.search(r'.*_proc[/\\]?([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\]([.a-zA-Z0-9_]*?)[/\\].*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line)
                  if m.group(3) == 'build':
                      if(m.group(4) in libs):
                          csv_fh.write(m.group(1) + ',' + m.group(2) + ',' + '--' + ',' + m.group(4) + ',' + m.group(5) + ',' + str(float(libs.get(m.group(4)))/1024) + '\n')
                          check = 1
                      else:
                          csv_fh.write(m.group(1) + ',' + m.group(2) + ',' + '--' + ',' + m.group(4) + ',' + m.group(5) + '\n')                         
                  else:
                     if(m.group(4) in libs):
                         csv_fh.write( m.group(1) + ',' + m.group(2) + ',' + m.group(3) + ',' + m.group(4) + ',' + m.group(5) + ',' + str(float(libs.get(m.group(4)))/1024) + '\n')
                         check = 1
                     else:
                         csv_fh.write( m.group(1) + ',' + m.group(2) + ',' + m.group(3) + ',' + m.group(4) + ',' + m.group(5) + '\n')
                  if m.group(1) == 'core':
                     try:               
                         core_root += float(m.group(5))
                         if check:
                             core_discrepancy += float(libs.get(m.group(4)))
                     except ValueError:
                         csv_fh.write("Could not convert data to an float. \n")
                  else:
                      try:
                          pd_print_message = str(m.group(1))
                          user_PD += float(m.group(5))
                          if check:
                              pd_discrepancy += float(libs.get(m.group(4)))
                      except ValueError:
                          csv_fh.write("Could not convert data to an float. \n")
               if re.search(r'.*Tools.*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line) is not None: 
                  m = re.search(r'.*Tools.*[/\\]([.a-zA-Z0-9_]*?): ([.0-9]*?) KB', line)
                  if(m.group(1) in libs):
                      csv_fh.write('Hexagon Tools,--,--,'+m.group(1)+','+ m.group(2) + ',' + str(float(libs.get(m.group(1)))/1024) + '\n')
                      check = 1
                  else:
                      csv_fh.write('Hexagon Tools,--,--,'+m.group(1)+','+ m.group(2) + '\n')
                  try:
                      tools_root += float(m.group(2))
                      if check:
                          tools_discrepancy += float(libs.get(m.group(1)))
                  except ValueError:
                      csv_fh.write("Could not convert data to float. \n")
               if re.search(r'Total: ([.0-9]*?) KB',line) is not None:
                  m = re.search(r'Total: ([.0-9]*?) KB', line)
                  try:
                      root_total =float(m.group(1))
                  except ValueError:
                      csv_fh.write("Could not convert data to an float. \n")
            core_discrepancy = float(format(core_discrepancy/1024, '.2f'))
            pd_discrepancy = float(format(pd_discrepancy/1024, '.2f'))
            tools_discrepancy = float(format(tools_discrepancy/1024, '.2f'))
            alignment = float(format(alignment/1024, '.2f'))
            if user_PD > 0:
                pd_summary += 'Island ' + pd_print_message +  ' PD Summary:' + '\n'
            else:
                pd_summary += 'Island root PD Summary:' + '\n'
            csv_fh.write(',,,CoreBSP,' + str(core_root) + ',' +  str(core_discrepancy) + '\n')
            pd_summary += 'CoreBSP:' + str(core_root - core_discrepancy) + '\n'
            csv_fh.write(',,,' + pd_print_message + ',' + str(user_PD) + ',' +  str(pd_discrepancy) + '\n')
            if pd_print_message is not None:
                pd_summary += pd_print_message + ':' + str(user_PD - pd_discrepancy) + '\n'
            csv_fh.write(',,,Hexagon Tools,' +str(tools_root) + ',' +  str(tools_discrepancy) + '\n')
            pd_summary += 'Hexagon Tools:' + str(tools_root - tools_discrepancy) + '\n'
            csv_fh.write(',,,Total,' + str(core_root + user_PD + tools_root) + ',' +  str(core_discrepancy + pd_discrepancy + tools_discrepancy) + '\n')
            csv_fh.write(',,,Alignment,' + ',' + ',' +  str(alignment) + '\n')
            csv_fh.write(',,,Total Adjusted size,' + str(core_root + user_PD + tools_root + alignment/1024 - (core_discrepancy + pd_discrepancy + tools_discrepancy)) + '\n')
            pd_summary += 'Total Adjusted size:' + str(core_root + user_PD + tools_root + alignment - (core_discrepancy + pd_discrepancy + tools_discrepancy)) + '\n'
        finally:
            csv_fh.close()
    except IOError:
        print "The file does not exist, exiting gracefully"

def generate_discrepancy_report(file , csv_out, exestr):
    #extract symbols and sections from elf
    command=[exestr, "-s", "-W", "-S", str(file[0])]
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    elfout, err = p.communicate()
    if p.returncode != 0:
        raise RuntimeError("%r failed, status code %s stdout %r stderr %r" % (command, p.returncode, Symbols, err))
    ParseElf(elfout)
    if symbol_list:
        CompareSymbols()

    if csv_out:
        IslandCSVout(csv_out)
    print "#############################################"
    print "             Summary                 "
    print "#############################################"
    print '\n'
    print "Data based on section headers:"
    print "Total Island size: %u" % total_section_size
    print '\n'
    print "Data based on symbol table (should be close to section header):"
    print "Total Island symbol sizes in ELF (A): %u" % (Total_size)#+section_align_size)
    print "Symbol Alignment (B): %u" % Symbol_align
    print "Total size (A+B) = %u" % (Total_size+Symbol_align)
    print '\n'


def parseArchive(archive_str, archive):
    global symbol_list
    #get archive match, save archive
    archpattern = re.compile('^(.*):\s')
    archmatch = archpattern.match(archive_str)

    object = None
    current_section = ""
    size_arch[archive] = 0
    size_data[archive] = 0 #(.data .data.*), (.sdata .sdata.*)
    size_text[archive] = 0 #(.text .text.*)
    size_bss[archive] = 0 #(.bss .bss.*) (.sbss .sbss.*)
    size_common[archive] = 0 #(COMMON) (.scommon .scommon.*)
    size_rodata[archive] = 0 #(.rodata .rodata.*)
    size_other[archive] = 0 # Any unaccounted section

    for archline in archive_str.split("\n"):
        # Strip out ".hidden" modifiers
        archline = archline.replace(' .hidden','')
        # handle path and parenthesis from object file string 
        archline = archline.replace('(','/',1)
        archline = archline.replace(')','',1)
        archline = os.path.basename(archline)
        objpattern = re.compile('^(.*\.[a-zA-Z]*):\s')
        sympattern = re.compile('(\S{8}){1}\s+(\S*\s+\S*)\s+?(\S+){1}\s+(\S{8}){1}\s+(\S+){1}\s*')
        objmatch = objpattern.match(archline)
        symmatch = sympattern.match(archline)
        if objmatch:
            current_section = ""
            object = objmatch.group(1)
            size_obj[object] = 0
            size_data[object] = 0 #(.data* .sdata*)
            size_text[object] = 0 #(.text*)
            size_bss[object] = 0 #(.bss .bss.*) (.sbss .sbss.*)
            size_common[object] = 0 #(COMMON) (.scommon .scommon.*)
            size_rodata[object] = 0 #(.rodata*)
            size_other[object] = 0 # Any unaccounted section
        elif symmatch and object:
            if symmatch.group(3) != "*UND*":
                current_section = symmatch.group(3)
            if (((str(archive) in libs) or 
                (str(archive)+":"+object in libs) or 
                (str(archive)+":"+object+"("+current_section+")" in libs) or
                (object+"("+current_section+")" in libs) or
                (str(archive)+"("+current_section+")" in libs) or
                (object in libs) or
                ("("+symmatch.group(3)+")" in libs))):
                if ((symmatch.group(5) != symmatch.group(3)) and ("df" not in str(symmatch.group(2)))):
                    if ((symmatch.group(3) == "*UND*")):
                        referencers = set()
                        objstr = object
                        if archive:
                            objstr = archive + ":" + object
                        if undefdict.has_key(symmatch.group(5)):
                            referencers = set(undefdict[symmatch.group(5)])
                        referencers.add(objstr)
                        undefdict[symmatch.group(5)]=referencers
                    else:
                        defdict_obj[symmatch.group(5)]=object
                        defdict_sect[symmatch.group(5)]=current_section
                        if archive:
                            defdict_arch[symmatch.group(5)]=archive
                            
                            if archive not in defdict_arch_obj:
                                defdict_arch_obj[archive] = list()
                            if object not in defdict_arch_obj.get(archive):
                                defdict_arch_obj[archive].append(object)
                            
                            if (('.data' in current_section)or('.sdata' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_data[archive] = size_data[archive] + size_of_symbol
                            elif('.text' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_text[archive] = size_text[archive] + size_of_symbol
                            elif(('.bss' in current_section)or('.sbss' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_bss[archive] = size_bss[archive] + size_of_symbol
                            elif('COM' in current_section):
                                size_of_symbol = int(symmatch.group(1).split()[0],16)
                                size_common[archive] = size_common[archive] + size_of_symbol
                            elif('.rodata' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_rodata[archive] = size_rodata[archive] + size_of_symbol
                            else:
                                size_of_symbol = int(symmatch.group(4),16)
                                size_other[archive] = size_other[archive] + size_of_symbol

                            size_arch[archive] = size_arch[archive] + size_of_symbol
                        else:
                            if (('.data' in current_section)or('.sdata' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_data[object] = size_data[object] + size_of_symbol
                            elif('.text' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_text[object] = size_text[object] + size_of_symbol
                            elif(('.bss' in current_section)or('.sbss' in current_section)):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_bss[object] = size_bss[object] + size_of_symbol
                            elif('COM' in current_section):
                                size_of_symbol = int(symmatch.group(1).split()[0],16)
                                size_common[object] = size_common[object] + size_of_symbol
                            elif('.rodata' in current_section):
                                size_of_symbol = int(symmatch.group(4),16)
                                size_rodata[object] = size_rodata[object] + size_of_symbol
                            else:
                                size_of_symbol = int(symmatch.group(4),16)
                                size_other[object] = size_other[object] + size_of_symbol

                            size_obj[object] = size_obj[object] + size_of_symbol
                        size_sym[symmatch.group(5)] = size_of_symbol

# Main function that prints object references
if __name__ == "__main__":
    libname = ''
    usage = "usage: %prog [options]\nExample:\npython scripts\\footprint\\%prog -l ..\\install\\ADSPv5MP\\lib -l ..\\build\\ADSPv5MP\\libs\\qurt\\src -l ..\\build\\ADSPv5MP\\kernel\\asm -l build\\ADSPv5MP\\qurt_island_t1 -l ..\\build\\ADSPv5MP\\libs\\timer\\asm -l ..\\build\\ADSPv5MP\\libs\\qurt\\asm -l C:\\Qualcomm\\HEXAGON_Tools\\5.0.07\\dinkumware\\lib -s island.ispec -i island_ignore.txt -a island_api.txt"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('-s', '--spec', dest="spec", action="append", help="spec file to identify libs and objects")
    parser.add_option('-o', '--libs', dest="libs", action="append", default=[], help="specify an object or library")
    parser.add_option('-l', '--location', dest="locations", action="append", default=[], help="location to to find objects and libraries from command line and spec file (optional)")
    parser.add_option('-i', '--ignore', dest="ignore", action="append", default=[], help="symbols to ignore, or file with symbols to ignore (1 per line)")
    parser.add_option('-f', '--feedback', dest="feedback", action="store", default="", help="generate files for feedback, defined.txt and undefined.txt in the directory specified")
    parser.add_option('-t', '--tools', dest="toolsver", action="store", default="5.1.04", help="hexagon version number to find objdump (optional)")
    parser.add_option('-a', '--api', dest="api", action="append", help="list of API that should be defined in Objects and libraries that are specified (optional)")
    parser.add_option('-v', '--verbose', dest="verbose", action="store_true", default=False, help="print everything (optional)")
    parser.add_option('-e', '--executable', dest="exestr", action="store", default="", help="explicit objdump executable")
    parser.add_option('-d', '--debug', dest="debug", action="store_true", default=False, help="experimental")
    parser.add_option('-x', '--elf', dest="elffile", action="append", default=[], help="path for elf")
    parser.add_option('-c', '--csv', dest="csvfile", action="append", default=[], help="Location to save csv with libs summary")
    (options, args) = parser.parse_args(sys.argv)

    #setup parameters
    ispec = options.spec
    libs = options.libs
    locations = options.locations
    ignore = options.ignore
    default_tools_version = options.toolsver
    feedback_dir = options.feedback
    api = options.api
    verbose = options.verbose
    exestr = options.exestr
    debug = options.debug
    elffile = options.elffile
    csvfile = options.csvfile
    ret_val = 0
    #where to find objdump
    if exestr == "":
        if os.name == 'posix':
            default_tools_root = os.path.join('/pkg/qct/software/hexagon/releases/tools',default_tools_version)
        elif os.name == 'nt':
            default_tools_root = os.path.join(os.path.realpath('C:/Qualcomm/HEXAGON_Tools'),default_tools_version)
        if int(default_tools_version.split(".")[0]) >= 7:
            exestr = os.path.join(default_tools_root,"Tools/bin/hexagon-llvm-objdump")
        else:
            exestr = os.path.join(default_tools_root,"gnu/bin/hexagon-objdump")
    else:
        exestr = os.path.realpath(exestr)

    import lib.qurt
    trial_link = lib.qurt.Elf32TrialLink()

    #read in any and all ispec files, add libs to libs list
    if ispec:
        for specfile in ispec:
            if os.path.isfile(specfile):
                f = file(specfile) 
                for line in f: 
                    trial_link.add_ispec(line)
                    comment_pattern = re.compile('^#')
                    lib_pattern = re.compile('\**(.*)(\s+)\**(.*)(\s+)\**(.*)\s*')
                    line = line.strip() 
                    comment_match = comment_pattern.match(line)
                    lib_match = lib_pattern.match(line)
                    if comment_match:
                        continue
                    elif lib_match:
                        libstr = ""
                        if lib_match.group(1) != '':
                            if lib_match.group(3) != '':
                                libstr = lib_match.group(1)+':'+lib_match.group(3)
                            else:
                                libstr = lib_match.group(1)
                        elif lib_match.group(3) != '':
                            libstr = lib_match.group(3)
                        if lib_match.group(5) != '':
                            libstr += "("+lib_match.group(5)+")"
                        libs.append(libstr)
                f.close()
            else:
                print "ispec file does not exist: "+specfile

    if len(libs) < 1:
        print "No Libs or Objects have been specified."
        exit(-1)

    #if api list specified, read in list of API that needs to be supported in island
    apis = set()
    if api:
        for apifile in api:
            if os.path.isfile(apifile):
                f = file(apifile) 
                for line in f: 
                    apis.add(line.strip())
                f.close() 
            else:
                apis.add(apifile.strip())

    #if ignore list is specified, read symbols which will be defined to be ignored.
    if ignore:
        for ignorefile in ignore:
            if os.path.isfile(ignorefile):
                f = file(ignorefile) 
                for line in f: 
                    defdict_obj[line.strip()]="ignorelist"
                f.close() 
            else:
                defdict_obj[ignorefile]="ignorelist"

    i=0
    libfiles=set()
    while(i<len(libs)):
        sectiontemp = libs[i].split('(',1)
        section=None
        if len(sectiontemp)>1:
            section=sectiontemp[1]
        temp = sectiontemp[0].split(":")
        obj=''
        #if first is a drive letter
        if len(temp[0]) == 1:
            file = temp[0]+":"+temp[1]
            if len(temp) > 2:
                obj = temp[2]
        else:
            file = temp[0]
            if len(temp) > 1:
                obj = temp[1]
        #find path
        if not os.path.isfile(file):
            for loc in locations:
                found = False
                fullpathlib = os.path.join(loc,file)
                if os.path.isfile(fullpathlib):
                    if obj:
                        libs[i]=fullpathlib+":"+obj
                    else:
                        libs[i]=fullpathlib
                    libfiles.add(fullpathlib)
                    found = True
                    break
            #if not found:
                #print "File could not be found: "+file
        else:
            libfiles.add(file)
        if section:
            libs[i] += "("+section
        i+=1

    libfiles = set(libfiles)
    for libfile in libfiles:
        for libname, objname, fi in lib.qurt.QurtElfContainer(libfile):
            trial_link.add_object(lib.qurt.Elf32Object(fi, libname=libname, objname=objname))

        command=[exestr, "-t", libfile]
        p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, err = p.communicate()
        parseArchive(out, libfile);

    #trial_link.run()

    undefined = set(undefdict.keys())
    defined = set(defdict_obj.keys())

    if verbose:
        print "################################################################################"
        print "# Defined Symbols                                                              #"
        print "################################################################################"
        
        for sym in sorted(defined):
            callers_str = "Caller:"
            if undefdict.has_key(sym):
                first = True
                for obj in undefdict[sym]:
                    if first:
                        callers_str += "\t"+obj
                        first = False
                    else:
                        callers_str += "\n\t"+obj
            else:
                callers_str += " Local or None"
            symsize = 0
            if sym in size_sym:
                symsize = size_sym[sym]
            print "Symbol:\t"+sym + " (" + str(symsize) +"bytes)"
            def_str = "Defined: "
            if sym in defdict_arch:
                def_str = def_str + defdict_arch[sym]
                libname = os.path.split(defdict_arch[sym])
                symbol_list.append([sym,symsize,libname[1]])
            print def_str + ":" + defdict_obj[sym]
            print callers_str
            print "--------------------------------------------------------------------------------"
        print "\nTotal: "+str(len(defined))+"\n"

        objset = set(defdict_obj.values())
        for file in sorted(size_arch, key=size_arch.get, reverse=True):
            print "\n%-69.69s: %.2f KB" % (str(file)[-69:], float(size_arch[file])/1024)
            if file in defdict_arch_obj:
                #for obj in defdict_arch_obj[file]:
                for obj in sorted(defdict_arch_obj[file]):
                    if obj in objset:
                        print "    " + obj + ":"
                        for sym in defdict_obj:
                            if defdict_obj[sym] == obj:
                                print "        %-56.56s: %s bytes" % (sym[-56:], str(size_sym[sym]))

    if api:
        print "################################################################################"
        print "# Unsupported API                                                              #"
        print "################################################################################"
        for sym in (apis - defined):
            print sym
        print "\nTotal: "+str(len(apis - defined))

        objects=[]
        for sym in apis:
            symobj="*"
            if sym in defdict_obj:
                symobj=defdict_obj[sym]
            symarch="*"
            if sym in defdict_arch:
                symarch=defdict_arch[sym]
            symsect="*"
            if sym in defdict_sect:
                symsect=defdict_sect[sym]
            objstr = symarch+" "+symobj+" "+symsect
            if objstr not in objects:
                if sym in '_Dint.1.1_i16_0' and symobj in 'xrint.o':
                    continue
                else:
                    objects.append(objstr)

        appended = True
        while appended:
            appended = False
            for objstr in objects:
                temp=[]
                obj = objstr.split(" ")[1]
                for sym in undefdict:
                    #if obj in undefdict[sym]:
                    if [s for s in undefdict[sym] if obj == re.split('.*\.[a-zA-Z]*:',s)[1]]:
                        temp.append(sym)
                for sym in temp:
                    symobj="*"
                    if sym in defdict_obj:
                        symobj=defdict_obj[sym]
                    symarch="*"
                    if sym in defdict_arch:
                        symarch=defdict_arch[sym]
                    symsect="*"
                    if sym in defdict_sect:
                        symsect=defdict_sect[sym]
                    objstr = symarch+" "+symobj+" "+symsect
                    if objstr not in objects:
                        objects.append(objstr)
                        appended = True

        print "required: "
        for obj in sorted(objects):
            print obj
            
    if feedback_dir != "":
        def_out = os.path.join(feedback_dir,"defined.txt")
        undef_out = os.path.join(feedback_dir,"undefined.txt")
        def_fh=open(def_out, 'w+')
        undef_fh=open(undef_out, 'w+')
        for sym in (defined):
            def_fh.write(sym+"\n")
        for sym in (undefined - defined):
            undef_fh.write(sym+"\n")

    undef_total=len(undefined - defined)
    if undef_total > 0:
        print "################################################################################"
        print "# Undefined Island Symbols                                                     #"
        print "################################################################################"
        for sym in (undefined - defined):
            callers_str = "Caller:"
            first = True
            for obj in undefdict[sym]:
                if first:
                    callers_str += "\t"+obj
                    first = False
                else:
                    callers_str += "\n\t"+obj
            print "Symbol:\t"+sym
            print callers_str
            print "--------------------------------------------------------------------------------"
        print "\nTotal Undefined: "+str(undef_total)+"\n"

    print "################################################################################"
    print "# Island Footprint                                                             #"
    print "################################################################################"
    total=0
    all_sizes = size_arch
#    for obj in sorted(size_obj, key=size_obj.get, reverse=True):
#        if os.path.isfile(obj):
#            all_sizes[obj] = size_obj[obj]
    for file in sorted(all_sizes, key=all_sizes.get, reverse=True):
        total+=all_sizes[file]
        print "\n\n"
        print "%s: %.2f KB" % (file, float(all_sizes[file])/1024)
        csv_list.append("%s: %.2f KB \n" % (file, float(all_sizes[file])/1024))
        print "%   -56.56s: %.2f KB" % (str("(.data* .sdata*)")[-56:], float(size_data[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(.text*)")[-56:], float(size_text[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(.bss* .sbss* )")[-56:], float( size_bss[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(COMMON .scommon*)")[-56:], float( size_common[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(.rodata*)")[-56:], float(size_rodata[file])/1024)
        print "%   -56.56s: %.2f KB" % (str("(other sections)")[-56:], float(size_other[file])/1024)

    print "\nTotal: %.2f KB\n" % (float(total)/1024)

    symbol_list.sort(key=lambda x: x[2]) #sort by libnames
    if elffile:
        if exestr == "":
            if os.name == 'posix':
                default_tools_root = os.path.join('/pkg/qct/software/hexagon/releases/tools',default_tools_version)
            elif os.name == 'nt':
                default_tools_root = os.path.join(os.path.realpath('C:/Qualcomm/HEXAGON_Tools'),default_tools_version)
            exestr = os.path.join(default_tools_root,"gnu/bin/hexagon-readelf")
        else:
            exestr = os.path.realpath(exestr)
        exestr = exestr.split('hexagon-')
        exestr = os.path.join(exestr[0],'hexagon-readelf')
        generate_discrepancy_report(elffile, csvfile, exestr)

#    We could simply have returned sys.exit(undef_total). This works on windows machine. But on linux machines it doesn't give the actual value. It seems to be
#   passing some garbage value. So get around this, we either return 0 or 1 for windows machine and 0 or 256 for linux machines. 
    if(undef_total > 0):
        sys.exit(1)
    else:
        sys.exit(0)
