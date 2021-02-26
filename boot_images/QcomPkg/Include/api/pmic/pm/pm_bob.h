#ifndef __PM_BOB_H__
#define __PM_BOB_H__

/*! \file pm_bob.h
 *  \n
 *  \brief This header file contains enums and API definitions for PMIC BOB
 *         power rail driver.
 *  \n
 *  \n &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/pmic/pm/pm_bob.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/20/17   czq     Add pm_bob_ocp_latched_status_clear()
06/22/17   al      Get BOB volt level status
12/22/15   al      Created
=============================================================================*/

/*===========================================================================

                 HEADER FILE INCLUDE

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*!
 *  \brief 
 *  BOB peripheral index. This enum type contains all bob regulators that you may need. 
 */

enum
{
  PM_BOB_1,
  PM_BOB_INVALID
};

enum
{
  PM_BOB_EXT_PIN_RESERVED = 0,
  PM_BOB_EXT_PIN_CTRL1 = 1,
  PM_BOB_EXT_PIN_CTRL2 = 2,
  PM_BOB_EXT_PIN_CTRL3 = 3,
  PM_BOB_EXT_PIN_MAX,
};

typedef enum
{
  PM_BOB_MODE_PASS,
  PM_BOB_MODE_PFM,
  PM_BOB_MODE_AUTO,
  PM_BOB_MODE_PWM,
  PM_BOB_MODE_INVALID,
}pm_bob_mode_type; 

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/**
 * Gets the status of the modes of BOB.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *   
 * @param[in] peripheral_index LDOperipheral index. Starts from 0
 *
 * @param[out] sw_mode             Pointer to the mode of a bob, e.g.,
 *                                 HPM, LPM, BYPASS. See #pm_bob_mode_type.
 * 
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_bob_sw_mode_status(uint8 pmic_chip, uint8 peripheral_index, pm_bob_mode_type* mode);


/**
 *  Gets the voltage level status for BOB PIN.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] peripheral_index     peripheral index. Starts from
 *       0
 *
 * @param[out] volt_level          Pointer to the voltage level.
 * 
 * @return 
 *  SUCCESS or Error -- See        pm_err_flag_type.
 */
pm_err_flag_type pm_bob_pin_volt_level_status(uint8 pmic_chip, uint8 peripheral_index, uint8 pin_number, pm_volt_level_type* volt_level);


/**
 *  Gets the BOB voltage level status.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] peripheral_index     peripheral index. Starts from 0
 *       
 *
 * @param[out] volt_level          Pointer to the voltage level.
 * 
 * @return 
 *  SUCCESS or Error -- See        pm_err_flag_type.
 */
pm_err_flag_type pm_bob_volt_level_status(uint8 pmic_chip, uint8 peripheral_index, pm_volt_level_type *volt_level);


/** Configures to clear VREG_OCP status of the BOB .

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] peripheral_index       BOB peripheral index.
                                      Starts at 0 (for the first
                                      BOB peripheral). See
                                      @xnameref{hdr:ldoPeripheralIndex}.
    @param[in] *ocp_occured           1=OCP status bit was set
                                      0=OCP status bit was NOT
                                      set
    @return 
    Error flag type -- See #pm_err_flag_type.
*/
pm_err_flag_type 
pm_bob_ocp_latched_status_clear(uint8 pmic_chip, uint8 peripheral_index,  boolean *ocp_occured);

#endif /* __PM_BOB_H__ */

