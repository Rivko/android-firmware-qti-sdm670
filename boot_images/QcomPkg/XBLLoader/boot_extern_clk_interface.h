#ifndef BOOT_EXTERN_CLK_INTERFACE_H
#define BOOT_EXTERN_CLK_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN CLK DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external clock drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2015, 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/17   kpa     Update boot_clock_init_rpm argument
03/11/15   kpa     Added boot_Clock_SetBIMCSpeed
09/23/14   ck      Removed feature flag as API is always enabled in XBL
06/10/14   ck      Added boot_clock_get_apps_speed
07/26/13   rj      Added new api for enabling i2C clocks
07/11/13   rj      Passing ddr type parameter to boot_pre_ddr_clock_init() 
                   function
04/03/13   sl      Add boot_clock_get_ddr_speed
12/13/12   jz      Add boot_clock_set_perf_level
09/21/12   dh      Add boot_clock_debug_init
08/22/12   jz      Fixed compilation when extern clock driver is disabled
07/31/12   dh      Add boot_clock_exit_boot
05/08/12   dh      add new clock init api
09/23/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_authenticator.h"
#include "ClockBoot.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_clock_init

** ==========================================================================
*/
/*!
*
*  This function initializes the apps processor clocks and ramps up
*  the processor speed to the initial one specified in the BSP.
*  It should be invoked from the apps boot loader code.
*
* 
*
* @par Dependencies Shared memory must be ready.
*
* @return  FALSE if initialization failed.
*
* @par Side Effects  The core clock and MSMC2 voltage are switched.
*
*/
boolean boot_clock_init (void);


/* ========================================================================
**  Function : boot_clock_pre_ddr_init
** ======================================================================*/
/*!
    Description: Configure all clocks needed for DDR configuration.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean boot_pre_ddr_clock_init (uint32 ddr_type);


/* ========================================================================
**  Function : boot_pre_i2c_clock_init
** ======================================================================*/
/*!
    Description: Configure i2c clocks needed for eeprom init.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean boot_pre_i2c_clock_init (void);


/* ============================================================================
**  Function : boot_dload_clock_init
** ============================================================================
*/
/*!
    This will initialize clocks for the RAMs in the system that are to be saved
    during crash dump procedures in the downloader.

    @param
      None
    @return
      None

    @dependencies
    None.

    @sa None
*/
void boot_dload_clock_init(void);


/* ============================================================================
**  Function : boot_clock_exit_boot
** ============================================================================
*/
/*!
    This function will disable clocks that is not needed after SBL

    @param
      None
    @return
      True if success, False otherwise

    @dependencies
    None.

    @sa None
*/
boolean boot_clock_exit_boot(void);


/* ============================================================================
**  Function : Clock_DebugInit
** ============================================================================
*/
/*!

    This function is called very early in boot.  It is used for work-arounds that
    need to be done before JTAG attaches at the SBL "hold focus button" spin loop.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    You cannot use busywait in this function.

*/
boolean boot_clock_debug_init(void);


/*===========================================================================
**  Function :  boot_clock_set_perf_level
** ==========================================================================
*/
/*!
*
* This function Configures CPU to a specific perf level.
*
* @param eCPUPerfLevel [in] - CPU performance level.
*
* @return
*  TRUE -- CPU was configured to perf level successful.
*  FALSE -- Configuration failed.
*
* @dependencies
* None.
*
*/
boolean boot_clock_set_cpu_perf_level(ClockBootPerfLevelType eCPUPerfLevel);


/*===========================================================================
**  Function :  boot_clock_set_L2_perf_level
** ==========================================================================
*/
/*!
*
* This function Configures CPU to a specific perf level.
*
* @param eCPUPerfLevel [in] - CPU performance level.
*
* @return
*  TRUE -- CPU was configured to perf level successful.
*  FALSE -- Configuration failed.
*
* @dependencies
* None.
*
*/
boolean boot_clock_set_L2_perf_level(ClockBootPerfLevelType eCPUPerfLevel);


/*===========================================================================
**  Function :  boot_clock_set_sys_perf_level
** ==========================================================================
*/
/*!
*
* This function Configures CPU to a specific perf level.
*
* @param eCPUPerfLevel [in] - CPU performance level.
*
* @return
*  TRUE -- CPU was configured to perf level successful.
*  FALSE -- Configuration failed.
*
* @dependencies
* None.
*
*/
boolean boot_clock_set_sys_perf_level(ClockBootPerfLevelType eCPUPerfLevel);


/*===========================================================================
**  Function :  boot_clock_get_ddr_speed
** ==========================================================================
*/
/*!
*
* This function returns DDR clock speed in KHz.
*
* @param
*   None
*
* @return
*   DDR clock speed in KHz
*
* @dependencies
*   None
*
*/
uint32 boot_clock_get_ddr_speed(void);


/*===========================================================================
**  Function :  boot_clock_init_rpm
** ==========================================================================
*/
/*!
*
* This function initializes clock before the RPM launches 
*
* @param
*   None
*
* @return
*   True : Success
*   False : Failure
*
* @dependencies
*   None
*
*/
void boot_clock_init_rpm(void);


/*===========================================================================
**  Function :  boot_clock_get_apps_speed
** ==========================================================================
*/
/*!
*
* This function returns apps clock speed in KHz.
*
* @param
*   core - Core number to get clock speed for.
*
* @return
*   Apps clock speed in KHz
*
* @dependencies
*   None
*
*/
uint32 boot_clock_get_apps_speed(uint32 core);

/* =========================================================================
**  Function : boot_Clock_SetBIMCSpeed
** =========================================================================*/
/*!
    Switch DDR and the related BIMC roots to the requested frequency

    @param -  nFreqHz [in] Frequency to search for in the table
    @return - FALSE on fail, TRUE on success

    @dependencies
    None.

    @sa None
*/
boolean boot_Clock_SetBIMCSpeed(uint32 nFreqKHz );

#endif /* BOOT_EXTERN_CLK_INTERFACE_H */
