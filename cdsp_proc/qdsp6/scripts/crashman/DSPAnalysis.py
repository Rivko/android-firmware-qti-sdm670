import sys
import re
import os
import time
import subprocess
NUM_OF_ALLOCATIONS = 500
ALLOCATION_EQ_20 = 20
GUESTOS_AVAILABLE_MAX_ALLOC_LENGTH_CHECK = 100*1024
SENSORS_AVAILABLE_MAX_ALLOC_LENGTH_CHECK = 100*1024

CALLER_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS       =   {   'qurt_elite_memory_malloc': 40, \
                                                                'rcinit_internal_process_groups': 20,\
                                                                'DALSYS_Malloc': 40}
CALLER_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS  =   {   'qurt_elite_memory_malloc': 1000, \
                                                                'rcinit_internal_process_groups': 500, \
                                                                'DALSYS_Malloc': 1000 }
TCB_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS          =   {   'kernel_root': 40, \
                                                                'VSM': 20,\
                                                                'rcworker': 40}
TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS     =   {   'kernel_root': 1000, \
                                                                'VSM': 500, \
                                                                'rcworker': 1000 }
def ADSPAnalysis():
    DSP_QT = inputfileName+"\CheckDumpStatus.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if line == "1": 
                    f_dsp.write("TimeStamp Validation  : Time Stamp in Dumps and elf file are Matched \n")
                    break
                else:    
                    f_dsp.write("TimeStamp Validation  : Time Stamp in Dumps and elf file are NOT Matched \n")
                    break
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name CheckDumpStatus.txt\n')
    DSP_QT = inputfileName+"\ChickenBits_Value.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if "=" in line: 
                    QT = line.split()
                    f_dsp.write("Chicken bits settings : "+QT[2]+"\n")
                    break
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name ChickenBits_Value.txt\n')
        
    DSP_QT = inputfileName+"\\temp\\flush_cache_py.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if (not "QURTK_flush_cache_status" in line) and (line != "\n") and (not ' ' in line):
                    line = line.rstrip("\n")
                    if( line == "1") or ( line == "2") or ( line == "3"):
                        f_dsp.write("Cache Flush Status    : The DUMPS provided are Cache Flushed")
                    else:
                        f_dsp.write("Cache Flush Status    : The DUMPS provided are NOT Cache Flushed")
                    break
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name flush_cache_py.txt\n')
def SensorLogsError():
    DSP_QT = inputfileName+"\SensorF3Logs\\f3log.txt"
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\n\nErrors found in Sensor F3 logs:\n')
            f_dsp.write('============================================================\n')
            f_qt = open(DSP_QT, 'r+')
            num_lines = 0
            for line in f_qt:
                if (line != "\n"): 
                    if ("fail" in line) or ("error" in line) or ("underrun" in line) or ("overflow" in line) or ("UnSupported" in line):
                        num_lines=num_lines+1
            f_qt = open(DSP_QT, 'r+')
            count = 0
            for line in f_qt:
                if (line != "\n"): 
                    if ("fail" in line) or ("error" in line) or ("underrun" in line) or ("overflow" in line) or ("UnSupported" in line):
                        if (count >= (num_lines-20)) and (count <= num_lines): 
                            f_dsp.write(line)
                        count=count+1
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name SensorF3Logs.txt\n')

        
def F3LogsError():
    DSP_QT = inputfileName+"\\f3log.txt"
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\nErrors found in GuestOS F3 logs:\n')
            f_dsp.write('============================================================\n')
            f_qt = open(DSP_QT, 'r+')
            num_lines = 0
            for line in f_qt:
                if (line != "\n"): 
                    if ("fail" in line) or ("error" in line) or ("underrun" in line) or ("overflow" in line) or ("UnSupported" in line):
                        num_lines=num_lines+1
            f_qt = open(DSP_QT, 'r+')
            count = 0
            for line in f_qt:
                if (line != "\n"): 
                    if ("fail" in line) or ("error" in line) or ("underrun" in line) or ("overflow" in line) or ("UnSupported" in line):
                        if (count >= (num_lines-20)) and (count <= num_lines): 
                            f_dsp.write(line)
                        count=count+1
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name f3log.txt\n')

def SensorLogs():
    DSP_QT = inputfileName+"\SensorF3Logs\\f3log.txt"
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\nLast few messages in Sensor F3 logs:\n')
            f_dsp.write('============================================================\n')
            f_qt = open(DSP_QT, 'r+')
            num_lines = 0
            for line in f_qt:
                if (line != "\n"): 
                    num_lines=num_lines+1
            f_qt = open(DSP_QT, 'r+')
            count = 0
            for line in f_qt:
                if (count >= (num_lines-20)) and (count <= num_lines): 
                    f_dsp.write(line)
                count=count+1
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name SensorF3Logs.txt\n')

        
def F3Logs():
    DSP_QT = inputfileName+"\\f3log.txt"
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\n\nLast few messages in GuestOS F3 logs:\n')
            f_dsp.write('============================================================\n')
            f_qt = open(DSP_QT, 'r+')
            num_lines = 0
            for line in f_qt:
                if (line != "\n"): 
                    num_lines=num_lines+1
            f_qt = open(DSP_QT, 'r+')
            count = 0
            for line in f_qt:
                if (count >= (num_lines-20)) and (count <= num_lines): 
                    f_dsp.write(line)
                count=count+1
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name f3log.txt\n')
    
    
def CrashedCode():
    DSP_QT = inputfileName+"\\temp\\Crashed_Thread_RegSet.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if "ELR" in line: 
                    QT = line.split()
                    ELR = QT[3]
                    f_dsp.write('     The PC at which the ADSP crashed: '+ELR+'\n\n')
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name Crashed_Thread_RegSet.txt\n')

    DSP_QT = inputfileName+"\\temp\\Crashed_ThreadLastExe_Code.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if (not "B::" in line) and (line != "\n"): 
                    if ELR in line:
                        f_dsp.write(line)
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name Crashed_ThreadLastExe_Code.txt\n')
        
        
def HeapAnalysis_GuestOS():
    DSP_QT = inputfileName+"\Heap_Analysis_GuestOS.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('Heap Corruption Detected\n')
                            err = 1
                        cnt = 0
                        for line1 in f_qt:
                            f_dsp.write(line1)
                            cnt = cnt + 1
                            if cnt == 3:
                                break 
                        if cnt == 3:
                            break 
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in GuestOS\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_GuestOS.txt\n')
        
    
    cond = 0
    print_check = 0    

    DSP_QT = inputfileName+"\Def_Heap\___MemCost.csv"
    DSP_QT1 = inputfileName+"\Def_Heap\_MemCost.csv"
    if os.path.exists(DSP_QT1):
        DSP_QT = DSP_QT1
    for files in os.walk(inputfileName+"\\Def_Heap\\"): 
        for file in files:
            for ele in file:
                if "MemCost" in ele:
                    DSP_QT = ele
                    DSP_QT = inputfileName+"\\Def_Heap\\"+DSP_QT
                    print DSP_QT
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if "    Maximum possible Allocation Size      :" in line: 
                    QT = line.split(':')
                    temp = QT[1]
                    if int(temp) < GUESTOS_AVAILABLE_MAX_ALLOC_LENGTH_CHECK:
                        f_dsp.write(line)
                        cond = 1
                        print_check = 1
                        break
            f_qt.close()
            if print_check == 1: #Printing Heap/Memory Allocations only if Max Possible Allocation size is less than MIN_ALLOC_LENGTH
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line1 in f_qt:
                    if "Allocations Summary for DebugInfo (Sorted in total allocated bytes)" in line1: 
                        for line in f_qt:
                            if " 0x" in line: 
                                QT = line.split(',')
                                temp = QT[6].split('.')
                                check_value = ALLOCATION_EQ_20
                                for key in CALLER_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line:
                                        check_value = CALLER_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nPercent Allocated > 20% w.r.t Caller Pointer :\n")
                                        f_dsp.write("=============================\n")
                                        f_dsp.write("Caller Symbol, Percent Allocated\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[6].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "Caller Pointer" in line:
                                    continue
                                else:
                                    break
                f_qt.close()
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line1 in f_qt:
                    if "Allocations Summary for DebugInfo (Sorted in total allocated bytes)" in line1: 
                        for line in f_qt:
                            if " 0x" in line: 
                                QT = line.split(',')
                                temp = QT[2].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in CALLER_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line:
                                        check_value = CALLER_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nNum of Allocations > 500 w.r.t Caller Pointer: \n")
                                        f_dsp.write("==============================\n")
                                        f_dsp.write("Caller Symbol, Num of Allocations\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[2].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "Caller Pointer" in line:
                                    continue
                                else:
                                    break
                f_qt.close()
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line in f_qt:
                    if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "0x" in line1:
                                QT = line1.split(',')
                                temp = QT[7].split('.')
                                check_value = ALLOCATION_EQ_20
                                for key in TCB_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nPercent Allocated > 20% w.r.t TCB:\n")
                                        f_dsp.write("===================================\n")
                                        f_dsp.write("TCB Symbol, Percent Allocated\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[7].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                f_qt.close()
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line in f_qt:
                    if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "0x" in line1: 
                                QT = line1.split(',')
                                temp = QT[3].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nNum of Allocations > 500 w.r.t TCB:\n")
                                        f_dsp.write("===================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[3].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                f_qt.close()
                if cond ==  0:
                    f_dsp.write('\nMemory leak/exhaustion suspected : None\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_GuestOS.txt\n')

def HeapAnalysis_Sensors():
    DSP_QT = inputfileName+"\Heap_Analysis_Sensors.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('Heap Corruption Detected\n')
                            err = 1
                        cnt = 0
                        for line1 in f_qt:
                            f_dsp.write(line1)
                            cnt = cnt + 1
                            if cnt == 3:
                                break 
                        if cnt == 3:
                            break 
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in Sensors PD\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_Sensors.txt\n')
        
    
    cond = 0
    print_check = 0
    DSP_QT  = inputfileName+"\Def_Heap_forSensors_img\___MemCost.csv"
    DSP_QT1 = inputfileName+"\Def_Heap_forSensors_img\_MemCost.csv"
    if os.path.exists(DSP_QT1):
        DSP_QT = DSP_QT1
    for files in os.walk(inputfileName+"\\Def_Heap_forSensors_img\\"): 
        for file in files:
            for ele in file:
                if "MemCost" in ele:
                    DSP_QT = ele
                    DSP_QT = inputfileName+"\\Def_Heap_forSensors_img\\"+DSP_QT
                    print DSP_QT
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if "    Maximum possible Allocation Size      :" in line: 
                    QT = line.split(':')
                    temp = QT[1]
                    if int(temp) < SENSORS_AVAILABLE_MAX_ALLOC_LENGTH_CHECK:
                        f_dsp.write(line)
                        print_check = 1
                        cond = 1
                        break
            f_qt.close()
            if print_check == 1:
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line1 in f_qt:
                    if "Allocations Summary for DebugInfo (Sorted in total allocated bytes)" in line1: 
                        for line in f_qt:
                            if " 0x" in line: 
                                QT = line.split(',')
                                temp = QT[6].split('.')
                                check_value = ALLOCATION_EQ_20
                                for key in CALLER_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line:
                                        check_value = CALLER_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nPercent Allocated > 20% w.r.t Caller Pointer :\n")
                                        f_dsp.write("==============================================\n")
                                        f_dsp.write("Caller Symbol, Percent Allocated\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[6].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "Caller Pointer" in line:
                                    continue
                                else:
                                    break
                f_qt.close()
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line1 in f_qt:
                    if "Allocations Summary for DebugInfo (Sorted in total allocated bytes)" in line1: 
                        for line in f_qt:
                            if " 0x" in line: 
                                QT = line.split(',')
                                temp = QT[2].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in CALLER_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line:
                                        check_value = CALLER_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nNum of Allocations > 500 w.r.t Caller Pointer :\n")
                                        f_dsp.write("===============================================\n")
                                        f_dsp.write("Caller Symbol, Num of Allocations\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[2].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "Caller Pointer" in line:
                                    continue
                                else:
                                    break
                f_qt.close()
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line in f_qt:
                    if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "0x" in line1:
                                QT = line1.split(',')
                                temp = QT[7].split('.')
                                check_value = ALLOCATION_EQ_20
                                for key in TCB_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nPercent Allocated > 20% w.r.t TCB:\n")
                                        f_dsp.write("=====================================\n")
                                        f_dsp.write("TCB Symbol, Percent Allocated\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[7].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                f_qt.close()
                f_qt = open(DSP_QT, 'r+')
                err = 0
                for line in f_qt:
                    if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "0x" in line1: 
                                QT = line1.split(',')
                                temp = QT[3].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nNum of Allocations > 500 w.r.t TCB\n")
                                        f_dsp.write("=============================================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[3].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                f_qt.close()
                if cond ==  0:
                    f_dsp.write('\nMemory leak/exhaustion suspected : None\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_GuestOS.txt\n')
        
        
        
def HeapAnalysis_QURT():
    DSP_QT = inputfileName+"\Heap_Analysis_QURT.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('Heap Corruption Detected\n')
                            err = 1
                        cnt = 0
                        for line1 in f_qt:
                            f_dsp.write(line)
                            cnt = cnt + 1
                            if cnt == 3:
                                break 
                        if cnt == 3:
                            break 
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in QURT')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_QURT.txt\n')
        
        
def HeapAnalysis_QURTISLAND():
    DSP_QT = inputfileName+"\Heap_Analysis_QURTISLAND.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('Heap Corruption Detected\n')
                            err = 1
                        cnt = 0
                        for line1 in f_qt:
                            f_dsp.write(line)
                            cnt = cnt + 1
                            if cnt == 3:
                                break 
                        if cnt == 3:
                            break 
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in QURTISLAND')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_QURTISLAND.txt\n')

        
        
def RegistersInfo():
    DSP_QT = inputfileName+"\\temp\\Crashed_Thread_RegSet.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if (not "B::" in line) and (line != "\n"): 
                    f_dsp.write(line)
                    if "EVB" in line:
                        break
                    
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name Crashed_Thread_RegSet.txt\n')

def CallStack():
    DSP_QT = inputfileName+"\\temp\\Crashed_ThreadCallStack_Locals.txt"
    str_sch = "-000\*"
    for csno in range(1,1000):
        str_sch = str_sch+'|'+'-'+"{0:0=3d}".format(csno)+"\*"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if not "B::" in line: 
                    if "****end of frame" in line:
                        f_dsp.write("----|end of frame")
                        break;
                    y = re.search(str_sch,line)
                    if y:
                        cs = re.split('\(',str(line))[0]
                        cs = cs.replace('*', '|')
                        f_dsp.write(cs+"()\n")
                    
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name Crashed_ThreadCallStack_Locals.txt\n')

def Error_Info():
    DSP_QT = inputfileName+"\\temp\\Error_Info.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            f_dsp.write("\nCause For ADSP Crash  :  \n")
            for line in f_qt:
                if (not "B::" in line) and (not "-----" in line): 
                    if line != "\n":
                        f_dsp.write(line)
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name Error_Info.txt\n')
def Error_Info_Area():
    DSP_QT = inputfileName+"\Area_IO.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            f_dsp.write("Cause For ADSP Crash  :  \n")
            err = 0
            for line in f_qt:
                if "---------------------------" in line: 
                    for line1 in f_qt:
                        f_dsp.write(line1)
                        err = 1
                        break
                if err == 1:
                    break
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name Area_IO.txt\n')
        
def CoreDumpInfo():
    DSP_QT = inputfileName+"\\temp\\SWFatal_Error.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            f_dsp.write("\nThe info in core dump is:\n")
            f_dsp.write('=====================================\n')
            err = 0
            for line in f_qt:
                if (not "B::" in line) and (line != "\n"): 
                    QT = line.split('=')
                    QT[1] = QT[1].replace('"',' ')
                    if err == 0:
                        f_dsp.write("The error message is       :"+QT[1])
                    if err == 1:
                        f_dsp.write("The error message in file  :"+QT[1])
                    if err == 2:
                        f_dsp.write("The params to err message  :"+QT[1])
                    err = err + 1
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name SWFatal_Error.txt\n')
        
        
def CrashSWThreadCheck():
    DSP_QT = inputfileName+"\DSP_QT.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if TCBPointer[2:] in line:  #removing 0X from pointer values and searching in line
                    QT = line.split()
                    crashedThread = QT[len(QT)-1]
                    f_dsp.write("Crashed SW Thread     :  "+crashedThread+"\n")
                    if int(QT[4]) == 0:
                        f_dsp.write("Crashed PD            :  GuestOS\n")
                    elif int(QT[4]) == 2:
                        f_sum.write("Crashed PD            :  fastRPC\n")
                    else:
                        f_dsp.write("Crashed PD            :  Sensors\n")
                    break;
            f_qt.close()
    except:
        f_int.write('ERROR: No Such file name DSP_QT.txt\n')

def main():
    global inputfileName
    global targetName
    global f_int
    global f_dsp
    global TCBPointer
    global FatalErr
    inputfileName=str(sys.argv[1])
    targetName=str(sys.argv[2])
    intenal = inputfileName+"\\temp\\InternalLog.txt"
    f_int = open(intenal,'w+')
    DSPAnlay = inputfileName+"\DSPAnalysis.txt"
    f_dsp = open(DSPAnlay,'a+')
    TCBLIST = inputfileName+"\\temp\\GetCrashedTCB.txt"
    try:
        if os.path.exists(TCBLIST):
            f_tcb = open(TCBLIST, 'r+')
            for line in f_tcb:
                if "0x" in line or "0X" in line:
                    TCBPointer = line
            f_tcb.close()
    except:
        f_int.write('ERROR: No Such file name GetCrashedTCB.txt\n')
    try:
        if os.path.exists(TCBLIST):
            f_tcb = open(TCBLIST, 'r+')
            for line in f_tcb:
                FatalErr = line.rstrip('\n')
                break
            f_tcb.close()
    except:
        f_int.write('ERROR: No Such file name GetCrashedTCB.txt\n')
    # f_dsp.write('Crash Information\n')
    # f_dsp.write('=====================================\n')
    # CrashSWThreadCheck()
    # if int(FatalErr) != 2:
        # Error_Info()
    # else:
        # Error_Info_Area()
    # if int(FatalErr) == 2:
        # CoreDumpInfo()
    # f_dsp.write('\nCall Stack\n')
    # f_dsp.write('=====================================\n')
    # CallStack()
    # f_dsp.write('\n\nADSP\n')
    # f_dsp.write('=====================================\n')
    # ADSPAnalysis()
    # f_dsp.write('\n\nHeap Analysis\n')
    # f_dsp.write('=====================================\n')
    # f_dsp.write('\nGuestOS:\n')
    # HeapAnalysis_GuestOS()
    # f_dsp.write('\n\nSensors PD:\n')
    # HeapAnalysis_Sensors()
    # f_dsp.write('\n\nQURT:\n')
    # HeapAnalysis_QURT()
    # f_dsp.write('\n\nQURTISLAND:\n')
    # HeapAnalysis_QURTISLAND()
    f_dsp.write('\n\n\nRegisters Info\n')
    f_dsp.write('=====================================\n')
    RegistersInfo()
    f_dsp.write('\n\nThe disassembled code where the crashed happened\n')
    f_dsp.write('============================================================\n')
    CrashedCode()
    F3Logs()
    SensorLogs()
    F3LogsError()
    SensorLogsError()
    f_dsp.close()
    f_int.close()

if __name__ == '__main__':
    main()
