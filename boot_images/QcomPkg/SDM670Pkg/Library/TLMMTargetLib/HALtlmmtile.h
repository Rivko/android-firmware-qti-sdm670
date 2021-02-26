#ifndef HAL_TLMMTILE_H
#define HAL_TLMMTILE_H
/*
===========================================================================

FILE:         HALtlmmtile.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the tlmm csr block.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/TLMMTargetLib/HALtlmmtile.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/01/15   cpaulo  Created.

===========================================================================
        Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */

#include <HALcomdef.h>


/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

#define HALTLMMTILE_ERR     ( uint32 )( -1 )
#define HALTLMMTILE_SUCCESS ( uint32 )(  0 )

typedef enum
{
  HAL_TLMMTILE_REG_CFG = 0,
  HAL_TLMMTILE_REG_INOUT,
  HAL_TLMMTILE_REG_INTRCFG,
  HAL_TLMMTILE_REG_INTRSTATUS,
  HAL_TLMMTILE_REG_LP_CFG,
  HAL_TLMMTILE_REG_MAX
}
HAL_tlmmtile_RegType;

typedef enum
{
  HAL_TLMMTILE_FIELD_CFG_PULL = 0,
  HAL_TLMMTILE_FIELD_CFG_FUNC_SEL,
  HAL_TLMMTILE_FIELD_CFG_DRV_STR,
  HAL_TLMMTILE_FIELD_CFG_OUTPUT_ENABLE,
  HAL_TLMMTILE_FIELD_CFG_HIHYS,
  HAL_TLMMTILE_FIELD_INOUT_IN,
  HAL_TLMMTILE_FIELD_INOUT_OUT,
  HAL_TLMMTILE_FIELD_INTRCFG_EN,
  HAL_TLMMTILE_FIELD_INTRCFG_POL,
  HAL_TLMMTILE_FIELD_INTRCFG_DET,
  HAL_TLMMTILE_FIELD_INTRCFG_RAW,
  HAL_TLMMTILE_FIELD_INTRCFG_TARGET,
  HAL_TLMMTILE_FIELD_INTRCFG_DIRCONNEN,
  HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS,
  HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_SEL,
  HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_POL,
  HAL_TLMMTILE_FIELD_CFG_LP_OUTVAL,
  HAL_TLMMTILE_FIELD_CFG_LP_VALIDATED,
  HAL_TLMMTILE_FIELD_ALL
}
HAL_tlmmtile_FieldType;

/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

void HAL_tlmmtile_Init            ( void );

uint32 HAL_tlmmtile_Set ( uint32 nGPIO, HAL_tlmmtile_RegType nReg, HAL_tlmmtile_FieldType nField, uint32 nVal );
uint32 HAL_tlmmtile_Get ( uint32 nGPIO, HAL_tlmmtile_RegType nReg, HAL_tlmmtile_FieldType nField );

uint32 HAL_tlmmtile_SetDirConn ( uint32 nDirConn, HAL_tlmmtile_FieldType nField, uint32 nVal );
uint32 HAL_tlmmtile_GetDirConn ( uint32 nDirConn, HAL_tlmmtile_FieldType nField );

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_tlmmtile_Init( )

Description:
   This function is used to initialize the TLMM tile HAL interface structure.

===============================================================================

HAL_tlmmtile_Set( )

Description:
   This function is used to set the value of a field in the TLMM tile HAL.
   This function will simply write a field in the TLMM CSR based on the
   register passed in as well as the field.

Parameters:
   nGPIO  - GPIO number to access
   nReg   - Register in the TLMM CSR for the GPIO listed above
   nField - Field to access, use HAL_TLMMTILE_FIELD_ALL for full register contents
   nVal   - Value to set in the field specified above

Return:
   uint32 - HALTLMMTILE_SUCCESS for successful write.
            HALTLMMTILE_ERR if there is an issue with the params.
 
===============================================================================

HAL_tlmmtile_Get( )

Description:
   This function is used to get the value of a field in the TLMM tile HAL.
   This function will simply read a field in the TLMM CSR based on the
   register passed in as well as the field.

Parameters:
   nGPIO  - GPIO number to access
   nReg   - Register in the TLMM CSR for the GPIO listed above
   nField - Field to access, use HAL_TLMMTILE_FIELD_ALL for full register contents

Return:
   uint32 - Value of the register at the field passed in.
            Returns HALTLMMTILE_ERR if there is an issue with the params.
 
===============================================================================
*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_TLMMTILE_H */
