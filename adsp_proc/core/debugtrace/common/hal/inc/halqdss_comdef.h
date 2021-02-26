#ifndef HALQDSS_COMDEF_H
#define HALQDSS_COMDEF_H
/*
==============================================================================

FILE:         halqdss_comdef.h

DESCRIPTION:  

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtrace/common/hal/inc/halqdss_comdef.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/15/08   gfr     Added HAL_ENUM_32BITS macro.
02/14/08   gfr     Added bool32 type.
11/13/07   gfr     Removed dependency on comdef.h
01/08/07   hxw     Created 

==============================================================================
             Copyright © 2006 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/



/*============================================================================
                          INCLUDE FILES
============================================================================*/
#include "HALcomdef.h"
/*============================================================================
                         CONSTANT DEFINITIONS
============================================================================*/

/*
 *Boolean High
 */
#define HIGH 1

/*
 *Boolean Low
 */
#define LOW  0 


/*
 * Macro to allow forcing an enum to 32 bits.  The argument should be
 * an identifier in the namespace of the enumeration in question, i.e.
 * for the clk HAL we might use HAL_ENUM_32BITS(CLK_xxx).
 */
#define HAL_ENUM_32BITS(x) HAL_##x##_FORCE32BITS = 0x7FFFFFFF


typedef enum
{
   QDSS_COMP_STM,
   QDSS_COMP_ETM,
   QDSS_COMP_ETB,
   HAL_ENUM_32BITS(QDSS_COMP)
}HAL_qdss_CoreSightComponentType;

typedef enum
{
   QDSS_SUCCESS=0,
   QDSS_INPUT_ERR,
   QDSS_FEAT_NOT_IMPLEMENTED,
   QDSS_COMPONENT_BUSY,
   HAL_ENUM_32BITS(QDSS_RETCODE)
}HAL_qdss_retcode;

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/



/*a: array
  n:integer value
  b:boolean
  fp:function ptr
  m:mask
  s:shift value
  e:enum
  p:pointer
  u:union
  st:structure
  */

/*============================================================================
                          GLOBAL VARIABLES
============================================================================*/


/*============================================================================
                          PUBLIC FUNCTION
============================================================================*/

#endif /* HAL_COMDEF_H */

