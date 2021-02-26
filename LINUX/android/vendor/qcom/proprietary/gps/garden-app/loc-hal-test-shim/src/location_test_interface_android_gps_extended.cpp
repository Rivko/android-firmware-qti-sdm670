/* Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "location_test_interface_android_gps_extended.h"
#include <stdio.h>
#include <loc_extended.h>

const LocGpsInterface *pGpsInterface = NULL;
LocAGpsRilInterface *pAgpsRilInterface = NULL;
static garden_ext_cb_t *garden_ext_cb_ptr = NULL;

extern "C" const LocGpsInterface *loc_eng_gps_get_hardware_interface ();
void test_android_gps_location_cb (LocGpsLocation * location){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_location_cb) {
        garden_ext_cb_ptr->garden_location_cb(location);
    }
}
void test_android_gps_status_cb (LocGpsStatus * status){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_status_cb) {
        garden_ext_cb_ptr->garden_status_cb(status);
    }
}
void test_android_gps_sv_status_cb (LocGpsSvStatus * sv_info){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_sv_status_cb) {
        garden_ext_cb_ptr->garden_sv_status_cb(sv_info);
    }
}
void test_android_gps_nmea_cb (LocGpsUtcTime timestamp, const char *nmea, int length){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_nmea_cb) {
        garden_ext_cb_ptr->garden_nmea_cb(timestamp, nmea, length);
    }
}
void test_android_gps_set_capabilities_cb (uint32_t capabilities){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_set_cap_cb) {
        garden_ext_cb_ptr->garden_set_cap_cb(capabilities);
    }
}
void test_android_gps_xtra_time_req_cb (){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_request_utc_time_cb) {
        garden_ext_cb_ptr->garden_request_utc_time_cb();
    }
}
void test_android_gps_xtra_download_req_cb (){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_xtra_download_request_cb) {
        garden_ext_cb_ptr->garden_xtra_download_request_cb();
    }
}

void test_android_agps_ext_status_cb (AGpsExtStatus * status){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_agps_ext_status_cb) {
        garden_ext_cb_ptr->garden_agps_ext_status_cb(status);
    }
}

void test_android_gps_ni_notify_cb(LocGpsNiNotification *notification, bool esEnabled){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_ni_notify_cb) {
        garden_ext_cb_ptr->garden_ni_notify_cb(notification);
    }
}

void test_android_gps_acquire_wakelock_cb (){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_acquire_wakelock_cb) {
        garden_ext_cb_ptr->garden_acquire_wakelock_cb();
    }
}
void test_android_gps_release_wakelock_cb (){
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_release_wakelock_cb) {
        garden_ext_cb_ptr->garden_release_wakelock_cb();
    }
}

pthread_t test_android_gps_create_thread_cb (const char *name,
                                             void (*start) (void *),
                                             void *arg) {
    pthread_t tid = -1;
    if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_create_thread_cb) {
         tid = garden_ext_cb_ptr->garden_create_thread_cb(name, start, arg);
    }
    return tid;
}

void test_report_xtra_server_callback(const char* server1, const char* server2, const char* server3)
{
    //garden_print ("## report_xtra_server_callback ##:");
    char buf[] = "## report_xtra_server_callback ##:";
    fprintf(stderr,"GARDEN: %s\n",buf);
    //if(garden_ext_cb_ptr != NULL && garden_ext_cb_ptr->garden_xtra_report_server_cb) {
    //    garden_ext_cb_ptr->garden_xtra_report_server_cb(server1, server2, server3);
    //}
}

LocGpsCallbacks myCallbacks = {
    sizeof (LocGpsCallbacks),
    test_android_gps_location_cb,
    test_android_gps_status_cb,
    test_android_gps_sv_status_cb,
    test_android_gps_nmea_cb,
    test_android_gps_set_capabilities_cb,
    test_android_gps_acquire_wakelock_cb,
    test_android_gps_release_wakelock_cb,
    test_android_gps_create_thread_cb,
    test_android_gps_xtra_time_req_cb
};

GpsXtraExtCallbacks myGpsXtraExtCallbacks = {
    test_android_gps_xtra_download_req_cb,
    test_android_gps_create_thread_cb,
    test_report_xtra_server_callback
};

AGpsExtCallbacks myAGpsExtCallbacks = {
    test_android_agps_ext_status_cb,
    test_android_gps_create_thread_cb
};

GpsExtCallbacks myGpsExtCallbacks = {
    sizeof(GpsExtCallbacks),
    test_android_gps_set_capabilities_cb,
    test_android_gps_acquire_wakelock_cb,
    test_android_gps_release_wakelock_cb,
    test_android_gps_create_thread_cb,
    test_android_gps_xtra_time_req_cb
};

GpsNiExtCallbacks myGpsNiExtCallbacks = {
    test_android_gps_ni_notify_cb
};

extern "C" location_test_interface_base * create_test_interface() {
    return new location_test_interface_android_gps_extended;
}

extern "C" void destroy_test_interface(location_test_interface_base *p) {
    delete p;
}

// gps test interface

location_test_interface_android_gps_extended::location_test_interface_android_gps_extended() {
}

location_test_interface_android_gps_extended::~location_test_interface_android_gps_extended() {
}

int location_test_interface_android_gps_extended::location_init(garden_ext_cb_t *pcb) {

    pGpsInterface = (LocGpsInterface *) loc_eng_gps_get_hardware_interface ();

    if (NULL == pGpsInterface) {
        return -1;
    }

    if(pcb != NULL) {
        garden_ext_cb_ptr = pcb;
    }

    pGpsInterface->init(&myCallbacks);

    loc_extended_init(&myGpsExtCallbacks);
    loc_extended_agps_init(&myAGpsExtCallbacks);
    loc_extended_ni_init(&myGpsNiExtCallbacks);

    pAgpsRilInterface = (LocAGpsRilInterface *) pGpsInterface->get_extension (LOC_AGPS_RIL_INTERFACE);
    if (NULL == pAgpsRilInterface) {
        //garden_print("AGPS RIL Interface is NULL...but proceeding");
    }

    loc_extended_xtra_init(&myGpsXtraExtCallbacks);

    return 0;
}

int location_test_interface_android_gps_extended::location_set_position_mode(
                                           uint32_t mode,
                                           uint32_t recurrence,
                                           uint32_t min_interval,
                                           uint32_t preferred_accuracy,
                                           uint32_t preferred_time) {

    if (NULL != pGpsInterface) {
        return pGpsInterface->set_position_mode (mode, recurrence, min_interval,
                                                 preferred_accuracy,
                                                 preferred_time);
    }
    return -1;
}

int location_test_interface_android_gps_extended::location_start_navigation() {
    if (NULL != pGpsInterface) {
      return pGpsInterface->start();
    }
    return -1;
}

int location_test_interface_android_gps_extended::location_stop_navigation() {
    if(pGpsInterface != NULL) {
        pGpsInterface->stop ();
    }
    return 0;
}
int location_test_interface_android_gps_extended::location_close() {

    if(pGpsInterface != NULL) {
        pGpsInterface->cleanup ();
    }
    loc_extended_cleanup();
    pGpsInterface = NULL;
    pAgpsRilInterface = NULL;
    garden_ext_cb_ptr = NULL;
    return 0;
}

int location_test_interface_android_gps_extended::location_delete_aiding_data(uint16_t flags) {

    if(pGpsInterface != NULL) {
        if(flags != 0) {
            pGpsInterface->delete_aiding_data (flags);
        }
    }
    return 0;
}

int location_test_interface_android_gps_extended::location_inject_time(
                                                         int64_t time,
                                                         int64_t timeReference,
                                                         int uncertainty) {

    int rc = 0;

    rc = loc_extended_inject_time(time, timeReference, uncertainty);

    return rc;
}

int location_test_interface_android_gps_extended::location_inject_location(
                                         double latitude,
                                         double longitude,
                                         float accuracy) {
    if(pGpsInterface != NULL) {
            pGpsInterface->inject_location(latitude, longitude, accuracy);
    }
    return 0;
}

int location_test_interface_android_gps_extended::location_xtra_inject_data(
                                             char *data,
                                             int length) {

    int rc = 0;

    loc_extended_xtra_inject_data(data, length);

    return rc;
}

int location_test_interface_android_gps_extended::location_agps_data_conn_open(
                                                      int16_t atype,
                                                      const char *apn,
                                                      int16_t btype) {

    loc_extended_agps_open(atype, apn, btype);

    return 0;
}

int location_test_interface_android_gps_extended::location_agps_data_conn_closed
                                                         (int16_t atype) {

    loc_extended_agps_closed(atype);

    return 0;
}

int location_test_interface_android_gps_extended::location_agps_data_conn_failed
                                                         (int16_t atype) {

    loc_extended_agps_open_failed(atype);

    return 0;
}

int location_test_interface_android_gps_extended::location_agps_set_server(
                                         int16_t atype,
                                         const char * hostname,
                                         int port) {

    int rc = 0;

    rc = loc_extended_agps_set_server(atype, hostname, port);

    return rc;
}

int location_test_interface_android_gps_extended::location_ni_respond(int notif_id,
                                                             int user_response) {

    loc_extended_ni_respond(notif_id, user_response);

    return 0;
}


int location_test_interface_android_gps_extended::location_agps_ril_update_network_availability(
                                                          int available,
                                                          const char * apn) {

    if(pAgpsRilInterface != NULL) {
        pAgpsRilInterface->update_network_availability(available, apn);
    }
    return 0;
}


