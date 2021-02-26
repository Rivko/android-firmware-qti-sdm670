#ifndef _QCMAP_UTIL_H_
#define _QCMAP_UTIL_H_

/******************************************************************************

                           QCMAP_UTIL.H

******************************************************************************/

/******************************************************************************

  @file    qcmap_util.h
  @brief   QCMAP Utility API

  DESCRIPTION
  Header file for QCMAP UTILITY.

  ---------------------------------------------------------------------------
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
10/15/14   vm         Initial Module
01/05/15   rk         qtimap offtarget support.
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <netinet/ether.h>
#include <stdbool.h>
#include "ds_util.h"

#ifdef FEATURE_QTIMAP_OFFTARGET
#include <tf_qcmap.h>
#endif

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

#define QCMAP_NL_MAX_NUM_OF_FD              10
#define QCMAP_NL_MSG_MAX_LEN                16384
#define IF_NAME_LEN                         16
#define QCMAP_UTIL_SUCCESS                    0
#define QCMAP_UTIL_FAILURE                    (-1)

#define QCMAP_NL_RNDIS_INTERFACE            "rndis0"
#define QCMAP_NL_ECM_INTERFACE              "ecm0"
#define QCMAP_NL_PPP_INTERFACE              "ppp0"
#define QCMAP_NL_ETH_INTERFACE              "eth0"
#define QCMAP_NL_WLAN0_INTERFACE            "wlan0"
#define QCMAP_NL_WLAN1_INTERFACE            "wlan1"
#define QCMAP_NL_WLAN2_INTERFACE            "wlan2"
#define QCMAP_NL_WLAN3_INTERFACE            "wlan3"

#define QCMAP_GET_IFINDEX 1
#define QCMAP_GET_IFNAME 2

/*===========================================================================
LOG Msg Macros
=============================================================================*/
#ifndef FEATURE_QTIMAP_OFFTARGET
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}
#endif
/*===========================================================================
                              VARIABLE DECLARARTIONS
===========================================================================*/
/*---------------------------------------------------------------------------
   Function pointer registered with the socket listener
   This function is used for reading from a socket on receipt of an incoming
   netlink event
---------------------------------------------------------------------------*/
typedef int (* qcmap_sock_thrd_fd_read_f) (int fd);

/*--------------------------------------------------------------------------
   Stores the mapping of a socket descriptor and its associated read
   function
---------------------------------------------------------------------------*/
typedef struct
{
 int sk_fd;
 qcmap_sock_thrd_fd_read_f read_func;
} qcmap_sk_fd_map_info_t;

/*--------------------------------------------------------------------------
   Stores the socket information associated with netlink sockets required
   to listen to netlink events
---------------------------------------------------------------------------*/
typedef struct
{
 qcmap_sk_fd_map_info_t sk_fds[QCMAP_NL_MAX_NUM_OF_FD];
 fd_set fdset;
 int num_fd;
 int max_fd;
} qcmap_sk_fd_set_info_t;

/*--------------------------------------------------------------------------
   Socket descriptor paramters
---------------------------------------------------------------------------*/
typedef struct
{
 int                 sk_fd;       /* socket descriptor */
 struct sockaddr_nl  sk_addr_loc; /*  stores socket parameters */
} qcmap_sk_info_t;

/*--------------------------------------------------------------------------
   Packet socket descriptor paramters
---------------------------------------------------------------------------*/
typedef struct
{
 int                 sk_fd;       /* socket descriptor */
} qcmap_pf_sk_info_t;

/*--------------------------------------------------------------------------
   Stoes the metainfo present in the incoming netlink message
---------------------------------------------------------------------------*/
typedef struct
{
 struct ifinfomsg  metainfo;
} qcmap_nl_link_info_t;

/*-------------------------------------------------------------------------
   Stores the address info present in incoming netlink message
--------------------------------------------------------------------------*/
typedef struct qcmap_nl_if_addr_s {
        struct ifaddrmsg          metainfo;      /*from header*/
        struct                                   /*attributes*/
        {
          struct in_addr               ifa_local_v4;
          struct in6_addr              ifa_local_v6;
          struct sockaddr_storage ip6_addr;
          uint8_t                 prefix_len;
        } attr_info;
} qcmap_nl_if_addr_t;

/*--------------------------------------------------------------------------
   Stoes the neighbor info present in the incoming netlink message
---------------------------------------------------------------------------*/
typedef struct qcmap_nl_neigh_info_s {
        struct ndmsg                  metainfo;     /* from header */
        struct                                      /* attributes  */
        {
          unsigned int                param_mask;
          struct sockaddr_storage     local_addr;
          struct  sockaddr            lladdr_hwaddr;
        } attr_info;
} qcmap_nl_neigh_info_t;

/*--------------------------------------------------------------------------
   Stoes the Route info present in the incoming netlink message
---------------------------------------------------------------------------*/
#define QCMAP_NL_ROUTE_INFO_DST_ADDR 0x0001
#define QCMAP_NL_ROUTE_INFO_IFINDEX 0x0002
typedef struct qcmap_nl_route_info_s {
        struct rtmsg                   metainfo;     /* from header */
        struct                                      /* attributes  */
        {
          unsigned int                 param_mask;
          struct in6_addr              dst_addr;
          int                          ifindex;
        } attr_info;
} qcmap_nl_route_info_t;

/*--------------------------------------------------------------------------
   Netlink message: used to decode the incoming netlink message
---------------------------------------------------------------------------*/
typedef struct
{
 unsigned int type;
 bool link_event;
 qcmap_nl_neigh_info_t  nl_neigh_info;
 qcmap_nl_if_addr_t nl_if_addr;
 qcmap_nl_route_info_t  nl_route_info;
 qcmap_nl_link_info_t nl_link_info;
}qcmap_nl_msg_t;

/*--------------------------------------------------------------------------
  Netlink message structure used to send GET_LINK
---------------------------------------------------------------------------*/
typedef struct
{
  struct nlmsghdr hdr;
  struct rtgenmsg gen;
}qcmap_nl_req_type;


typedef enum
{
  DECODE_RTM_NEIGH = 0,
  DECODE_RTM_ADDR,
  DECODE_RTM_ROUTE
}qcmap_decode_nl_type;

/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

int qcmap_nl_decode_rtm_entries(const char *, unsigned int,
                                qcmap_nl_msg_t *,
                                qcmap_decode_nl_type);

int qcmap_nl_decode_rtm_link(const char *, unsigned int, qcmap_nl_link_info_t *);

int qcmap_nl_query_if(qcmap_sk_info_t *sk_info, unsigned int);

int qcmap_nl_listener_init
(
  unsigned int              nl_type,
  unsigned int              nl_groups,
  qcmap_sk_fd_set_info_t *sk_fdset,
  qcmap_sock_thrd_fd_read_f read_f,
  qcmap_sk_info_t *sk_info,
  int maxfds
);

int qcmap_nl_decode_link_nlmsg
(
  const char     *buffer,
  unsigned int   buflen,
  qcmap_nl_msg_t   *msg_ptr,
  int nlmsg_type
);

void qcmap_nl_release_msg
(
  struct msghdr * msgh
);

int qcmap_nl_recv
(
  int              fd,
  struct msghdr    **msg_pptr,
  unsigned int     *msglen_ptr
);

int qcmap_nl_addfd_map
(
  qcmap_sk_fd_set_info_t   *fd_set,
  int                            fd,
  qcmap_sock_thrd_fd_read_f   read_f,
  int maxfds
);

int qcmap_listener_start
(
  qcmap_sk_fd_set_info_t    * sk_fd_set
);

int qcmap_nl_is_recv_on_dev
(
  const char *dev_type,
  char * dev_name
);

int qcmap_nl_get_interface_details
(
  char *   if_name,
  int *    if_index,
  int      detail
);

int qcmap_nl_decode_rtm_route
(
  const char              *buffer,
  unsigned int             buflen,
  qcmap_nl_route_info_t *route_info
);

int qcmap_nl_decode_rtm_addr
(
  const char              *buffer,
  unsigned int             buflen,
  qcmap_nl_if_addr_t      *if_addr_info
);

int qcmap_nl_decode_rtm_neigh
(
  const char              *buffer,
  unsigned int             buflen,
  struct qcmap_nl_neigh_info_s   *neigh_info
);

#ifdef __cplusplus
}
#endif

#endif /*_QCMAP_NETLINK_H_*/

