#ifndef __HAL_QDSS_TPDM_H__
#define __HAL_QDSS_TPDM_H__

/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tpda/hal/inc/hal_qdss_tpda.h#1 $
  ===========================================================================
*/

#include "HALcomdef.h"

#define PERIOD_BYTES 0
#define PERIOD_2POWN 1


void HAL_qdss_tpda_SetBaseAddress(uint32 virtAddr);
void HAL_qdss_tpda_port_enable_disable(uint32 tpda_port_num,uint32 setbit);
void HAL_qdss_tpda_port_set_cmbesize(uint32 tpda_port_num,uint8 esize);
void HAL_qdss_tpda_set_freq_val(uint32 val);
void HAL_qdss_tpda_freq_pkt_req(void);
void HAL_qdss_tpda_global_flush_req(void);
void HAL_qdss_tpda_control_freq_timestamp_on_off(uint32 setbit);
void HAL_qdss_tpda_set_atid(uint32 atid);
void HAL_qdss_tpda_set_sync_pkt_periodicity(uint32 mode,uint32 bytes);
void HAL_qdss_tpda_set_avfreqen(uint8 setbit);
void HAL_qdss_tpda_flag_pkt_via_CTI(uint32 setbit);
void HAL_qdss_tpda_freq_pkt_via_CTI(uint32 setbit);
void HAL_qdss_tpda_sync_pkt_via_CTI(uint32 setbit);
void HAL_qdss_tpda_unlock_access(void);
void HAL_qdss_tpda_lock_access(void);

#endif /* __HAL_QDSS_TPDM_H__ */

