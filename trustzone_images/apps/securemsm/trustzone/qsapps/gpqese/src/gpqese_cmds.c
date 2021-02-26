#include <stddef.h>
#include <stdint.h>
#include "stringl.h"
#include "gpqese_cmds.h"
#include "qsee_ese_service.h"
#include "qsee_ecc.h"
#include "tee_internal_api.h"
#include "tee_internal_se_api.h"
#include "tee_internal_se_api_ext.h"
#include "EseDebug.h"

/**
 * Get the channel handle pointer for the give index. Returns null if index is invalid
 */
static TEE_SEChannelHandle *GPQESE_GetFreeChannelHandle(GPQESE_Session *pSessionContext)
{
  TEE_SEChannelHandle *pSeChannelHandle = NULL;

  QSEE_ESE_ENTER();

  for (size_t i = 0; i < QSEE_ESE_ARR_SZ(pSessionContext->seChannelHandles); ++i)
  {
    // NULL handle indicates free entry
    if (pSessionContext->seChannelHandles[i] == TEE_HANDLE_NULL)
    {
      pSeChannelHandle = pSessionContext->seChannelHandles + i;
      break;
    }
  }

  QSEE_ESE_EXIT("pSeChannelHandle=%X", pSeChannelHandle);

  return pSeChannelHandle;
}

/**
 * Iterates through all the channel handles to fetch the pointer to the one
 * corresponding to the channel number passed in.
 *
 */
static TEE_SEChannelHandle *GPQESE_GetChannelNumberHandle(GPQESE_Session *pSessionContext,
                                                          uint32_t        nChannelNumber)
{
  TEE_SEChannelHandle *pSeChannelHandle = NULL;

  QSEE_ESE_ENTER("nChannelNumber=%d", nChannelNumber);

  for (size_t i = 0; i < QSEE_ESE_ARR_SZ(pSessionContext->seChannelHandles); ++i)
  {
    if (pSessionContext->seChannelHandles[i] == TEE_HANDLE_NULL)
    {
      continue;
    }

    uint32_t thisChannelNumber = QSEE_ESE_ARR_SZ(pSessionContext->seChannelHandles);

    TEE_Result retval =
      TEE_SEChannelGetNumber_ext(pSessionContext->seChannelHandles[i], &thisChannelNumber);

    if ((retval == TEE_SUCCESS) && (thisChannelNumber == nChannelNumber))
    {
      pSeChannelHandle = pSessionContext->seChannelHandles + i;
      break;
    }
  }

  QSEE_ESE_EXIT("pSeChannelHandle=%X", pSeChannelHandle);

  return pSeChannelHandle;
}

static TEE_Result GPQESE_GetSecureChannelParams(TEE_SC_Params *pSCParams)
{
  TEE_Result retval = TEE_ERROR_BAD_PARAMETERS;

  QSEE_ESE_ENTER("pSCParams=%X", pSCParams);

  do
  {
    if (pSCParams == NULL)
    {
      break;
    }

    secure_memset(pSCParams, 0, sizeof(*pSCParams));

    // Sample code to demonstrate how to get key pair values

    // eccCurve            is one of the TEE_ECC_CURVE_* values
    // eccKeyBits          is the key length in bits (256 for Brainpool P256r1)
    // dest                transient object which will contain the key pair
    static const uint32_t eccCurve     = TEE_ECC_CURVE_PROP_BP_P256r1;
    static const uint32_t eccKeyBits   = 256;
    TEE_Attribute         attrECCCurve = { 0 };
    TEE_ObjectHandle      dest         = { 0 };

    retval = TEE_AllocateTransientObject(TEE_TYPE_ECDSA_KEYPAIR, eccKeyBits, &dest);

    if (retval != TEE_SUCCESS)
    {
      QSEE_ESE_LOG_ERR("TEE_AllocateTransientObject() failed");
      break;
    }

    TEE_InitValueAttribute(&attrECCCurve, TEE_ATTR_ECC_CURVE, eccCurve, 0);

    retval = TEE_GenerateKey(dest, eccKeyBits, &attrECCCurve, 1);

    if (retval != TEE_SUCCESS)
    {
      QSEE_ESE_LOG_ERR("TEE_GenerateKey() failed");
      break;
    }

    QSEE_qrlbn_ecc_bigval_t       privateKey          = { 0 };
    QSEE_qrlbn_ecc_affine_point_t publicKey           = { 0 };
    uint8_t                       keyBuffer[256]      = { 0 };
    size_t                        keySize             = sizeof(keyBuffer);
    static const int              ecc_conversion_mode = 256; // binary

    keySize = sizeof(keyBuffer);
    retval  = TEE_GetObjectBufferAttribute(dest, TEE_ATTR_ECC_PRIVATE_VALUE, keyBuffer, &keySize);

    if (retval != TEE_SUCCESS)
    {
      QSEE_ESE_LOG_ERR("TEE_GetObjectBufferAttribute(TEE_ATTR_ECC_PRIVATE_VALUE) failed");
      break;
    }

    if (qsee_SW_GENERIC_ECC_convert_input_to_bigval(&privateKey,
                                                    keyBuffer,
                                                    keySize,
                                                    ecc_conversion_mode) != 0)
    {
      QSEE_ESE_LOG_ERR("qsee_SW_GENERIC_ECC_convert_input_to_bigval(privateKey) failed");
      retval = TEE_ERROR_GENERIC;
      break;
    }

    keySize = sizeof(keyBuffer);
    retval  = TEE_GetObjectBufferAttribute(dest, TEE_ATTR_ECC_PUBLIC_VALUE_X, keyBuffer, &keySize);

    if (retval != TEE_SUCCESS)
    {
      QSEE_ESE_LOG_ERR("TEE_GetObjectBufferAttribute(TEE_ATTR_ECC_PUBLIC_VALUE_X) failed");
      break;
    }

    if (qsee_SW_GENERIC_ECC_convert_input_to_bigval(&publicKey.x,
                                                    keyBuffer,
                                                    keySize,
                                                    ecc_conversion_mode) != 0)
    {
      QSEE_ESE_LOG_ERR("TEE_GetObjectBufferAttribute(publicKey.x) failed");
      retval = TEE_ERROR_GENERIC;
      break;
    }

    keySize = sizeof(keyBuffer);
    retval  = TEE_GetObjectBufferAttribute(dest, TEE_ATTR_ECC_PUBLIC_VALUE_Y, keyBuffer, &keySize);

    if (retval != TEE_SUCCESS)
    {
      QSEE_ESE_LOG_ERR("TEE_GetObjectBufferAttribute(TEE_ATTR_ECC_PUBLIC_VALUE_Y) failed");
      break;
    }

    if (qsee_SW_GENERIC_ECC_convert_input_to_bigval(&publicKey.y,
                                                    keyBuffer,
                                                    keySize,
                                                    ecc_conversion_mode) != 0)
    {
      QSEE_ESE_LOG_ERR("TEE_GetObjectBufferAttribute(publicKey.y) failed");
      retval = TEE_ERROR_GENERIC;
      break;
    }

    // OEM can use above keys pairs

    retval = TEE_ERROR_NOT_IMPLEMENTED;
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 * Opens a channel with the SE API.
 */
static TEE_Result GPQESE_ChannelOpen(GPQESE_Session *    pSessionContext,
                                     TEE_SESessionHandle pSeSessionHandle,
                                     uint8_t *           pCmd,
                                     uint32_t            sizeCmd,
                                     uint8_t *           pRsp,
                                     uint32_t *          pSizeRsp,
                                     TEE_Result (*pfnOpen)(TEE_SESessionHandle,
                                                           TEE_SEAID *,
                                                           TEE_SEChannelHandle *))
{
  TEE_Result retval = TEE_ERROR_COMMUNICATION;

  QSEE_ESE_ENTER("sizeCmd=%d, *pSizeRsp=%d", sizeCmd, *pSizeRsp);

  do
  {
    TEE_SEChannelHandle *pSeChannelHandle = GPQESE_GetFreeChannelHandle(pSessionContext);

    if (pSeChannelHandle == NULL)
    {
      break;
    }

    const uint8_t INS = pCmd[QSEE_ESE_APDU_OFFSET_INS];
    const uint8_t P1  = pCmd[QSEE_ESE_APDU_OFFSET_P1];
    const uint8_t LC  = pCmd[QSEE_ESE_APDU_OFFSET_LC];

    // Basic channel receives select as first command - open channel by passing the AID if one is
    // provided
    if ((INS == QSEE_ESE_APDU_INS_SELECT) && (P1 == QSEE_ESE_APDU_P1_SELECT) && (LC != 0))
    {
      TEE_SEAID seAID = {.buffer = pCmd + QSEE_ESE_APDU_OFFSET_CDATA, .bufferLen = LC };

      retval = (*pfnOpen)(pSeSessionHandle, &seAID, pSeChannelHandle);
    }
    else
    {
      retval = (*pfnOpen)(pSeSessionHandle, NULL, pSeChannelHandle);
    }

    if (retval == TEE_ERROR_COMMUNICATION)
    {
      break;
    }

    if (retval == TEE_ERROR_CANCEL)
    {
      break;
    }

    GPQESE_Instance *pInstanceData = TEE_GetInstanceData();

    if (retval != TEE_SUCCESS)
    {
      // Failed to open the channel - report status back to client
      retval = TEE_SEReaderGetStatus_ext(pInstanceData->seReaderHandle, pRsp, pSizeRsp);
      break;
    }

    uint32_t nChannelNumber = QSEE_ESE_ARR_SZ(pSessionContext->seChannelHandles);

    // We need to pass back the SELECT Response to HLOS if we are dealing with a SELECT command
    if ((INS == QSEE_ESE_APDU_INS_SELECT) && (P1 == QSEE_ESE_APDU_P1_SELECT))
    {
      retval = TEE_SEChannelGetSelectResponse(*pSeChannelHandle, pRsp, pSizeRsp);
    }
    else
    {
      // Try to get the channel number - the channel may have opened but failed to select
      // We get TEE_ERROR_ITEM_NOT_FOUND if channel failed to open
      retval = TEE_SEChannelGetNumber_ext(*pSeChannelHandle, &nChannelNumber);

      if (retval != TEE_SUCCESS)
      {
        retval = TEE_SEReaderGetStatus_ext(pInstanceData->seReaderHandle, pRsp, pSizeRsp);
        break;
      }

      // Channel Number goes into the CLA byte
      pRsp[QSEE_ESE_APDU_OFFSET_CLA] = (uint8_t)nChannelNumber;

      const size_t nChannelNumberSize = sizeof(pRsp[QSEE_ESE_APDU_OFFSET_CLA]);

      uint32_t statusSize = *pSizeRsp - nChannelNumberSize;

      *pSizeRsp = nChannelNumberSize;

      retval = TEE_SEReaderGetStatus_ext(pInstanceData->seReaderHandle,
                                         pRsp + nChannelNumberSize,
                                         &statusSize);

      if (retval == TEE_SUCCESS)
      {
        *pSizeRsp += statusSize;
      }
    }

    if (retval != TEE_SUCCESS)
    {
      break;
    }

    // Should we perform Secure Channel Protocol(SCP)
    if (GPQESE_GetSecureChannelParams(&pSessionContext->scParams) != TEE_SUCCESS)
    {
      // We still return TEE_SUCCESS since we continue without SCP
      break;
    }

    // Per v1.1.1 of GPD_SPE_024
    if (TEE_SESecureChannelGetSecurityLevel(*pSeChannelHandle) != TEE_SC_NO_SECURE_MESSAGING)
    {
      TEE_SESecureChannelClose(*pSeChannelHandle);
    }

    retval = TEE_SESecureChannelOpen(*pSeChannelHandle, &pSessionContext->scParams);

    if (retval == TEE_ERROR_CANCEL)
    {
      break;
    }

    if (retval != TEE_SUCCESS)
    {
      // Send status back to client
      retval = TEE_SEReaderGetStatus_ext(pInstanceData->seReaderHandle, pRsp, pSizeRsp);
      break;
    }
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 * Opens a logical channel with the SE API.
 */
static TEE_Result GPQESE_ChannelOpenLogical(GPQESE_Session *    pSessionContext,
                                            TEE_SESessionHandle pSeSessionHandle,
                                            uint8_t *           pCmd,
                                            uint32_t            sizeCmd,
                                            uint8_t *           pRsp,
                                            uint32_t *          pSizeRsp)
{
  TEE_Result retval = TEE_ERROR_COMMUNICATION;

  QSEE_ESE_ENTER("sizeCmd=%d, *pSizeRsp=%d", sizeCmd, *pSizeRsp);

  retval = GPQESE_ChannelOpen(pSessionContext,
                              pSeSessionHandle,
                              pCmd,
                              sizeCmd,
                              pRsp,
                              pSizeRsp,
                              TEE_SESessionOpenLogicalChannel);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 * Opens a basic channel with the SE API.
 */
static TEE_Result GPQESE_ChannelOpenBasic(GPQESE_Session *    pSessionContext,
                                          TEE_SESessionHandle pSeSessionHandle,
                                          uint8_t *           pCmd,
                                          uint32_t            sizeCmd,
                                          uint8_t *           pRsp,
                                          uint32_t *          pSizeRsp)
{
  TEE_Result retval = TEE_ERROR_COMMUNICATION;

  QSEE_ESE_ENTER("sizeCmd=%d, *pSizeRsp=%d", sizeCmd, *pSizeRsp);

  retval = GPQESE_ChannelOpen(pSessionContext,
                              pSeSessionHandle,
                              pCmd,
                              sizeCmd,
                              pRsp,
                              pSizeRsp,
                              TEE_SESessionOpenBasicChannel);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 *
 */
static TEE_Result GPQESE_ChannelClose(GPQESE_Session *    pSessionContext,
                                      TEE_SESessionHandle pSeSessionHandle,
                                      uint8_t *           pCmd,
                                      uint32_t            sizeCmd,
                                      uint8_t *           pRsp,
                                      uint32_t *          pSizeRsp)
{
  TEE_Result retval = TEE_ERROR_BAD_STATE;

  QSEE_ESE_ENTER("sizeCmd=%d, *pSizeRsp=%d", sizeCmd, *pSizeRsp);

  do
  {
    const uint8_t  CLA            = pCmd[QSEE_ESE_APDU_OFFSET_CLA];
    const uint32_t nChannelNumber = CLA & QSEE_ESE_APDU_CLA_CHANNEL_MASK;

    TEE_SEChannelHandle *pSeChannelHandle =
      GPQESE_GetChannelNumberHandle(pSessionContext, nChannelNumber);

    if (pSeChannelHandle == NULL) // Already closed
    {
      break;
    }

    // Unsecure the channel
    if (TEE_SESecureChannelGetSecurityLevel(*pSeChannelHandle) != TEE_SC_NO_SECURE_MESSAGING)
    {
      TEE_SESecureChannelClose(*pSeChannelHandle);
    }

    TEE_SEChannelClose(*pSeChannelHandle);

    const GPQESE_Instance *pInstanceData = TEE_GetInstanceData();

    // For channel close. If we get this at it means the channel has closed successfully
    retval = TEE_SEReaderGetStatus_ext(pInstanceData->seReaderHandle, pRsp, pSizeRsp);

    *pSeChannelHandle = TEE_HANDLE_NULL;
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 * Transmits a C-APDU from the CA. Waits for a response from the Secure Element.
 * Writes back the response into the response buffer if successful.
 */
static TEE_Result GPQESE_Transmit(GPQESE_Session *    pSessionContext,
                                  TEE_SESessionHandle pSeSessionHandle,
                                  uint8_t *           pCmd,
                                  uint32_t            sizeCmd,
                                  uint8_t *           pRsp,
                                  uint32_t *          pSizeRsp)
{
  TEE_Result retval = TEE_ERROR_COMMUNICATION;

  QSEE_ESE_ENTER("sizeCmd=%d, *pSizeRsp=%d", sizeCmd, *pSizeRsp);

  do
  {
    const uint8_t  CLA            = pCmd[QSEE_ESE_APDU_OFFSET_CLA];
    const uint32_t nChannelNumber = CLA & QSEE_ESE_APDU_CLA_CHANNEL_MASK;

    TEE_SEChannelHandle *pSeChannelHandle =
      GPQESE_GetChannelNumberHandle(pSessionContext, nChannelNumber);

    if (pSeChannelHandle == NULL)
    {
      break;
    }

    // We do not do anything with the C-APDU/R-APDU - Just piped through
    retval = TEE_SEChannelTransmit_ext(*pSeChannelHandle,
                                       pCmd,
                                       sizeCmd,
                                       pRsp,
                                       pSizeRsp,
                                       true /*Allow SELECT */);

  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 *
 */
static TEE_Result GPQESE_ChannelSelect(GPQESE_Session *    pSessionContext,
                                       TEE_SESessionHandle pSeSessionHandle,
                                       uint8_t *           pCmd,
                                       uint32_t            sizeCmd,
                                       uint8_t *           pRsp,
                                       uint32_t *          pSizeRsp)
{
  TEE_Result retval = TEE_ERROR_COMMUNICATION;

  QSEE_ESE_ENTER("sizeCmd=%d, *pSizeRsp=%d", sizeCmd, *pSizeRsp);

  const uint8_t  CLA            = pCmd[QSEE_ESE_APDU_OFFSET_CLA];
  const uint32_t nChannelNumber = CLA & QSEE_ESE_APDU_CLA_CHANNEL_MASK;

  if (nChannelNumber == QSEE_ESE_BASIC_CHANNEL_NUMBER)
  {
    // pSeChannelHandle of NULL => basic channel not yet selected.
    if (GPQESE_GetChannelNumberHandle(pSessionContext, nChannelNumber) == NULL)
    {
      retval =
        GPQESE_ChannelOpenBasic(pSessionContext, pSeSessionHandle, pCmd, sizeCmd, pRsp, pSizeRsp);
    }
    // No AID selected - reset to default applet i.e. close basic channel
    else if (pCmd[QSEE_ESE_APDU_OFFSET_LC] == 0)
    {
      retval =
        GPQESE_ChannelClose(pSessionContext, pSeSessionHandle, pCmd, sizeCmd, pRsp, pSizeRsp);
    }
    // A SELECT command for already acquired basic channel handle
    else
    {
      retval = GPQESE_Transmit(pSessionContext, pSeSessionHandle, pCmd, sizeCmd, pRsp, pSizeRsp);
    }
  }
  else
  {
    retval = GPQESE_Transmit(pSessionContext, pSeSessionHandle, pCmd, sizeCmd, pRsp, pSizeRsp);
  }

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 *
 */
TEE_Result GPQESE_Open(GPQESE_Session *pSessionContext, uint32_t nParamTypes)

{
  TEE_Result retval = TEE_ERROR_BAD_PARAMETERS;

  QSEE_ESE_ENTER("nParamTypes=0x%08X", nParamTypes);

  do
  {
    if (nParamTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE))
    {
      break;
    }

    TEE_SESessionHandle *pSeSessionHandle = &pSessionContext->seSessionHandle;
    GPQESE_Instance *    pInstanceData    = TEE_GetInstanceData();

    retval = TEE_SEServiceOpen(&pInstanceData->seServiceHandle);

    // TEE_ERROR_ACCESS_CONFLICT if Service already opened
    if ((retval != TEE_SUCCESS) && (retval != TEE_ERROR_ACCESS_CONFLICT))
    {
      break;
    }

    uint32_t numReaders = 1;

    retval = TEE_SEServiceGetReaders(pInstanceData->seServiceHandle,
                                     &pInstanceData->seReaderHandle,
                                     &numReaders);

    if (retval != TEE_SUCCESS)
    {
      break;
    }

    retval = TEE_SEReaderOpenSession(pInstanceData->seReaderHandle, pSeSessionHandle);

    if (retval != TEE_SUCCESS)
    {
      break;
    }
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 *
 */
TEE_Result GPQESE_Close(GPQESE_Session *pSessionContext, uint32_t nParamTypes)
{
  QSEE_ESE_ENTER("nParamTypes=0x%08X", nParamTypes);

  TEE_Result retval = TEE_ERROR_BAD_PARAMETERS;

  do
  {
    if (nParamTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE))
    {
      break;
    }

    if (pSessionContext->seSessionHandle != TEE_HANDLE_NULL)
    {
      TEE_SESessionClose(pSessionContext->seSessionHandle);

      pSessionContext->seSessionHandle = TEE_HANDLE_NULL;
    }

    GPQESE_Instance *pInstanceData = TEE_GetInstanceData();

    if (pInstanceData->seReaderHandle != TEE_HANDLE_NULL)
    {
      uint32_t nNumOfSessions = 0;

      retval = TEE_SEReaderGetSessionCount_ext(pInstanceData->seReaderHandle, &nNumOfSessions);

      if (retval != TEE_SUCCESS)
      {
        break;
      }

      if (nNumOfSessions != 0)
      {
        // Frees the the reader resources. Closes session and therefore all open channels
        TEE_SEServiceClose(pInstanceData->seServiceHandle);

        pInstanceData->seServiceHandle = TEE_HANDLE_NULL;
        pInstanceData->seReaderHandle  = TEE_HANDLE_NULL;

        // All the channels are now closed. Clear the handles
        secure_memset(pSessionContext->seChannelHandles,
                      0,
                      sizeof(pSessionContext->seChannelHandles));
      }
    }

    retval = TEE_SUCCESS;
  } while (0);

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}

/*
 *
 */
TEE_Result GPQESE_Transceive(GPQESE_Session *pSessionContext,
                             uint32_t        nParamTypes,
                             TEE_Param       pParams[MAX_GP_PARAMS])
{
  TEE_Result retval = TEE_ERROR_BAD_PARAMETERS;
  uint8_t *  pCmd   = NULL;
  uint8_t *  pRsp   = NULL;

  QSEE_ESE_ENTER("nParamTypes=0x%08X", nParamTypes);

  do
  {
    if (nParamTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,  // C-APDU
                                       TEE_PARAM_TYPE_MEMREF_OUTPUT, // R-APDU
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE))
    {
      break;
    }

    if (TEE_GetCancellationFlag())
    {
      retval = TEE_ERROR_CANCEL;
      break;
    }

    TEE_Buffer *const pMemRefIn  = &(pParams[0].memref);
    TEE_Buffer *const pMemRefOut = &(pParams[1].memref);

    if (pMemRefIn->size < QSEE_ESE_APDU_MIN_SZ)
    {
      break;
    }

    if (pMemRefOut->size == 0)
    {
      retval = TEE_ERROR_SHORT_BUFFER;
      break;
    }

    // We use local memory to perform the transaction as client can update the buffers at any time
    pCmd = TEE_Malloc(pMemRefIn->size, TEE_MALLOC_FILL_ZERO);
    pRsp = TEE_Malloc(pMemRefOut->size, TEE_MALLOC_FILL_ZERO);

    if ((pCmd == NULL) || (pRsp == NULL))
    {
      retval = TEE_ERROR_OUT_OF_MEMORY;
      break;
    }

    uint32_t sizeCmd = memscpy(pCmd, pMemRefIn->size, pMemRefIn->buffer, pMemRefIn->size);
    uint32_t sizeRsp = pMemRefOut->size;

    EseLogBuffer(pCmd, sizeCmd, "HLOS2TA");

    const uint8_t INS = pCmd[QSEE_ESE_APDU_OFFSET_INS];
    const uint8_t P1  = pCmd[QSEE_ESE_APDU_OFFSET_P1];

    if ((INS == QSEE_ESE_APDU_INS_MANAGE_CHANNEL) && (P1 == QSEE_ESE_APDU_P1_MANAGE_CHANNEL_OPEN))
    {
      retval = GPQESE_ChannelOpenLogical(pSessionContext,
                                         pSessionContext->seSessionHandle,
                                         pCmd,
                                         sizeCmd,
                                         pRsp,
                                         &sizeRsp);
    }
    else if ((INS == QSEE_ESE_APDU_INS_MANAGE_CHANNEL) &&
             (P1 == QSEE_ESE_APDU_P1_MANAGE_CHANNEL_CLOSE))
    {
      retval = GPQESE_ChannelClose(pSessionContext,
                                   pSessionContext->seSessionHandle,
                                   pCmd,
                                   sizeCmd,
                                   pRsp,
                                   &sizeRsp);
    }
    else if ((INS == QSEE_ESE_APDU_INS_SELECT) && (P1 == QSEE_ESE_APDU_P1_SELECT))
    {
      retval = GPQESE_ChannelSelect(pSessionContext,
                                    pSessionContext->seSessionHandle,
                                    pCmd,
                                    sizeCmd,
                                    pRsp,
                                    &sizeRsp);
    }
    else
    {
      retval = GPQESE_Transmit(pSessionContext,
                               pSessionContext->seSessionHandle,
                               pCmd,
                               sizeCmd,
                               pRsp,
                               &sizeRsp);
    }

    if (retval != TEE_SUCCESS)
    {
      pMemRefOut->size = 0;
      break;
    }

    pMemRefOut->size = memscpy(pMemRefOut->buffer, pMemRefOut->size, pRsp, sizeRsp);

    if (pMemRefOut->size)
    {
      EseLogBuffer(pMemRefOut->buffer, pMemRefOut->size, "TA2HLOS");
    }

  } while (0);

  if (pCmd != NULL)
  {
    TEE_Free(pCmd);
  }

  if (pRsp != NULL)
  {
    TEE_Free(pRsp);
  }

  QSEE_ESE_EXIT("retval=%X", retval);

  return retval;
}
