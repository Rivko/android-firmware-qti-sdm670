/******************************************************************************

                       N E T M G R _ Q M I _ W D A . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_wda.h
  @brief   Netmanager QMI Wireless Data Administrative helper

  DESCRIPTION
  Netmanager QMI Wireless Data Administrative helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_QMI_WDA_H__
#define __NETMGR_QMI_WDA_H__

/* Need below include for wda_set_data_format_req_msg_v01 */
#include "wireless_data_administrative_service_v01.h"

#define NETMGR_WDA_SUCCESS        0
#define NETMGR_WDA_BAD_ARGUMENTS  1
#define NETMGR_WDA_QMI_FAILURE    2
#define NETMGR_WDA_MODEM_REJECTED 3

#define NETMGR_WDA_UL_QMAP      0x05
#define NETMGR_WDA_UL_QMAPV3    0x07
#define NETMGR_WDA_UL_QMAPV4    0x08
#define NETMGR_WDA_DL_QMAP      0x05
#define NETMGR_WDA_DL_QMAPV3    0x07
#define NETMGR_WDA_DL_QMAPV4    0x08

int netmgr_wda_set_data_format(const char *dev_id,
                               wda_set_data_format_req_msg_v01 *request,
                               wda_set_data_format_resp_msg_v01 *response);

int netmgr_wda_set_qmap_settings(const char *dev_id,
                                 wda_set_qmap_settings_req_msg_v01  *request,
                                 wda_set_qmap_settings_resp_msg_v01 *response);

/*===========================================================================
  FUNCTION netmgr_wda_get_ul_agg_modem_limits
===========================================================================*/
/*!
@brief
  Gets maximum supported UL aggregation parameters from the modem
  Values are set to 0 if QMI messaging fails

@arg *dev_id Name of device to call WDA_GET_DATA_FORMAT on
@arg *ul_data_aggregation_max_datagrams
@arg *ul_data_aggregation_max_size

@return
  int - NETMGR_WDA_SUCCESS on successful operation,
  NETMGR_WDA_BAD_ARGUMENTS if called with null/invalid arguments
  NETMGR_WDA_QMI_FAILURE if there was an error sending QMI message
*/
/*=========================================================================*/
int
netmgr_wda_get_ul_agg_modem_limits
(
  const char *dev_id,
  int *ul_data_aggregation_max_datagrams,
  int *ul_data_aggregation_max_size
);

#endif /* __NETMGR_QMI_WDA_H__ */
