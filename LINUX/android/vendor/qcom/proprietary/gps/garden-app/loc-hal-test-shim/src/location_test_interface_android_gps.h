/* Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef LOCATION_TEST_INTERFACE_GPS_H
#define LOCATION_TEST_INTERFACE_GPS_H

#include "location_test_interface_base.h"

class location_test_interface_android_gps: public location_test_interface_base {

public:
    location_test_interface_android_gps();
    ~location_test_interface_android_gps();
    int location_init(garden_cb_t *pcb);
    int location_set_position_mode(uint32_t mode,
                                   uint32_t recurrence,
                                   uint32_t min_interval,
                                   uint32_t preferred_accuracy,
                                   uint32_t preferred_time);
    int location_start_navigation();
    int location_stop_navigation();
    int location_close();
    int location_delete_aiding_data(uint16_t flags);
    int location_inject_time(int64_t time,
                             int64_t timeReference,
                             int uncertainty);
    int location_inject_location(double latitude,
                                 double longitude,
                                 float accuracy);
    int location_xtra_inject_data(char *data,
                                  int length);
    int location_agps_data_conn_open(int16_t atype,
                                     const char *apn,
                                     int16_t btype);
    int location_agps_data_conn_closed(int16_t atype);
    int location_agps_data_conn_failed(int16_t atype);
    int location_agps_set_server(int16_t atype,
                                 const char * hostname,
                                 int port);
    int location_ni_respond(int notif_id,
                            int user_response);
    int location_agps_ril_update_network_availability(
                                                     int available,
                                                     const char * apn);
};


extern "C" location_test_interface_base * create_test_interface();

extern "C" void destroy_test_interface(location_test_interface_base *p);


#endif
