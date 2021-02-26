import sys
import re
import os

if (len(sys.argv) < 3):
    exit()

readfile = open(sys.argv[1],"rb")
writefile = open(sys.argv[2],"a")
#with open(sys.argv[1],"rb") as readfile:
#    with open(sys.argv[2],"a") as writefile:
writefile.write("B::Analysis\n")

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
    if "Q6_BUILD" in line:
        match = re.search(r'(QCOM time:Q6_BUILD[^\x00]+)\x00',line)
        if match:
            if match1 == 0:
                writefile.write(match.group(1)+"\n")
                match1 = 1
        match = re.search(r'(ENGG time:Q6_BUILD[^\x00]+)\x00',line)
        if match:
            if match2 == 0:
                writefile.write(match.group(1)+"\n")
                match2 = 1
    if match1 == 1:
        if match2 == 1:
            break

readfile.close()
writefile.close()

if ".bin" in sys.argv[1]:
    readfile = open(sys.argv[1],"rb")
    readfile.seek(index)
    qcom=None
    engg=None
    for line in readfile:
        if "Q6_BUILD" in line and "_PD_SSC" in line:
            split_line = line.split('_PD_SSC')
            qcom = "QCOM time: Q6_BUILD"+split_line[0].split('Q6_BUILD')[1].split('_PD_SSC')[0]+"_PD_SSC"
            engg = "ENGG time: Q6_BUILD"+split_line[1].split('Q6_BUILD')[1].split('_PD_SSC')[0]+"_PD_SSC"
            if qcom != None and engg != None:
                writefile = open(sys.argv[2].replace('DumpTimeStamp','DumpTimeStamp_Sensor'),"a")
                writefile.write("Dump TimeStamp:\n")
                writefile.write(qcom+"\n")
                writefile.write(engg+"\n")
                writefile.close()
                break
    readfile.close()
    readfile = open(sys.argv[1],"rb")
    readfile.seek(index)
    qcom=None
    engg=None
    for line in readfile:
        if "Q6_BUILD" in line and "_PD_AUDIO" in line:
            split_line = line.split('_PD_AUDIO')
            qcom = "QCOM time: Q6_BUILD"+split_line[0].split('Q6_BUILD')[1].split('_PD_AUDIO')[0]+"_PD_AUDIO"
            engg = "ENGG time: Q6_BUILD"+split_line[1].split('Q6_BUILD')[1].split('_PD_AUDIO')[0]+"_PD_AUDIO"
            if qcom != None and engg != None:
                writefile = open(sys.argv[2].replace('DumpTimeStamp','DumpTimeStamp_Audio'),"a")
                writefile.write("Dump TimeStamp:\n")
                writefile.write(qcom+"\n")
                writefile.write(engg+"\n")
                writefile.close()
                break
    readfile.close()
elif ".elf" in sys.argv[1]:
    sensor_elf = sys.argv[1].replace('.elf','_SENSOR.elf')
    if "ROOT_660" in sys.argv[1]:
        audio_elf = sys.argv[1].replace('ROOT_660','AUDIO_660')
        sensor_elf = sys.argv[1].replace('ROOT_660','SENSOR_660')
    elif "ROOT_670" in sys.argv[1]:
        audio_elf = sys.argv[1].replace('ROOT_670','AUDIO_670')
        sensor_elf = sys.argv[1].replace('ROOT_670','SENSOR_670')
    elif "ROOT_845" in sys.argv[1]:
        audio_elf = sys.argv[1].replace('ROOT_845','AUDIO_845')
        sensor_elf = sys.argv[1].replace('ROOT_845','SENSOR_845')
    else:
        audio_elf = sys.argv[1].replace('.elf','_AUDIO.elf')
        sensor_elf = sys.argv[1].replace('.elf','_SENSOR.elf')
    if os.path.isfile(sensor_elf):
        readfile = open(sensor_elf,"rb")
        readfile.seek(index)
        qcom=None
        engg=None
        for line in readfile:
            if "Q6_BUILD" in line and "_PD_SSC" in line:
                split_line = line.split('_PD_SSC')
                qcom = "QCOM time: Q6_BUILD"+split_line[0].split('Q6_BUILD')[1].split('_PD_SSC')[0]+"_PD_SSC"
                engg = "ENGG time: Q6_BUILD"+split_line[1].split('Q6_BUILD')[1].split('_PD_SSC')[0]+"_PD_SSC"
                if qcom != None and engg != None:
                    writefile = open(sys.argv[2].replace('DumpTimeStamp','DumpTimeStamp_Sensor'),"a")
                    writefile.write("Sensor ELF TimeStamp:\n")
                    writefile.write(qcom+"\n")
                    writefile.write(engg+"\n")
                    writefile.close()
                    break
        readfile.close()
    if os.path.isfile(audio_elf):
        readfile = open(audio_elf,"rb")
        readfile.seek(index)
        qcom=None
        engg=None
        for line in readfile:
            if "Q6_BUILD" in line and "_PD_AUDIO" in line:
                split_line = line.split('_PD_AUDIO')
                qcom = "QCOM time: Q6_BUILD"+split_line[0].split('Q6_BUILD')[1].split('_PD_AUDIO')[0]+"_PD_AUDIO"
                engg = "ENGG time: Q6_BUILD"+split_line[1].split('Q6_BUILD')[1].split('_PD_AUDIO')[0]+"_PD_AUDIO"
                if qcom != None and engg != None:
                    writefile = open(sys.argv[2].replace('DumpTimeStamp','DumpTimeStamp_Audio'),"a")
                    writefile.write("Audio ELF TimeStamp:\n")
                    writefile.write(qcom+"\n")
                    writefile.write(engg+"\n")
                    writefile.close()
                    break
        readfile.close()

        
