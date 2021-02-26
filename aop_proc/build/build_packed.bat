@echo off

rem ==========================================================================
rem
rem RPM packed build launcher
rem
rem Copyright (c) 2017 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary and Confidential
rem ==========================================================================

cd ..
python build\packrat_bfam.py %*

