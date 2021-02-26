import sys
import re
import os

if (len(sys.argv) < 2):
    exit()

if ".bin" in sys.argv[1]:
    if os.path.isfile(sys.argv[1]):
        readfile = open(sys.argv[1],"rb")
        writefile = open(sys.argv[2],"a")

        start_addr = "0"
        if len(sys.argv)>=4:
            start_addr = sys.argv[3]
        match1 = 0
        match2 = 0
        if "0x8" in start_addr or "0X8" in start_addr:
            start_addr = start_addr[0]+start_addr[1]+start_addr[3:]
        index = int(start_addr,0)
        readfile.seek(index)
        for line in readfile:
            if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in line:
                words = line.split('\x00')
                for word in words:
                    if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in word:
                        writefile.write(word.split("}_")[1])
                        writefile.write('\n')
                        break
                break
        readfile.close()
        writefile.close()        

else:
    if os.path.isfile(sys.argv[1]):
        readfile = open(sys.argv[1],"rb")
        writefile = open(sys.argv[2].replace('DumpTimeStamp.txt','ELFTimeStamp_v0.txt'),"a")

        start_addr = "0"
        if len(sys.argv)>=4:
            start_addr = sys.argv[3]
        match1 = 0
        match2 = 0
        if "0x8" in start_addr or "0X8" in start_addr:
            start_addr = start_addr[0]+start_addr[1]+start_addr[3:]
        index = int(start_addr,0)
        readfile.seek(index)
        for line in readfile:
            if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in line:
                words = line.split('\x00')
                for word in words:
                    if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in word:
                        writefile.write(word.split("}_")[1])
                        writefile.write('\n')
                        break
                break
        readfile.close()
        writefile.close()        
    if os.path.isfile(sys.argv[1].replace('.elf','_v1.elf')):
        readfile = open(sys.argv[1].replace('.elf','_v1.elf'),"rb")
        writefile = open(sys.argv[2].replace('DumpTimeStamp.txt','ELFTimeStamp_v1.txt'),"a")

        start_addr = "0"
        if len(sys.argv)>=4:
            start_addr = sys.argv[3]
        match1 = 0
        match2 = 0
        if "0x8" in start_addr or "0X8" in start_addr:
            start_addr = start_addr[0]+start_addr[1]+start_addr[3:]
        index = int(start_addr,0)
        readfile.seek(index)
        for line in readfile:
            if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in line:
                words = line.split('\x00')
                for word in words:
                    if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in word:
                        writefile.write(word.split("}_")[1])
                        writefile.write('\n')
                        break
                break
        readfile.close()
        writefile.close()        
    if os.path.isfile(sys.argv[1].replace('.elf','_v2.elf')):
        readfile = open(sys.argv[1].replace('.elf','_v2.elf'),"rb")
        writefile = open(sys.argv[2].replace('DumpTimeStamp.txt','ELFTimeStamp_v2.txt'),"a")

        start_addr = "0"
        if len(sys.argv)>=4:
            start_addr = sys.argv[3]
        match1 = 0
        match2 = 0
        if "0x8" in start_addr or "0X8" in start_addr:
            start_addr = start_addr[0]+start_addr[1]+start_addr[3:]
        index = int(start_addr,0)
        readfile.seek(index)
        for line in readfile:
            if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in line:
                words = line.split('\x00')
                for word in words:
                    if "OEM_IMAGE_UUID_STRING=Q_SENTINEL_{" in word:
                        writefile.write(word.split("}_")[1])
                        writefile.write('\n')
                        break
                break
        readfile.close()
        writefile.close()        
            
