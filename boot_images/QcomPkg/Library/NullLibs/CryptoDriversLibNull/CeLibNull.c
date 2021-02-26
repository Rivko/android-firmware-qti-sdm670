/** @file CeLibNull.c
  
  Stub functions for CeLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/01/14   ck      Added CeMLHashAtomic
 09/24/14   ck      Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "CeML.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/**
 * @brief This function deinitializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @see 
 */
CeMLErrorType CeMLDeInit(void)
{
  return CEML_ERROR_SUCCESS;
}


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
CeMLErrorType CeMLHashDeInit(CeMLCntxHandle ** _h)
{
  return CEML_ERROR_SUCCESS;
}


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
CeMLErrorType CeMLInit(void)
{
  return CEML_ERROR_SUCCESS;
}


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
CeMLErrorType CeMLHashInit(CeMLCntxHandle ** _h, 
                           CeMLHashAlgoType pAlgo)
{
  return CEML_ERROR_SUCCESS;
}


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
CeMLErrorType CeMLHashUpdate(CeMLCntxHandle * _h, 
                             CEMLIovecListType ioVecIn)
{
  return CEML_ERROR_SUCCESS;
}


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

CeMLErrorType CeMLHashFinal(CeMLCntxHandle * _h,
                            CEMLIovecListType * ioVecOut)
{
  return CEML_ERROR_SUCCESS;
}


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
CeMLErrorType CeMLHashSetParam(CeMLCntxHandle * _h,
                               CeMLHashParamType nParamID, 
                               const void * pParam, 
                               uint64 cParam,
                               CeMLHashAlgoType pAlgo )
{
  return CEML_ERROR_SUCCESS;
}


/**
 * @brief This function will hash data into the hash context
 *        structure and compute the final digest hash value.
 *
 * @param _h       [in] Pointer to Hash context 
 * @param ioVecIn  [in] Pointer to input message to be hashed 
 * @param ioVecOut [Out] Pointer to output digest
 *
 * @return CeMLErrorType  
 *
 * @see CeMLHashInit
 *
 */

CeMLErrorType 
CeMLHashAtomic      (CeMLCntxHandle       * _h, 
                     CEMLIovecListType    ioVecIn,
                     CEMLIovecListType    * ioVecOut)
{
  return CEML_ERROR_SUCCESS;
}
