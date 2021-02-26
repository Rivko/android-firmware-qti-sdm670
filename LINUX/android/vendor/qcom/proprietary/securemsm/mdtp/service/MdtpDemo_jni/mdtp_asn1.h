#ifndef MDTP_ASN1_H
#define MDTP_ASN1_H

/** @file mdtp_asn1.h
 * @brief
 * This file contains the definitions of the MDTP ASN1 primitives.
 */

/*===========================================================================
  Copyright (c) 2015-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

 EDIT HISTORY FOR FILE

 when         who      what, where, why
 ----------   ---      -------------------------------------------------------
 12/26/2016   ysreekan Removed Firmware lock related sources

 ===========================================================================*/

#include "openssl/asn1t.h"
#include "openssl/ossl_typ.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define SIGNER_OK 1
#define SIGNER_FAIL 0

/** MDTP message type. */
typedef enum {
    MDTP_SET_STATE = 0,
    MDTP_SET_LOCK,
    MDTP_SET_KILL,
    MDTP_MSG_LAST,
    MDTP_SET_SIZE = 0x7FFFFFFF
} mdtp_msg_type_t;

typedef struct mdtp_buf {
	uint32_t len;
	uint8_t *data;
} mdtp_buf_t;

typedef struct command_set_mdtp_state
{
  ASN1_ENUMERATED *mdtpState;
  ASN1_OCTET_STRING *isvDeviceId;
  ASN1_OCTET_STRING *isvDeactivationPin;
  ASN1_OCTET_STRING *isvFriendlyName;
  ASN1_BIT_STRING *subsystemSelection;
} COMMAND_SET_MDTP_STATE;

DECLARE_ASN1_FUNCTIONS(COMMAND_SET_MDTP_STATE)

typedef struct command_set_lock_state
{
  ASN1_ENUMERATED *lockState;
  ASN1_BIT_STRING *subsystemSelection;
} COMMAND_SET_LOCK_STATE;

DECLARE_ASN1_FUNCTIONS(COMMAND_SET_LOCK_STATE)

typedef struct command_set_kill_state
{
  ASN1_ENUMERATED *killState;
  ASN1_BIT_STRING *killFunctionality;
} COMMAND_SET_KILL_STATE;

DECLARE_ASN1_FUNCTIONS(COMMAND_SET_KILL_STATE)

typedef struct mdtp_v2_command
{
  int type;
  union   {
    COMMAND_SET_MDTP_STATE* commandSetMdtpState;
    COMMAND_SET_LOCK_STATE* commandSetLockState;
    COMMAND_SET_KILL_STATE* commandSetKillState;
    } command;
} MDTP_V2_COMMAND;

DECLARE_ASN1_FUNCTIONS(MDTP_V2_COMMAND)

typedef struct mdtp_v2_message
{
  ASN1_OCTET_STRING *idToken;
  ASN1_ENUMERATED *messageType;
  MDTP_V2_COMMAND *mdtpV2command;
} MDTP_V2_MESSAGE;

DECLARE_ASN1_FUNCTIONS(MDTP_V2_MESSAGE)

typedef struct wrapped_mdtp_message
{
  ASN1_INTEGER *version;
  ASN1_OCTET_STRING *mdtpMessage;
} WRAPPER_MDTP_MESSAGE;

DECLARE_ASN1_FUNCTIONS(WRAPPER_MDTP_MESSAGE)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * mdtp_build_set_state_msg
 *
 * Create an ASN1 octet string holding the entire set state message payload
 *
 * @return - 0 for an error, positive for success.
 */
int mdtp_build_set_state_msg(uint8_t active, uint8_t simLockSelect,
							 mdtp_buf_t *idToken, mdtp_buf_t *isv_pin, mdtp_buf_t *isv_deviceid, mdtp_buf_t *isv_friendlyname, mdtp_buf_t *msg);

/**
 * mdtp_build_set_lock_msg
 *
 * Create an ASN1 octet string holding the entire set state message payload
 *
 * @return - 0 for an error, positive for success.
 */
int mdtp_build_set_lock_msg(uint8_t lock, uint8_t simLockSelect, mdtp_buf_t *idToken, mdtp_buf_t *msg);

/**
 * mdtp_build_set_kill_msg
 *
 * Create an ASN1 octet string holding the entire set state message payload
 *
 * @return - 0 for an error, positive for success.
 */
int mdtp_build_set_kill_msg(uint8_t kill, uint8_t simSelect, mdtp_buf_t *idToken, mdtp_buf_t *msg);

#ifdef  __cplusplus
}
#endif

#endif /* MDTP_ASN1_H */
