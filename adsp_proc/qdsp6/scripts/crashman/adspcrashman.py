# ::*****************************************************************************************************************************************************************
# :: Filename: adspcrashman.py
# ::
# :: Version : External_Release_1.44
# ::
# :: Usage:
# ::    adspcrashman.py -t Target_Name[8994/8974/9x25/8x10/9x35/8084] -b EBI_Binary_File -o Output_folderpath -c CRM_Buildpath -e CustomerprovidedOBJPath[Optional]
# ::
# :: Example:
# ::    adspcrashman.py  -t 8974 -b C:\Dropbox\DDRSCO.bin -o C:\dropbox\123456 -c C:\YUG_M8960AAAAANAZL1047\lpass_proc -e C:\Dropbox\8k_dumps\
# ::
# ::*****************************************************************************************************************************************************************
import os
import re
import subprocess
import sys
import time
import datetime
import fnmatch
import shutil, errno
import socket
import binascii

VALID_TARGETS=["8994","8974","9x25","9x35","8x10","8084","8x26","8996","8998","8952","8992","8976","8953","8937","8917","8940","660","670","845"]
possible_dump_name=["DDRCS0.BIN","DDRCS1.BIN","DDRCS0_0.BIN","DDRCS0_1.BIN","DDRCS0_2.BIN","DDRCS1_0.BIN","DDRCS1_1.BIN","DDRCS1_2.BIN","ADSP0.bin","EBICS0.RAM","dram_0x80000000--0xffffffff.lst","dram1_0x100000000--0x17fffffff.lst","dram_0x40000000--0xffffffff.lst","dram1_0x100000000--0x1bfffffff.lst"]

usage = "\n\
python adspcrashman.py -t[arget] <TARGET> -d[ump] <DumpFile> -o[utput] <Output_Path> -b[uild] <CRM_Buildpath> -e[lf] <CustomerprovidedOBJPath[Optional]>\n\n\
Mandatory Arguments:\n \
  -t, --target    Targets Supported on Crashman \n \
  -d, --dump      Dump File along with path \n \
  -o, --output    Output folder \n \
  -b, --build     ADSP Build Path Location\n\n\
Optional Arguments:\n\
   -e, --elf              Customer ELF Location \n\
   -gdb,--gdb             GDB based crashman\n\
   -lite, --lite          Crashman Lite Version(Only Dumps are loaded)\n\
   -fullload, --fullload  Loads the complete RAM Dumps\n\
   -sofile, --sofile      Provide dynamic So files location. Presently Supports only 8952/8953/8937/8976/8996/8998/8910\n\
   -use_t32,--use_t32     Use T32 from a shared location\n\
   -smmu32, --smmu32      Provide 32 bit vmlinux path\n\
   -smmu64, --smmu64      Provide 64 bit vmlinux path\n\
   -load_etm, --load_etm  Enable etm \n\
   -vmlinux32,--vmlinux32 Provide 32 bit vmlinux path\n\
   -vmlinux64,--vmlinux64 Provide 64 bit vmlinux path\n\
\n\nExample:\n\n\
  python adspcrashman.py -t 8994 -d C:\DDRCS0.BIN -o C:\output -b \\\\snowcone\\builds698\\PROD\\ADSP.8994.2.6.1-00120-00374-1\\adsp_proc\n\n\
Example with -elf argument: \n\n\
  python adspcrashman.py  -target 9x35 -dump C:\DDRCS0.BIN -output C:\output -build \\\\cone\\builds634\\PROD\\ADSP.BF.2.4.1.c1-00021-M9635AAAAANAZL-1 -elf \\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\Users\\Praveen\\Testing\9x35\\ELFS\n\n\
Example with -fullload argument: \n\n\
  python adspcrashman.py -t 8952 -o C:\Temp -d \\lab5385\TriageTeam\Crashman\TestSuite\8952\mser_171\DDRCS0.BIN -b \\snowcone\builds677\PROD\ADSP.8952.2.6-00107-00000-1 -elf \\lab5385\TriageTeam\Crashman\TestSuite\8952\mser_171 -fullload \\lab5385\TriageTeam\Crashman\TestSuite\8952\mser_171\n\n\
Example with -sofile argument: \n\n\
  python adspcrashman.py -t 8910  -d \\lab5385\TriageTeam\Crashman\TestSuite\8910\evernote_dead_ramdump_1\DDRCS0.BIN -o c:\output -b \\chronicle\zipbuild225\PROD\ADSP.BF.2.2.1-00023-M8610AAAAAAAZL-1 -elf \\lab5385\TriageTeam\Crashman\TestSuite\8910\jpege_CAMERA_JPEGE_01_01_07 -sofile \\lab5385\TriageTeam\Crashman\TestSuite\8910\jpege_CAMERA_JPEGE_01_01_07\n\n \
Example with -use_t32 argument: \n\n\
  python adspcrashman.py  -target 9x35 -dump C:\DDRCS0.BIN -output C:\output -build \\\\cone\\builds634\\PROD\\ADSP.BF.2.4.1.c1-00021-M9635AAAAANAZL-1 -e \\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\Users\\Praveen\\Testing\9x35\\ELFS -use_t32 \\\\rover\\hyd_dspfw\\ADSP_Tools\\T32 \n\n\
Example with -smmu64 argument: \n\n\
  python adspcrashman.py -t 8952 -o C:\Temp -d \\\\lab3079\\Sahara\\matting_171\\DDRCS0.BIN -b \\\\snowcone\\builds677\\PROD\\ADSP.8952.2.6-00107-00000-1 -smmu64 \\\\holi\\builds539\\TEST\\LA.BR.1.3.1-27101-8952.1-1\\LINUX\\android\\out\\target\\product\\msm8952_64\\obj\\KERNEL_OBJ\\vmlinux\n\n \
Example with -load_etm argument: \n\n\
  python adspcrashman.py -t 8952 -o C:\Temp -d \\\\lab5385\\TriageTeam\\Crashman\\TestSuite\\8952\\Port_COM82_log2ETM\\DDRCS0.BIN -b \\\\snowcone\\builds732\\INTEGRATION\\ADSP.8952.2.6-00137-00000-1 -load_etm  -vmlinux64 \\\\diwali\\builds534\\TEST\\LA.BR.1.3.1-34103-8952.1-5\\LINUX\\android\\out\\target\\product\\msm8952_64\\obj\\KERNEL_OBJ\\vmlinux\n\n \
Example with -linux_ramdump_parser argument for External Customers: \n\n\
  python adspcrashman.py -t 8976 -o C:\Temp -d \\lab5385\TriageTeam\Crashman\TestSuite\8952\fastcv\DDRCS0.BIN -e \\lab5385\TriageTeam\Crashman\TestSuite\8952\fastcv -smmu64 \\lab5385\TriageTeam\Crashman\TestSuite\8952\fastcv\vmlinux_8976 -b \\chronicle\zipbuild222\PROD\ADSP.8976.2.6-00087-00000-1 -linux_ramdump_parser \\rover\hyd_dspfw\ADSP_Tools\TriageTeam\Users\linux-ramdump-parser-v2\n\n \
"


hostname = socket.getfqdn()
customerflag = False
hydserver = True

if not "qualcomm.com" in hostname:
    customerflag = True
elif "ap.qualcomm.com" in hostname:
    hydserver = True
else:    
    hydserver = False

    
if hydserver==True:
    ramparse_path = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\linux-ramdump-parser-v2"
    T32_path = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\T32"
    DDModular_Script = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\DD_Modular_Script"
else:    
    ramparse_path = "\\\\hulk\\vocoder_appdsp5\\users\\Crashman\\linux-ramdump-parser-v2"
    T32_path = "\\\\hulk\\vocoder_appdsp5\\users\\Crashman\\T32"
    DDModular_Script = "\\\\hulk\\vocoder_appdsp5\\users\\Crashman\\DD_Modular_Script"
    if os.path.exists("\\\\10.23.101.14\\vocoder_appdsp5\\users\\Crashman"):
        ramparse_path = "\\\\10.23.101.14\\vocoder_appdsp5\\users\\Crashman\\linux-ramdump-parser-v2"
        T32_path = "\\\\10.23.101.14\\vocoder_appdsp5\\users\\Crashman\\T32"
        DDModular_Script = "\\\\10.23.101.14\\vocoder_appdsp5\\users\\Crashman\\DD_Modular_Script"
    
    if not os.path.exists("\\\\10.23.101.14\\vocoder_appdsp5\\users\\Crashman"):
        if not os.path.exists("\\\\hulk\\vocoder_appdsp5\\users\\Crashman"):
            print "\\\\hulk\\vocoder_appdsp5\\users\\Crashman path is not accessible.Please get the access permission from ITCS.\n"
            if os.path.exists("\\\\rover\\hyd_dspfw\\ADSP_Tools\\T32"):
                print "Meanwhile, it is using scripts from QIPL location.\n"
                ramparse_path = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\linux-ramdump-parser-v2"
                T32_path = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\T32"
                DDModular_Script = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\DD_Modular_Script"
            else:
                print "\\\\rover\\hyd_dspfw\\ADSP_Tools\\T32 T32 is not accessible. Please have local T32 or provide T32 path with -use_t32 <T32_path> to crashman command."
    
    
if len(sys.argv) <= 1:
    print usage
    sys.exit()
    
CurrDirectory =  os.getcwd()
curdir = sys.argv[0]
if "\\" in curdir or "//" in curdir:
    CurrDirectory = curdir[:-16]

if CurrDirectory[-1]== "\\":
    CurrDirectory=CurrDirectory[:len(CurrDirectory)-1]

Crashman_Version = "External_Release_1.44"
print "************************************"
print "*******Crashman Started*************"
print "************************************"
print "Crashman version          : ",Crashman_Version

try:
   import argparse
   from argparse import RawTextHelpFormatter
except ImportError:
   print 'Python Version is: ' + sys.version
   print 'Crashman requires Python version 2.7.6 and above.'
   print 'If you have Python version 2.7.6 installed, please check the environment path.' 
   sys.exit(0)
   
   
if sys.version:
   print "Python Version Installed  : ", ((sys.version.rstrip('\n')).split('(default')[0]).split(' ')[0]
   if sys.version_info[0] != 2 and sys.version_info[1] != 7:
       print "ERROR:: You are not using Python 2.7.6. Please use 2.7.6, preferably 2.7.6 and above"
       sys.exit(0)
else:
   print '\n\nERROR: Python not installed!!!'
   print 'Recommended Python Installation v2.7.8'
   print 'Crashman tested on Python versions- v2.7.6  v2.5.2  v2.6.2  v2.7.2  v3.0.1  v2.7.6'
   print 'If installed already, please verify if respective path added to PATH environment variable!!!\n\n'
   sys.exit(0)

tool_version = ''.join(['perl -v'])
proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
(out, err) = proc.communicate()
if out:
    try:
        if " (v" in out:
            perl_version =  out.split('(')[1].split(')')[0]
        elif "This is perl," in out:    
            perl_version =  out.split('This is perl,')[1].split('built')[0]
        else:
            perl_version = "Untested Perl Version"
        print "Perl Version Installed    :", perl_version
    except:
        print "Perl Version Installed    :"
else:
   print '\n\nERROR: Perl not installed!!!'
   print 'Recommended perl Installation v5.6.1'
   print 'Crashman tested on Perl versions- v5.10.1 , v5.6.1 , v5.8.7 , v5.12.4 , v5.14.2 '
   print 'If installed already, please verify if respective path added to PATH environment variable!!!\n\n'
   sys.exit(0)


parser = argparse.ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)  
parser.add_argument('-target','--target', help='TARGET: '+', '.join(VALID_TARGETS), dest='TARGET', action='store')
parser.add_argument('-dump','--dump', help="DUMPFILE: Please Provide Proper Dump File Location", dest='DUMPFILE', action='store')
parser.add_argument('-output','--output', help="OUTPUT: Please Provide Proper Output Path Location", dest='OUTPUT', action='store')
parser.add_argument('-build','--build', help="CRM: Please Provide Proper CRM Build Path Location", dest='BUILD', action='store')
parser.add_argument('-elf','--elf', default="",help="ELF: Please Provide Proper ELF Path Location", dest='ELF', action='store')
parser.add_argument('-lite','--lite',help="To Enbale Crashman Lite give -lite or --lite",action='store_true')
parser.add_argument('-asha','--asha',help="To Enbale Crashman for ASHA give -a or -asha",action='store_true')
parser.add_argument('-frameworktest','--frameworktest',help="for automated testing give -frameworktest or --frameworktest",action='store_true')
parser.add_argument('-gdb','--gdb',help="To Enbale GDB Simulator give -g or -gdb",action='store_true')
parser.add_argument('-slpi','--slpi',help="To Enable SLPI for 8996 give -s or -slpi",action='store_true')
parser.add_argument('-cdsp','--cdsp',help="To Enable CDSP Dumps give -c or -cdsp",action='store_true')
parser.add_argument('-sofile','--sofile', default="",help="ELF: Please Provide Dynamic so Path Location", dest='SOFILE', action='store')
parser.add_argument('-fullload','--fullload', default="",help="Full Load: please provide full dump path", dest='FL',action='store')
parser.add_argument('-smmu32','--smmu32', help="vmlinux 32 bit: Please Provide Proper 32 bit vmlinux Path Location", dest='SMMU32', action='store')
parser.add_argument('-smmu64','--smmu64', help="vmlinux 64 bit: Please Provide Proper 64 bit vmlinux Path Location", dest='SMMU64', action='store')
parser.add_argument('-use_t32','--use_t32', help="Please Provide T32 Path Location", dest='trace32', action='store')
parser.add_argument('-load_etm','--load_etm', help="To Enable ETM give -load_etm", action='store_true')
parser.add_argument('-vmlinux32','--vmlinux32', help="vmlinux 32 bit: Please Provide Proper 32 bit vmlinux Path Location", dest='VMLINUX32', action='store')
parser.add_argument('-vmlinux64','--vmlinux64', help="vmlinux 64 bit: Please Provide Proper 64 bit vmlinux Path Location", dest='VMLINUX64', action='store')
parser.add_argument('-frpcshell','--frpcshell', default="",help="Full Load: please provide fast rpc shell name along with path", dest='FRPC',action='store')
parser.add_argument('-linux_ramdump_parser','--linux_ramdump_parser', help="Linux Ram Dump Parser: Please Provide linux ram dump parser scripts path", dest='LINUX_RDP', action='store')
parser.add_argument('-smmuoutput','--smmuoutput', help="Linux Ram Dump Parser: Please Provide linux ram dump parser scripts path", dest='LINUX_SMMU', action='store')
parser.add_argument('-claraoutput','--claraoutput', help="Linux Ram Dump Parser: Please Provide clara output path", dest='CLARA_SMMU', action='store')
parser.add_argument('-start_address','--start_address', help='Provide the Start Address', dest='STARTADDR', action='store')
parser.add_argument('-kaslr-offset','--kaslr-offset', help='Provide the kaslr offset', dest='KASLR', action='store')
parser.add_argument('-kimage-voffset','--kimage-voffset', help='Provide the kimage voffset', dest='KIMAGE', action='store')
parser.add_argument('-phys-offset ','--phys-offset ', help='Provide the phys offset', dest='PHYS', action='store')
parser.add_argument('-wp_smmu','--wp_smmu', help="WP SMMU pagetable path: Please Provide Proper WP SMMU pagetable Path Location", dest='WP_SMMU', action='store')

opts = parser.parse_args()
argument_condition = 0
if opts.OUTPUT and opts.TARGET and opts.DUMPFILE and opts.ELF and opts.BUILD:
    argument_condition = 5
elif opts.OUTPUT and opts.TARGET and opts.DUMPFILE and opts.BUILD:
    argument_condition = 4
elif opts.TARGET and opts.DUMPFILE and opts.BUILD:
    argument_condition = 3
elif opts.DUMPFILE and opts.ELF:
    argument_condition = 2
elif opts.DUMPFILE:
    argument_condition = 1
# print argument_condition
if argument_condition == 1 or argument_condition == 2:
    opts.OUTPUT = "C:\\Temp"
    i = datetime.datetime.now()
    dateref = str("{0:0=2d}".format(i.month))+"_"+str("{0:0=2d}".format(i.day))+"_"+str(i.hour)+"h"+str(i.minute)+"m"+str(i.second)+"s"
    OUTDumpPath = opts.OUTPUT+"\\Crashman_"+dateref+"\\Logs"
    if not os.path.exists(OUTDumpPath):
        try:    
            os.makedirs(OUTDumpPath) 
            os.makedirs(OUTDumpPath+"\\temp") 
            os.makedirs(OUTDumpPath+"\\Qurt_logs") 
            os.makedirs(OUTDumpPath+"\\ulog") 
            # os.makedirs(OUTDumpPath+"\\dal") 
            os.makedirs(OUTDumpPath+"\\adsppm") 
        except:
            print "Output Folder is not accessible. Please Give Proper Output path."
            sys.exit(0)
    
    cmd = '''python %s\\strings.py %s %s\\temp\\buildid.txt'''%(CurrDirectory,opts.DUMPFILE,OUTDumpPath)
    # print cmd
    os.system(cmd)
    qcom = None
    qcom_line = None
    f_op = open(OUTDumpPath+"\\temp\\buildid.txt",'rb')
    for line in f_op: 
        if "QCOM time" in line:
            for list in VALID_TARGETS:
                if list in line:
                    qcom = list 
                    qcom_line = line
                    break
    f_op.close()
    if qcom != None:
        opts.TARGET = qcom
    # print qcom
    if qcom_line != None:
        build_found=None
        id = qcom_line.split('_')[-1].rstrip('\\n').split('_PD_SSC')[0].split('_PD_AUDIO')[0]
        # print id
        # print "//sun/CRMTools/Smart/nt/bin/Findbuild.exe","*"+id.rstrip('\r\n')+"*"
        output = subprocess.Popen(["//sun/CRMTools/Smart/nt/bin/Findbuild.exe","*"+id.rstrip('\r\n')+"*"],stdout = subprocess.PIPE).communicate()[0]
        builds = output.split("\r\n\r\n\r\n")
        # print builds
        for build in builds:
            if build and ("\nStatus:         Completed" in build or "\nStatus:         Approved" in build):
                build_found = build.split("Location:")[1].split("\r\n")[0].strip()
                break
        if build_found != None:
            opts.BUILD = build_found

t32_loc = opts.trace32
start_address = opts.STARTADDR
if not start_address: start_address = False
kaslr_offset = opts.KASLR
if not kaslr_offset: kaslr_offset = False
smmu_output = opts.LINUX_SMMU
if not smmu_output: smmu_output = False
kimage_voffset = opts.KIMAGE
if not kimage_voffset: kimage_voffset = False
if not t32_loc: t32_loc=False
if t32_loc != False:
    T32_path = t32_loc

wp_smmu_enable = opts.WP_SMMU
if not wp_smmu_enable: wp_smmu_enable = False
smmu_32bt = opts.SMMU32
if not smmu_32bt: smmu_32bt=False
smmu_64bt = opts.SMMU64
if not smmu_64bt: smmu_64bt=False
vmlinux_32bt = opts.VMLINUX32
if not vmlinux_32bt: vmlinux_32bt=False
vmlinux_64bt = opts.VMLINUX64
if not vmlinux_64bt: vmlinux_64bt=False
clara_output = opts.CLARA_SMMU
if not clara_output: clara_output = False
targetid = opts.TARGET
if targetid == "8917":
    targetid = "8937"
if targetid == "8940":
    targetid = "8937"
if targetid == "9635":
    targetid = "9x35"
if targetid == "sdm660" or targetid == "SDM660" or targetid == "660ADSP" or targetid == "660CDSP":
    targetid = "660"
if targetid == "sdm630" or targetid == "SDM630" or targetid == "630ADSP" or targetid == "630CDSP":
    targetid = "660"
if targetid == "sdm670" or targetid == "SDM670" or targetid == "670ADSP" or targetid == "670CDSP":
    targetid = "670"
if targetid == "sdm845" or targetid == "SDM845" or targetid == "845ADSP" or targetid == "845CDSP":
    targetid = "845"
INDumpPath = opts.DUMPFILE
CustomerPath = opts.ELF
if not CustomerPath: CustomerPath=False
SOFILE = opts.SOFILE
if not SOFILE: SOFILE=False
UniqueNumber="NotGiven"
crashman_lite = opts.lite
load_dump_full = opts.FL
if not load_dump_full: load_dump_full=False
frpcshell = opts.FRPC
if not frpcshell: 
    frpcshell_name = False
    frpcshell=False
if frpcshell: 
    frpcshell_name = frpcshell.split('\\')[-1].split('.')[0]
asha_flag = opts.asha
quit_flag = opts.frameworktest
gdb_flag  = opts.gdb
slpi_flag = opts.slpi
etm_flag = opts.load_etm
smmu_enable=False

        
if targetid == "8937" or targetid == "8953":
    if customerflag==False:
        if hydserver==True:
            ramparse_path = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\8937\\linux-ramdump-parser-v2"
        else:
            ramparse_path = "\\\\hulk\\vocoder_appdsp5\\users\\Crashman\\8937\\linux-ramdump-parser-v2"
            if not os.path.exists("\\\\hulk\\vocoder_appdsp5\\users\\Crashman\\8937\\linux-ramdump-parser-v2"):
                ramparse_path = "\\\\rover\\hyd_dspfw\\ADSP_Tools\\TriageTeam\\8937\\linux-ramdump-parser-v2"

if targetid == "8998":
    ramparse_path = ramparse_path.replace('linux-ramdump-parser-v2','linux-ramdump-parser-v2_8998')
if targetid == "660":
    ramparse_path = ramparse_path.replace('linux-ramdump-parser-v2','linux-ramdump-parser-v2_660')
if targetid == "845" or targetid == "670":
    ramparse_path = ramparse_path.replace('linux-ramdump-parser-v2','linux-ramdump-parser-v2_845')
    
if opts.LINUX_RDP:
    ramparse_path = opts.LINUX_RDP

if load_dump_full!=False:
    if not os.path.isfile(os.path.dirname(INDumpPath)+"\\load.cmm"):
        print "**************************************************************"
        print "If you are using -smmu32/smmu64 crahman option , the dump argument given in crashman must point to complete ramdump.\n"
        print "Mandatory contents to be present in dump path - load.cmm, dumps etc"
        print "**************************************************************"
        sys.exit(0)

if crashman_lite==True and gdb_flag==True:
    print "Crashman Lite and GDB are not concurrently supported. Please select any one option"
    sys.exit(0)
    
if not os.path.isfile(INDumpPath):
    print "Dump File is not accessible/available. Please Check the File Path."
    sys.exit(0)
    
if os.access(INDumpPath,os.R_OK) == False:
    print "Dump File is not accessible/available. Please Check the File Path."
    sys.exit(0)



if argument_condition != 1 and argument_condition != 2:
    i = datetime.datetime.now()
    dateref = str("{0:0=2d}".format(i.month))+"_"+str("{0:0=2d}".format(i.day))+"_"+str(i.hour)+"h"+str(i.minute)+"m"+str(i.second)+"s"
    if not opts.OUTPUT:
        opts.OUTPUT = "C:\\Temp"
    if len(opts.OUTPUT) > 34:
        print "Output folder path length is too long. Please reduce the output path length"
    OUTDumpPath = opts.OUTPUT+"\\Crashman_"+dateref+"\\Logs"
    if not os.path.exists(OUTDumpPath):
        try:    
            os.makedirs(OUTDumpPath) 
            os.makedirs(OUTDumpPath+"\\temp") 
            if gdb_flag != True:
                os.makedirs(OUTDumpPath+"\\Qurt_logs") 
                os.makedirs(OUTDumpPath+"\\ulog") 
                # os.makedirs(OUTDumpPath+"\\dal") 
                os.makedirs(OUTDumpPath+"\\adsppm") 
        except:
            print "Output Folder is not accessible. Please Give Proper Output path."
            sys.exit(0)
        
print "Crashman Output Folder    : ",OUTDumpPath
print "Processing Inputs Please Wait..."

CRM_buildpath = opts.BUILD
if not CRM_buildpath:
    print "ADSP Build Path is not accessible/available. Unable to Find Build ID from Dumps. Please Check and Provide the Proper Build Path."
    sys.exit(0)

if CRM_buildpath[-1]== "\\":
    CRM_buildpath=CRM_buildpath[:len(CRM_buildpath)-1]

if os.path.exists(CRM_buildpath+"\\adsp_proc"):
    CRM_buildpath = CRM_buildpath+"\\adsp_proc"
if os.path.exists(CRM_buildpath+"\\slpi_proc"):
    slpi_flag=True
    CRM_buildpath = CRM_buildpath+"\\slpi_proc"

if not os.path.exists(CRM_buildpath):
    print "ADSP Build Path is not accessible/available. Please Check the Build Path."
    sys.exit(0)
    
if "slpi_proc" in CRM_buildpath:
    slpi_flag=True

cdsp_flag = opts.cdsp
if os.path.exists(CRM_buildpath+"\\cdsp_proc"):
    CRM_buildpath = CRM_buildpath+"\\cdsp_proc"

if "cdsp_proc" in CRM_buildpath:
    cdsp_flag = True

if cdsp_flag == True and not "cdsp_proc" in CRM_buildpath:
    print "CDSP Build Path is not accessible/available. Please Provide the proper CDSP Build Path."
    sys.exit(0)
    
if not os.path.exists(CRM_buildpath+"\\core"):
    print "Core folder is not present in ADSP Build. Please provide proper CRM Build Path."
    sys.exit(0)

if os.access(CRM_buildpath,os.R_OK) == False:
    print "ADSP Build Path is not accessible/available. Please Check the Build Path."
    sys.exit(0)

Temp_DIR = "C:\Temp"
if not os.path.exists(Temp_DIR):
    os.makedirs(Temp_DIR) 

   
dump_path=None
if os.path.isfile(INDumpPath):
    dump_path = os.path.dirname(INDumpPath)

if os.path.isfile(dump_path+"\\dump_info.txt"):
    f_info = open(dump_path+"\\dump_info.txt",'r')
    f_store = open(OUTDumpPath+"\\temp\\ExtractAddress.txt",'w')
    for line in f_info:
        if ".BIN" in line:
            val1 = line.split()[1]
            val2 = line.split()[2]
            val_add = int(val2,10)+int(val1,16)
            f_store.write(val1+" "+hex(val_add).replace('L','')+" "+val2+" "+hex(int(val2,10)).replace('L','')+" "+line.split()[-1]+"\n")
    f_info.close()
    f_store.close()
elif os.path.isfile(dump_path+"\\load.cmm"):
    f_info = open(dump_path+"\\load.cmm",'r')
    f_store = open(OUTDumpPath+"\\temp\\ExtractAddress.txt",'w')
    for line in f_info:
        if "d.load.binary" in line:
            input_file = dump_path + "\\" + line.split()[1]
            if os.path.isfile(input_file):
                input_file_size = str(os.path.getsize(input_file))
                val1 = line.split()[2]
                val2 = input_file_size
                val_add = int(val2,10)+int(val1,16)
                f_store.write(line.split()[2]+" "+hex(val_add).replace('L','')+" "+str(input_file_size)+" "+hex(int(input_file_size,10)).replace('L','')+" "+line.split()[1]+"\n")
    f_info.close()
    f_store.close()
if smmu_32bt!=False or smmu_64bt!=False:
    ddrcs0_offset = None
    ddrcs1_offset = None
    ocimem_offset = None
    ddrcs0_filename = "DDRCS0.BIN"
    ddrcs1_filename = "DDRCS1.BIN"
    
    dram_filename = None
    dram_offset1 = None
    dram_offset2 = None
    
    dram1_filename = None
    dram1_offset1 = None
    dram1_offset2 = None
    
    ocimem_filename = None
    ocimem_offset1 = None
    ocimem_offset2 = None
    kaslr_address = None
    
    try:
        if os.path.isfile(dump_path+"\\dump_info.txt"):
            f_dump = open(dump_path+"\\dump_info.txt",'r')
            for line in f_dump:
                if "DDRCS0.BIN" in line:
                    offset = line.split()
                    ddrcs0_offset = offset[1]
                    ddrcs0_size = offset[2]
                    ddrcs0_filename = "DDRCS0.BIN"
                if "DDRCS0_0.BIN" in line:
                    offset = line.split()
                    ddrcs0_offset = offset[1]
                    ddrcs0_size = offset[2]
                    ddrcs0_filename = "DDRCS0_0.BIN"
                if "DDRCS1.BIN" in line :
                    offset = line.split()
                    ddrcs1_offset = offset[1]
                    ddrcs1_size = offset[2]
                    ddrcs1_filename = "DDRCS1.BIN"
                if "DDRCS1_0.BIN" in line:
                    offset = line.split()
                    ddrcs1_offset = offset[1]
                    ddrcs1_size = offset[2]
                    ddrcs1_filename = "DDRCS1_0.BIN"
                if "OCIMEM.BIN" in line:
                    offset = line.split()
                    ocimem_offset = offset[1]
                    ocimem_size = offset[2]
            f_dump.close()
        else:
            for r,d,f in os.walk(dump_path):
                for file in f:
                    win_dump  = os.path.join(r,file)
                    # print win_dump
                    if "dram_" in win_dump:
                        dram_filename = win_dump.split('\\')[-1]
                        dram_offset1 = win_dump.split('dram_')[1].split('--')[0]
                        dram_offset2 = win_dump.split('dram_')[1].split('--')[1].split('.')[0]
                        load = OUTDumpPath+"\\temp\\load.cmm"
                        f_load = open(load,'a')
                        f_load.write('d.load.binary '+win_dump+' a:'+dram_offset1+' /noclear\n')
                        f_load.close()
                    if "dram1_" in win_dump:
                        dram1_filename = win_dump.split('\\')[-1]
                        dram1_offset1 = win_dump.split('dram1_')[1].split('--')[0]
                        dram1_offset2 = win_dump.split('dram1_')[1].split('--')[1].split('.')[0]
                        load = OUTDumpPath+"\\temp\\load.cmm"
                        f_load = open(load,'a')
                        f_load.write('d.load.binary '+win_dump+' a:'+dram1_offset1+' /noclear\n')
                        f_load.close()
                    if "ocimem_" in win_dump:
                        ocimem_filename = win_dump.split('\\')[-1]
                        ocimem_offset1 = win_dump.split('ocimem_')[1].split('--')[0]
                        ocimem_offset2 = win_dump.split('ocimem_')[1].split('--')[1].split('.')[0]
                    if "kaslr_" in win_dump:
                        if os.path.isfile(win_dump) and kaslr_offset == False:
                            file_kaslr = open(win_dump,"rb")
                            data = file_kaslr.read(4)
                            s1=binascii.hexlify(file_kaslr.read(1))
                            s2=binascii.hexlify(file_kaslr.read(1))
                            s3=binascii.hexlify(file_kaslr.read(1))
                            s4=binascii.hexlify(file_kaslr.read(1))
                            kaslr_address = "0x"+s4+s3+s2+s1
                            file_kaslr.close()
            
    except:
        print "dump_info.txt not exists!!"
        
    offset = " "
    if dram_filename!=None and dram1_filename!=None and ocimem_filename!=None:
        phy_offset  = dram_offset1
        if int(dram1_offset1,16) < int(dram_offset1,16):
            phy_offset = dram1_offset1
        if opts.PHYS:
            phy_offset = opts.PHYS
        if kaslr_offset != False:
            if kimage_voffset != False:
                offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --phys-offset %s --kaslr-offset %s --kimage-voffset %s'''%(dump_path,dram_filename,dram_offset1,dram_offset2,dump_path,dram1_filename,dram1_offset1,dram1_offset2,dump_path,ocimem_filename,ocimem_offset1,ocimem_offset2,phy_offset,kaslr_offset,kimage_voffset)
            else:
                offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --phys-offset %s --kaslr-offset %s'''%(dump_path,dram_filename,dram_offset1,dram_offset2,dump_path,dram1_filename,dram1_offset1,dram1_offset2,dump_path,ocimem_filename,ocimem_offset1,ocimem_offset2,phy_offset,kaslr_offset)
        else:
            if kaslr_address != None:
                offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --phys-offset %s --kaslr-offset %s'''%(dump_path,dram_filename,dram_offset1,dram_offset2,dump_path,dram1_filename,dram1_offset1,dram1_offset2,dump_path,ocimem_filename,ocimem_offset1,ocimem_offset2,phy_offset,kaslr_address)
            else:
                offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --phys-offset %s '''%(dump_path,dram_filename,dram_offset1,dram_offset2,dump_path,dram1_filename,dram1_offset1,dram1_offset2,dump_path,ocimem_filename,ocimem_offset1,ocimem_offset2,phy_offset)
        # print offset
    # elif ddrcs0_offset!=None and ddrcs1_offset!=None:
        # ddrcs0_offset = hex(int(ddrcs0_offset,16)).replace('L','')
        # ddrcs1_offset = hex(int(ddrcs1_offset,16)).replace('L','')
        # ocimem_offset = hex(int(ocimem_offset,16)).replace('L','')
        
        # ddrcs0_size = hex(int(ddrcs0_offset,16) + int(hex(int(ddrcs0_size)).replace('L',''),16) - 1).replace('L','')
        # ddrcs1_size = hex(int(ddrcs1_offset,16) + int(hex(int(ddrcs1_size)).replace('L',''),16) - 1).replace('L','')
        # ocimem_size = hex(int(ocimem_offset,16) + int(hex(int(ocimem_size)).replace('L',''),16) - 1).replace('L','')

        # phy_offset  = ddrcs0_offset
        # if int(ddrcs1_offset,16) < int(ddrcs0_offset,16):
            # phy_offset = ddrcs1_offset
        # if opts.PHYS:
            # phy_offset = opts.PHYS
        # if kaslr_offset != False:
            # if kimage_voffset != False:
                # offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\OCIMEM.BIN %s %s --phys-offset %s --kaslr-offset %s --kimage-voffset %s'''%(dump_path,ddrcs0_filename,ddrcs0_offset,ddrcs0_size,dump_path,ddrcs1_filename,ddrcs1_offset,ddrcs1_size,dump_path,ocimem_offset,ocimem_size,phy_offset,kaslr_offset,kimage_voffset)
            # else:
                # offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\OCIMEM.BIN %s %s --phys-offset %s --kaslr-offset %s'''%(dump_path,ddrcs0_filename,ddrcs0_offset,ddrcs0_size,dump_path,ddrcs1_filename,ddrcs1_offset,ddrcs1_size,dump_path,ocimem_offset,ocimem_size,phy_offset,kaslr_offset)
        # else:
            # offset = '''--ram-file %s\\%s %s %s --ram-file %s\\%s %s %s --ram-file %s\\OCIMEM.BIN %s %s --phys-offset %s'''%(dump_path,ddrcs0_filename,ddrcs0_offset,ddrcs0_size,dump_path,ddrcs1_filename,ddrcs1_offset,ddrcs1_size,dump_path,ocimem_offset,ocimem_size,phy_offset)
        # print offset
        
if vmlinux_32bt!=False or vmlinux_64bt!=False:
    if vmlinux_64bt!=False:
        if not os.path.exists(ramparse_path):
            print "**************************************************************"
            print "Linux ram dump parser path is not accessible/available. Please Check the Path"
            print "Expecting aarch64-linux-gnu-gdb.exe/aarch64-linux-gnu-nm.exe in linux ram dump parser path location!!!"
            print "**************************************************************"
            sys.exit(0)
        if os.path.isfile(vmlinux_64bt):
            command = '''python %s\\ramparse.py --64-bit -a %s  -o %s   -v %s  --parse-debug-image --nm-path %s\\aarch64-linux-gnu-nm.exe --gdb-path %s\\aarch64-linux-gnu-gdb.exe'''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",vmlinux_64bt,ramparse_path,ramparse_path)
        else:
            print "**************************************************************"
            print "vmlinux path is not accessible/available. Please Check the Path "
            print "**************************************************************"
            sys.exit(0)
    if vmlinux_32bt!=False:
        if not os.path.exists(ramparse_path):
            print "**************************************************************"
            print "Linux ram dump parser path is not accessible/available. Please Check the Path "
            print "Expecting aarch64-linux-gnu-gdb.exe/aarch64-linux-gnu-nm.exe in linux ram dump parser path location!!!"
            print "**************************************************************"
            sys.exit(0)
        if os.path.isfile(vmlinux_32bt):
            command = '''python %s\\ramparse.py --32-bit -a %s  -o %s   -v %s  --parse-debug-image --nm-path %s\\aarch64-linux-gnu-nm.exe --gdb-path %s\\aarch64-linux-gnu-gdb.exe '''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",vmlinux_32bt,ramparse_path,ramparse_path)
        else:
            print "**************************************************************"
            print "vmlinux path is not accessible/available. Please Check the Path "
            print "**************************************************************"
            sys.exit(0)
    
    print "Processing ETM Dump... it might take 4-5 minutes. Please wait..."
    os.system(command)

if etm_flag == True and (smmu_32bt!=False or smmu_64bt!=False):
    etm_flag = True
elif etm_flag == True:
    dumpfile = None
    if "tmc-etr.bin" in INDumpPath:
        dumpfile = INDumpPath
    elif "etr.bin" in INDumpPath:
        dumpfile = INDumpPath
    elif os.path.isfile(os.path.dirname(INDumpPath)+"\\tmc-etr.bin"):
        dumpfile = os.path.dirname(INDumpPath)+"\\tmc-etr.bin"
    elif os.path.isfile(os.path.dirname(INDumpPath)+"\\etr.bin"):
        dumpfile = os.path.dirname(INDumpPath)+"\\etr.bin"
    elif os.path.isfile(OUTDumpPath+"\\temp\\tmc-etr.bin"):
        dumpfile = OUTDumpPath+"\\temp\\tmc-etr.bin"
    elif os.path.isfile(OUTDumpPath+"\\temp\\etr.bin"):
        dumpfile = OUTDumpPath+"\\temp\\etr.bin"
        
    if dumpfile == None:
        print "ETM Dump File is not accessible/available. Please Check the File Path."
        sys.exit(0)

    if CustomerPath==False:
        CustomerPath = CRM_buildpath+"\\build\\ms"
        
    elfname = CustomerPath+"\\M"+targetid+"AAAAAAAAQ1234.elf"
    if targetid == "660":
        elfname = CustomerPath+"\\ROOT_660.adsp.prodQ.elf"
        if not os.path.isfile(elfname):
            elfname = CustomerPath+"\\ROOT_660.cdsp.prodQ.elf"
    if targetid == "845":
        elfname = CustomerPath+"\\ROOT_845.adsp.prodQ.elf"
        if not os.path.isfile(elfname):
            elfname = CustomerPath+"\\ROOT_845.cdsp.prodQ.elf"
    if targetid == "670":
        elfname = CustomerPath+"\\ROOT_670.adsp.prodQ.elf"
        if not os.path.isfile(elfname):
            elfname = CustomerPath+"\\ROOT_670.cdsp.prodQ.elf"

    if not os.path.isfile(elfname):
        print "ELF file is not accessible/available @ "+ elfname
        sys.exit(0)

    
    print "\nStarting T32 for ETM Dump."
    print "************************************"
    t32_file = OUTDumpPath+"\\temp\\config_sim_usb.t32"
    f_t32 = open(t32_file,'w+')
    f_t32.write("OS=\n")
    f_t32.write("ID=SIM_${4}            ; unique ID needed for temporary file name uniqueness\n")
    f_t32.write("TMP="+OUTDumpPath+"\\temp"+"           ; T32 temp file names are prepended with the unique ID above\n")
    if quit_flag == True:
        f_t32.write("SYS="+T32_path+"                ; make system directory the same as the executable directory\n\n")
    else:
        if t32_loc != False:
            f_t32.write("SYS="+T32_path+"                ; make system directory the same as the executable directory\n\n")
        else:
            f_t32.write("SYS=C:\T32                ; make system directory the same as the executable directory\n\n")
    f_t32.write("SCREEN=\n")
    f_t32.write("HEADER=${4}\n\n")
    f_t32.write("; Ethernet on Host information \n")
    f_t32.write("PBI=SIM\n")
    f_t32.write("\n")
    f_t32.write("; Printer settings\n")
    f_t32.write("PRINTER=WINDOWS\n\n")
    f_t32.write("; Screen fonts\n")
    f_t32.write("SCREEN=\n")
    f_t32.write("FONT=SMALL\n\n")
    f_t32.close()

    if t32_loc != False:
        Path = T32_path+"\\bin\\windows64"
    else:
        T32_path = "C:\\T32"
        Path = "C:\\T32\\bin\\windows64"
    
    if os.path.exists(Path):
        command = '''start %s\\bin\\windows64\\t32mqdsp6.exe -c %s, %s %s %s %s %s'''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb.t32",CurrDirectory+"\\load_adsp_etm_dump.cmm",elfname,dumpfile,targetid,cdsp_flag)
        os.system(command)
    else:    
        command = '''start %s\\t32mqdsp6.exe -c %s, %s %s %s %s %s'''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb.t32",CurrDirectory+"\\load_adsp_etm_dump.cmm",elfname,dumpfile,targetid,cdsp_flag)
        os.system(command)
    sys.exit(0)
        
        
flag = 0
if smmu_32bt!=False and smmu_output== False and clara_output==False:
    if not os.path.exists(ramparse_path):
        print "**************************************************************"
        print "Linux ram dump parser path is not accessible/available. Please Check the Path "
        print "Expecting aarch64-linux-gnu-gdb.exe/aarch64-linux-gnu-nm.exe in linux ram dump parser path location!!!"
        print "**************************************************************"
        sys.exit(0)
    if os.path.isfile(smmu_32bt):
        if targetid == "8953":
            command = '''python %s\\ramparse.py  --32-bit -a %s  -o %s   -v %s  --print-iommu-pg-tables --force-hardware %s %s --nm-path %s\\arm-none-eabi-nm.exe --gdb-path %s\\arm-none-eabi-gdb.exe'''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",smmu_32bt,"8937",offset,ramparse_path,ramparse_path)
        elif targetid == "845":
            command = '''python %s\\ramparse.py  --32-bit -a %s  -o %s   -v %s  --print-iommu-pg-tables --force-hardware %s %s --nm-path %s\\arm-none-eabi-nm.exe --gdb-path %s\\arm-none-eabi-gdb.exe'''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",smmu_32bt,"sdm845",offset,ramparse_path,ramparse_path)
        else:
            command = '''python %s\\ramparse.py  --32-bit  -a %s  -o %s   -v %s  --print-iommu-pg-tables --force-hardware %s %s --nm-path %s\\arm-none-eabi-nm.exe --gdb-path %s\\arm-none-eabi-gdb.exe'''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",smmu_32bt,targetid,offset,ramparse_path,ramparse_path)
        print "Processing SMMU pagetable parsing... it might take 4-5 minutes. Please wait..."
        # print command
        bin = OUTDumpPath+"\\temp\\ProcessingCommands.txt"
        f_bin = open(bin,'a')
        f_bin.write(str(command)+'\n')
        f_bin.close()
        os.system(command)
        smmu_enable=True
    else:
        print "**************************************************************"
        print "vmlinux path is not accessible/available. Please Check the Path "
        print "**************************************************************"
    load_dump_full=dump_path
    flag = 1
    
if smmu_64bt!=False and smmu_output== False and clara_output==False:
    if not os.path.exists(ramparse_path):
        print "**************************************************************"
        print "Linux ram dump parser path is not accessible/available. Please Check the Path "
        print "Expecting aarch64-linux-gnu-gdb.exe/aarch64-linux-gnu-nm.exe in linux ram dump parser path location!!!"
        print "**************************************************************"
        sys.exit(0)
    if os.path.isfile(smmu_64bt):
        if targetid == "8953":
            command = '''python %s\\ramparse.py --64-bit -a %s  -o %s   -v %s  --print-iommu-pg-tables --force-hardware %s %s --nm-path %s\\aarch64-linux-gnu-nm.exe --gdb-path %s\\aarch64-linux-gnu-gdb.exe '''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",smmu_64bt,"8937",offset,ramparse_path,ramparse_path)
        elif targetid == "845":
            command = '''python %s\\ramparse.py --64-bit -a %s  -o %s   -v %s  --print-iommu-pg-tables --force-hardware %s %s --nm-path %s\\aarch64-linux-gnu-nm.exe --gdb-path %s\\aarch64-linux-gnu-gdb.exe '''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",smmu_64bt,"sdm845",offset,ramparse_path,ramparse_path)
        else:
            command = '''python %s\\ramparse.py --64-bit -a %s  -o %s   -v %s  --print-iommu-pg-tables --force-hardware %s %s --nm-path %s\\aarch64-linux-gnu-nm.exe --gdb-path %s\\aarch64-linux-gnu-gdb.exe '''%(ramparse_path,dump_path,OUTDumpPath+"\\temp",smmu_64bt,targetid,offset,ramparse_path,ramparse_path)
        print "Processing SMMU pagetable parsing... it might take 4-5 minutes. Please wait..."
        # print command
        bin = OUTDumpPath+"\\temp\\ProcessingCommands.txt"
        f_bin = open(bin,'a')
        f_bin.write(str(command)+'\n')
        f_bin.close()
        os.system(command)
    else:
        print "**************************************************************"
        print "vmlinux path is not accessible/available. Please Check the Path "
        print "**************************************************************"
    smmu_enable=True
    load_dump_full=dump_path
    flag = 1
    
if smmu_64bt!=False or smmu_32bt!=False or smmu_output!=False:
    if opts.LINUX_SMMU:
        s_output = opts.LINUX_SMMU
        load_dump_full=dump_path
        smmu_enable=True
        # shutil.copytree(s_output, OUTDumpPath+"\\temp")
        for r,d,f in os.walk(s_output):
            for file in f:
                win_dump  = os.path.join(r,file)
                if "arm_" in win_dump or "ams_" in win_dump:
                    shutil.copy2(win_dump, OUTDumpPath+"\\temp\\")

if smmu_64bt!=False or smmu_32bt!=False or clara_output!=False:
    if opts.CLARA_SMMU:
        s_output = opts.CLARA_SMMU
        load_dump_full=dump_path
        smmu_enable=True
        for r,d,f in os.walk(s_output):
            for file in f:
                win_dump  = os.path.join(r,file)
                if "arm_" in win_dump or "ams_" in win_dump or "_msm" in win_dump:
                    print "Please wait... claraoutput copying in progress...."
                    shutil.copy2(win_dump, OUTDumpPath+"\\temp\\")

if smmu_enable==True and clara_output==False:
    if not os.path.isfile(OUTDumpPath+"\\temp\\msm_iommu_domain_02.txt") and not os.path.isfile(OUTDumpPath+"\\temp\\arm_iommu_domain_02.txt"):
        print "**************************************************************"
        print "Not able to generate SMMU pagetable mappings with provided dumps and vmlinux."
        print "Please provide matching vmlinux and dumps"
        print "**************************************************************"
        if os.path.isfile(OUTDumpPath+"\\temp\\dmesg_TZ.txt"):
            print "\n\n#####################################################################"
            print "Error Log from dmesg_TZ.txt file"
            print "#####################################################################"
            f_dump = open(OUTDumpPath+"\\temp\\dmesg_TZ.txt",'r')
            for line in f_dump:
                print line
            f_dump.close()
            print "#####################################################################"
        sys.exit(0)
    
input = OUTDumpPath+"\\temp\\version_check.txt"
f_out = open(input,"w")
Pythonversion = sys.version
Pythonversion1 = Pythonversion.split() 
f_out.write('Python Version Installed : v' + Pythonversion1[0] + '\n' )
f_out.write('Perl   Version Installed : ' + perl_version + '\n' )
f_out.write('Crashman Version         : '+ Crashman_Version + '\n')
f_out.write('VersionFileEnd\n')
f_out.close()
win = 0
if ".dmp" == INDumpPath[-4:]:
    win = 1
elif ".tmp" == INDumpPath[-4:]:
    win = 1

if win == 1:
    print "\n\n************************************"
    print "Windows Dumps Extraction Started...\n"
    os.makedirs(OUTDumpPath+"\\Dexter") 
    command = '''%s\dexter.exe /v /i %s /o %s '''%(CurrDirectory,INDumpPath,OUTDumpPath+"\\Dexter")
    print command
    os.system(command)
    
    INDumpPath=OUTDumpPath+"\\Dexter\\ADSP0.bin"
    if slpi_flag == True:
        INDumpPath=OUTDumpPath+"\\Dexter\\SCSS0.bin"
    
    for r,d,f in os.walk(OUTDumpPath+"\\Dexter\\"):
        cond= False
        for file in f:
            win_dump  = os.path.join(r,file)
            # print win_dump
            if "ADSP0.bin" in win_dump or "ADSP0_" in win_dump or ("ADSP__2" in win_dump and ".bin" in win_dump):
                INDumpPath = win_dump
                cond= True
                break
            if slpi_flag == True:
                if "SCSS0.bin" in win_dump or "SCSS0_" in win_dump:
                    INDumpPath = win_dump
                    cond = True
                    break
        if cond == True:
            break
    print "\nWindows Dumps Extraction Done\n"
    print "************************************\n\n\n"
        
bin = OUTDumpPath+"\\temp\\binarylength.txt"
f_bin = open(bin,'w')
size = os.path.getsize(INDumpPath)
f_bin.write(str(size)+'\n')
f_bin.close()

if start_address == False:
    # print datetime.datetime.now()
    command = '''python %s\Start_Address.py  %s %s %s %s %s'''%(CurrDirectory,INDumpPath,OUTDumpPath,targetid,slpi_flag,cdsp_flag)
    # print command
    bin = OUTDumpPath+"\\temp\\ProcessingCommands.txt"
    f_bin = open(bin,'a')
    f_bin.write(str(command)+'\n')
    f_bin.close()
    process = subprocess.Popen(command, stdout=subprocess.PIPE)
    process.wait()
    # print datetime.datetime.now()
else:
     if not os.path.isfile(OUTDumpPath+"\\temp\\adspstartaddr.txt"):
        extension = os.path.splitext(str(INDumpPath))[1]
        if extension == "":
            if binascii.hexlify(data) == "7f454c46":
                extension = ".ELF"
        file_dump = open(OUTDumpPath+"\\temp\\dumpformant.txt",'w')
        if (extension == ".elf" or extension == ".ELF"):
            file_dump.write('2')
        else:
            file_dump.write('1')
        file_dump.close()
        file = open(OUTDumpPath+"\\temp\\adspstartaddr.txt",'w')
        file.write(start_address)
        file.close()

if not os.path.isfile(OUTDumpPath+"\\temp\\adspstartaddr.txt"):
    for file in possible_dump_name:
        if not file in INDumpPath:
            dump_file = os.path.dirname(INDumpPath)+"\\"+file
            if os.path.isfile(dump_file):
                if start_address == False:
                    command = '''python %s\Start_Address.py  %s %s %s %s %s'''%(CurrDirectory,dump_file,OUTDumpPath,targetid,slpi_flag,cdsp_flag)
                    process = subprocess.Popen(command, stdout=subprocess.PIPE)
                    process.wait() 
                    if os.path.isfile(OUTDumpPath+"\\temp\\adspstartaddr.txt"):
                        INDumpPath = dump_file
                        break

DumpSize = 0
for file in possible_dump_name:
    dump_file_name = os.path.dirname(INDumpPath)+"\\"+file
    if os.path.isfile(dump_file_name):
        DumpSize = DumpSize + os.path.getsize(dump_file_name)
        
Enable_6GB_dumps = False
if DumpSize > 4294967296 and smmu_enable == True:
    Enable_6GB_dumps = True 
    
if gdb_flag == True:
    bin = OUTDumpPath+"\\temp\\binarylength.txt"
    line1 = 0
    f_bin = open(bin,'r+')
    for line in f_bin:
        line1 = line
        break
    f_bin.close()
    bin = OUTDumpPath+"\\temp\\adspstartaddr.txt"
    start_address = 0
    f_bin = open(bin,'r+')
    for line in f_bin:
        start_address = line
        # print start_address
        break
    f_bin.close()
    
    if targetid == "8996" or targetid == "8998":
        print "8996/8998 GDB simulator is not supported\n"
        sys.exit(0)
    DUMP_SIZE_CHECK = 36700160 #Dump file check as 35MB
    if (((".bin" in INDumpPath) or (".BIN" in INDumpPath)) and (int(line1) > DUMP_SIZE_CHECK)):
        if CustomerPath==False:
            CustomerPath = CRM_buildpath+"\\build\\ms"
            
        command = '''python %s\gdb\gdbADSPcrashman.py %s %s %s %s %s %s %s %s'''%(CurrDirectory,targetid,INDumpPath,OUTDumpPath,CRM_buildpath,CustomerPath,SOFILE,start_address.rstrip('\n'),smmu_enable)
        # print command
        os.system(command)
    else:
        print "Please provide proper dumps for GDB simulator. Its supports only .BIN files"
        sys.exit(0)
        
    if targetid == "8952"  or targetid == "8953"  or targetid == "8937"  or targetid == "8976" :
        gdb_ui = '''
********************************************************************
Disclaimer for GDB UI Simulator 
********************************************************************
    
Prerequisites:

    Hexagon Tools Version 5.0.14 or Later
    Hexagon SDK Version 2.0 or Later
    Hexagon Plug-in for Crash Simulator

Update Environment:
    Update the {Hexagon_Tools_Install_Location} to ENV PATH
    Update the {Hexagon_SDK_Install_Location}/tools/eclipse to ENV PATH

Install Crash Sim Plug-in:
    To use crash simulator you need to have eclipse and IDE.zip plug-ins.
    installing the Hexagon plug ins can be done in two ways:
        1. Install using Help -> Install new software in eclipse
        2. Install using manage_plugins.py 
           [ python manage_plugins.py -E <eclipse_location> -Z <full_path_of_IDE.zip>
             manage_plugins.py is in Hexagon SDK at {Hexagon_SDK_Install_Location}/tools/scripts
             ex: python manage_plugins.py -E C:\Qualcomm\Hexagon_SDK\\2.0\\tools\eclipse -Z C:\Dropbox\IDE.zip ]

*************************************************************************************************
    '''
        print gdb_ui
        BUILD_ASIC=targetid
        BUILD_ID="AAAAAAAAQ"
        BUILD_VER="1234"
        F010 = ['8994','8996','8998','8992','8952','8953','8937']        
        if targetid in F010: va_start="0xf0100000"
        else: va_start="0xf0000000"
        print "Do you want to open UI Simulator(yes/no):"
        a = raw_input()
        if a == "yes" or a == "y":
            command = '''eclipse.exe -G 9010 -dump %s 0x80000000 0x1FFFFFFF -dpage %s %s 0x1400000 -asid 0x0 -elf %s --launch_crash_sim --tools_path C:\\Qualcomm\\HEXAGON_Tools\\5.0.14\\ --crash_sim %s\\gdb\crash_sim.exe  -data %s'''%(INDumpPath,va_start,start_address.rstrip('\n'),CustomerPath+"\\M"+BUILD_ASIC+BUILD_ID+BUILD_VER+".elf",CurrDirectory,OUTDumpPath+"\\temp\\eclipse")
            os.system(command)
    sys.exit(0)

print "\nStarting T32 for further processing."
print "************************************"
t32_file = OUTDumpPath+"\\temp\\config_sim_usb.t32"
f_t32 = open(t32_file,'w+')
f_t32.write("OS=\n")
f_t32.write("ID=SIM_${2}            ; unique ID needed for temporary file name uniqueness\n")
f_t32.write("TMP="+OUTDumpPath+"\\temp"+"           ; T32 temp file names are prepended with the unique ID above\n")
if quit_flag == True:
    f_t32.write("SYS="+T32_path+"                ; make system directory the same as the executable directory\n\n")
else:
    if t32_loc != False:
        f_t32.write("SYS="+T32_path+"                ; make system directory the same as the executable directory\n\n")
    else:
        f_t32.write("SYS=C:\T32                ; make system directory the same as the executable directory\n\n")
f_t32.write("SCREEN=\n")
f_t32.write("HEADER=${2}\n\n")
f_t32.write("; Ethernet on Host information \n")
f_t32.write("PBI=SIM\n")
f_t32.write("\n")
f_t32.write("; Printer settings\n")
f_t32.write("PRINTER=WINDOWS\n\n")
f_t32.write("; Screen fonts\n")
f_t32.write("SCREEN=\n")
f_t32.write("FONT=SMALL\n\n")
f_t32.close()

if SOFILE:
    if SOFILE != "8952" and SOFILE != "8953" and SOFILE != "8937" and SOFILE != "8910"  and SOFILE != "8996" and SOFILE != "8998" and SOFILE != "8976":
        print "Crashman Presently Support dynamic loading only for 8952/8953/8937/8976/8996/8998/8910."

        
if CustomerPath!=False and os.path.isfile(CustomerPath+"\\ROOT_660.adsp.prodQ.elf"):
    print "ELF is present"
else:
    if CustomerPath!=False and os.path.isfile(CustomerPath+"\\M660660.adsp.testQ1234.elf"):
        shutil.copy2(CustomerPath+"\\M660660.adsp.testQ1234.elf", OUTDumpPath+"\\temp\\ROOT_660.adsp.prodQ.elf")
        CustomerPath = OUTDumpPath+"\\temp"
    else:
        if os.path.isfile(CRM_buildpath+"\\build\\ms\\M660660.adsp.testQ1234.elf"):
            shutil.copy2(CRM_buildpath+"\\build\\ms\\M660660.adsp.testQ1234.elf", OUTDumpPath+"\\temp\\ROOT_660.adsp.prodQ.elf")
            CustomerPath = OUTDumpPath+"\\temp"
            
    if CustomerPath!=False and os.path.isfile(CustomerPath+"\\M660660.adsp.testQ1234_AUDIO.elf"):
        shutil.copy2(CustomerPath+"\\M660660.adsp.testQ1234_AUDIO.elf", OUTDumpPath+"\\temp\\AUDIO_660.adsp.prodQ.elf")
    else:
        if os.path.isfile(CRM_buildpath+"\\build\\ms\\M660660.adsp.testQ1234_AUDIO.elf"):
            shutil.copy2(CRM_buildpath+"\\build\\ms\\M660660.adsp.testQ1234_AUDIO.elf", OUTDumpPath+"\\temp\\AUDIO_660.adsp.prodQ.elf")

           
    if CustomerPath!=False and os.path.isfile(CustomerPath+"\\M660660.adsp.testQ1234_SENSOR.elf"):
        shutil.copy2(CustomerPath+"\\M660660.adsp.testQ1234_SENSOR.elf", OUTDumpPath+"\\temp\\SENSOR_660.adsp.prodQ.elf")
    else:        
        if os.path.isfile(CRM_buildpath+"\\build\\ms\\M660660.adsp.testQ1234_SENSOR.elf"):
            shutil.copy2(CRM_buildpath+"\\build\\ms\\M660660.adsp.testQ1234_SENSOR.elf", OUTDumpPath+"\\temp\\SENSOR_660.adsp.prodQ.elf")
        
if CustomerPath!=False and os.path.isfile(CustomerPath+"\\ROOT_845.adsp.prodQ.elf"):
    print "ELF is present"
else:
    if CustomerPath!=False and os.path.isfile(CustomerPath+"\\M845845.adsp.testQ1234.elf"):
        shutil.copy2(CustomerPath+"\\M845845.adsp.testQ1234.elf", OUTDumpPath+"\\temp\\ROOT_845.adsp.prodQ.elf")
        CustomerPath = OUTDumpPath+"\\temp"
    else:
        if os.path.isfile(CRM_buildpath+"\\build\\ms\\M845845.adsp.testQ1234.elf"):
            shutil.copy2(CRM_buildpath+"\\build\\ms\\M845845.adsp.testQ1234.elf", OUTDumpPath+"\\temp\\ROOT_845.adsp.prodQ.elf")
            CustomerPath = OUTDumpPath+"\\temp"
            
    if CustomerPath!=False and os.path.isfile(CustomerPath+"\\M845845.adsp.testQ1234_AUDIO.elf"):
        shutil.copy2(CustomerPath+"\\M845845.adsp.testQ1234_AUDIO.elf", OUTDumpPath+"\\temp\\AUDIO_845.adsp.prodQ.elf")
    else:
        if os.path.isfile(CRM_buildpath+"\\build\\ms\\M845845.adsp.testQ1234_AUDIO.elf"):
            shutil.copy2(CRM_buildpath+"\\build\\ms\\M845845.adsp.testQ1234_AUDIO.elf", OUTDumpPath+"\\temp\\AUDIO_845.adsp.prodQ.elf")

           
    if CustomerPath!=False and os.path.isfile(CustomerPath+"\\M845845.adsp.testQ1234_SENSOR.elf"):
        shutil.copy2(CustomerPath+"\\M845845.adsp.testQ1234_SENSOR.elf", OUTDumpPath+"\\temp\\SENSOR_845.adsp.prodQ.elf")
    else:        
        if os.path.isfile(CRM_buildpath+"\\build\\ms\\M845845.adsp.testQ1234_SENSOR.elf"):
            shutil.copy2(CRM_buildpath+"\\build\\ms\\M845845.adsp.testQ1234_SENSOR.elf", OUTDumpPath+"\\temp\\SENSOR_845.adsp.prodQ.elf")
		
arg_file = OUTDumpPath+"\\temp\\CMMArguments.txt"
f_arg = open(arg_file,'w+')
cmd = '''%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n'''%(targetid,UniqueNumber,INDumpPath,OUTDumpPath,CRM_buildpath,CurrDirectory,CustomerPath,crashman_lite,slpi_flag,asha_flag,SOFILE,load_dump_full,quit_flag,smmu_enable,frpcshell,frpcshell_name,wp_smmu_enable,INDumpPath.split('\\')[-1],Enable_6GB_dumps,cdsp_flag,smmu_64bt,clara_output)
f_arg.write(cmd)
f_arg.close()
		
if quit_flag == True:
    Path = T32_path+"\\bin\\windows64"
    if os.path.exists(Path):
        command = '''%s\\bin\\windows64\\t32mqdsp6.exe -c %s, %s %s %s'''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb.t32",CurrDirectory+"\\DSP_load_memorydump_crashman.cmm",targetid,OUTDumpPath)
        process_t32 = subprocess.Popen(command, stdout=subprocess.PIPE)
        # os.system(command)
    else:    
        command = '''%s\\t32mqdsp6.exe -c %s, %s %s %s'''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb.t32",CurrDirectory+"\\DSP_load_memorydump_crashman.cmm",targetid,OUTDumpPath)
        process_t32 = subprocess.Popen(command, stdout=subprocess.PIPE)
        # os.system(command)
else:        
    if t32_loc != False:
        Path = T32_path+"\\bin\\windows64"
    else:
        T32_path = "C:\\T32"
        Path = "C:\\T32\\bin\\windows64"
    if os.path.exists(Path):
        command = '''start %s\\bin\\windows64\\t32mqdsp6.exe -c %s, %s %s %s'''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb.t32",CurrDirectory+"\\DSP_load_memorydump_crashman.cmm",targetid,OUTDumpPath)
        os.system(command)
    else:    
        command = '''start %s\\t32mqdsp6.exe -c %s, %s %s %s'''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb.t32",CurrDirectory+"\\DSP_load_memorydump_crashman.cmm",targetid,OUTDumpPath)
        os.system(command)

if asha_flag == True:
    file_check=OUTDumpPath+"\\temp\\dumpsloaddone.txt"
    while True:
        time.sleep(10) #wait for 1min for checking
        if os.path.exists(file_check):
            print "Dumps and ELF Loading Done!!!!!"
            break

    minutes=10
    dsp_check=OUTDumpPath+"\\temp\\DSPAnalysis.end"
    dsp_analy=OUTDumpPath+"\\DSPAnalysis.txt"

    cond = 0
    for i in range(0,minutes):
        time.sleep(60) #wait for 1min for checking
        if os.path.exists(dsp_check):
            cond = 1
            break
                
    if cond == 0:    
        f_dsp_check = open(dsp_check,'w+')
        f_dsp_analy = open(dsp_analy,'r+')
        for line in f_dsp_analy:
            f_dsp_check.write(line)
        f_dsp_analy.close()
        f_dsp_check.close()
    print "Searching for JIRA tickets"
    log=OUTDumpPath+"\\temp\\JiraLog.txt"
    command = '''python %s\DD_module.py %s > %s'''%(DDModular_Script,OUTDumpPath,log)
    os.system(command)
    dsp_check=OUTDumpPath+"\\temp\\DSPOutput.complete"
    dsp_analy=OUTDumpPath+"\\temp\\DSPAnalysis.end"
    f_dsp_check = open(dsp_check,'w+')
    f_dsp_analy = open(dsp_analy,'r+')
    for line in f_dsp_analy:
        f_dsp_check.write(line)
    f_dsp_analy.close()
    f_dsp_check.close()
    
    print "Searching for JIRA tickets Done"

    
if quit_flag == True:
    minutes=20
    dsp_check=OUTDumpPath+"\\temp\\DSPAnalysis.end"
    cond_1 = 0
    for i in range(0,minutes):
        time.sleep(60) #wait for 1min for checking
        if os.path.exists(dsp_check):
            cond_1 = 1
            break
    if cond_1 == 0:
        process_t32.terminate()
        raise ValueError('Crashman stuck or not able to load dumps properly.')
        
        
file_check=OUTDumpPath+"\\temp\\DSPAnalysis.end"
while True:
    time.sleep(10) #wait for 1min for checking
    if os.path.exists(file_check):
        if os.path.exists(OUTDumpPath+"\\AvailableLogs.txt"):
            os.rename(OUTDumpPath+"\\AvailableLogs.txt", OUTDumpPath+"\\temp\\AvailableLogs.txt")
        if os.path.exists(OUTDumpPath+"\\f3tokens_temp.txt"):
            os.rename(OUTDumpPath+"\\f3tokens_temp.txt", OUTDumpPath+"\\temp\\f3tokens_temp.txt")
        if os.path.exists(OUTDumpPath+"\\BAM.ulog"):
            os.rename(OUTDumpPath+"\\BAM.ulog", OUTDumpPath+"\\temp\\BAM.ulog")
        if os.path.exists(OUTDumpPath+"\\NPA Log.ulog"):
            os.rename(OUTDumpPath+"\\NPA Log.ulog", OUTDumpPath+"\\temp\\NPA Log.ulog")
        if os.path.exists(OUTDumpPath+"\\SLIMBUS.ulog"):
            os.rename(OUTDumpPath+"\\SLIMBUS.ulog", OUTDumpPath+"\\ulog\\SLIMBUS.ulog")
        if os.path.exists(OUTDumpPath+"\\search_op.json"):
            os.rename(OUTDumpPath+"\\search_op.json", OUTDumpPath+"\\temp\\search_op.json")
        if os.path.exists(OUTDumpPath+"\\Qurt_logs\\Heap_Analysis_GuestOS.txt"):
            shutil.copy2(OUTDumpPath+"\\Qurt_logs\\Heap_Analysis_GuestOS.txt", OUTDumpPath+"\\Def_Heap\\Heap_Analysis_GuestOS.txt")
        if os.path.exists(OUTDumpPath+"\\Qurt_logs\\Heap_Analysis_Audio.txt"):
            shutil.copy2(OUTDumpPath+"\\Qurt_logs\\Heap_Analysis_Audio.txt", OUTDumpPath+"\\Def_Heap_for_AUDIO_img\\Heap_Analysis_Audio.txt")
        if os.path.exists(OUTDumpPath+"\\Qurt_logs\\Heap_Analysis_Sensors.txt"):
            shutil.copy2(OUTDumpPath+"\\Qurt_logs\\Heap_Analysis_Sensors.txt", OUTDumpPath+"\\Def_Heap_forSensors_img\\Heap_Analysis_Sensors.txt")
        if os.path.isfile(CRM_buildpath+"\\performance\\tools\\SysmonParser.exe"):
            if os.path.isfile(OUTDumpPath+"\\temp\\sysmon_dump.bin"):
                print "Processing sysmonparser.exe."
            else:
                print "Please wait..."
                time.sleep(30)
            if os.path.isfile(OUTDumpPath+"\\temp\\sysmon_dump.bin"):
                try:
                    print "Processing sysmonparser.exe. Please wait..."
                    command = '''%s\\performance\\tools\\SysmonParser.exe %s\\temp\\sysmon_dump.bin %s\\sysmon_dump default  --crashman=%s\\temp'''%(CRM_buildpath,OUTDumpPath,OUTDumpPath,OUTDumpPath)
                    os.system(command)
                except:
                    print "sysmon parser is not working"
        for r,d,f in os.walk(OUTDumpPath+"\\ulog"):
            for file in f:
                win_dump  = os.path.join(r,file)
                if ".ulog" in win_dump:
                    file = open(win_dump,'r')
                    file_out = open(win_dump.replace('.ulog','_time.ulog'),'w')
                    for line in file:
                        try:
                            if "0x" in line[:2] or "0X" in line[:2]:
                                hex = line.split(':')[0]
                                ms = str(int(hex,16)/19200000.000).split('.')[1][:3]
                                sec = int(hex,16)/19200000
                                temp = str(datetime.timedelta(seconds=sec))
                                line_t = line.replace(hex,hex+" : "+temp+"."+ms+" ")
                            else:
                                line_t = line
                        except:
                            line_t = line
                        file_out.write(line_t)
                    file_out.close()
                    if os.path.isfile(win_dump.replace('.ulog','_time.ulog')):
                        shutil.copy2(win_dump.replace('.ulog','_time.ulog'), win_dump)
                        os.remove(win_dump.replace('.ulog','_time.ulog'))
                    file.close()
        break
        
if etm_flag == True and smmu_enable == True:
    dumpfile = None
    if "tmc-etr.bin" in INDumpPath:
        dumpfile = INDumpPath
    elif "etr.bin" in INDumpPath:
        dumpfile = INDumpPath
    elif os.path.isfile(os.path.dirname(INDumpPath)+"\\tmc-etr.bin"):
        dumpfile = os.path.dirname(INDumpPath)+"\\tmc-etr.bin"
    elif os.path.isfile(os.path.dirname(INDumpPath)+"\\etr.bin"):
        dumpfile = os.path.dirname(INDumpPath)+"\\etr.bin"
    elif os.path.isfile(OUTDumpPath+"\\temp\\tmc-etr.bin"):
        dumpfile = OUTDumpPath+"\\temp\\tmc-etr.bin"
    elif os.path.isfile(OUTDumpPath+"\\temp\\etr.bin"):
        dumpfile = OUTDumpPath+"\\temp\\etr.bin"
        
    if dumpfile == None:
        print "ETM Dump File is not accessible/available. Please Check the File Path."
        sys.exit(0)
        
    f_out = open(OUTDumpPath+"\\temp\\load_adsp_etm_dump.cmm",'wb')
    if targetid == "8994" or targetid == "8992" or targetid == "8952" or targetid == "8953" or targetid == "8937" or targetid == "8976":
        f_out.write('system.cpu HexagonV56\n')
    elif targetid == "8996":
        f_out.write('system.cpu HexagonV60\n')
    elif targetid == "8998":
        f_out.write('system.cpu HexagonV62\n')
    elif targetid == "660":
        if cdsp_flag == True:
            f_out.write('system.cpu HexagonV60\n')
        else:
            f_out.write('system.cpu HexagonV62\n')
    elif targetid == "845":
        if cdsp_flag == True:
            f_out.write('system.cpu HexagonV65\n')
        else:
            f_out.write('system.cpu HexagonV65\n')
    elif targetid == "670":
        if cdsp_flag == True:
            f_out.write('system.cpu HexagonV65\n')
        else:
            f_out.write('system.cpu HexagonV65\n')
    else:
        f_out.write('system.cpu HexagonV5\n')
    f_out.write('sys.config.etr.base 0x4\n')
    f_out.write('sys.config.tpiu.base 0x8\n')
    f_out.write('system.up\n')
    f_out.write('trace.method.la\n')
    f_out.write('etm.portmode wrapped\n')
    if targetid == "660":
        if cdsp_flag == True:
            f_out.write('etm.traceid 0x26\n')
        else:        
            f_out.write('etm.traceid 0x28\n')
    elif targetid == "670" or targetid == "845":
        if cdsp_flag == True:
            f_out.write('etm.traceid 0x26\n')
        else:        
            f_out.write('etm.traceid 0x28\n')
    else:        
        f_out.write('etm.traceid 0x28\n')
    f_out.write('etm.ca ON\n')
    f_out.write('analyzer.clock 500.mhz\n')
    f_out.write('la.mode.flowtrace.on\n')
    
    crash_log = OUTDumpPath+"\\temp\\Crashman_Log.txt"
    adsp = ""
    if os.path.isfile(crash_log):
        f_op = open(crash_log,'rb')
        for line in f_op:
            if "ADSP build Provided  :   " in line:
                adsp = line.split('ADSP build Provided  :   ')[1]
            if "CDSP build Provided  :   " in line:
                adsp = line.split('CDSP build Provided  :   ')[1]
            if "Loading ELF:" in line:
                elf = line.split('Loading ELF:')[1]
                if ".\\build\\ms" in elf:
                    elf = adsp.rstrip('\r\n') + "\\build\\ms\\" + elf.split('.\\build\\ms\\')[1]
                f_out.write('d.load.elf '+elf.rstrip('\r\n')+' /nocode /noclear\n')
        f_op.close()
    crash_log = OUTDumpPath+"\\temp\\loaddynmaic.cmm"
    if os.path.isfile(crash_log):
        f_op = open(crash_log,'rb')
        for line in f_op:
            if "d.load.elf" in line:
                f_out.write(line.replace('/nocode',''))
        f_op.close()
    f_out.write('la.import.etb '+dumpfile+'\n')
    f_out.write('trace.list tp tpc tpinfo def l.nd.off\n')
    f_out.write('enddo\n')    
    f_out.close()
    print "\nStarting T32 for ETM Dump."
    print "************************************"
    t32_file = OUTDumpPath+"\\temp\\config_sim_usb1.t32"
    f_t32 = open(t32_file,'w+')
    f_t32.write("OS=\n")
    f_t32.write("ID=SIM_${4}            ; unique ID needed for temporary file name uniqueness\n")
    f_t32.write("TMP="+OUTDumpPath+"\\temp"+"           ; T32 temp file names are prepended with the unique ID above\n")
    if quit_flag == True:
        f_t32.write("SYS="+T32_path+"                ; make system directory the same as the executable directory\n\n")
    else:
        if t32_loc != False:
            f_t32.write("SYS="+T32_path+"                ; make system directory the same as the executable directory\n\n")
        else:
            f_t32.write("SYS=C:\T32                ; make system directory the same as the executable directory\n\n")
    f_t32.write("SCREEN=\n")
    f_t32.write("HEADER=${4}\n\n")
    f_t32.write("; Ethernet on Host information \n")
    f_t32.write("PBI=SIM\n")
    f_t32.write("\n")
    f_t32.write("; Printer settings\n")
    f_t32.write("PRINTER=WINDOWS\n\n")
    f_t32.write("; Screen fonts\n")
    f_t32.write("SCREEN=\n")
    f_t32.write("FONT=SMALL\n\n")
    f_t32.close()

    if t32_loc != False:
        Path = T32_path+"\\bin\\windows64"
    else:
        T32_path = "C:\\T32"
        Path = "C:\\T32\\bin\\windows64"
    
    if os.path.exists(Path):
        command = '''start %s\\bin\\windows64\\t32mqdsp6.exe -c %s, %s '''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb1.t32",OUTDumpPath+"\\temp\\load_adsp_etm_dump.cmm")
        os.system(command)
    else:    
        command = '''start %s\\t32mqdsp6.exe -c %s, %s '''%(T32_path,OUTDumpPath+"\\temp\\config_sim_usb1.t32",OUTDumpPath+"\\temp\\load_adsp_etm_dump.cmm")
        os.system(command)
    sys.exit(0)
