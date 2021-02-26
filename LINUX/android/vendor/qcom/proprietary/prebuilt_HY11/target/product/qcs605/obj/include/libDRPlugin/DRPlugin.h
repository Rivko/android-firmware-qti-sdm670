/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  DRPlugin Interface header file

GENERAL DESCRIPTION
  This file contains the data structure, functions and variables used for handling
  request and response between LocAmtProxy and DR Plugin component.
  This acts as an interface point to integrate AP DR solution.

=============================================================================*/
#ifndef _DRPLUGIN_H
#define _DRPLUGIN_H
#include "gps_extended_c.h"
#include <gps_extended.h>
#include "MsgTask.h"


typedef bool (* reportDrPositionCb_T)(UlpLocation &location, GpsLocationExtended &locationExtended, enum loc_sess_status sessionStatus, LocPosTechMask loc_technology_mask);

typedef pthread_t (* dr_create_thread)(const char* name, void (*start)(void *), void* arg);

typedef void (* reportDrNMEACb_T)(LocGpsUtcTime  timestamp,
                                   const char* nmea, int length);

typedef bool (* reportDrSvStatusCb_T)(const GnssSvNotification& svNotify);


/*++
* The loc_amt_callbacks_t is the structure holding the callbacks DRPlugin component
* make use for communicating with LocAmtProxy.
--*/
typedef struct {

    reportDrPositionCb_T        reportDrPositionCb;
    reportDrNMEACb_T            reportDrNMEACb;
    reportDrSvStatusCb_T        reportSvStatusCb;
    dr_create_thread            dr_create_thread_cb;
    void *                      locAmtProxyRef;

} loc_amt_callbacks_t;


/*++
* DR Session Status
--*/
typedef enum  {
    DR_SESSION_STATUS_OFF = 0,
    DR_SESSION_STATUS_ON,
    DR_SESSION_STATUS_UNKNOWN
}DR_SESSION_STATUS_T;

/*++
* The DRPlugin object is the interface definition between LocAmtProxy to DRPlugin
* component.
--*/
class DRPlugin {

private:
    int                         mDrSessionState;
    loc_amt_callbacks_t*        mLocAmtCb;
    enum loc_sess_status        mLocStatus;

public:
    inline DRPlugin();
    ~DRPlugin();

    bool DRInit(loc_amt_callbacks_t *locAmtCallback);
    bool startDrSession();
    bool stopDrSession();
    bool setFixMode(LocPosMode &params);
    bool deleteAidingData(const LocGpsAidingData aidingData);
    bool reportDrPositionCb(LocGpsLocation & locReport,GpsLocationExtended & locExtendedReport,
                               enum loc_sess_status status, LocPosTechMask loc_technology_mask);
    void reportDrNMEA(long long int timestamp,
                    const char* nmea, int length);
    bool reportDrSVStatusCb(const GnssSvNotification& svNotify);

    bool injectGpsPosition(const UlpLocation &location,
                           const GpsLocationExtended &locationExtended,
                           enum loc_sess_status status,
                           LocPosTechMask loc_technology_mask);

    bool injectGnssSvMeasurement(GnssSvMeasurementSet &svMeasurementSet);
    bool injectGnssSvPolynomial(GnssSvPolynomial &svPolynomial);
    bool injectGnssSvStatus(const GnssSvNotification& svNotify);

};

typedef DRPlugin* (*getDRPlugin_T)();

#ifdef __cplusplus
extern "C" {
#endif
extern DRPlugin* getDRPlugin(void);

#ifdef __cplusplus
}
#endif


#endif /*ifndef _DRPLUGIN_H */

