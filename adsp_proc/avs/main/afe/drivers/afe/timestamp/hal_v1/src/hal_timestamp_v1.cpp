/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/hal_v1/src/hal_timestamp_v1.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hal_timestamp_v3.cpp

DESCRIPTION: VFR register programming

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013-2014,2017 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary. 
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/hal_v1/src/hal_timestamp_v1.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/02/2014  AR      created

============================================================================*/

#include "qurt_elite.h"
#include "LPASS_ADDRESS_FILE_TS_V1.h"
#include "common.h"
#include "hal_timestamp.h"
#include "AfeTimeStampDriver.h"


/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

#define VFR_TS_OFFSET_0(_reg_) ((_reg_)-LPASS_VFR_IRQ_MUX_CTL)
#define VFR_TS_OFFSET_1(_reg_) ((_reg_)-LPASS_VFR_IRQ_MUX_CTL_1)
#define GP_TS_OFFSET_0(_reg_) ((_reg_)-LPASS_GP_IRQ_TS_MUX_CTL_0)
#define GP_TS_OFFSET_1(_reg_) ((_reg_)-LPASS_GP_IRQ_TS_MUX_CTL_1)


volatile static uint32_t         vfr_reg_base = 0; 
static int32_t                   vfr_mux_reg_virtoffset0 = 0;
static int32_t                   vfr_mux_reg_virtoffset1 = 0;
static int32_t                   vfr_gp_mux_reg_virtoffset0 = 0;
static int32_t                   vfr_gp_mux_reg_virtoffset1 = 0;

void hal_ts_init_v1(vfr_hw_info_t *vfr_hw_info_ptr)
{
   vfr_reg_base = vfr_hw_info_ptr->reg_base_addr;

   vfr_mux_reg_virtoffset0 = reg_virt_offset(vfr_hw_info_ptr->reg_virt_base, vfr_reg_base, vfr_reg_base);
   vfr_mux_reg_virtoffset1 = reg_virt_offset(vfr_hw_info_ptr->reg_virt_base, vfr_reg_base, vfr_hw_info_ptr->vfr_ts_mux_offset1);
   vfr_gp_mux_reg_virtoffset0 = reg_virt_offset(vfr_hw_info_ptr->reg_virt_base, vfr_reg_base, vfr_hw_info_ptr->gp_ts_mux_offset0);
   vfr_gp_mux_reg_virtoffset1 = reg_virt_offset(vfr_hw_info_ptr->reg_virt_base, vfr_reg_base, vfr_hw_info_ptr->gp_ts_mux_offset1);

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "vfr hal v1 [vfr_reg_base:0x%x] [vfr virt base:0x%x] [vfr offset0:0x%x] [vfr offset1:0x%x] [vfr gp offset0:0x%x] [vfr gp offset1:0x%x]", \
      vfr_reg_base, vfr_hw_info_ptr->reg_virt_base, vfr_mux_reg_virtoffset0, vfr_mux_reg_virtoffset1, vfr_gp_mux_reg_virtoffset0, vfr_gp_mux_reg_virtoffset1);
}

void hal_ts_set_source_type_v1(uint32_t mux_id, uint32 mux_ctl_sel)
{
   uint32 field_value; 
   switch(mux_id)
   {
      case AFE_TS_VFR0_MUX_ID:
         field_value = mux_ctl_sel << LPASS_VFR_IRQ_MUX_CTL__SEL___S;
         UpdateRegisterCSR(vfr_reg_base + VFR_TS_OFFSET_0(LPASS_VFR_IRQ_MUX_CTL), vfr_mux_reg_virtoffset0, LPASS_VFR_IRQ_MUX_CTL__SEL___M, field_value, 0);
         break;
      case AFE_TS_VFR1_MUX_ID:
         field_value = mux_ctl_sel << LPASS_VFR_IRQ_MUX_CTL_1__SEL___S;
         UpdateRegisterCSR(vfr_reg_base + VFR_TS_OFFSET_1(LPASS_VFR_IRQ_MUX_CTL_1), vfr_mux_reg_virtoffset1, LPASS_VFR_IRQ_MUX_CTL_1__SEL___M, field_value, 0);
         break;
      case AFE_TS_GP0_MUX_ID:
         field_value = mux_ctl_sel << LPASS_GP_IRQ_TS_MUX_CTL_0___S;
         UpdateRegisterCSR(vfr_reg_base + GP_TS_OFFSET_0(LPASS_GP_IRQ_TS_MUX_CTL_0), vfr_gp_mux_reg_virtoffset0, LPASS_GP_IRQ_TS_MUX_CTL_0___M, field_value, 0);
         break;
      case AFE_TS_GP1_MUX_ID:
         field_value = mux_ctl_sel << LPASS_GP_IRQ_TS_MUX_CTL_1___S;
         UpdateRegisterCSR(vfr_reg_base + GP_TS_OFFSET_1(LPASS_GP_IRQ_TS_MUX_CTL_1), vfr_gp_mux_reg_virtoffset1, LPASS_GP_IRQ_TS_MUX_CTL_1___M, field_value, 0);
         break;
      default:
         break;
   }

   return;
}

void hal_ts_set_timer_source_v1(uint32_t mux_id, uint32 timer_source)
{
   uint32 field_value;   
   field_value =  timer_source << LPASS_VFR_IRQ_MUX_CTL__VFR___S;

   switch(mux_id)
   {
      case AFE_TS_VFR0_MUX_ID:
         UpdateRegisterCSR(vfr_reg_base + VFR_TS_OFFSET_0(LPASS_VFR_IRQ_MUX_CTL), vfr_mux_reg_virtoffset0, LPASS_VFR_IRQ_MUX_CTL__VFR___M, field_value, 0);
         break;
      case AFE_TS_VFR1_MUX_ID:
         UpdateRegisterCSR(vfr_reg_base + VFR_TS_OFFSET_1(LPASS_VFR_IRQ_MUX_CTL_1), vfr_mux_reg_virtoffset1, LPASS_VFR_IRQ_MUX_CTL_1__VFR___M, field_value, 0);
         break;
      case AFE_TS_GP0_MUX_ID:
         UpdateRegisterCSR(vfr_reg_base + GP_TS_OFFSET_0(LPASS_GP_IRQ_TS_MUX_CTL_0), vfr_gp_mux_reg_virtoffset0, LPASS_GP_IRQ_TS_MUX_CTL_0__VFR___M, field_value, 0);
         break;
      case AFE_TS_GP1_MUX_ID:
         UpdateRegisterCSR(vfr_reg_base + GP_TS_OFFSET_1(LPASS_GP_IRQ_TS_MUX_CTL_1), vfr_gp_mux_reg_virtoffset1, LPASS_GP_IRQ_TS_MUX_CTL_1__VFR___M, field_value, 0);
         break;
      default:
         break;
   }

   return;
}

void hal_ts_get_timestamp_v1(uint32_t mux_id, uint64_t *timestamp)
{
   uint32_t             lsb, msb1, msb2;
  
   *timestamp = 0;
   switch(mux_id)
   {
      case AFE_TS_VFR0_MUX_ID:
         ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_0(LPASS_VFR_STC_MSB), vfr_mux_reg_virtoffset0, &msb1);
         msb1 = (msb1 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;
         ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_0(LPASS_VFR_STC_LSB), vfr_mux_reg_virtoffset0, &lsb);
         ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_0(LPASS_VFR_STC_MSB), vfr_mux_reg_virtoffset0, &msb2);
         msb2 = (msb2 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;
         
         //if MSB has flipped then read LSB register again
         if(msb2 != msb1)
         {
            ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_0(LPASS_VFR_STC_LSB), vfr_mux_reg_virtoffset0, &lsb);
         }
       
         break;
      case AFE_TS_VFR1_MUX_ID:
         ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_1(LPASS_VFR_STC_MSB_1), vfr_mux_reg_virtoffset1, &msb1);
         msb1 = (msb1 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;
         ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_1(LPASS_VFR_STC_LSB_1), vfr_mux_reg_virtoffset1, &lsb);
         ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_1(LPASS_VFR_STC_MSB_1), vfr_mux_reg_virtoffset1, &msb2);
         msb2 = (msb2 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;

         //if MSB has flipped then read LSB register again
         if(msb2 != msb1)
         {
      	   ReadRegister1(vfr_reg_base + VFR_TS_OFFSET_1(LPASS_VFR_STC_LSB_1), vfr_mux_reg_virtoffset1, &lsb);
         }
         break;
      case AFE_TS_GP0_MUX_ID:
         ReadRegister1(vfr_reg_base + GP_TS_OFFSET_0(LPASS_GP_IRQ_STC_MSB_0), vfr_gp_mux_reg_virtoffset0, &msb1);
         msb1 = (msb1 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;
         ReadRegister1(vfr_reg_base + GP_TS_OFFSET_0(LPASS_GP_IRQ_STC_LSB_0), vfr_gp_mux_reg_virtoffset0, &lsb);
         ReadRegister1(vfr_reg_base + GP_TS_OFFSET_0(LPASS_GP_IRQ_STC_MSB_0), vfr_gp_mux_reg_virtoffset0, &msb2);
         msb2 = (msb2 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;

         //if MSB has flipped then read LSB register again
         if(msb2 != msb1)
         {
      	   ReadRegister1(vfr_reg_base + GP_TS_OFFSET_0(LPASS_GP_IRQ_STC_LSB_0), vfr_gp_mux_reg_virtoffset0, &lsb);
         }
         break;
      case AFE_TS_GP1_MUX_ID:
         ReadRegister1(vfr_reg_base + GP_TS_OFFSET_1(LPASS_GP_IRQ_STC_MSB_1), vfr_gp_mux_reg_virtoffset1, &msb1);
         msb1 = (msb1 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;
         ReadRegister1(vfr_reg_base + GP_TS_OFFSET_1(LPASS_GP_IRQ_STC_LSB_1), vfr_gp_mux_reg_virtoffset1, &lsb);
         ReadRegister1(vfr_reg_base + GP_TS_OFFSET_1(LPASS_GP_IRQ_STC_MSB_1), vfr_gp_mux_reg_virtoffset1, &msb2);
         msb2 = (msb2 & LPASS_VFR_STC_MSB___M) >> LPASS_VFR_STC_MSB___S;

         //if MSB has flipped then read LSB register again
         if(msb2 != msb1)
         {
      	   ReadRegister1(vfr_reg_base + GP_TS_OFFSET_1(LPASS_GP_IRQ_STC_LSB_1), vfr_gp_mux_reg_virtoffset1, &lsb);
         }
         break;
      default:
         return;
   }


   lsb = (lsb & LPASS_VFR_STC_LSB___M) >> LPASS_VFR_STC_LSB___S;

   *timestamp = ((uint64_t)msb2 << 32) | lsb;

   return;
}

void hal_ts_config_vfr_ts_mux_v1(uint32_t intr_type, 
                                 uint32_t mux_ctl_sel_in,
                                 uint32_t *mux_id)
{
   ADSPResult     result = ADSP_EOK;
   uint32_t       mux_ctl_sel, num_ts_mux, *mux_bit_field;

   *mux_id = AFE_TS_INVALID_MUX_ID;

   /* Get the number of VFR mux and available mux bit field as per interrupt type */
   switch(intr_type)
   {
      case HW_TS_INTR_TYPE_VFR:
         {
            num_ts_mux = vfr_drv_global_stat.hw_info.num_vfr_ts_mux;
            mux_bit_field = &ts_hw_drv_global.vfr_ts_mux_bitfield;
         }
         break;
      case HW_TS_INTR_TYPE_BT_TX:
      case HW_TS_INTR_TYPE_BT_RX:
      case HW_TS_INTR_TYPE_FM:
         {
            num_ts_mux = vfr_drv_global_stat.hw_info.num_gp_ts_mux;
            mux_bit_field = &ts_hw_drv_global.gp_ts_mux_bitfield;
         }
         break;
      default:
         return;
   }

   /* Get the available mux ID */
   if( ADSP_EOK == (result = hal_ts_get_available_mux_id(num_ts_mux, mux_bit_field, mux_id)) )
   {
      if(HW_TS_INTR_TYPE_VFR != intr_type)
      {
         *mux_id += AFE_TS_GP0_MUX_ID;
      }
   }
   else if(HW_TS_INTR_TYPE_VFR == intr_type)
   {
      /* Search if any mux is free in GP TS mux which can be used for VFR TS */
      if( ADSP_EOK == (result = hal_ts_get_available_mux_id(vfr_drv_global_stat.hw_info.num_gp_ts_mux,
                                                            &ts_hw_drv_global.gp_ts_mux_bitfield,
                                                            mux_id)) )
      {
         *mux_id += AFE_TS_GP0_MUX_ID;
      }
   }
   else
{
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HW TS mux not available, intr_type: 0x%lx", intr_type);
      return;
   }

   switch(intr_type)
   {
      case HW_TS_INTR_TYPE_VFR:
         mux_ctl_sel = mux_ctl_sel_in;
         break;
      case HW_TS_INTR_TYPE_BT_TX:
         mux_ctl_sel = LPASS_VFR_IRQ_MUX_CTL__SEL__BLUETOOTH_VFR_0;
         break;
      case HW_TS_INTR_TYPE_BT_RX:
         mux_ctl_sel = LPASS_VFR_IRQ_MUX_CTL__SEL__BLUETOOTH_VFR_1;
         break;
      case HW_TS_INTR_TYPE_FM:
         mux_ctl_sel = LPASS_VFR_IRQ_MUX_CTL__SEL__FM_VFR;
         break;
      default:
         return;
   }

   /* Configure the TS mux input */
   hal_ts_set_source_type_v1(*mux_id, mux_ctl_sel);

   /* Configure it to follow AV-timer LPASS_VFR_IRQ_MUX_CTL__VFR__AVTIMER*/
   hal_ts_set_timer_source_v1(*mux_id, LPASS_VFR_IRQ_MUX_CTL__VFR__AVTIMER);

   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW TS mux config success, mux_id: 0x%x, mux_ctl_sel: 0x%x", *mux_id, mux_ctl_sel);

   return;
}
