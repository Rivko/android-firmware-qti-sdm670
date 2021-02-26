/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_RAWCOMMANDINJECTOR_H__
#define __IZAT_MANAGER_RAWCOMMANDINJECTOR_H__

#include <IRawCommandInjector.h>
#include <ulp_service.h>

//TODO: Change namespace ?
namespace izat_manager {


class RawCommandInjector : public IRawCommandInjector {

public:

    static RawCommandInjector * getInstance ();
    static void destroyInstance ();

    // IRawCommandInjector overrides
    void inject (char * bundle, int bundleLen);

private:

    RawCommandInjector ();
    ~RawCommandInjector ();

    // Data members
    static RawCommandInjector * mInstance;
    const InjectRawCmdInterface * mInjectRawCmdInterface;
};

} // namespace izat_manager


#endif // #ifndef __IZAT_MANAGER_RAWCOMMANDINJECTOR_H__
