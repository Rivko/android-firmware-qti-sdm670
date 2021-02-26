/*===========================================================================

                    BOOT EXTERN CRYPTO AUTH DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external authentication drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/15   kpa     Addedboot_crypto_bam_enable
09/24/14   ck      Removed FEATURE_CRYPTO_NONBLOCK_SUPPORTED flag
10/15/13   kpa     Added boot_CeMLHashSetParam
06/14/13   plc     Add boot_securemsm_write_shk
04/03/13   dh      Add boot_PrngML_getdata_lite
11/08/12   kpa     Update to directly call Crypto api's.
06/11/12   kpa     Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_crypto_interface.h"
#include <Include/api/securemsm/PrngML.h>

/*===========================================================================
                           DEFINITIONS
===========================================================================*/


/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/ 

/*===========================================================================
**  Function :  boot_CeMLInit
** ==========================================================================
*/
/**
 * @brief This function initializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */
CeMLErrorType boot_CeMLInit (void)
{
  return CeMLInit();
}

/*===========================================================================
**  Function :  boot_CeMLHashInit
** ==========================================================================
*/

/**
 * @brief Intialize a hash context for Hash update and final functions
 *
 * @param _h      [in] Pointer to a pointer to the hash context
 * @param pAlgo   [in] Algorithm type
 *
 * @return CeMLErrorType
 *
 * @see CeMLHashUpdate and CeMLHashfinal
 *
 */
CeMLErrorType 
boot_CeMLHashInit        (CeMLCntxHandle       ** _h, 
                         CeMLHashAlgoType     pAlgo)
{
  return CeMLHashInit( _h, pAlgo);
}

/*===========================================================================
**  Function :  boot_CeMLHashUpdate
** ==========================================================================
*/
/**
 * @brief This function will hash data into the hash context
 *        structure, which must have been initialized by
 *        CeMLHashInit.
 *
 * @param _h          [in] Pointer to Hash context
 * @param ioVecIn     [in] Pointer to input message to be
 *                     hashed
 * @return CeMLErrorType 
 *
 * @see CeMLHashInit 
 *
 */
CeMLErrorType 
boot_CeMLHashUpdate      (CeMLCntxHandle       * _h, 
                         CEMLIovecListType    ioVecIn)
{
  return CeMLHashUpdate( _h, ioVecIn);
}

/*===========================================================================
**  Function :  boot_CeMLHashFinal
** ==========================================================================
*/
/**
 * @brief Compute the final digest hash value.
 *
 * @param _h         [in] Pointer to Hash context
 * @param ioVecOut   [out] Pointer to output digest

 * @return CeMLErrorType 
 *
 * @see CeMLHashInit
 *
 */
CeMLErrorType 
boot_CeMLHashFinal       (CeMLCntxHandle       * _h,
                         CEMLIovecListType    * ioVecOut)
{
  return CeMLHashFinal( _h, ioVecOut);
}                         

/*===========================================================================
**  Function :  boot_CeMLHashDeInit
** ==========================================================================
*/
/**
 * @brief Deintialize a hash context  
 *
 * @param _h      [in] Pointer to a pointer to the hash context
 *
 * @return CeMLErrorType
 *
 * @see CeMLDeInit
 *
 */
CeMLErrorType 
boot_CeMLHashDeInit      (CeMLCntxHandle       ** _h)
{
  return CeMLHashDeInit( _h);
}

/*===========================================================================
**  Function :  boot_CeMLDeInit
** ==========================================================================
*/
/**
 * @brief This function deinitializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @see 
 */
CeMLErrorType 
boot_CeMLDeInit          (void)
{
  return CeMLDeInit();
}


/*===========================================================================
**  Function :  boot_PrngML_getdata_lite
** ==========================================================================
*/
/**
 * @brief This function returns random number and can be used when Mutex,
 *        DAL framework are not yet initialised.
 *
 * @param random_ptr [in/out]Random number pointer
 * @param random_len [in] Length of random number 
 *
 * @return PrngML_Result_Type
 *
 * @see 
 */
PrngML_Result_Type boot_PrngML_getdata_lite
(
  uint8*  random_ptr,
  uint16  random_len
)
{
  return PrngML_getdata_lite(random_ptr, random_len);
}


/*===========================================================================
**  Function :  boot_CeMLHashSetParam
** ==========================================================================
*/
/**
 * @brief This functions sets the Hash paramaters - Mode and Key for HMAC
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 * @param palgo     [in]  Algorithm type
 *
 * @return CeMLErrorType
 *
 */
CeMLErrorType  
boot_CeMLHashSetParam  (CeMLCntxHandle       * _h,
                       CeMLHashParamType    nParamID, 
                       const void           *pParam, 
                       uint32               cParam,
                       CeMLHashAlgoType     pAlgo )
{
  return CeMLHashSetParam (_h, nParamID, pParam, cParam, pAlgo);
}


/*===========================================================================
**  Function :  boot_crypto_bam_enable
** ==========================================================================
*/
/**
 * @brief Enable crypto BAM post-DDR init
 *
 * @param   NA
 *
 * @return  NA
 *
 */
void boot_crypto_bam_enable(void)
{
  // TODO: _FIX_  crypto team to support/stub the api for all targets
  // crypto_bam_enable();
}

