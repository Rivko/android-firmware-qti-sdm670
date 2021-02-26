#ifndef __HALVCS_H__
#define __HALVCS_H__
/*
==============================================================================

FILE:         HALcpu.h

DESCRIPTION:
  Common includes for the LDO HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/hal/inc/HALvcs.h#1 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALcomdef.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * Forward declaration of structs b/c of the cross references within these
 * structs
 */
typedef struct HAL_vcs_LDODescType    HAL_vcs_LDODescType;
typedef struct HAL_vcs_CoprocDescType HAL_vcs_CoprocDescType;


/*
 * HAL_vcs_Context
 *
 * HAL ldo driver context data.
 *
 *  BusyWait     - Pointer to busy wait function.
 *                 Assumption: Will be implemented or passed down by the
 *                 top level driver. If this function pointer is NULL then
 *                 HAL modules that require a wait/pause may fail - this
 *                 will be functional failure and not compile time failure.
 *
 *  nChipVersion - Version of the chip. Compare against 
 *                 CHIP_VERSION(major, minor).
 *  nChipId      - ID of the chip.
 *  nChipFamily  - Family of the chip.
 */
typedef struct
{
  void (*BusyWait) (uint32 uSeconds);
  uint32 nChipVersion;
  uint32 nChipId;
  uint32 nChipFamily;
} HAL_vcs_ContextType;


/*
 * HAL_vcs_RegisterSettingType
 *
 * nAddress - Address of register.
 * nValue   - Setting value.
 *
 * nBaseAddr - Base address of registers for this LDO.
 */
typedef struct
{
  uint32 nAddr;
  uint32 nValue;
} HAL_vcs_RegisterSettingType;


/*
 * HAL_vcs_LDOConfigType
 *
 * Parameters used for configuring an LDO.
 *
 *  pSettings    - Set of configuration register/value pairs.
 *  nNumSettings - Length of pSettings array.
 */
typedef struct HAL_vcs_LDOConfigType
{
  HAL_vcs_RegisterSettingType *pSettings;
  uint32                       nNumSettings;
} HAL_vcs_LDOConfigType;


/*============================================================================

               FUNCTION DECLARATIONS FOR MODULE

============================================================================*/


/* ============================================================================
**    Mandatory Functions
** ==========================================================================*/


/*=========================================================================
**  Function : HAL_vcs_Init
** =========================================================================*/
/**
  Initialize the LDO HAL. 

  @param pHALCtxt [in] HAL LDO context.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_Init
(
  HAL_vcs_ContextType *pHALCtxt
);


/*=========================================================================
**  Function : HAL_vcs_ConfigLDO
** =========================================================================*/
/**
  Program a LDO's config registers.

  @param pDesc [in] Pointer to HAL desc of LDO.
  @param pConfig [in] LDO configuration.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_ConfigLDO
(
  HAL_vcs_LDODescType   *pDesc,
  HAL_vcs_LDOConfigType *pConfig
);


/*=========================================================================
**  Function : HAL_vcs_EnableLDO
** =========================================================================*/
/**
  Enable LDO for the specified CPU.

  @param pDesc [in] Pointer to HAL desc of LDO.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_EnableLDO
(
  HAL_vcs_LDODescType *pDesc
);


/*=========================================================================
**  Function : HAL_vcs_DisableLDO
** =========================================================================*/
/**
  Disable LDO for the specified CPU.

  @param pDesc [in] Pointer to HAL desc of LDO.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_DisableLDO
(
  HAL_vcs_LDODescType *pDesc
);


/*=========================================================================
**  Function : HAL_vcs_IsLDOEnabled
** =========================================================================*/
/**
  Read HW status to determine whether LDO is enabled.

  @param pDesc [in] Pointer to HAL desc of LDO.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_IsLDOEnabled
(
  HAL_vcs_LDODescType *pDesc
);


/*=========================================================================
**  Function : HAL_vcs_SetLDOVoltage
** =========================================================================*/
/**
  Set voltage for the specified LDO.

  @param pDesc [in] Pointer to HAL desc of LDO.
  @param nVoltageUV [in] Voltage in uV.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_SetLDOVoltage
(
  HAL_vcs_LDODescType *pDesc,
  uint32               nVoltageUV
);


/*=========================================================================
**  Function : HAL_vcs_GetLDOVoltage
** =========================================================================*/
/**
  Get voltage for the specified LDO.

  @param pDesc [in] Pointer to HAL desc of LDO.
  @param pVoltageUV [in] Pointer to address where uV voltage should be stored.
                         If NULL, then no uV value returned.
  @param pVoltageHW [in] Pointer to address where HW voltage should be stored.
                         If NULL, then no HW value returned.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_GetLDOVoltage
(
  HAL_vcs_LDODescType *pDesc,
  uint32              *pVoltageUV,
  uint32              *pVoltageHW
);


/*=========================================================================
**  Function : HAL_vcs_EnableCoproc
** =========================================================================*/
/**
  Turn on power to the coprocessor HW modules specified in the modules mask.

  @param pDesc [in] Pointer to HAL desc of coproc.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_EnableCoproc
(
  HAL_vcs_CoprocDescType *pDesc
);


/*=========================================================================
**  Function : HAL_vcs_DisableCoproc
** =========================================================================*/
/**
  Turn on power to the coprocessor HW modules specified in the modules mask.

  @param pDesc [in] Pointer to HAL desc of coproc.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_DisableCoproc
(
  HAL_vcs_CoprocDescType *pDesc
);


/*=========================================================================
**  Function : HAL_vcs_IsCoprocEnabled
** =========================================================================*/
/**
  Read HW status to determine whether coproc is enabled.

  @param pDesc [in] Pointer to HAL desc of coproc.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_vcs_IsCoprocEnabled
(
  HAL_vcs_CoprocDescType *pDesc
);


/*=========================================================================
**  Function : HAL_vcs_GetChipVersion
** =========================================================================*/
/**
  Return the chip version.

  @return 
  Chip version.

  @dependencies
  None.
*/

uint32 HAL_vcs_GetChipVersion
(
  void
);


/*=========================================================================
**  Function : HAL_vcs_GetChipId
** =========================================================================*/
/**
  Return the chip ID.

  @return 
  Chip ID.

  @dependencies
  None.
*/

uint32 HAL_vcs_GetChipId
(
  void
);


/*=========================================================================
**  Function : HAL_vcs_GetChipFamily
** =========================================================================*/
/**
  Return the chip family.

  @return 
  Chip family.

  @dependencies
  None.
*/

uint32 HAL_vcs_GetChipFamily
(
  void
);


#endif  /* __HALVCS_H__ */ 

