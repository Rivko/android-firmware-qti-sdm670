import os
import re
import sys
import shutil
import math
import csv
interFlag = 0
LibAccum=0
LibBaseAddrDec = 0
LibName = 0
LibNamePrev = 0
TextCheckFlag = 0
RodataCheckFlag = 0
DataCheckFlag = 0
SdataCheckFlag = 0
SbssCheckFlag = 0
BssCheckFlag = 0
LibBaseAddrDecPrev = 0
LibLevelCompPrev = 0
DevList = []
DevIndex = 0
hash =  dict([])
libhash = {}
libhash1 = {}
comphash = {}
lines = {}
TextIndex = 0
RodataIndex = 1
DataIndex = 2
SdataIndex = 3
SbssIndex = 4
BssIndex = 5
CompIndex = 0
Textflag = 0
Rodataflag = 0
Dataflag = 0
Sdataflag = 0
Sbssflag = 0
Bssflag = 0
crmexe = 0
AlignHoleuKer =0
alignHoleBtwSections = 0
arg_len=len(sys.argv)
if arg_len > 6:
    msmchipset=sys.argv[1]
    mapfile=sys.argv[2]
    PDType=sys.argv[3]
    toolsUsed=int(sys.argv[4])
    Location=sys.argv[5]
    BuildStatus=sys.argv[6]
    MemLocation=sys.argv[7]
    crmexe=1
elif arg_len == 6:
    msmchipset=sys.argv[1]
    mapfile=sys.argv[2]
    PDType=sys.argv[3]
    toolsUsed=int(sys.argv[4])
    imageName=sys.argv[5]
    islandFlag = 1
    ii=0
    sdcFlag = 0
    loopcount=2    
    if imageName == 'adsp' or imageName == 'slpi':
       loopcount=2
    if PDType == 'AudioPD' or imageName == 'cdsp' or PDType == 'sdc':
       loopcount=1
       islandFlag = 0
    if PDType == 'sdc':
       sdcFlag = 1
    if msmchipset == 'sdm845' and imageName == 'adsp':
       loopcount=1
       islandFlag = 0   
    if msmchipset == 'sdm855' and imageName == 'adsp':
       loopcount=1
       islandFlag = 0   
else:    
    msmchipset=sys.argv[1]
    mapfile=sys.argv[2]
    PDType=sys.argv[3]
    toolsUsed=int(sys.argv[4])
    imageName='adsp'
    loopcount=1
    islandFlag = 0
    ii = 0
    sdcFlag = 0    
while ii < loopcount:
 interFlag = 0
 LibAccum=0
 LibBaseAddrDec = 0
 LibName = 0
 TextCheckFlag = 0
 RodataCheckFlag = 0
 DataCheckFlag = 0
 SdataCheckFlag = 0
 SbssCheckFlag = 0
 BssCheckFlag = 0
 DevList = []
 DevIndex = 0
 hash =  dict([])
 libhash = {}
 libhash1 = {}
 comphash = {}
 lines = {}
 TextIndex = 0
 RodataIndex = 1
 DataIndex = 2
 SdataIndex = 3
 SbssIndex = 4
 BssIndex = 5
 CompIndex = 0
 Textflag = 0
 Rodataflag = 0
 Dataflag = 0
 Sdataflag = 0
 Sbssflag = 0
 Bssflag = 0
 crmexe = 0
 infile = open("%s" % mapfile, 'r')
 if crmexe:
    csvout = csv.writer(open("%s\\Memory_info_%s_%s.csv" % (MemLocation,msmchipset,BuildStatus), "wb"))
 else:
    if islandFlag == 1:
       csvout = csv.writer(open("Memory_info_%s_%s_island.csv" % (msmchipset,PDType), "wb"))
    else:
       csvout = csv.writer(open("Memory_info_%s_%s.csv" % (msmchipset,PDType), "wb"))    
 lines = infile.readlines()         
 line_number = 0
 while line_number < len(lines):
     to_line = lines[line_number]
     line_number += 1
     start=re.search('^.start\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if start:
          StartBase=start.group(1)
          StartSize=start.group(2)
     init=re.search('^.init\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if init:
          InitBase=init.group(1)
          InitSize=init.group(2)
     plt=re.search('^.plt\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if plt:
          PltBase=plt.group(1)
          PltSize=plt.group(2)
     if islandFlag == 1:
        text=re.search('^.text.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
        if not text:
           text = re.search('^.text\..+\.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)   
     elif sdcFlag == 1:
        text=re.search('^CODE_RAM\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     else:
        text=re.search('^.text\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if text:
          TextBase=text.group(1)
          TextSize=text.group(2)
          TextBaseDec=int(TextBase, 16)
          TextSizeDec=int(TextSize, 16)
          TextOverallSize=TextBaseDec+TextSizeDec
          line_number=line_number
          while TextBaseDec < TextOverallSize:
                  line_number=line_number+1
                  textLine=lines[line_number]
                  if islandFlag == 1:
                     nextsec=re.search('^\.(\w+.\w+)\s+(\w+)\s+(\w+)\s+\#.+', textLine)
                     if not nextsec:
                        nextsec=re.search('^\.(\w+\..+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', textLine)
                  else:      
                     nextsec=re.search('^\.(\w+)\s+(\w+)\s+(\w+)', textLine)
                     if not nextsec:
                           nextsec=re.search('^\.(\w+.\w+)\s+(\w+)\s+(\w+)', textLine)
                           if not nextsec:
                                 nextsec=re.search('^(\w+.\w+)\s+(\w+)\s+(\w+)', textLine) 
                  FirstPattern=re.search('^\.\w+\.\w+\n',textLine)
                  SecondPattern=re.search('^\.\w+\.\w+\.\w+\n',textLine)                    
                  if FirstPattern or SecondPattern:
                        line_number=line_number+1
                        ScanLine=lines[line_number]
                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                        if not CommonPattern:
                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',ScanLine)
                             if not CommonPattern:
                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)                                       
                                 if not CommonPattern:
                                    CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)      
                                    if not CommonPattern:                            
                                       CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                    
                                       if not CommonPattern:                            
                                           CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                                           
                                           if not CommonPattern:                            
                                               CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                          
                                               if not CommonPattern:                            
                                                  CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                                                  if not CommonPattern:
                                                     CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',ScanLine)         
                                                     if not CommonPattern:                            
                                                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                                     
                                                        if not CommonPattern:                            
                                                           CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+' % imageName,ScanLine)                                                    
                                       
                        if CommonPattern:
                                   LibBaseAddr=CommonPattern.group(1)
                                   LibSize=CommonPattern.group(2)
                                   LibLevelComp=CommonPattern.group(3)
                                   LibLevel2Comp=CommonPattern.group(4)
                                   LibName=CommonPattern.group(5)
                                   LibBaseAddrDec=int(LibBaseAddr, 16)
                                   if TextCheckFlag == 0:
                                         TextCheckFlag = 1
                                         TextAlignHole=LibBaseAddrDec-TextBaseDec
                                         if TextAlignHole != 0:
                                               TextAlignName="Text_Align"
                                               libhash[TextAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash1[TextAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash[TextAlignName][TextIndex] = TextAlignHole
                                               libhash1[TextAlignName][CompIndex] = TextAlignName
                                   LibSizeDec=int(LibSize, 16)
                                   if Textflag == 1:        
                                         if not LibNamePrev in libhash:
                                             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                          
                                             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                             if LibBaseAddrDec != 0:                                            
                                                 libhash[LibNamePrev][TextIndex] = libhash[LibNamePrev][TextIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                                 Comp1 = 'Qualcomm'
                                                 Comp2 = 'avs'
                                                 Comp3 = 'core'
                                                 Comp4 = 'Sensors'
                                                 if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                         LibLevelCompPrev = LibLevel2CompPrev
                                                 libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev    
                                         else:
                                             if LibBaseAddrDec != 0:                                        
                                                 libhash[LibNamePrev][TextIndex] = libhash[LibNamePrev][TextIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)                                                                                   
                                   Textflag=1; 
                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',textLine)
                  if not CommonPattern1:
                     CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',textLine)
                     if not CommonPattern1:
                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',textLine)
                         if not CommonPattern1:
                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',textLine)
                            if not CommonPattern1:
                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,textLine)                            
                               if not CommonPattern1:
                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,textLine)                               
                                  if not CommonPattern1:
                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,textLine)
                                     if not CommonPattern1:
                                        CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,textLine)
                                        if not CommonPattern1:
                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,textLine)                                         
                                           if not CommonPattern1:
                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,textLine)                                        
                                              if not CommonPattern1:
                                                 CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',textLine)                                         
                                                 if not CommonPattern1:
                                                    CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',textLine) 
                                                    if not CommonPattern1:
                                                       CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,textLine)                             
                                                       if not CommonPattern1:
                                                          CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,textLine)                                
                                                          if not CommonPattern1:
                                                             CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,textLine)
                                                             if not CommonPattern1:
                                                                CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,textLine)                                             
                                                                if not CommonPattern1:
                                                                   CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',textLine)                            
                                                                   if not CommonPattern1:
                                                                      CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',textLine)                               
                                                                      if not CommonPattern1:
                                                                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',textLine)
                                                                         if not CommonPattern1:
                                                                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',textLine)                                                          
                                                                            if not CommonPattern1:
                                                                               CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+' % imageName,textLine)                                                                                                                               
                                                                               if not CommonPattern1:
                                                                                  CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+' % imageName,textLine)
                                                                                  if not CommonPattern1:
                                                                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,textLine)                                                                                   
                                                                                     if not CommonPattern1:
                                                                                        CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,textLine)
                                                                                        if not CommonPattern1:
                                                                                           CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,textLine)                                                                                  
                                                                                           if not CommonPattern1:
                                                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',textLine)
                                                                                              if not CommonPattern1:
                                                                                                 CommonPattern1=re.search('\.\w+\..+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',textLine)
                                                                                                 if not CommonPattern1:
                                                                                                    CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\s+\#.+' % imageName,textLine)     
                                                                                                    if not CommonPattern1:
                                                                                                       CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\s+\#.+' % imageName,textLine)     
                                                                                                       if not CommonPattern1:
                                                                                                          CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,textLine)     
                                                                                                          if not CommonPattern1:
                                                                                                             CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,textLine)     
                                                                                                             if not CommonPattern1:
                                                                                                                CommonPattern1=re.search('\.\w+\.\w+.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,textLine)     
                                                                                                                if not CommonPattern1:
                                                                                                                   CommonPattern1=re.search('\.\w+\.\.\w+.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\.+\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,textLine)     
                                                                                                                   if not CommonPattern1:
                                                                                                                      CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\Apps\\\\(LLVM)\\\\(.+)\/(.+)\.\w+\(\w+\.\w+\)\s+\#.+',textLine)     
                                                                                                                      if not CommonPattern1:
                                                                                                                         CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\Apps\\\\(LLVM)\\\\(.+)\/(.+)\.\w+\(\w+\.\w+\)\s+\#.+',textLine)     
                                                                                                                         if not CommonPattern1:
                                                                                                                            CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\Apps\\\\(LLVM)\\\\(.+)\/(.+)\.\w+\(\w+\.\w+\)\s+\#.+',textLine)     
                                                                                                                            if not CommonPattern1:
                                                                                                                               CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\s+\#.+' % imageName,textLine)     
                                                                                                                               if not CommonPattern1:
                                                                                                                                  CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,textLine)                                                                                                      
                                              
                  if CommonPattern1:
                        LibBaseAddr=CommonPattern1.group(1)
                        LibSize=CommonPattern1.group(2)
                        LibLevelComp=CommonPattern1.group(3)
                        LibLevel2Comp=CommonPattern1.group(4)
                        LibName=CommonPattern1.group(5)
                        LibBaseAddrDec=int(LibBaseAddr, 16)
                        if TextCheckFlag == 0:
                             TextCheckFlag = 1
                             TextAlignHole=LibBaseAddrDec-TextBaseDec
                             if TextAlignHole != 0:
                                 TextAlignName="Text_Align"
                                 libhash[TextAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash1[TextAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash[TextAlignName][TextIndex] = TextAlignHole
                                 libhash1[TextAlignName][CompIndex] = TextAlignName
                        LibSizeDec=int(LibSize, 16)                    
                        if Textflag == 1 and LibNamePrev != 0:                                 
                             if not LibNamePrev in libhash:
                                 libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                 libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                             
                                 if LibBaseAddrDec != 0:                                
                                      libhash[LibNamePrev][TextIndex] = libhash[LibNamePrev][TextIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                      Comp1 = 'Qualcomm'
                                      Comp2 = 'avs'
                                      Comp3 = 'core'
                                      Comp4 = 'Sensors'
                                      if sdcFlag == 1 and LibLevelCompPrev != 0:
                                         if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                              LibLevelCompPrev = LibLevel2CompPrev
                                         libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev
                                      else:
                                         if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                              LibLevelCompPrev = LibLevel2CompPrev
                                         libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                                                            
                             else:
                                 if LibBaseAddrDec != 0:                            
                                      libhash[LibNamePrev][TextIndex] = libhash[LibNamePrev][TextIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                        Textflag = 1
                        LibSizeDec=int(LibSize, 16)
                  if sdcFlag == 1 and LibBaseAddrDec != 0:                     
                      LibBaseAddrDecPrev = LibBaseAddrDec 
                      LibNamePrev=LibName
                      LibLevelCompPrev=LibLevelComp
                      LibLevel2CompPrev=LibLevel2Comp
                  elif LibBaseAddrDec != 0:
                      LibBaseAddrDecPrev = LibBaseAddrDec 
                      LibSizeDecPrev = LibSizeDec
                      LibNamePrev=LibName
                      LibLevelCompPrev=LibLevelComp
                      LibLevel2CompPrev=LibLevel2Comp                  
                  if nextsec:
                        if islandFlag == 1:
                           Dataflag=0
                           Bssflag=0
                           Rodataflag=0
                           Textflag=0
                        nextsecBase=nextsec.group(2)
                        nextsecBaseDec=int(nextsecBase, 16)
                        TextBaseDec=nextsecBaseDec
          if not LibNamePrev in libhash:
             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                                            
             libhash[LibNamePrev][TextIndex] = libhash[LibNamePrev][TextIndex] + (TextOverallSize - LibBaseAddrDecPrev)             
             Comp1 = 'Qualcomm'
             Comp2 = 'avs'
             Comp3 = 'core'
             Comp4 = 'Sensors'
             if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                LibLevelCompPrev = LibLevel2CompPrev
             libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev
          else:                         
             libhash[LibNamePrev][TextIndex] = libhash[LibNamePrev][TextIndex] + (TextOverallSize - LibBaseAddrDecPrev)
          if islandFlag == 1:
             alignHoleBtwSectionsOnly = (TextBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
             if alignHoleBtwSectionsOnly >= 0:
              if alignHoleBtwSectionsOnly > 4024 :
                 AlignHole = float(LibBaseAddrDecPrev+LibSizeDecPrev)/(4.0*1024.0)
                 AlignHole=math.ceil(AlignHole)
                 AlignHole=AlignHole*4*1024
                 AlignHole=AlignHole-(LibBaseAddrDecPrev+LibSizeDecPrev)
                 alignHoleBtwSections = alignHoleBtwSections + (AlignHole)
              else:
                alignHoleBtwSections = alignHoleBtwSections + (TextBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
             LibBaseAddrDecPrev = TextOverallSize
     if islandFlag == 1:
        rodata=re.search('^.rodata.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
        if not rodata:
           rodata = re.search('^.rodata\..+\.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     elif sdcFlag == 1:
        rodata=re.search('^DATA_RAM\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     else:      
        rodata=re.search('^.rodata\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if rodata:
          RodataBase=rodata.group(1)
          RodataSize=rodata.group(2)
          RodataBaseDec=int(RodataBase, 16)
          RodataSizeDec=int(RodataSize, 16)
          RodataOverallSize=RodataBaseDec+RodataSizeDec
          line_number=line_number
          while RodataBaseDec < RodataOverallSize:
                  line_number=line_number+1
                  rodataLine=lines[line_number]
                  if islandFlag == 1:
                     nextsec=re.search('^\.(\w+\.island)\s+(\w+)\s+(\w+)\s+\#.+', rodataLine)
                     if nextsec:
                        Commonnextsec=nextsec
                     nextsec3=re.search('^\.(\w+\..+\.island)\s+(\w+)\s+(\w+)\s+\#.+', rodataLine)
                     if nextsec3:
                        Commonnextsec=nextsec3
                     nextsec1=re.search('^\.(\w+\..+\.island)\s+(\w+)\s+(\w+)\s+\#.+', rodataLine)
                     if nextsec1:
                        Commonnextsec=nextsec1                        
                  else:      
                     nextsec=re.search('^\.(\w+)\s+(\w+)\s+(\w+)', rodataLine)
                     if nextsec:
                             Commonnextsec=nextsec
                     nextsec3=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)', rodataLine)
                     if nextsec3:
                             Commonnextsec=nextsec3                         
                     nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', rodataLine)     
                     if nextsec1:
                            line_number1=line_number+1
                            rodataLine1=lines[line_number1]
                            nextsec2=re.search('(\s+)(\w+)\s+(\w+)', rodataLine1)
                            Commonnextsec=nextsec2               
                  FirstPattern=re.search('^\.\w+\.\w+\n',rodataLine)
                  SecondPattern=re.search('^\.\w+\.\w+\.\w+\n',rodataLine)  
                  ThirdPattern=re.match('\s+QSR_STR\.\w+\.\w+\.\n',rodataLine)      
                  FourthPattern=re.match('\s+QSR_STR\.\w+\.\w+\..+\n',rodataLine)                
                  if FirstPattern or SecondPattern or ThirdPattern or FourthPattern:
                        line_number=line_number+1
                        ScanLine=lines[line_number]
                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                        if not CommonPattern:
                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',ScanLine)
                             if not CommonPattern:                                     
                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)                                
                                 if not CommonPattern:                            
                                    CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)    
                                    if not CommonPattern:
                                       CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)
                                       if not CommonPattern:                            
                                           CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                   
                                           if not CommonPattern:                            
                                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                                                                   
                                             if not CommonPattern:
                                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine) 
                                                 if not CommonPattern:
                                                     CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',ScanLine)         
                                                     if not CommonPattern:                            
                                                         CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                                                             
                        if CommonPattern:
                                   LibBaseAddr=CommonPattern.group(1)
                                   LibSize=CommonPattern.group(2)
                                   LibLevelComp=CommonPattern.group(3)
                                   LibLevel2Comp=CommonPattern.group(4)
                                   LibName=CommonPattern.group(5)
                                   LibBaseAddrDec=int(LibBaseAddr, 16)
                                   if RodataCheckFlag == 0:
                                         RodataCheckFlag = 1
                                         RodataAlignHole=LibBaseAddrDec-RodataBaseDec
                                         if RodataAlignHole != 0:
                                               RodataAlignName="Rodata_Align"
                                               libhash[RodataAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash1[RodataAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash[RodataAlignName][RodataIndex] = RodataAlignHole
                                               libhash1[RodataAlignName][CompIndex] = RodataAlignName
                                   LibSizeDec=int(LibSize, 16)
                                   if Rodataflag == 1:      
                                         if not LibNamePrev in libhash:                             
                                             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                          
                                             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                             if LibBaseAddrDec != 0: 
                                                     libhash[LibNamePrev][RodataIndex] = libhash[LibNamePrev][RodataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                                     Comp1 = 'Qualcomm'
                                                     Comp2 = 'avs'
                                                     Comp3 = 'core'
                                                     Comp4 = 'Sensors'                                                  
                                                     if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                             LibLevelCompPrev = LibLevel2CompPrev
                                                     libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                    
                                         else:
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][RodataIndex] = libhash[LibNamePrev][RodataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                   Rodataflag=1; 
                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',rodataLine)
                  if not CommonPattern1:
                     CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',rodataLine)
                     if not CommonPattern1:
                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',rodataLine)
                         if not CommonPattern1:
                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',rodataLine)
                            if not CommonPattern1:
                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,rodataLine)                            
                               if not CommonPattern1:
                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,rodataLine)                                 
                                  if not CommonPattern1:
                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,rodataLine)
                                     if not CommonPattern1:
                                        CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,rodataLine)
                                        if not CommonPattern1:
                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,rodataLine)                                       
                                           if not CommonPattern1:
                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,rodataLine)                                       
                                              if not CommonPattern1:
                                                  CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',rodataLine)                                          
                                                  if not CommonPattern1:
                                                      CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',rodataLine)
                                                      if not CommonPattern1:
                                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,rodataLine)                            
                                                          if not CommonPattern1:
                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,rodataLine)                              
                                                              if not CommonPattern1:
                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,rodataLine)
                                                                  if not CommonPattern1:
                                                                      CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,rodataLine)                                                                                          
                                                                      if not CommonPattern1:
                                                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',rodataLine)                           
                                                                          if not CommonPattern1:
                                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',rodataLine)                                 
                                                                              if not CommonPattern1:
                                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',rodataLine)
                                                                                  if not CommonPattern1:
                                                                                      CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',rodataLine)   
                                                                                      if not CommonPattern1:
                                                                                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,rodataLine)                                                                                   
                                                                                         if not CommonPattern1:
                                                                                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,rodataLine)
                                                                                            if not CommonPattern1:
                                                                                               CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,rodataLine)                                                                                      
                                                                                               if not CommonPattern1:
                                                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',rodataLine)
                                                                                                  if not CommonPattern1:
                                                                                                     CommonPattern1=re.search('\.\w+\..+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',rodataLine)                                                                                               
                                                                                                     if not CommonPattern1:
                                                                                                        CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\s+\#.+' % imageName,rodataLine)     
                                                                                                        if not CommonPattern1:
                                                                                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\s+\#.+' % imageName,rodataLine)     
                                                                                                           if not CommonPattern1:
                                                                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,rodataLine)     
                                                                                                              if not CommonPattern1:
                                                                                                                 CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,rodataLine)     
                                                                                                                 if not CommonPattern1:
                                                                                                                    CommonPattern1=re.search('\.\w+\.\w+.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,rodataLine)     
                                                                                                                    if not CommonPattern1:
                                                                                                                       CommonPattern1=re.search('\.\w+\.\.\w+.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\.+\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,rodataLine)     
                                                                                                                       if not CommonPattern1:
                                                                                                                          CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\Apps\\\\(LLVM)\\\\(.+)\/(.+)\.\w+\(\w+\.\w+\)\s+\#.+',rodataLine)     
                                                                                                                          if not CommonPattern1:
                                                                                                                             CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\Apps\\\\(LLVM)\\\\(.+)\/(.+)\.\w+\(\w+\.\w+\)\s+\#.+',rodataLine)     
                                                                                                                             if not CommonPattern1:
                                                                                                                                CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\Apps\\\\(LLVM)\\\\(.+)\/(.+)\.\w+\(\w+\.\w+\)\s+\#.+',rodataLine)     
                                                                                                                                if not CommonPattern1:
                                                                                                                                   CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\s+\#.+' % imageName,rodataLine)     
                                                                                                                                   if not CommonPattern1:
                                                                                                                                      CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\\\\.+\\\\%s\_proc\\\\\w+\\\\(\w+)\\\\(.+)\\\\(.+)\.\w+\(\w+\.\w+\)\s+\#.+' % imageName,rodataLine)                                                                                                          
                  if CommonPattern1: 
                        LibBaseAddr=CommonPattern1.group(1)
                        LibSize=CommonPattern1.group(2)
                        LibLevelComp=CommonPattern1.group(3)
                        LibLevel2Comp=CommonPattern1.group(4)
                        LibName=CommonPattern1.group(5)
                        LibBaseAddrDec=int(LibBaseAddr, 16)
                        if RodataCheckFlag == 0:
                              RodataCheckFlag = 1
                              RodataAlignHole=LibBaseAddrDec-RodataBaseDec
                              if RodataAlignHole != 0:
                                     RodataAlignName="Rodata_Align"
                                     libhash[RodataAlignName] = [0, 0, 0, 0, 0, 0]
                                     libhash1[RodataAlignName] = [0, 0, 0, 0, 0, 0]
                                     libhash[RodataAlignName][RodataIndex] = RodataAlignHole
                                     libhash1[RodataAlignName][CompIndex] = RodataAlignName
                        LibSizeDec=int(LibSize, 16)                    
                        if Rodataflag == 1:                               
                             if not LibNamePrev in libhash:                         
                                 libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                 libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                             
                                 if LibBaseAddrDec != 0:                                
                                         libhash[LibNamePrev][RodataIndex] = libhash[LibNamePrev][RodataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                         Comp1 = 'Qualcomm'
                                         Comp2 = 'avs'
                                         Comp3 = 'core'
                                         Comp4 = 'Sensors'                                      
                                         if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                 LibLevelCompPrev = LibLevel2CompPrev
                                         libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev
                             else:
                                 if LibBaseAddrDec != 0:                                        
                                         libhash[LibNamePrev][RodataIndex] = libhash[LibNamePrev][RodataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)                                        
                        Rodataflag = 1
                        LibSizeDec=int(LibSize, 16)
                  if LibBaseAddrDec != 0:
                     LibBaseAddrDecPrev = LibBaseAddrDec 
                     LibNamePrev=LibName
                     LibSizeDecPrev = LibSizeDec
                     LibLevelCompPrev=LibLevelComp
                     LibLevel2CompPrev=LibLevel2Comp                    
                  if nextsec or nextsec1 or nextsec3:
                        #print islandFlag
                        if islandFlag == 1:
                           Dataflag=0
                           Textflag=0
                           Bssflag=0
                           Rodataflag=0
                        nextsecBase=Commonnextsec.group(2)
                        nextsecBaseDec=int(nextsecBase, 16)
                        RodataBaseDec=nextsecBaseDec
          if not LibNamePrev in libhash:                        
             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                                            
             libhash[LibNamePrev][RodataIndex] = libhash[LibNamePrev][RodataIndex] + (RodataOverallSize - LibBaseAddrDecPrev)
             Comp1 = 'Qualcomm'
             Comp2 = 'avs'
             Comp3 = 'core'
             Comp4 = 'Sensors'                                      
             if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                LibLevelCompPrev = LibLevel2CompPrev
             libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev              
          else:                         
             libhash[LibNamePrev][RodataIndex] = libhash[LibNamePrev][RodataIndex] + (RodataOverallSize - LibBaseAddrDecPrev)                 
          if islandFlag == 1:
              alignHoleBtwSectionsOnly = (RodataBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
              if alignHoleBtwSectionsOnly >= 0:
               if alignHoleBtwSectionsOnly > 4024 :
                  AlignHole = float(LibBaseAddrDecPrev+LibSizeDecPrev)/(4.0*1024.0)
                  AlignHole=math.ceil(AlignHole)
                  AlignHole=AlignHole*4*1024
                  AlignHole=AlignHole-(LibBaseAddrDecPrev+LibSizeDecPrev)
                  alignHoleBtwSections = alignHoleBtwSections + (AlignHole)
               else:
                 alignHoleBtwSections = alignHoleBtwSections + (RodataBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
              LibBaseAddrDecPrev = RodataBaseDec
             
     if islandFlag == 1:
        data=re.search('^.data.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
        if not data:
           data = re.search('^.data\..+\.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     else:      
        data=re.search('^.data\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if data:
          DataBase=data.group(1)
          DataSize=data.group(2)
          DataBaseDec=int(DataBase, 16)
          DataSizeDec=int(DataSize, 16)
          DataOverallSize=DataBaseDec+DataSizeDec
          line_number=line_number
          while DataBaseDec < DataOverallSize:
                  line_number=line_number+1
                  dataLine=lines[line_number]
                  #print dataLine
                  if islandFlag == 1:
                     nextsec=re.search('^\.(\w+\.island)\s+(\w+)\s+(\w+)\s+\#.+', dataLine)                 
                     if nextsec:
                        Commonnextsec=nextsec
                     nextsec3=re.search('^\.(\w+\..+\.island)\s+(\w+)\s+(\w+)\s+\#.+', dataLine)       
                     if nextsec3:
                        Commonnextsec=nextsec3
                     nextsec1=re.search('^.(\w+)\s+(0x\w+)\s+(0x\w+)\s+\#.+', dataLine)
                     if nextsec1:
                        Commonnextsec=nextsec1
                  else:      
                     nextsec=re.search('^\.(\w+)\s+(\w+)\s+(\w+)\s+\#.+', dataLine)
                     if nextsec:
                             Commonnextsec=nextsec
                     nextsec3=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)\s+\#.+', dataLine)
                     if nextsec3:
                             Commonnextsec=nextsec3                             
                     nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', dataLine)       
                     if nextsec1:
                            line_number1=line_number+1
                            dataLine1=lines[line_number1]
                            nextsec2=re.search('(\s+)(\w+)\s+(\w+)', dataLine1)
                            Commonnextsec=nextsec2               
                  FirstPattern=re.search('^\.\w+\.\w+\n',dataLine)
                  SecondPattern=re.search('^\.\w+\.\w+\.\w+\n',dataLine)                    
                  if FirstPattern or SecondPattern:
                        line_number=line_number+1
                        ScanLine=lines[line_number]
                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                        if not CommonPattern:
                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',ScanLine)
                             if not CommonPattern:                                     
                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)                                
                                 if not CommonPattern:                            
                                    CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)    
                                    if not CommonPattern:
                                       CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)
                                       if not CommonPattern:                            
                                           CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                       
                                           if not CommonPattern:                            
                                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                                                                   
                                             if not CommonPattern:
                                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                                       
                                                 if not CommonPattern:
                                                     CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',ScanLine)         
                                                     if not CommonPattern:                            
                                                         CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                             
                        if CommonPattern:
                                   LibBaseAddr=CommonPattern.group(1)
                                   LibSize=CommonPattern.group(2)
                                   LibLevelComp=CommonPattern.group(3)
                                   LibLevel2Comp=CommonPattern.group(4)
                                   LibName=CommonPattern.group(5)
                                   LibBaseAddrDec=int(LibBaseAddr, 16)
                                   if DataCheckFlag == 0:
                                         DataCheckFlag = 1
                                         DataAlignHole=LibBaseAddrDec-DataBaseDec
                                         if DataAlignHole != 0:
                                               DataAlignName="Data_Align"
                                               libhash[DataAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash1[DataAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash[DataAlignName][DataIndex] = DataAlignHole
                                               libhash1[DataAlignName][CompIndex] = DataAlignName
                                   LibSizeDec=int(LibSize, 16)
                                   if Dataflag == 1:        
                                         if not LibNamePrev in libhash:
                                             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                          
                                             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][DataIndex] = libhash[LibNamePrev][DataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                                     Comp1 = 'Qualcomm'
                                                     Comp2 = 'avs'
                                                     Comp3 = 'core'
                                                     Comp4 = 'Sensors'                                                  
                                                     if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                             LibLevelCompPrev = LibLevel2CompPrev
                                                     libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev
                                         else:
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][DataIndex] = libhash[LibNamePrev][DataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)                                            
                                   Dataflag=1; 
                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',dataLine)
                  if not CommonPattern1:
                     CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',dataLine)
                     if not CommonPattern1:
                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',dataLine)
                         if not CommonPattern1:
                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',dataLine)
                            if not CommonPattern1:
                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,dataLine)                              
                               if not CommonPattern1:
                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,dataLine)                               
                                  if not CommonPattern1:
                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,dataLine)
                                     if not CommonPattern1:
                                        CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,dataLine)
                                        if not CommonPattern1:
                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,dataLine)                                         
                                           if not CommonPattern1:
                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,dataLine)                                                                              
                                              if not CommonPattern1:
                                                  CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',dataLine)                                        
                                                  if not CommonPattern1:
                                                      CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',dataLine)                                            
                                                      if not CommonPattern1:
                                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,dataLine)                          
                                                          if not CommonPattern1:
                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,dataLine)                                
                                                              if not CommonPattern1:
                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,dataLine)
                                                                  if not CommonPattern1:
                                                                      CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,dataLine)                                                                                          
                                                                      if not CommonPattern1:
                                                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',dataLine)                             
                                                                          if not CommonPattern1:
                                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',dataLine)                               
                                                                              if not CommonPattern1:
                                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',dataLine)
                                                                                  if not CommonPattern1:
                                                                                     CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',dataLine)                                                         
                                                                                     if not CommonPattern1:
                                                                                        CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,dataLine)                                                                                   
                                                                                        if not CommonPattern1:
                                                                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,dataLine)
                                                                                           if not CommonPattern1:
                                                                                              CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,dataLine)                                                                                      
                                                                                              if not CommonPattern1:
                                                                                                 CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',dataLine)
                                                                                                 if not CommonPattern1:
                                                                                                    CommonPattern1=re.search('\.\w+\..+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',dataLine)                                                                                              
                  if CommonPattern1: 
                        LibBaseAddr=CommonPattern1.group(1)
                        LibSize=CommonPattern1.group(2)
                        LibLevelComp=CommonPattern1.group(3)
                        LibLevel2Comp=CommonPattern1.group(4)
                        LibName=CommonPattern1.group(5)
                        LibBaseAddrDec=int(LibBaseAddr, 16)
                        if DataCheckFlag == 0:
                             DataCheckFlag = 1
                             DataAlignHole=LibBaseAddrDec-DataBaseDec
                             if DataAlignHole != 0:
                                 DataAlignName="Data_Align"
                                 libhash[DataAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash1[DataAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash[DataAlignName][DataIndex] = DataAlignHole
                                 libhash1[DataAlignName][CompIndex] = DataAlignName 
                        LibSizeDec=int(LibSize, 16)                             
                        if Dataflag == 1:                                 
                             if not LibNamePrev in libhash:
                                 libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                 libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                             
                                 if LibBaseAddrDec != 0:                                
                                         libhash[LibNamePrev][DataIndex] = libhash[LibNamePrev][DataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                         Comp1 = 'Qualcomm'
                                         Comp2 = 'avs'
                                         Comp3 = 'core'
                                         Comp4 = 'Sensors'                                      
                                         if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                 LibLevelCompPrev = LibLevel2CompPrev
                                         libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                        
                             else:
                                 if LibBaseAddrDec != 0:
                                         libhash[LibNamePrev][DataIndex] = libhash[LibNamePrev][DataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)                                         
                        Dataflag = 1
                        LibSizeDec=int(LibSize, 16)
                  if LibBaseAddrDec != 0:
                     LibBaseAddrDecPrev = LibBaseAddrDec 
                     LibNamePrev=LibName
                     LibSizeDecPrev = LibSizeDec
                     LibLevelCompPrev=LibLevelComp
                     LibLevel2CompPrev=LibLevel2Comp                    
                  if nextsec or nextsec1 or nextsec3:
                        if islandFlag == 1:
                          Rodataflag=0
                          Textflag=0
                          Bssflag=0
                          Dataflag=0
                        nextsecBase=Commonnextsec.group(2)
                        nextsecBaseDec=int(nextsecBase, 16)
                        DataBaseDec=nextsecBaseDec
          if not LibNamePrev in libhash:
             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                                        
             libhash[LibNamePrev][DataIndex] = libhash[LibNamePrev][DataIndex] + (DataOverallSize - LibBaseAddrDecPrev)
             Comp1 = 'Qualcomm'
             Comp2 = 'avs'
             Comp3 = 'core'
             Comp4 = 'Sensors'                                      
             if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                LibLevelCompPrev = LibLevel2CompPrev
             libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev
          else:
             libhash[LibNamePrev][DataIndex] = libhash[LibNamePrev][DataIndex] + (DataOverallSize - LibBaseAddrDecPrev)
          if islandFlag == 1:
              alignHoleBtwSectionsOnly = (DataBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))             
              if alignHoleBtwSectionsOnly >= 0:
               if alignHoleBtwSectionsOnly > 4024 :
                  AlignHole = float(LibBaseAddrDecPrev+LibSizeDecPrev)/(4.0*1024.0)
                  AlignHole=math.ceil(AlignHole)
                  AlignHole=AlignHole*4*1024
                  AlignHole=AlignHole-(LibBaseAddrDecPrev+LibSizeDecPrev)
                  alignHoleBtwSections = alignHoleBtwSections + (AlignHole)
               else:
                 alignHoleBtwSections = alignHoleBtwSections + (DataBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
              LibBaseAddrDecPrev = DataOverallSize               
     if islandFlag == 1:
        sdata=re.search('^.sdata.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
        if not sdata:
           sdata = re.search('^.sdata\..+\.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     else:
        sdata=re.search('^.sdata\s+(\w+)\s+(\w+)\s+\#.+', to_line) 
     if sdata:
          sdataPriFlag=0;          
          SdataBase=sdata.group(1)
          SdataSize=sdata.group(2)
          SdataBaseDec=int(SdataBase, 16)
          SdataSizeDec=int(SdataSize, 16)
          SdataOverallSize=SdataBaseDec+SdataSizeDec
          line_number=line_number
          while SdataBaseDec < SdataOverallSize:
                  line_number=line_number+1
                  sdataLine=lines[line_number]
                  if islandFlag == 1:
                     nextsec=re.search('^\.(\w+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', sdataLine)                 
                     if nextsec:
                        Commonnextsec=nextsec
                     nextsec3=re.search('^\.(\w+\..+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', sdataLine)       
                     if nextsec3:
                        Commonnextsec=nextsec3
                     nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', sdataLine)         
                  else:                  
                     nextsec=re.search('^\.(\w+)\s+(\w+)\s+(\w+)', sdataLine)
                     if nextsec:
                             Commonnextsec=nextsec
                     nextsec3=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)', sdataLine)
                     if nextsec3:
                             Commonnextsec=nextsec3                             
                     nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', sdataLine)      
                     if nextsec1:
                            line_number1=line_number+1
                            sdataLine1=lines[line_number1]
                            nextsec2=re.search('(\s+)(\w+)\s+(\w+)', sdataLine1)
                            Commonnextsec=nextsec2               
                  FirstPattern=re.search('^\.\w+\.\w+\n',sdataLine)
                  SecondPattern=re.search('^\.\w+\.\w+\.\w+\n',sdataLine)                   
                  if FirstPattern or SecondPattern:
                        line_number=line_number+1
                        ScanLine=lines[line_number]
                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                        if not CommonPattern:
                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',ScanLine)
                             if not CommonPattern:                                     
                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)                                
                                 if not CommonPattern:                            
                                    CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)
                                    if not CommonPattern:
                                       CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)
                                       if not CommonPattern:                            
                                           CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                       
                                           if not CommonPattern:                            
                                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                                                                   
                                             if not CommonPattern:
                                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                                        
                                                 if not CommonPattern:
                                                     CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',ScanLine)         
                                                     if not CommonPattern:                            
                                                         CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                             
                        if CommonPattern:
                                   sdataPriFlag = 1
                                   LibBaseAddr=CommonPattern.group(1)
                                   LibSize=CommonPattern.group(2)
                                   LibLevelComp=CommonPattern.group(3)
                                   LibLevel2Comp=CommonPattern.group(4)
                                   LibName=CommonPattern.group(5)
                                   LibBaseAddrDec=int(LibBaseAddr, 16)
                                   if SdataCheckFlag == 0:
                                         SdataCheckFlag = 1
                                         SdataAlignHole=LibBaseAddrDec-SdataBaseDec
                                         if SdataAlignHole != 0:
                                               SdataAlignName="Sdata_Align"
                                               libhash[SdataAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash1[SdataAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash[SdataAlignName][SdataIndex] = SdataAlignHole
                                               libhash1[SdataAlignName][CompIndex] = SdataAlignName
                                   LibSizeDec=int(LibSize, 16)
                                   if Sdataflag == 1:       
                                         if not LibNamePrev in libhash:
                                             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                          
                                             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][SdataIndex] = libhash[LibNamePrev][SdataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                                     Comp1 = 'Qualcomm'
                                                     Comp2 = 'avs'
                                                     Comp3 = 'core'
                                                     Comp4 = 'Sensors'                                                  
                                                     if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                             LibLevelCompPrev = LibLevel2CompPrev
                                                     libhash1[LibNamePrev][CompIndex] = LibLevelComp        
                                         else:
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][SdataIndex] = libhash[LibNamePrev][SdataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)                                            
                                   Sdataflag=1;                                  
                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sdataLine)
                  if not CommonPattern1:
                     CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sdataLine)
                     if not CommonPattern1:
                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',sdataLine)
                         if not CommonPattern1:
                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',sdataLine)
                            if not CommonPattern1:
                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sdataLine)                             
                               if not CommonPattern1:
                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sdataLine)                              
                                  if not CommonPattern1:
                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sdataLine)
                                     if not CommonPattern1:
                                        CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sdataLine)
                                        if not CommonPattern1:
                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sdataLine)                                        
                                           if not CommonPattern1:
                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sdataLine)                                                                                                                     
                                              if not CommonPattern1:
                                                  CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sdataLine)                                       
                                                  if not CommonPattern1:
                                                      CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sdataLine)                                           
                                                      if not CommonPattern1:
                                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sdataLine)                             
                                                          if not CommonPattern1:
                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sdataLine)                               
                                                              if not CommonPattern1:
                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sdataLine)
                                                                  if not CommonPattern1:
                                                                      CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sdataLine)                                                                                          
                                                                      if not CommonPattern1:
                                                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',sdataLine)                            
                                                                          if not CommonPattern1:
                                                                              CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',sdataLine)                              
                                                                              if not CommonPattern1:
                                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',sdataLine)
                                                                                  if not CommonPattern1:
                                                                                      CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',sdataLine)                                                         
                                                                                      if not CommonPattern1:
                                                                                         CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,sdataLine)                                                                                   
                                                                                         if not CommonPattern1:
                                                                                            CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,sdataLine)
                                                                                            if not CommonPattern1:
                                                                                               CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,sdataLine)                                                                                            
                                                                                               if not CommonPattern1:
                                                                                                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',sdataLine)
                                                                                                  if not CommonPattern1:
                                                                                                     CommonPattern1=re.search('\.\w+\..+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',sdataLine)                                                                                               
                  if CommonPattern1: 
                        sdataPriFlag = 1
                        LibBaseAddr=CommonPattern1.group(1)
                        LibSize=CommonPattern1.group(2)
                        LibLevelComp=CommonPattern1.group(3)
                        LibLevel2Comp=CommonPattern1.group(4)
                        LibName=CommonPattern1.group(5)
                        LibBaseAddrDec=int(LibBaseAddr, 16)
                        if SdataCheckFlag == 0:
                             SdataCheckFlag = 1
                             SdataAlignHole=LibBaseAddrDec-SdataBaseDec
                             if SdataAlignHole != 0:
                                 SdataAlignName="Sdata_Align"
                                 libhash[SdataAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash1[SdataAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash[SdataAlignName][SdataIndex] = SdataAlignHole
                                 libhash1[SdataAlignName][CompIndex] = SdataAlignName   
                        LibSizeDec=int(LibSize, 16)                             
                        if Sdataflag == 1:                                
                             if not LibNamePrev in libhash:
                                 libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                 libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                             
                                 if LibBaseAddrDec != 0:                                
                                         libhash[LibNamePrev][SdataIndex] = libhash[LibNamePrev][SdataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                         Comp1 = 'Qualcomm'
                                         Comp2 = 'avs'
                                         Comp3 = 'core'
                                         Comp4 = 'Sensors'                                      
                                         if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                 LibLevelCompPrev = LibLevel2CompPrev
                                         libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                                              
                             else:
                                 if LibBaseAddrDec != 0:
                                         libhash[LibNamePrev][SdataIndex] = libhash[LibNamePrev][SdataIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                        Sdataflag = 1
                        LibSizeDec=int(LibSize, 16)
                  if LibBaseAddrDec != 0:
                     LibBaseAddrDecPrev = LibBaseAddrDec 
                     LibNamePrev=LibName
                     LibSizeDecPrev = LibSizeDec
                     LibLevelCompPrev=LibLevelComp
                     LibLevel2CompPrev=LibLevel2Comp                    
                  if nextsec or nextsec1 or nextsec3:
                        nextsecBase=Commonnextsec.group(2)
                        nextsecBaseDec=int(nextsecBase, 16)
                        SdataBaseDec=nextsecBaseDec
          if not LibNamePrev in libhash:
             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                                             
             libhash[LibNamePrev][SdataIndex] = libhash[LibNamePrev][SdataIndex] + (SdataOverallSize - LibBaseAddrDecPrev)
             Comp1 = 'Qualcomm'
             Comp2 = 'avs'
             Comp3 = 'core'
             Comp4 = 'Sensors'                                      
             if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                LibLevelCompPrev = LibLevel2CompPrev
             libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev            
          else:
             if sdataPriFlag == 1:
                 libhash[LibNamePrev][SdataIndex] = libhash[LibNamePrev][SdataIndex] + (SdataOverallSize - LibBaseAddrDecPrev)
          if islandFlag == 1:
               alignHoleBtwSectionsOnly = (SdataBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
               if alignHoleBtwSectionsOnly >= 0:
                if alignHoleBtwSectionsOnly > 4024 :
                  AlignHole = float(LibBaseAddrDecPrev+LibSizeDecPrev)/(4.0*1024.0)
                  AlignHole=math.ceil(AlignHole)
                  AlignHole=AlignHole*4*1024
                  AlignHole=AlignHole-(LibBaseAddrDecPrev+LibSizeDecPrev)
                  alignHoleBtwSections = alignHoleBtwSections + (AlignHole)
                else:
                  alignHoleBtwSections = alignHoleBtwSections + (SdataBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
               LibBaseAddrDecPrev = SdataOverallSize                   
     if islandFlag == 1:
        sbss=re.search('^.sbss.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
        if not sbss:
           sbss = re.search('^.sbss\..+\.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     else:          
        sbss=re.search('^.sbss\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if sbss: 
          sbssPriFlag=0
          SbssBase=sbss.group(1)
          SbssSize=sbss.group(2)
          SbssBaseDec=int(SbssBase, 16)
          SbssSizeDec=int(SbssSize, 16)
          line_number=line_number
          if SbssSizeDec != 0:   
             SbssOverallSize=SbssBaseDec+SbssSizeDec
             while SbssBaseDec < SbssOverallSize:
                     line_number=line_number+1
                     sbssLine=lines[line_number]
                     if islandFlag == 1:
                        nextsec=re.search('^\.(\w+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', sbssLine)                 
                        if nextsec:
                           Commonnextsec=nextsec
                        nextsec3=re.search('^\.(\w+\..+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', sbssLine)       
                        if nextsec3:
                           Commonnextsec=nextsec3
                        nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', sbssLine)           
                     else:                   
                        nextsec=re.search('^\.(\w+)\s+(\w+)\s+(\w+)', sbssLine)
                        if nextsec:
                                Commonnextsec=nextsec   
                        nextsec3=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)', sbssLine)
                        if nextsec3:
                                Commonnextsec=nextsec3                                  
                        nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', sbssLine)        
                        if nextsec1:
                               line_number1=line_number+1
                               sbssLine1=lines[line_number1]
                               nextsec2=re.search('(\s+)(\w+)\s+(\w+)', sbssLine1)
                               Commonnextsec=nextsec2                
                     FirstPattern=re.search('^\.\w+\.\w+\n',sbssLine)
                     SecondPattern=re.search('^\.\w+\.\w+\.\w+\n',sbssLine)                 
                     if FirstPattern or SecondPattern:
                           line_number=line_number+1
                           ScanLine=lines[line_number]
                           CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                           if not CommonPattern:
                                CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',ScanLine)
                                if not CommonPattern:                                      
                                    CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)                                 
                                    if not CommonPattern:                            
                                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                       
                                        if not CommonPattern:
                                          CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)
                                          if not CommonPattern:                            
                                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                          
                                             if not CommonPattern:                            
                                                CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                                                                                               
                                                if not CommonPattern:                            
                                                   CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                                            
                                                   if not CommonPattern:
                                                      CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',ScanLine)         
                                                      if not CommonPattern:                            
                                                         CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                              
                           if CommonPattern:
                                      sbssPriFlag=1
                                      LibBaseAddr=CommonPattern.group(1)
                                      LibSize=CommonPattern.group(2)
                                      LibLevelComp=CommonPattern.group(3)
                                      LibLevel2Comp=CommonPattern.group(4)
                                      LibName=CommonPattern.group(5)
                                      LibBaseAddrDec=int(LibBaseAddr, 16)
                                      if SbssCheckFlag == 0:
                                            SbssCheckFlag = 1
                                            SbssAlignHole=LibBaseAddrDec-SbssBaseDec
                                            if SbssAlignHole != 0:
                                                  SbssAlignName="Sbss_Align"
                                                  libhash[SbssAlignName] = [0, 0, 0, 0, 0, 0]
                                                  libhash1[SbssAlignName] = [0, 0, 0, 0, 0, 0]
                                                  libhash[SbssAlignName][SbssIndex] = SbssAlignHole
                                                  libhash1[SbssAlignName][CompIndex] = SbssAlignName
                                      LibSizeDec=int(LibSize, 16)
                                      if Sbssflag == 1:     
                                            if not LibNamePrev in libhash:
                                                libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                           
                                                libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                                if LibBaseAddrDec != 0:
                                                        libhash[LibNamePrev][SbssIndex] = libhash[LibNamePrev][SbssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                                        Comp1 = 'Qualcomm'
                                                        Comp2 = 'avs'
                                                        Comp3 = 'core'
                                                        Comp4 = 'Sensors'                                                       
                                                        if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                                LibLevelCompPrev = LibLevel2CompPrev
                                                        libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                 
                                            else:
                                                if LibBaseAddrDec != 0:
                                                        libhash[LibNamePrev][SbssIndex] = libhash[LibNamePrev][SbssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)                                               
                                      Sbssflag=1; 
                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sbssLine)
                     if not CommonPattern1:
                        CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sbssLine)
                        if not CommonPattern1:
                            CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',sbssLine)
                            if not CommonPattern1:
                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',sbssLine)
                               if not CommonPattern1:
                                  CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sbssLine)                               
                                  if not CommonPattern1:
                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sbssLine)                                    
                                     if not CommonPattern1:
                                        CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sbssLine)
                                        if not CommonPattern1:
                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sbssLine)
                                           if not CommonPattern1:
                                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sbssLine)                                         
                                               if not CommonPattern1:
                                                   CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sbssLine)                                           
                                                   if not CommonPattern1:
                                                       CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sbssLine)                                           
                                                       if not CommonPattern1:
                                                           CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',sbssLine)                                               
                                                           if not CommonPattern1:
                                                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sbssLine)                             
                                                               if not CommonPattern1:
                                                                   CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,sbssLine)                               
                                                                   if not CommonPattern1:
                                                                       CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sbssLine)
                                                                       if not CommonPattern1:
                                                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,sbssLine)                                                                                             
                                                                           if not CommonPattern1:
                                                                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',sbssLine)                            
                                                                               if not CommonPattern1:
                                                                                   CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',sbssLine)                              
                                                                                   if not CommonPattern1:
                                                                                       CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',sbssLine)
                                                                                       if not CommonPattern1:
                                                                                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',sbssLine)                                                                                                             
                                                                                           if not CommonPattern1:
                                                                                               CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,sbssLine)                                                                                   
                                                                                               if not CommonPattern1:
                                                                                                   CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,sbssLine)
                                                                                                   if not CommonPattern1:
                                                                                                       CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,sbssLine)
                                                                                                       if not CommonPattern1:
                                                                                                          CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',sbssLine)
                                                                                                          if not CommonPattern1:
                                                                                                             CommonPattern1=re.search('\.\w+\..+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',sbssLine) 
                     if CommonPattern1:
                           sbssPriFlag=1
                           LibBaseAddr=CommonPattern1.group(1)
                           LibSize=CommonPattern1.group(2)
                           LibLevelComp=CommonPattern1.group(3)
                           LibLevel2Comp=CommonPattern1.group(4)
                           LibName=CommonPattern1.group(5)
                           LibBaseAddrDec=int(LibBaseAddr, 16)
                           if SbssCheckFlag == 0:
                                SbssCheckFlag = 1
                                SbssAlignHole=LibBaseAddrDec-SbssBaseDec
                                if SbssAlignHole != 0:
                                    SbssAlignName="Sbss_Align"
                                    libhash[SbssAlignName] = [0, 0, 0, 0, 0, 0]
                                    libhash1[SbssAlignName] = [0, 0, 0, 0, 0, 0]
                                    libhash[SbssAlignName][SbssIndex] = SbssAlignHole
                                    libhash1[SbssAlignName][CompIndex] = SbssAlignName  
                           LibSizeDec=int(LibSize, 16)                              
                           if Sbssflag == 1:                                  
                                if not LibNamePrev in libhash:
                                    libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                    libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                              
                                    if LibBaseAddrDec != 0:                                
                                            libhash[LibNamePrev][SbssIndex] = libhash[LibNamePrev][SbssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                            Comp1 = 'Qualcomm'
                                            Comp2 = 'avs'
                                            Comp3 = 'core'
                                            Comp4 = 'Sensors'
                                            if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                    LibLevelCompPrev = LibLevel2CompPrev
                                            libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                                           
                                else:
                                    if LibBaseAddrDec != 0:
                                            libhash[LibNamePrev][SbssIndex] = libhash[LibNamePrev][SbssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                           Sbssflag = 1
                           LibSizeDec=int(LibSize, 16)
                     if LibBaseAddrDec != 0:
                        LibBaseAddrDecPrev = LibBaseAddrDec 
                        LibNamePrev=LibName
                        LibSizeDecPrev = LibSizeDec
                        LibLevelCompPrev=LibLevelComp
                        LibLevel2CompPrev=LibLevel2Comp                     
                     if nextsec or nextsec1 or nextsec3:
                           if islandFlag == 1:
                              Bssflag=0
                              Textflag=0
                              Rodataflag=0 
                              Sbssflag=0
                           nextsecBase=Commonnextsec.group(2)
                           nextsecBaseDec=int(nextsecBase, 16)
                           SbssBaseDec=nextsecBaseDec
             if not LibNamePrev in libhash:
                libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                              
                libhash[LibNamePrev][SbssIndex] = libhash[LibNamePrev][SbssIndex] + (SbssOverallSize - LibBaseAddrDecPrev)
                Comp1 = 'Qualcomm'
                Comp2 = 'avs'
                Comp3 = 'core'
                Comp4 = 'Sensors'
                if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                    LibLevelCompPrev = LibLevel2CompPrev
                libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev
             else:
                if sbssPriFlag == 1:
                   libhash[LibNamePrev][SbssIndex] = libhash[LibNamePrev][SbssIndex] + (SbssOverallSize - LibBaseAddrDecPrev)             
             if islandFlag == 1:
                alignHoleBtwSectionsOnly = (SbssBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
                if alignHoleBtwSectionsOnly >= 0:
                 if alignHoleBtwSectionsOnly > 4024 :
                    AlignHole = float(LibBaseAddrDecPrev+LibSizeDecPrev)/(4.0*1024.0)
                    AlignHole=math.ceil(AlignHole)
                    AlignHole=AlignHole*4*1024
                    AlignHole=AlignHole-(LibBaseAddrDecPrev+LibSizeDecPrev)
                    alignHoleBtwSections = alignHoleBtwSections + (AlignHole)
                 else:
                   alignHoleBtwSections = alignHoleBtwSections + (SbssBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
                LibBaseAddrDecPrev = SbssOverallSize                   
     if islandFlag == 1:
        bss=re.search('^.bss.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
        if not bss:
           bss=re.search('^.bss\..+\.island\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     else:      
        bss=re.search('^.bss\s+(\w+)\s+(\w+)\s+\#.+', to_line)
     if bss:
          BssBase=bss.group(1)
          BssSize=bss.group(2)
          BssBaseDec=int(BssBase, 16)
          BssSizeDec=int(BssSize, 16)
          BssOverallSize=BssBaseDec+BssSizeDec
          line_number=line_number
          while BssBaseDec < BssOverallSize:
                  line_number=line_number+1
                  bssLine=lines[line_number]
                  if islandFlag == 1:
                     nextsec=re.search('^\.(\w+.island)\s+(\w+)\s+(\w+)\s+\#.+', bssLine)
                     if nextsec:
                        Commonnextsec=nextsec
                     nextsec3=re.search('^\.(\w+\..+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', bssLine)
                     if nextsec3:
                        Commonnextsec=nextsec3
                     nextsec4=re.search('^\.(\w+\..+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+', bssLine)   
                     if nextsec4:
                        Commonnextsec=nextsec4                     
                  else:      
                     nextsec=re.search('^\.(\w+)\s+(\w+)\s+(\w+)', bssLine)
                     if nextsec:
                             Commonnextsec=nextsec
                     nextsec3=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)', bssLine)
                     if nextsec3:
                             Commonnextsec=nextsec3                             
                     nextsec4=re.search('(QSR_STRING)\s+(\w+)\s+(\w+)', bssLine)
                     if nextsec4:
                             Commonnextsec=nextsec4                             
                     nextsec1=re.search('^\.(\w+)\_DEVCFG\_DATA\n', bssLine)        
                     if nextsec1:
                            line_number1=line_number+1
                            bssLine1=lines[line_number1]
                            nextsec2=re.search('(\s+)(\w+)\s+(\w+)', bssLine1)
                            Commonnextsec=nextsec2               
                  FirstPattern=re.search('^\.\w+\.\w+\n',bssLine)
                  SecondPattern=re.search('^\.\w+\.\w+\.\w+\n',bssLine) 
                  TwenthPattern=re.search('\COMMON\.\w+\n',bssLine)
                  ThirtithPattern=re.search('\COMMON\.\w+\.\w+\n',bssLine)      
                  TwenthonePattern=re.search('\common\.\w+\n',bssLine)
                  ThirtithonePattern=re.search('\common\.\w+\.\w+\n',bssLine)                
                  if FirstPattern or SecondPattern or TwenthPattern or ThirtithPattern or TwenthonePattern or ThirtithonePattern:
                        line_number=line_number+1
                        ScanLine=lines[line_number]
                        CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)
                        if not CommonPattern:
                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',ScanLine)
                             if not CommonPattern:                                     
                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)                                
                                 if not CommonPattern:                            
                                    CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)
                                    if not CommonPattern:
                                       CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,ScanLine)
                                       if not CommonPattern:                            
                                          CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                       
                                          if not CommonPattern:                            
                                             CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,ScanLine)                                                                                   
                                             if not CommonPattern:
                                                 CommonPattern=re.search('(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                                       
                                                 if not CommonPattern:
                                                      CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',ScanLine)         
                                                      if not CommonPattern:                            
                                                         CommonPattern=re.search('(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',ScanLine)                                             
                        if CommonPattern:
                                   LibBaseAddr=CommonPattern.group(1)
                                   LibSize=CommonPattern.group(2)
                                   LibLevelComp=CommonPattern.group(3)
                                   LibLevel2Comp=CommonPattern.group(4)
                                   LibName=CommonPattern.group(5)
                                   LibBaseAddrDec=int(LibBaseAddr, 16)
                                   if BssCheckFlag == 0:
                                         BssCheckFlag = 1
                                         BssAlignHole=LibBaseAddrDec-BssBaseDec
                                         if BssAlignHole != 0:
                                               BssAlignName="Bss_Align"
                                               libhash[BssAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash1[BssAlignName] = [0, 0, 0, 0, 0, 0]
                                               libhash[BssAlignName][BssIndex] = BssAlignHole
                                               libhash1[BssAlignName][CompIndex] = BssAlignName
                                   LibSizeDec=int(LibSize, 16)
                                   if Bssflag == 1:     
                                         if not LibNamePrev in libhash:
                                             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                          
                                             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][BssIndex] = libhash[LibNamePrev][BssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                                     Comp1 = 'Qualcomm'
                                                     Comp2 = 'avs'
                                                     Comp3 = 'core'
                                                     Comp4 = 'Sensors'                                                  
                                                     if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                             LibLevelCompPrev = LibLevel2CompPrev
                                                     libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev                                
                                         else:
                                             if LibBaseAddrDec != 0:
                                                     libhash[LibNamePrev][BssIndex] = libhash[LibNamePrev][BssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)       
                                   Bssflag=1; 
                  CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                  if not CommonPattern1:
                     CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                     if not CommonPattern1:
                        CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',bssLine)
                        if not CommonPattern1:
                           CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',bssLine)
                           if not CommonPattern1:
                              CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,bssLine) 
                              if not CommonPattern1:
                                 CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,bssLine)    
                                 if not CommonPattern1:
                                    CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)
                                    if not CommonPattern1: 
                                       CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)
                                       if not CommonPattern1:
                                          CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)      
                                          if not CommonPattern1:
                                             CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine) 
                                             if not CommonPattern1:
                                                 CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                                                 if not CommonPattern1:
                                                    CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                                                    if not CommonPattern1: 
                                                       CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                                                       if not CommonPattern1:
                                                          CommonPattern1=re.search('\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                                                          if not CommonPattern1:
                                                             CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',bssLine)
                                                             if not CommonPattern1:
                                                                 CommonPattern1=re.search('\w+\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+$',bssLine)
                                                                 if not CommonPattern1:                    
                                                                     CommonPattern1=re.search('\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,bssLine)
                                                                     if not CommonPattern1:
                                                                         CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,bssLine)
                                                                         if not CommonPattern1:
                                                                             CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)
                                                                             if not CommonPattern1: 
                                                                                 CommonPattern1=re.search('\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)
                                                                                 if not CommonPattern1:
                                                                                     CommonPattern1=re.search('\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)   
                                                                                     if not CommonPattern1: 
                                                                                         CommonPattern1=re.search('\w+\s+(\w+)\s+(\w+)\s+\w+\:\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine) 
                                                                                         if not CommonPattern1:
                                                                                             CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,bssLine) 
                                                                                             if not CommonPattern1:
                                                                                                 CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+$' % imageName,bssLine) 
                                                                                                 if not CommonPattern1:
                                                                                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)
                                                                                                     if not CommonPattern1:
                                                                                                         CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+\(.+\)' % imageName,bssLine)                                                                                                                       
                                                                                                         if not CommonPattern1:
                                                                                                             CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',bssLine) 
                                                                                                             if not CommonPattern1:
                                                                                                                 CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+$',bssLine) 
                                                                                                                 if not CommonPattern1:
                                                                                                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                                                                                                                     if not CommonPattern1:
                                                                                                                         CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\/.+\/software\/(\w+)\/\w+\/(\w+).+\/(.+)\.\w+\(.+\)',bssLine)
                                                                                                                         if not CommonPattern1:
                                                                                                                            CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,bssLine)                                                                                   
                                                                                                                            if not CommonPattern1:
                                                                                                                               CommonPattern1=re.search('\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,bssLine)
                                                                                                                               if not CommonPattern1:
                                                                                                                                  CommonPattern1=re.search('\.\w+\.\w+\.\w+\s+(\w+)\s+(\w+)\s+\w+\:.+\/%s\_proc\/(\w+)\/(\w+).+\/(.+)\.\w+' % imageName,bssLine)
                                                                                                                                  if not CommonPattern1:
                                                                                                                                     CommonPattern1=re.search('\.\w+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',bssLine)
                                                                                                                                     if not CommonPattern1:
                                                                                                                                        CommonPattern1=re.search('\.\w+\..+\s+(\w+)\s+(\w+)\s+\.\.\/\.\.\/(\w+)\/(\w+).+\/(.+)\.\w+',bssLine)                                                                                                                                                                                                                                 
                  if CommonPattern1:       
                        LibBaseAddr=CommonPattern1.group(1)
                        LibSize=CommonPattern1.group(2)
                        LibLevelComp=CommonPattern1.group(3)
                        LibLevel2Comp=CommonPattern1.group(4)
                        LibName=CommonPattern1.group(5)
                        LibBaseAddrDec=int(LibBaseAddr, 16)
                        if BssCheckFlag == 0:
                             BssCheckFlag = 1
                             BssAlignHole=LibBaseAddrDec-BssBaseDec
                             if BssAlignHole != 0:
                                 BssAlignName="Bss_Align"
                                 libhash[BssAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash1[BssAlignName] = [0, 0, 0, 0, 0, 0]
                                 libhash[BssAlignName][BssIndex] = BssAlignHole
                                 libhash1[BssAlignName][CompIndex] = BssAlignName   
                        LibSizeDec=int(LibSize, 16)                             
                        if Bssflag == 1:                                  
                             if not LibNamePrev in libhash:
                                 libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
                                 libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                             
                                 if LibBaseAddrDec != 0:                                
                                         libhash[LibNamePrev][BssIndex] = libhash[LibNamePrev][BssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                                         Comp1 = 'Qualcomm'
                                         Comp2 = 'avs'
                                         Comp3 = 'core'
                                         Comp4 = 'Sensors'                                      
                                         if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                                                 LibLevelCompPrev = LibLevel2CompPrev
                                         libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev    
                             else:
                                 if LibBaseAddrDec != 0:
                                         libhash[LibNamePrev][BssIndex] = libhash[LibNamePrev][BssIndex] + (LibBaseAddrDec - LibBaseAddrDecPrev)
                        Bssflag = 1
                        LibSizeDec=int(LibSize, 16)
                  if LibBaseAddrDec != 0:
                     LibBaseAddrDecPrev = LibBaseAddrDec 
                     LibNamePrev=LibName
                     LibSizeDecPrev = LibSizeDec
                     LibLevelCompPrev=LibLevelComp
                     LibLevel2CompPrev=LibLevel2Comp                    
                  if nextsec or nextsec1 or nextsec4 or nextsec3:
                        if islandFlag == 1:                  
                           Dataflag=0
                           Textflag=0
                           Rodataflag=0
                           Bssflag=0
                        nextsecBase=Commonnextsec.group(2)
                        nextsecBaseDec=int(nextsecBase, 16)
                        nextsecBaseLen=Commonnextsec.group(3)
                        nextsecBaseLenDec=int(nextsecBaseLen, 16)
                        if nextsecBaseDec != 0 and nextsecBaseLenDec != 0:                        
                              BssBaseDec=nextsecBaseDec
                        else:
                              BssBaseDec=BssOverallSize                    
          if not LibNamePrev in libhash:
             libhash[LibNamePrev] = [0, 0, 0, 0, 0, 0]
             libhash1[LibNamePrev] = [0, 0, 0, 0, 0, 0]                                                           
             libhash[LibNamePrev][BssIndex] = libhash[LibNamePrev][BssIndex] + (BssOverallSize - LibBaseAddrDecPrev)
             Comp1 = 'Qualcomm'
             Comp2 = 'avs'
             Comp3 = 'core'
             Comp4 = 'Sensors'                                      
             if Comp1 == LibLevelCompPrev or Comp2 == LibLevelCompPrev or Comp3 == LibLevelCompPrev or Comp4 == LibLevelCompPrev:
                LibLevelCompPrev = LibLevel2CompPrev
             libhash1[LibNamePrev][CompIndex] = LibLevelCompPrev           
          else:
             libhash[LibNamePrev][BssIndex] = libhash[LibNamePrev][BssIndex] + (BssOverallSize - LibBaseAddrDecPrev) 
          if islandFlag == 1:
              alignHoleBtwSectionsOnly = (BssBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))                 
              if alignHoleBtwSectionsOnly >= 0:
               if alignHoleBtwSectionsOnly > 4024 :
                  AlignHole = float(LibBaseAddrDecPrev+LibSizeDecPrev)/(4.0*1024.0)
                  AlignHole=math.ceil(AlignHole)
                  AlignHole=AlignHole*4*1024
                  AlignHole=AlignHole-(LibBaseAddrDecPrev+LibSizeDecPrev)
                  alignHoleBtwSections = alignHoleBtwSections + (AlignHole)
               else:
                 alignHoleBtwSections = alignHoleBtwSections + (BssBaseDec - (LibBaseAddrDecPrev+LibSizeDecPrev))
              LibBaseAddrDecPrev = BssOverallSize   
     if toolsUsed <= 6:
      devcfgdata=re.search('^(\.\w+\_DEVCFG\_DATA)\s+\w+\s+(\w+)', to_line)
      if devcfgdata:         
         Base8974=devcfgdata.group(1)
         Size8974=devcfgdata.group(2)   
         DevList.append(Base8974)
         DevIndex=DevIndex+1
         Size8974=int(Size8974, 16)
         DevList.append(Size8974)
         DevIndex=DevIndex+1    
      devdata=re.search('^(\.\w+)\n', to_line)
      devdata1=re.search('^(\w+)\n', to_line)
      devdata2=re.search('^(\w+\.\w+\..+)\n', to_line)  
      devdata3=re.search('^(\.\w+\.\w+.+)\n', to_line)
      if devdata:
        dev8974=devdata
      if devdata1:
        dev8974=devdata1
      if devdata2:
        dev8974=devdata2      
      if devdata3:
        dev8974=devdata3           
      if devdata or devdata1 or devdata2 or devdata3:   
          DevCfgData=dev8974.group(1)                    
          DevCfgData1=re.search('\.debug.*', DevCfgData)
          DevCfgData2=re.search('\.comment.*', DevCfgData)       
          if DevCfgData1 or DevCfgData2:
              iii=0
          else:
             de8974line=lines[line_number]
             dev8974cfg=re.search('\s+(\w+)\s+(\w+)$', de8974line)   
             if dev8974cfg:      
                 Base8974=dev8974cfg.group(1)
                 Size8974=dev8974cfg.group(2)
                 DevList.append(DevCfgData)
                 DevIndex=DevIndex+1
                 Size8974=int(Size8974, 16)
                 Size8974=float(Size8974/1024.0)
                 Size8974=round(Size8974,2)                          
                 DevList.append(Size8974)
                 DevIndex=DevIndex+1     
     ehframe1=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)$', to_line) 
     ehframe=re.search('^(\.\w+)\s+(\w+)\s+(\w+)$', to_line)    
     ehframe2=re.search('^(\.\w+\.\w+)\s+(\w+)\s+(\w+)$', to_line)
     ehframe3=re.search('^(\w+)\s+(0x\w+)\s+(0x\w+)\s+\#.+$', to_line)  
     ehframe4=re.search('^(\.\w+)\s+(\w+)\s+(\w+)\s+\#.+$', to_line)    
     ehframe5=re.search('^(\.\w+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+$', to_line) 
     ehframe6=re.search('^(\.\w+\..+\.\w+)\s+(\w+)\s+(\w+)\s+\#.+$', to_line)
     if ehframe:
           Commpat=ehframe
     if ehframe1:
           Commpat=ehframe1     
     if ehframe2:
           Commpat=ehframe2 
     if ehframe3:
           Commpat=ehframe3
     if ehframe4:
           Commpat=ehframe4
     if ehframe5:
           Commpat=ehframe5 
     if ehframe6:
           Commpat=ehframe6           
     if ehframe or ehframe1 or ehframe2 or ehframe3 or ehframe4 or ehframe5 or ehframe6:
          FiniBase=Commpat.group(1)
          FiniStart=Commpat.group(2)
          FiniSize=Commpat.group(3)
          if FiniBase == '.start':
              ImageStartBase=FiniStart
              ImageStartBase=int(ImageStartBase, 16)
          if FiniBase == '.bss':
              ImageEndBase=FiniStart
              ImageEndBase=int(ImageEndBase, 16)    
              ImageEndSize=FiniSize
              ImageEndSize=int(ImageEndSize, 16)
              ImageEnd1=ImageEndBase+ImageEndSize            
          FiniBase1=re.search('\.debug.*', FiniBase)
          FiniBase2=re.search('\.comment.*', FiniBase)
          if toolsUsed >= 7:
           FiniBase4=re.search('\.shstrtab', FiniBase)
           FiniBase44=re.search('\.shstrtab.*', FiniBase)
           FiniBase5=re.search('\.symtab', FiniBase)
           FiniBase55=re.search('\.symtab.*', FiniBase)
           FiniBase6=re.search('\.strtab', FiniBase)
           FiniBase66=re.search('\.strtab.*', FiniBase) 
          FiniBase3=re.search('QSR_STRING*', FiniBase)
          if toolsUsed <= 6:         
           if FiniBase1 or FiniBase2 or FiniBase3:
               iii=0
           else:
              DevList.append(FiniBase)
              DevIndex=DevIndex+1            
              FiniSize=int(FiniSize, 16)
              FiniSize=float(FiniSize/1024.0)
              FiniSize=round(FiniSize,2)                 
              DevList.append(FiniSize)               
              DevIndex=DevIndex+1
          if toolsUsed >= 7:
           if FiniBase1 or FiniBase2 or FiniBase3 or FiniBase4 or FiniBase44 or FiniBase5 or FiniBase55 or FiniBase6 or FiniBase66:
               iii=0
           else:
              DevList.append(FiniBase)
              DevIndex=DevIndex+1        
              FiniSize=int(FiniSize, 16)
              FiniSize=float(FiniSize/1024.0)
              FiniSize=round(FiniSize,2)                 
              DevList.append(FiniSize)               
              DevIndex=DevIndex+1
 if sdcFlag == 1:              
    csvout.writerow(("Library Name", "CODE_RAM", "DATA_RAM", "Total Size KB", "ComponentName"))
 else:
    csvout.writerow(("Library Name", ".text", ".rodata", ".data", ".sdata", ".sbss", ".bss", "Total Size KB", "ComponentName"))
 for lib in libhash:
    LibAccum=libhash[lib][0]+libhash[lib][1]+libhash[lib][2]+libhash[lib][3]+libhash[lib][4]+libhash[lib][5]
    LibAccum=float(LibAccum/1024.0)
    LibAccum=round(LibAccum,2)
    if sdcFlag ==1:
       csvout.writerow((lib, libhash[lib][0], libhash[lib][1], LibAccum, libhash1[lib][0]))
    else:    
       csvout.writerow((lib, libhash[lib][0], libhash[lib][1], libhash[lib][2], libhash[lib][3], libhash[lib][4], libhash[lib][5], LibAccum, libhash1[lib][0]))
 for lib in libhash:
     if not libhash1[lib][0] in comphash:
          comphash[libhash1[lib][0]] = [0, 0, 0, 0, 0, 0]
          comphash[libhash1[lib][0]][0] = comphash[libhash1[lib][0]][0]+libhash[lib][0]
          comphash[libhash1[lib][0]][1] = comphash[libhash1[lib][0]][1]+libhash[lib][1]
          comphash[libhash1[lib][0]][2] = comphash[libhash1[lib][0]][2]+libhash[lib][2]
          comphash[libhash1[lib][0]][3] = comphash[libhash1[lib][0]][3]+libhash[lib][3]       
          comphash[libhash1[lib][0]][4] = comphash[libhash1[lib][0]][4]+libhash[lib][4]
          comphash[libhash1[lib][0]][5] = comphash[libhash1[lib][0]][5]+libhash[lib][5]       
     else:
          comphash[libhash1[lib][0]][0] = comphash[libhash1[lib][0]][0]+libhash[lib][0]
          comphash[libhash1[lib][0]][1] = comphash[libhash1[lib][0]][1]+libhash[lib][1]
          comphash[libhash1[lib][0]][2] = comphash[libhash1[lib][0]][2]+libhash[lib][2]
          comphash[libhash1[lib][0]][3] = comphash[libhash1[lib][0]][3]+libhash[lib][3]       
          comphash[libhash1[lib][0]][4] = comphash[libhash1[lib][0]][4]+libhash[lib][4]
          comphash[libhash1[lib][0]][5] = comphash[libhash1[lib][0]][5]+libhash[lib][5] 
 csvout.writerow("\n")  
 if sdcFlag == 1:
    csvout.writerow(("Component Name", "CODE_RAM", "DATA_RAM", "Total Size KB"))
 else:    
    csvout.writerow(("Component Name", ".text", ".rodata", ".data", ".sdata", ".sbss", ".bss", "Total Size KB"))          
 for lib in comphash:
    CompAccum=comphash[lib][0]+comphash[lib][1]+comphash[lib][2]+comphash[lib][3]+comphash[lib][4]+comphash[lib][5]
    CompAccum=float(CompAccum/1024.0)
    CompAccum=round(CompAccum,2)
    if sdcFlag == 1:
       csvout.writerow((lib, comphash[lib][0], comphash[lib][1], CompAccum))   
    else:   
       csvout.writerow((lib, comphash[lib][0], comphash[lib][1], comphash[lib][2], comphash[lib][3], comphash[lib][4], comphash[lib][5], CompAccum))   
 DevListLen=len(DevList)
 TotalImageAccu=0
 csvout.writerow("\n")
 for i in range(0,DevListLen,2):
    secname=DevList[i]
    secsize=float(DevList[i+1])
    TotalImageAccu=TotalImageAccu+secsize
    if islandFlag == 1:
       if secsize != 0:
          SecPattern=re.search('.+\.island',secname)
          if SecPattern:
             if secname == '.ukernel.island':
                 secsizeB = float(secsize * 1024.0)
                 AlignHole = float(secsizeB)/(4.0*1024.0)
                 AlignHole=math.ceil(AlignHole)
                 AlignHole=AlignHole*4*1024
                 AlignHoleuKer=AlignHole-secsizeB
             csvout.writerow(('The Size of %s in KB:' %(secname),'%.2f' % (secsize)))
    else:         
       if secsize != 0:
          SecPattern=re.search('.+\.island',secname)
          if not SecPattern:
             csvout.writerow(('The Size of %s in KB:' %(secname),'%.2f' % (secsize)))
 if alignHoleBtwSections != 0 and islandFlag ==1:
   alignHoleBtwSectionsKB=float((alignHoleBtwSections+AlignHoleuKer)/1024.0)
   alignHoleBtwSectionsKB=round(alignHoleBtwSectionsKB,3)
   csvout.writerow(('The Alignment Hole btw Sections in KB:', (alignHoleBtwSectionsKB)))
 infile.close()
 ii=ii+1
 if imageName == 'adsp' or imageName == 'slpi':
    islandFlag=0
