echo off
rem Script executes the specified tests in provided configuration file
rem
rem dsp_tfw_run.cmd  <adsp_proc_root> <cfg_file> <log_file>
rem
rem adsp_proc_root   -  This is the root directory of adsp_proc
rem cfg_file         -  This is the configuration file which contains the tests to execute
rem log_file         -  Filename to save output log to.

set FWK_LOCATION=%1\avs\aDSPSim

rem Copy TEST_PASS.xml and TEST_FAIL.xml to the provided locations
set CFG_FILE=%2
set LOG_FILE=%3

rem Move to aDSPSim folder
pushd %FWK_LOCATION%

rem Execute tests
set SIM_CMD=--sim
set FWK_CMD=..\..\platform\testFramework\WinNT_Debug_msvc9\libadsprpc.exe 
set CFG_CMD=-- -config ..\..\platform\testFramework\tests\WinNT_Debug\%CFG_FILE%
call adsptest-run.cmd %SIM_CMD% %FWK_CMD% %CFG_CMD% >> %LOG_FILE%

popd
set CWD=%~dp0
