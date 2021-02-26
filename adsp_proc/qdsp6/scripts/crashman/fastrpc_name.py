import sys
import os
import re

def main():
    dynamicobjpath    = sys.argv[1]
    QT = sys.argv[2]
    if QT == "QT":
        list = []
        f_in = open(dynamicobjpath+'\\DSP_QT.txt','rb')
        f_qt = open(dynamicobjpath+'\\DSP_QT_New.txt','a+')
        for line in f_in:
            if "/frpc/" in line and not "/frpc/Audio_PD" in line and not "/frpc/AudioPD" in line:
                cond = False
                for check in list:
                    if check in line:
                        cond = True
                        break
                if cond == False:
                    print line.split('/frpc/')[1].rstrip('\r\n')
                    list.append(line.split('/frpc/')[1].rstrip('\r\n'))
                    print list
                    print '/frpc/'+line.split('/frpc/')[1].rstrip('\r\n')+"\n"
                    f_qt.write('/frpc/'+line.split('/frpc/')[1].rstrip('\r\n')+"\n")
        f_in.close()
        f_qt.close()
    else:
        try:
            frpcshell_name = dynamicobjpath.split('\\')[-1].split('.')[0]
        except:
            frpcshell_name = "None"
        f_out = open('C:\\Temp\\fprc.txt','w')
        f_out.write(frpcshell_name+"\n")
        f_out.close()
    return
    
if __name__ == '__main__':
  main()

