#ifndef __PM_FG_MEM_IF_H__
#define __PM_FG_MEM_IF_H__

/*! \file pm_fg_mem_if.h
 *  \n
 *  \brief  PMIC-FG MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC FG Fule Gauge memory interface module.
 *
 *  Copyright (c) 2016 -2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/31/17   va      Adding DMA access for profile load
12/19/16   sm      Added API to get FG PmicIndex
11/04/16   va      Adding read API for HW status 
10/05/16   va      IMA exception handling fix apis
08/24/16   aab     Added pm_fg_mem_if_get_dma_err_sts () and pm_fg_mem_if_dma_clear_log()
04/21/16   va      Updating IMA APIs
03/22/16   va      Update to make FG Protocol compile for WP and add necessary APIs
01/12/16   al      Initial Release 
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Common Interrupt Prototype */
typedef enum
{
  PM_FG_MEM_IF_IMA_RDY,    /*Behaves as Ready/End of Transaction depending on configuration*/
  PM_FG_MEM_IF_MEM_XCP,    /*Interleave Memory Access(IMA) exception*/
  PM_FG_MEM_IF_DMA_GNT,    /*Dma Granted*/
  PM_FG_MEM_IF_FG_PEEKMUX, /*peek mux output*/
  PM_FG_MEM_IF_IRQ_INVALID /*Invalid fg_mem_if irq*/
} pm_fg_mem_if_irq_type;

typedef enum{
  /* Selects the MID that will receive the interrupt */
  PM_FG_MEMIF_INT_MID_SEL_MID0,
  PM_FG_MEMIF_INT_MID_SEL_MID1,
  PM_FG_MEMIF_INT_MID_SEL_MID2,
  PM_FG_MEMIF_INT_MID_SEL_MID3,
  PM_FG_MEMIF_INT_MID_SEL_INVALID
}pm_fg_mem_if_int_mid_sel;

/* End of Common Interrupt */

typedef enum
{
  /* 0:DMA 1:IMA; change this bit when MEM_ACCESS_REQ = 0
      0x0 : SELECT_DMA
      0x1 : SELECT_IMA */
  PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT= 0X05,

  /* memory access request: Enables RIF memory interface and the RIF Memory Access Mode
      0x0 : MEM_ACCESS_REQ_FALSE
      0x1 : MEM_ACCESS_REQ_TRUE */
  PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ = 0x07,
  PM_FG_MEMIF_MEM_INTF_CFG_INVALID
} pm_fg_mem_if_mem_intf_cfg;

typedef enum
{
  /*Defines Write/Read Access.0 = Read Access.1 = Write Access.
      0x0 : READ_ACCESS
      0x1 : WRITE_ACCESS */
  PM_FG_MEMIF_MEM_INTF_CTL_WR_EN=0x06,
  /* Defines Single/Burst Access. 0 = Single Mode enabled.1 = Burst Mode enabled.
      0x0 : MEM_ACS_SINGLE
      0x1 : MEM_ACS_BURST */
  PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST,
  PM_FG_MEMIF_MEM_INTF_CTL_INVALID
} pm_fg_mem_if_mem_intf_ctl;

typedef enum
{
  /* Interleave access sub-system clear; 0: Normal operation; 1: Clear operation; If a SPMI/pBus transaction is issued, even before the previous transaction completes, it will result in error. Please see register IMA_EXCEPTION_STS; To clear the error set this bit to 1 and perform these transactions to reset the sub-system; write to adrsMsbRg, dataWrMsbRg; read from dataRdMsbRg; Then clear this bit back to 0
     0x0 : IACS_NORMAL_MODE
     0x1 : IACS_CLEAR_MODE */
  PM_FG_MEMIF_IMA_CFG_IACS_CLR= 0X02,
  /* Interrupt source; 0: End of transaction; 1: Interleave access ready (note: when interleave access is not pending it will assert true)
      0x0 : END_OF_TRANSACTION
      0x1 : IACS_RDY */
  PM_FG_MEMIF_IMA_CFG_IACS_INTR_SRC_SLCT,
  /* Timing of clock access request; 0: Dynamic - only when transaction is pending; 1: Static - when interleave access is enabled
    0x0 : IACS_DYNAMIC_CLK_EN
    0x1 : IACS_STATIC_CLK_EN */
  PM_FG_MEMIF_IMA_CFG_STATIC_CLK_EN,
  /* Read request from SPMI/pBus is kept pending until FG exits its transaction period and is then executed; 1: Read transaction from SPMI/pBus is executed any time including FG transactions period;
      0x0 : DISABLE_RD_FGXCT_PRD
      0x1 : ENABLE_RD_FGXCT_PRD */
  PM_FG_MEMIF_IMA_CFG_EN_RD_FGXCT_PRD,
  /* Write request from SPMI/pBus is kept pending until FG exits its transaction period and is then executed; 1: Write transaction from SPMI/pBus is executed any time including FG transactions period;
      0x0 : DISABLE_WR_FGXCT_PRD
      0x1 : ENABLE_WR_FGXCT_PRD */
  PM_FG_MEMIF_IMA_CFG_EN_WR_FGXCT_PRD,
  PM_FG_MEMIF_IMA_CFG__INVALID
} pm_fg_mem_if_ima_cfg;

typedef enum
{
  /* Structural: Peripheral 0 accesses address [0:255] and 1 does [256:511]; */
  PM_FG_MEMIF_DMA_CTL_STRUCTURAL,

  /* Partition: Peripheral 0 accesses System Profile, 1 does Battery Profile and 2 gets to scratch;  */
  PM_FG_MEMIF_DMA_CTL_PARTITION,
  PM_FG_MEMIF_DMA_CTL_INVALID
} pm_fg_mem_if_dma_ctl_cfg;

typedef enum
{
  /* Memory request to arbiter/pause algorithm from accessing memory 0x0 : NO_MEM_REQ 0x1 : MEM_REQ */
  PM_FG_MEMIF_ARB_REQ,

  /* Memory arbitration does not wait for begin of long wait; If outside computation period, memory is granted to pBus/spmi;
  Hence with this bit set, it is possible to get access memory quicker, with the caveat that algorithm has a higher probability 
  that it would be denied memory and keep it waiting - recognized as cycle stretch in status; Hence it is suitable for swift, short access;
    0x0 : HI_LATENCY  0x1 : LO_LATENCY  */
  PM_FG_MEMIF_ARB_LO_LATENCY_EN,

  /* Clear memory exceptions; Current, there is only one - cycle stretch;
    0x0 : CLR_LOG_FALSE 0x1 : CLR_LOG_TRUE*/
  PM_FG_MEMIF_CLR_LOG,
  PM_FG_MEMIF_ARB_INVALID
} pm_fg_mem_if_arb_ctl_cfg;

typedef struct {
  boolean iacs_rdy;
  boolean iacs_intr_sts;
  boolean iacs_clk_req;
  boolean rd_fgxct_prd_log;
  boolean wr_fgxct_prd_log;
  boolean fgxct_prd;
}pm_fg_memif_ima_opr_sts;

typedef struct {
  boolean iacs_err;
  boolean xct_type_err;
  boolean be_burstwr_warn;
  boolean data_rd_err;
  boolean data_wr_err;
  boolean addr_burst_wrap;
  boolean addr_stable_err;
}pm_fg_memif_ima_exception_sts;

typedef struct {
  boolean data_msb_rd_tgl_rcv;
  boolean data_msb_wr_tgl_rcv;
  boolean addr_msb_wr_tgl_rcv;
  boolean data_msb_rd_tgl;
  boolean data_msb_wr_tgl;
  boolean addr_msb_wr_tgl;
}pm_fg_memif_ima_hw_sts;

typedef struct {
  pm_fg_memif_ima_opr_sts opr_sts;
  pm_fg_memif_ima_exception_sts excep_sts;
  pm_fg_memif_ima_hw_sts hw_sts;
}pm_fg_memif_ima_sts;

typedef enum
{
  /* Enable write of data[ 7:0], mem_intf_wr_data_0, interleave access only
      0x0 : BE0_DSBL
      0x1 : BE0_EN */
  PM_FG_MEMIF_IMA_BYTE_EN_BE0,
  /* Enable write of data[15:8], mem_intf_wr_data_1, interleave access only
      0x0 : BE1_DSBL
      0x1 : BE1_EN */
  PM_FG_MEMIF_IMA_BYTE_EN_BE1,
  /* Enable write of data[23:16], mem_intf_wr_data_2, interleave access only
      0x0 : BE2_DSBL
      0x1 : BE2_EN */
  PM_FG_MEMIF_IMA_BYTE_EN_BE2,
  /* Enable write of data[31:24], mem_intf_wr_data_3, interleave access only
      0x0 : BE3_DSBL
      0x1 : BE3_EN */
  PM_FG_MEMIF_IMA_BYTE_EN_BE3,
  PM_FG_MEMIF_IMA_BYTE_EN_ALL,
  PM_FG_MEMIF_IMA_BYTE_EN_INVALID
} pm_fg_mem_if_ima_byte_en_cfg;

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/**
* @brief This function enables irq
*
* @details
* This function enables irq
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_mem_if_irq_type               irq type
* @param[in] boolean        enable/disable value
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_irq_enable(uint32 device_index, pm_fg_mem_if_irq_type irq, boolean enable);

/**
* @brief This function clears irq
*
* @details
*  This function clears irq
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_mem_if_irq_type               irq type
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
  pm_err_flag_type pm_fg_mem_if_irq_clear(uint32  device_index, pm_fg_mem_if_irq_type irq);

/**
* @brief This function sets interrupt triggers
*
* @details
*  This function sets interrupt triggers
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_mem_if_irq_type               irq type
* @param[in] pm_irq_trigger_type               trigger
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_irq_set_trigger(uint32 device_index, pm_fg_mem_if_irq_type irq, pm_irq_trigger_type trigger);

/**
* @brief This function returns irq status
*
* @details
*  This function returns irq status
*
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pm_fg_mem_if_irq_type        irq type
* @param[in] pm_irq_status_type        irq status type
* @param[in] boolean        irq TRUE/FALSE status
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_irq_status(uint32 device_index, pm_fg_mem_if_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function returns Fule Gauge Memory Interface Configuration *
 * @details
 *  This function returns Fule Gauge Memory Interface Configuration
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]pm_fg_mem_if_mem_intf_cfg memory Interface configuration
 * @param[out]enable TRUE/FALSE value of passed memory interface cfg
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_get_mem_intf_cfg(uint32 device_index, pm_fg_mem_if_mem_intf_cfg mem_intf_cfg, boolean *enable);

/**
 * @brief This function sets Fule Gauge Memory Interface Configuration *
 * @details
 *  This function sets Fule Gauge Memory Interface Configuration
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]pm_fg_mem_if_mem_intf_cfg memory Interface configuration
 * @param[in]enable TRUE/FALSE value of passed memory interface cfg
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_set_mem_intf_cfg(uint32 device_index, pm_fg_mem_if_mem_intf_cfg mem_intf_cfg, boolean enable);


/**
 * @brief This function returns Fule Gauge Memory Interface Configuration Control *
 * @details
 *  This function returns Fule Gauge Memory Interface Configuration Control
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]fg_memif_addr mem intf address
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_get_mem_intf_ctl(uint32 device_index, pm_fg_mem_if_mem_intf_ctl mem_intf_ctl, boolean *enable);

/**
 * @brief This function sets Fule Gauge Memory Interface Configuration Control *
 * @details
 *  This function sets Fule Gauge Memory Interface Configuration Control
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]pm_fg_mem_if_mem_intf_ctl memory Interface Configuration Control
 * @param[in]enable TRUE/FALSE value of passed memory interface ctl
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_set_mem_intf_ctl(uint32 device_index, pm_fg_mem_if_mem_intf_ctl mem_intf_ctl, boolean enable);

/**
 * @brief  This function Writes Fule Gauge Memory Interface Address Regsiter *
 * @details
 *  This function Writes Fule Gauge Memory Interface Address Regsiter
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]pm_fg_mem_if_mem_intf_ctl memory Interface configuration Control
 * @param[out]enable TRUE/FALSE value of passed memory interface ctl
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_write_addr(uint32 device_index, uint16 fg_memif_addr);

/**
 * @brief This function Reads Fule Gauge Memory Interface Address Regsiter*
 * @details
 *  This function Reads Fule Gauge Memory Interface Address Regsiter
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]fg_memif_addr mem intf address
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_read_addr_reg(uint32 device_index, uint16 *fg_memif_addr);


/**
 * @brief  This function Writes Fule Gauge Memory Interface Data Regsiter *
 * @details
 *  This function Writes Fule Gauge Memory Interface Data Regsiter
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]fg_memif_data mem intf data
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_write_data(uint32 device_index, uint32 fg_memif_data);

/**
 * @brief This function Reads Fule Gauge Memory Interface Address Regsiter*
 * @details
 *  This function Reads Fule Gauge Memory Interface Address Regsiter
 *
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]fg_memif_data mem intf data
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_fg_mem_if_read_data_reg(uint32 device_index, uint32 *fg_memif_data);

/**
* @brief This function returns Fule Gauge Memory Interface IMA Configuration *
* @details
*  This function returns Fule Gauge Memory Interface IMA Configuration
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_mem_if_ima_cfg  memory Interface configuration
* @param[out]enable TRUE/FALSE value of passed memory interface cfg
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_get_ima_cfg(uint32 pmic_device, pm_fg_mem_if_ima_cfg fg_ima_cfg, boolean *enable);


/**
* @brief This function sets Fule Gauge Memory Interface IMA Configuration *
* @details
*  This function sets Fule Gauge Memory Interface IMA Configuration
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_mem_if_ima_cfg memory Interface configuration
* @param[in]enable TRUE/FALSE value of passed memory interface cfg
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_set_ima_cfg(uint32 pmic_device, pm_fg_mem_if_ima_cfg fg_ima_cfg, boolean enable);

/**
* @brief This function returns Fule Gauge Memory Interface IMA Configuration *
* @details
*  This function returns Fule Gauge Memory Interface IMA Configuration
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_mem_if_ima_cfg  memory Interface configuration
* @param[out]enable TRUE/FALSE value of passed memory interface cfg
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_get_fg_beat_count(uint32 pmic_device, uint8 *beat_cnt);


/**
* @brief This function sets Fule Gauge Memory Interface IMA Configuration *
* @details
*  This function sets Fule Gauge Memory Interface IMA Configuration
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_mem_if_ima_cfg memory Interface configuration
* @param[in]enable TRUE/FALSE value of passed memory interface cfg
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_set_ima_byte_en_cfg(uint32 pmic_device, pm_fg_mem_if_ima_byte_en_cfg fg_ima_byte_en_cfg, boolean enable);


/**
* @brief This function returns Fule Gauge Memory Interface IMA exception status *
* @details
*  This function returns Fule Gauge Memory Interface IMA exception status
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_mem_if_ima_cfg  memory Interface configuration
* @param[out]enable TRUE/FALSE value of passed memory interface cfg
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_get_ima_sts(uint32 pmic_device, pm_fg_memif_ima_sts *fg_ima_sts);

/**
* @brief  This function Runs IMA clear sequnce  *
* @details
*  This function Runs IMA clear sequnce
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_run_ima_clr_sequence(uint32 pmic_device);

/**
* @brief  This function Writes Fule Gauge Memory Interface Data Regsiter *
* @details
*  This function Writes Fule Gauge Memory Interface Data Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] data8
* @param[in] Offset
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_write_offset_data(uint32 pmic_device, uint8 data8, uint8 offset);





/**
* @brief  This function returns Fule Gauge DMA ERROR status
*
* @details
*  This function returns Fule Gauge DMA Error status
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] boolean
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_memif_get_dma_err_sts(uint32 pmic_device, boolean *dma_err_sts);


/**
* @brief  This function clears DMA log
*
* @details
*  This function clears DMA read/write error
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] boolean
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_memif_dma_clear(uint32 pmic_device, boolean clear_log);

/**
* @brief  This function Writes Fule Gauge Memory Interface Address Regsiter *
* @details
*  This function Writes Fule Gauge Memory Interface Address Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]pm_fg_mem_if_mem_intf_ctl memory Interface configuration Control
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_write_addr_msb(uint32 pmic_device, UINT8 fg_memif_addr_msb);

/**
* @brief This function Reads Fule Gauge Memory Interface Address Regsiter*
* @details
*  This function Reads Fule Gauge Memory Interface Address Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]fg_memif_data mem intf data
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_read_data_reg3(uint32 pmic_device, uint8 *fg_memif_data);

/**
* @brief  This function Writes Fule Gauge Memory Interface Data Regsiter *
* @details
*  This function Writes Fule Gauge Memory Interface Data Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]fg_memif_data mem intf data
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_write_data3(uint32 pmic_device, uint8 fg_memif_data);

/**
* @brief  This function Writes Fule Gauge Memory Interface Data Regsiter *
* @details
*  This function Writes Fule Gauge Memory Interface Data Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]fg_memif_data mem intf data
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_set_ima_cfg_value(uint32 pmic_device, uint8 value);

/**
* @brief  This function Writes Fule Gauge Memory Interface Data Regsiter *
* @details
*  This function Writes Fule Gauge Memory Interface Data Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]fg_memif_data mem intf data
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_set_mem_intf_ctl_value(uint32 pmic_device, uint8 value);

/**
* @brief  This function Writes Fule Gauge Memory Interface Data Regsiter *
* @details
*  This function Writes Fule Gauge Memory Interface Data Regsiter
*
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]fg_memif_data mem intf data
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_set_mem_intf_cfg_value(uint32 pmic_device, uint8 value);

/**
* @brief  This function Read Fuel Gauge Hw IMA status register * 
* @details
*  This function Reads Read Fuel Gauge Hw IMA status register 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]data mem intf data 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_mem_if_get_ima_hw_sts(uint32 pmic_device, uint8 *data);

/**
* @brief This function provides FG PMIC info.
*
* @details
*  This function provides FG PMIC info.
*
* @param[out] pmic_index         On what PMIC Index, SCHG charger resides.
* @param[out] slave_id           On what SlaveID for given PMIC, SCHG charger resides.
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available.
*          PM_ERR_FLAG_INVALID_PARAMETER     = Invalid parameters.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*/
pm_err_flag_type pm_fg_mem_if_get_fg_pmic_info(uint8 *pmic_index, uint8 *slave_id);


/**
* @brief  This function set dma ctl cfg 
* @details
*  This function set dma ctl cfg 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] data dma_ctl_cfg
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type 
pm_fg_memif_dma_ctl(uint32 pmic_device, pm_fg_mem_if_dma_ctl_cfg dma_ctl_cfg);


/**
* @brief  This function set arb ctl cfg 
* @details
*  This function set arb ctl cfg 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] data arb_ctl_cfg
* @param[in] enable true/false
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type 
pm_fg_memif_arb_ctl(uint32 pmic_device, pm_fg_mem_if_arb_ctl_cfg arb_ctl_cfg, boolean enable);

/**
* @brief  This function perform dma byte write on spmi
* @details
*  This function perform dma byte write on spmi
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] addr dma address
* @param[in] data value to write
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type 
pm_fg_memif_dma_write_byte(uint32 pmic_device, uint16 addr, uint8 data);

/**
* @brief  This function perform dma byte read on spmi
* @details
*  This function perform dma byte read on spmi
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] addr dma address
* @param[out] data read byte
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type 
pm_fg_memif_dma_read_byte(uint32 pmic_device, uint16 addr, uint8 *data);

/**
* @brief  This function perform dma byte long on spmi
* @details
*  This function perform dma long write on spmi
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] addr dma address
* @param[in] data bytes to write
* @param[in] writelength data length
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type 
pm_fg_memif_dma_write_long(uint32 pmic_device, uint16 addr, uint8 *data, uint32 writelength);

/**
* @brief  This function perform dma byte long read on spmi
* @details
*  This function perform dma long read on spmi
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] addr dma address
* @param[out] data bytes to write
* @param[in] readlength data to be read length
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type 
pm_fg_memif_dma_read_long(uint32 pmic_device, uint16 addr, uint8 *data, uint32 readlength);

#endif /* __PM_FG_MEM_IF_H__ */

