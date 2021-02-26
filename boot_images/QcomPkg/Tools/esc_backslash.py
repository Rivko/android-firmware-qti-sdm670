#===========================================================================
#  helper script to escape backslash in list files
# Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#
# when          who     what, where, why 
# --------      ---     ------------------------------------------------------- 
# 2012-10-08    jb      First Release
# ===========================================================================*/

import struct, sys

if __name__ == "__main__":

    if len(sys.argv) < 2:
    	print "Usage: esc_backslash.py filename"
    	sys.exit(1);

    filename = sys.argv[1]
    s=open(filename).read()
    s=s.replace("\\", "\\\\")
    f=open(filename, 'w')
    f.write(s)
    f.flush()
    f.close()

