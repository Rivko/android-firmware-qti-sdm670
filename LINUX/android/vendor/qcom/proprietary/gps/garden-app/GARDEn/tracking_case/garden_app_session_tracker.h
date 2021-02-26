/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef _GARDEN_APP_SESSION_TRACKER_H_
#define _GARDEN_APP_SESSION_TRACKER_H_

#include <pthread.h>
#include <IGnssAPI.h>

namespace garden {
#define CHECK_AND_DELETE(x) if (NULL != (x)) \
                            { \
                               delete (x); \
                            }

typedef enum {
    GARDEN_SESSION_STATE_INITIALIZED = 0,
    GARDEN_SESSION_STATE_SESSION_OFF,
    GARDEN_SESSION_STATE_SESSION_ON,
    GARDEN_SESSION_STATE_GPS_OFF,
    GARDEN_SESSION_STATE_MAX,
    GARDEN_SESSION_STATE_UNKNOWN
}garden_session_state_t;

class garden_state_base {
public:
    garden_state_base();
    virtual ~garden_state_base();
    virtual inline garden_session_state_t process_system_event(unsigned int /*sys_event*/){
        return GARDEN_SESSION_STATE_INITIALIZED;
    }
    virtual inline bool state_action(){
        return false;
    }
};

class garden_state_initialized: public garden_state_base {
private:
    IGnssAPI          *m_gps_interface;
public:
    garden_state_initialized(IGnssAPI * a_gps_interface);
    virtual ~garden_state_initialized();
    virtual garden_session_state_t process_system_event(unsigned int sys_event);
    virtual bool state_action();
};

class garden_state_session_off: public garden_state_base {
private:
    IGnssAPI          *m_gps_interface;
public:
    garden_state_session_off(IGnssAPI * a_gps_interface);
    virtual ~garden_state_session_off();
    virtual inline garden_session_state_t process_system_event(unsigned int sys_event);
    virtual bool state_action();
};

class garden_state_session_on: public garden_state_base {
private:
    IGnssAPI          *m_gps_interface;
public:
    garden_state_session_on(IGnssAPI * a_gps_interface);
    virtual ~garden_state_session_on();
    virtual garden_session_state_t process_system_event(unsigned int sys_event);
    virtual bool state_action();
};

class garden_state_gps_off: public garden_state_base {
private:
    IGnssAPI          *m_gps_interface;
public:
    garden_state_gps_off(IGnssAPI * a_gps_interface);
    virtual ~garden_state_gps_off();
    virtual garden_session_state_t process_system_event(unsigned int sys_event);
    virtual bool state_action();
};


class garden_app_session_tracker {
private:
    pthread_mutex_t             m_sm_mutex;
    garden_session_state_t      m_current_sesssion_state;
    garden_state_base           *m_garden_session_states[GARDEN_SESSION_STATE_MAX];

public:
    garden_app_session_tracker(IGnssAPI *a_gps_interface);
    virtual ~garden_app_session_tracker();
    virtual void process_system_event(unsigned int sys_event);
};

} // namespace garden

#endif //_GARDEN_APP_SESSION_TRACKER_H_
