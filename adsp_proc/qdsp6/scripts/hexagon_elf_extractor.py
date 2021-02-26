#! /usr/bin/python
# This Tool will parse a QCT ELF file and extract Hexagon related information.
# The output file needs to be sent to the target to be used by the on-device application
# for capturing Hexagon information during crashes.
#
# Run this tool with -h option to see all available options.
#
#  Copyright (c) 2011, 2012
#  Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary
#
from optparse import OptionParser
import os
import re

# Definitions
program_version             = "2.01"
objdump_program_name        = ''
elf_file_name               = ''
output_directory            = ''
output_file_name            = ''
DELIMITER                   = "|"
STR_VALUE                   = "_value"
STR_TMP_DOT_TXT             = "_tmp.txt"
objdump_x_file_name         = 'objdump_x.txt'
readelf_s_file_name         = 'readelf_s.txt'
elf_path_prefix             = "ELF_PATH" + DELIMITER
kernel_prefix               = "KERNEL" + DELIMITER
ADDRESS_FORMATTING          = "%s" + DELIMITER + "%s" + DELIMITER + "%s"
BUILD_INFO_FORMATTING       = "%s" + STR_VALUE + DELIMITER + "%s"
NO_VALUE                    = "NONE"
EMPTY_ADDRESS               = "00000000"
Q6_BUILD_PREFIX             = "Q6_BUILD"
BLAST_ERROR_INFO            = "BLAST_error_info"
QURT_ERROR_INFO             = "QURT_error_info"
MPSS                        = "mpss"
LPASS                       = "lpass"
ADSP                        = "adsp"
kernels                     = ['BLAST', 'QURT']
output_file_name_prefix     = "symbol_to_addressmap_" 
objdump_name_value_list     = []
readelf_name_value_list     = []
build_info_name_value_list  = []
kernel_version              = ''
kernel_version_from_elf     = ''

# Default values
default_objdump_program_name= "hexagon-objdump"
default_readelf_program_name= "hexagon-readelf"
default_output_directory    = NO_VALUE
out_put_file_header         =   "# This file contains values read from the ELF file needed by on device application\r\n" \
                                "# to extract different information from the phone during a crash.\r\n" \
                                "#\r\n" \
                                "# Following format is followed where the '" + DELIMITER + "' is the delimiter between values:\r\n" \
                                "#\r\n" \
                                "# variable name " + DELIMITER + " value " + DELIMITER + " value\r\n" \
                                "#\r\n" 

description_text            =   "This Tool will parse a QCT ELF file and extract Hexagon related information. " \
                                "The output file needs to be sent to the target to be used by the on-device application " \
                                "for capturing Hexagon information during crashes.\n" \
                                "A QDSP6 compatible 'objdump' and 'readelf' program need to be in the PATH of your Machine. " \
                                "If they are not installed on your machine you can install them from the installation package or from the link below: \n\n" \
                                "http://qdsp6.qualcomm.com/tools/builds \n\n"
usage_text                  =   "usage: %prog [options]\n\n" \
                                "Example1:\n" \
                                "        %prog  --elf=modem_image.elf --target=mpss\n\n" \
                                "In this case the default 'qdsp6-objdump' and 'qdsp6-readelf' will be used.\n" \
                                "'qdsp6-objdump' and 'qdsp6-readelf' are included in the installation package, which came with this tool.\n" \
                                "\n\n" \
                                "Example2:\n" \
                                "        %prog  --elf=modem_image.elf --target=lpass --outdir=\\\\network_drive\output_directory\ \n\n" \
                                "In this case he provided output directory and the default 'qdsp6-objdump' and 'qdsp6-readelf' will be used.\n" \
                                "'qdsp6-objdump' and 'qdsp6-readelf' are included in the installation package, which came with this tool.\n" \
                                "\n\n" \
                                "Example3:\n" \
                                "        %prog  --objdump=another-objdump-program --readelf=another-readelf-program --elf=modem_image.elf --target=mpss\n\n" \
                                "In this case another QDSP6 compatible 'objdump' and 'readelf' program will be used.\n" \



# Class definitions
class AddressInfo:
    def __init__(self, memoryName, memoryAddress, memoryValue):
        self.memoryName     = memoryName
        self.memoryAddress  = memoryAddress
        self.memoryValue    = memoryValue
    def __repr__(self):
        return ADDRESS_FORMATTING % (self.memoryName, self.memoryAddress, self.memoryValue) 

class AddressInfoWithSearchValue:
    def __init__(self, memoryName, memoryAddress, fileOffset, memoryValue, valueToSearchFor):
        self.memoryName         = memoryName
        self.memoryAddress      = memoryAddress
        self.fileOffset         = fileOffset
        self.memoryValue        = memoryValue
        self.valueToSearchFor   = valueToSearchFor
    def __repr__(self):
        return ADDRESS_FORMATTING % (self.memoryName, self.memoryAddress, self.memoryValue) 

class BuildInfo:
    def __init__(self, name, value):
        self.name   = name
        self.value  = value
    def __repr__(self):
        return BUILD_INFO_FORMATTING % (self.name, self.value) 

 
# Function definitions are here
def init_build_info_array():
    "This popluates the array with name value of build info"
    # Populate the build info name values
    build_info_name_value_list.append(BuildInfo(kernel_version + "K_version","00000000"))
    build_info_name_value_list.append(BuildInfo("q6_build","00000000"))
    build_info_name_value_list.append(BuildInfo("mob_sw_rev","00000000"))
    build_info_name_value_list.append(BuildInfo("rel_time","00000000"))
    build_info_name_value_list.append(BuildInfo("rel_date","00000000"))
    build_info_name_value_list.append(BuildInfo("ver_dir","00000000"))
    build_info_name_value_list.append(BuildInfo("ver_time","00000000"))
    build_info_name_value_list.append(BuildInfo("ver_date","00000000")) 
    build_info_name_value_list.append(BuildInfo("build_hostname", "00000000")) 
    build_info_name_value_list.append(BuildInfo("build_userid","00000000")) 
    return;

def init_address_info_array_for_blast():
    "This popluates the array with name of addresses to search for"
    # Populate the objdump names
    for build_info in build_info_name_value_list:  
        objdump_name_value_list.append(AddressInfo(build_info.name,"00000000","00000000"))
    # add other values than the build info one         
    objdump_name_value_list.append(AddressInfo("BLASTK_flush_cache_status","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLAST_error_info","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_thread_contexts","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_CONTEXT_SIZE","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_MAX_THREADS","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_MAX_THREADS_IN_TCM","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_trace_buffers","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_trace_buffer_size","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_trace_mask","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_pagetables","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("blast_PT_L1_ASID0","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_debug_buf","00000000","00000000"))    
    objdump_name_value_list.append(AddressInfo("coredump","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_event_waiting_list","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_errorlist","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("fws_smem_intf","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("BLASTK_futexhash","00000000","00000000")) 
    objdump_name_value_list.append(AddressInfo("BLASTK_futex_num","00000000","00000000"))   
    objdump_name_value_list.append(AddressInfo("qxdm_dbg_msg","00000000","00000000"))   
    objdump_name_value_list.append(AddressInfo("blast_config_bitmask","00000000","00000000"))	
    # Populate the readelf names
    readelf_name_value_list.append(AddressInfoWithSearchValue("START","00000000","00000000","00000000","start"))
    readelf_name_value_list.append(AddressInfoWithSearchValue("TEXT_SECTION","00000000","00000000","00000000","text"))
    readelf_name_value_list.append(AddressInfoWithSearchValue("BSS_SECTION","00000000","00000000","00000000","bss"))   
    readelf_name_value_list.append(AddressInfoWithSearchValue("RODATA","00000000","00000000","00000000","rodata"))  
    return;
	
def init_address_info_array_for_qurt():
    "This popluates the array with name of addresses to search for"
    # Populate the objdump names
    for build_info in build_info_name_value_list:  
        objdump_name_value_list.append(AddressInfo(build_info.name,"00000000","00000000"))
    # add other values than the build info one             
    objdump_name_value_list.append(AddressInfo("QURTK_flush_cache_status","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURT_error_info","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_thread_contexts","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_thread_contexts_tcm","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_CONTEXT_SIZE","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_MAX_THREADS","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_MAX_THREADS_IN_TCM","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_trace_buffers","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_trace_buffer_size","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_trace_mask","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_pagetables","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("qurt_PT_L1_ASID0","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_debug_buf","00000000","00000000"))    
    objdump_name_value_list.append(AddressInfo("coredump","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_event_waiting_list","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_errorlist","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("fws_smem_intf","00000000","00000000"))
    objdump_name_value_list.append(AddressInfo("QURTK_futexhash","00000000","00000000")) 
    objdump_name_value_list.append(AddressInfo("QURTK_futex_num","00000000","00000000"))   
    objdump_name_value_list.append(AddressInfo("QURTK_futex_objs","00000000","00000000"))   
    objdump_name_value_list.append(AddressInfo("QURTK_futex_objs_end","00000000","00000000"))   	
    objdump_name_value_list.append(AddressInfo("qxdm_dbg_msg","00000000","00000000")) 
    objdump_name_value_list.append(AddressInfo("qurt_config_bitmask","00000000","00000000"))		
    # Populate the readelf names
    readelf_name_value_list.append(AddressInfoWithSearchValue("START","00000000","00000000","00000000","start"))
    readelf_name_value_list.append(AddressInfoWithSearchValue("TEXT_SECTION","00000000","00000000","00000000","text"))
    readelf_name_value_list.append(AddressInfoWithSearchValue("BSS_SECTION","00000000","00000000","00000000","bss"))   
    readelf_name_value_list.append(AddressInfoWithSearchValue("RODATA","00000000","00000000","00000000","rodata"))   
    return;

def init_address_info_array():
    "Determin the Hexagon Kernel version"
    global kernel_version
	
    # Start processing the file
    try:
        input_file = open(objdump_x_file_name, 'rb')
    except:
        print("Error: Couldn't open the file '" + objdump_x_file_name + "'.") 
        os._exit(1)	
	
    #Reset the file index
    input_file.seek(0)  
    index = input_file.read().find(BLAST_ERROR_INFO)
    if(index >=0):
        #BLAST kernel
        kernel_version = kernels[0]
        print("Hexagon Kernel : " + kernel_version)
        init_build_info_array()
        init_address_info_array_for_blast()
        input_file.close()  
        return;
		
    input_file.seek(0)  
    index = input_file.read().find(QURT_ERROR_INFO)
    if(index >=0):
        #QURT kernel
        kernel_version = kernels[1]
        print("Hexagon Kernel : " + kernel_version)
        init_build_info_array()
        init_address_info_array_for_qurt()
    input_file.close()  
    return;	
		
def convert_elf_to_objdump_text():
    "Parses the ELF file and creates a objdump text file of it"
    # First remove the output file if it exists.
    if(os.path.isfile(objdump_x_file_name)):
        try:
            os.remove(objdump_x_file_name)
        except:
            print("\nError: Couldn't delete the existing file '" + objdump_x_file_name + "'.") 
            print("Please make sure that it's not a READ-ONLY file or opened by another program and there is no problem with the file system.") 
            os._exit(1)

    # Now run the command to convert ELF to Text file
    command_to_run = objdump_program_name + " -x " + elf_file_name + " > " + objdump_x_file_name
    print("Running '" + command_to_run + "' please wait...")  
    result = os.system(command_to_run)
    if(result != 0):
        print("\nError: Could not run '" + objdump_program_name + "' tool on the '" + elf_file_name + "' file.")
        print("Please double check the path to the '" + elf_file_name + "' file.")
        print("Please also make sure that a QDSP6 compatible 'objdump' program exists in the PATH of your PC.")
        print("Run this script with -h option to get more info about QDSP6 compatible 'objdump' installation.")
        os._exit(1)
    else:
        print(objdump_x_file_name + " was successfully created.\n")  
    return;


def convert_elf_to_readelf_text():
    "Parses the ELF file and create a readelf text file of it"
    # First remove the output file if it exists.
    if(os.path.isfile(readelf_s_file_name)):
        try:
            os.remove(readelf_s_file_name)
        except:
            print("\nError: Couldn't delete the existing file '" + readelf_s_file_name + "'.") 
            print("Please make sure that it's not a READ-ONLY file or opened by another program and there is no problem with the file system.") 
            os._exit(1)

    # Now run the command to convert ELF to Text file
    command_to_run = readelf_program_name + " -S " + elf_file_name + " > " + readelf_s_file_name
    print("Running '" + command_to_run + "' please wait...")  
    result = os.system(command_to_run)
    if(result != 0):
        print("\nError: Could not run '" + readelf_program_name + "' tool on the '" + elf_file_name + "' file.")
        print("Please double check the path to the '" + elf_file_name + "' file.")
        print("Please also make sure that a QDSP6 compatible 'readelf' program exists in the PATH of your PC.")
        print("Run this script with -h option to get more info about QDSP6 compatible 'readelf' installation.")
        os._exit(1)
    else:
        print(readelf_s_file_name + " was successfully created.\n")  
    return;


def extract_objdump_values():
    "Parses the objdump text based file and extract needed information"
    print("Extracting values from the '" + objdump_x_file_name + "' file. Please wait...\n")  
    # Start processing the file
    try:
        input_file = open(objdump_x_file_name, "r")
    except:
        print("Error: Couldn't open the file '" + objdump_x_file_name + "'.") 
        os._exit(1)

    # Search for the name in the file
    for line in input_file:                 
        # Loop through address info values
        for address_info in objdump_name_value_list:                
            p = re.compile(r'\b%s\b' % address_info.memoryName) 
            if(p.search(line)):             
                splitted_line = line.split()
                print splitted_line
                array_size = len(splitted_line) 
                newAddress_info =  AddressInfo(address_info.memoryName, splitted_line[0], splitted_line[array_size - 2])   
                try:
                    loc = objdump_name_value_list.index(address_info)
                    objdump_name_value_list[loc] = newAddress_info
                except ValueError:
                    print("Error: Couldn't replace the address info in the memory. Should never happens unless if the memory is corrupted.\n") 
                    input_file.close()  
                    os._exit(1)               
    input_file.close()
    # Remove the temp file
    if(os.path.isfile(objdump_x_file_name)):
        try:
            os.remove(objdump_x_file_name)
            print(objdump_x_file_name + " was successfully deleted.\n")
        except: 
            pass    
    return;

def convert_elf_to_objdump_s_f_text():
    "Parses the ELF file and creates a objdump text file with -s and -F flag of it"
    # Find the start and end address to use  
    for build_info in build_info_name_value_list:        
        startAddress            = NO_VALUE
        sizeValue               = NO_VALUE
        build_info_name         = build_info.name
        objdump_s_f_file_name   = build_info_name + STR_TMP_DOT_TXT
        objdump_s_f_file_name   = os.path.join(output_directory, objdump_s_f_file_name)  
        
        # First remove the output file if it exists.
        if(os.path.isfile(objdump_s_f_file_name)):
            try:
                os.remove(objdump_s_f_file_name)
            except:
                print("\nError: Couldn't delete the existing file '" + objdump_s_f_file_name + "'.") 
                print("Please make sure that it's not a READ-ONLY file or opened by another program and there is no problem with the file system.") 
                os._exit(1)      
        for address_info in objdump_name_value_list: 
            if(address_info.memoryName == build_info_name and address_info.memoryAddress != EMPTY_ADDRESS):
                startAddress = address_info.memoryAddress
                sizeValue    = address_info.memoryValue
                
        # Do error checking     
        if((startAddress == NO_VALUE) or (sizeValue   == NO_VALUE)):
            print("\nWarning: Could not find any address and size in the '" + build_info_name + "' variable." )
            print("The processing will still continue but the value of '" + build_info_name + STR_VALUE + "' will be set to '" + NO_VALUE + "'.")
            print("The on-device application or any other tool for checking ELF missmatching issues, won't be able to do any checking of ")
            print("the ELF build information to make sure the right ELF is loaded into the device or used for debugging crashes.\n")           
            
            # Update the value in the array with the new value which is "NO_VALUE"
            index = build_info_name_value_list.index(build_info)
            if(index >= 0):
                build_info.value = NO_VALUE
                build_info_name_value_list[index] = build_info
        else:
            print("\nThe '" + build_info_name + "' variable was found and that provides the start address and the size of '" + build_info_name + STR_VALUE + "':")
            print("\n\tstart address = " + startAddress + " , size = " + sizeValue + "\n")
    
            # Calculate the end address from start and size
            endAddress_int = int(startAddress, 16) + int(sizeValue, 16) - 1
            endAddress = "0x%08x" % endAddress_int
            startAddress = "0x" + startAddress
            
            # Now run the command to convert ELF to Text file
            command_to_run = objdump_program_name + " -s -F -j .rodata -j .start -j .data -j .sdata --start-address=" + startAddress \
               + " --stop-address=" + endAddress + " " + elf_file_name + " > " + objdump_s_f_file_name
            print("Running '" + command_to_run + "' please wait...")  
            result = os.system(command_to_run)
            if(result != 0):
                print("\nWarning: Could not run '" + objdump_program_name + "' tool on the '" + elf_file_name + "' file.")
                print("Please double check the path to the '" + elf_file_name + "' file.")
                print("Please also make sure that a QDSP6 compatible 'objdump' program exists in the PATH of your PC.")
                print("Run this script with -h option to get more info about QDSP6 compatible 'objdump' installation.")
                os._exit(1)
            else:
                print(objdump_s_f_file_name + " was successfully created.\n")  
    
    return;

def extract_build_information():
    "Parses the objdump text based file and extract needed information"  
    for build_info in build_info_name_value_list:        
        build_info_name         = build_info.name
        build_info_value        = build_info.value
        objdump_s_f_file_name   = build_info_name + STR_TMP_DOT_TXT
        objdump_s_f_file_name   = os.path.join(output_directory, objdump_s_f_file_name)  
       
        if(build_info_value != NO_VALUE):
            print("Extracting values from the '" + objdump_s_f_file_name + "' file. Please wait...")  
            # Start processing the file
            try:
                input_file = open(objdump_s_f_file_name, "r")
            except:
                print("Error: Couldn't open the file '" + objdump_s_f_file_name + "'.") 
                os._exit(1)
        
            # Search for the build information
            retrieved_buffer   = ''
            build_info_value = retrieved_buffer
            number_of_lines = 0
            for line in input_file: 
                number_of_lines = number_of_lines + 1 
                line_size = len(line)
                
                #The real data dump part starts at offset value_offset
                if(number_of_lines > 4 and line_size > 17):
                    value_offset = line_size - 16 -1;
                    retrieved_buffer = line[value_offset:].strip()
                    build_info_value += retrieved_buffer
					
			# in case that the symbol is not available
            if(number_of_lines <= 4):
                build_info_value = NO_VALUE
				
            # Update the value in the array with the new value
            index = build_info_name_value_list.index(build_info)
            if(index >= 0):
                build_info.value = build_info_value
                build_info_name_value_list[index] = build_info                        
            input_file.close() 
            print("The build information found, %s = '%s'" % (build_info_name, build_info_value))  
            
			# Remove the temp file
            if(os.path.isfile(objdump_s_f_file_name)):
                try:
                    os.remove(objdump_s_f_file_name)
                    print(objdump_s_f_file_name + " was successfully deleted.\n")
                except: 
                    pass   
    return;


def extract_readelf_values():
    "Parses the readelf text based file and extract needed information"
    print("\nExtracting values from the '" + readelf_s_file_name + "' file. Please wait...\n")  
    # Start processing the file
    try:
        input_file = open(readelf_s_file_name, "r")
    except:
        print("Error: Couldn't open the file '" + readelf_s_file_name + "'.") 
        os._exit(1)

    # Search for the name in the file
    for line in input_file:               
        # Loop through address info values
        for address_info in readelf_name_value_list:                
            p = re.compile(r'\b%s\b' % address_info.valueToSearchFor) 
            if(p.search(line)):           
                splitted_line = line.split()
                print splitted_line
                array_size = len(splitted_line) 
                newAddress_info =  AddressInfoWithSearchValue(address_info.memoryName, splitted_line[array_size - 8], splitted_line[array_size - 7], splitted_line[array_size - 6], address_info.valueToSearchFor)   
                try:
                    loc = readelf_name_value_list.index(address_info)
                    readelf_name_value_list[loc] = newAddress_info
                except ValueError:
                    print("Error: Couldn't replace the address info in the memory. Should never happens unless if the memory is corrupted.\n") 
                    input_file.close()  
                    os._exit(1)               
    input_file.close() 
    # Remove the temp file
    if(os.path.isfile(readelf_s_file_name)):
        try:
            os.remove(readelf_s_file_name)
            print(readelf_s_file_name + " was successfully deleted.\n")
        except: 
            pass 
    return;

def scan_q6_build_info():
    "Scan the elf to search for Q6_BUILD that has build date/time to identify LPASS version"
	# Find the file offset of the build info and also the address of rodata. 
	# Need to calculate the address of the build info based on rodata section information.
	
    if (options.target != LPASS and options.target != ADSP):
        return;
    print("Scanning '" + elf_file_name + "' file to find Q6_BUILD...")  
    # Start processing the file
    try:
        input_file = open(elf_file_name, 'rb')
    except:
        print("Error: Couldn't open the file '" + elf_file_name + "'.") 
        os._exit(1)	
			
    #Reset the file index
    input_file.seek(0)  
    index = input_file.read().find(Q6_BUILD_PREFIX)
    if(index >=0):
        #Values were found
        input_file.seek(index) 
        string_to_parse = input_file.read(100)
        splitted_string = string_to_parse.split('\x00')
        print("found: " + splitted_string[0])
		
        for section_info in readelf_name_value_list:                
            if(section_info.memoryName == "RODATA"):  	
				index = int(section_info.memoryAddress, 16) + index - int(section_info.fileOffset, 16)
		
        for build_info in build_info_name_value_list:     
            if(build_info.name == "q6_build"):
                build_info.value = splitted_string[0]       
        for addr_info in objdump_name_value_list:     
            if(addr_info.memoryName == "q6_build"):
                addr_info.memoryAddress = "%0.8x" % index
                addr_info.memoryValue = "%0.8x" % len(splitted_string[0])      
    input_file.close()  
    return;
	
def write_the_information_into_the_file():
    "write the extracted information to the output file"
    # First remove the output file if it exists.
    if(os.path.isfile(output_file_name)):
        try:
            os.remove(output_file_name)
        except:
            print("\nError: Couldn't delete the existing file '" + output_file_name + "'.") 
            print("Please make sure that it's not a READ-ONLY file or opened by another program and there is no problem with the file system.") 
            os._exit(1)
      
    # Now save values into the file    
    try:
        out_file = open(output_file_name, 'wb+') 
        out_file.write(out_put_file_header) 
        out_file.write(elf_path_prefix + elf_file_name) 
        out_file.write("\r\n")    
        out_file.write(kernel_prefix + kernel_version) 
        out_file.write("\r\n")    
        for build_info in build_info_name_value_list:
            out_file.write(str(build_info))
            out_file.write("\r\n")      
        for address_info in objdump_name_value_list: 
            out_file.write(str(address_info))
            out_file.write("\r\n")
        for address_info in readelf_name_value_list: 
            out_file.write(str(address_info))
            out_file.write("\r\n")
    except:
        print("\nError: Couldn't write values into the file '" + output_file_name + "'.") 
        print("Please make sure that it's not a READ-ONLY file and there is enough space on the disk.") 
        os._exit(1)
    
    print("\nDone! All values are now saved into the output file '" + output_file_name + "' and ready to be transferred to the target.\n")     
    out_file.close()  
    return;

#-------------------- Start of the program --------------------
# Parse the input arguments and print usage and help if needed
parser = OptionParser(usage = usage_text, version = "%prog version " + program_version, description=description_text)
parser.add_option("--elf", dest="elf_file_name",
                  help="The name of ELF file to parse.")
parser.add_option("--target", dest="target", 
                  help="The Target subsystem. mpass, lpass or adsp")
parser.add_option("--outdir", dest="output_directory", default=default_output_directory,
                  help="(Optional) The output directory, where to save the result. Default will be where the program is run from.")
parser.add_option("--objdump", dest="objdump_program_name", default=default_objdump_program_name,
                  help="(Optional) The name of 'objdump' program to use for dumping the ELF file. Default will be '" + default_objdump_program_name +"'")
parser.add_option("--readelf", dest="readelf_program_name", default=default_readelf_program_name,
                  help="(Optional) The name of 'readelf' program to use for dumping the ELF file. Default will be '" + default_readelf_program_name +"'")


(options, args) = parser.parse_args()

if len(args) > 0:
    parser.error("incorrect number of arguments")

if not options.elf_file_name:         
    parser.error('No ELF file name was provided. \nPlease provide an ELF file name to parse. Run this tool with -h to see all options.')
if not options.target:         
    parser.error('No target was provided. \nPlease specify the target subsystem(mpss or lpass). Run this tool with -h to see all options.')
if not (options.target == MPSS or options.target == LPASS or options.target == ADSP) : 
	parser.error('Incorrect target was provided. \nPlease specify the target subsystem(mpss or lpass). Run this tool with -h to see all options.')
	
objdump_program_name        = options.objdump_program_name
readelf_program_name        = options.readelf_program_name
elf_file_name               = options.elf_file_name
output_directory            = options.output_directory
if(output_directory == NO_VALUE):
    output_directory        = os.getcwd()
output_file_name_str        = output_file_name_prefix + options.target + ".txt"
output_file_name            = os.path.join(output_directory, output_file_name_str)  
objdump_x_file_name         = os.path.join(output_directory, objdump_x_file_name)  
readelf_s_file_name         = os.path.join(output_directory, readelf_s_file_name)  

print("\nBased on user inputs, the tool will use following arguments:\n")
print("objdump program name \t\t= " + objdump_program_name)
print("readelf program name \t\t= " + readelf_program_name)
print("elf file name \t\t\t= " + elf_file_name)
print("target \t\t\t\t= " + options.target)
print("output directory \t\t= " + output_directory)
print("output file name \t\t= " + output_file_name +"\n")


convert_elf_to_objdump_text()
convert_elf_to_readelf_text()
init_address_info_array()
extract_objdump_values()
convert_elf_to_objdump_s_f_text()
extract_build_information()
extract_readelf_values()
scan_q6_build_info()
write_the_information_into_the_file()

