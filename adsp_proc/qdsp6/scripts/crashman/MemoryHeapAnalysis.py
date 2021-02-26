import sys
import re
import os
import time
import subprocess
import CacheStatusAnalysis

DDField1  = "DDRunningCS" #Runnning Call Stack
DDField2  = "DDQurtSystemState" #Stack Walker Call Stack
DDField3  = "DDMemoryLeakGuestOS" 
DDField5  = "DDCoreDumpInfo"
DDField6  = "DDCrashedThreadName"
DDField7  = "DDQurtAssert"
DDField8  = "DDException"
DDField9  = "DDF3Error"
DDField10 = "DDDBError"
DDField11 = "DDAPRError"
DDField12 = "DDMBError"
DDField13 = "DDStackWalkerCS" #Stack Walker Call Stack
DDField14  = "DDMemoryLeakSensors" 
DDField15  = "DDMemoryLeakPerAllocCallPtrGuestOS"
DDField16  = "DDMemoryLeakNumAllocCallPtrGuestOS"
DDField17  = "DDMemoryLeakPerAllocTCBGuestOS"
DDField18  = "DDMemoryLeakNumAllocTCBGuestOS"
DDField19  = "DDMemoryLeakPerAllocCallPtrSensor"
DDField20  = "DDMemoryLeakNumAllocCallPtrSensor"
DDField21  = "DDMemoryLeakPerAllocTCBSensor"
DDField22  = "DDMemoryLeakNumAllocTCBSensor"
DDField23  = "DDMemoryCorruptionGuestOS"
DDField24  = "DDMemoryCorruptionSensor"
DDField25  = "DDMemoryCorruptionQURT"
DDField26  = "DDMemoryCorruptionQURTISLAND"
DDField27  = "DDMemoryCorruptionAUDIO"
DDField28  = "DDMemoryLeakPerAllocCallPtrAudio"
DDField29  = "DDMemoryLeakNumAllocCallPtrAudio"
DDField30  = "DDMemoryLeakPerAllocTCBAudio"
DDField31  = "DDMemoryLeakNumAllocTCBAudio"

NUM_OF_ALLOCATIONS = 500 # Threshold for memory leak detection- number of allocations
ALLOCATION_EQ_20 = 20    # Threshold for memory leak detection - percentage of allocation
GUESTOS_AVAILABLE_MAX_ALLOC_LENGTH_CHECK = 100*1024 #  Threshold for "maximum freeblock size" for guest os
SENSORS_AVAILABLE_MAX_ALLOC_LENGTH_CHECK = 100*1024 #  Threshold for "maximum freeblock size" for sensors
AUDIO_AVAILABLE_MAX_ALLOC_LENGTH_CHECK = 100*1024 #  Threshold for "maximum freeblock size" for Audio

CALLER_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS       =   {   'qurt_elite_memory_malloc': 40, \
                                                                'rcinit_internal_process_groups': 20,\
                                                                'MMPM_Register_Ext': 700,\
                                                                'glink_os_calloc': 700,\
                                                                'DALSYS_Malloc': 40}
CALLER_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS  =   {   'qurt_elite_memory_malloc': 1000, \
                                                                'rcinit_internal_process_groups': 500, \
                                                                'MMPM_Register_Ext': 700,\
                                                                'glink_os_calloc': 700,\
                                                                'rcinit_worker': 1600,\
                                                                '_rtld_load_object': 2300,\
                                                                'DALSYS_Malloc': 1000 }
TCB_SYMBOLS_PERCENT_ALLOCATED_CHECK_CONDITIONS          =   {   'kernel_root': 40, \
                                                                'VSM': 20,\
                                                                'MMPM_Register_Ext': 700,\
                                                                'glink_os_calloc': 700,\
                                                                'NoSymbol':1000,\
                                                                'rcworker': 40}
TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS     =   {   'kernel_root': 1000, \
                                                                'VSM': 500, \
                                                                'MMPM_Register_Ext': 700,\
                                                                'glink_os_calloc': 700,\
                                                                'rcinit_worker': 1600,\
                                                                'AMDB0': 1000,\
                                                                'AMDB1': 1000,\
                                                                'AMDB2': 1000,\
                                                                'AMDB3': 1000,\
                                                                'NoSymbol':1000000,\
                                                                'rcworker': 1300 }
        
def HeapAnalysis_GuestOS(inputfileName):
    intenal = inputfileName+"\\temp\\InternalLog.txt"
    f_int = open(intenal,'a')
    DSPAnlay = inputfileName+"\DSPAnalysis.txt"
    f_dsp = open(DSPAnlay,'a')
    f_dsp.write('\n\nHeap Analysis\n')
    f_dsp.write('=====================================\n')
    DSP_QT = inputfileName+"\\Qurt_logs\\Heap_Analysis_GuestOS.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\nGuestOS:\n')
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionGuestOS\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField23)
                            err = 1
                        for line1 in f_qt:
                            if "Trying to find out next valid block" in line1:
                                break 
                            f_dsp.write(line1)
                    if "Corruption detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionGuestOS\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField23)
                            err = 1
                        f_dsp.write(" -----------------------------------------------------------------------------------------------------------------------------------------------------\n")
                        f_dsp.write(line)
                        for line1 in f_qt:
                            if "End:free BINs verification" in line1:
                                break 
                            f_dsp.write(line1)
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in GuestOS\n')
            if heapstatsFlag ==  1:
                CacheStatusAnalysis.UpdateDDField(inputfileName,DDField4)
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_GuestOS.txt\n')
        
    
    cond = 0
    print_check = 1  
    percent_allocation_check = 0
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
                        f_dsp.write('\nPossible Memory Exhaustion:\n')
                        num=line.split(':')[1].replace(' ','')
                        f_dsp.write('Maximum Possible Allocation Size      :       '+num+'\n')
                        # f_dsp.write(line)
                        # cond = 1
                        # print_check = 1
                        percent_allocation_check = 1
                        break
            f_qt.close()
            if print_check == 1: #Printing Heap/Memory Allocations
                if percent_allocation_check == 1:
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
                                            f_dsp.write("\n\nMemoryLeakPerAllocCallPtrGuestOS\nPercent Allocated > 20% w.r.t Caller Pointer for GuestOS :\n")
                                            f_dsp.write("=============================\n")
                                            f_dsp.write("Caller Symbol, Percent Allocated\n")
                                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField15)
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
                                        f_dsp.write("\n\nMemoryLeakNumAllocCallPtrGuestOS\nNum of Allocations > 500 w.r.t Caller Pointer for GuestOS :\n")
                                        f_dsp.write("==============================\n")
                                        f_dsp.write("Caller Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField16)
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
                if percent_allocation_check == 1:
                    f_qt = open(DSP_QT, 'r+')
                    err = 0
                    for line in f_qt:
                        if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line or "Allocations Summary for tasks (Sorted in total allocated bytes)" in line: 
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
                                            f_dsp.write("\n\nMemoryLeakPerAllocTCBGuestOS\nPercent Allocated > 20% w.r.t TCB for GuestOS :\n")
                                            f_dsp.write("===================================\n")
                                            f_dsp.write("TCB Symbol, Percent Allocated\n")
                                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField17)
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
                                        f_dsp.write("\n\nMemoryLeakNumAllocTCBGuestOS\nNum of Allocations > 500 w.r.t TCB for GuestOS :\n")
                                        f_dsp.write("===================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField18)
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[3].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                    if "Allocations Summary for tasks (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "Task Name" in line1:
                                continue
                            if line1 == "\n":
                                break
                            else:    
                                QT = line1.split(',')
                                temp = QT[2].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nMemoryLeakNumAllocTCBGuestOS\nNum of Allocations > 500 w.r.t TCB for GuestOS :\n")
                                        f_dsp.write("===================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField18)
                                        err = 1
                                    f_dsp.write(QT[0]+",\t")
                                    f_dsp.write(QT[2].rstrip('\n')+"\n")
                                    cond = 1
                f_qt.close()
                if cond ==  0:
                    f_dsp.write('\nMemory leak/exhaustion suspected : None\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_GuestOS.txt\n')
    f_dsp.close()
    f_int.close()
    return

def HeapAnalysis_AUDIO(inputfileName):
    intenal = inputfileName+"\\temp\\InternalLog.txt"
    f_int = open(intenal,'a')
    DSPAnlay = inputfileName+"\DSPAnalysis.txt"
    f_dsp = open(DSPAnlay,'a')
    DSP_QT = inputfileName+"\\Qurt_logs\\Heap_Analysis_Audio.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\n\nAUDIO:\n')
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line or "<<<IncomingBlock got *concatenated*>>>" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionAUDIO\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField27)
                            err = 1
                        for line1 in f_qt:
                            if "Trying to find out next valid block" in line1 or "<<<End of info for this block>>>" in line1:
                                break 
                            f_dsp.write(line1)
                    if "Corruption detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionAUDIO\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField27)
                            err = 1
                        f_dsp.write(" -----------------------------------------------------------------------------------------------------------------------------------------------------\n")
                        f_dsp.write(line)
                        for line1 in f_qt:
                            if "End:free BINs verification" in line1:
                                break 
                            f_dsp.write(line1)
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in AUDIO\n')
            if heapstatsFlag ==  1:
                CacheStatusAnalysis.UpdateDDField(inputfileName,DDField4)
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_Audio.txt\n')
        
    
    cond = 0
    print_check = 1  
    percent_allocation_check = 0
    DSP_QT = inputfileName+"\Def_Heap_for_AUDIO_img\___MemCost.csv"
    DSP_QT1 = inputfileName+"\Def_Heap_for_AUDIO_img\_MemCost.csv"
    if os.path.exists(DSP_QT1):
        DSP_QT = DSP_QT1
    for files in os.walk(inputfileName+"\\Def_Heap_for_AUDIO_img\\"): 
        for file in files:
            for ele in file:
                if "MemCost" in ele:
                    DSP_QT = ele
                    DSP_QT = inputfileName+"\\Def_Heap_for_AUDIO_img\\"+DSP_QT
                    print DSP_QT
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if "    Maximum possible Allocation Size      :" in line: 
                    QT = line.split(':')
                    temp = QT[1]
                    if int(temp) < AUDIO_AVAILABLE_MAX_ALLOC_LENGTH_CHECK:
                        f_dsp.write('\nPossible Memory Exhaustion:\n')
                        num=line.split(':')[1].replace(' ','')
                        f_dsp.write('Maximum Possible Allocation Size      :       '+num+'\n')
                        # f_dsp.write(line)
                        # cond = 1
                        # print_check = 1
                        percent_allocation_check = 1
                        break
            f_qt.close()
            if print_check == 1: #Printing Heap/Memory Allocations
                if percent_allocation_check == 1:
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
                                            f_dsp.write("\n\nMemoryLeakPerAllocCallPtrAudio\nPercent Allocated > 20% w.r.t Caller Pointer for AUDIO :\n")
                                            f_dsp.write("=============================\n")
                                            f_dsp.write("Caller Symbol, Percent Allocated\n")
                                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField28)
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
                                        f_dsp.write("\n\nMemoryLeakNumAllocCallPtrAudio\nNum of Allocations > 500 w.r.t Caller Pointer for AUDIO :\n")
                                        f_dsp.write("==============================\n")
                                        f_dsp.write("Caller Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField29)
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
                if percent_allocation_check == 1:
                    f_qt = open(DSP_QT, 'r+')
                    err = 0
                    for line in f_qt:
                        if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line or "Allocations Summary for tasks (Sorted in total allocated bytes)" in line: 
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
                                            f_dsp.write("\n\nMemoryLeakPerAllocTCBAudio\nPercent Allocated > 20% w.r.t TCB for AUDIO :\n")
                                            f_dsp.write("===================================\n")
                                            f_dsp.write("TCB Symbol, Percent Allocated\n")
                                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField30)
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
                                        f_dsp.write("\n\nMemoryLeakNumAllocTCBAudio\nNum of Allocations > 500 w.r.t TCB for AUDIO :\n")
                                        f_dsp.write("===================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField31)
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[3].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                    if "Allocations Summary for tasks (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "Task Name" in line1:
                                continue
                            if line1 == "\n":
                                break
                            else:    
                                QT = line1.split(',')
                                temp = QT[2].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nMemoryLeakNumAllocTCBAudio\nNum of Allocations > 500 w.r.t TCB for AUDIO :\n")
                                        f_dsp.write("===================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField31)
                                        err = 1
                                    f_dsp.write(QT[0]+",\t")
                                    f_dsp.write(QT[2].rstrip('\n')+"\n")
                                    cond = 1
                f_qt.close()
                if cond ==  0:
                    f_dsp.write('\nMemory leak/exhaustion suspected : None\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_Audio.txt\n')
    f_dsp.close()
    f_int.close()
    return
    
def HeapAnalysis_Sensors(inputfileName):
    intenal = inputfileName+"\\temp\\InternalLog.txt"
    f_int = open(intenal,'a')
    DSPAnlay = inputfileName+"\DSPAnalysis.txt"
    f_dsp = open(DSPAnlay,'a')
    DSP_QT = inputfileName+"\\Qurt_logs\\Heap_Analysis_Sensors.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\n\nSensors PD:\n')
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionSensor\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField24)
                            err = 1
                        for line1 in f_qt:
                            if "Trying to find out next valid block" in line1:
                                break 
                            f_dsp.write(line1)
                    if "Corruption detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionSensor\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField24)
                            err = 1
                        f_dsp.write(" -----------------------------------------------------------------------------------------------------------------------------------------------------\n")
                        f_dsp.write(line)
                        for line1 in f_qt:
                            if "End:free BINs verification" in line1:
                                break 
                            f_dsp.write(line1)
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in Sensors PD\n')
            if heapstatsFlag ==  1:
                CacheStatusAnalysis.UpdateDDField(inputfileName,DDField4)
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_Sensors.txt\n')
        
    
    cond = 0
    print_check = 1
    percent_allocation_check_sensor = 0
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
                        f_dsp.write('\nPossible Memory Exhaustion:\n')
                        num=line.split(':')[1].replace(' ','')
                        f_dsp.write('Maximum Possible Allocation Size      :       '+num+'\n')
                        # f_dsp.write(line)
                        # print_check = 1
                        # cond = 1
                        percent_allocation_check_sensor = 1
                        break
            f_qt.close()
            if print_check == 1:
                if percent_allocation_check_sensor == 1:
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
                                            f_dsp.write("\n\nMemoryLeakPerAllocCallPtrSensor\nPercent Allocated > 20% w.r.t Caller Pointer for Sensors :\n")
                                            f_dsp.write("==============================================\n")
                                            f_dsp.write("Caller Symbol, Percent Allocated\n")
                                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField19)
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
                                        f_dsp.write("\n\nMemoryLeakNumAllocCallPtrSensor\nNum of Allocations > 500 w.r.t Caller Pointer for Sensors :\n")
                                        f_dsp.write("===============================================\n")
                                        f_dsp.write("Caller Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField20)
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
                if percent_allocation_check_sensor == 1:
                    f_qt = open(DSP_QT, 'r+')
                    err = 0
                    for line in f_qt:
                        if "Allocations Summary for Tcb (Sorted in total allocated bytes)" in line or "Allocations Summary for tasks (Sorted in total allocated bytes)" in line: 
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
                                            f_dsp.write("\n\nMemoryLeakPerAllocTCBSensor\nPercent Allocated > 20% w.r.t TCB for Sensors :\n")
                                            f_dsp.write("=====================================\n")
                                            f_dsp.write("TCB Symbol, Percent Allocated\n")
                                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField21)
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
                                        f_dsp.write("\n\nMemoryLeakNumAllocTCBSensor\nNum of Allocations > 500 w.r.t TCB  for Sensors :\n")
                                        f_dsp.write("=============================================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField22)
                                        err = 1
                                    f_dsp.write(QT[1]+",\t")
                                    f_dsp.write(QT[3].rstrip('\n')+"\n")
                                    cond = 1
                            else:
                                if "TCB Pointer" in line1:
                                    continue
                                else:
                                    break
                    if "Allocations Summary for tasks (Sorted in total allocated bytes)" in line: 
                        for line1 in f_qt:
                            if "Task Name" in line1:
                                continue
                            if line1 == "\n":
                                break
                            else:    
                                QT = line1.split(',')
                                temp = QT[2].split('.')
                                check_value = NUM_OF_ALLOCATIONS
                                for key in TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS:
                                    if key in line1:
                                        check_value = TCB_SYMBOLS_NUM_OF_BLOCK_ALLOCATED_CHECK_CONDITIONS[key]
                                if int(temp[0]) >= check_value:
                                    if err == 0:
                                        f_dsp.write("\n\nMemoryLeakNumAllocTCBSensor\nNum of Allocations > 500 w.r.t TCB  for Sensors :\n")
                                        f_dsp.write("=============================================================\n")
                                        f_dsp.write("TCB Symbol, Num of Allocations\n")
                                        CacheStatusAnalysis.UpdateDDField(inputfileName,DDField22)
                                        err = 1
                                    f_dsp.write(QT[0]+",\t")
                                    f_dsp.write(QT[2].rstrip('\n')+"\n")
                                    cond = 1
                f_qt.close()
                if cond ==  0:
                    f_dsp.write('\nMemory leak/exhaustion suspected : None\n')
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_GuestOS.txt\n')
        
    f_dsp.close()
    f_int.close()
    return
        
        
def HeapAnalysis_QURT(inputfileName):
    intenal = inputfileName+"\\temp\\InternalLog.txt"
    f_int = open(intenal,'a')
    DSPAnlay = inputfileName+"\DSPAnalysis.txt"
    f_dsp = open(DSPAnlay,'a')
    DSP_QT = inputfileName+"\\Qurt_logs\\Heap_Analysis_QURT.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\n\nQURT:\n')
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionQURT\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField25)
                            err = 1
                        for line1 in f_qt:
                            if "Trying to find out next valid block" in line1:
                                break 
                            f_dsp.write(line1)
                    if "Corruption detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionQURT\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField25)
                            err = 1
                        f_dsp.write(" -----------------------------------------------------------------------------------------------------------------------------------------------------\n")
                        f_dsp.write(line)
                        for line1 in f_qt:
                            if "End:free BINs verification" in line1:
                                break 
                            f_dsp.write(line1)
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in QURT')
            if heapstatsFlag ==  1:
                CacheStatusAnalysis.UpdateDDField(inputfileName,DDField4)
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_QURT.txt\n')
        
    f_dsp.close()
    f_int.close()
    return
        
def HeapAnalysis_QURTISLAND(inputfileName):
    intenal = inputfileName+"\\temp\\InternalLog.txt"
    f_int = open(intenal,'a')
    DSPAnlay = inputfileName+"\DSPAnalysis.txt"
    f_dsp = open(DSPAnlay,'a')
    DSP_QT = inputfileName+"\\Qurt_logs\\Heap_Analysis_QURTISLAND.txt"
    heapstatsFlag = 0
    try:
        if os.path.exists(DSP_QT):
            f_dsp.write('\n\nQURTISLAND:\n')
            f_qt = open(DSP_QT, 'r+')
            err = 0
            for line in f_qt:
                if line != "\n": 
                    if "Heap Corruption Detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionQURTISLAND\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField26)
                            err = 1
                        for line1 in f_qt:
                            if "Trying to find out next valid block" in line1:
                                break 
                            f_dsp.write(line1)
                    if "Corruption detected" in line: 
                        heapstatsFlag = 1
                        if err == 0 :
                            f_dsp.write('\n\nMemoryCorruptionQURTISLAND\nHeap Corruption Detected\n')
                            CacheStatusAnalysis.UpdateDDField(inputfileName,DDField26)
                            err = 1
                        f_dsp.write(" -----------------------------------------------------------------------------------------------------------------------------------------------------\n")
                        f_dsp.write(line)
                        for line1 in f_qt:
                            if "End:free BINs verification" in line1:
                                break 
                            f_dsp.write(line1)
            
            f_qt.close()
            if heapstatsFlag ==  0:
                f_dsp.write('No Heap Corruption Detected in QURTISLAND')
            if heapstatsFlag ==  1:
                CacheStatusAnalysis.UpdateDDField(inputfileName,DDField4)
    except:
        f_int.write('ERROR: No Such file name Heap_Analysis_QURTISLAND.txt\n')

    f_dsp.close()
    f_int.close()
    return
        
        
def main():
    inputfileName=str(sys.argv[1])
    HeapAnalysis_GuestOS(inputfileName)
    HeapAnalysis_AUDIO(inputfileName)
    HeapAnalysis_Sensors(inputfileName)
    HeapAnalysis_QURT(inputfileName)
    HeapAnalysis_QURTISLAND(inputfileName)

if __name__ == '__main__':
    main()
