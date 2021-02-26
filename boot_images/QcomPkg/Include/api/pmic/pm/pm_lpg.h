#ifndef __PM_LPG_H__
#define __PM_LPG_H__
/*! \file pm_lpg.h
 *  \n
 *  \brief  PMIC LPG driver APIs
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC LPG (Light Pulse Generator) module.
 *
 *
 *  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/14   al      Added API to set LPG DTEST 
06/17/14   al      Arch update
05/12/13   al      Modified set interval counter api to configure pause time
01/14/13   al      Adding get_status
10/23/12   al	   Updating copyright info and adding additional lut apis
10/16/12   al	   added LUT support
08/21/12   al	   Added enums required for 8974 LPG
07/27/12   al	   Renamed all Enums,added device index and resource index
05/24/11   dy      Port to UEFI
=============================================================================*/

#include "pm_err_flags.h"
#include "com_dtypes.h"
/*===========================================================================

                     GLOBAL TYPE DEFINITIONS

===========================================================================*/
#define PM_LPG_PWM_PRE_DIVIDE_EXP_MAX   0x8
#define PM_LPG_INDEX_MAX                0x3F
#define PM_LPG_PAUSE_MAX                0x38
#define PM_LPG_INTERVAL_MAX             0xF
#define PM_LPG_BANK_MAX                 8


/*! \enum pm_pwm_src_select lpg_type PWM_SRC_SELECT.
 *  \brief LPG source - LUT or Register
 */
typedef enum{
    PM_LPG_EN_PWM_HI =0,
    PM_LPG_EN_PWM_LO ,
    PM_LPG_EN_PWM_INVALID 
}pm_lpg_pwm_output_type;

/*! \enum pm_pwm_src_select lpg_type PWM_SRC_SELECT.
 *  \brief LPG source - LUT or Register
 */
typedef enum{
    PM_LPG_PWM_SRC_LUT =0,
    PM_LPG_PWM_SRC_PWM_REGISTER ,
    PM_LPG_PWM_SRC_INVALID 
}pm_lpg_src_type;

/*! \enum pm_lpg_chan_type.
 *  \brief LPG channel 1 to 7
 */
typedef enum{
    PM_LPG_CHAN_NONE,
    PM_LPG_CHAN_1 ,
    PM_LPG_CHAN_2 ,
    PM_LPG_CHAN_3 ,
    PM_LPG_CHAN_4 ,
    PM_LPG_CHAN_5 ,
    PM_LPG_CHAN_6 ,
    PM_LPG_CHAN_7 ,
    PM_LPG_CHAN_8, 
    PM_LPG_CHAN_INVALID 
}pm_lpg_chan_type;


/*! \enum pm_lpg_pwm_size_type
 *  \brief LPG Bit Size (6 or 9 bit for generic LPG), 7 bit PWM (8 bit piecewise mapping),8 bit PWM (keypad BL only)
 */
typedef enum{
    PM_LPG_PWM_6BIT,
    PM_LPG_PWM_7BIT,
    PM_LPG_PWM_8BIT,
    PM_LPG_PWM_9BIT,
    PM_LPG_PWM_SIZE_MAX
} pm_lpg_pwm_size_type;


/*! \enum pm_lpg_pwm_clock_type
 *  \brief LPG PWM Clock Select
 */
typedef enum{
    PM_LPG_PWM_OFF,
    PM_LPG_PWM_1_0_KHZ,
    PM_LPG_PWM_32_0_KHZ,
    PM_LPG_PWM_19_2_MHZ,
    PM_LPG_PWM_CLOCK_MAX
} pm_lpg_pwm_clock_type;


/*! \enum pm_lpg_pwm_pre_divide_type
 *  \brief PWM Pre-divider value
 */
typedef enum{
    PM_LPG_PWM_PRE_DIV_1=0,/**< Value 1. */
    PM_LPG_PWM_PRE_DIV_3=1,/**< Value 3. */
    PM_LPG_PWM_PRE_DIV_5=2,/**< Value 5. */
    PM_LPG_PWM_PRE_DIV_6=3,/**< Value 6. */
    PM_LPG_PWM_PRE_DIV_MAX
} pm_lpg_pwm_pre_divide_type;


/*! \enum pm_lpg_pwm_freq_expo_type
 *  \brief PWM freq expo value
 */
typedef enum{
    PM_LPG_PWM_FREQ_EXPO_0,
    PM_LPG_PWM_FREQ_EXPO_1,
    PM_LPG_PWM_FREQ_EXPO_2,
    PM_LPG_PWM_FREQ_EXPO_3,
    PM_LPG_PWM_FREQ_EXPO_4,
    PM_LPG_PWM_FREQ_EXPO_5,
    PM_LPG_PWM_FREQ_EXPO_6,
    PM_LPG_PWM_FREQ_EXPO_7,
    PM_LPG_PWM_FREQ_EXPO_MAX
} pm_lpg_pwm_freq_expo_type;


/*! \enum pm_lpg_phase_stag_shift_type
 *  \brief LPG phase shift from previous slice
 */
typedef enum {
    /*0 degree shift from previous slice*/
    PM_LPG_PHASE_STAG_SHIFT_0_DEG,
    /*90 degree shift from previous slice*/
    PM_LPG_PHASE_STAG_SHIFT_90_DEG,
    /*180 degree shift from previous slice*/
    PM_LPG_PHASE_STAG_SHIFT_180_DEG,
    /*270 degree shift from previous slice*/
    PM_LPG_PHASE_STAG_SHIFT_270_DEG,
    /*invalid shift from previous slice*/
    PM_LPG_PHASE_STAG_SHIFT_INVALID
}pm_lpg_phase_stag_shift_type;


typedef enum
{
    PM_LPG_INT_RT_STS =0,
    PM_LPG_INT_SET_TYPE,
    PM_LPG_INT_POLARITY_HIGH,
    PM_LPG_INT_POLARITY_LOW,
    PM_LPG_INT_LATCHED_CLR,
    PM_LPG_INT_EN_SET,
    PM_LPG_INT_EN_CLR,
    PM_LPG_INT_LATCHED_STS,
    PM_LPG_INT_PENDING_STS,
    PM_LPG_INT_MID_SEL,
    PM_LPG_INT_PRIORITY,
    PM_LPG_IRQ_INVALID	  
}pm_lpg_irq_type;

/*! \struct pm_lpg_status_type
 *  \brief LPG status
 */
typedef struct{
   boolean pwm_output;
   /* LPG PWM output enabled or disabled */ 
   uint16 pwm_value;
   /* LPG PWM value for duty cycle */
   pm_lpg_pwm_pre_divide_type pre_div;
   /* LPG PWM frequency pre-divider */
   pm_lpg_pwm_freq_expo_type exp;
   /* LPG PWM frequency divider exponent */
   pm_lpg_pwm_clock_type pwm_freq_clk;
   /* LPG PWM clock frequency */
   pm_lpg_pwm_size_type  pwm_bit_size;
   /* LPG PWM bit size */
   pm_lpg_src_type lpg_src_type;
   /* LPG source */
   boolean glitch_removal;
   /* Glitch removal enable/disable*/
   boolean full_scale;
   /* LPG full scale enabled or disabled */
   boolean en_phase_stagger;
   /* LPG phase stagger enabled or disabled */
   pm_lpg_phase_stag_shift_type phase_stagger;
   /* LPG phase shift */
   boolean ramp_direction;
   /* LPG ramp direction HI or LO */
   boolean pattern_repeat;
   /* LPG pattern repeat enabled or disabled */
   boolean ramp_toggle;
   /* LPG ramp toggle enabled or disabled */
   boolean en_pause_hi;
   /* LPG pause high enabled or disabled */
   boolean en_pause_lo;
   /* LPG pause low enabled or disabled */
   uint32 lut_low_index; 
   /* LPG LUT low index value */
   uint32 lut_high_index;
   /* LPG LUT high index value */
  uint32 ramp_step_duration;
  /* LPG interval count value*/
   uint32 hi_multiplier;
   /* Pause Hi multiuplier */
   uint32 low_multiplier;
   /* Pause low multiuplier */
}pm_lpg_status_type;


/*! \struct pm_lpg_chan_dtest_type
 *  \brief LPG DTEST
 */
typedef enum 
{
    PM_LPG_CHAN_DTEST_1,
    PM_LPG_CHAN_DTEST_2,
    PM_LPG_CHAN_DTEST_3,
    PM_LPG_CHAN_DTEST_4,
    PM_LPG_CHAN_DTEST_INVALID
} pm_lpg_chan_dtest_type;

/*! \struct pm_lpg_chan_lpg_out_type
 *  \brief LPG out of each LPG channel
 */
typedef enum 
{
    PM_LPG_CHAN_LPG_OUT_DISABLED,
    PM_LPG_CHAN_LPG_OUT_1,
    PM_LPG_CHAN_LPG_OUT_2,
    PM_LPG_CHAN_LPG_OUT_3,
    PM_LPG_CHAN_LPG_OUT_4,
    PM_LPG_CHAN_LPG_OUT_INVALID
} pm_lpg_chan_lpg_out_type;


/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION  pm_lpg_pwm_enable                            EXTERNAL FUNCTION

DESCRIPTION
   This function enables/disables LPG PWM module

PARAMETERS
   1) Name: enable
      - Enable/Disable PWM
      Type: boolean
      - Valid inputs:
           TRUE/FALSE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_pwm_enable(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, boolean enable );


/*===========================================================================
FUNCTION  pm_lpg_pwm_output_enable                     EXTERNAL FUNCTION

DESCRIPTION
   This function enables/disables LPG PWM output

PARAMETERS
   1) Name: enable
      - Enable/Disable PWM Output
      Type: boolean
      - Valid inputs:
           TRUE/FALSE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_pwm_output_enable( uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan,boolean enable );


/*===========================================================================
FUNCTION  pm_lpg_pwm_set_pwm_value                      EXTERNAL FUNCTION

DESCRIPTION
   This function sets the PWM value used to calculate duty cycle

PARAMETERS
   1) Name: pwm_value
      - PWM value used to calculate duty cycle
      Type: uint16

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_pwm_set_pwm_value( uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan,uint16 pwm_value );


/*===========================================================================
FUNCTION  pm_lpg_pwm_set_pre_divide                     EXTERNAL FUNCTION

DESCRIPTION
   This function sets the PWM pre-divider and exponent used to calculate duty cycle

PARAMETERS
   1) Name: pre_div
      - pre-divider value
      Type: pm_lpg_pwm_pre_divide_type
      - Valid Inputs:
          PM_LPG_PWM_PRE_DIV_2
          PM_LPG_PWM_PRE_DIV_3
          PM_LPG_PWM_PRE_DIV_5
          PM_LPG_PWM_PRE_DIV_6
        
   2) Name: exp
      - pre-divider exponent
      Type: uint8

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG_INVALID_PARAMETER     -> pre-divider value out of range.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_pwm_set_pre_divide(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, pm_lpg_pwm_pre_divide_type pre_div, pm_lpg_pwm_freq_expo_type exp );


/*===========================================================================
FUNCTION  pm_lpg_pwm_clock_sel                         EXTERNAL FUNCTION

DESCRIPTION
   This function sets PWM clock
PARAMETERS
   1) Name: clock
      Type: pm_lpg_pwm_clock_type
      - Valid Inputs:
          PM_LPG_PWM_OFF      - Clock is OFF
          PM_LPG_PWM_1_0_KHZ  - 1 kHz 
          PM_LPG_PWM_32_0_KHZ - 32 kHz
          PM_LPG_PWM_19_2_MHZ - 19.2 MHz
        
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.
     - PM_ERR_FLAG_INVALID_PARAMETER     -> clock value out of range.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_pwm_clock_sel( uint32 pmic_device_index,pm_lpg_chan_type pm_lpg_chan, pm_lpg_pwm_clock_type clock );


/*===========================================================================
FUNCTION  pm_lpg_set_pwm_bit_size                      EXTERNAL FUNCTION

DESCRIPTION
   This function sets PWM size (6 or 9 bit)
PARAMETERS
   1) Name: bit_size
      Type: pm_lpg_pwm_size_type
      - Valid Inputs:
          PM_LPG_PWM_6BIT
          PM_LPG_PWM_9BIT
        
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.
     - PM_ERR_FLAG_INVALID_PARAMETER     -> clock value out of range.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_set_pwm_bit_size( uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan,pm_lpg_pwm_size_type bit_size );


/*===========================================================================
FUNCTION  pm_lpg_bank_enable                           EXTERNAL FUNCTION

DESCRIPTION
   This function enables a LPG bank
PARAMETERS
   1) Name: bank
      Type: uint8
      - Select LPG Bank Number (0-7)
      
   2) Name: enable
      Type: boolean
        
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_bank_enable( uint32 pmic_device_index, uint8 bank, boolean enable );


/*===========================================================================
FUNCTION  pm_lpg_bank_select                           EXTERNAL FUNCTION

DESCRIPTION
   This function selects the LPG bank
PARAMETERS
   1) Name: bank
      Type: uint8
      - Select LPG Bank Number (0-7)
        
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_bank_select( uint32 pmic_device_index, uint8 bank );


/*===========================================================================
FUNCTION  pm_lpg_pwm_value_bypass_enable               EXTERNAL FUNCTION

DESCRIPTION
   This function enables user-inputted PWM value
PARAMETERS
   1) Name: enable
      Type: boolean
        
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.
     - PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_lpg_pwm_value_bypass_enable( uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan,boolean enable );


/**
 * @brief Selects LPG PWM channel
 *
 * @param [in] device index
 *             lpg channel
 *             lpg source: LUT or pwm register 
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.  
 *         PM_ERR_FLAG_BUS_ERR           = Communication with PM chip failed.
 *                                              with the PMIC.
 *         PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lpg_pwm_src_select (uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan,pm_lpg_src_type lpg_src_type );

/**
 * @brief Configures the pwm type
 *
 * @param [in] device index 
 *                 PmicDeviceIndex - For primary 0 
 *             lpg channel
 *                lpg channel number. refer pm_lpg_chan_type
 *             glitch_removal:
 *                      FLASE : No glitch removal -PWM outputs are updated immediately
 *                      TRUE : glitch removal WM outputs are updated only on PWM period boundaries
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.  
 *         PM_ERR_FLAG_BUS_ERR           = Communication with PM chip failed.
 *                                              with the PMIC.
 *         PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lpg_config_pwm_type(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, boolean glitch_removal);

/**
 * @brief Configures lpg pattern
 *
 * @param [in] device index 
 *                 PmicDeviceIndex - For primary 0 
 *             lpg channel
 *                lpg channel number. refer pm_lpg_chan_type
 *             ramp_direction:
 *                FLASE : ramp from start (HI_INDEX) to end (LO_INDEX)
 *                TRUE :  ramp from start (LO_INDEX) to end (HI_INDEX)
 *            pattern_repeat:
 *                FLASE : no looping (stop after 1 pattern
 *                TRUE :  pattern looping until this bit is set to 0
 *             ramp_toggle
 *                FALSE: return to start when end is reached
 *                TRUE:  if start was LO_INDEX, then return to LO_INDEX when HI_INDEX value is reached (pause is disabled) if start was HI_INDEX, then return to HI_INDEX when
 *						   LO_INDEX value is reached (pause is disabled)
 *             en_pause_hi:
 *                FLASE : disable
 *                TRUE :  enable pause time at HI_INDEX (timing defined in pause hi counter)
 *             en_pause_lo: 
 *                FLASE : disable
 *                TRUE :  enable pause time at HI_INDEX (timing defined in pause hi counter)
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.  
 *         PM_ERR_FLAG_BUS_ERR           = Communication with PM chip failed.
 *                                              with the PMIC.
 *         PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lpg_pattern_config(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, boolean ramp_direction, boolean pattern_repeat, boolean ramp_toggle, boolean en_pause_hi, boolean en_pause_lo);

/**
 * @brief API to write a value to the look up table
 *
 * @param [in] device index
 *				   PmicDeviceIndex - For primary 0
 *             lut_index:
                    LUT byte number from 0 to 63
 *             pwm_value:
 *					16 bit value(msb and lsb) for LUT byte number
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.  
 *         PM_ERR_FLAG_BUS_ERR           = Communication with PM chip failed.
 *                                              with the PMIC.
 *         PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_lpg_lut_config_set(uint32 pmic_device_index, uint32 lut_index, uint32 pwm_value);


/**
 * @brief API to read one value from the LPG look up table
 * 
 * @details This API reads one of the values from the LPG look
 *          up table.
 * 
 * @brief read the last look up table entry written 
 * @param [in] device index
 *				   PmicDeviceIndex - For primary 0 
 * @param lut_index     Look up table index (0 to 63)
 * @param pwm_value     The look up table value
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_lut_config_get(uint32 pmic_device_index, uint32 lut_index, uint32 *pwm_value);

/**
 * @brief API to write an array of values to the look up table 
 *        starting at the index given
 * 
 * @details This API writes a number of values to consecutive 
 *          locations in the LUT starting at the index given
 * @param [in] device index
 *				   PmicDeviceIndex - For primary 0  
 * @param start_index  which of the look up table entries to 
 *                    write
 * @param value  pointer to an array of 9 bit PWM values. The 
 *               array must be large enough to contain at least
 *               "count" values.
 * @param count  number of entries in the array 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_lut_config_set_array(uint32 pmic_device_index, uint32 start_index, uint32 *value, uint32 count);

/**
 * @brief API to read values from consecutive locations in the 
 *        LUT starting at the index given and store the values
 *        to the array passed.
 *  
 * @details This API reads the requested number of values from
 *          consecutive locations in the LUT starting at the
 *          index given and stores them in the array passed.
 * @param [in] device index
 *				   PmicDeviceIndex - For primary 0   
 * @param start_index  which of the look up table entries to 
 *                    read
 * @param value  pointer to an array of 9 bit PWM values. The 
 *               array must be large enough to store "count"
 *               values.
 * @param count  number of entries in the array 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_lut_config_get_array(uint32 pmic_device_index, uint32 start_index, uint32 *value, uint32 count);

/**
 * @brief Starts the lut ramp for a selected lpg
 *
 * @param [in] device index
 *				   PmicDeviceIndex - For primary 0
 *             lpg: Refer pm_lpg_chan_type for more.
 *
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *                                              version of the PMIC.
 *         PM_ERR_FLAG_INVALID_PARAMETER     = Input Parameter is out of range.  
 *         PM_ERR_FLAG_BUS_ERR           = Communication with PM chip failed.
 *                                              with the PMIC.
 *         PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */


pm_err_flag_type pm_lpg_pwm_ramp_generator_start(uint32 pmic_device_index, pm_lpg_chan_type lpg);

/**
 * @brief API to set the look up table index
 * 
 * @details This API sets the look up table index used for loop
 *          and ramp functions.
 * 
 * @param [in] device index 
 *                 PmicDeviceIndex - For primary 0 
 *             lpg channel
 *                lpg channel number. refer pm_lpg_chan_type
 * @param low_index  Low look up table index to use with loop and ramp functions.
 *                   Range is 0 to 63.
 * @param high_index High look up table index to use with loop and ramp functions.
 *                   Range is 0 to 63.
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_pwm_lut_index_set(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, uint32 low_index, uint32 high_index);

/**
 * @brief API  to configure hi and low pause time for LPG
 * 
 * @details This API  configures  pause time by setting  ramp step, pause_hi multiplier and pause_lo multiplier
 *                             hi  pause time=ramp_step * hi_multiplier    milli seconds
 *                             low pause time=ramp_step * low_multiplier    milli seconds
 * 
 * @param [in] device index 
 *                  PmicDeviceIndex - For primary 0 
 * @param [in] lpg channel
 *                  lpg channel number. refer pm_lpg_chan_type
 * @param [in] ramp_step_duration 
 *                  Ramp step, also time between access of 2 entries in the LUT, in milliseconds. 
 *                  Valid value is 0 to 511 milliseconds
 * @param [in] hi_multiplier
 *                  multiplier for HI_INDEX. Valid value is 0 to 0xFFF
 * @param [in] low_multiplier 
 *                  multiplier for LOW_INDEX. Valid value is 0 to 0xFFF
 *  
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_config_pause_time(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, uint32 ramp_step_duration, uint32 hi_multiplier, uint32 low_multiplier);


/**
 * @brief API to enable or disable the PWM ramp generator
 * 
 * @details This API enables or disables the LPG PWM ramp
 *          generator.
 * 
 * @param [in] device index 
 *                  PmicDeviceIndex - For primary 0 
 * @param [in] lpg channel
 *                lpg channel number. refer pm_lpg_chan_type
 * @param enable    - TRUE enable the LPG ramp generator
 *                  - FALSE disable the LPG ramp generator
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_pwm_ramp_generator_enable(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, boolean enable);


/**
 * @brief API to read LPG status
 * 
 * @details This API reads the LPG register settings 
 *  
 * 
 * @param [in] device index 
 *                  PmicDeviceIndex - For primary 0 
 * @param [in] lpg channel
 *                lpg channel number. refer pm_lpg_chan_type
 * @param [out] lpg_status
 *                  Refer enum pm_lpg_status_type
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_get_status(uint32 pmic_device_index, pm_lpg_chan_type pm_lpg_chan, pm_lpg_status_type *lpg_status);


/**
 * @brief API to set LPG OUT for LGP channel on select LPG DTEST line
 * 
 * @details This API reads the LPG register settings 
 *  
 * 
 * @param [in] device index 
 *                  PmicDeviceIndex - For primary 0 
 * @param [in] lpg channel
 *                lpg channel number. refer pm_lpg_chan_type
 * @param [in] dtest_type
 *                  lpg dtest line. Refer enum pm_lpg_chan_dtest_type
 * @param [in] lpg_out
 *                  lpg out for dtest line. Refer enum pm_lpg_chan_lpg_out_type
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_lpg_set_lpg_dtest(uint32 pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_chan_dtest_type dtest_type, pm_lpg_chan_lpg_out_type lpg_out);



#endif // __PM_LPG_H__
