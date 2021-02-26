::****************************************************************
:: Filename: adspcrashman.bat
::
:: Version : External_Release_1.44
::
:: Usage:
:: 	adspcrashman.bat Target_Name[8994/8974/9x25/8x10/9x35/8084] EBI_Binary_File Output_folderpath CRM_Buildpath CustomerprovidedOBJPath[Optional]
::
:: Example:
:: 	adspcrashman.bat 8974 C:\Dropbox\8k_dumps\lpass_crash\dram_0x80000000--0xbfffffff.lst C:\dropbox\123456 C:\YUG_M8960AAAAANAZL1047\lpass_proc ::   C:\Dropbox\8k_dumps\lpass_proc\obj\qdsp6v4_ReleaseG
::
::****************************************************************

@echo off

set Crashman_Version= External_Release_1.44

echo ************************************
echo *******Crashman Started*************
echo ************************************
echo Crashman version          :%Crashman_Version%
set hour=%time:~0,2%
if "%time:~0,1%"==" " set hour=0%time:~1,1%
set datetimef=%date:~4,2%_%date:~-7,2%_%hour%h%time:~3,2%m%time:~6,2%s


if not "%1"=="" (
for /f "tokens=1,2 delims=_" %%A in ("%1") do (
set TARGET=%%A
)
goto argumentspresent
) else (
echo Usage: adspcrashman.bat Target_Name[8974/9x25/8x10] EBI_Binary_File Output_folderpath CRM_Buildpath CustomerprovidedOBJPath[Optional]
goto endall
)
:argumentspresent
set targetid=%TARGET%
set INDumpPath=%2%
set OutPath=%3\Crashman_%datetimef%
set CRM_buildpath=%4%
if exist %CRM_buildpath%\adsp_proc ( set CRM_buildpath=%CRM_buildpath%\adsp_proc )
set CustomerPath=%5%
set OUTDumpPath=%OutPath%
set OUTDumpPath=%OUTDumpPath%\Logs
mkdir %OUTDumpPath%
cd > %OUTDumpPath%\currentdir.txt

for /f "tokens=* delims= " %%v in (%OUTDumpPath%\currentdir.txt) do (
set curdir=%%v%
goto donedir
)
:donedir
set CurrDirectory=%curdir%
set BinaryLength=%6%
set adspstartadd=%7%
set choice1=%8%
set DumpCheckFlag=%9%
SHIFT
SHIFT
SHIFT
SHIFT
SHIFT
SHIFT
SHIFT
SHIFT
SHIFT
set SlpiFlag=%1%
SHIFT
set Qube_pa_guestos_click=%1%
SHIFT
set alignment=%1%
SHIFT
set cdsp_flag=%1%
echo %alignment% > %OUTDumpPath%\alignment.txt
echo Starting T32 for further processing.
echo ************************************
if exist C:\T32\bin\windows64 ( start C:\T32\bin\windows64\t32mqdsp6.exe -c %CurrDirectory%\config_sim_usb.t32, "%CurrDirectory%\DSP_load_memorydump_crashman_click.cmm" %targetid% %INDumpPath% %OUTDumpPath% %CRM_buildpath% %CurrDirectory% %CustomerPath% %BinaryLength% %adspstartadd% %choice1% %DumpCheckFlag% %SlpiFlag% %Qube_pa_guestos_click% %alignment% %cdsp_flag%
) else ( start C:\T32\t32mqdsp6.exe -c %CurrDirectory%\config_sim_usb.t32, "%CurrDirectory%\DSP_load_memorydump_crashman_click.cmm" %targetid% %INDumpPath% %OUTDumpPath% %CRM_buildpath% %CurrDirectory% %CustomerPath% %BinaryLength% %adspstartadd% %choice1% %DumpCheckFlag% %SlpiFlag% %Qube_pa_guestos_click% %alignment% %cdsp_flag%
)


:endall


 
