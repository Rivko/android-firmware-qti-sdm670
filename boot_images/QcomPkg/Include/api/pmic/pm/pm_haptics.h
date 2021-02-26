#ifndef __PM_HAPTICS_H__
#define __PM_HAPTICS_H__

/*! \file  pm_haptics.h 
*  \n
*  \brief   PMIC-HAPTICS MODULE RELATED DECLARATION
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting HAPTICS motor driver for the Qualcomm PMIC 
*  chip set.
*
*
*  \n &copy; Copyright 2014-2017 QUALCOMM Technologies Inc, All Rights Reserved
*/

/* =======================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/17   czq      Supported for WF source selection.
03/20/17   czq      1. Added get haptics ststus1 API
                    2. Added get play rate configuration and LRA auto resonance API.
09/29/14   akm      Created this file to add haptics driver for 8994
========================================================================== */

/*===========================================================================
                INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"




/*=============================================================================
                    LOCAL VARIABLE DEFINITIONS
=============================================================================*/

typedef enum 
{
  PM_HAPTICS_PLAY,
  PM_HAPTICS_PAUSE,
  PM_HAPTICS_INVALID
}pm_haptics_vibrate_type;


typedef enum 
{
  PM_HAPTICS_WF_SOURCE_VMAX,
  PM_HAPTICS_WF_SOURCE_BUFFER,
  PM_HAPTICS_WF_SOURCE_AUDIO,
  PM_HAPTICS_WF_SOURCE_EXTPWM,
  PM_HAPTICS_WF_SOURCE_INVALID
}pm_haptics_wf_source_type;


typedef enum 
{
  PM_HAPTICS_WF_TRIG_HAPTICS_PLAY,
  PM_HAPTICS_WF_TRIG_LINE_IN,
  PM_HAPTICS_WF_TRIG_INVALID
}pm_haptics_wf_trigger_type;


typedef enum 
{
  PM_HAPTICS_ACTUATOR_LRA,
  PM_HAPTICS_ACTUATOR_ERM,
  PM_HAPTICS_ACTUATOR_INVALID
}pm_haptics_actuator_type;

typedef enum 
{
  PM_HAPTICS_LRA_RES_SIN,
  PM_HAPTICS_LRA_RES_SQUARE,
  PM_HAPTICS_LRA_RES_INVALID
}pm_haptics_lra_res_type;

typedef enum 
{
  PM_HAPTICS_SLEW_6NS,
  PM_HAPTICS_SLEW_16NS,
  PM_HAPTICS_SLEW_INVALID
}pm_haptics_slew_rate;


typedef enum 
{
  PM_HAPTICS_PWM_CAP_26PF,
  PM_HAPTICS_PWM_CAP_13PF,
  PM_HAPTICS_PWM_CAP_8P7PF,
  PM_HAPTICS_PWM_CAP_6P5PF,
  PM_HAPTICS_PWM_CAP_INVALID
}pm_haptics_pwm_cap_sel_type;


typedef enum 
{
  PM_HAPTICS_WF_REPEAT_1_TIMES,
  PM_HAPTICS_WF_REPEAT_2_TIMES,
  PM_HAPTICS_WF_REPEAT_4_TIMES,
  PM_HAPTICS_WF_REPEAT_8_TIMES,
  PM_HAPTICS_WF_REPEAT_16_TIMES,
  PM_HAPTICS_WF_REPEAT_32_TIMES,
  PM_HAPTICS_WF_REPEAT_64_TIMES,
  PM_HAPTICS_WF_REPEAT_128_TIMES,
  PM_HAPTICS_WF_REPEAT_INVALID
}pm_haptics_waveform_repeat_type;

typedef enum 
{
  PM_HAPTICS_WF_S_REPEAT_1_TIMES,
  PM_HAPTICS_WF_S_REPEAT_2_TIMES,
  PM_HAPTICS_WF_S_REPEAT_4_TIMES,
  PM_HAPTICS_WF_S_REPEAT_8_TIMES,
  PM_HAPTICS_WF_S_REPEAT_INVALID
}pm_haptics_waveform_s_repeat_type;


typedef enum
{
  PM_HAPTICS_LRA_AUTO_RES_MODE_NO_AUTO_RES, /*no auto resonance*/
  PM_HAPTICS_LRA_AUTO_RES_MODE_ZXD,         /*Zero crossing based discontinuous method (ZXD)*/
  PM_HAPTICS_LRA_AUTO_RES_MODE_QWD,         /*Quarter wave drive method (QWD)*/
  PM_HAPTICS_LRA_AUTO_RES_MODE_MAX_QWD,     /*MAX QWD*/
  PM_HAPTICS_LRA_AUTO_RES_MODE_ZXD_EOP,     /*Zero crossing based discontinuous method with high z after end of pattern*/
  PM_HAPTICS_LRA_AUTO_RES_MODE_INVALID,     /*Invalid*/
}pm_haptics_lra_auto_res_mode_type; 

typedef enum
{
  PM_HAPTICS_LRA_HIGH_Z_NO_HIGHZ,   /*no high-z*/
  PM_HAPTICS_LRA_HIGH_Z_HIGHZ_OPT1, /*2 LRA period (ZXD), 1/8 LRA period (QWD)*/
  PM_HAPTICS_LRA_HIGH_Z_HIGHZ_OPT2, /*3 LRA period (ZXD), 1/4 LRA period (QWD)*/
  PM_HAPTICS_LRA_HIGH_Z_HIGHZ_OPT3, /*4 LRA period (ZXD), 1/2 LRA period (QWD)*/
  PM_HAPTICS_LRA_HIGH_Z_INVALID,    /*Invalid*/
}pm_haptics_lra_high_z_type; 

typedef enum
{   
  PM_HAPTICS_RATE_PLAY_CONF,          /*haptics play rate by configuration*/
  PM_HAPTICS_RATE_LRA_AUTO_RES,  /*LRA auto resonance calibration rate*/
  PM_HAPTICS_RATE_INVALID,            /*Invalid*/
}pm_haptics_rate_type;  

typedef enum
{
  PM_HAPTICS_LRA_RES_CALIB_FOUR_PERIODS,      /*AFTER EVERY 4 LRA PERIOD*/
  PM_HAPTICS_LRA_RES_CALIB_EIGHT_PERIODS,     /*AFTER EVERY 8 LRA PERIOD*/
  PM_HAPTICS_LRA_RES_CALIB_SIXTEEN_PERIODS,   /*AFTER EVERY 16 LRA PERIOD*/
  PM_HAPTICS_LRA_RES_CALIB_THIRTYTWO_PERIODS, /*AFTER EVERY 32 LRA PERIOD*/
  PM_HAPTICS_LRA_RES_CALIB_INVALID,           /*INVALID*/
}pm_haptics_lra_res_calib_type;

typedef enum
{
  PM_HAPTICS_SIGN_FORWARD,
  PM_HAPTICS_SIGN_REVERSE,
  PM_HAPTICS_SIGN_INVALID,
}pm_haptics_wf_s_sign_type;

typedef enum
{
  PM_HAPTICS_OVD_1X,
  PM_HAPTICS_OVD_2X,
  PM_HAPTICS_OVD_INVALID,
}pm_haptics_wf_s_ovd_type;

typedef struct
{
  boolean play_sts;
  boolean busy;
  boolean oc_flag;
  boolean sc_flag;
  boolean auto_res_err;
  boolean en_sts;
}pm_haptics_status; 

typedef struct
{
  pm_haptics_wf_s_sign_type sign;
  pm_haptics_wf_s_ovd_type  overdrive;
  uint32                    amp_mv;        /*range: from 0 to 3596 mV*/
}pm_haptics_sample;  

/*===========================================================================

                 HAPTICS DRIVER FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_haptics_enable

DESCRIPTION
  This function enables haptics .

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: mode
      - Mode to enable/disable.
   3) Name: enable
      - pm_on_off_type to enable/disable the given mode.
      - Valid Inputs PM_ON/PM_OFF

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/
pm_err_flag_type pm_haptics_enable ( uint32  pmic_chip,  pm_on_off_type enable);


/*===========================================================================

FUNCTION pm_haptics_set_actuator_type

DESCRIPTION
  This function enables haptics .

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: actuator_type
      - Valid Inputs   PM_HAPTICS_ACTUATOR_LRA/PM_HAPTICS_ACTUATOR_ERM,

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

pm_err_flag_type pm_haptics_set_actuator_type( uint32 pmic_chip, pm_haptics_actuator_type actuator_type);


/*===========================================================================

FUNCTION pm_haptics_set_lra_resonance_type

DESCRIPTION
  This function enables haptics .

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: lra_res
      - Valid Inputs     PM_HAPTICS_LRA_RES_SIN/PM_HAPTICS_LRA_RES_SQUARE

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

pm_err_flag_type pm_haptics_set_lra_resonance_type( uint32 pmic_chip, pm_haptics_lra_res_type lra_res);


/*===========================================================================

FUNCTION pm_haptics_set_lra_resonance_type

DESCRIPTION
  This function enables haptics .

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: slew_rate
      - Valid Inputs       PM_HAPTICS_SLEW_2NS/PM_HAPTICS_SLEW_10NS

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

pm_err_flag_type pm_haptics_set_slew_rate( uint32 pmic_chip, pm_haptics_slew_rate slew_rate);



/*===========================================================================

FUNCTION pm_haptics_play

DESCRIPTION
  This function enables haptics .

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: vibrate
           PM_HAPTICS_PLAY,
           PM_HAPTICS_PAUSE,
           PM_HAPTICS_INVALID

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/


pm_err_flag_type pm_haptics_play(  uint32 pmic_chip,  pm_haptics_vibrate_type vibrate);

/*===========================================================================

FUNCTION pm_haptics_set_vmax_volt

DESCRIPTION
  This function enables haptics .

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: vmax_cfg_volt
      - Vibrate config volatge.

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL    = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_BUS_ERR               = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/



pm_err_flag_type pm_haptics_set_vmax_volt( uint32 pmic_chip, uint32 vmax_cfg_volt);

/*===========================================================================

FUNCTION pm_haptics_internal_pwm_freq_sel

DESCRIPTION
  This function enables haptics various modes.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: freq_sel_data_in_khz
        - Ranges from 25 - 100kHz           
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL    = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_BUS_ERR               = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/
pm_err_flag_type pm_haptics_internal_pwm_freq_sel (uint32 pmic_chip, uint32 freq_sel_data_in_khz);

/*===========================================================================

FUNCTION pm_haptics_external_pwm_freq_sel

DESCRIPTION
  This function enables haptics various modes.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: freq_sel_data_in_khz
    - Ranges from 25 - 100kHz           
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED   = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL      = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_BUS_ERR                 = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER      = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

pm_err_flag_type pm_haptics_external_pwm_freq_sel (uint32 pmic_chip,uint32 freq_sel_data_in_khz);
/*===========================================================================

FUNCTION pm_haptics_pwm_cap_sel

DESCRIPTION
  This function enables haptics various modes.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: cap_sel_data
        PM_HAPTICS_PWM_CAP_26PF,
        PM_HAPTICS_PWM_CAP_13PF,
        PM_HAPTICS_PWM_CAP_8P7PF,
        PM_HAPTICS_PWM_CAP_6P5PF
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED  = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL     = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_BUS_ERR                = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER      = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

pm_err_flag_type pm_haptics_pwm_cap_sel (uint32 pmic_chip, pm_haptics_pwm_cap_sel_type cap_sel_data);

/*===========================================================================

FUNCTION pm_haptics_waveform_repeat

DESCRIPTION
  This function enables haptics various modes of waveforms(WF).

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: haptics
      - Selects which haptics peripheral.
   3) Name: wf_repeat_data BITS [6:4]
        PM_HAPTICS_WF_REPEAT_1_TIMES,
        PM_HAPTICS_WF_REPEAT_2_TIMES,
        PM_HAPTICS_WF_REPEAT_4_TIMES,
        PM_HAPTICS_WF_REPEAT_8_TIMES,
        PM_HAPTICS_WF_REPEAT_16_TIMES,
        PM_HAPTICS_WF_REPEAT_32_TIMES,
        PM_HAPTICS_WF_REPEAT_64_TIMES,
        PM_HAPTICS_WF_REPEAT_128_TIMES,
   4)Name: wf_s_repeat_data BITS [6:4]
        PM_HAPTICS_WF_S_REPEAT_1_TIMES,
        PM_HAPTICS_WF_S_REPEAT_2_TIMES,
        PM_HAPTICS_WF_S_REPEAT_4_TIMES,
        PM_HAPTICS_WF_S_REPEAT_8_TIMES,
            
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL   = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_BUS_ERR              = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

pm_err_flag_type pm_haptics_wf_repeat  (uint32 pmic_chip, pm_haptics_waveform_repeat_type wf_repeat_data,pm_haptics_waveform_s_repeat_type wf_s_repeat_data);


/*===========================================================================

FUNCTION pm_haptics_haptics_rate_cfg1

DESCRIPTION
  This function enables haptics various modes.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: config_rate
        - Ranges from 0 - 20475us                   
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()
*/
pm_err_flag_type pm_haptics_config_haptics_rate(uint32 pmic_chip,uint32 config_rate_in_us);


/*===========================================================================

FUNCTION pm_haptics_auto_res_cfg

DESCRIPTION
  This function configures auto resonance settings

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: lra_res_calib
        - Auto resoance technique for LRA. Refer #pm_haptics_lra_res_calib_type     
   3) Name: lra_high_z
        - Duration of high-Z time based on LRA resoanace technique. Refer #pm_haptics_lra_high_z_type 
   4) Name: auto_res_mode
        - Frequency of LRA resonanace calibration for ZXD. Refer #pm_haptics_lra_auto_res_mode_type              
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()
*/
pm_err_flag_type 
pm_haptics_auto_res_cfg(uint32  pmic_chip, pm_haptics_lra_res_calib_type lra_res_calib, pm_haptics_lra_high_z_type lra_high_z, pm_haptics_lra_auto_res_mode_type auto_res_mode);


/*===========================================================================

FUNCTION pm_haptics_get_status

DESCRIPTION
  This function is use to get haptics status.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: *haptics_status
        - point address to store status                  
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL      = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()
*/
pm_err_flag_type pm_haptics_get_status(uint32 pmic_chip, pm_haptics_status *haptics_status);


/*===========================================================================

FUNCTION pm_haptics_get_rate

DESCRIPTION
  This function is used to get haptics play rate and lra auto resonance rate.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: rate_type
        - PM_HAPTICS_RATE_PLAY_CONF
        - PM_HAPTICS_RATE_LRA_AUTO_RES		
   2) Name: rate
        - Ranges from 0 - 20475us                   
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()
*/
pm_err_flag_type pm_haptics_get_rate(uint32 pmic_chip, pm_haptics_rate_type rate_type, uint32 *rate);


/*===========================================================================

FUNCTION pm_haptics_wf_sel

DESCRIPTION
  This function is used to select WF source.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: wf_source_type
        - PM_HAPTICS_WF_SOURCE_VMAX
        - PM_HAPTICS_WF_SOURCE_BUFFER
        - PM_HAPTICS_WF_SOURCE_AUDIO
        - PM_HAPTICS_WF_SOURCE_EXTPWM	
   2) Name: wf_trigger_type
        - PM_HAPTICS_WF_TRIG_HAPTICS_PLAY,
        - PM_HAPTICS_WF_TRIG_LINE_IN,                  
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()
*/
pm_err_flag_type pm_haptics_wf_sel( uint32 pmic_chip, pm_haptics_wf_source_type wf_source_type, pm_haptics_wf_trigger_type wf_trigger_type);


/*===========================================================================

FUNCTION pm_haptics_fill_wf_buff

DESCRIPTION
  This function is used to fill WF buffer.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: wf_arry
        - sign [7]
        - overdrive [6]
        - amp(mV) [5:1]
   3) Name: length
        - the actual waveform arry size used (range: from 0 to 8).   
        
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL       = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()
*/
pm_err_flag_type pm_haptics_fill_wf_buff( uint32 pmic_chip, pm_haptics_sample *wf_arry, size_t length);

#endif

