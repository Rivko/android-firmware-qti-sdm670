/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCHandlerRequest

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#ifndef __IZAT_MANAGER_IIPCHANDLERREQUEST_H__
#define __IZAT_MANAGER_IIPCHANDLERREQUEST_H__

#include <string>

namespace qc_loc_fw {
class OutPostcard;
}

namespace izat_manager {

class IIPCHandlerRequest {
public:
    // If you get messages for string, call on pointer
    virtual int send
    (
        OutPostcard * const outCard,
        const std :: string & to
    ) = 0;

    // dtor
    virtual ~IIPCHandlerRequest () {}
};

}

#endif // #ifndef __IZAT_MANAGER_IIPCHANDLERREQUEST_H__
