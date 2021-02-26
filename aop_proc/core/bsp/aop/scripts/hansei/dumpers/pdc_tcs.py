from __future__ import print_function
import struct
import logging
logger = logging.getLogger(__name__)
import json
from pprint import pprint
import os
import itertools
import math
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *
from targ_spec_config import *
import csv
from summary import chip_version
import xml.etree.ElementTree as ET
import pdb


drvList = ["SENSORS","COMPUTE","MODEM","APPS"]
hwioName = ["SSC","TURING","MSS","APSS"]

dic = {}

def readReg(address):
  
    address2 = int(address, 16)
    data = memory.read(address2, 4)
    value = struct.unpack('<L', data)[0]
   
    return value

def dump(debug_data): #dump_path, memory, debug_info, fill_char):
    global dump_error
    global memory
    global numDrv
    
    numDrv = 1
    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    fill_char = debug_data['args'].fill_char
    dump_path = debug_data['dump_path']
    target = debug_data['target'] 
    
    debug_data['printer'].p("Dumping PDC TCS Contents...")
    
    try:
        dump_path = os.path.join(dump_path, 'PDC_Contents')
        if not os.path.exists(dump_path):
            os.makedirs(dump_path)
        
        pdc2_file_name = os.path.join(dump_path, 'LastReq.txt')
        open(pdc2_file_name, 'w').close()
    
        for i in range(4):

            image = drvList[i]
            
            if image == "APPS":
                numDrv = 3
                
            with open(pdc2_file_name, 'a') as pdc2_file:
            
                print("===========================================", file=pdc2_file)
                print("Image       : " +image, file=pdc2_file)
                
                pdc_file_name = os.path.join(dump_path, image +'.txt')
                

                with open(pdc_file_name, 'w') as pdc_file:

                    hwioPath = os.path.abspath(os.path.join(os.path.dirname( __file__ ), '..', 'tcs_content_dump', target,  hwioName[i], 'hwiosave_pdc.xml'))

                    tree = ET.parse(hwioPath)
                    root = tree.getroot()

                    for chip in root.findall('chip'):
                        for register in chip.findall('register'):
                            name = register.get('name')
                            address = register.get('address')
                            #print("register " + name + " address " + address)
                            #pdb.set_trace()
                            dic[name]=readReg(address)
                    
                    for j in range(numDrv):                    
                        voteCmpReg="RPMH_PDC_"+image+"_VOTE_CMP_DATA_DRV"+str(j)
                        epcbRxAddrReg="RPMH_PDC_"+image+"_DBG_EPCB_RX_ADDR"+str(j)
                        epcbRxDataReg="RPMH_PDC_"+image+"_DBG_EPCB_RX_DATA"+str(j)
                        
                        print("-------------------------------------------", file=pdc2_file)
                        print("Drv no.      : " +str(j), file=pdc2_file)
                        
                        voteCmp = dic[voteCmpReg]
                        slvId = (voteCmp&0xE00000)>>0x15
                        offset = (voteCmp&0xffff)>>0x0
                        
                        print("Last Acked.         --> "+str(hex(voteCmp)) + " (Slave: " + str(slvId) +", Offset: " +str(hex(offset))+" )", file=pdc2_file)
                        
                        epcbRxAddr = dic[epcbRxAddrReg]
                        slvId = (epcbRxAddr&0xE00000)>>0x15
                        offset = (epcbRxAddr&0xffff)>>0x0
                        
                        print("Last Rcvd. Addr.    --> "+str(hex(epcbRxAddr)) + " (Slave: " + str(slvId) +", Offset: " +str(hex(offset))+" )", file=pdc2_file)
                        print("Last Rcvd. Data.    --> "+str(hex(dic[epcbRxDataReg])), file=pdc2_file)
                    
                    
                    rscParamReg="RPMH_PDC_"+image+"_PDC_PARAM_RESOURCE_DRV0"

                    numCmd=(dic[rscParamReg]&0xe0)>>0x5
                    numTcs=(dic[rscParamReg]&0xf00)>>0x8
     
                    for i in range(numTcs):
                        print("===========================================", file=pdc_file)
                        print("TCS no.      : " +str(i), file=pdc_file)

                        controlReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_CONTROL"
                        #print(controlReg)
                        isAmc=(dic[controlReg]&0x10000)>>0x10
                        if(isAmc==0):
                            print("Type         : PDC Sleep/Wake TCS", file=pdc_file)
                        else:
                            print("Type         : PDC Active TCS", file=pdc_file)

                        idleReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_DRV"+str(j)+"_STATUS"
                        isIdle=(dic[idleReg]&0x1)>>0x0
                        if(isIdle==0):
                            print("Status       : Busy", file=pdc_file)
                        else:
                            print("Status       : Idle", file=pdc_file)
                            
                        print("=============================================", file=pdc_file)
                        print("No  Enbld  R/W   Resp Len Complete Issue Trigger   Address     Data", file=pdc_file)
                        print("---------------------------------------------------------------------", file=pdc_file)
                        for k in range(numCmd):
                            commandEnableReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_CMD_ENABLE_BANK"
                            
                            #print(dic[commandEnableReg])
                            isEnabled=dic[commandEnableReg]&(1<<k)

                            if(isEnabled==0):
                                enabledStr="N"
                            else:
                                enabledStr="Y"
                                
                            cmdAddressReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_CMD"+str(k)+"_ADDR"
                            cmdAddress=dic[cmdAddressReg]

                            cmdDataReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_CMD"+str(k)+"_DATA"
                            cmdData=dic[cmdDataReg]

                            msgIdReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_CMD"+str(k)+"_MSGID"
                            msgId=dic[msgIdReg]

                            respReq=(msgId&0x100)>>0x8
                            if(respReq==0):
                                resp="F&F"
                            else:
                                resp="Req"

                            readWrite=(msgId&0x10000)>>0x10
                            if(readWrite==0):
                                read=" Read"
                            else:
                                read="Write"

                            msgLength=(msgId&0xf)>>0x00

                            cmdStatusReg="RPMH_PDC_"+image+"_TCS"+str(i)+"_CMD"+str(k)+"_DRV"+str(j)+"_STATUS"
                            #print(cmdStatusReg)
                            cmdStatus=dic[cmdStatusReg]

                            #print(cmdStatus)

                            completedVal=(cmdStatus&0x10000)>>0x10
                            issuedVal=(cmdStatus&0x100)>>0x8
                            triggeredVal=(cmdStatus&0x1)>>0x0
                                
                                

                            print(str(k)+ "   " + enabledStr +"  " +read +"   " + resp + "  "+  str(msgLength) +"      " + str(completedVal) +"       "+ str(issuedVal) + "      " + str(triggeredVal)+ "      "+ '{:8s}'.format(str(hex(cmdAddress))) +  "     "+ str(hex(cmdData)), file=pdc_file)

        
                
    except Exception as e:
        dump_error = 1
        
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")
