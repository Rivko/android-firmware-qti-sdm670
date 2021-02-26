/******************************************************************************

                  Q C R I L _ D A T A _ S T U B S . H

******************************************************************************/

/******************************************************************************

  @file    qcril_data_stubs.h
  @brief   QCRIL Data test stubs Header File

  DESCRIPTION
  Header file for QCRIL_Data utility functions.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/22/11   ar         Initial version

******************************************************************************/

#ifndef __QCRIL_DATA_STUBS_H__
#define __QCRIL_DATA_STUBS_H__

#include "qmi_wds_srvc.h"
#include "qmi_embms_v01.h"
#include "wireless_data_service_v01.h"
#include "DataCommon.h"
/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

typedef enum {
  QCRIL_STUB_RESPONSE_REQ,
  QCRIL_STUB_RESPONSE_UNS,
  QCRIL_STUB_RESPONSE_MAX
} qcril_stub_response_type;


typedef void (*qcril_stub_response_cb)( qcril_stub_response_type type,
                                        void *param_ptr,
                                        void *user_data );

typedef struct{
  int result; //success/failure
  int error_cause; //valid value if result is failure
} dds_status_info;

#define bool int
#define true 1
#define false 0
#endif /* __QCRIL_DATA_STUBS_H__ */
