/*-----------------------------------------------------------------------
   Copyright (c) 2015 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/
#ifndef ADSP_DCVS_H_
#define ADSP_DCVS_H_

#include "qurt_qdi_constants.h"

/**
 *  @file adsp_dcvs.h
 *  @brief header for ADSP DCVS APIs for clients
 */

#ifndef _BOOLEAN_DEFINED
typedef unsigned char       boolean;
#define _BOOLEAN_DEFINED
#endif

/** DCVS QDI device name */
#define ADSP_DCVS_DEV_NAME              "/adsp/dcvs"
/** DCVS register client QDI method number */
#define DCVS_REGISTER_CLIENT            (QDI_PRIVATE+0)
/** DCVS de-register client QDI method number */
#define DCVS_DEREGISTER_CLIENT          (QDI_PRIVATE+1)
/** DCVS client request QDI method number */
#define DCVS_CLIENT_REQUEST             (QDI_PRIVATE+2)

/** @enum ADSPDCVS_RETURN
 *  @brief ADSP DCVS API return values */
typedef enum
{
    ADSP_DCVS_SUCCESS = 0,
    ADSP_DCVS_MALLOC_FAILURE = -1,
    ADSP_DCVS_CLIENT_NOT_FOUND = -2,
    ADSP_DCVS_CLIENT_ID_INVALID = -3,
    ADSP_DCVS_REGISTER_FAILED = -4,
    ADSP_DCVS_CLIENT_INIT_FAILED = -5,
    ADSP_DCVS_CLIENT_INIT_ALREADY = -6,
    ADSP_DCVS_CLIENT_INIT_NOT_DONE = -7,
    ADSP_DCVS_CLIENT_ID_NULL = -8,
    ADSP_DCVS_CLIENT_INVALID_PARAMS = -9,
} ADSPDCVS_RETURN;

/** @enum dcvs_policy_t
 *  @brief DCVS policies */
typedef enum
{
    ADSP_DCVS_ADJUST_UP_DOWN = 0x1,
    ADSP_DCVS_ADJUST_ONLY_UP = 0x2,
    ADSP_DCVS_POWER_SAVER_MODE = 0x4,
    ADSP_DCVS_POWER_SAVER_AGGRESSIVE_MODE = 0x8,
    ADSP_DCVS_PERFORMANCE_MODE = 0x10,
    ADSP_DCVS_DUTY_CYCLE_MODE = 0x20,
} dcvs_policy_t;

/** @enum dcvs_voltage_corner_t
 *  @brief Voltage corners supported in DCVS vote for min, max and target
 *         corners */
typedef enum {
    ADSP_DCVS_VCORNER_DISABLE = 0,
    ADSP_DCVS_VCORNER_SVS2 = 1,
    ADSP_DCVS_VCORNER_SVS = 2,
    ADSP_DCVS_VCORNER_SVSPLUS = 3,
    ADSP_DCVS_VCORNER_NOM = 4,
    ADSP_DCVS_VCORNER_NOMPLUS = 5,
    ADSP_DCVS_VCORNER_TURBO = 6,
    ADSP_DCVS_VCORNER_NUM_LEVELS
} dcvs_voltage_corner_t;

/** @struct dcvs_params_t
 *  @brief DCVS corner voting structure */
typedef struct {
    dcvs_voltage_corner_t target_corner;
    /**< Client's request for the target voltage corner */

    dcvs_voltage_corner_t min_corner;
    /**< Client's request for minimum voltage corner */

    dcvs_voltage_corner_t max_corner;
    /**< Client's request for maximum voltage corner */

    unsigned int param1;
    /**< Reserved */

    unsigned int param2;
    /**< Reserved */

    unsigned int param3;
    /**< Reserved */
} dcvs_params_t;

/** @struct adsp_dcvs_request_t
 *  @brief DCVS request structure */
typedef struct
{
    boolean dcvs_enable;
    /**< TRUE for DCVS enable and FALSE for DCVS disable */

    dcvs_policy_t dcvs_policy;
    /**< Requested DCVS policy in case DCVS enable is TRUE */

    boolean set_latency;
    /**< TRUE if sleep latency request has to be considered */

    unsigned int latency;
    /**< Sleep latency request in micro seconds */

    boolean set_dcvs_params;
    /**< Flag to mark DCVS params structure validity, TRUE for valid DCVS
     *   params request and FALSE otherwise */

    dcvs_params_t dcvs_params;
    /**< Intended DCVS params if set_dcvs_params is set to TRUE */
} adsp_dcvs_request_t;

/**************************************************************************//**
 * @fn adsp_dcvs_client_register
 *
 * @brief Registers a client with DCVS and returns a client id for client's
 * requests
 *
 * @return unsigned int client_id (> 0) - for success
 * @return unsigned int 0 - for failure
 *****************************************************************************/
unsigned int adsp_dcvs_client_register(void);

/**************************************************************************//**
 * @fn adsp_dcvs_client_deregister
 *
 * @brief Removes registered client with the given client id and
 * aggregates DCVS modes and votes across clients
 *
 * @param client_id Client ID to be de-registered
 *
 * @return ADSPDCVS_RETURN ADSP_DCVS_SUCCESS for success
 * @return ADSPDCVS_RETURN other values indicating failure reason
 *****************************************************************************/
ADSPDCVS_RETURN adsp_dcvs_client_deregister(unsigned int client_id);

/**************************************************************************//**
 * @fn adsp_dcvs_client_request
 *
 * @brief A registered client with the given client id can request for
 * DCVS parameters using the given request structure.
 *
 * @param client_id client id (return value of register API)
 * @param request Pointer to the DCVS request structure
 *
 * @return ADSPDCVS_RETURN ADSP_DCVS_SUCCESS for success
 * @return ADSPDCVS_RETURN other values indicating failure reason
 *****************************************************************************/
ADSPDCVS_RETURN adsp_dcvs_client_request(unsigned int client_id,
                                         adsp_dcvs_request_t* request);

#endif /*ADSP_DCVS_H_*/
