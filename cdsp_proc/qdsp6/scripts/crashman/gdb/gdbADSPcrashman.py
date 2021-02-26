import subprocess
import os
import sys

def TimeStampCheck(filename,elf_name,Target_name,output_folder,CurrDirectory,start_address):
    if Target_name == "8996" or Target_name == "8998":
        command = '''python %s\strings_sensor.py %s %s %s '''%(CurrDirectory[:-5],filename,output_folder+"\\temp\\\DumpTimeStamp.txt",start_address)
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
        command = '''python %s\strings_sensor.py %s %s %s'''%(CurrDirectory[:-5],elf_name,output_folder+"\\temp\\\DumpTimeStamp.txt",0)
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
        command = '''python %s\TimeStampCheck_slpi.py %s '''%(CurrDirectory[:-5],output_folder+"\\temp")
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
    else:
        command = '''python %s\strings.py %s %s '''%(CurrDirectory[:-5],filename,output_folder+"\\temp\\\DumpTimeStamp.txt")
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
        command = '''python %s\strings.py %s %s '''%(CurrDirectory[:-5],elf_name,output_folder+"\\temp\\\DumpTimeStamp.txt")
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
        command = '''python %s\TimeStampCheck.py %s '''%(CurrDirectory[:-5],output_folder+"\\temp")
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
    status = False
    if os.path.isfile(output_folder+"\\temp\\\CheckDumpStatus.txt"):
        file = open(output_folder+"\\temp\\\CheckDumpStatus.txt",'r+')
        status = file.readline()
        if status == "1":
            status = True
    return status
    
def GetOffset(sensor_elf,output_folder):
    command = '''qdsp6-readelf -t %s > %s\\temp\\elfheader.txt'''%(sensor_elf,output_folder)
    os.system(command) 
    
    offset = None
    try:
        elf = output_folder + "\\temp\\elfheader.txt"
        f_elf = open(elf,'r+')
        for line in f_elf:
            if ".text" in line and not ".text." in line:
                for line1 in f_elf:
                    offset = line1.split('PROGBITS        ')[1].split(' ')[0]
                    break
        f_elf.close()
    except:
        print "Unable to process elf header!!!"
    
    return offset

def write_gdb_starter(elf_file,sensor_elf,output_folder,CurrDirectory,sofile_path,filename):
        f=open(output_folder+"\\temp\\gdb_starter.txt",'w')
        f.write("set tcp connect-timeout 900\n")
        f.write("target remote localhost:9010\n")
        f.write("symbol-file "+elf_file.replace('\\','\\\\')+"\n")
        if sensor_elf != " ":
            offset = GetOffset(sensor_elf,output_folder)
            if offset != None:
                f.write("add-symbol-file "+sensor_elf.replace('\\','\\\\')+" 0x"+str(offset)+"\n")
        f.write('python output_folder = "'+output_folder.replace('\\','\\\\')+'"\n')
        f.write('python sofile_path = "'+sofile_path.replace('\\','\\\\')+'"\n')
        f.write('python input_dump_path = "'+filename.replace('\\','\\\\')+'"\n')
        f.write('python cwd = "'+CurrDirectory.replace('\\','\\\\')+'"\n')
        f.write("python execfile('"+CurrDirectory.replace('\\','\\\\')+"signature.py')\nquit\ny\n")
        f.close()
        
F010 = ['8994','8996','8998','8992','8952','8953','8937']        
        
if (len(sys.argv)!=5 and len(sys.argv)!=6) and (len(sys.argv)!=7)  and (len(sys.argv)!=8)  and (len(sys.argv)!=9):  
    print "Usage: python gdbADSPcrashman.py Target_Name[8974/9x25/8x10] EBI_Binary_File Output_folderpath CRM_Buildpath CustomerprovidedOBJPath[Optional]"
    sys.exit()

if len(sys.argv)==5: sensor_elf_file=sys.argv[4]
else: sensor_elf_file="NA"

#check for valid target
target = ["8x10","8974","8926","8x26","9x35","8084","8994","8996","8998","9x25","8910","8610",'8992','8952','8953','8937']

#Creating elf name list from target name
elf_name_list = {'8x10':'M8x10AAAAAAAAQ1234.elf','8610':'M8x10AAAAAAAAQ1234.elf','8910':'M8x10AAAAAAAAQ1234.elf','8926':'M8x26AAAAAAAAQ1234.elf',
            '8x26':'M8x26AAAAAAAAQ1234.elf','8994':'M8994AAAAAAAAQ1234.elf','8996':'M8996AAAAAAAAQ1234.elf','8998':'M8998AAAAAAAAQ1234.elf',
            '9x35':'M9x35AAAAAAAAQ1234.elf','9x25':'M9x25AAAAAAAAAQ1234.elf','8974':'M8974AAAAAAAAQ1234.elf',
            '8084':'M8084AAAAAAAAQ1234.elf','8992':'M8992AAAAAAAAQ1234.elf','8952':'M8952AAAAAAAAQ1234.elf','8953':'M8953AAAAAAAAQ1234.elf','8937':'M8937AAAAAAAAQ1234.elf'
            }

            
sensor_elf_name_list = {'8x10':'M8x10AAAAAAAAQ1234_SENSOR.elf','8610':'M8x10AAAAAAAAQ1234_SENSOR.elf','8910':'M8x10AAAAAAAAQ1234_SENSOR.elf','8926':'M8x26AAAAAAAAQ1234_SENSOR.elf',
            '8x26':'M8x26AAAAAAAAQ1234_SENSOR.elf','8994':'M8994AAAAAAAAQ1234_SENSOR.elf','8996':'M8996AAAAAAAAQ1234_SENSOR.elf','8998':'M8998AAAAAAAAQ1234_SENSOR.elf',
            '9x35':'M9x35AAAAAAAAQ1234_SENSOR.elf','9x25':'M9x25AAAAAAAAAQ1234_SENSOR.elf','8974':'M8974AAAAAAAAQ1234_SENSOR.elf',
            '8084':'M8084AAAAAAAAQ1234_SENSOR.elf','8992':'M8992AAAAAAAAQ1234_SENSOR.elf','8952':'M8952AAAAAAAAQ1234_SENSOR.elf','8953':'M8953AAAAAAAAQ1234_SENSOR.elf','8937':'M8937AAAAAAAAQ1234_SENSOR.elf'
            }

            
CurrDirectory =  " "
curdir = sys.argv[0]
if "\\" in curdir or "//" in curdir:
    CurrDirectory = curdir[:-18]
Target_name = sys.argv[1]
if Target_name not in target:
    print "Target name not proper..try again\n"
    print "Correct Format is:  <start_address> <Dump> <ELF> <Sensor ELF>[Optional]"
    sys.exit()
    
filename = sys.argv[2]
output_folder=sys.argv[3]
build_path = sys.argv[4]

#Generate elfs name from path
elf_name=os.path.join(build_path,"\\build\\ms",elf_name_list[Target_name])
sensor_elf_file=os.path.join(build_path,"\\build\\ms",sensor_elf_name_list[Target_name])


if len(sys.argv)>=6:
    elf_path=sys.argv[5]
    elf_name=os.path.join(elf_path,elf_name_list[Target_name])
    sensor_elf_file=os.path.join(elf_path,sensor_elf_name_list[Target_name])
    smmu_enable = str(sys.argv[8])
    start_address = str(sys.argv[7])
    sofile_path = str(sys.argv[6])
    
if not os.path.isfile(filename):
    print "Dump Filename given does not exist!"
    sys.exit()

if not os.path.isfile(elf_name):
    print "Elf file %s given does not exist!"%elf_name
    sys.exit()

if not os.path.isfile(sensor_elf_file):
    sensor_elf_file=False



#Calculate start address
# start_address = start_address(filename,Target_name)
print "\nChecking for timestamp. Please wait..."
print "\n************************************"
# timestamp=time_stamp_check(filename,elf_name,sensor_elf_file,start_address)
timestamp = TimeStampCheck(filename,elf_name,Target_name,output_folder,CurrDirectory,start_address)
# print timestamp
if not sensor_elf_file: sensor_elf_file=" "

if timestamp == False:
    print "TimeStamp Match Failed..!!!"
    sys.exit()
else: print "TimeStamp Check Passed !!!"
print "************************************\n"

#If output folder doesn't exist, create one
if not os.path.exists(output_folder):
    os.makedirs(output_folder)


#Changing VA start according to target    
if Target_name in F010: va_start="0xf0100000"
else: va_start="0xf0000000"



# print "start: "+start_address
size="0x1400000"
write_gdb_starter(elf_name,sensor_elf_file,output_folder,CurrDirectory,sofile_path,filename)

#print Target_name,filename,start_address,elf_name,sensor_elf_file
if Target_name == "8952" or Target_name == "8976" or Target_name == "8953" or Target_name == "8937":
    command = CurrDirectory+"crash_sim.exe"+" -G "+"9010 "+"-dump "+filename+" 0x80000000 "+"0x1FFFFFFF "+"-dpage "+va_start+" "+start_address+" "+size+" -asid "+"0x0 "+"-elf "+elf_name
elif Target_name == "8996" or Target_name == "8998":
    command = CurrDirectory+"crash_sim.exe"+" -G "+"9010 "+"-dump "+filename+" 0x80000000 "+"0x1FFFFFFF "+"-dpage "+va_start+" "+start_address+" "+size+" -asid "+"0x0 "+"-elf "+elf_name+" --lldb"
else:    
    command = CurrDirectory+"crash_sim.exe"+" -G "+"9010 "+"-dump "+filename+" 0x0 "+"0x1FFFFFFF "+"-dpage "+va_start+" "+start_address+" "+size+" -asid "+"0x0 "+"-elf "+elf_name
# print command
if smmu_enable=="True":
    if Target_name == "8952" or Target_name == "8976" or Target_name == "8953" or Target_name == "8937":
        command = CurrDirectory+"crash_sim.exe"+" -G "+"9010 "+"-dump "+filename+" 0x80000000 "+"0x3FFFFFFF "+"-dump "+filename.replace('DDRCS0','DDRCS1')+" 0xC0000000 "+"0x3FFFFFFF "+"-dpage "+va_start+" "+start_address+" "+size+" -asid "+"0x0 "+"-elf "+elf_name
    elif Target_name == "8996" or Target_name == "8998":
        command = CurrDirectory+"crash_sim.exe"+" -G "+"9010 "+"-dump "+filename+" 0x80000000 "+"0x1FFFFFFF "+"-dpage "+va_start+" "+start_address+" "+size+" -asid "+"0x0 "+"-elf "+elf_name+" --lldb"
    else:    
        command = CurrDirectory+"crash_sim.exe"+" -G "+"9010 "+"-dump "+filename+" 0x0 "+"0x1FFFFFFF "+"-dpage "+va_start+" "+start_address+" "+size+" -asid "+"0x0 "+"-elf "+elf_name
    command_pgt=command+" -listpage "+output_folder+"\\temp\\DSP_pgt.txt"
    # print command_pgt
    process = subprocess.Popen(command_pgt, stdout=subprocess.PIPE)
    process.wait() 
    command_smmu = '''python %s\ADSP_SMMU.py %s'''%(CurrDirectory[:-5],output_folder+"\\temp")
    # print command_smmu
    process = subprocess.Popen(command_smmu, stdout=subprocess.PIPE)
    process.wait() 
    command = command+" -page "+output_folder+"\\temp\\DSP_smmu_pagetables.cmm"
    # print command
    os.system("start cmd /c "+command)
    
else:    
    os.system("start cmd /c "+command)
#p1 = subprocess.Popen(command)
#p2= subprocess.Popen("hexagon-gdb -q -x gdb_starter.txt")

command="hexagon-gdb -q -x "+output_folder+"\\temp\\gdb_starter.txt"
os.system("start cmd /c "+command)
print "\n\n********************************************************************"
print "Ouput Logs are Shared @ "+output_folder
print "********************************************************************\n\n"
