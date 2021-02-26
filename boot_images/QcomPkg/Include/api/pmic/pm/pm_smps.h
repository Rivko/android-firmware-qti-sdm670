#ifndef __PM_SMPS_H__
#define __PM_SMPS_H__


/*===========================================================================
                            PMIC VREG MODULE

DESCRIPTION
  This file contains the implementation of the VREG SMPS driver developed
  for the Qualcomm Power Management IC.

  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/08/16   aab     Added pm_smps_ocp_latched_status_clear()
11/03/15   al      Updated rails API
08/11/14   al      Adding API to read VREG_OK
11/22/13   va      PmicLib Dec Addition
03/26/13   al	   Adding pin ctrl, pull down and sw enable status 
02/08/13   al      Adding prototypes to fix warnings
07/27/12   al	   Added buck switching freq
7/27/12    al	   Renamed all Enums,added device index and resource index
09/14/11   sm      New File.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/* SMPS peripheral index. This enum type contains all ldo regulators that you may need. */
enum
{
  PM_SMPS_1,   /**< SMPS 1. */
  PM_SMPS_2,   /**< SMPS 2. */
  PM_SMPS_3,   /**< SMPS 3. */
  PM_SMPS_4,   /**< SMPS 4. */
  PM_SMPS_5,   /**< SMPS 5. */    
  PM_SMPS_6,   /**< SMPS 6. */
  PM_SMPS_7,   /**< SMPS 7. */
  PM_SMPS_8,   /**< SMPS 8. */
  PM_SMPS_9,   /**< SMPS 9. */
  PM_SMPS_10,  /**< SMPS 10. */  
  PM_SMPS_11,  /**< SMPS 11. */   
  PM_SMPS_12,  /**< SMPS 12. */
  PM_SMPS_13,  /**< SMPS 13. */  
  PM_SMPS_INVALID
};


/* smps mode type */
typedef enum
{
    PM_ILIM_SMPS_PWM_MODE,  
    PM_ILIM_SMPS_AUTO_MODE,
    PM_ILIM_SMPS_MODE_INVALID
}pm_smps_ilim_mode_type;



/** SMPS switching frequency. This enumeration assumes an input clock
    frequency of 19.2 MHz and is five bits long.

    Clock frequency = 
    (input clock frequency) / ((CLK_PREDIV+1)(CLK_DIV + 1)).
 */
typedef enum 
{
    PM_CLK_19p2_MHz     = 0,   /**< Clock frequency = 19.2 MHz. */
    PM_CLK_9p6_MHz      = 1,   /**< Clock frequency = 9.6 MHz. */
    PM_CLK_6p4_MHz      = 2,   /**< Clock frequency = 6.2 MHz. */
    PM_CLK_4p8_MHz      = 3,   /**< Clock frequency = 4.8 MHz. */
    PM_CLK_3p84_MHz     = 4,   /**< Clock frequency = 3.84 MHz. */
    PM_CLK_3p2_MHz      = 5,   /**< Clock frequency = 3.2 MHz. */
    PM_CLK_2p74_MHz     = 6,   /**< Clock frequency = 2.74 MHz. */
    PM_CLK_2p4_MHz      = 7,   /**< Clock frequency = 2.4 MHz. */
    PM_CLK_2p13_MHz     = 8,   /**< Clock frequency = 2.13 MHz. */
    PM_CLK_1p92_MHz     = 9,   /**< Clock frequency = 1.92 MHz. */
    PM_CLK_1p75_MHz     = 10,  /**< Clock frequency = 1.75 MHz. */
    PM_CLK_1p6_MHz      = 11,  /**< Clock frequency = 1.6 MHz. */
    PM_CLK_1p48_MHz     = 12,  /**< Clock frequency = 1.48 MHz. */
    PM_CLK_1p37_MHz     = 13,  /**< Clock frequency = 1.37 MHz. */
    PM_CLK_1p28_MHz     = 14,  /**< Clock frequency = 1.28 MHz. */
    PM_CLK_1p2_MHz      = 15,  /**< Clock frequency = 1.2 MHz. */
    PM_CLK_1p13_MHz     = 16,  /**< Clock frequency = 1.13 MHz. */
    PM_CLK_1p07_MHz     = 17,  /**< Clock frequency = 1.07 MHz. */
    PM_CLK_1p01_MHz     = 18,  /**< Clock frequency = 1.01 MHz. */
    PM_CLK_960_KHz      = 19,  /**< Clock frequency = 960 kHz. */
    PM_CLK_914_KHz      = 20,  /**< Clock frequency = 914 kHz. */
    PM_CLK_873_KHz      = 21,  /**< Clock frequency = 873 kHz. */
    PM_CLK_835_KHz      = 22,  /**< Clock frequency = 835 kHz. */
    PM_CLK_800_KHz      = 23,  /**< Clock frequency = 800 kHz. */
    PM_CLK_768_KHz      = 24,  /**< Clock frequency = 768 kHz. */
    PM_CLK_738_KHz      = 25,  /**< Clock frequency = 738 kHz. */
    PM_CLK_711_KHz      = 26,  /**< Clock frequency = 711 kHz. */
    PM_CLK_686_KHz      = 27,  /**< Clock frequency = 686 kHz. */
    PM_CLK_662_KHz      = 28,  /**< Clock frequency = 662 kHz. */
    PM_CLK_640_KHz      = 29,  /**< Clock frequency = 640 kHz. */
    PM_CLK_619_KHz      = 30,  /**< Clock frequency = 619 kHz. */
    PM_CLK_600_KHz      = 31,  /**< Clock frequency = 600 kHz. */
    PM_SWITCHING_FREQ_INVALID,
    PM_SWITCHING_FREQ_FREQ_NONE
}pm_smps_switching_freq_type;

/* Quiet Mode */
typedef enum
{
    PM_QUIET_MODE__DISABLE,      /**< Quiet mode disabled (default). */
    PM_QUIET_MODE__QUIET,        /**< Quiet mode enabled. */
    PM_QUIET_MODE__SUPER_QUIET,  /**< Super Quiet mode enabled. @newpage */
    PM_QUIET_MODE__INVALID
}pm_quiet_mode_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** Switches between HPM, LPM, and  modes of a regulator.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] sw_mode                Selects the mode of a regulator.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_sw_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type sw_mode);

/** Returns the mode status (LPM, NPM, Auto, Bypass) of the selected
    power rail.
    
    @note1hang The regulator mode changes dynamically.
 
    @param[in] pmic_chip             Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[out] sw_mode              Variable to return to the caller
                                     with the mode status. Refer to
                                     #peripheral_info_type.

    @return   
    Error flag type -- See #pm_err_flag_type.
    @newpage
 */
pm_err_flag_type 
pm_smps_sw_mode_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type* sw_mode);

/** Gets the register value [MODE_CTL] of an instance.
 
    @note1hang The regulator mode changes dynamically.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] mode                   Obtains the mode_ctl register
                                      value of a regulator, i.e., NPM,
                                      Auto, etc.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_sw_mode_status_raw(uint8 pmic_chip, uint8 smps_peripheral_index, uint8* mode);

/** Enables or disables the SMPS rail.

    @param[in] pmic_chip             Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
									 See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] on_off                Turn the SMPS on/off. See #pm_on_off_type.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_sw_enable(uint8 pmic_chip, uint8  smps_peripheral_index, pm_on_off_type  on_off);

/** Returns the software enable status of the SMPS.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] on_off                 Pointer to the on/off status of
                                      the regulator.
 
    @return 
    Error flag type -- See #pm_err_flag_type.

    @newpage
 */
pm_err_flag_type 
pm_smps_sw_enable_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type* on_off);

/** Returns the VREG_READY status of the SMPS which includes
    VREG_OK and STEPPER_DONE combined status.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[out] vreg_ready            Pointer to the VREG_READY
          status.
  
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_vreg_ready_status(uint8 pmic_chip, uint8 smps_peripheral_index, boolean* vreg_ready);

/**
 * Sets the voltage of a SMPS rail.
 
 *  @note1hang Differnt types of voltage rails (HFS, FTS, etc.) may have different
 *   programmable voltage steps. Only support the correct
 *   programmable steps. Do not round voltage values if the voltage
 *   selected is not part of the programmable steps.
 * 
 * @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
 *                                   0 (for the first SMPS peripheral).
 *                                   See @xnameref{hdr:smpsPeripheralIndex}.
 * 
 * @param[in] volt_level Select the voltage.
 * 
 * @return 
 * Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_volt_level(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level);

/**
 * Returns the voltage level (in microvolts) of the selected power rail.
 *
 * @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
 *                                   0 (for the first SMPS peripheral).
 *                                   See @xnameref{hdr:smpsPeripheralIndex}.
 *
 * @param[out] volt_level            Variable to return to the caller
 *                                   with the voltage level in
 *                                   microvolts (uint32).
 *
 * @return 
 * Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_volt_level_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type* volt_level);


/** Selects a frequency for the SMPS. Each SMPS can operate under a
    different frequency.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] switching_freq         Select a frequency, e.g.,
                                      19.2 MHz, 3.2 MHz, etc. See 
                                      #pm_smps_switching_freq_type.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_switching_freq(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type switching_freq);

/** Returns the SMPS frequency. Each SMPS can operate under a
    different frequency.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[out] freq                 Pointer to the SMPS frequency.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_switching_freq_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type *freq);

/**
  Configures and sets quiet mode for SMPS.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] quiet_mode            Enable/disable Quiet mode and
                                     select the Quiet mode type. See
                                     #pm_quiet_mode_type.

	@return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_quiet_mode_mask(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type quiet_mode);

/**
  Sets global quiet mode enable for SMPS.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] on_off                Enable/disable global Quiet
                                     mode enable.

	@return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_global_quiet_mode_enable(uint8 pmic_chip, pm_on_off_type on_off);

/**
 * @name 
 *     pm_smps_quiet_mode
 *
 * @description
 *    Set quiet mode for SMPS.
 * 
 * @param 
 *     quiet_mode - Enable/Disable quiet mode as well as selecting different types of quiet mode.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_smps_quiet_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type quiet_mode, uint16 voltage_mv);




/**
 * @name 
 *     pm_smps_pull_down
 *
 * @description
 *     Allows you to enable disable active pulldown.
 * 
 * @param 
 *     on_off - turn on and off active pulldown
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_smps_pull_down(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off);

/**
 * @name 
 *     pm_smps_pull_down_status
 *
 * @description
 *     Reads smps pull down status
 * 
 * @param 
 *     on_off - turn on and off active pulldown
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_smps_pull_down_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type* on_off);

/**
 * @name 
 *     pm_smps_pin_ctrled
 *
 * @description
 *     select the pin ( connected to external signal ) that you would like to use
 *     to effect the state ( on/off ) and mode ( HPM, LPM etc ) of a regulator
 * 
 * @param 
 *     select_pin - Select a pin
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_smps_pin_ctrled(uint8 pmic_chip, uint8 smps_peripheral_index, uint8 select_pin);

/** 
 * @name pm_smps_pin_ctrled_status 
 *  
 * @brief This function returns the pin ctrled status or h/w 
 *        enable status (On/Off) of the selected power rail.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  smps_peripheral_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] on_off:  
 *                Variable to return to the caller with pin ctrl
 *                status. Refer to pm_resources_and_types.h for
 *                the enum info.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_smps_pin_ctrled_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type* on_off);
/**
 * @name 
 *     pm_smps_inductor_ilim
 *
 * @description
 *    Select the current limit for the inductor of a selected
 *    SMPS.
 * 
 * @param 
 *  
 *     pmic_chip. Primary: 0. Secondary: 1
 *  
 *     smps_peripheral_index: Starts from 0 (for first SMPS
 *     peripheral)
 *  
 *     ilim_level_milli_amps - Select the current limit level in
 *     milli-amps. If a current limit level between the
 *     supported current limit values is sent then it's rounded
 *     to the upper limit value.
 *  
 *     smps_mode :
 *      PM_ILIM_SMPS_PWM_MODE :  Current limit for PWM mode
 *      PM_ILIM_SMPS_AUTO_MODE:  Current limit for AUTO mode
 *      (Whenever there is a mode transition b/n PFM and PWM
 *      mode under Auto-mode operation) Current limit setting
 *      for AUTO mode is not supported on FTS bucks.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 * @example 
 *     pm_smps_inductor_ilim(0,PM_SMPS_1,2500,PM_ILIM_SMPS_PWM_MODE);
 */
pm_err_flag_type 
pm_smps_inductor_ilim(uint8 pmic_chip, uint8 smps_peripheral_index, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode);


/**
 * @name 
 *     pm_smps_inductor_ilim_status
 *
 * @description
 *    Obtain the current limit for the inductor of a selected
 *    SMPS by reading the SPMI register.
 * 
 * @param 
 *  
 *     pmic_chip. Primary: 0. Secondary: 1
 *  
 *     smps_peripheral_index: Starts from 0 (for first SMPS
 *     peripheral)
 *  
 *     *ilim_level_milli_amps - Returns the current limit level in
 *      milli-amps
 *  
 *     smps_mode : 
 *      PM_ILIM_SMPS_PWM_MODE :  Current limit for PWM mode
 *      PM_ILIM_SMPS_AUTO_MODE:  Current limit for AUTO mode
 *      (Whenever there is a mode transition b/n PFM and PWM
 *      mode under Auto-mode operation) Current limit setting
 *      for AUTO mode is not supported on FTS bucks.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_smps_inductor_ilim_status(uint8 pmic_chip, uint8 smps_peripheral_index, uint32 *ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode);



/** Configures to clear VREG_OCP status of the SMPS .

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] peripheral_index       SMPS peripheral index.
                                      Starts at 0 (for the first
                                      SMPS peripheral). See
                                      @xnameref{hdr:smpsPeripheralIndex}.
    
    @param[in] *ocp_occured           1=OCP status bit was set
                                      0=OCP status bit was NOT
                                      set
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_smps_ocp_latched_status_clear(uint8 pmic_chip, uint8 peripheral_index, boolean *ocp_occured);


/** Enables or disables the SMPS OCP broadcast.

    @param[in] pmic_chip             Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
									 See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] on_off                Turn the SMPS on/off. See #pm_on_off_type.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_ocp_broadcast
(uint8 pmic_chip, uint8  smps_peripheral_index, pm_on_off_type  on_off);



#endif /* PM_SMPS__H */
