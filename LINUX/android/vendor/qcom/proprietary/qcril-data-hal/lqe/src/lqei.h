/*******************************************************************************

                         L Q E _ Q C C I _ W D S . C

******************************************************************************/

/******************************************************************************

  @file    lqe_qmi_ott.c
  @brief   LTE uplink/downlink quality estimation

  DESCRIPTION
  Implementation of wrapper over a bunch of QMI APIs which
  provide LTE quality estimation data to its clients.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef __LQEI_H__
#define __LQEI_H__

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ds_util.h"
#include "lqe.h"
#include "ds_cmdq.h"

#include "over_the_top_v01.h"
#include "wireless_data_service_v01.h"

#define LQE_LOG_INFO        ds_log_high
#define LQE_LOG_DEBUG       ds_log_low
#define LQE_LOG_ERROR       ds_log_err

/* Entry and Exit macros */
#define LQE_LOG_ENTRY       LQE_LOG_DEBUG( "%s: ENTRY", __func__ );
#define LQE_LOG_EXIT        LQE_LOG_DEBUG( "%s: EXIT", __func__ );
#define LQE_LOG_EXIT_RET                                 \
  if( LQE_SUCCESS == ret ) {                             \
    LQE_LOG_DEBUG( "%s: EXIT with suc", __func__ );      \
  } else if( LQE_NOT_SUPPORTED == ret ){                 \
    LQE_LOG_DEBUG( "%s: EXIT with not supported", __func__ );      \
  } else {                                               \
    LQE_LOG_DEBUG( "%s: EXIT with err", __func__ );      \
  }


#define LQE_CLIENT_MAX  (5)

typedef enum
{
  LQE_DATA_UPLINK_DIRECTION   = 0x00,
  LQE_DATA_DOWNLINK_DIRECTION = 0x01
}lqe_data_direction_e;

typedef enum
{
  LQE_QMI_SYS_EVENT_INVALID = 0,
  LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND,
  LQE_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND
} lqe_qmi_sys_event_type;

typedef struct {
  ott_downlink_throughput_info_ind_msg_v01  dl_throughput_info;
  ott_uplink_throughput_info_ind_msg_v01    ul_throughput_info;
} lqe_ott_ind_event_info_t;

typedef struct {
  lqe_ott_ind_event_info_t ind_data;
  lqe_ind_events_t         event_id;
} lqe_qmi_ott_event_ind_t;

typedef struct {
  wds_downlink_throughput_info_ind_msg_v01  dl_throughput_info;
} lqe_wds_ind_event_info_t;

typedef struct {
  lqe_wds_ind_event_info_t ind_data;
  lqe_ind_events_t         event_id;
} lqe_qmi_wds_event_ind_t;

typedef struct
{
  lqe_qmi_sys_event_type   event_id;
  void                    *user_data;
}lqe_qmi_sys_event_ind_t;

typedef struct
{
  boolean                  is_valid;

  int                      dl_report_interval;
  lqe_indications_switch_e dl_ind_switch;

  int                      ul_report_interval;
  lqe_indications_switch_e ul_ind_switch;
} lqe_start_req_t;

typedef struct lqe_client_s {
    boolean             is_valid;
    lqei_ind_f          cb_f;
    lqe_start_req_t     lqe_params;
    void               *data;
} lqe_client_t;

struct lqe_client_config {
  lqe_client_t            clients_info[LQE_CLIENT_MAX];
  pthread_mutex_t         mutex;
  lqe_qmi_sys_event_type  modem_ssr_state;
};

typedef enum {
  LQE_QMI_SERVICE_NONE = 0,
  LQE_QMI_SERVICE_WDS,
  LQE_QMI_SERVICE_OTT,
} lqe_qmi_service_type;

extern struct lqe_client_config lqe_config;

/*===========================================================================
  FUNCTION  lqe_ott_ind_cb
===========================================================================*/
/*!
@brief
  Function called from lqe ott to report asynchronous indications

@return
  int - LQE_SUCCESS on successful operation, LQE_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void lqe_process_cmd_cb
(
  struct ds_cmd_s *ds_cmd,
  void * data
);
#endif /*__LQEI_H__*/
