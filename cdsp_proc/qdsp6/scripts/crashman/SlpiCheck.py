import sys
import os
import re

def main():
    TargetName    = sys.argv[1]
    OutputPath    = sys.argv[2]
    sensor        = str(sys.argv[3])
    Qube_va_guestos = str(sys.argv[4])
    cache_flush_offset = str(sys.argv[5])
    DSP_Pgt=OutputPath+"\\"+"DSP_pgt.txt"
    DSP_Pgt_out=OutputPath+"\\"+"DSP_pgt_startaddr.txt"
    DSP_Pgt_out2=OutputPath+"\\"+"No_DSP_pgt.txt"
    DSP_Pgt_out1=OutputPath+"\\"+"adspstartaddr.txt"
    cache_flush_offset=cache_flush_offset[:-3]
    DSP_Pgt_out3=OutputPath+"\\"+"DSP_pgt_startaddr_cf.txt"
    if TargetName == "8994" or TargetName == "8992" or TargetName == "8952" or TargetName == "8976" or TargetName == "8953" or TargetName == "8937" or (TargetName == "8996" and sensor =="True")or (TargetName == "8998" and sensor =="True")or (TargetName == "845" and sensor =="True"):
        VAAddr="0xF0100"
    else:
        VAAddr="0xF0000"
        
    if Qube_va_guestos != "None":
        VAAddr=Qube_va_guestos[:-3]
        
    try:
        if os.path.exists(DSP_Pgt):
            f_qurt = open(DSP_Pgt,'r+')
            for line in f_qurt:
                if VAAddr in line:
                    if "*****" in line:
                        f_qurt1 = open(DSP_Pgt_out2,'w')
                        f_qurt1.write("1\n")
                        f_qurt1.close()
                        break
                    
                    temp = line.split('0x')[2]
                    temp=temp.replace(' ','').replace('*','')
                    StartAddr = "0x"+temp+"000"
                    # print StartAddr
                    f_qurt1 = open(DSP_Pgt_out,'w')
                    f_qurt1.write(StartAddr+"\n")
                    f_qurt1.close()
                    f_qurt2 = open(DSP_Pgt_out1,'w')
                    f_qurt2.write(StartAddr+"\n")
                    f_qurt2.close()
                off  = "0x"+cache_flush_offset.upper()[2:]
                if off in line:
                    temp = line.split('0x')[2]
                    temp=temp.replace(' ','').replace('*','')
                    StartAddr = "0x"+temp+"000"
                    VirtAddr = "0x"+cache_flush_offset.upper()[2:]+"000"
                    print StartAddr
                    f_qurt3 = open(DSP_Pgt_out3,'w')
                    f_qurt3.write(StartAddr+"\n")
                    f_qurt3.write(VirtAddr+"\n")
                    f_qurt3.close()
            f_qurt.close()
    except:
        print "File Not Exits DSP_pgt.txt"
        f_qurt1 = open(DSP_Pgt_out2,'w')
        f_qurt1.write("1\n")
        f_qurt1.close()
        sys.exit()
    DSP_Pgt=OutputPath+"\\"+"DSP_pgt.txt"
    DSP_Pgt_out=OutputPath+"\\"+"DSP_pgt_startaddr_f000.txt"
    VAAddr="0xF0000"
    if Qube_va_guestos != "None":
        if "0xF010" in Qube_va_guestos:
            VAAddr="0xF0000"
        if "0xB010" in Qube_va_guestos:
            VAAddr="0xB0000"
    try:
        if os.path.exists(DSP_Pgt):
            f_qurt = open(DSP_Pgt,'r+')
            for line in f_qurt:
                if VAAddr in line:
                    temp = line.split('0x')[2]
                    temp=temp.replace(' ','').replace('*','')
                    StartAddr = "0x"+temp+"000"
                    # print StartAddr
                    f_qurt1 = open(DSP_Pgt_out,'w')
                    f_qurt1.write(StartAddr+"\n")
                    f_qurt1.close()
            f_qurt.close()
    except:
        print "File Not Exits DSP_pgt.txt"
        sys.exit()
    return
if __name__ == '__main__':
  main()

