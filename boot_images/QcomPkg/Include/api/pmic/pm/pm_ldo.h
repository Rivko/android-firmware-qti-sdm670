#ifndef __PM_LDO_H__
#define __PM_LDO_H__

/*===========================================================================
                            PMIC VREG MODULE

DESCRIPTION
  This file contains the implementation of the VREG LDO driver developed
  for the Qualcomm Power Management IC.

Copyright (c) 2011 - 2017 by QUALCOMM Technologies Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/08/16   aab     Added pm_ldo_ocp_latched_status_clear()
11/19/15   al      Fix copyright info 
11/03/15   al      Updated rails API
08/11/14   al      Adding API to read VREG_OK
05/20/14   al      Architecture update 
03/26/13   al	   Adding pin ctrl, pull down and sw enable status
07/27/12   al	   Renamed all Enums,added device index and resource index
09/14/11   sm      New File.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"


/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/

/*!
 *  \brief 
 *  LDO peripheral index. This enum type contains all ldo regulators that you may need. 
 */
enum
{
  PM_LDO_1,       /**< LDO 1. */
  PM_LDO_2,       /**< LDO 2. */
  PM_LDO_3,       /**< LDO 3. */
  PM_LDO_4,       /**< LDO 4. */
  PM_LDO_5,       /**< LDO 5. */
  PM_LDO_6,       /**< LDO 6. */
  PM_LDO_7,       /**< LDO 7. */
  PM_LDO_8,       /**< LDO 8. */
  PM_LDO_9,       /**< LDO 9. */
  PM_LDO_10,      /**< LDO 10. */
  PM_LDO_11,      /**< LDO 11. */
  PM_LDO_12,      /**< LDO 12. */
  PM_LDO_13,      /**< LDO 13. */
  PM_LDO_14,      /**< LDO 14. */
  PM_LDO_15,      /**< LDO 15. */
  PM_LDO_16,      /**< LDO 16. */
  PM_LDO_17,      /* LDO 17. */
  PM_LDO_18,      /* LDO 18. */
  PM_LDO_19,      /* LDO 19. */
  PM_LDO_20,      /* LDO 20. */
  PM_LDO_21,      /* LDO 21. */
  PM_LDO_22,      /* LDO 22. */
  PM_LDO_23,      /* LDO 23. */
  PM_LDO_24,      /* LDO 24. */
  PM_LDO_25,      /* LDO 25. */
  PM_LDO_26,      /* LDO 26. */
  PM_LDO_27,      /* LDO 27. */
  PM_LDO_28,      /* LDO 28. */
  PM_LDO_29,      /* LDO 29. */
  PM_LDO_30,      /* LDO 30. */
  PM_LDO_31,      /* LDO 31. */
  PM_LDO_32,      /* LDO 32. */
  PM_LDO_33,      /* LDO 33. */
  PM_LDO_INVALID  /**< Invalid LDO. */
};

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** Switches between NPM, LPM, and other modes of a regulator.

    @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
                                    (for the first LDO peripheral).
                                    See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in] sw_mode              Selects the mode of a regulator, e.g.,
                                    HPM, LPM, BYPASS. See #pm_sw_mode_type.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_ldo_sw_mode(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type sw_mode);

/**
 * Gets the status of the modes of a regulator.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *   
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[out] sw_mode             Pointer to the mode of a regulator, e.g.,
 *                                 HPM, LPM, BYPASS. See #pm_sw_mode_type.
 * 
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */

pm_err_flag_type 
pm_ldo_sw_mode_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type* sw_mode);


/**
   Enables or disables a regulator or voltage switcher.

    @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] ldo_peripheral_index LDO peripheral index. The first LDO
                                    peripheral starts at 0. See
                                    @xnameref{hdr:ldoPeripheralIndex}.

    @param[in] on_off               Turns the regulator on and off.
                                    See #pm_on_off_type.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_ldo_sw_enable(uint8 pmic_chip, uint8  ldo_peripheral_index, pm_on_off_type  on_off);

/** Returns software enable status of the LDOs.

    @param[in]  pmic_chip Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in]  ldo_peripheral_index LDO peripheral index.
                Starts from 0 (for the first LDO peripheral).
                See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in] on_off Pointer to the on/off status of the regulator.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_ldo_sw_enable_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off);

/** Returns the VREG_READY status of LDOs which includes VREG_OK
    and STEPPER_DONE combined status.

    @param[in]  pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in]  ldo_peripheral_index LDO peripheral index. Starts from 0
                                     (for the first LDO peripheral).
                                     See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in]  vreg_ready           Pointer to the VREG_READY
          status.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_ldo_vreg_ready_status(uint8 pmic_chip, uint8 ldo_peripheral_index, boolean* vreg_ready);


/**
 * Sets the voltage level for an LDO.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[in] volt_level           Voltage level.
 * 
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */

pm_err_flag_type 
pm_ldo_volt_level(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level);

/**
 *  Gets the voltage level status for an LDO.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[out] volt_level          Pointer to the voltage level.
 * 
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_ldo_volt_level_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type* volt_level);


/**
 * @name 
 *     pm_ldo_pull_down
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
pm_ldo_pull_down(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off);


/**
 * @name 
 *     pm_ldo_pull_down_status
 *
 * @description
 *     Allows read pulldown status.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                LDO periphearl 
 * @param[out] on_off:  
 *                Refer to pm_resources_and_types.h for
 *                the enum info.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_ldo_pull_down_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off);


/**
 * @name 
 *     pm_ldo_pin_ctrled
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
pm_ldo_pin_ctrled(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8 select_pin);

/** 
 * @name pm_ldo_pin_ctrled_status 
 *  
 * @brief This function returns the pin ctrled status or h/w 
 *        enable status (On/Off) of the selected power rail.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
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
pm_ldo_pin_ctrled_status(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off);

/**
 * @name 
 *     pm_ldo_pull_down
 *
 * @description
 *     Allows you to enable disable active pulldown.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                LDO periphearl 
 * @param[in]  on_off:  
 *                Refer to pm_resources_and_types.h for
 *                the enum info.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type 
pm_ldo_pull_down(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off);


/** Configures to clear VREG_OCP status of the LDO .

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] peripheral_index       LDO peripheral index.
                                      Starts at 0 (for the first
                                      LDO peripheral). See
                                      @xnameref{hdr:ldoPeripheralIndex}.
    @param[in] *ocp_occured           1=OCP status bit was set
                                      0=OCP status bit was NOT
                                      set
    @return 
    Error flag type -- See #pm_err_flag_type.
*/
pm_err_flag_type 
pm_ldo_ocp_latched_status_clear(uint8 pmic_chip, uint8 peripheral_index,  boolean *ocp_occured);

/** Enables or disables the LDO OCP broadcast.
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                LDO periphearl 
 * @param[in]  on_off:  
 *                Refer to pm_ldo.h for
 *                the enum info.
 *
 *  @return 
 *  Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_ldo_ocp_broadcast(uint8 pmic_chip, uint8  ldo_peripheral_index, pm_on_off_type  on_off);


/** @} */ /* end_addtogroup pm_ldo */

#endif /* PM_LDO__H */

