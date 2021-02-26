#ifndef GPQESE_CMDS_H_
#define GPQESE_CMDS_H_

#include <stdint.h>
#include "qsee_ese_service.h"
#include "tee_internal_api.h"
#include "tee_internal_se_api.h"

/*
 * Global structure available to all calls in this Trusted Application. Set when the TA is loaded.
 * Unset
 * when the TA is unloaded
 *
 * @see TEE_SetInstanceData
 * @see TEE_GetInstanceData
 */
typedef struct
{
  TEE_SEServiceHandle seServiceHandle;
  TEE_SEReaderHandle  seReaderHandle;
} GPQESE_Instance;

/*
 * Global structure available to all calls in this TA Session. Set when a TA session is opened.
 * Detroyed
 * when the TA session is closed.
 *
 * @see TA_OpenSessionEntryPoint
 * @see TA_CloseSessionEntryPoint
 */
typedef struct
{
  TEE_SC_Params       scParams;
  TEE_SESessionHandle seSessionHandle;
  TEE_SEChannelHandle seChannelHandles[QSEE_ESE_MAX_NUMBER_OF_CHANNELS];
} GPQESE_Session;

/*
 * Opens a service with the API. Gets the reader handle given an index for the reader
 * Opens a session to the Secure Element
 *
 * @param[in] pSessionContext pointer to session context obtained from TA_OpenSessionEntryPoint
 *
 * @param[in] nParamTypes  GP Parameter types
 *                         - nParamTypes#0 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#1 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#2 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#3 - TEE_PARAM_TYPE_NONE
 *
 * @returns TEE_SUCCESS on success run. GP TEE SE API v1.1.1 error code otherwise
 *
 * @see GPQESE_Close
 * @see TEE_SEServiceOpen
 * @see TEE_SEServiceGetReaders
 * @see TEE_SEReaderOpenSession
 */
TEE_Result GPQESE_Open(GPQESE_Session *pSessionContext,
                       uint32_t        nParamTypes);

/*
 * Close a service with the API by closing all the sessions open on the service,
 * freeing the TEE_SEReaderHandle and freeing resources allocated for the SE API
 *
 * @param[in] pSessionContext pointer to session context obtained from TA_OpenSessionEntryPoint
 *
 * @param[in] nParamTypes  GP Parameter types
 *                         - nParamTypes#0 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#1 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#2 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#3 - TEE_PARAM_TYPE_NONE
 *
 * @returns TEE_SUCCESS on success run. GP TEE SE API v1.1.1 error code otherwise
 *
 * @see GPQESE_Open
 * @see TEE_SESessionClose
 * @see TEE_SEReaderCloseSessions
 * @see TEE_SEServiceClose
 */
TEE_Result GPQESE_Close(GPQESE_Session *pSessionContext,
                        uint32_t        nParamTypes);

/*
 * Sends C-APDU to SE and waits for response for BWT timeout
 *
 * @param[in] pContext     pointer to session context obtained from TA_OpenSessionEntryPoint
 *
 * @param[in] nParamTypes  GP Parameter types
 *                         - nParamTypes#0 - TEE_PARAM_TYPE_MEMREF_INPUT
 *                         - nParamTypes#1 - TEE_PARAM_TYPE_MEMREF_OUTPUT
 *                         - nParamTypes#2 - TEE_PARAM_TYPE_NONE
 *                         - nParamTypes#3 - TEE_PARAM_TYPE_NONE
 *
 * @param[in,out] pParams GP Parameters
 *                         - pParams[0].memref.buffer - C-APDU buffer
 *                         - pParams[0].memref.size   - Size of C-APDU
 *                         - pParams[1].memref.buffer - buffer to write R-APDU
 *                         - pParams[1].memref.size   - size of R-APDU buffer
 *
 * @returns TEE_SUCCESS if sent and received APDU. Otherwise failure code per GPD_SPE_024
 */
TEE_Result GPQESE_Transceive(GPQESE_Session *pSessionContext,
                             uint32_t        nParamTypes,
                             TEE_Param       pParams[MAX_GP_PARAMS]);

#endif // GPQESE_CMDS_H_
