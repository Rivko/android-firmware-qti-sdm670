/******************************************************************************

                        IPACM_PERF_NETLINK.CPP

******************************************************************************/

/******************************************************************************

  @file    ipacm_perf_netlink.cpp
  @brief   netlink listener module for ipacm_perf

  DESCRIPTION
  Implementation of ipacm_perf_netlink.

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

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "ipacm_perf.h"
#include "ipacm_perf_netlink.h"
#include "ipacm_perf_rm.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

/* start netlink socket monitor*/
void* netlink_start(void *param)
{
	ipacm_perf_nl_sk_fd_set_info_t sk_fdset;
	int ret = 0;
	memset(&sk_fdset, 0, sizeof(ipacm_perf_nl_sk_fd_set_info_t));

	ret = ipacm_perf_nl_listener_init(NETLINK_ROUTE, RTMGRP_LINK, &sk_fdset, ipacm_perf_nl_recv_msg);

	if (ret != IPACM_PERF_SUCCESS)
	{
		IPACM_PERF_ERR("Failed to initialize IPA netlink event listener\n");
	}

	return NULL;
}

/* Initialization routine for listener on NetLink sockets interface */
int ipacm_perf_nl_listener_init
(
	unsigned int nl_type,
	unsigned int nl_groups,
	ipacm_perf_nl_sk_fd_set_info_t *sk_fdset,
	ipacm_perf_sock_thrd_fd_read_f read_f
)
{
	ipacm_perf_nl_sk_info_t sk_info;
	int ret;

	memset(&sk_info, 0, sizeof(ipacm_perf_nl_sk_info_t));
	IPACM_PERF_DBG("Entering IPA NL listener init\n");

	if(ipacm_perf_nl_open_socket(&sk_info, nl_type, nl_groups) == IPACM_PERF_SUCCESS)
	{
		IPACM_PERF_DBG("IPA Open netlink socket succeeds\n");
	}
	else
	{
		IPACM_PERF_ERR("Netlink socket open failed\n");
		return IPACM_PERF_FAILURE;
	}

	/* Add NETLINK socket to the list of sockets that the listener
					 thread should listen on. */

	if(ipacm_perf_nl_addfd_map(sk_fdset, sk_info.sk_fd, read_f) != IPACM_PERF_SUCCESS)
	{
		IPACM_PERF_ERR("Failed to add nl routing sock for reading\n");
		close(sk_info.sk_fd);
		return IPACM_PERF_FAILURE;
	}

	/* Start the socket listener thread */
	ret = ipacm_perf_nl_sock_listener_start(sk_fdset);

	if(ret != IPACM_PERF_SUCCESS)
	{
		IPACM_PERF_ERR("Failed to start NL listener\n");
		close(sk_info.sk_fd);
		return IPACM_PERF_FAILURE;
	}

	return IPACM_PERF_SUCCESS;
}

/* Opens a netlink socket*/
static int ipacm_perf_nl_open_socket
(
	ipacm_perf_nl_sk_info_t *sk_info,
	int protocol,
	unsigned int grps
)
{
	int *p_sk_fd;
	int buf_size = 6669999, sendbuff=0, res;
	struct sockaddr_nl *p_sk_addr_loc;
	socklen_t optlen;

	p_sk_fd = &(sk_info->sk_fd);
	p_sk_addr_loc = &(sk_info->sk_addr_loc);

	/* Open netlink socket for specified protocol */
	if((*p_sk_fd = socket(AF_NETLINK, SOCK_RAW, protocol)) < 0)
	{
		IPACM_PERF_ERR("Failed to open netlink socket\n");
		return IPACM_PERF_FAILURE;
	}

	optlen = sizeof(sendbuff);
	res = getsockopt(*p_sk_fd, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);

	if(res == -1)
	{
		IPACM_PERF_DBG("Error getsockopt one\n");
	}
	else
	{
		IPACM_PERF_DBG("Orignal send buffer size = %d\n", sendbuff);
	}
	IPACM_PERF_DBG("Sets the send buffer to %d\n", buf_size);

	if(setsockopt(*p_sk_fd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(int)) == -1)
	{
		IPACM_PERF_DBG("Error setting socket opts\n");
	}

	/* Initialize socket addresses to null */
	memset(p_sk_addr_loc, 0, sizeof(struct sockaddr_nl));

	/* Populate local socket address using specified groups */
	p_sk_addr_loc->nl_family = AF_NETLINK;
	p_sk_addr_loc->nl_pid = getpid();
	p_sk_addr_loc->nl_groups = grps;

	/* Bind socket to the local address, i.e. specified groups. This ensures
	 that multicast messages for these groups are delivered over this
	 socket. */

	if(bind(*p_sk_fd, (struct sockaddr *)p_sk_addr_loc, sizeof(struct sockaddr_nl)) < 0)
	{
		IPACM_PERF_DBG("Socket bind failed\n");
		close(*p_sk_fd);
		return IPACM_PERF_FAILURE;
	}

	return IPACM_PERF_SUCCESS;
}

/* Add fd to fdmap array and store read handler function ptr (up to MAX_NUM_OF_FD).*/
static int ipacm_perf_nl_addfd_map
(
	ipacm_perf_nl_sk_fd_set_info_t *info,
	int fd,
	ipacm_perf_sock_thrd_fd_read_f read_f
)
{
	if(info->num_fd < MAX_NUM_OF_FD)
	{
		FD_SET(fd, &info->fdset);

		/* Add fd to fdmap array and store read handler function ptr */
		info->sk_fds[info->num_fd].sk_fd = fd;
		info->sk_fds[info->num_fd].read_func = read_f;

		/* Increment number of fds stored in fdmap */
		info->num_fd++;
		if(info->max_fd < fd)
			info->max_fd = fd;
	}
	else
	{
		return IPACM_PERF_FAILURE;
	}

	return IPACM_PERF_SUCCESS;
}

/*  start socket listener */
static int ipacm_perf_nl_sock_listener_start
(
	ipacm_perf_nl_sk_fd_set_info_t *sk_fd_set
)
{
	int i, ret;

	while(true)
	{
	    for(i = 0; i < sk_fd_set->num_fd; i++ )
		{
			FD_SET(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
		}

		if((ret = select(sk_fd_set->max_fd + 1, &(sk_fd_set->fdset), NULL, NULL, NULL)) < 0)
		{
			IPACM_PERF_ERR("ipa_nl select failed\n");
		}
		else
		{
			for(i = 0; i < sk_fd_set->num_fd; i++)
			{

				if(FD_ISSET(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset)))
				{

					if(sk_fd_set->sk_fds[i].read_func)
					{
						if(IPACM_PERF_SUCCESS != ((sk_fd_set->sk_fds[i].read_func)(sk_fd_set->sk_fds[i].sk_fd)))
						{
							IPACM_PERF_ERR("Error on read callback[%d] fd=%d\n", i,
											 sk_fd_set->sk_fds[i].sk_fd);
						}
						FD_CLR(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
					}
					else
					{
						IPACM_PERF_ERR("No read function\n");
						FD_CLR(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
					}
				}

			} /* end of for loop*/
		} /* end of else */
	} /* end of while */

	return IPACM_PERF_SUCCESS;
}

/*  function registered to receive incoming messages over the NETLINK routing socket*/
int ipacm_perf_nl_recv_msg(int fd)
{
	struct msghdr *msghdr = NULL;
	struct iovec *iov = NULL;
	unsigned int msglen = 0;
	ipacm_perf_nl_msg_t *nlmsg = NULL;

	nlmsg = (ipacm_perf_nl_msg_t *)malloc(sizeof(ipacm_perf_nl_msg_t));
	if(NULL == nlmsg)
	{
		IPACM_PERF_ERR("Failed to allocate nlmsg\n");
		goto error;
	}
	else
	{
		if(IPACM_PERF_SUCCESS != ipacm_perf_nl_recv(fd, &msghdr, &msglen))
		{
			IPACM_PERF_ERR("Failed to receive nl message\n");
			goto error;
		}

		if(msghdr== NULL)
		{
			IPACM_PERF_ERR("Failed to get msghdr\n");
			goto error;
		}

		iov = msghdr->msg_iov;

		memset(nlmsg, 0, sizeof(ipacm_perf_nl_msg_t));
		if(IPACM_PERF_SUCCESS != ipacm_perf_nl_decode_nlmsg((char *)iov->iov_base, msglen, nlmsg))
		{
			IPACM_PERF_ERR("Failed to decode nl message \n");
			goto error;
		}
		/* Release NetLink message buffer */
		if(msghdr)
		{
			ipacm_perf_nl_release_msg(msghdr);
		}
		if(nlmsg)
		{
			free(nlmsg);
		}
	}

	return IPACM_PERF_SUCCESS;

error:
	if(msghdr)
	{
		ipacm_perf_nl_release_msg(msghdr);
	}
	if(nlmsg)
	{
		free(nlmsg);
	}

	return IPACM_PERF_FAILURE;
}

/* receive and process nl message */
static int ipacm_perf_nl_recv
(
	int              fd,
	struct msghdr **msg_pptr,
	unsigned int  *msglen_ptr
)
{
	struct msghdr *msgh = NULL;
	int rmsgl;

	msgh = ipacm_perf_nl_alloc_msg(IPA_NL_MSG_MAX_LEN);
	if(NULL == msgh)
	{
		IPACM_PERF_ERR("Failed to allocate NL message\n");
		goto error;
	}

	/* Receive message over the socket */
	rmsgl = recvmsg(fd, msgh, 0);

	/* Verify that something was read */
	if(rmsgl <= 0)
	{
		IPACM_PERF_ERR("NL recv error");
		goto error;
	}

	/* Verify that NL address length in the received message is expected value */
	if(sizeof(struct sockaddr_nl) != msgh->msg_namelen)
	{
		IPACM_PERF_ERR("rcvd msg with namelen != sizeof sockaddr_nl\n");
		goto error;
	}

	/* Verify that message was not truncated. This should not occur */
	if(msgh->msg_flags & MSG_TRUNC)
	{
		IPACM_PERF_ERR("Rcvd msg is truncated!\n");
		goto error;
	}

	*msg_pptr    = msgh;
	*msglen_ptr = rmsgl;

	return IPACM_PERF_SUCCESS;

/* An error occurred while receiving the message. Free all memory before
				 returning. */
error:
	ipacm_perf_nl_release_msg(msgh);
	*msg_pptr    = NULL;
	*msglen_ptr  = 0;

	return IPACM_PERF_FAILURE;
}

/* allocate memory for nl msg */
static struct msghdr* ipacm_perf_nl_alloc_msg
(
	uint32_t msglen
)
{
	unsigned char *buf = NULL;
	struct sockaddr_nl *nladdr = NULL;
	struct iovec *iov = NULL;
	struct msghdr *msgh = NULL;

	if(IPA_NL_MSG_MAX_LEN < msglen)
	{
		IPACM_PERF_ERR("Netlink message exceeds maximum length\n");
		return NULL;
	}

	msgh = (struct msghdr *)malloc(sizeof(struct msghdr));
	if(msgh == NULL)
	{
		IPACM_PERF_ERR("Failed to allocate msghdr\n");
		return NULL;
	}

	nladdr = (struct sockaddr_nl *)malloc(sizeof(struct sockaddr_nl));
	if(nladdr == NULL)
	{
		IPACM_PERF_ERR("Failed to allocate sockaddr\n");
		free(msgh);
		return NULL;
	}

	iov = (struct iovec *)malloc(sizeof(struct iovec));
	if(iov == NULL)
	{
		IPACM_PERF_ERR("Failed to allocate iovec");
		free(nladdr);
		free(msgh);
		return NULL;
	}

	buf = (unsigned char *)malloc(msglen);
	if(buf == NULL)
	{
		IPACM_PERF_ERR("Failed to allocate mglen\n");
		free(iov);
		free(nladdr);
		free(msgh);
		return NULL;
	}

	memset(nladdr, 0, sizeof(struct sockaddr_nl));
	nladdr->nl_family = AF_NETLINK;

	memset(msgh, 0x0, sizeof(struct msghdr));
	msgh->msg_name = nladdr;
	msgh->msg_namelen = sizeof(struct sockaddr_nl);
	msgh->msg_iov = iov;
	msgh->msg_iovlen = 1;

	memset(iov, 0x0, sizeof(struct iovec));
	iov->iov_base = buf;
	iov->iov_len = msglen;

	return msgh;
}

/* release memory for nl msg */
static void ipacm_perf_nl_release_msg
(
	struct msghdr *msgh
)
{
	unsigned char *buf = NULL;
	struct sockaddr_nl *nladdr = NULL;
	struct iovec *iov = NULL;

	if(NULL == msgh)
	{
		return;
	}

	nladdr = (struct sockaddr_nl *)msgh->msg_name;
	iov = msgh->msg_iov;
	if(msgh->msg_iov)
	{
		buf = (unsigned char *)msgh->msg_iov->iov_base;
	}

	if(buf)
	{
		free(buf);
	}
	if(iov)
	{
		free(iov);
	}
	if(nladdr)
	{
		free(nladdr);
	}
	if(msgh)
	{
		free(msgh);
	}
	return;
}

/* decode the nl msg */
static int ipacm_perf_nl_decode_nlmsg
(
	const char   *buffer,
	unsigned int  buflen,
	ipacm_perf_nl_msg_t  *msg_ptr
)
{
	char dev_name[IF_NAME_LEN]={0};
	int ret_val;
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;
	ipacm_perf_rm* rm_ptr = NULL;

	while(NLMSG_OK(nlh, buflen))
	{
		memset(dev_name, 0, IF_NAME_LEN);
		IPACM_PERF_DBG("Received msg:%d from netlink\n", nlh->nlmsg_type)
		switch(nlh->nlmsg_type)
		{
		case RTM_NEWLINK:
			msg_ptr->type = nlh->nlmsg_type;
			if(IPACM_PERF_SUCCESS != ipacm_perf_nl_decode_rtm_link(buffer, buflen, &(msg_ptr->nl_link_info)))
			{
				IPACM_PERF_ERR("Failed to decode rtm link message\n");
				return IPACM_PERF_FAILURE;
			}
			else
			{
				IPACM_PERF_DBG("Got RTM_NEWLINK with below values\n");
				IPACM_PERF_DBG("RTM_NEWLINK, ifi_change:%d\n", msg_ptr->nl_link_info.metainfo.ifi_change);
				IPACM_PERF_DBG("RTM_NEWLINK, ifi_flags:%d\n", msg_ptr->nl_link_info.metainfo.ifi_flags);
				IPACM_PERF_DBG("RTM_NEWLINK, ifi_index:%d\n", msg_ptr->nl_link_info.metainfo.ifi_index);
				IPACM_PERF_DBG("RTM_NEWLINK, family:%d\n", msg_ptr->nl_link_info.metainfo.ifi_family);
#ifdef FEATURE_IPA_ANDROID
				if (msg_ptr->nl_link_info.metainfo.ifi_family == AF_BRIDGE)
				{
					IPACM_PERF_ERR("Ignore this RTM_NEWLINK msg\n");
					return IPACM_PERF_SUCCESS;
				}
#endif
				if(IFF_UP & msg_ptr->nl_link_info.metainfo.ifi_change)
				{
					IPACM_PERF_DBG("Got useful newlink event\n");
					ret_val = ipacm_perf_get_if_name(dev_name, IF_NAME_LEN, msg_ptr->nl_link_info.metainfo.ifi_index);
					if(ret_val != IPACM_PERF_SUCCESS)
					{
						IPACM_PERF_ERR("Error while getting interface name\n");
						return IPACM_PERF_FAILURE;
					}
					dev_name[IF_NAME_LEN-1] = '\0';
					if(msg_ptr->nl_link_info.metainfo.ifi_flags & IFF_UP)
					{
						IPACM_PERF_DBG("Interface %s bring up with IP-family: %d \n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_family);
						if( strcmp(dev_name, RNDIS_IFACE_NAME) == 0 || strcmp(dev_name, ECM_IFACE_NAME) == 0)
						{
							IPACM_PERF_DBG("USB interface comes up\n");
							rm_ptr = ipacm_perf_rm::get_instance();
							if(rm_ptr)
							{
								rm_ptr->set_monitored_iface_state(IFACE_USB, true);
							}
						}
						if(strcmp(dev_name, CPE_IFACE_NAME) == 0)
						{
							IPACM_PERF_DBG("CPE interface comes up\n");
							rm_ptr = ipacm_perf_rm::get_instance();
							if(rm_ptr)
							{
								rm_ptr->set_monitored_iface_state(IFACE_CPE, true);
							}
						}
					}
					else
					{
						IPACM_PERF_DBG("Interface %s bring down with IP-family: %d \n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_family);
						if( strcmp(dev_name, RNDIS_IFACE_NAME) == 0 || strcmp(dev_name, ECM_IFACE_NAME) == 0)
						{
							IPACM_PERF_DBG("USB interface goes down\n");
							rm_ptr = ipacm_perf_rm::get_instance();
							if(rm_ptr)
							{
								rm_ptr->set_monitored_iface_state(IFACE_USB, false);
							}
						}
						if(strcmp(dev_name, CPE_IFACE_NAME) == 0)
						{
							IPACM_PERF_DBG("CPE interface goes down\n");
							rm_ptr = ipacm_perf_rm::get_instance();
							if(rm_ptr)
							{
								rm_ptr->set_monitored_iface_state(IFACE_CPE, false);
							}
						}
					}
				}

				/* Add IPACM support for ECM plug-in/plug_out */
				/*--------------------------------------------------------------------------
                   Check if the interface is running.If its a RTM_NEWLINK and the interface
                ---------------------------------------------------------------------------*/
                if((msg_ptr->nl_link_info.metainfo.ifi_flags & IFF_RUNNING) &&
                        (msg_ptr->nl_link_info.metainfo.ifi_flags & IFF_LOWER_UP))
                {
					ret_val = ipacm_perf_get_if_name(dev_name, IF_NAME_LEN, msg_ptr->nl_link_info.metainfo.ifi_index);
					if(ret_val != IPACM_PERF_SUCCESS)
					{
						IPACM_PERF_ERR("Error while getting interface name\n");
						return IPACM_PERF_FAILURE;
					}
					dev_name[IF_NAME_LEN-1] = '\0';
					IPACM_PERF_DBG("Got a usb link_up event: Interface %s, index %d\n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_index);
					IPACM_PERF_DBG("Interface %s bring up with IP-family: %d \n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_family);
					if( strcmp(dev_name, RNDIS_IFACE_NAME) == 0 || strcmp(dev_name, ECM_IFACE_NAME) == 0)
					{
						IPACM_PERF_DBG("USB interface comes up\n");
						rm_ptr = ipacm_perf_rm::get_instance();
						if(rm_ptr)
						{
							rm_ptr->set_monitored_iface_state(IFACE_USB, true);
						}
					}
					if(strcmp(dev_name, CPE_IFACE_NAME) == 0)
					{
						IPACM_PERF_DBG("CPE interface comes up\n");
						rm_ptr = ipacm_perf_rm::get_instance();
						if(rm_ptr)
						{
							rm_ptr->set_monitored_iface_state(IFACE_CPE, true);
						}
					}
				}
				else if(!(msg_ptr->nl_link_info.metainfo.ifi_flags & IFF_LOWER_UP))
                {
					ret_val = ipacm_perf_get_if_name(dev_name, IF_NAME_LEN, msg_ptr->nl_link_info.metainfo.ifi_index);
					if(ret_val != IPACM_PERF_SUCCESS)
					{
						IPACM_PERF_ERR("Error while getting interface name\n");
						return IPACM_PERF_FAILURE;
					}
					dev_name[IF_NAME_LEN-1] = '\0';
					IPACM_PERF_DBG("Got a usb link_down event: Interface %s, index %d\n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_index);
					IPACM_PERF_DBG("Interface %s bring down with IP-family: %d \n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_family);
					if( strcmp(dev_name, RNDIS_IFACE_NAME) == 0 || strcmp(dev_name, ECM_IFACE_NAME) == 0)
					{
						IPACM_PERF_DBG("USB interface goes down\n");
						rm_ptr = ipacm_perf_rm::get_instance();
						if(rm_ptr)
						{
							rm_ptr->set_monitored_iface_state(IFACE_USB, false);
						}
					}
					if(strcmp(dev_name, CPE_IFACE_NAME) == 0)
					{
						IPACM_PERF_DBG("CPE interface goes down\n");
						rm_ptr = ipacm_perf_rm::get_instance();
						if(rm_ptr)
						{
							rm_ptr->set_monitored_iface_state(IFACE_CPE, false);
						}
					}
				}
			}
			break;

		case RTM_DELLINK:
			IPACM_PERF_DBG("Got dellink event\n");
			msg_ptr->type = nlh->nlmsg_type;
			IPACM_PERF_DBG("Entering rtm decode\n");
			if(IPACM_PERF_SUCCESS != ipacm_perf_nl_decode_rtm_link(buffer, buflen, &(msg_ptr->nl_link_info)))
			{
				IPACM_PERF_ERR("Failed to decode rtm link message\n");
				return IPACM_PERF_FAILURE;
			}
			else
			{
				IPACM_PERF_DBG("Got RTM_DELLINK with below values\n");
				IPACM_PERF_DBG("RTM_DELLINK, ifi_change:%d\n", msg_ptr->nl_link_info.metainfo.ifi_change);
				IPACM_PERF_DBG("RTM_DELLINK, ifi_flags:%d\n", msg_ptr->nl_link_info.metainfo.ifi_flags);
				IPACM_PERF_DBG("RTM_DELLINK, ifi_index:%d\n", msg_ptr->nl_link_info.metainfo.ifi_index);
				IPACM_PERF_DBG("RTM_DELLINK, family:%d\n", msg_ptr->nl_link_info.metainfo.ifi_family);
#ifdef FEATURE_IPA_ANDROID
				if (msg_ptr->nl_link_info.metainfo.ifi_family == AF_BRIDGE)
				{
					IPACM_PERF_ERR("Ignore this RTM_DELLINK msg \n");
					return IPACM_PERF_SUCCESS;
				}
#endif
				ret_val = ipacm_perf_get_if_name(dev_name, IF_NAME_LEN, msg_ptr->nl_link_info.metainfo.ifi_index);
				if(ret_val != IPACM_PERF_SUCCESS)
				{
					IPACM_PERF_ERR("Error while getting interface name\n");
					return IPACM_PERF_FAILURE;
				}
				dev_name[IF_NAME_LEN-1] = '\0';
				IPACM_PERF_DBG("Interface %s bring down \n", dev_name);

				if( strcmp(dev_name, RNDIS_IFACE_NAME) == 0 || strcmp(dev_name, ECM_IFACE_NAME) == 0)
				{
					IPACM_PERF_DBG("USB interface goes down\n");
					rm_ptr = ipacm_perf_rm::get_instance();
					if(rm_ptr)
					{
						rm_ptr->set_monitored_iface_state(IFACE_USB, false);
					}
				}
				if(strcmp(dev_name, CPE_IFACE_NAME) == 0)
				{
					IPACM_PERF_DBG("CPE interface goes down\n");
					rm_ptr = ipacm_perf_rm::get_instance();
					if(rm_ptr)
					{
						rm_ptr->set_monitored_iface_state(IFACE_CPE, false);
					}
				}
			}
			break;

		default:
			IPACM_PERF_DBG("Ignore NL event %d\n ", nlh->nlmsg_type);
			break;

		}
		nlh = NLMSG_NEXT(nlh, buflen);
	}
	return IPACM_PERF_SUCCESS;
}

/* decode the rtm netlink message */
static int ipacm_perf_nl_decode_rtm_link
(
	 const char              *buffer,
	 unsigned int             buflen,
	 ipacm_perf_nl_link_info_t      *link_info
)
{
	/* NL message header */
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;

	/* Extract the header data */
	link_info->metainfo = *(struct ifinfomsg *)NLMSG_DATA(nlh);
	buflen -= sizeof(struct nlmsghdr);

	return IPACM_PERF_SUCCESS;
}

/*  get interface name */
int ipacm_perf_get_if_name
(
	char *if_name,
	unsigned int if_len,
	int if_index
)
{
	int fd;
	struct ifreq ifr;

	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		IPACM_PERF_ERR("Get interface name socket create failed\n");
		return IPACM_PERF_FAILURE;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	ifr.ifr_ifindex = if_index;
	IPACM_PERF_DBG("Interface index %d\n", if_index);

	if(ioctl(fd, SIOCGIFNAME, &ifr) < 0)
	{
		IPACM_PERF_ERR("call_ioctl_on_dev: ioctl failed:\n");
		close(fd);
		return IPACM_PERF_FAILURE;
	}

	(void)strlcpy(if_name, ifr.ifr_name, if_len);
	IPACM_PERF_DBG("Interface name %s\n", ifr.ifr_name);
	close(fd);

	return IPACM_PERF_SUCCESS;
}
