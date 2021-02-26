/*! @file PmicTestProtocol.c 

 *  PMIC-TEST MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC Test module.
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/27/17   pb      Exposing API for BOB driver and removing MPP support
10/10/16   al      Adding new key press debounce for compilation
08/19/16   pbarot  Exposed pm_write and read APIs for Access Control. Also expose APIs from other drivers.
06/15/16   sarpit  Added more API support for LPG
06/08/15   rm      New file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIPmicTest.h>
#include <Library/DebugLib.h>
#include <Drivers/PmicDxe/Test/PmicTestGpio.h>
#include <Drivers/PmicDxe/Test/PmicTestBSI.h>
#include <Drivers/PmicDxe/Test/PmicTestLPG.h>
#include <Drivers/PmicDxe/Test/PmicTestAC.h>
#include <Drivers/PmicDxe/Test/PmicTestWLED.h>
#include <Drivers/PmicDxe/Test/PmicTestPON.h>
#include <Drivers/PmicDxe/Test/PmicTestCoincell.h>
#include "pm_uefi.h"
#include "PmicFileLog.h"
#include "pm_gpio.h"
#include "pm_bsi.h"
#include "pm_lpg.h"
#include "pm_haptics.h"
#include "pm_wled.h"
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pm_coincell.h"
#include "pm_bob.h"

// needed for Access control testing (expose pmic spmi write)
#include <../../Library/PmicLib/pmic_devices/common/src/bare_metal_info.h> 
#include <../../Library/PmicLib/comm/src/pm_comm.h>


/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*
====================================================================
 PMIC write
====================================================================
*/

pm_err_flag_type
TestBOBDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

   switch(DriverTypeFunction.BOBTypeFunction)
    {
        case EFI_PM_BOB_VOLT_LEVEL_STATUS:
        {                                                                            
            EFI_PM_TEST_BOB_VOLT_LEVEL_STATUS_PARAM VoltLevelStatus = DriverParam->u.BOBFunctionType.BobVoltLevelStatus;
            errFlag = pm_bob_volt_level_status(VoltLevelStatus.PmicChip,VoltLevelStatus.PeripheralIndex,
                                                  (pm_volt_level_type *) VoltLevelStatus.VoltLevel);
            break;
        }
      default:
         break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;
    
    return errFlag;
    
       
 
}


pm_err_flag_type
TestAccessControl
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    
       
    //EFI_PM_TEST_GPIO_SET_CURRENT_SOURCE_PULLS_PARAM SetCurrentSourcePull = DriverParam->u.GpioFuntionType.GpioSetCurrentSourcePull;
    
    //errFlag = pm_gpio_set_current_source_pulls(SetCurrentSourcePull.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetCurrentSourcePull.GPIOTypeIndex.GPIOType,
     //                                                  (pm_gpio_current_src_pulls_type)SetCurrentSourcePull.CurrentSrcPullType);
    EFI_PM_TEST_AC_WRITE_PARAM WriteParam = DriverParam->u.ACFunctionType.AccessControlWrite;
    
    errFlag = pm_comm_write_byte(WriteParam.SlaveID, WriteParam.Address, WriteParam.Data, 0);  // 1:0  

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;
    
    return errFlag;
}


pm_err_flag_type
TestGPIODriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.GpioTypeFunction)
    {
        case EFI_PM_GPIO_CONFIG_BIAS_VOLTAGE:
        {
            EFI_PM_TEST_GPIO_CONFIG_BIAS_VOLTAGE_PARAM ConfigBiasVoltage = DriverParam->u.GpioFuntionType.GpioConfigBiasVoltage;
            errFlag = pm_gpio_config_bias_voltage(ConfigBiasVoltage.GPIOTypeIndex.GPIOIndex,(pm_gpio_perph_index)ConfigBiasVoltage.GPIOTypeIndex.GPIOType,
                                                  (pm_gpio_volt_src_type)ConfigBiasVoltage.VoltageSource);
            break;
        }
        case EFI_PM_GPIO_CONFIG_DIGITAL_INOUT:
        {
            EFI_PM_TEST_GPIO_CONFIG_DIGITAL_INOUT_PARAM ConfigDigitalInOut = DriverParam->u.GpioFuntionType.GpioConfigDigitalInOut;
            errFlag = pm_gpio_config_digital_input_output(ConfigDigitalInOut.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)ConfigDigitalInOut.GPIOTypeIndex.GPIOType, 
                                                            (pm_gpio_src_config_type)ConfigDigitalInOut.GPIOSrc,
                                                            (pm_gpio_current_src_pulls_type)ConfigDigitalInOut.GPIOPulls,
                                                            (pm_gpio_volt_src_type)ConfigDigitalInOut.VoltageSource, 
                                                            (pm_gpio_out_buffer_config_type)ConfigDigitalInOut.BufferConfig,
                                                            (pm_gpio_out_buffer_drv_strength_type)ConfigDigitalInOut.BufferStrength);
            break;
        }
        case EFI_PM_GPIO_SET_EXT_PIN_CONFIG:
        {
            EFI_PM_TEST_GPIO_SET_EXT_PIN_CONFIG_PARAM SetExtPinConfig = DriverParam->u.GpioFuntionType.GpioSetExtPinConfig;
            errFlag = pm_gpio_set_ext_pin_config(SetExtPinConfig.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetExtPinConfig.GPIOTypeIndex.GPIOType,
                                                 (pm_gpio_ext_pin_config_type)SetExtPinConfig.ConfigType);
            break;
        }
        case EFI_PM_GPIO_SET_VOLTAGE_SOURCE:
        {
            EFI_PM_TEST_GPIO_SET_VOLTAGE_SOURCE_PARAM SetVoltageSource = DriverParam->u.GpioFuntionType.GpioSetVoltageSource;
            errFlag = pm_gpio_set_voltage_source(SetVoltageSource.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetVoltageSource.GPIOTypeIndex.GPIOType,
                                              (pm_gpio_volt_src_type)SetVoltageSource.VoltageSource);
            break;
        }
        case EFI_PM_GPIO_CONFIG_DIGITALINPUT: // already present in PmicGpioProtocol.c (EFI_PmGpioConfigDigitalInput)
          break;

        case EFI_PM_GPIO_CONFIG_DIGITALOUTPUT: // // already present in PmicGpioProtocol.c (EFI_PM_GPIO_CONFIG_DIGITAL_OUTPUT)
          break;

        case EFI_PM_GPIO_SET_INVERSION_CONFIG:
        {
            EFI_PM_TEST_GPIO_SET_INVERSION_CONFIG_PARAM SetInversionConfig = DriverParam->u.GpioFuntionType.GpioSetInversionConfig;
            errFlag = pm_gpio_set_inversion_configuration(SetInversionConfig.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetInversionConfig.GPIOTypeIndex.GPIOType, SetInversionConfig.Inversion);
            break;
        }

        case EFI_PM_GPIO_SET_OUT_BUFFER_DRIVE_STRENGTH:
        {
            EFI_PM_TEST_GPIO_DRIVE_STRENGTH_TYPE_PARAM SetOutBufferDriveStrength = DriverParam->u.GpioFuntionType.GpioDriveStrengthType;
            errFlag = pm_gpio_set_output_buffer_drive_strength(SetOutBufferDriveStrength.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetOutBufferDriveStrength.GPIOTypeIndex.GPIOType,
                                                               (pm_gpio_out_buffer_drv_strength_type)SetOutBufferDriveStrength.DriveStrengthType);
            break;
        }
        case EFI_PM_GPIO_SET_SOURCE_CONFIG:
        {
            EFI_PM_TEST_GPIO_SET_SOURCE_CONFIG_TYPE_PARAM SetSourceConfig = DriverParam->u.GpioFuntionType.GpioSetSourceConfigType;
            errFlag = pm_gpio_set_source_configuration(SetSourceConfig.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetSourceConfig.GPIOTypeIndex.GPIOType,
                                                       (pm_gpio_src_config_type)SetSourceConfig.SourceConfigType);
            break;
        }
        case EFI_PM_GPIO_SET_OUTPUT_BUFFER_CONFIG:
        {
            EFI_PM_TEST_GPIO_OUTPUT_BUFFER_CONFIGURATION_PARAM SetOutputBufferConfig = DriverParam->u.GpioFuntionType.GpioOutputBufferConfigType;
            errFlag = pm_gpio_set_output_buffer_configuration(SetOutputBufferConfig.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetOutputBufferConfig.GPIOTypeIndex.GPIOType,
                                                              (pm_gpio_out_buffer_config_type)SetOutputBufferConfig.BufferConfigType);
            break;
        }
        case EFI_PM_GPIO_SET_CURRENT_SOURCE_PULLS:
        {
            EFI_PM_TEST_GPIO_SET_CURRENT_SOURCE_PULLS_PARAM SetCurrentSourcePull = DriverParam->u.GpioFuntionType.GpioSetCurrentSourcePull;
            errFlag = pm_gpio_set_current_source_pulls(SetCurrentSourcePull.GPIOTypeIndex.GPIOIndex, (pm_gpio_perph_index)SetCurrentSourcePull.GPIOTypeIndex.GPIOType,
                                                       (pm_gpio_current_src_pulls_type)SetCurrentSourcePull.CurrentSrcPullType);
            break;
        }
		case EFI_PM_GPIO_ENABLE:
        {
            EFI_PM_TEST_GPIO_ENABLE_PARAM Enable = DriverParam->u.GpioFuntionType.GpioEnable;
            errFlag = pm_gpio_enable(Enable.GPIOTypeIndex.GPIOIndex,(pm_gpio_perph_index)Enable.GPIOTypeIndex.GPIOType,
                                                  Enable.enable);
            break;
        }
		
		

        default:
        break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;
    
    return errFlag;
}


pm_err_flag_type
TestBSIDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.BsiTypeFunction)
    {
       case EFI_PM_TEST_BSI_BAT_GONE_CFG:
       {
           EFI_PM_TEST_BSI_BAT_GONE_CFG_PARAM BatGoneCFG = DriverParam->u.BSIFuntionType.BsiBatGoneCFG;
           errFlag = pm_bsi_bat_gone_deb_cfg(BatGoneCFG.DeviceIndex, BatGoneCFG.SelectBatt);
           break;
       }
       case EFI_PM_TEST_BSI_CFG_TX_DLY:
       {
           EFI_PM_TEST_BSI_CFG_TX_DLY_PARAM CfgTxDly = DriverParam->u.BSIFuntionType.BsiCfgTxDly;
           errFlag = pm_bsi_cfg_tx_dly(CfgTxDly.DeviceIndex, (pm_bsi_dly_type)CfgTxDly.DlyType);
           break;
       }
       case EFI_PM_TEST_BSI_FSM_RESET:
       {
           EFI_PM_TEST_BSI_FSM_RESET_PARAM fsmReset = DriverParam->u.BSIFuntionType.BsiFsmReset;
           errFlag = pm_bsi_fsm_reset(fsmReset.DeviceIndex,fsmReset.FSMReset, fsmReset.RxFIFOReset);
           break;
       }
       case EFI_PM_TEST_BSI_GET_RX_FIFO_WORD:
       {
           EFI_PM_TEST_BSI_GET_RX_FIFO_WORD_PARAM GetRxFifoWord = DriverParam->u.BSIFuntionType.BsiGetRxFifoWord;
           errFlag = pm_bsi_get_rx_fifo_word(GetRxFifoWord.DeviceIndex, (uint64 *)&GetRxFifoWord.Words);
           break;
       }

       default:
       break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;

    return errFlag;
}

pm_err_flag_type
TestLPGDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.LPGTypeFunction)
    {
       case EFI_PM_TEST_LPG_PATTERN_CONFIG:
       {
           EFI_PM_TEST_LPG_PATTERN_CONFIG_PARAM PatternConfig = DriverParam->u.LPGFunctionType.LpgPatternConfig;
           errFlag = pm_lpg_pattern_config(PatternConfig.PmicChip,(pm_lpg_chan_type)PatternConfig.ChanType, PatternConfig.RampDir,
                                           PatternConfig.PatternRepeat, PatternConfig.RampToggle,
                                           PatternConfig.EnPauseHigh, PatternConfig.EnPauseLow);
           break;
       }
       case EFI_PM_TEST_LPG_PWN_CLK_SELC:
       {
           EFI_PM_TEST_LPG_PWN_CLK_SELC_PARAM PwmClkSel = DriverParam->u.LPGFunctionType.LpgPwmClkSel;
           errFlag = pm_lpg_pwm_clock_sel(PwmClkSel.PmicChip, (pm_lpg_chan_type)PwmClkSel.ChanType, (pm_lpg_pwm_clock_type)PwmClkSel.ClkType);
           break;
       }
       case EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_ENABLE:
       {
           EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_ENABLE_PARAM PwmRampGeneratorEnablePar = DriverParam->u.LPGFunctionType.LpgPwmRampGeneratorEnableParam;
           errFlag = pm_lpg_pwm_ramp_generator_enable(PwmRampGeneratorEnablePar.PmicChip, (pm_lpg_chan_type)PwmRampGeneratorEnablePar.ChanType,
                                                      PwmRampGeneratorEnablePar.Enable);
           break;
       }
       case EFI_PM_TEST_LPG_PWM_LUT_INDEX_SET:
       {
           EFI_PM_TEST_LPG_PWM_LUT_INDEX_SET_PARAM PwmLutIndexSet = DriverParam->u.LPGFunctionType.LpgPwmLutIndexSet;
           errFlag = pm_lpg_pwm_lut_index_set(PwmLutIndexSet.PmicChip, (pm_lpg_chan_type)PwmLutIndexSet.ChanType,
                                              PwmLutIndexSet.LowIndex, PwmLutIndexSet.HighIndex);
           break;
       }
       case EFI_PM_TEST_LPG_SET_PWM_BIT_SIZE:
       {
           EFI_PM_TEST_LPG_SET_PWM_BIT_SIZE_PARAM SetPwmBitsize = DriverParam->u.LPGFunctionType.LpgSetPwmBitsize;
           errFlag = pm_lpg_set_pwm_bit_size(SetPwmBitsize.PmicChip, (pm_lpg_chan_type)SetPwmBitsize.ChanType,
                                             (pm_lpg_pwm_size_type)SetPwmBitsize.PwmSizeType);
           break;
       }
       case EFI_PM_TEST_LPG_PWM_SRC_SELECT:
       {
           EFI_PM_TEST_LPG_PWM_SRC_SELECT_PARAM PwmSrcSelect = DriverParam->u.LPGFunctionType.LpgPwmSrcSelect;
           errFlag = pm_lpg_pwm_src_select(PwmSrcSelect.PmicChip, (pm_lpg_chan_type)PwmSrcSelect.ChanType,
                                           (pm_lpg_src_type)PwmSrcSelect.SrcType);
           break;
       }
       case EFI_PM_TEST_LPG_LUT_CONFIG_SET_ARRAY:
       {
           EFI_PM_TEST_LPG_LUT_CONFIG_SET_ARRAY_PARAM LutConfigSetArray = DriverParam->u.LPGFunctionType.LpgLutConfigSetArray;
           errFlag = pm_lpg_lut_config_set_array(LutConfigSetArray.PmicChip, LutConfigSetArray.StartIndex,
                                                 LutConfigSetArray.Value, LutConfigSetArray.Count);
           break;
       }
       case EFI_PM_TEST_LPG_LUT_CONFIG_GET_ARRAY:
       {
           EFI_PM_TEST_LPG_LUT_CONFIG_GET_ARRAY_PARAM LutConfigGetArray = DriverParam->u.LPGFunctionType.LpgLutConfigGetArray;
           errFlag = pm_lpg_lut_config_get_array(LutConfigGetArray.PmicChip, LutConfigGetArray.StartIndex,
                                                 LutConfigGetArray.Value, LutConfigGetArray.Count);
           break;
       }
       case EFI_PM_TEST_LPG_LUT_CONFIG_SET:
       {
           EFI_PM_TEST_LPG_LUT_CONFIG_PARAM LutConfig = DriverParam->u.LPGFunctionType.LpgLutConfig;
           errFlag = pm_lpg_lut_config_set(LutConfig.PmicChip, LutConfig.LutIndex,LutConfig.PwmValue);
           break;
       }

       case EFI_PM_TEST_LPG_LUT_CONFIG_GET:
       {
           EFI_PM_TEST_LPG_LUT_CONFIG_PARAM LutConfig = DriverParam->u.LPGFunctionType.LpgLutConfig;
           errFlag = pm_lpg_lut_config_get(LutConfig.PmicChip, LutConfig.LutIndex,&LutConfig.PwmValue);
           break;
       }
       
	   case EFI_PM_TEST_LPG_CONFIG_PAUSE_TIME:
        {
            EFI_PM_TEST_LPG_CONFIG_PAUSE_TIME_PARAM ConfigPauseTime = DriverParam->u.LPGFunctionType.LpgConfigPauseTime;
            errFlag = pm_lpg_config_pause_time(ConfigPauseTime.PmicChip, (pm_lpg_chan_type)ConfigPauseTime.ChanType, 
                                               ConfigPauseTime.Ramp_Step_Duration, ConfigPauseTime.Hi_Multiplier,
                                               ConfigPauseTime.Low_Multiplier);
            break;
        }
		
	  case EFI_PM_TEST_LPG_PWM_OUTPUT_ENABLE:
        {
            EFI_PM_TEST_LPG_PWM_OUTPUT_ENABLE_PARAM PwmOutputEnable = DriverParam->u.LPGFunctionType.LpgPwmOutputEnable;
            errFlag = pm_lpg_pwm_output_enable(PwmOutputEnable.PmicChip, (pm_lpg_chan_type)PwmOutputEnable.ChanType, PwmOutputEnable.OutputEnable);                                 
            break;
        }
	     
	  case EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_START:
        {
            EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_START_PARAM PwmRampGeneratorStart = DriverParam->u.LPGFunctionType.LpgPwmRampGeneratorStart;
            errFlag = pm_lpg_pwm_ramp_generator_start(PwmRampGeneratorStart.PmicChip, (pm_lpg_chan_type)PwmRampGeneratorStart.ChanType);                                 
            break;
        }
		
       default:
       break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;

    return errFlag;
}

pm_err_flag_type
TestHAPTICSDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.HapticsTypeFunction)
    {
       case EFI_PM_TEST_HAPTICS_CONFIG_HAPTICS_RATE:
       {
           EFI_PM_TEST_HAPTICS_CONFIG_HAPTICS_RATE_PARAM configrate = DriverParam->u.HapticsFunctionType.Hapticsconfigrate;
           errFlag = pm_haptics_config_haptics_rate(configrate.DeviceIndex, configrate.config_rate);
           break;
       }
       case EFI_PM_TEST_HAPTICS_AUTO_RES_CONFIG:
       {
           EFI_PM_TEST_HAPTICS_AUTO_RES_CONFIG_PARAM autores = DriverParam->u.HapticsFunctionType.Hapticsautores;
           errFlag = pm_haptics_auto_res_cfg(autores.DeviceIndex, (pm_haptics_lra_res_calib_type)autores.CalibType , (pm_haptics_lra_high_z_type)autores.ZType,
		   (pm_haptics_lra_auto_res_mode_type)autores.ModeType);
           break;
       }

       default:
       break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;

    return errFlag;
}

pm_err_flag_type
TestWLEDDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.WLEDTypeFunction)
    {
       case EFI_PM_TEST_WLED_EN_AMOLED:
       {
           EFI_PM_TEST_WLED_EN_AMOLED_PARAM enamoled = DriverParam->u.WLEDFunctionType.Wledenamoled;
           errFlag = pm_wled_en_amoled(enamoled.DeviceIndex, (pm_wled_display_type)enamoled.display_type);
           break;
       }
       case EFI_PM_TEST_WLED_CONFIG_SINK_MODULATION:
       {
           EFI_PM_TEST_WLED_CONFIG_SINK_MODULATION_PARAM sinkmodulation = DriverParam->u.WLEDFunctionType.Wledsinkmodulation;
           errFlag = pm_wled_config_sink_modulation(sinkmodulation.DeviceIndex, (pm_wled_mode_clk_type)sinkmodulation.mode_clk,
		   sinkmodulation.en_phase_stagger,(pm_wled_mode_clk_type)sinkmodulation.accum_clk,sinkmodulation.en_hybrid_dim,
		   (pm_wled_pwm_mode_type)sinkmodulation.pwm_mode ,(pm_wled_mode_dim_mode_type)sinkmodulation.dim_mode );
           break;
       }
	   case EFI_PM_TEST_WLED_SET_CURRENT_SENSE_RATIO:
       {
           EFI_PM_TEST_WLED_SET_CURRENT_SENSE_RATIO_PARAM currentsenseratio = DriverParam->u.WLEDFunctionType.Wledcurrentsenseratio;
           errFlag = pm_wled_set_current_sense_ratio(currentsenseratio.DeviceIndex, (pm_wled_display_type)currentsenseratio.display_type ,currentsenseratio.resistance_kohm );
           break;
       }

       default:
       break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;

    return errFlag;
}


pm_err_flag_type
TestPONDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.PONTypeFunction)
    {
        case EFI_PM_TEST_PON_PS_HOLD_CFG:
        {
           EFI_PM_TEST_PON_PS_HOLD_CFG_PARAM PsHoldCfg = DriverParam->u.PONFunctionType.PonPsHoldCfg;
           errFlag = pm_pon_ps_hold_cfg(PsHoldCfg.DeviceIndex, (pm_pon_reset_cfg_type)PsHoldCfg.ResetCfgType);
           break;
        }
        case EFI_PM_TEST_PON_RESET_SOURCE_CFG:
        {
            EFI_PM_TEST_PON_RESET_SOURCE_CFG_PARAM ResetSourceCfg = DriverParam->u.PONFunctionType.PonResetSourceCfg;
            errFlag = pm_pon_reset_source_cfg(ResetSourceCfg.DeviceIndex, (pm_pon_reset_source_type)ResetSourceCfg.ResetSourceType,
                                         ResetSourceCfg.S1_timer, ResetSourceCfg.S2_timer,
                                         (pm_pon_reset_cfg_type)ResetSourceCfg.ResetCfgType);
            break;
        }
        case EFI_PM_TEST_PON_OVERTEMP_RESET_CFG:
        {
            EFI_PM_TEST_PON_OVERTEMP_RESET_CFG_PARAM OvertempResetCfg = DriverParam->u.PONFunctionType.PonOvertempResetCfg;
            /*below is no longer supported on GEN2*/
            (void)OvertempResetCfg;
            //errFlag = pm_pon_overtemp_reset_cfg(OvertempResetCfg.DeviceIndex, (pm_pon_reset_cfg_type)OvertempResetCfg.ResetCfgType);
            break;
        }
        case EFI_PM_TEST_PON_RESET_SOURCE_CTL:
        {
            EFI_PM_TEST_PON_RESET_SOURCE_CTL_PARAM ResetSourceCtl = DriverParam->u.PONFunctionType.PonResetSourceCtl;
            errFlag = pm_pon_reset_source_ctl(ResetSourceCtl.DeviceIndex,(pm_pon_reset_source_type)ResetSourceCtl.ResetSourceType,
                                                (pm_on_off_type)ResetSourceCtl.On_Off );
            break;
        }
        case EFI_PM_TEST_PON_STAGE3_RESET_SOURCE_CFG:
        {
            EFI_PM_TEST_PON_STAGE3_RESET_SOURCE_CFG_PARAM Stage3ResetSourceCfg = DriverParam->u.PONFunctionType.PonStage3ResetSourceCfg;
            errFlag = pm_pon_stage3_reset_source_cfg(Stage3ResetSourceCfg.DeviceIndex,(pm_pon_reset_source_type)Stage3ResetSourceCfg.ResetSourceType,
                                                Stage3ResetSourceCfg.S3_timer );
            break;
        }
        case EFI_PM_TEST_PON_GET_POFF_REASON:
        {
            EFI_PM_TEST_PON_GET_POFF_REASON_PARAM PonGetPoffReason = DriverParam->u.PONFunctionType.PonGetPoffReason;
            errFlag = pm_pon_get_poff_reason(PonGetPoffReason.DeviceIndex, (pm_pon_poff_reason_type*)PonGetPoffReason.Reason );
            break;
        }
        case EFI_PM_TEST_PON_GET_WARM_RESET:
        {
            EFI_PM_TEST_PON_GET_WARM_RESET_PARAM GetGetWarmResetReason = DriverParam->u.PONFunctionType.PonGetGetWarmResetReason;
            errFlag = pm_pon_get_warm_reset_reason(GetGetWarmResetReason.DeviceIndex, (pm_pon_warm_reset_reason_type*)GetGetWarmResetReason.Reason);
            break;
        }
        case EFI_PM_TEST_PON_GET_SOFT_RESET_REASON:
        {
            EFI_PM_TEST_PON_GET_SOFT_RESET_REASON_PARAM GetSoftResetReason = DriverParam->u.PONFunctionType.PonGetSoftResetReason;
            (void)GetSoftResetReason;
            //below call is no longer supported for GEN2
            //errFlag = pm_pon_get_soft_reset_reason(GetSoftResetReason.DeviceIndex, (pm_pon_soft_reset_reason_type*)GetSoftResetReason.Reason);
            break;
        }
        case EFI_PM_TEST_PON_GET_ALL_REASONS:
        {
            EFI_PM_TEST_PON_GET_ALL_REASONS_PARAM GetAllPonReasons = DriverParam->u.PONFunctionType.PonGetAllPonReasons;
            errFlag = pm_pon_get_all_pon_reasons(GetAllPonReasons.DeviceIndex, GetAllPonReasons.Reason);
            break;
        }
        case EFI_PM_TEST_PON_DEBOUNCE_CTL:
        {
            EFI_PM_TEST_PON_DEBOUNCE_CTL_PARAM debounceCtl = DriverParam->u.PONFunctionType.PondebounceCtl;
            errFlag = pm_pon_debounce_ctl(debounceCtl.DeviceIndex, debounceCtl.Time_delay);
            break;
        }
        case EFI_PM_TEST_PON_CONFIG_UVLO:
        {
            EFI_PM_TEST_PON_CONFIG_UVLO_PARAM ConfigUVLO = DriverParam->u.PONFunctionType.PonConfigUVLO;
            errFlag = pm_pon_config_uvlo(ConfigUVLO.DeviceIndex, ConfigUVLO.Uvlo_rising_thresh_voltage_in_mv, ConfigUVLO.Hyst_voltage_in_mv,ConfigUVLO.uvlo_enable);
            break;
        }

        case EFI_PM_TEST_PON_WARM_RESET_STATUS:
        {
            EFI_PM_TEST_PON_WARM_RESET_STATUS_PARAM WarmResetStatus = DriverParam->u.PONFunctionType.PonWarmResetStatus;
            errFlag = pm_pon_warm_reset_status(WarmResetStatus.DeviceIndex, WarmResetStatus.Status);
            break;
        }
        case EFI_PM_TEST_PON_WARM_RESET_STATUS_CLEAR:
        {
            EFI_PM_TEST_PON_WARM_RESET_STATUS_CLEAR_PARAM WarmResetStatusClear = DriverParam->u.PONFunctionType.PonWarmResetStatusClear;
            errFlag = pm_pon_warm_reset_status_clear(WarmResetStatusClear.DeviceIndex);
            break;
        }
        case EFI_PM_TEST_PON_TRIGGER_ENABLE_STATUS:
        {
            EFI_PM_TEST_PON_TRIGGER_ENABLE_STATUS_PARAM TriggerEnableStatus = DriverParam->u.PONFunctionType.PonTriggerEnableStatus;
            errFlag = pm_pon_trigger_enable_status(TriggerEnableStatus.DeviceIndex, (pm_pon_trigger_type)TriggerEnableStatus.TriggerType,
                                                   TriggerEnableStatus.Status);
            break;
        }
		case EFI_PM_TEST_PON_GET_FSM_CURRENT_STATE:
        {
            EFI_PM_TEST_PON_GET_FSM_CURRENT_STATE_PARAM GetFsmCurrentState = DriverParam->u.PONFunctionType.PonGetFsmCurrentState;
            errFlag = pm_pon_get_fsm_current_state(GetFsmCurrentState.DeviceIndex, (pm_pon_fsm_status_type*)GetFsmCurrentState.pon_fsm_status);
            break;
        }
		case EFI_PM_TEST_PON_GET_ON_REASON:
        {
            EFI_PM_TEST_PON_GET_ON_REASON_PARAM GetonReason = DriverParam->u.PONFunctionType.PonGetonReason;
            errFlag = pm_pon_get_on_reason(GetonReason.DeviceIndex, (pm_pon_on_state_reason_type*)GetonReason.Reason);
            break;
        }
		case EFI_PM_TEST_PON_GET_PON_REASON:
        {
            EFI_PM_TEST_PON_GET_PON_REASON_PARAM GetPonReason = DriverParam->u.PONFunctionType.PonGetPonReason;
            errFlag = pm_pon_get_pon_reason(GetPonReason.DeviceIndex, (pm_pon_reason_type*)GetPonReason.Reason);
            break;
        }
        case EFI_PM_TEST_PON_UVLO_RESET_STATUS:
        {
            //EFI_PM_TEST_PON_UVLO_RESET_STATUS_PARAM UvloResetStatus = DriverParam->u.PONFunctionType.PonUvloResetStatus;
            //errFlag = pm_pon_uvlo_reset_status(UvloResetStatus.DeviceIndex, UvloResetStatus.Status);
            break;
        }
		case EFI_PM_TEST_PON_GET_STAGE3_RESET_REASON:
        {
            EFI_PM_TEST_PON_GET_STAGE3_RESET_REASON_PARAM Stage3ResetReason = DriverParam->u.PONFunctionType.PonStage3ResetReason;
            errFlag = pm_pon_get_stage3_reset_reason(Stage3ResetReason.DeviceIndex,(pm_pon_stage3_reset_reason_type*)Stage3ResetReason.Reason);
            break;
        }
		
		case EFI_PM_TEST_PON_GET_FAULT_REASON:
        {
            EFI_PM_TEST_PON_GET_FAULT_REASON_PARAM GetFaultReason = DriverParam->u.PONFunctionType.PonGetFaultReason;
            errFlag = pm_pon_get_fault_reason(GetFaultReason.DeviceIndex,(pm_pon_fault_state_reason_type*)GetFaultReason.Reason);
            break;
        }
		
		case EFI_PM_TEST_PON_GET_OFF_REASON:
        {
            EFI_PM_TEST_PON_GET_OFF_REASON_PARAM GetOffReason = DriverParam->u.PONFunctionType.PonGetOffReason;
            errFlag = pm_pon_get_off_reason(GetOffReason.DeviceIndex,(pm_pon_off_state_reason_type*)GetOffReason.Reason);
            break;
        }
	
		case EFI_PM_TEST_PON_WDOG_CFG:
        {
            EFI_PM_TEST_PON_WDOG_CFG_PARAM WdogCfg = DriverParam->u.PONFunctionType.PonWdogCfg;
            errFlag = pm_pon_wdog_cfg(WdogCfg.DeviceIndex,WdogCfg.S1_timer, WdogCfg.S2_timer,(pm_pon_reset_cfg_type)WdogCfg.ResetCfgType);
            break;
        }

		case EFI_PM_TEST_PON_WDOG_ENABLE:
        {
            EFI_PM_TEST_PON_WDOG_ENABLE_PARAM WdogEnable = DriverParam->u.PONFunctionType.PonWdogEnable;
            errFlag = pm_pon_wdog_enable(WdogEnable.DeviceIndex,(pm_on_off_type)WdogEnable.enable);
            break;
        }
		
	/*	case EFI_PM_TEST_PON_WDOG_STATUS_GET:
        {
            EFI_PM_TEST_PON_WDOG_STATUS_GET_PARAM WdogGetStatus = DriverParam->u.PONFunctionType.PonWdogGetStatus;
            errFlag = pm_wdog_status_get(WdogGetStatus.DeviceIndex,WdogGetStatus.dog_status);
            break;
        } */
		case EFI_PM_TEST_PON_WDOG_PET:
        {
            EFI_PM_TEST_PON_WDOG_PET_PARAM WdogPet = DriverParam->u.PONFunctionType.PonWdogPet;
            errFlag = pm_pon_wdog_pet(WdogPet.DeviceIndex);
            break;
        }
	
		case EFI_PM_APP_LONG_PWR_KEY_HOLD_CHECK:
        {
            EFI_PM_APP_LONG_PWR_KEY_HOLD_CHECK_PARAM PwrKeyHoldCheck = DriverParam->u.PONFunctionType.PonPwrKeyHoldCheck;
            (void)PwrKeyHoldCheck;
            //errFlag = pm_app_long_pwr_key_hold_check(PwrKeyHoldCheck.is_key_hold_valid );
               errFlag = pm_app_pwrkey_long_press_check(PM_PON_PWRKEY_DBNC_CHK_AT_CORE);
            break;
        }
		
        default:
            break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;

    return errFlag;
}

pm_err_flag_type
TestCOINCELLDriver
(
    EFI_PM_TEST_DRIVER_TYPE_FUNCTION DriverTypeFunction,
    EFI_PM_TEST_DRIVER_PARAM *DriverParam,
    EFI_PM_TEST_DRIVER_OUT   *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverTypeFunction.CoincellTypeFunction)
    {
       case EFI_PM_TEST_COINCELL_ENABLE:
       {
           EFI_PM_TEST_COINCELL_ENABLE_PARAM Enable = DriverParam->u.CoincellFunctionType.CoincellEnable;
           errFlag = pm_coincell_enable(Enable.DeviceIndex, (pm_on_off_type)Enable.enable);
           break;
       }
       case EFI_PM_TEST_COINCELL_GET_STATUS:
       {
           EFI_PM_TEST_COINCELL_GET_STATUS_PARAM Status = DriverParam->u.CoincellFunctionType.CoincellStatus;
           errFlag = pm_coincell_get_status(Status.DeviceIndex,(BOOLEAN*)Status.status);
           break;
       }
	   case EFI_PM_TEST_COINCELL_SET_CHARGE_CURRENT_LIMIT_RESISTOR:
       {
           EFI_PM_TEST_COINCELL_SET_CHARGE_CURRENT_LIMIT_RESISTOR_PARAM RsetValue = DriverParam->u.CoincellFunctionType.CoincellRsetValue;
           errFlag = pm_coincell_set_charge_current_limit_resistor(RsetValue.DeviceIndex,RsetValue.COIN_CHG_RSET_VALUE);
           break;
       }
	   
	   case EFI_PM_TEST_COINCELL_SET_CHARGING_VOLTAGE:
       {
           EFI_PM_TEST_COINCELL_SET_CHARGING_VOLTAGE_PARAM VsetValue = DriverParam->u.CoincellFunctionType.CoincellVsetValue;
           errFlag = pm_coincell_set_charging_voltage(VsetValue.DeviceIndex,VsetValue.COIN_CHG_VSET_VALUE);
           break;
       }

       default:
       break;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
    }
    else
    {
        DriverOut->Status = EFI_PM_TEST_DRIVER_SUCCESS;
    }

    DriverOut->ErrorType = errFlag;

    return errFlag;
}



/**
  EFI_TestPMICTestDriver()

  @brief
  ConfigDigitalInput implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_TestPMICTestDriver
(
  IN EFI_PM_TEST_DRIVER_TYPE                  DriverType,
  IN EFI_PM_TEST_DRIVER_TYPE_FUNCTION         DriverTypeFunction,
  IN EFI_PM_TEST_DRIVER_PARAM                 *DriverParam,
  OUT EFI_PM_TEST_DRIVER_OUT                  *DriverOut
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if(NULL == DriverParam || NULL == DriverOut)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    switch(DriverType)
    {
        case EFI_PM_GPIO:
        {
            errFlag = TestGPIODriver(DriverTypeFunction,DriverParam,DriverOut);
            break;
        }
        case EFI_PM_BSI:
        {
            errFlag = TestBSIDriver(DriverTypeFunction,DriverParam,DriverOut);
            break;
        }
       case EFI_PM_LPG:
       {
           errFlag = TestLPGDriver(DriverTypeFunction,DriverParam,DriverOut);
           break;
       }
       case EFI_PM_AC:
        {
            errFlag = TestAccessControl(DriverTypeFunction,DriverParam,DriverOut);
            break;
        }
	   case EFI_PM_HAPTICS:
       {
           errFlag = TestHAPTICSDriver(DriverTypeFunction,DriverParam,DriverOut);
           break;
       }
	   case EFI_PM_WLED:
       {
           errFlag = TestWLEDDriver(DriverTypeFunction,DriverParam,DriverOut);
           break;
       }
	   case EFI_PM_PON:
       {
           errFlag = TestPONDriver(DriverTypeFunction,DriverParam,DriverOut);
           break;
       }
	   case EFI_PM_COINCELL:
       {
           errFlag = TestCOINCELLDriver(DriverTypeFunction,DriverParam,DriverOut);
           break;
       }
       case EFI_PM_BOB:
       {
           errFlag = TestBOBDriver(DriverTypeFunction,DriverParam,DriverOut);
           break;
       }
        default:
        {
            DriverOut->Status = EFI_PM_TEST_DRIVER_FAILURE;
            DriverOut->ErrorType = PM_ERR_FLAG_INVALID_PERIPHERAL;
            return EFI_UNSUPPORTED;
        }
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        PMIC_UART_DEBUG((EFI_D_ERROR, "PmicDxe: EFI_TestPMICTestDriver : Error : %d \r\n", errFlag));
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}


/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
  PMIC GPIO UEFI Protocol implementation
 */
EFI_QCOM_PMIC_TEST_PROTOCOL PmicTestProtocolImplementation = 
{
    PMIC_TEST_PROTOCOL_REVISION,
    EFI_TestPMICTestDriver
};

