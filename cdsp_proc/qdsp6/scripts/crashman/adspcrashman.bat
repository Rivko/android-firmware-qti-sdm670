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

echo **************************************************************************************
echo adspcrashman.bat is deprecated. Please use python dynamic argument model for crashman
echo **************************************************************************************
python adspcrashman.py -help

