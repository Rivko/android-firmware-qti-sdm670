import sys
import re
import os

#input = ipage_address
#outputs-
# found_smmu
# smmu_vpage_address, smmu_ppage_address, smmu_size
current_path=os.getcwd()
input=str(sys.argv[1])
smmu_filename=str(sys.argv[2])
pgt_filename=str(sys.argv[3])
cdsp_flag=str(sys.argv[4])
print input
os.chdir(input)

if cdsp_flag == "True":
    smmu_fastrpc_list = { \
    '0x1421':'msm_fastrpc_compute_cb1',\
    '0x1422':'msm_fastrpc_compute_cb2',\
    '0x1423':'msm_fastrpc_compute_cb3',\
    '0x1424':'msm_fastrpc_compute_cb4',\
    '0x1425':'msm_fastrpc_compute_cb5',\
    '0x1426':'msm_fastrpc_compute_cb6',\
    '0x1429':'msm_fastrpc_compute_cb7',\
    '0x142A':'msm_fastrpc_compute_cb8',\

    }
else:
    smmu_fastrpc_list = { \
    '0x1803':'msm_fastrpc_compute_cb9',\
    '0x1804':'msm_fastrpc_compute_cb10',\
    '0x1805':'msm_fastrpc_compute_cb11',\
    '0x1806':'msm_fastrpc_compute_cb12',\
    }

def create_smmutxt():
    f_smmutxt = open ('smmu.txt', 'w')
    for cnt in range(0,800):
        cnt1 = str("{0:0=2d}".format(cnt))
        smmu_files = 'msm_iommu_domain_'+cnt1+".txt"
        if not os.path.isfile(smmu_files):  
            smmu_files = 'arm_iommu_domain_'+cnt1+".txt"
        if os.path.isfile(smmu_files):
            f_smmuInputfile= open (smmu_files, 'r')
            
            first = 0
            for line_smmufile in f_smmuInputfile:
                if first == 0:
                    if not "audio" in line_smmufile:
                        break
                first = 1
                if (line_smmufile[0:2] == '0x'):
                    if not (re.search('UNMAPPED',line_smmufile)):
                        f_smmutxt.write(line_smmufile)
        
            f_smmuInputfile.close()
    f_smmutxt.close()

def create_smmutxt_asid():

    for key,smmu_fastrpc in smmu_fastrpc_list.items():
        f_smmutxt = open (smmu_fastrpc+'_1.txt', 'w')
        for cnt in range(0,800):
            cnt1 = str("{0:0=2d}".format(cnt))
            smmu_files = 'msm_iommu_domain_'+cnt1+".txt"
            if not os.path.isfile(smmu_files):
                smmu_files = 'arm_iommu_domain_'+cnt1+".txt"
            if os.path.isfile(smmu_files):
                f_smmuInputfile = open (smmu_files, 'r')
                first = 0
                for line_smmufile in f_smmuInputfile:
                    if first == 0:
                        if not smmu_fastrpc in line_smmufile:
                            break
                    first = 1
                    if (line_smmufile[0:2] == '0x'):
                        if not (re.search('UNMAPPED',line_smmufile)):
                            f_smmutxt.write(line_smmufile)
            
                f_smmuInputfile.close()
        f_smmutxt.close()
        #Parsing time reduced
        f_dsppgt = open (smmu_fastrpc+'_1.txt', 'r')
        f_out = open (smmu_fastrpc+'.txt', 'w')
        temp0 = "0"
        temp1 = "0"
        temp2 = 0
        temp3 = "0"
        temp4 = "0"
        temp5 = 0
        for line_dsppgt in f_dsppgt:
            if ( line_dsppgt[0:2] == '0x'):
                list_dsppgt = line_dsppgt.split(' ')
                t0 = list_dsppgt[0].split('--')[0]
                t1 = list_dsppgt[0].split('--')[1]
                t2 = list_dsppgt[1].split('[')[1].split(']')[0]
                t3 = list_dsppgt[2].split('A:')[1].split('--')[0]
                t4 = list_dsppgt[2].split('A:')[1].split('--')[1]
                t5 = list_dsppgt[3].split('[')[1].split(']')[0]
                if (int(temp1,16)+1==int(t0,16) and int(temp4,16)+1==int(t3,16)):
                    temp1 = t1
                    temp2 = temp2+int(t2,16)
                    temp4 = t4
                    temp5 = temp5 + int(t5,16)
                else:
                    if temp0 != "0":
                        f_out.write(temp0+'--'+temp1+' ['+str(hex(temp2))+'] A:'+temp3+'--'+temp4+' ['+str(hex(temp5))+'] [R/W][4K] \n')
                    temp0 = t0
                    temp1 = t1
                    temp2 = int(t2,16)
                    temp3 = t3
                    temp4 = t4
                    temp5 = int(t5,16)
        if temp0 != "0":
            f_out.write(temp0+'--'+temp1+' ['+str(hex(temp2))+'] A:'+temp3+'--'+temp4+' ['+str(hex(temp5))+'] [R/W][4K] \n')
        f_dsppgt.close()
        f_out.close()
        f_out = open (smmu_fastrpc+'.txt', 'r')
        for line in f_out:
            temp = line.split('--')[0]
            if int(temp,16) >= int('0x80000000',16) and int(temp,16) < int('0x81000000',16):
                f_out1 = open (smmu_fastrpc+'_0x80.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x81000000',16) and int(temp,16) < int('0x82000000',16):
                f_out1 = open (smmu_fastrpc+'_0x81.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x82000000',16) and int(temp,16) < int('0x83000000',16):
                f_out1 = open (smmu_fastrpc+'_0x82.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x83000000',16) and int(temp,16) < int('0x84000000',16):
                f_out1 = open (smmu_fastrpc+'_0x83.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x84000000',16) and int(temp,16) < int('0x85000000',16):
                f_out1 = open (smmu_fastrpc+'_0x84.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x85000000',16) and int(temp,16) < int('0x86000000',16):
                f_out1 = open (smmu_fastrpc+'_0x85.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x86000000',16) and int(temp,16) < int('0x87000000',16):
                f_out1 = open (smmu_fastrpc+'_0x86.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x87000000',16) and int(temp,16) < int('0x88000000',16):
                f_out1 = open (smmu_fastrpc+'_0x87.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x88000000',16) and int(temp,16) < int('0x89000000',16):
                f_out1 = open (smmu_fastrpc+'_0x88.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            elif int(temp,16) >= int('0x89000000',16) and int(temp,16) < int('0x8a000000',16):
                f_out1 = open (smmu_fastrpc+'_0x89.txt', 'a')
                f_out1.write(line)
                f_out1.close()
            else:
                f_out1 = open (smmu_fastrpc+'_else.txt', 'a')
                f_out1.write(line)
                f_out1.close()
        f_out.close()

    
def find_smmu(ipage_address):

    f_smmu = open ('smmu.txt', 'rU')  
    for line_smmu in f_smmu:
        list_smmu = re.split(']|0x| |--',line_smmu)
        smmu_vpage_address = int(list_smmu[1],16)        
        smmu_ppage_address = int(list_smmu[8],16)
        smmu_size = int(list_smmu[5],16)
        found_smmu=0
        if (ipage_address >= smmu_vpage_address) and (ipage_address < (smmu_vpage_address+smmu_size)):
            found_smmu = 1
            #print line_smmu #to be commented
            break
            
    f_smmu.close()
    return (found_smmu, smmu_vpage_address, smmu_ppage_address, smmu_size)
    
def find_smmu_asid(ipage_address,asid):

    element = None
    if asid not in smmu_fastrpc_list:
        element = "smmu.txt"
    else:
        element = smmu_fastrpc_list[asid]+'.txt'
        if ipage_address >= int('0x80000000',16) and ipage_address < int('0x81000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x80.txt'):
                element = smmu_fastrpc_list[asid]+'_0x80.txt'
        elif ipage_address >= int('0x81000000',16) and ipage_address < int('0x82000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x81.txt'):
                element = smmu_fastrpc_list[asid]+'_0x81.txt'
        elif ipage_address >= int('0x82000000',16) and ipage_address < int('0x83000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x82.txt'):
                element = smmu_fastrpc_list[asid]+'_0x82.txt'
        elif ipage_address >= int('0x83000000',16) and ipage_address < int('0x84000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x83.txt'):
                element = smmu_fastrpc_list[asid]+'_0x83.txt'
        elif ipage_address >= int('0x84000000',16) and ipage_address < int('0x85000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x84.txt'):
                element = smmu_fastrpc_list[asid]+'_0x84.txt'
        elif ipage_address >= int('0x85000000',16) and ipage_address < int('0x86000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x85.txt'):
                element = smmu_fastrpc_list[asid]+'_0x85.txt'
        elif ipage_address >= int('0x86000000',16) and ipage_address < int('0x87000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x86.txt'):
                element = smmu_fastrpc_list[asid]+'_0x86.txt'
        elif ipage_address >= int('0x87000000',16) and ipage_address < int('0x88000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x87.txt'):
                element = smmu_fastrpc_list[asid]+'_0x87.txt'
        elif ipage_address >= int('0x88000000',16) and ipage_address < int('0x89000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x88.txt'):
                element = smmu_fastrpc_list[asid]+'_0x88.txt'
        elif ipage_address >= int('0x89000000',16) and ipage_address < int('0x8a000000',16):
            if os.path.isfile(smmu_fastrpc_list[asid]+'_0x89.txt'):
                element = smmu_fastrpc_list[asid]+'_0x89.txt'
        else:
            if os.path.isfile(smmu_fastrpc_list[asid]+'_else.txt'):
                element = smmu_fastrpc_list[asid]+'_else.txt'
    f_smmu = open (element, 'rU')  
    for line_smmu in f_smmu:
        list_smmu = re.split(']|0x| |--',line_smmu)
        smmu_vpage_address = int(list_smmu[1],16)        
        smmu_ppage_address = int(list_smmu[8],16)
        smmu_size = int(list_smmu[5],16)
        found_smmu=0
        if (ipage_address >= smmu_vpage_address) and (ipage_address < (smmu_vpage_address+smmu_size)):
            found_smmu = 1
            #print line_smmu #to be commented
            break
            
    f_smmu.close()
    return (found_smmu, smmu_vpage_address, smmu_ppage_address, smmu_size)


def map_smmu(vpage_address, ipage_address, size, f_out):
        
    vpage_address_temp= vpage_address
    ipage_address_temp= ipage_address
    size_temp = size
    f_output = f_out
    while(size_temp!= 0x0):
        found_smmu=0
        smmu_vpage_address=0x0
        smmu_ppage_address=0x0
        smmu_size=0x0 
        result_find_smmu= find_smmu(ipage_address_temp)
        found_smmu= result_find_smmu[0]
        smmu_vpage_address = result_find_smmu[1]
        smmu_ppage_address = result_find_smmu[2]
        smmu_size = result_find_smmu[3]
        
        if found_smmu:
            size3=  smmu_vpage_address+smmu_size- ipage_address_temp
            size1=  smmu_size - size3
            size2 = size_temp
            if (size3 <= size_temp): 
                size2 = size3 
            smmu_ppage_result_address = smmu_ppage_address + size1
            f_output.write( 'mmu.create ' + hex(vpage_address_temp).rstrip('L') + '++' + hex(size2-1).rstrip('L')+' ' + hex(smmu_ppage_result_address).rstrip('L') + '++' + hex(size2-1).rstrip('L')+'\n' )
            size_temp =  size_temp-size2
            #print 'size_temp'+ hex(size_temp)#
            vpage_address_temp = vpage_address_temp+ size2
            ipage_address_temp = ipage_address_temp+ size2
        else:
            f_output.write( 'mmu.create ' + hex(vpage_address_temp).rstrip('L') + '++' + hex(size_temp-1).rstrip('L')+' ' + hex(ipage_address_temp).rstrip('L') + '++' + hex(size_temp-1).rstrip('L')+'\n' )
            size_temp=0x0
    
def map_smmu_asid(vpage_address, ipage_address, size, f_out,asid):
        
    vpage_address_temp= vpage_address
    ipage_address_temp= ipage_address
    size_temp = size
    f_output = f_out
    while(size_temp!= 0x0):
        found_smmu=0
        smmu_vpage_address=0x0
        smmu_ppage_address=0x0
        smmu_size=0x0 
        result_find_smmu= find_smmu_asid(ipage_address_temp,asid)
        found_smmu= result_find_smmu[0]
        smmu_vpage_address = result_find_smmu[1]
        smmu_ppage_address = result_find_smmu[2]
        smmu_size = result_find_smmu[3]
        
        if found_smmu:
            size3=  smmu_vpage_address+smmu_size- ipage_address_temp
            size1=  smmu_size - size3
            size2 = size_temp
            if (size3 <= size_temp): 
                size2 = size3 
            smmu_ppage_result_address = smmu_ppage_address + size1
            f_output.write( 'mmu.create ' + hex(vpage_address_temp).rstrip('L') + '++' + hex(size2-1).rstrip('L')+' ' + hex(smmu_ppage_result_address).rstrip('L') + '++' + hex(size2-1).rstrip('L')+'\n' )
            size_temp =  size_temp-size2
            #print 'size_temp'+ hex(size_temp)#
            vpage_address_temp = vpage_address_temp+ size2
            ipage_address_temp = ipage_address_temp+ size2
        else:
            f_output.write( 'mmu.create ' + hex(vpage_address_temp).rstrip('L') + '++' + hex(size_temp-1).rstrip('L')+' ' + hex(ipage_address_temp).rstrip('L') + '++' + hex(size_temp-1).rstrip('L')+'\n' )
            size_temp=0x0


def main():
        
    f_out= open (smmu_filename,'w')
    f_dsppgt= open (pgt_filename, 'rU')
    # f_out.write('mmu.reset\n')
    create_smmutxt()
    create_smmutxt_asid()
    
    for line_dsppgt in f_dsppgt:
        if ( line_dsppgt[0:2] == '0x'):
            list_dsppgt = line_dsppgt.split()
            vpage_address = int(list_dsppgt[0].replace('*',''),16)
            vpage_address = vpage_address * 0x1000
            asid = None
            if len(list_dsppgt[1]) > 8:
                asid = str(list_dsppgt[1][1:4])
                list_dsppgt[1] = list_dsppgt[1].replace('*','')
                list_dsppgt[1] = '0x'+list_dsppgt[1][3:]
            ipage_address= int(list_dsppgt[1].replace('*',''),16)
            ipage_address = ipage_address * 0x1000
            size= int(list_dsppgt[2].replace('*',''),16)
            #print line_dsppgt
            if asid == None:
                map_smmu(vpage_address, ipage_address, size, f_out)
            else:
                map_smmu_asid(vpage_address, ipage_address, size, f_out,asid)
    

    f_out.write('mmu.format QURT QURTK_pagetables\n')
    f_out.write('mmu.on\n')
    f_out.write('mmu.autoscan on\n')
    f_out.write('mmu.tablewalk on\n')
    
    f_dsppgt.close()
    f_out.close()



if __name__ == '__main__':
  main()
  
os.chdir(current_path)
