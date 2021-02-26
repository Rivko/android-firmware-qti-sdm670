/******************************************************************************

                          N E T M G R _ Q M I . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi.c
  @brief   Network Manager QMI Driver Interface

  DESCRIPTION
  Implementation of NetMgr QMI Driver interface.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "ds_list.h"
#include "ds_util.h"
#include "ds_cmdq.h"
#include "ds_trace.h"
#include "qmi_platform.h"
#include "netmgr_defs.h"
#include "netmgr_exec.h"
#include "netmgr_platform.h"
#include "netmgr_util.h"
#include "netmgr_tc.h"
#include "netmgr_tc_i.h"
#include "netmgr_qmi.h"
#include "netmgr_main.h"
#include "wireless_data_administrative_service_v01.h"

#include "netmgr_config.h"
#include "netmgr_qmi_wda.h"
#include "netmgr_rmnet.h"
#include "netmgr_qmi_dfs.h"
#include "netmgr_qmi_wds.h"
#include "netmgr_qmi_dpm.h"
#include "netmgr_qmi_dsd.h"


#include "netmgr_cb.h"
#include "netmgr_qmi_cb.h"

#include <linux/rmnet_data.h>

#ifdef FEATURE_DS_LINUX_ANDROID
#include <cutils/properties.h>

#ifndef NETMGR_OFFTARGET
  #include <private/android_filesystem_config.h>
#endif

#endif

#define NETMGR_QMI_MAX_RETRY_COUNT               10
#define NETMGR_QMI_WAIT_TIME_BEFORE_NEXT_RETRY   500000 /* usec */

#ifdef FEATURE_DATA_IWLAN
  #define NETMGR_IS_REV_IP_TRANS_CONN_ID(cid)                     \
    ((0 == strncmp((cid), "rev_rmnet", strlen("rev_rmnet"))) ||   \
     (0 == strncmp((cid), "r_rmnet_data", strlen("r_rmnet_data"))))

  #define NETMGR_IS_REV_IP_TRANS_LOCAL_BRK_OUT_CONN_ID(cid)         \
   ((0 == strncmp((cid), "rev_rmnet0", strlen("rev_rmnet0"))) ||   \
     (0 == strncmp((cid), "r_rmnet_data0", strlen("r_rmnet_data0"))))

  #define NETMGR_QMI_INVALID_TXN_ID            (0)

  static pthread_mutex_t  txn_list_mutex = PTHREAD_MUTEX_INITIALIZER;

  #define NETMGR_QMI_IS_BEARER_TECH_EX_IWLAN(bt)                  \
    ((WDS_BEARER_TECH_NETWORK_3GPP_V01 == (bt)->technology) &&    \
     (WDS_BEARER_TECH_RAT_EX_3GPP_WLAN_V01 == (bt)->rat_value) && \
     (QMI_WDS_3GPP_SO_MASK_S2B_V01 == (bt)->so_mask))

#endif /* FEATURE_DATA_IWLAN */

#define NETMGR_QMI_QOS_HEADER_6_BYTE         (1<<0)
#define NETMGR_QMI_QOS_HEADER_8_BYTE         (1<<1)
#define NETMGR_QMI_QOS_HEADER_DEFAULT        (0)

#define UNUSED(x)    (void)(x)
#define NETMGR_RADIO_3GMODEM 1

qmi_client_type netmgr_route_look_up_hndl = NETMGR_QMI_CLIENT_INVALID;
boolean is_route_look_up_handle_init = FALSE;

struct netmgr_qos_modify_cache_s netmgr_qos_modify_cache;

LOCAL int
netmgr_qmi_verify_link (int link);

/*qmi message library handle*/
LOCAL int qmi_handle = QMI_INVALID_CLIENT_HANDLE;

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

struct netmgr_qmi_cfg_s netmgr_qmi_cfg;

/*---------------------------------------------------------------------------
   Forward declaration of the function used to process QMI DSD Ind Command
---------------------------------------------------------------------------*/

LOCAL void
netmgr_dsd_ind
(
  void
);

/*---------------------------------------------------------------------------
   Forward declaration of the function used to process QMI WDS Ind Command
---------------------------------------------------------------------------*/
LOCAL void
netmgr_wds_ind
(
  netmgr_qmi_wds_ind_t  *netmgr_qmi_wds_ind
);

/*---------------------------------------------------------------------------
   Inline accessor for getting the WDS Client ID for a given QMI Link
---------------------------------------------------------------------------*/
LOCAL __inline__ qmi_client_type
netmgr_qmi_wds_get_clnt_id (int link, netmgr_qmi_client_type_t type)
{
  if( (unsigned int)type < (unsigned int)NETMGR_QMI_CLIENT_MAX ) {
    return netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[type];
  } else {
    return NETMGR_QMI_CLIENT_INVALID;
  }
}

/*---------------------------------------------------------------------------
   Inline accessor for getting the QOS Client ID for a given QMI Link
---------------------------------------------------------------------------*/
LOCAL __inline__ qmi_client_type
netmgr_qmi_qos_get_clnt_id (int link, netmgr_qmi_client_type_t type)
{
  if( (unsigned int)type < (unsigned int)NETMGR_QMI_CLIENT_MAX ) {
    return netmgr_qmi_cfg.links[link].qos_info.clnt_hdl[type];
  } else {
    return NETMGR_QMI_CLIENT_INVALID;
  }
}

/*---------------------------------------------------------------------------
   Inline accessor for getting a pointer to the QMI Driver Control Info for
   a given QMI Link
---------------------------------------------------------------------------*/
LOCAL __inline__ netmgr_wds_qmi_drv_info_t *
netmgr_qmi_get_wds_qmi_drv_info (int link)
{
  return &netmgr_qmi_cfg.links[link].wds_info;
}

LOCAL netmgr_qmi_client_type_t
netmgr_qmi_convert_ip_pref_to_client_type(wds_ip_family_enum_v01  ip_family)
{
  netmgr_qmi_client_type_t client_type;

  switch( ip_family )
  {
    case WDS_IP_FAMILY_IPV6_V01:
      client_type = NETMGR_QMI_CLIENT_IPV6;
      break;
    case WDS_IP_FAMILY_IPV4_V01:
    default:
      client_type = NETMGR_QMI_CLIENT_IPV4;
      break;
  }

  return client_type;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_wds_get_tech_name
===========================================================================*/
/*!
@brief
  Retrives the technology name for Modem network interface

@return
 unsigned int - technology name

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
wds_technology_name_enum_v01 netmgr_qmi_wds_get_tech_name(int link)
{
  return netmgr_qmi_cfg.links[link].wds_info.tech_name;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_map_dev_conn_instance
===========================================================================*/
/*!
@brief
  maps qmi connection instance to that of the qmi framework

@arg *dev_id Name of device to get the connection instance
@arg *conn_id populated with the frameworks connection instance

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message
*/
/*=========================================================================*/
int netmgr_qmi_map_dev_conn_instance
(
  const char                      *dev_id,
  qmi_client_qmux_instance_type   *conn_id
)
{
  int rc = NETMGR_SUCCESS;
  int mux_id = -1;
  int ep_type = -1;
  int epid = -1;

  if ( !dev_id || !conn_id )
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return NETMGR_FAILURE;
  }

  switch (QMI_PLATFORM_DEV_NAME_TO_CONN_ID_EX(dev_id, &ep_type, &epid, &mux_id))
  {
    case QMI_CONN_ID_RMNET_0:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_0;
      break;
    case QMI_CONN_ID_RMNET_1:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_1;
      break;
    case QMI_CONN_ID_RMNET_2:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_2;
      break;
    case QMI_CONN_ID_RMNET_3:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_3;
      break;
    case QMI_CONN_ID_RMNET_4:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_4;
      break;
    case QMI_CONN_ID_RMNET_5:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_5;
      break;
    case QMI_CONN_ID_RMNET_6:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_6;
      break;
    case QMI_CONN_ID_RMNET_7:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_7;
      break;

    case QMI_CONN_ID_RMNET_MDM_0:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0;
      break;
    case QMI_CONN_ID_RMNET_MDM_1:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_1;
      break;
    case QMI_CONN_ID_RMNET_MDM_2:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_2;
      break;
    case QMI_CONN_ID_RMNET_MDM_3:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_3;
      break;
    case QMI_CONN_ID_RMNET_MDM_4:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_4;
      break;
    case QMI_CONN_ID_RMNET_MDM_5:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_5;
      break;
    case QMI_CONN_ID_RMNET_MDM_6:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_6;
      break;
    case QMI_CONN_ID_RMNET_MDM_7:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_7;
      break;

    case QMI_CONN_ID_RMNET_MHI_0:
      *conn_id = QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0;
      break;
    case QMI_CONN_ID_RMNET_MHI_1:
      *conn_id = QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_1;
      break;

    default:
      netmgr_log_err("Invalid connection ID %s passed no match received", dev_id);
      rc = NETMGR_FAILURE;
    break;
  }

  return rc;
}

/*===========================================================================
                            LOCAL FUNCTION DEFINITIONS
===========================================================================*/

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_wds_util_enq_txn
===========================================================================*/
/*!
@brief
  Enqueues the reverse IP txn for the given IP client type and link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int netmgr_qmi_wds_util_enq_txn
(
  netmgr_qmi_client_type_t  clnt,
  int                       link,
  uint32_t                  txn_id
)
{
  netmgr_util_circ_list_type  *txn_list = NULL;
  uint32_t                    *txn_ptr = NULL;
  int rc = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_LOCK_MUTEX(txn_list_mutex);

  if (clnt >= NETMGR_QMI_CLIENT_MAX)
  {
    netmgr_log_err("invalid clnt=%d\n", clnt);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link(link))
  {
    netmgr_log_err("invalid link=%d\n", link);
    goto bail;
  }

  txn_ptr = ds_malloc(sizeof(uint32_t));

  if (!txn_ptr)
  {
    netmgr_log_err("failed to alloc memory for txn_id=%ld clnt=%d link=%d\n",
                   (long)txn_id, clnt, link);
    goto bail;
  }

  *txn_ptr = txn_id;

  txn_list = &netmgr_qmi_cfg.links[link].wds_info.rev_ip_txns[clnt];

  if (NETMGR_SUCCESS != netmgr_util_enq_circ_list(txn_list, (void *)txn_ptr))
  {
    netmgr_log_err("failed to enqueue txn_id=%ld, clnt=%d link=%d\n",
                   (long)txn_id, clnt, link);
    ds_free(txn_ptr);
    goto bail;
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_UNLOCK_MUTEX(txn_list_mutex);
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_wds_util_deq_txn
===========================================================================*/
/*!
@brief
  Dequeues and returns the next reverse IP txn for the given IP client type and
  link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int netmgr_qmi_wds_util_deq_txn
(
  netmgr_qmi_client_type_t  clnt,
  int                       link,
  uint32_t                  *txn_id
)
{
  netmgr_util_circ_list_type  *txn_list = NULL;
  uint32_t  *txn_ptr = NULL;
  int rc = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_LOCK_MUTEX(txn_list_mutex);

  if (!txn_id)
  {
    netmgr_log_err("invalid input\n");
    goto bail;
  }

  if (clnt >= NETMGR_QMI_CLIENT_MAX)
  {
    netmgr_log_err("invalid clnt=%d\n", clnt);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link(link))
  {
    netmgr_log_err("invalid link=%d\n", link);
    goto bail;
  }

  txn_list = &netmgr_qmi_cfg.links[link].wds_info.rev_ip_txns[clnt];

  if (NETMGR_SUCCESS != netmgr_util_deq_circ_list(txn_list, (void **)&txn_ptr))
  {
    netmgr_log_err("txn dequeue failed clnt=%d, link=%d\n", clnt, link);
    goto bail;
  }

  *txn_id = *txn_ptr;

  ds_free(txn_ptr);

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_UNLOCK_MUTEX(txn_list_mutex);
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_qmi_reset_link_info
===========================================================================*/
/*!
@brief
 Helper function to reset state information for a given QMI link.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qmi_reset_link_info (int link)
{
  /* Reset interface info */
  netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] = NETMGR_QMI_CLIENT_INVALID;
  netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] = NETMGR_QMI_CLIENT_INVALID;
  netmgr_qmi_cfg.links[link].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] = NETMGR_QMI_CLIENT_INVALID;
  netmgr_qmi_cfg.links[link].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] = NETMGR_QMI_CLIENT_INVALID;
  netmgr_qmi_cfg.links[link].wds_info.mtu_ipv4 = NETMGR_MTU_INVALID;
  netmgr_qmi_cfg.links[link].wds_info.mtu_ipv6 = NETMGR_MTU_INVALID;

  memset( &netmgr_qmi_cfg.links[link].wds_info.addr_info, 0x0,
          sizeof(netmgr_qmi_cfg.links[link].wds_info.addr_info) );

#ifdef FEATURE_DATA_IWLAN
  netmgr_qmi_cfg.links[link].dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] = NETMGR_QMI_CLIENT_INVALID;
  netmgr_qmi_cfg.links[link].dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] = NETMGR_QMI_CLIENT_INVALID;
  netmgr_qmi_cfg.links[link].assoc_link = NETMGR_LINK_MAX;

  netmgr_util_circ_list_destroy(&netmgr_qmi_cfg.links[link].wds_info.rev_ip_txns[NETMGR_QMI_CLIENT_IPV4]);
  netmgr_util_circ_list_destroy(&netmgr_qmi_cfg.links[link].wds_info.rev_ip_txns[NETMGR_QMI_CLIENT_IPV6]);
#endif /* FEATURE_DATA_IWLAN */

  netmgr_qmi_cfg.links[link].wds_info.first_conn_clnt = NETMGR_QMI_CLIENT_MAX;
  netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn = FALSE;
  netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn = FALSE;
  netmgr_qmi_cfg.links[link].wds_info.in_use      = FALSE;

  if(0 == link)
  {
    netmgr_dsd_clnt_hndl = NETMGR_QMI_CLIENT_INVALID;
  }
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_driver_cleanup
===========================================================================*/
/*!
@brief
  Performs cleanup of QMI client connections.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qmi_driver_cleanup
(
  int link
)
{
  int qmi_err;
  netmgr_link_info_t * link_info;
  int clnt;

  if( !netmgr_qmi_cfg.link_array[link].enabled )
  {
    netmgr_log_err("%s(): QMI link=%d is disabled\n",
                   __func__,link);
    return;
  }

  netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_LINK_OOS, (void *)&link);

  link_info = &netmgr_qmi_cfg.links[link];

  if (NETMGR_QMI_CLIENT_INVALID != link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    netmgr_log_high("Releasing the V4 WDS qmi_client_handle %p\n",
                    link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4]);
    netmgr_qmi_wds_release_client( link, link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] );
  }
  else
  {
    netmgr_log_high("Invalid V4 WDS qmi_client_handle %p\n",
                    link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4]);
  }

  if (NETMGR_QMI_CLIENT_INVALID != link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    netmgr_log_high("Releasing the V6 WDS qmi_client_handle %p\n",
                    link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
    netmgr_qmi_wds_release_client( link, link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] );
  }
  else
  {
    netmgr_log_high("Invalid V6 WDS qmi_client_handle %p\n",
                    link_info->wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
  }

#ifdef NETMGR_QOS_ENABLED
  if (NETMGR_QMI_CLIENT_INVALID != link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    netmgr_qmi_qos_release_client(link,
                                  link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4]);
    netmgr_log_high("Releasing the QOS qmi_client_handle %p \n",
                    link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4]);
  }
  else
  {
    netmgr_log_high("Invalid QOS qmi_client_handle %p \n",
                    link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4]);
  }

  if (NETMGR_QMI_CLIENT_INVALID != link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    netmgr_qmi_qos_release_client(link,
                                  link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
    netmgr_log_high("Releasing the QOS qmi_client_handle %p \n",
                    link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
  }
  else
  {
    netmgr_log_high("Invalid QOS qmi_client_handle %p \n",
                    link_info->qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
  }
#endif /* NETMGR_QOS_ENABLED */

#ifdef FEATURE_DATA_IWLAN
  if (NETMGR_QMI_CLIENT_INVALID != link_info->dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    netmgr_log_high("Releasing the IPv4 DFS qmi_client_handle %p\n",
                    link_info->dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4]);
    qmi_client_release( link_info->dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] );
  }

  if (NETMGR_QMI_CLIENT_INVALID != link_info->dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    netmgr_log_high("Releasing the IPv6 DFS qmi_client_handle %p\n",
                    link_info->dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
    qmi_client_release( link_info->dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] );
  }
#endif /* FEATURE_DATA_IWLAN */

  if (0 == link)
  {
    (void) netmgr_qmi_dsd_release_client();
    netmgr_log_high("Releasing the DSD qmi_client_handle ");
  }
  /* reset link info to prevent accidental usage */
  netmgr_qmi_reset_link_info(link);

  NETMGR_QMI_SET_WDA_REQ_SENT(FALSE);
}

/*===========================================================================
  FUNCTION  netmgr_qmi_cleanup
===========================================================================*/
/*!
@brief
  Disconnects completely from QMI.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qmi_cleanup
(
  void
)
{
  int i=0;
  /* Release QMI connections */
  /* Release client handles */
  for( i=0; i < NETMGR_MAX_LINK; i++ ) {
    netmgr_qmi_driver_cleanup(i);
  }

  /* Release QMI library connection */
  if(qmi_handle >= 0)
  {
    qmi_release(qmi_handle);
  }
}


/*===========================================================================
  FUNCTION  netmgr_qmi_verify_link
===========================================================================*/
/*!
@brief
 Helper function to verify that a given QMI link is valid.

@return
  int - NETMGR_SUCCESS if the link is valid, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_verify_link (int link)
{
  /* Range check */
  if ((link < 0) || (link >= NETMGR_MAX_LINK)) {
    return NETMGR_FAILURE;
  }

  if( !netmgr_qmi_cfg.link_array[link].enabled ) {
    return NETMGR_FAILURE;
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_get_dev_str_for_link
===========================================================================*/
/*!
@brief
  Helper function to return the device string associated for the link.

@return
  Device name associated with the link (rmnet_data0 etc)
*/
/*=========================================================================*/
LOCAL char*
netmgr_qmi_get_dev_str_for_link (int link)
{
  static char dev_name[NETMGR_IF_NAME_MAX_LEN];

  /* Verify that link id is valid */
  if (netmgr_qmi_verify_link(link) == NETMGR_SUCCESS &&
      (unsigned int)link >= netmgr_main_cfg.def_link)
  {
    snprintf(dev_name, NETMGR_IF_NAME_MAX_LEN, "%s%d",
             NETMGR_MAIN_RMNET_DATA_PREFIX, (unsigned int)link - netmgr_main_cfg.def_link);
    return dev_name;
  }
  return NULL;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_get_conn_id_for_link
===========================================================================*/
/*!
@brief
 Helper function to return the QMI connection ID (port number) for a given
 QMI link.

@return
  qmi_connection_id_type - QMI Connection ID for the link

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL const char*
netmgr_qmi_get_conn_id_for_link (int link)
{
  /* Verify that link id is valid */
  if (netmgr_qmi_verify_link(link) == NETMGR_SUCCESS) {
    /* Return qmi connection id for the link */
    return netmgr_qmi_cfg.link_array[link].qmi_conn_id;
  }
  return NULL;
}

/*===========================================================================
  FUNCTION netmgr_qmi_qos_modify_info_alloc
===========================================================================*/
/*!
@brief
 Allocate buffer for qmi qos modify cache.

@return
 pointer to allocated memory

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL netmgr_qmi_qos_modify_info_t *
netmgr_qmi_qos_modify_info_alloc(void)
{
    netmgr_qmi_qos_modify_info_t *qmi_qos_modify_buf = NULL;

    NETMGR_LOG_FUNC_ENTRY;

    if((qmi_qos_modify_buf = netmgr_malloc(sizeof(netmgr_qmi_qos_modify_info_t))) == NULL) {
        netmgr_log_err("%s(): netmgr malloc failed\n", __func__);
    }else{
        memset(qmi_qos_modify_buf, 0x0, sizeof(netmgr_qmi_qos_modify_info_t));
    }

    NETMGR_LOG_FUNC_EXIT;

    return qmi_qos_modify_buf;
}
/*===========================================================================
  FUNCTION  netmgr_qmi_get_modem_link_info
===========================================================================*/
/*!
@brief
  Function to query Modem WDS information for specific link.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - netmgr_qmi_init() must have been called

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_qmi_get_modem_link_info
(
  uint8                   link,
  wds_ip_family_enum_v01  evt_ip_family
)
{
  qmi_client_type                        client_hndl;
  wds_get_runtime_settings_resp_msg_v01  call_info;
  netmgr_address_set_t                   *addr_info_ptr = NULL;
  netmgr_qmi_client_type_t clnt;
  unsigned long *mtu_ptr = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  clnt = netmgr_qmi_convert_ip_pref_to_client_type(evt_ip_family);
  client_hndl = netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[clnt];

  if (NETMGR_SUCCESS != netmgr_qmi_wds_get_runtime_settings(link,
                                                            client_hndl,
                                                            (QMI_WDS_MASK_REQ_SETTINGS_DNS_ADDR_V01  |
                                                             QMI_WDS_MASK_REQ_SETTINGS_IP_ADDR_V01   |
                                                             QMI_WDS_MASK_REQ_SETTINGS_GATEWAY_INFO_V01 |
                                                             QMI_WDS_MASK_REQ_SETTINGS_MTU_V01          |
                                                             QMI_WDS_MASK_REQ_SETTINGS_IP_FAMILY_V01),
                                                            &call_info))
  {
    netmgr_log_err("failed on qmi_wds_get_curr_call_info on link=%d ",
                   link);
    return NETMGR_FAILURE;
  }

  if( WDS_IP_FAMILY_IPV4_V01 == evt_ip_family )
  {
    addr_info_ptr = &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4;
    mtu_ptr = &netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv4;
  }
  else if(WDS_IP_FAMILY_IPV6_V01 == evt_ip_family )
  {
    addr_info_ptr = &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6;
    mtu_ptr = &netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv6;
  }
  else
  {
    netmgr_log_err("unsupported IP family value[%d]\n", evt_ip_family );
    return NETMGR_FAILURE;
  }

  /* Cache results in WDS state info */
  if (call_info.mtu_valid)
  {
    *mtu_ptr = (NETMGR_MTU_MAX < call_info.mtu)? NETMGR_MTU_MAX : call_info.mtu;
    netmgr_log_med("Link MTU: %d\n", call_info.mtu );
  }
  else
  {
    netmgr_log_err("Link MTU undefined\n");
    *mtu_ptr = NETMGR_MTU_INVALID;
    return NETMGR_FAILURE;
  }

  if (call_info.ip_family_valid)
  {
    addr_info_ptr->if_addr.type = call_info.ip_family;
    netmgr_log_med("IP address family: 0x%02x\n", addr_info_ptr->if_addr.type );
  }
  else
  {
    netmgr_log_err("IP address family undefined\n");
    return NETMGR_FAILURE;
  }

  /* Process Um interface address */
  if (call_info.ipv4_address_preference_valid)
  {
    /* Update the address only if an address purge is not pending */
    if (FALSE == addr_info_ptr->is_addr_purge_pend)
    {
      addr_info_ptr->if_addr.addr.v4 = htonl( (uint32_t)call_info.ipv4_address_preference );
      NETMGR_LOG_IPV4_ADDR( med, "Modem iface", addr_info_ptr->if_addr.addr.v4 );
    }
  }
  else
  {
    /* Update the address only if an address purge is not pending */
    if (FALSE == addr_info_ptr->is_addr_purge_pend)
    {
      if (call_info.ipv6_addr_valid)
      {
        memcpy( addr_info_ptr->if_addr.addr.v6_addr8,
                call_info.ipv6_addr.ipv6_addr,
                sizeof(addr_info_ptr->if_addr.addr.v6_addr8) );
        NETMGR_LOG_IPV6_ADDR( med, "Modem iface",
                              addr_info_ptr->if_addr.addr.v6_addr64 );
      }
      else
      {
        netmgr_log_err("Modem iface IP address undefined\n");
        return NETMGR_FAILURE;
      }
    }

    /* Process IPv6 prefix len */
    addr_info_ptr->if_mask = (unsigned int)call_info.ipv6_addr.ipv6_prefix_length;
    netmgr_log_med( "Modem iface IPV6 Prefix len = %d\n", addr_info_ptr->if_mask );
  }

  /* Process IPV4 subnet mask */
  if (call_info.ipv4_subnet_mask_valid)
  {
    addr_info_ptr->if_mask = htonl( (uint32_t)call_info.ipv4_subnet_mask );
    NETMGR_LOG_IPV4_ADDR( med, "Subnet mask", addr_info_ptr->if_mask );
  }

  /* Process Gateway  address */
  if (call_info.ipv4_gateway_addr_valid) {
    addr_info_ptr->gateway.addr.v4 = htonl( (uint32_t)call_info.ipv4_gateway_addr );
    addr_info_ptr->gateway.type = NETMGR_IPV4_ADDR;
    NETMGR_LOG_IPV4_ADDR( med, "Gateway", addr_info_ptr->gateway.addr.v4 );
  }
  else
  {
    if (call_info.ipv6_gateway_addr_valid)
    {
      memcpy( addr_info_ptr->gateway.addr.v6_addr8,
              call_info.ipv6_gateway_addr.ipv6_addr,
              sizeof(addr_info_ptr->gateway.addr.v6_addr8) );
      addr_info_ptr->gateway.type = NETMGR_IPV6_ADDR;
      NETMGR_LOG_IPV6_ADDR( med, "Gateway", addr_info_ptr->gateway.addr.v6_addr64 );

      addr_info_ptr->gw_mask = (unsigned int)call_info.ipv6_addr.ipv6_prefix_length;
      netmgr_log_med( "Gateway IPV6 Prefix len = %d\n", addr_info_ptr->gw_mask );
    }
    else
    {
      netmgr_log_err("Gateway IP address undefined\n");
      return NETMGR_FAILURE;
    }
  }

  /* Process DNS addresses (primary & secondary) */
  memset( &addr_info_ptr->dns_primary, 0x0, sizeof(addr_info_ptr->dns_primary) );
  memset( &addr_info_ptr->dns_secondary, 0x0, sizeof(addr_info_ptr->dns_secondary) );

  if (call_info.primary_DNS_IPv4_address_preference_valid)
  {
    addr_info_ptr->dns_primary.type = NETMGR_IPV4_ADDR;
    addr_info_ptr->dns_primary.addr.v4 =
      htonl( (uint32_t)call_info.primary_DNS_IPv4_address_preference );
    NETMGR_LOG_IPV4_ADDR( med, "DNS Primary",
                          addr_info_ptr->dns_primary.addr.v4 );
  }
  else if (call_info.primary_dns_IPv6_address_valid)
  {
    addr_info_ptr->dns_primary.type = NETMGR_IPV6_ADDR;
    memcpy( addr_info_ptr->dns_primary.addr.v6_addr8,
            call_info.primary_dns_IPv6_address,
            sizeof(addr_info_ptr->dns_primary.addr.v6_addr8) );
    NETMGR_LOG_IPV6_ADDR( med, "DNS Primary",
                          addr_info_ptr->dns_primary.addr.v6_addr64 );
  }
  else
  {
    netmgr_log_low("DNS primary address undefined\n");
  }

  if (call_info.secondary_DNS_IPv4_address_preference_valid)
  {
    addr_info_ptr->dns_secondary.type = NETMGR_IPV4_ADDR;
    addr_info_ptr->dns_secondary.addr.v4 =
      htonl( (uint32_t)call_info.secondary_DNS_IPv4_address_preference );
    NETMGR_LOG_IPV4_ADDR( med, "DNS Secondary",
                          addr_info_ptr->dns_secondary.addr.v4 );
  }
  else if (call_info.secondary_dns_IPv6_address_valid)
  {
    addr_info_ptr->dns_secondary.type = NETMGR_IPV6_ADDR;
    memcpy( addr_info_ptr->dns_secondary.addr.v6_addr8,
            call_info.secondary_dns_IPv6_address,
            sizeof(addr_info_ptr->dns_secondary.addr.v6_addr8) );
    NETMGR_LOG_IPV6_ADDR( med, "DNS Secondary",
                          addr_info_ptr->dns_secondary.addr.v6_addr64 );
  }
  else
  {
    netmgr_log_low("DNS secondary address undefined\n");
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;
}


/*===========================================================================
  FUNCTION  netmgr_sys_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming QMI SYS Indication message. This function
 is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Depending on the system indication, appropriate action is
      taken to release/acquire QMI resources
*/
/*=========================================================================*/
LOCAL void
netmgr_sys_ind
(
  netmgr_qmi_sys_event_type  event_id,
  int                        link
)
{
  netmgr_sm_events_t sm_ev = NETMGR_INVALID_EV;
  netmgr_exec_cmd_t * cmd = NULL;
  int i=0;

  /* Process based on indication type */
  switch (event_id) {
  case NETMGR_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND:
    netmgr_log_high("rcvd NETMGR_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND on "
                    "link=%d\n",
                    link);

    if ( TRUE == netmgr_main_cfg.dpm_enabled )
    {
      netmgr_qmi_dpm_process_sys_event( NETMGR_QMI_DPM_OOS_EV);
    }
    else if(netmgr_main_cfg.single_qmux_ch_enabled == TRUE)
    {
      netmgr_qmi_wds_register_notifier(0);
    }

    netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_OOS, NULL);
    netmgr_rmnet_remove_all_configuration();
    if (netmgr_main_get_low_latency_filters_enabled())
    {
      netmgr_kif_ifioctl_change_sleep_state((const char *)netmgr_main_get_phys_net_dev(),
                                            NETMGR_KIF_RESET_SLEEP_STATE);
      rmnet_qmi_dfs_release_qmi_client((const char *)netmgr_main_get_phys_net_dev());
    }

    if(NETMGR_QMI_CLIENT_INVALID != netmgr_route_look_up_hndl)
    {
      qmi_client_release(netmgr_route_look_up_hndl);
      is_route_look_up_handle_init = FALSE;
      netmgr_route_look_up_hndl = NETMGR_QMI_CLIENT_INVALID;
    }
    break;

  case NETMGR_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND:
    netmgr_log_high("rcvd NETMGR_QMI_SYS_EVENT_MODEM_IS_SERVICE_IND on "
                    "link=%d\n",
                    link);

    NETMGR_QMI_SET_WDA_REQ_SENT(FALSE);
    netmgr_log_med("%s(): wda_req_sent [%d]",__func__,NETMGR_QMI_GET_WDA_REQ_SENT());

    if ( TRUE == netmgr_main_cfg.dpm_enabled )
    {
      netmgr_qmi_dpm_process_sys_event( NETMGR_QMI_DPM_IS_EV);
    }
    else if(netmgr_main_cfg.single_qmux_ch_enabled == TRUE)
    {
      netmgr_qmi_wds_release_notifier(0);
    }

    netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_IS, NULL);
    if (netmgr_main_get_rmnet_data_enabled())
    {
      netmgr_rmnet_configure_embedded_data();
    }
    netmgr_kif_set_qos_header_format(NETMGR_RMNET_START,
                                     &(netmgr_main_cfg.data_format));
    if (netmgr_main_get_low_latency_filters_enabled())
    {
      netmgr_qmi_dfs_enable_low_latency_filters((const char *)netmgr_main_get_phys_net_dev());
    }
    netmgr_main_cfg.df_configured = FALSE;
    break;

  default:
    netmgr_log_low("ignore qmi sys ind [%d]\n", event_id);
    break;
  }

  return;
}

/*===========================================================================
  FUNCTION  netmgr_wds_pkt_srvc_status_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming WDS Packet Service Status Indication
 message. This function is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_wds_pkt_srvc_status_ind
(
  int                                    link,
  qmi_client_type                        user_handle,
  const wds_pkt_srvc_status_ind_msg_v01  *ind_data
)
{
  wds_ip_family_enum_v01 ip_family = WDS_IP_FAMILY_IPV4_V01; /* assume IPV4 */
  netmgr_exec_cmd_t * cmd;
  netmgr_address_set_t * address_set_ptr = NULL;
  netmgr_qmi_client_type_t clnt;
  boolean xlat_capable = FALSE;

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify link id */
  NETMGR_ASSERT(netmgr_qmi_verify_link(link) == NETMGR_SUCCESS);

  /* verify indication data */
  NETMGR_ASSERT(ind_data);

  if (ind_data->ip_family_valid)
  {
    ip_family = ind_data->ip_family;
  }

  clnt = netmgr_qmi_convert_ip_pref_to_client_type(ip_family);

  /* Ignore the indication on the different QMI WDS handle */
  if (user_handle != netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[clnt])
  {
    netmgr_log_low("ignoring IP[%d] type pkt_srvc_ind on WDS handle[%p] for link[%d]\n",
                   ip_family,
                   user_handle,
                   link);
    return;
  }

  /* Check Modem interface link status */
  switch (ind_data->status.connection_status)
  {
    case WDS_CONNECTION_STATUS_CONNECTED_V01:
    {
      /* Allocate address buffer; released in SM module */
      address_set_ptr = ds_malloc( sizeof(netmgr_address_set_t) );
      if( NULL == address_set_ptr )
      {
        netmgr_log_err("failed to allocate address_set\n");
        return;
      }
      memset(address_set_ptr, 0, sizeof(netmgr_address_set_t));

      /* Update the 'first_conn_clnt' after first PACKET_DATA_CONNECTED event */
      if (NETMGR_QMI_CLIENT_MAX == netmgr_qmi_cfg.links[link].wds_info.first_conn_clnt
         || (netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn == FALSE &&
         netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn == FALSE )
      )
      {
        netmgr_qmi_cfg.links[link].wds_info.first_conn_clnt = clnt;
      }

      if (clnt == NETMGR_QMI_CLIENT_IPV6){
        netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn = TRUE;
      }
      else if (clnt == NETMGR_QMI_CLIENT_IPV4){
        netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn = TRUE;
      }

      netmgr_log_low("V4 and V6 pkt_srvc_counts are : [%d] [%d]",
                      netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn,
                      netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn);

      /*-------------------------------------------------------------------------
        For reconfiguraiton, capture the current address before its updated
      -------------------------------------------------------------------------*/
      if (ind_data->status.reconfiguration_required)
      {
        if (NETMGR_ADDRSET_MASK_INVALID !=
            netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask)
        {
          /* Copy current address into buffer */
          *address_set_ptr = (WDS_IP_FAMILY_IPV4_V01 == ip_family)?
                             netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4 :
                             netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6;
        }
        else
        {
          netmgr_log_err("no previously cached address for link[%d]\n", link);
        }
        address_set_ptr->is_reconfig = TRUE;
      }

      /* Store XLAT capability of the call. This is passed on to the exec for calling
         corresponding extended ioctl */
      if (ind_data->xlat_capable_valid)
      {
        netmgr_qmi_cfg.links[link].wds_info.xlat_capable = ind_data->xlat_capable;
      }

      /*-------------------------------------------------------------------------
        Query Modem network interface for link info
      -------------------------------------------------------------------------*/
      if (
#ifndef FEATURE_DATA_PREFIX_DELEGATION
          (ip_family == WDS_IP_FAMILY_IPV4_V01) &&
#endif /* FEATURE_DATA_PREFIX_DELEGATION */
          (NETMGR_SUCCESS != netmgr_qmi_get_modem_link_info( link, ip_family ))
         )
      {
        /* Just report error and continue */
        netmgr_log_err("failed on netmgr_qmi_get_modem_link_info\n");
      }

      if (!ind_data->status.reconfiguration_required)
      {
        if (ip_family == WDS_IP_FAMILY_IPV4_V01)
        {
          netmgr_log_high("setting valid_mask for ip_family %d for link[%d]",ip_family,link);
          netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask |= NETMGR_ADDRSET_MASK_IPV4;

          *address_set_ptr = netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4;
        }
        else if (ip_family == WDS_IP_FAMILY_IPV6_V01)
        {
          netmgr_log_high("setting valid_mask for ip_family %d for link[%d]",ip_family,link);

          /* Update necessary fields */
          netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask |= NETMGR_ADDRSET_MASK_IPV6;
          netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6.if_addr.type = NETMGR_IPV6_ADDR;

          *address_set_ptr = netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6;
        }
        else
        {
          netmgr_log_err("no previously cached address for link[%d]\n", link);
        }
      }

      /*-------------------------------------------------------------------------
        Post message to executive
      -------------------------------------------------------------------------*/
      cmd = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd);

      cmd->data.type                   = NETMGR_WDS_CONNECTED_EV;
      cmd->data.link                   = link;
      cmd->data.info.connect_msg.reconfig_required =
        ind_data->status.reconfiguration_required;
      cmd->data.info.connect_msg.addr_info_ptr = address_set_ptr;

      if (ind_data->tech_name_valid)
      {
        netmgr_qmi_cfg.links[ link ].wds_info.tech_name =
          ind_data->tech_name;
      }
      else
      {
        netmgr_qmi_cfg.links[ link ].wds_info.tech_name =
          WDS_TECHNOLOGY_NAME_ENUM_MAX_ENUM_VAL_V01;
      }

      netmgr_log_med("Posting connect event - family[%d] reconfig[%d]\n",
                     ip_family, cmd->data.info.connect_msg.reconfig_required );

      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) ) {
        NETMGR_ABORT("netmgr_wds_pkt_srvc_status_ind: failed to put commmand\n");
        netmgr_exec_release_cmd(cmd);
        ds_free( address_set_ptr );
        return;
      }
      netmgr_l2s_format_msg(link,"WDS_CONNECTED IPV%d",
                           (ip_family == WDS_IP_FAMILY_IPV4_V01)? 4 : 6);
    }
    break;

    case WDS_CONNECTION_STATUS_DISCONNECTED_V01:
    {
      /* Modem interface has disconnected. */

      /* Check call end reason code if present */
      if (ind_data->call_end_reason_valid)
      {
        if (ind_data->call_end_reason < WDS_CER_UNSPECIFIED_V01)
        {
          netmgr_log_high("Invalid CALL_END_CODE received : %d\n",
                          ind_data->call_end_reason);
        }
      }

      /*-------------------------------------------------------------------------
        Post message to executive
      -------------------------------------------------------------------------*/
      cmd = netmgr_exec_get_cmd();
      NETMGR_ASSERT(cmd);

      cmd->data.type                                = NETMGR_WDS_DISCONNECTED_EV;
      cmd->data.link                                = link;

      /* Set the interface teardown flag.  For single-IP calls, the
       * interface will teardown when disconnect indication arrives.
       * For dual-IP calls, it may be two IP addresses are on a single
       * interface.  So the first disconnect must purge the specified
       * IP addreess, yet the other address lives on and network
       * interface state remains unchanged.  We clear the valid_mask
       * below for this address family to indicate the interface is
       * now effectively a single-IP call.  The next disconnect
       * indication will then teardown interface as normal.
       */
      cmd->data.info.disconnect_msg.teardown_iface       =
        (NETMGR_ADDRSET_MASK_IPV4V6 != netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask);
      cmd->data.info.disconnect_msg.addr_info_ptr   =
        (WDS_IP_FAMILY_IPV6_V01 == ip_family)?
        &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6 :
        &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4;

      /* Clear the address purge pending flags */
      netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4.is_addr_purge_pend = FALSE;
      netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6.is_addr_purge_pend = FALSE;

      /* If an address purge is being initiated, set the corresponding flag */
      if (FALSE == cmd->data.info.disconnect_msg.teardown_iface)
      {
        if (WDS_IP_FAMILY_IPV4_V01 == ip_family)
        {
          netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4.is_addr_purge_pend = TRUE;
        }
        else
        {
          netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6.is_addr_purge_pend = TRUE;
        }
      }

      //Decrement pkt_srvc_count for whichever client disconnect was received for
      if(WDS_IP_FAMILY_IPV4_V01 == ip_family)
      {
        /* After the link goes down mark the MTU value to invalid */
        netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv4 = NETMGR_MTU_INVALID;
        netmgr_qmi_cfg.links[ link ].wds_info.v4_pkt_conn = FALSE;
        netmgr_log_low("Setting default mtu on link for ipv4");
      }
      else if (WDS_IP_FAMILY_IPV6_V01 == ip_family)
      {
        /* After the link goes down mark the MTU value to invalid */
        netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv6 = NETMGR_MTU_INVALID;
        netmgr_qmi_cfg.links[ link ].wds_info.v6_pkt_conn = FALSE;
        netmgr_log_low("Setting default mtu on link for ipv6");
      }

      netmgr_log_low("V4 and V6 pkt_srvc_counts are : [%d] [%d]",
                      netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn,
                      netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn);


      netmgr_log_med("Posting disconnect event - family[%d] teardown[%d]\n",
                     ip_family, cmd->data.info.disconnect_msg.teardown_iface );

      if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) ) {
        NETMGR_ABORT("netmgr_wds_pkt_srvc_status_ind: failed to put commmand\n");
        netmgr_exec_release_cmd(cmd);
        return;
      }
      netmgr_l2s_format_msg(link,"WDS_DISCONNECTED IPV%d",
                           (ip_family == WDS_IP_FAMILY_IPV4_V01)? 4 : 6);

      /* Clear disconnected address from cached valid mask if specified */
      if (ind_data->ip_family_valid)
      {
        netmgr_log_low("clearing valid_mask for ip_family %d for link[%d]",ip_family,link);
        netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask &=
          (WDS_IP_FAMILY_IPV4_V01 == ip_family)?
          (unsigned int)~NETMGR_ADDRSET_MASK_IPV4 : (unsigned int)~NETMGR_ADDRSET_MASK_IPV6;
      }
    }
    break;

    case WDS_CONNECTION_STATUS_SUSPENDED_V01:
    case WDS_CONNECTION_STATUS_AUTHENTICATING_V01:
    default:
      /* Ignore in all other as this message is unsupported */
      netmgr_log_err("netmgr_wds_pkt_srvc_status_ind called, ignoring\n");
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_query_modem_mtu
===========================================================================*/
/*!
@brief
  Function to query Modem MTU information for specific link.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - netmgr_qmi_init() must have been called

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int netmgr_qmi_query_modem_mtu
(
  uint8  link
)
{
  qmi_client_type  client_hndl;
  wds_get_runtime_settings_resp_msg_v01  call_info;
  int ret = NETMGR_FAILURE;
  int qmi_result = QMI_INTERNAL_ERR;
  int result, err_code;
  unsigned long *mtu_ptr = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  /* Validate input */
  if (NETMGR_SUCCESS != netmgr_qmi_verify_link(link))
  {
    netmgr_log_err("netmgr_qmi_query_modem_mtu: invalid input param\n");
    goto bail;
  }

  /* Start with the IPv4 client handle */
  client_hndl = netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4];
  mtu_ptr = &netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv4;

  netmgr_log_med("netmgr_qmi_query_modem_mtu: trying with IPv4 handle\n");

  /* Query Modem for requested parameters */
  if (NETMGR_SUCCESS != netmgr_qmi_wds_get_runtime_settings( link,
                                                             client_hndl,
                                                             QMI_WDS_MASK_REQ_SETTINGS_MTU_V01,
                                                             &call_info ))

  {
    goto bail;
  }

  if (call_info.resp.result == QMI_RESULT_SUCCESS_V01)
  {
    qmi_result = QMI_NO_ERR;
  }
  /* If the WDS client says that there is not an active call then try using the IPv6 handle */
  else if (call_info.resp.result == QMI_RESULT_FAILURE_V01 &&
           call_info.resp.error == QMI_ERR_OUT_OF_CALL_V01)
  {
    netmgr_log_med("netmgr_qmi_query_modem_mtu: trying with IPv6 handle\n");

    client_hndl = netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6];
    mtu_ptr = &netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv6;

    if (NETMGR_SUCCESS != netmgr_qmi_wds_get_runtime_settings( link,
                                                               client_hndl,
                                                               QMI_WDS_MASK_REQ_SETTINGS_MTU_V01,
                                                               &call_info ))
    {
      goto bail;
    }

    if (call_info.resp.result == QMI_RESULT_SUCCESS_V01)
    {
      qmi_result = QMI_NO_ERR;
    }
  }

  if (QMI_NO_ERR != qmi_result)
  {
    netmgr_log_err("netmgr_qmi_query_modem_mtu: failed on netmgr_qmi_wds_get_runtime_settings: "
                   "ret=%d err=%d\n",
                   call_info.resp.result, call_info.resp.error);
    goto bail;
  }

  /* Cache results in WDS state info */
  if (TRUE == call_info.mtu_valid)
  {
    *mtu_ptr = (NETMGR_MTU_MAX < call_info.mtu)? NETMGR_MTU_MAX : call_info.mtu;
     netmgr_log_med("%s: received link MTU: %d from modem, using %lu\n",
                    __func__, call_info.mtu, *mtu_ptr);
  }
  else
  {
    netmgr_log_err("netmgr_qmi_query_modem_mtu: didn't receive link MTU from modem\n");
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_wds_event_report_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming WDS Event Report Indication message.
 This function is executed in the Command Thread context. This function
 currently reports 'physlink events' to clients.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_wds_event_report_ind
(
  int                                  link,
  const wds_event_report_ind_msg_v01   *ind_data
)
{
  netmgr_exec_cmd_t * cmd;

  NETMGR_LOG_FUNC_ENTRY;

  /* Verify link id */
  NETMGR_ASSERT( netmgr_qmi_verify_link(link) == NETMGR_SUCCESS );

  /* Verify indication data */
  NETMGR_ASSERT( ind_data != NULL );

  /* Check event mask, process acoordingly... */

  if (TRUE == ind_data->bearer_tech_ex_valid)
  {
#ifdef FEATURE_DATA_IWLAN
    wds_bearer_tech_info_type_v01  *prev_bt = NULL;
    prev_bt = &netmgr_qmi_cfg.links[link].wds_info.bearer_tech_ex.bt;
#endif /* FEATURE_DATA_IWLAN */

    netmgr_log_high("bearer_tech_ex_ind: link=%d technology=0x%x rat_mask=0x%08x so_mask=0x%016llx\n",
                    link,
                    ind_data->bearer_tech_ex.technology,
                    ind_data->bearer_tech_ex.rat_value,
                    (unsigned long long)ind_data->bearer_tech_ex.so_mask);

#ifdef FEATURE_DATA_IWLAN
    /* Consider only valid bearer techs */
    if (WDS_BEARER_TECH_RAT_EX_NULL_BEARER_V01 == ind_data->bearer_tech_ex.rat_value)
    {
      netmgr_log_med("bearer_tech_ex_ind: NULL bearer for link=%d, ignoring\n",
                     link);
      goto bail;
    }

    if (TRUE != netmgr_qmi_cfg.links[link].wds_info.bearer_tech_ex.is_valid)
    {
      netmgr_log_med("bearer_tech_ex_ind: prev bearer_tech invalid for link=%d, ignoring\n",
                     link);
    }
    else
    {
      const wds_bearer_tech_info_type_v01  *curr_bt = NULL;
      boolean is_prev_bt_iwlan = FALSE, is_curr_bt_iwlan = FALSE;

      curr_bt = &ind_data->bearer_tech_ex;

      is_prev_bt_iwlan = NETMGR_QMI_IS_BEARER_TECH_EX_IWLAN(prev_bt);
      is_curr_bt_iwlan = NETMGR_QMI_IS_BEARER_TECH_EX_IWLAN(curr_bt);

      netmgr_log_med("bearer_tech_ex_ind: link=%d is_prev_bt_iwlan=%d is_curr_bt_iwlan=%d\n",
                     link,
                     is_prev_bt_iwlan,
                     is_curr_bt_iwlan);

      /* Update the interface MTU if moving between iWLAN and WWAN */
      if ((FALSE == is_prev_bt_iwlan && TRUE == is_curr_bt_iwlan) ||
          (TRUE == is_prev_bt_iwlan && FALSE == is_curr_bt_iwlan))
      {
         if (NETMGR_SUCCESS == netmgr_qmi_query_modem_mtu((uint8)link))
         {
           netmgr_log_med("bearer_tech_ex_ind: link=%d transition=%s updating Modem MTU\n",
                          link,
                          (TRUE == is_prev_bt_iwlan) ? "IWLAN -> WWAN" : "WWAN -> IWLAN");
           (void)netmgr_kif_set_mtu(link, TRUE);
         }
         else
         {
           netmgr_log_err("bearer_tech_ex_ind: link=%d transition=%s MTU query failed\n",
                          link,
                          (TRUE == is_prev_bt_iwlan) ? "IWLAN -> WWAN" : "WWAN -> IWLAN");
         }
      }
    }

    /* Save the previous bearer tech */
    *prev_bt = ind_data->bearer_tech_ex;
    netmgr_qmi_cfg.links[link].wds_info.bearer_tech_ex.is_valid = TRUE;
#endif /* FEATURE_DATA_IWLAN */
  }
  /* (QMI_WDS_EVENT_BEARER_TECH_IND & ind_data->event_report.event_mask)*/
  else if (TRUE == ind_data->current_bearer_tech_valid)
  {
    netmgr_log_high("Bearer tech change current NW:%dx, SO mask: %d, rat mask:%d\n",
                    ind_data->current_bearer_tech.current_nw,
                    ind_data->current_bearer_tech.so_mask,
                    ind_data->current_bearer_tech.rat_mask);
    /* When Bearer tech changes, network service might adopt different configuration
     need to query and reset MTU in the kernel for the new network service */
    if( NETMGR_SUCCESS ==
        netmgr_qmi_get_modem_link_info((uint8)link, WDS_IP_FAMILY_IPV4_V01))
    {
      (void)netmgr_kif_set_mtu(link, TRUE);
    }
    else
    {
      netmgr_log_err("qmi query modem link info failed!\n ");
    }
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_wds_ext_ip_config_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming WDS Extended IP Indication message.
 This function is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void netmgr_wds_ext_ip_config_ind
(
  int                                 link,
  qmi_client_type                     user_handle,
  const wds_extended_ip_config_ind_msg_v01  *ind_data
)
{
  NETMGR_LOG_FUNC_ENTRY;
  netmgr_nl_event_info_t * event_info = NULL;
  netmgr_kif_state_t kif_state = netmgr_kif_get_state(link);
  const char * iface_name = NULL;
  netmgr_qmi_client_type_t  clnt;
  netmgr_exec_cmd_t * cmd_buf = NULL;

  /* If kernel iface is not completely configured, we can drop the indication as link updates info will
  be fetched anyways after kernel iface is configured, using get runtime settings */
  if( kif_state != NETMGR_KIF_OPEN )
  {
    netmgr_log_high("netmgr_wds_ext_ip_config_ind: netmgr kif is not configured yet."
                    "Dropping the EXT_IP indication");
    goto bail;
  }

  iface_name = netmgr_kif_get_name(link);
  if( NULL == iface_name)
  {
    netmgr_log_err("%s(): unable to determine name for link=%d\n", __func__, link);
    goto bail;
  }

  /* If indication is for DNS info, send RECONFIGURED event to cleints */
  if( ind_data->changed_ip_config_valid &&
      ind_data->changed_ip_config & QMI_WDS_MASK_REQ_SETTINGS_DNS_ADDR_V01 ) {
    /* Post reconfigured indication to clients */
    event_info = netmgr_malloc( sizeof(netmgr_nl_event_info_t) );
    if( NULL == event_info ) {
      netmgr_log_err("Failed to allocate event buffer. Discarding EXT_IP ind");
      goto bail;
    }
    else {
      memset( event_info, 0x0, sizeof(netmgr_nl_event_info_t) );
      event_info->event = NET_PLATFORM_RECONFIGURED_EV;

      event_info->addr_info.addr.ip_addr.ss_family = AF_INET6;
      event_info->param_mask |= NETMGR_EVT_PARAM_IPADDR;

      event_info->link = link;
      event_info->param_mask |= NETMGR_EVT_PARAM_LINK;
      strlcpy( event_info->dev_name,
               iface_name,
               sizeof(event_info->dev_name) );
      event_info->param_mask |= NETMGR_EVT_PARAM_DEVNAME;

      if( NETMGR_SUCCESS != netmgr_kif_send_event_msg( event_info ) ) {
        netmgr_log_err("Failed on kif_send_event RECONFIGURED\n");
      }
      else {
        netmgr_log_high("kif_send_event[RECONFIGURED]: Success\n");
      }
    }
    netmgr_free( event_info );
  }

/*Perform handling of MTU updates*/
  if(ind_data->changed_ip_config_valid &&
     ind_data->changed_ip_config & QMI_WDS_MASK_REQ_SETTINGS_MTU_V01)
  {
    if (user_handle == netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
    {
      netmgr_qmi_get_modem_link_info((uint8)link, WDS_IP_FAMILY_IPV4_V01);
      netmgr_log_low("Get runtime setting for ipv4 clients");
    }
    else if (user_handle == netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
    {
      netmgr_qmi_get_modem_link_info((uint8)link, WDS_IP_FAMILY_IPV6_V01);
      netmgr_log_low("Get runtime setting for ipv6 clients");
    }
    if(NETMGR_SUCCESS != netmgr_kif_update_link(link))
    {
      netmgr_log_err("Failed to update MTU on link %d",link);
    }
  }
bail:
  NETMGR_LOG_FUNC_EXIT;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_wds_handle_rev_ip_connect_ind
===========================================================================*/
/*!
@brief
  Handler for the reverse IP "connected" transport connection indication

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_wds_handle_rev_ip_connect_ind
(
  int                                                    link,
  qmi_client_type                                        user_handle,
  const wds_reverse_ip_transport_connection_ind_msg_v01  *rev_ip_ind
)
{
  netmgr_exec_cmd_t  *cmd = NULL;
  netmgr_address_info_t  *addr_info_ptr = NULL;
  netmgr_address_set_t  *addr_set_ptr = NULL;
  netmgr_wds_qmi_drv_info_t  *wds_info_ptr = NULL;
  int ip_type;
  netmgr_qmi_client_type_t  clnt;
  int rc = NETMGR_FAILURE;

  NETMGR_ASSERT( rev_ip_ind != NULL );

  NETMGR_LOG_FUNC_ENTRY;

  /* Ensure that we got the indication on the correctly bound WDS client */
  if (user_handle == netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    if (!(rev_ip_ind->ipv4_addr_valid))
    {
      netmgr_log_err("netmgr_wds_handle_rev_ip_connect_ind: indication on V4 WDS client without address\n");
      goto bail;
    }
    clnt = NETMGR_QMI_CLIENT_IPV4;
  }
  else if (user_handle == netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    if (!(rev_ip_ind->ipv6_addr_valid))
    {
      netmgr_log_err("netmgr_wds_handle_rev_ip_connect_ind: indication on V6 WDS client without address\n");
      goto bail;
    }
    clnt = NETMGR_QMI_CLIENT_IPV6;
  }
  else
  {
    netmgr_log_err("netmgr_wds_handle_rev_ip_connect_ind: invalid handle=%p received\n",
                   user_handle);
    goto bail;
  }

  if (!(rev_ip_ind->tech_name_valid))
  {
    netmgr_log_err("netmgr_wds_handle_rev_ip_connect_ind: missing technology\n");
    goto bail;
  }
  else if (WDS_TECHNOLOGY_NAME_WLAN_LOCAL_BRKOUT_V01 != rev_ip_ind->tech_name &&
           WDS_TECHNOLOGY_NAME_IWLAN_S2B_V01 != rev_ip_ind->tech_name)
  {
    netmgr_log_err("netmgr_wds_handle_rev_ip_connect_ind: unexpected technology=0x%x\n",
                   rev_ip_ind->tech_name);
    goto bail;
  }

  /* Allocate address buffer; released in SM module */
  addr_set_ptr = ds_malloc( sizeof(netmgr_address_set_t) );
  if (NULL == addr_set_ptr)
  {
    netmgr_log_err("failed to allocate address_set\n");
    goto bail;
  }

  memset(addr_set_ptr, 0, sizeof(*addr_set_ptr));

  /* Enqueue the reverse ip txn id */
  if (NETMGR_SUCCESS != netmgr_qmi_wds_util_enq_txn(clnt, link, rev_ip_ind->transaction_id))
  {
    netmgr_log_err("failed to enqueue txn=%ld, clnt=%d, link=%d\n",
                   (long)rev_ip_ind->transaction_id,
                   clnt,
                   link);
    goto bail;
  }

  wds_info_ptr = &netmgr_qmi_cfg.links[ link ].wds_info;
  addr_info_ptr = &wds_info_ptr->addr_info;

  netmgr_log_low("netmgr_wds_handle_rev_ip_connect_ind: tech_name=0x%x, txn_id=%ld, clnt=%d, link=%d\n",
                 rev_ip_ind->tech_name,
                 (long)rev_ip_ind->transaction_id,
                 clnt,
                 link);

  wds_info_ptr->tech_name = rev_ip_ind->tech_name;

  if (NETMGR_QMI_CLIENT_MAX == netmgr_qmi_cfg.links[link].wds_info.first_conn_clnt
      || (netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn == FALSE &&
         netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn == FALSE)
     )
  {
    wds_info_ptr->first_conn_clnt = clnt;
  }

  /* Store the address and send a NETMGR_WDS_CONNECTED_EV and increment respective
     pkt_srvc_conn indication counter*/
  if (rev_ip_ind->ipv4_addr_valid)
  {
    netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn = TRUE;
    addr_info_ptr->valid_mask |= NETMGR_ADDRSET_MASK_IPV4;
    addr_info_ptr->ipv4.if_addr.type = NETMGR_IPV4_ADDR;
    addr_info_ptr->ipv4.if_addr.addr.v4 = htonl((uint32_t)rev_ip_ind->ipv4_addr);
    addr_info_ptr->ipv4.sa.is_sa_valid = FALSE;
    addr_info_ptr->ipv4.sa.is_sa_shared = FALSE;
    addr_info_ptr->ipv4.sa.ts_mode = NETMGR_TS_MODE_DISABLED;

    if (rev_ip_ind->is_shared_valid)
    {
      addr_info_ptr->ipv4.sa.is_sa_shared = rev_ip_ind->is_shared;
    }

    NETMGR_LOG_IPV4_ADDR( med, "Modem iface", addr_info_ptr->ipv4.if_addr.addr.v4 );

    *addr_set_ptr = addr_info_ptr->ipv4;
  }
  else if (rev_ip_ind->ipv6_addr_valid)
  {
    netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn = TRUE;
    addr_info_ptr->valid_mask |= NETMGR_ADDRSET_MASK_IPV6;
    addr_info_ptr->ipv6.if_addr.type = NETMGR_IPV6_ADDR;
    addr_info_ptr->ipv6.if_mask = (unsigned int)rev_ip_ind->ipv6_addr.ipv6_prefix_length;
    memcpy(addr_info_ptr->ipv6.if_addr.addr.v6_addr8,
           rev_ip_ind->ipv6_addr.ipv6_addr,
           sizeof(addr_info_ptr->ipv6.if_addr.addr.v6_addr8));

    addr_info_ptr->ipv6.sa.is_sa_valid = FALSE;
    addr_info_ptr->ipv6.sa.is_sa_shared = FALSE;
    addr_info_ptr->ipv6.sa.ts_mode = NETMGR_TS_MODE_DISABLED;

    if (rev_ip_ind->is_shared_valid)
    {
      addr_info_ptr->ipv6.sa.is_sa_shared = rev_ip_ind->is_shared;
    }

    NETMGR_LOG_IPV6_ADDR( med, "Modem iface",
                          addr_info_ptr->ipv6.if_addr.addr.v6_addr64 );

    *addr_set_ptr = addr_info_ptr->ipv6;
  }

  netmgr_log_low("V4 and V6 pkt_srvc_counts are : [%d] [%d]",
                   netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn,
                   netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn);

  /*-------------------------------------------------------------------------
    Post message to executive
  -------------------------------------------------------------------------*/
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

  cmd->data.type = NETMGR_WDS_CONNECTED_EV;
  cmd->data.link = link;
  cmd->data.info.connect_msg.reconfig_required = FALSE;
  cmd->data.info.connect_msg.addr_info_ptr = addr_set_ptr;

  netmgr_log_med("Posting reverse connect event - family[%d] \n",
                 clnt);

  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) ) {
    NETMGR_ABORT("netmgr_wds_handle_rev_ip_connect_ind: failed to put commmand\n");
    netmgr_exec_release_cmd(cmd);
    goto bail;
  }

  rc = NETMGR_SUCCESS;

bail:
  if (NETMGR_SUCCESS != rc)
  {
    if (NULL != addr_set_ptr)
    {
      ds_free( addr_set_ptr );
    }
  }

  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_wds_handle_rev_ip_disconnect_ind
===========================================================================*/
/*!
@brief
  Handler for the reverse IP "disconnected" transport connection indication
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_wds_handle_rev_ip_disconnect_ind
(
  int                                                    link,
  qmi_client_type                                        user_handle,
  const wds_reverse_ip_transport_connection_ind_msg_v01  *rev_ip_ind
)
{
  netmgr_exec_cmd_t  *cmd = NULL;
  netmgr_qmi_client_type_t  clnt;

  NETMGR_ASSERT( rev_ip_ind != NULL );

  NETMGR_LOG_FUNC_ENTRY;

  /* Modem interface has disconnected. */

  /* Determine the IP type of the call that got disconnected */
  if (user_handle == netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    clnt = NETMGR_QMI_CLIENT_IPV4;
  }
  else if (user_handle == netmgr_qmi_cfg.links[link].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    clnt = NETMGR_QMI_CLIENT_IPV6;
  }
  else
  {
    netmgr_log_err("netmgr_wds_handle_rev_ip_disconnect_ind: invalid handle=0x%p received\n",
                   user_handle);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_wds_util_enq_txn(clnt, link, rev_ip_ind->transaction_id))
  {
    netmgr_log_err("failed to enqueue txn=%ld, clnt=%d, link=%d\n",
                   (long)rev_ip_ind->transaction_id,
                   clnt,
                   link);
    goto bail;
  }

  netmgr_log_low("netmgr_wds_handle_rev_ip_disconnect_ind: tech_name=0x%x, txn_id=%ld, clnt=%d,"
                 " link=%d\n",
                 rev_ip_ind->tech_name,
                 (long)rev_ip_ind->transaction_id,
                 clnt,
                 link);

  /*-------------------------------------------------------------------------
    Post message to executive
  -------------------------------------------------------------------------*/
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

  cmd->data.type = NETMGR_WDS_DISCONNECTED_EV;
  cmd->data.link = link;

   if (clnt == NETMGR_QMI_CLIENT_IPV6){
     netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn = FALSE;
   }
   else if (clnt == NETMGR_QMI_CLIENT_IPV4){
     netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn = FALSE;
   }

   netmgr_log_low("V4 and V6 pkt_srvc_counts are : [%d] [%d]",
                   netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn,
                   netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn);

  /* Set the interface teardown flag.  For single-IP calls, the
   * interface will teardown when disconnect indication arrives.
   * For dual-IP calls, it may be two IP addresses are on a single
   * interface.  So the first disconnect must purge the specified
   * IP addreess, yet the other address lives on and network
   * interface state remains unchanged.  We clear the valid_mask
   * below for this address family to indicate the interface is
   * now effectively a single-IP call.  The next disconnect
   * indication will then teardown interface as normal.
   */
  cmd->data.info.disconnect_msg.teardown_iface       =
    (NETMGR_ADDRSET_MASK_IPV4V6 != netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask);
  cmd->data.info.disconnect_msg.addr_info_ptr   =
    (NETMGR_QMI_CLIENT_IPV6 == clnt)?
    &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6 :
    &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4;

  /* Clear the address purge pending flags */
  netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4.is_addr_purge_pend = FALSE;
  netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6.is_addr_purge_pend = FALSE;

  /* If an address purge is being initiated, set the corresponding flag */
  if (FALSE == cmd->data.info.disconnect_msg.teardown_iface)
  {
    if (NETMGR_QMI_CLIENT_IPV4 == clnt)
    {
      netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4.is_addr_purge_pend = TRUE;
    }
    else
    {
      netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6.is_addr_purge_pend = TRUE;
    }
  }

  /* Clear disconnected address from cached valid mask if specified */
  netmgr_qmi_cfg.links[ link ].wds_info.addr_info.valid_mask &= (NETMGR_QMI_CLIENT_IPV4 == clnt) ?
                                                                (unsigned int)~NETMGR_ADDRSET_MASK_IPV4 :
                                                                (unsigned int)~NETMGR_ADDRSET_MASK_IPV6;

  netmgr_log_med("Posting reverse disconnect event - family[%d] \n",
                 clnt);

  if (NETMGR_SUCCESS != netmgr_exec_put_cmd(cmd))
  {
    NETMGR_ABORT("netmgr_wds_pkt_srvc_status_ind: failed to put commmand\n");
    netmgr_exec_release_cmd(cmd);
    return;
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_wds_rev_ip_trans_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming WDS reverse IP transport connection
 indication. This function is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_wds_rev_ip_trans_conn_ind
(
  int                                                    link,
  qmi_client_type                                        user_handle,
  const wds_reverse_ip_transport_connection_ind_msg_v01  *ind_data
)
{
  NETMGR_LOG_FUNC_ENTRY;

  /* Verify link id */
  NETMGR_ASSERT( netmgr_qmi_verify_link(link) == NETMGR_SUCCESS );

  /* Verify indication data */
  NETMGR_ASSERT( ind_data != NULL );

  switch (ind_data->rev_ip_transport_conn_status)
  {
     case WDS_REVERSE_IP_TRANSPORT_CONNECTED_V01:
      netmgr_wds_handle_rev_ip_connect_ind(link, user_handle, ind_data);
      netmgr_l2s_msg(link,"REV_IP_TRANS_CONNECTED");
      break;

    case WDS_REVERSE_IP_TRANSPORT_DISCONNECTED_V01:
      netmgr_wds_handle_rev_ip_disconnect_ind(link, user_handle, ind_data);
      netmgr_l2s_msg(link,"REV_IP_TRANS_DISCONNECTED");
      break;

    default:
      netmgr_log_err("netmgr_wds_rev_ip_trans_conn_ind: unknown conn_status=%d\n",
                     ind_data->rev_ip_transport_conn_status);
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_wds_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming WDS Indication message. This function
 is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_wds_ind
(
  netmgr_qmi_wds_ind_t  *netmgr_qmi_wds_ind
)
{
  /* Verify input parameter */
  NETMGR_ASSERT( netmgr_qmi_wds_ind != NULL );

  /* Verify link id is valid before proceeding */
  NETMGR_ASSERT(netmgr_qmi_verify_link(netmgr_qmi_wds_ind->link) == NETMGR_SUCCESS);

  /* Process based on indication type */
  switch (netmgr_qmi_wds_ind->ind_id) {
    case QMI_WDS_PKT_SRVC_STATUS_IND_V01:
      /* Process packet service status indication */
      netmgr_wds_pkt_srvc_status_ind(netmgr_qmi_wds_ind->link,
                                     netmgr_qmi_wds_ind->user_hndl,
                                     &netmgr_qmi_wds_ind->ind_data.pkt_srvc_status);
      break;

    case QMI_WDS_EVENT_REPORT_IND_V01:
      /* Process Event Report indication */
      netmgr_wds_event_report_ind(netmgr_qmi_wds_ind->link,
                                  &netmgr_qmi_wds_ind->ind_data.event_report);

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
      netmgr_util_wakelock_release();
#endif
      break;

#ifdef FEATURE_DATA_IWLAN
    case QMI_WDS_REVERSE_IP_TRANSPORT_CONNECTION_IND_V01:
      /* Process Event Report indication */
      netmgr_wds_rev_ip_trans_conn_ind(netmgr_qmi_wds_ind->link,
                                       netmgr_qmi_wds_ind->user_hndl,
                                       &netmgr_qmi_wds_ind->ind_data.rev_ip_conn_ind);
      break;
#endif /* FEATURE_DATA_IWLAN */

    case QMI_WDS_EXTENDED_IP_CONFIG_IND_V01:
      /* Process Extended IP indication */
      netmgr_log_high("Processing Extended IP indication");
      netmgr_wds_ext_ip_config_ind(netmgr_qmi_wds_ind->link,
                                   netmgr_qmi_wds_ind->user_hndl,
                                   &(netmgr_qmi_wds_ind->ind_data.ext_ip_ind));

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
      netmgr_util_wakelock_release();
#endif
      break;

  case QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01:
#ifdef NETMGR_QOS_ENABLED
    do
    {
      int i = 0;
      int max_links = netmgr_main_cfg.num_data_ports;
      unsigned long rcvd_tc_ul_baserate_val = 0;

      rcvd_tc_ul_baserate_val = (netmgr_qmi_wds_ind->ind_data.tput_info_ind.ul_configured_throughput)*1000 + NETMGR_TC_BASE_OFFSET;

      netmgr_log_med("%s(): Processing configured throughput indication - UL throughput %u",
                     __func__, netmgr_qmi_wds_ind->ind_data.tput_info_ind.ul_configured_throughput);

      if (netmgr_qmi_wds_ind->ind_data.tput_info_ind.ul_configured_throughput == 0)
      {
        netmgr_log_err("Modem is reporting 0!, not updating tc_ul_baserate");
        break;
      }
      else if (rcvd_tc_ul_baserate_val == netmgr_main_cfg.tc_ul_baserate)
      {
        netmgr_log_med("%s(): Received tc ul baserate [%lu] is same as present in config [%lu], skipping update",
                        __func__,
                        rcvd_tc_ul_baserate_val,
                        netmgr_main_cfg.tc_ul_baserate);
        break;
      }
      else
      {
        netmgr_main_cfg.tc_ul_baserate = rcvd_tc_ul_baserate_val;
        netmgr_log_med("New tc_ul_baserate is %lu", netmgr_main_cfg.tc_ul_baserate);
      }

      for (i = 0; i < max_links; i++)
      {
        netmgr_log_med("%s(): updating link %d", __func__, i);
        netmgr_tc_flow_update_data_rate_hdlr(i, netmgr_main_cfg.tc_ul_baserate);
      }
    }while(0);

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
      netmgr_util_wakelock_release();
#endif
#endif /* NETMGR_QOS_ENABLED */
    break;

    default:
      /* Ignore all other indications */
      netmgr_log_high("Ignoring QMI WDS IND of type %d\n", netmgr_qmi_wds_ind->ind_id);
  }

  return;
}

#ifdef NETMGR_QOS_ENABLED
/*===========================================================================
  FUNCTION  netmgr_qos_flow_decode_umts_params
===========================================================================*/
/*!
@brief
  Decodes the QoS specificaiton UMTS parameters for priority and datarate

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qos_flow_decode_umts_params
(
  const netmgr_qos_umts_flow_desc_type * umts_desc,
  uint8                             * priority,
  uint32                            * datarate
)
{
  NETMGR_ASSERT( umts_desc );
  NETMGR_ASSERT( priority );
  NETMGR_ASSERT( datarate );

  NETMGR_LOG_FUNC_ENTRY;

  /* Decode datarate paramater */
  if( umts_desc->param_mask & NETMGR_QOS_UMTS_FLOW_PARAM_DATA_RATE ) {
    /* Use guaranteed datarate value */
    *datarate = (uint32)umts_desc->data_rate.guaranteed_rate;
  }
  else {
    /* Assume default datarate */
    netmgr_log_err("undefined data rate, assuming default\n");
    *datarate = NETMGR_TC_DEFAULT_DATARATE;
  }

  /* Decode priority paramater.  This is derived from traffic class */
  if( umts_desc->param_mask & NETMGR_QOS_UMTS_FLOW_PARAM_TRAFFIC_CLASS ) {
    switch( umts_desc->traffic_class ) {
      case NETMGR_QOS_UMTS_TC_CONVERSATIONAL:
        *priority = NETMGR_TC_CLASS_PRIO_CONVERSATIONAL;
        break;
      case NETMGR_QOS_UMTS_TC_STREAMING:
        *priority = NETMGR_TC_CLASS_PRIO_STREAMING;
        break;
      case NETMGR_QOS_UMTS_TC_INTERACTIVE:
        *priority = NETMGR_TC_CLASS_PRIO_INTERACTIVE;
        break;
      case NETMGR_QOS_UMTS_TC_BACKGROUND:
        *priority = NETMGR_TC_CLASS_PRIO_BACKGROUND;
        break;
      default:
        /* Assume default datarate */
        netmgr_log_err("unknown traffic class, assuming default\n");
        *priority = NETMGR_TC_DEFAULT_PRIORITY;
        break;
    }
  }
  else {
    /* Assume default datarate */
    netmgr_log_err("undefined traffic class, assuming default\n");
    *priority = NETMGR_TC_DEFAULT_PRIORITY;
  }

  NETMGR_LOG_FUNC_EXIT;

  return NETMGR_SUCCESS;
}


/*===========================================================================
  FUNCTION  netmgr_qos_flow_decode_cdma_params
===========================================================================*/
/*!
@brief
  Decodes the QoS specificaiton CDMA parameters for priority and datarate

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qos_flow_decode_cdma_params
(
  const netmgr_qos_cdma_flow_desc_type * cdma_desc,
  uint8                             * priority,
  uint32                            * datarate
)
{
  NETMGR_ASSERT( cdma_desc );
  NETMGR_ASSERT( priority );
  NETMGR_ASSERT( datarate );

  /* Lookup the parameters based on the profile ID */
  if( cdma_desc->param_mask & NETMGR_QOS_CDMA_FLOW_PARAM_PROFILE_ID ) {
    if( NETMGR_SUCCESS !=
        netmgr_tc_get_qos_params_by_profile_id( (uint16)cdma_desc->profile_id,
                                                datarate,
                                                priority ) ) {

      netmgr_log_err( "Error decoding CDMA TX flow data!\n");
      return NETMGR_FAILURE;
    }

  }
  else {
    netmgr_log_err("undefined profile ID, assuming defaults\n");
    *priority = NETMGR_TC_DEFAULT_PRIORITY;
    *datarate = NETMGR_TC_DEFAULT_DATARATE;
  }
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_qos_flow_activated
===========================================================================*/
/*!
@brief
  Performs processing of an QoS flow activated notification.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_flow_activated
(
  int                                   link,
  netmgr_qos_event_report_flow_info_type * flow_ptr
)
{
  netmgr_exec_cmd_t * cmd = NULL;
  uint8     priority = NETMGR_TC_DEFAULT_PRIORITY;
  uint32    datarate = NETMGR_TC_DEFAULT_DATARATE;
  const netmgr_qos_flow_req_type * flow = NULL;
  netmgr_qos_granted_filter_data_type * filter = NULL;
  netmgr_qmi_qos_modify_info_t *qos_modify_info = NULL;
  unsigned int i;

  NETMGR_ASSERT( flow_ptr );

  NETMGR_LOG_FUNC_ENTRY;

  /* Check if this activation is for a new flow.  For new flows, need
   * to decode QoS specification parameters.  For exisiting flows,
   * this data has previously been received. */
  if( flow_ptr->qos_flow_state.new_flow ) {

    /* Check for QoS TX flow specification */
    if( flow_ptr->tx_granted_flow_data_is_valid )
    {
      flow = &flow_ptr->tx_granted_flow_data.qos_flow_granted;

      /* Check for UMTS parameter set */
      if( flow->umts_flow_desc.param_mask ) {
        if( NETMGR_SUCCESS !=
            netmgr_qos_flow_decode_umts_params( &flow->umts_flow_desc,
                                                &priority,
                                                &datarate ) ) {

          netmgr_log_err( "Error decoding UMTS TX flow data!\n");
          return;
        }
      }
      /* Check for CDMA parameter set */
      else if( flow->cdma_flow_desc.param_mask ) {
        if( NETMGR_SUCCESS !=
            netmgr_qos_flow_decode_cdma_params( &flow->cdma_flow_desc,
                                                &priority,
                                                &datarate ) ) {

          netmgr_log_err( "Error decoding CDMA TX flow data!\n");
          return;
        }
      }
      else {
        netmgr_log_err( "TX flow data undefined, using defaults!\n");
      }
    }
    else {
      /* No specification available, assume default values */
      netmgr_log_err( "TX flow data undefined, using defaults!\n");
    }
  }

  /* Allocate a command object */
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

  if ( netmgr_main_get_skip_buffered_qos_modify() != TRUE)
  {
      netmgr_log_high("Feature to skip buffered QoS modify is not enabled for this target");
  }
  else
  {
      /*Update QoS cache before pushing command*/
      qos_modify_info = netmgr_qmi_qos_modify_info_alloc();

      if ( qos_modify_info != NULL )
      {
          qos_modify_info->flow_id = (uint32)flow_ptr->qos_flow_state.qos_identifier;
          qos_modify_info->rcvd_qos_modify_counter = 0;

          netmgr_log_low("enqueing obj in QoS Modify cache with details : "
                  "flow_id = [%d] \n"
                  "rcvd_qos_modify_counter = [%d]",
                  qos_modify_info->flow_id,
                  qos_modify_info->rcvd_qos_modify_counter);

          pthread_mutex_lock(&(netmgr_qos_modify_cache.modify_mutex));
          ds_dll_enq( netmgr_qos_modify_cache.links[link].flow_list, NULL, (void*)qos_modify_info);
          pthread_mutex_unlock(&(netmgr_qos_modify_cache.modify_mutex));
      }
      else
      {
          netmgr_log_err("%s(): skip_buffered_qos_modify enabled but malloc failed", __func__);
      }
  }

  /* Set command object parameters */
  cmd->data.type                                = NETMGR_QOS_ACTIVATE_EV;
  cmd->data.link                                = link;

  cmd->data.info.qos_flow.flow_id   = (uint32)flow_ptr->qos_flow_state.qos_identifier;
  cmd->data.info.qos_flow.is_new    = flow_ptr->qos_flow_state.new_flow;
  cmd->data.info.qos_flow.flow_type = (uint8)((NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM &
                                       flow_ptr->param_mask)?
                                       flow_ptr->flow_type : NETMGR_FLOW_TYPE_INVALID);
  cmd->data.info.qos_flow.priority  = priority;
  cmd->data.info.qos_flow.datarate  = datarate;

  cmd->data.info.qos_flow.num_filter = (uint8)flow_ptr->tx_filter_count;
  memcpy( (void*)cmd->data.info.qos_flow.filter_list,
          flow_ptr->tx_granted_filter_data,
          sizeof(cmd->data.info.qos_flow.filter_list) );

  /* Post command for processing in the command thread context */
  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) ) {
    NETMGR_ABORT("netmgr_qos_flow_activated: failed to put commmand\n");
    netmgr_exec_release_cmd(cmd);
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qos_flow_modified
===========================================================================*/
/*!
@brief
  Performs processing of an QoS flow modified notification.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_flow_modified
(
  int                                  link,
  netmgr_qos_event_report_flow_info_type * flow_ptr
)
{
  netmgr_exec_cmd_t* cmd = NULL;
  uint8 priority = NETMGR_TC_DEFAULT_PRIORITY;
  uint32 datarate = NETMGR_TC_DEFAULT_DATARATE;
  const netmgr_qos_flow_req_type* flow = NULL;
  netmgr_qmi_qos_modify_info_t *qos_modify_info = NULL;
  int err;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( flow_ptr );

  // Check for Qos TX flow specification
  if (flow_ptr->tx_granted_flow_data_is_valid)
  {
    flow = &flow_ptr->tx_granted_flow_data.qos_flow_granted;

    if (flow->umts_flow_desc.param_mask)
    {
      err = netmgr_qos_flow_decode_umts_params(&flow->umts_flow_desc,
                                               &priority,
                                               &datarate);

      if (err != NETMGR_SUCCESS)
      {
        netmgr_log_err("Error decoding UMTS TX flow data\n");
        return;
      }
    }
    else if (flow->cdma_flow_desc.param_mask)
    {
      err = netmgr_qos_flow_decode_cdma_params(&flow->cdma_flow_desc,
                                               &priority,
                                               &datarate);

      if (err != NETMGR_SUCCESS)
      {
        netmgr_log_err("Error decoding CDMA TX flow data\n");
        return;
      }
    }
    else
    {
      // no spec provided
      netmgr_log_err("TX flow data undefined, using defaults\n");
    }
  }

  if ( netmgr_main_get_skip_buffered_qos_modify() != TRUE)
  {
      netmgr_log_high("Feature to skip buffered QoS modify is not enabled for this target");
  }
  else
  {
      /*Search for the flow in the list and then check for count and then post
       * the event to main queue
       */
      pthread_mutex_lock(&(netmgr_qos_modify_cache.modify_mutex));

      qos_modify_info = netmgr_tc_find_qos_modify_cache_flow_info(link,
              (uint32)flow_ptr->qos_flow_state.qos_identifier);

      if ( qos_modify_info != NULL )
      {
          /*Since now posting the command, thus increasing the counter*/
          netmgr_log_low("flow with id [%d] found, posting qos modify event now",
                  (uint32)flow_ptr->qos_flow_state.qos_identifier);
          if ( netmgr_tc_incr_rcvd_qos_modify_counter(link,
                      (uint32)flow_ptr->qos_flow_state.qos_identifier) != NETMGR_SUCCESS )
          {
              netmgr_log_err("%s(): failure in incrmenting count, not posting command and exiting",
                      __func__);
              pthread_mutex_unlock(&(netmgr_qos_modify_cache.modify_mutex));
              goto out;
          }

      }

      pthread_mutex_unlock(&(netmgr_qos_modify_cache.modify_mutex));
  }


   // Allocate cmd object
   cmd = netmgr_exec_get_cmd();
   NETMGR_ASSERT(cmd);

   // Set command data
   cmd->data.type = NETMGR_QOS_MODIFY_EV;
   cmd->data.link = link;
   cmd->data.info.qos_flow.flow_id = (uint32)flow_ptr->qos_flow_state.qos_identifier;
   cmd->data.info.qos_flow.is_new = flow_ptr->qos_flow_state.new_flow;
   cmd->data.info.qos_flow.flow_type = (uint8)((NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM &
                                               flow_ptr->param_mask)?
                                               flow_ptr->flow_type : NETMGR_FLOW_TYPE_INVALID);
   cmd->data.info.qos_flow.priority = priority;
   cmd->data.info.qos_flow.datarate = datarate;

   cmd->data.info.qos_flow.num_filter = (uint8)flow_ptr->tx_filter_count;
   memcpy(cmd->data.info.qos_flow.filter_list,
             flow_ptr->tx_granted_filter_data,
             sizeof(cmd->data.info.qos_flow.filter_list));

   // Post command for processing in the command thread context
   if ( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) )
   {
       NETMGR_ABORT("netmgr_qos_flow_modified: failed to put commmand\n");
       netmgr_exec_release_cmd(cmd);
   }

out:
   NETMGR_LOG_FUNC_EXIT;
   return;
}

/*===========================================================================
  FUNCTION  netmgr_qos_flow_deleted
===========================================================================*/
/*!
@brief
  Performs processing of an QoS flow deleted notification.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_flow_deleted
(
  int                                   link,
  netmgr_qos_event_report_flow_info_type * flow_ptr
)
{
  netmgr_exec_cmd_t * cmd = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( flow_ptr );

  // Allocate cmd object
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);
  cmd->data.type = NETMGR_QOS_DELETE_EV;
  cmd->data.link = link;
  cmd->data.info.qos_flow.flow_id = (uint32)flow_ptr->qos_flow_state.qos_identifier;
  cmd->data.info.qos_flow.flow_type = (uint8)((NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM &
                                       flow_ptr->param_mask)?
                                      flow_ptr->flow_type : NETMGR_FLOW_TYPE_INVALID);

  // Post command
  if (NETMGR_SUCCESS != netmgr_exec_put_cmd(cmd))
  {
    NETMGR_ABORT("netmgr_qos_flow_delete: failed to put command");
    netmgr_exec_release_cmd(cmd);
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qos_flow_suspended
===========================================================================*/
/*!
@brief
  Performs processing of an QoS flow suspended notification.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_flow_suspended
(
  int                                         link,
  const netmgr_qos_event_report_flow_info_type   *flow_ptr
)
{
  netmgr_exec_cmd_t * cmd = NULL;
  uint8     priority = NETMGR_TC_DEFAULT_PRIORITY;
  uint32    datarate = NETMGR_TC_DEFAULT_DATARATE;
  const netmgr_qos_flow_req_type * flow = NULL;
  netmgr_qos_granted_filter_data_type * filter = NULL;
  unsigned int i;

  NETMGR_ASSERT( flow_ptr );

  NETMGR_LOG_FUNC_ENTRY;

  /* Check if this suspend is for a new flow.  For new flows, need
   * to decode QoS specification parameters.  For existing flows,
   * this data has previously been received. */
  if( flow_ptr->qos_flow_state.new_flow )
  {

    /* Check for QoS TX flow specification */
    if( flow_ptr->tx_granted_flow_data_is_valid )
    {
      flow = &flow_ptr->tx_granted_flow_data.qos_flow_granted;

      /* Check for UMTS parameter set */
      if( flow->umts_flow_desc.param_mask )
      {
        if( NETMGR_SUCCESS != netmgr_qos_flow_decode_umts_params(&flow->umts_flow_desc,
                                                                 &priority,
                                                                 &datarate))
        {

          netmgr_log_err( "Error decoding UMTS TX flow data!\n");
          return;
        }
      }
      /* Check for CDMA parameter set */
      else if( flow->cdma_flow_desc.param_mask )
      {
        if( NETMGR_SUCCESS != netmgr_qos_flow_decode_cdma_params(&flow->cdma_flow_desc,
                                                                 &priority,
                                                                 &datarate))
        {

          netmgr_log_err( "Error decoding CDMA TX flow data!\n");
          return;
        }
      }
      else
      {
        netmgr_log_err( "TX flow data undefined, using defaults!\n");
      }
    }
    else
    {
      /* No specification available, assume default values */
      netmgr_log_err( "TX flow data undefined, using defaults!\n");
    }
  }

  /* Allocate a command object */
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

  /* Set command object parameters */
  cmd->data.type                                = NETMGR_QOS_SUSPEND_EV;
  cmd->data.link                                = link;

  cmd->data.info.qos_flow.flow_id   = (uint32)flow_ptr->qos_flow_state.qos_identifier;
  cmd->data.info.qos_flow.is_new    = flow_ptr->qos_flow_state.new_flow;
  cmd->data.info.qos_flow.flow_type = (uint8)((NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM &
                                       flow_ptr->param_mask)?
                                       flow_ptr->flow_type : NETMGR_FLOW_TYPE_INVALID);
  cmd->data.info.qos_flow.priority  = priority;
  cmd->data.info.qos_flow.datarate  = datarate;

  cmd->data.info.qos_flow.num_filter = (uint8)flow_ptr->tx_filter_count;
  memcpy((void*)cmd->data.info.qos_flow.filter_list,
         flow_ptr->tx_granted_filter_data,
         sizeof(cmd->data.info.qos_flow.filter_list));

  /* Post command for processing in the command thread context */
  if(NETMGR_SUCCESS != netmgr_exec_put_cmd(cmd))
  {
    NETMGR_ABORT("netmgr_qos_flow_activated: failed to put command\n");
    netmgr_exec_release_cmd(cmd);
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qos_flow_control
===========================================================================*/
/*!
@brief
  Performs processing of an QoS flow control notification.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_flow_control
(
  int                                   link,
  netmgr_qos_event_report_flow_info_type * flow_ptr,
  netmgr_global_flow_state_change              state_change
)
{
  netmgr_exec_cmd_t * cmd = NULL;
  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT( flow_ptr );

  // Allocate cmd object
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);
  cmd->data.type = NETMGR_QOS_FLOCNTRL_EV;
  cmd->data.link = link;
  cmd->data.info.qos_flow.flow_id = (uint32)flow_ptr->qos_flow_state.qos_identifier;
  cmd->data.info.qos_flow.flow_type = (uint8)((NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM &
                                       flow_ptr->param_mask)?
                                      flow_ptr->flow_type : NETMGR_FLOW_TYPE_INVALID);
  cmd->data.info.qos_flow.state = (NETMGR_QOS_FLOW_DISABLED == state_change)?
                                  NETMGR_TC_FLOW_DISABLED : NETMGR_TC_FLOW_ACTIVE;

  // Post command
  if (NETMGR_SUCCESS != netmgr_exec_put_cmd(cmd))
  {
    NETMGR_ABORT("netmgr_qos_flow_suspended: failed to put command");
    netmgr_exec_release_cmd(cmd);
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}

#endif /* NETMGR_QOS_ENABLED */

/*===========================================================================
  FUNCTION  netmgr_dsd_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming DSD Indication message. This function
 is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_dsd_ind
(
  void
)
{

#ifdef NETMGR_QOS_ENABLED

  int i,rc = 0;
  int max_links = netmgr_main_cfg.num_data_ports;
  wds_get_configured_throughput_info_resp_msg_v01  tput_info;
  qmi_client_type                                 clnt_hndl;

    NETMGR_LOG_FUNC_ENTRY;
  /* Query throughput info for default SUBS and apply this to TC */

  /* Use link 0 IPV4 family handle for throughput query */
  if( NETMGR_QMI_CLIENT_INVALID ==
      netmgr_qmi_cfg.links[0].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] )
  {
    netmgr_log_err( "QMI WDS client not initalized \n");

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
    netmgr_util_wakelock_release();
#endif

    return ;
  }
  clnt_hndl = netmgr_qmi_cfg.links[0].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4];


  netmgr_log_med("%s(): Processing DSD indication",__func__);
  memset (&tput_info,0x0, sizeof(tput_info));

  rc=netmgr_qmi_wds_get_configured_throughput_info( clnt_hndl, &tput_info);

  netmgr_log_med(" Get configured throughput indication - UL throughput %u",
                   tput_info.ul_configured_throughput);

  if ( (TRUE == tput_info.ul_configured_throughput_valid ) && 
       (tput_info.ul_configured_throughput != 0) )
  {
    netmgr_main_cfg.tc_ul_baserate = (tput_info.ul_configured_throughput)*1000 + NETMGR_TC_BASE_OFFSET;
    netmgr_log_med("New tc_ul_baserate is %lu", netmgr_main_cfg.tc_ul_baserate);
  }

  for (i = 0; i < max_links; i++)
  {
    netmgr_tc_flow_update_data_rate_hdlr(i, netmgr_main_cfg.tc_ul_baserate);
  }

  NETMGR_LOG_FUNC_EXIT;

#endif /* NETMGR_QOS_ENABLED */

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
    netmgr_util_wakelock_release();
#endif

  return;
}

/*===========================================================================
  FUNCTION  netmgr_qos_event_report_ind
===========================================================================*/
/*!
@brief
  Performs processing of an incoming QOS Event Report Indication message.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_event_report_ind
(
  int                                  link,
  netmgr_qmi_qos_indication_data_type  *ind_data
)
{
  netmgr_qos_event_report_type    *event = NULL;
  int i=0;

  NETMGR_ASSERT( ind_data );
  event = &ind_data->event_report;
  (void)link;

  NETMGR_LOG_FUNC_ENTRY;

#ifdef NETMGR_QOS_ENABLED
  /* Verify the flow info is present */
  if( event->param_mask & NETMGR_QOS_EVENT_REPORT_GLOBAL_FLOW_INFO_PARAM )
  {
    /* Loop over list of flows */
    netmgr_log_med( "Processing event reports - num_flows[%d]", event->num_flows );

    for( i=0; i < event->num_flows; i++ )
    {
      if( 0 ==
          (NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_STATE_PARAM & event->flow_info[i].param_mask) )
      {
        netmgr_log_err( "Flow state not present, skipping[%d]\n", i);
        continue;
      }

      /* Process the QoS flow event */
      switch( event->flow_info[i].qos_flow_state.report_flow_state_chng ) {

        case NETMGR_QOS_FLOW_ACTIVATED:
          netmgr_qos_flow_activated( link,
                                     &event->flow_info[i]);
          break;
        case NETMGR_QOS_FLOW_MODIFIED:
          netmgr_qos_flow_modified( link, &event->flow_info[i] );
          break;
        case NETMGR_QOS_FLOW_DELETED:
          netmgr_qos_flow_deleted( link, &event->flow_info[i] );
          break;
        case NETMGR_QOS_FLOW_SUSPENDED:
          netmgr_qos_flow_suspended( link, &event->flow_info[i] );
          break;

        case NETMGR_QOS_FLOW_ENABLED:
        case NETMGR_QOS_FLOW_DISABLED:
          if (!netmgr_main_get_ibfc_enabled())
          {
             netmgr_qos_flow_control( link, &event->flow_info[i],
                                      event->flow_info[i].qos_flow_state.report_flow_state_chng );
          }
          break;

        default:
          netmgr_log_err("unsupported QoS flow event: %d\n",
                         event->flow_info[i].qos_flow_state.report_flow_state_chng );
          break;
      }
    }
  }
#endif /* NETMGR_QOS_ENABLED */

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qos_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming QOS Indication message. This function
 is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qos_ind
(
  int                                  link,
  int                                  ind_id,
  netmgr_qmi_qos_indication_data_type  *ind_data
)
{
  NETMGR_LOG_FUNC_ENTRY;

  /* Verify link id is valid before proceeding */
  NETMGR_ASSERT(netmgr_qmi_verify_link(link) == NETMGR_SUCCESS);

  /* Process based on indication type */
  switch (ind_id) {
    case QMI_QOS_EVENT_REPORT_IND_V01:
    case QMI_QOS_GLOBAL_QOS_FLOW_IND_V01:
      /* Process Event Report indication */
      netmgr_qos_event_report_ind( link, ind_data );
      break;

    default:
      /* Ignore all other indications */
      netmgr_log_high("Ignoring QMI QOS IND of type %d\n", ind_id);
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_cmd_exec
===========================================================================*/
/*!
@brief
 Virtual function registered with the Command Thread to executes a QMI WDS
 Interface command.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_cmd_exec (ds_cmd_t * cmd, void * data)
{
  netmgr_qmi_cmd_t * qmi_cmd;
  (void)cmd;

  /* Get qmi cmd ptr from user data ptr */
  qmi_cmd = (netmgr_qmi_cmd_t *)data;
  NETMGR_ASSERT( qmi_cmd );

  /* Process based on command type */
  switch( qmi_cmd->type ) {
    case NETMGR_QMI_SYS_IND_CMD:
      netmgr_sys_ind( qmi_cmd->data.sys_ind.event_id,
                      qmi_cmd->data.sys_ind.link );
      break;

    case NETMGR_QMI_WDS_IND_CMD:
      netmgr_wds_ind( &qmi_cmd->data.wds_ind );
      break;

    case NETMGR_QMI_DSD_IND_CMD:
      netmgr_dsd_ind();
      break;


    case NETMGR_QMI_QOS_EVENT_REPORT_IND_CMD:
      netmgr_qos_ind( qmi_cmd->data.qos_ind.link,
                      qmi_cmd->data.qos_ind.ind_id,
                      &qmi_cmd->data.qos_ind.info );
      break;

  case NETMGR_QMI_DFS_IND_CMD:
      netmgr_qmi_dfs_process_ind( qmi_cmd->data.dfs_ind.link,
                                  qmi_cmd->data.dfs_ind.ind_id,
                                  &qmi_cmd->data.dfs_ind.info );
      break;

    default:
      netmgr_log_err("received unknown command type %d\n", qmi_cmd->type);
      return NETMGR_FAILURE;
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_get_flow_info
===========================================================================*/
/*!
@brief
  Function to query QoS flow info via QMI Message Library

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - netmgr_qmi_init() must have been called

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_qmi_get_flow_info
(
  uint8     link,
  uint32    flow_id,
  uint8   * priority,
  uint32  * datarate
)
{
#ifdef NETMGR_QOS_ENABLED
  netmgr_qos_granted_info_rsp_type granted_info;
  qmi_client_type client_hndl;
  netmgr_qos_flow_req_type *flow;
  int err_code;
  int result;

  NETMGR_LOG_FUNC_ENTRY;

  /* Use IPV4 family handle for flow query */
  if( NETMGR_QMI_CLIENT_INVALID ==
      netmgr_qmi_cfg.links[link].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] )
  {
    netmgr_log_err( "QMI QOS client not initalized \n");
    return NETMGR_FAILURE;
  }
  client_hndl = netmgr_qmi_cfg.links[link].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4];

  /* Check for primary or secondary flow */
  if( NETMGR_IS_DEFAULT_FLOW( flow_id ) ) {
    /* Query primary flow */
    if (NETMGR_SUCCESS != netmgr_qmi_qos_get_granted_qos(link,
                                                         client_hndl,
                                                         TRUE,
                                                         flow_id,
                                                         &granted_info)) {
      netmgr_log_err("failed on query for primary QoS\n");
      return NETMGR_FAILURE;
    }
  }
  else {
    /* Query secondary flow */
    if (NETMGR_SUCCESS != netmgr_qmi_qos_get_granted_qos(link,
                                                         client_hndl,
                                                         FALSE,
                                                         flow_id,
                                                         &granted_info)) {
      netmgr_log_err("failed on query for secondary QoS\n");
      return NETMGR_FAILURE;
    }
  }

  /* Check for QoS TX flow specification */
  if( granted_info.tx_granted_flow_data_is_valid ) {
    flow = &granted_info.tx_granted_flow_data.qos_flow_granted;

    /* Check for UMTS parameter set */
    if( flow->umts_flow_desc.param_mask ) {
      if( NETMGR_SUCCESS !=
          netmgr_qos_flow_decode_umts_params( &flow->umts_flow_desc,
                                              priority,
                                              datarate ) ) {

        netmgr_log_err( "Error decoding UMTS TX flow data!\n");
        return NETMGR_FAILURE;
      }
    }
    /* Check for CDMA parameter set */
    else if( flow->cdma_flow_desc.param_mask ) {
      if( NETMGR_SUCCESS !=
          netmgr_qos_flow_decode_cdma_params( &flow->cdma_flow_desc,
                                              priority,
                                              datarate ) ) {

        netmgr_log_err( "Error decoding CDMA TX flow data!\n");
        return NETMGR_FAILURE;
      }
    }
    else {
      netmgr_log_err( "TX flow data undefined!\n");
      return NETMGR_FAILURE;
    }
  }

  NETMGR_LOG_FUNC_EXIT;

  return NETMGR_SUCCESS;

#else
  (void)link; (void)flow_id; (void)priority; (void)datarate;
  netmgr_log_err( "Operation not supported!\n");
  return NETMGR_FAILURE;
#endif /* NETMGR_QOS_ENABLED */
}

/*===========================================================================
  FUNCTION  netmgr_qmi_wds_route_look_up_init
===========================================================================*/
/*!
@brief
  Initializes the QMI WDS handle for Route Lookup.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int netmgr_qmi_wds_route_look_up_init
(
  int                     link,
  char                    *qmi_cid
)
{

  int                 ret = NETMGR_FAILURE;
  qmi_client_type     clnt_hndl;

  /* Initialize Route look up WDS service client */
  if (NETMGR_SUCCESS != netmgr_qmi_wds_init_route_look_up_client ( link,
                                                                   qmi_cid,
                                                                   &clnt_hndl ))
  {
    netmgr_log_err("%s(): qmi_wds_srvc_init_client failed "
                   "for link %d qmi_cid %s\n",
                   __func__, link, qmi_cid);
    goto error;
  }

  netmgr_route_look_up_hndl = clnt_hndl;
  ret = NETMGR_SUCCESS;

error:
  NETMGR_LOG_FUNC_EXIT;

  return ret;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_wds_service_init
===========================================================================*/
/*!
@brief
  Initializes the QMI WDS services for a specific link.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - Link should be enabled as part of the config

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_wds_service_init
(
  int                     link,
  char                    *qmi_cid,
  wds_ip_family_enum_v01  ip_family
)
{
  int                                     ret = NETMGR_FAILURE;
  netmgr_qmi_client_type_t                clnt;
  int                                     rc = QMI_NO_ERR;
  qmi_client_type                         *clnt_hndl = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  clnt = netmgr_qmi_convert_ip_pref_to_client_type(ip_family);
  clnt_hndl = &netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[clnt];

  /* Initialize WDS service client */
  if (NETMGR_SUCCESS != netmgr_qmi_wds_init_client ( link,
                                                     qmi_cid,
                                                     ip_family,
                                                     clnt_hndl ))
  {
    netmgr_log_err("%s(): qmi_wds_srvc_init_client failed "
                   "for link %d qmi_cid %s\n",
                   __func__, link, qmi_cid);
    goto error;
  }

  netmgr_log_med("%s(): Successfully initialized WDS client=%p on link=%d\n",
                  __func__, *clnt_hndl, link);

/* TODO: Implement qmi_wds_indication_register on off-target then remove this check */
#ifndef NETMGR_OFFTARGET
  if (NETMGR_SUCCESS != netmgr_qmi_wds_ext_ip_ind_reg(link,
                                                      *clnt_hndl))
  {
    netmgr_log_err( "Failed to register for ext IP config ind on "
                     "IPv%d link=%d, clnt_hndl=%p",
                     ip_family,
                     link,
                     clnt_hndl );
  }
  else
  {
    netmgr_log_high("Successfully registered for ext IP config "
                    "ind on IPv%d clnt_hndl=%p",
                    ip_family,
                    clnt_hndl );
  }
#endif /* NETMGR_OFFTARGET */

#ifdef FEATURE_DS_LINUX_ANDROID
  if (netmgr_main_get_dynamic_tc_ul_baserate())
  {
    if ((link == 0) && (ip_family == WDS_IP_FAMILY_IPV4_V01))
    {
      netmgr_qmi_wds_config_tput_ind_reg(ip_family, *clnt_hndl);
    }
  }
#endif /* FEATURE_DS_LINUX_ANDROID */

  /* Always bind NETMGR wds client to DONT CARE Subs */
  if (NETMGR_SUCCESS != netmgr_qmi_wds_bind_subscription(link,
                                                         *clnt_hndl))
  {
    netmgr_log_err("netmgr_qmi_wds_service_init: subs_id binding failed on link=%d",
                   link);
  }

#ifdef FEATURE_DATA_IWLAN
  /* Register for reverse IP transport indications. */
  if (NETMGR_IS_REV_IP_TRANS_CONN_ID(qmi_cid))
  {
    /* If iWLAN client is disabled then we need to ensure that we register for reverse IP
       indications after all reverse links have moved to inited state. We will do a late
       registration of reverse IP transport indication to prevent race conditions between
       modem and netmgr. If iWLAN client is enabled then we proceed with legacy behavior */
    if (netmgr_main_cfg.iwlan_client_enable &&
          NETMGR_SUCCESS != netmgr_qmi_wds_rev_ip_ind_reg(link,
                                                          *clnt_hndl))
    {
      netmgr_log_err("netmgr_qmi_wds_service_init: rev ip ind register failed on link=%d",
                     link);
      goto error;
    }
  }
  else if (TRUE == netmgr_main_get_iwlan_enabled())
  {
    if (NETMGR_SUCCESS != netmgr_qmi_wds_bearer_tech_ex_ind_reg(link,
                                                                *clnt_hndl))
    {
      netmgr_log_err("netmgr_qmi_wds_service_init: bearer tech ex reg failed on link=%d",
                     link);
    }
  }
#endif /* FEATURE_DATA_IWLAN */

  ret = NETMGR_SUCCESS;

error:
  NETMGR_LOG_FUNC_EXIT;

  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_service_init
===========================================================================*/
/*!
@brief
  Initializes the QMI QOS services for a specific link.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - Link should be enabled as part of the config

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_qos_service_init
(
  int                     link,
  char                    *qmi_cid,
  qos_ip_family_enum_v01  ip_family
)
{
  int ret = NETMGR_FAILURE;

#ifdef NETMGR_QOS_ENABLED
  qmi_response_type_v01  err;
  qmi_client_type  *clnt_hndl = NULL;
  netmgr_qmi_client_type_t clnt = netmgr_qmi_convert_ip_pref_to_client_type((wds_ip_family_enum_v01)ip_family);
  netmgr_qos_event_report_state_type   qos_report_state;

  NETMGR_LOG_FUNC_ENTRY;

  clnt_hndl = &netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[clnt];

  /* Initialize QOS service clients */
  if (NETMGR_SUCCESS != netmgr_qmi_qos_init_client( link,
                                                    qmi_cid,
                                                    ip_family,
                                                    clnt_hndl,
                                                    &err ))
  {
    if ( (QMI_RESULT_FAILURE_V01 == err.result) &&
         (QMI_ERR_INVALID_SERVICE_TYPE_V01 == err.error) )
    {
      netmgr_log_err("%s(): QOS not supported on Modem, suppressing\n", __func__);

      /* Clear QOS state variables */
      netmgr_main_cfg.runmode &= ~NETMGR_MAIN_RUNMODE_QOSHDR;
      netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[clnt] = NETMGR_QMI_CLIENT_INVALID;

      /* Treat as warning to keep link active */
      ret = NETMGR_SUCCESS;
    }
    else
    {
      netmgr_log_err("%s(): qmi_qos_srvc_init_client failed "
                     "for link %d qmi_cid %s with error %d,\n",
                     __func__,link, qmi_cid, err.error);
    }
    goto error;
  }
  else
  {
    if (NETMGR_SUCCESS != netmgr_qmi_qos_bind_subscription(link,
                                                           *clnt_hndl))
    {
      netmgr_log_err("%s(): subs_id binding failed", __func__);
    }

    /* Register for QoS flow event reporting. */
    if (NETMGR_SUCCESS != netmgr_qmi_qos_enable_global_flow_report(link,
                                                                   *clnt_hndl))
    {
      netmgr_log_err("%s(): failed to enable flow report", __func__);
      goto error;
    }

    ret = NETMGR_SUCCESS;
  }

error:
  NETMGR_LOG_FUNC_EXIT;
#endif /* NETMGR_QOS_ENABLED */

  return ret;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_service_init
===========================================================================*/
/*!
@brief
  Initializes the QMI DFS services for a specific link.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - Link should be enabled as part of the config

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_dfs_service_init
(
  int                     link,
  char                    *qmi_cid,
  dfs_ip_family_enum_v01  ip_family
)
{
  int ret = NETMGR_FAILURE;
  netmgr_qmi_client_type_t clnt;

  clnt = netmgr_qmi_convert_ip_pref_to_client_type((wds_ip_family_enum_v01)ip_family);

  if (NETMGR_SUCCESS == netmgr_qmi_dfs_init_qmi_client(link,
                                                       qmi_cid,
                                                       ip_family,
                                                       &netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[clnt]))
  {
    dfs_indication_register_req_msg_v01 req;
    dfs_indication_register_resp_msg_v01 resp;
    int rc;

    memset (&req, 0, sizeof(req));
    memset (&resp, 0, sizeof(resp));

    req.report_reverse_ip_transport_filters_update_valid = TRUE;
    req.report_reverse_ip_transport_filters_update = TRUE;

    /* Send indication registration request */
    rc = qmi_client_send_msg_sync(netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[clnt],
                                  QMI_DFS_INDICATION_REGISTER_REQ_V01,
                                  (void *)&req,
                                  sizeof(req),
                                  (void*)&resp,
                                  sizeof(resp),
                                  NETMGR_QMI_TIMEOUT);

    netmgr_log_high("The rev ip filter update registration link[%d] ip_family[%d] status[%s]\n",
                    link,
                    ip_family,
                    (rc == QMI_NO_ERR && resp.resp.result == QMI_RESULT_SUCCESS_V01) ? "SUCCESS" : "FAIL");
  }
  else
  {
    netmgr_log_err("failed to allocate dfs client on link[%d]\n",
                   link);
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  return ret;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_qmi_dev_set_data_format
===========================================================================*/
/*!
@brief
  Sets the WDA data format for the physical net device


@return
  NETMGR_SUCCESS - if successful
  NETMGR_ERROR - if any errors occurred during sending WDA message.

*/
/*=========================================================================*/
static int
netmgr_qmi_dev_set_data_format
(
  wda_set_data_format_req_msg_v01 *request,
  wda_set_data_format_resp_msg_v01 *response
)
{
  unsigned int egress_flags = 0;
  unsigned int ingress_flags = 0;
  unsigned int dl_minimum_padding = 0;

  if ((request == NULL) || (response == NULL))
  {
    netmgr_log_err("%s(): Called with invalid arguments", __func__);
    return NETMGR_FAILURE;
  }

  if ((request->dl_data_aggregation_protocol_valid == TRUE) &&
    (response->dl_data_aggregation_protocol_valid == TRUE) &&
    (request->dl_data_aggregation_protocol == response->dl_data_aggregation_protocol))
  {
    if (request->dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV4)
    {
      ingress_flags = RMNET_INGRESS_FORMAT_MAP | RMNET_INGRESS_FORMAT_DEMUXING
        | RMNET_INGRESS_FORMAT_MAP_CKSUMV4;
    }
    else if (request->dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV3)
    {
      ingress_flags = RMNET_INGRESS_FORMAT_MAP | RMNET_INGRESS_FORMAT_DEMUXING
        | RMNET_INGRESS_FORMAT_MAP_CKSUMV3;
    }
    else if (request->dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAP)
    {
      ingress_flags = RMNET_INGRESS_FORMAT_MAP | RMNET_INGRESS_FORMAT_DEMUXING;
    }
    else
    {
      netmgr_log_err("%s(): DL data format %d not supported by modem",
                     __func__,
                     request->dl_data_aggregation_protocol);
      return NETMGR_FAILURE;
    }

    if (netmgr_main_get_ibfc_enabled())
    {
      ingress_flags = ingress_flags | RMNET_INGRESS_FORMAT_MAP_COMMANDS;
    }

    if ((request->dl_data_aggregation_max_datagrams_valid == TRUE) &&
      (request->dl_data_aggregation_max_datagrams_valid ==
        response->dl_data_aggregation_max_datagrams_valid) &&
      (request->dl_data_aggregation_max_datagrams == response->dl_data_aggregation_max_datagrams))
    {
      if (request->dl_data_aggregation_max_datagrams > 1)
      {
        ingress_flags = ingress_flags | RMNET_INGRESS_FORMAT_DEAGGREGATION;

        if ((request->dl_minimum_padding_valid == TRUE) &&
          (request->dl_minimum_padding_valid == response->dl_minimum_padding_valid) &&
          (response->dl_minimum_padding == request->dl_minimum_padding))
        {
          dl_minimum_padding = response->dl_minimum_padding;
          netmgr_log_med("%s() dl_minimum_padding is %u",
                         __func__,
                         dl_minimum_padding);
        }
        else
        {
          netmgr_log_high("%s() dl_minimum_padding will not be configured",__func__);
        }
      }
    }
    else
    {
      netmgr_log_err("%s(): DL agg %d not supported by modem",
                     __func__,
                     request->dl_data_aggregation_max_datagrams);
      return NETMGR_FAILURE;
    }
  }
  else
  {
    netmgr_log_err("%s(): DL data format rejected by modem, req %d resp %d",
                   __func__,
                   request->dl_data_aggregation_protocol,
                   response->dl_data_aggregation_protocol);
    return NETMGR_FAILURE;
  }

  if ((request->ul_data_aggregation_protocol_valid == TRUE) &&
    (response->ul_data_aggregation_protocol_valid == TRUE) &&
    (request->ul_data_aggregation_protocol == response->ul_data_aggregation_protocol))
  {
    if (request->ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV4)
    {
      egress_flags = RMNET_EGRESS_FORMAT_MAP  | RMNET_EGRESS_FORMAT_MUXING
        | RMNET_EGRESS_FORMAT_MAP_CKSUMV4 | RMNET_EGRESS_FORMAT_AGGREGATION;
    }
    else if (request->ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV3)
    {
      egress_flags = RMNET_EGRESS_FORMAT_MAP  | RMNET_EGRESS_FORMAT_MUXING
        | RMNET_EGRESS_FORMAT_MAP_CKSUMV3;
    }
    else if (request->ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAP)
    {
      egress_flags = RMNET_EGRESS_FORMAT_MAP  | RMNET_EGRESS_FORMAT_MUXING;
    }
    else
    {
      netmgr_log_err("%s(): UL data format %d not supported by modem",
                     __func__,
                     request->ul_data_aggregation_protocol);
      return NETMGR_FAILURE;
    }
  }
  else
  {
    netmgr_log_err("%s(): UL data format rejected by modem, req %d resp %d",
                   __func__,
                   request->ul_data_aggregation_protocol,
                   response->ul_data_aggregation_protocol);
    return NETMGR_FAILURE;
  }

  if (netmgr_rmnet_set_feature_flags((const char *)netmgr_main_get_phys_net_dev(),
                                     ingress_flags,
                                     egress_flags,
                                     dl_minimum_padding)
    != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s() setting of feature flags failed on dev %s",
                   __func__,
                   (const char *)netmgr_main_get_phys_net_dev());
    return NETMGR_FAILURE;
  }

  if (netmgr_kif_set_data_format((const char *)netmgr_main_get_phys_net_dev(),
                                 ingress_flags,
                                 egress_flags)
    != NETMGR_SUCCESS)
  {
    netmgr_log_err("%s() setting data format failed on dev %s",
                   __func__,
                   (const char *)netmgr_main_get_phys_net_dev());
    return NETMGR_FAILURE;
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_set_data_format
===========================================================================*/
/*!
@brief
  Sets the WDA data format for the link


@return
  NETMGR_SUCCESS - if successful
  NETMGR_ERROR - if any errors occurred during sending WDA message.

*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_set_data_format
(
  char *dev_name,
  netmgr_data_format_t data_format
)
{
  wda_set_data_format_req_msg_v01 wda_request;
  wda_set_data_format_resp_msg_v01 wda_response;
  int ret_val = NETMGR_WDA_BAD_ARGUMENTS;

init:
  memset(&wda_request, 0, sizeof(wda_set_data_format_req_msg_v01));

  if (netmgr_main_get_qos_enabled() & (data_format.qos_header_format_valid))
  {
    wda_request.qos_format_valid = TRUE;
    wda_request.qos_format = (uint8_t)data_format.qos_format;

    wda_request.qos_header_format_valid = (uint8_t)data_format.qos_header_format_valid;
    wda_request.qos_header_format = data_format.qos_header_format;
  }

  if ((netmgr_main_get_qos_enabled()) &&
      ((wda_request.qos_format_valid == FALSE) ||
      (wda_request.qos_format == 0)) &&
      (!netmgr_main_get_ibfc_enabled()))
  {
    wda_request.flow_control_valid = TRUE;
    wda_request.flow_control = TRUE;
  }

  wda_request.link_prot_valid = TRUE;
  wda_request.link_prot = data_format.link_prot;

  if (data_format.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAP
        || data_format.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV3
        || data_format.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV4)
  {
    wda_request.ul_data_aggregation_protocol_valid = TRUE;
    wda_request.ul_data_aggregation_protocol =
      data_format.ul_data_aggregation_protocol;
  }

  if (data_format.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAP
        || data_format.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV3
        || data_format.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV4)
  {
    wda_request.dl_data_aggregation_protocol_valid = TRUE;
    wda_request.dl_data_aggregation_protocol =
      data_format.dl_data_aggregation_protocol;

    wda_request.dl_data_aggregation_max_size_valid = TRUE;
    wda_request.dl_data_aggregation_max_size = (uint32_t)data_format.dl_agg_size;

    wda_request.dl_data_aggregation_max_datagrams_valid = TRUE;
    wda_request.dl_data_aggregation_max_datagrams = (uint32_t)data_format.dl_agg_cnt;

    if (data_format.dl_agg_cnt > 1)
    {
      wda_request.dl_minimum_padding_valid = TRUE;
      wda_request.dl_minimum_padding = (uint8_t)data_format.dl_agg_pad;
    }
  }

  if ((ret_val = netmgr_wda_set_data_format(dev_name,
                                            &wda_request,
                                            &wda_response)) != QMI_NO_ERR)
  {
    int restart = FALSE;

    netmgr_log_err("%s() WDA Set Data Format Failed on (%s)", __func__, dev_name);

    if ((!wda_request.dl_data_aggregation_protocol_valid) ||
        (wda_request.dl_data_aggregation_protocol != wda_response.dl_data_aggregation_protocol))
    {
      if (wda_request.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV4)
      {
        data_format.dl_data_aggregation_protocol = NETMGR_WDA_DL_QMAPV3;
        restart = TRUE;
      }
      else if (wda_request.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAPV3)
      {
        data_format.dl_data_aggregation_protocol = NETMGR_WDA_DL_QMAP;
        restart = TRUE;
      }
      else if (wda_request.dl_data_aggregation_protocol == NETMGR_WDA_DL_QMAP)
      {
        netmgr_log_err("%s() Modem does not support dl_agg_prot [%d], %s",
                       __func__,
                       wda_request.dl_data_aggregation_protocol,
                       "data path may be broken");
      }
    }

    if ((!wda_request.ul_data_aggregation_protocol_valid) ||
        (wda_request.ul_data_aggregation_protocol != wda_response.ul_data_aggregation_protocol))
    {
      if (wda_request.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV4)
      {
        data_format.ul_data_aggregation_protocol = NETMGR_WDA_UL_QMAPV3;
        restart = TRUE;
      }
      else if (wda_request.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAPV3)
      {
        data_format.ul_data_aggregation_protocol = NETMGR_WDA_UL_QMAP;
        restart = TRUE;
      }
      else if (wda_request.ul_data_aggregation_protocol == NETMGR_WDA_UL_QMAP)
      {
        netmgr_log_err("%s() Modem does not support ul_agg_prot [%d], %s",
                       __func__,
                       wda_request.ul_data_aggregation_protocol,
                       "data path may be broken");
      }
    }

    if (restart == TRUE)
    {
      netmgr_log_err("%s() Modem does not support dl_agg_prot [%d] ul_agg_prot [%d]",
                     __func__,
                     wda_request.dl_data_aggregation_protocol,
                     wda_request.ul_data_aggregation_protocol);
      netmgr_log_err("%s() Retrying with dl_agg_prot [%d] ul_agg_prot [%d]",
                     __func__,
                     data_format.dl_data_aggregation_protocol,
                     data_format.ul_data_aggregation_protocol);
      goto init;
    }

    if ((wda_request.flow_control == TRUE) &&
        (wda_response.flow_control != wda_request.flow_control))
    {
      netmgr_log_err("%s() Modem does not support flow_control on (%s)",
                      __func__,
                      dev_name);
    }

    if ((netmgr_main_get_qos_enabled()) &&
        (netmgr_main_get_rmnet_data_enabled()) &&
        (ret_val == NETMGR_WDA_MODEM_REJECTED) &&
        (wda_request.qos_header_format_valid))
    {
      netmgr_log_high("%s() request.qos_header_format is %d, response.qos_header_format is %d",
                        __func__,
                        wda_request.qos_header_format,
                        wda_response.qos_header_format);
      if ((wda_request.qos_header_format == NETMGR_QMI_QOS_HEADER_8_BYTE) &&
          (wda_response.qos_header_format == NETMGR_QMI_QOS_HEADER_6_BYTE))
      {
        netmgr_log_high("%s() Modem is using qos_header_format size 6 on (%s)",
                        __func__,
                        dev_name);
      }
      /* If modem fails does not support qos_header_format to set 8 byte QoS, revert to
         6 byte QoS which is the default */
      if ((wda_request.qos_header_format_valid) && (!(wda_response.qos_header_format_valid)))
      {
        netmgr_log_high("%s() Modem does not support qos_header_format on (%s)",
                        __func__,
                        dev_name);
        data_format.qos_header_format = NETMGR_QMI_QOS_HEADER_6_BYTE;
      }
      if ((wda_request.qos_header_format == NETMGR_QMI_QOS_HEADER_6_BYTE) &&
          (wda_response.qos_header_format == NETMGR_QMI_QOS_HEADER_8_BYTE))
      {
        netmgr_log_high("%s() Modem is using qos_header_format size 6 ,%s on (%s)",
                        __func__,
                        "qos_header_format to be used is size 8",
                        dev_name);
        return NETMGR_FAILURE;
      }

    }
  }

  if ((wda_request.qos_header_format_valid) && (wda_response.qos_header_format_valid))
  {
    netmgr_log_high("%s() request qos_header_format is %d, response.qos_header_format is %d",
                    __func__,
                    wda_request.qos_header_format,
                    wda_response.qos_header_format);
    data_format.qos_header_format = wda_response.qos_header_format;
  }

  if (netmgr_main_cfg.df_configured == FALSE)
  {
    if (netmgr_qmi_dev_set_data_format(&wda_request, &wda_response) != NETMGR_SUCCESS)
    {
      netmgr_log_err("%s() Failed to set data format on physical net device", __func__);
      return NETMGR_FAILURE;
    }
    else
    {
      netmgr_log_high("%s() Successfully set data format on physical net device", __func__);
      netmgr_main_cfg.df_configured = TRUE;
    }
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_set_qmap_settings
===========================================================================*/
/*!
@brief
  Sets the QMAP settings for the link


@return
  NETMGR_SUCCESS - if successful
  NETMGR_ERROR - if any errors occurred during sending WDA message.

*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_set_qmap_settings
(
  char *dev_name
)
{
  wda_set_qmap_settings_req_msg_v01 qmap_request;
  wda_set_qmap_settings_resp_msg_v01 qmap_response;
  memset(&qmap_request, 0, sizeof(wda_set_qmap_settings_req_msg_v01));
  memset(&qmap_response, 0, sizeof(wda_set_qmap_settings_resp_msg_v01));

  if (netmgr_main_get_ibfc_enabled())
  {
    qmap_request.in_band_flow_control_valid = TRUE;
    qmap_request.in_band_flow_control = TRUE;
  }

  if (QMI_NO_ERR != netmgr_wda_set_qmap_settings(dev_name, &qmap_request, &qmap_response))
  {
    netmgr_log_err("%s() WDA Set QMAP Settings Failed on (%s)", __func__, dev_name);
    return NETMGR_FAILURE;
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION netmgr_qmi_update_data_format
===========================================================================*/
/*!
@brief
  Gets the currently configured data format from the modem and updates RmNet
  Data configufation.

@todo
  Ideally, all data format paramters should be update from here into the config
  struct. Then from nm_main, we can apply these to RmNet Data.

@return
  NETMGR_SUCCESS - if successful
  NETMGR_ERROR - if any errors occurred during sending WDA message.
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_update_data_format
(
  const char *dev_name,
  int link
)
{
  int rc, agg_count, agg_size, c_agg_count, c_agg_size;

  c_agg_count = netmgr_main_cfg.data_format.ul_agg_cnt;
  c_agg_size = netmgr_main_cfg.data_format.ul_agg_size;

  rc = netmgr_wda_get_ul_agg_modem_limits(netmgr_qmi_get_dev_str_for_link(link), &agg_count, &agg_size);
  if (rc != NETMGR_WDA_SUCCESS)
  {
    netmgr_log_med("%s(): WARNING: Could not retreive UL aggregation parameters"
                   " modem. UL aggregation will be disabled", __func__);
    agg_count = 0;
    agg_size = 0;
  }

  netmgr_log_low("%s(): From modem: agg_count=%d, agg_size=%d\n",
                 __func__, agg_count, agg_size);
  netmgr_log_low("%s(): From local config: agg_count=%d, agg_size=%d\n",
                 __func__, c_agg_count, c_agg_size);
  agg_count = MIN(agg_count, c_agg_count);
  agg_size = MIN(agg_size, c_agg_size);
  netmgr_log_low("%s(): Using agg values: agg_count=%d, agg_size=%d\n",
                 __func__, agg_count, agg_size);

  rc = netmgr_rmnet_update_ul_aggregation(dev_name, agg_count, agg_size);
  if (rc != NETMGR_RMNET_SUCCESS)
  {
    netmgr_log_err("%s(): Failed to set UL agg params on RmNet Data", __func__);
    return NETMGR_FAILURE;
  }
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_service_init
===========================================================================*/
/*!
@brief
  Initializes the QMI services for a specific link.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - Link should be enabled as part of the config

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_service_init( int link, netmgr_data_format_t data_format )
{
  char                               *qmi_cid;
  char                               *dev_str;
  int                                 qmi_err;
  int                                 wds_clnt_id;
  qmi_link_layer_protocol_type        ll_prot;
  qmi_data_format_qos_hdr_state_type  qos_hdr;

  NETMGR_LOG_FUNC_ENTRY;

  netmgr_log_med("netmgr_qmi_service_init: entry ");
  /* Get qmi connection id for the interface */
  qmi_cid = (char *)netmgr_qmi_get_conn_id_for_link( link );
  dev_str = (char *)netmgr_qmi_get_dev_str_for_link( link );

  if (qmi_cid == NULL)
  {
    netmgr_log_err("%s: failed. netmgr_qmi_get_conn_id_for_link %d returned qmi_cid as NULL\n",
                   __func__, link);
    goto error;
  }

  if (dev_str == NULL)
  {
    netmgr_log_err("netmgr_qmi_service_init: failed. netmgr_qmi_get_dev_str_for_link returned dev_str as NULL\n");
    goto error;
  }

  if( !netmgr_qmi_cfg.link_array[link].enabled )
  {
    netmgr_log_err("%s(): QMI link=%d is disabled\n",
                   __func__,link);
    goto error;
  }

  if( !is_route_look_up_handle_init)
  {
    netmgr_log_med("%s", "initializing Route Look up client");

    if(NETMGR_SUCCESS != netmgr_qmi_wds_route_look_up_init(link, qmi_cid) )
    {
      netmgr_log_err("netmgr_qmi_wds_route_look_up_init: failed on Route Lookup handle init\n",
                     link);
    }
    else
    {
      is_route_look_up_handle_init = TRUE;
      netmgr_log_err("netmgr_qmi_wds_route_look_up_init: Route Lookup handle initialized [%d] on link %d\n",
                      netmgr_route_look_up_hndl, link);
    }
  }

  netmgr_log_med("%s", "initializing IPv4 WDS client");

  if( NETMGR_SUCCESS != netmgr_qmi_wds_service_init( link, qmi_cid, WDS_IP_FAMILY_IPV4_V01) )
  {
    netmgr_log_err("netmgr_qmi_service_init: failed on WDS init, link[%d] IP family[%d]\n",
                   link, WDS_IP_FAMILY_IPV4_V01);
    goto error;
  }

  /* Check for successful service registration */
  netmgr_log_med("%s", "initializing IPv6 WDS client");

  if( NETMGR_QMI_CLIENT_INVALID !=
      netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] )
  {
    if( NETMGR_SUCCESS != netmgr_qmi_wds_service_init( link, qmi_cid, WDS_IP_FAMILY_IPV6_V01) )
    {
      netmgr_log_err("netmgr_qmi_service_init: failed on WDS init, link[%d] IP family[%d]\n",
                     link, WDS_IP_FAMILY_IPV6_V01);
      goto error;
    }

    netmgr_log_high("The wds client ids [%p/%p]\n",
                    netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4],
                    netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
  }

#ifdef FEATURE_DATA_IWLAN
  /* Initialize QoS and data format on non-reverse Rmnet ports */
  if (NETMGR_IS_REV_IP_TRANS_CONN_ID(qmi_cid))
  {
    if (NETMGR_SUCCESS != netmgr_qmi_dfs_service_init(link, qmi_cid, DFS_IP_FAMILY_IPV4_V01))
    {
      netmgr_log_err("netmgr_qmi_service_init: failed on DFS init, link[%d] IP family[%d]\n",
                     link, DFS_IP_FAMILY_IPV4_V01);
    }

    if (NETMGR_SUCCESS != netmgr_qmi_dfs_service_init(link, qmi_cid, DFS_IP_FAMILY_IPV6_V01))
    {
      netmgr_log_err("netmgr_qmi_service_init: failed on DFS init, link[%d] IP family[%d]\n",
                     link, DFS_IP_FAMILY_IPV6_V01);
    }

    netmgr_log_high("The dfs client ids [%p/%p]\n",
                    netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4],
                    netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);

    netmgr_log_low("netmgr_qmi_service_init: skipping data format and QoS init on cid=%s",
                   qmi_cid);
  }
  else
#endif /* FEATURE_DATA_IWLAN */
  {
    /* Initialize QOS service clients */
    if( netmgr_main_get_qos_enabled() )
    {
      if( NETMGR_SUCCESS != netmgr_qmi_qos_service_init( link, qmi_cid, QOS_IP_FAMILY_IPV4_V01 ) )
      {
        netmgr_log_err("netmgr_qmi_service_init: failed on QOS init, link[%d] IP family[%d]\n",
                       link, QOS_IP_FAMILY_IPV4_V01);
        goto error;
      }

      /* Check for successful service registration */
      if( NETMGR_QMI_CLIENT_INVALID !=
          netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] )
      {
        if( NETMGR_SUCCESS != netmgr_qmi_qos_service_init( link, qmi_cid, QOS_IP_FAMILY_IPV6_V01) )
        {
          netmgr_log_err("netmgr_qmi_service_init: failed on QOS init, link[%d] IP family[%d]\n",
                         link, QOS_IP_FAMILY_IPV6_V01);
          goto error;
        }

        netmgr_log_high("The qos client ids [%p/%p]\n",
                        netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4],
                        netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6]);
      }
    }

    /* Configure data format using either CTL or WDA message */
#ifdef NETMGR_OFFTARGET
  netmgr_main_cfg.wda_data_format = FALSE; // For now WDA data format is disabled.
#endif
    if (!netmgr_main_cfg.wda_data_format)
    {
      /* Configure data channel framing mode and QoS header state.
       * This must be done at device powerup. */
      ll_prot = (netmgr_main_cfg.runmode & NETMGR_MAIN_RUNMODE_ETHERNET)?
                QMI_DATA_FORMAT_LINK_PROTOCOL_ETHERNET : QMI_DATA_FORMAT_LINK_PROTOCOL_IP;
      qos_hdr = (netmgr_main_cfg.runmode & NETMGR_MAIN_RUNMODE_QOSHDR)?
                QMI_DATA_FORMAT_WITH_QOS_HDR : QMI_DATA_FORMAT_WITHOUT_QOS_HDR;

      netmgr_log_low("netmgr_qmi_service_init: setting data format: "
                     "qos_hdr:%d, ll_prot:%d\n", qos_hdr, ll_prot);

      if( QMI_NO_ERR !=
          qmi_set_port_data_format( qmi_cid,
                                    qos_hdr,
                                    &ll_prot,
                                    &qmi_err ) )
      {
        netmgr_log_err("netmgr_qmi_service_init: qmi_set_port_data_format failed "
                       "for link %d qmi_cid %s with error %ld,\n",
                       link, qmi_cid, (long int)qmi_err);
      }
    }
    else
    {
      if (FALSE == netmgr_main_cfg.single_qmux_ch_enabled ||
          FALSE == NETMGR_QMI_GET_WDA_REQ_SENT())
      {
        if (NETMGR_SUCCESS != netmgr_qmi_set_data_format(qmi_cid, data_format))
        {
          netmgr_log_err("netmgr_qmi_service_init: netmgr_qmi_set_data_format failed"
                         "for link %d qmi_cid %s\n", link, qmi_cid);
        }
        else
        {
          NETMGR_QMI_SET_WDA_REQ_SENT(TRUE);
          netmgr_log_med("%s(): wda_req_sent [%d]", __func__, NETMGR_QMI_GET_WDA_REQ_SENT());
        }
        if (netmgr_main_get_ibfc_enabled())
        {
          if (NETMGR_SUCCESS != netmgr_qmi_set_qmap_settings(qmi_cid))
          {
            netmgr_log_err("netmgr_qmi_service_init: netmgr_wda_set_qmap_setings failed"
                           "for link %d qmi_cid %s\n", link, qmi_cid);
          }
        }
        if (NETMGR_SUCCESS != netmgr_qmi_update_data_format((const char *)netmgr_main_get_phys_net_dev(), link))
        {
          netmgr_log_err("%s: netmgr_qmi_update_data_format failed"
                         "for link %d qmi_cid %s\n", __func__, link, qmi_cid);
        }

      }
    }
  }
#if !defined(NETMGR_OFFTARGET)
  /* Initialize DSD service client for once */
  if( NETMGR_QMI_CLIENT_INVALID == netmgr_dsd_clnt_hndl )
  {
    netmgr_qmi_dsd_init();
  }
#endif

#ifdef FEATURE_DATA_IWLAN
  netmgr_util_circ_list_init(&netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_txns[NETMGR_QMI_CLIENT_IPV4]);
  netmgr_util_circ_list_init(&netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_txns[NETMGR_QMI_CLIENT_IPV6]);
#endif /* FEATURE_DATA_IWLAN */

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;

error:
  if (NETMGR_QMI_CLIENT_INVALID != netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    netmgr_qmi_wds_release_client( link,
                                   netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] );
    netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] = NETMGR_QMI_CLIENT_INVALID;
  }

  if (NETMGR_QMI_CLIENT_INVALID != netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    netmgr_qmi_wds_release_client( link,
                                   netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] );
    netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] = NETMGR_QMI_CLIENT_INVALID;
  }

  if (NETMGR_QMI_CLIENT_INVALID != netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4])
  {
    netmgr_qmi_qos_release_client( link,
                                   netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] );
    netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV4] = NETMGR_QMI_CLIENT_INVALID;
  }

  if (NETMGR_QMI_CLIENT_INVALID != netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6])
  {
    netmgr_qmi_qos_release_client( link,
                                   netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] );
    netmgr_qmi_cfg.links[ link ].qos_info.clnt_hdl[NETMGR_QMI_CLIENT_IPV6] = NETMGR_QMI_CLIENT_INVALID;
  }

  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_FAILURE;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_driver_init
===========================================================================*/
/*!
@brief
 Initializes the QMI Driver for list of specified links.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_qmi_driver_init( netmgr_data_format_t data_format )
{
  int i;
  char * qmi_cid;
  int qmi_err, retry = 0;

  NETMGR_LOG_FUNC_ENTRY;
  UNUSED(data_format);
  NETMGR_QMI_SET_WDA_REQ_SENT(FALSE);

  /*
        For targets that dont support WDA, we need to register back with
        QMUXD for CTL interactions
   */
  if(!netmgr_main_cfg.wda_data_format)
  {
    do
    {
      qmi_handle = qmi_init( NULL, NULL );

      if (qmi_handle < 0)
      {
        netmgr_log_err("Error initializing qmi message library... retry=%d", retry + 1);
        ++retry;
        usleep(NETMGR_QMI_WAIT_TIME_BEFORE_NEXT_RETRY);
      }
    } while (qmi_handle < 0 && retry < NETMGR_QMI_MAX_RETRY_COUNT);

    if (qmi_handle < 0)
    {
      NETMGR_STOP("Error initializing qmi message library");
    }
  }

  /* Register process exit cleanup handler */
  atexit(netmgr_qmi_cleanup);

  NETMGR_LOG_FUNC_EXIT;
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_reset_connection
===========================================================================*/
/*!
@brief
  Reinitialize QMI connections on reset command.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_qmi_reset_connection
(
  int  link
)
{
  int rc = NETMGR_SUCCESS;
  NETMGR_LOG_FUNC_ENTRY;

  netmgr_log_high( "initing QMI link[%d]\n", link );

  netmgr_qmi_reset_link_info (link);

  netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_LINK_IS, (void *)&link);

  /* Perform QMI service init */
  if( NETMGR_SUCCESS != netmgr_qmi_service_init( link, netmgr_qmi_cfg.data_format ) )
  {
    netmgr_log_err( "Error on QMI service init link[%d] \n", link );
    rc = NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_qmi_get_ip_addr_type_first_conn_clnt
===========================================================================*/
/*!
@brief
  Retrives the IP address type for Modem network interface

@return
  netmgr_ip_addr_t - IP address type,
                     NETMGR_IP_ADDR_INVALID if undefined

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
netmgr_ip_addr_t netmgr_qmi_get_ip_addr_type_first_conn_clnt( int link )
{
  netmgr_ip_addr_t rc = NETMGR_IP_ADDR_INVALID;

  if( NETMGR_SUCCESS == netmgr_qmi_verify_link( link ) )
  {
    switch( netmgr_qmi_cfg.links[ link ].wds_info.first_conn_clnt )
    {
      case NETMGR_QMI_CLIENT_IPV4:
        rc = NETMGR_IPV4_ADDR;
        break;
      case NETMGR_QMI_CLIENT_IPV6:
        rc = NETMGR_IPV6_ADDR;
        break;
      default:
        netmgr_log_err( "netmgr_qmi_get_ip_addr_type_first_conn_clnt unsupported value[%d]\n",
                        netmgr_qmi_cfg.links[ link ].wds_info.first_conn_clnt );
        break;
    }
  }
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_get_addr_info
===========================================================================*/
/*!
@brief
  Retrives the network address info for Modem netwok interface

@return
  netmgr_address_info_t * - IP address info struct pointer,
                            NULL if undefined

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
netmgr_address_info_t * netmgr_qmi_get_addr_info( int link )
{
  if( NETMGR_SUCCESS == netmgr_qmi_verify_link( link ) )
    return &netmgr_qmi_cfg.links[ link ].wds_info.addr_info;
  else
    return NULL;
}


/*===========================================================================
  FUNCTION: netmgr_qmi_port_in_call
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
LOCAL int netmgr_qmi_port_in_call
(
  int iface,
  int modem_subs_id
)
{
  boolean inuse_ptr = FALSE;
  netmgr_link_status_type link_state_v4 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  netmgr_link_status_type link_state_v6 = WDS_CONNECTION_STATUS_DISCONNECTED_V01;
  int qmi_ret, qmi_err_code;
  int ret = NETMGR_FAILURE;
  qmi_client_type wds_hndl;

  NETMGR_LOG_FUNC_ENTRY;

  wds_bind_mux_data_port_req_msg_v01 bind_params;

  memset(&bind_params, 0, sizeof(wds_bind_mux_data_port_req_msg_v01));
  bind_params.ep_id_valid = TRUE;
  ds_get_epid(netmgr_main_cfg.phys_net_dev, &bind_params.ep_id.ep_type, &bind_params.ep_id.iface_id);

  netmgr_log_med("Got ep_type %d, EPID %d, from %s",
      bind_params.ep_id.ep_type, bind_params.ep_id.iface_id, netmgr_main_cfg.phys_net_dev);

  bind_params.mux_id_valid = TRUE;
  bind_params.mux_id = (uint8_t) iface + 1;

  if (NETMGR_SUCCESS != netmgr_qmi_wds_bind_mux_data_port(netmgr_route_look_up_hndl,
                                                          &bind_params))
  {
    netmgr_log_err("%s", "qmi_wds_bind_mux_data_port failed\n");
  }

  if (NETMGR_SUCCESS != netmgr_qmi_wds_set_bind_subscription(netmgr_route_look_up_hndl,
                                                             modem_subs_id))
  {
    netmgr_log_err("%s", "qmi_wds_bind_subscription failed\n");
  }

  do
  {
    /* Retrieve IPv6 call statuses */
    if(NETMGR_SUCCESS != netmgr_qmi_wds_get_pkt_srvc_status(netmgr_route_look_up_hndl,
                                                             WDS_IP_FAMILY_IPV6_V01,
                                                             &link_state_v6))
    {
      netmgr_log_err("netmgr_qmi_wds_get_pkt_srvc_status failed on iface [%d]", iface);
      break;
    }

    /* Retrieve IPv4 call statuses */
    if(NETMGR_SUCCESS != netmgr_qmi_wds_get_pkt_srvc_status(netmgr_route_look_up_hndl,
                                                            WDS_IP_FAMILY_IPV4_V01,
                                                            &link_state_v4))
    {
      netmgr_log_err("netmgr_qmi_wds_get_pkt_srvc_status failed on iface [%d]", iface);
      break;
    }

    /* If either V4 or V6 call is up, then use a different port */
    if( WDS_CONNECTION_STATUS_DISCONNECTED_V01 != link_state_v4 ||
        WDS_CONNECTION_STATUS_DISCONNECTED_V01 != link_state_v6)
    {
      inuse_ptr = TRUE;
    }

    netmgr_log_med( "QMI port link status for iface[%d]: "
                     "v4_state=%d, v6_state=%d, inuse=%d",
                     iface, link_state_v4, link_state_v6, inuse_ptr );

    ret = NETMGR_SUCCESS;
  } while (0);

  if (NETMGR_SUCCESS == ret)
  {
    netmgr_log_med( "%s", "netmgr_qmi_port_in_call: " \
                   "exit with success" );
  }
  else
  {
    netmgr_log_med( "%s", "netmgr_qmi_port_in_call: " \
                   "exit with error" );
  }

  if(inuse_ptr == FALSE && ret == NETMGR_SUCCESS)
    return NETMGR_SUCCESS;
  else
    return NETMGR_FAILURE;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_find_available_iface
===========================================================================*/
/*!
@brief
  Retrieves the available interface in Netmgr

@return
  Selected interface if found, Invalid otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int netmgr_qmi_find_available_iface
(
  netmgr_qmi_wds_route_look_up_params_type        *params,
  int                                              modem_subs_id
)
{
  int i;
  int selected_iface = NETMGR_INVALID_IFACE;
  int ret = NETMGR_FAILURE;

  // Find an inited link on Netmgr that is not assigned for any other call
  for(i=0; i < netmgr_main_cfg.num_data_ports; ++i)
  {
    netmgr_log_err("%s() State %d Link %d in_use %d", __func__,
    (int)stm_get_state(&NETMGR_SM[i]), i, netmgr_qmi_cfg.links[i].wds_info.in_use);

    if( NETMGR_STATE_INITED == (int)stm_get_state(&NETMGR_SM[i]) &&
         netmgr_qmi_cfg.links[i].wds_info.in_use == FALSE)
    {
      if(netmgr_qmi_port_in_call(i, modem_subs_id) == NETMGR_SUCCESS)
        break;
      else
        continue;
    }
  }

  if(i >= netmgr_main_cfg.num_data_ports)
  {
    netmgr_log_err("No available interface");
  }
  else
  {
    selected_iface = i;
    netmgr_qmi_cfg.links[i].wds_info.in_use = TRUE;
  }

  return selected_iface;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_route_look_up
===========================================================================*/
/*!
@brief
  Perform Route lookup with the provided params

@return
  NETMGR_SUCCESS on successful route lookup with valid Iface
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_qmi_route_look_up
(
  netmgr_qmi_wds_route_look_up_params_type        *params,
  netmgr_qmi_wds_route_look_up_dsi_rsp_type       *dsi_rsp_data,
  int modem_subs_id,
  int app_type
)
{

  int ret = NETMGR_FAILURE;
  netmgr_qmi_wds_route_look_up_rsp_type rsp_data;
  int iface = NETMGR_INVALID_IFACE;

  if(!netmgr_route_look_up_hndl)
  {
    netmgr_log_err("%s() Route look up handle not initialized ", __func__);
    goto bail;
  }

  if(!params || !dsi_rsp_data)
  {
    netmgr_log_err("%s() Bad Params ", __func__);
    goto bail;
  }

  memset(&rsp_data, 0, sizeof(rsp_data));

 if(NETMGR_SUCCESS != netmgr_qmi_wds_set_bind_subscription(netmgr_route_look_up_hndl,
                                                           modem_subs_id))
  {
    netmgr_log_err("%s() Route look up failed ", __func__);
    goto bail;
  }

  netmgr_log_med("%s() Running route look up for Ip family %d ", __func__, params->ip_family_pref);

  //Need to do double route look up
  if(NETMGR_SUCCESS != netmgr_qmi_wds_route_look_up(netmgr_route_look_up_hndl,
                                                    params,
                                                    &rsp_data))
  {
    netmgr_log_err("%s() Route look up failed ", __func__);
    goto bail;
  }

  /* return the selected modem to the caller */
  if(rsp_data.result == QMI_RESULT_SUCCESS_V01)
  {
    dsi_rsp_data->pmodem = NETMGR_RADIO_3GMODEM;
    dsi_rsp_data->ptech = rsp_data.tech_name;
    netmgr_log_med("Updating %d ptech ",dsi_rsp_data->ptech);
    netmgr_log_med("modem [%s] returned an interface [0x%lud]" \
                  " with priority set to [%lud] tech set to" \
                  " [0x%x] on qmi inst [%d] and result is [%d]",
                  "NETMGR_3G_MODEM",
                  rsp_data.iface_handle,
                  rsp_data.priority,
                  rsp_data.tech_name,
                  rsp_data.qmi_inst_is_valid?
                    rsp_data.qmi_inst :
                    NETMGR_INVALID_IFACE,rsp_data.result);
  }

  // Valid Route look up instance not received,
  //Do second IP family route look up
  if (rsp_data.qmi_inst_is_valid == 0)
  {
    if(params->ip_family_pref == WDS_IP_FAMILY_PREF_IPV6_V01)
    {
      params->ip_family_pref = WDS_IP_FAMILY_PREF_IPV4_V01;
    }
    else
    {
      params->ip_family_pref = WDS_IP_FAMILY_PREF_IPV6_V01;
    }

    netmgr_log_med("%s() Running route look up for companion Ip family %d ", __func__,
                                                                            params->ip_family_pref);
    /*Reaching this point implies that the Route Lookup for the intended IP family
      had succeeded. Even if the Route lookup for the Other IP family fails, we
      need to select an iface to return to DSI*/
    if(NETMGR_SUCCESS != netmgr_qmi_wds_route_look_up(netmgr_route_look_up_hndl,
                                                     params,
                                                     &rsp_data))
    {
      netmgr_log_err("%s() Route look up failed ", __func__);
    }

    if(rsp_data.result == QMI_RESULT_SUCCESS_V01)
    {
      dsi_rsp_data->pmodem = NETMGR_RADIO_3GMODEM;
      dsi_rsp_data->ptech = rsp_data.tech_name;
      netmgr_log_med("Updating %d ptech ",dsi_rsp_data->ptech);
      netmgr_log_med("modem [%s] returned an interface [0x%lud]" \
                  " with priority set to [%lud] tech set to" \
                  " [0x%x] on qmi inst [%d] and result is [%d]",
                  "NETMGR_3G_MODEM",
                  rsp_data.iface_handle,
                  rsp_data.priority,
                  rsp_data.tech_name,
                  rsp_data.qmi_inst_is_valid?
                    rsp_data.qmi_inst :
                    NETMGR_INVALID_IFACE,rsp_data.result);
    }

    if(rsp_data.qmi_inst_is_valid == 0)
    {
      dsi_rsp_data->is_qmi_inst_valid = FALSE;

      if(NETMGR_INVALID_IFACE ==
          (iface = netmgr_qmi_find_available_iface(params, modem_subs_id)))
      {
        netmgr_log_err("%s() No valid iface present in Netmgr", __func__);
        goto bail;
      }
      else
      {
        dsi_rsp_data->qmi_inst = iface;
        netmgr_log_med("%s() Choosing iface %d ", __func__, dsi_rsp_data->qmi_inst);
      }
    }
    else
    {
      dsi_rsp_data->is_companion_ip = TRUE;
      /*Set the flag to TRUE here, DSI will decide whether to set it for the store handle
       based on the IP family requested.*/
      netmgr_log_med("%s() Setting companion IP flag to TRUE", __func__);
    }

  }

  if (rsp_data.qmi_inst_is_valid)
  {
    dsi_rsp_data->is_qmi_inst_valid = TRUE;
    dsi_rsp_data->qmi_inst = rsp_data.qmi_inst;
    netmgr_log_med("%s() Valid instance found in Route look up %d", __func__, dsi_rsp_data->qmi_inst);
  }

  ret = NETMGR_SUCCESS;

  netmgr_log_err("%s() DBG Exit ret %d ", __func__, ret);

  bail:
    return ret;
}

/*===========================================================================
  FUNCTION  netmgr_clear_qmi_iface
===========================================================================*/
/*!
@brief
  Unsets the in_use flag for an iface

@return
  NETMGR_SUCCESS on successful unsetting of the flag
  NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_clear_qmi_iface(int link)
{

  int ret = NETMGR_FAILURE;

  if( NETMGR_SUCCESS != netmgr_qmi_verify_link( link ) )
  {
    netmgr_log_err("%s() Invalid link received %d ", __func__, link);
    goto bail;
  }

  netmgr_qmi_cfg.links[link].wds_info.in_use = FALSE;
  netmgr_log_err("%s() Cleared iface %d ", __func__, link);

  ret = NETMGR_SUCCESS;

  bail:
    return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_get_xlat_state
===========================================================================*/
/*!
@brief
  Retrives the xlat state of the link

@return
  TRUE - if the call on this link is XLAT capable
  FALSE - otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
boolean netmgr_qmi_get_xlat_state( int link )
{
  if( NETMGR_SUCCESS == netmgr_qmi_verify_link( link ) )
    return netmgr_qmi_cfg.links[ link ].wds_info.xlat_capable;
  else
    return FALSE;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_get_ipsec_tunnel_endpoints
===========================================================================*/
/*!
@brief
  Retrives the tunnel endpoint addresses for a given link and ip family

@return
  netmgr_address_info_t * - IP address info struct pointer,
                            NULL if undefined

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_qmi_get_ipsec_tunnel_endpoints
(
  int link,
  int ip_family,
  const char **local,
  const char **dest,
  int *tunnel_family
)
{
  int ret = NETMGR_FAILURE;

  if( NETMGR_SUCCESS == netmgr_qmi_verify_link( link ) )
  {
    netmgr_ipsec_sa_t *sa = (AF_INET == ip_family) ? &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv4.sa :
                                                     &netmgr_qmi_cfg.links[ link ].wds_info.addr_info.ipv6.sa;

    if (TRUE == sa->tunnel_ep.is_valid)
    {
      *local = sa->tunnel_ep.local_addr;
      *dest = sa->tunnel_ep.dest_addr;
      *tunnel_family = sa->tunnel_ep.ip_family;
      ret = NETMGR_SUCCESS;
    }
    else
    {
      netmgr_log_err("%s(): tunnel endpoint addresses invalid for link=%d, ip_family=%d\n",
                     __func__,
                     link,
                     ip_family);
    }
  }

  return ret;
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
  FUNCTION  netmgr_qmi_get_mtu
===========================================================================*/
/*!
@brief
  Retrives the link MTU for Modem network interface

@return
 unsigned int - MTU value, NETMGR_MTU_INVALID if undefined

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
unsigned int netmgr_qmi_get_mtu( int link, unsigned int type )
{
  if( NETMGR_SUCCESS == netmgr_qmi_verify_link( link ) ) {
    switch (type) {
      case QMI_IP_FAMILY_PREF_IPV4:
        return (unsigned int)netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv4;

      case QMI_IP_FAMILY_PREF_IPV6:
        return (unsigned int)netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv6;

      default:
        return NETMGR_MTU_INVALID;
    }
  } else {
    return NETMGR_MTU_INVALID;
  }
}


/*===========================================================================
  FUNCTION  netmgr_qmi_reset_link_wds_data
===========================================================================*/
/*!
@brief
  Reset the QMI WDS state information associated with the given link

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_qmi_reset_link_wds_data
(
  int link
)
{
  if (NETMGR_SUCCESS == netmgr_qmi_verify_link( link ))
  {
    memset(&netmgr_qmi_cfg.links[ link ].wds_info.addr_info,
           0,
           sizeof(netmgr_qmi_cfg.links[ link ].wds_info.addr_info));

    netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv4 = NETMGR_MTU_INVALID;
    netmgr_qmi_cfg.links[ link ].wds_info.mtu_ipv6 = NETMGR_MTU_INVALID;
    netmgr_qmi_cfg.links[ link ].wds_info.tech_name = WDS_TECHNOLOGY_NAME_ENUM_MAX_ENUM_VAL_V01;
    netmgr_qmi_cfg.links[ link ].wds_info.first_conn_clnt = NETMGR_QMI_CLIENT_MAX;
    netmgr_qmi_cfg.links[link].wds_info.v4_pkt_conn = FALSE;
    netmgr_qmi_cfg.links[link].wds_info.v6_pkt_conn = FALSE;
    netmgr_qmi_cfg.links[link].wds_info.in_use      = FALSE;

#ifdef FEATURE_DATA_IWLAN
    netmgr_util_circ_list_destroy(&netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_txns[NETMGR_QMI_CLIENT_IPV4]);
    netmgr_util_circ_list_destroy(&netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_txns[NETMGR_QMI_CLIENT_IPV6]);

    memset (&netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_config_status,
            0,
            sizeof (netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_config_status));
    memset (&netmgr_qmi_cfg.links[ link ].wds_info.bearer_tech_ex,
            0,
            sizeof (netmgr_qmi_cfg.links[ link ].wds_info.bearer_tech_ex));
#endif /* FEATURE_DATA_IWLAN */
  }
}


#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_send_rev_ip_config_complete
===========================================================================*/
/*!
@brief
  Send the reverse IP configuration status to the modem

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_qmi_send_rev_ip_config_complete
(
  netmgr_qmi_iwlan_call_mode_t  mode,
  int                           link,
  int                           ip_family,
  int                           status
)
{
  qmi_client_type clnt_hndl;
  netmgr_qmi_wds_rev_ip_config_result_type res;
  int rc, qmi_err;
  netmgr_qmi_client_type_t clnt;
  ds_dll_el_t  *node = NULL;
  const void *data = NULL;
  uint32_t  txn_id;

  NETMGR_LOG_FUNC_ENTRY;

  if (ip_family != AF_INET && ip_family != AF_INET6)
  {
    netmgr_log_err("netmgr_qmi_send_rev_ip_config_complete: invalid ip_family=%d\n",
                   ip_family);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link( link ))
  {
    netmgr_log_err("netmgr_qmi_send_rev_ip_config_complete: invalid link=%d\n",
                   link);
    goto bail;
  }

  clnt = (AF_INET == ip_family) ? NETMGR_QMI_CLIENT_IPV4 : NETMGR_QMI_CLIENT_IPV6 ;

  if (NETMGR_QMI_IWLAN_CALL_BRINGUP == mode)
  {
    /* Query and install rev ip filters (if any) */
    if (NULL == netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[clnt] ||
        NETMGR_SUCCESS != netmgr_qmi_dfs_query_and_process_rev_ip_filters(link,
                                                                          netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[clnt]))
    {
      netmgr_log_err("netmgr_qmi_send_rev_ip_config_complete: failed to install rev ip filters, "
                     "link=%d, clnt_hdl=%p",
                     link,
                     netmgr_qmi_cfg.links[ link ].dfs_info.clnt_hdl[clnt]);
    }
  }

  if (!(clnt_hndl = netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[clnt]))
  {
    netmgr_log_err("netmgr_qmi_send_rev_ip_config_complete: invalid clnt_hndl=%p, link=%d, ip_family=%d\n",
                   clnt_hndl,
                   link,
                   clnt);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_wds_util_deq_txn(clnt, link, &txn_id))
  {
    netmgr_log_err("netmgr_qmi_send_rev_ip_config_complete: failed dequeue txn clnt=%d, link=%d\n",
                   clnt,
                   link);
    goto bail;
  }

  netmgr_log_med("netmgr_qmi_send_rev_ip_config_complete: mode=%s, dequeued txn=%ld, clnt=%d, link=%d, status=%d\n",
                 (NETMGR_QMI_IWLAN_CALL_BRINGUP == mode) ? "BRINGUP" : "CLEANUP",
                 (long)txn_id,
                 clnt,
                 link,
                 status);

  res = (status == NETMGR_SUCCESS) ? NETMGR_WDS_REV_IP_CONFIG_COMPLETE_SUCCESS :
                                     NETMGR_WDS_REV_IP_CONFIG_COMPLETE_FAILURE;

  if (NETMGR_SUCCESS != (rc = netmgr_qmi_wds_rev_ip_config_complete(link,
                                                                    clnt_hndl,
                                                                    txn_id,
                                                                    res)))
  {
    netmgr_log_err("netmgr_qmi_send_rev_ip_config_complete: failed rc=%d\n",
                   rc);
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return;
}


/*===========================================================================
  FUNCTION  netmgr_qmi_query_ipsec_sa_config
===========================================================================*/
/*!
@brief
  Query the Security Associations (SA) for the given family and link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_query_ipsec_sa_config
(
  int                                          ip_family,
  int                                          link,
  wds_get_ipsec_static_sa_config_resp_msg_v01  *sa_config
)
{
  int rc, ret = NETMGR_FAILURE, qmi_err;
  netmgr_qmi_client_type_t clnt;
  qmi_client_type  clnt_hndl;

  NETMGR_LOG_FUNC_ENTRY;

  if (!sa_config)
  {
    netmgr_log_err("netmgr_qmi_query_ipsec_sa_config: invalid input\n");
    goto bail;
  }

  if (AF_INET != ip_family && AF_INET6 != ip_family)
  {
    netmgr_log_err("netmgr_qmi_query_ipsec_sa_config: invalid ip_family=%d\n",
                   ip_family);
    goto bail;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link( link ))
  {
    netmgr_log_err("netmgr_qmi_query_ipsec_sa_config: invalid link=%d\n",
                   link);
    goto bail;
  }

  clnt = (ip_family == AF_INET) ? NETMGR_QMI_CLIENT_IPV4 : NETMGR_QMI_CLIENT_IPV6;

  if (!(clnt_hndl = netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[clnt]))
  {
    netmgr_log_err("netmgr_qmi_query_ipsec_sa_config: invalid clnt_hndl=%p, link=%d, ip_family=%d\n",
                   clnt_hndl,
                   link,
                   clnt);
    goto bail;
  }

  if (NETMGR_SUCCESS != (rc = netmgr_qmi_wds_get_ipsec_sa_config(link,
                                                                 clnt_hndl,
                                                                 sa_config)))
  {
    netmgr_log_err("netmgr_qmi_query_ipsec_sa_config: SA query failed rc=%d\n",
                   rc);
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_iwlan_update_link_assoc
===========================================================================*/
/*!
@brief
  Updates the association for a given link with the corresponding forward or
  reverse link with matching address or prefix

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_iwlan_update_link_assoc
(
  int  link,
  int  *assoc_link
)
{
  int  ret = NETMGR_FAILURE;
  int  start, end;
  int  i, j;
  netmgr_main_link_type  type;
  netmgr_address_info_t  *addr_info = NULL;
  netmgr_address_info_t  *tmp_addr_info = NULL;
  int cur_assoc = NETMGR_LINK_MAX;

  NETMGR_LOG_FUNC_ENTRY;

  addr_info = netmgr_qmi_get_addr_info(link);

  if (NULL == addr_info)
  {
    netmgr_log_err("netmgr_qmi_iwlan_update_link_assoc: invalid link=%d\n",
                   link);
    goto bail;
  }

  if (NULL != assoc_link)
  {
    *assoc_link = NETMGR_LINK_MAX;
  }

  type = (NETMGR_KIF_IS_REV_RMNET_LINK(link) ? NETMGR_REV_LINK : NETMGR_FWD_LINK);

  for (i = 0; i < NETMGR_MAX_MODEMS; ++i)
  {
    if (link >= netmgr_main_dev_prefix_tbl[i][type].inst_min &&
        link <= netmgr_main_dev_prefix_tbl[i][type].inst_max)
    {
      break;
    }
  }

  if (i >= NETMGR_MAX_MODEMS)
  {
    netmgr_log_err("netmgr_qmi_iwlan_update_link_assoc: unable to determine modem for link=%d\n",
                   link);
    goto bail;
  }

  /* Clear any prior associations for this link */
  if (NETMGR_LINK_MAX != (cur_assoc = netmgr_qmi_iwlan_get_link_assoc(link)))
  {
    (void) netmgr_qmi_iwlan_clear_link_assoc(link, cur_assoc);

    if (NETMGR_FWD_LINK == type &&
        NETMGR_STATE_GOING_DOWN == stm_get_state(&NETMGR_SM[cur_assoc]))
    {
      netmgr_log_med("netmgr_qmi_iwlan_update_link_assoc: updating link=%d, bringing down prior associated rev_link=%d\n",
                     link,
                     cur_assoc);
      netmgr_kif_iface_close((uint8)cur_assoc, NULL, TRUE);
    }
  }

  /* Search for a iface with mtaching address in the other link set */
  if (NETMGR_FWD_LINK == type)
  {
    start = netmgr_main_dev_prefix_tbl[i][NETMGR_REV_LINK].inst_min;
    end   = netmgr_main_dev_prefix_tbl[i][NETMGR_REV_LINK].inst_max;
  }
  else
  {
    start = netmgr_main_dev_prefix_tbl[i][NETMGR_FWD_LINK].inst_min;
    end   = netmgr_main_dev_prefix_tbl[i][NETMGR_FWD_LINK].inst_max;
  }

  netmgr_log_med("netmgr_qmi_iwlan_update_link_assoc: finding assoc for link=%d [v4=0x%x, v6=0x%llx], type=%d, modem=%d in [%d, %d]\n",
                 link,
                 (addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4) ? addr_info->ipv4.if_addr.addr.v4 : 0u,
                 (addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6) ? addr_info->ipv6.if_addr.addr.v6_addr64[0] : 0llu,
                 type,
                 i,
                 start,
                 end);

  for (j = start; j <= end; ++j)
  {
    tmp_addr_info = netmgr_qmi_get_addr_info(j);

    if (NULL == tmp_addr_info)
    {
      continue;
    }

    if ((addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4) &&
        (tmp_addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV4))
    {
      netmgr_log_med("netmgr_qmi_iwlan_update_link_assoc: valid V4 addr link=%d [0x%x], link=%d [0x%x]\n",
                     link,
                     addr_info->ipv4.if_addr.addr.v4,
                     j,
                     tmp_addr_info->ipv4.if_addr.addr.v4);


      /* If a matching IPv4 address is found */
      if (addr_info->ipv4.if_addr.addr.v4 == tmp_addr_info->ipv4.if_addr.addr.v4)
      {
        break;
      }
    }
    else if ((addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6) &&
             (tmp_addr_info->valid_mask & NETMGR_ADDRSET_MASK_IPV6))
    {
      netmgr_log_med("netmgr_qmi_iwlan_update_link_assoc: valid V6 addr link=%d [0x%llx], link=%d [0x%llx]\n",
                     link,
                     (unsigned long long)addr_info->ipv6.if_addr.addr.v6_addr64[0],
                     j,
                     (unsigned long long)tmp_addr_info->ipv6.if_addr.addr.v6_addr64[0]);

      /* If a matching IPv6 prefix is found */
      if (addr_info->ipv6.if_addr.addr.v6_addr64[0] == tmp_addr_info->ipv6.if_addr.addr.v6_addr64[0])
      {
        break;
      }
    }
  }

  if (j > end)
  {
    netmgr_log_med("netmgr_qmi_iwlan_update_link_assoc: unable to find a matching association for link=%d\n",
                   link);

    goto bail;
  }

  netmgr_log_med("netmgr_qmi_iwlan_update_link_assoc: associating link=%d with link=%d\n",
                 link,
                 j);

  netmgr_qmi_cfg.links[link].assoc_link = j;
  netmgr_qmi_cfg.links[j].assoc_link = link;

  if (NULL != assoc_link)
  {
    *assoc_link = j;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_iwlan_clear_link_assoc
===========================================================================*/
/*!
@brief
  Removes the association between the given forward and reverse links.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_iwlan_clear_link_assoc
(
  int  fwd_link,
  int  rev_link
)
{
  int ret_fwd = NETMGR_FAILURE;
  int ret_rev = NETMGR_FAILURE;
  int ret;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link (fwd_link))
  {
    netmgr_log_med("netmgr_qmi_iwlan_clear_link_assoc: invalid fwd_link=%d\n",
                   fwd_link);
  }
  else
  {
    netmgr_qmi_cfg.links[fwd_link].assoc_link = NETMGR_LINK_MAX;
    ret_fwd = NETMGR_SUCCESS;
  }

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link (rev_link))
  {
    netmgr_log_med("netmgr_qmi_iwlan_clear_link_assoc: invalid rev_link=%d\n",
                   rev_link);
  }
  else
  {
    netmgr_qmi_cfg.links[rev_link].assoc_link = NETMGR_LINK_MAX;
    ret_rev = NETMGR_SUCCESS;
  }

  ret = (NETMGR_SUCCESS == ret_fwd || NETMGR_SUCCESS == ret_rev) ?
        NETMGR_SUCCESS :
        NETMGR_FAILURE;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_iwlan_get_link_assoc
===========================================================================*/
/*!
@brief
  Returns the corresponding link associated with the given forward or reverse
  iWLAN links.

@return
  A valid link on success, NETMGR_LINK_MAX on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_iwlan_get_link_assoc
(
  int  link
)
{
  int ret = NETMGR_LINK_MAX;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link (link))
  {
    netmgr_log_err("netmgr_qmi_iwlan_get_link_assoc: invalid link=%d\n",
                   link);
    goto bail;
  }

  ret = netmgr_qmi_cfg.links[link].assoc_link;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_save_rev_ip_config_status
===========================================================================*/
/*!
@brief
  Saves the reverse IP config status for the given link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_save_rev_ip_config_status
(
  int  link,
  int  ip_family,
  int  status
)
{
  int ret = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link( link ))
  {
    netmgr_log_err("netmgr_qmi_save_rev_ip_config_status: invalid link=%d\n",
                   link);
    goto bail;
  }

  if (AF_INET != ip_family && AF_INET6 != ip_family)
  {
    netmgr_log_err("netmgr_qmi_save_rev_ip_config_status: invalid ip_family=%d\n",
                   ip_family);
    goto bail;
  }

  if (NETMGR_SUCCESS != status && NETMGR_FAILURE != status)
  {
    netmgr_log_err("netmgr_qmi_save_rev_ip_config_status: invalid status=%d\n",
                   status);
    goto bail;
  }

  netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_config_status.ip_family = ip_family;
  netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_config_status.status = status;

  /* Set the state info as valid */
  netmgr_qmi_cfg.links[link].wds_info.rev_ip_config_status.is_valid = TRUE;

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_retrieve_rev_ip_config_status
===========================================================================*/
/*!
@brief
  Retrieves the reverse IP config status for the given link

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_retrieve_rev_ip_config_status
(
  int  link,
  int  *ip_family,
  int  *status
)
{
  int ret = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_SUCCESS != netmgr_qmi_verify_link( link ))
  {
    netmgr_log_err("netmgr_qmi_retrieve_rev_ip_config_status: invalid link=%d\n",
                   link);
    goto bail;
  }

  if (!ip_family || !status)
  {
    netmgr_log_err("netmgr_qmi_retrieve_rev_ip_config_status: invalid input params\n");
    goto bail;
  }

  if (FALSE == netmgr_qmi_cfg.links[link].wds_info.rev_ip_config_status.is_valid)
  {
    netmgr_log_med("netmgr_qmi_retrieve_rev_ip_config_status: invalid state info\n");
    goto bail;
  }

  *ip_family = netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_config_status.ip_family;
  *status = netmgr_qmi_cfg.links[ link ].wds_info.rev_ip_config_status.status;

  /* Invalidate the status info upon retrieval */
  netmgr_qmi_cfg.links[link].wds_info.rev_ip_config_status.is_valid = FALSE;

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_initiate_esp_rekey
===========================================================================*/
/*!
@brief
  Sends ESP rekey message

@return
  NETMGR_SUCCESS on operation success
  NETMGR_FAILURE otherwise

@note
  - Dependencies
    - ESP sequence number has overflown

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_qmi_initiate_esp_rekey
(
  int link,
  netmgr_qmi_client_type_t qmi_client
)
{
  int rc;
  int ret = NETMGR_FAILURE;
  qmi_client_type wds_hndl = NETMGR_QMI_CLIENT_INVALID;

  NETMGR_LOG_FUNC_ENTRY;

  do
  {
    /* Validate input parameters */
    if (NETMGR_QMI_CLIENT_MAX <= qmi_client ||
        (NETMGR_QMI_CLIENT_IPV4 != qmi_client &&
        NETMGR_QMI_CLIENT_IPV6 != qmi_client))
    {
      netmgr_log_err("%s", "netmgr_qmi_initiate_esp_rekey: Invalid QMI client");
      break;
    }

    wds_hndl = netmgr_qmi_wds_get_clnt_id(link, qmi_client);

    if (NETMGR_QMI_CLIENT_INVALID == wds_hndl)
    {
      netmgr_log_err("netmgr_qmi_initiate_esp_rekey: Unable to retrive valid qmi client for link [%d]",
                     link);
      break;
    }

    /* Send QMI message */
    rc = netmgr_qmi_wds_initiate_esp_rekey(link, wds_hndl);

    if (NETMGR_SUCCESS != rc)
    {
      netmgr_log_err("netmgr_qmi_initiate_esp_rekey: qmi_wds_initiate_esp_rekey failed [%d] for link [%d]",
                     rc, link);
      break;
    }

    /* Rekey succeeded */
    ret = NETMGR_SUCCESS;
  } while (0);

  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_rev_ip_ind_registration
===========================================================================*/
/*!
@brief
  Register for reverse IP transport indications on reverse links

@return
  NETMGR_SUCCESS on operation success
  NETMGR_FAILURE otherwise

@note
  - Dependencies
    - Corresponding QMI clients should have been created for the link

  - Side Effects
    - Registers for QMI indications
*/
/*=========================================================================*/
int
netmgr_qmi_rev_ip_ind_registration
(
  int link,
  wds_ip_family_enum_v01  ip_family
)
{
  int                       ret = NETMGR_FAILURE;
  const char*               qmi_cid = netmgr_qmi_get_conn_id_for_link(link);
  qmi_client_type           *clnt_hndl = NULL;
  netmgr_qmi_client_type_t  clnt;

  NETMGR_LOG_FUNC_ENTRY;

  /* Check if the link is enabled and initialized */
  if (netmgr_qmi_cfg.link_array[link].enabled)
  {
    /* Check if link is a reverse link */
    if (!qmi_cid || !NETMGR_IS_REV_IP_TRANS_CONN_ID(qmi_cid))
    {
      netmgr_log_low("%s(): Link [%d] is not a reverse rmnet link, skipping", __func__, link);
      ret = NETMGR_SUCCESS;
      goto bail;
    }

    clnt = netmgr_qmi_convert_ip_pref_to_client_type(ip_family);
    clnt_hndl = &netmgr_qmi_cfg.links[ link ].wds_info.clnt_hdl[clnt];

    if (NETMGR_QMI_CLIENT_INVALID == *clnt_hndl)
    {
      netmgr_log_err("%s(): Failed to get WDS client [%d] for link [%d]",
                     __func__, clnt, link);
      goto bail;
    }

    /* Register for indication */
    if (NETMGR_SUCCESS != netmgr_qmi_wds_rev_ip_ind_reg(link,
                                                        *clnt_hndl))
    {
      netmgr_log_err("%s(): WDS reverse ip transport indication registration failed"
                     " for clnt [%d] on link [%d]",
                     __func__, clnt, link);
      goto bail;
    }

    netmgr_log_low("%s(): Successfully registered for rev ip transport indications"
                   " for client [%d] on link [%d]",
                   __func__, clnt, link);
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION netmgr_qmi_rev_ip_ind_reg_all
===========================================================================*/
/*!
@brief
  Routine that registers for rev_ip indications for all rev links for both
  families.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

static void *netmgr_qmi_rev_ip_ind_reg_all(void *arg)
{
  int link = 0;

  (void) arg;

  if(!netmgr_main_cfg.iwlan_client_enable)
  {
    for (link = 0; link < NETMGR_MAX_LINK; link++)
    {
      netmgr_qmi_rev_ip_ind_registration(link,WDS_IP_FAMILY_IPV4_V01);
      netmgr_qmi_rev_ip_ind_registration(link, WDS_IP_FAMILY_IPV6_V01);
    }
  }
  return 0;
}

#endif /* FEATURE_DATA_IWLAN */


/*===========================================================================
  FUNCTION  netmgr_qmi_update_data_format_config
===========================================================================*/
/*!
@brief
  Updates the qmi data format with the main configuration data format

@return
  void

*/
/*=========================================================================*/
void netmgr_qmi_update_data_format_config(netmgr_data_format_t data_format)
{
    netmgr_qmi_cfg.data_format = data_format;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_init
===========================================================================*/
/*!
@brief
 Main initialization routine of the QMI WDS Interface module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Initializes the QMI Driver
*/
/*=========================================================================*/
void
netmgr_qmi_init (int nlink, netmgr_ctl_port_config_type links[], netmgr_data_format_t data_format)
{
  struct qmi_vtbl vtable;

  NETMGR_LOG_FUNC_ENTRY;

  /* Set number of links in the configuration blob */
  netmgr_qmi_cfg.nlink = nlink;
  netmgr_qmi_cfg.link_array = links;   /* Needed for reset */
  netmgr_qmi_cfg.data_format = data_format;   /* Needed for reset */

  /* Register with Platform layer */
  vtable.out_of_service    = netmgr_qmi_driver_cleanup;
  vtable.reset             = netmgr_qmi_reset_connection;
  vtable.verify            = netmgr_qmi_verify_link;
  vtable.dispatch          = netmgr_qmi_cmd_exec;
  vtable.qos_get_flow_info = netmgr_qmi_get_flow_info;

  if( NETMGR_SUCCESS !=
      netmgr_platform_register_vtbl( NETMGR_PLATFORM_VTBL_QMI,
                                     (void*)&vtable ) )
  {
    NETMGR_ABORT("netmgr_qmi_init: cannot register vtable with platform layer");
    return;
  }

  /* Create plugin table entries for QMI */
  netmgr_cb_create_table(NETMGR_QMI_CB_TABLE);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_OOS);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_OOS_COMPLETE);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_IS);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_LINK_OOS);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_LINK_IS);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_DFS_REVIP_FILTER_IND);
  netmgr_cb_create_chain(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_IS_COMPLETE);

  /* Initialize the QMI Client Driver and start WDS clients for each
  ** interface.
  */
  netmgr_qmi_driver_init(data_format);

#ifdef FEATURE_DATA_IWLAN

  netmgr_cb_insert_cb(NETMGR_QMI_CB_TABLE,
                      NETMGR_QMI_CB_MODEM_IS_COMPLETE,
                      NETMGR_QMI_CB_MODNAME,
                      &netmgr_qmi_rev_ip_ind_reg_all);

#endif /* FEATURE_DATA_IWLAN */

  NETMGR_LOG_FUNC_EXIT;
  return;
}
