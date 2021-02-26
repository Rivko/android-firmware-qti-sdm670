import json,os
from json import loads,dumps
from collections import OrderedDict
import struct
import binascii
import csv
import array
from optparse import OptionParser


#Function to convert hex string from Little Endian to Big Endian
#Parameters:
#	- hex_str 	: The hexadecimal string to be given as input
#	- get8bits 	: Boolean value indicating whether we only want 8 bits or the whole string
def littleToBig(hex_str,get8bits=False):
	little_hexstr=long(hex_str,16)
	little_hexstr=long(little_hexstr)
	little_hexstr=struct.pack(">L",little_hexstr)
	little_hexstr = little_hexstr[::-1]
	if get8bits:
		return little_hexstr[:2]
	else:
		return little_hexstr

#Function to convert Decimal string to Hexadecimal string
#Parameters:
#	- dec_str 		: The Decimal string to be given as input
#	- align_size	: The size of the final hexadecimal string in bits
def convert_str(dec_str,align_size=4):
	hex_str = int(dec_str)
	hex_str = "%0.2X" %hex_str
	if len(hex_str)<4:
		hex_str = hex_str.zfill(align_size)
	return hex_str

#Helper function to process list of pre-procs/post-procs from decimal to hexadecimal
def counter_table(procs_list):
	procs = []
	for proc_ind in procs_list:
		proc_ind = int(proc_ind)
		procs.append(convert_str(proc_ind))
		if proc_ind==0:
			return procs
	return procs

#Helper function to convert OrderedDict to Dict object
def to_dict(ordered_dict):
    return loads(dumps(ordered_dict,sort_keys=False))

#Ensure that the final imag is byte aligned
def bin_file_align(bin_file_name):
	f_size=os.path.getsize(bin_file_name)
	f_size=(f_size%4)
	if(f_size!=0):
	    f_size=4-f_size
	    fp = open(bin_file_name, 'ab')                
	    for x in range(f_size):
	        fp.write(chr(0))
	    fp.close() 
	    
	print(".bin file generated and aligned")

parser = OptionParser()

parser.add_option("-i", "--input_json_file",
                action="store", type="string", dest="input_json_file",
                help="Location of the JSON file containg the SBL Images.")


parser.add_option("-o", "--output_bin_file",
                action="store", type="string", dest="output_bin_file",
                help="Location of the genrated ELF file parsed from the input file.")

(options, args) = parser.parse_args()

if not options.input_json_file:
	parser.error("Input JSON file not supplied.")

if not options.output_bin_file:
	parser.error("Output destination of ELF file not supplied.")

json_file = options.input_json_file

#Load the json as a dictionary
data = json.load(open(json_file), object_pairs_hook=OrderedDict)
data = to_dict(data)

#Create the binary file
bin_file_name = options.output_bin_file
if os.path.isfile(bin_file_name):
	os.remove(bin_file_name) 
fp = open(bin_file_name, 'wb') 

#These lists contain the sequence in which the fields are to be read
bool_values = ['load','auth','exec','jump','boot_ssa_enabled','xpu_enable']
funcs_addr = ['exec_func','jump_func','load_cancel']
image_list = data['Image_Order']


#Loop through all the images		
for image in image_list:
	
	#Extract sec type
	sec_type = data["Images"][image]['target']['sec_type']
	for hex_char in littleToBig(convert_str(sec_type,align_size=2))[:4]:
		fp.write(hex_char)
	

	#Extract Partition Id
	for p_id in data["Images"][image]['target']["partition_id"]:
		if type(p_id)!=type([]):
			if len(p_id)>8:
				get8bits = False
			else:
				get8bits = True
			for hex_char in littleToBig(p_id,get8bits):
				fp.write(hex_char)
		else:
			for p_id_8bits in p_id:
				for hex_char in binascii.unhexlify(p_id_8bits[2:]):
					fp.write(hex_char)	

	#Hexdecimal buffer to store subsequent entries
	hex_buf=[]

	#Extract target_str
	for i in data["Images"][image]['target']["target_str"]:
		hex_buf.append(i)
	#Perform padding if length of target_str < 8
	if len(data["Images"][image]['target']["target_str"])!=8:
		temp_buf = (chr(int('0', 16)))*(8-len(data["Images"][image]['target']["target_str"]))
		hex_buf.extend(temp_buf)
	
	#Extract the boolean values like load,exec,auth,jump,exable xpu and boot_ssa_enabled
	for funcs in bool_values:
		hex_buf.append(binascii.unhexlify(convert_str(data["Images"][image]['funcs_bool'][funcs],align_size=2)))


	#Extract ID for load_cancel function, jump function and exec function
	for addr in funcs_addr:
		func_actual_id =data["Images"][image]["funcs_addr"][addr]
		hex_buf.append(binascii.unhexlify(convert_str(func_actual_id)[2:]))


	#convert the pre_procs table
	procs_type = ["pre_procs","post_procs"]

	for proc in procs_type:
		procs_buf = counter_table(data["Images"][image][proc])

		#Store length of procs buffer
		procs_len = (convert_str(len(procs_buf))[2:])
		hex_buf.append(binascii.unhexlify(procs_len))

		#Iterate over the procs list
		for procs in procs_buf:
			hex_buf.append(binascii.unhexlify(procs[2:]))

	#Store Whitelist size
	whitelist_size = len(data["Images"][image]['whitelist_ptr']['base'])
	hex_buf.append(binascii.unhexlify(convert_str(whitelist_size)[2:]))

	#Write the buffer into the binary
	#Since whitlist requires a different conversion we won't be storing it in the buffer
	for x in hex_buf:
		fp.write(x)
	
	#Parse the whitlist 
	for i in range(0,whitelist_size-1):
		#Extract the whitlist base at index 'i'
		hex_base = str(data["Images"][image]['whitelist_ptr']['base'][i])
		for hex_char in littleToBig(hex_base):
			fp.write(hex_char)

		#Extract the whitlist size at index 'i'
		hex_size = str(data["Images"][image]['whitelist_ptr']['size'][i])
		for hex_char in littleToBig(hex_size):
			fp.write(hex_char)

	#Extract the seg_elf_entry_point
	hex_num	 =	data["Images"][image]['other_addr']['seg_elf_entry_point']
	if hex_num=='0x0':
		#This is for padding with zeros in case the seg-elf_entry point is 0x0
		for i in range(0,2):
			for ch1 in littleToBig(hex_num):
				fp.write(ch1)
	else:
		for ch1 in littleToBig(hex_num):
			fp.write(ch1)
		#This is for aligning the image 
		hex_num = '0x0'
		for ch1 in littleToBig(hex_num):
				fp.write(ch1)

	#End of for loop

#Close the file pointer
fp.close()


#Align the final image if not aligned
bin_file_align(bin_file_name)

	