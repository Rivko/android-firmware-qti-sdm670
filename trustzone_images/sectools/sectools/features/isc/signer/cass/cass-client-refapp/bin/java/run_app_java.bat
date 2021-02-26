@echo off

REM Enable the section below to switch java runtimes when the ref app is launched
REM echo Setting JAVA_HOME
REM set JAVA_HOME=C:\Program Files\Java\jdk1.6.0_11
REM echo setting PATH
REM set PATH=C:\Program Files\Java\jdk1.6.0_11\bin;%PATH%
REM echo Display java version
REM java -version

if "%1"=="" (echo Usage: run_app_java.bat [-version] ^<Signing Package Path^> [-v] [^<app.properties Key^> ^<app.properties value^>] & goto end)
set program_arguments=%1
shift
if "%1"=="-v" ( set program_arguments=%program_arguments% %1 & shift)

set override_app_properties=-D%1=%2

:loop
if "%1"=="" goto endloop
set override_app_properties=%override_app_properties% -D%1=%2
shift
goto loop

:endloop
java %override_app_properties% -jar cass-client-refapp %program_arguments%


:end