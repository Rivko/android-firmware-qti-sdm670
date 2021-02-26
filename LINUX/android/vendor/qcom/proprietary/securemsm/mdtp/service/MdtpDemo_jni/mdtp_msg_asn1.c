/*
 * @file mdtp_msg_asn1.c
 * @brief
 * MDTP message ASN1 parser implementation.
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

#define LOG_TAG "mdtp_msg_asn1"

#include <utils/Log.h>
#include "mdtp_asn1.h"

/*----------------------------------------------------------------------------
 * Global Variables
 * -------------------------------------------------------------------------*/

#define MDTP_ASN1_VERSION (2)

/* This is the ASN.1 definition of the MDTP message (MDTP API Spec RevB) */

ASN1_SEQUENCE(COMMAND_SET_MDTP_STATE) = {
		ASN1_SIMPLE(COMMAND_SET_MDTP_STATE, mdtpState, ASN1_ENUMERATED),
		ASN1_SIMPLE(COMMAND_SET_MDTP_STATE, isvDeviceId, ASN1_OCTET_STRING),
		ASN1_SIMPLE(COMMAND_SET_MDTP_STATE, isvDeactivationPin, ASN1_OCTET_STRING),
		ASN1_SIMPLE(COMMAND_SET_MDTP_STATE, isvFriendlyName, ASN1_OCTET_STRING),
		ASN1_SIMPLE(COMMAND_SET_MDTP_STATE, subsystemSelection, ASN1_BIT_STRING)
} ASN1_SEQUENCE_END(COMMAND_SET_MDTP_STATE)

IMPLEMENT_ASN1_FUNCTIONS(COMMAND_SET_MDTP_STATE)

ASN1_SEQUENCE(COMMAND_SET_LOCK_STATE) = {
		ASN1_SIMPLE(COMMAND_SET_LOCK_STATE, lockState, ASN1_ENUMERATED),
		ASN1_SIMPLE(COMMAND_SET_LOCK_STATE, subsystemSelection, ASN1_BIT_STRING)
} ASN1_SEQUENCE_END(COMMAND_SET_LOCK_STATE)

IMPLEMENT_ASN1_FUNCTIONS(COMMAND_SET_LOCK_STATE)

ASN1_SEQUENCE(COMMAND_SET_KILL_STATE) = {
		ASN1_SIMPLE(COMMAND_SET_KILL_STATE, killState, ASN1_ENUMERATED),
		ASN1_SIMPLE(COMMAND_SET_KILL_STATE, killFunctionality, ASN1_BIT_STRING)
} ASN1_SEQUENCE_END(COMMAND_SET_KILL_STATE)

IMPLEMENT_ASN1_FUNCTIONS(COMMAND_SET_KILL_STATE)

ASN1_CHOICE(MDTP_V2_COMMAND) = {
		ASN1_EXP(MDTP_V2_COMMAND, command.commandSetMdtpState, COMMAND_SET_MDTP_STATE, 0),
		ASN1_EXP(MDTP_V2_COMMAND, command.commandSetLockState, COMMAND_SET_LOCK_STATE, 1),
		ASN1_EXP(MDTP_V2_COMMAND, command.commandSetKillState, COMMAND_SET_KILL_STATE, 2)
} ASN1_CHOICE_END(MDTP_V2_COMMAND)

IMPLEMENT_ASN1_FUNCTIONS(MDTP_V2_COMMAND)

ASN1_SEQUENCE(MDTP_V2_MESSAGE) = {
		ASN1_SIMPLE(MDTP_V2_MESSAGE, idToken, ASN1_OCTET_STRING),
		ASN1_SIMPLE(MDTP_V2_MESSAGE, messageType, ASN1_ENUMERATED),
		ASN1_SIMPLE(MDTP_V2_MESSAGE, mdtpV2command, MDTP_V2_COMMAND)
} ASN1_SEQUENCE_END(MDTP_V2_MESSAGE)

IMPLEMENT_ASN1_FUNCTIONS(MDTP_V2_MESSAGE)

ASN1_SEQUENCE(WRAPPER_MDTP_MESSAGE) = {
		ASN1_SIMPLE(WRAPPER_MDTP_MESSAGE, version, ASN1_INTEGER),
		ASN1_SIMPLE(WRAPPER_MDTP_MESSAGE, mdtpMessage, ASN1_OCTET_STRING),
} ASN1_SEQUENCE_END(WRAPPER_MDTP_MESSAGE)

IMPLEMENT_ASN1_FUNCTIONS(WRAPPER_MDTP_MESSAGE)

/*----------------------------------------------------------------------------
 * Local Functions
 * -------------------------------------------------------------------------*/

/**
 * i2d_WRAPPER_MDTP_MESSAGE_bio
 *
 * Write a WRAPPER_MDTP_MESSAGE ASN1 structure into a given BIO
 *
 * @return - 0 - fail, 1 - success
 */
static int i2d_WRAPPER_MDTP_MESSAGE_bio(BIO *bp, WRAPPER_MDTP_MESSAGE *wrapperMdtpMsg)
{
	return ASN1_item_i2d_bio(ASN1_ITEM_rptr(WRAPPER_MDTP_MESSAGE), bp, wrapperMdtpMsg);
}

/**
 * i2d_MDTP_V2_MESSAGE_bio
 *
 * Write a MDTP_V2_MESSAGE ASN1 structure into a given BIO
 *
 * @return - 0 - fail, 1 - success
 */
static int i2d_MDTP_V2_MESSAGE_bio(BIO *bp, MDTP_V2_MESSAGE *mdtpV2Message)
{
	return ASN1_item_i2d_bio(ASN1_ITEM_rptr(MDTP_V2_MESSAGE), bp, mdtpV2Message);
}

/*----------------------------------------------------------------------------
 * External Functions
 * -------------------------------------------------------------------------*/
/**
 * mdtp_build_set_state_msg
 *
 * Create an ASN1 octet string holding the entire set state message payload
 *
 * @return - 0 for an error, positive for success.
 */
int mdtp_build_set_state_msg(uint8_t active, uint8_t simLockSelect,
							 mdtp_buf_t *idToken, mdtp_buf_t *isv_pin, mdtp_buf_t *isv_deviceid, mdtp_buf_t *isv_friendlyname, mdtp_buf_t *msg) {

	BIO* msg_bio = NULL;
	BIO* msg_v2_bio = NULL;
	mdtp_buf_t msg_v2_buf = {0,0};
	WRAPPER_MDTP_MESSAGE* wrapper_mdt_msg = NULL;
	MDTP_V2_MESSAGE* mdtp_v2_msg = NULL;
	int status = SIGNER_FAIL;
	uint8_t firmwareLockSelect = 1;

	wrapper_mdt_msg = WRAPPER_MDTP_MESSAGE_new();
	if (!wrapper_mdt_msg) {
		ALOGD("WRAPPER_MDTP_MESSAGE_new failed");
		return SIGNER_FAIL;
	}

	do {
		mdtp_v2_msg = MDTP_V2_MESSAGE_new();
		if (!mdtp_v2_msg) {
			ALOGD("MDTP_V2_MESSAGE_new failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(mdtp_v2_msg->idToken, idToken->data, idToken->len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_ENUMERATED_set(mdtp_v2_msg->messageType, MDTP_SET_STATE)) {
			ALOGD("ASN1_ENUMERATED_set() failed");
			break;
		}

		mdtp_v2_msg->mdtpV2command->type = MDTP_SET_STATE;

		mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState = COMMAND_SET_MDTP_STATE_new();
		if (!mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState)
		{
			ALOGD("COMMAND_SET_MDTP_STATE_new failed");
			break;
		}

		if (SIGNER_OK != ASN1_ENUMERATED_set(mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->mdtpState, active)) {
			ALOGD("ASN1_ENUMERATED_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_BIT_STRING_set_bit(
				mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->subsystemSelection, 0, firmwareLockSelect)) {
			ALOGD("ASN1_BIT_STRING_set_bit() failed");
			break;
		}

		if (SIGNER_OK != ASN1_BIT_STRING_set_bit(
				mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->subsystemSelection, 1, simLockSelect)) {
			ALOGD("ASN1_BIT_STRING_set_bit() failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->isvDeviceId, isv_deviceid->data, isv_deviceid->len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->isvDeactivationPin, isv_pin->data, isv_pin->len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->isvFriendlyName, isv_friendlyname->data, isv_friendlyname->len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		mdtp_v2_msg->mdtpV2command->command.commandSetMdtpState->subsystemSelection->flags = ASN1_STRING_FLAG_BITS_LEFT | 6;

		if (SIGNER_OK != ASN1_INTEGER_set(wrapper_mdt_msg->version, MDTP_ASN1_VERSION)) {
			ALOGD("ASN1_INTEGER_set() failed");
			break;
		}

		msg_v2_bio = BIO_new(BIO_s_mem());
		if (!i2d_MDTP_V2_MESSAGE_bio(msg_v2_bio, mdtp_v2_msg)) {
			ALOGD("i2d_MDTP_V2_MESSAGE_bio() failed");
			break;
		}

		msg_v2_buf.len = BIO_get_mem_data(msg_v2_bio, &msg_v2_buf.data);
		if (!msg_v2_buf.len || !msg_v2_buf.data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(wrapper_mdt_msg->mdtpMessage, msg_v2_buf.data, msg_v2_buf.len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		msg_bio = BIO_new(BIO_s_mem());
		if (!i2d_WRAPPER_MDTP_MESSAGE_bio(msg_bio, wrapper_mdt_msg)) {
			ALOGD("i2d_WRAPPER_MDTP_MESSAGE_bio() failed");
			break;
		}

		msg->len = BIO_get_mem_data(msg_bio, &msg->data);
		if (!msg->len || !msg->data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}
		status = SIGNER_OK;
	} while(0);

	WRAPPER_MDTP_MESSAGE_free(wrapper_mdt_msg);
	MDTP_V2_MESSAGE_free(mdtp_v2_msg);
	BIO_free_all(msg_v2_bio);
	return status;
}

/**
 * mdtp_build_set_lock_msg
 *
 * Create an ASN1 octet string holding the entire set state message payload
 *
 * @return - 0 for an error, positive for success.
 */
int mdtp_build_set_lock_msg(uint8_t lock, uint8_t simLockSelect, mdtp_buf_t *idToken, mdtp_buf_t *msg) {

	BIO* msg_bio = NULL;
	BIO* msg_v2_bio = NULL;
	mdtp_buf_t msg_v2_buf = {0,0};
	WRAPPER_MDTP_MESSAGE* wrapper_mdt_msg = NULL;
	MDTP_V2_MESSAGE* mdtp_v2_msg = NULL;
	int status = SIGNER_FAIL;
	uint8_t firmwareLockSelect = 1;

	wrapper_mdt_msg = WRAPPER_MDTP_MESSAGE_new();
	if (!wrapper_mdt_msg) {
		ALOGD("WRAPPER_MDTP_MESSAGE_new failed");
		return SIGNER_FAIL;
	}

	do {
		mdtp_v2_msg = MDTP_V2_MESSAGE_new();
		if (!mdtp_v2_msg) {
			ALOGD("MDTP_V2_MESSAGE_new failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(mdtp_v2_msg->idToken, idToken->data, idToken->len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_ENUMERATED_set(mdtp_v2_msg->messageType, MDTP_SET_LOCK)) {
			ALOGD("ASN1_ENUMERATED_set() failed");
			break;
		}

		mdtp_v2_msg->mdtpV2command->type = MDTP_SET_LOCK;

		mdtp_v2_msg->mdtpV2command->command.commandSetLockState = COMMAND_SET_LOCK_STATE_new();
		if (!mdtp_v2_msg->mdtpV2command->command.commandSetLockState)
		{
			ALOGD("COMMAND_SET_MDTP_STATE_new failed");
			break;
		}

		if (SIGNER_OK != ASN1_ENUMERATED_set(mdtp_v2_msg->mdtpV2command->command.commandSetLockState->lockState, lock)) {
			ALOGD("ASN1_ENUMERATED_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_BIT_STRING_set_bit(
				mdtp_v2_msg->mdtpV2command->command.commandSetLockState->subsystemSelection, 0, firmwareLockSelect)) {
			ALOGD("ASN1_BIT_STRING_set_bit() failed");
			break;
		}

		if (SIGNER_OK != ASN1_BIT_STRING_set_bit(
				mdtp_v2_msg->mdtpV2command->command.commandSetLockState->subsystemSelection, 1, simLockSelect)) {
			ALOGD("ASN1_BIT_STRING_set_bit() failed");
			break;
		}

		mdtp_v2_msg->mdtpV2command->command.commandSetLockState->subsystemSelection->flags = ASN1_STRING_FLAG_BITS_LEFT | 6;

		if (SIGNER_OK != ASN1_INTEGER_set(wrapper_mdt_msg->version, MDTP_ASN1_VERSION)) {
			ALOGD("ASN1_INTEGER_set() failed");
			break;
		}

		msg_v2_bio = BIO_new(BIO_s_mem());
		if (!i2d_MDTP_V2_MESSAGE_bio(msg_v2_bio, mdtp_v2_msg)) {
			ALOGD("i2d_MDTP_V2_MESSAGE_bio() failed");
			break;
		}

		msg_v2_buf.len = BIO_get_mem_data(msg_v2_bio, &msg_v2_buf.data);
		if (!msg_v2_buf.len || !msg_v2_buf.data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(wrapper_mdt_msg->mdtpMessage, msg_v2_buf.data, msg_v2_buf.len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		msg_bio = BIO_new(BIO_s_mem());
		if (!i2d_WRAPPER_MDTP_MESSAGE_bio(msg_bio, wrapper_mdt_msg)) {
			ALOGD("i2d_WRAPPER_MDTP_MESSAGE_bio() failed");
			break;
		}

		msg->len = BIO_get_mem_data(msg_bio, &msg->data);
		if (!msg->len || !msg->data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}
		status = SIGNER_OK;
	} while(0);

	WRAPPER_MDTP_MESSAGE_free(wrapper_mdt_msg);
	MDTP_V2_MESSAGE_free(mdtp_v2_msg);
	BIO_free_all(msg_v2_bio);
	return status;
}

/**
 * mdtp_build_set_kill_msg
 *
 * Create an ASN1 octet string holding the entire set state message payload
 *
 * @return - 0 for an error, positive for success.
 */
int mdtp_build_set_kill_msg(uint8_t kill, uint8_t simSelect, mdtp_buf_t *idToken, mdtp_buf_t *msg) {

	BIO* msg_bio = NULL;
	BIO* msg_v2_bio = NULL;
	mdtp_buf_t msg_v2_buf = {0,0};
	WRAPPER_MDTP_MESSAGE* wrapper_mdt_msg = NULL;
	MDTP_V2_MESSAGE* mdtp_v2_msg = NULL;
	int status = SIGNER_FAIL;

	wrapper_mdt_msg = WRAPPER_MDTP_MESSAGE_new();
	if (!wrapper_mdt_msg) {
		ALOGD("WRAPPER_MDTP_MESSAGE_new failed");
		return SIGNER_FAIL;
	}

	do {
		mdtp_v2_msg = MDTP_V2_MESSAGE_new();
		if (!mdtp_v2_msg) {
			ALOGD("MDTP_V2_MESSAGE_new failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(mdtp_v2_msg->idToken, idToken->data, idToken->len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_ENUMERATED_set(mdtp_v2_msg->messageType, MDTP_SET_KILL)) {
			ALOGD("ASN1_ENUMERATED_set() failed");
			break;
		}

		mdtp_v2_msg->mdtpV2command->type = MDTP_SET_KILL;

		mdtp_v2_msg->mdtpV2command->command.commandSetKillState = COMMAND_SET_KILL_STATE_new();
		if (!mdtp_v2_msg->mdtpV2command->command.commandSetKillState)
		{
			ALOGD("COMMAND_SET_MDTP_STATE_new failed");
			break;
		}

		if (SIGNER_OK != ASN1_ENUMERATED_set(mdtp_v2_msg->mdtpV2command->command.commandSetKillState->killState, kill)) {
			ALOGD("ASN1_ENUMERATED_set() failed");
			break;
		}

		if (SIGNER_OK != ASN1_BIT_STRING_set_bit(
				mdtp_v2_msg->mdtpV2command->command.commandSetKillState->killFunctionality, 0, simSelect)) {
			ALOGD("ASN1_BIT_STRING_set_bit() failed");
			break;
		}

		mdtp_v2_msg->mdtpV2command->command.commandSetKillState->killFunctionality->flags = ASN1_STRING_FLAG_BITS_LEFT | 7;

		if (SIGNER_OK != ASN1_INTEGER_set(wrapper_mdt_msg->version, MDTP_ASN1_VERSION)) {
			ALOGD("ASN1_INTEGER_set() failed");
			break;
		}

		msg_v2_bio = BIO_new(BIO_s_mem());
		if (!i2d_MDTP_V2_MESSAGE_bio(msg_v2_bio, mdtp_v2_msg)) {
			ALOGD("i2d_MDTP_V2_MESSAGE_bio() failed");
			break;
		}

		msg_v2_buf.len = BIO_get_mem_data(msg_v2_bio, &msg_v2_buf.data);
		if (!msg_v2_buf.len || !msg_v2_buf.data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}

		if (SIGNER_OK != ASN1_OCTET_STRING_set(wrapper_mdt_msg->mdtpMessage, msg_v2_buf.data, msg_v2_buf.len)) {
			ALOGD("ASN1_OCTET_STRING_set() failed");
			break;
		}

		msg_bio = BIO_new(BIO_s_mem());
		if (!i2d_WRAPPER_MDTP_MESSAGE_bio(msg_bio, wrapper_mdt_msg)) {
			ALOGD("i2d_WRAPPER_MDTP_MESSAGE_bio() failed");
			break;
		}

		msg->len = BIO_get_mem_data(msg_bio, &msg->data);
		if (!msg->len || !msg->data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}
		status = SIGNER_OK;
	} while(0);

	WRAPPER_MDTP_MESSAGE_free(wrapper_mdt_msg);
	MDTP_V2_MESSAGE_free(mdtp_v2_msg);
	BIO_free_all(msg_v2_bio);
	return status;
}
