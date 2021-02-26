/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCHandlerResponse

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#ifndef __IZAT_MANAGER_IIPCHANDLERRESPONSE_H__
#define __IZAT_MANAGER_IIPCHANDLERRESPONSE_H__

#include <string>

namespace qc_loc_fw {
class InPostcard;
}

namespace izat_manager {

using namespace qc_loc_fw;

class IIPCHandlerResponse {
public:
    virtual void handle
    (
        InPostcard * const inCard,
        const std :: string & from
    ) = 0;

    // dtor
    virtual ~IIPCHandlerResponse () {}
};

}

#endif // #ifndef __IZAT_MANAGER_IIPCHANDLERRESPONSE_H__

