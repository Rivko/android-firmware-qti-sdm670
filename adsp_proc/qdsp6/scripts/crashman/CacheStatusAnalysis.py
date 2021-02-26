##################################################################################
# Filename : CacheStatusAnalysis.py
##################################################################################
#
# Pseudo Algorithm for Cache Flush Status
#
# if not( (flush state == 1) or (flush state == 2) or (flush state == 3) )
#   Update dspanalysis.txt with Cache Not Flushed , QURT System State with correponding macro
#   Update Summary.txt with Cache Not Flushed, QURT System State with correponding macro
# else
#   Update dspanalysis.txt that Cache Flushed
#
# if Qurt system state if not any valid value
#      Update dspanalysis.txt, Summary.txt with Bad Dumps 
#
# if SystemState = 0xc
#   Update dspanalysis.txt,Summary.txt with Power collapse message
# else if Not (SystemState == 0x1B or SystemState == 0xB or SystemState == 0xc)
#   Update dspanalysis.txt, Summary.txt with boot failure message, QURT System State with correponding macro
#   Update DD.txt with corresponding DDfield2
# else if SystemState = 0x1B
#   if flush state = 0
#       Update dspanalysis.txt, Summary.txt with " NMI received but not able to service. ADSP probably in hang state"
# else if SystemState = 0x8
#   Update dspanalysis.txt, Summary.txt with NMI or Dump Capturing Problem
# Proceed Further
#
##################################################################################
# Filename : CacheStatusAnalysis.py
##################################################################################
#
# Pseudo Algorithm for Cache Flush Status
#
# if flush state = 0
#   Update Files with Cache Not Flushed
# else
#   Update Files with Cache Flushed
#
# Update Files with QURT System State with correponding macro
#
# if SystemState = 0xc
#   Update Files with Power collapse message
# else if Not (SystemState = 0x1B or SystemState = 0xB or SystemState = 0xc)
#   Update Files with boot failure message
# else if SystemState = 0x1B
#   if flush state = 0
#       Update Files with NMI Not received Message
# else if SystemState = 0x8
#   Update Files with NMI or Dump Capturing Problem
# else
#   Bad Dumps 
# Proceed Further
##################################################################################
##################################################################################
# Filename : ErrorCauseAnalysis.py
##################################################################################
#
# Pseudo Algorithm
#
# DDFIELD1,DDFIELD6 :CALLSTACK,THREADNAME
# if "TCB HWID" in analyze_qurt_error.cmm
    # do loadtcm  TCB
    # Update DDField1(call stack)
    # Update DDFIeld6(ThreadName)
# else
    # v.f
    # Update Callstack
    # do stack walker .cmm 
    # Update Callstack
    # Update Threadname

# DDFIELD7: QURT_error_info
# if Cause == 12:
    # Dont Update
# else
    # Update Cause
    # if cause == ASSERT:
        # Update Cause2
    # Update DDFIeld

# DDFIELD5 : CoreDUMP check
# If Coredump exists:

    # if "USER PD crashed, see coredump.aux_msg for more details" exist
      #   MSG = aux_msg 
    # else 
      #   MSG =  message
	  
    # if "Exception detected" in that MSG:
        # Dont Update anything
    # else:
        # Update DSP Anlaysis
        # if check in LIST:
            # Update DD Field in DD.txt
# Proceed Further
##################################################################################

import os
import re
import subprocess
import sys
import time
import datetime
import fnmatch
import shutil, errno

DDField1  = "DDRunningCS" #Runnning Call Stack
DDField2  = "DDQurtSystemState" #Stack Walker Call Stack
DDField3  = "DDMemoryLeakGuestOS" 
DDField4  = "DDMemoryCorrupted"
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

DSPANALYSIS_FILE_NAME  = "DSPAnalysis.txt"
SUMMARY_FILE_NAME  = "Summary.txt"
DD_FILE_NAME  = "DD.txt"

QURT_SYSTEM_STATE = { \
"QURT_PREPARE_SINGLE_MODE      " : "   1 " ,  #Q6 is entered single threaded mode for power collapse) \
"QURT_PREPARE_END              " : "   2 " ,  #Q6 power collapse preparation is done \
"QURT_PERFORM_IPEND            " : "   3 " ,  #Q6 check IPEND register for interrupt \
"QURT_PERFORM_SAVE_ISDP        " : "   4 " ,  #Q6 power collapse - saving setting IPDS debug registers  \
"QURT_PERFORM_SAVE_PMU         " : "   5 " ,  #Q6 power collapse -  save PMU before entering power collapse   \
"QURT_PERFORM_SAVE_TLB         " : "   6 " ,  #Q6 power collapse -  save TLB entries table \
"QURT_PERFORM_SWITCH_PC        " : "   7 " ,  #Q6 is in performing Power collapse stage \
"QURT_PERFORM_EXIT             " : "   8 " ,  #Q6 is exiting power collapse) \
"QURT_FLUSH_L1CACHE            " : "   9 " ,  #Q6 flush L1 cache done \
"QURT_FLUSH_L2CACHE            " : "  10 " ,  #Q6 flush L2 cache done \
"QURT_FLUSH_CACHE_DONE         " : "  11 " ,  #Q6 flush cache done \
"QURT_SWITCH_PC_DONE           " : "  12 " ,  #Q6 is at end of performing Power collapse stage \
"QURT_BOOT_SETUP_ISDP          " : "  13 " ,  #Q6 bootup setup ISDP register \
"QURT_WBOOT_INIT_TLB           " : "  14 " ,  #Q6 bootup setup ISDP register \
"QURT_WBOOT_SET_1TO1_MAP       " : "  15 " ,  #Q6 bootup setup ISDP register \
"QURT_WBOOT_CFG_ADV_SYSCFG     " : "  16 " ,  #Q6 bootup setup ISDP register \
"QURT_WBOOT_REMOVE_1TO1_MAP    " : "  17 " ,  #Q6 bootup setup ISDP register \
"QURT_CBOOT_BSP_INIT           " : "  18 " ,  #Q6 bootup setup ISDP register \
"QURT_CBOOT_END_CLEAN_L1CACHE  " : "  19 " ,  #Q6 bootup setup ISDP register \
"QURT_CBOOT_END_CLEAN_INIT     " : "  20 " ,  #Q6 bootup setup ISDP register \
"QURT_CBOOT_END_OS_INIT        " : "  21 " ,  #Q6 bootup setup ISDP register \
"QURT_CBOOT_TLB_DUMP_LOAD      " : "  22 " ,  #Q6 cold boot - TLB dump load \
"QURT_CBOOT_TLB_STATIC_LOAD    " : "  23 " ,  #Q6 cold boot - TLB load \
"QURT_CBOOT_KERNEL_INIT_DONE   " : "  24 " ,  #Q6 cold boot - init kernel done \
"QURT_CBOOT_PLAT_CONFIG_DONE   " : "  25 " ,  #Q6 cold boot - plat config done \
"QURT_CBOOT_ROOT_TASK_STARTED  " : "  26 " ,  #Q6 cold boot - Root task start \
"QURT_IMPRECISE_EXCEPTION      " : "  27 " ,  #Q6 receives NMI exception and starts NMI handler \
"QURT_WBOOT_DEBUG_L2_START     " : "  28 " , \
"QURT_WBOOT_DEBUG_L2_END       " : "  29 " , \
"QURT_NMI_SAVE_L2VIC_COMPLETE  " : "  30 " , \
"QURT_NMI_HANDLER_COMPLETE     " : "  31 " , \
"QURT_NMI_AFTER_SAVE_GLOBAL    " : "  32 " , \
"QURT_WBOOT_START              " : "  33 " , \
"QURT_ENTER_ISLAND             " : "  34 " , \
"QURT_EXIT_ISLAND              " : "  35 " , \
"QURT_LOAD_NOTIFIER_TCB        " : "  36 " }

QURT_SYSTEM_STATE_DESCRIPTION = { \
"   1 " :  " #Q6 is entered single threaded mode for power collapse) " , \
"   2 " :  " #Q6 power collapse preparation is done " , \
"   3 " :  " #Q6 check IPEND register for interrupt " , \
"   4 " :  " #Q6 power collapse - saving setting IPDS debug registers  " , \
"   5 " :  " #Q6 power collapse -  save PMU before entering power collapse   " , \
"   6 " :  " #Q6 power collapse -  save TLB entries table " , \
"   7 " :  " #Q6 is in performing Power collapse stage " , \
"   8 " :  " #Q6 is exiting power collapse) " , \
"   9 " :  " #Q6 flush L1 cache done " , \
"  10 " :  " #Q6 flush L2 cache done " , \
"  11 " :  " #Q6 flush cache done " , \
"  12 " :  " #Q6 is at end of performing Power collapse stage " , \
"  13 " :  " #Q6 bootup setup ISDP register " , \
"  14 " :  " #Q6 bootup setup ISDP register " , \
"  15 " :  " #Q6 bootup setup ISDP register " , \
"  16 " :  " #Q6 bootup setup ISDP register " , \
"  17 " :  " #Q6 bootup setup ISDP register " , \
"  18 " :  " #Q6 bootup setup ISDP register " , \
"  19 " :  " #Q6 bootup setup ISDP register " , \
"  20 " :  " #Q6 bootup setup ISDP register " , \
"  21 " :  " #Q6 bootup setup ISDP register " , \
"  22 " :  " #Q6 cold boot - TLB dump load " , \
"  23 " :  " #Q6 cold boot - TLB load " , \
"  24 " :  " #Q6 cold boot - init kernel done " , \
"  25 " :  " #Q6 cold boot - plat config done " , \
"  26 " :  " #Q6 cold boot - Root task start " , \
"  27 " :  " #Q6 receives NMI exception and starts NMI handler " }

ERROR_CAUSE_LIST = { \
"   1" :  " - ERR_PRECISE, a precise exception occurred. ", \
"   2" :  " - ERR_NMI, an NMI occurred occurred." , \
"   3" :  " - ERR_TLBMISS, TLBMISS RW occurred." , \
"   4" :  " - ERR_RSVD_VECTOR, Interrupt was raised on reserved vector, should never happen." , \
"   5" :  " - ERR_ASSERT, Kernel Assert. See list of assert types below.   " , \
"   6" :  " - ERR_BADTRAP, trap0(#num) was called with unsupported num." , \
"   7" :  " - ERR_TRAP1, trap1 is not supported. Using Trap1 causes this error. " , \
"   8" :  " - ERR_EXIT, Application called qurt_exit(). Could be called from C library. " , \
"  10" :  " - ERR_TLBMISS_X, TLBMISS X (execution) occurred." , \
"  11" :  " - ERR_STOPPED, Running thread stopped due to Fatal error on other HW thread" , \
"  12" :  " - ERR_FATAL_EXIT, Application called qurt_fatal_exit()." , \
"  13" :  " - ERR_INVALID_INT, Kernel received an invalid L1 interrupt." , \
"  14" :  " - ERR_FLOATING_POINT, Kernel received an floating point error." }

ERROR_CAUSE2_LIST = { \
"ABORT_FUTEX_WAKE_MULTIPLE         " :"   1",#    // futex_asm.s: Abort cause - futex Wake multiple \
"ABORT_WAIT_WAKEUP_SINGLE_MODE     " :"   2",#    // power.c: Abort cause -Thread waiting to wake up in Single threaded  mode\
"ABORT_TCXO_SHUTDOWN_NOEXIT        " :"   3",#    // power.c : Abort cause - tcxo shutdown is call without exit\
"ABORT_FUTEX_ALLOC_QUEUE_FAIL      " :"   4",#    // futex.c: Abort cause - Futex aloc queue fail -  QURTK_futexhash_lifo empty\
"ABORT_INVALID_CALL_QURTK_WARM_INIT" :"   5",#    // init_asm.S: Abort cause - invalid Call QURTK_warm_init() in NONE CONFIG_POWER_MGMT mode\
"ABORT_THREAD_SCHEDULE_SANITY      " :"   6",#    // switch.S: Abort cause - Sanity schedule thread is not supposed to run on current hw thread \
"ABORT_REMAP                       " :"   7",#    //Remap in the page table. The right behavior should be always remove mapping if necessary\
"ABORT_NOMAP                       " :"   8",#    //No mapping in page table when remove a user mapping\
"ABORT_OUT_OF_SPACES               " :"   9",#\
"ABORT_INVALID_MEM_MAPPING_TYPE    " :"  10",#   //Invalid memory mapping type when creating qmemory\
"ABORT_NOPOOL                      " :"  11",#   //No pool available to attach \
"ABORT_LIFO_REMOVE_NON_EXIST_ITEM  " :"  12",#   //Can not allocate more futex waiting queue\
"ABORT_ARG_ERROR                   " :"  13",#\
"ABORT_ASSERT                      " :"  14",#   // Assert abort\
"ABORT_FATAL                       " :"  15",#   //FATAL error that shall never happens\
"ABORT_FUTEX_RESUME_INVALID_QUEUE  " :"  16",#   // futex_asm.s: Abort cause - invalid queue Id in futex resume\
"ABORT_FUTEX_WAIT_INVALID_QUEUE    " :"  17",#   // futex_asm.s: Abort cause - invalid queue Id in futex wait\
"ABORT_FUTEX_RESUME_INVALID_FUTEX  " :"  18",#   // futex.c: Abort cause - invalid futex object in hashtable\
"ABORT_NO_ERHNDLR                  " :"  19",#   // No registered Error handler\
"ABORT_ERR_REAPER                  " :"  20" }#   // Exception in Reaper thread itself


Neglect_coredump_error_list= [ 'BOUNDARY_CHECK_SECTIONS', 
                               'Exception Received',
                               'PID',
                               'forw_offset',
                               'INTEGRITY_CHECK_ON_USED_HEADER',
                               'APPS force stop'] 

def GetTCBPointer(OutputPath):
    qurt = OutputPath+"\\analy_qurt_error.txt"
    tcb  = OutputPath+"\\TCBPointer.txt"
    f_tcb = open(tcb,'a+')
    flag = 0
    try:
        if os.path.exists(qurt):
            f_qt = open(qurt, 'r+')
            no_error = 1
            for line in f_qt:
                if "TCB        HWTID  errorcode  pcycle" in line: 
                    no_error = 0
                    break
                if "error TCB " in line: 
                    no_error = 0
                    break
            f_qt.close()
            if no_error == 1:
                f_tcb.write("0\n")
                f_tcb.write("0\n")
                f_tcb.close()
                return
            
            f_qt = open(qurt, 'r+')
            flag = 0
            TCBPointer=None
            for line in f_qt:
                if "TCB        HWTID  errorcode  pcycle" in line: 
                    for line1 in f_qt:
                        TCBPointer = line1.split(' ')[0]
                        if "0x" in TCBPointer:
                            flag = 1
                            f_tcb.write(TCBPointer+"\n")
                            f_tcb.write("2"+"\n")
                        break
            f_qt.close()
            if flag == 0:
                f_qt = open(qurt, 'r+')
                for line in f_qt:
                    if "error TCB " in line:
                        TCBPointer = line.split(' ')[3].rstrip('\n')
                        f_tcb.write(TCBPointer+"\n")
                        f_tcb.write("2"+"\n")
                        break
                f_qt.close()
    except:
        print 'ERROR: No Such file name StackWalkerCallStack.txt\n'
    f_tcb.close()
    


def ErrorCause(Cause,Cause2,OutputPath):
    dspanal = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    sum = OutputPath+"\\temp\\"+SUMMARY_FILE_NAME
    f_dspanal = open(dspanal,'a+')
    f_sum = open(sum,'a+')

    if ( ( (Cause) > 0xe) or ( (Cause) < 0x1) or  ((Cause) == 12 ) ):
        pass
    elif ( (Cause) == 5 ):
        if ( ((Cause2) > 0x14 ) or ( (Cause2) < 0x1) ):
            pass
        else:
            for key in ERROR_CAUSE2_LIST:
                if (int(ERROR_CAUSE2_LIST[key])) == (Cause2):
                    f_dspanal.write('\nError Cause Info\n')
                    f_dspanal.write('================================\n')
                    f_dspanal.write("Kernel Assert (Cause2): "+str(Cause2)+" => "+key+"\n")
                    f_sum.write(str(Cause2)+" ==> "+key+"\n")
                    break 
            UpdateDDField(OutputPath,DDField7)
    else:
        for key in ERROR_CAUSE_LIST:
            if int(key) == (Cause):
                f_dspanal.write('\nError Cause Info\n')
                f_dspanal.write('=====================================\n')
                f_dspanal.write("Qurt Error Cause : "+str(Cause)+" => "+ERROR_CAUSE_LIST[key]+"\n")
                f_sum.write(str(Cause)+" ==> "+ERROR_CAUSE_LIST[key]+"\n")
                break
        UpdateDDField(OutputPath,DDField7)

    f_dspanal.close()
    f_sum.close()
    return
                    

def UpdateStackWalkerCallStack(OutputPath):
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    DSP_QT = OutputPath+"\\temp\\StackWalkerCallStack.txt"
    try:
        if os.path.exists(DSP_QT):
            f_sum.write("\n\nStack Walker CallStack\n")
            f_sum.write("=========================\n")
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if not "B::" in line: 
                    if line != "\n":
                        f_sum.write(line)
            f_sum.write("\n\n")
            UpdateDDField(OutputPath,DDField13)
            f_qt.close()
    except:
        print 'ERROR: No Such file name StackWalkerCallStack.txt\n'
    f_sum.close()
    return

def UpdateRunningCallStack(OutputPath):
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    DSP_QT = OutputPath+"\\temp\\Crashed_ThreadCallStack.txt"
    str_sch = "-000\*"
    for csno in range(1,1000):
        str_sch = str_sch+'|'+'-'+"{0:0=3d}".format(csno)+"\*"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            f_sum.write("\n\nRunning CallStack\n")
            f_sum.write("=========================\n")
            for line in f_qt:
                if not "B::" in line: 
                    if "****end of frame" in line:
                        f_sum.write("----|end of frame\n\n")
                        UpdateDDField(OutputPath,DDField1)
                        break;
                    y = re.search(str_sch,line)
                    if y:
                        cs = re.split('\(',str(line))[0]
                        cs = cs.replace('*', '|')
                        f_sum.write(cs+"()\n")
                    
            f_qt.close()
    except:
        print 'ERROR: No Such file name Crashed_ThreadCallStack.txt\n'
    f_sum.close()
    return

   
def UpdateFiles(f_dsp,flag):
    if flag == 0:
        f_dsp.write("ADSP is in Power Collapse Mode\n")
    if flag == 1:
        f_dsp.write("Supected ADSP Boot-Up Failure\n")
    if flag == 2:
        f_dsp.write("NMI Interrupt Might Not Have Been Received\n")
    if flag == 4:
        f_dsp.write("NMI Was Not Recived or Dump Capturing Problem\n")
    if flag == 5:
        f_dsp.write("Bad Dumps/Invalid Dumps\n")
        
    return    
 
# If Coredump exists:

    # if "USER PD crashed, see coredump.aux_msg for more details" exist
      #   MSG = aux_msg 
    # else 
      #   MSG =  message

    # if "Exception detected" in that MSG:
        # Dont Update anything
    # else:
        # Update DSP Anlaysis
        # if check in LIST:
            # Update DD Field in DD.txt
# Proceed Further
 
def UpdateCoreDumpInfo(OutputPath):
    dsp = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp = open(dsp,'a+')
    Coredmp = OutputPath+"\\temp\\SWFatal_Error.txt"
    try:
        if os.path.exists(Coredmp):
            f_Coredmp = open(Coredmp, 'r+')
            for line in f_Coredmp:
                if "err.message =" in line:
                    err_message = line
                    break
            for line in f_Coredmp:
                if "aux_msg =" in line:
                    aux_message = line
                    break
            
            if "USER PD crashed" in err_message:
                crash_message2 = aux_message
            else:
                crash_message2 = err_message
                
            crash_message2 = crash_message2.split(' = ')
            crash_message = crash_message2[1]
            if '""' in crash_message:
                f_dsp.close()
                f_Coredmp.close()
                return
            if "Exception detected" in crash_message:
                f_dsp.close()
                f_Coredmp.close()
                return
            if "ExIPC" in crash_message:
                f_dsp.close()
                f_Coredmp.close()
                return

            f_dsp.write('\nCoreDump Info\n')
            f_dsp.write('=====================================\n')
            #removing special characters from a coredump message
            crash_message = crash_message.replace('[',' ').replace(']',' ')
            f_dsp.write(crash_message+"\n")
             
            updateDDFiled= 1
            for msg in Neglect_coredump_error_list:
                if msg in crash_message:
                    updateDDFiled= 0
            if (updateDDFiled == 1 ):
                UpdateDDField(OutputPath,DDField5)
            f_Coredmp.close()
            f_dsp.close()
            return
    except:
        print 'ERROR: No Such file name SWFatal_Error.txt\n'
    return

def Error_Info(OutputPath):
    dsp = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp = open(dsp,'a+')
    DSP_QT = OutputPath+"\\temp\\get_error.txt"
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
        print 'ERROR: No Such file name get_error.txt\n'
    f_dsp.close() 

def Error_Info_Area(OutputPath):
    dsp = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp = open(dsp,'a+')
    DSP_QT = OutputPath+"\\temp\\analy_qurt_error.txt"
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
        print 'ERROR: No Such file name analy_qurt_error.txt\n'
    f_dsp.close()
def UpdateCrashSWThread(OutputPath,TCBPointer,Target_Name,slpi):
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    DSP_QT = OutputPath+"\\Qurt_logs\\DSP_QT.txt"
    try:
        if os.path.exists(DSP_QT):
            f_qt = open(DSP_QT, 'r+')
            for line in f_qt:
                if TCBPointer[2:] in line:  #removing 0X from pointer values and searching in line
                    QT = line.split()
                    crashedThread = QT[len(QT)-1]
                    f_sum.write('\nThread Name\n')
                    f_sum.write('=====================================\n')
                    if crashedThread == "RUNNING" or crashedThread == "WAITING":
                        f_sum.write("Crashed SW Thread     :   \n")
                    else:
                        f_sum.write("Crashed SW Thread     :  "+crashedThread+"\n")
                    con = 0
                    if os.path.isfile(OutputPath+"\\temp\\process_ids.txt"):
                        f_pro = open(OutputPath+"\\temp\\process_ids.txt","r+")
                        for line in f_pro:
                            if "Process_ID" in line:
                                temp = line.split(' Process_ID: ')[1].split(',')[0]
                                pd = "GuestOS"
                                if "frpc" in line:
                                    pd = "fastRPC"
                                elif "AUDIO" in line:
                                    pd = "AUDIO"
                                elif "Sensor" in line or "SENSOR" in line:
                                    pd = "Sensors"
                                if "0x"+QT[4] in temp or "0X"+QT[4] in temp:
                                    con = 1
                                    f_sum.write("Crashed PD            :  "+pd+"\n")
                                    break
                        f_pro.close()
                    if con == 0:
                        if (Target_Name=="8996" and slpi=="True") or (Target_Name=="8998" and slpi=="True") or (Target_Name=="845" and slpi=="True"):
                            if int(QT[4]) == 0:
                                f_sum.write("Crashed PD            :  GuestOS\n")
                            elif int(QT[4]) == 1:
                                f_sum.write("Crashed PD            :  Sensors\n")
                        elif Target_Name=="8996" or Target_Name=="8998":
                            if int(QT[4]) == 0:
                                f_sum.write("Crashed PD            :  GuestOS\n")
                            elif int(QT[4]) == 2:
                                f_sum.write("Crashed PD            :  fastRPC\n")
                            elif int(QT[4]) == 1:
                                f_sum.write("Crashed PD            :  AUDIO\n")
                        elif Target_Name=="8952" or Target_Name=="8953" or Target_Name=="8974" or Target_Name=="8x26" or Target_Name=="8992" or Target_Name=="8994" or Target_Name=="8976" or Target_Name=="8937":
                            if int(QT[4]) == 0:
                                f_sum.write("Crashed PD            :  GuestOS\n")
                            elif int(QT[4]) == 2:
                                f_sum.write("Crashed PD            :  fastRPC\n")
                            elif int(QT[4]) == 1:
                                f_sum.write("Crashed PD            :  Sensors\n")
                        else:
                            if int(QT[4]) == 0:
                                f_sum.write("Crashed PD            :  GuestOS\n")
                    UpdateDDField(OutputPath,DDField6)
                    break;
            f_qt.close()
    except:
        print 'ERROR: No Such file name DSP_QT.txt\n'
    f_sum.close()    
    return
        
def UpdateDDField(OutputPath,field):
    DD = OutputPath+"\\temp\\"+DD_FILE_NAME
    f_DD = open(DD,'a')
    f_DD.write(field+"\n")
    f_DD.close()
    return
    
def UpdateDDText(OutputPath,flag):
    DD = OutputPath+"\\temp\\"+DD_FILE_NAME
    f_DD = open(DD,'a')
    UpdateFiles(f_DD,flag)
    f_DD.close()
    return

    
def UpdateDSPAnalysis(OutputPath,flag):
    dsp = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp = open(dsp,'a+')
    f_dsp.write(flag+"\n")
    f_dsp.close()
    return
    

def UpdateSummary(OutputPath,flag):
    sum = OutputPath+"\\temp\\"+SUMMARY_FILE_NAME
    f_sum = open(sum,'a')
    f_sum.write(flag+"\n")
    f_sum.close()
    return
    
    
def QurtSytemState(OutputPath,QurtSystemState,CacheFlushFlag):

    if (((QurtSystemState) < 1) or ((QurtSystemState) > 36)):
        UpdateDSPAnalysis(OutputPath,"Invalid QURT System State.Probably dump capturing problem or ADSP not started")
        UpdateSummary(OutputPath,"Invalid QURT System State.Probably dump capturing problem or ADSP not started")  
    elif (QurtSystemState) == 0xC:
        UpdateDSPAnalysis(OutputPath,"ADSP is in Power Collapse Mode")
        UpdateSummary(OutputPath,"ADSP is in Power Collapse Mode")
    elif (QurtSystemState) == 0x8:
        if (CacheFlushFlag) == 0:
            UpdateDSPAnalysis(OutputPath,"This can occur when - NMI is not sent to ADSP or ADSP is not able to service NMI.\n\
IF \"NMI is not sent to ADSP\" : It is a Dump Capturing Problem.\n\
IF \"ADSP is not able to service NMI\" : ADSP is in Hang State.")
            UpdateSummary(OutputPath,"NMI is not sent to ADSP or ADSP is not able to service NMI.\n\
IF \"NMI is not sent to ADSP\" it is: It is a Dump Capturing Problem.\n\
IF \"ADSP is not able to service NMI\" : ADSP is in Hang State.")
    elif (QurtSystemState) == 0x1B:
        if (CacheFlushFlag) == 0:
            UpdateDSPAnalysis(OutputPath,"NMI Received But not able to Service. ADSP probably in Hang State.")
            UpdateSummary(OutputPath,"NMI Received But not able to Service. ADSP probably in Hang State.")
    elif (((QurtSystemState) != 0x1B) and ((QurtSystemState) != 0xB) and ((QurtSystemState) != 0xC) and ((QurtSystemState) != 0x1F)  and ((QurtSystemState) != 0x24)):
        UpdateDSPAnalysis(OutputPath,"Suspected ADSP Boot-Up Failure")
        UpdateSummary(OutputPath,"Suspected ADSP Boot-Up Failure")
        UpdateDDField(OutputPath,DDField2)
    return
    
def UpdateSystemStatusDescription(f_dsp,QurtSystemState):
    if str(QurtSystemState)!= "27" and str(QurtSystemState) != "11":
        for key in QURT_SYSTEM_STATE_DESCRIPTION:
            if (int(key)) == QurtSystemState:
                for val in QURT_SYSTEM_STATE:
                    if int(QURT_SYSTEM_STATE[val]) == int(key):
                        f_dsp.write(val+"==> "+QURT_SYSTEM_STATE_DESCRIPTION[key]+"\n")
    return

def UpdateSystemStatus(f_dsp,QurtSystemState):
    if str(QurtSystemState)!= "27" and str(QurtSystemState) != "11":
        f_dsp.write('\nQURT System State Status\n')
        f_dsp.write("=====================================\n")
        for key in QURT_SYSTEM_STATE:
            if (int(QURT_SYSTEM_STATE[key])) == QurtSystemState:
                f_dsp.write('QURT_system_state = '+str(QurtSystemState)+' ==> '+key+"\n")
    return

    
def UpdateSystemStateMacro(OutputPath,QurtSystemState):
    
    dsp = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp = open(dsp,'a+')
    UpdateSystemStatus(f_dsp,QurtSystemState)
    UpdateSystemStatusDescription(f_dsp,QurtSystemState)
    f_dsp.close()

    dsp = OutputPath+"\\temp\\"+SUMMARY_FILE_NAME
    f_dsp = open(dsp,'a')
    UpdateSystemStatus(f_dsp,QurtSystemState)
    UpdateSystemStatusDescription(f_dsp,QurtSystemState)
    f_dsp.close()
    return
    
def UpdateCacheStatus(f_dsp, CacheFlushFlag,check_FlushFlag):

    f_dsp.write('\nCache Status\n')
    f_dsp.write('=====================================\n')
    if CacheFlushFlag == 0x1 or CacheFlushFlag == 0x2 or CacheFlushFlag == 0x3:
        f_dsp.write("Cache Flush Status    : The DUMPS provided are Cache Flushed\n")
        if check_FlushFlag == "0x4":
            f_dsp.write("Dumps are Cache Flushed as SDI got triggerred. Note that if ADSP is in power collapse mode then dumps may not be reliable.\n")
    else:
        f_dsp.write("Cache Flush Status    : The DUMPS provided are NOT Cache Flushed\n")

    return

def CacheFlushStatus(OutputPath,CacheFlushFlag,check_FlushFlag):
    
    dsp1 = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp1 = open(dsp1,'a+')
    UpdateCacheStatus(f_dsp1,CacheFlushFlag,check_FlushFlag)
    f_dsp1.close()

    dsp = OutputPath+"\\temp\\"+SUMMARY_FILE_NAME
    f_dsp = open(dsp,'a')
    UpdateCacheStatus(f_dsp,CacheFlushFlag,check_FlushFlag)
    f_dsp.close()

    return
def CrashmanCommand(OutputPath,Target_Name,ramdumpfile,ADSP_BUILDROOT,Customer_Elf_Path,smmu64): 
    dsp1 = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_dsp1 = open(dsp1,'a+')
    if Customer_Elf_Path != None:
        f_dsp1.write("pushd \\\\rover\hyd_dspfw\ADSP_Tools\TriageTeam\Crashman\Latest\n")
        if not "False" in smmu64:
            f_dsp1.write("python adspcrashman.py -t "+Target_Name+" -d "+ramdumpfile+" -o C:\Temp -b "+ADSP_BUILDROOT+" -e "+Customer_Elf_Path+" -smmu64 "+smmu64+"\n\n")
        else:
            f_dsp1.write("python adspcrashman.py -t "+Target_Name+" -d "+ramdumpfile+" -o C:\Temp -b "+ADSP_BUILDROOT+" -e "+Customer_Elf_Path+"\n\n")
    else:
        f_dsp1.write("pushd \\\\rover\hyd_dspfw\ADSP_Tools\TriageTeam\Crashman\Latest\n")
        if not "False" in smmu64:
            f_dsp1.write("python adspcrashman.py -t "+Target_Name+" -d "+ramdumpfile+" -o C:\Temp -b "+ADSP_BUILDROOT+" -smmu64 "+smmu64+"\n\n")
        else:
            f_dsp1.write("python adspcrashman.py -t "+Target_Name+" -d "+ramdumpfile+" -o C:\Temp -b "+ADSP_BUILDROOT+"\n\n")
    
    f_dsp1.close()
    return
    
def StackOverflowCheck(OutputPath):
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    DSP_QT = OutputPath+"\\Qurt_Logs\\DSP_ST.txt"
    try:
        if os.path.exists(DSP_QT):
            dsp2 = OutputPath+"\\temp\\analy_qurt_error.txt"
            f_dsp2 = open(dsp2,'r+')
            cond = False
            for line in f_dsp2:
                if "-----------" in line:
                    f_sum.write('\n\nQurt Error Info\n')
                    f_sum.write('=====================================\n')
                    for line1 in f_dsp2:
                        if "---------" in line1:
                            cond = True
                            break
                        f_sum.write(line1+'\n')
                elif "Invalid Packet" in line:
                    f_sum.write('\n\nQurt Error Info\n')
                    f_sum.write('=====================================\n')
                    f_sum.write(line+'\n')
                    for line1 in f_dsp2:
                        if "---------------------------" in line1:
                            cond = True
                            break
                        f_sum.write(line1+'\n')
                elif "Imprecise Data abort" in line or "NMI Exception occurred" in line:
                    f_sum.write('\n\nQurt Error Info\n')
                    f_sum.write('=====================================\n')
                    f_sum.write(line+'\n')
                    break
                if cond == True:
                    break
            f_dsp2.close()
            f_sum.write("\n\nChecking Stack OverFlow\n")
            f_sum.write("=========================\n")
            f_qt = open(DSP_QT, 'r+')
            thread_name = None
            for line in f_qt:
                if not "B::" in line: 
                    if not "tcbp" in line:
                        if "*" in line:
                            temp = line.split('*')[2]
                            if not "unknown" in temp and not "128 /  128" in line:
                                try:
                                    thread_name = line.split('*')[6]
                                except:
                                    thread_name = line.split('*')[4]
            if thread_name == None or "dbgr_ex_loop" in thread_name or "dbgr_irq_poll_l" in thread_name or "uerr_ex" in thread_name or "000001B0" in thread_name or "kthread" in thread_name:
                f_sum.write("No Stack Overflow!!!")
            else:    
                f_sum.write("Stack Overflow in Thread "+str(thread_name.rstrip('\n'))+" !!!")
            f_sum.write("\n\n")
            f_qt.close()
    except:
        print 'ERROR: No Such file name StackWalkerCallStack.txt\n'
    f_sum.close()
    return
    
def FutexList(OutputPath):
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    if os.path.isfile(OutputPath+"\\Qurt_logs\\dsp_FT.txt"):
        file = open(OutputPath+"\\Qurt_logs\\dsp_FT.txt",'r')
        first_line = None
        second_line = None
        check = None
        for line in file:
            if "Futex_addr" in line:
                first_line = line
            elif "mutex" in line:
                second_line = line
                if not "AMBD" in second_line and not "fcv" in second_line:
                    if check == None:
                        f_sum.write("\n\nQurt Futex List\n")
                        f_sum.write("=========================\n")
                        f_sum.write(first_line)
                        check = first_line
                    f_sum.write(second_line)
        file.close()
    if first_line != None:
        f_sum.write("\n\n")
    f_sum.close()
            
def NMIException(OutputPath):
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    pcycles = None
    if os.path.isfile(OutputPath+"\\Qurt_logs\\dsp_MB.txt"):
        file = open(OutputPath+"\\Qurt_logs\\dsp_MB.txt",'r')
        for line in file:
            if not "B:" in line and not "PCYCLES" in line:
                print line
                pcycles = '0x'+line.split()[0]
                break
        file.close()
    pcycleshi = None
    pcycleslo = None
    if os.path.isfile(OutputPath+"\\Qurt_logs\\Qurt_Error_Info.txt"):
        file = open(OutputPath+"\\Qurt_logs\\Qurt_Error_Info.txt",'r')
        for line in file:
            if "pcyclehi" in line:
                pcycleshi = line.split('=')[1].split(',')[0]
            if "pcyclelo" in line:
                pcycleslo = line.split('=')[1].split(',')[0]
            if pcycleshi != None and pcycleslo != None:
                break
        file.close()
    khz = None
    cond = False
    for r,d,f in os.walk(OutputPath+"\\ulog"):
        for file in f:
            win_dump  = os.path.join(r,file)
            if "NPA" in win_dump and "1234.ulog" in win_dump:
                file = open(win_dump,'r')
                clk = None
                for line in file:
                    if 'npa_resource ' in line and '(name: "/clk/cpu")' in line:
                        clk = line
                if clk != None:
                    khz = clk.split('(active state: ')[1].split(')')[0]
                cond = True
                break
                file.close()
        if cond == True:
            break
    pcycleshilo = None
    if pcycleshi != None and pcycleslo != None:
        pcycleshilo = pcycleshi + pcycleslo.split('0x')[1]
    if khz != None:
        print pcycles,pcycleshilo,khz
    if int(pcycleshilo,16) == 0:
        return
    print (int(pcycleshilo,16) - int(pcycles,16))
    if float(float((int(pcycleshilo,16)) - float(int(pcycles,16)))/float(int(khz,16))) >= 2000.0:
        f_sum.write("\n\nNMI Exception\n")
        f_sum.write("=========================\n")
        f_sum.write("Difference in PCYCLES from MB and QURT_error_info is more than 2000ms.This Could be Watchdog Exception")
        f_sum.write("\n\n")
    f_sum.close()
    
def ICacheError(OutputPath):
    icache0  = sys.argv[3]
    if '0xFFFFFFFF' in sys.argv[4]:
        icache1  = "0x"+sys.argv[4].split('0xFFFFFFFF')[1]
    else:
        icache1  = sys.argv[4]
    if '0xFFFFFFFF' in sys.argv[5]:
        icache2  = "0x"+sys.argv[5].split('0xFFFFFFFF')[1]
    else:
        icache2  = sys.argv[5]
    if '0xFFFFFFFF' in sys.argv[6]:
        icache3  = "0x"+sys.argv[6].split('0xFFFFFFFF')[1]
    else:
        icache3  = sys.argv[6]
    if '0xFFFFFFFF' in sys.argv[7]:
        icache4  = "0x"+sys.argv[7].split('0xFFFFFFFF')[1]
    else:
        icache4  = sys.argv[7]
    if '0xFFFFFFFF' in sys.argv[8]:
        icache5  = "0x"+sys.argv[8].split('0xFFFFFFFF')[1]
    else:
        icache5  = sys.argv[8]
    if '0xFFFFFFFF' in sys.argv[9]:
        icache6  = "0x"+sys.argv[9].split('0xFFFFFFFF')[1]
    else:
        icache6  = sys.argv[9]
    if '0xFFFFFFFF' in sys.argv[10]:
        icache7  = "0x"+sys.argv[10].split('0xFFFFFFFF')[1]
    else:
        icache7  = sys.argv[10]
    if '0xFFFFFFFF' in sys.argv[11]:
        icache8  = "0x"+sys.argv[11].split('0xFFFFFFFF')[1]
    else:
        icache8  = sys.argv[11]
    sum = OutputPath+"\\"+DSPANALYSIS_FILE_NAME
    f_sum = open(sum,'a+')
    f_sum.write("\n\nQURTK_icache_error\n")
    f_sum.write("=========================\n")
    if icache1 != "0x0" and icache2 != "0x0":
        f_sum.write(icache1+' <--> '+icache2+'\n')
    if icache3 != "0x0" and icache4 != "0x0":
        f_sum.write(icache3+' <--> '+icache4+'\n')
    if icache5 != "0x0" and icache6 != "0x0":
        f_sum.write(icache5+' <--> '+icache6+'\n')
    if icache7 != "0x0" and icache8 != "0x0":
        f_sum.write(icache7+' <--> '+icache8+'\n')
    f_sum.write("\n\n")
    f_sum.close()
    
def main():
    CallFun      = sys.argv[1]
    if "ICacheError" in CallFun:
        OutputPath= sys.argv[2]
        ICacheError(OutputPath)
    if "NMIException" in CallFun:
        OutputPath= sys.argv[2]
        NMIException(OutputPath)
    if "FutexList" in CallFun:
        OutputPath= sys.argv[2]
        FutexList(OutputPath)
    if "GetTCBPointer" in CallFun:
        OutputPath= sys.argv[2]
        GetTCBPointer(OutputPath)
    if "StackOverflowCheck" in CallFun:
        OutputPath= sys.argv[2]
        StackOverflowCheck(OutputPath)
    if "UpdateRunningCallStack" in CallFun:
        OutputPath= sys.argv[2]
        UpdateRunningCallStack(OutputPath)
    if "UpdateStackWalkerCallStack" in CallFun:
        OutputPath= sys.argv[2]
        UpdateStackWalkerCallStack(OutputPath)
    if "CacheFlushStatus" in CallFun:
        OutputPath= sys.argv[2]
        CacheFlushFlag=sys.argv[3]
        check_FlushFlag=sys.argv[4]
        CacheFlushFlag= (int(CacheFlushFlag,16))
        CacheFlushStatus(OutputPath,CacheFlushFlag,check_FlushFlag)
    if "UpdateSystemStateMacro" in CallFun:
        OutputPath= sys.argv[2]
        QurtSystemState=sys.argv[3]
        QurtSystemState= (int(QurtSystemState,16))
        UpdateSystemStateMacro(OutputPath,QurtSystemState)
    if "QurtSytemState" in CallFun:
        OutputPath= sys.argv[2]
        QurtSystemState=sys.argv[3]
        CacheFlushFlag=sys.argv[4]
        CacheFlushFlag= (int(CacheFlushFlag,16))
        QurtSystemState= (int(QurtSystemState,16))
        QurtSytemState(OutputPath,QurtSystemState,CacheFlushFlag)
    if "UpdateCoreDumpInfo" in CallFun:
        OutputPath= sys.argv[2]
        UpdateCoreDumpInfo(OutputPath)
    if "Error_Info" in CallFun:
        OutputPath= sys.argv[2]
        Error_Info(OutputPath)
    if "Error_Info_Area" in CallFun:
        OutputPath= sys.argv[2]
        Error_Info_Area(OutputPath)
    if "UpdateCrashSWThread" in CallFun:
        OutputPath= sys.argv[2]
        TCBPointer= sys.argv[3]
        Target_Name=sys.argv[4]
        slpi=sys.argv[5]
        UpdateCrashSWThread(OutputPath,TCBPointer,Target_Name,slpi)
    if "ErrorCause" in CallFun:
        Cause= sys.argv[2]
        Cause2= sys.argv[3]
        OutputPath= sys.argv[4]
        Cause= (int(Cause,16))
        Cause2= (int(Cause2,16))
        ErrorCause(Cause,Cause2,OutputPath)
    if "CrashmanCommand" in CallFun:
        OutputPath= sys.argv[2]
        Target_Name= sys.argv[3]
        ramdumpfile= sys.argv[4]
        ADSP_BUILDROOT= sys.argv[5]
        smmu64= sys.argv[6]
        try:
            Customer_Elf_Path=sys.argv[7]
        except:
            Customer_Elf_Path=None
        CrashmanCommand(OutputPath,Target_Name,ramdumpfile,ADSP_BUILDROOT,Customer_Elf_Path,smmu64)
    return

if __name__ == '__main__':
  main()
