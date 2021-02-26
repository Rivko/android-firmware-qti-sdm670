/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  LocAmtProxy header file

GENERAL DESCRIPTION
  This file contains the data structure and variables used for handling requests
  sent by GPS HAL. This acts as a proxy for the ULP module and generally forwards
  the requests to the ULP msg queue. This module also bridges between ULP with DRPlugin component
  to connect to external DR (Dead Reckoning) solution
=============================================================================*/

#ifndef LOC_AMT_PROXY_H
#define LOC_AMT_PROXY_H

#include "DRPlugin.h"
#include <LocAdapterBase.h>

using namespace loc_core;

class LocAmtProxy : public LocUlpProxy {

protected:

    DRPlugin    *mDRPlugin;

public:
    LocAmtProxy();
    virtual ~LocAmtProxy();

    virtual bool DRInit(void *);
    virtual bool sendStartFix();
    virtual bool sendStopFix();
    virtual bool sendFixMode(LocPosMode &params);
    virtual bool reportDeleteAidingData(LocGpsAidingData aidingData);
    virtual bool reportPosition(const UlpLocation &location,
                                const GpsLocationExtended &locationExtended,
                                enum loc_sess_status status,
                                LocPosTechMask loc_technology_mask);

    virtual bool reportDrPosition(UlpLocation &location,
                                GpsLocationExtended &locationExtended,
                                enum loc_sess_status status,
                                LocPosTechMask loc_technology_mask);

    virtual bool reportSv(const GnssSvNotification& svNotify);

    virtual bool reportDrSv(const GnssSvNotification& svNotify);

    virtual bool reportSvPolynomial(GnssSvPolynomial &svPolynomial);

    virtual bool reportSvMeasurement(GnssSvMeasurementSet &svMeasurementSet);

    virtual bool reportStatus(LocGpsStatusValue status);

    inline void sendLocMsg(LocMsg* msg) {
        LocUlpProxy::sendLocMsg(msg);
    }
    inline virtual void setAdapter(LocAdapterBase* adapter)
    {
        LocUlpProxy::setAdapter(adapter);
    }
    inline virtual void setCapabilities(unsigned long capabilities) {
        LocUlpProxy::setCapabilities(capabilities);
    }
    inline virtual LocAdapterBase* getAdapter() { return LocUlpProxy::getAdapter(); }

    virtual bool sendFixModeToDr(LocPosMode &params);
    virtual bool sendGnssPositionToDr(const UlpLocation &location,
                                      const GpsLocationExtended &locationExtended,
                                      enum loc_sess_status status,
                                      LocPosTechMask loc_technology_mask);
    virtual bool sendGnssSvMeasurementToDr(GnssSvMeasurementSet &svMeasurementSet);
    virtual bool sendGnssSvPolynomialToDr(GnssSvPolynomial &svPolynomial);
    virtual bool sendDeleteAidingDataToDr(const LocGpsAidingData aidingData);
    virtual bool sendGnssSvToDr(const GnssSvNotification& svNotify);

    virtual bool startDrSession();
    virtual bool stopDrSession();
    inline virtual int sendSystemEvent(unsigned int sysEvent){ return LocUlpProxy::sendSystemEvent(sysEvent); }

};

/*++
* Callback function called by DR Plugin when DR Position is received.
--*/
bool reportDrPositionCb(UlpLocation &location,
                        GpsLocationExtended &locationExtended,
                        enum loc_sess_status status,
                        LocPosTechMask loc_technology_mask);

#endif /* LOC_AMT_PROXY_H */
