/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IEmergencyRequest

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IODCPIREQUEST_H__
#define __IZAT_MANAGER_IODCPIREQUEST_H__

#include <comdef.h>
#include "IzatRequest.h"

namespace izat_manager {

using namespace std;

/**
 * @brief IOdcpiRequest class
 * @details This class is used to create/set ODCPI requests.
 */
class IOdcpiRequest : public IzatRequest {
public:

    bool getEmergency () const {
        return mEmergencyRequest;
    }

    void setEmergency (bool emergency) {
        mEmergencyRequest = emergency;
    }


    /**
     * @brief Default contructor
     * @details Default constructor
     *
     */
    IOdcpiRequest(int64 timeBetweenFixes, bool emergencyRequest)
    {
        setProvider (IZAT_STREAM_NETWORK);
        setHorizontalAccuracy (IZAT_HORIZONTAL_BLOCK);
        setInterval (timeBetweenFixes);
        setDistance (0);
        setEmergency(emergencyRequest);
    }

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    ~IOdcpiRequest () {}


private:

    bool mEmergencyRequest;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IODCPIREQUEST_H__
