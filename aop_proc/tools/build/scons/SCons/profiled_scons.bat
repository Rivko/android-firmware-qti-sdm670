@REM Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 The SCons Foundation
@REM src/script/scons.bat 3842 2008/12/20 22:59:52 scons
@echo off
set SCONS_ERRORLEVEL=
if "%OS%" == "Windows_NT" goto WinNT

@REM for 9x/Me you better not have more than 9 args
python -c "from os.path import join; import sys; sys.path = [ join(abspath('../../bsp/tools'))] + sys.path; import SCons.Script; SCons.Script.main()" %1 %2 %3 %4 %5 %6 %7 %8 %9
@REM no way to set exit status of this script for 9x/Me
goto endscons

@REM Credit where credit is due:  we return the exit code despite our
@REM use of setlocal+endlocal using a technique from Bear's Journal:
@REM http://code-bear.com/bearlog/2007/06/01/getting-the-exit-code-from-a-batch-file-that-is-run-from-a-python-program/

:WinNT
setlocal
@REM ensure the script will be executed with the Python it was installed for
set path=%~dp0;%~dp0..;%path%

@REM this file is run from tools/build/scons/SCons, internal builds include python in the core/bsp/tools/python
set COREBSP_PYTHON=%~dp0..\..\..\..\core\bsp\tools\python\
set CRM_PYTHON=C:\CRMApps\Apps\Python262\
set CRM_PYTHON_OPT=C:\Apps\Python262\

set SCONS_ROOT=%~dp0..

set PYTHONPATH=
IF EXIST %CRM_PYTHON% SET PYTHONPATH=%CRM_PYTHON%
IF EXIST %CRM_PYTHON_OPT% SET PYTHONPATH=%CRM_PYTHON_OPT%
IF EXIST %COREBSP_PYTHON% SET PYTHONPATH=%COREBSP_PYTHON%

set PYTHONCMD=%PYTHONPATH%python

%PYTHONCMD% -m cProfile -o buildstats do_scons.py %*
endlocal & set SCONS_ERRORLEVEL=%ERRORLEVEL%

if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto returncode
if errorlevel 9009 echo you do not have python in your PATH
goto endscons

:returncode
exit /B %SCONS_ERRORLEVEL%

:endscons
call :returncode %SCONS_ERRORLEVEL%
