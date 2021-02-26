#!/bin/bash
#******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************


#List of dependencies
LIBS=( libc++.so libqcrilFramework.so libcutils.so liblog.so libprotobuf-cpp-full.so libril-qc-hal-qmi.so libsettings.so libz-host.so libril-qc-ltedirectdisc.so librilutils.so libril-qc-radioconfig.so libicuuc-host.so libdiag.so)

#Cleanup existing binaries
if [ -e bin ]; then
    rm -rf bin
fi

#Create dir structure for executable
mkdir bin
mkdir bin/exe bin/lib64

#Copy exe
cp $ANDROID_HOST_OUT/bin/riltest ./bin/exe/

#Copy libraries
for lib in "${LIBS[@]}"; do
    cp $ANDROID_HOST_OUT/lib64/$lib ./bin/lib64/;
done

#Copy gcno coverage file 
files="$(find $ANDROID_HOST_OUT/obj/ | grep gcno)"
for file in $files; do
    cp $file ./bin/
done
export GCOV_PREFIX=./bin/
export GCOV_PREFIX_STRIP=123

#Run exe with valgrind
valgrind  --leak-check=full --track-origins=yes --show-reachable=yes --error-limit=no --log-file=./valgrind.log  --gen-suppressions=all  --suppressions=./valgrind.suppress  ./bin/exe/riltest --gtest_output=xml:report.xml "$1"
