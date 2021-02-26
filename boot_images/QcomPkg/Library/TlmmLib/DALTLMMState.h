#ifndef __DALTLMMSTATE_H__
#define __DALTLMMSTATE_H__
/*
============================================================================

                 D A L T L M M   S T A T E   S T R U C T U R E
                             H E A D E R   F I L E

FILE:         DALTLMMState.h

DESCRIPTION:  
              This file contains state information structure definitions for
              each GPIO maintained on a particular platform.  This is a common
              file for use within the DALTLMM driver only.  It must not be
              exported to any other module or driver.

============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/30/10   dcf     Created the initial revision. 
 
============================================================================
             Copyright (c) 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include <DALStdDef.h>
#include <DALGlbCtxt.h>
#include <HALtlmm.h>
#include "DALTLMM.h"



/*==========================================================================

                 STRUCTURES DEFINED IN THIS MODULE

==========================================================================*/


/*
 * The DALTLMMStateAttrType is a basic attribute type for a GPIO.  Each 
 * GPIO will have an attribute structure associated with it which will 
 * provided current state information about a particular GPIO. 
*/
typedef struct
{
  uint8 nActive;     /* States whether the GPIO is currently in use. */
  uint8 nUser;       /* States the current user (or owner) of the GPIO. */
  uint8 nSleepVal;   /* The low-power output value for the particular GPIO. */
  uint8 nDedicated;  /* Set to 1 if the GPIO is dedicated to the nUser. */
}DALTLMMStateAttrType;



/*
 * The DALTLMMStateGPIOType is a structure which defines all attributes and 
 * configuration information pertaining to a particular GPIO.  This structure 
 * is used to access and describe the available low-power configuration, 
 * current confiuration and current attributes of a GPIO. 
*/
typedef struct
{
  HAL_tlmm_GpioType    tInactive; /* The low-power configuration for a GPIO. */
  DALTLMMStateAttrType tAttr;     /* Attributes of a particular GPIO. */

}DALTLMMStateGPIOType;


/*
 * The DALTLMMStateFlagType contains initialization flags which provide 
 * information about whether particular steps in the initialization process 
 * have taken place. 
 */
typedef struct
{
  DALBOOL        bDriverInited;   /* Describes if the driver has been inited. */
  DALBOOL        bSMInited;       /* Describes if shared memory has been inited. */
}DALTLMMStateFlagType;


/* 
 * The DALTLMMStateType defines the main data structure for accessing and
 * controlling GPIO pins.  This data structure will hold information pertaining
 * to every GPIO on a particular target. 
 */ 
typedef struct
{
  DALGLB_HEADER        hdr;             /* Shared memory header, where used. */
  DALTLMMStateFlagType tFlags;
  DALTLMMStateGPIOType ptGPIO[1];

}DALTLMMStateType;




/*==========================================================================

               APPLICATION PROGRAMMING INTERFACE DECLARATIONS

==========================================================================*/


/*==========================================================================

  FUNCTION      DALTLMMState_Init

  DESCRIPTION   Initializes the local DALTLMM state structure including
                allocating space for the GPIO state structure.  When called
                from the boot-loader, the allocation is static.  This API
                is to be called as part of the DALTLMM initialization
                routine.  It is not for exportation.

  PARAMETERS    pCtxt      - Pointer to the client context.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void DALTLMMState_Init(TLMMClientCtxt* pCtxt);


/*==========================================================================

  FUNCTION      DALTLMMState_StructToMacro

  DESCRIPTION   Converts a HAL_tlmm_GpioType configuration to a
                DAL_GPIO_CFG() packed configuration for use with the TLMM
                HAL and to return current configurations.

  PARAMETERS    nGpioNumber - The GPIO number to convert.
                ptState     - A pointer to the GPIO configuration state.

  DEPENDENCIES  None.

  RETURN VALUE  DALGpioSignalType configuration in DAL_GPIO_CFG() format.

  SIDE EFFECTS  None.

==========================================================================*/

DALGpioSignalType DALTLMMState_StructToMacro
(
  uint32 nGpioNumber,
  HAL_tlmm_GpioType* ptState
);


DALGpioSignalType DALTLMMState_StructToMacroLp
(
  uint32 nGpioNumber,
  HAL_tlmm_GpioType* ptState,
  uint32             nOutVal
);

/*==========================================================================

  FUNCTION      DALTLMMState_MacroToStruct

  DESCRIPTION   Converts a DALGpioSignalType configuration to a
                HAL_tlmm_GpioType for storage within the DALTLMM state
                structure.

  PARAMETERS    eGpioConfig - The DAL_GPIO_CFG() configuration to convert.
                ptState     - A pointer to the GPIO configuration state to
                              set.  This is used inside the current GPIO
                              state structure.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void DALTLMMState_MacroToStruct
(
  DALGpioSignalType eGpioConfig,
  HAL_tlmm_GpioType* ptState
);

#endif /* __DALTLMMSTATE_H__ */

