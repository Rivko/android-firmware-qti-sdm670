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

#ifndef _SYSDBG_QDSS_H
#define _SYSDBG_QDSS_H

#define ETM_REG_OFFSET      0x100000 
#define ETM_REG_BLOCK_SIZE  0x1000
#define TMC_REG_BLOCK_SIZE  0x1000
#define ETB_SIZE            0x10000

/* Format Revision Numbers for the various QDSS debug features */
#define ETB_DUMP_FORMAT_VERSION   0x11
#define ETM_DUMP_FORMAT_VERSION   0x11
#define TMC_DUMP_FORMAT_VERSION   0x11


/* ETB Dump type */
typedef struct
{
    uint32 entries[ETB_SIZE];
}sysdbg_etb_type;

/* ETM Register dump type */
typedef struct
{
    uint64 entries[ETM_REG_BLOCK_SIZE];
}sysdbg_etm_type;

/* TMC Register dump type */
typedef struct
{
    uint32 entries[TMC_REG_BLOCK_SIZE];
}sysdbg_tmc_type;


/*===========================================================================
** 	Function: SysdbgFlushTrace
** ==========================================================================
*/
/*!
* 
* @brief
*	Flush the ETF and ETR traces out to the sinks 
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

boolean SysdbgFlushTrace(void);

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

boolean SysdbgDumpETB(void);

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

boolean SysdbgDumpETM(void);

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

boolean SysdbgDumpTMC(uint32 block);

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
boolean SysdbgDumpTrace(uint32);


#endif /* _SYSDBG_QDSS_H */
