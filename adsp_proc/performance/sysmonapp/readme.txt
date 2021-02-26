----------------------------
To view SysMon F3 messages :
----------------------------
QXDM configuration
- Options->Message View Config... (ctrl + F5)
- Enable: Known Messages (By Subsystem)->QDSP6->ADSP General->ADSP Resource Manager
- Press F3 to pop up the message view window

---------------------
Set up for sysMonApp :
---------------------

adb root
adb wait-for-device
adb remount

adb push <ADSP_CRM>/adsp_proc/performance/sysmonapp/sysMonApp /data/
adb shell chmod 777 /data/sysMonApp

-----------------------
Steps to run SysMonApp :
-----------------------

*********
PROFILER :
*********

adb shell
/data/sysMonApp profiler -help

example command:
/data/sysMonApp profiler --samplingPeriod 100

<note>: 
  For user configurable PMU events, place a pmu_events.txt file in /data folder with desired PMU events (max of 256 events) separated by spaces

****************************************
Extraction and Parsing of profiling data :
****************************************

One profiler is done, output file will be stored at /sdcard/sysmon.bin, pull the file and run parser on top this

example command:
adb pull /sdcard/sysmon.bin <output path>\<desired file name>.bin

************
PARSER USAGE:
************
SysmonParser.exe <Input file name> <Output file name> <ModeType> [--pmustats] [--dsat <Input file>]

<Input file name>: path to bin file extracted from the target (/sdcard/sysmon.bin)
<ModeType>: default <OR> user
    Select default if --defaultSetEnable 1 is given when you ran sysMonApp profiler
    Select user for --defaultSetEnable 0 or when you ran sysMonApp profiler without any options
--pmustats: This is Optional Argument
        Give this argument if you want to generate the .txt file which can be loaded onto "pmustats_spreadsheet"
        which resides at Hexagon LLVM tools\Examples\Profiling\pmustats_spreadsheet
--dsat <Input file>: This is an optional arugment
        Provide sysmon_QoS.bin file as input to --dsat option to extract DSAT logs
        This option can be used as a standalone option as well (output file will be an excel sheet with input file name) i.e, SysmonParser.exe --dsat <input file>
Example commands:
    SysmonParser.exe sysmon.bin SysmonProfStat default
    SysmonParser.exe sysmon.bin SysmonProfStat user --pmustats
    SysmonParser.exe c:\temp\sysmon.bin c:\temp\SysmonProfStat user
    SysmonParser.exe sysmon.bin SysmonProfStat default --dsat sysmon_QoS.bin

*********************
THREAD LEVEL PROFILER :
*********************
adb shell
/data/sysMonApp tlp -help

example command:
If user want to execute TLP alone.
	/data/sysMonApp tlp --samplingPeriod 10
	After the profiling, output is stoted @/sdcard/sysmontlp.bin or /data/sysmontlp.bin. Pull the file and run the parser on top of it.
	adb pull /sdcard/sysmontlp.bin
	SysmonParser.exe --tlp sysmontlp.bin
	If profiling is done for SLPI name of the bin will sysmontlp_sdsp.bin.
If user want to execute the profiler and thread level profiler.
	/data/sysMonApp tlp --samplingPeriod 10 --profile 1
	After the profiling, output is stoted @/sdcard/sysmontlp.bin or /data/sysmontlp.bin and /sdcard/sysmon.bin or /data/sysmon.bin.
	Pull the file and run the parser on top of it.
	adb pull /sdcard/sysmontlp.bin
	adb pull /sdcard/sysmon.bin
	SysmonParser.exe --tlp sysmon.bin sysmonStats user --tlp sysmontlp.bin
	If profiling is done for SLPI name of the bin will sysmon_sdsp.bin and sysmontlp_sdsp.bin.
If user want to profile selected SW threads.
	/data/sysMonApp tlp --samplingPeriod 10 --profile 1 --tname AfeDy4000,MXAR
	Only comma should be provided.


***********
PROFILEROPT :
***********

adb shell
/data/sysMonApp profileropt -help

example command:
/data/sysMonApp profileropt --samplingPeriod 100

<notes>: 
  User needs to push libsysmonapps_skel.so file from <ADSP_CRM>/adsp_proc/performance/sysmonapp/ to /system/lib using ADB shell for this option to work.
  For user configurable PMU events, place a pmu_events.txt file in /data folder with desired PMU events (max of 256 events) separated by spaces
  
*********
SETTINGS :
*********
adb shell
/data/sysMonApp settings -help

example commands:
<For 8996 only>
  /data/sysMonApp settings --cache_lock 128
  /data/sysMonApp settings --cache_unlock
  /data/sysMonApp --client_classes
  /data/sysMonApp --hvx_lock 64
  /data/sysMonApp --hvx_unlock
  /data/sysMonApp --danger 0 to clear, 1 for generating a danger

*************
STRESS TESTS :
*************
adb shell
/data/sysMonApp stressTest -help

example commands:
/data/sysMonApp stressTest --testCase 1 --nthreads 2 --iterations 1000
/data/sysMonApp stressTest --testCase 1 --swFetchLines 100
/data/sysMonApp stressTest --busClock 0 --coreClock 0 --clientClass 4
/data/sysMonApp stressTest --testCase 1 --nthreads 2 --iterations 1000 --busClock 0 --coreClock 0
/data/sysMonApp stressTest --testCase 1 --nthreads 1 --iterations 1000 --busClock 200 --coreClock 100

********
GETSTATE :
********
adb shell
/data/sysMonApp getstate

********************
DCVS ENABLE/DISABLE :
********************
adb shell
/data/sysMonApp dcvs -help

example commands:
/data/sysMonApp dcvs enable
/data/sysMonApp dcvs disable

**************************************
SYSMON PROFILER SERVICE ENABLE/DISABLE :
**************************************
adb shell
/data/sysMonApp prof -help

example commands:
/data/sysMonApp prof enable
/data/sysMonApp prof disable

*******
CLOCKS :
*******
adb shell
/data/sysMonApp clocks -help

example commands:
/data/sysMonApp clocks set --coreClock 200 --busClock 100 --ahbClock 19 --sleepLatency 10
/data/sysMonApp clocks set --sleepLatency 10
/data/sysMonApp clocks remove

******
DSAT :
******
adb shell
/data/sysMonApp DSAT

example commands:
/data/sysMonApp DSAT --asid 1

Note: Once finished, output DSAT log file can be pulled from /sdcard/sysmon_qos.bin

**************
PROFILER_DEBUG:
**************
## This is for debug purpose only. To override sampling period of sysmon profiler timer without enabling 
## logging i.e, without saving the output in /sdcard/sysmon.bin file. 
adb shell
/data/sysMonApp profiler_debug -help

example commands:
/data/sysMonApp profiler_debug enable
/data/sysMonApp profiler_debug disable


****************
TROUBLE SHOOTING:
****************
## Working on engineering ADSP image / side loading another ADSP image which is not part of META ##
> If you load an engineering ADSP image or change the ADSP image other than the one loaded as part of meta, follow below steps to load dependencies
    adb root
	adb remount
	adb shell mkdir /system/lib/rfsa/adsp/
	adb push <ADSP Image>\adsp_proc\build\dynamic_signed\shared_obj\libsysmon_skel.so /system/lib/rfsa/adsp/
	adb push <ADSP Image>\adsp_proc\build\dynamic_signed\shared_obj\fastrpc_shell_0 /system/lib/rfsa/adsp/
	adb shell sync
	adb reboot
## IF SysMonApp hangs or returns '-1' ##
> Check if adsprpcd daemon is running. 
    adb shell ps | grep adsprpcd
> If there is no adsprpcd process in the PS command, run the following after every reboot
    adb root
	adb remount
	adb shell
	> /system/bin/adsprpcd &
> The above commands will start the adsprpcd process required for sysMonApp to communicate with ADSP sysmon server using FastRPC.
> In case the FastRPC is still hanging/crashing/returning -1, try renaming fastrpc_shell_0 objects
	adb shell mv /dsp/fastrpc_shell_0 /dsp/fastrpc_shell_1
	adb shell mv /system/lib/rfsa/adsp/fastrpc_shell_0 /system/lib/rfsa/adsp/fastrpc_shell_1
	
	Note: Renaming fastrpc_shell_0 object may have side effect on other applications which use fastRPC.
