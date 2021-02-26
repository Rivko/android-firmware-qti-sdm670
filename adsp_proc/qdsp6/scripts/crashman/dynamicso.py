import os
import re
import subprocess
import sys
import time
import datetime
import fnmatch
import shutil, errno
from os import walk

log_mask = 1

def build_from_id(id):
    build=""
    output = subprocess.Popen(["//sun/CRMTools/Smart/nt/bin/Findbuild.exe","*"+id+"*"],stdout = subprocess.PIPE).communicate()[0]
    builds = output.split("\r\n\r\n\r\n")
    for build in builds:
        if build and ("\nStatus:         Completed" in build or "\nStatus:         Approved" in build):
            build = build.split("Location:")[1].split("\r\n")[0].strip()
            break
    return build

def GetBuildID(timestamp):
    buildid = None
    if "MSM8952" in timestamp:
        if "1.0" in timestamp:
            buildid=timestamp.split(".0")[0]
            buildid=buildid+".0"
        else:    
            buildid = temp2.split('_')[0]
    return buildid    
    
    
def GetStartAddress(list,OutputPath):
    log=OutputPath+"\\temp\\soaddress.txt"
    Addr=None
    if "_unstrip" in list:
        list = list.split("_unstrip")[0]
    elif "_unsigned" in list:
        list = list.split("_unsigned")[0]
    else:
        list = list.split(".")[0]
        list=list+".so"
        
    try:
        if os.path.exists(log):
            f_log = open(log,'rb')
            for line in f_log:
                line=line.rstrip('\n').rstrip('\r')
                soname = "Dummy String for Testing Verification"
                if '"' in line:
                    soname=line.replace('./','').rstrip('\n').rstrip('\r').split('"')[1].split('.')[0]
                if list in line or soname in list:
                    temp = line.split(" Start address: ")[1]
                    temp1 = temp.split(",")[0]
                    Addr = temp1
                    break
    except:
        print "File Not Exits soaddress.bin"
    
    return Addr 

def GetFileNameExtension(soname,dynamicpath,OutputPath,SoNameList):

    ext=None
    for line in SoNameList:
        line=line.rstrip('\n').rstrip('\r')
        if soname in line:
            if ".so" in line:
                ext = line.split('line')[-1]
                if "unstrip" in ext:
                    break
    return ext
    
def GetTimeStamp(file,name,DynamicSOList_temp):
    TS = []
    try:
        if os.path.exists(file):
            for list in DynamicSOList_temp:
                if list in name:
                    soname = list
                    break
            f_file = open(file,'rb')
            brk = 1
            for line in f_file:
                if "Q6_BUILD_TS_Q6_BUILD_TS" in line:
                    if soname in line:
                        temp = line.split(soname)
                        for list in temp:
                            if "Q6_BUILD_TS_Q6_BUILD_TS" in list:
                                TS.append(list.replace('\x00',''))
                                brk = 0
                                break
                if brk == 0:
                    break
            f_file.close()
    except:
        f_file.close()
        print "File Not Present"+file,name    
    return TS
    
    
def GetCRMBuildSoFileNamePathAndExtension(crmpath,OutputPath,Target_Name):
    if Target_Name == "660":
        if "adsp_proc" in crmpath:
            avs_files = crmpath + "\\build\\dynamic_modules\\660.adsp.prod\\map_AVS_SHARED_LIBS_660.adsp.prodQ.txt"
            non_avs_files = crmpath + "\\build\\dynamic_modules\\660.adsp.prod\\map_SHARED_LIBS_660.adsp.prodQ.txt"
        else:
            avs_files = crmpath + "\\build\\dynamic_modules\\660.cdsp.prod\\map_AVS_SHARED_LIBS_660.adsp.prodQ.txt"
            non_avs_files = crmpath + "\\build\\dynamic_modules\\660.cdsp.prod\\map_SHARED_LIBS_660.adsp.prodQ.txt"
    else:
        avs_files = crmpath + "\\build\\dynamic_modules\\map_avs_shared_libs.txt"
        non_avs_files = crmpath + "\\build\\dynamic_modules\\map_SHARED_LIBS_AAAAAAAAQ.txt"
    if not os.path.isfile(avs_files):
        if os.path.isfile(crmpath + "\\build\\dynamic_modules\\"+Target_Name+"\\map_avs_shared_libs.txt"):
            avs_files = crmpath + "\\build\\dynamic_modules\\"+Target_Name+"\\map_avs_shared_libs.txt"
            non_avs_files = crmpath + "\\build\\dynamic_modules\\"+Target_Name+"\\map_SHARED_LIBS_AAAAAAAAQ.txt"
    if not os.path.isfile(avs_files):
        return
        sys.exit(0)
    
    log = OutputPath+"\\temp\\adsp_sofiles.txt"
    f_log = open(log,'w+')

    file_crmpath = crmpath
    comparepath=None
    if os.path.isfile(avs_files):
        f_file = open(avs_files,'rb')
        lines=f_file.read().splitlines()
        for line in lines:
            if line:
                temp = line.split('/')
                if "../../" in line:
                    crmpath1 = crmpath
                elif "../" in line:
                    crmpath1 = crmpath + "\\build"
                else:
                    crmpath1 = crmpath + "\\build\\ms"
                file_crmpath = crmpath1
                for i in range(0,len(temp)):
                    if not ".." in temp[i] and not ".so" in temp[i] and not "fastrpc_shell_0" in temp[i]:
                        file_crmpath = file_crmpath + "\\"+temp[i]
                if comparepath != file_crmpath:
                    comparepath = file_crmpath
                    list = os.listdir(file_crmpath)
                    for i in range(0,len(list)):
                        if ".so" in list[i]:
                            f_log.write(file_crmpath+"\\"+list[i]+"\n")
        f_file.close()
        
    file_crmpath = crmpath
    comparepath=None
    if os.path.isfile(non_avs_files):
        f_file = open(non_avs_files,'rb')
        lines=f_file.read().splitlines()
        for line in lines:
            if line:
                temp = line.split('/')
                if "../../" in line:
                    crmpath1 = crmpath
                elif "../" in line:
                    crmpath1 = crmpath + "\\build"
                else:
                    crmpath1 = crmpath + "\\build\\ms"
                file_crmpath = crmpath1
                for i in range(0,len(temp)):
                    if not ".." in temp[i] and not ".so" in temp[i] and not "fastrpc_shell_0" in temp[i]:
                        file_crmpath = file_crmpath + "\\"+temp[i]
                if comparepath != file_crmpath:
                    comparepath = file_crmpath
                    list = os.listdir(file_crmpath)
                    for i in range(0,len(list)):
                        if ".so" in list[i]:
                            f_log.write(file_crmpath+"\\"+list[i]+"\n")
        f_file.close()
                
    f_log.close()

def CheckCRMBuildForSoFiles(soname,OutputPath):
    ret = None
    log = OutputPath+"\\temp\\adsp_sofiles.txt"
    if os.path.isfile(log):
        f_log = open(log,'r+')
        for line in f_log:
            if soname in line and not "inter" in line:
                ret = line
                if "unstrip" in line:
                    break
    return ret
    
def main():
    OutputPath  = sys.argv[1]
    crmpath     = sys.argv[2]
    dynamicpath = sys.argv[3]
    inpdumppath = sys.argv[4]
    Target_Name = sys.argv[5]
    if dynamicpath == "False":
        dynamicpath = crmpath
    log=OutputPath+"\\temp\\soaddress.txt"
    if log_mask==1:
        error_log = OutputPath+"\\temp\\errorlog.txt"
        f_error = open(error_log,'a+')
    
    GetCRMBuildSoFileNamePathAndExtension(crmpath,OutputPath,Target_Name)    
    DynamicSOList=[]
    DynamicSOList_temp=[]
    SoNameList = []
    for (dirpath, dirnames, filenames) in walk(dynamicpath):
        SoNameList.extend(filenames)
        break    
    try:
        if os.path.isfile(log):
            f_log = open(log,'rb')
            for line in f_log:
                if " Start address: " in line:
                    line = line.rstrip('\n').rstrip('\r')
                    temp1 = line.split("\"")[1]
                    if "./" in temp1:
                        soname = temp1.split("./")[1].split(".")[0]
                    else:
                        soname = temp1.split(".")[0]
                    DynamicSOList_temp.append(soname)
                    
            cmm=OutputPath+"\\temp\\loaddynmaic.cmm"
            f_cmm = open(cmm,'w+')
            load=OutputPath+"\\DynamicSoLoaded.txt"
            f_load=open(load,'w+')
            unload=OutputPath+"\\DynSoFile.txt"
            f_unload=open(unload,'a+')
            f_unload.write("\n\nDynamic So's Not Loaded\n")
            f_unload.write("==========================\n")
            first = 0
            for soname in DynamicSOList_temp:
                list = GetFileNameExtension(soname,dynamicpath,OutputPath,SoNameList)
                test_cond = 0
                test_fil = 0
                test_list = 0
                fil =  dynamicpath
                if list:
                    fil = fil+"\\"+list
                if os.path.isfile(fil):
                    test_cond = 1
                    test_fil = fil
                    test_list = list
                list1 = None
                ret = CheckCRMBuildForSoFiles(soname,OutputPath)
                if ret != None:
                    list1 = ret.split('\\')[-1]
                    sofilepath = ret.split('\\'+list1)[0]
                    list1 = list1.strip()
                else:
                    sofilepath = dynamicpath
                if True:
                    fil =  dynamicpath
                    if list:
                        fil = fil+"\\"+list
                    if os.path.isfile(fil):
                        fil =  dynamicpath+"\\"+list
                    else:
                        if list1:
                            fil =  sofilepath+"\\"+list1
                            list = list1
                    if test_cond == 1:
                        fil = test_fil
                        sofilepath = dynamicpath
                        list = test_list
                    if os.path.isfile(fil):
                        ts_dump = []
                        ts_so_file = []
                        if  os.path.isfile(OutputPath+"\\slpi_Dump.bin"):
                            ts_dump = GetTimeStamp(OutputPath+"\\slpi_Dump.bin",list,DynamicSOList_temp)
                        elif  os.path.isfile(OutputPath+"\\Adsp_Dump.bin"):
                            ts_dump = GetTimeStamp(OutputPath+"\\Adsp_Dump.bin",list,DynamicSOList_temp)
                        elif  os.path.isfile(inpdumppath):
                            ts_dump = GetTimeStamp(inpdumppath,list,DynamicSOList_temp)
                        ts_so_file = GetTimeStamp(sofilepath+"\\"+list,list,DynamicSOList_temp)
                        res = cmp(ts_dump,ts_so_file)
                        f_cmm.write("area.select Crashman\n")
                        Addr = GetStartAddress(list,OutputPath)
                        if Addr != None:
                            if res == 0 or Target_Name == "660":
                                f_cmm.write("print \"Dynamic Object Loading: "+sofilepath+"\\"+list+"\"\n")
                                f_cmm.write("d.load.elf "+sofilepath+"\\"+list+" "+Addr+" /nocode /noclear\n")
                                f_cmm.write("print \"Dynamic Object Loading: Done"+"\"\n")
                            else:
                                f_cmm.write("print \"Time Stamp Mismatch "+sofilepath+"\\"+list+"\"\n")
                                f_unload.write("Mismatched Time Stamp "+sofilepath+"\\"+list+"\n")
                            if first == 0:
                                f_load.write("Dynamic Loaded So's\n")
                                f_load.write("==========================\n\n")
                                first = 1
                            f_load.write(sofilepath+"\\"+soname+"\n")
                        else:
                            f_unload.write(sofilepath+"\\"+soname+"\n")
                    else:
                        f_unload.write(sofilepath+"\\"+soname+"\n")
                else:
                    f_unload.write(sofilepath+"\\"+soname+"\n")
                
                
            f_log.close()
            f_cmm.close()
            f_unload.close()
            f_load.close()
                    
    except:
        print "File Not Exits Adsp_Dump.bin"
        if log_mask==1:f_error.write("File Not Exits Adsp_Dump.bin")
        sys.exit()
    if log_mask==1:
        f_error.close()

if __name__ == '__main__':
  main()
