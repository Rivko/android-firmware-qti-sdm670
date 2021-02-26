import gdb
import os
import subprocess

def get(command):
    if os.path.isfile(output_folder+'\\temp\\temp.log'): os.remove(output_folder+'\\temp\\temp.log')
    gdb.execute('set logging file '+output_folder+'\\temp\\temp.log')
    gdb.execute('set logging on')
    gdb.execute(command)
    gdb.execute('set logging off')
    f=open(output_folder+'\\temp\\temp.log','r')
    name = f.readline()
    f.close()
    return name
    
def get_value(variable):
    name = get('print '+str(variable)).split("= ")[1]
    return name
    
def get_function(address):
    name = get('info symbol '+str(address)).split(" ")[0]
    if name == "No":
        name = hex(address)
    return name

def extract_thread_name(string):
    return string.split("\\")[0]+'"'
 
def thread_context(TCB,arg):
    return int(get_value('((QURTK_thread_context*)'+str(TCB)+').'+arg)) 

def pointer_value(address):
    line = get('x '+str(address))
    return line.split('\t')[1]

def GetOffset(sensor_elf,output_folder):
    command = '''qdsp6-readelf -t %s > %s\\temp\\elfheader.txt'''%(sensor_elf,output_folder)
    os.system(command) 
    
    offset = None
    try:
        elf = output_folder + "\\temp\\elfheader.txt"
        f_elf = open(elf,'r+')
        for line in f_elf:
            if ".text" in line and not ".text." in line:
                for line1 in f_elf:
                    offset = line1.split('PROGBITS        ')[1].split(' ')[0]
                    break
        f_elf.close()
    except:
        print "Unable to process elf header!!!"
    
    return offset
    
    
def DynamicSoLoading(output_folder,sofile_path):
    try:
        temp = "_rtld_debug.r_map.l_next"
        Linkedlist_l_next =  int(get_value(temp).split(')')[1].replace(' ',''),16)
        if Linkedlist_l_next == 0:
            print " No Dynamic objects loaded using fastrpc"
            return
            
        soaddr=os.path.join(output_folder,"temp","soaddress.txt")
        f_soaddr = open(soaddr,'w+')
        
        print " DYNAMIC OBJECTS LOADED: \n"
        f_soaddr.write(" DYNAMIC OBJECTS LOADED: \n")
        while Linkedlist_l_next != 0:
            Linkedlist_l_name =  get_value(temp+".l_name").split('"')[1].replace(' ','')
            Linkedlist_l_addr =  get_value(temp+".l_addr").split(') ')[1].split(' ')[0]
            print " Start address: "+Linkedlist_l_addr+", Obj Name: "+Linkedlist_l_name+"\n"
            f_soaddr.write(" Start address: "+Linkedlist_l_addr+", Obj Name: -> \""+Linkedlist_l_name+"\"\n")
            temp = temp+".l_next"
            Linkedlist_l_next =  int(get_value(temp).split(')')[1].replace(' ',''),16)
        f_soaddr.close()
        command = '''python %s\dynamicso.py %s %s %s %s'''%(cwd[:-5],output_folder,sofile_path,sofile_path,input_dump_path)
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        process.wait() 
        
        soaddr=os.path.join(output_folder,"temp","loaddynmaic.cmm")
        f_soaddr = open(soaddr,'r+')
        for line in f_soaddr:
            if "d.load.elf" in line:
                print 'add-symbol-file '+line.split(' ')[1]+' '+line.split(' ')[2]
                offset = GetOffset(line.split(' ')[1],output_folder)
                offset = hex(int(offset,16)+int(line.split(' ')[2],16))
                gdb.execute('add-symbol-file '+line.split(' ')[1].replace('\\','\\\\')+' '+offset)
        f_soaddr.close()
        
    except:
        file.write("_rtld_debug Symbol is not found")
# gdb.execute('target remote localhost:9010')
# gdb.execute('symbol-file M8974AAAAAAAAQ1234.elf')

    
#gdb.execute('quit')
filename=os.path.join(output_folder,"DSPAnalysis.txt")
file = open(filename,'w')
gdb.execute('set logging off')
fatal = int(get_value('QURT_error_info.status.fatal'))
count = int(get_value('QURT_error_info.users_errors.counter'))


if sofile_path != "False":
    DynamicSoLoading(output_folder,sofile_path)

if count>5:count=5
counter = 0
TCB = 0
while counter<count:
    TCB = int(get_value('QURT_error_info.users_errors.entry['+str(counter)+'].error_tcb').split(" ")[-1],16)
    if TCB != 0:
        break
    counter += 1

if fatal == 1 and TCB==0:
    TCB = int(get_value('QURT_error_info.globregs.context').split(" ")[-1],16)
    
if TCB !=0:
    val = thread_context(TCB,'ugpgp')>>32
    nbits = 32
    ugp = (val + (1 << nbits)) % (1 << nbits)    #converts negative integer to hex
    elr = thread_context(TCB,'elr')
    ssr = thread_context(TCB,'ssr')
    LR_FP = thread_context(TCB,'r3130')
    val = LR_FP>>32
    LR = (val + (1 << nbits)) % (1 << nbits)
    FP = LR_FP&0xFFFFFFFF
    ASID = (ssr>>8)&255
    thread_name = extract_thread_name(get_value('((*((QURT_utcb_t*)'+str(ugp)+')).attr.name)'))
    file.write("Crash Information:\n")
    file.write("\nName of the crashed thread         : "+thread_name)
    if ASID == 0:
        file.write("\nPD where crash happened            : Guest PD")
    elif ASID == 2:
        file.write("\nPD where crash happened            : fastRPC")
    else:
        file.write("\nPD where crash happened            : Sensors PD")
    
    file.write("\nSSR of crashed thread              : "+str(hex(ssr)))
    file.write("\nTCB Pointer of crashed thread      : "+str(hex(TCB))[:-1])
    file.write("\nUGP of crashed thread              : "+str(hex(ugp))[:-1])
    file.write("\nELR of crashed thread              : "+str(hex(elr))[:-1])
    file.write("\n\n\nCall Stack:\n.....................................")
    file.write("\n"+get_function(elr)+"()")
    file.write("\n"+get_function(LR)+"()")
    
    while(FP !=0):
        LR = FP+4
        LR = int(pointer_value(LR),16)
        FP = int(pointer_value(FP),16)
        file.write("\n"+get_function(LR)+"()")
        if "qurt_trampoline" in get_function(LR): break
    file.write("\n.....................................")
if os.path.isfile(output_folder+'\\temp\\temp.log'): os.remove(output_folder+'\\temp\\temp.log')
file.close()
