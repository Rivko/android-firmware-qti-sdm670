/*=============================================================================

                       Common Boot A53 Handler

GENERAL DESCRIPTION
  This file contains definitions of functions for A53 specific routines.

Copyright 2010-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   kpa     Changed boot_pm_ps_hold_cfg() API to boot_pm_app_pon_pshold_cfg() to match pmic API name
03/03/15   kpa     Correctly configure boot_pm_pon_ps_hold_cfg for hard reset.
02/12/15   ck      Removed boot_switch_to_aarch64
01/07/15   aab     Updated boot_pm_pon_ps_hold_cfg() API call
10/29/14   kpa     Support 64 bit memory map
03/18/14   ck      Removed boot_hw_warm_reset as it is redundant
03/18/14   ck      boot_hw_reset now takes a parameter specifying what kind of reset to perform
02/05/14   ck      Added A53 logic to switch from 32 to 64 bit mode.
12/06/13   ck      Removed GCC_WDOG_DEBUG as this does not exist on A53
12/06/13   ck      Initial creation from boot_krait.c, preserving change log
07/31/13   dh      backed out pmic hard reset per windows request
07/24/13   aus     Added boot_hw_warm_reset
07/09/13   dh      Perform hard pmic reset in boot_hw_reset if sbl is in dload mode  
04/02/13   dh      Add boot_pm_pon_ps_hold_cfg to properly configure reset and shutdown 
01/22/13   dh      Disable watchdog debug feature in boot_hw_reset
12/10/12   plc     Fix Klockwork Errors
10/25/12   dh      Remove pragma to surpass inline assemly since armct5.01 allows it
09/19/12   dh      enable ps hold reset
08/08/12   dh      use self loop in boot_hw_reset ps hold doesn't work
06/26/12   dh      use ps hold in boot_hw_reset
08/05/11   kpa     Relocated boot_hw_powerdown() here from boothw_target.c.
05/19/11   kpa     1. Remove inclusion of boot_scorpion_mmu.h, code common to
                   boot_page_table.c. 
                   2. Check the jtag debug disable fuse before output to jtag
                   terminal to avoid crash
5/11/11    plc     Change to include boot_page_table_armv7.h
01/25/11   dh      fixed l2 page table overflow issue
09/10/10   aus     Enabling data cache and MMU in boot
07/22/10   aus     Intial creation.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_msm.h"
#include "boot_target.h"
#include "boot_extern_busywait_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_dload.h"
#include "boothw_target.h"

/**
 * Infinite loop
 */
#define INFINITE_LOOP() while(1)

/**
 * Max number of retries before giving up to output to jtag terminal
 */
#define JTAG_TERM_MAX_NUM_RETRY  5

/**
 * Time out value for outputing a single char to jtag terminal
 */
#define JTAG_TERM_TIME_OUT       0x10000

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_clk_busy_wait

** ==========================================================================
*/
/*!
* 
* @brief
*   This function implements the clock busy wait.  
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_clk_busy_wait(uint32 delay)
{
  boot_busywait(delay);
} /* boot_clk_busy_wait */


/*===========================================================================

**  Function :  boot_hw_reset

** ==========================================================================
*/
/*!
* 
* @brief
*   This function resets the A53 processor.  Parameter specifies what kind
*   of reset to perform.
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   Target is reset
* 
*/
void boot_hw_reset(boot_hw_reset_type reset_type)
{
  /* Set the PMIC to do the requested type of reset when PS_HOLD is asserted */
  switch (reset_type)
  {
  case BOOT_WARM_RESET_TYPE:
    /* Configure PS_HOLD to do warm reset */
    boot_pm_app_pon_pshold_cfg(PM_APP_PON_CFG_WARM_RESET);
    break;
	
  case BOOT_DVDD_HARD_RESET_TYPE:
    /* Configure PS_HOLD to do dvdd hard reset */
    boot_pm_app_pon_pshold_cfg(PM_APP_PON_CFG_DVDD_HARD_RESET); 
    break;

  case BOOT_HARD_RESET_TYPE:
  default:
    /* Configure PS_HOLD to do hard reset.  This is also the default type.*/
    boot_pm_app_pon_pshold_cfg(PM_APP_PON_CFG_HARD_RESET);
    break;
  }


  /* pull down pmic PS_HOLD */
  HWIO_OUTF(MPM_PS_HOLD, PSHOLD, 0x0 );


  /* Loop forever */
  INFINITE_LOOP();
} /* boot_hw_reset */


/*===========================================================================

**  Function :  boot_hw_powerdown

** ==========================================================================
*/
/*!
* 
* @brief
*   This function powers off the PMIC
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   Target is switched off
* 
*/
void boot_hw_powerdown
(
  void
)
{
  /* Configure PS_HOLD to shutdown */
  boot_pm_app_pon_pshold_cfg(PM_APP_PON_CFG_NORMAL_SHUTDOWN);
  
  /* pull down pmic PS_HOLD */
  HWIO_OUTF(MPM_PS_HOLD, PSHOLD, 0x0 );

  /* Loop forever */
  INFINITE_LOOP();
} /* boot_hw_powerdown */

/*===========================================================================

**  Function :  boot_set_pwrkey_reset_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the power key reset to hardreset
*   or warmreset as per the provided argument
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_set_pwrkey_reset_type(boot_hw_reset_type reset_type)
{
  switch (reset_type)
  {
  case BOOT_WARM_RESET_TYPE:
    //PON KPDPWR PON Reset configuration to WARM_RESET
    pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_KPDPWR, PM_APP_PON_CFG_WARM_RESET, 10256, 2000); 
    break;

  case BOOT_HARD_RESET_TYPE:
  default:
    //PON KPDPWR PON Reset configuration to HARD_RESET
    pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_KPDPWR, PM_APP_PON_CFG_HARD_RESET, 10256, 2000); 
    break;
  } 
} /* boot_set_pwrkey_to_hardreset */

/*===========================================================================

**  Function :  boot_set_pshold_reset_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the pshold reset to hardreset
*   or warmreset as per the provided argument
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_set_pshold_reset_type(boot_hw_reset_type reset_type)
{
  switch (reset_type)
  {
  case BOOT_WARM_RESET_TYPE:
    /* Configure PS_HOLD to do warm reset */
    boot_pm_app_pon_pshold_cfg(PM_APP_PON_CFG_WARM_RESET);
    break;

  case BOOT_HARD_RESET_TYPE:
  default:
    /* Configure PS_HOLD to do hard reset.  This is also the default type.*/
    boot_pm_app_pon_pshold_cfg(PM_APP_PON_CFG_HARD_RESET);
    break;
  }
} /* boot_set_pshold_reset_type */

/*===========================================================================

**  Function :  boot_jtag_term_putc

** ==========================================================================
*/
/*!
* 
* @brief
* Pushes a single character to the JTAG terminal window.
* It checks to make sure the previous character has been read by the terminal.
* If no JTAG terminal is open then it will skip any remaining output.  
* If it timeouts several times then it will never attempt
* to write out to JTAG again.
* This function only works on ARM11 and Scorpion
*
* @param[in] character single character to display.
*
* @par Dependencies
*   None
*   
* @retval
*   None
*
* @par Side Effects
*   None
* 
*/
void boot_jtag_term_putc(char character) 
{
//TODO: _FIX_ for 8996 : inline assembly not allowed
#if 0
  uint32 write_status;
  uint32 flag;
  uint32 timeout_count;

  /*Used to determine if writing to JTAG should even be attempted.  If 
    writing to the terminal fails JTAG_TERM_MAX_NUM_RETRY times,
    then it won't attempt anymore.*/
  static uint32 timeout_final = JTAG_TERM_MAX_NUM_RETRY;
  
  /*This section of assembly will output the given character to a JTAG 
    terminal If the terminal is not up then it will skip outputting.  If it
    misses writing to the terminal twice, then it will never attempt to write 
    to the terminal again.                                                 */
  /* First we check to see if any apps processor jtag debug disable fuse is blown, 
    if there is then we skip the jtag output
    When debug is disabled, accessing jtag register will likely result a crash*/
      
  if(!(DEBUG_DISABLED_APPS))
  {
    if(0 != timeout_final)
    {
      __asm 
      {
        mov  flag, #0x20000000                    /*The writeable flag is bit 29*/
        mov  timeout_count, #JTAG_TERM_TIME_OUT	   /*Used to watch for timeout*/
        loop:
          cmp     timeout_count, #0               
          beq     timeout                         /*Timed out, skip this output*/
          sub     timeout_count, timeout_count, #1   /*Decrement timeout_count*/
          mrc     p14, 0, write_status, c0, c1, 0       /*Grab writeable flag*/
          ands    write_status, write_status, flag   /*Check if writeable*/
          bne     loop                           /*Loop if not writeable*/
          mcr     p14, 0, character, c0, c5, 0      /*write to terminal*/
        b end
        timeout:
          sub timeout_final, timeout_final, #1   /*record timeout*/
        end:
      }
    } 
  }
#endif  
}
