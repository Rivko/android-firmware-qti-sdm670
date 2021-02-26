/*! \file vib_api.c
*  \n
*  \details This file contains the top-level API wrappers for the Vib
*           peripheral.
*
*  \n &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/14   al       Updating comm lib 
05/20/14   al       Architecture update  
========================================================================== */

/*===========================================================================
INCLUDE FILES
===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_vib.h"
#include "pm_vib_driver.h"
#include "pm_comm.h"

//Macros for 8994 Haptics support
#define LRA_AUTORES_CALIB_SBL_ONLY 1

#define VIB_CFG1_ACTUATOR_TYPE_VALUE             0      //0-LRA,1-ERM (Bit - 0), reg - 0x1C04C 
#define VIB_CFG1_REG                             0xC04C //Reg Address without Slave ID
#define VIB_CFG1_REG_MASK                        0x01

#define VIB_CFG2_LRA_RES_TYPE_VALUE              1      //0 - SINUSOID (Bit - 0) reg - 0x1C04D
                                                        //1 - SQUARE
#define VIB_CFG2_REG                             0xC04D //Reg Address Without Slave Id
#define VIB_CFG2_REG_MASK                        0x01

#define VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_BIT_SHIFT 4

#define VIB_AUTO_RES_CFG_REG                     0xC04F //Reg Address without Slave Id
#define VIB_AUTO_RES_CFG_LRA_NO_AUTO_RES_VAL     0
#define VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_VAL   4 //0 - No_Auto_Res (Bits - 6:4) reg - 0x1C04F
                                                   //1 - ZXD
                                                   //2 - QWD
                                                   //3 - MAX_QWD
                                                   //4 - ZXD_EOP
#define VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_MASK  0x70


#define VIB_AUTO_RES_CFG_LRA_HIGH_Z_VAL          3 //0 - NO_HIGHZ (Bits - 3:2) reg - 0x1C04F
                                                   //1 - HIGHZ_OPT1
                                                   //2 - HIGHZ_OPT2
                                                   //3 - HIGHZ_OPT3
#define VIB_AUTO_RES_CFG_LRA_HIGH_Z_MASK         0x0C
#define VIB_AUTO_RES_CFG_LRA_HIGH_Z_BIT_SHIFT    2

#define VIB_AUTO_RES_CFG_LRA_RES_CALIB_VAL       3 //0 - FOUR_PERIODS (Bits - 1:0) reg - 0x1C04F
                                                   //1 - EIGHT_PERIODS
                                                   //2 - SIXTEEN_PERIODS
                                                   //3 - THIRTYTWO_PERIODS
#define VIB_AUTO_RES_CFG_LRA_RES_CALIB_MASK      0x03



#define VIB_ILIM_SEL_VAL                         0 //0 - ILIM_400MA (Bits - 0) reg - 0x1C052
                                                   //1 - ILIM_800MA
#define VIB_ILIM_REG                             0xC052
#define VIB_ILIM_SEL_MASK                        0x01


#define VIB_PWM_CAP_SEL_VAL                      1 //0 - PWM_CAP_26PF (Bits: 1- 0) reg - 0x1C058
                                                   //1 - PWM_CAP_13PF
                                                   //2 - PWM_CAP_8P7PF
                                                   //3 - PWM_CAP_6P5PF
#define VIB_PWM_CAP_REG                          0xC058
#define VIB_PWM_CAP_SEL_MASK                     0x03


#define VIB_EXTERNAL_PWM_FREQ_SEL_VAL            0 //0 - EXT_PWM_25KHZ (Bits: 1 - 0) reg - 0x1C057
                                                   //1 - EXT_PWM_50KHZ
                                                   //2 - EXT_PWM_75KHZ
                                                   //3 - EXT_PWM_100KHZ
#define VIB_EXTERNAL_PWM_REG                     0xC057
#define VIB_EXTERNAL_PWM_FREQ_SEL_MASK           0x03
                                                   
#define VIB_INTERNAL_PWM_FREQ_SEL_VAL            1 //0 - PWM_253KHZ (Bits: 1 - 0) reg - 0x1C056
                                                   //1 - PWM_505KHZ
                                                   //2 - PWM_739KHZ
                                                   //3 - PWM_1076KHZ
#define VIB_INTERNAL_PWM_REG                     0xC056
#define VIB_INTERNAL_PWM_FREQ_SEL_MASK           0x03

#define VIB_RATE_CFG1_REG                        0xC054 // Bits: 7 - 0; reg - 0x1C054
#define VIB_RATE_CFG1_VAL                        0xCF   // Corresponds to 207, with value 3 in CFG2, it is 205 Hz

#define VIB_RATE_CFG2_REG                        0xC055 // Bits: 7 - 0; reg - 0x1C055
#define VIB_RATE_CFG2_VAL                        0x03   // with value with value 207 in CFG1, it is 205 Hz

#define VIB_CFG2_REG                             0xC04D // Bits: 0; reg - 0x1C04D
#define VIB_CFG2_LRA_RES_TYPE_VAL                1      // Default Square Wave

#define VIB_SLEW_CFG                             0xC050 //Bits: 0; reg - 0x1C050
#define VIB_SLEW_RATE_VAL                        1      //Default 0 - 6 ns; 1- 16 ns


pm_err_flag_type pm_vib_init(pm_vib_data_type *vib_ptr, pm_vib_which_type vib);

pm_err_flag_type pm_vib_enable
(
   uint32            pmic_chip,
   pm_vib_which_type vib,
   pm_vib_mode_type  mode,
   boolean           enable
   )
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

/* //Vib perepheral do not exist PMI8994.  Need to use Haptics instead
 
   pm_register_address_type reg_vib_en_ctl = 0;
   pm_register_address_type reg_vib_play = 0;
   pm_register_address_type periph_index = (pm_register_address_type)vib;
   pm_vib_data_type *vib_ptr = pm_vib_get_data(pmic_chip);

   if (NULL == vib_ptr)
   {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }

   if (periph_index >= vib_ptr->num_of_peripherals)
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   if (TRUE == enable)
   {
      err_flag = pm_vib_init(vib_ptr, vib);
      if (err_flag != PM_ERR_FLAG_SUCCESS)
      {
         goto pm_vib_enable_exit;
      }
   }


   reg_vib_en_ctl = vib_ptr->vib_reg_table->base_address + (vib_ptr->vib_reg_table->peripheral_offset * periph_index) + vib_ptr->vib_reg_table->en_ctl;
   reg_vib_play = vib_ptr->vib_reg_table->base_address + (vib_ptr->vib_reg_table->peripheral_offset * periph_index) + vib_ptr->vib_reg_table->play;

   switch (mode)
   {
      case PM_VIB_MODE__MANUAL:
         err_flag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib_play, 0x80, enable << 7, 0);
         if (err_flag != PM_ERR_FLAG_SUCCESS)
         {
            goto pm_vib_enable_exit;
         }

         if ((enable) || (VIB_CFG1_ACTUATOR_TYPE_VALUE ==  1) || ((VIB_CFG1_ACTUATOR_TYPE_VALUE == 0) && (LRA_AUTORES_CALIB_SBL_ONLY != 1)))
         {
            err_flag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib_en_ctl, 0x80, (pm_register_data_type)(enable << 7), 0);
            if (err_flag != PM_ERR_FLAG_SUCCESS)
            {
               goto pm_vib_enable_exit;
            }
         }
         break;
      default:
         err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }

pm_vib_enable_exit:
*/
   return err_flag;
}


pm_err_flag_type pm_vib_set_volt
(
   uint32            pmic_chip,
   pm_vib_which_type  vib,
   uint16             voltage
   )
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

/* //Vib perepheral do not exist PMI8994.  Need to use Haptics instead

   pm_register_address_type reg_vib_vmax_cfg = 0;
   pm_register_address_type periph_index = (pm_register_address_type)vib;
   pm_vib_data_type *vib_ptr = pm_vib_get_data(pmic_chip);

   if (NULL == vib_ptr)
   {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }

   if (periph_index >= vib_ptr->num_of_peripherals)
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   reg_vib_vmax_cfg = vib_ptr->vib_reg_table->base_address + (vib_ptr->vib_reg_table->peripheral_offset * periph_index) + vib_ptr->vib_reg_table->vmax_cfg;

   if (voltage < vib_ptr->vib_data->MinVoltage ||
       voltage > vib_ptr->vib_data->MaxVoltage)
   {
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      err_flag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib_vmax_cfg, 0x3E, (pm_register_data_type)(voltage / 116), 0);
   }
*/

   return err_flag;
}

pm_err_flag_type pm_vib_init(pm_vib_data_type *vib_ptr, pm_vib_which_type vib)
{
   pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;

/* //Vib perepheral do not exist PMI8994.  Need to use Haptics instead
 
   pm_register_address_type reg_vib =  0x0;
   pm_register_data_type data = 0x0;


   reg_vib = VIB_CFG1_REG;
   //select Vib Actuator type
   errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, VIB_CFG1_REG_MASK, VIB_CFG1_ACTUATOR_TYPE_VALUE, 0);
   if (errFlag != PM_ERR_FLAG_SUCCESS)
   {
      goto pm_vib_init_exit;
   }

   if (VIB_CFG1_ACTUATOR_TYPE_VALUE == 1) // ERM actuator
   {
      //disable auto resonance
      reg_vib = VIB_AUTO_RES_CFG_REG;
      data = (VIB_AUTO_RES_CFG_LRA_NO_AUTO_RES_VAL << VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_BIT_SHIFT);

      //Configure Auto Resonance configuration
      errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, 0x7F, data, 0);
      if (errFlag != PM_ERR_FLAG_SUCCESS)
      {
         goto pm_vib_init_exit;
      }
   }

   else
   {
      // if the mode is QWD, then set ZXD Calibration freq as 0
      if (2 == VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_VAL)
      {
         data = ((VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_VAL << VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_BIT_SHIFT) |
                    (VIB_AUTO_RES_CFG_LRA_HIGH_Z_VAL << VIB_AUTO_RES_CFG_LRA_HIGH_Z_BIT_SHIFT) |
                    (0));
      }
      else
      {
         data = ((VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_VAL << VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_BIT_SHIFT) |
                    (VIB_AUTO_RES_CFG_LRA_HIGH_Z_VAL << VIB_AUTO_RES_CFG_LRA_HIGH_Z_BIT_SHIFT) |
                    (VIB_AUTO_RES_CFG_LRA_RES_CALIB_VAL));
      }

      reg_vib = VIB_AUTO_RES_CFG_REG;

      //Configure Auto Resonance configuration
      errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, 0x7F, data, 0);
      if (errFlag != PM_ERR_FLAG_SUCCESS)
      {
         goto pm_vib_init_exit;
      }

      //Configure CFG2 LRA_RES_TYPE to Square type
      reg_vib = VIB_CFG2_REG;
      data = VIB_CFG2_LRA_RES_TYPE_VAL;
      errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, 0x01, data, 0);
      if (errFlag != PM_ERR_FLAG_SUCCESS)
      {
         goto pm_vib_init_exit;
      }

      //Configure Rate_CFG1 and Rate_CFG2 for LRA resonant freq of 205 Hz
      //Rate_CFG1 will be 207 and Rate_CFG2 will be 3 for LRA resonant (205 Hz - Value 975)
      reg_vib = VIB_RATE_CFG1_REG;
      data = VIB_RATE_CFG1_VAL;
      errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, 0xFF, data, 0);
      if (errFlag != PM_ERR_FLAG_SUCCESS)
      {
         goto pm_vib_init_exit;
      }

      reg_vib = VIB_RATE_CFG2_REG;
      data = VIB_RATE_CFG2_VAL;
      errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, 0xFF, data, 0);
      if (errFlag != PM_ERR_FLAG_SUCCESS)
      {
         goto pm_vib_init_exit;
      }

   }

   //Configure Slew rate
   reg_vib = VIB_SLEW_CFG;
   data = VIB_SLEW_RATE_VAL;
   errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, 0x01, data, 0);
   if (errFlag != PM_ERR_FLAG_SUCCESS)
   {
      goto pm_vib_init_exit;
   }

   //Configure ILIM
   reg_vib = VIB_ILIM_REG;
   errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, VIB_ILIM_SEL_MASK, VIB_ILIM_SEL_VAL, 0);
   if (errFlag != PM_ERR_FLAG_SUCCESS)
   {
      goto pm_vib_init_exit;
   }

   //Configure Internal PWM Freq Sel
   reg_vib = VIB_INTERNAL_PWM_REG;
   errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, VIB_INTERNAL_PWM_FREQ_SEL_MASK, VIB_INTERNAL_PWM_FREQ_SEL_VAL, 0);
   if (errFlag != PM_ERR_FLAG_SUCCESS)
   {
      goto pm_vib_init_exit;
   }

   //Configure External PWM Freq Sel
   reg_vib = VIB_EXTERNAL_PWM_REG;
   errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, VIB_EXTERNAL_PWM_FREQ_SEL_MASK, VIB_EXTERNAL_PWM_FREQ_SEL_VAL, 0);
   if (errFlag != PM_ERR_FLAG_SUCCESS)
   {
      goto pm_vib_init_exit;
   }

   //Configure PWM Capacitance
   reg_vib = VIB_PWM_CAP_REG;
   errFlag = pm_comm_write_byte_mask(vib_ptr->comm_ptr->slave_id, reg_vib, VIB_PWM_CAP_SEL_MASK, VIB_PWM_CAP_SEL_VAL, 0);
   if (errFlag != PM_ERR_FLAG_SUCCESS)
   {
      goto pm_vib_init_exit;
   }

pm_vib_init_exit:
*/
   return errFlag;
}

