/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2011-2014, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  LIBULP Data Variables File

GENERAL DESCRIPTION
  This file contains the data structure and variables used for ulp module to
  make decision, eg: the unique criterias, phone context info and other engine
  start/stop state.

Copyright (c) 2011-2013 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
=============================================================================*/

#ifndef ULP_INTERNAL_H
#define ULP_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ulp_data.h>
#include <comdef.h>
/*=============================================================================================
                          LIBULP module function declaration
 =============================================================================================*/
// LIB ULP Message (via linked list message queue) Handling Functions
extern int ulp_msg_init                            (void);
extern int ulp_msg_send_generic_position_report    (enum loc_sess_status status,
                                                    LocPosTechMask     tech_mask,
                                                    UlpLocation* locationPtr,
                                                    const GpsLocationExtended* locationExtendedPtr);
extern int ulp_msg_forward_quipc_position_report   (int                quipc_error_code,
                                                    const UlpLocation* quipc_location_ptr,
                                                    int                quipc_debug_info_length,
                                                    char*              quipc_debug_info);
extern int ulp_msg_forward_quipc_coarse_position_request ();
extern int ulp_msg_send_monitor_request            (void);
extern void ulp_msg_free                           (void* msg);


// LIB ULP Module Data Management functions
extern int ulp_data_init                   (void);
extern int ulp_data_deinit                 (void);
extern int ulp_data_add_criteria           (const UlpLocationCriteria* locationCriteriaPtr);
extern int ulp_data_remove_criteria        (const UlpLocationCriteria* locationCriteriaPtr);
extern int ulp_data_reset_criteria_array   (void);
extern bool ulp_data_criteria_identical (const UlpLocationCriteria* criteriaPtr1,
                                            const UlpLocationCriteria* criteriaPtr2);

// LIB ULP module brain functions
extern int ulp_brain_select_providers              (void);
extern int ulp_brain_stop_all_providers            (void);
extern int ulp_brain_retrieve_cache_position       (UlpLocation* cached_location_ptr);
extern int ulp_brain_process_criteria_update       (void);
extern int ulp_brain_process_phone_setting_update  (void);
extern int ulp_brain_process_gnp_position_report   (UlpLocation* location_ptr,
        const GpsLocationExtended* location_extended_ptr);
extern int ulp_brain_process_quipc_position_report (int quipc_error_code,
                                                    UlpLocation* quipc_location_ptr);
extern int ulp_brain_process_gnss_position_report  (enum loc_sess_status status,
                                                    LocPosTechMask     tech_mask,
                                                    UlpLocation* gps_location_ptr,
                                                    const GpsLocationExtended* gps_location_extended_ptr);

extern int ulp_brain_process_dr_position_report (enum loc_sess_status status,
                                            LocPosTechMask       tech_mask,
                                            const UlpLocation* dr_location_ptr,
                                            const GpsLocationExtended* dr_location_extended_ptr);
extern int ulp_brain_process_zpp_position_report   (const enum loc_sess_status status,
                                                    LocPosTechMask     tech_mask,
                                                    UlpLocation* zpp_location_ptr);
extern int ulp_brain_process_geofence_position_report (UlpLocation* location_ptr);
extern int ulp_brain_process_hw_flp_positions_report (int32_t num_locations,
                                                      uint32_t* techMask,
                                                      UlpLocation * locations,
                                                      GpsLocationExtended* extendedLocations);

extern int ulp_brain_process_gnss_sv_report        (const GnssSvNotification& svNotify);
extern int ulp_brain_process_monitor_request       (void);
extern int ulp_brain_process_coarse_pos_request    (void);
extern int ulp_brain_process_dr_pos_request   (void);

extern int ulp_brain_transition_all_providers      (void);
extern int ulp_brain_process_system_update         (const UlpSystemEvent systemEvent);
extern int ulp_brain_report_gnss_status            (const uint16 status);

// ULP debug module
extern int ulp_debug_process_raw_command(const char* rawCmd, int rawCmdLength);
extern int ulp_debug_get_debug_info  (void** debug_info, int* debug_info_size);

// ULP GNSS module
extern int ulp_gnss_start_engine       (void);
extern int ulp_gnss_stop_engine        (void);
extern bool ulp_gnss_engine_running (void);
extern int ulp_gnss_set_state          (gnss_state_e_type new_state);
extern int ulp_gnss_process_gnss_status (const uint16 status);

// ULP DR module
extern int ulp_dr_start_engine (void);
extern int ulp_dr_stop_engine (void);
extern bool ulp_dr_engine_running (void);
extern int ulp_dr_set_state (dr_state_e_type new_state);

// ULP GNP (network provider) module
extern int ulp_gnp_start_engine       (void);
extern int ulp_gnp_stop_engine        (void);
extern bool ulp_gnp_engine_running (void);
extern int ulp_gnp_set_state          (gnp_state_e_type new_state);

// QUIPC module
extern int ulp_quipc_init               (void);
extern int ulp_quipc_start_engine       (void);
extern int ulp_quipc_stop_engine        (void);
extern bool ulp_quipc_engine_running (void);
extern int ulp_quipc_set_state          (quipc_state_e_type new_state);
extern int ulp_quipc_request_debug_info  (int debug_type);
extern int ulp_quipc_set_lci_selection_mode (bool user_lci_specified,
                                             unsigned char* lci_id);

// XT-WIFI module
extern int ulp_xtwifi_send_position (const char*                provider_name,
                                     enum loc_sess_status      status,
                                     LocPosTechMask             tech_mask,
                                     const UlpLocation*         locationPtr,
                                     const GpsLocationExtended* gpsLocationExtendedPtr);

// ULP monitor module
extern int ulp_monitor_init ();

// ULP ZPP (Zero Power provider) module
extern int ulp_zpp_start_engine       (void);
extern int ulp_zpp_stop_engine        (void);
extern bool ulp_zpp_engine_running    (void);

// UTIL module
extern uint64_t ulp_util_get_time_ms    (void);

// Ulp Main
extern int ulp_main_report_gnss_status (const uint16 status);

#ifdef __cplusplus
}
#endif

#endif /* ULP_INTERNAL_H */
