/******************************************************************************

                        N E T M G R _ Q M I _ C B . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_cb.h
  @brief   Network Manager QMI Module header file

  DESCRIPTION
  Header file for QMI module plugin interface

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __NETMGR_QMI_CB_H__
#define __NETMGR_QMI_CB_H__

#define NETMGR_QMI_CB_TABLE   "QMI"

#define NETMGR_QMI_CB_MODEM_OOS      "MODEM_OOS"      /* Global SSR Cleanup      */
#define NETMGR_QMI_CB_MODEM_OOS_COMPLETE  "MODEM_OOS_COMPLETE"  /* Global SSR after per link cleanup      */
#define NETMGR_QMI_CB_MODEM_IS       "MODEM_IS"       /* Global SSR In-Service   */
#define NETMGR_QMI_CB_MODEM_IS_COMPLETE  "MODEM_IS_COMPLETE"  /* Global SSR after per link init      */

#define NETMGR_QMI_CB_MODNAME "QMI"

/* Below take (int *link) as arguments */
#define NETMGR_QMI_CB_LINK_OOS   "LINK_OOS"     /* Per-link SSR Cleanup    */
#define NETMGR_QMI_CB_LINK_IS    "LINK_IS"      /* Per-link SSR In-Service */

#define NETMGR_QMI_CB_DFS_REVIP_FILTER_IND     "DFS_FILTER_IND"

#define NETMGR_QMI_CB_IFTYPE_FORWARD     1
#define NETMGR_QMI_CB_IFTYPE_REVERSE     2

typedef enum
{
  REV_IP_FILTER_OP_INVALID = -1,
  REV_IP_FILTER_OP_ADD,
  REV_IP_FILTER_OP_REMOVE,
  REV_IP_FILTER_OP_MAX
} rev_ip_filter_op_t;

struct netmgr_qmi_cb_ifinfo
{
  int                 link;                            /* Netmgr internal link number */
  int                 type;                            /* Forward/Reverse */
  int                 ip_family;                       /* IP family */
  unsigned int        spi;                             /* SPI value to be used in packet
                                                          marking rules */
  rev_ip_filter_op_t  filter_op;                       /* Add/Remove rule */
};

#endif /* __NETMGR_QMI_CB_H__ */
