/*=============================================================================
                        S Y S D B G    Q D S S   M O D U L E  

GENERAL DESCRIPTION     
  This module declares all functions related to qdss block operations
  
	Copyright 2014 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---         --------------------------------------------------
01/08/2014 AJCheriyan   New for HoneyBadger
===========================================================================*/

#include "sysdbg.h"
#include "sysdbg_qdss.h"
#include "stringl/stringl.h"
#include "sysdbg_target.h"
#include "sysdbg_system.h"
#include "LoaderUtils.h"



/*===========================================================================
** 	Function: SysdbgDumpETB
** ==========================================================================
*/
/*!
* 
* @brief
*	Save the entire ETB to the DDR buffer 
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*	Needs to be done early in init
*/

boolean SysdbgDumpETB(void)
{
    sysdbg_etb_type *databuffer;
    dump_data_type *buffer = sysdbg_get_buffer(get_current_subsys(), ETB_DUMP, 0); 

    /* Sanity check */
    if (buffer)
    {
        if (!buffer->magic)
        {
            /* Get out databuffer */
            databuffer = (sysdbg_etb_type *)buffer->start_addr;
            
            if (!databuffer)
                /* Log an error */
                return FALSE;

            if (SysdbgTargetDumpETB(databuffer->entries))
            {
                /* Write out the magic number and other details */
                buffer->magic = SYSDBG_MAGIC_NUMBER;
                buffer->version = ETB_DUMP_FORMAT_VERSION;
                memscpy(&buffer->name, sizeof(SYSDBG_NAME), SYSDBG_NAME, sizeof(SYSDBG_NAME));
            }
        }
    }

    return TRUE;
}


/*===========================================================================
** 	Function: SysdbgDumpETM
** ==========================================================================
*/
/*!
* 
* @brief
*	Save the entire ETM register to the DDR buffer 
*
* @param[in] 
*	Buffer location
*  
* @par Dependencies
*	None
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*	Needs to be done early in init
*/

boolean SysdbgDumpETM(void)
{
    sysdbg_etm_type *databuffer = NULL;
    uint32 cpu = 0;

    dump_data_type *buffer = sysdbg_get_buffer(get_current_subsys(), ETM_DUMP, cpu);
    
    /* Sanity check */
    if (buffer)
    {
        if (!buffer->magic)
        {
            /* Get out databuffer */
            databuffer = (sysdbg_etm_type *)buffer->start_addr;
            
            if (!databuffer)
                /* Log an error */
                return FALSE;
            else
            {

                if (SysdbgTargetDumpETM(databuffer->entries))
                {

                /* Now write out the magic numbers to the header */
                    buffer->magic = SYSDBG_MAGIC_NUMBER;
                    buffer->version = ETM_DUMP_FORMAT_VERSION;
                    memscpy(&buffer->name, sizeof(SYSDBG_NAME), SYSDBG_NAME, sizeof(SYSDBG_NAME));
                }
            }
        }
    }

    return TRUE;
}

/*===========================================================================
** 	Function: SysdbgDumpTMC
** ==========================================================================
*/
/*!
* 
* @brief
*	Save the entire TMC register set to the DDR buffer for a specified TMC
*
* @param[in] 
*	Buffer location, tmc base address
*  
* @par Dependencies
*	None
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*	Needs to be done early in init
*/

boolean SysdbgDumpTMC(uint32 block)
{
#define TMC_RRD 0x4
#define TMC_RWD 0x9

    sysdbg_tmc_type *databuffer;

    dump_data_type *buffer = sysdbg_get_buffer(get_current_subsys(), TMC_DUMP, block);


    /* Sanity check */
    if (buffer)
    {
        if (!buffer->magic)
        {
            /* Get out databuffer */
            databuffer = (sysdbg_tmc_type *)buffer->start_addr;
            
            if (!databuffer)
                /* Log an error */
                return FALSE;
            else
            {
                if (SysdbgTargetDumpTMC(databuffer->entries, block))
                {
                    /* Write out the version and other info out */
                    buffer->magic = SYSDBG_MAGIC_NUMBER;
                    buffer->version = TMC_DUMP_FORMAT_VERSION;
                    memscpy(&buffer->name, sizeof(SYSDBG_NAME), SYSDBG_NAME, sizeof(SYSDBG_NAME));
                }
            }

        }
    }

    return TRUE;
}

/*===========================================================================
** 	Function: SysdbgDumpTrace
** ==========================================================================
*/
/*!
* 
* @brief
*	Single wrapper for all QDSS related features
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*	None
*/
boolean SysdbgDumpTrace(uint32 subsys)
{
    boolean success = TRUE;

    /* ETB save */
    success &= SysdbgDumpETB();

    /* ETM save */
    success &= SysdbgDumpETM();

    /* TMC save */
    success &= SysdbgDumpTMC(1);
    success &= SysdbgDumpTMC(0);

    return success;
}




