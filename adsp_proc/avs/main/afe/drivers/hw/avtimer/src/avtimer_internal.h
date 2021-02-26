/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_internal.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/06/12   fzhou      Created file.
========================================================================== */
/**
@file avtimer_internal.h

@brief This file contains avtimer structs that is required for the server and client
*/

#ifndef AVTIMER_INTERNAL_H
#define AVTIMER_INTERNAL_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "avtimer_hal.h"

#include "hwd_devcfg.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

#define MAX_NUM_OF_AVTIMER_CLIENTS        16   

typedef enum 
{
   AVTIMER_HW_TYPE_AVTIMER = 0,
   AVTIMER_HW_TYPE_AVSYNC_BT,
   AVTIMER_HW_TYPE_AVSYNC_FM,
   AVTIMER_HW_TYPE_AVSYNC_INVALID = 0xFFFFFFFF
} avtimer_hw_type_t;

typedef struct
{
   int16_t avsync_bt_job_cnt;
   /**< Job count of Bluetooth AVsync module */
   int16_t avsync_fm_job_cnt;
   /**< Job count of FM AVsync module */
   uint64_t client_name;         
   /**< AV timer driver client name. */
   int16_t index;
   /**< AV timer client index in client info array of AVtimer global structure */
   int16_t job_open_cnt;
   /**< Number of times AVtimer driver opened under same client name and same hardware */
   avtimer_hw_index which_avtimer;
   /**< Which AVtimer Hardware 
     Supported Values:
    - #AVTIMER_DRV_OPEN_FLAG_DEFAULT_AVTIMER 
    - #AVTIMER_DRV_OPEN_FLAG_ATIMER_1 
    - #AVTIMER_DRV_OPEN_FLAG_ATIMER_2 */
} avtimer_client_info_t;

typedef struct
{
   int32_t avtimer_job_cnt[AVTIMER_HW_INSTANCE_INDEX_MAX];
   int32_t avsync_bt_job_cnt;
   int32_t avsync_fm_job_cnt;
} mmpm_info_t;

typedef struct
{
   bool_t is_initialized;
   mmpm_info_t mmpm_info;
   HwdAvtimerPropertyType avt_prop;
   qurt_elite_mutex_t lock_hw_access;
   avtimer_client_info_t* client_info_ptr[MAX_NUM_OF_AVTIMER_CLIENTS];
   qurt_mem_region_t avt_qmem_region; /* avtimer register mapping region */
   int16_t avsync_hw_reset;
} avtimer_drv_ctx_t;

/**
 * avtimer_server_get_virt_addr
 *
 *
 *
 * @param[in] reg_addr_phy physical address
 * @param[in] page_size    page size for mapping
 *
 * @return uint32  virtual address
 */
uint32 avtimer_server_get_virt_addr(uint32 reg_addr_phy, size_t page_size);

ADSPResult enable_avtimer_hw(avtimer_client_info_t* client_info_ptr);
ADSPResult disable_avtimer_hw(avtimer_client_info_t* client_info_ptr);

ADSPResult enable_avsync_hw(avtimer_hw_type_t type);
ADSPResult disable_avsync_hw(avtimer_hw_type_t type);

/** @} */ /* end_addtogroup avtimer */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_INTERNAL_H

