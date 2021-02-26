import sys
import re
import os

#input = ipage_address
#outputs-
# found_smmu
# smmu_vpage_address, smmu_ppage_address, smmu_size
current_path=os.getcwd()
input=str(sys.argv[1])
wp_files=str(sys.argv[2])
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
    
def check_wp_smmu(ipage_address):
    physcal_page=False
    for r,d,f in os.walk(wp_files):
        for file in f:
            wp_pt  = os.path.join(r,file)
            f_wp = open(wp_pt,'rU')
            for line_dsppgt in f_wp:
                if ( line_dsppgt[0:2] == '0x'):
                    # print "Search ",str(hex(ipage_address).rstrip('L'))
                    if str(hex(ipage_address).rstrip('L')) in line_dsppgt:
                        physcal_page = line_dsppgt.split()[3]
                        print line_dsppgt
                        print "physical page ", physcal_page,ipage_address
                        return physcal_page
                        break
            f_wp.close()
    return physcal_page
    
def map_smmu(vpage_address, ipage_address, size, f_out):
        
    vpage_address_temp= vpage_address
    ipage_address_temp= ipage_address
    ipage_address_end = ipage_address+size
    size_temp = size
    f_output = f_out
    while(ipage_address_temp < ipage_address_end):
        physpage = check_wp_smmu(ipage_address_temp)
        if physpage != False:
            print hex(vpage_address_temp).rstrip('L'),physpage
            f_output.write( 'mmu.create ' + hex(vpage_address_temp).rstrip('L') + '++0xFFF' +' ' + physpage + '++0xFFF' +'\n' )
        else:
            f_output.write( 'mmu.create ' + hex(vpage_address_temp).rstrip('L') + '++0xFFF' +' ' + hex(ipage_address_temp).rstrip('L') + '++0xFFF' +'\n' )
        
        ipage_address_temp = ipage_address_temp + 0x1000
        vpage_address_temp = vpage_address_temp + 0x1000


def main():
        
    f_out= open ('DSP_smmu_pagetables.cmm','w')
    f_dsppgt= open ('DSP_pgt.txt', 'rU')
    f_out.write('mmu.reset\n')
    # create_smmutxt()
    
    for line_dsppgt in f_dsppgt:
        if ( line_dsppgt[0:2] == '0x'):
            list_dsppgt = line_dsppgt.split()
            vpage_address = int(list_dsppgt[0].replace('*',''),16)
            vpage_address = vpage_address * 0x1000
            if len(list_dsppgt[1]) > 8:
                list_dsppgt[1] = list_dsppgt[1].replace('*','')
                list_dsppgt[1] = '0x'+list_dsppgt[1][3:]
                # print list_dsppgt[1]
            ipage_address= int(list_dsppgt[1].replace('*',''),16)
            ipage_address = ipage_address * 0x1000
            # print hex(ipage_address)
            size= int(list_dsppgt[2].replace('*',''),16)
            # print line_dsppgt
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
