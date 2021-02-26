#ifndef __WFDSERVICE_CLIENT_H__
#define __WFDSERVICE_CLIENT_H__

/*==============================================================================
*       WFDMMServiceClient.h
*
*  DESCRIPTION:
*       WFDMMServiceClient class
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

#include "IWFDMMService.h"
#include "IWFDMMListener.h"

class WFDMMServiceClient: public BnWFDMMListener
{
public:

    typedef void (*NotifyCbType)(void* AppCtx,
                                 StreamingEventType event,
                                 void* PvtData);

    WFDMMServiceClient(void* pAppContext, NotifyCbType pCb);

    ~WFDMMServiceClient();

    int isStreamingSupported(int codec,
                             int profile,
                             int level,
                             int width,
                             int height,
                             bool bSecure,
                             bool bConstrainedProfile);

    static bool isStreamingFeatureEnabled();

    int startStreaming();

    int writeFrame(unsigned long nLen,
                   char* pData,
                   unsigned long long time_us);

    int writeFrameFd(unsigned long nLen,
                     int Fd,
                     unsigned long long time_us,
                     void* pData,
                     int nFlags,
                     bool bSecure);

    int renderFrame(unsigned long long time_us);

    int pauseStreaming();

    int endStreaming();

    static bool isSessionActive();

    static bool isAppWhiteListed();

    WFDMMServiceClient* setWfdMMListener(bool flag);

    virtual int notify(int event,
               void* PvtData);

    int flush();

    void setAppInfo(const char* name,const char* operation);

    class WFDMMServiceDeathRecepient: public IBinder::DeathRecipient
    {
    public:
        WFDMMServiceDeathRecepient(){;};
        virtual ~WFDMMServiceDeathRecepient();
        virtual void binderDied(const wp<IBinder>& who);
        WFDMMServiceClient* pMeClient;
    };


private:
    static IWFDMMService* getWFDMMServiceInstance();

    void*                       mpAppData;
    NotifyCbType                mpNotify;
};

#endif//__WFDSERVICE_CLIENT_H__
