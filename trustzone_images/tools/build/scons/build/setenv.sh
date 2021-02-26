#!/bin/bash
#===============================================================================
#
# Set Environment Script for Linux
#
# General Description
#    Setup Qualcomm-specific environment variables for the build system.
#
# Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/scons.tz/2.0.2/build/scons/build/setenv.sh#1 $
# $DateTime: 2019/02/07 03:06:10 $
# $Change: 18261931 $
#
#===============================================================================
unamestr=`uname`
if [ "$unamestr" = "Linux" ]; then
   
   # set up local environment
   export_armlmd_license()
   {
     # know where the host is located
     mdb $(echo `hostname`) return site > __temp.out

     # in boulder?
     grep -q "site: boulder" __temp.out
     if [ $? -eq 0 ]
     then
       echo "in boulder"
       export ARMLMD_LICENSE_FILE=8224@redcloud:8224@swiftly:7117@license-wan-arm1
     else
       # in rtp?
       grep -q "site: rtp" __temp.out
       if [ $? -eq 0 ]
       then
         echo "in rtp"
         export ARMLMD_LICENSE_FILE=8224@license-wan-rtp1
       else
         # in hyderabad?
         grep -q "site: hyderabad" __temp.out
         if [ $? -eq 0 ]
         then
           echo "in hyderabad"
           export ARMLMD_LICENSE_FILE=7117@license-hyd1:7117@license-hyd2:7117@license-hyd3
         else
           # in sandiego and others
           echo "in sandiego"
           export ARMLMD_LICENSE_FILE=7117@license-wan-arm1
         fi
       fi
     fi

     rm -f __temp.out
   }

   ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/2.2BLD593/RVCT/Programs/2.2/593/linux-pentium
   PYTHON_PATH=/pkg/qct/software/python/2.6.6/bin
   MAKE_PATH=/pkg/gnu/make/3.81/bin
   export ARMTOOLS=RVCT221
   export ARMROOT=/pkg/qct/software/arm/RVDS/2.2BLD593
   export ARMLIB=$ARMROOT/RVCT/Data/2.2/349/lib
   export ARMINCLUDE=$ARMROOT/RVCT/Data/2.2/349/include/unix
   export ARMINC=$ARMINCLUDE
   export ARMCONF=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMDLL=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMBIN=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export PATH=$MAKE_PATH:$PYTHON_PATH:$ARM_COMPILER_PATH:$PATH
   export ARMHOME=$ARMROOT
   export_armlmd_license

   # Hexagon Setup
   export HEXAGON_ROOT=/pkg/qct/software/hexagon/releases/tools
fi

#==============================================================================
# Klocwork
#==============================================================================
export BUILDSPEC=KLOCWORK

#==============================================================================
# Dump environment to stdout so that calling scripts can read it.
#==============================================================================
env

