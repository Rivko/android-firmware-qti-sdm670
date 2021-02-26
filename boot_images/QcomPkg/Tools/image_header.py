#============================================================================
#
#/** @file image_header.py
#
# GENERAL DESCRIPTION
#   Creates a header to attach to the generated .fd file to be used by
#   bootloaders to know where to load the UEFI bootloader in memory
#
# Copyright 2010, 2013-2014, 2016 by Qualcomm Technologies Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#**/
#
#----------------------------------------------------------------------------
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 04/16/16   et        Appended path to import correct mbn_tools
# 11/04/14   ck        Removed elf type parameters from mbn_tools.py calls
# 11/28/13   aus       Added 64 bit elf support
# 11/01/13   aus       Added support to generate elf images
# 11/11/10   niting    Initial revision
#
#============================================================================
import os
import sys
# Add path to mbn_tools
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                'sectools','sectools','features','isc','parsegen'))
import mbn_tools

if len(sys.argv) < 5:
   print "Incorrect Usage: image_header.py <source file> <target file> <image_destination> <mbn type> <opt: elf_type>"
   print "Example: image_header.py ..\..\Build\Msm8660\DEBUG_RVCT31\FV\MSM8660_EFI.fd appsboot.mbn 0x40100000 bin"
   print "Example: image_header.py ..\..\Build\Msm8660\DEBUG_RVCT31\FV\MSM8660_EFI.fd appsboot.mbn 0x40100000 elf 64"
   print "Example: image_header.py ..\..\Build\Msm8660\DEBUG_RVCT31\FV\MSM8660_EFI.fd appsboot.mbn 0x40100000 elf 32"
   raise RuntimeError, "Usage: image_header.py <source file> <target file> <image_destination> <mbn type> <opt: elf_type>" 

gen_dict = {}
source_full = sys.argv[1]
target_full = sys.argv[2]
# Attempt to evaluate value
try:
   image_destination = eval(sys.argv[3])
# Catch exceptions and do not evaluate
except:
   raise RuntimeError, "Invalid image destination address"

mbn_type = sys.argv[4]

is_elf_64_bit = False
if len(sys.argv) >= 6:
   if sys.argv[5] == '64':
      is_elf_64_bit = True

source_base = os.path.splitext(str(source_full))[0]
target_base = os.path.splitext(str(source_full))[0]

header_format = 'reg' 
gen_dict['IMAGE_KEY_IMAGE_ID'] = mbn_tools.ImageType.APPSBL_IMG
gen_dict['IMAGE_KEY_IMAGE_SOURCE'] = 0
gen_dict['IMAGE_KEY_IMAGE_DEST'] = image_destination
gen_dict['IMAGE_KEY_MBN_TYPE'] = mbn_type
image_header_secflag = 'non_secure'
image_size = os.stat(source_full).st_size

#----------------------------------------------------------------------------
# Generate UEFI mbn
#----------------------------------------------------------------------------

# MBN is generated in BIN format
if mbn_type == 'bin':

    # Create mbn header for UEFI
    rv = mbn_tools.image_header(os.environ, gen_dict, source_full, target_base + ".hd", 
                           image_header_secflag, header_format)
    if rv:
       raise RuntimeError, "Failed to create mbn header" 

    files_to_cat_in_order = [target_base + ".hd", source_full]
    mbn_tools.concat_files (target_full, files_to_cat_in_order)	

# MBN is generated in ELF format 
elif mbn_type == 'elf':

    source_elf = source_base + ".elf"
    target_hash = target_base + ".hash"
    target_hash_hd = target_base + "_hash.hd"
    target_phdr_elf = target_base + "_phdr.pbn"
    target_nonsec = target_base + "_combined_hash.mbn"

    # Create elf header for UEFI
    rv = mbn_tools.create_elf_header(target_base + ".hd", image_destination, image_size, is_elf_64_bit = is_elf_64_bit)
    if rv:
       raise RuntimeError, "Failed to create elf header" 

    files_to_cat_in_order = [target_base + ".hd", source_full]
    mbn_tools.concat_files (source_elf, files_to_cat_in_order)
    
    # Create hash table 
    rv = mbn_tools.pboot_gen_elf([], source_elf, target_hash, 
                                 elf_out_file_name = target_phdr_elf,
                                 secure_type = image_header_secflag)        
    if rv:
       raise RuntimeError, "Failed to run pboot_gen_elf" 

    # Create hash table header
    rv = mbn_tools.image_header(os.environ, gen_dict, target_hash, target_hash_hd, 
                         image_header_secflag, elf_file_name = source_elf)
    if rv:
       raise RuntimeError, "Failed to create image header for hash segment" 

    files_to_cat_in_order = [target_hash_hd, target_hash]
    mbn_tools.concat_files (target_nonsec, files_to_cat_in_order)

    # Add the hash segment into the ELF 
    mbn_tools.pboot_add_hash([], target_phdr_elf, target_nonsec, target_full)
     
