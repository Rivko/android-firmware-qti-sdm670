#!/bin/bash
#******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************

err_exit() {
    if [ "$1" ]; then
        echo -e 1>&2 "\nERROR: $1"
    fi
    exit 1
}

#Toch all files so that everything in qcril-hal is built again.
for file in `find .`; do touch $file; done

printf "\nRemove meta files"
rm -rf bin report.xml valgrind.log

#Remove exported headers from target build
printf "\nRemoved target includes"
rm -rf $ANDROID_BUILD_TOP/out/target/product/$TARGET_PRODUCT/obj/include/qcril/*

#Remove exported headers from host build
printf "\nRemoved host includes"
rm -rf $ANDROID_BUILD_TOP/out/host//linux-x86/obj/include/qcril/*

SHARED_LIBS=( libqcrilFramework libril-qc-hal-qmi )
STATIC_LIBS=( qcrilAndroidAudioModule qcrilQmiModule qcrilVoiceModule qcrilSampleModule qcrilLpaModule qcrilAmModule qcrilImsModule qcrilDataModule qcrilCellInfoModule qcrilUimModule qcrilNasModule qcrilPbmModule qcrilMbnModule)

for lib in "${SHARED_LIBS[@]}"; do
    host_shared="$ANDROID_HOST_OUT/obj/SHARED_LIBRARIES/$lib""_intermediates/"
    product_shared="$ANDROID_PRODUCT_OUT/obj/SHARED_LIBRARIES/$lib""_intermediates/"
    printf "\nRemoved host shared: $host_shared"
    rm -rf $host_shared
    if [ $? -ne 0 ]; then
        err_exit "    FAILED: removing $host_shared"
    fi

    printf "\nRemoved target shared: $product_shared"
    rm -rf $product_shared
    if [ $? -ne 0 ]; then
        err_exit "    FAILED: removing $product_shared"
    fi
done

for lib in "${STATIC_LIBS[@]}"; do
    host_static="$ANDROID_HOST_OUT/obj/STATIC_LIBRARIES/$lib""_intermediates/"
    product_static="$ANDROID_PRODUCT_OUT/obj/STATIC_LIBRARIES/$lib""_intermediates/"

    printf "\nRemoved host static: $host_static"
    rm -rf $host_static
    if [ $? -ne 0 ]; then
        err_exit "    FAILED: removing $host_static"
    fi
    printf "\nRemoved target static: $product_static"
    rm -rf $product_static
    if [ $? -ne 0 ]; then
        err_exit "    FAILED: removing $product_static"
    fi
done
#Remove host and target executable.
HOST_EXE=$ANDROID_HOST_OUT/bin/riltest
TARGET_EXE=$ANDROID_PRODUCT_OUT/system/bin/riltest
if [ -e $HOST_EXE ] ; then
    printf "\nRemoved host exe: $HOST_EXE"
    rm $HOST_EXE
fi
if [ -e $TARGET_EXE ] ; then
    printf "\nRemoved target exe: $TARGET_EXE"
    rm $TARGET_EXE
fi

