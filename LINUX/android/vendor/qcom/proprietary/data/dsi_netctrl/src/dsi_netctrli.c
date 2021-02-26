/*!
  @file
  dsi_netctrli.c

  @brief
  This file implements the functions exported by dsi_netctrli.h

*/

/*===========================================================================

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/10   js      Created
===========================================================================*/
#include <stdlib.h>
#include <pthread.h>
#include "comdef.h"
#undef LOG_TAG
#ifdef DSI_NETCTRL_OFFTARGET
#include <sys/socket.h>
#else
#include <linux/socket.h>
#include "diag_lsm.h"
#endif

#ifdef FEATURE_DS_LINUX_ANDROID
#include <cutils/properties.h>
#endif

#include "ds_string.h"
#include "dsi_netctrli.h"
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl_cb_thrd.h"
#include "dsi_netctrl_mni.h"

/* indicates if dsi is initialized or not */
boolean dsi_inited = DSI_FALSE;

/* set to TRUE at dsi_release() entry
   set to FALSE at dsi_init() exit */
boolean dsi_released = DSI_FALSE;

/*Set to true when default WDS handle for a modem initialized*/
boolean is_default_handle_inited[DSI_MAX_MODEMS] = {DSI_FALSE, DSI_FALSE};

pthread_t dsi_ping_tid = 0;
pthread_mutex_t dsi_ping_tid_mutex = PTHREAD_MUTEX_INITIALIZER;

/* callback info storage required after dsi is initialized */
dsi_init_cb_info_t dsi_init_cb_info ={
  .cb_func = NULL,
  .cb_data = NULL
};

/* indicates if query to netmgr readiness has been made or not */
boolean netmgr_ready_queried = DSI_FALSE;

/* global mutex */
pthread_mutex_t dsi_global_mutex = PTHREAD_MUTEX_INITIALIZER;

/* netmgr txn mutex */
pthread_mutex_t dsi_netmgr_txn_mutex = PTHREAD_MUTEX_INITIALIZER;

/* netmgr client handle */
netmgr_client_hdl_t netmgr_hndl;
/* nas client id - goes away once we do global route look up */
int nas_clnt_id = 0;
/* stores modem network interface related functions */
dsi_mni_vtable_t dsi_mni_vtbl =
{
  /* default functions */
  .mni_init_f = dsi_mni_init,
  .mni_cleanup_f = dsi_mni_cleanup,
  .mni_start_f = dsi_mni_start,
  .mni_stop_f = dsi_mni_stop,
  .mni_abort_start_f = dsi_mni_abort_start,
  .mni_look_up_f = dsi_mni_look_up,
  .mni_reg_unreg_handoff_ind_f = dsi_mni_reg_unreg_handoff_ind,
  .mni_register_embms_ind_f = dsi_mni_register_embms_ind,
  .mni_embms_tmgi_activate_f = dsi_mni_embms_tmgi_activate,
  .mni_embms_tmgi_deactivate_f = dsi_mni_embms_tmgi_deactivate,
  .mni_embms_tmgi_act_deactivate_f = dsi_mni_embms_tmgi_activate_deactivate,
  .mni_embms_tmgi_list_query_f = dsi_mni_embms_tmgi_list_query,
  .mni_embms_tmgi_content_desc_update_f = dsi_mni_embms_tmgi_content_desc_update,
  .mni_embms_tmgi_svc_interest_list_f = dsi_mni_embms_tmgi_svc_interest_list,
};

dsi_store_tbl_t  dsi_store_table[DSI_MAX_DATA_CALLS];

dsi_modem_t dsi_modem_tbl[DSI_MAX_MODEMS];

dsi_iface_t dsi_iface_tbl[DSI_MAX_IFACES];

char * dsi_qmi_port_names[DSI_MAX_IFACES] =
{
    DSI_QMI_PORT_RMNET_0,
    DSI_QMI_PORT_RMNET_1,
    DSI_QMI_PORT_RMNET_2,
    DSI_QMI_PORT_RMNET_3,
    DSI_QMI_PORT_RMNET_4,
    DSI_QMI_PORT_RMNET_5,
    DSI_QMI_PORT_RMNET_6,
    DSI_QMI_PORT_RMNET_7,
    DSI_QMI_PORT_RMNET_8,
    DSI_QMI_PORT_RMNET_9,
    DSI_QMI_PORT_RMNET_10,
    DSI_QMI_PORT_RMNET_SDIO_0,
    DSI_QMI_PORT_RMNET_SDIO_1,
    DSI_QMI_PORT_RMNET_SDIO_2,
    DSI_QMI_PORT_RMNET_SDIO_3,
    DSI_QMI_PORT_RMNET_SDIO_4,
    DSI_QMI_PORT_RMNET_SDIO_5,
    DSI_QMI_PORT_RMNET_SDIO_6,
    DSI_QMI_PORT_RMNET_SDIO_7,
    DSI_QMI_PORT_RMNET_SDIO_0,
    DSI_QMI_PORT_RMNET_SDIO_0,
    DSI_QMI_PORT_RMNET_SDIO_0
};

char * dsi_device_names[DSI_MAX_IFACES];

/* DSI_RADIO_TECH_* is used as index in the following
   table. Since DSI_RADIO_TECH_* represents bit mask,
   it can not be 0, so following array always starts from
   1 and ends at DSI_RADIO_TECH_MAX+1 */
dsi_qmi_to_dsi_tech_t
dsi_qmi_tech_mapping_tbl[DSI_RADIO_TECH_MAX] =
{
  {DSI_RADIO_TECH_UMTS, QMI_WDS_MASK_TECH_PREF_3GPP_V01},
  {DSI_RADIO_TECH_CDMA, QMI_WDS_MASK_TECH_PREF_3GPP2_V01}
};

typedef struct dsi_map_qmi_dsi_ce_type_s
{
  wds_verbose_call_end_reason_type_enum_v01 qmi_ce_reason_type;
  dsi_ce_reason_type_t dsi_ce_reason_type;
} dsi_map_ce_reason_type_t;

dsi_map_ce_reason_type_t
dsi_qmi_ce_reason_type_mapping_tbl[] =
{
  { WDS_VCER_TYPE_UNSPECIFIED_V01,          DSI_CE_TYPE_INVALID              },
  { WDS_VCER_TYPE_MOBILE_IP_V01,            DSI_CE_TYPE_MOBILE_IP            },
  { WDS_VCER_TYPE_INTERNAL_V01,             DSI_CE_TYPE_INTERNAL             },
  { WDS_VCER_TYPE_CALL_MANAGER_DEFINED_V01, DSI_CE_TYPE_CALL_MANAGER_DEFINED },
  { WDS_VCER_TYPE_3GPP_SPEC_DEFINED_V01,    DSI_CE_TYPE_3GPP_SPEC_DEFINED    },
  { WDS_VCER_TYPE_PPP_V01 ,                 DSI_CE_TYPE_PPP                  },
  { WDS_VCER_TYPE_EHRPD_V01,                DSI_CE_TYPE_EHRPD                },
  { WDS_VCER_TYPE_IPV6_V01,                 DSI_CE_TYPE_IPV6                 }
};

/*===========================================================================
                    HELPER FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION:  set_dsi_init_state
===========================================================================*/
/*!
       @brief
       This function changes the state of the dsi_inited variable either to
       DSI_TRUE or DSI_FALSE and then executes the callback with
       data given in dsi_init_ex

       @param
       dsi_inited_val - The value to which dsi_inited has to be set

*/
/*=========================================================================*/
void set_dsi_init_state
(
  boolean dsi_inited_val
)
{
  dsi_inited = dsi_inited_val;
  if(dsi_init_cb_info.cb_func != NULL)
  {
    DSI_LOG_DEBUG("Executing Callback function %p with %p data",
                  dsi_init_cb_info.cb_func,
                  dsi_init_cb_info.cb_data);

    dsi_init_cb_info.cb_func( dsi_init_cb_info.cb_data );
  }
}

/*===========================================================================
  FUNCTION:  dsi_get_str_for_id
===========================================================================*/
/*!
    @brief
    Use the str_table structure to search for the str_id and return
    the appropriate string for the str_id

    @param
    str_table - Global table of ids and corresponding strings
    str_id    - ID to searhc in the table

    @return
    String for the str_id
*/
/*=========================================================================*/
const char* dsi_get_str_for_id
(
  dsi_id_str_map_t *str_map,
  int              str_id,
  int              map_size
)
{
  int i = 0;

  for(i=0; i < map_size; ++i)
  {
    if (str_id == str_map[i].str_id)
    {
      return str_map[i].str;
    }
  }
  return "INVALID ID";
}

/*===========================================================================
  FUNCTION:  dsi_translate_qmi_to_dsi_bearer_tech
===========================================================================*/
/*!
    @brief
    This function translates the given QMI WDS reported data bearer
    technology to DSI reported data bearer technology

    @param
    qmi_bearer_tech - QMI WDS reported bearer technology to translate

    @return
    DSI data bearer technology
*/
/*=========================================================================*/
dsi_data_bearer_tech_t
dsi_translate_qmi_to_dsi_bearer_tech
(
  wds_current_bearer_tech_type_v01 *qmi_bearer_tech
)
{
  dsi_data_bearer_tech_t  bearer_tech = DSI_DATA_BEARER_TECH_UNKNOWN;


  DSI_LOG_DEBUG( "%s", "dsi_translate_qmi_to_dsi_bearer_tech: ENTER" );

  if (!qmi_bearer_tech)
  {
    DSI_LOG_ERROR( "%s", "dsi_translate_qmi_to_dsi_bearer_tech: bad param" );
    goto bail;
  }
  else if (WDS_CURRENT_NETWORK_UNKNOWN_V01 == qmi_bearer_tech->current_nw)
  {
    DSI_LOG_ERROR( "%s", "dsi_translate_qmi_to_dsi_bearer_tech: unknown network" );
    goto bail;
  }

  if (WDS_CURRENT_NETWORK_3GPP2_V01 == qmi_bearer_tech->current_nw)
  {
    switch (qmi_bearer_tech->rat_mask)
    {
      case QMI_WDS_MASK_RAT_CDMA_1X_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_CDMA_1X;
        break;

      case QMI_WDS_MASK_RAT_EVDO_REVO_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_EVDO_REV0;
        break;

      case QMI_WDS_MASK_RAT_EVDO_REVA_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_EVDO_REVA;

        /* Determine if we need to upgrade the bearer_tech to eHRPD */
        if (QMI_WDS_MASK_SO_EVDO_REVA_EMPA_EHRPD_V01 == qmi_bearer_tech->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_EHRPD;
        }
        break;

      case QMI_WDS_MASK_RAT_EVDO_REVB_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_EVDO_REVB;

        /* Determine if we need to upgrade the bearer_tech to eHRPD */
        if (QMI_WDS_MASK_SO_EVDO_REVB_EMPA_EHRPD_V01 == qmi_bearer_tech->so_mask ||
            QMI_WDS_MASK_SO_EVDO_REVB_MMPA_EHRPD_V01 == qmi_bearer_tech->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_EHRPD;
        }
        break;

      case QMI_WDS_MASK_RAT_EHRPD_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_EHRPD;
        break;

      case QMI_WDS_MASK_RAT_FMC_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_FMC;
        break;

      default:
        DSI_LOG_ERROR( "dsi_translate_qmi_to_dsi_bearer_tech: unknown cmda_rat_mask=0x%x",
                       qmi_bearer_tech->rat_mask);
        goto bail;
    }
  }
  else if (WDS_CURRENT_NETWORK_3GPP2_V01 == qmi_bearer_tech->current_nw)
  {
    switch(qmi_bearer_tech->rat_mask)
    {
     case QMI_WDS_MASK_RAT_WCDMA_V01:
      bearer_tech = DSI_DATA_BEARER_TECH_WCDMA;
      break;

      case QMI_WDS_MASK_RAT_GPRS_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_GPRS;
        break;

      case QMI_WDS_MASK_RAT_HSDPA_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_HSDPA;
        break;

      case QMI_WDS_MASK_RAT_HSUPA_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_HSUPA;
        break;

      case QMI_WDS_MASK_RAT_EDGE_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_EDGE;
        break;

      case QMI_WDS_MASK_RAT_LTE_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_LTE;
        break;

      case QMI_WDS_MASK_RAT_HSDPA_PLUS_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_HSDPA_PLUS;
        break;

      case QMI_WDS_MASK_RAT_DC_HSDAP_PLUS_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_DC_HSDPA_PLUS;
        break;

      case QMI_WDS_MASK_RAT_64_QAM_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_64_QAM;
        break;

      case QMI_WDS_MASK_RAT_TDSCDMA_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_TDSCDMA;
        break;

      default:
        DSI_LOG_ERROR( "dsi_translate_qmi_to_dsi_bearer_tech: unknown umts_rat_mask=0x%x",
                     qmi_bearer_tech->rat_mask);
        goto bail;
    }
  }

bail:
  DSI_LOG_DEBUG( "%s", "dsi_translate_qmi_to_dsi_bearer_tech: EXIT" );
  return bearer_tech;
}

/*===========================================================================
  FUNCTION:  dsi_translate_qmi_to_dsi_bearer_tech_ex
===========================================================================*/
/*!
    @brief
    This function translates the given QMI WDS reported data bearer
    technology to DSI reported data bearer technology

    @param
    qmi_bearer_tech - QMI WDS reported bearer technology to translate

    @return
    DSI data bearer technology
*/
/*=========================================================================*/
dsi_data_bearer_tech_t
dsi_translate_qmi_to_dsi_bearer_tech_ex
(
  wds_bearer_tech_info_type_v01  *qmi_bearer_tech_ex
)
{
  dsi_data_bearer_tech_t  bearer_tech = DSI_DATA_BEARER_TECH_UNKNOWN;

  DSI_LOG_DEBUG( "%s", "dsi_translate_qmi_to_dsi_bearer_tech_ex: ENTER" );

  if (!qmi_bearer_tech_ex)
  {
    DSI_LOG_ERROR( "%s", "dsi_translate_qmi_to_dsi_bearer_tech_ex: bad param" );
    goto bail;
  }

  if (WDS_BEARER_TECH_NETWORK_3GPP_V01 == qmi_bearer_tech_ex->technology)
  {
    switch (qmi_bearer_tech_ex->rat_value)
    {
      case WDS_BEARER_TECH_RAT_EX_3GPP_WCDMA_V01:
        if (QMI_WDS_3GPP_SO_MASK_HSDPAPLUS_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_HSDPA_PLUS;
        }
        else if (QMI_WDS_3GPP_SO_MASK_DC_HSDPAPLUS_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_DC_HSDPA_PLUS;
        }
        else if (QMI_WDS_3GPP_SO_MASK_64_QAM_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_64_QAM;
        }
        else if ((QMI_WDS_3GPP_SO_MASK_HSPA_V01 & qmi_bearer_tech_ex->so_mask) ||
                 ((QMI_WDS_3GPP_SO_MASK_HSUPA_V01 & qmi_bearer_tech_ex->so_mask) &&
                  (QMI_WDS_3GPP_SO_MASK_HSDPA_V01 & qmi_bearer_tech_ex->so_mask)))
        {
          bearer_tech = DSI_DATA_BEARER_TECH_HSPA;
        }
        else if(QMI_WDS_3GPP_SO_MASK_HSUPA_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_HSUPA;
        }
        else if(QMI_WDS_3GPP_SO_MASK_HSDPA_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_HSDPA;
        }
        else
        {
          bearer_tech = DSI_DATA_BEARER_TECH_WCDMA;
        }
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP_GERAN_V01:
        if (QMI_WDS_3GPP_SO_MASK_EDGE_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_EDGE;
        }
        else if (QMI_WDS_3GPP_SO_MASK_GPRS_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_GPRS;
        }
        else if (QMI_WDS_3GPP_SO_MASK_GSM_V01 & qmi_bearer_tech_ex->so_mask)
        {
          bearer_tech = DSI_DATA_BEARER_TECH_GSM;
        }
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP_LTE_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_LTE;
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP_TDSCDMA_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_TDSCDMA;
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP_WLAN_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_3GPP_WLAN;
        break;

      default:
        DSI_LOG_ERROR( "dsi_translate_qmi_to_dsi_bearer_tech_ex: unknown 3gpp_rat_mask=0x%x",
                       qmi_bearer_tech_ex->rat_value );
        break;
    }
  }
  else if (WDS_BEARER_TECH_NETWORK_3GPP2_V01 == qmi_bearer_tech_ex->technology)
  {
    switch (qmi_bearer_tech_ex->rat_value)
    {
      case WDS_BEARER_TECH_RAT_EX_3GPP2_1X_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_CDMA_1X;
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP2_HRPD_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_HRPD;
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP2_EHRPD_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_EHRPD;
        break;

      case WDS_BEARER_TECH_RAT_EX_3GPP2_WLAN_V01:
        bearer_tech = DSI_DATA_BEARER_TECH_3GPP2_WLAN;
        break;

      default:
        DSI_LOG_ERROR( "dsi_translate_qmi_to_dsi_bearer_tech_ex: unknown 3gpp2_rat_mask=0x%x",
                       qmi_bearer_tech_ex->rat_value );
        break;
    }
  }

bail:
  DSI_LOG_DEBUG( "%s", "dsi_translate_qmi_to_dsi_bearer_tech_ex: EXIT" );
  return bearer_tech;
}

/*===========================================================================
  FUNCTION:  dsi_cleanup_store_tbl
===========================================================================*/
/*!
    @brief
    Clean up the static store table entry. parameter index is an index
    into the dsi_store_table[] array.

    @return
    none
*/
/*=========================================================================*/
void dsi_cleanup_store_tbl(short int index)
{
  dsi_store_t * st;

  DSI_LOG_DEBUG( "%s", "dsi_cleanup_store_tbl: ENTRY" );
  if (index >= 0 && index < DSI_MAX_DATA_CALLS)
  {
    if (dsi_store_table[index].dsi_store_ptr != NULL)
    {
      st = (dsi_store_t*)dsi_store_table[index].dsi_store_ptr;
      /* Check for cached event payload */
      if( NULL != st->ev_payload ) {
        free( st->ev_payload );
      }

      DSI_LOG_DEBUG( "dsi_cleanup_store_tbl: Freeing up store pointer[%p]", st );
      memset( st, 0, sizeof(dsi_store_t) );
      free( st );
    }

    dsi_store_table[index].dsi_store_ptr = NULL;
    dsi_store_table[index].is_valid = DSI_FALSE;
  }
  else
  {
    DSI_LOG_ERROR( "%s", "dsi_cleanup_store_tbl: Invalid index sent" );
  }
  DSI_LOG_DEBUG( "%s", "dsi_cleanup_store_tbl: EXIT" );
  return;
}

/*===========================================================================
  FUNCTION:  dsi_notify_users
===========================================================================*/
/*!
    @brief
    go through the store_tbl[] and notify event
    on all the calls that are using the given
    dsi interface

    dsi_id identifies the dsi interface
    event is a dsi event

    @return
    none
*/
/*=========================================================================*/
void dsi_notify_users
(
  int dsi_id,
  int event,
  dsi_ip_family_t              ipf
)
{
  int i=0;
  dsi_store_t * st;

  for(i=0; i<DSI_MAX_DATA_CALLS; i++)
  {
    if (dsi_store_table[i].is_valid)
    {
      st = (dsi_store_t *)dsi_store_table[i].dsi_store_ptr;
      if (NULL == st)
      {
        DSI_LOG_ERROR("%s","fix programming err: NULL handle pointer found" \
                      "on a store entry that is valid");
        continue;
      }

      if(st->priv.dsi_iface_id == dsi_id)
      {

        if( (ipf == DSI_NUM_IP_FAMILIES) ||
             (DSI_IP_VERSION_4_6 == st->priv.ip_version)||
             ( ((DSI_IP_VERSION_4 == st->priv.ip_version)  && (ipf == DSI_IP_FAMILY_V4)) ||
               ((DSI_IP_VERSION_6 == st->priv.ip_version)  && (ipf == DSI_IP_FAMILY_V6)) ) )
       {
          if (NULL != st->net_ev_cb)
          {
            DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                          event, st);
            st->net_ev_cb( (dsi_hndl_t *)st,
                           st->user_data,
                           event,
                           st->ev_payload );

            /* Release cached event payload */
            if( NULL != st->ev_payload ) {
              free( st->ev_payload );
              st->ev_payload = NULL;
            }
          }
          else
          {
            DSI_LOG_ERROR("NULL callback found on store pointer" \
                          "[%p]", (void *) st);
          }
        }
      }
    }
  } /* for loop */

}

/*===========================================================================
  FUNCTION:  dsi_fill_addr
===========================================================================*/
/*!
    @brief
    fill specified address after validation

    src_addr_ptr is pointer to source sockaddr_storage
    dst_addr_ptr is pointer to destination sockaddr_storage

    @return
    DSI_SUCCESS on success, DSI_ERROR otherwise
*/
/*=========================================================================*/
int dsi_fill_addr
(
  const struct sockaddr_storage * src_addr_ptr,
  struct sockaddr_storage       * dst_addr_ptr
)
{
  int ret = DSI_ERROR;

  DSI_LOG_DEBUG("%s","dsi_fill_addr: ENTRY");

  do
  {
    /* Check for NULL pointers */
    if( !(src_addr_ptr && dst_addr_ptr) )
    {
      DSI_LOG_ERROR("%s","NULL pointer specified");
      break;
    }

    /* Validate address family */
    if( !DSI_IS_ADDR_FAMILY_VALID( src_addr_ptr ) )
    {
      DSI_LOG_ERROR("we don't support ss_family [%d]", src_addr_ptr->ss_family);
      break;
    }

    *dst_addr_ptr = *src_addr_ptr;
    ret = DSI_SUCCESS;
  } while(0);

  DSI_LOG_DEBUG("%s","dsi_fill_addr: EXIT");
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_fill_addr_info
===========================================================================*/
/*!
    @brief
    go through the store_tbl[] and fill address info for all handles
    associated with the given dsi iface id

    dsi_id identifies the dsi interface
    info_ptr is pointer to netmgr_nl_event_info_t

    @return
    none
*/
/*=========================================================================*/
void dsi_fill_addr_info
(
  int                           dsi_id,
  dsi_ip_family_t               ipf,
  const netmgr_nl_event_info_t  *info
)
{
  dsi_store_t *st = NULL;
  int i;
  boolean client_found = FALSE;
  wds_technology_name_enum_v01 tech_name = WDS_TECHNOLOGY_NAME_ENUM_MAX_ENUM_VAL_V01;

  DSI_LOG_DEBUG("%s","dsi_fill_addr_info: ENTRY");

  if (!DSI_IS_ID_VALID(dsi_id))
  {
    DSI_LOG_ERROR("invalid dsi id [%d] rcvd", dsi_id);
    DSI_LOG_DEBUG("%s","dsi_fill_addr_info: EXIT");
    return;
  }

  if (!DSI_IS_IP_FAMILY_VALID(ipf))
  {
    DSI_LOG_ERROR("invalid ip family [%d] rcvd", ipf);
    DSI_LOG_DEBUG("%s","dsi_fill_addr_info: EXIT");
    return;
  }

  if (!info)
  {
    DSI_LOG_ERROR("%s","invalid nl info rcvd");
    DSI_LOG_DEBUG("%s","dsi_fill_addr_info: EXIT");
    return;
  }

  /* Check for any valid client for the iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;


    if ((st != NULL && dsi_id == st->priv.dsi_iface_id) &&
        ((st->priv.ip_version == DSI_IP_VERSION_4_6) ||
         ((st->priv.ip_version == DSI_IP_VERSION_4) && (ipf == DSI_IP_FAMILY_V4)) ||
         ((st->priv.ip_version == DSI_IP_VERSION_6) && (ipf == DSI_IP_FAMILY_V6))  )
       )
    {
      client_found = TRUE;
      break;
    }
  }
  if (TRUE == client_found)
  {
    DSI_LOG_VERBOSE("dsi_fill_addr_info: valid client=0x%p found at "
                    "store index=%d", (void *)st, i);
    tech_name = st->priv.call_tech;
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_fill_addr_info: valid client not found");
    return;
  }

  if (QDI_SUCCESS != qdi_get_addr_info(DSI_GET_QDI_HNDL(dsi_id),
                                       DSI_GET_DEV_STR(dsi_id),
                                       ((DSI_IP_FAMILY_V4 == ipf) ? AF_INET : AF_INET6),
                                       &DSI_GET_ADDRINFO(dsi_id, ipf),
                                       tech_name))
  {
    DSI_LOG_ERROR("%s","failed to get addr info from QDI");
  }

  if (info->param_mask & NETMGR_EVT_PARAM_MTU)
  {
    DSI_SET_MTU(dsi_id, info->mtu);
  }

  DSI_LOG_DEBUG("%s","dsi_fill_addr_info: EXIT");
}

/*===========================================================================
  FUNCTION:  dsi_fill_qos_info
===========================================================================*/
/*!
    @brief
    go through the store_tbl[] and fill QOS event info for all handles
    associated with the given dsi iface id

    dsi_id identifies the dsi interface
    info_ptr is pointer to netmgr_nl_event_info_t

    @return
    none
*/
/*=========================================================================*/
void dsi_fill_qos_info
(
  int dsi_id,
  const netmgr_nl_event_info_t * info_ptr
)
{
  dsi_store_t * st = NULL;
  dsi_evt_payload_t * payload = NULL;
  int i;

  DSI_LOG_DEBUG("%s","dsi_fill_qos_info: ENTRY");

  if (!DSI_IS_ID_VALID(dsi_id))
  {
    DSI_LOG_ERROR("invalid dsi id [%d] rcvd", dsi_id);
    DSI_LOG_DEBUG("%s","dsi_fill_qos_info: EXIT");
    return;
  }

  for(i=0; i<DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;
    if( st && (dsi_id == st->priv.dsi_iface_id) )
    {
      /* Allocate event payload, released in dsi_notify_users() */
      payload = malloc( sizeof(dsi_evt_payload_t) );
      if( NULL == payload ) {
        DSI_LOG_ERROR("failed to allocate event payload for dsi id[%d]", dsi_id);
        DSI_LOG_DEBUG("%s","dsi_fill_qos_info: EXIT");
        return;
      }

      /* Populate event attributes */
      memset( payload, 0x0, sizeof(dsi_evt_payload_t) );
      payload->qos_info.flow_id = info_ptr->flow_info.flow_id;
      payload->qos_info.flow_type = (dsi_qos_flow_type)info_ptr->flow_info.flow_type;
      switch( info_ptr->event )
      {
        case NET_PLATFORM_FLOW_ACTIVATED_EV:
          payload->qos_info.status_evt = DSI_QOS_ACTIVATED_EV;
          break;
        case NET_PLATFORM_FLOW_SUSPENDED_EV:
          payload->qos_info.status_evt = DSI_QOS_SUSPENDED_EV;
          break;
        case NET_PLATFORM_FLOW_DELETED_EV:
          payload->qos_info.status_evt = DSI_QOS_GONE_EV;
          break;
        case NET_PLATFORM_FLOW_MODIFIED_EV:
          payload->qos_info.status_evt = DSI_QOS_MODIFY_ACCEPTED_EV;
          break;
        default:
          DSI_LOG_ERROR("unsupported NetMgr event[%d]", info_ptr->event );
          break;
      }

      /* Cache event payload in state structure */
      st->ev_payload = payload;
      DSI_LOG_DEBUG("Cached event payload to handle [%p]", st);
    }
  }

  DSI_LOG_DEBUG("%s","dsi_fill_qos_info: EXIT");
}


/*===========================================================================
  FUNCTION:  dsi_wait_for_sig_with_timeout
===========================================================================*/
/*!
    @brief
    wait on the conditional variable with a time out or until signalled by
    a response received from netmgr for the corresponding txn

    signal_ptr dsi_signal data structure to hold the cv
    timeout_milli_secs  time for which we need to wait before returning a
                        time out error

    @return
    DSI_ERROR - in case of timeout
    DSI_SUCCESS - if receiving a response from netmgr
*/
/*=========================================================================*/
int
dsi_wait_for_sig_with_timeout
(
  dsi_signal_data_t  *signal_ptr,
  int                timeout_milli_secs
)
{
  int rc = DSI_SUCCESS;
  struct timeval curr_time;
  struct timespec wait_till_time;

  /* Get current time of day */
  gettimeofday (&curr_time,NULL);

  /* Set wait time seconds to current + the number of seconds needed for timeout */
  wait_till_time.tv_sec =  curr_time.tv_sec + (timeout_milli_secs/1000);
  wait_till_time.tv_nsec = (curr_time.tv_usec * 1000) +  ((timeout_milli_secs % 1000) * 1000 * 1000);

  /* Check the nano sec overflow */
  if (wait_till_time.tv_nsec >= NANO_SEC ) {

      wait_till_time.tv_sec +=  wait_till_time.tv_nsec/NANO_SEC;
      wait_till_time.tv_nsec %= NANO_SEC;
  }

  while((signal_ptr)->cond_predicate == FALSE)
  {
    if (pthread_cond_timedwait (&(signal_ptr)->cond_var,
                                &(signal_ptr)->cond_mutex,
                                &wait_till_time) == ETIMEDOUT)
    {
      rc = DSI_ERROR;
      break;
    }
  }

  pthread_mutex_unlock(&(signal_ptr)->cond_mutex);

  return rc;
}

/*===========================================================================
  FUNCTION:  dsi_release_dsi_iface
===========================================================================*/
/*!
    @brief
    detach the given interface from all it's associated calls
    cleans up the interface state accordingly

    i is dsi interface id
    call_end_reason is the call end reason

    @return
    none
*/
/*=========================================================================*/
void dsi_release_dsi_iface
(
  int i,
  dsi_ce_reason_t * ce_reason,
  dsi_ip_family_t ipf
)
{
  int count=0;
  dsi_store_t * st = NULL;
  dsi_ip_family_t temp_ipf;

  if (NULL == ce_reason ||
      !DSI_IS_ID_VALID(i))
  {
    DSI_LOG_ERROR("%s","**programming err** bad param rcvd");
    return;
  }

    DSI_LOG_ERROR("%s","dsi_release_dsi_iface");


  /* detach this interface from associated calls */
  for(count=0; count<DSI_MAX_DATA_CALLS; count++)
  {
    st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
    if (st != NULL && st->priv.dsi_iface_id == i)
    {

      DSI_LOG_DEBUG("ipf %d st->priv.ip_version %d",ipf, st->priv.ip_version);

      if( (ipf == DSI_NUM_IP_FAMILIES) ||
           (DSI_IP_VERSION_4_6 == st->priv.ip_version)||
             ( ((DSI_IP_VERSION_4 == st->priv.ip_version)  && (ipf == DSI_IP_FAMILY_V4)) ||
               ((DSI_IP_VERSION_6 == st->priv.ip_version)  && (ipf == DSI_IP_FAMILY_V6)) ) )
      {
        st->priv.dsi_iface_id = DSI_INVALID_IFACE;
        st->priv.companion_ip_call = FALSE;
        DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_IDLE);


        /* invalidate qdi_hndle to force new handle will be created with new call attempt */
        if (QDI_INVALID_CALL_HANDLE != st->priv.qdi_call_hndl)
        {
          qdi_release_call_handle(st->priv.qdi_call_hndl);
          st->priv.qdi_call_hndl = QDI_INVALID_CALL_HANDLE;
        }

        /* step on current call_end_reason only if we
         * know about it. This is to avoid accidental reset
         * of call end reason */
        if (ce_reason->reason_code != DSI_CE_REASON_UNKNOWN)
        {
          DSI_LOG_DEBUG("setting call end reason code/type [%d/%d] on"
                        "handle [%p]", ce_reason->reason_code,
                        ce_reason->reason_type, st);

          for (temp_ipf = DSI_IP_FAMILY_V4; temp_ipf < DSI_NUM_IP_FAMILIES; ++temp_ipf)
          {
            st->priv.ce_reason[temp_ipf].reason_code = ce_reason->reason_code;
            st->priv.ce_reason[temp_ipf].reason_type = ce_reason->reason_type;
          }
        }
      }
    }
  }

  if(DSI_NUM_IP_FAMILIES == ipf)
  {
    DSI_SET_V4_COUNT(i,0);
    DSI_SET_V6_COUNT(i,0);
  }
  else if(DSI_IP_FAMILY_V6 == ipf)
  {
    DSI_SET_V6_COUNT(i,0);
  }
  else
  {
    DSI_SET_V4_COUNT(i,0);
  }

  /* reset the interface stats */
  DSI_SET_STATE(i,DSI_IFACE_STATE_NOT_IN_USE);

  DSI_SET_MTU(i,DSI_INVALID_MTU);

  /* Clear both the family address in case we are unable to determine IP family*/
  if(ipf == DSI_NUM_IP_FAMILIES)
  {
    for (temp_ipf = DSI_IP_FAMILY_V4; temp_ipf < DSI_NUM_IP_FAMILIES; ++temp_ipf)
    {
      memset( &DSI_GET_ADDRINFO(i, temp_ipf), 0x0, sizeof(dsi_addr_info_t) );
    }
  }
  else
  {
    memset( &DSI_GET_ADDRINFO(i, ipf), 0x0, sizeof(dsi_addr_info_t) );
  }

}

/*===========================================================================
  FUNCTION: dsi_is_qmiport_incall
===========================================================================*/
/*!
    @brief
    Determine if QMI port is already incall via another process.

    Use QMI_WDS_GET_PKT_SRVC_STATUS message to test for active data
    call result.

    @return
    DSI_SUCCESS on success, DSI_ERROR otherwise
*/
/*=========================================================================*/
int dsi_is_qmiport_incall
(
  int iface,
  int *inuse_ptr,
  dsi_link_status_type *link_state_v4,
  dsi_link_status_type *link_state_v6
)
{
  int qmi_ret, qmi_err_code;
  int ret = DSI_ERROR;
  qmi_client_type wds_hndl;
  int modem  = DSI_GET_MODEM(iface);

  DSI_LOG_DEBUG( "%s", "dsi_is_qmiport_incall: entry" );

  DSI_ASSERT_H( inuse_ptr, "NULL incall pointer passed" );
  DSI_ASSERT_H( link_state_v4, "NULL incall pointer passed" );
  DSI_ASSERT_H( link_state_v6, "NULL incall pointer passed" );

  *inuse_ptr = FALSE;
  *link_state_v4 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  *link_state_v6 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;

  if(dsi_config.single_qmux_ch_enabled == DSI_TRUE)
  {
    wds_bind_mux_data_port_req_msg_v01 bind_params;

    memset(&bind_params, 0, sizeof(wds_bind_mux_data_port_req_msg_v01));

    bind_params.ep_id_valid = TRUE;
    ds_get_epid(dsi_config.phys_net_dev, (ds_ep_type_t *) &bind_params.ep_id.ep_type, (int *) &bind_params.ep_id.iface_id);

    DSI_LOG_DEBUG("Got ep_type %d, EPID %d, from %s",
        bind_params.ep_id.ep_type, bind_params.ep_id.iface_id, dsi_config.phys_net_dev);

    bind_params.mux_id_valid = TRUE;
    bind_params.mux_id = (uint8_t) iface + 1;

    if (DSI_SUCCESS != dsi_qmi_wds_bind_mux_data_port(DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem),
                                                      &bind_params))
    {
      DSI_LOG_ERROR("%s", "qmi_wds_bind_mux_data_port failed\n");
    }

    if (DSI_SUCCESS != dsi_qmi_wds_bind_subscription ( DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem),
                                                       dsi_get_modem_subs_id()))
    {
      DSI_LOG_ERROR("%s", "qmi_wds_bind_subscription failed\n");
    }

    wds_hndl = DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem);

  }
  else
  {

    if (DSI_SUCCESS != dsi_qmi_wds_init_qmi_client(DSI_GET_WDS_STR(iface), &wds_hndl))
    {
      DSI_LOG_ERROR("dsi_qmi_wds_init_qmi_client failed, iface:0x%x", iface);
      return ret;
    }

  }


  do
  {

    /* Retrieve IPv6 call statuses */
    if(DSI_SUCCESS != dsi_qmi_wds_get_pkt_srvc_status(wds_hndl,
                                                      WDS_IP_FAMILY_IPV6_V01,
                                                      link_state_v6))
    {
      DSI_LOG_ERROR("dsi_qmi_wds_get_pkt_srvc_status failed on iface [%d]", iface);
      break;
    }

    /* Retrieve IPv4 call statuses */
    if(DSI_SUCCESS != dsi_qmi_wds_get_pkt_srvc_status(wds_hndl,
                                                      WDS_IP_FAMILY_IPV4_V01,
                                                      link_state_v4))
    {
      DSI_LOG_ERROR("dsi_qmi_wds_get_pkt_srvc_status failed on iface [%d]", iface);
      break;
    }

    /* If either V4 or V6 call is up, then use a different port */
    if( WDS_CONNECTION_STATUS_DISCONNECTED_V01 != *link_state_v4 ||
        WDS_CONNECTION_STATUS_DISCONNECTED_V01 != *link_state_v6)
    {
      *inuse_ptr = TRUE;
    }

    DSI_LOG_VERBOSE( "QMI port link status for iface[%d]: "
                     "v4_state=%d, v6_state=%d, inuse=%d",
                     iface, *link_state_v4, *link_state_v6, *inuse_ptr );

    ret = DSI_SUCCESS;
  } while (0);


  if(dsi_config.single_qmux_ch_enabled == DSI_FALSE)
  {
    dsi_qmi_wds_release_qmi_client(wds_hndl);
  }

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_is_qmiport_incall: " \
                   "exit with success" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_is_qmiport_incall: " \
                   "exit with error" );
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_find_any_valid_iface_on_modem
===========================================================================*/
/*!
    @brief
    finds a valid dsi interface id given the modem id

    @return
    dsi interface id if successful, -1 other wise

    @notes
*/
/*=========================================================================*/
int dsi_find_any_valid_iface_on_modem(int modem_id)
{
  int iface_id = 0;

  for(iface_id=DSI_MODEM_GET_IFACE_START(modem_id);
      iface_id<=DSI_MODEM_GET_IFACE_END(modem_id);
      iface_id++)
  {
    if (DSI_GET_WDS_HNDL(iface_id) != DSI_INVALID_WDS_HNDL)
    {
      return iface_id;
    }
  }

  return -1;
}

/*===========================================================================
  FUNCTION:  dsi_find_available_iface_on_modem
===========================================================================*/
/*!
    @brief
    finds an available dsi interface id given the modem id

    @return
    dsi interface id if successful, -1 other wise

    @notes
*/
/*=========================================================================*/
int dsi_find_available_iface_on_modem
(
  int modem_id,
  int app_type
)
{
  int iface_id = 0;
  int port_incall = FALSE;
  dsi_link_status_type link_state_v4;
  dsi_link_status_type link_state_v6;

  if (app_type == DSI_CALL_PARAM_APP_TYPE_IMS)
  {
    /* Sometimes two processes may try to do SNI over the same iface
     * due to a race condition in querying for available ifaces. This
     * issue is currently being seen with the IMS application where
     * both IMS and RIL processes see iface 0 as the valid iface for
     * SNI. To prevent we start iface search from last iface to the
     * first iface in case of IMS */
    for(iface_id=DSI_MODEM_GET_IFACE_END(modem_id);
        iface_id>=DSI_MODEM_GET_IFACE_START(modem_id);
        iface_id--)
    {
      /* Verify QMI port is not in use by another process */
      if( !DSI_RESERVED(iface_id) &&
          DSI_SUCCESS == dsi_is_qmiport_incall( iface_id, &port_incall,
                                                 &link_state_v4,&link_state_v6) &&
          FALSE == port_incall )
      {
        return iface_id;
      }
    }
  }
  else
  {
    /* For normal cases do iface search from the first iface
     * on the given modem */
    for(iface_id=DSI_MODEM_GET_IFACE_START(modem_id);
        iface_id<=DSI_MODEM_GET_IFACE_END(modem_id);
        iface_id++)
    {
      /* Verify QMI port is not in use by another process */
      if( !DSI_RESERVED(iface_id) &&
          DSI_SUCCESS == dsi_is_qmiport_incall( iface_id, &port_incall,
                                  &link_state_v4,&link_state_v6) &&
          FALSE == port_incall )
      {
        return iface_id;
      }
    }
  }

  return -1;
}

/*===========================================================================
  FUNCTION:  dsi_find_dsi_id_by_tech
===========================================================================*/
/*!
    @brief
    finds the dsi interface id given the technology.

    @return
    dsi interface id if successful, -1 other wise

    @notes
    if unused is set to DSI_TRUE, this function would only find
    interfaces that are availalbe (not in use). Other wise
    the default interface for the corresponding technology is
    returned.

    if we get back-to-back requests for same Um iface,
    we may end up using multiple QMI ports to send qmi_wds_start..
    if a given technolgoy has only few qmi ports available, we may
    run out of these ports and may have not ports left to issue
    qmi_wds_start_nw_iface()

    The real problem is: How do we know which QMI port to use
    for the interface that receives QMI_SERVICE_ERR_NONE?
*/
/*=========================================================================*/
int dsi_find_dsi_id_by_tech
(
  int technology,
  boolean unused
)
{
  int m=0; /* modem */
  int i=0; /* interface */

  /* return default iface of default modem
     if tech is unknown */
  if (technology == DSI_RADIO_TECH_UNKNOWN)
  {
    return DSI_MODEM_GET_DEFAULT_IFACE(
      DSI_DEFAULT_MODEM);
  }

  /* find default iface of the modem that
     supports the tech provided */
  DSI_LOG_DEBUG("now finding modem that supports tech [0x%x]",
                technology);
  for(m=0; m<DSI_MAX_MODEMS; m++)
  {
    DSI_LOG_DEBUG("modem [%d] supports tech(s) [0x%x]",
                  m, DSI_MODEM_GET_TECH(m));
    if (DSI_MODEM_GET_TECH(m) & technology)
    {
      /* if don't care about whether interface
         is in-use or not, return any interface
         that has a valid wds handle */
      if(!unused)
      {
        for(i=DSI_MODEM_GET_IFACE_START(m);
            i<=DSI_MODEM_GET_IFACE_END(m);
            i++)
        {
          if (DSI_GET_WDS_HNDL(i) != DSI_INVALID_WDS_HNDL)
          {
            return i;
          }
          DSI_LOG_DEBUG("dsi id [%d] has invalid wds hndl [%p]",
                        i, DSI_GET_WDS_HNDL(i));
        }
      }

      /* find unused interface only */
      for(i=DSI_MODEM_GET_IFACE_START(m);
          i<=DSI_MODEM_GET_IFACE_END(m);
          i++)
      {
        if (!DSI_RESERVED(i) &&
            DSI_GET_WDS_HNDL(i) != DSI_INVALID_WDS_HNDL)
        {
          return i;
        }
        DSI_LOG_DEBUG("dsi id [%d] is either in-use or has " \
                      "invalid wds hndl [%p]",i,
                      DSI_GET_WDS_HNDL(i));
      }

    } /* if(modem supports the technology)*/
  } /* for(every modem) */

  return -1;
}

/*===========================================================================
  FUNCTION:  dsi_find_dsi_id_by_qmi
===========================================================================*/
/*!
    @brief
    find dsi interface for this QMI port (qmi_inst)

    dsi_iface_index is used to provided so we can match the qmi_inst
    in the context of the modem that dsi_iface_index is part of

    @return
    dsi interface id if successful, -1 other wise
*/
/*=========================================================================*/
int dsi_find_dsi_id_by_qmi
(
  unsigned char qmi_inst,
  int dsi_iface_index
)
{
  int i=0;
  int m=0;
  int count=0;
  int index=0;

  /* qmi_inst returned by QMI has an implied meaning of
     0..n where 0 is the first port mapped to a specific modem,
     accounting for target port offset */
  m = DSI_GET_MODEM(dsi_iface_index);
  index = qmi_inst + DSI_MODEM_GET_QMI_OFFSET(m);

  for(count=0,i=DSI_MODEM_GET_IFACE_START(m); i<=DSI_MODEM_GET_IFACE_END(m);
      i++,count++)
  {
    if (index == count)
    {
      return i;
    }
  }

  return -1;
}

/*===========================================================================
  FUNCTION:  dsi_map_qmi_to_dsi_tech
===========================================================================*/
/*!
    @brief
    map the QMI tech enum to dsi tech enum

    @return
    dsi technology if successful, -1 other wise
*/
/*=========================================================================*/
int dsi_map_qmi_to_dsi_tech(int qmi_tech)
{
  int i=0;

  for(i=0; i<DSI_RADIO_TECH_MAX; i++)
  {
    if (dsi_qmi_tech_mapping_tbl[i].qmi_tech == qmi_tech)
      return dsi_qmi_tech_mapping_tbl[i].dsi_tech;
  }

  return -1;
}

/*===========================================================================
  FUNCTION:  dsi_map_dsi_to_qmi_tech
===========================================================================*/
/*!
    @brief
    map the dsi tech enum to qmi tech enum

    @return
    qmi tech if successful, -1 other wise
*/
/*=========================================================================*/
int dsi_map_dsi_to_qmi_tech(int dsi_tech)
{
  int i=0;

  for(i=0; i<DSI_RADIO_TECH_MAX; i++)
  {
    if (dsi_qmi_tech_mapping_tbl[i].dsi_tech == dsi_tech)
      return dsi_qmi_tech_mapping_tbl[i].qmi_tech;
  }

  return -1;
}

/*===========================================================================
  FUNCTION:  dsi_map_qmi_to_dsi_ce_reason_type
===========================================================================*/
/*!
    @brief
    maps qmi call end reason type to dsi call end reason type

    @return
    dsi_ce_reason_type_t. Returns DSI_CE_TYPE_INVALID if unsuccessful
*/
/*=========================================================================*/
dsi_ce_reason_type_t dsi_map_qmi_to_dsi_ce_reason_type
(
  wds_verbose_call_end_reason_type_enum_v01 qmi_ce_reason_type
)
{
  unsigned int i=0;

  for(i=0; i<(sizeof(dsi_qmi_ce_reason_type_mapping_tbl)/
              sizeof(dsi_map_ce_reason_type_t)); i++)
  {
    if (dsi_qmi_ce_reason_type_mapping_tbl[i].qmi_ce_reason_type ==
        qmi_ce_reason_type)
    {
      DSI_LOG_DEBUG("found matching dsi ce reason type [%d]",
                    dsi_qmi_ce_reason_type_mapping_tbl[i].dsi_ce_reason_type);
      return dsi_qmi_ce_reason_type_mapping_tbl[i].dsi_ce_reason_type;
    }
  }

  return DSI_CE_TYPE_INVALID;
}

/*===========================================================================
  FUNCTION:  dsi_attach_dsi_iface
===========================================================================*/
/*!
    @brief
    attach the given interface with the given call (store)
    i is dsi_iface id
    st is the store handle

    @return
    none
*/
/*=========================================================================*/
DSI_INLINE void dsi_attach_dsi_iface(int i, dsi_store_t * st)
{
  if (st == NULL)
  {
    DSI_LOG_ERROR("%s","NULL st received");
    return;
  }
  st->priv.dsi_iface_id = i;
  DSI_SET_STATE(i, DSI_IFACE_STATE_IN_USE);
}

/*===========================================================================
  FUNCTION:  dsi_detach_dsi_iface
===========================================================================*/
/*!
    @brief
    detach the given interface from the given call (store)
    st is the store handle

    @return
    void
*/
/*=========================================================================*/
DSI_INLINE void dsi_detach_dsi_iface(dsi_store_t * st)
{
  if (st == NULL)
  {
    DSI_LOG_ERROR("%s","NULL st received");
    return;
  }

    DSI_LOG_ERROR("%s","dsi_detach_dsi_iface");

  /* if this was the last call store that this interface was
     attached to, then make this interface available for use*/
  if ( (( DSI_IP_VERSION_4 == st->priv.ip_version ) && (0 == DSI_GET_V4_COUNT(st->priv.dsi_iface_id)))||
       (( DSI_IP_VERSION_6 == st->priv.ip_version ) && (0 == DSI_GET_V6_COUNT(st->priv.dsi_iface_id))) ||
       (( DSI_IP_VERSION_4_6 == st->priv.ip_version ) && (0 == DSI_GET_V4_COUNT(st->priv.dsi_iface_id))
             && (0 == DSI_GET_V6_COUNT(st->priv.dsi_iface_id))) )
  {
    DSI_SET_STATE(st->priv.dsi_iface_id, DSI_IFACE_STATE_NOT_IN_USE);
  }

  /* invalidate qdi_hndle to force new handle will be created with new call attempt */

  if (QDI_INVALID_CALL_HANDLE != st->priv.qdi_call_hndl)
  {
    DSI_LOG_ERROR("%s","Releasing qdi_release_call_handle");
    qdi_release_call_handle(st->priv.qdi_call_hndl);
    st->priv.qdi_call_hndl = QDI_INVALID_CALL_HANDLE;
  }

  st->priv.dsi_iface_id = DSI_INVALID_IFACE;
  st->priv.ref_count = 0;
}

/*===========================================================================
  FUNCTION:  dsi_prepare_route_lookup_params
===========================================================================*/
/*!
    @brief
    Given a store handle, this function prepares route look up params
    The params are returned in the pparams place holder

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_prepare_route_lookup_params
(
  dsi_store_t * dsi_hndl,
  dsi_qmi_wds_route_look_up_params_type * pparams
)
{
  int ret = DSI_ERROR;

  DSI_LOG_DEBUG("%s","dsi_prepare_route_lookup_params: entry");

  do
  {
    if (!(DSI_IS_HNDL_VALID(dsi_hndl)) || NULL == pparams)
    {
      DSI_LOG_ERROR("%s","invalid params received");
      break;
    }

    /* copy only relevant parameters from start_nw_params
     * into route_lookup_params */
    memset(pparams, 0, sizeof(*pparams));

    if (dsi_hndl->priv.start_nw_params.profile_index_valid)
    {
      pparams->params_mask |=
        DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_PARAM;
      pparams->profile_index =
        dsi_hndl->priv.start_nw_params.profile_index;
      DSI_LOG_DEBUG("setting 3gpp profile id [%d] in "
                    "route_look_up params",
                    pparams->profile_index);
    }

    if (dsi_hndl->priv.start_nw_params.profile_index_3gpp2_valid)
    {
      pparams->params_mask |=
        DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_3GPP2_PARAM;
      pparams->profile_index_3gpp2 =
        dsi_hndl->priv.start_nw_params.profile_index_3gpp2;
      DSI_LOG_DEBUG("setting 3gpp2 profile id [%d] in "
                    "route_look_up params",
                    pparams->profile_index_3gpp2);
    }

    if (dsi_hndl->priv.start_nw_params.technology_preference_valid)
    {
      pparams->params_mask |=
        DSI_QMI_WDS_ROUTE_LOOKUP_TECH_PREF_PARAM;
      pparams->tech_pref =
        dsi_hndl->priv.start_nw_params.technology_preference;
      DSI_LOG_DEBUG("setting tech pref [%d] in "
                    "route_look_up params",
                    pparams->tech_pref);
    }

    if (dsi_hndl->priv.start_nw_params.ext_technology_preference_valid)
    {
      pparams->params_mask |=
        DSI_QMI_WDS_ROUTE_LOOKUP_XTENDED_TECH_PREF_PARAM;
      pparams->xtended_tech_pref =
        (wds_technology_name_enum_v01) dsi_hndl->priv.start_nw_params.ext_technology_preference;
      DSI_LOG_DEBUG("setting extended tech pref [%d] in "
                    "route_look_up params",
                    pparams->xtended_tech_pref);
    }

    if (dsi_hndl->priv.start_nw_params.ip_family_preference_valid)
    {
      pparams->params_mask |=
        DSI_QMI_WDS_ROUTE_LOOKUP_IP_FAMILY_PREF_PARAM;
      pparams->ip_family_pref =
        dsi_hndl->priv.start_nw_params.ip_family_preference;
      DSI_LOG_DEBUG("setting ip family [%d] in "
                    "route_look_up params",
                    pparams->ip_family_pref);
    }

    if (dsi_hndl->priv.start_nw_params.apn_name_valid)
    {
      pparams->params_mask |=
        DSI_QMI_WDS_ROUTE_LOOKUP_APN_NAME_PARAM;
      strlcpy(pparams->apn_name,
              dsi_hndl->priv.start_nw_params.apn_name,
              QMI_WDS_APN_NAME_MAX_V01+1);
      DSI_LOG_DEBUG("setting apn [%s] in "
                    "route_look_up params",
                    pparams->apn_name);
    }

    pparams->route_lookup = DSI_INTERFACE_LOOKUP;

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG("%s","dsi_prepare_route_lookup_params: " \
                  "exit with success");
  }
  else
  {
    DSI_LOG_ERROR("%s","dsi_prepare_route_lookup_params: " \
                  "exit with error");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_update_store_table
===========================================================================*/
/*!
    @brief
    Given an interface id, this function updates the corresponding
    store table entries.

    @return
    none
*/
/*=========================================================================*/
void dsi_update_store_table
(
  int iface,
  wds_verbose_call_end_reason_type_enum_v01 type,
  int verbose_code,
  dsi_ip_family_t ipf
)
{
  int i=0;
  dsi_store_t *st = NULL;
  boolean handle_found = FALSE;

  if (!DSI_IS_ID_VALID(i))
  {
    DSI_LOG_ERROR("%s", "** Programming Err ** invalid dsi iface");
    return;
  }

  if (!DSI_IS_IP_FAMILY_VALID(ipf))
  {
    DSI_LOG_ERROR("%s", "dsi_update_store_table: invalid ip family");
    return;
  }

  for(i=0; i<DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;
    if ((st != NULL && iface == st->priv.dsi_iface_id) &&
        ((st->priv.ip_version == DSI_IP_VERSION_4_6) ||
        ((st->priv.ip_version == DSI_IP_VERSION_4) && (ipf == DSI_IP_FAMILY_V4)) ||
        ((st->priv.ip_version == DSI_IP_VERSION_6) && (ipf == DSI_IP_FAMILY_V6))  )
       )
    {
      st->priv.ce_reason[ipf].reason_type =
        dsi_map_qmi_to_dsi_ce_reason_type(type);
      st->priv.ce_reason[ipf].reason_code = verbose_code;
      DSI_LOG_VERBOSE("data store [0x%p] updated", (unsigned int*)st);
      handle_found = TRUE;
    }
  }

  if (FALSE == handle_found)
  {
    DSI_LOG_ERROR("** FATAL ** Could not find dsi handle for iface [%d]", iface);
  }
}

/*===========================================================================
  FUNCTION:  dsi_update_call_tech
===========================================================================*/
/*!
    @brief
    This function updates the technology in the store tables with the
    technology value received from packet service indication when the
    call is connected.

    @return
    none
*/
/*=========================================================================*/
void
dsi_update_call_tech
(
  int iface,
  wds_technology_name_enum_v01 call_tech,
  int ip_version
)
{
  int i = 0;
  dsi_store_t *st = NULL;
  boolean handle_found = FALSE;

  if (!DSI_IS_ID_VALID(i))
  {
    DSI_LOG_ERROR("%s", "** Programming Err ** invalid dsi iface");
    return;
  }

  for(i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;
    if ((st != NULL && iface == st->priv.dsi_iface_id) &&
        ((st->priv.ip_version == DSI_IP_VERSION_4_6) ||
        ((st->priv.ip_version == DSI_IP_VERSION_4) && (ip_version == DSI_IP_VERSION_4)) ||
        ((st->priv.ip_version == DSI_IP_VERSION_6) && (ip_version == DSI_IP_VERSION_6))  )
       )
    {

      st->priv.call_tech = call_tech;

      DSI_LOG_VERBOSE("data store [0x%p] updated with call technology:%x",
                      (unsigned int*)st, call_tech);

      handle_found = TRUE;
    }
  }

  if (FALSE == handle_found)
  {
    DSI_LOG_ERROR("** FATAL ** Could not find dsi handle for iface [%d]", iface);
  }
}


/*===========================================================================
                                  RESET - CONFIGURATION
===========================================================================*/
/*===========================================================================
  FUNCTION:  dsi_invalidate_modems
===========================================================================*/
/*!
    @brief
    reset modem array at power up

    @return
    none
*/
/*=========================================================================*/
static DSI_INLINE void dsi_invalidate_modems(boolean from_ssr)
{
  int m=0;

  if( !from_ssr)
  {
    memset((void *)dsi_modem_tbl, 0, sizeof(dsi_modem_tbl));
  }

  for(m = 0; m<DSI_MAX_MODEMS; m++)
  {
    DSI_MODEM_SET_TECH(m, DSI_RADIO_TECH_UNKNOWN);
    DSI_MODEM_SET_IFACE_START(m, DSI_INVALID_IFACE);
    DSI_MODEM_SET_IFACE_END(m, DSI_INVALID_IFACE);
    DSI_MODEM_SET_DEFAULT_IFACE(m, DSI_INVALID_IFACE);
    DSI_MODEM_SET_QMI_OFFSET(m, 0);
    if(is_default_handle_inited[m] == DSI_FALSE)
    {
      DSI_MODEM_SET_DEFAULT_WDS_HNDL(m,DSI_INVALID_WDS_HNDL);
    }
  }
}

/*===========================================================================
  FUNCTION:  dsi_invalidate_iface
===========================================================================*/
/*!
    @brief
    invalidates the dsi interface

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static DSI_INLINE void dsi_invalidate_iface(int dsi_iface)
{
    dsi_ip_family_t ipf;

    DSI_SET_STATE(dsi_iface,DSI_IFACE_STATE_NOT_IN_USE);
    DSI_SET_WDS_HNDL(dsi_iface,DSI_INVALID_WDS_HNDL);
    DSI_SET_QOS_HNDL(dsi_iface,DSI_INVALID_QOS_HNDL);
    DSI_SET_V4_COUNT(dsi_iface,0);
    DSI_SET_V6_COUNT(dsi_iface,0);
    DSI_SET_MTU(dsi_iface,DSI_INVALID_MTU);

    for (ipf = DSI_IP_FAMILY_V4; ipf < DSI_NUM_IP_FAMILIES; ++ipf)
    {
      DSI_SET_INVALID_ADDR(dsi_iface, ipf, iface_addr);

      /* Clear cached address info */
      memset( &DSI_GET_ADDRINFO(dsi_iface, ipf), 0x0, sizeof(dsi_addr_info_t) );
    }
}


/*===========================================================================
  FUNCTION:  dsi_call_cleanup
===========================================================================*/
/*!
    @brief
    Performs call clean up for a given iface
    - notify users of NET_NO_NET on each call entry
    - clean up call entries

    @return
    None
*/
/*=========================================================================*/
static DSI_INLINE void dsi_call_cleanup(int conn_id)
{
  short int i = 0;
  dsi_store_t * st_ptr = NULL;

  if (conn_id >= DSI_MAX_IFACES)
  {
    DSI_LOG_ERROR("conn_id [%d] not recognized",
                  conn_id);
    return;
  }

  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st_ptr = dsi_store_table[i].dsi_store_ptr;

    if (dsi_store_table[i].is_valid == TRUE &&
        st_ptr != NULL &&
        st_ptr->priv.dsi_iface_id != DSI_INVALID_IFACE &&
        st_ptr->priv.dsi_iface_id == conn_id)
    {
      if (NULL != st_ptr->net_ev_cb )
      {
        st_ptr->net_ev_cb((dsi_hndl_t *)st_ptr,
                          st_ptr->user_data,
                          DSI_EVT_NET_NO_NET,
                          NULL);
        DSI_LOG_DEBUG("%s(): DSI_EVT_NET_NO_NET notified on call [%d]", __func__, i);
      }
      /* clear store table (call) entry */
      dsi_cleanup_store_tbl(i);
    }
  }

}

/*===========================================================================
  FUNCTION:  dsi_modem_out_of_service_hdlr
===========================================================================*/
/*!
    @brief
    Handles system event indicating modem is out of service.
    - kills callback thread
    - invalidate dsi interfaces
    - notify users of NET_NO_NET on each call entry
    - clean up call entries

    @return
    None
*/
/*=========================================================================*/
void dsi_modem_out_of_service_hdlr(int conn_id)
{
  short int i=0;
  int modem;
  int qmi_err = QMI_SERVICE_ERR_NONE;

  /* this is where we handle modem restart */
  DSI_LOG_DEBUG("%s", "dsi_modem_out_of_service_hdlr ENTRY");

  DSI_GLOBAL_LOCK;

  /* For iwlan we enumerate reverse rmnet ports which are not known to
   * the DSI library. We will skip the out_of_service processing for
   * these links */
  if (conn_id >= DSI_MAX_IFACES)
  {
    DSI_LOG_ERROR("conn_id [%d] not recognized",
                  conn_id);
    DSI_GLOBAL_UNLOCK;
    return;
  }

  /* release QMI client */
  dsi_mni_release_client(conn_id);
  dsi_invalidate_iface(conn_id);
  DSI_LOG_DEBUG("iface [%d] invalidated", conn_id);

  /* Cleanup call entries */
  dsi_call_cleanup(conn_id);

  modem = DSI_GET_MODEM(conn_id);

  if(DSI_MODEM_GET_DEFAULT_IFACE(modem) == conn_id)
  {
    is_default_handle_inited[modem] = DSI_FALSE;
    if(DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem) != DSI_INVALID_WDS_HNDL)
    {
      dsi_qmi_wds_release_qmi_client(DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem));
      DSI_MODEM_SET_DEFAULT_WDS_HNDL(modem, DSI_INVALID_WDS_HNDL);
    }
  }

  DSI_GLOBAL_UNLOCK;

  DSI_LOG_DEBUG("%s", "dsi_modem_out_of_service_hdlr EXIT");
}

/*===========================================================================
  FUNCTION:  dsi_modem_in_service_hdlr
===========================================================================*/
/*!
    @brief
    Handles system event indicating modem is back in service.
    - starts callback thread

    @return
    None
*/
/*=========================================================================*/
void dsi_modem_in_service_hdlr(int conn_id)
{
  /* this is where we re-init dsi/ qmi */
  int qmi_err_code;
  qmi_client_type wds_hndl;
  int modem;
  DSI_LOG_DEBUG("%s", "dsi_modem_in_service_hdlr ENTRY");

  DSI_GLOBAL_LOCK;

  /* See above description */
  if (conn_id >= DSI_MAX_IFACES)
  {
    DSI_LOG_ERROR("conn_id [%d] not recognized",
                  conn_id);
    DSI_GLOBAL_UNLOCK;
    return;
  }

  /* init QMI clients for non-single QMUX targets */
  if(dsi_config.single_qmux_ch_enabled == FALSE)
  {
    dsi_mni_init_client(conn_id);
  }

  modem = DSI_GET_MODEM(conn_id);
  if(DSI_MODEM_GET_DEFAULT_IFACE(modem) == conn_id)
  {
    if(DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem) == DSI_INVALID_WDS_HNDL)
    {
      if(is_default_handle_inited[modem] == DSI_FALSE)
      {
        if(DSI_SUCCESS == dsi_qmi_wds_init_qmi_client(DSI_GET_WDS_STR(conn_id),
                                               &wds_hndl))
        {
          DSI_MODEM_SET_DEFAULT_WDS_HNDL(modem, wds_hndl);
          is_default_handle_inited[modem] = DSI_TRUE;
          DSI_LOG_DEBUG( "dsi_qmi_wds_init_qmi_client for %d modem: %p wds_hndl",modem, DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem));
        }
        else
        {
          DSI_LOG_ERROR("Could not allocate default handle conn_id = %d",conn_id);
        }
      }
      else
      {
        DSI_LOG_DEBUG( "dsi_qmi_wds_init_qmi_client for %d modem: %p wds_hndl",modem, DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem));
      }
    }
  }

  DSI_LOG_DEBUG("%s", "let go of global lock");
  DSI_GLOBAL_UNLOCK;

  DSI_LOG_DEBUG("%s", "dsi_modem_in_service_hdlr EXIT");
}

/*===========================================================================
                            INIT - CONFIGURATION
===========================================================================*/

/*===========================================================================
  FUNCTION  dsi_install_device_names
===========================================================================*/
/*!
@brief
  Installs the device names in dsi_device_names table with the given prefix

@return
  DSI_SUCCESS on success, DSI_ERROR otherwise

*/
/*=========================================================================*/
static int dsi_install_device_names
(
  const char    *prefix,
  unsigned int  start,
  unsigned int  end
)
{
  unsigned int i;
  int rc = DSI_ERROR, ret;
  unsigned int iface;

  if (end >= DSI_NUM_ENTRIES(dsi_device_names))
  {
    goto bail;
  }

  for (i = start, iface = 0; i <= end; ++i, ++iface)
  {
    if (NULL != dsi_device_names[i])
    {
      free(dsi_device_names[i]);
      dsi_device_names[i] = NULL;
    }

    if (DSI_DEV_RMNET_INVALID_PREFIX != prefix)
    {
      dsi_device_names[i] = (char *)malloc(DSI_DEV_STR_MAX_LEN * sizeof(char));

      if (NULL == dsi_device_names[i])
      {
        goto bail;
      }

      ret = snprintf(dsi_device_names[i], DSI_DEV_STR_MAX_LEN, "%s%d", prefix, iface);

      if (ret >= DSI_DEV_STR_MAX_LEN)
      {
        goto bail;
      }
    }
  }

  rc = DSI_SUCCESS;

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  dsi_cleanup_device_names
===========================================================================*/
/*!
@brief
  Clean up the names in teh dsi_device_names tables

@return
  void

*/
/*=========================================================================*/
static void dsi_cleanup_device_names(void)
{
  unsigned int i;

  for (i = 0; i < DSI_NUM_ENTRIES(dsi_device_names); ++i)
  {
    if (NULL != dsi_device_names[i])
    {
      free(dsi_device_names[i]);
    }
    dsi_device_names[i] = NULL;
  }
}

/*===========================================================================
  FUNCTION  dsi_process_target
===========================================================================*/
/*!
@brief
  Updates DSI data strutures based on current target configuration

@return
  DSI_SUCCESS on success, DSI_ERROR otherwise

*/
/*=========================================================================*/
static int dsi_process_target()
{
  int ret = DSI_SUCCESS;
  ds_target_t target;
  const char *target_str;

  target = ds_get_target();
  target_str = ds_get_target_str(target);

  DSI_LOG_DEBUG("dsi_process_target(): [%d] : [%s]", target, target_str);


  dsi_config.modem_ssr_state = -1;
  ret = dsi_config_load(DSI_CONFIG_FILE_PATH, target_str);

  if (DSI_SUCCESS != ret)
  {
    DSI_LOG_ERROR("%s: unable to load config for target:%s",
                   __func__, target_str);
    dsi_cleanup_device_names();
  } else {
    DSI_LOG_DEBUG("%s: loaded config for target:%s", __func__, target_str);
  }

  return ret;
}

#ifdef FEATURE_DS_LINUX_ANDROID
/*===========================================================================
  FUNCTION  dsi_configure_qos
===========================================================================*/
/*!
@brief
  Configures rmnet_data support based on android property value

@return
  void

*/
/*=========================================================================*/
static int dsi_configure_qos()
{
  char args[PROPERTY_VALUE_MAX];
  int ret = DSI_ERROR;

  do {
    memset(args, 0, sizeof(args));

    /* retrieve value of DSI_PROPERTY_QOS */
    ret = property_get(DSI_PROPERTY_QOS, args, DSI_QOS_VALUE_ENABLE);

    /* Update the QOS state */
    dsi_config.qos_enable = (0==strcmp(DSI_QOS_VALUE_ENABLE, args))? DSI_TRUE : DSI_FALSE;

    DSI_LOG_INFO("QOS property is set to [%s] enable [%d]", args, dsi_config.qos_enable);
    ret = DSI_SUCCESS;
  }
  while (0);

  return ret;
}

/*===========================================================================
  FUNCTION  dsi_configure_rmnet_data
===========================================================================*/
/*!
@brief
  Configures QOS support based on QOS enabled property value

@return
  void

*/
/*=========================================================================*/
static int dsi_configure_rmnet_data()
{
  char args[PROPERTY_VALUE_MAX];
  char def[DSI_PROPERTY_RMNET_DATA_SIZE+1];
  int ret = DSI_ERROR;

  do {
    memset(def, 0, sizeof(def));
    memset(args, 0, sizeof(args));

    /* retrieve value of DSI_PROPERTY_RMNET_DATA */
    ret = property_get(DSI_PROPERTY_RMNET_DATA, args, def);

    /* Update the rmnet data state */
    dsi_config.rmnet_data_enable = (0==strcmp(DSI_RMNET_DATA_ENABLE_VALUE,
                                       args))? DSI_TRUE : DSI_FALSE;

    DSI_LOG_INFO("Rnet data is set to [%s] enable [%d]", args, dsi_config.rmnet_data_enable);
    ret = DSI_SUCCESS;
  }
  while (0);

  return ret;
}
#endif

/*===========================================================================
  FUNCTION:  dsi_netctrl_join_ping_thread
===========================================================================*/
/*!
    @brief
    Call a join on the init_ping_thread if the thread id is valid. This
    ensures that the thread's resources are released.

    @param
    None

    @return
    None
*/
/*=========================================================================*/
void dsi_netctrl_join_ping_thread()
{
  DSI_LOCK_MUTEX(&dsi_ping_tid_mutex);
  if ( dsi_ping_tid != 0)
  {
    DSI_LOG_DEBUG("%s: calling join on ping thread tid[%lu]",
                   __func__, (long unsigned int)dsi_ping_tid);
    pthread_join(dsi_ping_tid, NULL);
    dsi_ping_tid = 0;
  }
  DSI_UNLOCK_MUTEX(&dsi_ping_tid_mutex);
}

/*===========================================================================
  FUNCTION:  dsi_init_ifaces
===========================================================================*/
/*!
    @brief
    reset dsi interface array at power up

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static DSI_INLINE int dsi_init_ifaces()
{
  int i=0, ret;

  /* Assume QOS feature disabled by default */
  dsi_config.qos_enable = DSI_FALSE;
  /* Assume rmnet data to be disabled by default */
  dsi_config.rmnet_data_enable = DSI_FALSE;

  /* Install the default device names */
  if (DSI_SUCCESS != dsi_install_device_names(DSI_DEV_RMNET_SMD_PREFIX,
                                              DSI_LOCAL_MODEM_IFACE_START,
                                              DSI_LOCAL_MODEM_IFACE_END))
  {
    DSI_LOG_ERROR("%s", "error on device name installation (local modem)");
    return DSI_ERROR;
  }

  if (DSI_SUCCESS != dsi_install_device_names(DSI_DEV_RMNET_SDIO_PREFIX,
                                              DSI_REMOTE_MODEM_IFACE_START,
                                              DSI_REMOTE_MODEM_IFACE_END))
  {
    DSI_LOG_ERROR("%s", "error on device name installation (remote modem)");
    return DSI_ERROR;
  }

#ifdef FEATURE_DS_LINUX_ANDROID

  /* Legacy port configuration code for older targets */
  /* Query the Android propoerty to check availability of rmnet data driver */
  if( DSI_ERROR == dsi_configure_rmnet_data())
  {
    DSI_LOG_ERROR("%s", "error on QOS property query");
    return DSI_ERROR;
  }

  /* Query the Android propoerty to determine platform QOS status */
  if( DSI_ERROR == dsi_configure_qos())
  {
    DSI_LOG_ERROR("%s", "error on QOS property query");
    return DSI_ERROR;
  }

#endif

#ifdef FEATURE_DATA_MDM_SINGLE_QMUX_CHANNEL
  dsi_config.rmnet_data_enable = DSI_TRUE;
  dsi_config.qos_enable = DSI_FALSE;
  strlcpy(dsi_config.phys_net_dev, "rmnet_ipa0", sizeof(dsi_config.phys_net_dev));
  dsi_config.single_qmux_ch_enabled = DSI_TRUE;
  strlcpy(dsi_config.single_qmux_ch_name, DSI_QMI_PORT_RMNET_0,
          sizeof(dsi_config.single_qmux_ch_name));

  dsi_qmi_port_names[0]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[1]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[2]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[3]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[4]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[5]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[6]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[7]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[8]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[9]  = DSI_QMI_PORT_RMNET_0;
  dsi_qmi_port_names[10] = DSI_QMI_PORT_RMNET_0;

  dsi_qmi_port_names[11] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[12] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[13] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[14] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[15] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[16] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[17] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[18] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[19] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[20] = DSI_QMI_PORT_RMNET_INVALID;
  dsi_qmi_port_names[21] = DSI_QMI_PORT_RMNET_INVALID;

  dsi_install_device_names(DSI_DEV_RMNET_DATA_PREFIX,
                           DSI_LOCAL_MODEM_IFACE_START,
                           DSI_LOCAL_MODEM_IFACE_END);
#endif

  /* Query the Android propoerty to deterince the platform configuration */
  if( DSI_ERROR == dsi_process_target())
  {
    DSI_LOG_ERROR("%s", "error on target configuration");
    return DSI_ERROR;
  }

  /* Map QMI ports to interfaces */
  for(i = 0; i<DSI_MAX_IFACES; i++)
  {
    dsi_invalidate_iface(i);

    /* set qmi port name string in the dsi iface */
    DSI_SET_WDS_STR(i,
                    dsi_qmi_port_names[i],
                    ret);
    if (DSI_ERROR == ret)
    {
      DSI_LOG_ERROR("dsi_qmi_port_name[%d] is invalid", i);
      return DSI_ERROR;
    }

    DSI_SET_DEV_STR(i,
                    dsi_device_names[i],
                    ret);
    if (DSI_ERROR == ret)
    {
      DSI_LOG_ERROR("dsi_device_names[%d] is invalid", i);
      return DSI_ERROR;
    }

    /* Initialize the interface mutex */
    pthread_mutex_init(DSI_GET_MUTEX(i), NULL);
  }
  return DSI_SUCCESS;
}

/*===========================================================================
  FUNCTION:  dsi_invalidate_store_handles
===========================================================================*/
/*!
    @brief
    reset dsi_store_table[] array at power up

    @return
    none
*/
/*=========================================================================*/
static DSI_INLINE void dsi_invalidate_store_handles()
{
  int c=0;
  for(c = 0; c<DSI_MAX_DATA_CALLS; c++)
  {
    dsi_store_table[c].is_valid = DSI_FALSE;
    dsi_store_table[c].dsi_store_ptr = NULL;
  }
}

/*===========================================================================
  FUNCTION:  dsi_init_internal
===========================================================================*/
/*!
    @brief
    This function initializes the dsi_netctrl module

    @return
    DSI_SUCCESS
    DSI_ERROR
*/
/*=========================================================================*/
int dsi_init_internal(dsi_mni_vtable_t * mni_vtbl, boolean from_ssr)
{
  int ret = DSI_ERROR;

  DSI_LOG_DEBUG( "%s", "dsi_init_internal: ENTRY" );

  do
  {
    ret = DSI_ERROR;

    if (dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_init_internal: dsi already inited");
      break;
    }

#ifndef DSI_NETCTRL_OFFTARGET
#ifdef FEATURE_DATA_LOG_QXDM
    /* init diag */
    if (DSI_TRUE != Diag_LSM_Init(NULL))
    {
      DSI_LOG_ERROR("%s","couldn't init diag");
    }
#endif
#endif

    /* initialize multicast logging layer */
    ds_log_multicast_init();

    /* overrides */
    if (NULL != mni_vtbl)
    {
      if (NULL != mni_vtbl->mni_init_f)
      {
        dsi_mni_vtbl.mni_init_f = mni_vtbl->mni_init_f;
      }
      if (NULL != mni_vtbl->mni_start_f)
      {
        dsi_mni_vtbl.mni_start_f = mni_vtbl->mni_start_f;
      }
      if (NULL != mni_vtbl->mni_stop_f)
      {
        dsi_mni_vtbl.mni_stop_f = mni_vtbl->mni_stop_f;
      }
      if (NULL != mni_vtbl->mni_abort_start_f)
      {
        dsi_mni_vtbl.mni_abort_start_f = mni_vtbl->mni_abort_start_f;
      }
      if (NULL != mni_vtbl->mni_look_up_f)
      {
        dsi_mni_vtbl.mni_look_up_f = mni_vtbl->mni_look_up_f;
      }
    }

    /* reset the static tables */
    dsi_invalidate_modems(from_ssr);
    if (DSI_ERROR == dsi_init_ifaces())
    {
      break;
    }
    dsi_invalidate_store_handles();

    /* =======================================================
      currently, we are statically configuring modem
      technologies here.
      later on, if modem supports query technology feature OR
      we decide to do global route look up,
      we should use those dynamic values
      ======================================================= */
    DSI_LOG_DEBUG("%s","dsi_init init modem DSI_RADIO_4GMODEM" \
                  " with LTE/DO technology support" \
                  " cotains iface ids 11..21");

    /* modem DSI_RADIO_4GMODEM contains iface 11..21 */
    DSI_MODEM_SET_IFACE_START(DSI_RADIO_4GMODEM, DSI_REMOTE_MODEM_IFACE_START);
    DSI_MODEM_SET_IFACE_END(DSI_RADIO_4GMODEM, DSI_REMOTE_MODEM_IFACE_END);
    /* set this modem index on iface 11..21 */
    DSI_SET_MODEM(11, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(12, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(13, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(14, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(15, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(16, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(17, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(18, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(19, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(20, DSI_RADIO_4GMODEM);
    DSI_SET_MODEM(21, DSI_RADIO_4GMODEM);
    /* define technologies for 4G modem only for Fusion target */
    /* modem DSI_RADIO_4GMODEM supports following techs  */
    DSI_MODEM_SET_TECH(DSI_RADIO_4GMODEM,DSI_RADIO_TECH_LTE);
    DSI_MODEM_SET_TECH(DSI_RADIO_4GMODEM,DSI_RADIO_TECH_DO);
    DSI_MODEM_SET_TECH(DSI_RADIO_4GMODEM,DSI_RADIO_TECH_CDMA);
    DSI_MODEM_SET_TECH(DSI_RADIO_4GMODEM,DSI_RADIO_TECH_UMTS);
    /* modem DSI_RADIO_4GMODEM has default iface 11 */
    DSI_MODEM_SET_DEFAULT_IFACE(DSI_RADIO_4GMODEM, 11);

    DSI_LOG_DEBUG("%s","dsi_init init modem DSI_RADIO_3GMODEM" \
                  " with 1X technology support " \
                  " contains iface id 0..10");
    /* modem DSI_RADIO_3GMODEM contains iface 0..10 */
    DSI_MODEM_SET_IFACE_START(DSI_RADIO_3GMODEM, DSI_LOCAL_MODEM_IFACE_START);
    DSI_MODEM_SET_IFACE_END(DSI_RADIO_3GMODEM, DSI_LOCAL_MODEM_IFACE_END);
    /* set this modem index on iface 0..10*/
    DSI_SET_MODEM(0, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(1, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(2, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(3, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(4, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(5, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(6, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(7, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(8, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(9, DSI_RADIO_3GMODEM);
    DSI_SET_MODEM(10, DSI_RADIO_3GMODEM);
    DSI_MODEM_SET_TECH(DSI_RADIO_3GMODEM,DSI_RADIO_TECH_CDMA);
    DSI_MODEM_SET_TECH(DSI_RADIO_3GMODEM,DSI_RADIO_TECH_1X);
    DSI_MODEM_SET_TECH(DSI_RADIO_3GMODEM,DSI_RADIO_TECH_UMTS);

    /* modem DSI_RADIO_3GMODEM has default iface 0 */
    DSI_MODEM_SET_DEFAULT_IFACE(DSI_RADIO_3GMODEM, 0);

    if(!from_ssr)
    {
      if (DSI_SUCCESS != dsi_netctrl_cb_init())
      {
        DSI_LOG_ERROR("%s", "couldn't init dsi_netctrl_cb");
        break;
      }
    }

    /* now init the real modem network interfaces */
    ret = dsi_mni_vtbl.mni_init_f(from_ssr);
    if (DSI_ERROR == ret)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  /* print appropriate status before exiting */
  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_init_internal: EXIT with success" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_init_internal: EXIT with error" );
  }

  return ret;
}/* dsi_init_internal() */

/*===========================================================================
  FUNCTION:  dsi_release_internal
===========================================================================*/
/*!
    @brief
    This function initializes the dsi_netctrl module

    @return
    DSI_SUCCESS
    DSI_ERROR
*/
/*=========================================================================*/
void dsi_release_internal(void)
{
  int i;

  DSI_LOG_DEBUG( "%s", "dsi_release_internal: ENTRY" );

  if (0 != ds_cmdq_flush(&dsi_netctrl_cb_cmdq))
  {
    DSI_LOG_ERROR("%s","*** could not flush dsi_netctrl_cb_cmdq ***");
  }

  for(i = 0; i<DSI_MAX_IFACES; i++)
  {
    /* cleanup call entries */
    dsi_call_cleanup(i);
  }

  /* Perform mni cleanup */
  dsi_mni_vtbl.mni_cleanup_f(TRUE);

  /* invalidate iface and destroy mutexes */
  for(i = 0; i<DSI_MAX_IFACES; i++)
  {
    dsi_invalidate_iface(i);
    pthread_mutex_destroy(DSI_GET_MUTEX(i));
  }

  DSI_LOG_DEBUG( "%s", "dsi_release_internal: EXIT" );
}/* dsi_release_internal() */
