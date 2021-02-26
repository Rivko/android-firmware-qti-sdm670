import os
import re
import subprocess
import sys
import time
import datetime
import fnmatch
import shutil, errno
from os import walk


def CheckLoadCMM(DumpPath):
    file = DumpPath + "\\load.cmm"
    binary_list = []
    if os.path.isfile(file):
        f = open(file,'r')
        for line in f:
            if 'd.load.binary' in line:
                temp = line.split()
                if os.path.isfile(DumpPath+"\\"+temp[1]):
                    temp.append(hex(os.path.getsize(DumpPath+"\\"+temp[1])).replace('L',''))
                    binary_list.append(temp)
        f.close()
    return binary_list
    
def GetAddress(Output,address,size,address_1,size_1,DumpPath,binary_list,line_bsc):
    fil = Output + '\\6GB_BasicMapping_Ranges.txt'
    print line_bsc
    if os.path.isfile(fil):
        f_fil = open(fil,'r')
        if int(address,16) >= int('0xFFFFFFFF',16):
            start =  "0x"+address[3:]
            print start,size
        else:
            start = address
        size_check = size
        start_end = hex(int(start,16) + int(size_check,16)).replace('L','')
        cond = False
        for line in f_fil:
            add = line.split()[3]
            siz = line.split()[5]
            srt_add = add
            srt_end = hex(int(add,16) + int(siz,16)).replace('L','')
            if (int(start,16) >= int(srt_add,16) and int(start,16) <= int(srt_end,16)) or (int(start_end,16) >= int(srt_add,16) and int(start_end,16) <= int(srt_end,16)):
                print int(start,16), int(srt_add,16) , int(start,16) , int(srt_end,16),(int(start,16) >= int(srt_add,16) and int(start,16) <= int(srt_end,16))
                print int(start_end,16) , int(srt_add,16) , int(start_end,16) , int(srt_end,16),(int(start_end,16) >= int(srt_add,16) and int(start_end,16) <= int(srt_end,16))
                print line
                cond = True
                break
        if cond == False:
            f_fil.close()
            f_fil = open(fil,'a')
            f_fil.write(address_1+' -- '+size_1+'          '+start+' -- '+size_check+'\n')
            f_fil.close()
            value = None
            for name in binary_list:
                if int(address,16) >= int(name[2],16) and int(address,16) <= int(name[2],16)+int(name[4],16):
                    value = name
                    break
            if value != None:
                file_cmm = open(Output+'\\6GB_Binary_Loading.cmm','a')
                skip = hex( int(address,16) - int(name[2],16)).replace('L','')
                file_cmm.write(';'+line_bsc)
                file_cmm.write('mmu.create '+address_1+'++'+size_1+' '+start+'++'+size_check+'\n')
                file_cmm.write('d.load.binary '+DumpPath+'\\'+value[1]+'  a:'+start+'++'+size_check+' /SKIP '+str(skip)+' /noclear\n\n')
                file_cmm.close()
                return
        
        f_fil = open(fil,'r')
        size_check = size
        start = '0x0'
        while(1):
            start_end = hex(int(start,16) + int(size_check,16)).replace('L','')
            cond = False
            for line in f_fil:
                add = line.split()[3]
                siz = line.split()[5]
                srt_add = add
                srt_end = hex(int(add,16) + int(siz,16)).replace('L','')
                if (int(start,16) >= int(srt_add,16) and int(start,16) <= int(start_end,16)) or (int(start_end,16) >= int(srt_add,16) and int(start_end,16) <= int(srt_end,16)):
                    cond = True
                    break
            if cond == False:
                # print start,size_check
                f_fil.close()
                f_fil = open(fil,'a')
                f_fil.write(address_1+' -- '+size_1+'          '+start+' -- '+size_check+'\n')
                f_fil.close()
                value = None
                for name in binary_list:
                    if int(address,16) >= int(name[2],16) and int(address,16) <= int(name[2],16)+int(name[4],16):
                        value = name
                        break
                if value != None:
                    file_cmm = open(Output+'\\6GB_Binary_Loading.cmm','a')
                    skip = hex( int(address,16) - int(name[2],16)).replace('L','')
                    file_cmm.write(';'+line_bsc)
                    file_cmm.write('mmu.create '+address_1+'++'+size_1+' '+start+'++'+size_check+'\n')
                    file_cmm.write('d.load.binary '+DumpPath+'\\'+value[1]+'  a:'+start+'++'+size_check+' /SKIP '+str(skip)+' /noclear\n\n')
                    file_cmm.close()
                    break
            start = hex(int(start,16)+int(size_check,16)+1).replace('L','')

def CreateCMM(Output,DumpPath,binary_list):
    file = Output + '\\DSP_smmu_pagetables_nonbasic.cmm'
    if os.path.isfile(file):
        f_file = open(file,'r')
        for line in f_file:
            if 'mmu.create' in line:
                address_1 = line.split( )[1].split('++')[0]
                size_1 = line.split( )[1].split('++')[1]
                address = line.split( )[2].split('++')[0]
                size = line.split( )[2].split('++')[1]
                GetAddress(Output,address,size,address_1,size_1,DumpPath,binary_list,line)
        f_file.close()
    f_out = open(Output + '\\6GB.cmm','w')
    f_out1 = open(Output + '\\6GB_Dumps.cmm','w')
    f_out.write('mmu.reset\n')
    f_check = Output + '\\DSP_smmu_pagetables_basic.cmm'
    if os.path.isfile(f_check):
        f_open = open(f_check,'r')
        for line in f_open:
            if 'mmu.create' in line:
                f_out.write(line)
        f_open.close()
    f_check = Output + '\\6GB_Binary_Loading.cmm'
    if os.path.isfile(f_check):
        f_open = open(f_check,'r')
        for line in f_open:
            if 'd.load.binary' in line:
                f_out1.write(line)
                f_out.write(';'+line)
            else:
                f_out.write(line)
        f_open.close()
        
    # f_check = Output + '\\6GB_Binary_Loading.cmm'
    # if os.path.isfile(f_check):
        # f_open = open(f_check,'r')
        # f_temp = open(f_check.replace('.cmm','_temp.cmm'),'a')
        # while True:
            # line = f_open.readline()
            # if not line:break
            # line1 = f_open.readline()
            # if not line1:break
            # line2 = f_open.readline()
            # if not line2:break
            # f_temp.write(line)
            # f_temp.write(line1)
            # f_temp.write(line2)
        # f_temp.close()
        # f_open.close()                    

    # f_check = Output + '\\6GB_Binary_Loading.cmm'
    # if os.path.isfile(f_check):
        # f_open = open(f_check,'r')
        # while True:
            # line1 = f_open.readline()
            # if not line1:break
            # line2 = f_open.readline()
            # if not line2:break
            # line3 = f_open.readline()
            # if not line3:break
            # f_1 = open(f_check.replace('.cmm','_temp.cmm'),'r')
            # cond_check = False
            # while True:
                # line_1 = f_1.readline()
                # if not line_1:break
                # line_2 = f_1.readline()
                # if not line_2:break
                # line_3 = f_1.readline()
                # if not line_3:break
                # if ';mmu.create' in line1:
                    # temp = line1.split()[2].split('++')[0]
                    # temp1 = line1.split()[2].split('++')[1]
                    # temp2 = line2.split()[2].split('++')[0]
                    # if ';mmu.create' in line_1 and temp in line_1 and temp1 in line_1 and (not temp2[3:] in line_1 or temp2 == "0x0"):
                        # f_out.write(line_1)
                        # f_out.write(';'+line_2)
                        # f_out.write(';'+line_3)
                        # cond_check = True                        
                        # print "888888888888"+line_1,line_2,line_3
                        # break
                # elif line1 == line_1 and line2 == line_2 and line3 == line_3:
                    # f_out.write(line_1)
                    # f_out.write(line_2)
                    # f_out.write(line_3)
                    # cond_check = True
                    # print "777777777"+line_1,line_2,line_3
                    # break
            # f_1.close()
            # if cond_check == False:
                # f_out.write(line1)
                # f_out.write(line2)
                # f_out.write(line3)
        # f_open.close()
    f_out.write('mmu.format QURT QURTK_pagetables\n')
    f_out.write('mmu.on\n')
    f_out.write('mmu.autoscan on\n')
    f_out.write('mmu.tablewalk on\n')
    f_out.close()
    f_out1.close()
    
def SortPGT(file):
    f_srt = open(file.replace('.txt','_sort.txt'),'w')
    list = []
    if os.path.isfile(file):
        f_file = open(file,'r')
        for line in f_file:
            list.append(line)
        f_file.close()
    # print len(list)
    a = []
    for i in range(0,len(list)):
        for j in range(i+1,len(list)):
            temp1 = list[i].split()[1].replace('*','')
            temp2 = list[j].split()[1].replace('*','')
            temp1 = int(temp1,16)
            temp2 = int(temp2,16)
            if temp2 <= temp1:
                a = list[i]
                list[i] = list[j]
                list[j] = a
    for i in range(0,len(list)):
        f_srt.write(str(list[i]))
    f_srt.close()
    
def CheckRangesandClub(file):    
    file_out = file.replace('.txt','_Ranges.txt')
    f_out = open(file_out,'w')
    if os.path.isfile(file):
        f_file = open(file,'r')
        va = None
        pa = None
        size = None
        size_add = None
        for line in f_file:
            list = line.split()
            if va == None and pa == None and size == None:
                va = list[0]+"000"
                pa = list[1].replace('*','')+"000"
                size = list[2].replace('*','')
                size_add = size
            else:
                va_temp = list[0]+"000"
                pa_temp = list[1].replace('*','')+"000"
                size_temp = list[2].replace('*','')
                if int(pa,16) + int(size_add,16) == int(pa_temp,16):
                    size_add = str(hex(int(size_add,16) + int(size_temp,16))).replace('L','')
                else:
                    f_out.write(va+' -- '+size_add+'          '+pa+' -- '+size_add+'\n')
                    va = va_temp
                    pa = pa_temp
                    size_add = size_temp
        f_file.close()
    f_out.close()
    
def main():
    OutputPath  = sys.argv[1]
    DumpPath    = os.path.dirname(sys.argv[2])
    CurrentDirectory = sys.argv[3]
    targetid = sys.argv[4]
    cdsp_flag = sys.argv[5]
    clara_op=sys.argv[6]
    pgt_file = OutputPath+"\\DSP_pgt.txt"
    if os.path.isfile(pgt_file):
        f_dsppgt = open(pgt_file,'rU')
        f_bsc = open(OutputPath+"\\6GB_BasicMapping.txt",'w')
        f_nonbsc = open(OutputPath+"\\6GB_NonBasicMapping.txt",'w')
        for line_dsppgt in f_dsppgt:
            if ( line_dsppgt[0:2] == '0x'):
                list_dsppgt = line_dsppgt.split()
                asid = None
                if len(list_dsppgt[1]) > 8:
                    asid = str(list_dsppgt[1][1:4])
                if asid == "0x0":
                    f_bsc.write(line_dsppgt)
                else:
                    f_nonbsc.write(line_dsppgt)
                
        f_bsc.close()
        f_nonbsc.close()
        f_dsppgt.close()
        
    # SortPGT(OutputPath+"\\6GB_BasicMapping.txt")
    CheckRangesandClub(OutputPath+"\\6GB_BasicMapping.txt")
    CheckRangesandClub(OutputPath+"\\6GB_NonBasicMapping.txt")
    if targetid == "660":
        if clara_op=="False":
            command = '''python %s\\ADSP_SMMU_660_6GB.py %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt',cdsp_flag)
            print command
            os.system(command)
            command = '''python %s\\ADSP_SMMU_660_6GB.py %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt',cdsp_flag)
            print command
            os.system(command)
        else:
            command = '''python %s\\clara_SMMU_6GB.py %s %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt',cdsp_flag,targetid)
            print command
            os.system(command)
            command = '''python %s\\clara_SMMU_6GB.py %s %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt',cdsp_flag,targetid)
            print command
            os.system(command)            
    elif targetid == "845":
        if clara_op=="False":
            command = '''python %s\\ADSP_SMMU_845_6GB.py %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt',cdsp_flag)
            print command
            os.system(command)
            command = '''python %s\\ADSP_SMMU_845_6GB.py %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt',cdsp_flag)
            print command
            os.system(command)
        else:
            command = '''python %s\\clara_SMMU_6GB.py %s %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt',cdsp_flag,targetid)
            print command
            os.system(command)
            command = '''python %s\\clara_SMMU_6GB.py %s %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt',cdsp_flag,targetid)
            print command
            os.system(command)
    elif targetid == "670":
        if clara_op=="False":
            command = '''python %s\\ADSP_SMMU_670_6GB.py %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt',cdsp_flag)
            print command
            os.system(command)
            command = '''python %s\\ADSP_SMMU_670_6GB.py %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt',cdsp_flag)
            print command
            os.system(command)
        else:
            command = '''python %s\\clara_SMMU_6GB.py %s %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt',cdsp_flag,targetid)
            print command
            os.system(command)
            command = '''python %s\\clara_SMMU_6GB.py %s %s %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt',cdsp_flag,targetid)
            print command
            os.system(command)
    else:
        command = '''python %s\\ADSP_SMMU_8996_6GB.py %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_basic.cmm','6GB_BasicMapping.txt')
        os.system(command)
        command = '''python %s\\ADSP_SMMU_8996_6GB.py %s %s %s'''%(CurrentDirectory,OutputPath,'DSP_smmu_pagetables_nonbasic.cmm','6GB_NonBasicMapping.txt')
        os.system(command)
    binary_list = CheckLoadCMM(DumpPath)
    CreateCMM(OutputPath,DumpPath,binary_list)
    

if __name__ == '__main__':
  main()
