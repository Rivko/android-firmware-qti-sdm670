/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __OSFRAMEWORKCLEANER_H__
#define __OSFRAMEWORKCLEANER_H__

#include <IOSFrameworkCleaner.h>

using namespace izat_manager;

class OSFrameworkCleaner : public IOSFrameworkCleaner {

public:

    static OSFrameworkCleaner * getInstance ();
    static void destroyInstance ();

    // IOSFrameworkCleaner overrides
    void cleanOSNP ();

private:
     OSFrameworkCleaner ();
    ~OSFrameworkCleaner ();

    static OSFrameworkCleaner * mInstance;
};

#endif // #ifndef __OSFRAMEWORKCLEANER_H__

