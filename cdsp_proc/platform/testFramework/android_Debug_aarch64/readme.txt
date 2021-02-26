Table Of Contents
########################################

.toc

libadsprpc
==================

libadsprpc is an application that allows executing Shared Object (SO) tests. 
This executable can be run through the Hexagon Simulator or as a standalone application in Android. 

libadsprpc - Parameters
==================

.  -config  <config_name>
.      <config_name> specifies all tests to be run.

.  -help
.      Prints a help message.

libadsprpc - config Parameter
==================

The path of the file for the -config paramter can be specified using absolute or relative paths.
See Config File Entries - Types of Path Supported. The same applies in the command line to specify 
the location of the config file.


libadsprpc - Running on Hexagon Simulator
==================

Using the Hexagon Simulator, libadsprpc can be run as follows:

.   adsptest-run.cmd --sim <path_to_libadsprpc> [-- -config <config_file>]

   If no config file is specified, libadsprpc looks for a default.cfg file in the same directory where libadsprpc resides in.


libadsprpc - Running on Android
==================

libadsprpc can be run as follows:

.   ./libadsprpc [-config <config_file>]

   If no config file is specified, libadsprpc looks for a default.cfg file in the same directoy where libadsprpc resides in.


Test Framework Config File
==================

A configuration file (.cfg) is used to specify the tests to run from a particular Shared Object (SO). Multiple
tests can be listed and each entry specifies one test to run from a particular SO.

Config File - Entries
==================

Entries in the configuration file must follow this format:

.ccode
   file://<path_to_foo.SO>?<test_name>

   path_to_foo.SO    This URI path (upto the ?) provides the path to the SO
   test_name         This is the test name of the function to call


Config File Entries - Types of Path Supported
==================

Absolute and relative paths are supported to specify the location of the test SO.


Config File Entries - Absolute Paths
==================

Absolute path to where the SO is located

   e.g.

.ccode

   file://D:/MyTests/Area.dll?get_area
   file://D:/MyTests/Area.dll?set_area


Config File Entries - Relative Paths
==================

Relative paths are relative to the location of the configuration file

e.g   ./ relative path

.ccode

   file://./MyTests/Area.dll?get_area

   If configuration file resides in
      
      D:\MyTests\ReleaseVersion

   The SO will be looked for in

      D:\MyTests\ReleaseVersion\.\MyTests

e.g   Current working directory

.ccode
   file://ReleaseVersion/MyTests/Area.dll?set_area

   If configuration file resides in
      
      D:\Proj\Proto\Release\Tests\area

   The SO will be looked for in

      D:\Proj\Proto\Release\Tests\area\ReleaseVersion\MyTests

e.g   ../ relative path

.ccode
   file://../ReleaseVersion/MyTests/Area.dll?set_area

   If configuration file resides in
      
      D:\Proj\Proto\Release\Tests\area

   The SO will be looked for in

      D:\Proj\Proto\Release\Tests\area\..\ReleaseVersion\MyTests

