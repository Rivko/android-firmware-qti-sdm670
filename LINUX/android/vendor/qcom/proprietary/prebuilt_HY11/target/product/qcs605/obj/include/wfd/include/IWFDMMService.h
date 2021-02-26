#ifndef __IWFDSERVICE_H__
#define __IWFDSERVICE_H__

/*==============================================================================
*       IWFDMMService.h
*
*  DESCRIPTION:
*       WFD MM Service Binder Interface
*
*
*  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
06/09/2014         SK            InitialDraft
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

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include "IWFDMMListener.h"

using namespace android;

class IWFDMMService: public android::IInterface
{
public:

    enum
    {
        STREAMING_REQ = IBinder::FIRST_CALL_TRANSACTION,
        START_STREAM,
        WRITE_FRAME,
        RENDER_FRAME,
        PAUSE_STREAM,
        SET_WFD_LISTENER,
        WRITE_FRAME_FD,
        END_STREAM,
        CURRENT_DS_SUPPORT,
        FLUSH_STREAM,
        APP_INFO
    };

    DECLARE_META_INTERFACE(WFDMMService);

    virtual int isStreamingSuccess(int codecType,
                                   int profile,
                                   int level,
                                   int width,
                                   int height,
                                   bool bSecure,
                                   bool bConstrainedProfile) = 0;

    virtual int isStreamingFeatureEnabled() = 0;

    virtual int startStreaming() = 0;

    virtual int writeFrame(unsigned long nLen,
                           char* pData,
                           unsigned long long time_us) = 0;

    virtual int writeFrameFd(unsigned long nLen,
                             int fd,
                             unsigned long long time_us,
                             void* pData,
                             int nFlags,
                             bool bSecure) = 0;

    virtual int renderFrame(unsigned long long time_us) = 0;

    virtual int pauseStreaming() = 0;

    virtual int endStreaming() = 0;

    virtual int setWFDMMListener( bool bActive,
                              const sp<IWFDMMListener>&WfdMMServiceListener)= 0;
    virtual int flush() = 0;

    virtual int setAppInfo(const char *name, const char *operation) = 0;

    virtual int isAppWhiteListed() = 0;
};

class BpWFDMMService: public BpInterface<IWFDMMService>
{
public:
    BpWFDMMService(
                  const sp<IBinder>& wfdIf):BpInterface<IWFDMMService>(wfdIf){};

    virtual int isStreamingSuccess(int codecType,
                                   int profile,
                                   int level,
                                   int width,
                                   int height,
                                   bool bSecure,
                                   bool bConstrainedProfile);

    virtual int isStreamingFeatureEnabled();

    virtual int startStreaming();

    virtual int writeFrame(unsigned long nLen,
                           char* pData,
                           unsigned long long time_us);

    virtual int writeFrameFd(unsigned long nLen,
                             int fd,
                             unsigned long long time_us,
                             void* pData,
                             int nFlags,
                             bool bSecure);

    virtual int renderFrame(unsigned long long time_us);

    virtual int pauseStreaming();

    virtual int endStreaming();

    virtual int setWFDMMListener(bool bActive,
                                 const sp<IWFDMMListener>& WfdMMServiceListener);
    virtual int flush();

    virtual int setAppInfo(const char *name, const char *operation);

    virtual int isAppWhiteListed();

};

class BnWFDMMService: public BnInterface<IWFDMMService>
{
public:
    virtual int onTransact(uint32_t code,
                           const Parcel& data,
                           Parcel* reply,
                           uint32_t flags = 0);
};


#endif //__IWFDSERVICE_H__
