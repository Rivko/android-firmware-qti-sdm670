/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#define LOG_TAG "OSFrameworkCleaner"
#define LOG_NDEBUG 0

#include <OSFrameworkCleaner.h>
#include <OsNpGlue.h>

using namespace std;
using namespace izat_manager;

OSFrameworkCleaner * OSFrameworkCleaner::mInstance = 0;

OSFrameworkCleaner * OSFrameworkCleaner :: getInstance () {

    if (!mInstance) {
        mInstance = new OSFrameworkCleaner ();
    }

    return mInstance;
}

void OSFrameworkCleaner :: destroyInstance () {
    delete mInstance;
    mInstance = NULL;
}

// ctor
OSFrameworkCleaner :: OSFrameworkCleaner () {}

OSFrameworkCleaner :: ~OSFrameworkCleaner () {}

// overrides
void OSFrameworkCleaner :: cleanOSNP () {
    OsNpGlue :: destroyInstance ();
}


