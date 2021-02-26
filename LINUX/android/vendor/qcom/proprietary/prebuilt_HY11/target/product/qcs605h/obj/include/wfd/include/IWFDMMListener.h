#ifndef __IWFD_LISTENER_H__
#define __IWFD_LISTENER_H__

/*==============================================================================
*       IWFDMMListener.h
*
*  DESCRIPTION:
*       WFD MM Service Listener Interface
*
*
*  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*
*   Not a Contribution.
*  Apache license notifications and license are retained
*  for attribution purposes only.
*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
10/10/2014         UV            InitialDraft
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

using namespace android;

typedef enum StreamingEvent {
    STREAMING_PAUSE_PLAYBACK,
    STREAMING_RESUME_PLAYBACK,
    STREAMING_FRAME_COMPLETE,
    STREAMING_FD_FRAME_COMPLETE,
    STREAMING_CLOSE_REQ,
    STREAMING_START_FAIL,
    STREAMING_FRAME_REC_FAIL,
    STREAMING_STOP_FAIL,
    STREAMING_PAUSE_FAIL,
    STREAMING_QUEUE_FULL,
    STREAMING_MEM_ALLOC_FAIL,
    STREAMING_ERROR_NONE,
    STREAMING_SERVICE_DEAD
} StreamingEventType;


class IWFDMMListener: public android::IInterface
{
public:

    enum {
        NOTIFY = IBinder::FIRST_CALL_TRANSACTION
    };

    DECLARE_META_INTERFACE(WFDMMListener);

    virtual int notify(int event, void* PvtData) = 0;

};

class BpWFDMMListener: public BpInterface<IWFDMMListener>
{
public:
    BpWFDMMListener(const sp<IBinder>& wfdIfListner):
                                    BpInterface<IWFDMMListener>(wfdIfListner){};
    virtual int notify(int event, void* PvtData);

};

class BnWFDMMListener: public BnInterface<IWFDMMListener>
{
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel *reply,
                                                         uint32_t flags = 0);
};


#endif //__IWFDSERVICE_H__
