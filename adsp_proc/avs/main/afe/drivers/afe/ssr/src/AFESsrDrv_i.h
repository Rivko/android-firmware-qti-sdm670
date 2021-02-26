/*========================================================================
   This file contains AFE SSR handler implementation

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: 
 ====================================================================== */

#ifndef _AFE_SSR_DRV_I_H_
#define _AFE_SSR_DRV_I_H_

/*==========================================================================
  Include files
  ========================================================================== */

#include <stringl.h>
#include "hwd_devcfg.h"
#include "LPASS_ADDRESS_FILE.h"
#include "afe_lpm.h"

#if !defined(SIM)
#include "err.h"
#endif


/*----------------------------------------------------------------------------
 * Macro definition
 * -------------------------------------------------------------------------*/

typedef enum ssr_info_guard
{
   AFE_SSR_GUARD_WORD_HEAD = 0xAFE0AAAAUL,
   AFE_SSR_GUARD_WORD_TAIL = 0xAFE0BBBBUL
}ssr_info_guard_t;

typedef enum lpass_core_pwr_status
{
   AFE_SSR_LPASS_CORE_OFF = 0xAFE0CCCCUL,
   AFE_SSR_LPASS_CORE_ON = 0xAFE0DDDDUL
}lpass_core_pwr_status_t;


/*----------------------------------------------------------------------------
 * Structure
 * -------------------------------------------------------------------------*/

typedef struct afe_ssr_lpaif_info
{
   uint32_t             core_pwr_status;                 
   /**< LPAIF core power ON status */

   uint32_t             rddma_status;
   /**< LPAIF DMA power ON/OFF status; each bit is corresponding to read  dma channel number*/

   uint32_t             wrdma_status;                      
   /**< LPAIF DMA power ON/OFF status;; each bit is corresponding to write dma channel number */ 
} afe_ssr_lpaif_info_t;


typedef struct afe_ssr_lpm_info
{ 
   uint32_t             lpm_pwr_status;                  
   /**< LPM power ON status */ 
   uint64_t lpm_virt_addr;
   /**< LPM virt address */
   uint32_t             lpm_size;                        
   /**< LPM size in bytes */ 
}afe_ssr_lpm_info_t;


typedef struct afe_ssr_info
{
   uint32_t                ssr_info_guard_head;             
   /**< Header guard word */ 
   afe_ssr_lpaif_info_t    lpaif_info;                      
   /**< LPAIF info */ 
   afe_ssr_lpm_info_t      lpm_info;  
   /**< LPM info */ 
   uint32_t                ssr_info_guard_tail;             
   /**< Tail guard word for SSR info */ 
   uint32_t                lpm_buf_guard_head;              
   /**< Header guard word for LPM buf */ 
#ifdef AFE_SSR_DUMP_LPM_MEM
   uint8_t                 *lpm_buf_ptr;
// david need to be allocated based on dev config or so.
   /**< LPM buffer */ 
#endif //AFE_SSR_DUMP_LPM_MEM   
   uint32_t                lpm_buf_guard_tail;              
   /**< Tail guard word for LPM buf */ 
   
} afe_ssr_info_t;

/*----------------------------------------------------------------------------
 * Externs
 * -------------------------------------------------------------------------*/
extern afe_ssr_info_t      afe_ssr_info_global;


/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
 
typedef void (*afe_ssr_handler_fp)(void);

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#if !defined(SIM)
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb);
#endif

#ifdef __cplusplus
}
#endif //__cplusplus

/*==========================================================================
  Function declarations
  ========================================================================== */
/**
  AFE SSR handler callback

  @param[in]  None

  @result
  None

  @dependencies
  None.
*/
void afe_ssr_handler_cb(void);

#endif /* _AFE_SSR_DRV_I_H_ */

