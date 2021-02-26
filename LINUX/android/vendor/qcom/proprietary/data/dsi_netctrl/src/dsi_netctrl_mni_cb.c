/*!
  @file
  dsi_netctrl_mni_cb.c

  @brief
  implements dsi_netctrl callback processing functions

*/

/*===========================================================================

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
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
04/19/10   js      created

===========================================================================*/
#include "dsi_netctrli.h"
#include "dsi_netctrl_qmi_wds.h"
#include "ds_cmdq.h"
#include "qdi.h"
#include "dsi_netctrl.h"
#include "dsi_netctrl_mni.h"
#include "dsi_netctrl_qmi.h"
#include <stdint.h>

/*===========================================================================
              FUNCTIONS EXECUTED IN dsi_netctrl_cb THREAD
===========================================================================*/
/*===========================================================================
  FUNCTION:  dsi_process_stop_nw_async_rsp
===========================================================================*/
/*!
    @brief
    processes asynchronous response to the stop_nw qmi wds request
    i is dsi iface id
    st is the store handle pointer
    sys_err_code is the sys_err_code we got from qmi wds

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_process_stop_nw_async_rsp
(
  dsi_store_t * st,
  int i,
  int sys_err_code,
  int qmi_err_code,
  qdi_wds_async_rsp_data_type * rsp_data
)
{
  int ret = DSI_ERROR;
  int port_incall = FALSE;
  dsi_link_status_type link_state_v4 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  dsi_link_status_type link_state_v6 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  dsi_ce_reason_t ce_reason;
  boolean send_no_net = FALSE;
  boolean release_iface = FALSE;
  boolean handle_found = FALSE;
  int j;
  int ip_family = WDS_IP_FAMILY_PREF_IPV4_V01;
  dsi_ip_family_t ipf = DSI_IP_FAMILY_V4;

  DSI_LOG_DEBUG( "%s", "dsi_process_stop_nw_async_rsp: ENTRY" );

  do
  {
    if (NULL == st)
    {
      DSI_LOG_ERROR("%s","NULL st received");
      break;
    }

    if(NULL == rsp_data)
    {
      DSI_LOG_ERROR("%s","received NULL rsp_data");
      break;
    }

    if (rsp_data->flags == QDI_RSP_DATA_V4_V6)
    {
      ip_family = WDS_IP_FAMILY_PREF_UNSPEC_V01;
      ipf = DSI_NUM_IP_FAMILIES;
    }
    else if (rsp_data->flags == QDI_RSP_DATA_V4)
    {
      ip_family = WDS_IP_FAMILY_PREF_IPV4_V01;
      ipf = DSI_IP_FAMILY_V4;
    }
    else
    {
      ip_family = WDS_IP_FAMILY_PREF_IPV6_V01;
      ipf = DSI_IP_FAMILY_V6;
    }
    DSI_LOG_DEBUG(  "stop_nw_async_rsp: rsp_data->flags:%d, ip_family:%d, ipf:%d ",
                    rsp_data->flags,ip_family,ipf );
    /* check if txn was completed successfully */
    if(QMI_NO_ERR == sys_err_code)
    {
      DSI_LOG_DEBUG("%s", "qmi txn " \
                    "QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01 successful");

      /* Need to check for data call active on QMI port in another
       * process domain.  In such cases, the Modem will not send
       * WDS_PKT_SRVC_IND.  Need to locally generate call end
       * notification to clients as there will be no PLATFORM_DOWN
       * indication forthcoming. */
      if ( (DSI_IS_V4_CALL_DOWN(ipf, i) ||
            DSI_IS_V6_CALL_DOWN(ipf, i) ||
            DSI_IS_V4_V6_CALL_DOWN(ipf, i)
           ) &&
           DSI_SUCCESS == dsi_is_qmiport_incall
                          ( i, &port_incall,&link_state_v4,&link_state_v6 ) &&
           TRUE == port_incall )
      {


        if ( (DSI_IS_V4_CALL_DOWN(ipf, i) &&
               (link_state_v4 == WDS_CONNECTION_STATUS_CONNECTED_V01)) ||
             (DSI_IS_V6_CALL_DOWN(ipf, i) &&
               (link_state_v6 == WDS_CONNECTION_STATUS_CONNECTED_V01)) ||
             (DSI_IS_V4_V6_CALL_DOWN(ipf, i) &&
               ( (link_state_v4 == WDS_CONNECTION_STATUS_CONNECTED_V01) ||
                 (link_state_v6 == WDS_CONNECTION_STATUS_CONNECTED_V01)
               )))
        {
          send_no_net = TRUE;
          release_iface = TRUE;
        }
        else
        {
          send_no_net = FALSE;
          release_iface = FALSE;
        }
        DSI_LOG_DEBUG( " [%d] iface [%d] ip_family with [%d/%d]" \
                       "link_state_v4/link_state_v6 send_no_net:%d",
                      i,ip_family,link_state_v4,link_state_v6,send_no_net);
      }

      /* NOTE: if call was not active on some other qmi port, only then
       * we expect pkt_srvc_ind DISCONNECTED */
    }
    else if(QMI_SERVICE_ERR == sys_err_code &&
            QMI_ERR_NO_EFFECT_V01 == qmi_err_code)
    {
       /*No effect is received if the call is already disconnected by Modem
       If the proper processing of the WDS disconnects has been completed by
       Netmgr, then the iface would be released and its state changed to
       Not_In_Use. So if the state is In_Disconnect, we are awaiting the
       appropriate event from Netmgr to notify the DSI client*/
       if(DSI_GET_STATE(i) == DSI_IFACE_STATE_IN_DISCONNECT)
       {
          send_no_net = FALSE;
          release_iface = FALSE;
       }
       else
       {
          send_no_net = TRUE;
          release_iface = TRUE;
       }
    }
    else
    {
      /* we received sys err code that is not handled right now */
      DSI_LOG_DEBUG("%s", "qmi txn " \
                    "QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01 unsuccessful");
      break;
    }

    if (TRUE == send_no_net)
    {
      /* Send the event to all clients on this interface */
      for (j = 0; j < DSI_MAX_DATA_CALLS; j++)
      {
        dsi_store_t *hndl = (dsi_store_t *)dsi_store_table[j].dsi_store_ptr;
        if ((hndl != NULL &&
            i == hndl->priv.dsi_iface_id) &&
           ((int)hndl->priv.start_nw_params.ip_family_preference == ip_family))
        {
          /* notify user that network is down (even though it's not) */
          if( NULL != hndl->net_ev_cb )
          {
            DSI_LOG_DEBUG( "notifying user of st_hndl [%p] of event NET_NO_NET for [%d] ip_family",
                           hndl,ip_family );
            hndl->net_ev_cb( (dsi_hndl_t *)hndl,
                           hndl->user_data,
                           DSI_EVT_NET_NO_NET,
                           NULL );
          }
          else
          {
            DSI_LOG_ERROR( "callback on store pointer [%p] is NULL", hndl );
          }
        }
      }
    }

    /* release the interface is call is already down and there
     * are no further pkt_srvc_ind expected on it */
    if (TRUE == release_iface)
    {
      /* detach this interface from the client store handle */
      DSI_LOG_DEBUG( "st_hndl [%p] detached from interface [%d]",
                     st, i );
      DSI_LOCK_MUTEX( &(st->priv.mutex) );
      dsi_detach_dsi_iface( st );
      DSI_UNLOCK_MUTEX( &(st->priv.mutex) );

      /* we determine ce reason from QMI
       * set reason_code to UNKNOWN here */
      ce_reason.reason_code = DSI_CE_REASON_UNKNOWN;
      ce_reason.reason_type = DSI_CE_TYPE_UNINIT;

      /* set this interface free... */
      dsi_release_dsi_iface( i, &ce_reason ,ipf);

      /* Release the IFACE clients only if this store does not have any handle sharing the IFACE */
      if(send_no_net == TRUE)
      {
        for (j = 0; j < DSI_MAX_DATA_CALLS; j++)
        {
          dsi_store_t *hndl = (dsi_store_t *)dsi_store_table[j].dsi_store_ptr;
          if ((hndl != NULL && i == hndl->priv.dsi_iface_id) )
          {
            handle_found = TRUE;
            break;
          }
        }
        if(!handle_found)
        {
          if( (DSI_IP_VERSION_4_6 != st->priv.ip_version) ||
              ( (DSI_GET_WDS_V4_TXN(st) == DSI_INVALID_WDS_TXN) &&
                (DSI_GET_WDS_V6_TXN(st) == DSI_INVALID_WDS_TXN) ) )
          {
            dsi_mni_release_client(i);
          }
	}
      }
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_process_stop_nw_async_rsp: " \
                   "EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_process_stop_nw_async_rsp: " \
                   "EXIT with err" );
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_query_if_addr_from_qdi
===========================================================================*/
/*!
    @brief
    Queries QDI to obtain the interface addresses (IP, DNS, Gateway)

    @param
    iface_id   - dsi iface id
    ip_version - the IP version of the call (V4, V6 or V4V6)

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_query_if_addr_from_qdi
(
  int iface_id,
  int ip_version,
  qdi_wds_async_rsp_data_type * rsp_data
)
{
  int ret = DSI_SUCCESS;
  dsi_store_t *st = NULL;
  int i;
  boolean client_found = FALSE;
  wds_technology_name_enum_v01 tech_name = WDS_TECHNOLOGY_NAME_ENUM_MAX_ENUM_VAL_V01;

  if (!DSI_IS_ID_VALID(iface_id))
  {
    DSI_LOG_ERROR("dsi_query_if_addr_from_qdi: invalid iface_id=%d", iface_id);
    return DSI_ERROR;
  }

  /* Check for any valid client for the iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;

    if ((st != NULL && iface_id == st->priv.dsi_iface_id) &&
        ((st->priv.ip_version == DSI_IP_VERSION_4_6) ||
         ((st->priv.ip_version == DSI_IP_VERSION_4) && (ip_version == DSI_IP_VERSION_4)) ||
         ((st->priv.ip_version == DSI_IP_VERSION_6) && (ip_version == DSI_IP_VERSION_6))  )
       )
    {
      client_found = TRUE;
      break;
    }
  }
  if (TRUE == client_found)
  {
    DSI_LOG_VERBOSE("dsi_query_if_addr_from_qdi: valid client=0x%p found at "
                    "store index=%d", st, i);
    tech_name = st->priv.call_tech;
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_query_if_addr_from_qdi: valid client not found");
    return DSI_ERROR;
  }

  /* Since we won't be getting any platform events from netmgr, query for
     all the interface addresses directly from QDI */
  switch (ip_version)
  {
    case DSI_IP_VERSION_4:
      DSI_LOG_VERBOSE("%s","IPv4 call - querying addresses from QDI");
      if (QDI_SUCCESS != qdi_get_addr_info(DSI_GET_QDI_HNDL(iface_id),
                                           DSI_GET_DEV_STR(iface_id),
                                           AF_INET,
                                           &DSI_GET_ADDRINFO(iface_id, DSI_IP_FAMILY_V4),
                                           tech_name))
      {
        DSI_LOG_ERROR("%s","failed to get IPv4 addr info from QDI");
        ret = DSI_ERROR;
      }
      else
      {
        DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface_id));
        DSI_INCR_COUNT(iface_id,DSI_IP_VERSION_4);
        /* let go the lock before calling user cb */
        DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface_id));
      }
      break;

    case DSI_IP_VERSION_6:
    {
      int ret_v6 = QDI_FAILURE;
      DSI_LOG_VERBOSE("%s","IPv6 call - querying addresses from QDI");
      ret_v6 = qdi_get_addr_info(DSI_GET_QDI_HNDL(iface_id),
                                 DSI_GET_DEV_STR(iface_id),
                                 AF_INET6,
                                 &DSI_GET_ADDRINFO(iface_id, DSI_IP_FAMILY_V6),
                                 tech_name);
      if (QDI_FAILURE == ret_v6)
      {
        DSI_LOG_ERROR("%s","failed to get IPv6 addr info from QDI");
        ret = DSI_ERROR;
      }
      else
      {
        /*We still need to increment the per ip family ref count
         in the event of retry as there will no WDS conn event received*/
        if(ret_v6 == QDI_RETRY)
        {
           ret = DSI_RETRY;
        }
        DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface_id));
        DSI_INCR_COUNT(iface_id,DSI_IP_VERSION_6);
        /* let go the lock before calling user cb */
        DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface_id));
      }
    }
      break;

    case DSI_IP_VERSION_4_6:
    {
      int ret_v4 = QDI_FAILURE, ret_v6 = QDI_FAILURE;
      DSI_LOG_VERBOSE("%s","IPv4v6 call - querying addresses from QDI");

      /*QDI masks the No Effect error if either of the family has no QMI
      error. Hence for dual IP call requests, we query Interface and increment
      ref counts only for No Effect error.
      However if a different client brings up the call on the same iface,
      we get No_Err instead of No_effect*/

      if((rsp_data->rsp_data_v4.qmi_err == QMI_ERR_NO_EFFECT_V01) ||
          (rsp_data->rsp_data_v4.qmi_err == QMI_ERR_NONE_V01 &&
            st->priv.rl_port_incall == TRUE &&
            st->priv.rl_link_state_v4 == WDS_CONNECTION_STATUS_CONNECTED_V01))
      {
        if (QDI_SUCCESS != (ret_v4 = qdi_get_addr_info(DSI_GET_QDI_HNDL(iface_id),
                                                        DSI_GET_DEV_STR(iface_id),
                                                        AF_INET,
                                                        &DSI_GET_ADDRINFO(iface_id, DSI_IP_FAMILY_V4),
                                                        tech_name)))
        {
          DSI_LOG_ERROR("%s","failed to get IPv4 addr info from QDI");
        }
        else
        {
          DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface_id));
          DSI_INCR_COUNT(iface_id,DSI_IP_VERSION_4);
          /* let go the lock before calling user cb */
          DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface_id));
        }
      }

      if((rsp_data->rsp_data_v6.qmi_err == QMI_ERR_NO_EFFECT_V01) ||
          (rsp_data->rsp_data_v6.qmi_err == QMI_ERR_NONE_V01 &&
            st->priv.rl_port_incall == TRUE &&
            st->priv.rl_link_state_v6 == WDS_CONNECTION_STATUS_CONNECTED_V01))
      {
        ret_v6 = qdi_get_addr_info(DSI_GET_QDI_HNDL(iface_id),
                                   DSI_GET_DEV_STR(iface_id),
                                   AF_INET6,
                                   &DSI_GET_ADDRINFO(iface_id, DSI_IP_FAMILY_V6),
                                   tech_name);
        if (QDI_FAILURE == ret_v6)
        {
          DSI_LOG_ERROR("%s","failed to get IPv6 addr info from QDI");
        }
        else
        {
          /*We still need to increment the per ip family ref count
          in the event of retry as there will no WDS conn event received*/
          DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface_id));
          DSI_INCR_COUNT(iface_id,DSI_IP_VERSION_6);
          /* let go the lock before calling user cb */
          DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface_id));
        }
      }
      /* If we failed to get both addresses, return failure */
      if (QDI_SUCCESS != ret_v4 && QDI_FAILURE == ret_v6)
      {
        ret = DSI_ERROR;
      }

      /* If v4 address query failed,but v6 address is Tentative, return retry*/
      if (QDI_SUCCESS != ret_v4 && QDI_RETRY == ret_v6)
      {
        DSI_LOG_ERROR("%s","Returning Retry dsi_query_if_addr_from_qdi");
        ret = DSI_RETRY;
      }
      break;
    }

    default:
      DSI_LOG_ERROR("dsi_query_if_addr_from_qdi: invalid ip_version=%d", ip_version);
      ret = DSI_ERROR;
      break;
  }

  return ret;
}
/*===========================================================================
  FUNCTION:  dsi_query_if_tech_from_qdi
===========================================================================*/
/*!
    @brief
    Queries QDI to obtain the technology

    @param
    iface_id   - dsi iface id

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_query_if_tech_from_qdi
(
  int iface_id,
  int ip_family
)
{
  dsi_store_t *st = NULL;
  int i;
  boolean client_found = FALSE;
  wds_technology_name_enum_v01 tech_name = WDS_TECHNOLOGY_NAME_ENUM_MAX_ENUM_VAL_V01;

  if (!DSI_IS_ID_VALID(iface_id))
  {
    DSI_LOG_ERROR("dsi_query_if_tech_from_qdi: invalid iface_id=%d", iface_id);
    return DSI_ERROR;
  }

  DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface_id));

  /* Check for any valid client for the iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;

    if ((st != NULL && iface_id == st->priv.dsi_iface_id) &&
        ((st->priv.ip_version == DSI_IP_VERSION_4_6) ||
         ((st->priv.ip_version == DSI_IP_VERSION_4) && (ip_family == DSI_IP_FAMILY_V4)) ||
         ((st->priv.ip_version == DSI_IP_VERSION_6) && (ip_family == DSI_IP_FAMILY_V6))  )
       )
    {
      client_found = TRUE;
      break;
    }
  }

  if (TRUE == client_found)
  {
    DSI_LOG_VERBOSE("dsi_query_if_tech_from_qdi: valid client=0x%p found at "
                    "store index=%d", st, i);
    tech_name = st->priv.call_tech;
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_query_if_tech_from_qdi: valid client not found");
  }
  DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface_id));

  return tech_name;
}
/*===========================================================================
  FUNCTION:  dsi_update_call_end_reason
===========================================================================*/
/*!
    @brief
    Determines if a pending DSI_EVT_NET_IS_CONN event should be sent to the
    upper layers for a Dual-IP call

    @param
    st       - the store handle pointer
    rsp_data - the response data we got from qmi wds

    @return
    None
*/
/*=========================================================================*/
static void dsi_update_call_end_reason
(
  dsi_store_t                 *st,
  qdi_wds_async_rsp_data_type *rsp_data
)
{
  if (!st || !rsp_data)
  {
    DSI_LOG_ERROR("%s", "dsi_update_call_end_reason: bad parameter(s)");
    return;
  }

  /* Update the V4 call end reason */
  if(rsp_data->flags & QDI_RSP_DATA_V4)
  {
   if ( rsp_data->rsp_data_v4.rsp_data.start_nw_rsp.verbose_call_end_reason_valid == TRUE )
   {
      st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_code =
        rsp_data->rsp_data_v4.rsp_data.start_nw_rsp.verbose_call_end_reason.call_end_reason;
      st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_type =
        dsi_map_qmi_to_dsi_ce_reason_type(
          rsp_data->rsp_data_v4.rsp_data.start_nw_rsp.verbose_call_end_reason.call_end_reason_type);
    }
    else
    {
      DSI_LOG_DEBUG("dsi_update_call_end_reason: V4 call end reason invalid on handle [%p]", st);
      st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_type = DSI_CE_TYPE_INVALID;
    }

    DSI_LOG_DEBUG("set call end reason V4 code/type [%d/%d] on handle [%p]",
                  st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_code,
                  st->priv.ce_reason[DSI_IP_FAMILY_V4].reason_type, st);
  }

  /* Update the V6 call end reason */
  if(rsp_data->flags & QDI_RSP_DATA_V6)
  {
   if ( rsp_data->rsp_data_v6.rsp_data.start_nw_rsp.verbose_call_end_reason_valid == TRUE )
    {
      st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_code =
        rsp_data->rsp_data_v6.rsp_data.start_nw_rsp.verbose_call_end_reason.call_end_reason;
      st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_type =
        dsi_map_qmi_to_dsi_ce_reason_type(
          rsp_data->rsp_data_v6.rsp_data.start_nw_rsp.verbose_call_end_reason.call_end_reason_type);
    }
    else
    {
      DSI_LOG_DEBUG("dsi_update_call_end_reason: V6 call end reason invalid on handle [%p]", st);
      st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_type = DSI_CE_TYPE_INVALID;
    }

    DSI_LOG_DEBUG("set call end reason V6 code/type [%d/%d] on handle [%p]",
                  st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_code,
                  st->priv.ce_reason[DSI_IP_FAMILY_V6].reason_type, st);
  }
}

/*===========================================================================
  FUNCTION:  dsi_query_iface_addr_and_incr_ref_count
===========================================================================*/
/*!
    @brief
    This function queries the interface addresses from QDI and if they are
    valid then increments the iface ref count and clears any pending flags.
    Also, returns the appropriate DSI event to send to the clients.

    @param
    st       - the store handle pointer
    iface    - interface id

    @return
    DSI_EVT_NET_IS_CONN
    DSI_EVT_NET_NO_NET
    DSI_EVT_INVALID
*/
/*=========================================================================*/
static dsi_net_evt_t
dsi_query_iface_addr_and_incr_ref_count
(
  dsi_store_t  *st,
  int          iface,
  int          ip_family,
  qdi_wds_async_rsp_data_type * rsp_data
)
{
  dsi_net_evt_t event = DSI_EVT_INVALID;
  int ret = DSI_ERROR;

  if (!st)
  {
    DSI_LOG_ERROR("%s","dsi_query_iface_addr_and_incr_ref_count: invalid param");
    return event;
  }

  /* Since we won't be getting any platform events from netmgr, query for
     all the interface addresses directly from QDI */
  /* special case for eMBMS since there is no IP family associated with
     its interface */
  if ( (WDS_TECHNOLOGY_NAME_EMBMS_V01  != dsi_query_if_tech_from_qdi(iface, ip_family)) &&
       (WDS_EXT_TECH_PREF_EMBMS_V01  != st->priv.start_nw_params.ext_technology_preference)
       && (DSI_ERROR == (ret = dsi_query_if_addr_from_qdi(iface, st->priv.ip_version, rsp_data)))
     )
  {
    /* No valid address found on the interface */

    DSI_LOG_ERROR("No valid address found on iface=%d, ip_version=%d, incrementing count",
                  iface,
                  st->priv.ip_version);

    /* Increment the client ref_count */
    ++st->priv.ref_count;
    DSI_LOG_DEBUG("ref_count=%d for st=0x%p", st->priv.ref_count, st);

    /* Increment the V4 ref_count if V4 call resp was successful*/
    if ((DSI_IP_VERSION_4 == st->priv.ip_version ||  DSI_IP_VERSION_4_6 == st->priv.ip_version)  &&
       (QMI_NO_ERR == rsp_data->rsp_data_v4.sys_err ||
       (QMI_SERVICE_ERR == rsp_data->rsp_data_v4.sys_err &&
       ( QMI_SERVICE_ERR_NONE == rsp_data->rsp_data_v4.qmi_err)) ) )
    {
      DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface));
      DSI_INCR_COUNT(iface, DSI_IP_VERSION_4);
      DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface));
    }

   /* Increment the V6 ref_count if V6 call resp was successful*/
   if ((DSI_IP_VERSION_6 == st->priv.ip_version || DSI_IP_VERSION_4_6 == st->priv.ip_version) &&
      (QMI_NO_ERR == rsp_data->rsp_data_v6.sys_err ||
      ( QMI_SERVICE_ERR == rsp_data->rsp_data_v6.sys_err &&
      ( QMI_SERVICE_ERR_NONE == rsp_data->rsp_data_v6.qmi_err )) ) )
   {
     DSI_LOCK_MUTEX(DSI_GET_MUTEX(iface));
     DSI_INCR_COUNT(iface, DSI_IP_VERSION_6);
     DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(iface));
   }


   /* Return an INVALID event */
    event = DSI_EVT_INVALID;
  }
  /*Address configuration not complete, wait for event from Netmgr*/
  else if(ret == DSI_RETRY)
  {
    /* Return an INVALID event */
    event = DSI_EVT_INVALID;
    DSI_LOG_ERROR("%s() Returning Invalid event on Retry",
                  __func__);
  }
  /* If we already have a valid ref_count */
  else if (0 != st->priv.ref_count)
  {
    DSI_LOG_ERROR("Valid ref_count=%d found for st=0x%p",
                  st->priv.ref_count,
                  st);

    if (TRUE == st->priv.partial_retry)
    {
      event = DSI_EVT_NET_PARTIAL_CONN;
    }
    else
    {
      event = DSI_EVT_NET_NO_NET;
    }
  }
  else
  {
    /* Valid address found on the interface */
    DSI_LOG_VERBOSE("Valid address found on iface=%d, ip_version=%d, returning NET_IS_CONN",
                    iface,
                    st->priv.ip_version);


    DSI_LOCK_MUTEX(&(st->priv.mutex));
    /* Increment the client ref_count */
    ++st->priv.ref_count;
    DSI_UNLOCK_MUTEX(&(st->priv.mutex));

    DSI_LOG_VERBOSE("clearing pending flags call_state=%d, rl_qmi_inst=%d",
                    st->priv.call_state,
                    st->priv.rl_qmi_inst);

    /* Clear out the pending flags */
    if(DSI_STATE_CALL_CONNECTING == st->priv.call_state)
    {
      DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_CONNECTED);
      /* Return a NET_IS_CONN event */
      event = DSI_EVT_NET_IS_CONN;
    }

    st->priv.rl_qmi_inst = DSI_INVALID_QMI_INST;
    st->priv.rl_port_incall = FALSE;
    st->priv.rl_link_state_v4 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
    st->priv.rl_link_state_v6 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;

    /* Updating the call tech of store handle for a shared call */
    if( (WDS_TECHNOLOGY_NAME_EMBMS_V01 == st->priv.start_nw_params.ext_technology_preference)
        && (WDS_EXT_TECH_PREF_EMBMS_V01 != st->priv.call_tech))
    {
       st->priv.call_tech = WDS_TECHNOLOGY_NAME_EMBMS_V01;
    }
  }

  return event;
}

/*===========================================================================
  FUNCTION:  dsi_process_start_nw_async_rsp
===========================================================================*/
/*!
    @brief
    processes asynchronous response to start_nw request
    i is dsi iface id
    st is the store handle pointer
    sys_err_code is the sys_err_code we got from qmi wds
    qmi_err_code is the qmi_err_code we got from qmi wds
    rsp_data is the response data we got from qmi wds

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_process_start_nw_async_rsp
(
  dsi_store_t * st,
  int i,
  int sys_err_code,
  int qmi_err_code,
  qdi_wds_async_rsp_data_type * rsp_data
)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  /* notify event if notify_user is set to TRUE */
  boolean notify_user = DSI_FALSE;
  dsi_net_evt_t event = DSI_EVT_INVALID;
  int rl_qmi_inst;
  int port_incall = FALSE;
  dsi_link_status_type link_state_v4 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  dsi_link_status_type link_state_v6 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  int ip_family = DSI_NUM_IP_FAMILIES;
  boolean handle_found = FALSE;
  int j;

  DSI_LOG_DEBUG( "%s", "dsi_process_start_nw_async_rsp: ENTRY" );


  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if(NULL == st)
    {
      DSI_LOG_ERROR("%s","received NULL st");
      break;
    }

    if(NULL == rsp_data)
    {
      DSI_LOG_ERROR("%s","received NULL rsp_data");
      break;
    }

    if (rsp_data->flags == QDI_RSP_DATA_V4_V6)
    {
      ip_family = DSI_NUM_IP_FAMILIES;
      if((DSI_GET_WDS_V4_TXN(st) == DSI_INVALID_WDS_TXN)||
         (DSI_GET_WDS_V6_TXN(st) == DSI_INVALID_WDS_TXN))
      {
        DSI_LOG_DEBUG("dsi_qmi_wds_cmd_cb: v4-txn:%d, v6-txn:%d",
                        DSI_GET_WDS_V4_TXN(st), DSI_GET_WDS_V6_TXN(st));
        DSI_LOG_ERROR("%s","dsi_qmi_wds_cmd_cb called in invalid state");
        break;
      }
      else
      {
        DSI_SET_WDS_V6_TXN(st,DSI_INVALID_WDS_TXN);
        DSI_SET_WDS_V4_TXN(st,DSI_INVALID_WDS_TXN);
      }
    }
    else if (rsp_data->flags == QDI_RSP_DATA_V4)
    {
      ip_family = DSI_IP_FAMILY_V4;
      if(DSI_GET_WDS_V4_TXN(st) == DSI_INVALID_WDS_TXN)
      {
        DSI_LOG_ERROR("%s","dsi_qmi_wds_cmd_cb called in invalid state");
        break;
      }
      else
      {
        DSI_SET_WDS_V4_TXN(st,DSI_INVALID_WDS_TXN);
      }
    }
    else
    {
      ip_family = DSI_IP_FAMILY_V6;
      if(DSI_GET_WDS_V6_TXN(st) == DSI_INVALID_WDS_TXN)
      {
        DSI_LOG_ERROR("%s","dsi_qmi_wds_cmd_cb called in invalid state");
        break;
      }
      else
      {
        DSI_SET_WDS_V6_TXN(st,DSI_INVALID_WDS_TXN);
      }
    }

    /* Update the call end reason */
    dsi_update_call_end_reason(st, rsp_data);

    /* check if txn was completed successfully */
    if(QMI_NO_ERR == sys_err_code)
    {
      /* Check for case where interface is already up in another process (ie
       * Modem has just incremented its ref-count).  Currently the Modem does
       * not provide an explicit indication that packet service indication
       * should be expected after start/stop command response.  This poses
       * problem with start case in that we wait indefinitely for pkt_srvc_ind
       * event which will never arrive.  For *most* cases we can detect this
       * condition by looking at the QMI handle in routing results, which
       * indicates if Modem interface is already in UP state.  There is a race
       * condition however if interface teardown occurs between our route lookup
       * and start interface; need Modem change to address this case. */

      DSI_LOG_ERROR("rl_qmi_inst = [%d], companion IP [%d]",st->priv.rl_qmi_inst,
                                                            st->priv.companion_ip_call);

      if( ( DSI_INVALID_QMI_INST != st->priv.rl_qmi_inst) &&
           (st->priv.companion_ip_call != TRUE) )
      {
        /* some other process had already started this interface on modem */
        DSI_LOG_DEBUG("dsi interface [%d] already open " \
                      "by some other process", i);

        /* Query the interface addresses and increment the interface ref_count
           if a valid address is found */
        event = dsi_query_iface_addr_and_incr_ref_count(st, i,ip_family, rsp_data);

        if (DSI_EVT_INVALID != event)
        {
          DSI_LOG_DEBUG("notifying user of event=%d",
                        event);
          notify_user = DSI_TRUE;
        }
      }
      else
      {
        /* successfull !! */
        DSI_LOG_DEBUG("%s", "qmi txn success: rsp= " \
                      "QMI_WDS_START_NETWORK_INTERFACE_RESP_V01");

        /* If a Dual-IP call is already connected with only one valid address,
           send a DSI_EVT_NET_PARTIAL_CONN event */
        if (DSI_IP_VERSION_4_6 == st->priv.ip_version &&
            DSI_STATE_CALL_CONNECTED == st->priv.call_state &&
            (!DSI_IS_ADDR_VALID(st->priv.dsi_iface_id, DSI_IP_FAMILY_V4, iface_addr) ||
             !DSI_IS_ADDR_VALID(st->priv.dsi_iface_id, DSI_IP_FAMILY_V6, iface_addr)))
        {
          DSI_LOG_INFO( "Dual-IP call connected with one valid address, sending DSI_EVT_NET_PARTIAL_CONN on iface=%d",
                        st->priv.dsi_iface_id);
          event = DSI_EVT_NET_PARTIAL_CONN;
          notify_user = DSI_TRUE;
        }
        /* wait for pkt_srvc_ind now */
      }
    }
    else if(QMI_SERVICE_ERR == sys_err_code)
    {
      reti = DSI_SUCCESS;
      switch(qmi_err_code)
      {
      case QMI_ERR_NONE_V01:
      /* some other wds client had already
         started this interface on modem */
        DSI_LOG_DEBUG("dsi interface [%d] already open " \
                      "by some other wds client than [%p]",
                      i, DSI_GET_WDS_HNDL(i));

        /* Query the interface addresses and increment the interface ref_count
           if a valid address is found */
        event = dsi_query_iface_addr_and_incr_ref_count(st, i,ip_family, rsp_data);

        if (DSI_EVT_INVALID != event)
        {
          DSI_LOG_DEBUG("notifying user of event=%d",
                        event);
          notify_user = DSI_TRUE;
        }
        break;

      case QMI_ERR_NO_EFFECT_V01:
        /* some other wds client had already
           started this interface on modem */
        DSI_LOG_DEBUG("dsi interface [%d] already open " \
                      "by same  wds client [%p]",
                      i, DSI_GET_WDS_HNDL(i));

        /* Query the interface addresses and increment the interface ref_count
           if a valid address is found */

        event = dsi_query_iface_addr_and_incr_ref_count(st, i,ip_family, rsp_data);

        if (DSI_EVT_INVALID != event)
        {
          DSI_LOG_DEBUG("notifying client of event=%d",
                        event);
          notify_user = DSI_TRUE;
        }
        break;

      default:
        DSI_LOG_ERROR("received unknown service err [%d][%d]",
                      sys_err_code, qmi_err_code);
        reti = DSI_ERROR;
        event = DSI_EVT_NET_NO_NET;
        notify_user = DSI_TRUE;

        /* For a Dual-IP call, if we already have at least one valid address
           on the interface and this is a partial retry attempt then don't
           detach from the iface in this case */
        if (DSI_IP_VERSION_4_6 == st->priv.ip_version &&
            TRUE == st->priv.partial_retry &&
            (DSI_IS_ADDR_VALID(st->priv.dsi_iface_id, DSI_IP_FAMILY_V4, iface_addr) ||
             DSI_IS_ADDR_VALID(st->priv.dsi_iface_id, DSI_IP_FAMILY_V6, iface_addr)))
        {
          DSI_LOG_INFO( "Dual-IP partial retry failure, not detaching from iface=%d",
                        st->priv.dsi_iface_id);
          event = DSI_EVT_NET_PARTIAL_CONN;
          DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_CONNECTED);
        }
        else
        {
          DSI_LOG_INFO( "Detaching from iface=%d", st->priv.dsi_iface_id );
          DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_IDLE);

          /* detach interface from call store handle */
          if(DSI_SUCCESS != dsi_link_release(st->priv.dsi_iface_id))
          {
            DSI_LOG_ERROR("dsi_link_release failed: iface=[%d] " ,
                          st->priv.dsi_iface_id);
          }
          DSI_LOCK_MUTEX(&(st->priv.mutex));
          dsi_detach_dsi_iface(st);
          DSI_UNLOCK_MUTEX(&(st->priv.mutex));
        }
        break;
      }
      if (DSI_ERROR == reti)
      {
        break;
      }
    }
    else
    {
      /* Error condition reported */
      DSI_LOG_ERROR( "qmi txn err: sys=%d qmi=%d rsp=0x%X",
                       sys_err_code, qmi_err_code,
                     QMI_WDS_START_NETWORK_INTERFACE_RESP_V01);
      /* notify user that net is down */
      notify_user = DSI_TRUE;
      event = DSI_EVT_NET_NO_NET;

      /* For a Dual-IP call, if we already have at least one valid address
         on the interface and this is a partial retry attempt then don't
         detach from the iface in this case */
      if (DSI_IP_VERSION_4_6 == st->priv.ip_version &&
          TRUE == st->priv.partial_retry &&
          (DSI_IS_ADDR_VALID(st->priv.dsi_iface_id, DSI_IP_FAMILY_V4, iface_addr) ||
           DSI_IS_ADDR_VALID(st->priv.dsi_iface_id, DSI_IP_FAMILY_V6, iface_addr)))
      {
        DSI_LOG_INFO( "Dual-IP partial retry failure, not detaching from iface=%d",
                      st->priv.dsi_iface_id);
        event = DSI_EVT_NET_PARTIAL_CONN;
        DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_CONNECTED);
      }
      else
      {
        DSI_LOG_INFO( "Detaching from iface=%d", st->priv.dsi_iface_id );
        DSI_UPDATE_CALL_STATE(st, DSI_STATE_CALL_IDLE);

        /* detach interface from call store handle */
        if(DSI_SUCCESS != dsi_link_release(st->priv.dsi_iface_id))
        {
          DSI_LOG_ERROR("dsi_link_release failed: iface=[%d] " ,
                        st->priv.dsi_iface_id);
        }
        DSI_LOCK_MUTEX(&(st->priv.mutex));
        dsi_detach_dsi_iface(st);
        DSI_UNLOCK_MUTEX(&(st->priv.mutex));
      }
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_TRUE == notify_user)
  {
    /* notify user that net is connected as
       there won't be any pkt_srvc_ind */
    DSI_LOG_DEBUG( "notify users on interface [%d] of event [%d]",
                   i, event );
    if (NULL != st->net_ev_cb)
    {
      st->net_ev_cb((dsi_hndl_t *)st,
                    st->user_data,
                    event,
                    NULL);

   /* Release the IFACE clients only if this store does not have any handle sharing the IFACE */
      if(event == DSI_EVT_NET_NO_NET)
      {
        for (j = 0; j < DSI_MAX_DATA_CALLS; j++)
        {
          dsi_store_t *hndl = (dsi_store_t *)dsi_store_table[j].dsi_store_ptr;
          if ((hndl != NULL && i == hndl->priv.dsi_iface_id) )
          {
            handle_found = TRUE;
            break;
          }
        }
        if(!handle_found)
        {
          /* Release the client only if both the txn's are cleared for DUAL IP calls */
          if( (DSI_IP_VERSION_4_6 != st->priv.ip_version) ||
              ( (DSI_GET_WDS_V4_TXN(st) == DSI_INVALID_WDS_TXN) &&
                (DSI_GET_WDS_V6_TXN(st) == DSI_INVALID_WDS_TXN) ) )
          {
            dsi_mni_release_client(i);
          }
        }
      }
    }
    else
    {
      DSI_LOG_ERROR("callback on store pointer [%p] is NULL",
                    st);
    }
  }

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_process_start_nw_async_rsp: " \
                   "EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_process_start_nw_async_rsp: " \
                   "EXIT with err" );
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_process_async_wds_rsp
===========================================================================*/
/*!
    @brief
    callback function registered for asynchronous qmi wds responses
    currently used for
    start_nw_if
    stop_nw_if


    @return
    none
*/
/*=========================================================================*/
void dsi_process_async_wds_rsp
(
  int                           user_handle,     /* QMI Msg Lib client ID  */
  qmi_service_id_type           service_id,      /* QMI service ID         */
  int                           sys_err_code,    /* QMI Msg Lib error      */
  int                           qmi_err_code,    /* QMI error              */
  void                         *user_data,       /* Callback context       */
  int                           rsp_id,          /* QMI Msg Lib txn ID     */
  qdi_wds_async_rsp_data_type  *rsp_data         /* QMI Msg Lib txn data   */
)
{
  dsi_store_t * st = (dsi_store_t *)user_data;
  int i = 0;
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;

  DSI_LOG_VERBOSE("%s", "dsi_process_async_wds_rsp: ENTRY");
  DSI_LOG_DEBUG("qmi wds cmd_cb: rsp_id=%d", rsp_id);
  (void)service_id;

  DSI_GLOBAL_LOCK;

  do
  {
    /* verify we received valid rsp_data */
    if (NULL == rsp_data)
    {
      DSI_LOG_ERROR("%s",
                    "dsi_process_async_wds_rsp: null resp data pointer passed");
      break;
    }

    /* verify we received valid user data */
    if (!DSI_IS_HNDL_VALID(st))
    {
      DSI_LOG_ERROR("dsi_process_async_wds_rsp: received invalid user data [%p]",
                    (unsigned int*)user_data);
      break;
    }

    i = st->priv.dsi_iface_id;

    /* Verify user data (iface_id) */
    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_process_async_wds_rsp received invalid iface_id [%d]",
                    i);
      break;
    }

    /* verify wds handle */
    if(DSI_GET_QDI_HNDL(i) != user_handle)
    {
      DSI_LOG_ERROR("dsi_process_async_wds_rsp rcvd invalid wds hndl [%d]",
                    user_handle);
      break;
    }

    reti = DSI_SUCCESS;
    switch(rsp_id)
    {
    case QMI_WDS_START_NETWORK_INTERFACE_RESP_V01:
      DSI_LOG_VERBOSE("%s", "rcvd QMI_WDS_START_NETWORK_INTERFACE_RESP_V01");

      reti = dsi_process_start_nw_async_rsp(st,
                                            i,
                                            sys_err_code,
                                            qmi_err_code,
                                            rsp_data);

      break;
    case QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01:
      DSI_LOG_VERBOSE("%s", "rcvd QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01");
      reti = dsi_process_stop_nw_async_rsp(st,
                                           i,
                                           sys_err_code,
                                           qmi_err_code,
                                            rsp_data);
      break;
    default:
      DSI_LOG_ERROR("dsi_process_async_wds_rsp received rsp_id [%d] that" \
                    "we do not handle", rsp_id);
      reti = DSI_ERROR;
      break;
    }
    if (DSI_ERROR == reti)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE( "%s", "dsi_process_async_wds_rsp: EXIT with suc" );
  }
  else
  {
    DSI_LOG_VERBOSE( "%s", "dsi_process_async_wds_rsp: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;
}

/*===========================================================================
  FUNCTION:  dsi_is_valid_client_found_for_iface
===========================================================================*/
/*!
    @brief
    Determines if a valid client is present for a given iface

    @return
    TRUE if a valid client is found for the given iface
    FALSE otherwise
*/
/*=========================================================================*/
static boolean dsi_is_valid_client_found_for_iface
(
  int  iface,
  int  ip_version
)
{
  boolean      client_found = FALSE;
  dsi_store_t  *st = NULL;
  int          i;


  if (!DSI_IS_ID_VALID(iface))
  {
    DSI_LOG_ERROR("%s", "dsi_is_valid_client_found_for_iface: invalid dsi iface");
    goto bail;
  }

  /* Check for any valid client for the iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;

    if ((st != NULL && iface == st->priv.dsi_iface_id) &&
        ((st->priv.ip_version == DSI_IP_VERSION_4_6) ||
         ((st->priv.ip_version == DSI_IP_VERSION_4) && (ip_version == DSI_IP_VERSION_4)) ||
         ((st->priv.ip_version == DSI_IP_VERSION_6) && (ip_version == DSI_IP_VERSION_6))  )
       )
    {
      client_found = TRUE;
      break;
    }
  }

  if (TRUE == client_found)
  {
    DSI_LOG_VERBOSE("dsi_is_valid_client_found_for_iface: valid client=0x%p found at "
                    "store index=%d", st, i);
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_is_valid_client_found_for_iface: valid client not found");
  }

bail:
  return client_found;
}

/*===========================================================================
  FUNCTION:  dsi_update_client_ref_count
===========================================================================*/
/*!
    @brief
    Updates the ref_count associated with the client's store handle

    @return
    None
*/
/*=========================================================================*/
static void dsi_update_client_ref_count
(
  int  iface,
  wds_ip_family_enum_v01 ip_family
)
{
  boolean      client_found = FALSE;
  dsi_store_t  *st = NULL;
  int          i;


  if (!DSI_IS_ID_VALID(iface))
  {
    DSI_LOG_ERROR("dsi_update_client_ref_count: invalid dsi iface=%d",
                  iface);
    return;
  }

  /* Update the ref_count for all valid clients for this iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;

    if ((st != NULL && iface == st->priv.dsi_iface_id ) &&
         (((st->priv.ip_version == DSI_IP_VERSION_4) && (ip_family == WDS_IP_FAMILY_IPV4_V01))||
         ((st->priv.ip_version == DSI_IP_VERSION_6) && (ip_family == WDS_IP_FAMILY_IPV6_V01)) ||
         (st->priv.ip_version == DSI_IP_VERSION_4_6) ))
    {
      DSI_LOCK_MUTEX(&(st->priv.mutex));
      ++st->priv.ref_count;
      DSI_UNLOCK_MUTEX(&(st->priv.mutex));

      DSI_LOG_VERBOSE("dsi_update_client_ref_count: client=0x%p updated ref_count=%d",
                      st,
                      st->priv.ref_count);
    }
  }
}

/*===========================================================================
  FUNCTION:  dsi_decrement_client_ref_count
===========================================================================*/
/*!
    @brief
    Decrements the ref_count associated with the client's store handle

    @return
    None
*/
/*=========================================================================*/
static void dsi_decrement_client_ref_count
(
  int  iface,
  wds_ip_family_enum_v01 ip_family
)
{
  boolean      client_found = FALSE;
  dsi_store_t  *st = NULL;
  int          i;


  if (!DSI_IS_ID_VALID(iface))
  {
    DSI_LOG_ERROR("dsi_decrement_client_ref_count: invalid dsi iface=%d",
                  iface);
    return;
  }

  /* Update the ref_count for all valid clients for this iface */
  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    st = dsi_store_table[i].dsi_store_ptr;

    if ((st != NULL && iface == st->priv.dsi_iface_id ) &&
         (((st->priv.ip_version == DSI_IP_VERSION_4) && (ip_family == WDS_IP_FAMILY_IPV4_V01))||
         ((st->priv.ip_version == DSI_IP_VERSION_6) && (ip_family == WDS_IP_FAMILY_IPV6_V01)) ||
         (st->priv.ip_version == DSI_IP_VERSION_4_6) ))
    {
      DSI_LOCK_MUTEX(&(st->priv.mutex));
      if (st->priv.ref_count > 0)
      {
        --st->priv.ref_count;
      }
      else
      {
        DSI_LOG_ERROR("dsi_decrement_client_ref_count: ref_count of client=0x%p is already zero!\n",
                      st);
      }

      DSI_UNLOCK_MUTEX(&(st->priv.mutex));

      DSI_LOG_VERBOSE("dsi_decrement_client_ref_count: client=0x%p ref_count=%d",
                      st,
                      st->priv.ref_count);
    }
  }
}

/*===========================================================================
  FUNCTION:  dsi_handle_pkt_srvc_ind
===========================================================================*/
/*!
    @brief
    handles packet service indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_pkt_srvc_ind
(
  qmi_client_type wds_hndl,
  qmi_service_id_type sid,
  void * user_data,
  dsi_qmi_wds_indication_data_type * ind_data
)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  int i = (int)(intptr_t)user_data;
  int ip_version = DSI_IP_VERSION_4;
  dsi_ce_reason_t ce_reason;
  dsi_ip_family_t ipf = DSI_IP_FAMILY_V4;
  dsi_net_evt_t      event;
  dsi_evt_payload_t  payload;
  int                count;
  dsi_store_t *st = NULL;

  DSI_LOG_VERBOSE("%s", "dsi_handle_pkt_srvc_ind: ENTRY");

  do
  {
    DSI_LOG_DEBUG("received pkt_srvc_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in wds_ind_cb");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in wds_ind_cb", i);
      break;
    }

    reti = DSI_SUCCESS;
    switch(ind_data->pkt_srvc_status.status.connection_status)
    {
    case WDS_CONNECTION_STATUS_DISCONNECTED_V01:
      {
        ip_version = (ind_data->pkt_srvc_status.ip_family == WDS_IP_FAMILY_IPV6_V01)?
                   (DSI_IP_VERSION_6):(DSI_IP_VERSION_4);
        DSI_LOG_DEBUG("packet data call disconnected on iface [%d]", i);

        /* Since the pkt_srvc indication is a broadcast indication, we could get
           a spurious indication when we haven't initiated a call in a multi-PD scenario.
           We need to make sure to increment the ref_count only if the call
           was originated using this instance of the library */
        if (!dsi_is_valid_client_found_for_iface(i,ip_version))
        {
          DSI_LOG_ERROR("received spurious pkt_srvc ind on iface=%d "
                        "without any clients", i);
        }
        else
        {
          /* Client notification happens in dsi_netmgr_post_event() as
           * this ensures kernel interface is down and platform is ready
           * to support next call setup.  Here simply capture the call end
           * reason in local store. */
          DSI_LOCK_MUTEX(DSI_GET_MUTEX(i));

          /*Move the link state to In_Disconnect if a WDS disconnected event is received
          for a IP family associated with this interface. This means that an IP family
          has been disconnected from this interface, though there could still be another
          family up on this interface.*/
          DSI_SET_STATE(i, DSI_IFACE_STATE_IN_DISCONNECT);

          if (ind_data->pkt_srvc_status.ip_family_valid)
          {
            if (WDS_IP_FAMILY_IPV4_V01 == ind_data->pkt_srvc_status.ip_family)
            {
              ipf = DSI_IP_FAMILY_V4;
            }
            else if (WDS_IP_FAMILY_IPV6_V01 == ind_data->pkt_srvc_status.ip_family)
            {
              ipf = DSI_IP_FAMILY_V6;
            }
          }

          dsi_update_store_table
            (
              i,
              ind_data->pkt_srvc_status.verbose_call_end_reason.call_end_reason_type,
              ind_data->pkt_srvc_status.verbose_call_end_reason.call_end_reason,
              ipf
             );

          /* Set the iface ref count to zero for the given family
           * In case of dual-IP call if one of the families goes down
           * We need to ensure that the iface ref count of that family
           * is reset to zero. When partial-retry is kickstarted for
           * that IP family it will incremented correctly from zero */
          if (DSI_IP_VERSION_6 == ip_version)
          {
            DSI_SET_V6_COUNT(i,0);
            DSI_LOG_DEBUG("iface [%d] v6 ref count reset to [%d]",
                          i, dsi_iface_tbl[i].v6_ref_count);
          }
          else if (DSI_IP_VERSION_4 == ip_version)
          {
            DSI_SET_V4_COUNT(i,0);
            DSI_LOG_DEBUG("iface [%d] v4 ref count reset to [%d]",
                          i, dsi_iface_tbl[i].v4_ref_count);
          }

          DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(i));

          /* Decrement the client ref_count for the given ip family */
          dsi_decrement_client_ref_count(i, ind_data->pkt_srvc_status.ip_family);

          DSI_LOG_DEBUG("packet data call disconnected on iface [%d] "
                        "call end reason [%d/%d]", i,
                        ind_data->pkt_srvc_status.verbose_call_end_reason.call_end_reason_type,
                        ind_data->pkt_srvc_status.verbose_call_end_reason.call_end_reason);

          /* we don't notify the user until we get final event from
           * platform (NetMgr) */
        }
        break;
      }

    case WDS_CONNECTION_STATUS_CONNECTED_V01:
      if (ind_data->pkt_srvc_status.status.reconfiguration_required != TRUE)
      {
        ip_version = (ind_data->pkt_srvc_status.ip_family == WDS_IP_FAMILY_IPV6_V01)?
                   (DSI_IP_VERSION_6):(DSI_IP_VERSION_4);
        DSI_LOG_DEBUG("packet data call connected on iface [%d]", i);

        /* Since the pkt_srvc indication is a broadcast indication, we could get
           a spurious indication when we haven't initiated a call in a multi-PD scenario.
           We need to make sure to increment the ref_count only if the call
           was originated using this instance of the library */
        if (!dsi_is_valid_client_found_for_iface(i,ip_version))
        {
          DSI_LOG_ERROR("received spurious pkt_srvc ind on iface=%d "
                        "without any clients", i);
        }
        else
        {
          /* increment ref count and cache the call technology */
          DSI_LOCK_MUTEX(DSI_GET_MUTEX(i));
          DSI_INCR_COUNT(i,ip_version);
          if (ind_data->pkt_srvc_status.tech_name_valid)
          {
            dsi_update_call_tech
              (
                i,
                ind_data->pkt_srvc_status.tech_name,ip_version
              );
          }
          DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(i));

          dsi_update_client_ref_count(i, ind_data->pkt_srvc_status.ip_family);
        }

        memset(&payload, 0, sizeof(payload));
        /* Filling payload with IP type, which is the only
         element valid for DSI_EVT_WDS_CONNECTED event
        */
        payload.ip_type = ipf;

        for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
        {
          event  = DSI_EVT_WDS_CONNECTED;
          st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
          /* Checking IP type also to select right store element in case of
            companion IP call
        */

          if (st != NULL &&
              st->priv.dsi_iface_id == i &&
              ((int) ind_data->pkt_srvc_status.ip_family == st->priv.ip_version ||
               st->priv.ip_version == DSI_IP_VERSION_4_6))
          {
            if (NULL != st->net_ev_cb)
            {
              if(st->priv.call_state == DSI_STATE_CALL_DISCONNECTING)
              {
                DSI_LOG_DEBUG("Call disconnect is in progress on handle [%p] " \
                              "Sending NET_NO_NET evt instead", st);

                DSI_L2S_MSG(st->priv.dsi_iface_id, "posting NET_NO_NET");

                event = DSI_EVT_NET_NO_NET;

                /* detach this interface from the client store handle */
                DSI_LOG_DEBUG( "st_hndl [%p] detached from interface [%d]",
                               st, i );
                DSI_LOCK_MUTEX( &(st->priv.mutex) );
                DSI_DECR_COUNT(i, st, ind_data->pkt_srvc_status.ip_family);
                dsi_detach_dsi_iface( st );
                DSI_UNLOCK_MUTEX( &(st->priv.mutex) );

                /* we determine ce reason from QMI
                 * set reason_code to UNKNOWN here */
                ce_reason.reason_code = DSI_CE_REASON_UNKNOWN;
                ce_reason.reason_type = DSI_CE_TYPE_UNINIT;

              }

              DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                            event, st);
              st->net_ev_cb((dsi_hndl_t *)st,
                            st->user_data,
                            event,
                            &payload  );
            }
            else
            {
              DSI_LOG_ERROR("NULL callback found on store pointer" \
                            "[%p]", st);
            }
          }
        }
      }
      else
      {
        DSI_LOG_DEBUG("packet data call reconfigured on iface [%d]", i);
      }
      /* we don't notify the user until we get final event from
       * platform (NetMgr) except for DSI_EVT_WDS_CONNECTED event*/
      break;
    default:
      DSI_LOG_VERBOSE("not processing pkt_srvc_ind [%d]",
                      ind_data->pkt_srvc_status.status.connection_status);
      reti = DSI_ERROR;
      break;
    }
    if(reti == DSI_ERROR)
    {
      break;
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_pkt_srvc_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_pkt_srvc_ind: EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_tmgi_activate_status_ind
===========================================================================*/
/*!
    @brief
    handles embms tmgi activate status indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_tmgi_activate_status_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                reti         = DSI_SUCCESS;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;

  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_activate_status_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    DSI_LOG_DEBUG("received tmgi_activate_status_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in tmgi_activate_status_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in tmgi_activate_status_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", i);
      break;
    }

    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", i);
      break;
    }

    if(ind_data->embms_activate_status.tranx_id_valid)
    {
      DSI_LOG_DEBUG("activate_status_ind dbg_trace_id:[%d]",
                     ind_data->embms_activate_status.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"TMGI_ACT_IND dbg_trace_id=%d",
                         ind_data->embms_activate_status.tranx_id);
    }

    switch(ind_data->embms_activate_status.activate_status)
    {
      case WDS_TMGI_ACTIVATE_SUCCESS_V01:
        {
          event = DSI_NET_TMGI_ACTIVATED;

          break;
        }
      case WDS_TMGI_ACTIVATE_SUCCESS_DUPLICATE_V01:
      case WDS_TMGI_ACTIVATE_SUCCESS_IDLE_RADIO_TUNE_V01:
      case WDS_TMGI_ACTIVATE_SUCCESS_CONN_RADIO_TUNE_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_UNKNOWN_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_NOT_ALLOWED_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_MISSING_CONTROL_INFO_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_MISSING_TMGI_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_MCAST_OOS_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_UCAST_OOS_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_CAMPED_ON_OTHER_FREQ_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_SAI_MISMATCH_V01:
      case WDS_TMGI_ACTIVATION_FAILURE_MAX_TMGI_ALREADY_ACTIVE_V01:
        {
          event = DSI_NET_TMGI_DEACTIVATED;
          break;
        }
      default:
        {
          DSI_LOG_VERBOSE("unknown activate_status[%d], drop indication",
                           ind_data->embms_activate_status.activate_status);
          reti = DSI_ERROR;
          break;
        }
    }
    if(DSI_SUCCESS == reti)
    {
      memcpy(&payload.embms_info.embms_act_info,
             &ind_data->embms_activate_status,
             sizeof(payload.embms_info.embms_act_info));

      for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
      {
        st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
        if (st != NULL && st->priv.dsi_iface_id == i)
        {
          if (NULL != st->net_ev_cb)
          {
            DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                          event, st);
            st->net_ev_cb( (dsi_hndl_t *)st,
                           st->user_data,
                           event,
                           &payload  );
          }
          else
          {
            DSI_LOG_ERROR("NULL callback found on store pointer" \
                          "[%p]", st);
          }
        }
      }
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"act=%s",
               dsi_get_str_for_id(&embms_act_status_map[0],
                                  ind_data->embms_activate_status.activate_status,
                                  DSI_ARR_SIZE(embms_act_status_map)));
      ret = DSI_SUCCESS;
    }
    else
    {
      break;
    }
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_activate_status_ind: "
                    "EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_activate_status_ind: "
                    "EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_tmgi_deactivate_status_ind
===========================================================================*/
/*!
    @brief
    handles embms tmgi deactivate status indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_tmgi_deactivate_status_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                reti         = DSI_SUCCESS;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;

  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_deactivate_status_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    DSI_LOG_DEBUG("received tmgi_deactivate_status_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in tmgi_dactivate_status_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in tmgi_deactivate_status_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", i);
      break;
    }

    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", i);
      break;
    }

    if(ind_data->embms_deactivate_status.tranx_id_valid)
    {
      DSI_LOG_DEBUG("deactivate_status_ind dbg_trace_id:[%d]",
                     ind_data->embms_deactivate_status.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"TMGI_DEACT_IND dbg_trace_id=%d",
                         ind_data->embms_deactivate_status.tranx_id);
    }

    switch(ind_data->embms_deactivate_status.deactivate_status)
    {
      case WDS_TMGI_DEACTIVATE_SUCCESS_V01:
        {
          event = DSI_NET_TMGI_DEACTIVATED;

          memcpy(&payload.embms_info.embms_deact_info,
                 &ind_data->embms_deactivate_status,
                 sizeof(payload.embms_info.embms_deact_info));

        }
        break;

      default:
        {
          DSI_LOG_ERROR("unknown deactivate_status [%d] drop indication",
                           ind_data->embms_deactivate_status.deactivate_status);
          reti = DSI_ERROR;
        }
        break;
    }/* switch */

    if(DSI_SUCCESS == reti)
    {
      for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
      {
        st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
        if (st != NULL && st->priv.dsi_iface_id == i)
        {
          if (NULL != st->net_ev_cb)
          {
            DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                          event, st);
            st->net_ev_cb( (dsi_hndl_t *)st,
                           st->user_data,
                           event,
                           &payload  );
          }
          else
          {
            DSI_LOG_ERROR("NULL callback found on store pointer" \
                          "[%p]", st);
          }
        }
      }
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE, "deact=%s",
            dsi_get_str_for_id(&embms_deact_status_map[0],
                               ind_data->embms_deactivate_status.deactivate_status,
                               DSI_ARR_SIZE(embms_deact_status_map)));
      ret = DSI_SUCCESS;
    }
    else
    {
      break;
    }

  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_deactivate_status_ind: "
                    "EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_deactivate_status_ind: "
                    "EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_tmgi_act_deactivate_status_ind
===========================================================================*/
/*!
    @brief
    handles embms tmgi activate_deactivate status indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_tmgi_act_deactivate_status_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                reti         = DSI_SUCCESS;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;

  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_act_deactivate_status_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    DSI_LOG_DEBUG("received tmgi_act_deactivate_status_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in tmgi_act_deactivate_status_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in tmgi_act_deactivate_status_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", i);
      break;
    }

    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", i);
      break;
    }

    if(ind_data->embms_act_deact_status.tranx_id_valid)
    {
      DSI_LOG_DEBUG("act_deact_status_ind dbg_trace_id:[%d]",
                     ind_data->embms_act_deact_status.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"TMGI_ACT_DEACT_IND dbg_trace_id=%d",
                         ind_data->embms_act_deact_status.tranx_id);
    }

    switch(ind_data->embms_act_deact_status.act_status)
    {
      case WDS_TMGI_ACTIVATE_SUCCESS_V01:
      case WDS_TMGI_ACTIVATE_SUCCESS_DUPLICATE_V01:
      case WDS_TMGI_ACTIVATE_SUCCESS_IDLE_RADIO_TUNE_V01:
      case WDS_TMGI_ACTIVATE_SUCCESS_CONN_RADIO_TUNE_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_UNKNOWN_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_NOT_ALLOWED_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_MISSING_CONTROL_INFO_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_MISSING_TMGI_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_MCAST_OOS_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_UCAST_OOS_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_CAMPED_ON_OTHER_FREQ_V01:
      case WDS_TMGI_ACTIVATE_FAILURE_SAI_MISMATCH_V01:
      case WDS_TMGI_ACTIVATION_FAILURE_MAX_TMGI_ALREADY_ACTIVE_V01:
        break;
      default:
        reti = DSI_ERROR;
        break;
    }
    switch(ind_data->embms_act_deact_status.deact_status)
    {
      case WDS_TMGI_DEACTIVATE_SUCCESS_V01:
        break;
      default:
        reti = DSI_ERROR;
        break;
    }

    if(DSI_SUCCESS != reti)
    {
      DSI_LOG_ERROR("unknown act_deact_status act:[%d] deact:[%d] drop indication",
                    ind_data->embms_act_deact_status.act_status,
                    ind_data->embms_act_deact_status.deact_status);
      break;
    }

    event = DSI_NET_TMGI_ACTIVATED_DEACTIVATED;

    memcpy(&payload.embms_info.embms_act_deact_info,
           &ind_data->embms_act_deact_status,
           sizeof(payload.embms_info.embms_act_deact_info));


    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        if (NULL != st->net_ev_cb)
        {
          DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                        event, st);
          st->net_ev_cb( (dsi_hndl_t *)st,
                         st->user_data,
                         event,
                         &payload  );
        }
        else
        {
          DSI_LOG_ERROR("NULL callback found on store pointer" \
                        "[%p]", st);
        }
      }
      ret = DSI_SUCCESS;
    }
    DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE, "act=%s\\ndeact=%s",
        dsi_get_str_for_id(&embms_act_status_map[0],
                           ind_data->embms_act_deact_status.act_status,
                           DSI_ARR_SIZE(embms_act_status_map)),
        dsi_get_str_for_id(&embms_deact_status_map[0],
                           ind_data->embms_act_deact_status.deact_status,
                           DSI_ARR_SIZE(embms_deact_status_map)));

  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_act_deactivate_status_ind: "
                    "EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_act_deactivate_status_ind: "
                    "EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_tmgi_list_ind
===========================================================================*/
/*!
    @brief
    handles embms tmgi list indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_tmgi_list_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                reti         = DSI_SUCCESS;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;

  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_list_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    DSI_LOG_DEBUG("received tmgi_list_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in tmgi_list_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in tmgi_list_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", count);
      break;
    }
    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", count);
      break;
    }

    if(ind_data->embms_list.tranx_id_valid)
    {
      DSI_LOG_DEBUG("tmgi_list_ind dbg_trace_id:[%d]",
                     ind_data->embms_list.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"TMGI_LIST_IND dbg_trace_id=%d",
                         ind_data->embms_list.tranx_id);
    }

    if(ind_data->embms_list.tmgi_list_valid)
    {
      event = DSI_NET_TMGI_LIST_CHANGED;

      DSI_LOG_DEBUG("received tmgi_list_ind, list ind type [%d], tranx_id %d[%d]",
                    ind_data->embms_list.tmgi_list.list_type,
                    ind_data->embms_list.tranx_id_valid,
                    ind_data->embms_list.tranx_id);

      memcpy(&payload.embms_info.embms_list_info,
             &ind_data->embms_list,
             sizeof(payload.embms_info.embms_list_info));

      for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
      {
        st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
        if (st != NULL && st->priv.dsi_iface_id == i)
        {
          if (NULL != st->net_ev_cb)
          {
            DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                          event, st);
            st->net_ev_cb( (dsi_hndl_t *)st,
                           st->user_data,
                           event,
                           &payload  );
          }
          else
          {
            DSI_LOG_ERROR("NULL callback found on store pointer" \
                          "[%p]", st);
          }
        }
      }
    }
    else
    {
      DSI_LOG_DEBUG("%s", "received tmgi_list_ind with no valid tmgi list data");
    }

    if(reti == DSI_ERROR)
    {
      break;
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_list_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_tmgi_list_ind: EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_sai_list_ind
===========================================================================*/
/*!
    @brief
    handles embms sai list indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_sai_list_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;
  int temp_index;
  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_sai_list_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    DSI_LOG_DEBUG("received sai_list_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in sai_list_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in sai_list_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", count);
      break;
    }
    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", count);
      break;
    }

    if(ind_data->sai_list.tranx_id_valid)
    {
      DSI_LOG_DEBUG("sai_ind dbg_trace_id:[%d]",
                     ind_data->sai_list.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"SAI_LIST_IND dbg_trace_id=%d",
                         ind_data->sai_list.tranx_id);
    }

    if(!ind_data->sai_list.freq_sai_list_valid)
    {
      DSI_LOG_ERROR("%s", "Received SAI list ind with no sai list data ");
      break;
    }

    memcpy(&payload.embms_info.embms_sai_info,
           &(ind_data->sai_list),
           sizeof(payload.embms_info.embms_sai_info));

    event = DSI_NET_SAI_LIST_CHANGED;

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        if (NULL != st->net_ev_cb)
        {
          DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                        event, st);
          st->net_ev_cb( (dsi_hndl_t *)st,
                        st->user_data,
                        event,
                        &payload  );
        }
        else
        {
          DSI_LOG_ERROR("NULL callback found on store pointer" \
                        "[%p]", st);
        }
      }
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_sai_list_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_sai_list_ind: EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_content_desc_control_ind
===========================================================================*/
/*!
    @brief
    handles embms content desc control indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_content_desc_control_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;
  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_content_desc_control_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    event = DSI_NET_CONTENT_DESC_CONTROL;

    DSI_LOG_DEBUG("received content_desc_control_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in content_desc_control_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in content_desc_control_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", count);
      break;
    }
    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", count);
      break;
    }

    if(ind_data->embms_content_desc_control.tranx_id_valid)
    {
      DSI_LOG_DEBUG("embms_content_desc_ind dbg_trace_id:[%d]",
                     ind_data->embms_content_desc_control.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"CONTENT_DESC_IND dbg_trace_id=%d",
                         ind_data->embms_content_desc_control.tranx_id);
    }

    memcpy(&payload.embms_info.embms_content_desc_info,
           &ind_data->embms_content_desc_control,
           sizeof(payload.embms_info.embms_content_desc_info));

    DSI_LOG_DEBUG("received content_desc_control_ind, TMGI [%X,%X,%X,%X,%X,%X] ",
      payload.embms_info.embms_content_desc_info.tmgi.tmgi[0],
      payload.embms_info.embms_content_desc_info.tmgi.tmgi[1],
      payload.embms_info.embms_content_desc_info.tmgi.tmgi[2],
      payload.embms_info.embms_content_desc_info.tmgi.tmgi[3],
      payload.embms_info.embms_content_desc_info.tmgi.tmgi[4],
      payload.embms_info.embms_content_desc_info.tmgi.tmgi[5]);

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        if (NULL != st->net_ev_cb)
        {
          DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                        event, st);
          st->net_ev_cb( (dsi_hndl_t *)st,
                         st->user_data,
                         event,
                         &payload  );
        }
        else
        {
          DSI_LOG_ERROR("NULL callback found on store pointer" \
                        "[%p]", st);
        }
      }
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_content_desc_control_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_content_desc_control_ind: EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_embms_svc_interest_ind
===========================================================================*/
/*!
    @brief
    handles embms service interest indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_embms_svc_interest_ind
(
  qmi_client_type                wds_hndl,
  qmi_service_id_type            sid,
  void                          *user_data,
  dsi_qmi_wds_indication_data_type  *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t       *st           = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  boolean            handle_found = FALSE;
  DSI_LOG_VERBOSE("%s", "dsi_handle_embms_svc_interest_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    event = DSI_NET_TMGI_SERVICE_INTERESTED;

    DSI_LOG_DEBUG("received embms_svc_interest_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind data received in embms_svc_interest_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "\
                    "%d received in embms_svc_interest_ind", i);
      break;
    }

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        handle_found = TRUE;
        break;
      }
    }
    if(FALSE == handle_found)
    {
      DSI_LOG_ERROR("No call state for index [%d]", count);
      break;
    }
    if( NULL == st->net_ev_cb )
    {
      DSI_LOG_ERROR("No event callback for index [%d]", count);
      break;
    }

    if(ind_data->embms_svc_interest.tranx_id_valid)
    {
      DSI_LOG_DEBUG("embms_svc_interest_ind dbg_trace_id:[%d]",
                     ind_data->embms_svc_interest.tranx_id);
      DSI_L2S_FORMAT_MSG(DSI_L2S_INVALID_IFACE,"CONTENT_DESC_IND dbg_trace_id=%d",
                         ind_data->embms_svc_interest.tranx_id);
    }

    memcpy(&payload.embms_info.embms_svc_interest_info,
           &ind_data->embms_svc_interest,
           sizeof(payload.embms_info.embms_svc_interest_info));

    for(count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *)dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        if (NULL != st->net_ev_cb)
        {
          DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                        event, st);
          st->net_ev_cb( (dsi_hndl_t *)st,
                         st->user_data,
                         event,
                         &payload  );
        }
        else
        {
          DSI_LOG_ERROR("NULL callback found on store pointer" \
                        "[%p]", st);
        }
      }
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_svc_interest_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_embms_svc_interest_ind: EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_handle_handoff_ind
===========================================================================*/
/*!
    @brief
    Handles WDS handoff indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
static int dsi_handle_handoff_ind
(
  qmi_client_type                   wds_hndl,
  qmi_service_id_type               sid,
  void                             *user_data,
  dsi_qmi_wds_indication_data_type *ind_data
)
{
  int                ret          = DSI_ERROR;
  int                i            = (int)(intptr_t)user_data;
  dsi_net_evt_t      event;
  dsi_store_t        *st          = NULL;
  dsi_evt_payload_t  payload;
  int                count;
  qmi_client_type    qmi_wds_hndl_v4, qmi_wds_hndl_v6;

  DSI_LOG_VERBOSE("%s", "dsi_handle_handoff_ind: ENTRY");

  memset(&payload, 0, sizeof(payload));

  do
  {
    DSI_LOG_DEBUG("received handoff_ind with wds_hndl [%p] "
                  "sid [%d] user_data [%p] ind_data [%p]",
                  wds_hndl, sid, user_data, ind_data);

    if (NULL == ind_data)
    {
      DSI_LOG_ERROR("%s", "NULL ind_data received handoff_ind");
      break;
    }

    if (!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("invalid user data (dsi iface id) "
                    "%d received in handoff_ind", i);
      break;
    }

    event = DSI_EVT_NET_HANDOFF;
    payload.handoff_info.handoff_type = ind_data->handoff_info.handoff_information;
    if(ind_data->handoff_info.handoff_event_info_valid)
    {
      payload.handoff_info.source_rat = ind_data->handoff_info.handoff_event_info.srat;
      payload.handoff_info.target_rat = ind_data->handoff_info.handoff_event_info.trat;
    }

    /* Determine the IP family for which the handoff was received */
    qmi_wds_hndl_v4 = qdi_get_qmi_wds_handle_for_ip(DSI_GET_QDI_HNDL(i),
                                                    WDS_IP_FAMILY_PREF_IPV4_V01);
    qmi_wds_hndl_v6 = qdi_get_qmi_wds_handle_for_ip(DSI_GET_QDI_HNDL(i),
                                                    WDS_IP_FAMILY_PREF_IPV6_V01);
    if (wds_hndl == qmi_wds_hndl_v4)
    {
      if ( 0 == dsi_iface_tbl[i].v4_ref_count)
      {
        DSI_LOG_ERROR("%s: V4 WDS_HANDOFF_IND got on handle with no V4 call. v4_ref_cnt[%d]",
                      __func__, dsi_iface_tbl[i].v4_ref_count);
        return ret;
      }
      DSI_LOG_DEBUG("%s:Finding dsi_hndl for V4 HANDOFF_IND", __func__ );
      payload.handoff_info.ip_type = WDS_IP_FAMILY_PREF_IPV4_V01;
    }
    else if (wds_hndl == qmi_wds_hndl_v6 )
    {
      if ( 0 == dsi_iface_tbl[i].v6_ref_count)
      {
        DSI_LOG_ERROR("%s: V6 WDS_HANDOFF_IND got on handle with no V6 call. v6_ref_cnt[%d]",
                      __func__, dsi_iface_tbl[i].v6_ref_count);
        return ret;
      }
      DSI_LOG_DEBUG("%s:Finding dsi_hndl for V6 HANDOFF_IND", __func__ );
      payload.handoff_info.ip_type = WDS_IP_FAMILY_PREF_IPV6_V01;
    }

    for (count = 0; count < DSI_MAX_DATA_CALLS; count++)
    {
      st = (dsi_store_t *) dsi_store_table[count].dsi_store_ptr;
      if (st != NULL && st->priv.dsi_iface_id == i)
      {
        if (NULL == st->net_ev_cb)
        {
          DSI_LOG_ERROR("No event callback for index [%d]", i);

          /* Continue with the loop as multiple clients may have
           * registered for handoff events */
          continue;
        }

        /* Post event on callback */
        DSI_LOG_DEBUG("notifying event [%d] on handle [%p]",
                      event, st);
        st->net_ev_cb((dsi_hndl_t * )st,
                      st->user_data,
                      event,
                      &payload );
      }
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_handoff_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_handle_handoff_ind: EXIT with err");
  }
  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_process_wds_ind
===========================================================================*/
/*!
    @brief
    callback function registered for wds indications

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
void dsi_process_wds_ind
(
  qmi_client_type wds_hndl,
  qmi_service_id_type sid,
  void * user_data,
  int   ind_id,
  dsi_qmi_wds_indication_data_type * ind_data
)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;

  DSI_LOG_VERBOSE("%s", "dsi_process_wds_ind: ENTRY");

  DSI_GLOBAL_LOCK;

  do
  {
    DSI_LOG_VERBOSE("received ind_id [%d] with wds_hndl [%p] "
                    "sid [%d] user_data [%p] ind_data [%p]",
                    ind_id, wds_hndl, sid, user_data, ind_data);

    reti = DSI_SUCCESS;
    switch(ind_id)
    {
    case QMI_WDS_PKT_SRVC_STATUS_IND_V01:
      reti = dsi_handle_pkt_srvc_ind(wds_hndl,
                                     sid,
                                     user_data,
                                     ind_data);
      break;
    case QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01:
      reti = dsi_handle_embms_tmgi_activate_status_ind(wds_hndl,
                                                       sid,
                                                       user_data,
                                                       ind_data);
      break;
    case QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01:
      reti = dsi_handle_embms_tmgi_deactivate_status_ind(wds_hndl,
                                                         sid,
                                                         user_data,
                                                         ind_data);
      break;
    case QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01:
      reti = dsi_handle_embms_tmgi_act_deactivate_status_ind(wds_hndl,
                                                             sid,
                                                             user_data,
                                                             ind_data);
      break;
    case QMI_WDS_EMBMS_TMGI_LIST_IND_V01:
      reti = dsi_handle_embms_tmgi_list_ind(wds_hndl,
                                            sid,
                                            user_data,
                                            ind_data);
      break;
    case QMI_WDS_EMBMS_SAI_LIST_IND_V01:
      reti = dsi_handle_embms_sai_list_ind(wds_hndl,
                                           sid,
                                           user_data,
                                           ind_data);
      break;
    case QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01:
      reti = dsi_handle_embms_content_desc_control_ind(wds_hndl,
                                                       sid,
                                                       user_data,
                                                       ind_data);
      break;
    case QMI_WDS_EMBMS_SVC_INTEREST_IND_V01:
      reti = dsi_handle_embms_svc_interest_ind(wds_hndl,
                                               sid,
                                               user_data,
                                               ind_data);
      break;
    case QMI_WDS_HANDOFF_INFORMATION_IND_V01:
      reti = dsi_handle_handoff_ind(wds_hndl,
                                    sid,
                                    user_data,
                                    ind_data);
      break;

    default:
      DSI_LOG_VERBOSE("dsi_process_wds_ind: IGNORE: do not handle [%d]"
                      " indication as of now", ind_id);
      /* this is not an error, we just ignore this indication*/
      break;
    }
    if (DSI_ERROR == reti)
    {
      break;
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_process_wds_ind: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_process_wds_ind: EXIT with err");
  }

  DSI_GLOBAL_UNLOCK;
}

/*===========================================================================
  FUNCTION:  dsi_process_qmi_sys_ind
===========================================================================*/
/*!
    @brief
    Processes QMI systerm indications in cb thrd context

    @return
    none
*/
/*=========================================================================*/
void dsi_process_qmi_sys_ind
(
  dsi_qmi_sys_event_type event_id,
  void *user_data
)
{
  int iface_id;
  const char* dev_id;
  ds_target_t target = ds_get_target();

  dev_id = (char *)user_data;
  if (DS_TARGET_LE_MDM9X35 == target
      || DS_TARGET_LE_MDM9X25 == target
      || DS_TARGET_LE_MDM9X15 == target
      || DS_TARGET_LE_LEGACY == target)
  {
    DSI_LOG_DEBUG("Ignore SSR event on phys port: %s", dev_id);
    return;
  }

  switch(event_id)
  {
  case DSI_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND:
    DSI_LOG_DEBUG("%s","dsi_process_qmi_sys_ind: rcvd DSI_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND");
    DSI_L2S_MSG(DSI_L2S_INVALID_IFACE, "MDM_OOS_IND rcvd");
    for(iface_id = 0; iface_id < DSI_MAX_IFACES; ++iface_id )
    {
      DSI_LOG_DEBUG("dsi_process_qmi_sys_ind: calling dsi_modem_out_of_service_hdlr"
                    "for iface_id[%d]",iface_id);
      dsi_modem_out_of_service_hdlr(iface_id);
    }
    DSI_L2S_MSG(DSI_L2S_INVALID_IFACE,"MDM_OOS processed\\nfor all ifaces");
    break;
  case DSI_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND:
    DSI_LOG_DEBUG("%s","dsi_process_qmi_sys_ind: rcvd DSI_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND");
    DSI_L2S_MSG(DSI_L2S_INVALID_IFACE, "MDM_IS_IND rcvd");
    for(iface_id = 0; iface_id < DSI_MAX_IFACES; ++iface_id)
    {
      DSI_LOG_DEBUG("dsi_process_qmi_sys_ind: calling dsi_modem_in_service_hdlr"
                    "for iface_id[%d]",iface_id);
      dsi_modem_in_service_hdlr(iface_id);
    }
    DSI_L2S_MSG(DSI_L2S_INVALID_IFACE,"MDM_IS processed\\nfor all ifaces");
    break;
  default:
    DSI_LOG_DEBUG("do not handle: [%d] sys ind rcvd", event_id);
    break;
  }
}
