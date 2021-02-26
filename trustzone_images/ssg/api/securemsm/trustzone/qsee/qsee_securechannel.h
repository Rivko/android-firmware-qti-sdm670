#ifndef QSEE_SECURECHANNEL_H
#define QSEE_SECURECHANNEL_H

/**
@file qsee_securechannel.h
@brief These APIs are used by TZ applications to secure outgoing messages 
       and authenticate incoming messages.
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_securechannel.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
16/01/13   sm       updated secure channel APIs with client ID
10/12/11   jay      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/* SubSystem IDs that can establish a secure channel with TZ */
typedef enum
{
  QSEE_SC_TZ=0,                  /* Trustzone.                                  */ 
  QSEE_SC_MPSS,                  /* Modem subsystem.                            */
  QSEE_SC_LPASS,                 /* Low power audio subsystem.                  */
  QSEE_SC_WCNSS,                 /* Wireless conectivity subsystem.             */
  QSEE_SC_COUNT,                 /* Number of supported SS for SC.              */
  QSEE_SC_UNSUPPORTED_SS=0x7F    /* Subsystem not supported.                    */
}qsee_sc_ss_e_type;

/** 
    Client IDs that can establish a secure channel.
	Note: while adding enum value below.Please add number for the use case,
	So that it won't mismatch with the field values on modem side.
*/
typedef enum
{
  QSEE_SC_CID_SSM=0,                 /* Trustzone.                                  */ 
  QSEE_SC_CID_UIM=0x1,                   /* Modem subsystem.                            */
  QSEE_SC_CID_DATA_QMI=0x2,              /* Qmi client inside modem                     */
  QSEE_SC_CID_TLOC=0x3,                  /* Trusted Location                            */
  QSEE_SC_CID_COUNT=0x4,                 /* Number of supported SS for SC.              */
  QSEE_SC_UNSUPPORTED_CID=0x7F       /* Subsystem not supported.                    */
}qsee_sc_cid_e_type;

/** 
  @brief 
  This is used to secure the the input message
   
  @details
  The output buffer should be large enough to hold the encrypted message 
  and some internal headers and possible padding. Recommended output buffer size 
  is atleast input_msg_len + 100 bytes. The memory has to be managed by the caller.
   
  @param[in] ssid                    Subsystem ID
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the plaintext data.
  @param[in] input_msg_len           Length of the plaintext in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the secure blob 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the secure blob by the function.

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
int qsee_secure_message
(
  qsee_sc_ss_e_type      ssid,
  qsee_sc_cid_e_type     cid,
  const uint8_t*           input_msg_ptr,
  uint32_t                 input_msg_len,
  uint8_t*                 output_msg_ptr,
  uint32_t*                output_msg_len_ptr
);

/** 
  @brief  
  This is used to authenticate and decrypt the secure blob
   
  @details 
  The output buffer should be large enough to hold the decrypted message 
  Recommended output buffer size is atleast input_msg_len. 
  The memory has to be managed by the caller.
   
  @param[in] ssid                    Subsystem ID
  @param[in] cid                     client ID
  @param[in] input_msg_ptr           Pointer to the secure blob.
  @param[in] input_msg_len           Length of the secure blob in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the decrypted data 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the decrypted data on return.

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
int qsee_authenticate_decrypt_message
(
  qsee_sc_ss_e_type      ssid,
  qsee_sc_cid_e_type     cid,
  const uint8_t*           input_msg_ptr,
  uint32_t                 input_msg_len,
  uint8_t*                 output_msg_ptr,
  uint32_t*                output_msg_len_ptr
);

#endif /* QSEE_SECURECHANNEL_H */

