#ifndef TEE_INTERNAL_SE_API_EXT_H
#define TEE_INTERNAL_SE_API_EXT_H
/*
 * Contains QCOM Specific extensions to the GP TEE SE APIs. EOMs are advised not to used
 * this header file for implementing their TA as the contents may be changed by QCOM
 */

#include <stdint.h>
#include <stdbool.h>
#include "tee_internal_api.h"
#include "tee_internal_se_api.h"

#define QC_TEE_SE_WTX 0x00000006 /* QCOM: Waiting for R-APDU after sending WTX Response */

/**
 *
 * The TEE_SEChannelTransmit_ext function transmits an C-APDU to the secure
 * element and receives generated R-APDU from the secure. The API performs
 * similar function to TEE_SEChannelTransmit with the added capability of
 * allowing SELECT by DF Name command to be successfully transmitted which is
 * not allowed by the standard API.
 *
 * @param[in]  seChannelHandle: Reference to seChannel hand
 * @param[in]  command        : Input Buffer containing the command to send as a byte array
 * @param[in]  commandLen     : Size for the command buffer which needs to be big enough to
 *                              allow secure messaging wrapping inside the provided buffer
 * @param[in,out] response    : Output buffer containing the response as a byte array
 * @param[in,out] responseLen : Size of the response buffer which needs to be big enough to
 *                              allow secure messaging unwrapping inside the provided buffer
 * @param[in]  allowSelectByDF: Allow sending SELECT by DF name
 *
 * @return   TEE_SUCCESS      : In case operation is successful. Otherwise @see TEE_SEChannelTransmit
 *
 **/
TEE_Result TEE_SEChannelTransmit_ext(TEE_SEChannelHandle seChannelHandle,
                                     void *              command,
                                     uint32_t            commandLen,
                                     void *              response,
                                     uint32_t *          responseLen,
                                     bool                allowSelectByDF);

/**
 *
 * Get the current number of open sessions
 *
 * @param    [in]  seReaderHandle      : Reference to seReader handle
 * @param    [out] numOfSessions       : Reference to write the number of sessions of success
 *
 * @return   TEE_SUCCESS               : In case operation is successful.
 * @return   TEE_ERROR_BAD_STATE       : If the seSession is closed
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 **/
TEE_Result TEE_SEReaderGetSessionCount_ext(TEE_SEReaderHandle seReaderHandle,
                                           uint32_t *         numOfSessions);
/*
 * Returns with status word resulting from the message exchange with Secure Element
 *
 * @param   [in]     seReaderHandle : Reference to Reader handle
 * @param   [out]    response       : Response buffer pointer to return SELECT response.
 * @param   [in,out] responseLen    : Select response buffer length
 *
 * @return  TEE_SUCCESS             : In case of success.
 * @return  TEE_ERROR_COMMUNICATION : If the SE is not present or if there is an I/O error
 * @return  TEE_ERROR_BAD_STATE     : If the seSession is closed
 */
TEE_Result TEE_SEReaderGetStatus_ext(TEE_SEReaderHandle seReaderHandle,
                                     void *             response,
                                     uint32_t *         responseLen);

/**
 *
 * Function returns the channel number given the channel handle
 *
 * @param    [in] seChannelHandle : Reference to seChannel handle
 * @param    [out] channelNumber  : pointer to store the channel number
 *
 * @return   TEE_SUCCESS               : In case operation is successful.
 * @return   TEE_ERROR_BAD_STATE       : If the seSession for the channel handle is closed
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 **/
TEE_Result TEE_SEChannelGetNumber_ext(TEE_SEChannelHandle seChannelHandle, uint32_t *channelNumber);

/**
 *
 * Function returns the session handle to the particular channel handle
 *
 * @param    [in] seChannelHandle  : Reference to seChannel handle
 * @param    [out] seSessionHandle : reference to write the session handle into
 *
 * @return   TEE_SUCCESS               : In case operation is successful.
 * @return   TEE_ERROR_BAD_STATE       : If the seSession for the channel handle is closed
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 **/
TEE_Result TEE_SEChannelGetSession_ext(TEE_SEChannelHandle  seChannelHandle,
                                       TEE_SESessionHandle *seSessionHandle);

#endif /* ifndef TEE_INTERNAL_SE_API_EXT_H */
