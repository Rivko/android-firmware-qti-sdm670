@ECHO OFF
set LOG_DIR1=aop_proc\core\power\ddr_mgr\scripts
set LOG_DIR2=aop_proc\core\power\utils\scripts

if exist %1\MSGRAM7.BIN perl %LOG_DIR2%\wa_log.pl %1\MSGRAM7.BIN
if exist %1\MSGRAM6.BIN perl %LOG_DIR1%\ddr_log.pl %1\MSGRAM6.BIN
if exist %1\MSGRAM11.BIN perl %LOG_DIR1%\ddr_stats.pl %1\MSGRAM11.BIN
if exist %1\DATARAM.BIN perl %LOG_DIR1%\parse_dataram.pl %1\DATARAM.BIN
if exist %1\CODERAM.BIN perl %LOG_DIR1%\parse_aop_messages.pl %1\CODERAM.BIN
if exist %1\MSGRAM13.BIN perl %LOG_DIR1%\memdump13.pl %1\MSGRAM13.BIN
if exist %1\MSGRAM15.BIN perl %LOG_DIR1%\memdump15.pl %1\MSGRAM15.BIN

if exist %1\aop_msg7_0xc370000--0xc3703ff.lst perl %LOG_DIR2%\wa_log.pl  %1\aop_msg7_0xc370000--0xc3703ff.lst
if exist %1\aop_msg6_0xc360000--0xc3603ff.lst perl %LOG_DIR1%\ddr_log.pl %1\aop_msg6_0xc360000--0xc3603ff.lst
if exist %1\aop_msg11_0xc3b0000--0xc3b03ff.lst perl %LOG_DIR1%\ddr_stats.pl %1\aop_msg11_0xc3b0000--0xc3b03ff.lst
if exist %1\aop_data_0xb0e0000--0xb0e7fff.lst perl %LOG_DIR1%\parse_dataram.pl %1\aop_data_0xb0e0000--0xb0e7fff.lst
if exist %1\aop_code_0xb000000--0xb017fff.lst perl %LOG_DIR1%\parse_aop_messages.pl %1\aop_code_0xb000000--0xb017fff.lst
if exist %1\aop_msg13_0xc3d0000--0xc3d03ff.lst perl %LOG_DIR1%\memdump13.pl %1\aop_msg13_0xc3d0000--0xc3d03ff.lst
if exist %1\aop_msg15_0xc3f0000--0xc3f03ff.lst perl %LOG_DIR1%\memdump15.pl %1\aop_msg15_0xc3f0000--0xc3f03ff.lst

if exist %1\SHRM_MEM.BIN perl %LOG_DIR1%\memdump_shrm.pl %1
if exist %1\SHRM_MEM.BIN.ls_0x13e0000--0x13effff.lst  perl %LOG_DIR1%\memdump_shrm.pl %1
