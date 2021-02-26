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

  Copyright (c) 2011 Qualcomm Technologies, Inc. All Rights Reserved

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

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

typedef enum {
  QCRIL_STUB_RESPONSE_REQ,
  QCRIL_STUB_RESPONSE_UNS,
  QCRIL_STUB_RESPONSE_MAX
} qcril_stub_response_type;

/*! @brief Preferred Data Technology
*/
typedef enum
{
  QCRIL_ARB_PREF_DATA_TECH_INVALID = -1,
  QCRIL_ARB_PREF_DATA_TECH_UNKNOWN = 0,
  QCRIL_ARB_PREF_DATA_TECH_FIRST   = QCRIL_ARB_PREF_DATA_TECH_UNKNOWN,
  QCRIL_ARB_PREF_DATA_TECH_CDMA    = 1,
  QCRIL_ARB_PREF_DATA_TECH_EVDO    = 2,
  QCRIL_ARB_PREF_DATA_TECH_GSM     = 3,
  QCRIL_ARB_PREF_DATA_TECH_UMTS    = 4,
  QCRIL_ARB_PREF_DATA_TECH_EHRPD   = 5,
  QCRIL_ARB_PREF_DATA_TECH_LTE     = 6,
  QCRIL_ARB_PREF_DATA_TECH_TDSCDMA = 7,
  QCRIL_ARB_PREF_DATA_TECH_MAX
} qcril_arb_pref_data_tech_e_type;


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
