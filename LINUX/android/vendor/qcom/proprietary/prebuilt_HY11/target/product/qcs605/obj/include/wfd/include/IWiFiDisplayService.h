#ifndef _WFD_IWIFIDISPLAYSERVICE_H_
#define _WFD_IWIFIDISPLAYSERVICE_H_

/*==============================================================================
*       IWiFiDisplayService.h
*
*  DESCRIPTION:
*       Class declaration IWiFiDisplayService
*
*
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
11/06/2014                    InitialDraft
================================================================================
*/

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/

/* =============================================================================

                     INCLUDE FILES FOR MODULE

================================================================================
*/
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "WiFiDisplayServiceUtils.h"
#include <binder/IInterface.h>
#include <utils/StrongPointer.h>

struct WfdDevice;
class IWiFiDisplaySession;
class IWiFiDisplayListener;

namespace android{
    class Parcel;
}

class IWiFiDisplayService : public android::IInterface
{

public:

    DECLARE_META_INTERFACE(WiFiDisplayService);

    virtual android::sp<IWiFiDisplaySession> createWiFiDisplaySession (
        const android::sp<IWiFiDisplayListener>& listener,
        WfdDevice *localDevice,
        WfdDevice *peerDevice) = 0;

private:

    FORBID_CONSTRUCTORS(IWiFiDisplayService);

};

// ----------------------------------------------------------------------------

class BnWiFiDisplayService : public android::BnInterface<IWiFiDisplayService>
{

public:
    virtual int onTransact( uint32_t code,
                            const android::Parcel& data,
                            android::Parcel* reply,
                            uint32_t flags = 0);
};

#endif // _WFD_IWIFIDISPLAYSERVICE_H_
