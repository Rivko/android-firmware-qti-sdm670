/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_LocAmtProxy"
#include <dlfcn.h>
#include <log_util.h>
#include <loc_pla.h>
#include <ulp_engine.h>
#include <LocUlpProxy.h>
#include <LocAmtProxy.h>
#include <msg_q.h>
#include <ulp_internal.h>
#include "DRPlugin.h"

using namespace loc_core;

loc_amt_callbacks_t  gLocAmtCb;

bool reportDrPositionCb(UlpLocation &location,
                        GpsLocationExtended &locationExtended,
                        enum loc_sess_status status,
                        LocPosTechMask loc_technology_mask)
{
    LocAmtProxy*       pLocAmtProxyRef;

    ENTRY_LOG();

    pLocAmtProxyRef = (LocAmtProxy *)gLocAmtCb.locAmtProxyRef;

    /*++
    ** Report as DR position not normal reportPosition, reportPosition is used from LocEngAdapter to Amtproxy path
    -- */
    pLocAmtProxyRef->reportDrPosition(location,locationExtended,status,loc_technology_mask);

    return true;

} /* reportDrPositionCb */

bool reportDrSvStatusCb(const GnssSvNotification& svNotify)
{
    LocAmtProxy*       pLocAmtProxyRef;

    ENTRY_LOG();

    pLocAmtProxyRef = (LocAmtProxy *)gLocAmtCb.locAmtProxyRef;

    /*++
    ** Report DR SvStatus report as received from AP DR Core
    -- */
    pLocAmtProxyRef->reportDrSv(svNotify);

    return true;

} /* reportDrSvStatusCb */



LocAmtProxy::LocAmtProxy() :
             LocUlpProxy(),
             mDRPlugin(NULL)
{
}

LocAmtProxy::~LocAmtProxy()
{
}

/*===========================================================================
FUNCTION    DRInit

DESCRIPTION
  Initialize the external DR functionality

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::DRInit(void* dr_thread_cb)
{
    getDRPlugin_T pDrPlugin = NULL;
    bool ret_val = 0;

    ENTRY_LOG();

    if ( NULL != dr_thread_cb)
    {
        gLocAmtCb.dr_create_thread_cb = (dr_create_thread)dr_thread_cb;
    }
    else
    {
        LOC_LOGE("%s:: NULL dr_thread_cb()", __func__);
        return false;
    }

    gLocAmtCb.locAmtProxyRef = (void*) this;

    mDRPlugin = getDRPlugin();
    if ((void*)NULL != mDRPlugin) {
        gLocAmtCb.reportDrPositionCb = (reportDrPositionCb_T)reportDrPositionCb;
        gLocAmtCb.reportSvStatusCb   = (reportDrSvStatusCb_T)reportDrSvStatusCb;
        ret_val = mDRPlugin->DRInit(&gLocAmtCb);
        return ret_val;
    }
    else
    {
        LOC_LOGE("%s:: DRInit: Problem in Loading DRPlugin", __func__);
        return false;
    }
}


/*===========================================================================
FUNCTION    sendStartFix

DESCRIPTION
   Send msg to ULP queue to start a fix

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendStartFix()
{
    LOC_LOGD("%s:: Sending StartDrFix", __func__);

    ulp_msg *msg(new ulp_msg(this, ULP_MSG_START_DR_FIX));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}

/*===========================================================================
FUNCTION    sendStopFix

DESCRIPTION
   Send msg to ULP queue to stop fix

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendStopFix()
{
    LOC_LOGD("%s:: Sending StopDrFix", __func__);
    ulp_msg *msg(new ulp_msg(this, ULP_MSG_STOP_DR_FIX));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}

/*===========================================================================
FUNCTION    sendFixMode

DESCRIPTION
   Send msg to ULP queue to set position mode

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendFixMode(LocPosMode &params)
{
    ulp_msg_dr_position_mode *msg(new ulp_msg_dr_position_mode(&ulp_data, params));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}

/*===========================================================================
FUNCTION    reportDeleteAidingData

DESCRIPTION
   Send msg to ULP queue to send out delete aiding data

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportDeleteAidingData(LocGpsAidingData aidingData)

{
    ulp_msg_dr_delete_aiding_data *msg(new ulp_msg_dr_delete_aiding_data(&ulp_data, aidingData));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}


/*===========================================================================
FUNCTION    reportPosition

DESCRIPTION
   Send msg to ULP queue to with information about the position fix

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportPosition(const UlpLocation &location,
                                 const GpsLocationExtended &locationExtended,
                                 enum loc_sess_status status,
                                 LocPosTechMask loc_technology_mask)
{
    /*++
        **  Send the received GPS position to ULP which inturn forwards it to
        **  external DR Solution after necessary checks.
        --*/
    ulp_msg_send_position_external *msg(
        new ulp_msg_send_position_external(&ulp_data,
                                           location,
                                           locationExtended,
                                           status, loc_technology_mask));
    msg_q_snd(mQ, msg, ulp_msg_free);

    return true;
}

/*===========================================================================
FUNCTION    reportSvMeasurement

DESCRIPTION
   Send msg to ULP queue to with information about the SV Measurement

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportSvMeasurement(GnssSvMeasurementSet &svMeasurementSet)
{
    ENTRY_LOG();
    /*++.
    * Send SV Measurement to ULP for processing
    --*/
    ulp_msg_report_sv_measurement *msg(new ulp_msg_report_sv_measurement(&ulp_data, svMeasurementSet));
    msg_q_snd(mQ, msg, ulp_msg_free);

    return true;
}

/*===========================================================================
FUNCTION    reportSvPolynomial

DESCRIPTION
   Send msg to ULP queue to with information about the SV Polynomial

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportSvPolynomial(GnssSvPolynomial &svPolynomial)
{
    ENTRY_LOG();
    /*++
    * Send SV Measurement to ULP for processing
    --*/
    ulp_msg_report_sv_polynomial *msg(
         new ulp_msg_report_sv_polynomial(&ulp_data, svPolynomial));
    msg_q_snd(mQ, msg, ulp_msg_free);

    return true;
}



/*===========================================================================
FUNCTION    reportDrPosition

DESCRIPTION
   This function receives the DR position from external DR Solution through DRPlugin and forwards
   the same to LocEngInternalAdapter, to pass it to Android framework

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/

bool LocAmtProxy::reportDrPosition(UlpLocation &location,
                                   GpsLocationExtended &locationExtended,
                                   enum loc_sess_status status,
                                   LocPosTechMask loc_technology_mask)
{

   //Send the DR position to ULP
   ulp_msg_report_dr_position *msg(
        new ulp_msg_report_dr_position(&ulp_data,
                                    location,
                                    locationExtended,
                                    status,
                                    loc_technology_mask));
    msg_q_snd(mQ, msg, ulp_msg_free);

    return true;

}

/*===========================================================================
FUNCTION    sendFixModeToDr

DESCRIPTION
   Send msg to DRPlugin to set position mode

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendFixModeToDr(LocPosMode &params)
{
   return mDRPlugin->setFixMode(params);
}


/*===========================================================================
FUNCTION    sendGnssPositionToDr

DESCRIPTION
   Send msg to DRPlugin to set position mode

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendGnssPositionToDr(const UlpLocation &location,
                                       const GpsLocationExtended &locationExtended,
                                       enum loc_sess_status status,
                                       LocPosTechMask loc_technology_mask)
{
    //Send Gnss Position to AP DR for consumption.
    return mDRPlugin->injectGpsPosition(location,locationExtended,status,loc_technology_mask);
}


/*===========================================================================
FUNCTION    sendGnssSvToDr

DESCRIPTION
   Send Gnss SV msg to DRPlugin

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendGnssSvToDr(const GnssSvNotification& svNotify)
{
    //Send Gnss Sv Status to AP DR for consumption.
    return mDRPlugin->injectGnssSvStatus(svNotify);
}


/*===========================================================================
FUNCTION    sendGnssSvMeasurementToDr

DESCRIPTION
   Send msg to DRPlugin to set position mode

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendGnssSvMeasurementToDr(GnssSvMeasurementSet &svMeasurementSet)
{
    //Send Gnss Position to AP DR for consumption.
    return mDRPlugin->injectGnssSvMeasurement(svMeasurementSet);

}


/*===========================================================================
FUNCTION    sendGnssSvPolynomialToDr

DESCRIPTION
   Send msg to DRPlugin to set position mode

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendGnssSvPolynomialToDr(GnssSvPolynomial &svPolynomial)
{
    //Send Gnss Position to AP DR for consumption.
    return mDRPlugin->injectGnssSvPolynomial(svPolynomial);
}

/*===========================================================================
FUNCTION    sendDeleteAidingDataToDr

DESCRIPTION
   Send msg to DRPlugin to set position mode

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::sendDeleteAidingDataToDr(const LocGpsAidingData aidingData)
{
    //Send Gnss Position to AP DR for consumption.
    return mDRPlugin->deleteAidingData(aidingData);
}


/*===========================================================================
FUNCTION    startDrSession

DESCRIPTION
   Start the DR Fix Session with external DR solution

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::startDrSession()
{
    return mDRPlugin->startDrSession();
}

/*===========================================================================
FUNCTION    stopDrSession

DESCRIPTION
   Start the DR Fix Session with AP DR solution

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::stopDrSession()
{
    return mDRPlugin->stopDrSession();
}

/*===========================================================================
FUNCTION    reportSv

DESCRIPTION
   Send msg to ULP queue to with information about visible satellites
   as SV report

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportSv(const GnssSvNotification& svNotify)
{
    /*++
    ** Send the received SV Status to ULP which inturn forwards it to
    ** AP DR Solution.
    --*/
    ulp_msg_send_sv_external *msg(
        new ulp_msg_send_sv_external(&ulp_data, svNotify));
    msg_q_snd(mQ, msg, ulp_msg_free);

    return true;
}

/*===========================================================================
FUNCTION    reportDrSv

DESCRIPTION
   Send DR SVs to LocAdapter about visible satellites as SV report sent by AP DR component

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportDrSv(const GnssSvNotification& svNotify)
{
    /*++
    **  Send the received DR SV Status to ULP which inturn forwards it to
    **  LocEngAdapter after necessary processing.
    --*/
    ulp_msg_report_sv *msg(
        new ulp_msg_report_sv(&ulp_data, svNotify, ULP_SVINFO_IS_FROM_DR));
    msg_q_snd(mQ, msg, ulp_msg_free);

    return true;
}


/*===========================================================================
FUNCTION    reportStatus

DESCRIPTION
   Send msg to ULP queue about GPS engine status

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocAmtProxy::reportStatus(LocGpsStatusValue status)
{
    return LocUlpProxy::reportStatus(status);
}

