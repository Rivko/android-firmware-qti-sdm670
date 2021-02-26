#ifndef _I2CERROR_H_
#define _I2CERROR_H_
/*
===========================================================================

FILE:   I2cError.h

DESCRIPTION:
    This file contains the result definitions 

===========================================================================

        Edit History

$Header: //components/rel/core.tz/2.0.2/buses/api/i2c/I2cError.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/26/11 LK     Created

===========================================================================
        Copyright c 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "I2cTypes.h"


typedef enum  I2cResult
{
   I2C_RES_SUCCESS =0,

  // Error Classes
   I2C_RES_ERROR_CLS_I2C_CORE            = (int32)0x10000000, /**< I2C core error. */

   I2C_RES_ERROR_CLS_QUP_DEV_TIMEOUT     = (int32)0x20000000, /**< I2C transaction timeout error. */

   I2C_RES_ERROR_CLS_QUP_DEV             = (int32)0x30000000, /**< I2C qup core error. */

   // driver error class
   I2C_RES_ERROR_CLS_I2C_DRV             = (int32)0x40000000, /**< I2C driver error. */

   // Some future classes

   I2C_RES_ERROR_CLS_DEV_PLATFORM       = (int32)0xE0000000,  /**< I2C platform error. */
   I2C_RES_ERROR_CLS_DEV_OS             = (int32)0xF0000000,  /**< I2C os layer error. */

} I2cResult;

#endif // _I2CERROR_H_

