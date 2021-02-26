import sys
import os
import re

def main():
    OutputPath    = sys.argv[1]
    qurt=OutputPath+"\\"+"QURTVersion.txt"
    try:
        if os.path.exists(qurt):
            f_qurt = open(qurt,'r+')
            qurtversion=0
            for line in f_qurt:
                if "QURT Kernel ver.:" in line:
                    temp = line.split('QURT Kernel ver.:')[1]
                    qurtversion = temp.split('.')[2].split('"')[0]
                    break
            f_qurt.close()        
            status = OutputPath+"\\"+"QURTVerFlag.txt"
            f_status = open(status,'w+')
            if (int(qurtversion) > 19) or (int(qurtversion) == 0):
                f_status.write("3")
            else:    
                f_status.write("2")
            f_status.close()
    except:
        print "File Not Exits QURTVersion.txt"
        sys.exit()

if __name__ == '__main__':
  main()

