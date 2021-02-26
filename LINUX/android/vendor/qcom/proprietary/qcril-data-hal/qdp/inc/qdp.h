/*!
  @file
  qdp.h

  @brief
  API exposed by Data Profile module

*/

/*===========================================================================

  Copyright (c) 2010-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/10   js      created file

===========================================================================*/

#ifndef __QDP_H__
#define __QDP_H__

#include "comdef.h"
#include "wireless_data_service_v01.h"

#define QDP_SUCCESS 1
#define QDP_FAILURE 0

/*
 * RIL.h specifies an array of parameters for
 * SETUP_DATA_CALL request
 * following enum values are detemined based off
 * the respective indices of the corresponding
 * params in RIL.h specified array of params
*/
typedef enum qdp_ril_param_idx_e
{
  QDP_RIL_TECH = 0,
  QDP_RIL_PROFILE_ID = 1, /* a valid profile id must be >= 1000 */
  QDP_RIL_APN = 2,
  QDP_RIL_NAI = 3,
  QDP_RIL_PASSWORD = 4,
  QDP_RIL_AUTH = 5,
  QDP_RIL_IP_FAMILY = 6,
  QDP_RIL_IP_ROAMING = 7,
  QDP_RIL_PARAM_MAX = 8
} qdp_ril_param_idx_t;

#define QDP_RIL_CLASS  8

/* Mask for each technology */
typedef enum qdp_tech_e
{
  QDP_NOTECH = 0x00,
  QDP_3GPP = 0x01,
  QDP_3GPP2 = 0x02,
  QDP_EPC = 0x03
} qdp_tech_t;

#define QDP_INVALID_PROFILE ((unsigned int)0xFFFFFFFF)

/* Error code for profile lookup */
typedef enum qdp_error_e
{
  QDP_ERROR_NONE,
  QDP_ERROR_ONLY_IPV4_ALLOWED,
  QDP_ERROR_ONLY_IPV6_ALLOWED,
  QDP_ERROR_MAX
} qdp_error_t;

/* Structure for lookup error information */
typedef struct qdp_error_info_s
{
  qdp_error_t  error;
  qdp_tech_t   tech;
} qdp_error_info_t;

#define QDP_PERSISTENT_PROFILES 16
#define QDP_NON_PERSISTENT_PROFILES 8
#define QDP_NUM_PROFILES_EXPECTED_MAX (QDP_PERSISTENT_PROFILES + QDP_NON_PERSISTENT_PROFILES)

/* RIL provided values */
#define QDP_RIL_3GPP "1"
#define QDP_RIL_3GPP2 "0"
#define QDP_RIL_AUTO "2"
#define QDP_RIL_TECH_LEN 1
#define QDP_RIL_PAP_CHAP_NOT_ALLOWED "0"
#define QDP_RIL_PAP_ONLY_ALLOWED "1"
#define QDP_RIL_CHAP_ONLY_ALLOWED "2"
#define QDP_RIL_PAP_CHAP_BOTH_ALLOWED "3"
#define QDP_RIL_AUTH_LEN 1
#define QDP_RIL_IP_4     "IP"
#define QDP_RIL_IP_6     "IPV6"
#define QDP_RIL_IP_4_6   "IPV4V6"

/* QDP Profile PDN Type */
typedef enum
{
  QDP_PROFILE_PDN_TYPE_INVALID = -1,
  QDP_PROFILE_PDN_TYPE_IPV4,
  QDP_PROFILE_PDN_TYPE_IPV6,
  QDP_PROFILE_PDN_TYPE_IPV4V6
} qdp_profile_pdn_type;

/*Functions calling 3gpp multi param search*/
typedef enum qdp_calling_func_e
{
  QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL = 0,
  QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH = 1,
  QDP_PROFILE_LOOKUP_FOR_SET_DATA_PROFILE = 2
} qdp_calling_func_t;


typedef struct
{
  boolean is_valid;
  wds_profile_identifier_type_v01        prof_id;
  wds_get_profile_settings_resp_msg_v01  prof_set;
} qdp_profile_info_type;

/*===========================================================================
  FUNCTION:  qdp_lte_attach_profile_lookup
===========================================================================*/
/*!
  @brief
  This function is used to query and update LTE attach profiles on the modem.
  It receives the new LTE attach parameters and tries to lookup valid 3GPP
  profiles. If a valid profile does not exist then a new profile will be
  created with the profile parameters received in the input. If a valid
  profile exists, it will be updated with the new parameters.

  @params
  param_strings: a pointer to the array of parameter string which contains
  the new LTE attach parameters.

  profile_id_lte: the profile id which will be updated.

  input_prof_id: The input profile id from legacy behaviour in which case
  this will hold the default LTE attach profile id or zero th entry from
  attach pdn list or Invalid if no entry in the attach list


  prof_params: out parameter which will hold the profile parameters
  before the profile is updated. If the LTE attach process fails we would
  need this to restore the modem profile to its previous state

  @return
  QDP_SUCCESS
  QDP_FAILURE
*/
/*=========================================================================*/
int qdp_lte_attach_profile_lookup
(
  const char                  ** param_strings,
  unsigned int                * profile_id_lte,
  qdp_profile_pdn_type        * profile_pdp_type_lte,
  unsigned int                * input_prof_id,
  qdp_profile_info_type       * prof_params,
  qdp_error_info_t            * error_info
);

int qdp_match_lte_attach_profile_params
(
  unsigned int                prof_id,
  const char                  **param_strings
);

/*===========================================================================
  FUNCTION:  qdp_profile_look_up
===========================================================================*/
/*!
    @brief
    Given the set of RIL SETUP_DATA_CALL parameter string array
    (please refer to qdp_ril_param_idx_t to see what order strings
    are expected to be in, to omit certain parameter(s), leave the
    corresponding entry NULL in the array), this function determines
    the applicable technologies, and returns the corresponding 3gpp
    and/or 3gpp2 profile id(s) to be used for data call route look up.
    If required, this function would create new profile(s) on behalf
    of the caller.

    if a non-zero profile_id is returned for a given technology
    (i.e. 3gpp, 3gpp2), the caller of this API is automatically
    assumed to have a reference count on it, which, in turn, must
    be released by calling qdp_profile_release() API when ever
    the caller no longer needs to sustain a data call with that
    profile id.

    @params
    param_strings: a pointer to the array of parameter string such
    that *param_strings[i] is ith parameter string
    profile_id_3gpp: placeholder for the 3gpp profile id (output)
    profile_id_3gpp2: placeholder for the 3gpp2 profile id (output)
    error_info: place holder for specific lookup error information

    @Examples
    User can query both 3gpp and 3gpp2 profile ids using subset of parameters
    listed in qdp_ril_param_idx_t.

    Example 1: if user provides valid values for
    QDP_RIL_TECH = QDP_RIL_3GPP2
    QDP_RIL_APN = "3GPP2_APN"
    qdp_profile_look_up() will try to look up *only* 3gpp2 profile id with
    APN set to "3GPP2_APN"

    Example 2: If user provides valid values for
    QDP_RIL_TECH = "QDP_RIL_AUTO"
    QDP_RIL_APN = "APN"
    QDP_RIL_NAI = "USER"
    qdp_profile_look_up() will try to look up
    * 3gpp profile using "APN" (NAI does not apply to 3GPP profiles)
    * 3gpp2 profile using "APN", and "USER"

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_look_up
(
  const char  ** param_strings,    /* the order of params must match with the
                                      order specified in qdp_ril_param_idx_t */
  unsigned int * profile_id_3gpp,  /* value (not pointer it-self) must
                                      be set to zero by caller */
  qdp_profile_pdn_type *profile_pdn_type_3gpp,  /* 3gpp profile PDN type */
  unsigned int * profile_id_3gpp2, /* value must be set to zero by caller */
  qdp_profile_pdn_type *profile_pdn_type_3gpp2, /* 3gpp2 profile PDN type */
  qdp_error_info_t * error_info
);


int qdp_profile_look_up_by_param
(
  const char                             **param_strings,   /* the order of params must match with the
                                                               order specified in qdp_ril_param_idx_t */
  int                                    param_to_match,
  int                                    param_value,
  unsigned int                           *profile_id_3gpp_list,
  uint8                            *profile_id_3gpp_list_len,
  qdp_profile_pdn_type                   *profile_pdn_type_3gpp,  /* 3gpp profile PDN type */
  unsigned int                           *profile_id_3gpp2_list,
  uint8                           *profile_id_3gpp2_list_len,
  qdp_profile_pdn_type                   *profile_pdn_type_3gpp2,  /* 3gpp2 profile PDN type */
  qdp_tech_t                             *tech_type,
  qdp_error_info_t                       *error_info
);


int qdp_3gpp_profile_update_params
(
  const char                               **params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
);

int qdp_3gpp2_profile_update_params
(
  const char                               **params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
);

int qdp_epc_profile_update_params
(
  const char                               **params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
);

int qdp_profile_update_ex
(
  wds_modify_profile_settings_req_msg_v01  *p_params,
  int                                      *error_code
);

/*===========================================================================
 FUNCTION: qdp_modify_profile
===========================================================================*/
/*!
    @brief
    Updates the Modem profile referenced by the given input parameter

    @params
    params:
    profile: profile and parameters to update
    error_code: return any error during update (out)

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
int qdp_modify_profile
(
  qdp_profile_info_type  *profile,
  int                    *error_code
);

/*===========================================================================
  FUNCTION:  qdp_profile_release
===========================================================================*/
/*!
    @brief
    decreases the reference count on the given profile

    profile is automatically deleted when ref count goes to zero

    @params
    profile_id: profile id

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_release
(
  unsigned int profile_id
);

/*===========================================================================
  FUNCTION:  qdp_profile_release_ex
===========================================================================*/
/*!
    @brief
    Deletes the profile irrespective of the ref count.

    @params
    profile_id: profile id

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_profile_release_ex
(
  unsigned int profile_id
);
/*===========================================================================
  FUNCTION:  qdp_init
===========================================================================*/
/*!
    @brief
    initializes qdp module

    @params
    default_port: default qmi port to be used for QDP operations

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_init(const char * default_port);

/*===========================================================================
  FUNCTION:  qdp_set_subscription
===========================================================================*/
/*!
    @brief
    Sets the appropriate subscription as a result the WDS client get binded to this subscription

    @params
    subs_id:  Subscription ID

    @return
    QDP_SUCCESS
    QDP_FAILURE

    @notes
       Dependencies
    - qdp_init() must be called for the associated port first.
*/
/*=========================================================================*/
int qdp_set_subscription
(
  int subs_id
);

/*===========================================================================
  FUNCTION:  qdp_deinit
===========================================================================*/
/*!
    @brief
    free up resources acquired during initialization

    @params

    @return
    none
*/
/*=========================================================================*/
void qdp_deinit(void);


#define QDP_MALLOC(ptr,size) \
  do \
  { \
    ptr = malloc(size); \
    if (NULL != ptr) \
    { \
      memset(ptr, 0, size); \
      QDP_LOG_DEBUG("QDP: malloc'ed [%p]",ptr); \
    } \
  } while (0)

#define QDP_FREE(ptr) \
  do \
  { \
    if (NULL != ptr) \
    { \
      QDP_LOG_DEBUG("QDP: freeing [%p]",ptr); \
      free(ptr); \
      ptr = NULL; \
    } \
  } while (0)

#endif /*__QDP_H__*/
