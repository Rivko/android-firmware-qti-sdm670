========================================================================
    CONSOLE APPLICATION : gen_rand Project Overview
========================================================================

AppWizard has created this gen_rand application for you.  
This file contains a summary of what you will find in each of the files that
make up your gen_rand application.


gen_rand.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

gen_rand.cpp
    This is the main application source file.

Notes:
	the Microsoft C++ library function rand_s uses RtlGenRandom and is recommended by 
	Microsoft for secure applications.

/////////////////////////////////////////////////////////////////////////////
Usage:
	:\>gen_rand size_bytes

Output:
	generates a file "rand_seed.dat"

/////////////////////////////////////////////////////////////////////////////
