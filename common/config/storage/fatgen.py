import struct, os, sys, getopt

# Constants
SECTOR_SIZE            = 512
FAT16_ENTRY_SIZE       = 2
FAT32_ENTRY_SIZE       = 4
MIN_FAT16_SIZE_MB      = 64
MIN_FAT32_SIZE_MB      = 256
KB_PER_MB              = 1024
B_PER_KB               = 1024

# Structs
fat_common_header = struct.Struct('<3s8sHBHBHHBHHHII')
fat_16_header = struct.Struct('<HBI11s8s448s2s')
fat_32_header = struct.Struct('<IHHIHH12sBBBI11s8s420s2s')
fat_32_fsi = struct.Struct('<4s480s4sII12s2s2s')

# Info dictionaries
fat_header_info = {}
fat_32_fsi_info = {}

# Global properties
fat_container_file = None
fat_type = None

def round_up(dividend, divisor):
    result = dividend / divisor
    if dividend % divisor != 0:
        result = result + 1
    return result

def generate_fat16_container(size_mb):
    cluster_size = 16*1024
    print "Using cluster size %d bytes" % cluster_size
    num_clusters = round_up((size_mb * KB_PER_MB * B_PER_KB), cluster_size)
    fat_table_size = num_clusters * FAT16_ENTRY_SIZE
    fat_table_size = fat_table_size + (SECTOR_SIZE-(fat_table_size%SECTOR_SIZE))
    max_root_dir_ent = 512
    fp = open(fat_container_file, 'wb')
    
    all_bytes = [
        '\xEB\x3C\x90',                                          # jump_code_nop,
        "MSDOS5.0"+(" "*(11-len("MSDOS5.0"))),                   # oem_name
        SECTOR_SIZE,                                             # bytes_per_sec
        round_up(cluster_size, SECTOR_SIZE),                     # sec_per_cluster
        1,                                                       # reserved_sec
        2,                                                       # num_fat_copies
        max_root_dir_ent,                                        # max_root_dir_ent
        0,                                                       # num_sec_small_32mb
        0xF8,                                                    # media_desc
        round_up(fat_table_size, SECTOR_SIZE),                   # sec_per_fat
        63,                                                      # sec_per_track
        255,                                                     # num_heads
        0,                                                       # num_hidden_sec
        round_up((size_mb * KB_PER_MB * B_PER_KB), SECTOR_SIZE), # num_sec
    ]
    fp.write(fat_common_header.pack(*all_bytes))
    
    if fat_type.lower() == "fat16".lower():
        all_bytes = [
            0x80,                                # logical_drive_num
            #0,                                  # reserved
            0x29,                                # ext_sign
            12345678,                            # ser_num
            "NO NAME"+(" "*(11-len("NO NAME"))), # vol_name
            "FAT16"+(" "*(8-len("FAT16"))),      # fat_name
            '\x33\xC9\x8E\xD1\xBC\xF0\x7B\x8E\xD9\xB8\x00\x20\x8E\xC0\xFC\xBD\x00\x7C\x38\x4E\x24\x7D\x24\x8B\xC1\x99\xE8\x3C\x01\x72\x1C\x83\xEB\x3A\x66\xA1\x1C\x7C\x26\x66\x3B\x07\x26\x8A\x57\xFC\x75\x06\x80\xCA\x02\x88\x56\x02\x80\xC3\x10\x73\xEB\x33\xC9\x8A\x46\x10\x98\xF7\x66\x16\x03\x46\x1C\x13\x56\x1E\x03\x46\x0E\x13\xD1\x8B\x76\x11\x60\x89\x46\xFC\x89\x56\xFE\xB8\x20\x00\xF7\xE6\x8B\x5E\x0B\x03\xC3\x48\xF7\xF3\x01\x46\xFC\x11\x4E\xFE\x61\xBF\x00\x00\xE8\xE6\x00\x72\x39\x26\x38\x2D\x74\x17\x60\xB1\x0B\xBE\xA1\x7D\xF3\xA6\x61\x74\x32\x4E\x74\x09\x83\xC7\x20\x3B\xFB\x72\xE6\xEB\xDC\xA0\xFB\x7D\xB4\x7D\x8B\xF0\xAC\x98\x40\x74\x0C\x48\x74\x13\xB4\x0E\xBB\x07\x00\xCD\x10\xEB\xEF\xA0\xFD\x7D\xEB\xE6\xA0\xFC\x7D\xEB\xE1\xCD\x16\xCD\x19\x26\x8B\x55\x1A\x52\xB0\x01\xBB\x00\x00\xE8\x3B\x00\x72\xE8\x5B\x8A\x56\x24\xBE\x0B\x7C\x8B\xFC\xC7\x46\xF0\x3D\x7D\xC7\x46\xF4\x29\x7D\x8C\xD9\x89\x4E\xF2\x89\x4E\xF6\xC6\x06\x96\x7D\xCB\xEA\x03\x00\x00\x20\x0F\xB6\xC8\x66\x8B\x46\xF8\x66\x03\x46\x1C\x66\x8B\xD0\x66\xC1\xEA\x10\xEB\x5E\x0F\xB6\xC8\x4A\x4A\x8A\x46\x0D\x32\xE4\xF7\xE2\x03\x46\xFC\x13\x56\xFE\xEB\x4A\x52\x50\x06\x53\x6A\x01\x6A\x10\x91\x8B\x46\x18\x96\x92\x33\xD2\xF7\xF6\x91\xF7\xF6\x42\x87\xCA\xF7\x76\x1A\x8A\xF2\x8A\xE8\xC0\xCC\x02\x0A\xCC\xB8\x01\x02\x80\x7E\x02\x0E\x75\x04\xB4\x42\x8B\xF4\x8A\x56\x24\xCD\x13\x61\x61\x72\x0B\x40\x75\x01\x42\x03\x5E\x0B\x49\x75\x06\xF8\xC3\x41\xBB\x00\x00\x60\x66\x6A\x00\xEB\xB0\x42\x4F\x4F\x54\x4D\x47\x52\x20\x20\x20\x20\x0D\x0A\x52\x65\x6D\x6F\x76\x65\x20\x64\x69\x73\x6B\x73\x20\x6F\x72\x20\x6F\x74\x68\x65\x72\x20\x6D\x65\x64\x69\x61\x2E\xFF\x0D\x0A\x44\x69\x73\x6B\x20\x65\x72\x72\x6F\x72\xFF\x0D\x0A\x50\x72\x65\x73\x73\x20\x61\x6E\x79\x20\x6B\x65\x79\x20\x74\x6F\x20\x72\x65\x73\x74\x61\x72\x74\x0D\x0A\x00\x00\x00\x00\x00\x00\x00\xAC\xCB\xD8', #exec_code
            '\x55\xAA',                          # exec_marker
        ]
        fp.write(fat_16_header.pack(*all_bytes))
        fp.write('\x00' * (SECTOR_SIZE - (fat_common_header.size + fat_16_header.size)))
        all_bytes = '\xF8\xFF\xFF\xFF'+((fat_table_size-4)*'\x00')
        fp.write(all_bytes)
        fp.write(all_bytes)
        all_bytes = max_root_dir_ent * 32 * '\x00'
        fp.write(all_bytes)
        all_bytes = SECTOR_SIZE * '\x00'
        fp.write(all_bytes)    
    fp.close()

def usage():
    print """
Usage: python fatgen.py [OPTION...]
Generates a FAT16 or FAT32 container.

Examples:
  python fatgen.py --fat16 --name=test.bin --size=256 # Generate a 256MB FAT16 container.

 Options:

  -fat16, --f                FAT16
  -s, --size                 Size in MB
  -c, --sectorsize           Sector size to use
  -n, --name                 Container name
  -?, --help                 Display this help message
  -v, --verbose              Verbose messages

    """

def main():
    global fat_container_file, fat_type, SECTOR_SIZE
    fat_size = None
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "fts:c:n:?v", ["fat16", "fat32", "size=", "sectorsize=", "name=", "help", "verbose"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(1)
    
    for o, a in opts:
        if o in ("-f", "--fat16"):
            fat_type = "fat16"
        elif o in ("-t", "--fat32"):
            fat_type = "fat32"
        elif o in ("-s", "--size"):
            fat_size = int(a)
        elif o in ("-c", "--sectorsize"):
            print "-c is deprecated for size in sectors. It now specifies the sector size (512, 4096 etc.)"
            SECTOR_SIZE = int(a)
        elif o in ("-n", "--name"):
            fat_container_file = a
        elif o in ("-?", "--help"):
            usage()
            sys.exit(0)
        elif o in ("-v", "--verbose"):
            pass
        else:
            assert False, "unhandled option"
    
    if fat_type is None:
        usage()
        print "\nERROR: No FAT type selected. Choose --fat16 or --fat32"
        sys.exit(1)
    if fat_size is None or fat_size <= 0:
        usage()
        print "\nERROR: Invalid FAT size"
        sys.exit(1)
    if (fat_type.lower()=="fat16".lower() and fat_size < MIN_FAT16_SIZE_MB) or (fat_type.lower()=="fat32".lower() and fat_size < MIN_FAT32_SIZE_MB):
        print
        print "Based on the default cluster sizes used, the container must be larger than a minimum size"
        print "in order to be correctly recognized by Windows systems. See below for minimum sizes:"
        print "FAT16 -  Default cluster size: 16384, Minimum size:  64MB"
        print "FAT32 -  Default cluster size: 4096,  Minimum size: 256MB"
        print
        sys.exit(1)
    if fat_container_file is None:
        fat_container_file = "%s_%dMB.bin" % (fat_type.upper(), fat_size)
        print "No FAT container file name provided, name will be set to %s" % fat_container_file
    
    if not os.path.exists(fat_container_file):
        if fat_type.lower()=="fat16".lower():
            generate_fat16_container(fat_size)
            print "Generated %dMB %s container: %s" % (fat_size, fat_type.upper(), fat_container_file)
    else:
        print "File %s already exists. Exiting." % fat_container_file

if __name__ == "__main__":
    main()
