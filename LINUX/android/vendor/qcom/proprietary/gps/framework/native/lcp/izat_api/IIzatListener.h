/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIzatListener

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IIZATLISTENER_H__
#define __IZAT_MANAGER_IIZATLISTENER_H__

#include "IzatTypes.h"

namespace izat_manager {

class IzatLocation;
class IzatNmea;
class IzatSvInfo;
/**
 * @brief IIzatListener interface
 * @details IIzatListener interface;
 *          Must be implemented by OS dependent code to listen for location, status, and other data changes
 *
 */
class IIzatListener {

public:

    /**
     * @brief Invoked when location changes
     * @details Invoked when location changes
     *
     * @param location Pointer to an instance of IzatLocation
     */
    virtual void onLocationChanged (const IzatLocation * location, const IzatLocationStatus status) = 0;

    /**
     * @brief Invoked when status changes
     * @details Invoked when status changes
     *
     * @param status Pointer to an instance of IzatProviderStatus
     */
    virtual void onStatusChanged (const IzatProviderStatus status) = 0;

    /**
     * @brief Invoked when nmea changes
     * @details Invoked when nmea changes
     *
     * @param nmea Pointer to an instance of IzatNmea
     */
    virtual void onNmeaChanged (const IzatNmea * nmea) = 0;

    /**
     * @brief Invoked when svinfo changes
     * @details Invoked when svinfo changes
     *
     * @param nmea Pointer to an instance of IzatSvInfo
     */
    virtual void onSvReportChanged (const IzatSvInfo *svInfo) {
        (void)(svInfo); //Avoid compiler error
    }

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IIzatListener () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IIZATLISTENER_H__
