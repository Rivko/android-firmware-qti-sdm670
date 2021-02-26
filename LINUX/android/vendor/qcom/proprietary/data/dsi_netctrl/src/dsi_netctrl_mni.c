/*!
  @file
  dsi_netctrl_mni.c

  @brief
  implements dsi_netctrl modem network interface functions

*/

/*===========================================================================

  Copyright (c) 2008-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <string.h>
#include <stdlib.h>
#include "dsi_netctrli.h"
#include "dsi_netctrl_qmi_wds.h"
#include "qmi_nas_srvc.h"

#include "dsi_netctrl_cb_thrd.h"
#include "dsi_netctrl_multimodem.h"
#include "qmi_nas_srvc.h"
#include "dsi_netctrl_qmi_wds.h"
#include "dsi_netctrl_qmi_qos.h"

#include "qdi.h"
#include <stdint.h>
#include <linux/if.h>
#include <linux/rmnet_data.h>
#include <linux/msm_rmnet.h>

/* Default EPIDs for different transports  */
#define DSI_DEFAULT_EPID_MHI 4
#define DSI_DEFAULT_EPID_IPA 1

/*===========================================================================
  GLOBAL
===========================================================================*/

/*===========================================================================
  FUNCTION:  dsi_mni_start
===========================================================================*/
/*!
    @brief
    starts modem network interface
    i is dsi iface id
    st_hndl is store handle
    ce is a place holder for call end reason (OUT)

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_start
(
  int i,
  dsi_store_t * st_hndl
)
{
  int qmi_ret, qmi_err_code;
  dsi_wds_call_end_reason_type call_end_reason;
  int ret = DSI_ERROR;
  int ip_family = QDI_MODE_NOT_SET;

  DSI_LOG_DEBUG( "%s", "dsi_mni_start: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    memset( &call_end_reason, 0x0, sizeof(call_end_reason) );

    /* Allocate a QDI call handle if it doesn't exist */
    if (QDI_INVALID_CALL_HANDLE == st_hndl->priv.qdi_call_hndl)
    {
      st_hndl->priv.qdi_call_hndl = qdi_alloc_call_handle(DSI_GET_QDI_HNDL(i));

      /* If the allocation failed, return failure */
      if (QDI_INVALID_CALL_HANDLE == st_hndl->priv.qdi_call_hndl)
      {
        DSI_LOG_ERROR("Failed to allocate QDI call handle for iface=%d",
                      i);
        break;
      }
    }

    /* reset txn ids */
    DSI_SET_WDS_V4_TXN(st_hndl, DSI_INVALID_WDS_TXN);
    DSI_SET_WDS_V6_TXN(st_hndl, DSI_INVALID_WDS_TXN);

    /* reset call tech in the store store table. If the call was successful
       and packet service indication is later received with call connected
       status, a fresh call tech will be stored in the store table.
    */
    st_hndl->priv.call_tech = WDS_TECHNOLOGY_NAME_ENUM_MAX_ENUM_VAL_V01;

    qmi_ret = qdi_wds_start_nw_if(DSI_GET_QDI_HNDL(i),
                                  st_hndl->priv.qdi_call_hndl,
                                  &(st_hndl->priv.start_nw_params),
                                  st_hndl->priv.partial_retry,
                                  dsi_qmi_wds_cmd_cb,
                                  (void *)st_hndl,
                                  st_hndl->priv.rl_qmi_inst,
                                  &call_end_reason,
                                  &qmi_err_code,
                                  &ip_family);

    if(qmi_ret < 0)
    {
      dsi_ip_family_t ipf;

      /* For a Dual-IP call update both V4 and V6 call end reasons */
      if (DSI_IP_VERSION_4_6 == st_hndl->priv.ip_version)
      {
        for (ipf = DSI_IP_FAMILY_V4; ipf < DSI_NUM_IP_FAMILIES; ++ipf)
        {
          st_hndl->priv.ce_reason[ipf].reason_code =
            call_end_reason.call_end_reason_verbose.verbose_reason;
          st_hndl->priv.ce_reason[ipf].reason_type =
            dsi_map_qmi_to_dsi_ce_reason_type(
            call_end_reason.call_end_reason_verbose.verbose_reason_type);
        }

        DSI_LOG_ERROR("qdi_wds_start_nw_if failed on [%d] iface with"     \
                      "[%d] qmi err, dsi reason code/type [%d/%d], IPv4v6 call",
                      i,qmi_err_code,
                      st_hndl->priv.ce_reason[DSI_IP_FAMILY_V4].reason_code,
                      st_hndl->priv.ce_reason[DSI_IP_FAMILY_V4].reason_type);
      }
      else
      {
        ipf = (DSI_IP_VERSION_6 == st_hndl->priv.ip_version) ?
              DSI_IP_FAMILY_V6 : DSI_IP_FAMILY_V4;

        st_hndl->priv.ce_reason[ipf].reason_code =
          call_end_reason.call_end_reason_verbose.verbose_reason;
        st_hndl->priv.ce_reason[ipf].reason_type =
          dsi_map_qmi_to_dsi_ce_reason_type(
          call_end_reason.call_end_reason_verbose.verbose_reason_type);

        DSI_LOG_ERROR("qdi_wds_start_nw_if failed on [%d] iface with"     \
                      "[%d] qmi err, dsi reason code/type [%d/%d], ipf [%d]",
                      i,qmi_err_code,
                      st_hndl->priv.ce_reason[ipf].reason_code,
                      st_hndl->priv.ce_reason[ipf].reason_type,
                      ipf);
      }

      break;
    }

    /*
       store wds txn id for later abort if needed
       by virtue of the asynchronouse execution, the wds txn
       id and ref count by client are mutually exclusive.
       i.e. a client would either have a pending start wds
       txn, but when start is successfully executed, wds txn
       is finished, and client would acquire a ref count
    */
    DSI_LOCK_MUTEX(DSI_GET_MUTEX(i));
    if(st_hndl->priv.ip_version == DSI_IP_VERSION_4_6)
    {
      if(ip_family == QDI_MODE_V4V6)
      {
        DSI_SET_WDS_V4_TXN(st_hndl, qmi_ret);
        DSI_SET_WDS_V6_TXN(st_hndl, qmi_ret);
        DSI_LOG_DEBUG("wds-txns_dual = %d", qmi_ret);
      }
      else if(ip_family == QDI_MODE_V4)
      {
        DSI_SET_WDS_V4_TXN(st_hndl, qmi_ret);
        DSI_LOG_DEBUG("%s","wds-txn-v4");
      }
      else if(ip_family == QDI_MODE_V6)
      {
        DSI_SET_WDS_V6_TXN(st_hndl, qmi_ret);
        DSI_LOG_DEBUG("%s","wds-txn-v6");
      }
    }
    else if(st_hndl->priv.ip_version == DSI_IP_VERSION_4)
    {
      DSI_SET_WDS_V4_TXN(st_hndl, qmi_ret);
    }
    else if(st_hndl->priv.ip_version == DSI_IP_VERSION_6)
    {
      DSI_SET_WDS_V6_TXN(st_hndl, qmi_ret);
    }
    DSI_UNLOCK_MUTEX(DSI_GET_MUTEX(i));

    ret = DSI_SUCCESS;
  } while (0);


  if (ret == DSI_SUCCESS)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_start: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_start: EXIT with err" );
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_stop
===========================================================================*/
/*!
    @brief
    stops modem network interface
    i is dsi iface id
    st_hndl is the store handle pointer

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_stop(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;
  int qmi_ret, qmi_err_code;
  qdi_mode_t stop_ip_mode = QDI_MODE_NOT_SET;

  DSI_LOG_DEBUG( "%s", "dsi_mni_stop: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (NULL == st_hndl || !DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("%s", "invalid params received");
      break;
    }

    if(DSI_IP_VERSION_4_6 == st_hndl->priv.ip_version )
    {
      if(0 == DSI_GET_V4_COUNT(i) &&
         0 == DSI_GET_V6_COUNT(i))
      {
        stop_ip_mode = QDI_MODE_V4V6;
      }
      else if ( 0 == DSI_GET_V4_COUNT(i))
      {
        stop_ip_mode = QDI_MODE_V4;
      }
      else if ( 0 == DSI_GET_V6_COUNT(i))
      {
        stop_ip_mode = QDI_MODE_V6;
      }
    }

    qmi_ret = qdi_wds_stop_nw_if(DSI_GET_QDI_HNDL(i),
                                 st_hndl->priv.qdi_call_hndl,
                                 dsi_qmi_wds_cmd_cb,
                                 stop_ip_mode,
                                 (void *)st_hndl,
                                 &qmi_err_code);
    if( 0 > qmi_ret )
    {
      DSI_LOG_ERROR("qmi stop nw if failed: iface=[%d] err=[%d]",
                    i,qmi_err_code);
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_stop: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_stop: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_abort_start
===========================================================================*/
/*!
    @brief
    aborts previous start request
    i is dsi iface id
    st_hndl is the store handle pointer

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_abort_start(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;
  int qmi_ret = QMI_INTERNAL_ERR,qmi_ret6 = QMI_INTERNAL_ERR;
  int qmi_err_code = QMI_NO_ERR, qmi_err6_code = QMI_NO_ERR;

  DSI_LOG_DEBUG( "%s", "dsi_mni_abort_start: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (NULL == st_hndl || !DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("%s", "invalid params received");
      break;
    }

    switch(st_hndl->priv.ip_version)
    {
      case DSI_IP_VERSION_4:
              qmi_ret = qdi_wds_abort(DSI_GET_QDI_HNDL(i),
                                       st_hndl->priv.qdi_call_hndl,
                                       DSI_GET_WDS_V4_TXN(st_hndl),
                                       &qmi_err_code );
      break;
      case DSI_IP_VERSION_6:
              qmi_ret = qdi_wds_abort(DSI_GET_QDI_HNDL(i),
                                       st_hndl->priv.qdi_call_hndl,
                                       DSI_GET_WDS_V6_TXN(st_hndl),
                                       &qmi_err_code );
      break;
      case DSI_IP_VERSION_4_6:
              qmi_ret = qdi_wds_abort(DSI_GET_QDI_HNDL(i),
                                       st_hndl->priv.qdi_call_hndl,
                                       DSI_GET_WDS_V4_TXN(st_hndl),
                                       &qmi_err_code );
              qmi_ret6 = qdi_wds_abort(DSI_GET_QDI_HNDL(i),
                                       st_hndl->priv.qdi_call_hndl,
                                       DSI_GET_WDS_V6_TXN(st_hndl),
                                       &qmi_err6_code );
              qmi_ret = qmi_ret | qmi_ret6;
      break;
    }

    /* check if txn was completed successfully */
    if(QMI_NO_ERR == qmi_ret)
    {
      /* For a Dual-IP call with one valid address, wait for the NET_PLATFORM_DOWN event */
      if (DSI_IP_VERSION_4_6 == st_hndl->priv.ip_version &&
          DSI_IS_ID_VALID(st_hndl->priv.dsi_iface_id) &&
          (DSI_IS_ADDR_VALID(st_hndl->priv.dsi_iface_id, DSI_IP_FAMILY_V4, iface_addr) ||
           DSI_IS_ADDR_VALID(st_hndl->priv.dsi_iface_id, DSI_IP_FAMILY_V6, iface_addr)))
      {
        DSI_LOG_DEBUG("%s", "successfully aborted START (Dual-IP), waiting for NET_PLATFORM_DOWN");
      }
      else
      {
        DSI_LOG_DEBUG("%s", "successfully aborted START, sending NET_NO_NET");

        /* detach interface from the call store handle */
        if(DSI_SUCCESS != dsi_link_release(st_hndl->priv.dsi_iface_id))
        {
          DSI_LOG_ERROR("dsi_link_release failed: iface=[%d] " ,
                          st_hndl->priv.dsi_iface_id);
        }
        DSI_LOCK_MUTEX(&(st_hndl->priv.mutex));
        dsi_detach_dsi_iface(st_hndl);
        DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex));
        /* notify user that net is down */
        if (NULL != st_hndl->net_ev_cb)
        {
          st_hndl->net_ev_cb((dsi_store_t *)st_hndl,
                             st_hndl->user_data,
                             DSI_EVT_NET_NO_NET,
                             NULL);
        }
        else
        {
          DSI_LOG_ERROR("no callback set on st pointer [%p]", st_hndl);
        }
      }
    }
    else
    {
      DSI_LOG_ERROR("qmi abort failed: iface=[%d] qmi_ret=[%d]" \
                    "qmi_err_code=[%d]",i,qmi_ret,qmi_err_code);
      DSI_LOG_DEBUG("dsi_netctrl will try to stop the " \
                    "interface [%d] now", i);
      /* user had tried to stop the interface, hence
      * if abort failed above, try to stop it now */
      if (DSI_SUCCESS != dsi_mni_stop(i, st_hndl))
      {
        DSI_LOG_DEBUG("couldn't stop interface [%d]",i);
        break;
      }
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_SUCCESS)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_abort_start: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_abort_start: EXIT with err" );
  }

  return ret;
}


/*===========================================================================
  FUNCTION:  dsi_mni_look_up
===========================================================================*/
/*!
    @brief
    returns dsi_iface_id that matches
    with the parameters set on the store handle
    st_hndl is the store handle pointer
    pi is a place holder for dsi iface id (OUT)

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_look_up(dsi_store_t * st_hndl, int * pi)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  int technology = DSI_RADIO_TECH_UNKNOWN;
  int i = DSI_INVALID_IFACE;
  int route_look_up_modem;
  boolean iface_found = FALSE;
  int dsd_technology = DSI_RADIO_TECH_UNKNOWN;

  DSI_LOG_DEBUG( "%s", "dsi_mni_look_up: ENTRY" );

  if (NULL == pi || NULL == st_hndl)
  {
    DSI_LOG_ERROR("%s","invalid parameters");
    return DSI_ERROR;
  }
  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    iface_found = FALSE;
    /* map qmi tech to dsi tech if one provided */
    if (st_hndl->priv.start_nw_params.technology_preference_valid)
    {
      DSI_LOG_DEBUG("start_nw_params has tech_pref set to %d",
                    st_hndl->priv.start_nw_params.technology_preference);
      technology = dsi_map_qmi_to_dsi_tech(st_hndl->priv.
                                           start_nw_params.technology_preference);
      if (-1 == technology)
      {
        DSI_LOG_ERROR("qmi tech [%d] is currently not supported"
                      " by dsi_netctrl",
                      st_hndl->priv.start_nw_params.technology_preference);
        break;
      }
    }
    else if(DSI_RADIO_TECH_UNKNOWN != st_hndl->dsi_tech)
    {
      DSI_LOG_DEBUG("st_hndl has dsi_tech set to [%d]", st_hndl->dsi_tech);
      technology = st_hndl->dsi_tech;
    }

    DSI_LOG_VERBOSE("%s","QCRIL is using QDP. No need to create IPv6 profiles");

    /* perform multi-modem route look up*/
    if (DSI_ERROR ==
        dsi_mm_iface_look_up(st_hndl,
                             &i,
                             &route_look_up_modem,
                             &dsd_technology))
    {
      DSI_LOG_ERROR("%s","dsi_mm_iface_look_up failed");
      break;
    }

    /* make sure multi-modem route look up technology
     * matches with user preferred technology */
    reti = DSI_SUCCESS;
    switch(technology)
    {
    case DSI_RADIO_TECH_UNKNOWN:
      DSI_LOG_DEBUG("user did not provide a technology preference " \
                    "by default dsd tech [0x%x] would be used",
                    dsd_technology);
      break;
    case DSI_RADIO_TECH_UMTS:
      /* EPC(if handoff is supported) or UMTS will be returned from MODEM */
      if (!(dsd_technology == DSI_PS_UMTS_IFACE ||
            dsd_technology == DSI_PS_EPC_IFACE))
      {
        DSI_LOG_ERROR("DSD preferred technology [0x%x] " \
                      "does not match with user preferred " \
                      "DSI_RADIO_TECH_UMTS", dsd_technology);
        reti = DSI_ERROR;
      }
      break;
    case DSI_RADIO_TECH_CDMA:
      /* EPC(if handoff is supported) or CDMA will be returned from MODEM */
      if (!(dsd_technology == DSI_PS_CDMA_SN_IFACE ||
            dsd_technology == DSI_PS_FMC_IFACE     ||
            dsd_technology == DSI_PS_EPC_IFACE))
      {
        DSI_LOG_ERROR("DSD preferred technology [0x%x] " \
                      "does not match with user preferred " \
                      "DSI_RADIO_TECH_CDMA", dsd_technology);
        reti = DSI_ERROR;
      }
      break;
    case DSI_RADIO_TECH_1X:
      if (!(
            (dsd_technology == DSI_PS_CDMA_SN_IFACE && route_look_up_modem == DSI_RADIO_3GMODEM) ||
            (dsd_technology == DSI_PS_FMC_IFACE)
            )
          )
      {
        DSI_LOG_ERROR("DSD preferred technology [0x%x] " \
                      "on modem [%d] does not match with " \
                      "user preferred DSI_RADIO_TECH_1X",
                      dsd_technology, route_look_up_modem);
        reti = DSI_ERROR;
      }
      break;
    case DSI_RADIO_TECH_DO:
      if (!(
            (dsd_technology == DSI_PS_CDMA_SN_IFACE && route_look_up_modem == DSI_RADIO_4GMODEM) ||
            (dsd_technology == DSI_PS_FMC_IFACE)
            )
          )
      {
        DSI_LOG_ERROR("DSD preferred technology [0x%x] " \
                      "on modem [%d] does not match with " \
                      "user preferred DSI_RADIO_TECH_DO",
                      dsd_technology, route_look_up_modem);
        reti = DSI_ERROR;
      }
      break;
    case DSI_RADIO_TECH_LTE:
      /* EPC will be returned from MODEM */
      if (dsd_technology != DSI_PS_EPC_IFACE)
      {
        DSI_LOG_ERROR("DSD preferred technology [0x%x] " \
                      "does not match with user preferred " \
                      "DSI_RADIO_TECH_LTE", dsd_technology);
        reti = DSI_ERROR;
      }
      break;
    case DSI_RADIO_TECH_TDSCDMA:
      /* EPC or UMTS will be returned from MODEM */
      if (!(dsd_technology == DSI_PS_EPC_IFACE ||
            dsd_technology == DSI_PS_UMTS_IFACE))
      {
        DSI_LOG_ERROR("DSD preferred technology [0x%x] " \
                      "does not match with user preferred " \
                      "DSI_RADIO_TECH_TDSCDMA", dsd_technology);
        reti = DSI_ERROR;
      }
      break;
    default:
      DSI_LOG_ERROR("we should never have reached here " \
                    "technology local variable stores [0x%x] " \
                    "that is not recognized", technology);
      reti = DSI_ERROR;
      break;
    }
    /* if switch loop broke with error, break from do..while */
    if(DSI_ERROR == reti)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    *pi = i;
    DSI_LOG_DEBUG( "%s", "dsi_mni_look_up: EXIT with suc" );
  }
  else
  {
    *pi = DSI_INVALID_IFACE;

    /* release the NETMGR interface */
    if(DSI_SUCCESS != dsi_link_release(i))
    {
      DSI_LOG_ERROR("dsi_link_release failed: iface=[%d] " ,
                    i);
    }
    else
    {
      DSI_LOG_DEBUG("dsi_link_release success: iface=[%d] " ,
                    i);
    }

    DSI_LOG_DEBUG( "%s", "dsi_mni_look_up: EXIT with err" );
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_release_client
===========================================================================*/
/*!
    @brief
    This function releases the modem network interface handles

    @return
    none
*/
/*=========================================================================*/
void
dsi_mni_release_client(int conn_id)
{
  int qmi_err = QMI_SERVICE_ERR_NONE;
  int qmi_ret;

  DSI_LOG_DEBUG("%s","dsi_mni_release_client: ENTRY");

  if( DSI_IS_WDS_HNDL_VALID(conn_id) )
  {
    DSI_LOG_DEBUG("releasing qdi hndl [%x]", DSI_GET_QDI_HNDL(conn_id));
    if(QDI_SUCCESS != qdi_wds_srvc_release_client(DSI_GET_QDI_HNDL(conn_id)))
    {
      DSI_LOG_ERROR("%s", "QDI srvc release failed");
    }

    DSI_SET_QDI_HNDL(conn_id, QDI_INVALID_CLIENT_HANDLE);
    DSI_SET_WDS_HNDL(conn_id, DSI_INVALID_WDS_HNDL);
  }

  if( DSI_IS_QOS_HNDL_VALID(conn_id) )
  {
    DSI_LOG_DEBUG("releasing qos hndl [%p]", DSI_GET_QOS_HNDL(conn_id));
    if(QDI_SUCCESS != dsi_qmi_qos_release_qmi_client(DSI_GET_QOS_HNDL(conn_id)))
    {
      DSI_LOG_ERROR("%s", "qos srvc release failed");
    }

    DSI_SET_QOS_HNDL(conn_id, DSI_INVALID_QOS_HNDL);
  }

  DSI_LOG_DEBUG("%s","dsi_mni_release_client: EXIT");
}
/*===========================================================================
  FUNCTION:  dsi_mni_cleanup
===========================================================================*/
/*!
    @brief
    This function releases the QMI WDS service and associated clients.

    @return
    none
*/
/*=========================================================================*/
void
dsi_mni_cleanup(boolean from_ssr)
{

  int i = 0;
  DSI_LOG_DEBUG("%s","dsi_mni_cleanup: ENTRY");

  for(i=0; i<DSI_MAX_IFACES; i++)
  {
    dsi_mni_release_client(i);
  }

  for(i=0; i<DSI_MAX_MODEMS; i++)
  {
    is_default_handle_inited[i] = DSI_FALSE;
    if(DSI_MODEM_GET_DEFAULT_WDS_HNDL(i) != DSI_INVALID_WDS_HNDL)
    {
      dsi_qmi_wds_release_qmi_client(DSI_MODEM_GET_DEFAULT_WDS_HNDL(i));
      DSI_MODEM_SET_DEFAULT_WDS_HNDL(i, DSI_INVALID_WDS_HNDL);
    }
  }

  for (i = 0; i < DSI_MAX_DATA_CALLS; i++)
  {
    /* clear store table (call) entry */
    dsi_cleanup_store_tbl(i);
  }

  if(!from_ssr)
  {
    /* De-initialize QDI */
    if ( QDI_SUCCESS != qdi_release())
    {
      DSI_LOG_ERROR("%s", "QDI release failed");
    }
  }
  else
  {
    DSI_LOG_DEBUG("%s","qdi cmdq flush");
    if ( QDI_SUCCESS != qdi_flush_cmdq())
    {
      DSI_LOG_ERROR("%s", "QDI flush failed");
    }
  }

#ifndef DSI_NETCTRL_OFFTARGET
#ifdef FEATURE_DATA_LOG_QXDM
  Diag_LSM_DeInit();
#endif
#endif

  DSI_LOG_DEBUG("%s","dsi_mni_cleanup: EXIT");
}

/*===========================================================================
  FUNCTION:  dsi_at_exit_hdlr
===========================================================================*/
/*!
    @brief
    This function releases the resources when main thread is exiting.

    @return
    none
*/
/*=========================================================================*/
static void
dsi_at_exit_hdlr(void)
{
  /* release QMI resources */
  dsi_mni_cleanup(FALSE);
  /* kill and clean up callback thread */
  dsi_netctrl_cb_deinit();
}

/*===========================================================================
  FUNCTION:  dsi_get_epid
===========================================================================*/
/*!
    @brief
    This function returns the EPID for the physical network device provided.

    @params[in] net_dev: Physical network device
    @params[out] ep_type: The peripheral endpoint type
    @params[out] epid: End point ID obtained through IOCTL.

    @return
    none
*/
/*=========================================================================*/
static void dsi_get_epid
(
  char                  *net_dev,
  data_ep_type_enum_v01 *ep_type,
  uint32_t              *epid
)
{
  int fd;
  int rval = NETMGR_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

  if (NULL == net_dev || NULL == ep_type || NULL == epid)
  {
    DSI_LOG_ERROR("%s", "dsi_get_epid(): Invalid args");
    return;
  }

  DSI_LOG_DEBUG("dsi_get_epid on net dev %s", net_dev);

  /* EP Type is dependent upon underlying rmnet transport */
  if (!strcmp(DSI_PHYS_NET_DEV_RMNET_MHI0, net_dev))
  {
    *ep_type = DATA_EP_TYPE_PCIE_V01;
  }
  else if (!strcmp(DSI_PHYS_NET_DEV_RMNET_IPA0, net_dev))
  {
    *ep_type = DATA_EP_TYPE_EMBEDDED_V01;
  }
  else if (!strcmp(DSI_PHYS_NET_DEV_RMNET_BAM0, net_dev))
  {
    /* There is no epid associated for bam transport */
    *ep_type = DSI_QMI_WDS_PER_EP_TYPE_BAM_DMUX;
  }

  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) {
    DSI_LOG_ERROR("%s", "get_epid: socket failed");
    goto bail;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg) {
    DSI_LOG_ERROR("%s", "get_epid: malloc for ifru_data failed:");
    close(fd);
    goto bail;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, net_dev, sizeof(ifr.ifr_name));
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_GET_EPID;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0) {
    DSI_LOG_ERROR("%s", "get_epid: ioctl RMNET_IOCTL_GET_EPID failed");
    close(fd);
    free(ext_ioctl_arg);
    goto bail;
  }

  *epid = ext_ioctl_arg->u.data;
  DSI_LOG_INFO("get_epid: ioctl RMNET_IOCTL_GET_EPID as %d",ext_ioctl_arg->u.data);

  /* close temporary socket */
  close(fd);
  free(ext_ioctl_arg);
  return;

bail:
  DSI_LOG_ERROR("%s", "get_epid failed, using defaults");
  if (!strcmp(DSI_PHYS_NET_DEV_RMNET_MHI0, net_dev))
  {
    *ep_type = DATA_EP_TYPE_PCIE_V01;
    *epid = DSI_DEFAULT_EPID_MHI;
  }
  else if(!strcmp(DSI_PHYS_NET_DEV_RMNET_IPA0, net_dev))
  {
    *ep_type = DATA_EP_TYPE_EMBEDDED_V01;
    *epid = DSI_DEFAULT_EPID_IPA;
  }
  else
  {
    *ep_type = 0;
    *epid = 0;
  }
  return;

}


/*===========================================================================
  FUNCTION:  dsi_mni_init_client
===========================================================================*/
/*!
    @brief
    This function initializes the modem network interface clients

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_init_client(int conn_id)
{
  int i = 0;
  int ret = DSI_ERROR;
  int reti = DSI_ERROR;
  int qmi_err_code = QMI_NO_ERR;
  int qmi_ret;
  qdi_client_handle_t qdi_handle = QDI_INVALID_CLIENT_HANDLE;
  int subs_id;
  DSI_LOG_DEBUG( "%s", "dsi_mni_init_client: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if( !DSI_IS_IFACE_VALID(conn_id) )
    {
      DSI_LOG_ERROR("%s", "dsi_mni_init_client: invalid iface");
      break;
    }

    /* initialize dsi interfaces for this modem */
    DSI_LOG_DEBUG("dsi_mni_init_client: init iface [%d]: START", conn_id);

    DSI_LOG_DEBUG("dsi_mni_init_client: init qmi connection [%s]",
                  DSI_GET_WDS_STR(conn_id));

    DSI_LOG_DEBUG("dsi_mni_init_client: init wds srvc client"
                  "with name [%s]", DSI_GET_WDS_STR(conn_id));

    subs_id = dsi_get_modem_subs_id();

    /* obtain qmi_wds service client for the given qmi port */
    qdi_handle = qdi_wds_srvc_init_client(DSI_GET_WDS_STR(conn_id),
                                       DSI_GET_DEV_STR(conn_id),
                                       dsi_qmi_wds_ind_cb,
                                       (void *)(intptr_t)conn_id,
                                       subs_id);

    if (qdi_handle == QDI_INVALID_CLIENT_HANDLE)
    {
      DSI_LOG_ERROR(
        "%s", "dsi_mni_init_clients: qdi_wds_srvc_init_client failed");
      /* do not set reti to DSI_ERROR as we want to ignore
       * when wds client init doesn't succeed */
    }

    /* cache qdi handle for this dsi interface */
    DSI_SET_QDI_HNDL(conn_id, qdi_handle);
    DSI_LOG_DEBUG("qdi hndl is [0x%08x]",(unsigned int)DSI_GET_QDI_HNDL(conn_id));

    /* retrieve the default qmi handle from QDI */
    DSI_SET_WDS_HNDL(conn_id, qdi_get_qmi_wds_handle(qdi_handle));
    DSI_LOG_DEBUG("wds hndl is [%p]", DSI_GET_WDS_HNDL(conn_id));


    /* configure QOS if feature enabled */
    if( DSI_TRUE == dsi_config.qos_enable )
    {
      qmi_client_type qos_qmi_hndl;
      /* obtain qmi_qos service client for the given qmi port.
       * Use the device string to allow for automatic client binding if reqd */
      if(DSI_SUCCESS != dsi_qmi_qos_init_qmi_client(DSI_GET_DEV_STR(conn_id),
                                                    (void *)(intptr_t)conn_id,
                                                    subs_id,
                                                    &qos_qmi_hndl))
      {
        DSI_LOG_ERROR("%s", "dsi_qos_init: qmi_qos_srvc_init_client failed");
        /* Invalidate QOS handle to prevent flow operations */
        DSI_SET_QOS_HNDL(conn_id, DSI_INVALID_QOS_HNDL );
      }
      else
      {
        /* cache wds handle for this dsi interface */
        DSI_SET_QOS_HNDL(conn_id, qos_qmi_hndl);
        DSI_LOG_DEBUG("qos hndl is [%p]", DSI_GET_QOS_HNDL(conn_id));
      }
    }
    else
    {
      DSI_SET_QOS_HNDL(conn_id, DSI_INVALID_QOS_HNDL );
    }

    DSI_LOG_DEBUG("dsi_mni_init_client: init iface [%d]: END", conn_id);

    ret = DSI_SUCCESS;
  } while (0);

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_init
===========================================================================*/
/*!
    @brief
    This function initializes the QMI WDS service and corresponding
    wds handles

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_init(boolean from_ssr)
{
  int ret = DSI_ERROR;
  int reti = QDI_SUCCESS;
  int modem, default_iface;
  int qmi_ret, qmi_err_code;
  qmi_client_type wds_hndl;

  DSI_LOG_DEBUG( "%s", "dsi_mni_init: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if(!from_ssr)
    {
      if (QDI_SUCCESS != qdi_init())
      {
        reti = QDI_FAILURE;
        DSI_LOG_ERROR("%s","qdi_init failed");
        break;
      }
    }
    for (modem=0; modem < DSI_MAX_MODEMS; modem++)
    {
      if(is_default_handle_inited[modem] == DSI_FALSE)
      {
        default_iface = DSI_MODEM_GET_DEFAULT_IFACE(modem);

        if (DSI_SUCCESS != dsi_qmi_wds_init_qmi_client(DSI_GET_WDS_STR(default_iface), &wds_hndl))
        {
          DSI_LOG_ERROR("%s", "dsi_qmi_wds_init_qmi_client failed");
          continue;
        }


        DSI_MODEM_SET_DEFAULT_WDS_HNDL(modem, wds_hndl);
        is_default_handle_inited[modem] = DSI_TRUE;
        DSI_LOG_DEBUG( "dsi_qmi_wds_init_qmi_client for modem: %d wds_hndl: %p",modem, wds_hndl);
      }
      else
      {
        DSI_LOG_DEBUG( "dsi_qmi_wds_init_qmi_client for modem: %d wds_hndl: %p",modem, DSI_MODEM_GET_DEFAULT_WDS_HNDL(modem));
      }

    }

    if((DSI_MODEM_GET_DEFAULT_WDS_HNDL(DSI_RADIO_4GMODEM) == DSI_INVALID_WDS_HNDL ) &&
        (DSI_MODEM_GET_DEFAULT_WDS_HNDL(DSI_RADIO_3GMODEM)) == DSI_INVALID_WDS_HNDL)
    {
      ret = DSI_ERROR;
    }
    else
    {
      ret = DSI_SUCCESS;
    }


  } while (0);

  /* print appropriate status before exiting */
  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_init: EXIT with success" );
    atexit(dsi_at_exit_hdlr);
  }
  else
  {
    /* release any non-NULL wds handles and QDI if qdi_init was not successful */
    if(reti == QDI_SUCCESS)
    {
      dsi_mni_cleanup(TRUE);
    }
    else
    {
      dsi_mni_cleanup(FALSE);
    }
    DSI_LOG_DEBUG( "%s", "dsi_mni_init: EXIT with error" );
  }

  return ret;
}


/*===========================================================================
  FUNCTION: dsi_mni_reg_unreg_handoff_ind
===========================================================================*/
/*!
  @brief
  This function is used to register/unregister for Handoff indications

  @return
  DSI_ERROR
  DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_reg_unreg_handoff_ind
(
  int           i,
  dsi_store_t * st_hndl,
  boolean       reg_unreg
)
{
  int   ret = DSI_ERROR;
  int   reti = DSI_SUCCESS;
  int   qmi_ret;
  qmi_client_type  qmi_wds_hndl_v4, qmi_wds_hndl_v6;
  int ip_type = st_hndl->priv.start_nw_params.ip_family_preference;
  wds_indication_register_req_msg_v01 ind_type;

  DSI_LOG_DEBUG("%s", "dsi_mni_reg_unreg_handoff_ind: ENTRY");

  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    qmi_wds_hndl_v4 = qdi_get_qmi_wds_handle_for_ip(DSI_GET_QDI_HNDL(i),
                                                    WDS_IP_FAMILY_PREF_IPV4_V01);
    qmi_wds_hndl_v6 = qdi_get_qmi_wds_handle_for_ip(DSI_GET_QDI_HNDL(i),
                                                    WDS_IP_FAMILY_PREF_IPV6_V01);

    /* Mark the indications to register for */
    memset(&ind_type, 0, sizeof(ind_type));

    ind_type.report_handoff_information_valid = TRUE;
    ind_type.report_handoff_information = (TRUE == reg_unreg) ? TRUE : FALSE;

    /* Register for handoff events */
    switch (ip_type)
    {
    case WDS_IP_FAMILY_PREF_IPV4_V01:
      {
        if (DSI_INVALID_WDS_HNDL == qmi_wds_hndl_v4)
        {
          DSI_LOG_ERROR("invalid WDS handle for iface=%d", i);
          reti = DSI_ERROR;
          break;
        }

        /* V4 Indication */
        if(DSI_SUCCESS != dsi_qmi_wds_indication_register(qmi_wds_hndl_v4,
                                                          &ind_type))
        {
          DSI_LOG_ERROR("dsi_qmi_wds_indication_register for V4 failed: iface=[%d]", i);
          reti = DSI_ERROR;
          break;
        }
      }
      break;
    case WDS_IP_FAMILY_PREF_IPV6_V01:
      {
        if (DSI_INVALID_WDS_HNDL == qmi_wds_hndl_v6)
        {
          DSI_LOG_ERROR("invalid WDS handle for iface=%d", i);
          reti = DSI_ERROR;
          break;
        }

        /* V6 Indication */
        if(DSI_SUCCESS != dsi_qmi_wds_indication_register(qmi_wds_hndl_v6,
                                                          &ind_type))
        {
          DSI_LOG_ERROR("dsi_qmi_wds_indication_register for V6 failed: iface=[%d]", i);
          reti = DSI_ERROR;
          break;
        }
      }
      break;
    case WDS_IP_FAMILY_PREF_UNSPEC_V01:
      {
        if (DSI_INVALID_WDS_HNDL == qmi_wds_hndl_v4
            || DSI_INVALID_WDS_HNDL == qmi_wds_hndl_v6)
        {
          DSI_LOG_ERROR("invalid WDS handle for iface=%d", i);
          reti = DSI_ERROR;
          break;
        }

        if(DSI_SUCCESS != dsi_qmi_wds_indication_register(qmi_wds_hndl_v4,
                                                          &ind_type))
        {
          DSI_LOG_ERROR("dsi_qmi_wds_indication_register for V4/V6 failed: iface=[%d]", i);
          reti = DSI_ERROR;
          break;
        }

        if(DSI_SUCCESS != dsi_qmi_wds_indication_register(qmi_wds_hndl_v6,
                                                          &ind_type))
        {
          DSI_LOG_ERROR("dsi_qmi_wds_indication_register for V4/V6 failed: iface=[%d]", i);
          reti = DSI_ERROR;
          break;
        }
      }
      break;

    default:
      {
        DSI_LOG_ERROR("%s", "dsi_qmi_wds_indication_register: Unknown IP type!");
        reti = DSI_ERROR;
      }
      break;
    }

    if (DSI_SUCCESS != reti)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_reg_unreg_handoff_ind: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_reg_unreg_handoff_ind: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_register_embms_ind
===========================================================================*/
/*!
    @brief
    Register different WDS indication

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_register_embms_ind(int i, dsi_store_t * st_hndl)
{
  int                               ret = DSI_ERROR;
  wds_indication_register_req_msg_v01 ind_type;

  DSI_LOG_DEBUG( "%s", "dsi_mni_register_embms_ind: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    memset(&ind_type, 0, sizeof(ind_type));

    /* register tmgi list indication */
    ind_type.report_embms_tmgi_list_valid = TRUE;
    ind_type.report_embms_tmgi_list = TRUE;

    /* register sai list indication */
    ind_type.report_embms_sai_list_valid = TRUE;
    ind_type.report_embms_sai_list = TRUE;

    /* register control desc control indication */
    ind_type.report_embms_content_desc_control_valid = TRUE;
    ind_type.report_embms_content_desc_control = TRUE;

    /* register service interest indication */
    ind_type.report_embms_svc_interest_valid = TRUE;
    ind_type.report_embms_svc_interest = TRUE;

    if(DSI_SUCCESS != dsi_qmi_wds_indication_register(DSI_GET_WDS_HNDL(i),
                                                      &ind_type))
    {
      DSI_LOG_ERROR("dsi_qmi_wds_indication_register failed: iface=[%d]", i);
      break;
    }
    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_register_embms_ind: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_register_embms_ind: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_embms_tmgi_activate
===========================================================================*/
/*!
    @brief
    This function activate TMGI on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_embms_tmgi_activate(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;
  int ext_err_code;

  DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_activate: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    DSI_LOG_DEBUG("dsi_mni_embms_tmgi_activate dbg_trace_id:[%d]",
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.tranx_id);

    if(DSI_SUCCESS !=
          dsi_qmi_wds_embms_tmgi_activate(DSI_GET_WDS_HNDL(i),
                                          &(st_hndl->priv.embms_tmgi_list_info
                                                            .embms_tmgi_params.act_req),
                                          &ext_err_code))
    {
      DSI_LOG_ERROR("dsi_qmi_wds_embms_tmgi_activate failed: iface=[%d] extended_err=[%d]",
                    i, ext_err_code);
      st_hndl->priv.embms_tmgi_list_info.activate_status = ext_err_code;
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_activate: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_activate: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_embms_tmgi_deactivate
===========================================================================*/
/*!
    @brief
    This function deactivate TMGI on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_embms_tmgi_deactivate(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;
  int ext_err_code;

  DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_deactivate: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    DSI_LOG_DEBUG("dsi_mni_embms_tmgi_deactivate dbg_trace_id:[%d]",
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req.tranx_id);

    if(DSI_SUCCESS !=
          dsi_qmi_wds_embms_tmgi_deactivate(DSI_GET_WDS_HNDL(i),
                                            &(st_hndl->priv.embms_tmgi_list_info
                                                          .embms_tmgi_params.deact_req),
                                            &ext_err_code))
    {
      DSI_LOG_ERROR("dsi_qmi_wds_embms_tmgi_deactivate failed: iface=[%d] extended_err=[%d]",
                    i, ext_err_code);
      st_hndl->priv.embms_tmgi_list_info.deactivate_status = ext_err_code;
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_deactivate: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_deactivate: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_embms_tmgi_activate_deactivate
===========================================================================*/
/*!
    @brief
    This function activate and deactivate TMGI on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_embms_tmgi_activate_deactivate(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;
  int ext_err_code;

  DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_activate_deactivate: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    DSI_LOG_DEBUG("dsi_mni_embms_tmgi_activate_deactivate dbg_trace_id:[%d]",
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.tranx_id);

    if(DSI_SUCCESS !=
        dsi_qmi_wds_embms_tmgi_activate_deactivate(DSI_GET_WDS_HNDL(i),
                                                   &(st_hndl->priv.embms_tmgi_list_info
                                                                .embms_tmgi_params.act_deact_req),
                                                   &ext_err_code))
    {
      DSI_LOG_ERROR("dsi_mni_embms_tmgi_activate_deactivate failed: iface=[%d] extended_err=[%d]",
                    i, ext_err_code);
      st_hndl->priv.embms_tmgi_list_info.activate_status = ext_err_code;
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_activate_deactivate: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_activate_deactivate: EXIT with suc");
  }

  return ret;
}
/*===========================================================================
  FUNCTION:  dsi_mni_embms_tmgi_list_query
===========================================================================*/
/*!
    @brief
    This function queries for active or available TMGI list on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_embms_tmgi_list_query(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;
  int j = 0;

  wds_embms_tmgi_list_query_resp_msg_v01 list_query;

  DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_list_query: ENTRY" );

  memset(&list_query, 0, sizeof(list_query));

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    DSI_LOG_DEBUG("dsi_mni_embms_tmgi_list_query type:[%d] dbg_trace_id:[%d]",
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req.list_type,
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req.tranx_id);

    if(DSI_SUCCESS !=
          dsi_qmi_wds_embms_tmgi_list_query(DSI_GET_WDS_HNDL(i),
                                            &st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params
                                                                                    .list_query_req,
                                            &list_query))
    {
      DSI_LOG_ERROR("dsi_mni_embms_tmgi_list_query failed: iface=[%d]", i);
      break;
    }

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_info = list_query;

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_list_query: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_list_query: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_embms_tmgi_content_desc_update
===========================================================================*/
/*!
    @brief
    This function updates content desc on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_embms_tmgi_content_desc_update(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;

  DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_content_desc_update: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    DSI_LOG_DEBUG("dsi_mni_embms_tmgi_content_desc_update dbg_trace_id:[%d]",
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params
                              .content_desc_info.dbg_trace_id);

    if(DSI_SUCCESS !=
        dsi_qmi_wds_embms_content_desc_update(DSI_GET_WDS_HNDL(i),
                                              &(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params
                                                            .content_desc_info)))
    {
      DSI_LOG_ERROR("dsi_mni_embms_tmgi_content_desc_update failed: iface=[%d]", i);
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_content_desc_update: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_content_desc_update: EXIT with suc");
  }

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_mni_embms_tmgi_svc_interest_list
===========================================================================*/
/*!
    @brief
    This function updates content desc on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_mni_embms_tmgi_svc_interest_list(int i, dsi_store_t * st_hndl)
{
  int ret = DSI_ERROR;

  DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_svc_interest_list: ENTRY" );

  /* this do..while loop decides the overall return value
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    if (!DSI_IS_ID_VALID(i) ||
        !DSI_IS_HNDL_VALID(st_hndl))
    {
      DSI_LOG_ERROR("%s", "**programming err* invalid params received");
      break;
    }

    DSI_LOG_DEBUG("dsi_mni_embms_tmgi_svc_interest_list dbg_trace_id:[%d]",
                  st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params
                              .svc_interest_info.tranx_id);

    if(DSI_SUCCESS !=
        dsi_qmi_wds_embms_svc_interest(DSI_GET_WDS_HNDL(i),
                                       &(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params
                                                            .svc_interest_info)))
    {
      DSI_LOG_ERROR("dsi_mni_embms_tmgi_svc_interest_list failed: iface=[%d]", i);
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_svc_interest_list: EXIT with err");
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_mni_embms_tmgi_svc_interest_list: EXIT with suc");
  }

  return ret;
}

