/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <loc_pla.h>
#include <log_util.h>
#include <gps_extended.h>
#include "ulp_service.h"
#include "garden_app_session_tracker.h"

namespace garden {

garden_state_base::garden_state_base(){
}

garden_state_base::~garden_state_base(){
}

garden_state_initialized::garden_state_initialized(IGnssAPI * a_gps_interface): garden_state_base(),
                          m_gps_interface(a_gps_interface)
{
}

garden_state_initialized::~garden_state_initialized()
{
}

garden_session_state_t garden_state_initialized::process_system_event(unsigned int sys_event)
{
    garden_session_state_t  nextState = GARDEN_SESSION_STATE_INITIALIZED;

    switch (sys_event)
    {
    case ULP_LOC_GPS_SETTINGS_ON:
        nextState = GARDEN_SESSION_STATE_SESSION_ON;
        break;
    case ULP_LOC_GPS_SETTINGS_OFF:
        nextState = GARDEN_SESSION_STATE_GPS_OFF;
        break;
    }

    LOC_LOGV("%s] SystemEvent: %d, NextState: %d\n", __func__, sys_event, nextState);
    return nextState;
}

bool  garden_state_initialized::state_action()
{
    //NO Action for Initialized State
    return true;
}

garden_state_session_off:: garden_state_session_off(IGnssAPI * a_gps_interface): garden_state_base(),
                            m_gps_interface(a_gps_interface)
{
}

garden_state_session_off:: ~garden_state_session_off()
{
}

garden_session_state_t garden_state_session_off::process_system_event(unsigned int sys_event)
{
    garden_session_state_t  nextState = GARDEN_SESSION_STATE_SESSION_OFF;

    switch (sys_event)
    {
    case ULP_LOC_GPS_SETTINGS_ON:
    case ULP_LOC_SCREEN_ON:
        nextState = GARDEN_SESSION_STATE_SESSION_ON;
        break;
    case ULP_LOC_GPS_SETTINGS_OFF:
        nextState = GARDEN_SESSION_STATE_GPS_OFF;
        break;
    }

    LOC_LOGV("%s] SystemEvent: %d, NextState: %d\n", __func__, sys_event, nextState);
    return nextState;
}

bool  garden_state_session_off::state_action()
{
    int rc = 0;
    bool retVal = true;

    rc = m_gps_interface->gnssStop ();
    if(0 != rc){
        LOC_LOGE("%s] Error in Stop Session.\n", __func__);
        retVal = false;
    }

    return retVal;
}


garden_state_session_on::garden_state_session_on(IGnssAPI * a_gps_interface): garden_state_base(),
                            m_gps_interface(a_gps_interface)
{
}

garden_state_session_on::~garden_state_session_on(){
}

garden_session_state_t garden_state_session_on::process_system_event(unsigned int sys_event)
{
    garden_session_state_t  nextState = GARDEN_SESSION_STATE_SESSION_ON;

    switch (sys_event)
    {
    case ULP_LOC_SCREEN_OFF:
        nextState = GARDEN_SESSION_STATE_SESSION_OFF;
        break;
    case ULP_LOC_GPS_SETTINGS_OFF:
        nextState = GARDEN_SESSION_STATE_GPS_OFF;
        break;
    }

    LOC_LOGV("%s] SystemEvent: %d, NextState: %d\n", __func__, sys_event, nextState);
    return nextState;
}

bool  garden_state_session_on::state_action()
{
    int rc = 0;
    bool retVal = true;

    m_gps_interface->gnssSetPositionMode (LOC_GPS_POSITION_MODE_MS_BASED,
                                     LOC_GPS_POSITION_RECURRENCE_PERIODIC,
                                     1000,
                                     ULP_HORZ_ACCURACY_HIGH,
                                     1000);
    rc = m_gps_interface->gnssStart ();
    if(0 != rc){
        LOC_LOGE("%s] Error in Start Session.\n", __func__);
        retVal = false;
    }

    return retVal;

    return true;
}


garden_state_gps_off::garden_state_gps_off(IGnssAPI * a_gps_interface): garden_state_base(),
                        m_gps_interface(a_gps_interface)
{
}

garden_state_gps_off::~garden_state_gps_off(){
}

garden_session_state_t garden_state_gps_off::process_system_event(unsigned int sys_event)
{
    garden_session_state_t  nextState = GARDEN_SESSION_STATE_GPS_OFF;

    switch (sys_event)
    {
    case ULP_LOC_GPS_SETTINGS_ON:
        nextState = GARDEN_SESSION_STATE_SESSION_ON;
        break;
    }

    LOC_LOGV("%s] SystemEvent: %d, NextState: %d\n", __func__, sys_event, nextState);
    return nextState;

}

bool  garden_state_gps_off::state_action()
{
    int rc = 0;
    bool retVal = true;

    rc = m_gps_interface->gnssStop ();
    if(0 != rc){
        LOC_LOGE("%s] Error in Stop Session.\n", __func__);
        retVal = false;
    }

    return retVal;

}


garden_app_session_tracker::garden_app_session_tracker(IGnssAPI * a_gps_interface):
                                 m_current_sesssion_state(GARDEN_SESSION_STATE_INITIALIZED)
{
    m_garden_session_states[GARDEN_SESSION_STATE_INITIALIZED] = new garden_state_initialized(a_gps_interface);
    m_garden_session_states[GARDEN_SESSION_STATE_SESSION_OFF] = new garden_state_session_off(a_gps_interface);
    m_garden_session_states[GARDEN_SESSION_STATE_SESSION_ON] = new garden_state_session_on(a_gps_interface);
    m_garden_session_states[GARDEN_SESSION_STATE_GPS_OFF] = new garden_state_gps_off(a_gps_interface);


    /*initialize mutex for state machine*/
    pthread_mutex_init(&m_sm_mutex, NULL);

    if( (NULL == m_garden_session_states[GARDEN_SESSION_STATE_INITIALIZED]) ||
        (NULL == m_garden_session_states[GARDEN_SESSION_STATE_SESSION_OFF]) ||
        (NULL == m_garden_session_states[GARDEN_SESSION_STATE_SESSION_ON]) ||
        (NULL == m_garden_session_states[GARDEN_SESSION_STATE_GPS_OFF]) )
    {
        //Initialization failed move to unknown state
        m_current_sesssion_state = GARDEN_SESSION_STATE_UNKNOWN;
    }

    /*Simulate GPS_SETTINGS_ON to move to SESSION_ON*/
    LOC_LOGE("%s: Simulate GPS_SETTINGS_ON\n", __func__);
    process_system_event(ULP_LOC_GPS_SETTINGS_ON);
}

garden_app_session_tracker::~garden_app_session_tracker()
{
    CHECK_AND_DELETE(m_garden_session_states[GARDEN_SESSION_STATE_INITIALIZED]);
    CHECK_AND_DELETE(m_garden_session_states[GARDEN_SESSION_STATE_SESSION_OFF]);
    CHECK_AND_DELETE(m_garden_session_states[GARDEN_SESSION_STATE_SESSION_ON]);
    CHECK_AND_DELETE(m_garden_session_states[GARDEN_SESSION_STATE_GPS_OFF]);

    pthread_mutex_destroy(&m_sm_mutex);

}

void garden_app_session_tracker::process_system_event(unsigned int sys_event)
{
    garden_session_state_t  nextState = m_current_sesssion_state;

    pthread_mutex_lock(&m_sm_mutex);

    if(GARDEN_SESSION_STATE_UNKNOWN != m_current_sesssion_state)
    {
        nextState = m_garden_session_states[m_current_sesssion_state]->process_system_event(sys_event);
        if (m_current_sesssion_state != nextState)
        {
            LOC_LOGV("garden_app session state moves from %d To %d", m_current_sesssion_state, nextState);
            m_current_sesssion_state = nextState;
            m_garden_session_states[m_current_sesssion_state]->state_action();
        }
    }

    pthread_mutex_unlock(&m_sm_mutex);
}

} // namespace garden
