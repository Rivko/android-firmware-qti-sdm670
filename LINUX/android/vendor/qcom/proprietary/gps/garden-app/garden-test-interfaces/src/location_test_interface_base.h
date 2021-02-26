/* Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef LOCATION_TEST_INTERFACE_BASE_H
#define LOCATION_TEST_INTERFACE_BASE_H

#include <gps_extended.h>

typedef void (* garden_location_cb_t) (LocGpsLocation* location);
typedef void (* garden_status_cb_t) (LocGpsStatus *status);
typedef void (* garden_sv_status_cb_t) (LocGpsSvStatus* sv_info);
typedef void (* garden_nmea_cb_t)(int64_t timestamp, const char* nmea, int length);
typedef void (* garden_set_cap_cb_t)(uint32_t capabilities);
typedef void (* garden_request_utc_time_cb_t)();
typedef void (* garden_xtra_report_server_cb_t)(const char* server1, const char* server2, const char* server3);
typedef void (* garden_xtra_download_request_cb_t)();
typedef void (* garden_agps_status_cb_t)(LocAGpsStatus* status);
typedef void (* garden_agps_ext_status_cb_t)(AGpsExtStatus* status);
typedef void (* garden_ni_notify_cb_t)(LocGpsNiNotification *notification);

typedef void (* garden_acquire_wakelock_cb_t)();
typedef void (* garden_release_wakelock_cb_t)();
typedef pthread_t (* garden_create_thread_cb_t)(const char* name,
                                             void (*start)(void *),
                                             void* arg);

typedef struct {
    garden_location_cb_t              garden_location_cb;
    garden_status_cb_t                garden_status_cb;
    garden_sv_status_cb_t             garden_sv_status_cb;
    garden_nmea_cb_t                  garden_nmea_cb;
    garden_set_cap_cb_t               garden_set_cap_cb;
    garden_request_utc_time_cb_t      garden_request_utc_time_cb;
    garden_xtra_download_request_cb_t garden_xtra_download_request_cb;
    garden_agps_status_cb_t           garden_agps_status_cb;
    garden_ni_notify_cb_t             garden_ni_notify_cb;
    garden_acquire_wakelock_cb_t      garden_acquire_wakelock_cb;
    garden_release_wakelock_cb_t      garden_release_wakelock_cb;
    garden_create_thread_cb_t         garden_create_thread_cb;
} garden_cb_t;

typedef struct {
    garden_location_cb_t                garden_location_cb;
    garden_status_cb_t                  garden_status_cb;
    garden_sv_status_cb_t               garden_sv_status_cb;
    garden_nmea_cb_t                    garden_nmea_cb;
    garden_set_cap_cb_t                 garden_set_cap_cb;
    garden_request_utc_time_cb_t        garden_request_utc_time_cb;
    garden_xtra_download_request_cb_t   garden_xtra_download_request_cb;
    garden_agps_ext_status_cb_t         garden_agps_ext_status_cb;
    garden_ni_notify_cb_t               garden_ni_notify_cb;
    garden_acquire_wakelock_cb_t        garden_acquire_wakelock_cb;
    garden_release_wakelock_cb_t        garden_release_wakelock_cb;
    garden_create_thread_cb_t           garden_create_thread_cb;
} garden_ext_cb_t;

class location_test_interface_base {

public:
    location_test_interface_base() {}
    virtual ~location_test_interface_base() {}
    virtual int location_init(garden_cb_t *pcb) {return 0;}
    virtual int location_init(garden_ext_cb_t *pcb) {return 0;}
    virtual int location_set_position_mode(uint32_t mode,
                                           uint32_t recurrence,
                                           uint32_t min_interval,
                                           uint32_t preferred_accuracy,
                                           uint32_t preferred_time) = 0;
    virtual int location_start_navigation() = 0;
    virtual int location_stop_navigation() = 0;
    virtual int location_close() = 0;
    virtual int location_delete_aiding_data(uint16_t flags) = 0;
    virtual int location_inject_time(int64_t time,
                                     int64_t timeReference,
                                     int uncertainty) = 0;
    virtual int location_inject_location(double latitude,
                                         double longitude,
                                         float accuracy) = 0;
    virtual int location_xtra_inject_data(char *data,
                                          int length) = 0;
    virtual int location_agps_data_conn_open(int16_t atype,
                                             const char *apn,
                                             int16_t btype) = 0;
    virtual int location_agps_data_conn_closed(int16_t atype) = 0;
    virtual int location_agps_data_conn_failed(int16_t atype) = 0;
    virtual int location_agps_set_server(int16_t atype,
                                         const char * hostname,
                                         int port) = 0;
    virtual int location_ni_respond(int notif_id,
                                    int user_response) = 0;
    virtual int location_agps_ril_update_network_availability(
                                                    int available,
                                                    const char * apn) = 0;
};

typedef location_test_interface_base * create_test_interface_func_t();
typedef void destroy_test_interface_func_t(location_test_interface_base *);

#endif
