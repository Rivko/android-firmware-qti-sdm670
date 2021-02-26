import os
import re
import sys
import shutil
import math
flag = 0  
count = 0
i = 0
finPDSize=[]
finPDalgHole=[]
finPDIslandSize=[]
pdendVA=0
pdendSize=0
pdstartVA=0
pdmidVA=0
pdmidSize=0
allPDSize=0
allAligHole=0
sdcimg=0
dataSize=0
arg_len=len(sys.argv)
chipset=sys.argv[1]
imageName=sys.argv[2]
output=sys.argv[3]
secfile=sys.argv[4]
n=5
IslandHeapSizeDec1 = 0
CommonHeap = 0
UserHeap = 0
IslandHeapSizeDec = 0
CommonHeapSizeDec = 0
UserHeapSizeDec = 0
IslandHeapSizeDec = 0
SdcHeapSizeDec = 0
UserHeapSizeAudioDec = 0
threePds = 0
srmImage = 0
if arg_len == 7:
    p=2
    srmImage = 1
elif arg_len == 8:
    p=3
    threePds=1
else:
    p=1

secfilecheck = re.search('.+section_info_sdc\.txt', secfile)
if secfilecheck:
    sdcimg = 1  
    
for counter in range(5,n+p):
    mapfile=sys.argv[counter]
    infile = open("%s" % mapfile, 'r')
    lines = infile.readlines()
    line_number = 0
    while line_number < len(lines):
        to_line = lines[line_number]
        line_number += 1
        CommonHeap = re.search('^.bss.qurtos_app_heap\s+0x\w+\s+(\w+)\s+.+', to_line)
        if not CommonHeap:
            CommonHeap = re.search('^.bss.user.config\s+0x\w+\s+(\w+)\s+.+', to_line)
        if CommonHeap:
            CommonHeapSize=CommonHeap.group(1)
            CommonHeapSizeDec=int(CommonHeapSize, 16)
            CommonHeapSizeDec=float(CommonHeapSizeDec/(1024.0*1024.0))
        SdcHeap = re.search('^.bss.main_heap\s+0x\w+\s+(\w+)\s+.+', to_line)
        if SdcHeap:
            SdcHeapSize=SdcHeap.group(1)
            SdcHeapSizeDec=int(SdcHeapSize, 16)
            SdcHeapSizeDec=float(SdcHeapSizeDec/(1024.0))        
        UserHeap = re.search('^.bss.sns_main_heap\s+0x\w+\s+(\w+)\s+.+', to_line)        
        if not UserHeap:
            UserHeap = re.search('^.bss.sns_def_heap_mem\s+0x\w+\s+(\w+)\s+.+', to_line)
        if UserHeap:
            UserHeapSize=UserHeap.group(1)
            UserHeapSizeDec=int(UserHeapSize, 16)
            UserHeapSizeDec=float(UserHeapSizeDec/(1024.0*1024.0))
        UserHeapAudio = re.search('^.bss.my_sbrk_heap\s+0x\w+\s+(\w+)\s+.+', to_line)        
        if UserHeapAudio:
            UserHeapSizeAudio=UserHeapAudio.group(1)
            UserHeapSizeAudioDec=int(UserHeapSizeAudio, 16)
            UserHeapSizeAudioDec=float(UserHeapSizeAudioDec/(1024.0*1024.0))
        IslandHeap = re.search('^.bss.sns_island_heap.IS.island\s+0x\w+\s+(\w+)\s+.+', to_line)
        if not IslandHeap:
            IslandHeap = re.search('^.data.MEMMGR.IS.island\s+0x\w+\s+(\w+)\s+.+', to_line)
        if IslandHeap:
            IslandHeapSize=IslandHeap.group(1)            
            IslandHeapSizeDec=int(IslandHeapSize, 16) 
            if IslandHeapSizeDec != 0:
                IslandHeapSizeDec1=float(IslandHeapSizeDec/(1024.0*1024.0))       
    infile.close()
outputfile = open("%s" % output, 'w')
myfile = open("%s" % secfile, 'r')

if sdcimg == 1:
    for line in myfile:
        words=re.split('\s+', line)
        if words[1].isdigit():
           addr = int(words[4], 16)
           if words[2] == "CODE_RAM":
                codeSize = int(words[3], 16)
                codeSizeKB = float(codeSize/(1024.0))
                startaddr = int(words[4], 16)
           elif addr != 0:
                dataSize += int(words[3], 16)
                dataSizeKB = float(dataSize/(1024.0))
                endaddr = int(words[4], 16)
    TotalSdcSize = endaddr - startaddr
    sdcToSize = 96*1024
    AvailSDCSize = sdcToSize - (codeSize + dataSize)
    AvailSDCSizeKB = float(AvailSDCSize/(1024.0))
    dataSizeKB = dataSizeKB - SdcHeapSizeDec
    if codeSize > 65536:
        print "*****************SDC Memory FootPrint************************"
        print ('Error: SDC Code Size greater than declared memory size 64KB\n')
        outputfile.write("Error: SDC Code Size greater than declared memory size 64KB\n")
        codeSizeExceed = 65536 - codeSize
        codeSizeExceedKB = float(codeSizeExceed/(1024.0))
        print "SDC CODE Size exceeded by:                %.3fKB" %codeSizeExceedKB 
        outputfile.write("SDC CODE Size exceeded by:                %.3fKB" %codeSizeExceedKB)        
        print "*****************SDC Memory FootPrint End*********************" 
    elif dataSize > 32768:
        print "*****************SDC Memory FootPrint************************"
        print ('Error: SDC Data Size greater than declared memory size 32KB\n')
        outputfile.write("Error: SDC Data Size greater than declared memory size 32KB\n")
        dataSizeExceed = 32768 - dataSize
        dataSizeExceedKB = float(dataSizeExceed/(1024.0))
        print "SDC DATA Size exceeded by:                %.3fKB" %dataSizeExceedKB 
        outputfile.write("SDC DATA Size exceeded by:                %.3fKB" %dataSizeExceedKB)        
        print "*****************SDC Memory FootPrint End*********************"         
    else:
        print "*****************SDC Memory FootPrint************************"
        print "SDC CODE Size:                %.3fKB" %codeSizeKB 
        outputfile.write("SDC CODE Size:                %.3fKB" %codeSizeKB)
        print "SDC DATA Size:                %.3fKB" %dataSizeKB
        outputfile.write("SDC DATA Size:                %.3fKB" %dataSizeKB)
        print "SDC Heap Size:                %.3fKB" %SdcHeapSizeDec
        outputfile.write("SDC Heap Size:                %.3fKB" %SdcHeapSizeDec)
        print "SDC Available Size:           %.3fKB" %AvailSDCSizeKB
        outputfile.write("SDC Available Size:           %.3fKB" %AvailSDCSize)    
        print "*****************SDC Memory FootPrint End*********************"                  
        
else:    
 for line in myfile:
    words=re.split('\s+', line)    
    if words[1].isdigit():
          if (words[1] == '0') and (flag == 1):
                pdendVA=pdendVA+pdendSize
                pdilFinSize=pdendVA & 0x00ffffff    
                pdSize=(pdmidVA+pdmidSize)-pdstartVA    
                pdSize=float(pdSize/(4.0*1024.0))
                pdSize=math.ceil(pdSize)
                pdSize=pdSize*4*1024                                               
                pdfinSize=pdSize+pdilFinSize
                pdaliHole=float(pdfinSize/(1024.0*1024.0))
                pdaliHole=math.ceil(pdaliHole)
                pdaliHole=pdaliHole*1024*1024
                pdaliHole=pdaliHole-pdfinSize
                count=count+1
                finPDSize.insert(i, pdfinSize)
                finPDalgHole.insert(i, pdaliHole)
                finPDIslandSize.insert(i, pdilFinSize)
                i=i+1                
                pdendVA=0
                pdendSize=0
                pdstartVA=0
                pdmidVA=0
                pdmidSize=0
          flag = 1
          m=re.search('island', words[2])
          if words[2]  != ".qskernel_vspace":
              d=re.search('qskernel', words[2])
          else:
              d=0              
          if m or d:
              pdendVA=int(words[4], 16)
              pdendSize=int(words[3], 16)
          x=re.search('\.start.+', words[2])
          y=re.search('\.start', words[2])  
          u=re.search('\.qskstart', words[2])
          if x or y or u:             
             pdstartVA=int(words[4], 16)
          z=re.search('QSR_STRING', words[2])
          k=re.search('\.sdata', words[2])
          j=re.search('\.bss', words[2])
          h=re.search('\.sdata.srm.main', words[2])
          if z or k or j or h:
             pdmidVA=int(words[4], 16)
             pdmidSize=int(words[3], 16)

 pdendVA=pdendVA+pdendSize
 pdilFinSize=pdendVA & 0x00ffffff              
 pdSize=(pdmidVA+pdmidSize)-pdstartVA               
 if pdilFinSize != 0:
   pdSize=float(pdSize/(4.0*1024.0))
   pdSize=math.ceil(pdSize)
   pdSize=pdSize*4*1024                                               
 pdfinSize=pdSize+pdilFinSize
 pdaliHole=float(pdfinSize/(256.0*1024.0))
 pdaliHole=math.ceil(pdaliHole)
 pdaliHole=pdaliHole*256*1024
 pdaliHole=pdaliHole-pdfinSize
 count=count+1
 finPDSize.insert(i, pdfinSize)
 finPDalgHole.insert(i, pdaliHole)
 finPDIslandSize.insert(i, pdilFinSize)

 if os.path.exists('../../build/chipset/%s/%s/qdsp6.xml'%(chipset,imageName)):
   infile = open("../../build/chipset/%s/%s/qdsp6.xml" % (chipset,imageName), 'r')
 elif os.path.exists('../../build/chipset/%s/qdsp6.xml'% (chipset)):    
   infile = open("../../build/chipset/%s/qdsp6.xml" % chipset, 'r')
 else:
   print "Not able to find qdsp6.xml file";
   exit()
 lines = infile.readlines()          
 line_number = 0
 while line_number < len(lines):
   to_line = lines[line_number]
   z=re.search('\<l2_size\s+value\=\"(\w+)\"\/\>', to_line)
   if z:
        l2Size = z.group(1)
        if chipset == 'sdm845' or chipset == 'sdm670':
            l2Size = 1024
            l2Size = float((l2Size/1024.0))
        else:    
            l2Size = int(l2Size)
            l2Size = float((l2Size/1024.0))
   x=re.search('\<physical_pool name\=\"DEFAULT\_PHYSPOOL\"\>', to_line)
   if x:
        to_line = lines[line_number+1]
        x=re.search('\<region base\=\"\w+\" size\=\"(\w+)\" \/\>', to_line)
        if x:
           Image_Size=x.group(1)
           Image_SizeDec=int(Image_Size, 16)
           Image_SizeMB=float(Image_SizeDec/(1024.0*1024.0))
        else:
           to_line = lines[line_number+2]
           x=re.search('\<region base\=\"\w+\" size\=\"(\w+)\" \/\>', to_line)
           if x:
              Image_Size=x.group(1)
              Image_SizeDec=int(Image_Size, 16)
              Image_SizeMB=float(Image_SizeDec/(1024.0*1024.0))
   y=re.search('\<trace_size value \= \"(\w+)\" \/\>', to_line)
   if y:
         Trace_Size=y.group(1)
         Trace_Size=int(Trace_Size)
         if chipset == 'sdm845':
             if imageName == 'adsp' or imageName == 'slpi':
                 hwThreadNum = 2
             elif imageName == 'cdsp':
                 hwThreadNum = 4
         else:
             hwThreadNum = 2
         Trace_Size=hwThreadNum*Trace_Size
         Trace_Num=Trace_Size/1024 
         if Trace_Num <= 4:
               tracesizebytes=4*1024
         elif Trace_Num > 4 and Trace_Num <= 16:
               tracesizebytes=16*1024        
         elif Trace_Num > 16 and Trace_Num <= 64:
               tracesizebytes=64*1024
         elif Trace_Num > 64 and Trace_Num <= 256:
               tracesizebytes=256*1024                  
   line_number += 1
 if finPDalgHole[0] > tracesizebytes:
   finPDalgHole[0] = finPDalgHole[0] - tracesizebytes
 for index in range(len(finPDSize)):
    allPDSize=allPDSize+finPDSize[index]
    allAligHole=allAligHole+finPDalgHole[index]
 consImageSize=allPDSize+allAligHole+tracesizebytes;   
 consImageSizeMB=float(consImageSize/(1024.0*1024.0))


 if Image_SizeMB > consImageSizeMB:
        print "*****************Q6-%s Memory FootPrint************************" % imageName  
        outputfile.write("*****************Q6-%s Memory FootPrint************************\n" % imageName  )
        if (imageName == 'adsp') and (threePds != 1):
           if allAligHole < 2097152:
              twoMBsize=2097152-allAligHole;
              incDynmodSize=allPDSize+allAligHole+tracesizebytes+twoMBsize
              incDynmodSizeMB=float(incDynmodSize/(1024.0*1024.0))
              if incDynmodSizeMB > Image_SizeMB:
                    print ('**********************************************************************')
                    outputfile.write("**********************************************************************\n")
                    print ('Error: Not enough size to load the dynamic modules 2MB\n')
                    outputfile.write("Error: Not enough size to load the dynamic modules 2MB\n")
                    print ('**********************************************************************') 
                    outputfile.write("**********************************************************************\n")
                    print "Total Image Size Consumed in MB          :%.4f" % incDynmodSizeMB
                    outputfile.write("Total Image Size Consumed in MB          :%.4f\n" % incDynmodSizeMB)
                    shutil.rmtree('../../obj')
              else:                
                rootPDIS = float(finPDSize[0]/(1024.0*1024.0))
                rootPDIS = rootPDIS-CommonHeapSizeDec
                audioPDIS = float(finPDSize[1]/(1024.0*1024.0))
                audioPDIS = audioPDIS - UserHeapSizeAudioDec
                qurtkTBS = float(tracesizebytes/(1024.0*1024.0))
                onembAHS = float(finPDalgHole[0]/(1024.0*1024.0))
                endmbAHS = float(finPDalgHole[1]/(1024.0*1024.0))
                avaIS = float(Image_SizeMB - consImageSizeMB)
                rootPDIIS = float(finPDIslandSize[0]/(1024.0*1024.0))
                audioPDIIS = float(finPDIslandSize[1]/(1024.0*1024.0))
                audioPDIIS = audioPDIIS - IslandHeapSizeDec1
                avsIIS = l2Size - (rootPDIIS+audioPDIIS+IslandHeapSizeDec1)
                l2SizeCons = rootPDIIS + audioPDIIS + IslandHeapSizeDec1
                if rootPDIS >= 0 and audioPDIS >= 0:
                    print "RootPD Image:                %.3fMB" %rootPDIS 
                    outputfile.write("RootPD Image:                %.3fMB\n" %rootPDIS)
                    print "RootPD Heap:                 %.3fMB" %CommonHeapSizeDec
                    outputfile.write("RootPD Heap:                 %.3fMB\n" %CommonHeapSizeDec)
                    print "AudioPD Image:               %.3fMB" %audioPDIS
                    outputfile.write("AudioPD Image:               %.3fMB\n" %audioPDIS)
                    print "AudioPD Heap:                %.3fMB" %UserHeapSizeAudioDec
                    outputfile.write("AudioPD Heap:                %.3fMB\n" %UserHeapSizeAudioDec)
                    print "QURTK Trace Buffer:          %.3fMB" %qurtkTBS
                    outputfile.write("QURTK Trace Buffer:          %.3fMB\n" %qurtkTBS)
                    print "1MB Alignment btw PDs:       %.3fMB" %onembAHS
                    outputfile.write("1MB Alignment btw PDs:       %.3fMB\n" %onembAHS)
                    print "End Address Alignment:       %.3fMB" %endmbAHS
                    outputfile.write("End Address Alignment:       %.3fMB\n" %endmbAHS)
                    print "Total Image Size Consumed:   %.3fMB" %consImageSizeMB
                    outputfile.write("Total Image Size Consumed:   %.3fMB\n" %consImageSizeMB)
                    print "Available Size:              %.3fMB" %avaIS
                    outputfile.write("Available Size:              %.3fMB\n" %avaIS)
                    if rootPDIIS != 0:
                        print "\n"
                        outputfile.write("\n")
                        print "Island Size in RootPD:       %.3fMB" %rootPDIIS
                        outputfile.write("Island Size in RootPD:       %.3fMB\n" %rootPDIIS)
                        print "Island Size in AudioPD:    %.3fMB" %audioPDIIS
                        outputfile.write("Island Size in AudioPD:      %.3fMB\n" %audioPDIIS)
                        print "Island heap:                 %.3fMB" %IslandHeapSizeDec1
                        outputfile.write("Island heap:                 %.3fMB\n" %IslandHeapSizeDec1)
                        print "Total island Size Consumed:  %.3fMB" %l2SizeCons
                        outputfile.write("Total island Size Consumed:  %.3fMB\n" %l2SizeCons)
                        print "Available Size in Island:    %.3fMB" %avsIIS 
                        outputfile.write("Available Size in Island:    %.3fMB\n" %avsIIS )
        elif imageName == 'adsp' and threePds == 1:
           if allAligHole < 2097152:
              twoMBsize=2097152-allAligHole;            
              incDynmodSize=allPDSize+allAligHole+tracesizebytes+twoMBsize
              incDynmodSizeMB=float(incDynmodSize/(1024.0*1024.0))             
              if incDynmodSizeMB > Image_SizeMB and chipset != 'sdm670':
                    print ('**********************************************************************')
                    outputfile.write("**********************************************************************\n")
                    print ('Error: Not enough size to load the dynamic modules 2MB\n')
                    outputfile.write("Error: Not enough size to load the dynamic modules 2MB\n")
                    print ('**********************************************************************')
                    outputfile.write("**********************************************************************\n")
                    print "Total Image Size Consumed in MB          :%.4f" % incDynmodSizeMB
                    outputfile.write("Total Image Size Consumed in MB          :%.4f\n" % incDynmodSizeMB)
                    shutil.rmtree('../../obj')
              else:        
                rootPDIS = float(finPDSize[0]/(1024.0*1024.0))
                rootPDIS = rootPDIS-CommonHeapSizeDec
                SensorPDIS = float(finPDSize[1]/(1024.0*1024.0))
                sensorPDIS = SensorPDIS - UserHeapSizeDec                                
                audioPDIS = float(finPDSize[2]/(1024.0*1024.0))
                audioPDIS = audioPDIS - UserHeapSizeAudioDec                
                qurtkTBS = float(tracesizebytes/(1024.0*1024.0))                
                onembAHS = float((finPDalgHole[0]+finPDalgHole[1])/(1024.0*1024.0))
                endmbAHS = float(finPDalgHole[2]/(1024.0*1024.0))
                avaIS = float(Image_SizeMB - consImageSizeMB)
                rootPDIIS = float(finPDIslandSize[0]/(1024.0*1024.0))
                sensorPDIIS = float(finPDIslandSize[1]/(1024.0*1024.0))
                sensorPDIIS = sensorPDIIS - IslandHeapSizeDec1
                avsIIS = l2Size - (rootPDIIS+sensorPDIIS+IslandHeapSizeDec1)
                l2SizeCons = rootPDIIS + sensorPDIIS + IslandHeapSizeDec1
                if rootPDIS >= 0 and SensorPDIS >= 0 and audioPDIS >= 0:                    
                    print "RootPD Image:                %.3fMB" %rootPDIS
                    outputfile.write("RootPD Image:                %.3fMB\n" %rootPDIS)
                    print "RootPD Heap:                 %.3fMB" %CommonHeapSizeDec
                    outputfile.write("RootPD Heap:                 %.3fMB\n" %CommonHeapSizeDec)
                    print "SensorPD Image:              %.3fMB" %sensorPDIS
                    outputfile.write("SensorPD Image:              %.3fMB\n" %sensorPDIS)
                    print "SensorPD Heap:               %.3fMB" %UserHeapSizeDec  
                    outputfile.write("SensorPD Heap:               %.3fMB\n" %UserHeapSizeDec)
                    print "AudioPD Image:               %.3fMB" %audioPDIS
                    outputfile.write("AudioPD Image:               %.3fMB\n" %audioPDIS)
                    print "AudioPD Heap:                %.3fMB" %UserHeapSizeAudioDec
                    outputfile.write("AudioPD Heap:                %.3fMB\n" %UserHeapSizeAudioDec)
                    print "QURTK Trace Buffer:          %.3fMB" %qurtkTBS
                    outputfile.write("QURTK Trace Buffer:          %.3fMB\n" %qurtkTBS)
                    print "1MB Alignment btw PDs:       %.3fMB" %onembAHS
                    outputfile.write("1MB Alignment btw PDs:       %.3fMB\n" %onembAHS)
                    print "End Address Alignment:       %.3fMB" %endmbAHS
                    outputfile.write("End Address Alignment:       %.3fMB\n" %endmbAHS)
                    print "Total Image Size Consumed:   %.3fMB" %consImageSizeMB
                    outputfile.write("Total Image Size Consumed:   %.3fMB\n" %consImageSizeMB)
                    print "Available Size:              %.3fMB" %avaIS
                    outputfile.write("Available Size:              %.3fMB\n" %avaIS)
                    if rootPDIIS != 0:
                        print "\n"
                        outputfile.write("\n")
                        print "Island Size in RootPD:       %.3fMB" %rootPDIIS
                        outputfile.write("Island Size in RootPD:       %.3fMB\n" %rootPDIIS)
                        print "Island Size in SensorPD:     %.3fMB" %sensorPDIIS
                        outputfile.write("Island Size in SensorPD:     %.3fMB\n" %sensorPDIIS)
                        print "Island heap:                 %.3fMB" %IslandHeapSizeDec1
                        outputfile.write("Island heap:                 %.3fMB\n" %IslandHeapSizeDec1)
                        print "Total island Size Consumed:  %.3fMB" %l2SizeCons
                        outputfile.write("Total island Size Consumed:  %.3fMB\n" %l2SizeCons)
                        print "Available Size in Island:    %.3fMB" %avsIIS
                        outputfile.write("Available Size in Island:    %.3fMB\n" %avsIIS)
        elif imageName == 'cdsp' and srmImage == 1:
                rootPDIS = float(finPDSize[0]/(1024.0*1024.0))
                rootPDIS = rootPDIS-CommonHeapSizeDec
                audioPDIS = float(finPDSize[1]/(1024.0*1024.0))
                audioPDIS = audioPDIS - UserHeapSizeAudioDec
                qurtkTBS = float(tracesizebytes/(1024.0*1024.0))
                onembAHS = float(finPDalgHole[0]/(1024.0*1024.0))
                endmbAHS = float(finPDalgHole[1]/(1024.0*1024.0))
                avaIS = float(Image_SizeMB - consImageSizeMB)
                rootPDIIS = float(finPDIslandSize[0]/(1024.0*1024.0))
                audioPDIIS = float(finPDIslandSize[1]/(1024.0*1024.0))
                audioPDIIS = audioPDIIS - IslandHeapSizeDec1
                avsIIS = l2Size - (rootPDIIS+audioPDIIS+IslandHeapSizeDec1)
                l2SizeCons = rootPDIIS + audioPDIIS + IslandHeapSizeDec1
                if rootPDIS >= 0 and audioPDIS >= 0:                    
                    print "RootPD Image:                %.3fMB" %rootPDIS 
                    outputfile.write("RootPD Image:                %.3fMB\n" %rootPDIS)
                    print "RootPD Heap:                 %.3fMB" %CommonHeapSizeDec
                    outputfile.write("RootPD Heap:                 %.3fMB\n" %CommonHeapSizeDec)
                    print "SRMPD Image:               %.3fMB" %audioPDIS
                    outputfile.write("SRMPD Image:               %.3fMB\n" %audioPDIS)
                    if UserHeapSizeAudioDec != 0:
                        print "SRMPD Heap:                %.3fMB" %UserHeapSizeAudioDec
                        outputfile.write("SRMPD Heap:                %.3fMB\n" %UserHeapSizeAudioDec)
                    print "QURTK Trace Buffer:          %.3fMB" %qurtkTBS
                    outputfile.write("QURTK Trace Buffer:          %.3fMB\n" %qurtkTBS)
                    print "1MB Alignment btw PDs:       %.3fMB" %onembAHS
                    outputfile.write("1MB Alignment btw PDs:       %.3fMB\n" %onembAHS)
                    print "End Address Alignment:       %.3fMB" %endmbAHS
                    outputfile.write("End Address Alignment:       %.3fMB\n" %endmbAHS)
                    print "Total Image Size Consumed:   %.3fMB" %consImageSizeMB
                    outputfile.write("Total Image Size Consumed:   %.3fMB\n" %consImageSizeMB)
                    print "Available Size:              %.3fMB" %avaIS
                    outputfile.write("Available Size:              %.3fMB\n" %avaIS)        
        elif imageName == 'cdsp' and threePds == 1:
            rootPDIS = float(finPDSize[0]/(1024.0*1024.0))
            rootPDIS = rootPDIS-CommonHeapSizeDec
            facePDIS = float(finPDSize[1]/(1024.0*1024.0))
            facePDIS = facePDIS-UserHeapSizeDec
            srmPDIS = float(finPDSize[2]/(1024.0*1024.0))
            srmPDIS = srmPDIS - UserHeapSizeAudioDec
            qurtkTBS = float(tracesizebytes/(1024.0*1024.0))     
            onembAHS = float((finPDalgHole[0]+finPDalgHole[1])/(1024.0*1024.0))
            endmbAHS = float(finPDalgHole[2]/(1024.0*1024.0))
            avaIS = float(Image_SizeMB - consImageSizeMB)
            if rootPDIS >= 0 and facePDIS >= 0 and srmPDIS >= 0:
               print "RootPD Image:                %.3fMB" %rootPDIS
               outputfile.write("RootPD Image:                %.3fMB\n" %rootPDIS)
               print "RootPD Heap:                 %.3fMB" %CommonHeapSizeDec
               outputfile.write("RootPD Heap:                 %.3fMB\n" %CommonHeapSizeDec)
               print "FacePD Image:                %.3fMB" %facePDIS
               outputfile.write("FacePD Image:                %.3fMB\n" %facePDIS)
               if UserHeapSizeDec != 0:
                  print "FacePD Heap:              %.3fMB" %UserHeapSizeDec
                  outputfile.write("FacePD Heap:              %.3fMB\n" %UserHeapSizeDec)
               print "SRMPD Image:                 %.3fMB" %srmPDIS
               outputfile.write("SRMPD Image:                 %.3fMB\n" %srmPDIS)
               if UserHeapSizeAudioDec != 0:
                  print "SRMPD Heap:               %.3fMB" %UserHeapSizeAudioDec
                  outputfile.write("SRMPD Heap:               %.3fMB\n" %UserHeapSizeAudioDec)
               print "QURTK Trace Buffer:          %.3fMB" %qurtkTBS
               outputfile.write("QURTK Trace Buffer:          %.3fMB\n" %qurtkTBS)
               print "1MB Alignment btw PDs:       %.3fMB" %onembAHS
               outputfile.write("1MB Alignment btw PDs:       %.3fMB\n" %onembAHS)
               print "End Address Alignment:       %.3fMB" %endmbAHS
               outputfile.write("End Address Alignment:       %.3fMB\n" %endmbAHS)
               print "Total Image Size Consumed:   %.3fMB" %consImageSizeMB
               outputfile.write("Total Image Size Consumed:   %.3fMB\n" %consImageSizeMB)
               print "Available Size:              %.3fMB" %avaIS
               outputfile.write("Available Size:              %.3fMB\n" %avaIS)
        elif imageName == 'cdsp':
            rootPDIS = float(finPDSize[0]/(1024.0*1024.0))
            rootPDIS = rootPDIS-CommonHeapSizeDec
            qurtkTBS = float(tracesizebytes/(1024.0*1024.0))
            endmbAHS = float(finPDalgHole[0]/(1024.0*1024.0))
            avaIS = float(Image_SizeMB - consImageSizeMB)
            rootPDIIS = float(finPDIslandSize[0]/(1024.0*1024.0))
            if rootPDIS >= 0:                
                print "RootPD Image:                %.3fMB" %rootPDIS 
                outputfile.write("RootPD Image:                %.3fMB\n" %rootPDIS)
                print "RootPD Heap:                 %.3fMB" %CommonHeapSizeDec
                outputfile.write("RootPD Heap:                 %.3fMB\n" %CommonHeapSizeDec)
                print "QURTK Trace Buffer:          %.3fMB" %qurtkTBS
                outputfile.write("QURTK Trace Buffer:          %.3fMB\n" %qurtkTBS)
                print "End Address Alignment:       %.3fMB" %endmbAHS
                outputfile.write("End Address Alignment:       %.3fMB\n" %endmbAHS)
                print "Total Image Size Consumed:   %.3fMB" %consImageSizeMB
                outputfile.write("Total Image Size Consumed:   %.3fMB\n" %consImageSizeMB)
                print "Available Size:              %.3fMB" %avaIS
                outputfile.write("Available Size:              %.3fMB\n" %avaIS)
        elif imageName == 'slpi':
            rootPDIS = float(finPDSize[0]/(1024.0*1024.0))
            rootPDIS = rootPDIS-CommonHeapSizeDec
            sensorsPDIS = float(finPDSize[1]/(1024.0*1024.0))
            sensorsPDIS = sensorsPDIS - UserHeapSizeDec
            qurtkTBS = float(tracesizebytes/(1024.0*1024.0))
            onembAHS = float(finPDalgHole[0]/(1024.0*1024.0))
            endmbAHS = float(finPDalgHole[1]/(1024.0*1024.0))
            avaIS = float(Image_SizeMB - consImageSizeMB)
            rootPDIIS = float(finPDIslandSize[0]/(1024.0*1024.0))
            sensorsPDIIS = float(finPDIslandSize[1]/(1024.0*1024.0))
            sensorsPDIIS = sensorsPDIIS - IslandHeapSizeDec1
            avsIIS = l2Size - (rootPDIIS+sensorsPDIIS+IslandHeapSizeDec1)
            l2SizeCons = rootPDIIS + sensorsPDIIS + IslandHeapSizeDec1
            if rootPDIS >= 0 and sensorsPDIS >= 0:                
                print "RootPD Image:                %.3fMB" %rootPDIS
                outputfile.write("RootPD Image:                %.3fMB\n" %rootPDIS)
                print "RootPD Heap:                 %.3fMB" %CommonHeapSizeDec
                outputfile.write("RootPD Heap:                 %.3fMB\n" %CommonHeapSizeDec)
                print "SensorsPD Image:             %.3fMB" %sensorsPDIS
                outputfile.write("SensorsPD Image:             %.3fMB\n" %sensorsPDIS)
                print "SensorsPD Heap:              %.3fMB" %UserHeapSizeDec
                outputfile.write("SensorsPD Heap:              %.3fMB\n" %UserHeapSizeDec)
                print "QURTK Trace Buffer:          %.3fMB" %qurtkTBS
                outputfile.write("QURTK Trace Buffer:          %.3fMB\n" %qurtkTBS)
                print "1MB Alignment btw PDs:       %.3fMB" %onembAHS
                outputfile.write("1MB Alignment btw PDs:       %.3fMB\n" %onembAHS)
                print "End Address Alignment:       %.3fMB" %endmbAHS
                outputfile.write("End Address Alignment:       %.3fMB\n" %endmbAHS)
                print "Total Image Size Consumed:   %.3fMB" %consImageSizeMB
                outputfile.write("Total Image Size Consumed:   %.3fMB\n" %consImageSizeMB)
                print "Available Size:              %.3fMB" %avaIS
                outputfile.write("Available Size:              %.3fMB\n" %avaIS)
                if rootPDIIS != 0:
                    print "\n"
                    print "Island Size in RootPD:       %.3fMB" %rootPDIIS
                    outputfile.write("Island Size in RootPD:       %.3fMB\n" %rootPDIIS)
                    print "Island Size in SensorsPD:    %.3fMB" %sensorsPDIIS
                    outputfile.write("Island Size in SensorsPD:    %.3fMB\n" %sensorsPDIIS)
                    print "Island heap:                 %.3fMB" %IslandHeapSizeDec1
                    outputfile.write("Island heap:                 %.3fMB\n" %IslandHeapSizeDec1)
                    print "Total island Size Consumed:  %.3fMB" %l2SizeCons
                    outputfile.write("Total island Size Consumed:  %.3fMB\n" %l2SizeCons)
                    print "Available Size in Island:    %.3fMB" %avsIIS
                    outputfile.write("Available Size in Island:    %.3fMB\n" %avsIIS)
        print "*****************Q6-%s Memory FootPrint End********************" % imageName
        outputfile.write("*****************Q6-%s Memory FootPrint End********************\n" % imageName)
 else:
        print ('**********************************************************************')
        outputfile.write("**********************************************************************\n")
        print ('Error: Image Size greater than DEFAULT_PHYSPOOL defined in qdsp6.xml\n')
        outputfile.write("Error: Image Size greater than DEFAULT_PHYSPOOL defined in qdsp6.xml\n")
        print ('**********************************************************************')
        outputfile.write("**********************************************************************\n")
        print "Total Image size declared in MB          :%.4f" % Image_SizeMB
        outputfile.write("Total Image size declared in MB          :%.4f\n" % Image_SizeMB)
        print "Total Image Size Consumed in MB          :%.4f" % consImageSizeMB
        outputfile.write("Total Image Size Consumed in MB          :%.4f\n" % consImageSizeMB)
        shutil.rmtree('../../obj')
outputfile.close()
    

    
