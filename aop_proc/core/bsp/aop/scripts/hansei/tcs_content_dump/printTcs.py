import xml.etree.ElementTree as ET
import sys
import re
import os
import subprocess

devnull = open(os.devnull, 'w')

dumppath=sys.argv[1]
outputpath=sys.argv[2]

print("")

dccParserPath = "\\\\faith\\qipl_lss\\tools\\LRDPv2_production\\tools\\dcc_parser\\dcc_parser.py"

try:
    subprocess.call("python " + dccParserPath + " -s " + dumppath + os.sep + "DCC_SRAM.BIN --config-offset=0x6000 --v2 -o " + outputpath,stdout=devnull, stderr=devnull)
except:
    print("No access to dcc_parser. Skipping.")
    sys.exit(0)
    
dccpath=outputpath+os.sep+"dcc_captured_data.xml"

if not os.path.exists(dccpath):
    print("#    DCC Parsing not done. Skipping.")
    sys.exit(0)

if not os.path.exists(outputpath + os.sep + "TCS_Contents"):
    os.makedirs(outputpath + os.sep + "TCS_Contents")

images=["APSS","TURING","SSC","MSS"]

for image in images:

    filepath=outputpath+os.sep+"TCS_Contents"+os.sep+image+".txt"
    ipcatpath=os.path.dirname(__file__)+os.sep+"670"+os.sep+image+os.sep+"hwiosave.xml"

    try:
        dcctree = ET.parse(dccpath)
        dccroot = dcctree.getroot()

        ipcattree = ET.parse(ipcatpath)
        ipcatroot = ipcattree.getroot()
    except:
        sys.exit(0)
        
    fp = open(filepath,"w")

    drvNo = ["0"]

    dic = {}
    dic2 = {}

    for chip in ipcatroot.findall('chip'):
        for register in chip.findall('register'):
            name = register.get('name')
            address = register.get('address')
            address2 = int(address,16)
            address3 = "0x{:08x}".format(address2)
            #fp.write("register " + address + " value " + value)
            dic[name]=address3

    for chip in dccroot.findall('chip'):
        for register in chip.findall('register'):
            address = register.get('address')
            value = register.get('value')
            #fp.write("register " + address + " value " + value)
            dic2[address]=value



    fp.write("Image           : " +image +"\n")

    if(image=="APSS"):
        image="APSS_RSC"
        drvNo = ["2"]


    rscParamReg=image+"_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRV"+drvNo[0]
    try:
        numCmd=(int(dic2[dic[rscParamReg]],16)&0xf8000000)>>0x1b
        numTcs=(int(dic2[dic[rscParamReg]],16)&0x3f)>>0x0
    except KeyError:
        sys.exit(0)

    for i in range(numTcs):
        fp.write("===========================================\n")
        fp.write("TCS no.                   : " +str(i)+"\n")
        for j in drvNo:
            controlReg=image+"_RSCC_TCS"+str(i)+"_DRV"+str(j)+"_CONTROL"
            #fp.write(controlReg)
            #isAmc=(int(dic2[dic[controlReg]],16)&0x10000)>>0x10
            if(dic2.has_key(dic[controlReg])):
                fp.write("Type                      : RSC Sleep/Wake TCS\n")
            else:
                fp.write("Type                      : RSC Active TCS\n")
                fp.write("Contents not being backed up in DCC!\n")
                continue

            idleReg=image+"_RSCC_TCS"+str(i)+"_DRV"+str(j)+"_STATUS"
            isIdle=(int(dic2[dic[idleReg]],16)&0x1)>>0x0
            if(isIdle==0):
                fp.write("Status                    : Busy\n")
            else:
                fp.write("Status                    : Idle\n")

            irqEnableReg=image+"_RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV"+drvNo[0]
            irqStatusReg=image+"_RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV"+drvNo[0]

            irqEnable=int(dic2[dic[irqEnableReg]],16)&(1<<i)
            #fp.write(int(dic[irqEnableReg],16))
            if(irqEnable==0):
                fp.write("AMC Completion IRQ Mode   : Disabled\n")
            else:
                fp.write("AMC Completion IRQ Mode   : Enabled\n")

            irqStatus=int(dic2[dic[irqStatusReg]],16)&(1<<i)
            #fp.write(int(dic[irqStatusReg],16))
            if(irqStatus==0):
                fp.write("AMC Completion IRQ Status : Disabled\n")
            else:
                fp.write("AMC Completion IRQ Status : Enabled\n")
                
            fp.write("======================================\n")
            fp.write("No  Enbld     Address        Data\n")
            fp.write("--------------------------------------\n")
            for k in range(numCmd):
                commandEnableReg=image+"_RSCC_TCS"+str(i)+"_DRV"+str(j)+"_CMD_ENABLE"
                
                #fp.write(dic[commandEnableReg])
                isEnabled=int(dic2[dic[commandEnableReg]],16)&(1<<k)

                if(isEnabled==0):
                    enabledStr="N"
                else:
                    enabledStr="Y"
                    
                cmdAddressReg=image+"_RSCC_TCS"+str(i)+"_CMD"+str(k)+"_DRV"+str(j)+"_ADDR"
                cmdAddress=dic2[dic[cmdAddressReg]]

                cmdDataReg=image+"_RSCC_TCS"+str(i)+"_CMD"+str(k)+"_DRV"+str(j)+"_DATA"
                cmdData=dic2[dic[cmdDataReg]]

                
                fp.write(str(format(k, '02d'))+ "   " + enabledStr + "      "+ '{:8s}'.format(cmdAddress) +  "     "+ cmdData + "\n")


    fp.close()

print("#   Extracted the TCS Contents from the DCC Backup")
                

