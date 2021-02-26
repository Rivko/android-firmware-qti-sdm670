import sys
import os
import re

OutputPath    = sys.argv[1]

dump = OutputPath+"\\"+"DumpTimeStamp.txt"
dump0 = OutputPath+"\\"+"ELFTimeStamp_v0.txt"
dump1 = OutputPath+"\\"+"ELFTimeStamp_v1.txt"
dump2 = OutputPath+"\\"+"ELFTimeStamp_v2.txt"

line1 = False
if os.path.exists(dump):
    f_dump = open(dump,'r+')
    line1 = f_dump.readline()
    f_dump.close()

if line1 == False:
    status = OutputPath+"\\"+"CheckDumpStatus.txt"
    f_status = open(status,'w')
    f_status.write("0")
    f_status.close()
else:
    if os.path.exists(dump0):
        f_dump0 = open(dump0,'r+')
        line2 = f_dump0.readline()
        f_dump0.close()
        status = OutputPath+"\\"+"CheckDumpStatus.txt"
        f_status = open(status,'w')
        if line1 == line2:
            f_status.write("1")
        else:
            f_status.write("0")
        f_status.close()
    else:
        status = OutputPath+"\\"+"CheckDumpStatus.txt"
        f_status = open(status,'w')
        f_status.write("0")
        f_status.close()
    
    if os.path.exists(dump1):
        f_dump1 = open(dump1,'r+')
        line2 = f_dump1.readline()
        f_dump1.close()
        status = OutputPath+"\\"+"CheckDumpStatus_v1.txt"
        f_status = open(status,'w')
        if line1 == line2:
            f_status.write("1")
        else:
            f_status.write("0")
        f_status.close()
    else:
        status = OutputPath+"\\"+"CheckDumpStatus_v1.txt"
        f_status = open(status,'w')
        f_status.write("0")
        f_status.close()
    if os.path.exists(dump2):
        f_dump2 = open(dump2,'r+')
        line2 = f_dump2.readline()
        f_dump2.close()
        status = OutputPath+"\\"+"CheckDumpStatus_v2.txt"
        f_status = open(status,'w')
        if line1 == line2:
            f_status.write("1")
        else:
            f_status.write("0")
        f_status.close()
    else:
        status = OutputPath+"\\"+"CheckDumpStatus_v2.txt"
        f_status = open(status,'w')
        f_status.write("0")
        f_status.close()
            


