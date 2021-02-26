#============================================================================

#

# 

#

# GENERAL DESCRIPTION

#    Contains methods to parse Keys & generate keystore

#

# Copyright  2011 by Qualcomm Technologies, Incorporated.

# All Rights Reserved.

# QUALCOMM Proprietary/GTDR

#

#----------------------------------------------------------------------------

#

#  $Header: //components/rel/core.qdsp6/2.1.c4/securemsm/ssd/tools/dev_prov_scripts/generate_keystore.py#1 $

#  $DateTime: 2018/07/30 01:21:36 $

#  $Author: pwbldsvc $

#  $Change: 16759227 $

#

#                      EDIT HISTORY FOR FILE

#

#  This section contains comments describing changes made to the module.

#  Notice that changes are listed in reverse chronological order.

#

# when       who       what, where, why

# --------   ---       ------------------------------------------------------

# 12/14/09    hv       Initial revision

#

#============================================================================

import ConfigParser

import os

import binascii

import shutil

#----------------------------------------------------------------------------

# Global values

#----------------------------------------------------------------------------

E_SUCCESS = 0

E_FAIL = 1

SSD_FS_EMMC_BLOCK_SIZE = 512



def get_file_path(line):

    start = 1 +     line.find('>')

    end = line.find('<', start)

    



    return line[start:end]



def _to_hex_string(int_):

       

    hex_values = { 0: '0', 1: '1', 2: '2', 3: '3', 4:'4', 5:'5', 6:'6', 7:'7', 8:'8', 9:'9', \

                 10:'a', 11:'b', 12: 'c', 13:'d', 14: 'e', 15:'f'}

    

    A = int_/4096

    B = int_%4096



    C = B/256

    D = B%256



    E = D/16

    F = B%16

    return hex_values.get(E) + hex_values.get(F) + hex_values.get(A) + hex_values.get(C)



def main():



    #setup the path for working directories

    os.chdir("..")

    SSD_TOOLS_DIR = os.getcwd()

    SSD_TOOLS_DIR += "/"



    DEV_PROV_SCRIPT_DIR = SSD_TOOLS_DIR + "dev_prov_scripts/"

    KEY_CONFIG_FILE_DIR = DEV_PROV_SCRIPT_DIR + "keys/"

    KEY_CONFIG_FILE_NAME = KEY_CONFIG_FILE_DIR + "keys.conf"

    KEY_STORE_FILE_NAME = DEV_PROV_SCRIPT_DIR + "keystore.dat"

    ZERO_BUFF_FILE_NAME = DEV_PROV_SCRIPT_DIR + "zeros.dat"



    #open output file

    if True == os.path.exists(KEY_STORE_FILE_NAME):

      os.remove(KEY_STORE_FILE_NAME)



    key_store_fp = open(KEY_STORE_FILE_NAME, "wb")



    #open the zero buf file

    zero_buff_fp = open(ZERO_BUFF_FILE_NAME, "rb")

    zero_buff = zero_buff_fp.read()



    key_store_fp.write("ssdksimg")



    #write SSD_FS_EMMC_BLOCK_SIZE zero's 

    key_store_fp.write(zero_buff[:SSD_FS_EMMC_BLOCK_SIZE])



    #open the keys.conf file

    keys_conf_fp = open(KEY_CONFIG_FILE_NAME, "r")

    

    no_of_files = 0

    #read each line 

    for line in keys_conf_fp:

        bytes_written = 0

        #extract file name

        key_file_name = KEY_CONFIG_FILE_DIR + get_file_path(line)



        #open file

        print "Opening file :\n" + key_file_name

        key_file_fp = open(key_file_name, "rb")



        #read file

        key_file_buff = key_file_fp.read()

        

        #write the key size

        key_store_fp.write(binascii.unhexlify(_to_hex_string(os.path.getsize(key_file_name))))



        #add space for 16 bytes of IV

        key_store_fp.write(zero_buff[0:16])



        #write file to output file

        key_store_fp.write(key_file_buff)

        

        #calculate the padding required

        key_file_size = os.path.getsize(key_file_name) + 16 + 2

        no_of_blocks = ( key_file_size/SSD_FS_EMMC_BLOCK_SIZE )

        if key_file_size%SSD_FS_EMMC_BLOCK_SIZE != 0:

            no_of_blocks += 1



        bytes_to_pad = (no_of_blocks *SSD_FS_EMMC_BLOCK_SIZE) - os.path.getsize(key_file_name) - 16 -2 

        



        #write paddind to the output file

        key_store_fp.write(zero_buff[0:bytes_to_pad])

        

        no_of_files += 1 



    #write the number of keys we wrote to the file

    #for each key there is a public key ID

    no_of_keys = no_of_files/2



    #seek to the begining of the file

    key_store_fp.seek(8)



    key_store_fp.write(binascii.unhexlify(_to_hex_string(no_of_keys)))





    #close file

    key_store_fp.close()

        

if __name__ == "__main__":

    main()

