
#ifndef __PRNGCL_H__
#define __PRNGCL_H__

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       H e a d e r  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW Crypto specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/common/PrngCL.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
9/20/2012  amen     PRNG_LITE 
============================================================================*/

#include "comdef.h"

typedef enum
{
    PRNGCL_ERROR_NONE,
    PRNGCL_ERROR_BUSY,
    PRNGCL_ERROR_FAILED,
    PRNGCL_ERROR_INVALID_PARAM,
    PRNGCL_ERROR_UNSUPPORTED,
    PRNGCL_ERROR_BAD_STATE
} PrngCL_Result_Type;

/**
 * @brief    This function initializes PRNG Engine.  
 *
 * @param None
 *
 * @return PrngCL_Result_Type
 *
 * @see PrngCL_getdata
 *
 */
PrngCL_Result_Type PrngCL_init( void );


/**
 * @brief    This function returns the contents of the PRNG data out register.  
 *
 * @param None
 *
 * @return PrngCL_Resut_Type
 *
 * @see PrngCL_init
 *
 */

PrngCL_Result_Type PrngCL_getdata(uint8*  random_ptr,  uint16  random_len);

/**
 * @brief This function de-initializes PRNG Engine.
 *
 * @param None
 * @param None
 *
 * @return None
 *
 * @see PrngCL_init
 *
 */
PrngCL_Result_Type PrngCL_deinit(void);

/**
 * @brief    This function initializes PRNG Engine. Uses direct register write/reads  
 *
 * @param None
 *
 * @return PrngCL_Resut_Type
 *
 * @see PrngCL_lite_init
 *
 */

PrngCL_Result_Type PrngCL_lite_init(void);

/**
 * @brief This function de-initializes PRNG Engine. Uses direct register write/reads
 *
 * @param None
 *
 * @return None
 *
 * @see PrngCL_lite_deinit
 *
 */

PrngCL_Result_Type PrngCL_lite_deinit(void);

#endif /*__PRNGCL_H__ */
