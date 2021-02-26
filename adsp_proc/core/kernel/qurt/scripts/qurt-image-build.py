#! /usr/bin/python
#=============================================================================
#
#                                    qurt-image-build.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#
#  A wrapper that runs one of two version of qurt-image-build depending on the
#   XML setting in the main ELF file.
#
#  If you are looking for the actual implementation of qurt-image-build.py:
#    qurt-image-build0.py: for non-secure builds
#    qurt-image-build1.py: for secure builds
#

import os, sys
sys.path.append(os.path.dirname(__file__))
from lib.qurt import multiscript

#
#  Import the definitions in qurt-image-build0.py under module name QIB0
#  Import the definitions in qurt-image-build1.py under module name QIB1
#

multiscript_args = multiscript(globals(),
                               QIB0 = 'qurt-image-build0.py',
                               QIB1 = 'qurt-image-build1.py')

if multiscript_args:
    #
    #  We are being run as a script.
    #   multiscript_args is the value of sys.argv
    #
    from lib.qurt import run_script
    run_script(QIB0.QurtElfPatch().main)
else:
    #
    #  We are being imported
    #
    from QIB0 import *

