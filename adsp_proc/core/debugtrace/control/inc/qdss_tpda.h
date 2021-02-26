#ifndef QDSS_TPDA_H
#define QDSS_TPDA_H

/*=============================================================================

FILE:         qdss_tpda.h

DESCRIPTION:  TPDA interface functions

================================================================================
            Copyright (c) 2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "DALStdDef.h"

#define QDSS_TPDA_ID_LPASS  0x0B

/*TPDA parameter ids */
#define TPDA_CONTROL_SET_FREQ_VAL                  0
#define TPDA_CONTROL_GENERATE_FREQ_PKT             1
#define TPDA_CONTROL_GLOBAL_FLUSH_REQ              2
#define TPDA_CONTROL_FREQ_PKT_TIMESTAMP_ON_OFF     3
#define TPDA_CONTROL_ASYNC_PERIOD                  4
#define TPDA_CONTROL_GEN_FLAG_PKT_VIA_CTI_ON_OFF   5
#define TPDA_CONTROL_GEN_FREQ_PKT_VIA_CTI_ON_OFF   6
#define TPDA_CONTROL_GEN_SYNC_PKT_VIA_CTI_ON_OFF   7


/*-------------------------------------------------------------------------*/

/**
  @brief Retrieves a TPDA parameter 
  
  @param              tpda_id [in ] :  TPDA id
  @param             param_id [in ] :  Prameter id
  @param            param_val [out ]:  Pointer to a parmeter value location.
                                       On succes this location will contain
                                       the result

  @return 0 if successful, error code otherwise
 */
int qdss_tpda_get_param(uint8 tpda_id, uint8 param_id, uint32 *param_val);

/*-------------------------------------------------------------------------*/

/**
  @brief  Configures a TPDA parameter
  
  @param              tpda_id [in ] :  TPDA id
  @param             param_id [in ] :  Prameter id
  @param            param_val [in ] :  Parmeter value

  @return 0 if successful, error code otherwise
 */
int qdss_tpda_set_param(uint8 tpda_id, uint8 param_id, uint32 param_val);


#endif //QDSS_TPDA_H
