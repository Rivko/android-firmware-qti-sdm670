/**
@file sp_common.h

@brief

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/algos/sp/inc/sp_common.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef SP_COMMON_H_
#define SP_COMMON_H_


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "fbsp_common_api.h"


#define NUM_CXDATA_BUFFERS             3
#define TH_CX_BUF_UNDERRUN_TIME_THRESH 10000
#define EX_CX_BUF_UNDERRUN_TIME_THRESH 64000


/** @brief To represent the vi spkr type to the Rx processing.
 */
typedef enum sp_spkr_cfg_t
{
   SPKR_L   = 0,
   SPKR_R,
   SPKR_LR, /* cx data will always be of this type when 2 spkrs are there, feedback channel un-map would be
   done when we receive the buffers and set into this format for 2 spkr configuration */
   SPKR_MAX = (0xFFFFFFFF)  /* Maximum value to ensure that the enum is 4 bytes long */
}sp_spkr_cfg_t;

/** @brief To represent the vi client type to the Rx processing.
 */
typedef enum sp_vi_client_type_t
{
   THERM_VI = 0,
   EXCUR_VI = 1,
   MAX_VALUE_VI = (0xFFFFFFFF)  /* Maximum value to ensure that the enum is 4 bytes long */
}sp_vi_client_type_t;

/**
 * Payload for control data received from Tx thermal or
 * excursion control blocks by Speaker protection Rx processing
 */
typedef struct sp_vi_cx_data_payload_t
{
   /**< VI client type, TH or Ex */
   sp_vi_client_type_t client_type;
   /**< Spkr config */
   sp_spkr_cfg_t       spkr_cfg;
   /**< Beginning of the control data buffer. Rest of the payload depends upon VI
    *   client type */
   int8_t                 *cx_data_ptr[MAX_FBSP_SPKRS];
}sp_vi_cx_data_payload_t;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* SP_COMMON_H_ */
