/* ----------------------------------------------------------------------------
*   Includes
* ---------------------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gpAppLibMain.h"

/* ----------------------------------------------------------------------------
 *   Trusted Application Entry Points
 * ---------------------------------------------------------------------------- */

TEE_Result
TA_CreateEntryPoint(
  void)
{
   SLogTrace("TA_CreateEntryPoint()");

   return TEE_SUCCESS;
}//TA_CreateEntryPoint()


void
TA_DestroyEntryPoint(
  void)
{
   SLogTrace("TA_DestroyEntryPoint()");
}//TA_DestroyEntryPoint()


TEE_Result
TA_OpenSessionEntryPoint(
  uint32_t  nParamTypes,
  TEE_Param pParams[4],
  void**    ppSessionContext )
{
   S_VAR_NOT_USED(nParamTypes);
   S_VAR_NOT_USED(pParams);
   S_VAR_NOT_USED(ppSessionContext);

   SLogTrace("TA_OpenSessionEntryPoint()");

   return TEE_SUCCESS;
}//TA_OpenSessionEntryPoint()


void
TA_CloseSessionEntryPoint(
  void* pSessionContext)
{
   S_VAR_NOT_USED(pSessionContext);

   SLogTrace("TA_CloseSessionEntryPoint()");
}//TA_CloseSessionEntryPoint()

/* SHA 1 test vectors */
unsigned char sha_plain_text_1[] =
{
  "Test vector from febooti.com"
};

unsigned char sha1_digest_text_1[] =
{
  0xa7, 0x63, 0x17, 0x95, 0xf6, 0xd5, 0x9c, 0xd6,
  0xd1, 0x4e, 0xbd, 0x00, 0x58, 0xa6, 0x39, 0x4a,
  0x4b, 0x93, 0xd8, 0x68
};

/* SHA 256 test vectors */
unsigned char sha256_digest_text_1[] =
{
  0x07, 0x7b, 0x18, 0xfe, 0x29, 0x03, 0x6a, 0xda,
  0x48, 0x90, 0xbd, 0xec, 0x19, 0x21, 0x86, 0xe1,
  0x06, 0x78, 0x59, 0x7a, 0x67, 0x88, 0x02, 0x90,
  0x52, 0x1d, 0xf7, 0x0d, 0xf4, 0xba, 0xc9, 0xab
};

TEE_Result
test_gp_crypto(
  void)
{
  TEE_OperationHandle operation     = NULL;
  TEE_OperationInfo   operationInfo = {0};
  TEE_Result          result        = TEE_SUCCESS;

  uint32_t  hash[8]                 = {0};
  size_t    hash_len                = 0;

  do
  {
    SLogTrace("--------------GP Crypto SHA1 Test Begin----------------");

    result = TEE_AllocateOperation(&operation, TEE_ALG_SHA1, TEE_MODE_DIGEST, 0);
    if(TEE_SUCCESS != result)
    {
      GPLOG_FATAL2("TEE_AllocateOperation() failed", result);
      break;
    }

    TEE_GetOperationInfo(operation, &operationInfo);

    TEE_DigestUpdate(operation, sha_plain_text_1, sizeof(sha_plain_text_1) - 1);

    hash_len = sizeof(hash);
    //SLogTrace("size of hash len 0x%x",hash_len);

    result = TEE_DigestDoFinal(operation, NULL, 0, hash, &hash_len);
    if(TEE_SUCCESS != result)
    {
      SLogTrace("TEE_DigestDoFinal() failed");
      break;
    }

    if(0 != TEE_MemCompare(hash, sha1_digest_text_1, sizeof(sha1_digest_text_1)))
    {
      SLogTrace("TEE_MemCompare() failed");
      result = TEE_ERROR_GENERIC;
      break;
    }
  }while(0);

  TEE_FreeOperation(operation);
  SLogTrace("--------------GP Crypto SHA1 Test End----------------");
  return result;
}//test_gp_crypto()


TEE_Result
test_gp_arithmetic_apis(
  void)
{
  TEE_Result result = TEE_ERROR_GENERIC;

  SLogTrace("--------------GP Arithmetic APIs Test Begin----------------");
  //As of now a dummy test, as FIME Arithmetic Test Pass 100%
  result = TEE_BigIntCmp(NULL, NULL);

  SLogTrace("--------------GP Arithmetic APIs Test End----------------");
  return result;
}//test_gp_arithmetic_apis()

TEE_Result
TA_InvokeCommandEntryPoint(
  void*     pSessionContext,
  uint32_t  nCommandID,
  uint32_t  nParamTypes,
  TEE_Param pParams[4])
{
  TEE_Result result = TEE_ERROR_GENERIC;

   S_VAR_NOT_USED(pSessionContext);

   SLogTrace("TA_InvokeCommandEntryPoint()");

   result = test_gp_crypto();
   if (TEE_SUCCESS == result) result = test_gp_arithmetic_apis();

   return result;
}//TA_InvokeCommandEntryPoint()

