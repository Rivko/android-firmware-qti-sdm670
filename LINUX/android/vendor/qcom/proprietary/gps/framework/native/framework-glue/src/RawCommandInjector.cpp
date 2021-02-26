/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <RawCommandInjector.h>
#include <ulp_service.h>

#define LOG_TAG "RawCommandInjector"
#define LOG_NDEBUG 0


// TODO: change namespace ?
using namespace izat_manager;

RawCommandInjector * RawCommandInjector::mInstance = NULL;

RawCommandInjector * RawCommandInjector :: getInstance () {

    if (!mInstance) {
        mInstance = new RawCommandInjector ();
    }
    return mInstance;
}

void RawCommandInjector :: destroyInstance () {
    if (mInstance) {
        delete mInstance;
        mInstance = NULL;
    }
}

// ctor
RawCommandInjector :: RawCommandInjector ()
:
mInjectRawCmdInterface
( (const InjectRawCmdInterface*) ulp_get_extension (ULP_RAW_CMD_INTERFACE) )
{}

// dtor
RawCommandInjector :: ~RawCommandInjector () {}

// overrides
void RawCommandInjector :: inject
(
    char * bundle,
    int bundleLen
)
{
    if (mInjectRawCmdInterface != NULL) {
        mInjectRawCmdInterface->inject_raw_cmd (bundle, bundleLen);
    }
}


