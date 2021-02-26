set str=T:%~p0
echo.%str% 
set str=%str:~0,-1% 
echo.%str% 

  
cd %~p0QcomPkg 
\\qctdfsrt\prj\qct\asw\StaticAnalysis\Windows\Klocwork\User\bin\kwinject.exe -T tracefile.trace b.bat 
sagenconfig -r %str% -t trace -f QcomPkg\tracefile.trace 
ectool --server qctecommander runProcedure qcom.qct.sqm.klocwork --procedureName SABUILD --actualParameter MANIFESTFILE=static_analysis\sa_manifest.txt PROJECTDIR=\\corebsp-bl-02\pkg\builds\timely\180\1129 PROJECTNAME=ENG_UEFIKLOC PROJECTROOT="" SARESOURCEPOOL=KW_Prod
