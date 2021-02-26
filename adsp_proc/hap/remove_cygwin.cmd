@set path=%path:C:\utils\cygwin;=%
@set path=%path:C:\utils\cygwin\bin;=%
@set path=%path:C:\utils\cygwin64;=%
@set path=%path:C:\utils\cygwin64\bin;=%
@set path=%path:C:\cygwin;=%
@set path=%path:C:\cygwin\bin;=%
@set path=%path:C:\Utils\Misc;=%
@set path=%path:C:\Utils;=%
@set path=%path:C:\Program Files (x86)\Git\bin;=%
@set path=%path:\\sun\crmtools\smart\nt\bin;=%
@set path=%path:\\stone\aswcrm\smart\nt\bin;=%
@set path=%path:\\qctdfsrt\prj\qct\asw\StaticAnalysis\Windows\Klocwork\Server\bin;=%
@set path=%path:\\qctdfsrt\prj\qct\asw\StaticAnalysis\Windows\Klocwork\User\bin;=%
@set path=%path:C:\perl\bin;=%
@set path=%path:C:\CRMApps\apps\Perl\bin;=%
rem @set path=%path:c:\crmtools\smart\nt\bin;=%
rem @set path=%path:c:\apps\ecloud\i686_win32\bin;=%
rem @set path=%path:%C:\crmapps\apps\mks70\bin;=%

rem @set path=%path:%C:\crmapps\apps\mks70\bin\X11;=%
rem @set path=%path:%C:\crmapps\apps\mks70\mksnt;=%
rem @set path=%path:%C:\apps\rvds30\bin\win_32-pentium;=%

rem @set path=%path:%C:\Program Files\HP\NCU;=%
rem @set path=%path:%C:\Program Files\Electric Cloud\ElectricCommander\bin;=%
rem @set path=%path:%C:\Apps\McCabeIQ\bin;=%
rem @set path=%path:%C:\Program Files (x86)\Windows Imaging;=%
rem @set path=%path:%C:\Program Files\System Center Operations Manager 2007;=%
rem @set path=%path:%C:\Program Files\Electric Cloud\ElectricCommander\bin;=%
@path > path_with_cygwin_removed.txt
