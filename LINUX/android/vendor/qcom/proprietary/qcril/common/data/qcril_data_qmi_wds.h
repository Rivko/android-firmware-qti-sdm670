/******************************************************************************

                 Q C R I L _ D A T A _ Q M I _ W D S . H

******************************************************************************/

/******************************************************************************

  @file    qcril_data_qmi_wds.h
  @brief   QMI WDS interface file header for QCRIL Data

  DESCRIPTION
  QMI WDS interface file header for QCRIL Data

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef QCRIL_DATA_QMI_WDS_H
#define QCRIL_DATA_QMI_WDS_H

#include "qmi_client.h"
#include "wireless_data_service_v01.h"

#define QCRIL_DATA_QMI_TIMEOUT                (10000)

typedef union
{
  wds_event_report_ind_msg_v01        event_report;
  wds_extended_ip_config_ind_msg_v01  ext_ip_ind;
}qcril_data_wds_indication_data_type;

typedef struct
{
  qmi_client_type wds_hndl;
  void* user_data;
  int ind_id;
  qcril_data_wds_indication_data_type ind_data;
  void* self;
} qcril_data_wds_event_data_t;


/*===========================================================================

  FUNCTION:  qcril_data_process_wds_ind

===========================================================================*/
/*!
    @brief
    Processes qmi wds indications
    Currently, this function is responsible for processing
    physlink events - these are specific to an info_tbl entry (or a call)
    bearer tech ind - these are generic events

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_process_wds_ind
(
  qmi_client_type               wds_hndl,
  void                         *user_data,
  int                           ind_id,
  qcril_data_wds_indication_data_type *ind_data
);


/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_init_qmi_client
(
  const char                      *dev_str,
  void                            *user_data,
  wds_bind_subscription_enum_v01   subs_id,
  qmi_client_type                 *clnt_handle
);


/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@return
  int - QCRIL_DATA_SUCCESS on successful operation,
        QCRIL_DATA_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_release_qmi_client
(
  qmi_client_type          clnt_hndl
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_bind_mux_data_port
===========================================================================*/
/*!
@brief
  Bind Mux ID for a give data port instance

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_bind_mux_data_port
(
  qmi_client_type clnt_handle,
  const wds_bind_mux_data_port_req_msg_v01 *params
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to given configuration

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_bind_subscription
(
  qmi_client_type                     clnt_handle,
  wds_bind_subscription_enum_v01      subs_id
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_set_client_ip_pref
===========================================================================*/
/*!
@brief
  Set IP family preference for a client

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_set_client_ip_pref
(
  qmi_client_type         clnt_handle,
  wds_ip_family_enum_v01  ip_family
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_set_event_report
===========================================================================*/
/*!
@brief
  Set the connection state reporting conditions

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_set_event_report
(
  qmi_client_type                         clnt_handle,
  const wds_set_event_report_req_msg_v01 *params
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_indication_register
===========================================================================*/
/*!
@brief
  Register for indication messages from the modem

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_indication_register
(
  qmi_client_type clnt_handle,
  wds_indication_register_req_msg_v01   *params
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_data_system_status
===========================================================================*/
/*!
@brief
  Set the connection state reporting conditions

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_current_data_system_status
(
  qmi_client_type                   clnt_handle,
  wds_data_system_status_type_v01  *sys_status
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_pref_data_system
===========================================================================*/
/*!
@brief
  Queries the preferred data system

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_pref_data_system
(
  qmi_client_type                    clnt_handle,
  wds_curr_pref_data_sys_enum_v01   *current_sys
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_create_profile
===========================================================================*/
/*!
@brief
  Create a configured profile with specified settings

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_create_profile
(
  qmi_client_type                     clnt_handle,
  wds_create_profile_req_msg_v01     *params,
  int                                *profile_index
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_runtime_settings
===========================================================================*/
/*!
@brief
  Queries Modem for runtime settings

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_runtime_settings
(
  qmi_client_type                         clnt_handle,
  wds_req_settings_mask_v01               req_mask,
  wds_get_runtime_settings_resp_msg_v01  *runtime_set
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_remove_delegated_ipv6_prefix
===========================================================================*/
/*!
@brief
  Removes delegated IPv6 prefix

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_remove_delegated_ipv6_prefix
(
  qmi_client_type                                clnt_handle,
  const wds_remove_delegated_ipv6_prefix_req_msg_v01  *params
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_delete_profile
===========================================================================*/
/*!
@brief
  Deletes a profile

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_delete_profile
(
  qmi_client_type                      clnt_handle,
  const wds_delete_profile_req_msg_v01      *params
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_channel_rate
===========================================================================*/
/*!
@brief
  Gets the current channel rate

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_current_channel_rate
(
  qmi_client_type                                clnt_handle,
  wds_get_current_channel_rate_resp_msg_v01     *ch_rate_resp
);



/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_bearer_tech_ex
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_current_bearer_tech_ex
(
  qmi_client_type                                 clnt_handle,
  wds_get_data_bearer_technology_ex_resp_msg_v01 *bearer_tech_ex
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_bearer_tech
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - QCRIL_DATA_SUCCESS   on successful operation
        QCRIL_DATA_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_current_bearer_tech
(
  qmi_client_type                                       clnt_handle,
  wds_get_current_data_bearer_technology_resp_msg_v01  *bearer_tech
);


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_query_profile
===========================================================================*/
/*!
@brief
  Get the profile settings for the given profile id

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_query_profile
(
  qmi_client_type                        clnt_handle,
  wds_profile_identifier_type_v01       *profile_id,
  wds_get_profile_settings_resp_msg_v01 *profile_params
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_profile_list
===========================================================================*/
/*!
@brief
  Gets the list of profiles from the modem

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_profile_list
(
  qmi_client_type                clnt_handle,
  wds_profile_type_enum_v01      profile_type,
  unsigned int                  *num_profile_list_elements,
  wds_profile_info_type_v01     *profile_list
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_set_lte_attach_pdn_list
===========================================================================*/
/*!
@brief
  Sets the LTE attach PDN list

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_set_lte_attach_pdn_list
(
  qmi_client_type                           clnt_handle,
  wds_set_lte_attach_pdn_list_req_msg_v01  *pdn_list
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_lte_attach_pdn_list
===========================================================================*/
/*!
@brief
  Queries the attached PDN list

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_lte_attach_pdn_list
(
  qmi_client_type                           clnt_handle,
  wds_get_lte_attach_pdn_list_resp_msg_v01 *pdn_list
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_lte_max_attach_pdn_num
===========================================================================*/
/*!
@brief
  Queries the maximum number of attached PDNs supported

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_lte_max_attach_pdn_num
(
  qmi_client_type       clnt_handle,
  unsigned int         *max_attach_pdn_num
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_lte_attach_params
===========================================================================*/
/*!
@brief
  Queries the current attach PDN params

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message
*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_lte_attach_params
(
qmi_client_type                           clnt_handle,
wds_get_lte_attach_params_resp_msg_v01    *attach_param
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_dormant_status
===========================================================================*/
/*!
@brief
  Get the dormancy status from the modem

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_dormant_status
(
  qmi_client_type                clnt_handle,
  wds_dormancy_status_enum_v01  *dorm_status
);

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_go_dormant_req
===========================================================================*/
/*!
@brief
  Force the modem to go dormant

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_go_dormant_req
(
  qmi_client_type         clnt_handle
);

/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_release_and_init_client
===========================================================================*/
/*!
@brief
  Create a QMI client, releasing any stale handle if necessary

@return
  int - E_SUCCESS on successful operation,
        E_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_release_and_init_client
(
  const char                     *dev_str,
  void                           *user_data,
  wds_bind_subscription_enum_v01  subs_id,
  qmi_client_type                *clnt_handle
);

/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_init_qmi_client_ex
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_init_qmi_client_ex
(
  const char                      *dev_str,
  void                            *user_data,
  wds_bind_subscription_enum_v01   subs_id,
  int                              dsi_ip_ver,
  qmi_client_type                 *clnt_handle_v4,
  qmi_client_type                 *clnt_handle_v6
);
#endif /* QCRIL_DATA_QMI_WDS_H */
