#ifndef __PRNGML_H__
#define __PRNGML_H__
/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       H e a d e r  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW Crypto specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/common/PrngML.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/25/10   yk     Initial version
============================================================================*/


#include "comdef.h"

typedef enum
{
    PRNGML_ERROR_NONE,
    PRNGML_ERROR_BUSY,
    PRNGML_ERROR_FAILED,
    PRNGML_ERROR_INVALID_PARAM,
    PRNGML_ERROR_UNSUPPORTED,
    PRNGML_ERROR_BAD_STATE
} PrngML_Result_Type;
/**
 * @brief    This function returns random number.  
 *
 * @param random_ptr [in]Random number pointer
 * @param random_len [in]Length of random number 
 *
 * @return PrngML_Resut_Type
 *
 *
 */
 PrngML_Result_Type PrngML_getdata
(
  uint8*  random_ptr,
  uint16  random_len
);


/**
 * @brief This function returns random number and can be used when Mutex,
 *        DAL framework are not yet initialised.
 *
 * @param random_ptr [in/out]Random number pointer
 * @param random_len [in] Length of random number 
 *
 * @return PrngML_Result_Type
 */
PrngML_Result_Type PrngML_getdata_lite
(
  uint8*  random_ptr,
  uint16  random_len
);
#endif /*__PRNGML_H__*/
