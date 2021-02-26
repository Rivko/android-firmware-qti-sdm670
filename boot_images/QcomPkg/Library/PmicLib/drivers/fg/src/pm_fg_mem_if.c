/*! \file
*
*  \pm_fg_mem_if.c ---Fule Gauge Memory Interface Driver implementation.
*  \details FG Driver implementation.
*  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who     what, where, why
--------    ---     ----------------------------------------------------------
01/31/17    va      Adding DMA arb reg
12/19/16    sm      Added API to get FG PmicIndex
10/05/16    va      update IMA clear sequence as per systems recommendation
08/24/16    aab     Added pm_fg_mem_if_get_dma_err_sts () and pm_fg_mem_if_dma_clear_log() 
05/31/16    va      Add/Update for IMA sequence 
04/21/16    va      Updating IMA APIs
03/16/16    va      Adding mem if required APIs
09/22/14    aab     Porting FG driver to SBL 
08/20/14    al      Updating comm lib 
07/17/14    va      Driver Minor Update
04/18/14    va      New file
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_fg_mem_if.h"
#include "pm_fg_driver.h"
#include "pm_resource_manager.h"

/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/
#define ADDRESS(x) (fg_mem_if_ptr->fg_register->mem_if_register->base_address + fg_mem_if_ptr->fg_register->mem_if_register->x)
#define SLAVE_ID   (fg_mem_if_ptr->comm_ptr->slave_id)
#define PMI_DIG_MAJ_REV1 1

#define FG_SRAM_START_DMA_ADDR           0x4800
#define FG_SRAM_END_DMA_ADDR             0x4BFF
#define FG_SRAM_DMA_MAX_WRITE_LENGTH     0x100


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pm_fg_mem_if_unlock_perph_write(pm_fg_data_type *fg_mem_if_ptr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if (NULL == fg_mem_if_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type sec_access = ADDRESS(sec_access);
    err_flag = pm_comm_write_byte(SLAVE_ID, sec_access, 0xA5, 0);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_mem_if_irq_enable(uint32 pmic_device, pm_fg_mem_if_irq_type irq, boolean enable)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type irq_reg;
  pm_register_data_type data = 1 << irq;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_MEM_IF_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    if (enable)
    {
      irq_reg = ADDRESS(int_en_set);
    }
    else
    {
      irq_reg = ADDRESS(int_en_clr);
    }

    err_flag = pm_comm_write_byte(SLAVE_ID, irq_reg, data, 0);
  }
  return err_flag;
}

pm_err_flag_type pm_fg_mem_if_irq_clear(uint32  pmic_device, pm_fg_mem_if_irq_type irq)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 1 << irq;
  pm_register_address_type int_latched_clr;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_MEM_IF_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    int_latched_clr = ADDRESS(int_latched_clr);
    err_flag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_mem_if_irq_set_trigger(uint32 pmic_device, pm_fg_mem_if_irq_type irq, pm_irq_trigger_type trigger)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 1 << irq;
  pm_register_data_type set_type, polarity_high, polarity_low;
  pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == fg_mem_if_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_MEM_IF_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    int_set_type = ADDRESS(int_set_type);
    int_polarity_high = ADDRESS(int_polarity_high);
    int_polarity_low = ADDRESS(int_polarity_low);

    switch (trigger)
    {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
      set_type = 0x00;
      polarity_high = 0x00;
      polarity_low = 0xFF;
      break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
      set_type = 0x00;
      polarity_high = 0xFF;
      polarity_low = 0x00;
      break;
    case PM_IRQ_TRIGGER_RISING_EDGE:
      set_type = 0xFF;
      polarity_high = 0xFF;
      polarity_low = 0x00;
      break;
    case PM_IRQ_TRIGGER_FALLING_EDGE:
      set_type = 0xFF;
      polarity_high = 0x00;
      polarity_low = 0xFF;
      break;
    case PM_IRQ_TRIGGER_DUAL_EDGE:
      set_type = 0xFF;
      polarity_high = 0xFF;
      polarity_low = 0xFF;
      break;
    default:
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_mem_if_irq_status(uint32 pmic_device, pm_fg_mem_if_irq_type irq, pm_irq_status_type type, boolean *status)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;
  uint8 mask = 1 << irq;
  pm_register_address_type int_sts;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_MEM_IF_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    switch (type)
    {
    case PM_IRQ_STATUS_RT:
      int_sts = ADDRESS(int_rt_sts);
      break;
    case PM_IRQ_STATUS_LATCHED:
      int_sts = ADDRESS(int_latched_sts);
      break;
    case PM_IRQ_STATUS_PENDING:
      int_sts = ADDRESS(int_pending_sts);
      break;
    default:
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(SLAVE_ID, int_sts, mask, &data, 0);
    *status = data ? TRUE : FALSE;
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_get_mem_intf_cfg(uint32 pmic_device, pm_fg_mem_if_mem_intf_cfg fg_mem_intf_cfg, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type intf_cfg;
  pm_register_data_type data;
  uint8 mask = 1 << fg_mem_intf_cfg;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == enable || fg_mem_intf_cfg >= PM_FG_MEMIF_MEM_INTF_CFG_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    intf_cfg = ADDRESS(mem_intf_cfg);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, intf_cfg, mask, &data, 0);
    *enable = (boolean)data;
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_set_mem_intf_cfg(uint32 pmic_device, pm_fg_mem_if_mem_intf_cfg fg_mem_intf_cfg, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type intf_cfg;
  uint8 mask = 1 << fg_mem_intf_cfg;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (fg_mem_intf_cfg >= PM_FG_MEMIF_MEM_INTF_CFG_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    intf_cfg = ADDRESS(mem_intf_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, intf_cfg, mask, (pm_register_data_type)(enable << fg_mem_intf_cfg), 0);
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_get_mem_intf_ctl(uint32 pmic_device, pm_fg_mem_if_mem_intf_ctl mem_intf_ctl, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type intf_ctl;
  pm_register_data_type data;
  uint8 mask = 1 << mem_intf_ctl;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == enable || mem_intf_ctl >= PM_FG_MEMIF_MEM_INTF_CTL_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    intf_ctl = ADDRESS(mem_intf_ctl);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, intf_ctl, mask, &data, 0);
    *enable = (boolean)data;
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_set_mem_intf_ctl(uint32 pmic_device, pm_fg_mem_if_mem_intf_ctl mem_intf_ctl, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type intf_ctl;
  uint8 mask = 1 << mem_intf_ctl;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (mem_intf_ctl >= PM_FG_MEMIF_MEM_INTF_CTL_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    intf_ctl = ADDRESS(mem_intf_ctl);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, intf_ctl, mask, (pm_register_data_type)(enable << mem_intf_ctl), 0);
  }
  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_write_addr(uint32 pmic_device, UINT16 fg_memif_addr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_lsb  = 0;
  pm_register_data_type    data16[2] = {0};
  data16[0] = fg_memif_addr & 0x00FF;
  data16[1] = 0x00;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    addr_lsb = ADDRESS(mem_intf_addr_lsb);

    err_flag  = pm_comm_write_byte_array(SLAVE_ID, addr_lsb, 2, data16, 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_write_addr_msb(uint32 pmic_device, UINT8 fg_memif_addr_msb)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_msb  = 0;
  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (fg_memif_addr_msb > 0x01 )
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    addr_msb = ADDRESS(mem_intf_addr_msb);
    err_flag  = pm_comm_write_byte(SLAVE_ID, addr_msb, (pm_register_data_type)fg_memif_addr_msb, 0);
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_read_addr_reg(uint32 pmic_device, UINT16 *fg_memif_addr)
{
  pm_err_flag_type         err_flag   = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_lsb   = 0;
  pm_register_data_type    addr16[2]  = {0};
  UINT16                   memif_addr = 0x0000;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == fg_memif_addr)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    addr_lsb = ADDRESS(mem_intf_addr_lsb);

    err_flag  = pm_comm_read_byte_array(SLAVE_ID, addr_lsb, 2, (uint8*) &addr16, 0);

    memif_addr = ((addr16[1] | memif_addr) << 8) | (addr16[0] | memif_addr);
    *fg_memif_addr = memif_addr;
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_write_data(uint32 pmic_device, uint32 fg_memif_data)
{
  pm_err_flag_type         err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_lsb  = 0 ;
  pm_register_data_type    data32[4] = {0};

  data32[0] = fg_memif_data & 0xFF;
  data32[1] = (fg_memif_data & 0xFF00) >> 8;
  data32[2] = (fg_memif_data & 0xFF0000) >> 16;
  data32[3] = (fg_memif_data & 0xFF000000) >> 24;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    addr_lsb = ADDRESS(mem_intf_wr_data0);

    err_flag  = pm_comm_write_byte_array(SLAVE_ID, addr_lsb, 4, data32, 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_write_data3(uint32 pmic_device, uint8 fg_memif_data)
{
  pm_err_flag_type         err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_data3  = 0 ;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    addr_data3 = ADDRESS(mem_intf_wr_data3);

    err_flag = pm_comm_write_byte(SLAVE_ID, addr_data3, (pm_register_data_type)fg_memif_data, 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_write_offset_data(uint32 pmic_device, uint8 data8, uint8 offset)
{
  pm_err_flag_type         err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_lsb  = 0 ;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (offset >= 4)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }else
  {
    switch(offset)
    {
      case 0:
        addr_lsb = ADDRESS(mem_intf_wr_data0);
      break;
      case 1:
        addr_lsb = ADDRESS(mem_intf_wr_data1);
      break;
      case 2:
        addr_lsb = ADDRESS(mem_intf_wr_data2);
      break;
      case 3:
        addr_lsb = ADDRESS(mem_intf_wr_data3);
      break;
      default:
        return PM_ERR_FLAG_INVALID_PARAMETER;
        break;
    }
    err_flag  = pm_comm_write_byte(SLAVE_ID, addr_lsb, data8, 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_read_data_reg(uint32 pmic_device, uint32 *fg_memif_data)
{
  pm_err_flag_type         err_flag   = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_lsb   = 0;
  pm_register_data_type    data32[4]  = {0};
  UINT32                   memif_data = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == fg_memif_data)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {

    addr_lsb = ADDRESS(mem_intf_rd_data0);

    err_flag = pm_comm_read_byte_array(SLAVE_ID,  addr_lsb, 4, (uint8*) &data32, 0);

    memif_data = (data32[3]  << 24) | (data32[2] << 16) | (data32[1] << 8) | (data32[0] );

    *fg_memif_data = memif_data;
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_read_data_reg3(uint32 pmic_device, uint8 *fg_memif_data)
{
  pm_err_flag_type         err_flag  = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type addr_msb3 = 0;
  pm_register_data_type    data8 = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == fg_memif_data)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    addr_msb3 = ADDRESS(mem_intf_rd_data3);
    err_flag  = pm_comm_read_byte(SLAVE_ID,  addr_msb3, &data8, 0);
    *fg_memif_data = data8;
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_get_ima_cfg(uint32 pmic_device, pm_fg_mem_if_ima_cfg fg_ima_cfg, boolean *enable)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type ima_cfg_reg = 0;
  pm_register_data_type    data        = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if ((NULL == enable )|| (fg_ima_cfg  >= PM_FG_MEMIF_IMA_CFG__INVALID))
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    ima_cfg_reg = ADDRESS(ima_cfg);
    err_flag = pm_comm_read_byte(SLAVE_ID, ima_cfg_reg, &data, 0);
    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
       *enable = (data & (0x1 << fg_ima_cfg)) ? TRUE : FALSE;
    }
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_set_ima_cfg(uint32 pmic_device, pm_fg_mem_if_ima_cfg fg_ima_cfg, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type ima_cfg_reg = 0;
  uint8 mask = 1 << fg_ima_cfg;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (fg_ima_cfg >= PM_FG_MEMIF_IMA_CFG__INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    ima_cfg_reg = ADDRESS(ima_cfg);
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, ima_cfg_reg, mask, (pm_register_data_type)(enable << fg_ima_cfg), 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_get_ima_sts(uint32 pmic_device, pm_fg_memif_ima_sts *fg_ima_sts)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type ima_sts_addr     = 0;
  pm_register_address_type dig_major_addr   = 0;
  pm_register_address_type ima_rdy_sts_addr = 0;
  pm_register_data_type    data[3]          = { 0, 0, 0 };
  pm_register_data_type    data1 = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (fg_ima_sts == NULL )
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    ima_sts_addr     = ADDRESS(ima_operation_sts);
    dig_major_addr   = ADDRESS(dig_major);
    ima_rdy_sts_addr = ADDRESS(ima_rdy_sts);

    err_flag = pm_comm_read_byte_array(SLAVE_ID, ima_sts_addr, 3, (uint8*) &data, 0);

    err_flag |= pm_comm_read_byte(SLAVE_ID, dig_major_addr, &data1, 0);

    if (data1 >= PMI_DIG_MAJ_REV1)
    {
      err_flag |= pm_comm_read_byte(SLAVE_ID, ima_rdy_sts_addr, &data1, 0);
      fg_ima_sts->opr_sts.iacs_rdy         = (data1 & PM_BIT(0))? TRUE : FALSE;
    }
    else
    {
      fg_ima_sts->opr_sts.iacs_rdy         = (data[0] & PM_BIT(1))? TRUE : FALSE;
    }

    fg_ima_sts->opr_sts.iacs_intr_sts      = (data[0] & PM_BIT(3))? TRUE : FALSE;
    fg_ima_sts->opr_sts.iacs_clk_req       = (data[0] & PM_BIT(4))? TRUE : FALSE;
    fg_ima_sts->opr_sts.rd_fgxct_prd_log   = (data[0] & PM_BIT(5))? TRUE : FALSE;
    fg_ima_sts->opr_sts.wr_fgxct_prd_log   = (data[0] & PM_BIT(6))? TRUE : FALSE;
    fg_ima_sts->opr_sts.fgxct_prd          = (data[0] & PM_BIT(7))? TRUE : FALSE;

    fg_ima_sts->excep_sts.iacs_err         = (data[1] & PM_BIT(0))? TRUE : FALSE;
    fg_ima_sts->excep_sts.xct_type_err     = (data[1] & PM_BIT(1))? TRUE : FALSE;
    fg_ima_sts->excep_sts.be_burstwr_warn  = (data[1] & PM_BIT(2))? TRUE : FALSE;
    fg_ima_sts->excep_sts.data_rd_err      = (data[1] & PM_BIT(3))? TRUE : FALSE;
    fg_ima_sts->excep_sts.data_wr_err      = (data[1] & PM_BIT(4))? TRUE : FALSE;
    fg_ima_sts->excep_sts.addr_burst_wrap  = (data[1] & PM_BIT(5))? TRUE : FALSE;
    fg_ima_sts->excep_sts.addr_stable_err  = (data[1] & PM_BIT(7))? TRUE : FALSE;

    fg_ima_sts->hw_sts.data_msb_rd_tgl_rcv = (data[2] & PM_BIT(1))? TRUE : FALSE;
    fg_ima_sts->hw_sts.data_msb_wr_tgl_rcv = (data[2] & PM_BIT(2))? TRUE : FALSE;
    fg_ima_sts->hw_sts.addr_msb_wr_tgl_rcv = (data[2] & PM_BIT(3))? TRUE : FALSE;
    fg_ima_sts->hw_sts.data_msb_rd_tgl     = (data[2] & PM_BIT(5))? TRUE : FALSE;
    fg_ima_sts->hw_sts.data_msb_wr_tgl     = (data[2] & PM_BIT(6))? TRUE : FALSE;
    fg_ima_sts->hw_sts.addr_msb_wr_tgl     = (data[2] & PM_BIT(7))? TRUE : FALSE;
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_get_fg_beat_count(uint32 pmic_device, uint8 *beat_cnt)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type vt_cnt_reg = 0;
  pm_register_data_type data = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);


  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }else if (NULL == beat_cnt)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    vt_cnt_reg = ADDRESS(fg_beat_count);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, vt_cnt_reg, 0x0F, &data, 0);
    *beat_cnt = (boolean)data;
  }

  return err_flag;
}



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
pm_err_flag_type pm_fg_mem_if_set_ima_byte_en_cfg(uint32 pmic_device, pm_fg_mem_if_ima_byte_en_cfg fg_ima_byte_en_cfg, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type ima_byte_en_reg = 0;
  uint8 mask = 0x0F;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (fg_ima_byte_en_cfg >= PM_FG_MEMIF_IMA_BYTE_EN_INVALID )
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    ima_byte_en_reg = ADDRESS(ima_byte_en);
    if(PM_FG_MEMIF_IMA_BYTE_EN_ALL == fg_ima_byte_en_cfg)
    {
      err_flag = pm_comm_write_byte_mask(SLAVE_ID, ima_byte_en_reg, mask, 0x0F, 0);
    }
    else
    {
      err_flag = pm_comm_write_byte_mask(SLAVE_ID, ima_byte_en_reg, mask, (pm_register_data_type)(enable << fg_ima_byte_en_cfg), 0);
    }
  }

  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_run_ima_clr_sequence(uint32 pmic_device)
{
  pm_err_flag_type         err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type    wr_data  = 0;
  pm_fg_data_type         *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    /* Clear sequence from MDOS */
    err_flag  = pm_fg_mem_if_write_addr_msb(pmic_device, 0x00);
    err_flag |= pm_fg_mem_if_write_data3(pmic_device, wr_data);
    err_flag |= pm_fg_mem_if_read_data_reg3(pmic_device, &wr_data);
  }

  return err_flag;
}


pm_err_flag_type
pm_fg_memif_get_dma_err_sts(uint32 pmic_device, boolean *dma_err_sts)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type dma_sts_reg = 0;
  pm_register_data_type    data        = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if ( (NULL == fg_mem_if_ptr) || (dma_err_sts == NULL) )
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    dma_sts_reg = ADDRESS(dma_sts);
    err_flag = pm_comm_read_byte(SLAVE_ID, dma_sts_reg, &data, 0);
    if (PM_ERR_FLAG_SUCCESS == err_flag)
    {
      *dma_err_sts = (data & 0x06) ? TRUE : FALSE; //Check if RD or WR occurs
    }
  }

  return err_flag;
}


pm_err_flag_type
pm_fg_memif_dma_clear(uint32 pmic_device, boolean clear_log)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type dma_ctl_reg = 0;
  pm_register_data_type    data        = 0;
  pm_register_data_type    mask        = 0x01;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    dma_ctl_reg = ADDRESS(dma_intf_ctl);
    data = clear_log? 0x01:0x00;
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, dma_ctl_reg, mask, data, 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_set_ima_cfg_value(uint32 pmic_device, uint8 value)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type ima_cfg_reg = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    ima_cfg_reg = ADDRESS(ima_cfg);
    err_flag = pm_comm_write_byte(SLAVE_ID, ima_cfg_reg, (pm_register_data_type)value, 0);
  }

  return err_flag;
}

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
pm_err_flag_type pm_fg_mem_if_set_mem_intf_ctl_value(uint32 pmic_device, uint8 value)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type intf_ctl;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    intf_ctl = ADDRESS(mem_intf_ctl);
    err_flag = pm_comm_write_byte(SLAVE_ID, intf_ctl, (pm_register_data_type)value, 0);
  }
  return err_flag;
}


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
pm_err_flag_type pm_fg_mem_if_set_mem_intf_cfg_value(uint32 pmic_device, uint8 value)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type intf_cfg;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    intf_cfg = ADDRESS(mem_intf_cfg);
    err_flag = pm_comm_write_byte(SLAVE_ID, intf_cfg,  (pm_register_data_type)(value), 0);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_mem_if_get_ima_hw_sts(uint32 pmic_device, uint8 *data)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type ima_hw_err = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == data )
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    ima_hw_err = ADDRESS(ima_hardware_sts);
    err_flag = pm_comm_read_byte(SLAVE_ID, ima_hw_err, data, 0);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_mem_if_get_fg_pmic_info(uint8 *pmic_index, uint8 *slave_id)
{
    if(!pmic_index || !slave_id)
        return PM_ERR_FLAG_INVALID_PARAMETER;

    *pmic_index = (PM_MAX_NUM_PMICS + 1);
    *slave_id   = (PM_MAX_NUM_SLAVE_IDS + 1);

    pm_fg_get_pmic_info(pmic_index, slave_id);

    if(*pmic_index >= PM_MAX_NUM_PMICS || *slave_id >= PM_MAX_NUM_SLAVE_IDS)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        return PM_ERR_FLAG_SUCCESS;
    }
}


pm_err_flag_type 
pm_fg_memif_dma_ctl(uint32 pmic_device, pm_fg_mem_if_dma_ctl_cfg dma_ctl_cfg)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type dma_ctl_reg = 0;
  pm_register_data_type    data        = 0;
  pm_register_data_type    mask        = 0x02;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    dma_ctl_reg = ADDRESS(dma_intf_ctl);
    data = 1 << dma_ctl_cfg;
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, dma_ctl_reg, mask, data, 0);
  }

  return err_flag;
}


pm_err_flag_type 
pm_fg_memif_arb_ctl(uint32 pmic_device, pm_fg_mem_if_arb_ctl_cfg arb_ctl_cfg, boolean enable)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type arb_cfg_reg = 0;
  pm_register_data_type    mask        = 0;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    arb_cfg_reg  = ADDRESS(mem_arb_cfg);
    mask = 1 << arb_ctl_cfg;
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, arb_cfg_reg, mask, enable << arb_ctl_cfg, 0);
  }

  return err_flag;
}

pm_err_flag_type 
pm_fg_memif_dma_write_byte(uint32 pmic_device, uint16 addr, uint8 data)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if ((addr > FG_SRAM_END_DMA_ADDR )|| (addr < FG_SRAM_START_DMA_ADDR))
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_comm_write_byte(SLAVE_ID, addr, data, 0);
  }

  return err_flag;
}


pm_err_flag_type 
pm_fg_memif_dma_write_long(uint32 pmic_device, uint16 addr, uint8 *data, uint32 writelength)
{
  pm_err_flag_type   err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if ((addr > FG_SRAM_END_DMA_ADDR )|| (addr < FG_SRAM_START_DMA_ADDR )|| (writelength > FG_SRAM_DMA_MAX_WRITE_LENGTH))
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_comm_write_byte_array(SLAVE_ID, addr, writelength, data, 0);
  }

  return err_flag;
}

pm_err_flag_type 
pm_fg_memif_dma_read_long(uint32 pmic_device, uint16 addr, uint8 *data, uint32 readlength)
{
  pm_err_flag_type   err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if ((addr > FG_SRAM_END_DMA_ADDR )|| (addr < FG_SRAM_START_DMA_ADDR ) || (readlength > FG_SRAM_DMA_MAX_WRITE_LENGTH))
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else if (NULL == fg_mem_if_ptr)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_comm_read_byte_array(SLAVE_ID, addr, readlength, data, 0);
  }

  return err_flag;
}


pm_err_flag_type 
pm_fg_memif_dma_read_byte(uint32 pmic_device, uint16 addr, uint8 *data)
{
  pm_err_flag_type         err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_mem_if_ptr = pm_fg_get_data(pmic_device);

  if ((addr > FG_SRAM_END_DMA_ADDR )|| (addr < FG_SRAM_START_DMA_ADDR ))
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else if ((NULL == data )|| (NULL == fg_mem_if_ptr))
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_comm_read_byte(SLAVE_ID, addr, data, 0);
  }
  return err_flag;
}

