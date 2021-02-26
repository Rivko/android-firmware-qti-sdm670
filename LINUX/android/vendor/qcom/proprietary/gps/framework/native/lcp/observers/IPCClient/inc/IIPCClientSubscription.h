/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCClientSubscription

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IIPCCLIENTSUBSCRIPTION_H__
#define __IZAT_MANAGER_IIPCCLIENTSUBSCRIPTION_H__

#include <list>
#include <string>
#include <DataItemId.h>

namespace izat_manager
{

// provides internal interface to subscribe/unsubscribe for data items
class IIPCClientSubscription {

public:
    virtual void subscribe
    (
        const std :: list <DataItemId> & l,
        const std :: string & client
    ) = 0;;

    virtual void updateSubscription
    (
        const std :: list <DataItemId> & l,
        const std :: string & client
    ) = 0;

    virtual void requestData
    (
        const std :: list <DataItemId> & l,
        const std :: string & client
    ) = 0;

    virtual void unsubscribe
    (
        const std :: list <DataItemId> & l,
        const std :: string & client
    ) = 0;

    virtual void unsubscribeAll
    (
        const std :: string & client
    ) = 0;

    virtual void clientRestartEvent
    (
        const std :: string & client
    ) = 0;

    virtual ~IIPCClientSubscription () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IIPCCLIENTSUBSCRIPTION_H__

