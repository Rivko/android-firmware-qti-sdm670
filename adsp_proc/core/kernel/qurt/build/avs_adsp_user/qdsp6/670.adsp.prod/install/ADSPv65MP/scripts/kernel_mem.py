#=============================================================================
#
#                                    kernel_mem.py
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
#!/pkg/python/2.4.3/bin/python

#!/usr/bin/env python

import sys
import os
from operator import itemgetter
from optparse import OptionParser

#python kernel_mem.py kernel_lib_file

print "Usage: python kernel_mem.py -l libqurtkernel.a"

def print_mem_size(lib_file):
    print "========================================================"
    section_dict = {'.text':0, 
                    '.data':0,
                    '.bss':0,
                    '.sdata':0,
                    '.sbss':0,
                    '.rodata':0,
                    'QURTK.SCHEDULER.text':0,
                    'QURTK.INTERRUPT.data':0,
                    'QURTK.SCHEDULER.data':0,
                    'QURTK.CONTEXTS.data':0,
                    'QURTK.CONTEXTS.TCM.data':0,
                    'QURTK.FUTEX.data':0,
                    'QURTK.TRACEBUFFER.data':0
                    }

    #create dump file of the library
    os.system("qdsp6-objdump -h %s >ttmmpp.txt"%lib_file)
    f = open("ttmmpp.txt", "r");
    lines = f.readlines()
    f.close()
    os.system("rm -f ttmmpp.txt")

    for line in lines:
        list = line.split()
        if len(list) > 3:
            if section_dict.has_key(list[1]):
                section_dict[list[1]] += int(list[2], 16)

    items = sorted(section_dict.iteritems(), key=itemgetter(1),reverse=True)
    total = 0
    for item in items:
        print item[0],":","%x"%item[1]
        total += item[1]

    print "Total Kernel memory usage: 0x%x (%d) bytes"%(total, int(total))

def main():
    parser = OptionParser()
    parser.add_option("-l", "--lib", dest="lib_file", help="Kernel libary file, REQUIRED")

    (options, args) = parser.parse_args()
    #print options

    if options.lib_file == None:
        parser.print_help()
    else:
        print_mem_size(options.lib_file) 


if __name__ == "__main__":
    main()
                    
