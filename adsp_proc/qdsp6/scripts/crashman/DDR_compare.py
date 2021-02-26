import sys
import os
import subprocess
import filecmp
import binascii
from filecmp import dircmp
import datetime

List_Of_Sections = ['.rodata ', \
                    '.text '    ]

Tools_Location      = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\Hexagon_Tools\\5.1.05\\gnu\\bin\\"
Tools_Location_LLVM = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\Hexagon_Tools\\7.2.09\\Tools\\bin\\"

def Check_DDR_ELF(TargetName,ELF,Output,ADSP_BUILD,type):  
    # print TargetName,ELF,Output,type
    readelf = None
    if TargetName == "8996" or TargetName == "8998" or TargetName == "660":
        readelf    = Tools_Location_LLVM+"hexagon-readelf"
    else:
        readelf    = Tools_Location+"hexagon-readelf"
        
    #print  "elf start",datetime.datetime.now()
    if ".\\build\\ms" in ELF:
        ELF = ADSP_BUILD + '\\build\\ms\\'+ELF.split('\\')[-1]
    # print ELF
    
    if not os.path.isfile(ELF):
        print "ELF File is not accessible"
        return
    
    command = '''%s -S %s'''%(readelf,ELF)
    Content = subprocess.Popen(command, stdout=subprocess.PIPE).communicate()[0]
    # #print  Content
    #print  "elf processs ",datetime.datetime.now()
    if type == "memcpy":
        file_read  = open(ELF,'rb')
        file_write = open(Output+"\\DDR\\ELF\\"+type+".bin","wb")
        file_read.seek(0x7e90)
        file_write.write(file_read.read(0x2d1))
        file_read.close()
        file_write.close()
        return
    for key in List_Of_Sections:
        Offsets =  Content[Content.find(key):].split('\n')[0].split()
        if Offsets:
            key = key.replace(' ','')
            file_read  = open(ELF,'rb')
            file_write = open(Output+"\\DDR\\ELF\\"+type+key+".bin","wb")
            print Offsets,key
            print  Offsets[3],Offsets[4]
            file_read.seek(int(Offsets[3],16))
            file_write.write(file_read.read(int(Offsets[4],16)))
            file_read.close()
            file_write.close()
    #print  "elf end",datetime.datetime.now()
    
    
def Check_DDR_RAMDUMP(TargetName,ELF,RamDump,Output,StartAddr,ADSP_BUILD,type):
    Check_SymList = Output + "\\temp\\SymbolList.txt"
    if TargetName == "660":
        Ext_elfname = ELF.split('\\')[-1].split('.')[0]
    else:
        Ext_elfname = "1234"+ELF.split('\\')[-1].split('.')[0].split('1234')[1]
    start_address=int(StartAddr,0)
    address=start_address
    if type == "memcpy":
        file_write = open(Output+"\\temp\\ddrsave.cmm","a")
        file_write.write("d.save.binary "+Output+"\\DDR\\RAMDUMP\\"+type+".bin 0xf0006e90++0x2d0\n")
        file_write.close()
        return
    if start_address>0x80000000:
        address=start_address-0x80000000
    if os.path.isfile(Check_SymList):
        for key in List_Of_Sections:
            Ext_sectname = Ext_elfname+"\\"+key
            print  Ext_sectname
            file = open(Check_SymList,'r')
            for line in file:
                if Ext_sectname in line:
                    print line
                    start_addr = line.split(':')[1].split('--')[0]
                    end_addr   = line.split(':')[1].split('--')[1].split('*')[0]
                    size = int(end_addr,16) - int(start_addr,16)
                    file_write = open(Output+"\\temp\\ddrsave.cmm","a")
                    key = key.replace(' ','')
                    file_write.write("d.save.binary "+Output+"\\DDR\\RAMDUMP\\"+type+key+".bin 0x"+start_addr+"++("+"0x"+end_addr+"-0x"+start_addr+")\n")
                    file_write.close()
                    break
            file.close()
        
        
        
def GetInputsFromCrashmanOutput(Output):
    RamDump    = None
    TargetName = None
    StartAddr  = None
    ELF        = None
    AUDIO_ELF  = None
    SENSOR_ELF = None
    ADSP_BUILD = None
    
    Check_Log =  Output + "\\temp\\Crashman_Log.txt"
    if not os.path.isfile(Check_Log):
        Check_Log = Output + "\\Crashman_Log.txt"

    if os.path.isfile(Check_Log):
        file = open(Check_Log,'r')
        for line in file:
            if "Dump file Provided   :   " in line:
                RamDump = line.split('Dump file Provided   :   ')[1].rstrip('\n')
            elif "ADSP build Provided  :   " in line:
                ADSP_BUILD = line.split('ADSP build Provided  :   ')[1].rstrip('\n')
            elif "Target Name Provided :   " in line:
                TargetName = line.split('Target Name Provided :   ')[1].rstrip('\n')
            elif "ADSP Start Address: " in line:
                StartAddr = line.split('ADSP Start Address: ')[1].rstrip('\n')
            elif "Loading ELF: " in line:
                if "1234.elf" in line or "ROOT_660.adsp.prodQ.elf" in line or "ROOT_660.cdsp.prodQ.elf" in line:
                    ELF = line.split('Loading ELF: ')[1].rstrip('\n')
                elif "AUDIO.elf" in line or "AUDIO_660.adsp.prodQ.elf" in line or "AUDIO_660.cdsp.prodQ.elf" in line:
                    AUDIO_ELF = line.split('Loading ELF: ')[1].rstrip('\n')
                elif "SENSOR.elf" in line or "SENSOR_660.adsp.prodQ.elf" in line or "SENSOR_660.cdsp.prodQ.elf" in line:
                    SENSOR_ELF = line.split('Loading ELF: ')[1].rstrip('\n')
        file.close()
        
    return RamDump,TargetName,StartAddr,ELF,AUDIO_ELF,SENSOR_ELF,ADSP_BUILD

def print_diff_files(ELF,RAMDUMP,Output):
    dirs = os.listdir(ELF)
    out = Output + "\\DDR\\Result.txt"
    f_out = open(out,'w')
    for file in dirs:
        elf_file = ELF + file
        ram_file = RAMDUMP + file
        print elf_file
        if os.path.isfile(elf_file) and os.path.isfile(ram_file):
            if filecmp.cmp(elf_file,ram_file):
                print file + " --> MATCHED."
                f_out.write(file + " --> MATCHED.\n")
            else: 
                print file + " --> NOT MATCHED !!"
                f_out.write(file + " --> NOT MATCHED.\n")
    f_out.close()
        
def main():
    Output = sys.argv[1]
    Check  = sys.argv[2]
    
    if Check == "Compare":
        print_diff_files(Output+"\\DDR\\ELF\\", Output+"\\DDR\\RAMDUMP\\",Output)    
        
    if Check == "Process":
        #print  datetime.datetime.now()
        RamDump,TargetName,StartAddr,ELF,AUDIO_ELF,SENSOR_ELF,ADSP_BUILD = GetInputsFromCrashmanOutput(Output)
        # #print  RamDump,TargetName,StartAddr,ELF,AUDIO_ELF,SENSOR_ELF
        if not os.path.exists(Output+"\\DDR"):
            os.mkdir(Output+"\\DDR")
        if not os.path.exists(Output+"\\DDR\\ELF"):
            os.mkdir(Output+"\\DDR\\ELF")
        if not os.path.exists(Output+"\\DDR\\RAMDUMP"):
            os.mkdir(Output+"\\DDR\\RAMDUMP")

        #print  datetime.datetime.now()
        Check_DDR_ELF(TargetName,ELF,Output,ADSP_BUILD,"")
        #print  datetime.datetime.now()
        Check_DDR_RAMDUMP(TargetName,ELF,RamDump,Output,StartAddr,ADSP_BUILD,"")
        #print  datetime.datetime.now()
        if AUDIO_ELF != None:
            Check_DDR_ELF(TargetName,AUDIO_ELF,Output,ADSP_BUILD,"audio")
            Check_DDR_RAMDUMP(TargetName,AUDIO_ELF,RamDump,Output,StartAddr,ADSP_BUILD,"audio")
        #print  datetime.datetime.now()
        if SENSOR_ELF != None:
            Check_DDR_ELF(TargetName,SENSOR_ELF,Output,ADSP_BUILD,"sensor")
            Check_DDR_RAMDUMP(TargetName,SENSOR_ELF,RamDump,Output,StartAddr,ADSP_BUILD,"sensor")
if __name__ == '__main__':
  main()

