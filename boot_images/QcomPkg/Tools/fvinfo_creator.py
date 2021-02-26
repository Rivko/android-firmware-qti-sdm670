#!/usr/bin/python
#============================================================================
#
# fvinfo_creator.py
#
# GENERAL DESCRIPTION
#   Append FVs after the UEFI image is created
#
# USAGE
#   fvinfo_creator.py <source/destination file> <fv1 to be added> <fv2 to be added>..
#
# Copyright 2013 by Qualcomm Technologies Inc, Incorporated. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#============================================================================
import sys
import array
import os

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
ALIGNMENT                = 512
PAD                      = 0xFF
FINAL_ALIGNMENT          = 8192 #8k aligned image

if len(sys.argv) < 2:
    print "Usage: fv_info_creator.py uefi.mbn <fv1> <fv2> .."
    sys.exit(1);

#===========================================================================
# FV info sector format  
#===========================================================================
# magic number (F.V.): uint32
# version            : uint32
# num of FVs         : uint32
#
# FV0 size: 32 bits 
# FV1 size: 32 bits
# ....
#
# Sector aligned FV0 data : user defined length 
# Sector aligned FV1 data : user defined length 
# ....
#
#=============================================================================

# Source/Destination file  
bin_file_name = sys.argv[1]

# Header entry count
count = 2

# FV info header data
data = array.array('L')  # create array of bytes.

# Fill the header structure
data.append(int('462E562E', 16))   # magic number 'F.V.'
data.append(1)                     # version information
data.append(len(sys.argv) - count) # number of FVs

while (count < len(sys.argv)) :
   # Append the size of the FV to be appended
   file_size_in_bytes = os.stat(sys.argv[count]).st_size
   if file_size_in_bytes%ALIGNMENT != 0:
      file_size_in_bytes = file_size_in_bytes + ALIGNMENT - file_size_in_bytes%ALIGNMENT

   data.append(file_size_in_bytes)
   count = count + 1

# Write the array at once to a file
f = file(bin_file_name, "ab")

# Make file sector aligned
# Align to sector boundary if not aligned
file_size_in_bytes = os.stat(bin_file_name).st_size
if file_size_in_bytes%ALIGNMENT != 0:
   f.write (''.rjust(ALIGNMENT - file_size_in_bytes%ALIGNMENT, chr(PAD)))

# Write FV info header to the file
data.tofile(f)

# Make FV info header sector aligned
f.write (''.rjust(ALIGNMENT - 4*(count + 1), chr(PAD)))

# Append FVs to the file
count = 2
while (count < len(sys.argv)) :
   t = file(sys.argv[count], "rb")
   f.write(t.read())
   t.close()
   # Align to sector boundary if not aligned
   file_size_in_bytes = os.stat(sys.argv[count]).st_size
   if file_size_in_bytes%ALIGNMENT != 0:
      f.write (''.rjust(ALIGNMENT - file_size_in_bytes%ALIGNMENT, chr(PAD)))
   count = count + 1   

f.close()
   
# Pad the file to make it FINAL_ALIGNMENT aligned  
file_size_in_bytes = os.stat(bin_file_name).st_size
if file_size_in_bytes%FINAL_ALIGNMENT != 0:
   f = file(bin_file_name, "ab")
   f.write (''.rjust(FINAL_ALIGNMENT - file_size_in_bytes%FINAL_ALIGNMENT, chr(PAD)))
   f.close()


