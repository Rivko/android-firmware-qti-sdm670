#ifndef _IPACM_PERF_NETLINK_H_
#define _IPACM_PERF_NETLINK_H_
/******************************************************************************

                        IPACM_PERF_NETLINK.H

******************************************************************************/

/******************************************************************************

  @file ipacm_perf_netlink.h
  @brief ipacm_perf_netlink header file.

  DESCRIPTION
  Header file of ipacm_perf_netlink.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when        who       what, where, why
--------   -----      -------------------------------------------------------
03/05/15   shihuanl   Initial version

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <linux/if.h>

#define MAX_NUM_OF_FD (10)
#define IPA_NL_MSG_MAX_LEN (2048)
#define IF_NAME_LEN (16)
#define RNDIS_IFACE_NAME "rndis0"
#define ECM_IFACE_NAME "ecm0"
#define CPE_IFACE_NAME "eth0"

typedef int (*ipacm_perf_sock_thrd_fd_read_f)(int fd);

typedef struct
{
	int sk_fd;
	ipacm_perf_sock_thrd_fd_read_f read_func;
} ipacm_perf_nl_sk_fd_map_info_t;

typedef struct
{
	ipacm_perf_nl_sk_fd_map_info_t sk_fds[MAX_NUM_OF_FD];
	fd_set fdset;
	int num_fd;
	int max_fd;
} ipacm_perf_nl_sk_fd_set_info_t;

typedef struct
{
	int                 sk_fd;       /* socket descriptor */
	struct sockaddr_nl  sk_addr_loc; /* local address of socket */
} ipacm_perf_nl_sk_info_t;

typedef struct
{
	struct ifinfomsg  metainfo;                   /* from header */
} ipacm_perf_nl_link_info_t;

typedef struct
{
	unsigned int type;
	/* Optional parameters */
	ipacm_perf_nl_link_info_t nl_link_info;
} ipacm_perf_nl_msg_t;

/* start netlink socket monitor*/
void* netlink_start(void *param);

/* Initialization routine for listener on NetLink sockets interface */
static int ipacm_perf_nl_listener_init
(
	unsigned int nl_type,
	unsigned int nl_groups,
	ipacm_perf_nl_sk_fd_set_info_t *sk_fdset,
	ipacm_perf_sock_thrd_fd_read_f read_f
);

/* Opens a netlink socket*/
static int ipacm_perf_nl_open_socket
(
	ipacm_perf_nl_sk_info_t *sk_info,
	int protocol,
	unsigned int grps
);

/* Add fd to fdmap array and store read handler function ptr (up to MAX_NUM_OF_FD).*/
static int ipacm_perf_nl_addfd_map
(
	ipacm_perf_nl_sk_fd_set_info_t *info,
	int fd,
	ipacm_perf_sock_thrd_fd_read_f read_f
);

/*  start socket listener */
static int ipacm_perf_nl_sock_listener_start
(
	ipacm_perf_nl_sk_fd_set_info_t *sk_fd_set
);

/*  function registered to receive incoming messages over the NETLINK routing socket*/
static int ipacm_perf_nl_recv_msg(int fd);

/* receive and process nl message */
static int ipacm_perf_nl_recv
(
	int              fd,
	struct msghdr **msg_pptr,
	unsigned int  *msglen_ptr
);

/* allocate memory for nl msg*/
static struct msghdr* ipacm_perf_nl_alloc_msg
(
	uint32_t msglen
);

/* release memory for nl msg */
static void ipacm_perf_nl_release_msg
(
	struct msghdr *msgh
);

/* decode the nl msg */
static int ipacm_perf_nl_decode_nlmsg
(
	const char   *buffer,
	unsigned int  buflen,
	ipacm_perf_nl_msg_t  *msg_ptr
);

/* decode the rtm netlink message */
static int ipacm_perf_nl_decode_rtm_link
(
	const char              *buffer,
	unsigned int             buflen,
	ipacm_perf_nl_link_info_t      *link_info
);

/*  get interface name */
static int ipacm_perf_get_if_name
(
	char *if_name,
	unsigned int if_len,
	int if_index
);

#endif /* _IPACM_PERF_NETLINK_H_ */

