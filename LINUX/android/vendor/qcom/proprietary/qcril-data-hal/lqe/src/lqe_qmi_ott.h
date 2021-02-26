/******************************************************************************

                         L Q E _ Q M I _ O T T . H

******************************************************************************/

/******************************************************************************

  @file    lqe_qmi_ott.h
  @brief   LTE uplink/downlink quality estimation headers

  DESCRIPTION
  Headers for wrapper over a bunch of QMI APIs which
  provide LTE quality estimation data to its clients.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __LQE_QMI_OTT_H__
#define __LQE_QMI_OTT_H__

/*===========================================================================
  FUNCTION  lqe_release_ott_client
===========================================================================*/
/*!
@brief
  Release QMI handle
*/
/*=========================================================================*/
void lqe_release_ott_client
(
 int  index
);

/*===========================================================================
  FUNCTION  lqe_int_ott_client
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_int_ott_client
(
  int index
);

/*===========================================================================
  FUNCTION  lqe_ott_get_downlink_throughput_info_params
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_ott_get_downlink_throughput_info_params
(
  int qmi_client_handle,
  int *dl_interval_timer
);

/*===========================================================================
  FUNCTION  lqe_ott_set_throughput_info_params
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_ott_set_throughput_info_params
(
  int                   qmi_client_handle,
  lqe_data_direction_e  direction,
  int                   report_interval_timer
);

/*===========================================================================
  FUNCTION  lqe_ott_toggle_throughput_indications
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_ott_toggle_throughput_indications
(
  int                         qmi_client_handle,
  lqe_data_direction_e        direction,
  lqe_indications_switch_e    ind_switch
);

/*===========================================================================
  FUNCTION  lqe_ott_set_throughput_report_period
===========================================================================*/
/*!
@brief
  Set throughput report perioud
*/
/*===========================================================================*/
int lqe_ott_set_throughput_report_period
(
  int                   lqe_client_handle,
  lqe_data_direction_e  direction,
  int                   report_interval_timer
);

/*===========================================================================
  FUNCTION  lqe_ott_get_downlink_throughput_info
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_ott_get_downlink_throughput_info
(
  int qmi_client_handle,
  int *dl_allowed_rate,
  int *confidence_level
);

/*===========================================================================
  FUNCTION  lqe_ott_get_downlink_throughput_info_params
===========================================================================*/
/*!
@brief
  Get downlink throughput report status
*/
/*=========================================================================*/
int lqe_ott_get_downlink_throughput_report_status
(
  int                                      lqe_client_handle,
  lqe_downlink_throughput_report_status   *status
);

/*===========================================================================
  FUNCTION  lqe_ott_get_uplink_throughput_info
===========================================================================*/
/*!
@brief
  Initialize QMI hanule
*/
/*=========================================================================*/
int lqe_ott_get_uplink_throughput_info
(
  int qmi_client_hanule,
  int *ul_allowed_rate,
  int *confidence_level
);

/*===========================================================================
  FUNCTION  lqe_ott_get_uplink_throughput_info_params
===========================================================================*/
/*!
@brief
  Get uplink throughput report status
*/
/*=========================================================================*/
int lqe_ott_get_uplink_throughput_report_status
(
  int                                      lqe_client_hanule,
  lqe_uplink_throughput_report_status   *status
);

/*===========================================================================
  FUNCTION  lqe_ott_init
===========================================================================*/
/*!
@brief
  Initialize this module. This will be called only once.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqe_ott_init
(
  void
);

/*===========================================================================
  FUNCTION  lqe_ott_deinit
===========================================================================*/
/*!
@brief
  DeInitialize this module. This will be called only once.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqe_ott_deinit
(
  void
);

#endif /*__LQE_QMI_OTT_H__*/
