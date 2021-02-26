/******************************************************************************

                         L Q E _ Q C C I _ W D S . C

******************************************************************************/

/******************************************************************************

  @file    lqe_qmi_wds.c
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

#ifndef __LQE_QMI_WDS_H__
#define __LQE_QMI_WDS_H__

/*===========================================================================
  FUNCTION  lqe_release_wds_client
===========================================================================*/
/*!
@brief
  Release QMI handle
*/
/*=========================================================================*/
void lqe_release_wds_client
(
 int  index
);

/*===========================================================================
  FUNCTION  lqe_int_wds_client
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_int_wds_client
(
  int index
);

/*===========================================================================
  FUNCTION  lqe_wds_get_downlink_throughput_info_params
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_wds_get_downlink_throughput_info_params
(
  int qmi_client_handle,
  int *dl_interval_timer
);

/*===========================================================================
  FUNCTION  lqe_wds_set_throughput_info_params
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_wds_set_throughput_info_params
(
  int                   qmi_client_handle,
  lqe_data_direction_e  direction,
  int                   report_interval_timer
);

/*===========================================================================
  FUNCTION  lqe_wds_toggle_throughput_indications
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_wds_toggle_throughput_indications
(
  int                         qmi_client_handle,
  lqe_data_direction_e        direction,
  lqe_indications_switch_e    ind_switch
);

/*===========================================================================
  FUNCTION  lqe_wds_get_downlink_throughput_info
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_wds_get_downlink_throughput_info
(
  int qmi_client_handle,
  int *dl_allowed_rate,
  int *confidence_level
);

/*===========================================================================
  FUNCTION  lqe_wds_get_downlink_throughput_info_params
===========================================================================*/
/*!
@brief
  Get downlink throughput report status
*/
/*=========================================================================*/
int lqe_wds_get_downlink_throughput_report_status
(
  int                                          lqe_client_handle,
  lqe_downlink_throughput_report_status       *status
);

/*===========================================================================
  FUNCTION  lqe_wds_set_throughput_report_period
===========================================================================*/
/*!
@brief
  Set throughput report perioud
*/
/*===========================================================================*/
int lqe_wds_set_throughput_report_period
(
  int                   lqe_client_handle,
  lqe_data_direction_e  direction,
  int                   report_interval_timer
);

/*===========================================================================
  FUNCTION  lqe_wds_init
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
lqe_wds_init
(
  void
);

/*===========================================================================
  FUNCTION  lqe_wds_deinit
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
lqe_wds_deinit
(
  void
);

#endif /*__LQE_QMI_WDS_H__*/
