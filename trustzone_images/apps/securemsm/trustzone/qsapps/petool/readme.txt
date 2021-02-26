*****************************************************************
PETool Version 1.0
Qualcomm Technologies Inc.
*****************************************************************

This readme.txt file should be shipped along with following files:
 - PEToolLib.h
 - PETool
 - libpetool.so
 - Makefile
 - main.c

The above binaries are part of Provisioning Encryption tool that can be used to Encrypt a given blob of data


Linux:

main.c       - "Front-end" that opens the input files, calls the function in the lib with them, gets the output and writes that to a file

libpetool.so - Shared library used to perform the obfuscation

PETool       - Executable file 

Makefile     - Building the application

PEToolLib.h  - Contains the signature of the Encryption API 