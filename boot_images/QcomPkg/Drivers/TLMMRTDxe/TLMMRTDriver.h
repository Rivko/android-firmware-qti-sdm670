/** @file TLMMRTDriver.h
   
  This file defines TLMM driver APIs for non-DAL environments. 

  Copyright (c) 2011-2015, Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/02/11   dcf     Created.

=============================================================================*/


#ifndef __TLMMRTDRIVER_H__
#define __TLMMRTDRIVER_H__


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"

/*
 * TLMMControlType
 *
 * Set of function pointers used to configure GPIO pins.
 *
 * ConfigGpios  - Configure a group of GPIO pins.
 */
typedef struct
{
  boolean (*ConfigGpios) ( uint32* paConfigs, uint32 nSize );

} TLMMControlType;




/*
 * TLMM_HWIOBaseType
 *
 * Container for a HWIO base address.  This is used by the driver to
 * map the required physical bases to the provided base pointer.
 *
 *  nPhysAddr  - Physical base address of the base.
 *  nSize      - Size of the memory region in bytes.
 *  pnBaseAddr - Pointer to location for the driver to fill in with
 *               the virtual address.
 */
typedef struct
{
  uint32  nPhysAddr;
  uint32  nSize;
  uint32 *pnBaseAddr;
} TLMM_HWIOBaseType;


/*=========================================================================
      Function Prototypes
==========================================================================*/


/* =========================================================================
**  Function : TLMM_GetHWIOBases
** =========================================================================*/
/** 
  Gets the pointer to the array of HWIO bases.

  @param mHWIOBases   [in]  Array of HWIO bases.

  @return
  None.

  @dependencies
  None.
*/

void TLMM_GetHWIOBases(TLMM_HWIOBaseType **paHWIOBases);


/* =========================================================================
**  Function : TLMM_EnableSource
** =========================================================================*/
/** 
  Enables source.

  @param eSource   [in]  Source to enable.

  @return
  TRUE  -- Source enabled successfully. \n
  FALSE -- Source was not enabled.

  @dependencies
  None.
*/

boolean TLMM_ConfigGpios(uint32 *paConfigs, uint32 nSize);

boolean TLMM_DriverInit(void);

boolean TLMM_ReadGpio(uint32 nConfig, uint32* nValue);

boolean TLMM_WriteGpio(uint32 nConfig, uint32 nValue);

boolean TLMM_DriverInit (void);

boolean TLMM_ConfigGpios(uint32 *paConfigs, uint32 nSize);


#endif /* __TLMMRTDRIVER_H__ */

