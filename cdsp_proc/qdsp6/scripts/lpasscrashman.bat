@echo off
if not "%2"=="" (
set buildd=%2
goto allknown
)  
del dumpbuildid.txt
set sto=""
set stdir=""
set curdir=""
echo Parsing the dump file, please wait...
qdsp6-strings %1 > dumpstrings.txt
find "Q6_BUILD_TS" dumpstrings.txt > dumpbuild.txt
for /f "tokens=* delims= " %%a in (dumpbuild.txt) do (
call setset.bat %%a
)
for /f "tokens=* delims= " %%a in (dumpbuildid.txt) do set sto=%%a
if not %sto%=="" (
echo Timestamp from Memory Dump= %sto%
) else (
echo No Timestamp found
goto endall
)
set sto=%sto:~0,40%
rem echo "%sto%"
set sttt=const char timestampstring[] = "%sto%";
rem echo %sttt%
for /f "delims=" %%i in ('dir /ad /b /o-d') do (
if exist %%i\timestamp.h (
for /f "tokens=* delims= " %%k in (%%i\timestamp.h) do (
if "%sttt%"=="%%k%" (
set stdir=%%i
goto doneloop
)
)
)
)
:doneloop
rem echo %stdir%
popd
if not %stdir%=="" (
echo Match build is %stdir%
) else (
echo No build match
goto endall
)


:allknown
cd > currentdir.txt
for /f "tokens=* delims= " %%v in (currentdir.txt) do (
set curdir=%%v%
goto donedir
)
:donedir
echo current dir = %curdir%

set loadername=lds_d_%date:~4,2%_%date:~7,2%_%date:~10,4%_t_%time:~0,2%_%time:~3,2%_%time:~6,2%.cmm
set dirname=lds_d_%date:~4,2%_%date:~7,2%_%date:~10,4%_t_%time:~0,2%_%time:~3,2%_%time:~6,2%
mkdir %dirname%
echo script file=%loadername%
echo do "%curdir%\core_start.cmm" > %loadername%
echo SYSTEM.CPU HexagonV4C >> %loadername%
echo sys.u >> %loadername%
echo print "Dump File = %1" >> %loadername%
echo print "Build ID  = %buildd%" >> %loadername%
echo d.load.b %1 a:0x80000000 >> %loadername%
echo cd %buildd%\lpass_proc >> %loadername%
echo do %buildd%\lpass_proc\qdsp6\scripts\DSP_connectV4.cmm  0x8da00000 >> %loadername%
echo do %curdir%\storewind.cmm >> %loadername%
echo do %buildd%\lpass_proc\qurt\kernel\scripts\analy_qurt_error.cmm >> %loadername%


start C:\T32\t32mqdsp6.exe -c %curdir%\config_sim_usb.t32, "%curdir%\%loadername%" "%loadername%"

echo do recover_f3.cmm >> %loadername%

:endall

