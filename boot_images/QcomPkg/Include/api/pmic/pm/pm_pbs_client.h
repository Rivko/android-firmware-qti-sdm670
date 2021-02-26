#ifndef __PM_PBS_CLIENT_H__
#define __PM_PBS_CLIENT_H__

/*! \file pm_pbs_client.h
 *  \n
 *  \brief This header file contains API and type definitions for PBS Client driver.
 *  \n
 *  \n &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/17   ks      Added pm_pbs_oledb_sw_trigger(CR-2004206)
05/15/17   aab     Updated interface to pm_pbs_client_hw_trigger_enable() to include edge triggering
12/13/16   cs      added support for pbs triggered AFP
06/16/13   kt      Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/** 
  @enum pm_pbs_client_type
  @brief PBS Client types
 */
typedef enum 
{
  PM_PBS_CLIENT_0  = 0,
  PM_PBS_CLIENT_1  = 1,
  PM_PBS_CLIENT_2  = 2,
  PM_PBS_CLIENT_3  = 3,
  PM_PBS_CLIENT_INVALID
}pm_pbs_client_type;

/** 
  @enum pm_pbs_client_trigger
  @brief sw/hw triggers in each PBS client
 */
typedef enum 
{
  PM_PBS_CLIENT_HW_TRIGGER_0 = 0,
  PM_PBS_CLIENT_HW_TRIGGER_1 = 1,
  PM_PBS_CLIENT_HW_TRIGGER_2 = 2,
  PM_PBS_CLIENT_HW_TRIGGER_3 = 3,
  PM_PBS_CLIENT_HW_TRIGGER_4 = 4,
  PM_PBS_CLIENT_HW_TRIGGER_5 = 5,
  PM_PBS_CLIENT_HW_TRIGGER_6 = 6,
  PM_PBS_CLIENT_SW_TRIGGER,
  PM_PBS_CLIENT_TRIGGER_INVALID
}pm_pbs_client_trigger;

typedef enum
{
  PM_PBS_CLIENT_TRIGGER_RISING_EDGE  = 0,
  PM_PBS_CLIENT_TRIGGER_FALLING_EDGE = 1,
  PM_PBS_CLIENT_TRIGGER_DUAL_EDGE    = 2,
  PM_PBS_CLIENT_TRIGGER_EDGE_INVALID
}pm_pbs_client_trigger_edge_type;



/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/**
 * @name pm_pbs_client_enable
 * 
 * @brief This function enables or disables all the triggers
 *        corresponding to the specific PBS Client.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1  
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] enable:  
 *                TRUE: Enable PBS Client triggers 
 *                FALSE:Disable PBS Client triggers
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_enable(uint8 pmic_device_index, pm_pbs_client_type client, boolean enable);

/**
 * @name pm_pbs_client_enable_status
 * 
 * @brief This function returns the PBS Client enable/disable 
 *        status.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1  
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[out] status:  
 *                TRUE: PBS Client triggers are enabled
 *                FALSE:PBS Client triggers are disabled
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_enable_status(uint8 pmic_device_index, pm_pbs_client_type client, boolean *status);

/**
 * @name pm_pbs_oledb_sw_trigger
 *  
 * @brief Force trigger the PBS Amoled Display power off sequence through PBS client software 
 *        or hardware trigger by writing to the
 *        TRIG_CTL register.
 * 
 * @param[in] pmic_chip. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] trigger: PBS Client hardware trigger number or 
 *                     software trigger. Refer enum
 *                     pm_pbs_client_trigger type above.
 * @param[in] trigger_argument: Argument to be passed to PBS 
 *                             trigger sequence. Enter NULL, if
 *                             no argument
 * @param[out] trigger_return_value: value returned by PBS 
 *                                  trigger sequence. Enter
 *                                  NULL, if no return value is
 *                                  expected
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_oledb_sw_trigger(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_trigger trigger, 
                         uint8 *trigger_argument, uint8 *trigger_return_value);

/**
 * @name pm_pbs_client_sw_trigger
 *  
 * @brief Force triggers the corresponding PBS client software 
 *        or hardware trigger sequence by writing to the
 *        TRIG_CTL register.
 * 
 * @param[in] pmic_chip. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] trigger: PBS Client hardware trigger number or 
 *                     software trigger. Refer enum
 *                     pm_pbs_client_trigger type above.
 * @param[in] trigger_argument: Argument to be passed to PBS 
 *                             trigger sequence. Enter NULL, if
 *                             no argument
 * @param[out] trigger_return_value: value returned by PBS 
 *                                  trigger sequence. Enter
 *                                  NULL, if no return value is
 *                                  expected
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_sw_trigger(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_trigger trigger, 
                         uint8 *trigger_argument, uint8 *trigger_return_value);


/**
 * @name pm_pbs_client_hw_trigger_disable
 *  
 * @brief Force triggers the corresponding PBS client software 
 *        or hardware trigger sequence by writing to the
 *        TRIG_CTL register.
 * 
 * @param[in] pmic_chip. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] hw_trigger: PBS Client hardware trigger number or 
 *                     software trigger. Refer enum
 *                     pm_pbs_client_trigger type above.
 * @param[in] trigger_edge: PBS Client trigger edge type .
 *                     Refer enum
 *                     pm_pbs_client_trigger_edge_type type
 *                     above.
  * @param[in] enable: TRUE to enable and FALSE to disable
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_hw_trigger_enable(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger hw_trigger, pm_pbs_client_trigger_edge_type trigger_edge, boolean enable);

#endif /* __PM_PBS_CLIENT_H__ */
