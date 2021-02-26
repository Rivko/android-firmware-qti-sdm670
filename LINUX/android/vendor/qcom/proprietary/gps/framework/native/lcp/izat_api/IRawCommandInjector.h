/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IRawCommandInjector

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IRAWCOMMANDINJECTOR_H__
#define __IZAT_MANAGER_IRAWCOMMANDINJECTOR_H__

// TODO: Change namespace ?
namespace izat_manager {

class IRawCommandInjector {

public:

    virtual void inject (char * bundle, int bundleLen) = 0;

    virtual ~IRawCommandInjector () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IRAWCOMMANDINJECTOR_H__
