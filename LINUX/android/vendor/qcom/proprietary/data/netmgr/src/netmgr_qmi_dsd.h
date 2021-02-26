/*****************************************************************************

    N E T M G R R _ Q M I _D S D . H


/******************************************************************************

  @file    netmgr_qmi_dsd.h
  @brief   Network Manager QMI System determindation service header

  DESCRIPTION
  Network Manager QMI system determination header

******************************************************************************/
/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __NETMGR_QMI_DSD_H__
#define __NETMGR_QMI_DSD_H__

#include <stdint.h>
#include "qmi_client.h"
#include "data_system_determination_v01.h"


extern qmi_client_type         netmgr_dsd_clnt_hndl;

/*===========================================================================

  FUNCTION:  netmgr_qmi_dsd_init

===========================================================================*/
/*!
    @brief
    Initialize the QMI DSD services for Netmgr

    @return
    none
*/
/*=========================================================================*/
 void netmgr_qmi_dsd_init
(
  void
);

/*===========================================================================
  FUNCTION  netmgr_qmi_dsd_release_client
===========================================================================*/
/*!
@brief
  Release QMI DSD handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on failure

*/
/*=========================================================================*/
int netmgr_qmi_dsd_release_client
(
  void
);


#endif /* __NETMGR_QMI_DSD_H__ */
