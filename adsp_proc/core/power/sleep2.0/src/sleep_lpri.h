#ifndef _SLEEP_LPRI_H_
#define _SLEEP_LPRI_H_
/*==============================================================================
  FILE:         sleep_lpri.h
  
  OVERVIEW:     This file provides the internal functions and data used for
                sleep_lpr functionality.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/sleep_lpri.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "sleep_lpr.h"
#include "npa_resource.h"

/*==============================================================================
                             MACRO DEFINITIONS
 =============================================================================*/
/* ID for querying the list of registered LPRs.  List will persist in a valid
 * state until returned using SLEEP_LPR_RELEASE_RESOURCE_LIST. */
#define SLEEP_LPR_QUERY_RESOURCES (NPA_QUERY_RESERVED_END+1)

/* Internal query */
#define SLEEP_LPR_REGISTER        (NPA_QUERY_RESERVED_END+2)

/* Structure that stores information of a low power resource
 * and its handle. */
typedef struct lpr_registration_info_s
{
  sleep_lpr         *lpr;
  npa_client_handle client;
}lpr_registration_info;

/*==============================================================================
                             EXTERNAL DECLARATIONS
 =============================================================================*/
extern volatile uint32 g_SleepNumOfFreqs;

#ifdef __cplusplus
}
#endif

#endif /* _SLEEP_LPRI_H_ */

