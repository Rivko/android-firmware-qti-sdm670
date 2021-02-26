/*=============================================================================
                        System Debug Target Module 

GENERAL DESCRIPTION     
		  This module handles the target specific system debug functionalities.
  
	Copyright 2012- 2014 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

09/14/14    AJCheriyan  Refactored for HoneyBadger

===========================================================================*/

#ifndef _SYSDBG_TARGET_H
#define _SYSDBG_TARGET_H


#include "com_dtypes.h"

/* Device specific Configuration information */
typedef struct
{
    /* Reset debug global flag */
    const uint32 reset_dbgen;
    /* PMIC Resin Debug Enable */
    const uint32 pmic_resin_dbgen ;
    /* Temperature sensor Reset debug */
    const uint32 tsense_dbgen;
    /* Watchdog Reset Debug */
    const uint32 wdog_dbgen;
    /* Configuration controls within
     * first pass */
    /* Access Control reset */
    const uint32 acc_ctl_rst;
    /* Debug Logic reset */
    const uint32 dbg_ctl_rst;

    /* Debug Image timeouts */
    /* MSM level timeouts */
    const uint32 timeout1_en;
    const uint32 timeout1;
    /* PMIC level timeouts - bark and bite */
    const uint32 timeout23_en;
    const uint32 timeout2;
    const uint32 timeout3;
    /* MSM level delays for various blocks */
    const uint32 wind_down_dly;
    const uint32 save_data_dly;
    const uint32 stop_trace_dly;
    const uint32 prep_ares_dly;
}sysdbg_config_type;





/*===========================================================================
**  Function :  SYSDBG_COLD_BOOT_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* The function performs any init of the hardware before enabling debug features
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Calls target specific enablement function
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/

boolean SysdbgColdBootInit(void);




/*===========================================================================
**  Function :  SYSDBG_TARGET_ENABLE
** ==========================================================================
*/
/*!
* 
* @brief
* The function that enables the reset debug feature
* 
* @param[in] 
*   System debug entry point for the start of the SW reset debug flow
*
* @par Dependencies
*   Calls target specific enablement function
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/

boolean SysdbgTargetEnable(void *);


/*============================================================================
**   Function : SYSDBG_GET_SHARED_IMEM
**==========================================================================*/

/*!
* 
* @brief
* The function that retrieves the shared IMEM data structure
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Calls target specific enablement function
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/
void *SysdbgGetSharedImemBase(void);


/*============================================================================
**   Function : SYSDBG_MEMCONFIG
**==========================================================================*/

/*!
* 
* @brief
* The function that retrieves the memory configuration data
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Target data structure should be defined
* 
* @retval
*   Pointer to the memory configuration data
* 
* @par Side Effects
*   None
* 
*/
const void* Sysdbg_MemConfig(void);

/*============================================================================
**   Function : SYSDBG_CONFIG
**==========================================================================*/

/*!
* 
* @brief
* The function that retrieves the general system debug configuration data
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Target data structure should be defined
* 
* @retval
*   Pointer to the general configuration data
* 
* @par Side Effects
*   None
* 
*/
const void* Sysdbg_Config(void);


/*============================================================================
**   Function : SYSDBG_PASSCONFIG
**==========================================================================*/

/*!
* 
* @brief
* The function that retrieves the general reset pass configuration data
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Target data structure should be defined
* 
* @retval
*   Pointer to the general configuration data
* 
* @par Side Effects
*   None
* 
*/
const void* Sysdbg_PassConfig(void);

/*============================================================================
**   Function : HydraSRBFormat
**==========================================================================*/

/*!
* 
* @brief
* The function that retrieves format information of the buffer for Hydra CPUs
* 
* @param[in] 
*   None
*
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the Hydra SRB format data
* 
* @par Side Effects
*   None
* 
*/
const void *HydraSRBFormat(void);

/*============================================================================
**   Function : ARMSRBFormat 
**==========================================================================*/

/*!
* 
* @brief
* The function that retrieves format information of the buffer for ARM CPUs. 
* Note that, ARM CPUs do not have this. However, this abstracts the register
* save routine format from the final DDR CPU context format and keeps it 
* consistent across various ARMv8 CPUs
* 
* @param[in] 
*   None
*
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the ARM SRB format data
* 
* @par Side Effects
*   None
* 
*/
const void *ARMSRBFormat(void);

/*============================================================================
**   Function : SYSDBG_RESET_CORE
**==========================================================================*/

/*!
* 
* @brief
* Target specific function which resets the next core
* 
* @param[in] 
*   Subsystem name, current cluster, current active core.
*
* @par Dependencies
*   Calls target specific enablement function. It finds the next core
*   to be reset and L2 as well if needed. 
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/
boolean SysdbgTargetResetCore(uint32 cluster, uint32 core);

/*===========================================================================
** 	Function: SysdbgTargetClockInit
** ==========================================================================
*/
/*!
* 
* @brief
* 	Initialize the clock branches for the debug image
*
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
boolean SysdbgTargetClockInit(void);

/*===========================================================================
** 	Function: SysdbgTargetQDSSInit
** ==========================================================================
*/
/*!
* 
* @brief
* 	Initialize the QDSS related features for reset debug use
*
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
boolean SysdbgTargetQDSSInit(void);

/*===========================================================================
** 	Function: SysdbgTargetDumpETB
** ==========================================================================
*/
/*!
* 
* @brief
*	Save the entire ETB 
*
* @param[in] 
*	Pointer to the buffer to save ETB data
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

boolean SysdbgTargetDumpETB(uint32 *buffer);

/*===========================================================================
** 	Function: SysdbgTargetDumpETM
** ==========================================================================
*/
/*!
* 
* @brief
*	Save the entire ETM register space for  
*
* @param[in] 
*	Pointer to the buffer to save ETB data
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

boolean SysdbgTargetDumpETM(uint64 *buffer);

/*===========================================================================
** 	Function: SysdbgTargetDumpETM
** ==========================================================================
*/
/*!
* 
* @brief
*	Save the entire ETM register space for  
*
* @param[in] 
*	Pointer to the buffer to save ETB data
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

boolean SysdbgTargetDumpTMC(uint32 *buffer, uint32 block);

/*===========================================================================
**  Function :  SysdbgGetChipInfo(void)
** ==========================================================================
*/
/*!
* 
* @brief
* 	Returns chipset information details
* 
* @param[in] 
* 	Pointer to chipinfo struct
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*/

void SysdbgGetChipInfo(void *chipinfo);


#endif /* _SYSDBG_TARGET_H */
