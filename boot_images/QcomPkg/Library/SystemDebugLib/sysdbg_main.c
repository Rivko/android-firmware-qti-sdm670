/*=============================================================================
                        System Debug Main Module 

GENERAL DESCRIPTION     
          This module handles the main entry point of the system debug functionalities.
  
    Copyright 2012- 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

04/06/14    AJCheriyan  Added log messages and refactored reset
09/14/14    AJCheriyan  Refactored for HoneyBadger
01/13/14    davidai     Ported for Bear
07/02/13    AJCheriyan  Updated for Gandalf
01/28/13    AJCheriyan  Added BOOT_PARTITION_SELECT write for v2
01/15/13    AJCheriyan  Moved reset apps core and debug feature enablement here
02/22/12    AJCheriyan  First cut for Aragorn. First ever SYSDBG
===========================================================================*/

#include "comdef.h"
#include "sysdbg_main.h"
#include "sysdbg_target.h"
#include "sysdbg_util.h"
#include "sysdbg_log.h"
#include "sysdbg_asm.h"
#include "sysdbg_mem_dump.h"
#include "sysdbg_system.h"
#include "sysdbg_util.h"
#include "sysdbg_err.h"
#include "sysdbg_asm.h"
#include "sysdbg_sysreg.h"



/* Our main asm handler */
extern void system_debug_reset_handler(void);



/*===========================================================================
**  Function : SYSDBG_EXECUTE_PASS
** ==========================================================================
*/
/*!
* 
* @brief
*   This function is the workhorse for the image. It iteratively scans through 
*   the pass structure and executes every function in there. The functions are
*   classified as init, work and exit functions respectively. Init and exit functions
*   don't return values while work functions return a success / fail value. Every
*   function must be capable of being run on all cores. If you need to distinguish
*   between a particular core, handle it within the function.
*
* @param[in] 
*   The pointer to the SYSDBG pass data structure, pass_count j
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   Should be capable of handling multi-core situations as needed.
*
*
*/

static void sysdbg_execute_pass(const sysdbg_pass_data_type *sysdbg_pass_ptr)
{

    boolean pass = TRUE;
    uint32 i = 0, current_cpu = 0;
    subsys_type subsys = get_current_subsys();
    current_cpu = get_current_cpu_seq();

    const sysdbg_pass_sequence_type *sysdbg_seq_entry;

    if (sysdbg_pass_ptr)
    {
        // Log the entry
        SYSDBG_LOG_EVENT(PASS_ENTER, __current_pc(), __current_sp(), current_cpu);

        // We have our entry from the table. Now assign the pointer
        sysdbg_seq_entry = &sysdbg_pass_ptr->pass_sequence;
        
        do 
        {
            /* Now call all the functions that actually get something
             done if 1) it is intended for current cpu 2) valid  */
            for (i=0; (i < ARRAY_SIZE(sysdbg_seq_entry->fcn_info) && (pass)); i++)
    
                CALL_IF_VALID_AND_CHECK(pass, (sysdbg_seq_entry->fcn_info[i].cpu_affinity[subsys] & (1 << current_cpu)), sysdbg_seq_entry->fcn_info[i].sysdbg_fcn, subsys);
        
       
        }while(FALSE);
        
        // Log the exit
        SYSDBG_LOG_EVENT(PASS_EXIT, __current_pc(), __current_sp(), current_cpu);
    }
}

/*===========================================================================
**  Function :  SYSTEM_DEBUG_RESET_PASS_2
** ==========================================================================
*/
/*!
* 
* @brief
*   Function which resets the entire SoC back to pass 2 of the reset debug flow
*   
* 
* @param[in] 
*       none
*  
* @par Dependencies
*   lock status is checked to decide whether to reset 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*
*/
void system_debug_exit(uint32 cpu)
{
    /* If final core. Clear out the log entries and then toggle PSHOLD 
     * else. wait for the reset to come through */
   if(cpu == get_global_lock(get_cpu_lock()))
   {
       /* Log the reset attempt */ 
       SYSDBG_LOG_EVENT(GENERIC_EVENT, __current_pc(), __current_sp(), cpu);

       /* Trigger the reset */
       SysdbgResetSoC();
   }
   else
   {
       /* Wait for the other core to complete */
       SYSDBG_LOG_EVENT(ENTER_SLEEP, __current_pc(), __current_sp(), cpu);

       /* Wait for other cores */
       SysdbgWaitForReset();
   }

}

/*===========================================================================
**  Function :  SYSTEM_DEBUG_RESET_ENTRY
** ==========================================================================
*/
/*!
* 
* @brief
*   Reset debug entry point for the driver
*   
* 
* @param[in] 
*       none
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
*
*/
void system_debug_reset_entry(void)
{
    uint32 this_cpu = get_current_cpu_seq();
    uint32 this_subsys = get_current_subsys();

    /* Set the global lock value */
    set_global_lock(get_cpu_lock(), this_cpu);
    set_global_lock(get_subsys_lock(), this_subsys);

    /* If warm boot, execute the warm boot sequence */
    const sysdbg_pass_data_type *sysdbg_pass = Sysdbg_PassConfig(); 
        
    if (sysdbg_pass)
       sysdbg_execute_pass(sysdbg_pass);

    /* All done and exit */
    system_debug_exit(this_cpu);
}

/*===========================================================================
**  Function :  SYSTEM_DEBUG_MAIN
** ==========================================================================
*/
/*!
* 
* @brief
*   Cold boot driver entry point (not needed in the reset path)
*   
* 
* @param[in] 
*       Pass count
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
*
*/


void system_debug_main(void)
{
    boolean status = TRUE;

    /* Enable Logging */
    SysdbgLogInit();

    /* Get our configuration */
    const sysdbg_config_type* sysdbg_config = Sysdbg_Config();

    /* If driver is enabled, proceed */
    if (!sysdbg_config->reset_dbgen)
        return;

    /* Basic HW init */
    status &= SysdbgColdBootInit();

    /* Enable Debug */
    status &= SysdbgTargetEnable(&system_debug_reset_handler);

     /* Collect Target Information */
     status &= SysdbgTargetInfoUpdate();

     if (!status)
        SYSDBG_ERR_FATAL(FATAL_ERROR, __current_pc(), 0, 0 );
}


