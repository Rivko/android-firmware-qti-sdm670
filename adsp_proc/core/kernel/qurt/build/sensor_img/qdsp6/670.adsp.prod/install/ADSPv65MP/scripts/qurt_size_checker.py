#!/usr/bin/env python
#=============================================================================
#
#                                    qurt_size_checker.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#
#  qurt_size_checker.py checks that the total size of the PT_LOAD segments
#   in an ELF file is less than or equal to a threshold value.
#

from lib.qurt import *
from optparse import OptionParser
import sys

parser = OptionParser()
parser.add_option('--size', dest='size_threshold', action='store', help='Size threshold')
opts, args = parser.parse_args()

if opts.size_threshold != None:
    threshold = convert_size(opts.size_threshold)
else:
    threshold = None

if len(args) != 1:
    print '%s: should be passed a single ELF file' % (__file__,)
    sys.exit(1)

elfname = args[0]

fi = open(elfname, 'rb')
ehdr = Elf32Header(fi)
if ehdr.e_phnum == 0:
    print '%s: input ELF %s has no program headers' % (__file__, elfname)
    sys.exit(1)
    
fi.seek(ehdr.e_phoff)
phdrs = [Elf32ProgramHeader(fi) for _ in range(ehdr.e_phnum)]

elfsize = 0

print 'Size report: %s' % (elfname,)

for p in phdrs:
    if p.p_type == p.PT_LOAD:
        if p.p_memsz:
            elfsize += p.p_memsz
            print '    Load section of size %10.2fK (%u bytes)' % (p.p_memsz/1024.0, p.p_memsz)

returncode = 0

if elfsize:
    print         '=' * 78
print             '    Total size        is %10.2fK (%u bytes)' % (elfsize/1024.0, elfsize)
if threshold != None:
    print         '    Threshold         is %10.2fK (%u bytes)' % (threshold/1024.0, threshold)
    if elfsize > threshold:
        excess = elfsize - threshold
        print     ''
        print     '*** THRESHOLD EXCEEDED BY %.2fK (%u bytes) ***' % (excess/1024.0, excess)
        print     ''
        returncode = 1

sys.exit(returncode)

# Signatures of the files that this depends on
# 46a31eb420b1999d63d43a1df42e94ca /local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/core/kernel/qurt/build/sensor_img/qdsp6/670.adsp.prod/install/ADSPv65MP/scripts/lib/qurt.py
