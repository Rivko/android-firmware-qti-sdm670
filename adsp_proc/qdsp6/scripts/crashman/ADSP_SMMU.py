import sys
import re
import os

#input = ipage_address
#outputs-
# found_smmu
# smmu_vpage_address, smmu_ppage_address, smmu_size
current_path=os.getcwd()
input=str(sys.argv[1])
print input
os.chdir(input)
def create_smmutxt():
    f_smmutxt = open ('smmu.txt', 'w')
    for cnt in range(0,800):
        cnt1 = str("{0:0=2d}".format(cnt))
        smmu_files = 'msm_iommu_domain_'+cnt1+".txt"
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
        
        #to be commented
        #print found_smmu
        #print hex(smmu_vpage_address)
        #print hex(smmu_ppage_address)
        #print hex(smmu_size)
        if found_smmu:
            size3=  smmu_vpage_address+smmu_size- ipage_address_temp
            #print 'size3'+ hex(size3)#
            size1=  smmu_size - size3
            #print 'size1'+ hex(size1)#
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
        
    f_out= open ('DSP_smmu_pagetables.cmm','w')
    f_dsppgt= open ('DSP_pgt.txt', 'rU')
    f_out.write('mmu.reset\n')
    create_smmutxt()
    
    for line_dsppgt in f_dsppgt:
        if ( line_dsppgt[0:2] == '0x'):
            list_dsppgt = line_dsppgt.split()
            vpage_address = int(list_dsppgt[0].replace('*',''),16)
            vpage_address = vpage_address * 0x1000
            if len(list_dsppgt[1]) > 8:
                list_dsppgt[1] = list_dsppgt[1].replace('*','')
                list_dsppgt[1] = '0x'+list_dsppgt[1][3:]
            ipage_address= int(list_dsppgt[1].replace('*',''),16)
            ipage_address = ipage_address * 0x1000
            size= int(list_dsppgt[2].replace('*',''),16)
            #print line_dsppgt
            map_smmu(vpage_address, ipage_address, size, f_out)
    

    f_out.write('mmu.format QURT QURTK_pagetables\n')
    f_out.write('mmu.on\n')
    f_out.write('mmu.autoscan on\n')
    f_out.write('mmu.tablewalk on\n')
    
    f_dsppgt.close()
    f_out.close()



if __name__ == '__main__':
  main()
  
os.chdir(current_path)
