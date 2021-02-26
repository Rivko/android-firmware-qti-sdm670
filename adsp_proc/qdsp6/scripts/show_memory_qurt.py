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
pdendVA=0
pdendSize=0
pdstartVA=0
pdmidVA=0
pdmidSize=0
allPDSize=0
allAligHole=0
chipset=sys.argv[1]
imageName=sys.argv[2]
myfile = open('section_info.txt', 'r')
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
          if z or k:
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

if os.path.exists('./build/chipset/%s/%s/qdsp6.xml'%(chipset,imageName)):
   infile = open("./build/chipset/%s/%s/qdsp6.xml" % (chipset,imageName), 'r')
elif os.path.exists('./build/chipset/%s/qdsp6.xml'% (chipset)):    
   infile = open("./build/chipset/%s/qdsp6.xml" % chipset, 'r')
else:
   print "Not able to find qdsp6.xml file";
   exit()
lines = infile.readlines()			
line_number = 0
while line_number < len(lines):
   to_line = lines[line_number]
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
         Trace_Size=4*Trace_Size
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
for index in range(len(finPDSize)):
    allPDSize=allPDSize+finPDSize[index]
    allAligHole=allAligHole+finPDalgHole[index]

consImageSize=allPDSize+allAligHole+tracesizebytes;
consImageSizeMB=float(consImageSize/(1024.0*1024.0))

if Image_SizeMB > consImageSizeMB:
        print ('*****************LPASS Memory FootPrint************************')
        print "Total Image size declared in MB			:%.4f" % Image_SizeMB
        if (imageName == 'adsp') or (imageName == '0'):
           if allAligHole < 2097152:
              twoMBsize=2097152-allAligHole;
              incDynmodSize=allPDSize+allAligHole+tracesizebytes+twoMBsize
              incDynmodSizeMB=float(incDynmodSize/(1024.0*1024.0))
              if incDynmodSizeMB > Image_SizeMB:
                    print ('**********************************************************************')
                    print ('Error: Not enough size to load the dynamic modules 2MB\n')
                    print ('**********************************************************************') 
                    print "Total Image Size Consumed in MB			:%.4f" % incDynmodSizeMB
                    shutil.rmtree('obj')
              else:
                    #print "Total Image Size Consumed in MB(including alignment holes & Dynamic modules Size):%.4f" % consImageSizeMB
                    print "Total Image Size Consumed in MB			:%.4f" % incDynmodSizeMB
                    freeSpace=(Image_SizeMB-incDynmodSizeMB)*1024
                    print "Free space in KB				:%.4f" %freeSpace
        elif imageName == 'cdsp':
            print "Total Image Size Consumed in MB			:%.4f" % consImageSizeMB
else:
        print ('**********************************************************************')
        print ('Error: Image Size greater than DEFAULT_PHYSPOOL defined in qdsp6.xml\n')
        print ('**********************************************************************')
        print "Total Image size declared in MB			:%.4f" % Image_SizeMB
        print "Total Image Size Consumed in MB			:%.4f" % consImageSizeMB
        shutil.rmtree('obj')

    

    
