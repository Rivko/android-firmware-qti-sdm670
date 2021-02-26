import sys
import re
import os.path
import binascii
import datetime

possible_dump_name=["DDRCS0.BIN","DDRCS1.BIN","DDRCS0_0.BIN","DDRCS0_1.BIN","DDRCS0_2.BIN","DDRCS1_0.BIN","DDRCS1_1.BIN","DDRCS1_2.BIN","ADSP0.bin","EBICS0.RAM","dram_0x80000000--0xffffffff.lst","dram1_0x100000000--0x17fffffff.lst","dram_0x40000000--0xffffffff.lst","dram1_0x100000000--0x1bfffffff.lst","dram_cs0_0x80000000--0xffffffff.lst","dram_cs0_0x100000000--0x17fffffff.lst"]

def is_adsp_start_string(file,index,string,outputpath,input,seek_offset,TargetName,cdsp_flag):
    StartAddress = False
    VirtualAddress = False
    if TargetName == "660" or TargetName == "670" or TargetName == "630" or TargetName == "845":
        if cdsp_flag == "True":
            len_30mb = 10485760 #10MB value
            string_search = "IMAGE_VARIANT_STRING=660.cdsp"
            if TargetName == "670":
                string_search = "IMAGE_VARIANT_STRING=670.cdsp"
            elif TargetName == "845":
                string_search = "IMAGE_VARIANT_STRING=845.cdsp"
        else:
            len_30mb = 31457280 #35MB value
            string_search = "IMAGE_VARIANT_STRING=660.adsp"
            if TargetName == "670":
                string_search = "IMAGE_VARIANT_STRING=670.adsp"
            elif TargetName == "845":
                string_search = "IMAGE_VARIANT_STRING=845.adsp"
        file = open(input,'rb')
        file.seek(seek_offset)
        check_30mb = file.tell()
        count = 0
        cond_cdsp = False
        for line in file:
            count = count + 1
            offset = file.tell()
            if (offset-check_30mb)>=len_30mb:
                break
            if string_search in line:
                cond_cdsp = True
                print string_search
            if "*ADSP_START_ADDRESS: PA-" in line and cond_cdsp == True:
                try:
                    temp =  line.split('*ADSP_START_ADDRESS: PA-')[1].split()[0]
                    if not "%x" in temp:
                        StartAddress = temp
                        temp1 = line.split('VA-')[1].split('\x00')[0]
                        if not "%x" in temp1:
                            VirtualAddress = temp1
                            if "*" in VirtualAddress:
                                VirtualAddress = VirtualAddress.split('*')[0]
                except:
                    print "Error in Message!!"
                print StartAddress
                print VirtualAddress
                if VirtualAddress != False:
                    VA = outputpath+"\\temp\\VA.txt"
                    f2 = open(VA, 'w')
                    f2.write(VirtualAddress)
                    f2.close()
                    break
        file.close()
    else:
        len_30mb = 36700160 #35MB value
        file = open(input,'rb')
        file.seek(seek_offset)
        check_30mb = file.tell()
        count = 0
        for line in file:
            count = count + 1
            offset = file.tell()
            if (offset-check_30mb)>=len_30mb:
                break
            if "*ADSP_START_ADDRESS: PA-" in line:
                try:
                    temp =  line.split('*ADSP_START_ADDRESS: PA-')[1].split()[0]
                    if not "%x" in temp:
                        StartAddress = temp
                        temp1 = line.split('VA-')[1].split('\x00')[0]
                        if not "%x" in temp1:
                            VirtualAddress = temp1
                            if "*" in VirtualAddress:
                                VirtualAddress = VirtualAddress.split('*')[0]
                except:
                    print "Error in Message!!"
                print StartAddress
                print VirtualAddress
                if VirtualAddress != False:
                    VA = outputpath+"\\temp\\VA.txt"
                    f2 = open(VA, 'w')
                    f2.write(VirtualAddress)
                    f2.close()
                    break
        file.close()
    return StartAddress
    
def Start(inputdump,outputpath,extension,TargetName,cdsp_flag):
    list1=[0x00,0xC0,0x00,0x78,0x06,0x40,0x00,0x67]
    list2=[0x0E,0xC0,0x00,0x58,0x00,0xC0,0x00,0x58]
    output = outputpath+"\\temp\\adspstartaddr.txt"
    output2 = outputpath+"\\temp\\alignment.txt"
    string1= ""
    string2= ""
    for item in list1:
            string1=string1+chr(item)
    for item in list2:
            string2=string2+chr(item)
    inputfile = open(inputdump, 'rb')
    align_1mb = 0
    key=inputfile.read(8)
    first_time_skip = 0
    address = None
    while(key):
        if key == string1 or key == string2 or extension == ".elf" or extension == ".ELF":
            if first_time_skip == 0:
                first_time_skip = 1
                address = inputfile.tell()
            else:
                start_addr = is_adsp_start_string(inputfile,0,"ADSP_START_ADDRESS: PA-",outputpath,inputdump,inputfile.tell(),TargetName,cdsp_flag) 
                if start_addr != False:
                    f2 = open(output, 'w')
                    f2.write(start_addr)
                    f2.close()
                    f2 = open(output2, 'w')
                    i = int(start_addr,16) & 0x7FFFFFFF
                    if hex(i).replace('L','') == hex(align_1mb):
                        f2.write(str(hex(align_1mb)))
                    else:    
                        f2.write(str(hex(i).replace('L','')))
                    f2.close()
                    break
        align_1mb = align_1mb+(0x100000)
        inputfile.seek(align_1mb)
        key=inputfile.read(8)
    if first_time_skip == 1:
        if address != None:
            if not os.path.isfile(output):
                inputfile.seek(address)
                start_addr = is_adsp_start_string(inputfile,0,"ADSP_START_ADDRESS: PA-",outputpath,inputdump,inputfile.tell(),TargetName,cdsp_flag) 
                if start_addr != False:
                    f2 = open(output, 'w')
                    f2.write(start_addr)
                    f2.close()
    inputfile.close()


#Function to check for a string in the range of 1000 bytes from provided index:
def is_string(file,index,string):
    length = len(string)
    for distance in range(1000):
        file.seek(index+distance)
        if string == file.read(length):return True
    return False
    
def Ocimem_Check(input,outputpath,slpi_flag,cdsp_flag):
    ocimem_check = False
    dump_path = os.path.dirname(input)
    for r,d,f in os.walk(dump_path):
        cond = False
        for file in f:
            dump_files  = os.path.join(r,file)
            if "OCIMEM" in dump_files or "ocimem" in dump_files:
                ocimem_check = dump_files
                cond = True
                break
        if cond == True:
            break
    
    if ocimem_check != False:
        if os.path.isfile(ocimem_check):
            print "&&&&&&&&&&&&&& ",ocimem_check,cdsp_flag
            file = open(ocimem_check,'rb')
            if slpi_flag == "True":
                adsp_string = [0x73,0x6C,0x70,0x69]
            elif cdsp_flag == "True":
                adsp_string = [0x63,0x64,0x73,0x70]
            else:
                adsp_string = [0x61,0x64,0x73,0x70]
            
            adsp=""
            for item in adsp_string:
                adsp = adsp + chr(item)
            print adsp
            found = False
            address = False
            while (not found):
                key = file.read(4)
                if not key:
                    file.close()
                    return
                if key == adsp:
                    key = file.read(4)
                    s1=binascii.hexlify(file.read(1))
                    s2=binascii.hexlify(file.read(1))
                    s3=binascii.hexlify(file.read(1))
                    s4=binascii.hexlify(file.read(1))
                    address = "0x"+s4+s3+s2+s1
                    print "address ",address
                    found = True
            file.close()
                
            list1=[0x00,0xC0,0x00,0x78,0x06,0x40,0x00,0x67]
            list2=[0x0E,0xC0,0x00,0x58,0x00,0xC0,0x00,0x58]
            string= ""
            for item in list1:
                    string=string+chr(item)
            string1= ""
            for item in list2:
                    string1=string1+chr(item)
            f = open(input, 'rb')
            address_offset = "0x"+address[3:]
            offset = int(address_offset,16)
            for check in range(0,8):
                f.seek(offset)
                key=f.read(8)
                if key == string or key == string1:
                    print "Found adsp address in OCIMEM!!!"
                    output = outputpath+"\\temp\\adspstartaddr.txt"
                    f2 = open(output, 'w')
                    f2.write(address)
                    f2.close()
                    output = outputpath+"\\temp\\alignment.txt"
                    f2 = open(output, 'w')
                    f2.write(str(hex(offset)))
                    f2.close()
                offset = offset + 0x10000000
            f.close()
    return
    
input=str(sys.argv[1])
outputpath=str(sys.argv[2])
output = outputpath+"\\temp\\adspstartaddr.txt"
output1 = outputpath+"\\temp\\dumpformant.txt"
TargetName=str(sys.argv[3])
extension = os.path.splitext(input)[1]
sensor = str(sys.argv[4])
cdsp_flag = str(sys.argv[5])


f4 = open(input,"r")
data = f4.read(4)
f4.close()
if extension == "":
	if binascii.hexlify(data) == "7f454c46":
		extension = ".ELF"

address = "0x0"

if (extension == ".elf" or extension == ".ELF"):
		f1 = open(output1, 'w')
		sys.stdout = f1
		print "2"
		sys.stdout = sys.__stdout__
		f1.close()
		f4 = open(input,"r")
		data = f4.read(4)
		f4.seek(60)
		s1=binascii.hexlify(f4.read(1))
		s2=binascii.hexlify(f4.read(1))
		s3=binascii.hexlify(f4.read(1))
		s4=binascii.hexlify(f4.read(1))
		address = "0x"+s4+s3+s2+s1
		f4.close()
else:
		f1 = open(output1, 'w')
		sys.stdout = f1
		print "1"
		sys.stdout = sys.__stdout__
		f1.close()



s1 =  os.path.getsize(input)
print "Please wait, finding start address..."

DumpSize = 0
for file in possible_dump_name:
    dump_file = os.path.dirname(input)+"\\"+file
    if os.path.isfile(dump_file):
        DumpSize = DumpSize + os.path.getsize(dump_file)
print "Dump Size : ",DumpSize
Enable_6GB = False
if DumpSize > 4294967296:
    Enable_6GB = True 
    
if (TargetName == "8998" and sensor=="True") or (TargetName == "8996" and sensor=="True") or (TargetName == "845" and sensor=="True") or (Enable_6GB == True):
    if DumpSize > 4294967296:
        Ocimem_Check(input,outputpath,"True",cdsp_flag)
        ocimem_loadcmm_check = False
        dump_path = os.path.dirname(input)
        for r,d,f in os.walk(dump_path):
            cond_chk = False
            for file in f:
                dump_files  = os.path.join(r,file)
                if "load.cmm" in dump_files:
                    ocimem_loadcmm_check = dump_files
                    cond_chk = True
                    break
            if cond_chk == True:
                break
        ddr_offset = False
        if ocimem_loadcmm_check != False:
            if os.path.isfile(ocimem_loadcmm_check):
                file = open(ocimem_loadcmm_check,'r')
                for line in file:
                    file_name = input.split('\\')[-1].rstrip('\r\n')
                    if "d.load.binary " in line and file_name in line:
                        if ".BIN" in line:
                            ddr_offset = line.split('.BIN')[1].split(' ')[1]
                        elif ".bin" in line:    
                            ddr_offset = line.split('.bin')[1].split(' ')[1]
                        break
                file.close()
        if ddr_offset != False:
            file_out = open(outputpath+"\\temp\\ddroffset.txt",'w')
            file_out.write(ddr_offset)
            file_out.close()
    else:
        Ocimem_Check(input,outputpath,"True",cdsp_flag)
        if os.path.isfile(outputpath+"\\temp\\adspstartaddr.txt"):
            sys.exit(0)
    
                
if (TargetName == "8998" and sensor=="False") or TargetName == "8937" or TargetName == "8953"  or TargetName == "8976" or (TargetName == "8996" and sensor=="False") or (TargetName == "845" and sensor=="False"):
    Ocimem_Check(input,outputpath,"False",cdsp_flag)
    if os.path.isfile(outputpath+"\\temp\\adspstartaddr.txt"):
        sys.exit(0)
        

if (TargetName == "8998" and sensor=="False") or TargetName == "8937" or TargetName == "8953" or (TargetName == "8996" and sensor=="False") or TargetName == "660" or TargetName == "670" or (TargetName == "845" and sensor=="False") or TargetName == "8976":
    Start(input,outputpath,extension,TargetName,cdsp_flag)
    if os.path.isfile(outputpath+"\\temp\\adspstartaddr.txt"):
        sys.exit(0)


i=0x0
check_2_0_builds = 0
f = open(input, 'rb')
if (TargetName == "8994" or TargetName == "8996" or TargetName == "8998" or TargetName == "8992" or TargetName == "8952" or TargetName == "8953" or TargetName == "8937" or TargetName == "8976" or TargetName == "9x35") or TargetName == "660" or TargetName == "670" or TargetName == "845":
    key=f.read(28)
else:
    key=f.read(32)
f.seek(0x40)
key1=f.read(4)
if (TargetName == "8994" or TargetName == "8996" or TargetName == "8998" or TargetName == "8992" or TargetName == "8952" or TargetName == "8953" or TargetName == "8937" or TargetName == "8976" or TargetName == "9x35") or TargetName == "660" or TargetName == "670" or TargetName == "845":
    list=[0x00,0xc0,0x00,0x78,0x06,0x40,0x00,0x67,0x00,0xc5,0x0,0x78,0x12,0xc0,0x00,0x67,0x00,0xc0,0x00,0xa2,0x00,0xd0,0xc0,0x56,0x02,0xc0,0xc0,0x57]
    list2=[0x0E,0xC0,0x00,0x58,0x00,0xC0,0x00,0x58,0x00,0xC0,0x0,0x58,0x00,0xC0,0x00,0x58,0x00,0xc0,0x00,0x58,0x00,0xC0,0x00,0x58,0x0,0xC0,0x00,0x58]
else:
    list=[0x00,0xc0,0x00,0x78,0x06,0x40,0x00,0x67,0x00,0xc5,0x0,0x78,0x12,0xc0,0x00,0x67,0x00,0xc0,0x00,0xa2,0x00,0xd0,0xc0,0x56,0x02,0xc0,0xc0,0x57,0xf2,0xc1,0x00,0x58]
    list2=[0x0E,0xC0,0x00,0x58,0x00,0xC0,0x00,0x58,0x00,0xC0,0x0,0x58,0x00,0xC0,0x00,0x58,0x00,0xc0,0x00,0x58,0x00,0xC0,0x00,0x58,0x0,0xC0,0x00,0x58,0x0,0xC0,0x00,0x7f]
list1=[0x45,0x49,0x50,0x32]
string= ""
string1= ""
string2= ""


for item in list:
		string=string+chr(item)
for item in list1:
		string1=string1+chr(item)
for item in list2:
		string2=string2+chr(item)

if sensor=="False":
    if (key1 == string1 and key == string) or (key1 == string1 and key == string2):
        if sensor=="True": found = is_string(f,0,"SENSOR.pbn") 
        else: found = True
    else: found = False
else:
    if (key1 == string1 or key == string) or (key1 == string1 or key == string2):
        if sensor=="True": found = is_string(f,0,"SENSOR.pbn") 
        else: found = True
    else: found = False
    
if (extension == ".elf" or extension == ".ELF"):
    print extension
else:
    while (not found):
        if (TargetName == "9x35"):
            i=i+(0x80000)
        else:
            i=i+(0x100000)
        f.seek(i)
        if (TargetName == "8994" or TargetName == "8996" or TargetName == "8998" or TargetName == "8992" or TargetName == "8952" or TargetName == "8953" or TargetName == "8937" or TargetName == "8976" or TargetName == "9x35") or TargetName == "660" or TargetName == "670" or TargetName == "845":
            key=f.read(28)
        else:
            key=f.read(32)
        k=i+0x40
        f.seek(k)
        key1=f.read(4)
        if sensor=="False":
            if (key1 == string1 and key == string) or (key1 == string1 and key == string2):
                if TargetName == "8996" or TargetName == "8998" or TargetName == "845":
                    found_sensor = is_string(f,k,"SENSOR.pbn")
                    found_pbn = is_string(f,k,"1234.pbn")
                    if (sensor=="True" and found_sensor == True): found = True
                    elif (sensor=="False" and found_sensor == False): 
                        if found_pbn:
                            found = True 
                    else: found = False
                else: found = True 
        else:
            if (key1 == string1 or key == string) or (key1 == string1 or key == string2):
                if TargetName == "8996" or TargetName == "8998" or TargetName == "845":
                    found_sensor = is_string(f,k,"SENSOR.pbn")
                    found_pbn = is_string(f,k,"1234.pbn")
                    if (sensor=="True" and found_sensor == True): found = True
                    elif (sensor=="False" and found_sensor == False): 
                        if found_pbn:
                            found = True 
                    else: found = False
                else: found = True 
        if i >= s1:
            # print "ADSP start address not found in the Dump"
            check_2_0_builds = 1
            break
        if (found == True):
            if ((TargetName == "8996" and sensor=="True")  or (TargetName == "8998" and sensor=="True") or (TargetName == "845" and sensor=="True") or(Enable_6GB == True)):
                if DumpSize <= 4294967296:
                    i = i | 0x80000000
                else:
                    if os.path.isfile(outputpath+"\\temp\\adspstartaddr.txt"):
                        f2 = open(outputpath+"\\temp\\alignment.txt", 'w')
                        f2.write(str(hex(i)).replace('L',''))
                        f2.close()
                        sys.exit()
                    else:
                        f2 = open(outputpath+"\\temp\\alignment.txt", 'w')
                        f2.write(str(hex(i)).replace('L',''))
                        f2.close()
                        if not os.path.isfile(outputpath+"\\temp\\adspstartaddr.txt"):
                            if os.path.isfile(outputpath+"\\temp\\ddroffset.txt"):
                                f2 = open(outputpath+"\\temp\\ddroffset.txt", 'r')
                                for line in f2:
                                    offset = int(line,16)
                                    print "offset ",offset
                                    break
                                i = i + offset
                                f2.close()
                                f2 = open(outputpath+"\\temp\\adspstartaddr.txt", 'w')
                                f2.write(str(hex(i)).replace('L',''))
                                f2.close()
                        sys.exit()
            elif (TargetName == "8996" or TargetName == "8998" or TargetName == "8976"  or TargetName == "8952"   or TargetName == "8953"  or TargetName == "8937") or TargetName == "660" or TargetName == "670" or TargetName == "845": 
                print "i ",i
                if TargetName == "8996" or TargetName == "8998" or TargetName == "845":
                    f2 = open(outputpath+"\\temp\\alignment.txt", 'w')
                    f2.write(str(hex(i)).replace('L',''))
                    f2.close()
                    i = i & 0x0FFFFFFF
                    i = i | 0x80000000
                        
                else:
                    i = i | 0x80000000


f.close()
start_address_trial_list= [ 0xdc00000, 0xbf00000, 0xc500000, 0x6400000, 0x2000000, 
                            0x1300000, 0x5a00000, 0xdb00000, 0xd800000, 0xc300000,   
                            0xc200000, 0xc700000, 0x6f00000, 0x6200000, 0xc800000] 
if check_2_0_builds == 1:
    exit = 1
    f_check = open(input, 'rb')
    for list in start_address_trial_list:
        f_check.seek(list)
        key=f_check.read(32)
        if key == string:
            i = list
            exit = 0
            break
    f_check.close()
    if exit == 1:
        sys.exit()

    

f4 = open(input,"rb")
data = f4.read(4)
if TargetName == "8994" or TargetName == "8992" or TargetName == "8952"   or TargetName == "8953"  or TargetName == "8937" or TargetName == "8976" or (TargetName == "8996" and sensor =="True") or (TargetName == "8998" and sensor =="True") or TargetName == "660" or TargetName == "670" or (TargetName == "845" and sensor =="True"):
    VAAddr="000010f0"
    VAAddr1="000010b0"
else:
    VAAddr="000000f0"
    VAAddr1="000000b0"
if i == 0 and (extension != ".elf" and extension != ".ELF"):
    while binascii.hexlify(data)!=VAAddr and binascii.hexlify(data)!=VAAddr1:
		data=f4.read(4)
    else:
        if binascii.hexlify(data)==VAAddr or binascii.hexlify(data)==VAAddr1:
            s1=binascii.hexlify(f4.read(1))
            s2=binascii.hexlify(f4.read(1))
            s3=binascii.hexlify(f4.read(1))
            s4=binascii.hexlify(f4.read(1))
            address = "0x"+s4+s3+s2+s1
        else:
            print "Not found !"
f4.close()
f2 = open(output, 'w')
sys.stdout = f2
if address == "0x0":
	print str(hex(i)).replace('L','')
else:
	print address
sys.stdout = sys.__stdout__
f2.close()


