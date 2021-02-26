/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_LocUlpProxy"
#include <log_util.h>
#include <loc_pla.h>
#include <errno.h>
#include <loc_cfg.h>
#include <ulp_engine.h>
#include <LocUlpProxy.h>
#include <msg_q.h>
#include <ulp_internal.h>
#include <loc_target.h>
#include <EventObserver.h>

using namespace loc_core;

#define GARDEN_APP_PROC_NAME "garden_app"
#define PROC_ENABLED   "ENABLED"
#define MAX_PARAM_NAME_LENGTH 64
/*Length of process arguments in conf file*/
#define MAX_PARAM_ARGUMENT_LENGTH 100

LocUlpProxy::LocUlpProxy() :
    UlpProxyBase(), mAdapter(NULL), mEventObserver(NULL),
    mCapabilities(0), mQ((void*)msg_q_init2())
{
    FILE* conf_fp;
    bool create_event_q = false;
    int name_length=0;
    char conf_proc_name[MAX_PARAM_NAME_LENGTH];
    char conf_proc_argument[MAX_PARAM_ARGUMENT_LENGTH];
    char conf_proc_status[MAX_PARAM_NAME_LENGTH];
    char conf_hardware_type[MAX_PARAM_NAME_LENGTH];
    char autoplatform_name[MAX_PARAM_ARGUMENT_LENGTH];

    /* izat.conf Parameter spec table */
    loc_param_s_type izat_conf_parameter_table[] =
    {
        {"PROCESS_NAME",        &conf_proc_name,           NULL, 's'},
        {"PROCESS_ARGUMENT",    &conf_proc_argument,       NULL, 's'},
        {"PROCESS_STATE",       &conf_proc_status,         NULL, 's'},
        {"HARDWARE_TYPE",       &conf_hardware_type,       NULL, 's'},
    };

    if((conf_fp = fopen(LOC_PATH_IZAT_CONF, "r")) == NULL) {
        LOC_LOGE("%s:%d]: Error opening %s %s\n", __func__,
                 __LINE__, LOC_PATH_IZAT_CONF, strerror(errno));
    }

    /* If garden_app is enabled in izat.conf file, and automotive platform
    ** only then create Event_Q*/
    loc_get_auto_platform_name(autoplatform_name,sizeof(autoplatform_name));

    do {
        conf_proc_name[0] = 0;
        if(loc_read_conf_r(conf_fp, izat_conf_parameter_table,
           sizeof(izat_conf_parameter_table)/sizeof(izat_conf_parameter_table[0])))
        {
            LOC_LOGE("%s:%d]: Unable to read conf file. Failing\n", __func__, __LINE__);
            break;
        }
        name_length=(int)strlen(conf_proc_name);

        if( (name_length) && (!strcmp(conf_proc_name, GARDEN_APP_PROC_NAME)) &&
            (!strcmp(conf_proc_status, PROC_ENABLED)) &&
            (strcmp(autoplatform_name, conf_hardware_type) == 0))
        {
           create_event_q = true;
           LOC_LOGV("garden_app Enabled & Autoplatform, create EventObserver.\n");
           break;
        }
    } while(name_length);

    if(create_event_q)
    {
        mEventObserver = new EventObserver();
        if(NULL != mEventObserver)
        {
            LOC_LOGV("%s: EventObserver Created\n", __func__);
        }else{
            LOC_LOGE("%s: Error Creating EventObserver\n", __func__);
        }
    }
    if (NULL != conf_fp) {
        fclose(conf_fp);
    }
}

LocUlpProxy::~LocUlpProxy()
{
    msg_q_destroy(&mQ);
    if (NULL != mEventObserver)
    {
        delete mEventObserver;
    }
}
/*===========================================================================
FUNCTION    setAdapter

DESCRIPTION
   Set the value of adapter to be used

DEPENDENCIES
   None

RETURN VALUE

SIDE EFFECTS
   N/A
===========================================================================*/
void LocUlpProxy::setAdapter(LocAdapterBase* adapter)
{
    LOC_LOGV("%s] %p", __func__, adapter);
    mAdapter = adapter;
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
bool LocUlpProxy::sendStartFix()
{
    ulp_msg *msg(new ulp_msg(this, ULP_MSG_START_FIX));
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
bool LocUlpProxy::sendStopFix()
{
    ulp_msg *msg(new ulp_msg(this, ULP_MSG_STOP_GNSS_FIX));
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
bool LocUlpProxy::sendFixMode(LocPosMode &params)
{
    ulp_msg_position_mode *msg(
        new ulp_msg_position_mode(&ulp_data, params));
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
bool LocUlpProxy::reportPosition(const UlpLocation &location,
                                 const GpsLocationExtended &locationExtended,
                                 enum loc_sess_status status,
                                 LocPosTechMask loc_technology_mask)
{
    ulp_msg_report_position *msg(
        new ulp_msg_report_position(&ulp_data,
                                    location,
                                    locationExtended,
                                    status,
                                    loc_technology_mask));
    msg_q_snd(mQ, msg, ulp_msg_free);
    LOC_LOGV("%s] %d", __func__, location.position_source);
    return true;
}


/*===========================================================================
FUNCTION    reportPositions

DESCRIPTION
   Send msg to ULP queue to with information about the position fix

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocUlpProxy::reportPositions(const UlpLocation* ulpLocations,
                                  const GpsLocationExtended* extendedLocations,
                                  const uint32_t* techMasks,
                                  const size_t count)
{
    ulp_msg_report_positions *msg(
        new ulp_msg_report_positions(&ulp_data,
                                     ulpLocations, extendedLocations, techMasks, count));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}

/*===========================================================================
FUNCTION    reportBatchingSession

DESCRIPTION
   Send msg to ULP queue to with information about the Batching session

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocUlpProxy::reportBatchingSession(const LocationOptions& options,
                                        bool active)
{
    ulp_msg_report_batching_session *msg(
        new ulp_msg_report_batching_session(&ulp_data, options, active));
    msg_q_snd(mQ, msg, ulp_msg_free);
    LOC_LOGV("%s]", __func__);
    return true;
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
bool LocUlpProxy::reportSv(const GnssSvNotification& svNotify)
{
    ulp_msg_report_sv *msg(
        new ulp_msg_report_sv(&ulp_data, svNotify, ULP_SVINFO_IS_FROM_GNSS));
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
bool LocUlpProxy::reportStatus(LocGpsStatusValue status)
{
    ulp_msg_report_gnss_status *msg(
        new ulp_msg_report_gnss_status(&ulp_data, status));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}
/*===========================================================================
FUNCTION    reportNmea

DESCRIPTION
   Send msg to ULP queue about GPS NMEA strings

DEPENDENCIES
   None

RETURN VALUE
   true

SIDE EFFECTS
   N/A
===========================================================================*/
bool LocUlpProxy::reportNmea(const char* nmea, int length)
{
    ulp_msg_report_nmea *msg(
        new ulp_msg_report_nmea(&ulp_data, nmea, length));
    msg_q_snd(mQ, msg, ulp_msg_free);
    return true;
}

