#ifndef PM_CLK_DRIVER__H
#define PM_CLK_DRIVER__H

/*! \file pm_clk_driver.h
 *  \n
 *  \brief This file contains CLOCK peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/drivers/clk/src/pm_clk_driver.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
04/12/13    aks      Created as part of Code refactoring.
========================================================================== */
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_comm.h"
#include "pm_utils.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/

#define PM_CLK_INVALID_DATA   99

/* Please make sure to change the clk_info and clk_dep info arrays in
   config folder when the below macro is changed */
#define PM_MAX_NUM_CLKS       14

/** PMIC clock type. */
typedef enum
{
	PM_CLK_SLEEP,   /**< Sleep clock. */
	PM_CLK_XO,      /**< XO clock. */
	PM_CLK_RF_1,    /**< RF 1 clock. */
	PM_CLK_RF_2,    /**< RF 2 clock. */
	PM_CLK_RF_3,    /**< RF  clock. */
    PM_CLK_BB_1,    /**< BB 1 clock. */
    PM_CLK_BB_2,    /**< BB 2 clock. */
    PM_CLK_BB_3,    /**< BB 3 clock. */
    PM_CLK_DIFF_1,  /**< Diff 1 clock. */
    PM_CLK_DIV_1,   /**< Div 1 clock. */
    PM_CLK_DIV_2,   /**< Div 2 clock. */
    PM_CLK_DIV_3,   /**< Div 3 clock. */
    PM_CLK_DIST,    /**< Clock distribution module.  */ 
    PM_CLK_LN_BB,   /**< Low-noise baseband clock. */
    PM_ALL_CLKS,    /**< All clocks. */
    PM_CLK_INVALID  /**< Invalid clock. */
}pm_clk_type;

typedef struct 
{ 
    pm_register_address_type    base_address;  
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    EN_CTL;     //0x46
    pm_register_address_type    TIMER;      //0x4C
}pm_clk_register_info_type;

typedef struct 
{
    pm_comm_info_type               *comm_ptr;
    uint32                          num_of_peripherals;
    uint8                           periph_subtype[PM_MAX_NUM_CLKS];
    //array of clk peripheral index  to get to right offset from base
    uint8                           clk_perph_index[PM_ALL_CLKS];
    pm_clk_type                     *clk_common;
    pm_clk_register_info_type       *clk_reg_table;
}pm_clk_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_clk_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_clk_data_type* pm_clk_get_data(uint8 pmic_index);

uint8 pm_clk_get_num_peripherals(uint8 pmic_index);

/*===========================================================================

                        API

===========================================================================*/

/**
* Force enables or allows disable of the clock.
*
* @param[in] pmic_chip Selects the PMIC chip in which the buffers being
*                      controlled are located. The device index starts with
*                      zero.
* @param[in] periph Selects the clock peripheral to turn on.
*               See #pm_clk_type.
* @param[in] on_off Turns a clock on or off (enable or disable).
*                   See #pm_on_off_type.
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_clk_sw_enable(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off);

/**
* Returns the clock software enable status.
*
* @param[in] pmic_chip Selects the PMIC chip in which the buffers being
*                      controlled are located. The device index starts with
*                      zero.
* @param[in] periph Selects the clock peripheral to turn on.
*               See #pm_clk_type.
* @param[out] on_off Returns clock on or off status (enable or disable).
*                   See #pm_on_off_type.
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_clk_sw_enable_status(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type* on_off);

/**
* Returns the currently configured clock warmup time delay in 
* micro seconds. 
*
* @param[in] pmic_chip Selects the PMIC chip in which the buffers being
*                      controlled are located. The device index starts with
*                      zero.
* @param[in] periph Selects the clock peripheral to turn on.
*                   See #pm_clk_type.
* @param[out] warmup_time_usec Pointer to return the configured 
*                              warmup time delay value in uS.
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_clk_get_warmup_time(uint8 pmic_chip, pm_clk_type periph, uint32 *warmup_time_usec);

#endif /* PM_CLK_DRIVER_H */


