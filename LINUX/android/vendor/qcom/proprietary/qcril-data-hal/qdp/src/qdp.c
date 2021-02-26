/*!
  @file
  qdp.c

  @brief
  provides an API for data profile management.

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

$Header: //linux/pkgs/proprietary/qc-ril/main/source/qcril_data.c#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/10   js      created file

===========================================================================*/
/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "qdp_platform.h"
#include "qmi_platform_config.h"
#include "qdp.h"
#include "qdp_qmi_wds.h"

#ifdef FEATURE_QDP_LINUX_ANDROID
#include <cutils/properties.h>
#endif

#ifndef PROPERTY_VALUE_MAX
  #define PROPERTY_VALUE_MAX 256
#endif

typedef struct qdp_param_tech_map_s
{
  qdp_ril_param_idx_t ril_param_idx;
  qdp_tech_t tech_mask; /* 3gpp/3gpp2/both */
  int max_param_len;
} qdp_param_tech_map_t;

/*
 * when RIL does not provide a technology preference,
 * following table can be used to determine, based on the,
 * RIL parameter set, as which technologies to use for
 * subsequent profile look up
 * Currently, we only use APN,  NAI, and IP_FAMILY for
 * profile look up. I am still enumerating
 * all possible RIL parameters for completion and better maintenance.
 *
*/
/*
 * 3GPP2 parameters are super set of 3GPP parameters
*/
qdp_param_tech_map_t param_tech_map_tbl[QDP_RIL_PARAM_MAX] =
{

  { QDP_RIL_TECH,
    QDP_NOTECH, /* Not Applicable for the purpose of this table */
    0
  },

  { QDP_RIL_PROFILE_ID,
    QDP_NOTECH, /* Not Applicable for the purpose of this table */
    0
  },

  /* APN is 3GPP concept, but also applies to 3GPP2 (eHRPD) */
  { QDP_RIL_APN,
    (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP),
    QMI_WDS_APN_NAME_MAX_V01
  },

  /* NAI is 3GPP2 concept, NAI is username in 3GPP */
  { QDP_RIL_NAI,
    (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP),
    QMI_WDS_USER_NAME_MAX_V01
  },

  /* password is not used for profile look up */
  { QDP_RIL_PASSWORD,
   (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP),
    QMI_WDS_PASSWORD_MAX_V01
  },

  /* auth is not used for profile look up */
  { QDP_RIL_AUTH,
    (QDP_NOTECH | QDP_3GPP2 | QDP_3GPP),
    sizeof(wds_profile_auth_protocol_enum_v01)
  },

   /* ip_family is used for profile lookup */
  { QDP_RIL_IP_FAMILY,
    QDP_NOTECH | QDP_3GPP2 | QDP_3GPP,
    sizeof(QDP_RIL_IP_4_6)
  },

  /* ip_roaming is only supported in 3gpp */
  { QDP_RIL_IP_ROAMING,
    QDP_NOTECH | QDP_3GPP,
    sizeof(QDP_RIL_IP_4_6)
  }
};

#define QDP_VALIDATE_RIL_PARAM_INDEX(i) \
  if (i<QDP_RIL_PARAM_MAX && i>0)

#define QDP_GET_RIL_PARAM_IDX(i) \
  param_tech_map_tbl[i].ril_param_idx

#define QDP_GET_3GPP_MAX_LEN(i) \
  param_tech_map_tbl[i].max_param_len

#define QDP_GET_3GPP2_MAX_LEN(i) \
  param_tech_map_tbl[i].max_param_len

#define QDP_GET_TECH_MASK(i) \
  param_tech_map_tbl[i].tech_mask

#define QDP_GET_PARAM_TECH_TABLE_INDEX(param,i) \
  do \
  { \
    int tbl_idx; \
    i = -1; \
    for(tbl_idx=0; tbl_idx<QDP_RIL_PARAM_MAX; tbl_idx++) \
    { \
      if(param_tech_map_tbl[tbl_idx].param == param) \
      { \
        i = tbl_idx; \
        break; \
      } \
    } \
  } \
  while (0)

typedef struct qdp_param_s
{
  char * buf;
  int len;
} qdp_param_t;


#define QDP_QMI_PORT_LEN 12 /* rmnet_sdioxx */
char global_qmi_port[QDP_QMI_PORT_LEN+1]; /* +1 for ending NULL char */
qmi_client_type global_qmi_wds_hndl = QDP_QMI_CLIENT_INVALID;
boolean qdp_inited = FALSE;

/* By default set non-perssitent flag support to TRUE */
boolean qdp_modem_supports_nonpersist_flag = TRUE;

/* By default allow update modem profiles */
#define QDP_ALLOW_UPDATE_MODEM_PROFILE_PROP "persist.data.profile_update"
boolean qdp_allow_update_modem_profile = FALSE;

#define QDP_CDMA_USER_DEF_PROFILE_MIN 51
#define QDP_CDMA_USER_DEF_PROFILE_MAX 60

#define QDP_INIT_BARRIER \
  do \
  { \
    if (FALSE == qdp_inited) \
    { \
      QDP_LOG_ERROR("%s","qdp not inited"); \
      return QDP_FAILURE; \
    } \
  } while (0)

#define QDP_RIL_DATA_PROFILE_OEM_BASE 1000

typedef struct qdp_profile_meta_info_s
{
  boolean is_valid;
  wds_profile_info_type_v01 qmi_type;
  unsigned int ref_count;
  unsigned int is_persistent;
} qdp_profile_meta_info_t;

/* this array holds on to the profiles we created */
static qdp_profile_meta_info_t profile_ids[QDP_NUM_PROFILES_EXPECTED_MAX];
#define QDP_INVALID_PROFILE_ID 0xFF

/* max profile name length according to qmi is 32 */
#define QDP_UNIQUE_PROFILE_NAME "qdp_profile"

#define QDP_PROFILE_IA "qdp_profile_ia"

static int qdp_3gpp_profile_fill_common_params
(
  qdp_param_t                              *params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params,
  qdp_calling_func_t                        calling_function
);

static int qdp_3gpp2_profile_fill_common_params
(
  qdp_param_t                              *params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
);

static qdp_profile_pdn_type qdp_pdn_type_from_string
(
  const char *ip_string
);

static qdp_profile_pdn_type qdp_pdn_type_from_3gpp_pdp
(
  const wds_pdp_type_enum_v01 pdp_3gpp
);

static qdp_profile_pdn_type qdp_pdn_type_from_3gpp2_pdn
(
  const wds_profile_pdn_type_enum_v01 pdn_3gpp2
);

static qdp_profile_pdn_type qdp_pdn_type_from_3gpp_roaming_pdp
(
  const wds_common_pdp_type_enum_v01 roaming_pdp
);

static boolean qdp_profile_needs_update
(
  const qdp_param_t * qdp_params,
  const qdp_profile_info_type * profile_info
);

static boolean qdp_is_roaming_pdp_compatible
(
  const char * qdp_roaming_pdp,
  const wds_get_profile_settings_resp_msg_v01 * prof_params,
  qdp_error_t *error
);

/*===========================================================================
  FUNCTION:  qdp_insert_profile_id
===========================================================================*/
/*!
    @brief
    inserts given profile id into global profile_ids array. returns
    QDP_FAILURE if no space left.

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_insert_profile_id
(
  int profile_index,
  int technology,
  unsigned int is_persistent
)
{
  int i=0;
  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    if (profile_ids[i].is_valid == FALSE)
    {
      profile_ids[i].qmi_type.profile_index = (uint8_t)profile_index;
      profile_ids[i].is_persistent = is_persistent;

      switch(technology)
      {
      case QDP_3GPP:
        profile_ids[i].qmi_type.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
        QDP_LOG_DEBUG("3GPP profile_id [%d] created by QDP", profile_index);
        break;
      case QDP_3GPP2:
        profile_ids[i].qmi_type.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
        QDP_LOG_DEBUG("3GPP2 profile_id [%d] created by QDP", profile_index);
        break;
      default:
        QDP_LOG_ERROR("%s","programming error");
        break;
      }

      profile_ids[i].is_valid = TRUE;
      return QDP_SUCCESS;
    }
  }

  return QDP_FAILURE;
}

/*===========================================================================
  FUNCTION:  qdp_delete_profile_id
===========================================================================*/
/*!
    @brief
    deletes the given profile

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_delete_profile_id
(
  unsigned int profile_id
)
{
  int i=0;
  int rc, qmi_err_code;

  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    if (profile_ids[i].is_valid == TRUE &&
        profile_ids[i].qmi_type.profile_index == profile_id)
    {
      wds_delete_profile_req_msg_v01  dp_req;

      if (profile_ids[i].ref_count != 0)
      {
        QDP_LOG_DEBUG("deleting profile [%d] that is referred to by "
                      "[%d] users", profile_ids[i].qmi_type.profile_index,
                      profile_ids[i].ref_count);
      }

      memset(&dp_req, 0, sizeof(dp_req));
      dp_req.profile.profile_type = profile_ids[i].qmi_type.profile_type;
      dp_req.profile.profile_index = profile_ids[i].qmi_type.profile_index;

      if(QDP_SUCCESS != qdp_qmi_wds_delete_profile(global_qmi_wds_hndl,
                                                   &dp_req))
      {
        QDP_LOG_ERROR("could not delete profile [%d]",
                      profile_ids[i].qmi_type.profile_index);
        return QDP_FAILURE;
      }
      else
      {
        QDP_LOG_DEBUG("profile id [%d] successfully deleted",
                      profile_id);
        memset(&profile_ids[i].qmi_type, 0, sizeof(profile_ids[i].qmi_type));
        profile_ids[i].ref_count = 0;
        profile_ids[i].is_valid = FALSE;
        return QDP_SUCCESS;
      }
    }
  }

  QDP_LOG_ERROR("profile id [%d] not found in our global list",
                profile_id);

  return QDP_FAILURE;
}


/*===========================================================================
  FUNCTION:  qdp_find_profile_id
===========================================================================*/
/*!
    @brief
    Searches profile_ids[] for the specified ID, returns reference
    count as output pointer if not NULL.

    @return
    QDP_SUCCESS if profile ID found,
    QDP_FAILURE otherwise
*/
/*=========================================================================*/
static int qdp_find_profile_id
(
  unsigned int  profile_id,
  unsigned int *ref_count
)
{
  int i=0;

  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    if (profile_ids[i].is_valid == TRUE &&
        profile_ids[i].qmi_type.profile_index == profile_id)
    {
      /* Return reference count if caller interested */
      if( ref_count )
      {
        *ref_count = profile_ids[i].ref_count;
      }
      return QDP_SUCCESS;
    }
  }
  return QDP_FAILURE;
}

/*===========================================================================
  FUNCTION:  qdp_cleanup_profile_ids
===========================================================================*/
/*!
    @brief
    deletes all profiles listed in profile_ids[]

    @return
    none
*/
/*=========================================================================*/
static void qdp_cleanup_profile_ids
(
  void
)
{
  int i=0;
  int rc, qmi_err_code;

  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    if (profile_ids[i].is_valid == TRUE)
    {
      wds_delete_profile_req_msg_v01  dp_req;

      memset(&dp_req, 0, sizeof(dp_req));
      dp_req.profile.profile_type = profile_ids[i].qmi_type.profile_type;
      dp_req.profile.profile_index = profile_ids[i].qmi_type.profile_index;

      if(QDP_SUCCESS != qdp_qmi_wds_delete_profile(global_qmi_wds_hndl,
                                                   &dp_req))
      {
        QDP_LOG_ERROR("could not delete profile [%d]",
                      profile_ids[i].qmi_type.profile_index);
      }
      else
      {
        memset(&profile_ids[i].qmi_type, 0, sizeof(profile_ids[i].qmi_type));
        profile_ids[i].ref_count = 0;
        profile_ids[i].is_valid = FALSE;
      }
    }
  }
}

/*===========================================================================
  FUNCTION:  qdp_init_profile_ids
===========================================================================*/
/*!
    @brief
    inits profile meta info array

    @return
    none
*/
/*=========================================================================*/
static void qdp_init_profile_ids
(
  void
)
{
  unsigned int i=0;
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int rc, qmi_err_code;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  QDP_LOG_DEBUG("%s","qdp_init_profile_ids ENTRY");

  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    memset(&profile_ids[i].qmi_type, 0, sizeof(profile_ids[i].qmi_type));
    profile_ids[i].ref_count = 0;
    profile_ids[i].is_valid = FALSE;
  }

  do
  {
    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("get_profile_list failed with error [%d] " \
                        "qmi_err_code [%d]", pl_resp.resp.result, pl_resp.resp.error);
      break;
    }

    QDP_LOG_DEBUG("get_profile_list for profile_tech [%d] "   \
                  "returned [%d] profile ids",
                  pl_req.profile_type, pl_resp.profile_list_len);


    reti = QDP_SUCCESS;

    for(i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("query_profile failed with error [%d] qmi_err_code [%d]",
                           ps_resp.resp.result, ps_resp.resp.error);
        reti = QDP_FAILURE;
        break;
      }
      else
      {
        QDP_LOG_DEBUG("successfully queried 3GPP profile [%d]",
                      ps_req.profile.profile_index);
      }

      if(ps_resp.profile_name_valid &&
             !strncasecmp(QDP_UNIQUE_PROFILE_NAME,
                          ps_resp.profile_name,
                          std_strlen(QDP_UNIQUE_PROFILE_NAME)))
      {
        QDP_LOG_DEBUG("Found a QDP profile @ [%d]",
                          ps_req.profile.profile_index);

          profile_ids[i].qmi_type.profile_index =
                                           ps_req.profile.profile_index;

          /* Assuming that all QDP profiles in modem are persistent
           * until modem supports the ability to check if a profile is persistent or
           * not via query API */

          profile_ids[i].is_persistent = TRUE;
          profile_ids[i].is_valid = TRUE;
      }

    } /* for */
    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);


  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp_multi_param_search EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_multi_param_search EXIT success");
  }


}

/*===========================================================================
  FUNCTION:  qdp_free_qdp_params
===========================================================================*/
/*!
    @brief
    frees memory associated with qdp_param_t array

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static void qdp_free_qdp_params
(
  qdp_param_t * param_arr,
  unsigned int size
)
{
  unsigned int i=0;

  if (NULL == param_arr)
  {
    QDP_LOG_ERROR("%s","NULL param recvd");
    return;
  }

  for(i=0; i<size; i++)
  {
    if (param_arr[i].len > 0)
    {
      /* yes, QDP_FREE checks for NULL */
      QDP_FREE(param_arr[i].buf);
    }
  }
}

/*===========================================================================
  FUNCTION:  qdp_get_3gpp_profile_pdn_type
===========================================================================*/
/*!
    @brief
    This function returns the PDN type of the given 3gpp profile

    @param
    prof_params - 3gpp2 profile parameters

    @return
    PDN type of the given profile
*/
/*=========================================================================*/
static qdp_profile_pdn_type
qdp_get_3gpp_profile_pdn_type
(
  wds_pdp_type_enum_v01  profile_pdp_type,
  uint8_t                profile_pdp_type_valid
)
{
  qdp_profile_pdn_type  pdn_type = QDP_PROFILE_PDN_TYPE_INVALID;
  int ret = QDP_FAILURE;


  QDP_LOG_DEBUG("%s","qdp_get_3gpp_profile_pdn_type ENTRY");

  do
  {
    if (profile_pdp_type_valid)
    {
      switch (profile_pdp_type)
      {
        case WDS_PDP_TYPE_PDP_IPV4_V01:
          pdn_type = QDP_PROFILE_PDN_TYPE_IPV4;
          break;

        case WDS_PDP_TYPE_PDP_IPV6_V01:
          pdn_type = QDP_PROFILE_PDN_TYPE_IPV6;
          break;

        case WDS_PDP_TYPE_PDP_IPV4V6_V01:
          pdn_type = QDP_PROFILE_PDN_TYPE_IPV4V6;
          break;

        default:
          QDP_LOG_ERROR("unknown umts pdp_type=%d\n",
                        profile_pdp_type);
          break;
      }
    }

    ret = QDP_SUCCESS;
  }
  while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_get_3gpp_profile_pdn_type EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_get_3gpp_profile_pdn_type EXIT success");
  }

  return pdn_type;
}


/*===========================================================================
  FUNCTION:  qdp_get_3gpp2_profile_pdn_type
===========================================================================*/
/*!
    @brief
    This function returns the PDN type of the given 3gpp2 profile

    @param
    prof_params - 3gpp2 profile parameters

    @return
    PDN type of the given profile
*/
/*=========================================================================*/
static qdp_profile_pdn_type
qdp_get_3gpp2_profile_pdn_type
(
  wds_profile_pdn_type_enum_v01  profile_pdn_type,
  uint8_t                        profile_pdn_type_valid
)
{
  qdp_profile_pdn_type  pdn_type = QDP_PROFILE_PDN_TYPE_INVALID;
  int ret = QDP_FAILURE;


  QDP_LOG_DEBUG("%s","qdp_get_3gpp2_profile_pdn_type ENTRY");

  do
  {
    if (profile_pdn_type_valid)
    {
      switch (profile_pdn_type)
      {
      case WDS_PROFILE_PDN_TYPE_IPV4_V01:
        pdn_type = QDP_PROFILE_PDN_TYPE_IPV4;
        break;

      case WDS_PROFILE_PDN_TYPE_IPV6_V01:
        pdn_type = QDP_PROFILE_PDN_TYPE_IPV6;
        break;

      case WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01:
        pdn_type = QDP_PROFILE_PDN_TYPE_IPV4V6;
        break;

      default:
        QDP_LOG_ERROR("unknown cdma pdp_type=%d\n",
                      profile_pdn_type);
        break;
      }
    }

    ret = QDP_SUCCESS;
  }
  while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_get_3gpp2_profile_pdn_type EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_get_3gpp2_profile_pdn_type EXIT success");
  }

  return pdn_type;
}

/*===========================================================================
  FUNCTION:  qdp_match_3gpp_profile_params
===========================================================================*/
/*!
    @brief
    this function matches a set of profile parameters (typically received
    as a user input in the form of qdp_param_t array) with the parameters
    of a given profile and returns the result.

    @params
    params: input set of qdp_param_t parameters
    prof_params: parameters of a profile
    prof_id: Modem profile ID
    match_found: place holder for return boolean (TRUE/FALSE)
    lookup_error: specific lookup error code

    @notes

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_match_3gpp_profile_params
(
  const qdp_param_t                      *params, /* array index indicates which parameter
                                                   * it is based on qdp_ril_param_idx_t */
  wds_get_profile_settings_resp_msg_v01  *prof_params,
  uint8_t                                prof_id,
  boolean                                *match_found,
  qdp_error_t                            *lookup_error,
  qdp_calling_func_t                      calling_func
)
{
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int i=0;
  boolean qdp_created_profile = FALSE;
  qdp_error_t roam_pdp_error = QDP_ERROR_NONE;

  QDP_LOG_DEBUG("%s","qdp_match_3gpp_profile_params ENTRY");

  do
  {
    if (NULL == params ||
        NULL == prof_params ||
        NULL == match_found ||
        NULL == lookup_error )
    {
      QDP_LOG_ERROR("%s","invalid params");
      break;
    }

    if ((prof_params->profile_name_valid) &&
        (!strncasecmp(prof_params->profile_name,
                     QDP_UNIQUE_PROFILE_NAME,
                     strlen(QDP_UNIQUE_PROFILE_NAME))))
    {
      qdp_created_profile = TRUE;
      QDP_LOG_DEBUG("%s","Profile is a QDP profile.");
    }

    /* Initialize output parameters */
    *match_found = TRUE;

    /* this for loop looks for a mismatch */
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      /* try to match non-zero parameter & zero length APN name*/
      if (params[i].len > 0 || (i == QDP_RIL_APN && params[i].len == 0))
      {
        if (NULL == params[i].buf)
        {
          QDP_LOG_ERROR("%s","programming error: NULL buffer found"
                        "where length is not zero");
          reti = QDP_FAILURE;
          break;
        }
        switch(i)
        {
        case QDP_RIL_APN:
          if (!prof_params->apn_name_valid)
          {
            QDP_LOG_DEBUG("%s", "modem profile parameter APN is invalid");
            *match_found = FALSE;
            break;
          }
          else
          {
            size_t apn_name_len = std_strlen(prof_params->apn_name);

            if (apn_name_len == (size_t)params[i].len)
            {
              if (strncasecmp( params[i].buf,
                               prof_params->apn_name,
                               (size_t)params[i].len) == 0)
              {
                QDP_LOG_DEBUG("APN [%s] matched", params[i].buf);
              }
              else
              {
                QDP_LOG_DEBUG("APN [%s] did not match", params[i].buf);
                QDP_LOG_DEBUG("Modem contains [%s]", prof_params->apn_name);
                *match_found = FALSE;
                break;
             }
            }
            else
            {
              QDP_LOG_DEBUG("%s", "APN: Lengths did not match");
              QDP_LOG_DEBUG("Modem APN len [%d], Input APN len [%d]",
                            apn_name_len,
                            params[i].len);

              *match_found = FALSE;
              break;
            }
          }
          break;
        case QDP_RIL_NAI:
          QDP_LOG_DEBUG("%s","QDP_RIL_NAI is not used "
                        "for matching 3GPP profile");
          break;
        case QDP_RIL_PASSWORD:
          QDP_LOG_DEBUG("%s","QDP_RIL_PASSWORD is not used "
                        "for matching 3GPP profile");
          break;
        case QDP_RIL_AUTH:
          QDP_LOG_DEBUG("%s","QDP_RIL_AUTH is not used "
                        "for matching 3GPP profile");

#if 0
          /* This code is kept for later enablement (if needed) */
          if(!(prof_params->umts_profile_params.param_mask &
               QMI_WDS_UMTS_PROFILE_AUTH_PREF_PARAM_MASK))
          {
            QDP_LOG_DEBUG("%s","modem profile parameter AUTH_PREF not available");
            *match_found = FALSE;
            break;
          }
          switch(prof_params->umts_profile_params.auth_pref)
          {
          case QMI_WDS_AUTH_PREF_PAP_CHAP_NOT_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_PAP_CHAP_NOT_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          case QMI_WDS_AUTH_PREF_PAP_ONLY_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_PAP_ONLY_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          case QMI_WDS_AUTH_PREF_CHAP_ONLY_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_CHAP_ONLY_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          case QMI_WDS_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_PAP_CHAP_BOTH_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          default:
            QDP_LOG_DEBUG("profile contains invalid value [%d]",
                          prof_params->umts_profile_params.auth_pref);
            *match_found = FALSE;
            break;
          }
#endif
          break;
        case QDP_RIL_IP_FAMILY:

          if(qdp_allow_update_modem_profile)
          {
            QDP_LOG_DEBUG("%s","QDP_RIL_IP_FAMILY is not used "
                          "for matching 3GPP profile");
            break;
          }

          /* legacy */
          if(!prof_params->pdp_type_valid)
          {
            QDP_LOG_DEBUG("%s","modem profile parameter PDP_TYPE not available");
            *match_found = FALSE;
            break;
          }

          /*This fucntion is used by both qdp_profile_lookup and lte_attach_profile
            lookup.
            1. LTE attach profile lookup for Initial attach
            IP tye matching should only be done for a non-qdp profile as we
            never update the IP type for non-qdp profiles
            However, in case of QDP profiles, since we are free to update the IP type
            as provided by RIL, we do not need it for matching
            2 qdp_profile_lookup for setup_data_call
            QDP_profile_lookup happens as part of setup_data_call, so we need a uniform
            IP matching criteria here for both QDP and Non-QDP profiles*/
          if((calling_func == QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL) ||
           ((calling_func == QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH) && (!qdp_created_profile))
            )
          {
            switch(prof_params->pdp_type)
            {
            case WDS_PDP_TYPE_PDP_IPV4_V01:
              if ((strncasecmp( params[i].buf,
                                QDP_RIL_IP_4,
                                std_strlen(QDP_RIL_IP_4)+1) == 0) ||
                  (strncasecmp( params[i].buf,
                                QDP_RIL_IP_4_6,
                                std_strlen(QDP_RIL_IP_4_6)+1) == 0))
              {
                QDP_LOG_DEBUG("ip_family [%s] matched", params[i].buf);
              }
              else
              {
                QDP_LOG_DEBUG("ip_family [%s] did not match", params[i].buf);
                *match_found = FALSE;
                /* Restrict lookup_error to OEM pre-provisioned
                   persistent profiles. This is determined by checking
                   the profile ID against list of those which were
                   created by QDP itself.  This assumes only one
                   instance of QDP active in system.
                   Note Modem QMI currently does not store persistent
                   flag in profile data so this cannot be used. */
                if( QDP_FAILURE == qdp_find_profile_id( prof_id, NULL))
                {
                  if (!qdp_created_profile)
                  {
                    *lookup_error = QDP_ERROR_ONLY_IPV4_ALLOWED;
                     QDP_LOG_DEBUG("lookup_error updated to %d", *lookup_error);
                  }
                }
              }
              break;
            case WDS_PDP_TYPE_PDP_IPV6_V01:
              if ((strncasecmp( params[i].buf,
                                QDP_RIL_IP_6,
                                std_strlen(QDP_RIL_IP_6)+1) == 0) ||
                  (strncasecmp( params[i].buf,
                                QDP_RIL_IP_4_6,
                                std_strlen(QDP_RIL_IP_4_6)+1) == 0))
              {
                QDP_LOG_DEBUG("ip_family [%s] matched", params[i].buf);
              }
              else
              {
                QDP_LOG_DEBUG("ip_family [%s] did not match", params[i].buf);
                *match_found = FALSE;
                /* Restrict lookup_error to OEM pre-provisioned
                   persistent profiles. This is determined by checking
                   the profile ID against list of those which were
                   created by QDP itself.  This assumes only one
                   instance of QDP active in system.
                   Note Modem QMI currently does not store persistent
                   flag in profile data so this cannot be used. */
                if( QDP_FAILURE == qdp_find_profile_id( prof_id, NULL))
                {
                  if (!qdp_created_profile)
                  {
                    *lookup_error = QDP_ERROR_ONLY_IPV6_ALLOWED;
                     QDP_LOG_DEBUG("lookup_error updated to %d", *lookup_error);
                  }
                }
              }
              break;
            case WDS_PDP_TYPE_PDP_IPV4V6_V01:
              QDP_LOG_DEBUG("%s","any ip_family supported");
              break;
            default:
              QDP_LOG_DEBUG("invalid ip family [%d] found in profile",
                                 prof_params->pdp_type);
              *match_found = FALSE;
              break;
            }
          }
          else if ((calling_func == QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH)
                    && (qdp_created_profile))
          {
            QDP_LOG_DEBUG("%s","QDP_RIL_IP_FAMILY is not used  for matching "
                              "QDP created 3GPP profiles in Initial Attach");
          }

          break;

        case QDP_RIL_IP_ROAMING:

          if (qdp_allow_update_modem_profile)
          {
            QDP_LOG_DEBUG("%s","QDP_RIL_IP_ROAMING is not used "
                          "for matching 3GPP profile");
            break;
          }

          /* Only need to match roaming PDP type for non-qdp initial attach profile.
             For qdp inital attach profle -- it can be updated,
             For setup data call -- modem will reject if pdp is not compatible.
          */
          if (calling_func == QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH &&
              !qdp_created_profile)
          {
            if (!qdp_is_roaming_pdp_compatible(params[i].buf, prof_params, &roam_pdp_error))
            {
              QDP_LOG_DEBUG("%s", "Roaming PDP type does not match");
              *match_found = FALSE;

              if( QDP_FAILURE == qdp_find_profile_id( prof_id, NULL))
              {
                *lookup_error = roam_pdp_error;
                 QDP_LOG_DEBUG("lookup_error updated to %d", *lookup_error);
              }
            }
          }
          else
          {
            QDP_LOG_DEBUG("%s","QDP_RIL_IP_ROAMING is not used "
                          "for matching 3GPP profile");
          }
          break;

        default:
          /* not an error case. we ignore the parameters we don't care for */
          QDP_LOG_DEBUG("matching [%d] (qdp_ril_param_idx_t) not supported", i);
          break;
        } /* switch */
        if (*match_found == FALSE)
        {
          break;
        }
      } /* if */

    } /* for */
    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);


  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_match_3gpp_profile_params EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_match_3gpp_profile_params EXIT success");
  }

  return ret;

}

/*===========================================================================
  FUNCTION:  qdp_match_3gpp2_profile_params
===========================================================================*/
/*!
    @brief
    this function matches a set of profile parameters (typically received
    as a user input in the form of qdp_param_t array) with the parameters
    of a given profile and returns the result.

    @params
    params: input set of qdp_param_t parameters
    prof_params: parameters of a profile
    prof_id: Modem profile ID
    match_found: place holder for return boolean (TRUE/FALSE)
    lookup_error: specific lookup error code

    @notes

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_match_3gpp2_profile_params
(
  const qdp_param_t                      *params, /* array index indicates which parameter
                                                   * it is based on qdp_ril_param_idx_t */
  wds_get_profile_settings_resp_msg_v01  *prof_params,
  uint8_t                                prof_id,
  boolean                                *match_found,
  qdp_error_t                            *lookup_error
)
{
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int i=0;

  QDP_LOG_DEBUG("%s","qdp_match_3gpp2_profile_params ENTRY");

  do
  {
    if (NULL == params ||
        NULL == prof_params ||
        NULL == match_found ||
        NULL == lookup_error )
    {
      QDP_LOG_ERROR("%s","invalid params");
      break;
    }

    /* Initialize output parameters */
    *match_found = TRUE;

    /* this for loop looks for a mismatch */
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      /* try to match non-zero parameter & zero length APN name*/
      if (params[i].len > 0 || (i == QDP_RIL_APN && params[i].len == 0))
      {
        if (NULL == params[i].buf)
        {
          QDP_LOG_ERROR("%s","programming error: NULL buffer found"
                        "where length is non zero");
          reti = QDP_FAILURE;
          break;
        }
        switch(i)
        {
        case QDP_RIL_APN:
          if (!prof_params->apn_string_valid)
          {
            QDP_LOG_DEBUG("%s", "modem profile parameter APN is invalid");
            *match_found = FALSE;
            break;
          }
          else
          {
            size_t apn_string_len = std_strlen(prof_params->apn_string);

            if (apn_string_len == (size_t)params[i].len)
            {
              if (strncasecmp( params[i].buf,
                               prof_params->apn_string,
                               (size_t)params[i].len) == 0)
              {
                QDP_LOG_DEBUG("APN [%s] matched", params[i].buf);
              }
              else
              {
                QDP_LOG_DEBUG("APN [%s] did not match", params[i].buf);
                QDP_LOG_DEBUG("Modem contains [%s]", prof_params->apn_string);
                *match_found = FALSE;
                break;
             }
            }
            else
            {
              QDP_LOG_DEBUG("%s", "APN: Lengths did not match");
              QDP_LOG_DEBUG("Modem APN len [%d], Input APN len [%d]",
                            apn_string_len,
                            params[i].len);

              *match_found = FALSE;
              break;
            }
          }
          break;

        case QDP_RIL_NAI:
          if (!prof_params->user_id_valid)
          {
            QDP_LOG_DEBUG("%s","modem profile parameter NAI is NULL");
            *match_found = FALSE;
            break;
          }
          else
          {
            size_t user_id_len = std_strlen(prof_params->user_id);

            if (user_id_len == (size_t)params[i].len)
            {
              if (strncasecmp( params[i].buf,
                               prof_params->user_id,
                               (size_t)params[i].len) == 0)
              {
                QDP_LOG_DEBUG("NAI [%s] matched", params[i].buf);
              }
              else
              {
                QDP_LOG_DEBUG("NAI [%s] did not match", params[i].buf);
                QDP_LOG_DEBUG("Modem contains [%s]", prof_params->user_id);
                *match_found = FALSE;
                break;
             }
            }
            else
            {
              QDP_LOG_DEBUG("%s", "NAI: Lengths did not match");
              QDP_LOG_DEBUG("Modem NAI len [%d], Input NAI len [%d]",
                            user_id_len,
                            params[i].len);

              *match_found = FALSE;
              break;
            }
          }
          break;
        case QDP_RIL_PASSWORD:
          QDP_LOG_DEBUG("%s","QDP_RIL_PASSWORD is not used "
                        "for matching 3GPP2 profile");
          break;
        case QDP_RIL_AUTH:
          QDP_LOG_DEBUG("%s","QDP_RIL_AUTH is not used "
                        "for matching 3GPP2 profile");
#if 0
          /* This code is kept for later enablement (if needed) */
          if(!(prof_params->cdma_profile_params.param_mask &
               QMI_WDS_CDMA_PROFILE_AUTH_PROTOCOL_PARAM_MASK))
          {
            QDP_LOG_DEBUG("%s","modem profile parameter AUTH_PROTOCOL not available");
            *match_found = FALSE;
            break;
          }
          switch(prof_params->cdma_profile_params.auth_protocol)
          {
          case QMI_WDS_AUTH_PREF_PAP_CHAP_NOT_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_PAP_CHAP_NOT_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          case QMI_WDS_AUTH_PREF_PAP_ONLY_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_PAP_ONLY_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          case QMI_WDS_AUTH_PREF_CHAP_ONLY_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_CHAP_ONLY_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          case QMI_WDS_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED:
            if (strncasecmp(
                  params[i].buf,
                  QDP_RIL_PAP_CHAP_BOTH_ALLOWED,
                  QDP_RIL_AUTH_LEN) == 0)
            {
              QDP_LOG_DEBUG("auth_pref [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("auth_pref [%s] did not match", params[i].buf);
              *match_found = FALSE;
            }
            break;
          default:
            QDP_LOG_DEBUG("profile contains invalid value [%d]",
                          prof_params->cdma_profile_params.auth_protocol);
            *match_found = FALSE;
            break;
          }
#endif
          break;
        case QDP_RIL_IP_FAMILY:

          if(qdp_allow_update_modem_profile)
          {
            QDP_LOG_DEBUG("%s","QDP_RIL_IP_FAMILY is not used "
                          "for matching 3GPP2 profile");
            break;
          }

          /* Legacy */
          if(!prof_params->pdn_type_valid)
          {
            QDP_LOG_DEBUG("%s","modem profile parameter PDN_TYPE not available");
            *match_found = FALSE;
            break;
          }
          switch(prof_params->pdn_type)
          {
          case WDS_PROFILE_PDN_TYPE_IPV4_V01:
            if ((strncasecmp( params[i].buf,
                              QDP_RIL_IP_4,
                              std_strlen(QDP_RIL_IP_4)+1) == 0) ||
                (strncasecmp( params[i].buf,
                              QDP_RIL_IP_4_6,
                              std_strlen(QDP_RIL_IP_4_6)+1) == 0))
            {
              QDP_LOG_DEBUG("ip_family [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("ip_family [%s] did not match", params[i].buf);
              *match_found = FALSE;
              /* Restrict lookup_error to OEM pre-provisioned
                 persistent profiles. This is determined by checking
                 the profile ID against list of those which were
                 created by QDP itself.  This assumes only one
                 instance of QDP active in system.
                 Note Modem QMI currently does not store persistent
                 flag in profile data so this cannot be used. */
              if( (QDP_FAILURE == qdp_find_profile_id( (unsigned int)prof_id, NULL)) &&
                  ((prof_id < QDP_CDMA_USER_DEF_PROFILE_MIN)||
                   (prof_id > QDP_CDMA_USER_DEF_PROFILE_MAX)))
              {
                *lookup_error = QDP_ERROR_ONLY_IPV4_ALLOWED;
                QDP_LOG_DEBUG("lookup_error updated to %d", *lookup_error);
              }
            }
            break;
          case WDS_PROFILE_PDN_TYPE_IPV6_V01:
            if ((strncasecmp( params[i].buf,
                              QDP_RIL_IP_6,
                              std_strlen(QDP_RIL_IP_6)+1) == 0) ||
                (strncasecmp( params[i].buf,
                              QDP_RIL_IP_4_6,
                              std_strlen(QDP_RIL_IP_4_6)+1) == 0))
            {
              QDP_LOG_DEBUG("ip_family [%s] matched", params[i].buf);
            }
            else
            {
              QDP_LOG_DEBUG("ip_family [%s] did not match", params[i].buf);
              *match_found = FALSE;
              /* Restrict lookup_error to OEM pre-provisioned
                 persistent profiles. This is determined by checking
                 the profile ID against list of those which were
                 created by QDP itself.  This assumes only one
                 instance of QDP active in system.
                 Note Modem QMI currently does not store persistent
                 flag in profile data so this cannot be used. */
              if( (QDP_FAILURE == qdp_find_profile_id( (unsigned int)prof_id, NULL)) &&
                  ((prof_id < QDP_CDMA_USER_DEF_PROFILE_MIN)||
                   (prof_id > QDP_CDMA_USER_DEF_PROFILE_MAX)))
              {
                *lookup_error = QDP_ERROR_ONLY_IPV6_ALLOWED;
                QDP_LOG_DEBUG("lookup_error updated to %d", *lookup_error);
              }
            }
            break;
          case WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01:
            QDP_LOG_DEBUG("%s","any ip_family supported");
            break;
          case WDS_PROFILE_PDN_TYPE_UNSPECIFIED_V01:
            QDP_LOG_DEBUG("%s","unspecified ip_family found in profile");
            *match_found = FALSE;
            break;
          default:
            QDP_LOG_DEBUG("invalid ip family [%d] found in profile",
                          prof_params->pdn_type);
            *match_found = FALSE;
            break;
          }
          break;
        default:
          /* not an error case. we ignore the parameters we don't care for */
          QDP_LOG_DEBUG("matching [%d] (qdp_ril_param_idx_t) not supported", i);
          break;
        } /* switch */
      } /* if */
      if (*match_found == FALSE)
      {
        break;
      }

    } /* for */
    if (reti != QDP_SUCCESS)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);


  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_match_3gpp2_profile_params EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_match_3gpp2_profile_params EXIT success");
  }

  return ret;

}


/*===========================================================================
  FUNCTION:  qdp_3gpp_multi_param_search
===========================================================================*/
/*!
    @brief
    searches through list of 3GPP profiles using the combination of
    given parameters (ANDed together) as a key
    If one of more profiles are found based on this search, it
    returns the very first profile id in the place holder.

    @params
    params: pointer to the array of parameters such that *param[i] is
    the ith parameter. This array must be exactly QDP_RIL_PARAM_MAX
    long, and must contain parameters in the order specified by
    qdp_ril_param_idx_t. If a parameter is not required, it can be
    left out NULL in the array.
    profile_num: placeholder for the returned profile id (if found).
    lookup_error: specific lookup error code
    p_params: out parameter to hold the profile parameters for a given
    profile id before it is updated. Will be used to restore the profile
    in case LTE attach fails. For non-LTE attach cases this parameter will
    be NULL.

    @notes

    Calling func is the ID assigned to the function calling this one
    1 = QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH
    0 = QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL

    *ia_profile_id would contain the index of the profile which has
    'qdp_profile_ia' as its profile name

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_3gpp_multi_param_search
(
  const qdp_param_t      *params,       /* array index indicates which parameter
                                         * it is based on qdp_ril_param_idx_t */
  unsigned int           *profile_num,  /* out placeholder, *profile_num
                                         * must be set to 0 by caller */
  qdp_profile_pdn_type   *profile_pdn_type,  /* Profile PDN type */
  qdp_profile_info_type  *p_params,     /* out placeholder */
  qdp_error_t            *lookup_error,  /* out placeholder */
  qdp_calling_func_t      calling_func
)
{
  uint32_t i=0;
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int rc;
  boolean match_found = FALSE;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  QDP_INIT_BARRIER;

  do
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_multi_param_search ENTRY");

    if (NULL == params           ||
        NULL == profile_num      ||
        NULL == profile_pdn_type ||
        NULL == lookup_error )
    {
      QDP_LOG_ERROR("%s","invalid params");
      break;
    }

    if (QDP_INVALID_PROFILE != *profile_num)
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    *profile_num, QDP_INVALID_PROFILE);
      break;
    }

    if (QDP_PROFILE_PDN_TYPE_INVALID != *profile_pdn_type)
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    *profile_pdn_type, QDP_PROFILE_PDN_TYPE_INVALID);
      break;
    }

    if (p_params)
    {
      p_params->is_valid = FALSE;
    }

    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("get_profile_list failed with error [%d] " \
                    "qmi_err_code [%d]", pl_resp.resp.result, pl_resp.resp.error);
      break;
    }

    QDP_LOG_DEBUG("get_profile_list for profile_tech [%d] "   \
                  "returned [%d] profile ids",
                  pl_req.profile_type, pl_resp.profile_list_len);

    reti = QDP_SUCCESS;

    for(i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("query_profile failed with error [%d] " \
                      "qmi_err_code [%d]", ps_resp.resp.result, ps_resp.resp.error);
        reti = QDP_FAILURE;
        break;
      }
      else
      {
        QDP_LOG_DEBUG("successfully queried 3GPP profile [%d]",
                      ps_req.profile.profile_index);
      }

      /* every time there is specific lookup error that needs to be
         handled in QDP, the lookup_error variable will be updated.
      */
      rc = qdp_match_3gpp_profile_params( params,
                                          &ps_resp,
                                          ps_req.profile.profile_index,
                                          &match_found,
                                          lookup_error,
                                          calling_func);
      if (QDP_FAILURE == rc)
      {
        QDP_LOG_ERROR("%s","programming error");
        reti = QDP_FAILURE;
        break;
      }

      if(match_found)
      {
        *profile_num = ps_req.profile.profile_index;
        *profile_pdn_type = qdp_get_3gpp_profile_pdn_type(ps_resp.pdp_type,
                                                          ps_resp.pdp_type_valid);

        if (NULL != p_params)
        {
          /* Copy the profile information into the out placeholder for profile params */
          p_params->is_valid = TRUE;
          memcpy( &p_params->prof_id,  &ps_req.profile, sizeof(p_params->prof_id));
          memcpy( &p_params->prof_set, &ps_resp, sizeof(p_params->prof_set));
        }

        /* reset lookup error if it was set while matching
           any profiles queried before this point */
        QDP_LOG_DEBUG("%s", "resetting lookup_error - if any");
        *lookup_error = QDP_ERROR_NONE;
        break;
      }

    } /* for */
    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp_multi_param_search EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_multi_param_search EXIT success");
  }

  return ret;
}


int qdp_get_ia_type_profile_id
(
  int                    *ia_profile_id
)
{
  uint32_t i=0;
  int reti = QDP_SUCCESS;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  QDP_INIT_BARRIER;

  do
  {

    QDP_LOG_DEBUG("%s","qdp_get_ia_type_profile_id ENTRY");

    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("get_profile_list failed with error [%d] " \
                    "qmi_err_code [%d]", pl_resp.resp.result, pl_resp.resp.error);
      reti = QDP_FAILURE;
      break;
    }

    QDP_LOG_DEBUG("get_profile_list for profile_tech [%d] "   \
                  "returned [%d] profile ids",
                  pl_req.profile_type, pl_resp.profile_list_len);


    for(i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("query_profile failed with error [%d] " \
                      "qmi_err_code [%d]", ps_resp.resp.result, ps_resp.resp.error);
        reti = QDP_FAILURE;
        break;
      }
      else
      {
        QDP_LOG_DEBUG("successfully queried profile [%d]",
                      ps_req.profile.profile_index);
      }

      if(ps_resp.profile_name_valid)
      {
        if(!strncasecmp(ps_resp.profile_name, QDP_PROFILE_IA,strlen(QDP_PROFILE_IA)+1))
         {
           *ia_profile_id= ps_req.profile.profile_index;
           QDP_LOG_DEBUG("[%d] profile id is a qdp profile IA type profile", *ia_profile_id);
           break;
         }
      }

    }

  }while(0);

    if (QDP_SUCCESS != reti)
    {
      QDP_LOG_ERROR("%s","qdp_get_ia_type_profile_id EXIT failed");
    }
    else
    {
      QDP_LOG_DEBUG("%s","qdp_get_ia_type_profile_id EXIT success");
    }

  if(*ia_profile_id != -1)
  {
    return QDP_SUCCESS;
  }
  else
  {
    return QDP_FAILURE;
  }

}


/*===========================================================================
    FUNCTION:  qdp_3gpp_alt_multi_param_search
    ===========================================================================*/
/*!
      @brief
      qdp_3gpp_alt_multi_param_search is similar to qdp_3gpp_multi_param_search
      but would take a parameter to decide on the comparision.

      @return
      QDP_SUCCESS
      QDP_FAILURE
*/
/*=========================================================================*/
int qdp_3gpp_alt_multi_param_search
(
  int                                    param_to_match,
  int                                    param_value,
  unsigned int                           *profile_list,
  uint8                                  *profile_list_len,
  qdp_profile_pdn_type                   *profile_pdn_type,  /* Profile PDN type */
  qdp_error_t                            *lookup_error  /* out placeholder */
)
{
  uint32_t i=0;
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int rc;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  QDP_INIT_BARRIER;

  do
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_alt_multi_param_search ENTRY");

    QDP_LOG_DEBUG("%s","qdp_3gpp_alt_multi_param_search [%d] param_to_match and [%d] param_value"
                     ,param_to_match,param_value);

    if (NULL == profile_list      ||
        NULL == profile_pdn_type ||
        NULL == profile_list_len ||
        NULL == lookup_error )
    {
      QDP_LOG_ERROR("%s","invalid params");
      break;
    }

    if (QDP_INVALID_PROFILE != profile_list[0])
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    profile_list[0], QDP_INVALID_PROFILE);
      break;
    }

    if (QDP_PROFILE_PDN_TYPE_INVALID != *profile_pdn_type)
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    *profile_pdn_type, QDP_PROFILE_PDN_TYPE_INVALID);
      break;
    }

    *profile_list_len = 0;

    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("get_profile_list failed with error [%d] " \
                    "qmi_err_code [%d]", pl_resp.resp.result, pl_resp.resp.error);
      break;
    }

    QDP_LOG_DEBUG("get_profile_list for profile_tech [%d] "   \
                  "returned [%d] profile ids",
                  pl_req.profile_type, pl_resp.profile_list_len);

    reti = QDP_SUCCESS;

    for(i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("query_profile failed with error [%d] " \
                      "qmi_err_code [%d]", ps_resp.resp.result, ps_resp.resp.error);
        reti = QDP_FAILURE;
        break;
      }
      else
      {
        QDP_LOG_DEBUG("successfully queried 3GPP profile [%d]",
                      ps_req.profile.profile_index);

        QDP_LOG_DEBUG("Profile CLASS of [%d] profile is [%d/%d]",
                      ps_req.profile.profile_index,
                      ps_resp.apn_class_valid,ps_resp.apn_class);
      }

      if(param_to_match == QDP_RIL_CLASS)
      {
        if (ps_resp.apn_class_valid &&
            ps_resp.apn_class == param_value)
        {
          if(*profile_list_len < QDP_NUM_PROFILES_EXPECTED_MAX)
          {
            profile_list[*profile_list_len] = (unsigned int)ps_req.profile.profile_index;

            *profile_pdn_type = qdp_get_3gpp_profile_pdn_type(ps_resp.pdp_type,
                                                              ps_resp.pdp_type_valid);

            QDP_LOG_DEBUG("APN CLASS Matched with %d Profile, saved at index %d",
                                  ps_req.profile.profile_index,*profile_list_len);

            *profile_list_len += 1;
          }
        }
      }

    } /* for */
    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp_alt_multi_param_search EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_alt_multi_param_search EXIT success");
    *lookup_error = QDP_ERROR_NONE;
  }

  return ret;
}
/*===========================================================================
    FUNCTION:  qdp_3gpp2_alt_multi_param_search
    ===========================================================================*/
/*!
      @brief
      qdp_3gpp_alt_multi_param_search is similar to qdp_3gpp2_multi_param_search
      but would take a parameter to decide on the comparision.

      @return
      QDP_SUCCESS
      QDP_FAILURE
*/
/*=========================================================================*/
int qdp_3gpp2_alt_multi_param_search
(
  int                                    param_to_match,
  int                                    param_value,
  unsigned int                           *profile_list,
  uint8                                  *profile_list_len,
  qdp_profile_pdn_type                   *profile_pdn_type,  /* Profile PDN type */
  qdp_error_t                            *lookup_error  /* out placeholder */
)
{
  uint32_t i=0;
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int rc;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  QDP_INIT_BARRIER;

  do
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp2_alt_multi_param_search ENTRY");

    QDP_LOG_DEBUG("%s","qdp_3gpp2_alt_multi_param_search [%d] param_to_match and [%d] param_value"
                     ,param_to_match,param_value);

    if (NULL == profile_list      ||
        NULL == profile_pdn_type ||
        NULL == profile_list_len ||
        NULL == lookup_error )
    {
      QDP_LOG_ERROR("%s","invalid params");
      break;
    }

    if (QDP_INVALID_PROFILE != profile_list[0])
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    profile_list[0], QDP_INVALID_PROFILE);
      break;
    }

    if (QDP_PROFILE_PDN_TYPE_INVALID != *profile_pdn_type)
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    *profile_pdn_type, QDP_PROFILE_PDN_TYPE_INVALID);
      break;
    }

    *profile_list_len = 0;

    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("get_profile_list failed with error [%d] " \
                    "qmi_err_code [%d]", pl_resp.resp.result, pl_resp.resp.error);
      break;
    }

    QDP_LOG_DEBUG("get_profile_list for profile_tech [%d] "   \
                  "returned [%d] profile ids",
                  pl_req.profile_type, pl_resp.profile_list_len);

    reti = QDP_SUCCESS;

    for(i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("query_profile failed with error [%d] " \
                      "qmi_err_code [%d]", ps_resp.resp.result, ps_resp.resp.error);
        reti = QDP_FAILURE;
        break;
      }
      else
      {
        QDP_LOG_DEBUG("successfully queried 3GPP2 profile [%d]",
                      ps_req.profile.profile_index);

        QDP_LOG_DEBUG("Profile CLASS of [%d] profile is [%d/%d]",
                      ps_req.profile.profile_index,
                      ps_resp.apn_class_3gpp2_valid,ps_resp.apn_class_3gpp2);
      }

      if(param_to_match == QDP_RIL_CLASS)
      {
        if (ps_resp.apn_class_3gpp2_valid &&
            ps_resp.apn_class_3gpp2 == param_value)
        {
          if(*profile_list_len < QDP_NUM_PROFILES_EXPECTED_MAX)
          {
            profile_list[*profile_list_len] = (unsigned int)ps_req.profile.profile_index;

            *profile_pdn_type = qdp_get_3gpp2_profile_pdn_type(ps_resp.pdn_type,
                                                              ps_resp.pdn_type_valid);
            QDP_LOG_DEBUG("APN CLASS Matched with %d Profile, saved at index %d",
                                  ps_req.profile.profile_index,*profile_list_len);

            *profile_list_len += 1;

          }
        }
      }

    } /* for */
    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp2_alt_multi_param_search EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp2_alt_multi_param_search EXIT success");
    *lookup_error = QDP_ERROR_NONE;
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_3gpp2_multi_param_search
===========================================================================*/
/*!
    @brief
    searches through list of 3GPP2 profiles using the combination of
    given parameters (ANDed together) as a key
    If one of more profiles are found based on this search, it
    returns the profile ids in profile_num_list placeholder

    @params
    params: pointer to the array of parameters such that *param[i] is
    the ith parameter. This array must be exactly QDP_RIL_PARAM_MAX
    long, and must contain parameters in the order specified by
    qdp_ril_param_idx_t. If a parameter is not required, it can be
    left out NULL in the array.
    profile_num: placeholder for the returned profile id (if found).
    lookup_error: specific lookup error code

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_3gpp2_multi_param_search
(
  const qdp_param_t     *params,  /* array index indicates which parameter
                                   * it is based on qdp_ril_param_idx_t */
  unsigned int          *profile_num, /* out placeholder, *profile_num
                                       * must be set to 0 by caller */
  qdp_profile_pdn_type  *profile_pdn_type,  /* Profile PDN type */
  qdp_profile_info_type *p_params, /* out placeholder */
  qdp_error_t           *lookup_error  /* out placeholder */
)
{
  uint32_t i=0;
  int ret = QDP_FAILURE;
  int reti = QDP_SUCCESS;
  int rc;
  boolean match_found = FALSE;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  QDP_INIT_BARRIER;

  do
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp2_multi_param_search ENTRY");

    if (NULL == params           ||
        NULL == profile_num      ||
        NULL == profile_pdn_type ||
        NULL == lookup_error )
    {
      QDP_LOG_ERROR("%s","invalid params");
      break;
    }

    if (QDP_INVALID_PROFILE != *profile_num)
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    *profile_num, QDP_INVALID_PROFILE);
      break;
    }

    if (QDP_PROFILE_PDN_TYPE_INVALID != *profile_pdn_type)
    {
      QDP_LOG_ERROR("output placeholder [%d] is not [%d]",
                    *profile_pdn_type, QDP_PROFILE_PDN_TYPE_INVALID);
      break;
    }

    if (p_params)
    {
      p_params->is_valid = FALSE;
    }

    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("get_profile_list failed with error [%d] " \
                    "qmi_err_code [%d]", pl_resp.resp.result, pl_resp.resp.error);
      break;
    }

    QDP_LOG_DEBUG("get_profile_list for profile_tech [%d] "   \
                  "returned [%d] profile ids",
                  pl_req.profile_type, pl_resp.profile_list_len);

    reti = QDP_SUCCESS;

    for(i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("query_profile failed with error [%d] " \
                      "qmi_err_code [%d]", ps_resp.resp.result, ps_resp.resp.error);
        reti = QDP_FAILURE;
        break;
      }
      else
      {
        QDP_LOG_DEBUG("successfully queried 3GPP2 profile [%d]",
                      ps_req.profile.profile_index);
      }

      /* every time there is specific lookup error that needs to be
         handled in QDP, the lookup_error variable will be updated */
      rc = qdp_match_3gpp2_profile_params( params,
                                           &ps_resp,
                                           ps_req.profile.profile_index,
                                           &match_found,
                                           lookup_error );
      if (QDP_FAILURE == rc)
      {
        QDP_LOG_ERROR("%s","programming error");
        reti = QDP_FAILURE;
        break;
      }

      if(match_found)
      {
        *profile_num = ps_req.profile.profile_index;
        *profile_pdn_type = qdp_get_3gpp2_profile_pdn_type(ps_resp.pdn_type,
                                                           ps_resp.pdn_type_valid);

        if (NULL != p_params)
        {
          /* Copy the profile information into the out placeholder for profile params */
          p_params->is_valid = TRUE;
          memcpy( &p_params->prof_id,  &ps_req.profile, sizeof(p_params->prof_id));
          memcpy( &p_params->prof_set, &ps_resp, sizeof(p_params->prof_set));
        }

        /* reset lookup error if it was set while matching
           any profiles queried before this point */
        QDP_LOG_DEBUG("%s", "resetting lookup_error - if any");
        *lookup_error = QDP_ERROR_NONE;
        break;
      }

    } /* for */
    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp2_multi_param_search EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp2_multi_param_search EXIT success");
  }

  return ret;
}


/*===========================================================================
  FUNCTION: qdp_3gpp_profile_update
===========================================================================*/
/*!
  @brief
  Updates profile information on the modem

*/
/*=========================================================================*/
static int qdp_3gpp_profile_update
(
  qdp_param_t   *params,
  unsigned int  profile_id,
  int           *error_code,
  qdp_calling_func_t  calling_function
)
{
  int rc, i, ret = QDP_FAILURE;
  wds_modify_profile_settings_req_msg_v01   req;
  wds_modify_profile_settings_resp_msg_v01  resp;

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update ENTRY");

  do
  {
    if (params == NULL)
    {
      QDP_LOG_ERROR("Invalid profile params for profile_id [%d]", profile_id);
      break;
    }

    QDP_LOG_DEBUG("Trying to update profile for profile id [%d]", profile_id);

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    /* Fill the WDS 3GPP profile parameters request structure */
    if (QDP_SUCCESS != qdp_3gpp_profile_fill_common_params(params,
                                                           profile_id,
                                                           &req,
                                                           calling_function))
    {
      QDP_LOG_ERROR("Failed to fill 3gpp profile params for profile_id [%d]", profile_id);
      break;
    }

    /* Update profile on modem */
    if (QDP_SUCCESS != qdp_qmi_wds_modify_profile(global_qmi_wds_hndl,
                                                  &req,
                                                  &resp))
    {
      QDP_LOG_ERROR("Failed to modify 3gpp profile params for profile_id [%d]", profile_id);
      *error_code = resp.resp.error;
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update EXIT");

  return ret;
} /* qdp_3gpp_profile_update */


/*===========================================================================
 FUNCTION: qdp_3gpp_profile_fill_common_params
===========================================================================*/
/*!
    @brief
    Updates the 3gpp WDS request structure with the given QDP params

    @params

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
static int qdp_3gpp_profile_fill_common_params
(
  qdp_param_t                              *params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params,
  qdp_calling_func_t                       calling_function
)
{

  int rc, i, ret = QDP_FAILURE;
  wds_get_profile_settings_req_msg_v01   req;
  wds_get_profile_settings_resp_msg_v01  resp;
  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));
  boolean qdp_created_profile = FALSE;
  boolean clat_enabled = FALSE;
  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_fill_common_params ENTRY");

  do
  {
    if (params == NULL || p_params == NULL)
    {
      QDP_LOG_ERROR("Invalid profile params for 3GPP profile_id [%d]", profile_id);
      break;
    }

    QDP_LOG_DEBUG("Updating 3GPP WDS profile params for profile id [%d]", profile_id);

    p_params->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
    p_params->profile.profile_index = profile_id;


    req.profile.profile_index = profile_id;
    req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                 &req,
                                                 &resp))
    {
      QDP_LOG_ERROR("Profile querying failed for profile [%d] with error code [%d][%d]",
                     profile_id, resp.resp.result, resp.resp.error);
      break;
    }
    else
    {
      QDP_LOG_DEBUG("Succesfully queried [%d] 3GPP profile",profile_id);
    }

    if ((resp.profile_name_valid) &&
        (!strncasecmp(resp.profile_name,
                     QDP_UNIQUE_PROFILE_NAME,
                     strlen(QDP_UNIQUE_PROFILE_NAME))))
    {
      qdp_created_profile = TRUE;
      QDP_LOG_DEBUG("Profile ID [%d] is a QDP profile",profile_id);
    }

    if (resp.clat_enabled_valid)
    {
      clat_enabled = resp.clat_enabled ? TRUE : FALSE;
    }

    for (i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      switch (i)
      {
      case QDP_RIL_APN:
        if (params[i].len > QMI_WDS_APN_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of APN [%s] rcvd",
                        params[i].len, params[i].buf);
          ret = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          p_params->apn_name_valid = TRUE;
          std_strlcpy(p_params->apn_name,
                      params[i].buf,
                      sizeof(p_params->apn_name));
        }
        else if (params[i].len == 0)
        {
          p_params->apn_name_valid = TRUE;
          p_params->apn_name[0] = '\0';
        }
        break;
      case QDP_RIL_NAI:
        if (params[i].len > QMI_WDS_USER_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of USERNAME [%s] rcvd",
                        params[i].len, params[i].buf);
          ret = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          p_params->username_valid = TRUE;
          std_strlcpy(p_params->username,
                      params[i].buf,
                      sizeof(p_params->username));
        }
        break;
      case QDP_RIL_PASSWORD:
        if (params[i].len > QMI_WDS_PASSWORD_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of PASSWORD [%s] rcvd",
                        params[i].len, params[i].buf);
          ret = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          p_params->password_valid = TRUE;
          std_strlcpy(p_params->password,
                      params[i].buf,
                      sizeof(p_params->password));
        }
        break;
      case QDP_RIL_AUTH:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_NOT_ALLOWED) == 0)
          {
            p_params->authentication_preference_valid = TRUE;
            p_params->authentication_preference = 0;
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_ONLY_ALLOWED) == 0)
          {
            p_params->authentication_preference_valid = TRUE;
            p_params->authentication_preference = QMI_WDS_MASK_AUTH_PREF_PAP_V01;
          }
          else if (strcmp(params[i].buf, QDP_RIL_CHAP_ONLY_ALLOWED) == 0)
          {
            p_params->authentication_preference_valid = TRUE;
            p_params->authentication_preference = QMI_WDS_MASK_AUTH_PREF_CHAP_V01;
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_BOTH_ALLOWED) == 0)
          {
            p_params->authentication_preference_valid = TRUE;
            p_params->authentication_preference = (QMI_WDS_MASK_AUTH_PREF_PAP_V01 |
                                             QMI_WDS_MASK_AUTH_PREF_CHAP_V01);
          }
          else
          {
            QDP_LOG_ERROR("invalid auth_pref [%s]", params[i].buf);
            ret = QDP_FAILURE;
            break;
          }
        }
        break;
      case QDP_RIL_IP_FAMILY:
        if (params[i].len > 0)
        {
          /*Need to do blind profile updation for set_data_profile*/
          if(( qdp_created_profile &&
               calling_function == QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH) ||
              calling_function == QDP_PROFILE_LOOKUP_FOR_SET_DATA_PROFILE ||
              (qdp_allow_update_modem_profile && !clat_enabled))
          {
            if (strcmp(params[i].buf, QDP_RIL_IP_4) == 0)
            {
              p_params->pdp_type_valid = TRUE;
              p_params->pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
            }
            else if(strcmp(params[i].buf, QDP_RIL_IP_6) == 0)
            {
              p_params->pdp_type_valid = TRUE;
              p_params->pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
            }
            else if(strcmp(params[i].buf, QDP_RIL_IP_4_6) == 0)
            {
              p_params->pdp_type_valid = TRUE;
              p_params->pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
            }
            else
            {
              QDP_LOG_ERROR("invalid ip_family [%s]", params[i].buf);
              ret = QDP_FAILURE;
              break;
            }
          }
          else
          {
            p_params->pdp_type_valid = TRUE;
            p_params->pdp_type = resp.pdp_type;
            QDP_LOG_DEBUG("Profile is Non-qdp. Picking previous IP type [%d]",
                               resp.pdp_type);
          }
        }
        break;
      case QDP_RIL_IP_ROAMING:
        if (params[i].len > 0)
        {
          /*Need to do blind profile updation for set_data_profile*/
          if(( qdp_created_profile &&
               calling_function == QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH) ||
              calling_function == QDP_PROFILE_LOOKUP_FOR_SET_DATA_PROFILE ||
              (qdp_allow_update_modem_profile && !clat_enabled))
          {
            if (strcmp(params[i].buf, QDP_RIL_IP_4) == 0)
            {
              p_params->lte_roaming_pdp_type_valid = TRUE;
              p_params->umts_roaming_pdp_type_valid = TRUE;
              p_params->lte_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4_V01;
              p_params->umts_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4_V01;
            }
            else if(strcmp(params[i].buf, QDP_RIL_IP_6) == 0)
            {
              p_params->lte_roaming_pdp_type_valid = TRUE;
              p_params->umts_roaming_pdp_type_valid = TRUE;
              p_params->lte_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV6_V01;
              p_params->umts_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV6_V01;
            }
            else if(strcmp(params[i].buf, QDP_RIL_IP_4_6) == 0)
            {
              p_params->lte_roaming_pdp_type_valid = TRUE;
              p_params->umts_roaming_pdp_type_valid = TRUE;
              p_params->lte_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01;
              p_params->umts_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01;
            }
            else
            {
              QDP_LOG_ERROR("invalid roaming ip [%s]", params[i].buf);
              ret = QDP_FAILURE;
              break;
            }
          }
          else
          {
            p_params->lte_roaming_pdp_type_valid = FALSE;
            p_params->umts_roaming_pdp_type_valid = FALSE;
          }
        }
        break;
      default:
        QDP_LOG_DEBUG("ignoring RIL param idx [%d]", i);
        break;
      }
    }

    ret = QDP_SUCCESS;
  } while (0);

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_fill_common_params EXIT");

  return ret;
}

/*===========================================================================
  FUNCTION: qdp_3gpp2_profile_update
===========================================================================*/
/*!
  @brief
  Updates profile information on the modem

*/
/*=========================================================================*/
static int qdp_3gpp2_profile_update
(
  qdp_param_t   *params,
  unsigned int  profile_id,
  int           *error_code
)
{
  int rc, i, ret = QDP_FAILURE;
  wds_modify_profile_settings_req_msg_v01   req;
  wds_modify_profile_settings_resp_msg_v01  resp;

  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_update ENTRY");

  do
  {
    if (params == NULL)
    {
      QDP_LOG_ERROR("Invalid profile params for profile_id [%d]", profile_id);
      break;
    }

    QDP_LOG_DEBUG("Trying to update profile for profile id [%d]", profile_id);

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    /* Fill the WDS 3GPP profile parameters request structure */
    if (QDP_SUCCESS != qdp_3gpp2_profile_fill_common_params(params,
                                                           profile_id,
                                                           &req))
    {
      QDP_LOG_ERROR("Failed to fill 3gpp profile params for profile_id [%d]", profile_id);
      break;
    }

    /* Update profile on modem */
    if (QDP_SUCCESS != qdp_qmi_wds_modify_profile(global_qmi_wds_hndl,
                                                  &req,
                                                  &resp))
    {
      QDP_LOG_ERROR("Failed to modify 3gpp profile params for profile_id [%d]", profile_id);
      *error_code = resp.resp.error;
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update EXIT");

  return ret;
} /* qdp_3gpp_profile_update */


/*===========================================================================
 FUNCTION: qdp_3gpp2_profile_fill_common_params
===========================================================================*/
/*!
    @brief
    Updates the 3gpp2 WDS request structure with the given QDP params

    @params

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
static int qdp_3gpp2_profile_fill_common_params
(
  qdp_param_t                              *params,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
)
{

  int rc, i, ret = QDP_FAILURE;

  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_fill_common_params ENTRY");

  do
  {
    if (params == NULL || p_params == NULL)
    {
      QDP_LOG_ERROR("Invalid profile params for 3GPP2 profile_id [%d]", profile_id);
      break;
    }

    QDP_LOG_DEBUG("Updating 3GPP2 WDS profile params for profile id [%d]", profile_id);

    p_params->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
    p_params->profile.profile_index = profile_id;

    for (i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      switch (i)
      {
      case QDP_RIL_APN:
        if (params[i].len > QMI_WDS_APN_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of APN [%s] rcvd",
                        params[i].len, params[i].buf);
          ret = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          p_params->apn_string_valid = TRUE;
          std_strlcpy(p_params->apn_string,
                      params[i].buf,
                      sizeof(p_params->apn_string));
        }
        else if (params[i].len == 0)
        {
          p_params->apn_string_valid = TRUE;
          p_params->apn_string[0] = '\0';
        }
        break;
      case QDP_RIL_NAI:
        if (params[i].len > QMI_WDS_USER_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of USERNAME [%s] rcvd",
                        params[i].len, params[i].buf);
          ret = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          p_params->user_id_valid = TRUE;
          std_strlcpy(p_params->user_id,
                      params[i].buf,
                      sizeof(p_params->user_id));

          p_params->pdn_level_user_id_valid = TRUE;
          std_strlcpy(p_params->pdn_level_user_id,
                      params[i].buf,
                      sizeof(p_params->pdn_level_user_id));

        }
        break;
      case QDP_RIL_PASSWORD:
        if (params[i].len > QMI_WDS_PASSWORD_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of PASSWORD [%s] rcvd",
                        params[i].len, params[i].buf);
          ret = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          p_params->auth_password_valid = TRUE;
          std_strlcpy(p_params->auth_password,
                      params[i].buf,
                      sizeof(p_params->auth_password));

          /* Adding PDN level user name and password */
          p_params->pdn_level_auth_password_valid = TRUE;
          std_strlcpy(p_params->pdn_level_auth_password,
                      params[i].buf,
                      sizeof(p_params->pdn_level_auth_password));

        }
        break;
      case QDP_RIL_AUTH:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_NOT_ALLOWED) == 0)
          {
            p_params->auth_protocol_valid = TRUE;
            p_params->auth_protocol = 0;
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_ONLY_ALLOWED) == 0)
          {
            p_params->auth_protocol_valid = TRUE;
            p_params->auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_V01;
          }
          else if (strcmp(params[i].buf, QDP_RIL_CHAP_ONLY_ALLOWED) == 0)
          {
            p_params->auth_protocol_valid = TRUE;
            p_params->auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01;
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_BOTH_ALLOWED) == 0)
          {
            p_params->auth_protocol_valid = TRUE;
            p_params->auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;
          }
          else
          {
            QDP_LOG_ERROR("invalid auth_pref [%s]", params[i].buf);
            ret = QDP_FAILURE;
            break;
          }
        }
        break;
      case QDP_RIL_IP_FAMILY:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_IP_4) == 0)
          {
            p_params->pdn_type_valid = TRUE;
            p_params->pdn_type = WDS_PROFILE_PDN_TYPE_IPV4_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_6) == 0)
          {
            p_params->pdn_type_valid = TRUE;
            p_params->pdn_type = WDS_PROFILE_PDN_TYPE_IPV6_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_4_6) == 0)
          {
            p_params->pdn_type_valid = TRUE;
            p_params->pdn_type = WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01;
          }
          else
          {
            QDP_LOG_ERROR("invalid ip_family [%s]", params[i].buf);
            ret = QDP_FAILURE;
            break;
          }
        }
        break;
      default:
        QDP_LOG_DEBUG("ignoring RIL param idx [%d]", i);
        break;
      }
    }

    ret = QDP_SUCCESS;
  } while (0);

  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_fill_common_params EXIT");

  return ret;
}


/*===========================================================================
 FUNCTION: qdp_profile_update_ex
===========================================================================*/

/*=========================================================================*/
int qdp_profile_update_ex
(
  wds_modify_profile_settings_req_msg_v01  *p_params,
  int                                      *error_code
)
{
  int rc;
  wds_modify_profile_settings_resp_msg_v01  resp;

  QDP_LOG_DEBUG("%s","qdp_profile_update_ex ENTRY");

  QDP_INIT_BARRIER;

  memset(&resp, 0, sizeof(resp));

  rc = qdp_qmi_wds_modify_profile(global_qmi_wds_hndl, p_params, &resp);

  if (error_code)
  {
    *error_code = (rc != QDP_SUCCESS) ? resp.resp.error : QMI_ERR_NONE_V01;
  }

  QDP_LOG_DEBUG("%s","qdp_profile_update_ex EXIT");

  return rc;
}

/*===========================================================================
 FUNCTION: qdp_modify_profile
===========================================================================*/
/*!
    @brief
    Updates the Modem profile referenced by the given input parameter

    @params
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
)
{
  int rc = QDP_FAILURE;
  wds_modify_profile_settings_req_msg_v01   req;
  wds_modify_profile_settings_resp_msg_v01  resp;

  QDP_LOG_DEBUG("%s","qdp_modify_profile ENTRY");

  QDP_INIT_BARRIER;

  do
  {
    if (profile == NULL || FALSE == profile->is_valid)
    {
      QDP_LOG_ERROR("%s", "Invalid input params for profile");
      break;
    }

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.profile.profile_index = profile->prof_id.profile_index;
    req.profile.profile_type = profile->prof_id.profile_type;

    /* Update 3GPP profile specific parameters */
    if (WDS_PROFILE_TYPE_3GPP_V01 == profile->prof_id.profile_type)
    {
      if (profile->prof_set.apn_name_valid)
      {
        req.apn_name_valid = TRUE;
        std_strlcpy(req.apn_name,
                    profile->prof_set.apn_name,
                    sizeof(req.apn_name));
      }

      if (profile->prof_set.username_valid)
      {
        req.username_valid = TRUE;
        std_strlcpy(req.username,
                    profile->prof_set.username,
                    sizeof(req.username));
      }

      if (profile->prof_set.password_valid)
      {
        req.password_valid = TRUE;
        std_strlcpy(req.password,
                    profile->prof_set.password,
                    sizeof(req.password));
      }

      if (profile->prof_set.authentication_preference_valid)
      {
        req.authentication_preference_valid = TRUE;
        req.authentication_preference = profile->prof_set.authentication_preference;
      }

      if (profile->prof_set.pdp_type_valid)
      {
        req.pdp_type_valid = TRUE;
        req.pdp_type = profile->prof_set.pdp_type;
      }
    }

    /* Update 3GPP2 profile specific parameters */
    if (WDS_PROFILE_TYPE_3GPP2_V01 == profile->prof_id.profile_type)
    {
      if (profile->prof_set.apn_string_valid)
      {
        req.apn_string_valid = TRUE;
        std_strlcpy(req.apn_string,
                    profile->prof_set.apn_string,
                    sizeof(req.apn_string));
      }

      if (profile->prof_set.user_id_valid)
      {
        req.user_id_valid = TRUE;
        std_strlcpy(req.user_id,
                    profile->prof_set.user_id,
                    sizeof(req.user_id));
      }

      if (profile->prof_set.pdn_level_user_id_valid)
      {
        req.pdn_level_user_id_valid = TRUE;
        std_strlcpy(req.pdn_level_user_id,
                    profile->prof_set.pdn_level_user_id,
                    sizeof(req.pdn_level_user_id));
      }

      if (profile->prof_set.auth_password_valid)
      {
        req.auth_password_valid = TRUE;
        std_strlcpy(req.auth_password,
                    profile->prof_set.auth_password,
                    sizeof(req.auth_password));
      }

      if (profile->prof_set.pdn_level_auth_password_valid)
      {
        req.pdn_level_auth_password_valid = TRUE;
        std_strlcpy(req.pdn_level_auth_password,
                    profile->prof_set.pdn_level_auth_password,
                    sizeof(req.pdn_level_auth_password));
      }

      if (profile->prof_set.auth_protocol_valid)
      {
        req.auth_protocol_valid = TRUE;
        req.auth_protocol = profile->prof_set.auth_protocol;
      }

      if (profile->prof_set.pdn_level_auth_protocol_valid)
      {
        req.pdn_level_auth_protocol_valid = TRUE;
        req.pdn_level_auth_protocol = profile->prof_set.pdn_level_auth_protocol;
      }

      if (profile->prof_set.pdn_type_valid)
      {
        req.pdn_type_valid = TRUE;
        req.pdn_type = profile->prof_set.pdn_type;
      }
    }

    rc = qdp_qmi_wds_modify_profile(global_qmi_wds_hndl, &req, &resp);

    if (error_code)
    {
      *error_code = (rc != QDP_SUCCESS) ? resp.resp.error : QMI_ERR_NONE_V01;
    }
  }
  while (0);

  QDP_LOG_DEBUG("%s","qdp_modify_profile EXIT");

  return rc;
}

/*===========================================================================
 FUNCTION: qdp_3gpp_profile_update_params
===========================================================================*/
/*!
    @brief
    Updates the WDS input structure with the given 3gpp profile parameters

    @params
    profile: profile and parameters to update
    error_code: return any error during update (out)

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
int qdp_3gpp_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
)
{
  int ret = QDP_FAILURE;
  int i=0, temp_len = 0;

  /* profile lookup params */
  qdp_param_t params_qdp[QDP_RIL_PARAM_MAX];

  memset(params_qdp, 0, sizeof(params_qdp));

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update_params ENTRY");

  QDP_INIT_BARRIER;

  do
  {
    if( NULL == param_strings ||
        NULL == p_params )
    {
      QDP_LOG_ERROR("%s","NULL params rcvd");
      break;
    }

    /* go through each of the RIL parameter received in order to
     * determine which technology we need to look up profile for
     * prepare profile_look_up parameters */
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      if (param_strings[QDP_GET_RIL_PARAM_IDX(i)] != NULL &&
          strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) != 0)
      {
        /* +1 for NULL end character */
        temp_len = (int)strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)])+1;
        if(temp_len > QDP_GET_3GPP_MAX_LEN(i))
        {
          QDP_LOG_ERROR("RIL param length too long [%d] for ril index [%d]",
                        temp_len, i);
        }
        else
        {
          QDP_MALLOC(params_qdp[i].buf, (size_t)temp_len);
        }

        if(NULL != params_qdp[i].buf)
        {
          params_qdp[i].len = temp_len-1;
          std_strlcpy(params_qdp[i].buf,
                      param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                      (size_t)temp_len);
          QDP_LOG_DEBUG("copied [%s], len[%d] to [%p] loc",
                        param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                        params_qdp[i].len,
                        params_qdp[i].buf);
        }
        else
        {
          QDP_LOG_ERROR("memory error while trying to allocate 3gpp "
                        "param for [%s]", param_strings[QDP_GET_RIL_PARAM_IDX(i)]);
        }
      } /* for each valid RIL parameter */
    } /* for each RIL parameter */

    if (QDP_SUCCESS != qdp_3gpp_profile_fill_common_params(params_qdp,
                                                           profile_id,
                                                           p_params,
                                                           QDP_PROFILE_LOOKUP_FOR_SET_DATA_PROFILE))
    {
      QDP_LOG_ERROR("failed to update 3gpp params structure for profile_id [%d]",
                    profile_id);
      break;
    }

    ret = QDP_SUCCESS;

  } while (0);

  /* clean up memory */
  qdp_free_qdp_params(params_qdp, QDP_RIL_PARAM_MAX);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp_profile_update_params EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_profile_update_params EXIT success");
  }

  return ret;
}


/*===========================================================================
 FUNCTION: qdp_3gpp2_profile_update_params
===========================================================================*/
/*!
    @brief
    Updates the WDS input structure with the given profile parameters

    @params
    profile: profile and parameters to update
    error_code: return any error during update (out)

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/

/*=========================================================================*/
int qdp_3gpp2_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
)

{
  int ret = QDP_FAILURE;
  int i=0, temp_len = 0;

  /* profile lookup params */
  qdp_param_t params_qdp[QDP_RIL_PARAM_MAX];

  memset(params_qdp, 0, sizeof(params_qdp));

  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_update_params ENTRY");

  QDP_INIT_BARRIER;

  do
  {
    if( NULL == param_strings ||
        NULL == p_params )
    {
      QDP_LOG_ERROR("%s","NULL params rcvd");
      break;
    }

    /* go through each of the RIL parameter received in order to
     * determine which technology we need to look up profile for
     * prepare profile_look_up parameters */
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      if (param_strings[QDP_GET_RIL_PARAM_IDX(i)] != NULL &&
          strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) != 0)
      {
        /* +1 for NULL end character */
        temp_len = (int)strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)])+1;
        if(temp_len > QDP_GET_3GPP_MAX_LEN(i))
        {
          QDP_LOG_ERROR("RIL param length too long [%d] for ril index [%d]",
                        temp_len, i);
        }
        else
        {
          QDP_MALLOC(params_qdp[i].buf, (size_t)temp_len);
        }

        if(NULL != params_qdp[i].buf)
        {
          params_qdp[i].len = temp_len-1;
          std_strlcpy(params_qdp[i].buf,
                      param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                      (size_t)temp_len);
          QDP_LOG_DEBUG("copied [%s], len[%d] to [%p] loc",
                        param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                        params_qdp[i].len,
                        params_qdp[i].buf);
        }
        else
        {
          QDP_LOG_ERROR("memory error while trying to allocate "
                        "param for [%s]", param_strings[QDP_GET_RIL_PARAM_IDX(i)]);
        }
      } /* for each valid RIL parameter */
    } /* for each RIL parameter */

    if (QDP_SUCCESS != qdp_3gpp2_profile_fill_common_params(params_qdp,
                                                          profile_id,
                                                          p_params))
    {
      QDP_LOG_ERROR("failed to update 3gpp2 params structure for profile_id [%d]",
                    profile_id);
      break;
    }

    ret = QDP_SUCCESS;

  } while (0);

  /* clean up memory */
  qdp_free_qdp_params(params_qdp, QDP_RIL_PARAM_MAX);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp2_profile_update_params EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_update_params EXIT success");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_3gpp_profile_create
===========================================================================*/
/*!
    @brief
    creates a new 3gpp profile based on the default 3gpp profile and
    overrides any parameters that are given as input to this function.

    reference count on the newly created profile id is initialized with
    value 1

    @params
    params:
    num_params: number of parameters given as input
    params: pointer to the array of parameters such that *param[i] is
    the ith parameter
    profile_id: placeholder to return the newly created profile id
    p_params: out parameter to hold the profile parameters for a given
    profile id before it is updated. Will be used to restore the profile
    in case LTE attach fails. For non-LTE attach cases this parameter will
    be NULL.
    is_persistent : used for LTE atatch profile creation

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_3gpp_profile_create
(
  qdp_param_t           *params,
  unsigned int          *profile_id_ptr,
  qdp_profile_pdn_type  *profile_pdn_type,
  unsigned int          is_persistent,
  qdp_calling_func_t    calling_func
)
{
  int ret = QDP_FAILURE, i=0, reti = QDP_SUCCESS, rc;
  wds_create_profile_req_msg_v01   req;
  wds_create_profile_resp_msg_v01  resp;

  QDP_INIT_BARRIER;

  QDP_LOG_DEBUG("%s","qdp_3gpp_profile_create ENTRY");

  do
  {
    if (NULL == params         ||
        NULL == profile_id_ptr ||
        NULL == profile_pdn_type)
    {
      QDP_LOG_ERROR("%s","NULL start_nw_params received");
      break;
    }

    if (QDP_INVALID_PROFILE != *profile_id_ptr)
    {
      QDP_LOG_ERROR("%s","placeholder contains non-zero value");
      break;
    }

    if (QDP_PROFILE_PDN_TYPE_INVALID != *profile_pdn_type)
    {
      QDP_LOG_ERROR("%s","placeholder contains non-zero value");
      break;
    }

    /* start with clean params */
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    reti = QDP_SUCCESS;
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      switch(i)
      {
      case QDP_RIL_APN:
        if (params[i].len > QMI_WDS_APN_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of APN [%s] rcvd",
                        params[i].len, params[i].buf);
          reti = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          req.apn_name_valid = TRUE;
          std_strlcpy(req.apn_name,
                      params[i].buf,
                      sizeof(req.apn_name));
        }
        else if (params[i].len == 0)
        {
          req.apn_name_valid = TRUE;
          req.apn_name[0] = '\0';
        }
        break;
      case QDP_RIL_NAI:
        if (params[i].len > QMI_WDS_USER_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of USERNAME [%s] rcvd",
                        params[i].len, params[i].buf);
          reti = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          req.username_valid = TRUE;
          std_strlcpy(req.username,
                      params[i].buf,
                      sizeof(req.username));
        }
        break;
      case QDP_RIL_PASSWORD:
        if (params[i].len > QMI_WDS_PASSWORD_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of PASSWORD [%s] rcvd",
                        params[i].len, params[i].buf);
          reti = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          req.password_valid = TRUE;
          std_strlcpy(req.password,
                      params[i].buf,
                      sizeof(req.password));
        }
        break;
      case QDP_RIL_AUTH:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_NOT_ALLOWED) == 0)
          {
            req.authentication_preference_valid = TRUE;
            req.authentication_preference = 0;
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_ONLY_ALLOWED) == 0)
          {
            req.authentication_preference_valid = TRUE;
            req.authentication_preference = QMI_WDS_MASK_AUTH_PREF_PAP_V01;
          }
          else if (strcmp(params[i].buf, QDP_RIL_CHAP_ONLY_ALLOWED) == 0)
          {
            req.authentication_preference_valid = TRUE;
            req.authentication_preference = QMI_WDS_MASK_AUTH_PREF_CHAP_V01;
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_BOTH_ALLOWED) == 0)
          {
            req.authentication_preference_valid = TRUE;
            req.authentication_preference = (QMI_WDS_MASK_AUTH_PREF_PAP_V01 |
                                             QMI_WDS_MASK_AUTH_PREF_CHAP_V01);
          }
          else
          {
            QDP_LOG_ERROR("invalid auth_pref [%s]", params[i].buf);
            reti = QDP_FAILURE;
            break;
          }
        }
        break;
      case QDP_RIL_IP_FAMILY:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_IP_4) == 0)
          {
            req.pdp_type_valid = TRUE;
            req.pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_6) == 0)
          {
            req.pdp_type_valid = TRUE;
            req.pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_4_6) == 0)
          {
            req.pdp_type_valid = TRUE;
            req.pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
          }
          else
          {
            QDP_LOG_ERROR("invalid ip_family [%s]", params[i].buf);
            reti = QDP_FAILURE;
            break;
          }
        }
        break;
      case QDP_RIL_IP_ROAMING:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_IP_4) == 0)
          {
            req.lte_roaming_pdp_type_valid = TRUE;
            req.umts_roaming_pdp_type_valid = TRUE;
            req.lte_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4_V01;
            req.umts_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_6) == 0)
          {
            req.lte_roaming_pdp_type_valid = TRUE;
            req.umts_roaming_pdp_type_valid = TRUE;
            req.lte_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV6_V01;
            req.umts_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV6_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_4_6) == 0)
          {
            req.lte_roaming_pdp_type_valid = TRUE;
            req.umts_roaming_pdp_type_valid = TRUE;
            req.lte_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01;
            req.umts_roaming_pdp_type = WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01;
          }
          else
          {
            QDP_LOG_ERROR("invalid roaming ip [%s]", params[i].buf);
            reti = QDP_FAILURE;
            break;
          }
        }
        break;
      default:
        QDP_LOG_DEBUG("ignoring RIL param idx [%d]", i);
        break;
      } /* switch */
      if (QDP_FAILURE == reti)
      {
        break;
      }
    } /* for */
    if (QDP_FAILURE == reti)
    {
      break;
    }

    req.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if( TRUE == is_persistent)
    {
      req.persistent_valid = TRUE;
      req.persistent = TRUE;
    }
    /* Check for non-persistent profile support */
    else if( qdp_modem_supports_nonpersist_flag )
    {
      req.persistent_valid = TRUE;
      req.persistent = FALSE;
    }

    /* set proprietary name for identification */
    req.profile_name_valid = TRUE;
    if(calling_func == QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH)
    {
      std_strlcpy(req.profile_name,
                  QDP_PROFILE_IA,
                  sizeof(req.profile_name));
     QDP_LOG_DEBUG("%s","Creating QDP_profile_IA type profile.");
    }
   else if(calling_func == QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL ||
           calling_func == QDP_PROFILE_LOOKUP_FOR_SET_DATA_PROFILE)
    {
      std_strlcpy(req.profile_name,
                  QDP_UNIQUE_PROFILE_NAME,
                  sizeof(req.profile_name));
      QDP_LOG_DEBUG("%s","Creating QDP_profile type profile.");
    }

    rc = qdp_qmi_wds_create_profile( global_qmi_wds_hndl,
                                     &req,
                                     &resp );
    if (QDP_SUCCESS != rc)
    {
      QDP_LOG_ERROR("could not create wds profile. " \
                    "qmi returned [%d] qmi_err_code [%d]",
                    resp.resp.result,resp.resp.error);
      break;
    }
    QDP_LOG_DEBUG("profile [%d] created",
                  resp.profile.profile_index);

    *profile_id_ptr = (unsigned int)resp.profile.profile_index;
    *profile_pdn_type = qdp_get_3gpp_profile_pdn_type(req.pdp_type,
                                                      req.pdp_type_valid);

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp_profile_create EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp_profile_create EXIT success");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_3gpp2_profile_create
===========================================================================*/
/*!
    @brief
    creates a new 3gpp2 profile based on the default 3gpp profile and
    overrides any parameters that are given as input to this function.

    reference count on the newly created profile id is initialized with
    value 1

    @params
    params:
    num_params: number of parameters given as input
    params: pointer to the array of parameters such that *param[i] is
    the ith parameter
    profile_id: placeholder to return the newly created profile id

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_3gpp2_profile_create
(
  qdp_param_t           *params,
  unsigned int          *profile_id_ptr,
  qdp_profile_pdn_type  *profile_pdn_type,
  unsigned int           is_persistent
)
{
  int ret = QDP_FAILURE, i=0, reti = QDP_SUCCESS, rc;
  wds_create_profile_req_msg_v01   req;
  wds_create_profile_resp_msg_v01  resp;

  QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create ENTRY");

  QDP_INIT_BARRIER;

  do
  {
    if (NULL == params ||
        NULL == profile_id_ptr)
    {
      QDP_LOG_ERROR("%s","NULL start_nw_params received");
      break;
    }

    if (QDP_INVALID_PROFILE != *profile_id_ptr)
    {
      QDP_LOG_ERROR("%s","placeholder contains non-zero value");
      break;
    }

    /* start with clean set of params */
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    reti = QDP_SUCCESS;
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      switch(i)
      {
      case QDP_RIL_APN:
        if (params[i].len > QMI_WDS_APN_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of APN [%s] rcvd",
                        params[i].len, params[i].buf);
          reti = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          req.apn_string_valid = TRUE;
          std_strlcpy(req.apn_string,
                      params[i].buf,
                      sizeof(req.apn_string));
        }
        break;
      case QDP_RIL_NAI:
        if (params[i].len > QMI_WDS_USER_NAME_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of USERNAME [%s] rcvd",
                        params[i].len, params[i].buf);
          reti = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          req.user_id_valid = TRUE;
          std_strlcpy(req.user_id,
                      params[i].buf,
                      sizeof(req.user_id));

          /* Adding PDN level user name and password */
          req.pdn_level_user_id_valid = TRUE;
          std_strlcpy(req.pdn_level_user_id,
                      params[i].buf,
                      sizeof(req.pdn_level_user_id));

          QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create copied PDN level user");
        }
        break;
      case QDP_RIL_PASSWORD:
        if (params[i].len > QMI_WDS_PASSWORD_MAX_V01-1)
        {
          QDP_LOG_ERROR("Invalid length [%d] of PASSWORD [%s] rcvd",
                        params[i].len, params[i].buf);
          reti = QDP_FAILURE;
          break;
        }
        else if (params[i].len > 0)
        {
          req.auth_password_valid = TRUE;
          std_strlcpy(req.auth_password,
                      params[i].buf,
                      sizeof(req.auth_password));

          /* Adding PDN level user name and password */
          req.pdn_level_auth_password_valid = TRUE;
          std_strlcpy(req.pdn_level_auth_password,
                      params[i].buf,
                      sizeof(req.pdn_level_auth_password));

          QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create copied PDN password user");
        }
        break;
      case QDP_RIL_AUTH:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_NOT_ALLOWED) == 0)
          {
            req.auth_protocol_valid = TRUE;
            req.auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_NONE_V01;

            /* Adding PDN level AUTH */
            req.pdn_level_auth_protocol_valid = TRUE;
            req.pdn_level_auth_protocol = WDS_PROFILE_PDN_LEVEL_AUTH_PROTOCOL_NONE_V01;

            QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create copied PDN level !PAN&CHAP");
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_ONLY_ALLOWED) == 0)
          {
            req.auth_protocol_valid = TRUE;
            req.auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_V01;

            /* Adding PDN level AUTH */
            req.pdn_level_auth_protocol_valid = TRUE;
            req.pdn_level_auth_protocol = WDS_PROFILE_PDN_LEVEL_AUTH_PROTOCOL_PAP_V01;

            QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create copied PDN level PAP");
          }
          else if (strcmp(params[i].buf, QDP_RIL_CHAP_ONLY_ALLOWED) == 0)
          {
            req.auth_protocol_valid = TRUE;
            req.auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01;

            /* Adding PDN level AUTH */
            req.pdn_level_auth_protocol_valid = TRUE;
            req.pdn_level_auth_protocol = WDS_PROFILE_PDN_LEVEL_AUTH_PROTOCOL_CHAP_V01;

            QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create copied PDN level CHAP");
          }
          else if (strcmp(params[i].buf, QDP_RIL_PAP_CHAP_BOTH_ALLOWED) == 0)
          {
            req.auth_protocol_valid = TRUE;
            req.auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;

            /* Adding PDN level AUTH */
            req.pdn_level_auth_protocol_valid = TRUE;
            req.pdn_level_auth_protocol = WDS_PROFILE_PDN_LEVEL_AUTH_PROTOCOL_PAP_CHAP_V01;

            QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create copied PDN level PAP+CHAP");
          }
          else
          {
            QDP_LOG_ERROR("invalid auth_pref [%s]", params[i].buf);
            reti = QDP_FAILURE;
            break;
          }
        }
        break;
      case QDP_RIL_IP_FAMILY:
        if (params[i].len > 0)
        {
          if (strcmp(params[i].buf, QDP_RIL_IP_4) == 0)
          {
            req.pdn_type_valid = TRUE;
            req.pdn_type = WDS_PROFILE_PDN_TYPE_IPV4_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_6) == 0)
          {
            req.pdn_type_valid = TRUE;
            req.pdn_type = WDS_PROFILE_PDN_TYPE_IPV6_V01;
          }
          else if(strcmp(params[i].buf, QDP_RIL_IP_4_6) == 0)
          {
            req.pdn_type_valid = TRUE;
            req.pdn_type = WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01;
          }
          else
          {
            QDP_LOG_ERROR("invalid ip_family [%s]", params[i].buf);
            reti = QDP_FAILURE;
            break;
          }
        }
        break;
      default:
        QDP_LOG_DEBUG("ignoring RIL param idx [%d]", i);
        break;
      } /* switch */
      if (QDP_FAILURE == reti)
      {
        break;
      }
    } /* for */
    if (QDP_FAILURE == reti)
    {
      break;
    }

    if( TRUE == is_persistent)
    {
      req.persistent_valid = TRUE;
      req.persistent = TRUE;
    }
    req.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;

    /* Check for non-persistent profile support */
    if( qdp_modem_supports_nonpersist_flag )
    {
      /* Set profile to be non-persistent */
      req.persistent_valid = TRUE;
      req.persistent = FALSE;
    }

    rc = qdp_qmi_wds_create_profile( global_qmi_wds_hndl,
                                     &req,
                                     &resp );

    if (QDP_SUCCESS != rc)
    {
      QDP_LOG_ERROR("could not create wds profile. " \
                    "qmi returned [%d] qmi_err_code [%d]",
                    resp.resp.result,resp.resp.error);
      break;
    }
    QDP_LOG_DEBUG("profile [%d] created",
                  resp.profile.profile_index);

    *profile_id_ptr = (unsigned int)resp.profile.profile_index;
    *profile_pdn_type = qdp_get_3gpp2_profile_pdn_type(req.pdn_type,
                                                       req.pdn_type_valid);

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_3gpp2_profile_create EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_3gpp2_profile_create EXIT success");
  }

  return ret;
}



/*===========================================================================
  FUNCTION:  qdp_profile_ref_up
===========================================================================*/
/*!
    @brief
    increases the reference count on the given profile

    @params
    profile_id: profile id

    @return
    QDP_FAILURE
    QDP_SUCCESS
*/
/*=========================================================================*/
int qdp_profile_ref_up
(
  unsigned int profile_id
)
{
  int i=0;
  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    if (TRUE == profile_ids[i].is_valid &&
        profile_ids[i].qmi_type.profile_index == profile_id)
    {
      profile_ids[i].ref_count++;
      QDP_LOG_DEBUG("new ref_count on profile [%d] is [%d]",
                    profile_id, profile_ids[i].ref_count);
      return QDP_SUCCESS;
    }
  }

  QDP_LOG_DEBUG("profile id [%d] not found in QDP list", profile_id);
  return QDP_FAILURE;
}

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
)
{
  int i=0;

  QDP_LOG_DEBUG("%s","qdp_profile_release ENTRY");

  QDP_INIT_BARRIER;

  for(i=0; i<QDP_NUM_PROFILES_EXPECTED_MAX; i++)
  {
    if (TRUE == profile_ids[i].is_valid &&
        profile_ids[i].qmi_type.profile_index == profile_id)
    {
      if (0 == profile_ids[i].ref_count)
      {
        QDP_LOG_ERROR("ref count on profile id [%d] is already zero",
                      profile_id);
        return QDP_FAILURE;
      }
      else
      {
        profile_ids[i].ref_count--;

        QDP_LOG_DEBUG("new ref_count on profile [%d] is [%d]",
                      profile_id, profile_ids[i].ref_count);
      }

      if ((0 == profile_ids[i].ref_count) && (FALSE == profile_ids[i].is_persistent))
      {
        QDP_LOG_DEBUG("deleting profile id [%d] now", profile_id);
        qdp_delete_profile_id(profile_id);
      }

      return QDP_SUCCESS;
    }
  }

  QDP_LOG_DEBUG("profile id [%d] not found in QDP list", profile_id);
  return QDP_FAILURE;
}


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
)
{
  int rc;

  QDP_LOG_DEBUG("%s","qdp_profile_release_ex ENTRY");

  QDP_INIT_BARRIER;

  rc = qdp_delete_profile_id(profile_id);

  QDP_LOG_DEBUG("%s","qdp_profile_release_ex EXIT");

  return rc;
}

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

  profile_id_lte: It is a in/out parameter.In Legacy case, it provides default
  attach profile id. In new implementation, it is invalid parameter and gets
  updated with correct output profile id after look up.

  input_prof_id: It is a input parameter. Applicable to newer implementation.
  For non empty attach pdn list. it holds index 0 entry of attach list
  in case of empty attach pdn list, it holds invalid value

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
  const char             **param_strings,
  unsigned int           *profile_id_lte,
  qdp_profile_pdn_type   *profile_pdp_type_lte,
  unsigned int           *input_prof_id,
  qdp_profile_info_type  *prof_params,
  qdp_error_info_t       *error_info
)
{
  int ret = QDP_FAILURE, rc, qmi_err_code;
  int i=0, temp_len = 0;
  int error_code = 0;
  int create_profile = FALSE;
  int ia_profile_id =-1;
  unsigned int                profile_id_3ggp2 = QDP_INVALID_PROFILE;
  qdp_profile_pdn_type        profile_pdp_type_3gpp2 = QDP_PROFILE_PDN_TYPE_INVALID;

  /* profile lookup params */
  qdp_param_t params_lte[QDP_RIL_PARAM_MAX];
  qdp_error_t lookup_error =  QDP_ERROR_NONE;

  qdp_profile_pdn_type        profile_pdn_type_lte;
  /* We need to track profile tech type throgh out this function
   * to update accordingly.
   */
  qdp_tech_t  lte_profile_tech_type_i;

  /* Profile id type that will be used for the default
   * LTE attach process */
  wds_get_profile_settings_req_msg_v01 default_prof_id;

  memset(params_lte, 0, sizeof(params_lte));
  QDP_LOG_DEBUG("%s","qdp_lte_attach_profile_lookup ENTRY");
  /* Invalidate PDN type .*/
  profile_pdn_type_lte = QDP_PROFILE_PDN_TYPE_INVALID;

  /* lte_profile_tech_type_i decides to go for 3GPP profile update .*/
  lte_profile_tech_type_i = QDP_NOTECH;

  QDP_INIT_BARRIER;

  do
  {
    if( NULL == param_strings          ||
        NULL == profile_id_lte         ||
        NULL == profile_pdp_type_lte   ||
        NULL == input_prof_id          ||
        NULL == error_info )
    {
      QDP_LOG_ERROR("%s","NULL params rcvd");
      break;
    }

    prof_params->is_valid = FALSE;

    /* go through each of the RIL parameter received in order to
     * determine which technology we need to look up profile for
     * prepare profile_look_up parameters */
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {
      if (param_strings[QDP_GET_RIL_PARAM_IDX(i)] != NULL &&
            (strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) != 0 ||
              (i == QDP_RIL_APN &&
                strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) == 0)
            )
          )
      {
        /* +1 for NULL end character */
        temp_len = (int)strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)])+1;
        if(temp_len > QDP_GET_3GPP_MAX_LEN(i))
        {
          QDP_LOG_ERROR("RIL param length too long [%d] for ril index [%d]",
                        temp_len, i);
        }
        else
        {
          QDP_MALLOC(params_lte[i].buf, (size_t)temp_len);
        }

        if(NULL != params_lte[i].buf)
        {
          params_lte[i].len = temp_len-1;
          std_strlcpy(params_lte[i].buf,
                      param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                      (size_t)temp_len);
          QDP_LOG_DEBUG("copied [%s], len[%d] to [%p] loc",
                        param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                        params_lte[i].len,
                        params_lte[i].buf);
        }
        else
        {
          QDP_LOG_ERROR("memory error while trying to allocate 3gpp "
                        "param for [%s]", param_strings[QDP_GET_RIL_PARAM_IDX(i)]);
        }
      } /* for each valid RIL parameter */
    } /* for each RIL parameter */

    /* profile_id_lte will be holding the default attach profile in
       Legacy fall back case , which does not have new QMI API support */
    if (QDP_INVALID_PROFILE != *profile_id_lte)
    {
      default_prof_id.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      default_prof_id.profile.profile_index = (uint8_t)(*profile_id_lte);

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &default_prof_id,
                                                   &prof_params->prof_set))
      {
        QDP_LOG_ERROR("Querying default attach profile failed with error code [%d] [%d]",
                       prof_params->prof_set.resp.result, prof_params->prof_set.resp.error);
        break;
      }
      else
      {
        prof_params->is_valid = TRUE;
        memcpy(&prof_params->prof_id, &default_prof_id.profile, sizeof(prof_params->prof_id));

        QDP_LOG_DEBUG("Profile [%d] already exists", *profile_id_lte);
        /* Update the profile */
        rc = qdp_3gpp_profile_update( params_lte,
                                      *profile_id_lte,
                                      &error_code,
                                      QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH);

        if (QDP_SUCCESS != rc)
        {
          QDP_LOG_ERROR("Could not update profile [%d] on modem with LTE information,"
                         "Error code [%d][%d]",*profile_id_lte, rc, error_code);

        }
        else
        {
          QDP_LOG_DEBUG("Profile [%d] update success", *profile_id_lte);
          ret = QDP_SUCCESS;
        }
        /* in success and failure need to break */
        break;
      }
      /* default attach end */
    }
    else
    {

      /* new implementation which has QMI support APIS
         search the all modem profiles  first*/
      *profile_id_lte = QDP_INVALID_PROFILE;
      profile_pdn_type_lte  = QDP_PROFILE_PDN_TYPE_INVALID;

      error_info->error = QDP_ERROR_NONE;
      error_info->tech = QDP_NOTECH;

      
      rc = qdp_3gpp_multi_param_search( params_lte,
                                        profile_id_lte,
                                        profile_pdp_type_lte,
                                        prof_params,
                                        &lookup_error,
                                        QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH);

      if (QDP_SUCCESS != rc)
      {
        QDP_LOG_ERROR("qdp_3gpp_multi_param_search returned err [%d]", lookup_error);
        break;
      }
      else if (lookup_error == QDP_ERROR_NONE)
      {
        lte_profile_tech_type_i = QDP_3GPP;
      }

      if (QDP_INVALID_PROFILE == *profile_id_lte)
      {
        if( QDP_ERROR_NONE != lookup_error )
        {
          QDP_LOG_DEBUG("Profile lookup error[%d], no profile created",
                        lookup_error );
          error_info->error = lookup_error;
          error_info->tech = QDP_3GPP;
          break;
        }
      }

      /*No look up error,One or more profiles found */
      if (QDP_INVALID_PROFILE != *profile_id_lte)
      {
        QDP_LOG_DEBUG("found [%d] 3gpp profile id",
                      *profile_id_lte);

        rc = qdp_3gpp_profile_update( params_lte,
                                        *profile_id_lte,
                                        &error_code,
                                        QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH);
        if (QDP_SUCCESS != rc)
        {
          QDP_LOG_ERROR("Could not update profile [%d] on modem with LTE info,"
                        "Error code [%d][%d]",*profile_id_lte, rc, error_code);
          break;
        }
      }


      /*No matching profile on modem
        Search if a QDP_profile_IA present. If not, create one*/

      else
      {

          rc = qdp_get_ia_type_profile_id(&ia_profile_id);

        if(rc == QDP_SUCCESS)
        {
          QDP_LOG_DEBUG("found [%d] IA tye profile ID", ia_profile_id);

          wds_get_profile_settings_req_msg_v01   ps_req;
          wds_get_profile_settings_resp_msg_v01  ps_resp;

          memset(&ps_req, 0, sizeof(ps_req));
          memset(&ps_resp, 0, sizeof(ps_resp));

          /* Prepare to lookup profile */
          create_profile = FALSE;
          ps_req.profile.profile_index = ia_profile_id;
          
          ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

          if (QDP_SUCCESS == (rc = qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                            &ps_req,
                                                            &ps_resp)))
          {
            lte_profile_tech_type_i = QDP_3GPP;
          }

         /* if query failed , let us create the profile */

          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_ERROR("Querying default profile failed with error code [%d][%d]",
                          ps_resp.resp.result, ps_resp.resp.error);
            create_profile = TRUE;
          }
          else
          {
            prof_params->is_valid = TRUE;
            memcpy( &prof_params->prof_id,  &ps_req.profile, sizeof(prof_params->prof_id));
            memcpy( &prof_params->prof_set, &ps_resp, sizeof(prof_params->prof_set));
          }
          /* query fail or not a QDP profile then create a new prof */
          if (TRUE == create_profile)
          {
           QDP_LOG_ERROR("%s","Querying default profile failed or it is a modem profile ");

            prof_params->is_valid = FALSE;

            /* Create the default profile */
            *profile_id_lte = QDP_INVALID_PROFILE;

            rc = qdp_3gpp_profile_create( params_lte,
                                        profile_id_lte,
                                        profile_pdp_type_lte,
                                        TRUE,
                                        QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH);

            if (QDP_SUCCESS != rc)
            {
              QDP_LOG_ERROR("%s", "Default profile creation failed!");
              break;
            }
            else if (*profile_id_lte != QDP_INVALID_PROFILE)
            {
              qdp_insert_profile_id(*profile_id_lte, QDP_3GPP,TRUE);
           }
          }
           /* query passed , input prof id is a QDP created prof id,so we can update it */
          else
          {
            QDP_LOG_DEBUG("Profile [%d] already exists", ia_profile_id);
            /* Update the profile */
            /* lte_profile_tech_type_i decided the tech type of input_prof_id from above */
            rc = qdp_3gpp_profile_update( params_lte,
                                          ia_profile_id,
                                          &error_code,
                                          QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH);

            if (QDP_SUCCESS != rc)
            {
              QDP_LOG_ERROR("Could not update profile [%d] on modem with LTE"
                            "information,Error code [%d][%d]",ia_profile_id,
                            rc, error_code);
              break;
            }
            *profile_id_lte = ia_profile_id;
          }
        }
        /*could not find a qdp_profile_ia profile. Create a new one*/
        else
        {
          QDP_LOG_DEBUG("%s", "qdp_3gpp_multi_param_search could not find a valid profile");
          rc = qdp_3gpp_profile_create( params_lte,
                                        profile_id_lte,
                                        profile_pdp_type_lte,
                                        TRUE,
                                        QDP_PROFILE_LOOKUP_FOR_SET_INITIAL_ATTACH);
          if (QDP_SUCCESS != rc)
          {
             QDP_LOG_ERROR("qdp_3gpp_profile_create failed [%d]",rc);
             break;
          }

          if (QDP_INVALID_PROFILE != *profile_id_lte)
          {
             qdp_insert_profile_id(*profile_id_lte, QDP_3GPP,TRUE);
          }
        }
      }
      ret = QDP_SUCCESS;
    }
  } while (0);

  /* clean up memory */
  qdp_free_qdp_params(params_lte, QDP_RIL_PARAM_MAX);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_lte_attach_profile_lookup EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_lte_attach_profile_lookup EXIT success");
  }

  return ret;

} /* qdp_lte_attach_profile_lookup */

/*===========================================================================
  FUNCTION:  qdp_3gpp_match_lte_attach_profile_params
===========================================================================*/
/*!
    @brief

    @params

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_3gpp_match_lte_attach_profile_params
(
  const char                             *ril_apn,
  const char                             *ril_user,
  const char                             *ril_pass,
  const char                             *ril_auth,
  const char                             *ril_ipfamily,
  const char                             *ril_ip_roaming,
  wds_get_profile_settings_resp_msg_v01  *p_params
)
{
  int ret = QDP_FAILURE;
  size_t ril_user_len = 0, ril_pass_len = 0;
  boolean qdp_created_profile = FALSE;

  QDP_LOG_DEBUG("%s","qdp_3gpp_match_lte_attach_profile_params ENTRY");

  do
  {
    if (!ril_apn || !p_params)
    {
      QDP_LOG_ERROR("%s", "NULL input parameters");
      break;
    }

    if ((p_params->profile_name_valid) &&
        (!strncasecmp(p_params->profile_name,
                     QDP_UNIQUE_PROFILE_NAME,
                     strlen(QDP_UNIQUE_PROFILE_NAME))))
    {
      qdp_created_profile = TRUE;
      QDP_LOG_DEBUG("%s","Profile is a QDP profile");
    }


    ril_user_len = (ril_user) ? strlen(ril_user) : 0;
    ril_pass_len = (ril_pass) ? strlen(ril_pass) : 0;

    /* Check APN*/
    if (!p_params->apn_name_valid)
    {
      /* Modem APN parameter not available */
      QDP_LOG_DEBUG("%s", "APN information not available, we need to do LTE attach");
      break;
    }
    else if (strncasecmp(ril_apn,
                         p_params->apn_name,
                         strlen(ril_apn)+1) != 0)
    {
      QDP_LOG_ERROR("Modem APN [%s] Attach APN [%s]",
                    p_params->apn_name,
                    ril_apn);
      break;
    }

    /* Check protocol */
    if (!p_params->pdp_type_valid)
    {
      /* Modem protocol parameter not available */
      QDP_LOG_ERROR("%s", "PDP info not available, we need to do LTE attach");
      break;
    }
    else if (ril_ipfamily)
    {
      if (qdp_allow_update_modem_profile)
      {
        /* need exact match here */
        if (qdp_pdn_type_from_string(ril_ipfamily) !=
                     qdp_pdn_type_from_3gpp_pdp(p_params->pdp_type))
        {
          QDP_LOG_DEBUG("%s", "ril and modem IP family does not match");
          break;
        }
      }
      else if (WDS_PDP_TYPE_PDP_IPV4_V01 == p_params->pdp_type)
      {
          /*We only display an error if (the IPs are of opposite type)
            OR (if IP is a superset in case of QDP profile). Reason being
            we will update IP type for QDP profile so we need to fail it here.
            However for Non-qdp profiles, we don't update IP types. So
            we can do superset, subset matching here*/
          if ((0 == strncasecmp(ril_ipfamily,
                                  QDP_RIL_IP_6,
                               strlen(QDP_RIL_IP_6)+1)) ||
               (qdp_created_profile && (0 == strncasecmp(ril_ipfamily,
                                                          QDP_RIL_IP_4_6,
                                           strlen(QDP_RIL_IP_4_6)+1))
               )
              )
        {
          QDP_LOG_ERROR("Modem IP is [%s] and ril_ipfamily is [%s]",
                        QDP_RIL_IP_4, ril_ipfamily);
          break;
        }
      }
      else if (WDS_PDP_TYPE_PDP_IPV6_V01 == p_params->pdp_type)
      {
          if ((0 == strncasecmp(ril_ipfamily,
                                 QDP_RIL_IP_4,
                                 strlen(QDP_RIL_IP_4)+1)) ||
               (qdp_created_profile && (0 == strncasecmp(ril_ipfamily,
                                                          QDP_RIL_IP_4_6,
                                           strlen(QDP_RIL_IP_4_6)+1))
               )
              )
        {
          QDP_LOG_ERROR("Modem IP is [%s] and ril_ipfamily is [%s]",
                        QDP_RIL_IP_6, ril_ipfamily);
          break;
        }
      }
      else if (WDS_PDP_TYPE_PDP_IPV4V6_V01 == p_params->pdp_type)
      {
        if ((0 != strncasecmp( ril_ipfamily,QDP_RIL_IP_4_6,
            (strlen(QDP_RIL_IP_4_6)+1) )) && (qdp_created_profile))
        {
          QDP_LOG_ERROR("Modem IP is [%s] and ril_ipfamily [%s]"
                      "Modem profile is QDP",QDP_RIL_IP_4_6, ril_ipfamily);
          break;
        }
      }
      else
      {
        QDP_LOG_ERROR("%s", "Modem IP is [%s] and ril_ipfamily is [%s]",
                      "unknown", ril_ipfamily);
        break;
      }
    }

    /* Check roaming protocol */
    if (ril_ip_roaming &&
        qdp_allow_update_modem_profile)
    {
      /* need exact match here */
      if ((!p_params->lte_roaming_pdp_type_valid) ||
          (!p_params->umts_roaming_pdp_type_valid) ||
          (p_params->lte_roaming_pdp_type != p_params->umts_roaming_pdp_type) ||
          (qdp_pdn_type_from_string(ril_ip_roaming) !=
              qdp_pdn_type_from_3gpp_roaming_pdp(p_params->lte_roaming_pdp_type)))
      {
        QDP_LOG_DEBUG("%s", "ril and modem roaming IP does not match");
        break;
      }
    }

     /* user name checking */
    if (!p_params->username_valid)
    {
      /* Modem not provided user but ril provided */
      if (ril_user_len > 0)
      {
        QDP_LOG_DEBUG("%s", "Modem user name:NULL but ril has user so do LTE attach");
        break;
      }
      /* Modem and RIL didn't provide username do nothing */
    }
    else if (ril_user_len > 0)
    {
      /* modem provided username and ril also provided so compare*/
      if (strncasecmp( ril_user,
                       p_params->username,
                       (strlen(ril_user)+1) ) != 0)
      {
        QDP_LOG_ERROR("Modem user [%s] Attach user [%s]",
                      p_params->username,
                      ril_user);
        break;
      }
    }

    /* password checking */
    if (!p_params->password_valid)
    {
      /* Modem not provided password but ril provided */
      if (ril_pass_len > 0)
      {
        QDP_LOG_DEBUG("%s", "Modem pwd name:NULL but ril has password so do LTE attach");
        break;
      }
      /* Modem and RIL didn't provide password do nothing */
    }
    else if (ril_pass_len > 0)
    {
      if (strncasecmp( ril_pass,
                       p_params->password,
                       (strlen(ril_pass)+1) ) != 0)
      {
        QDP_LOG_ERROR("Modem pwd [%s] Attach pwd [%s]",
                      p_params->password,
                      ril_pass);
        break;
      }
    }

    /* Auth-protocol */
    if (!p_params->authentication_preference_valid)
    {
      /* Modem protocol parameter not available */
      QDP_LOG_ERROR("%s", "Auth-Protocol info not available, we need to do LTE attach");
      break;
    }
    else if (ril_auth)
    {
      if (!strncmp(ril_auth,
                   QDP_RIL_PAP_CHAP_NOT_ALLOWED,
                   sizeof(QDP_RIL_PAP_CHAP_NOT_ALLOWED)))
      {
        if (p_params->authentication_preference != 0)
        {
          QDP_LOG_ERROR("RIL auth type is [%s] and Modem auth type is [%d]",
                        ril_auth, p_params->authentication_preference);
          break;
        }
      }
      else if (!strncmp(ril_auth,
                        QDP_RIL_PAP_ONLY_ALLOWED,
                        sizeof(QDP_RIL_PAP_ONLY_ALLOWED)))
      {
        if (p_params->authentication_preference != QMI_WDS_MASK_AUTH_PREF_PAP_V01)
        {
          QDP_LOG_ERROR("RIL auth type is [%s] and Modem auth type is [%d]",
                        ril_auth, p_params->authentication_preference);
          break;
        }
      }
      else if (!strncmp(ril_auth,
                        QDP_RIL_CHAP_ONLY_ALLOWED,
                        sizeof(QDP_RIL_CHAP_ONLY_ALLOWED)))
      {
        if (p_params->authentication_preference != QMI_WDS_MASK_AUTH_PREF_CHAP_V01)
        {
          QDP_LOG_ERROR("RIL auth type is [%s] and Modem auth type is [%d]",
                        ril_auth, p_params->authentication_preference);
          break;
        }
      }
      else if (!strncmp(ril_auth,
                        QDP_RIL_PAP_CHAP_BOTH_ALLOWED,
                        sizeof(QDP_RIL_PAP_CHAP_BOTH_ALLOWED)))
      {
        if (p_params->authentication_preference !=
            (QMI_WDS_MASK_AUTH_PREF_PAP_V01|QMI_WDS_MASK_AUTH_PREF_CHAP_V01))
        {
          QDP_LOG_ERROR("RIL auth type is [%s] and Modem auth type is [%d]",
                        ril_auth, p_params->authentication_preference);
          break;
        }
      }
      else
      {
        QDP_LOG_ERROR("Unknown RIL auth type [%s]",
                      ril_auth);
        break;
      }
    }

    ret = QDP_SUCCESS;
  }
  while (0);


  QDP_LOG_DEBUG("%s","qdp_3gpp_match_lte_attach_profile_params EXIT");

  return ret;
}


/*===========================================================================
  FUNCTION:  qdp_match_lte_attach_profile_params
===========================================================================*/
/*!
    @brief

    @params

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
int qdp_match_lte_attach_profile_params
(
  unsigned int  profile_id,
  const char    **param_strings
)
{
  wds_get_profile_settings_req_msg_v01   req;
  wds_get_profile_settings_resp_msg_v01  resp;
  int                                    ret = QDP_FAILURE;
  const char                             *ril_apn = NULL, *ril_user = NULL,
                                         *ril_pass = NULL, *ril_auth = NULL,
                                         *ril_ipfamily = NULL,
                                         *ril_ip_roaming = NULL;

  QDP_LOG_DEBUG("%s","qdp_match_lte_attach_profile_param ENTRY");

  QDP_INIT_BARRIER;

  do
  {
    if (!param_strings)
    {
      QDP_LOG_ERROR("%s","NULL params rcvd");
      break;
    }

    /* Get the data */
    ril_apn       = param_strings[QDP_RIL_APN];
    ril_user      = param_strings[QDP_RIL_NAI];
    ril_pass      = param_strings[QDP_RIL_PASSWORD];
    ril_auth      = param_strings[QDP_RIL_AUTH];
    ril_ipfamily  = param_strings[QDP_RIL_IP_FAMILY];
    ril_ip_roaming = param_strings[QDP_RIL_IP_ROAMING];

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    if(req.profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01)
    {
      /* Check all the parameters of the profile with the ones sent
       * from telephony. If all the parameters match then we do not
       * need to do detach and attach. If one of the parameters
       * do not match, then LTE detach and attach is required */
      if (QDP_SUCCESS != qdp_3gpp_match_lte_attach_profile_params(ril_apn,
                                                                  ril_user,
                                                                  ril_pass,
                                                                  ril_auth,
                                                                  ril_ipfamily,
                                                                  ril_ip_roaming,
                                                                  &resp))
      {
        QDP_LOG_ERROR("LTE attach param match failed for 3GPP attach profile id [%d]",
                      profile_id);
        break;
      }
      else
      {
        QDP_LOG_DEBUG("LTE attach param match succeeded for 3GPP attach profile id [%d]",
                      profile_id);
      }
    }

	ret = QDP_SUCCESS;
  }
  while (0);

  QDP_LOG_DEBUG("%s","qdp_match_lte_attach_profile_param EXIT");

  return ret;
}

int qdp_profile_read_ril_params
(
  const char  ** param_strings,
  unsigned int * profile_id_3gpp,
  qdp_param_t *params_3gpp,
  boolean *lookup_3gpp_profile,
  unsigned int * profile_id_3gpp2,
  qdp_param_t *params_3gpp2,
  boolean *lookup_3gpp2_profile
)
{
  /* based on the input RIL parameters,
   * figure out which technologies are
   * applicable and perform profile look
   * ups for it */
  int ret = QDP_FAILURE;
  int i=0, temp_len = 0;
  int tech_mask = 0;
  int temp_profile_id = 0;

  *lookup_3gpp_profile = FALSE;
  *lookup_3gpp2_profile = FALSE;

  do
  {
    /* did RIL provide a technology preference? */
    if (NULL != param_strings[QDP_RIL_TECH])
    {
      if (0 == strncmp(QDP_RIL_3GPP,
                       param_strings[QDP_RIL_TECH],
                       QDP_RIL_TECH_LEN))
      {
        QDP_LOG_DEBUG("%s", "RIL prefers 3GPP");
        *lookup_3gpp_profile = TRUE;
        ret = QDP_SUCCESS;
      }
      else if(0 == strncmp(QDP_RIL_3GPP2,
                           param_strings[QDP_RIL_TECH],
                           QDP_RIL_TECH_LEN))
      {
        QDP_LOG_DEBUG("%s", "RIL prefers 3GPP2");
        *lookup_3gpp2_profile = TRUE;
        ret = QDP_SUCCESS;
      }
      else if(0 == strncmp(QDP_RIL_AUTO,
                           param_strings[QDP_RIL_TECH],
                           QDP_RIL_TECH_LEN))
      {
        QDP_LOG_DEBUG("%s", "RIL prefers Automatic tech");
        /* we will use the RIL parameters in order to
         * determine as for which technology profile need
         * to be looked up */
        ret = QDP_SUCCESS;
      }
      else
      {
        QDP_LOG_ERROR("RIL provied invalid tech [%s]",
                        param_strings[QDP_RIL_TECH]);
        break;
      }
    }

    /* did RIL already provide a profile id? */
    if (NULL != param_strings[QDP_RIL_PROFILE_ID])
    {
      temp_profile_id = atoi(param_strings[QDP_RIL_PROFILE_ID]);
      if (temp_profile_id < QDP_RIL_DATA_PROFILE_OEM_BASE)
      {
        QDP_LOG_DEBUG("RIL did not provide a valid OEM profile [%d]",
                        temp_profile_id);
        /* we will defer to profile_look_up that happens later
         * in this function */
      }
      else
      {
        temp_profile_id = (temp_profile_id -
                           QDP_RIL_DATA_PROFILE_OEM_BASE);
        if (TRUE == *lookup_3gpp_profile)
        {
          QDP_LOG_DEBUG("RIL provided 3GPP profile id [%d]",
                          temp_profile_id);
          profile_id_3gpp[0] = temp_profile_id;
          ret = QDP_SUCCESS;
          break;
        }
        else if (TRUE == *lookup_3gpp2_profile)
        {
          QDP_LOG_DEBUG("RIL provided 3GPP2 profile id [%d]",
                          temp_profile_id);
          *profile_id_3gpp2 = temp_profile_id;
          ret = QDP_SUCCESS;
          break;
        }
        else
        {
          QDP_LOG_DEBUG("RIL provided 3GPP2, and 3GPP profile id [%d]",
                          temp_profile_id);
          profile_id_3gpp[0] = temp_profile_id;
          profile_id_3gpp2[0] = temp_profile_id;
          ret = QDP_SUCCESS;
          break;
        }
      }
    }

    /* go through each of the RIL parameter received in order to
     * determine which technology we need to look up profile for
     * prepare profile_look_up parameters */
    for(i=0; i<QDP_RIL_PARAM_MAX; i++)
    {

      if (param_strings[QDP_GET_RIL_PARAM_IDX(i)] != NULL &&
           (strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) != 0 ||
             (i == QDP_RIL_APN &&
               strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)]) == 0)
            )
         )
      {
        /* keep track of technology for all parameters together */
        tech_mask |= QDP_GET_TECH_MASK(i);

        if (QDP_GET_TECH_MASK(i) & QDP_3GPP)
        {
          /* +1 for NULL end character */
          temp_len = std_strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)])+1;
          if(temp_len > QDP_GET_3GPP_MAX_LEN(i))
          {
            QDP_LOG_ERROR("RIL param length too long [%d]",
                          temp_len);
          }
          else
          {
            QDP_MALLOC(params_3gpp[i].buf, temp_len);
          }

          if(NULL != params_3gpp[i].buf)
          {
            params_3gpp[i].len = temp_len-1;
            std_strlcpy(params_3gpp[i].buf,
                    param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                    temp_len);
            QDP_LOG_DEBUG("copied [%s], len[%d] to [%p] loc",
                          param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                          params_3gpp[i].len,
                          params_3gpp[i].buf);
          }
          else
          {
            QDP_LOG_ERROR("memory error while trying to allocate 3gpp "
                          "param for [%s]", param_strings[QDP_GET_RIL_PARAM_IDX(i)]);
          }
        } /* if 3GPP */

        if (QDP_GET_TECH_MASK(i) & QDP_3GPP2)
        {
          /* +1 for NULL end character */
          temp_len = std_strlen(param_strings[QDP_GET_RIL_PARAM_IDX(i)])+1;
          if(temp_len > QDP_GET_3GPP2_MAX_LEN(i))
          {
            QDP_LOG_ERROR("RIL param length too long [%d]",
                          temp_len);
          }
          else
          {
            QDP_MALLOC(params_3gpp2[i].buf, temp_len);
          }

          if(NULL != params_3gpp2[i].buf)
          {
            params_3gpp2[i].len = temp_len - 1;
            QDP_LOG_DEBUG("copying [%s] len [%d] to [%p] loc",
                          param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                          params_3gpp2[i].len,
                          params_3gpp2[i].buf);
            std_strlcpy(params_3gpp2[i].buf,
                    param_strings[QDP_GET_RIL_PARAM_IDX(i)],
                    temp_len);
          }
          else
          {
            QDP_LOG_ERROR("memory error while trying to allocate 3gpp2 "
                          "param for [%s]", param_strings[QDP_GET_RIL_PARAM_IDX(i)]);
          }
        } /* if 3GPP2 */

      } /* for each valid RIL parameter */

    } /* for each RIL parameter */

    /* if RIL did not prefer a technology, infer what
     * profile lookup will be required based on the
     * parameter set */
    if (FALSE == *lookup_3gpp_profile &&
        FALSE == *lookup_3gpp2_profile)
    {
        if (tech_mask & QDP_3GPP)
        {
          *lookup_3gpp_profile = TRUE;
          ret = QDP_SUCCESS;
        }
        if (tech_mask & QDP_3GPP2)
        {
          *lookup_3gpp2_profile = TRUE;
          ret = QDP_SUCCESS;
        }
    }
  }while(0);

  return ret;
}

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
    ril_tech: current technology specified at RIL API
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
)
{
  /* based on the input RIL parameters,
   * figure out which technologies are
   * applicable and perform profile look
   * ups for it */
  int ret = QDP_FAILURE, rc;
  int i=0, temp_len = 0;
  int error_code = 0;
  boolean lookup_3gpp_profile = FALSE;
  boolean lookup_3gpp2_profile = FALSE;

  int temp_profile_id = 0;
  /* profile lookup params */
  qdp_param_t params_3gpp[QDP_RIL_PARAM_MAX];
  qdp_param_t params_3gpp2[QDP_RIL_PARAM_MAX];
  qdp_error_t lookup_error_3gpp =  QDP_ERROR_NONE;
  qdp_error_t lookup_error_3gpp2 =  QDP_ERROR_NONE;
  qdp_profile_info_type *profile_info_3gpp = NULL;
  qdp_profile_info_type *profile_info_3gpp2 = NULL;

  QDP_LOG_DEBUG("%s","qdp_profile_look_up ENTRY");

  QDP_INIT_BARRIER;

  /*
  Profile lookup result will be filled as it is in either 3GPP
  or 3GPP2 or both depending upon tech type.
  */

  do
  {
    if( NULL == param_strings          ||
        NULL == profile_id_3gpp        ||
        NULL == profile_pdn_type_3gpp  ||
        NULL == profile_id_3gpp2       ||
        NULL == profile_pdn_type_3gpp2 ||
        NULL == error_info )
    {
      QDP_LOG_ERROR("%s","NULL params rcvd");
      break;
    }

    /* don't want to step on real profile ids */
    if (*profile_id_3gpp || *profile_id_3gpp2)
    {
      QDP_LOG_ERROR("%s","non-zero values rcvd");
      break;
    }

    *profile_id_3gpp = QDP_INVALID_PROFILE;
    *profile_id_3gpp2 = QDP_INVALID_PROFILE;

    *profile_pdn_type_3gpp  = QDP_PROFILE_PDN_TYPE_INVALID;
    *profile_pdn_type_3gpp2 = QDP_PROFILE_PDN_TYPE_INVALID;

    error_info->error = QDP_ERROR_NONE;
    error_info->tech = QDP_NOTECH;

    memset(&params_3gpp, 0, (sizeof(qdp_param_t) * QDP_RIL_PARAM_MAX));
    memset(&params_3gpp2, 0, (sizeof(qdp_param_t) * QDP_RIL_PARAM_MAX));

    QDP_MALLOC(profile_info_3gpp, sizeof(qdp_profile_info_type));
    QDP_MALLOC(profile_info_3gpp2, sizeof(qdp_profile_info_type));
    if (NULL == profile_info_3gpp || NULL == profile_info_3gpp2)
    {
      QDP_LOG_ERROR("%s", "Failed to allocate profile info");
      break;
    }

    if (qdp_profile_read_ril_params(param_strings,
                                    profile_id_3gpp,
                                    params_3gpp,
                                    &lookup_3gpp_profile,
                                    profile_id_3gpp2,
                                    params_3gpp2,
                                    &lookup_3gpp2_profile) != QDP_SUCCESS)
    {
      ret = QDP_FAILURE;
      QDP_LOG_ERROR("%s","qdp_profile_read_ril_params failed");
      break;
    }
    /* now we know which technology profile db needs tobe
     * looked up
     */

    /* Check for 3GPP and 3GPP2 profiles if they are requested by RIL
    */
    if (TRUE == lookup_3gpp_profile)
    {
      rc = qdp_3gpp_multi_param_search( params_3gpp,
                                        profile_id_3gpp,
                                        profile_pdn_type_3gpp,
                                        profile_info_3gpp,
                                        &lookup_error_3gpp,
                                        QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL);
      if (QDP_SUCCESS != rc)
      {
        QDP_LOG_ERROR("qdp_3gpp_multi_param_search returned err [%d]", rc);
      }
    }

    if (TRUE == lookup_3gpp2_profile)
    {
      rc = qdp_3gpp2_multi_param_search(params_3gpp2,
                                        profile_id_3gpp2,
                                        profile_pdn_type_3gpp2,
                                        profile_info_3gpp2,
                                        &lookup_error_3gpp2 );
      if (QDP_SUCCESS != rc)
      {
        QDP_LOG_ERROR("qdp_3gpp2_multi_param_search returned err [%d]", rc);
      }
    }

    if (TRUE == lookup_3gpp_profile)
    {
      if(QDP_INVALID_PROFILE == *profile_id_3gpp)
      {
        QDP_LOG_DEBUG("qdp_3gpp_multi_param_search unsuccessful,"
                      " found [%d] profile_id_3gpp", *profile_id_3gpp);

        if( QDP_ERROR_NONE == lookup_error_3gpp )
        {
          /* create a profile */
          *profile_pdn_type_3gpp  = QDP_PROFILE_PDN_TYPE_INVALID;
          rc = qdp_3gpp_profile_create(params_3gpp,
                                       profile_id_3gpp,
                                       profile_pdn_type_3gpp,
                                       FALSE,
                                       QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL);
          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_ERROR("qdp_3gpp_profile_create failed [%d]",
                          rc);
            /* do not break, we would still like to query
             * for 3gpp2 */
          }
          if (*profile_id_3gpp != QDP_INVALID_PROFILE)
          {
            /* track the profile id for later clean up */
            qdp_insert_profile_id(*profile_id_3gpp, QDP_3GPP,FALSE);
            qdp_profile_ref_up(*profile_id_3gpp);
          }
        }
        else
        {
          QDP_LOG_DEBUG("Profile lookup error[%d], no profile created",
                        lookup_error_3gpp );
          error_info->error = lookup_error_3gpp;
          error_info->tech = QDP_3GPP;
          break;
        }
      }
      else /* one or more profiles found */
      {
        QDP_LOG_DEBUG("found [%d] 3gpp profile id",
                        *profile_id_3gpp);

        /* check if we need to update modem profile */
        if (qdp_allow_update_modem_profile &&
            qdp_profile_needs_update(params_3gpp, profile_info_3gpp))
        {
          QDP_LOG_DEBUG("3GPP profile [%d] needs update", *profile_id_3gpp);
          rc = qdp_3gpp_profile_update(params_3gpp,
                                       *profile_id_3gpp,
                                       &error_code,
                                       QDP_PROFILE_LOOKUP_FOR_SETUP_DATA_CALL);
          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_DEBUG("profile [%d] update failed. rc[%d] error[%d]",
                           *profile_id_3gpp, rc, error_code);
            error_info->error = lookup_error_3gpp;
            error_info->tech = QDP_3GPP;
            break;
          }
        }

        /* check to see if this profile is one of ours */
        if (QDP_SUCCESS == qdp_profile_ref_up(*profile_id_3gpp))
        {
          QDP_LOG_DEBUG("profile id [%d] is a temp 3gpp profile",
                        *profile_id_3gpp);
        }
        else
        {
          QDP_LOG_DEBUG("profile id [%d] is an existing modem 3gpp profile",
                        *profile_id_3gpp);
        }
      }
    }

    if (TRUE == lookup_3gpp2_profile)
    {
      if (QDP_INVALID_PROFILE == *profile_id_3gpp2)
      {
        QDP_LOG_DEBUG("qdp_3gpp2_multi_param_search unsuccessful, "
                       "found [%d] profile_id_3gpp2", *profile_id_3gpp2);

        if( QDP_ERROR_NONE == lookup_error_3gpp2 )
        {
          /* create a profile */
          *profile_pdn_type_3gpp2  = QDP_PROFILE_PDN_TYPE_INVALID;
          rc = qdp_3gpp2_profile_create(params_3gpp2,
                                        profile_id_3gpp2,
                                        profile_pdn_type_3gpp2,
                                        FALSE);
          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_ERROR("qdp_3gpp2_profile_create failed [%d]",
                          rc);
            break;
          }
          if (*profile_id_3gpp2 != QDP_INVALID_PROFILE)
          {
            /* track the profile id for later clean up */
            qdp_insert_profile_id(*profile_id_3gpp2, QDP_3GPP2,FALSE);
            qdp_profile_ref_up(*profile_id_3gpp2);
          }
        }
        else
        {
          QDP_LOG_DEBUG("Profile lookup error[%d], no profile created",
                        lookup_error_3gpp2 );
          error_info->error = lookup_error_3gpp2;
          error_info->tech = QDP_3GPP2;
          break;
        }
      }
      else
      {
        QDP_LOG_DEBUG("3gpp2 profile [%d] found",
                        *profile_id_3gpp2);

        /* check if we need to update modem profile */
        if (qdp_allow_update_modem_profile &&
            qdp_profile_needs_update(params_3gpp2, profile_info_3gpp2))
        {
          QDP_LOG_DEBUG("3GPP2 profile [%d] needs update", *profile_id_3gpp2);
          rc = qdp_3gpp2_profile_update(params_3gpp2,
                                        *profile_id_3gpp2,
                                        &error_code);
          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_DEBUG("profile [%d] update failed. rc[%d] error[%d]",
                           *profile_id_3gpp2, rc, error_code);
            error_info->error = lookup_error_3gpp2;
            error_info->tech = QDP_3GPP2;
            break;
          }
        }

        /* check to see if this profile is one of ours */
        if (QDP_SUCCESS == qdp_profile_ref_up(*profile_id_3gpp2))
        {
          QDP_LOG_DEBUG("profile id [%d] is a temp 3gpp2 profile",
                        *profile_id_3gpp2);
        }
        else
        {
          QDP_LOG_DEBUG("profile id [%d] is an existing modem 3gpp2 profile",
                        *profile_id_3gpp2);
        }
      }
    }

    ret = QDP_SUCCESS;
  } while (0);

  /* clean up memory */
  qdp_free_qdp_params(params_3gpp, QDP_RIL_PARAM_MAX);
  qdp_free_qdp_params(params_3gpp2, QDP_RIL_PARAM_MAX);

  QDP_FREE(profile_info_3gpp);
  QDP_FREE(profile_info_3gpp2);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_profile_look_up EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_profile_look_up EXIT success");
  }
  return ret;
}

/*===========================================================================
    FUNCTION:  qdp_profile_look_up_by_param
===========================================================================*/
/*!
     @brief
     qdp_profile_look_up_by_param is an extension of qdp_profile_look_up
     where profile look up is performed by looking for a profile ID that
     matches a given param in the profile. Currently only APN CLASS
     matching is supported.

     @params
     param_strings: a pointer to the array of parameter string such
     that *param_strings[i] is ith parameter string
     profile_id_3gpp: placeholder for the 3gpp profile id (output)
     profile_id_3gpp2: placeholder for the 3gpp2 profile id (output)
     ril_tech: current technology specified at RIL API
     error_info: place holder for specific lookup error information

     @return
     QDP_SUCCESS
     QDP_FAILURE
*/
/*=========================================================================*/
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
)
{
  /* based on the input RIL parameters,
   * figure out which technologies are
   * applicable and perform profile look
   * ups for it */
  int ret = QDP_FAILURE, rc;
  int i=0, temp_len = 0;
  boolean lookup_3gpp_profile = FALSE;
  boolean lookup_3gpp2_profile = FALSE;

  int temp_profile_id = 0;
  /* profile lookup params */
  qdp_param_t params_3gpp[QDP_RIL_PARAM_MAX];
  qdp_param_t params_3gpp2[QDP_RIL_PARAM_MAX];
  qdp_error_t lookup_error_3gpp =  QDP_ERROR_NONE;
  qdp_error_t lookup_error_3gpp2 =  QDP_ERROR_NONE;
  int error_code;
  QDP_LOG_DEBUG("%s","qdp_profile_look_up_by_param ENTRY");

  QDP_INIT_BARRIER;

  /*
  Profile lookup result will be filled as it is in either 3GPP
  or 3GPP2 or both depending upon tech type.
  */


  do
  {
    if( NULL == param_strings          ||
        NULL == profile_id_3gpp_list   ||
        NULL == profile_pdn_type_3gpp  ||
        NULL == profile_id_3gpp2_list  ||
        NULL == profile_pdn_type_3gpp2 ||
        NULL == error_info ||
        NULL == tech_type)
    {
      QDP_LOG_ERROR("%s","NULL params rcvd");
      break;
    }

    profile_id_3gpp_list[0] = QDP_INVALID_PROFILE;
    profile_id_3gpp2_list[0] = QDP_INVALID_PROFILE;
    *profile_id_3gpp_list_len = 0;
    *profile_id_3gpp2_list_len = 0;

    *profile_pdn_type_3gpp  = QDP_PROFILE_PDN_TYPE_INVALID;
    *profile_pdn_type_3gpp2 = QDP_PROFILE_PDN_TYPE_INVALID;

    error_info->error = QDP_ERROR_NONE;
    error_info->tech = QDP_NOTECH;

    *tech_type = QDP_NOTECH;

    memset(&params_3gpp, 0, (sizeof(qdp_param_t) * QDP_RIL_PARAM_MAX));
    memset(&params_3gpp2, 0, (sizeof(qdp_param_t) * QDP_RIL_PARAM_MAX));

    if (qdp_profile_read_ril_params(param_strings,
                                    profile_id_3gpp_list,
                                    params_3gpp,
                                    &lookup_3gpp_profile,
                                    profile_id_3gpp2_list,
                                    params_3gpp2,
                                    &lookup_3gpp2_profile) != QDP_SUCCESS)
    {
      ret = QDP_FAILURE;
      QDP_LOG_ERROR("%s","qdp_profile_read_ril_params failed");
      break;
    }
    /* now we know which technology profile db needs tobe
     * looked up
     */
    if (TRUE == lookup_3gpp_profile)
    {
      rc = qdp_3gpp_alt_multi_param_search( param_to_match,
                                            param_value,
                                            profile_id_3gpp_list,
                                            profile_id_3gpp_list_len,
                                            profile_pdn_type_3gpp,
                                            &lookup_error_3gpp);
      if (QDP_SUCCESS != rc)
      {
        QDP_LOG_ERROR("qdp_3gpp2_multi_param_search returned err [%d]", rc);
      }
      else
      {
        QDP_LOG_DEBUG("qdp_3gpp_alt_multi_param_search returned profile list %d",*profile_id_3gpp_list_len);
        *tech_type = QDP_3GPP;
      }
    }

    if (TRUE == lookup_3gpp2_profile)
    {
      rc = qdp_3gpp2_alt_multi_param_search( param_to_match,
                                            param_value,
                                            profile_id_3gpp2_list,
                                            profile_id_3gpp2_list_len,
                                            profile_pdn_type_3gpp2,
                                            &lookup_error_3gpp2);
      if (QDP_SUCCESS != rc)
      {
        QDP_LOG_ERROR("qdp_3gpp2_multi_param_search returned err [%d]", rc);
      }
      else
      {
        QDP_LOG_DEBUG("qdp_3gpp2_alt_multi_param_search returned profile list %d",*profile_id_3gpp2_list_len);
      }
    }

    if (lookup_3gpp_profile == TRUE)
    {
      profile_id_3gpp_list[0] = QDP_INVALID_PROFILE;
      *profile_id_3gpp_list_len = 1;
    }

    if (lookup_3gpp2_profile == TRUE)
    {
      profile_id_3gpp_list[0] = QDP_INVALID_PROFILE;
      *profile_id_3gpp2_list_len = 1;
    }

    if (TRUE == lookup_3gpp_profile)
    {
      if(QDP_INVALID_PROFILE == profile_id_3gpp_list[0] )
      {
        QDP_LOG_DEBUG("qdp_3gpp_multi_param_search unsuccessful,"
                      " found [%d] profile_id_3gpp", profile_id_3gpp_list[0] );

        if( QDP_ERROR_NONE == lookup_error_3gpp )
        {
          /* create a profile */
          *profile_pdn_type_3gpp  = QDP_PROFILE_PDN_TYPE_INVALID;
          rc = qdp_3gpp_profile_create(params_3gpp,
                                       profile_id_3gpp_list ,
                                       profile_pdn_type_3gpp,
                                       TRUE,
                                       QDP_PROFILE_LOOKUP_FOR_SET_DATA_PROFILE);
          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_ERROR("qdp_3gpp_profile_create failed [%d]",
                          rc);
             /* do not break, we would still like to query
                       * for 3gpp2 */
          }
          else
          {
            QDP_LOG_DEBUG("Created 3GPP profile %d",profile_id_3gpp_list[0]);
            *tech_type = QDP_3GPP;
            *profile_id_3gpp_list_len = 1;
          }
        }
        else
        {
          QDP_LOG_DEBUG("Profile lookup error[%d], no profile created",
                        lookup_error_3gpp );
          error_info->error = lookup_error_3gpp;
          error_info->tech = QDP_3GPP;
          break;
        }
      }
    }

    if (TRUE == lookup_3gpp2_profile)
    {
      if(QDP_INVALID_PROFILE == profile_id_3gpp2_list[0])
      {
        QDP_LOG_DEBUG("qdp_3gpp2_multi_param_search unsuccessful,"
                      " found [%d] profile_id_3gpp2", *profile_id_3gpp2_list);

        if( QDP_ERROR_NONE == lookup_error_3gpp2 )
        {
          /* create a profile */
          *profile_pdn_type_3gpp2  = QDP_PROFILE_PDN_TYPE_INVALID;
          rc = qdp_3gpp2_profile_create(params_3gpp2,
                                       profile_id_3gpp2_list,
                                       profile_pdn_type_3gpp2,
                                       TRUE);
          if (QDP_SUCCESS != rc)
          {
            QDP_LOG_ERROR("qdp_3gpp2_profile_create failed [%d]",
                          rc);
            break;
          }
          else
          {
            QDP_LOG_DEBUG("Created 3GPP2 profile %d",profile_id_3gpp2_list[0]);
            *profile_id_3gpp2_list_len = 1;
          }
        }
        else
        {
          QDP_LOG_DEBUG("Profile lookup error[%d], no profile created",
                        lookup_error_3gpp2 );
          error_info->error = lookup_error_3gpp2;
          error_info->tech = QDP_3GPP2;
          break;
        }
      }
    }


    ret = QDP_SUCCESS;
  } while (0);

  for(i = 0; i< *profile_id_3gpp_list_len; i++)
  QDP_LOG_DEBUG("%d @ profile_id_3gpp_list[%d]",i,profile_id_3gpp_list[i]);

  /* clean up memory */
  qdp_free_qdp_params(params_3gpp, QDP_RIL_PARAM_MAX);
  qdp_free_qdp_params(params_3gpp2, QDP_RIL_PARAM_MAX);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_profile_look_up_by_param EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_profile_look_up_by_param EXIT success");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_init_profile_cleanup
===========================================================================*/
/*!
    @brief
    Cleans up temp profiles (if any) at initialization time.
    Temp profiles, when created, are created with a unique
    profile name QDP_UNIQUE_PROFILE_NAME. Thus profile_name is used as a
    search parameter to identify all previously created
    temp profiles.

    @params
    default_port: default qmi port to be used for QDP operations

    @return
    QDP_SUCCESS
    QDP_FAILURE
*/
/*=========================================================================*/
static int qdp_init_profile_cleanup(void)
{
  int ret = QDP_FAILURE, reti;
  uint32_t i;
  wds_get_profile_list_req_msg_v01   pl_req;
  wds_get_profile_list_resp_msg_v01  pl_resp;

  do
  {
    memset(&pl_req, 0, sizeof(pl_req));
    memset(&pl_resp, 0, sizeof(pl_resp));

    /* query umts profile id. */
    pl_req.profile_type_valid = TRUE;
    pl_req.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

    if (QDP_SUCCESS != qdp_qmi_wds_get_profile_list(global_qmi_wds_hndl,
                                                    &pl_req,
                                                    &pl_resp))
    {
      QDP_LOG_ERROR("couldn't get 3gpp profile list that matches [%s] name",
                    QDP_UNIQUE_PROFILE_NAME);
      break;
    }

    reti = QDP_SUCCESS;
    for (i=0; i<pl_resp.profile_list_len; i++)
    {
      wds_get_profile_settings_req_msg_v01   ps_req;
      wds_get_profile_settings_resp_msg_v01  ps_resp;

      memset(&ps_req, 0, sizeof(ps_req));
      memset(&ps_resp, 0, sizeof(ps_resp));

      ps_req.profile.profile_index = pl_resp.profile_list[i].profile_index;
      ps_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;

      if (QDP_SUCCESS != qdp_qmi_wds_query_profile(global_qmi_wds_hndl,
                                                   &ps_req,
                                                   &ps_resp))
      {
        QDP_LOG_ERROR("%s", "query_profile failed");
        reti = QDP_FAILURE;
        break;
      }
      else if (ps_resp.profile_name_valid)
      {
        if(!strncasecmp(QDP_UNIQUE_PROFILE_NAME,
                        ps_resp.profile_name,
                        std_strlen(QDP_UNIQUE_PROFILE_NAME)))
        {
          wds_delete_profile_req_msg_v01  dp_req;

          QDP_LOG_DEBUG("3gpp profile [%d] has name [%s]",
                        ps_req.profile.profile_index,
                        ps_resp.profile_name);

          memset(&dp_req, 0, sizeof(dp_req));
          dp_req.profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
          dp_req.profile.profile_index = pl_resp.profile_list[i].profile_index;

          if(QDP_SUCCESS != qdp_qmi_wds_delete_profile(global_qmi_wds_hndl,
                                                       &dp_req))
          {
            QDP_LOG_ERROR("**profile leak**: couldn't delete temp profile [%d]",
                          pl_resp.profile_list[i].profile_index);
            reti = QDP_FAILURE;
            /* don't break as we want to try all profiles if possible */
          }
        }
        else
        {
          QDP_LOG_DEBUG("3gpp profile [%d] doesn't have name [%s]",
                        pl_resp.profile_list[i].profile_index,
                        QDP_UNIQUE_PROFILE_NAME);
        }
      } /* profile_name is set on profile */

    } /* for */

    if (QDP_SUCCESS != reti)
    {
      break;
    }

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_init_profile_cleanup EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_init_profile_cleanup EXIT success");
  }

  return ret;
}


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
int qdp_init
(
  const char * default_port
)
{
  int ret = QDP_FAILURE;
  int qmi_ret, qmi_err_code;
  char args[PROPERTY_VALUE_MAX];

  QDP_LOG_DEBUG("%s","qdp_init ENTRY");

  if (TRUE == qdp_inited)
  {
    QDP_LOG_ERROR("%s","qdp already inited");
    return QDP_SUCCESS;
  }

  do
  {
    if (NULL == default_port)
    {
      QDP_LOG_ERROR("%s","qdp_init: ERROR: NULL default port rcvd");
      break;
    }

    std_strlcpy(global_qmi_port, default_port, QDP_QMI_PORT_LEN);

    if (QDP_SUCCESS != qdp_qmi_wds_init_client(global_qmi_port, &global_qmi_wds_hndl))
    {
      QDP_LOG_ERROR("%s(): WDS client init failed on port [%s]",
                    __func__,
                    global_qmi_port);
      break;
    }

    /* Check for non-persistent profile support */
    if( !qdp_modem_supports_nonpersist_flag )
    {
      /* clean up left over profiles from last time (if any) */
      /* don't care of return value here */
      qdp_init_profile_cleanup();
    }

    QDP_LOG_DEBUG("acquired global wds hndl [%p]", global_qmi_wds_hndl);

    qdp_platform_init();

    qdp_inited = TRUE;

    /* init qdp profile meta info */
    qdp_init_profile_ids();

    memset(args, 0, sizeof(args));
    property_get(QDP_ALLOW_UPDATE_MODEM_PROFILE_PROP, args, "false");
    if (0 == strcmp(args, "true"))
    {
      QDP_LOG_DEBUG("%s is true", QDP_ALLOW_UPDATE_MODEM_PROFILE_PROP);
      qdp_allow_update_modem_profile = TRUE;
    }

    qdp_inited = TRUE;

    ret = QDP_SUCCESS;
  } while (0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_init EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_init EXIT success");
  }

  return ret;
}

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
)
{
  int ret = QDP_FAILURE;
  int qmi_ret, qmi_err_code;

  QDP_LOG_DEBUG("%s","qdp_set_subscription ENTRY");

  QDP_INIT_BARRIER;

  do
  {
    wds_bind_subscription_req_msg_v01 req;

    if (QDP_QMI_CLIENT_INVALID == global_qmi_wds_hndl)
    {
      QDP_LOG_ERROR("invalid qmi_wds_hndl [%p]", global_qmi_wds_hndl);
      break;
    }

    memset(&req, 0, sizeof(req));
    req.subscription = (wds_bind_subscription_enum_v01) subs_id;
    if (QDP_SUCCESS != qdp_qmi_wds_bind_subscription ( global_qmi_wds_hndl,
                                                      &req ))
    {
      QDP_LOG_ERROR("%s", "wds bind failed");
      break;
    }

    ret = QDP_SUCCESS;
  }while(0);

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_set_subscription EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_set_subscription EXIT success");
  }

  return ret;
}


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
void qdp_deinit
(
  void
)
{
  int ret = QDP_FAILURE;

  QDP_LOG_DEBUG("%s","qdp_deinit ENTRY");

  do
  {
    if (TRUE != qdp_inited)
    {
      QDP_LOG_ERROR("%s","qdp not inited yet");
      break;
    }

    qdp_inited = FALSE;

    /* clean up profiles created by qdp module */
    qdp_cleanup_profile_ids();

    if (QDP_QMI_CLIENT_INVALID != global_qmi_wds_hndl)
    {
      QDP_LOG_DEBUG("releasing global wds hndl [%p]", global_qmi_wds_hndl);

      if (QDP_SUCCESS != qdp_qmi_wds_release_client(global_qmi_wds_hndl))
      {
        QDP_LOG_ERROR("%s", " wds srvc release failed");
        break;
      }
    }

    ret = QDP_SUCCESS;
  } while (0);

  /* Reset non-persistent profile support flag */
  qdp_modem_supports_nonpersist_flag = TRUE;

  if (QDP_SUCCESS != ret)
  {
    QDP_LOG_ERROR("%s","qdp_deinit EXIT failed");
  }
  else
  {
    QDP_LOG_DEBUG("%s","qdp_deinit EXIT success");
  }
}

/*===========================================================================
  FUNCTION:  qdp_pdn_type_from_param
===========================================================================*/
/*!
    @brief
    Convert qpd pdp param string to profile pdn type

    @params
    param:  qdp pdp param

    @return
    qdp_profile_pdn_type: qdp profile pdn type
*/
/*=========================================================================*/
static qdp_profile_pdn_type qdp_pdn_type_from_string
(
  const char *ip_string
)
{
  qdp_profile_pdn_type ret = QDP_PROFILE_PDN_TYPE_INVALID;

  do
  {
    if (NULL == ip_string)
    {
      break;
    }

    if (strcasecmp(ip_string, QDP_RIL_IP_4) == 0)
    {
      ret = QDP_PROFILE_PDN_TYPE_IPV4;
    }
    else if (strcasecmp(ip_string, QDP_RIL_IP_6) == 0)
    {
      ret = QDP_PROFILE_PDN_TYPE_IPV6;
    }
    else if (strcasecmp(ip_string, QDP_RIL_IP_4_6) == 0)
    {
      ret = QDP_PROFILE_PDN_TYPE_IPV4V6;
    }

  } while (0);

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_pdn_type_from_3gpp_pdp
===========================================================================*/
/*!
    @brief
    Convert 3gpp pdp type to profile pdn type

    @params
    pdp_3gpp: 3gpp pdp type

    @return
    qdp_profile_pdn_type: qdp profile pdn type
*/
/*=========================================================================*/
static qdp_profile_pdn_type qdp_pdn_type_from_3gpp_pdp
(
  const wds_pdp_type_enum_v01 pdp_3gpp
)
{
  qdp_profile_pdn_type ret = QDP_PROFILE_PDN_TYPE_INVALID;

  switch (pdp_3gpp)
  {
    case WDS_PDP_TYPE_PDP_IPV4_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV4;
      break;
    case WDS_PDP_TYPE_PDP_IPV6_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV6;
      break;
    case WDS_PDP_TYPE_PDP_IPV4V6_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV4V6;
      break;
    default:
      ret = QDP_PROFILE_PDN_TYPE_INVALID;
      break;
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_pdn_type_from_3gpp2_pdn
===========================================================================*/
/*!
    @brief
    Convert 3gpp2 pdn type to profile pdn type

    @params
    pdn_3gpp2: 3gpp2 pdn type

    @return
    qdp_profile_pdn_type: qdp profile pdn type
*/
/*=========================================================================*/
static qdp_profile_pdn_type qdp_pdn_type_from_3gpp2_pdn
(
  const wds_profile_pdn_type_enum_v01 pdn_3gpp2
)
{
  qdp_profile_pdn_type ret = QDP_PROFILE_PDN_TYPE_INVALID;

  switch (pdn_3gpp2)
  {
    case WDS_PROFILE_PDN_TYPE_IPV4_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV4;
      break;
    case WDS_PROFILE_PDN_TYPE_IPV6_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV6;
      break;
    case WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV4V6;
      break;
    default:
      ret = QDP_PROFILE_PDN_TYPE_INVALID;
      break;
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_pdn_type_from_3gpp_roaming_pdp
===========================================================================*/
/*!
    @brief
    Convert 3gpp roaming pdp type to profile pdn type

    @params
    roaming_pdp: 3gpp roaming pdp type

    @return
    qdp_profile_pdn_type: qdp profile pdn type
*/
/*=========================================================================*/
static qdp_profile_pdn_type qdp_pdn_type_from_3gpp_roaming_pdp
(
  const wds_common_pdp_type_enum_v01 roaming_pdp
)
{
  qdp_profile_pdn_type ret = QDP_PROFILE_PDN_TYPE_INVALID;

  switch (roaming_pdp)
  {
    case WDS_COMMON_PDP_TYPE_PDP_IPV4_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV4;
      break;
    case WDS_COMMON_PDP_TYPE_PDP_IPV6_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV6;
      break;
    case WDS_COMMON_PDP_TYPE_PDP_IPV4V6_V01:
      ret = QDP_PROFILE_PDN_TYPE_IPV4V6;
      break;
    default:
      ret = QDP_PROFILE_PDN_TYPE_INVALID;
      break;
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_profile_needs_update
===========================================================================*/
/*!
    @brief
    Check if profile IP family matches qdp parameters. If no return true
    to indicate the profile needs update.

    @params
    qdp_params: qdp parameters
    profile_info: qdp profile info

    @return
    TRUE
    FALSE
*/
/*=========================================================================*/
static boolean qdp_profile_needs_update
(
  const qdp_param_t * qdp_params,
  const qdp_profile_info_type * profile_info
)
{
  char * ip_family = NULL;
  char * ip_roaming = NULL;
  boolean ret = FALSE;

  QDP_LOG_DEBUG("%s ENTRY", __func__);

  do
  {
    if (NULL == qdp_params ||
        NULL == profile_info ||
        FALSE == profile_info->is_valid)
    {
      QDP_LOG_ERROR("Invalid input [%d]",
           (NULL == profile_info) ? 0 : profile_info->is_valid);
      break;
    }

    /* If XLAT is enabled, ignore IP types */
    if (profile_info->prof_set.clat_enabled_valid &&
        profile_info->prof_set.clat_enabled)
    {
      QDP_LOG_DEBUG("CLAT enabled in profile [%d], not to update",
                            profile_info->prof_id.profile_index);
      break;
    }

    ip_family = qdp_params[QDP_RIL_IP_FAMILY].buf;
    ip_roaming = qdp_params[QDP_RIL_IP_ROAMING].buf;

    /* if modem profile IP type is invalid or is not the same as QDP,
       the profile needs to be updated. We don't need to check other
       parameters such as username/passwd. These will be provided in
       the QMI request and will override profile */
    if (WDS_PROFILE_TYPE_3GPP_V01 == profile_info->prof_id.profile_type)
    {
      if (ip_family)
      {
        if ((!profile_info->prof_set.pdp_type_valid) ||
            (qdp_pdn_type_from_3gpp_pdp(profile_info->prof_set.pdp_type) !=
                  qdp_pdn_type_from_string(ip_family)))
        {
          QDP_LOG_DEBUG("3gpp profile [%d] pdp needs update to [%s]",
                      profile_info->prof_id.profile_index, ip_family);
          ret = TRUE;
          break;
        }
      }

      if (ip_roaming)
      {
        if ((!profile_info->prof_set.lte_roaming_pdp_type_valid) ||
            (!profile_info->prof_set.umts_roaming_pdp_type_valid) ||
            (profile_info->prof_set.lte_roaming_pdp_type !=
                      profile_info->prof_set.umts_roaming_pdp_type) ||
            (qdp_pdn_type_from_string(ip_roaming) !=
                qdp_pdn_type_from_3gpp_roaming_pdp(profile_info->prof_set.lte_roaming_pdp_type)))
        {
          QDP_LOG_DEBUG("3gpp profile [%d] roaming pdp needs update to [%s]",
                         profile_info->prof_id.profile_index, ip_roaming);
          ret = TRUE;
          break;
        }
      }
    }
    else if (WDS_PROFILE_TYPE_3GPP2_V01 == profile_info->prof_id.profile_type)
    {
      if (ip_family)
      {
        if ((!profile_info->prof_set.pdn_type_valid) ||
            (qdp_pdn_type_from_3gpp2_pdn(profile_info->prof_set.pdn_type) !=
                  qdp_pdn_type_from_string(ip_family)))
        {
          QDP_LOG_DEBUG("3gpp2 profile [%d] pdn needs to be updated to [%s]",
                      profile_info->prof_id.profile_index, ip_family);
          ret = TRUE;
          break;
        }
      }
    }
    else
    {
      QDP_LOG_ERROR("Profile type [%d] not supported",
                        profile_info->prof_id.profile_type);
      break;
    }

  } while (0);

  QDP_LOG_DEBUG("%s EXIT with [%d]", __func__, ret);

  return ret;
}

/*===========================================================================
  FUNCTION:  qdp_is_roaming_pdp_compatible
===========================================================================*/
/*!
    @brief
    Check if qdp roaming pdp type is compatible with the profile

    @params
    qdp_roaming_pdp: qdp roaming PDP string
    prof_params: profile parameters
    error: qdp error

    @return
    TRUE
    FALSE
*/
/*=========================================================================*/
static boolean qdp_is_roaming_pdp_compatible
(
  const char * qdp_roaming_pdp,
  const wds_get_profile_settings_resp_msg_v01 * prof_params,
  qdp_error_t * error
)
{
  qdp_profile_pdn_type qdp_roaming_pdn = QDP_PROFILE_PDN_TYPE_INVALID;
  qdp_profile_pdn_type profile_umts_roaming_pdn = QDP_PROFILE_PDN_TYPE_INVALID;
  qdp_profile_pdn_type profile_lte_roaming_pdn = QDP_PROFILE_PDN_TYPE_INVALID;

  if (qdp_roaming_pdp == NULL || prof_params == NULL || error == NULL)
  {
    QDP_LOG_ERROR("%s: Invalid params", __func__);
    return FALSE;
  }

  *error = QDP_ERROR_NONE;

  qdp_roaming_pdn = qdp_pdn_type_from_string(qdp_roaming_pdp);
  if (prof_params->lte_roaming_pdp_type_valid)
  {
    profile_lte_roaming_pdn =
        qdp_pdn_type_from_3gpp_roaming_pdp(prof_params->lte_roaming_pdp_type);
  }
  if (prof_params->umts_roaming_pdp_type_valid)
  {
    profile_umts_roaming_pdn =
        qdp_pdn_type_from_3gpp_roaming_pdp(prof_params->umts_roaming_pdp_type);
  }

  /* If PDP types are opposite then they are not compatible.
     If profile roaming PDP type is not set, consider it as compatible */
  if ((qdp_roaming_pdn == QDP_PROFILE_PDN_TYPE_IPV4) &&
      (profile_lte_roaming_pdn == QDP_PROFILE_PDN_TYPE_IPV6 ||
       profile_umts_roaming_pdn == QDP_PROFILE_PDN_TYPE_IPV6))
  {
    QDP_LOG_DEBUG("%s: qdp roaming IP is v4, profile has v6", __func__);
    *error = QDP_ERROR_ONLY_IPV6_ALLOWED;
    return FALSE;
  }

  if ((qdp_roaming_pdn == QDP_PROFILE_PDN_TYPE_IPV6) &&
      (profile_lte_roaming_pdn == QDP_PROFILE_PDN_TYPE_IPV4 ||
       profile_umts_roaming_pdn == QDP_PROFILE_PDN_TYPE_IPV4))
  {
    QDP_LOG_DEBUG("%s: qdp roaming IP is v6, profile has v4", __func__);
    *error = QDP_ERROR_ONLY_IPV4_ALLOWED;
    return FALSE;
  }

  return TRUE;
}

