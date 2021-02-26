#ifndef MDTP_H
#define MDTP_H

/** @file mdtp.h
 * @brief
 * This file contains the definitions of the constants, data structures
 * and interfaces for the MDTP module.
 */
/*===========================================================================

Copyright (c) 2015-2017 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

 ===========================================================================*/

/*===========================================================================

 EDIT HISTORY FOR FILE

 when         who      what, where, why
 ----------   ---      -------------------------------------------------------
 12/26/2016   ysreekan Removed Firmware lock related sources
 02/21/2016   c_amirk  Updated PIN status responds.
 02/14/2016   ablay    Extend the get_state API.
 07/22/2015   mdgreen  Add support for PIN validation lockout.
 02/11/2015   ablay    Add support for ISV CA CRLs.
 12/02/2014   ablay    Added block mapping and FOTA support.
 11/04/2014   ablay    API rev B.
 06/10/2014   ablay    API rev A.

 ===========================================================================*/

/*----------------------------------------------------------------------------
 * Macro Declarations
 * -------------------------------------------------------------------------*/

#define MDTP_ID_TOKEN_LEN                  (16)
#define MDTP_PIN_LEN                       (8)
#define MDTP_MAX_MSG_LEN                   (4096)
#define MDTP_MAX_CRL_LEN                   (2048)
#define MDTP_MAX_CERT_LEN                  (2048)
#define MDTP_MAX_DIP_LEN                   (1024*128)
#define MDTP_MAX_RPMB_LEN                  (512)
#define MDTP_MAX_CRLS                      (20)
#define MDTP_FWLOCK_BLOCK_SIZE             (1024*1024*16)
#define MDTP_FWLOCK_MAX_BLOCKS             (512)
#define MDTP_FWLOCK_MAX_FILES              (100)
#define MDTP_FWLOCK_MAX_FILE_NAME_LEN      (100)
#define MDTP_FWLOCK_MAX_PARTITIONS         (3)
#define MDTP_FWLOCK_MAX_PARTITION_NAME_LEN (100)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

#pragma pack(push, mdtp, 1)

/** MDTP error types. */
typedef enum {
    MDTP_SUCCESS = 0,                 /**< Success error code. */
    MDTP_FAILURE,                     /**< General failure error code. */
    MDTP_MSG_VERIFICATION_FAILURE,    /**< Signature verification failure */
	MDTP_PIN_VALIDATION_FAILURE,      /**< PIN validation failure */
	MDTP_PIN_LOCKOUT_FAILURE,         /**< PIN lockout failure */
    MDTP_ERR_SIZE = 0x7FFFFFFF
} mdtp_status_t;

/** MDTP system state. */
typedef enum {
    MDTP_STATE_DISABLED,         /**<  MDTP was never activated on the device. */
    MDTP_STATE_INACTIVE,         /**<  MDTP is not activated on the device. */
    MDTP_STATE_ACTIVE,           /**<  MDTP is activated on the device. */
    MDTP_STATE_TAMPERED,         /**<  MDTP is tampered on the device. */
    MDTP_STATE_INVALID,          /**<  MDTP is invalid on the device. */
    MDTP_STATE_SIZE = 0x7FFFFFFF
} mdtp_system_state_t;

/** MDTP applicative state. */
typedef union mdtp_app_state
{
    struct {
        uint32 sim_locked     : 1;    /**< SIM is locked by MDTP */
        uint32 emergency_only : 1;    /**< Emergency only mdoe is enabled by MDTP */
    };
    uint32 value;
} mdtp_app_state_t;

/** MDTP version. */
typedef struct mdtp_version {
    uint32 lib_version;   /**< MDTP version number for MDTP lib. */
    uint32 tz_version;   /**< MDTP version number for the TZ component. */
} mdtp_version_t;

/** MDTP PIN. */
typedef struct mdtp_pin {
    char data[MDTP_PIN_LEN+1];  /**< A null terminated PIN. */
} mdtp_pin_t;

/** MDTP sized buffer type. */
typedef struct mdtp_buffer {
    size_t len;             /**< Buffer length in bytes. */
    unsigned char* data;    /**< Buffer data. */
} mdtp_buffer_t;

/** MDTP identification token. */
typedef struct mdtp_id_token {
    unsigned char data[MDTP_ID_TOKEN_LEN];   /**< UUID representing MDTP activation. */
} mdtp_id_token_t;

#pragma pack(pop, mdtp)

/*----------------------------------------------------------------------------
 * MDTP API functions
 * -------------------------------------------------------------------------*/

/*****************************************************************************************************************/
/* MDTP management API */
/*****************************************************************************************************************/

/**
  Initializes the MDTP library. This will initialize system resources needed by the MDTP library.
  This must be done before calling any other MDTP API function.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_initialize();

/**
  Shutdowns the MDTP library. This will free system resources used by the MDTP library.
  After calling this function, further calls to MDTP API will requires a call to mdtp_initialize() first.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_shutdown();

/**
  Returns the current states of the MDTP.
  MDTP system state: Either one of: DISABLED/INACTIVE/ACTIVE/TAMPERED/INVALID.
  MDTP applicative state: SIMLOCKED: true/false, EmergencyOnly: true/false.

  @param[out]  system_state  MDTP system state. \n
  @param[out]  app_state  MDTP applicative state. \n

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_get_state(mdtp_system_state_t *system_state, mdtp_app_state_t *app_state);

/**
  Returns the version of the MDTP library.

  @param[out]  mdtp_version  Current version of the MDP library.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_get_version(mdtp_version_t* mdtp_version);

/**
  Process a signed message sent to device by MDTP services. Signed messages are used for sensitive operations,
  such as MDTP activation, deactivation, etc. The structure of the content and the different messages supported
  by the MDTP is described in "MDTP Interface Specification" document. The function will process the message,
  validate its authenticity, and execute the command it carries.

  @param[in]    signed_msg signed message.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_process_signed_msg(mdtp_buffer_t* signed_msg);

/**
  Deactivates the MDTP protection, using local pin authentication, instead of a signed deactivation message arriving
  from MDTP services.  The pin is generated in the cloud, and received as part of a signed activation message.
  during MDTP activation. @see mdtp_process_signed_msg.

  @param[in]  mdtp_pin  PIN of MDTP_PIN_LEN digits.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_deactivate_local(mdtp_pin_t* mdtp_pin);

/**
  Get an identity token to uniquely identify a specific instance of MDTP activation/deactivation.
  The API will be available only When MDTP state is inactive. On each call, a new unique token shall
  be generated and persisted.
  Upon MDTP deactivation the identity token is cleared.

  @param[out]  id_token     16-byte token.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None.
*/
mdtp_status_t mdtp_get_id_token(mdtp_id_token_t* id_token);

/**
  Updates the MDTP with a given RFC5280 CRL

  @param[in]  crl  Certificate Revocation List (CRL), according to RFC5280.
  @param[in]  cert  The X509 certificate the CRL is signed with. This is to identify who signed the CRL (the root CA or its
                    subordinates CAs)

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None
*/
mdtp_status_t mdtp_update_crl(mdtp_buffer_t* crl, mdtp_buffer_t* cert);

/**
 * Returns the master PIN, used for recovering MDTP when RPMB is invalid.
 * This is available only when MDTP is not ACTIVATED.

  @param[out]  master_pin     8-digits PIN.

  @return
  @li @c MDTP_SUCCESS -- Success. \n
  @li @c MDTP_FAILURE -- Failure.

  @dependencies
  None.
 */
mdtp_status_t mdtp_get_master_pin(mdtp_pin_t* master_pin);

/*****************************************************************************************************************/

#endif /* MDTP_H */
