/*=======================================================================*//**
 * @file        UsbSharedInit.c
 * @author:     pmahajan
 * @date        03-feb-2017
 *
 * @brief       USB common & shared USB core init implementation.
 *
 * @details     This file contains API's the Init sequence for HS and SS core implementation.
 *
 * @note        
 *              Copyright 2017 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 03/30/17     pm       Added configuration for sec Clock and core
// 02/03/17     pm       Initial revision
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "DALSys.h"    // To get phy settings
#include "qusb_log.h"
#include "qusb_dci_common.h"
#include "UsbSharedLib.h"
#include "HalusbHWIO.h"
#include <stdio.h>
#include <string.h>


// ===========================================================================
/**
 * @function    usb_shared_lib_process_phy_reg_array
 * 
 * @brief   This function reads from array which define list of hwio writes for
 *          USB PHY
 * 
 * @param   address_array   - array holding address of HW register
 *          value_array     - array holding values to be written to HW register
 *          start_index     - starting index for array processing
 *          array_entry_cnt - number of entries in the array
 * 
 * @return  None
 * 
 */
// ===========================================================================
static void usb_shared_lib_process_phy_reg_array
(
  const uint32 *address_array, 
  const uint8  *value_array, 
  uint32        start_index, 
  uint32        array_entry_cnt
)
{
  uint32 index = start_index;

  if ( (NULL == address_array)
      || (NULL == value_array)
      || (0 == array_entry_cnt) )
  {
    usb_shared_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS__FAIL_LOG, 0, (uint32)address_array);
    usb_shared_uart_log_error("Process_Array: Null Parameter Passed", (uint32)address_array);
  }
  else
  {
    usb_shared_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS__START_LOG, 0, start_index);
    
    for (; index < array_entry_cnt; index++)
    {
      if (QUSB_HWIO_ADDR_END == address_array[index])
      {
        break;
      }

      if (QUSB_HWIO_ADDR_EMPTY == address_array[index])
      {
        continue;
      }

      out_dword(address_array[index], value_array[index]);
    }
  }
  usb_shared_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS____END_LOG, 0, index);

}

//============================================================================

/**
* @function USB_SHARED_HS_PHY_INIT
*
* @brief    API used to initialize the High Speed Primary PHY 
*
*/
//============================================================================
boolean usb_shared_hs_phy_init()
{

    uint8 hstx_trim_val = 0x0;
    uint8 pll_status = 0;
    uint32 pll_status_cnt = 0;
    const char *hs_phy_cfg_addr_prop_name = "hs_phy_cfg_addr";
    const char *hs_phy_cfg_val_prop_name = "hs_phy_cfg_val";
    DALSYSPropertyVar dal_hs_phy_addr;
    DALSYSPropertyVar dal_hs_phy_val;
    DALResult result;
    DALSYS_PROPERTY_HANDLE_DECLARE(hs_phy_cfg_h);

    usb_shared_uart_log_info("usb_shared_hs_phy_init: ++");
    
    /* get handle to hs phy settings */
    result = DALSYS_GetDALPropertyHandleStr("QUSB_PORT_PRIM", hs_phy_cfg_h);
    if (result != DAL_SUCCESS)
    {
       usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
       usb_shared_uart_log_error("usb_shared_hs_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
       return FALSE;
    }
    
    /* get hs phy address */
    result = DALSYS_GetPropertyValue(hs_phy_cfg_h, hs_phy_cfg_addr_prop_name, 0, &dal_hs_phy_addr);
    if (result != DAL_SUCCESS)
    {
       usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
       usb_shared_uart_log_error("usb_shared_hs_phy_init: get hs phy address Failed", result);
       return FALSE;
    }
    
    /* get hs phy values */
    result = DALSYS_GetPropertyValue(hs_phy_cfg_h, hs_phy_cfg_val_prop_name, 0, &dal_hs_phy_val);
    if (result != DAL_SUCCESS)
    {
       usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
       usb_shared_uart_log_error("usb_shared_hs_phy_init: get hs phy value Failed", result);
       return FALSE;
    }


    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_BMSK,
      (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_SHFT));

    usb_shared_lib_process_phy_reg_array(
      (const uint32*) dal_hs_phy_addr.Val.pStruct,
      (const uint8*) dal_hs_phy_val.Val.pStruct,
      0,
      QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT);

    // Update non-zero HSTX_TRIM value read from QPFROM fuse.
    hstx_trim_val = 
      (HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_BMSK) 
      >> HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_SHFT);

    if(hstx_trim_val)
    {
		usb_shared_uart_log_error("qusb2_1: hstx", hstx_trim_val);
        HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_OUTM(
            HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_HSTX_TRIM_BMSK, 
            (hstx_trim_val - QUSB_HSTX_TRIM_OFFSET) << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_HSTX_TRIM_SHFT);
    }


    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_BMSK,
      (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_SHFT));

    /* Recommended 88us PLL lock delay in sequence document */
    usb_shared_delay_us(100);

    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL2_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL2_VSTATUS_SEL_BMSK,
      (0x4) << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL2_VSTATUS_SEL_SHFT);

    /* Wait maximum of ~1ms for PLL to lock */
    for(pll_status_cnt = 0; (FALSE == pll_status) && (pll_status_cnt < QUSB_HS_PHY_PLL_MAX_CNT); pll_status_cnt++)
    {
      usb_shared_delay_us(QUSB_HS_PHY_PLL_CHECK_DELAY_PER_LOOP);

      pll_status = (HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_STAT5_INM
                    (HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_STAT5_VSTATUS_BMSK)
                    >> HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_STAT5_VSTATUS_SHFT);

      pll_status &= QUSB_HS_PHY_PLL_BMSK;
    }

    if(FALSE == pll_status)
    {
      usb_shared_error_log(DCI_HS_PHY_PLL_FAILURE_LOG, 0, pll_status_cnt);
      usb_shared_uart_log_error("PLL1 lock failed", 
        pll_status_cnt * QUSB_HS_PHY_PLL_CHECK_DELAY_PER_LOOP);
      return FALSE;
    }
    else
    {
      usb_shared_uart_log_error("PLL1 locked", 
        pll_status_cnt * QUSB_HS_PHY_PLL_CHECK_DELAY_PER_LOOP);
    }
    
    usb_shared_uart_log_info("usb_shared_hs_phy_init: --");
    return TRUE;
}


//============================================================================

/**
* @function USB_SHARED_HS_PHY_INIT_SEC
*
* @brief    API used to initialize the High Speed Secondary PHY 
*
*/
//============================================================================
boolean usb_shared_hs_phy_init_sec()
{

    uint8 hstx_trim_val = 0x0;
    uint8 pll_status = 0;
    uint32 pll_status_cnt = 0;
    const char *hs_phy_cfg_addr_prop_name = "hs_phy_sec_cfg_addr";
    const char *hs_phy_cfg_val_prop_name = "hs_phy_sec_cfg_val";
    DALSYSPropertyVar dal_hs_phy_addr;
    DALSYSPropertyVar dal_hs_phy_val;
    DALResult result;
    DALSYS_PROPERTY_HANDLE_DECLARE(hs_phy_sec_cfg_h);

    usb_shared_uart_log_info("usb_shared_hs_phy_init_sec: ++");
    
    /* get handle to hs phy settings */
    result = DALSYS_GetDALPropertyHandleStr("QUSB_PORT_SEC", hs_phy_sec_cfg_h);
    if (result != DAL_SUCCESS)
    {
       usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
       usb_shared_uart_log_error("usb_shared_hs_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
       return FALSE;
    }
    
    /* get hs phy address */
    result = DALSYS_GetPropertyValue(hs_phy_sec_cfg_h, hs_phy_cfg_addr_prop_name, 0, &dal_hs_phy_addr);
    if (result != DAL_SUCCESS)
    {
       usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
       usb_shared_uart_log_error("usb_shared_hs_phy_init: get hs phy address Failed", result);
       return FALSE;
    }
    
    /* get hs phy values */
    result = DALSYS_GetPropertyValue(hs_phy_sec_cfg_h, hs_phy_cfg_val_prop_name, 0, &dal_hs_phy_val);
    if (result != DAL_SUCCESS)
    {
       usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
       usb_shared_uart_log_error("usb_shared_hs_phy_init: get hs phy value Failed", result);
       return FALSE;
    }


    HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_POWER_DOWN_BMSK,
      (0x1 << HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_POWER_DOWN_SHFT));

    usb_shared_lib_process_phy_reg_array(
      (const uint32*) dal_hs_phy_addr.Val.pStruct,
      (const uint8*) dal_hs_phy_val.Val.pStruct,
      0,
      QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT);

    // Update non-zero HSTX_TRIM value read from QPFROM fuse.
    hstx_trim_val = 
      (HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT1_HSTX_TRIM_LSB_1_0_BMSK) 
      >> HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT1_HSTX_TRIM_LSB_1_0_SHFT);

    if(hstx_trim_val)
    {
        HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE1_OUTM(
            HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE1_HSTX_TRIM_BMSK, 
            (hstx_trim_val - QUSB_HSTX_TRIM_OFFSET) << HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE1_HSTX_TRIM_SHFT);
    }


    HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_POWER_DOWN_BMSK,
      (0x0 << HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_POWER_DOWN_SHFT));

    /* Recommended 88us PLL lock delay in sequence document */
    usb_shared_delay_us(100);

    HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_DEBUG_CTRL2_OUTM(
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_DEBUG_CTRL2_VSTATUS_SEL_BMSK,
      (0x4) << HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_DEBUG_CTRL2_VSTATUS_SEL_SHFT);

    /* Wait maximum of ~1ms for PLL to lock */
    for(pll_status_cnt = 0; (FALSE == pll_status) && (pll_status_cnt < QUSB_HS_PHY_PLL_MAX_CNT); pll_status_cnt++)
    {
      usb_shared_delay_us(QUSB_HS_PHY_PLL_CHECK_DELAY_PER_LOOP);

      pll_status = (HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_DEBUG_STAT5_INM
                    (HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_DEBUG_STAT5_VSTATUS_BMSK)
                    >> HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_DEBUG_STAT5_VSTATUS_SHFT);

      pll_status &= QUSB_HS_PHY_PLL_BMSK;
    }

    if(FALSE == pll_status)
    {
      usb_shared_error_log(DCI_HS_PHY_PLL_FAILURE_LOG, 0, pll_status_cnt);
      usb_shared_uart_log_error("PLL2 lock failed", 
        pll_status_cnt * QUSB_HS_PHY_PLL_CHECK_DELAY_PER_LOOP);
      return FALSE;
    }
    else
    {
      usb_shared_uart_log_error("PLL2 locked", 
        pll_status_cnt * QUSB_HS_PHY_PLL_CHECK_DELAY_PER_LOOP);
    }

    usb_shared_uart_log_info("usb_shared_hs_phy_init_sec: --");
    return TRUE;
}



//============================================================================

/**
* @function USB_SHARED_SS_PHY_INIT
*
* @brief    API used to initialize the Super Speed Primary PHY 
*
*/
//============================================================================
boolean usb_shared_ss_phy_init(uint32 usb_ss_phy_lane)
{

  uint32 qusb_timeout=0x0;
  const char *ss_phy_cfg_addr_prop_name = "ss_phy_cfg_addr";
  const char *ss_phy_cfg_val_prop_name = "ss_phy_cfg_val";
  DALSYSPropertyVar dal_ss_phy_addr;
  DALSYSPropertyVar dal_ss_phy_val;
  DALResult result;
  DALSYS_PROPERTY_HANDLE_DECLARE(ss_phy_cfg_h);

  usb_shared_error_log(QUSB_SS_PHY_INIT_LOG, 0, 0);
  usb_shared_uart_log_info("usb_shared_ss_phy_init: ++");

  usb_shared_uart_log_error("usb_lane", usb_ss_phy_lane);

  /* get handle to ss phy settings */
  result = DALSYS_GetDALPropertyHandleStr("QUSB_PORT_PRIM", ss_phy_cfg_h);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
     return FALSE;
  }

  /* get ss phy address */
  result = DALSYS_GetPropertyValue(ss_phy_cfg_h, ss_phy_cfg_addr_prop_name, 0, &dal_ss_phy_addr);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: get hs phy address Failed", result);
     return FALSE;
  }

  /* get ss phy values */
  result = DALSYS_GetPropertyValue(ss_phy_cfg_h, ss_phy_cfg_val_prop_name, 0, &dal_ss_phy_val);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: get hs phy value Failed", result);
     return FALSE;
  }
  
  usb_shared_lib_process_phy_reg_array(
    (const uint32*) dal_ss_phy_addr.Val.pStruct,
    (const uint8*) dal_ss_phy_val.Val.pStruct,
    0,
    QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT);

  // Override Type-C lane detection  override
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_TYPEC_CTRL_OUTM(
    HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_TYPEC_CTRL_SW_PORTSELECT_BMSK, 
    usb_ss_phy_lane << HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_TYPEC_CTRL_SW_PORTSELECT_SHFT);
  
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_TYPEC_CTRL_OUTM(
    HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_TYPEC_CTRL_SW_PORTSELECT_MUX_BMSK,
    0x1 << HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_TYPEC_CTRL_SW_PORTSELECT_MUX_SHFT);

  usb_shared_delay_us(100);

  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_SW_RESET_OUT(0x00);
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_SW_RESET_OUT(0x0);
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_START_CONTROL_OUT(0x3);

  usb_shared_delay_us(100);

  /* Poll PHYSTATUS field of Register USB3_PHY_PCS_STATUS to Go LOW after reset*/
  while(HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_PCS_STATUS_INM(
        HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_PCS_STATUS_PHYSTATUS_BMSK))
  {
    usb_shared_delay_us(100);
    if (qusb_timeout++ > 100)
    {
        USB_SHARED_ASSERT(DCI_SS_PHY_RESET_ERROR_LOG);
        usb_shared_uart_log_error("ssusb_phy_init_timeout", 0);
        return FALSE;
    }
  }
  
  usb_shared_uart_log_info("usb_shared_ss_phy_init: --");
  return TRUE;
}


//============================================================================

/**
* @function USB_SHARED_SS_PHY_INIT_SEC
*
* @brief    API used to initialize the Super Speed Secondary PHY 
*
*/
//============================================================================
boolean usb_shared_ss_phy_init_sec()
{

  uint32 qusb_timeout=0x0;
  const char *ss_phy_cfg_addr_prop_name = "ss_phy_sec_cfg_addr";
  const char *ss_phy_cfg_val_prop_name = "ss_phy_sec_cfg_val";
  DALSYSPropertyVar dal_ss_phy_addr;
  DALSYSPropertyVar dal_ss_phy_val;
  DALResult result;
  DALSYS_PROPERTY_HANDLE_DECLARE(ss_phy_sec_cfg_h);

  usb_shared_error_log(QUSB_SS_PHY_INIT_LOG, 0, 0);
  usb_shared_uart_log_info("usb_shared_ss_phy_init_sec: ++");

  /* get handle to ss phy settings */
  result = DALSYS_GetDALPropertyHandleStr("QUSB_PORT_SEC", ss_phy_sec_cfg_h);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: DALSYS_GetDALPropertyHandleStr Failed", result);
     return FALSE;
  }

  /* get ss phy address */
  result = DALSYS_GetPropertyValue(ss_phy_sec_cfg_h, ss_phy_cfg_addr_prop_name, 0, &dal_ss_phy_addr);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: get hs phy address Failed", result);
     return FALSE;
  }

  /* get ss phy values */
  result = DALSYS_GetPropertyValue(ss_phy_sec_cfg_h, ss_phy_cfg_val_prop_name, 0, &dal_ss_phy_val);
  if (result != DAL_SUCCESS)
  {
     usb_shared_error_log(DCI_DAL_ERROR_LOG, 0, __LINE__);
     usb_shared_uart_log_error("usb_shared_ss_phy_init: get hs phy value Failed", result);
     return FALSE;
  }
  
  usb_shared_lib_process_phy_reg_array(
    (const uint32*) dal_ss_phy_addr.Val.pStruct,
    (const uint8*) dal_ss_phy_val.Val.pStruct,
    0,
    QUSB_DCI_SS_PHY_SEC_COMMON_CFG_ARRAY_ENTRY_CNT);

  usb_shared_delay_us(100);

  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_SW_RESET_OUT(0x0);
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_START_CONTROL_OUT(0x3);

  usb_shared_delay_us(100);

  /* Poll PHYSTATUS field of Register USB3_PHY_PCS_STATUS to Go LOW after reset*/
  while(HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_PCS_STATUS_INM(
        HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_PCS_STATUS_PHYSTATUS_BMSK))
  {
    usb_shared_delay_us(100);
    if (qusb_timeout++ > 100)
    {
        USB_SHARED_ASSERT(DCI_SS_PHY_RESET_ERROR_LOG);
        usb_shared_uart_log_error("p2_ssusb_phy_init_timeout", 0);
        return FALSE;
    }
  }
  
  usb_shared_uart_log_info("usb_shared_ss_phy_init_sec: --");
  return TRUE;
}


//============================================================================
/**
* @function  usb_dci_get_chip_serial_number
*
* @brief Call API to uint32 unique MSM serial string
*
* @Note : None
*
* @param  None 
*
* @return uint32: Chip serial number
*
*/
//============================================================================
uint32 usb_dci_get_chip_serial_number(void)
{
  return HWIO_SERIAL_NUM_IN;
}

//============================================================================
/** 
 * API is used to add chip serial number to the string and return 
 * Product String Descriptor
 * 
 * @param pointer to product string
 *              Pointer to the buffer to be updated with Product string desc
 *              Length of the buffer
 *
 * @return Appended string length
 */ 
 //============================================================================
uint32 usb_get_product_str_desc_with_chip_serial(char* product_str, char* prod_str_desc, uint32 desc_len)
{
  char    chip_serial_str[QUSB_FD_PRODUCT_STRING_LENGTH_MAX];
  uint32  chip_serial;
  uint32  str_desc_index;
  uint32  str_desc_size;
  uint32  chip_str_len;
  uint32  chip_str_index;

  if ((NULL == product_str)||(NULL == prod_str_desc) )
  {
    // String is NULL;
    return 0;
  }

  chip_serial = usb_dci_get_chip_serial_number();
  
  chip_str_len = snprintf(chip_serial_str, sizeof(chip_serial_str), "%s_SN:%08X", product_str, chip_serial);

  if (chip_str_len >= sizeof(chip_serial_str))
  {
    // Chip Seiral is too long to fit
    return 0;
  }

  // "ASCII string size" X 2 (UNICODE) 
  //   + 2 (1 byte SIZE and 1 byte STRING DESC TYPE ENUM)
  str_desc_size = (chip_str_len * 2) + 2;

  if (str_desc_size >= desc_len)
  {
    // Destination buffer over flow
    return 0;    
  }

  str_desc_index = 0;
  prod_str_desc[str_desc_index++] = (char)str_desc_size;
  prod_str_desc[str_desc_index++] = USB_DESCTYPE_STRING;

  for (chip_str_index = 0; chip_str_index < chip_str_len; chip_str_index++)
  {
    prod_str_desc[str_desc_index++] = chip_serial_str[chip_str_index];
    prod_str_desc[str_desc_index++] = 0x0;  
  }

  return str_desc_size;
}


