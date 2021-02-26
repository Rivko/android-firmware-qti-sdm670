@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2011 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/aop.ho/1.1.c1/aop_proc/tools/build/scons/build/build.cmd#1 $
rem $DateTime: 2018/02/16 04:48:18 $
rem $Author: pwbldsvc $
rem $Change: 15485400 $
rem
rem ==========================================================================

setlocal
set startdate=%date%
set starttime=%time%

@echo Start Date=%startdate%, Time=%starttime% > build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # ENVIRONMENT BEGIN >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
set >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # ENVIRONMENT END >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt

@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # BUILD BEGIN >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo %~dp0..\SCons\scons %* >> build-log.txt
python %~dp0log_output.py build-log.txt -- %~dp0..\SCons\scons.bat %*
@set SCONS_ERRORLEVEL=%ERRORLEVEL%
@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # BUILD END >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
set endtime=%time%
set enddate=%date%
@echo.
@echo Start Date=%startdate%, Time=%starttime% - End Date=%enddate%, Time=%endtime% >> build-log.txt
@echo Start Date=%startdate%, Time=%starttime% - End Date=%enddate%, Time=%endtime%
@exit /B %SCONS_ERRORLEVEL%
