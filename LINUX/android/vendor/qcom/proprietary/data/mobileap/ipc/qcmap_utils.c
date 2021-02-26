/******************************************************************************

                        QCMAP_UTILS.C

******************************************************************************/

/******************************************************************************

  @file    qcmap_utils.c
  @brief   QCMAP Utils Implementation File

  DESCRIPTION
  Implementation file for QCMAP Utility functions.

  ---------------------------------------------------------------------------
   Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        --------------------------------------------------------
10/15/14 vm Initial module.

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include <linux/kernel.h>
#include <linux/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <linux/filter.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <fcntl.h>
#include "qcmap_utils.h"

/* Mask needed to ensure To ensure pid is 31 bits */
#define QCMAP_NL_PID_MASK                   (0x7FFFFFFF)
#define QCMAP_NL_PID                        (getpid() & QCMAP_NL_PID_MASK)
/*===========================================================================
                              VARIABLE DEFINITIONS
===========================================================================*/
#define QCMAP_NLA_PARAM_NONE        (0x0000)
#define QCMAP_NDA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#define QCMAP_IFA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ifaddrmsg))))
#define QCMAP_RTN_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct rtmsg))))
#define LOG_MSG_LEN           200

#define NLMSG_SEQ 1

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION QCMAP_NL_OPEN_SOCKET()

DESCRIPTION

  This function
  - opens netlink sockets
  - binds the socket to listen to the required netlink events

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
 int qcmap_nl_open_socket
(
  qcmap_sk_info_t   *sk_info,
  int               protocol,
  unsigned int      grps
)
{
  int                  *p_sk_fd;
  struct sockaddr_nl   *p_sk_addr_loc ;

  ds_assert(sk_info != NULL);

  p_sk_fd = &(sk_info->sk_fd);
  p_sk_addr_loc = &(sk_info->sk_addr_loc);

/*--------------------------------------------------------------------------
  Open netlink socket for specified protocol
---------------------------------------------------------------------------*/
  if ((*p_sk_fd = socket(AF_NETLINK, SOCK_RAW, protocol)) < 0)
  {
    LOG_MSG_ERROR("Cannot open netlink socket errno: %d", errno,0,0);
    return QCMAP_UTIL_FAILURE;
  }

/*--------------------------------------------------------------------------
  Initialize socket parameters to 0
--------------------------------------------------------------------------*/
  memset(p_sk_addr_loc, 0, sizeof(struct sockaddr_nl));

/*-------------------------------------------------------------------------
  Populate socket parameters
--------------------------------------------------------------------------*/
  p_sk_addr_loc->nl_family = AF_NETLINK;
  p_sk_addr_loc->nl_pid = 0;
  p_sk_addr_loc->nl_groups = grps;

/*-------------------------------------------------------------------------
  Bind socket to receive the netlink events for the required groups
--------------------------------------------------------------------------*/

  if( bind( *p_sk_fd,
            (struct sockaddr *)p_sk_addr_loc,
            sizeof(struct sockaddr_nl) ) < 0)
  {

    ds_log_med("Socket bind failed %s- Make sure no-one has opened a NL socket"
               " with\n", strerror(errno));
    close(*p_sk_fd);
    return QCMAP_UTIL_FAILURE;
  }
  return QCMAP_UTIL_SUCCESS;
}

/*===========================================================================

FUNCTION QCMAP_NL_ALLOC_MSG()

DESCRIPTION

  This function
  - allocates memory to receive the netlink message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

============================================================================*/
struct msghdr * qcmap_nl_alloc_msg
(
  uint32_t         msglen
)
{
  unsigned char          * buf = NULL;
  struct sockaddr_nl     * nladdr = NULL;
  struct iovec           * iov = NULL;
  struct msghdr          * msgh = NULL;

/*-------------------------------------------------------------------------*/

  if(QCMAP_NL_MSG_MAX_LEN < msglen)
  {
    LOG_MSG_ERROR("Netlink message exceeds maximum length", 0, 0, 0);
    return NULL;
  }

  if((msgh = malloc(sizeof(struct msghdr))) == NULL)
  {
    LOG_MSG_ERROR("Failed malloc for msghdr", 0, 0, 0);
    return NULL;
  }

  if((nladdr = malloc(sizeof(struct sockaddr_nl))) == NULL)
  {
    LOG_MSG_ERROR("Failed malloc for sockaddr", 0, 0, 0);
    free(msgh);
    return NULL;
  }

  if((iov = malloc(sizeof(struct iovec))) == NULL)
  {
    LOG_MSG_ERROR("Failed malloc for iovec", 0, 0, 0);
    free(nladdr);
    free(msgh);
    return NULL;
  }

  if((buf = malloc(msglen))== NULL)
  {
    LOG_MSG_ERROR("Failed malloc for buffer to store netlink message",
                  0, 0, 0);
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

/*===========================================================================

FUNCTION QCMAP_NL_ADDFD_MAP()

DESCRIPTION

  This function
  - maps the socket descriptor with the corresponding callback function
  - add the socket descriptor to the set of socket desc the listener thread
    listens on.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_nl_addfd_map
(
  qcmap_sk_fd_set_info_t      *fd_set_info,
  int                         fd,
  qcmap_sock_thrd_fd_read_f   read_f,
  int                         max_fds
)
{
  ds_assert(fd_set_info != NULL);

  if( fd_set_info->num_fd < max_fds )
  {
/* -----------------------------------------------------------------------
  Add the fd to the fd set which the listener thread should listen on
------------------------------------------------------------------------ */
    FD_SET(fd, &(fd_set_info->fdset));

/* -----------------------------------------------------------------------
  Associate fd with the corresponding read function
------------------------------------------------------------------------ */
    fd_set_info->sk_fds[fd_set_info->num_fd].sk_fd = fd;
    fd_set_info->sk_fds[fd_set_info->num_fd].read_func = read_f;
    fd_set_info->num_fd++;

/*-----------------------------------------------------------------------
  Increment the max socket desc number which the listener should listen
  if required
------------------------------------------------------------------------*/
    if(fd_set_info->max_fd < fd)
    {
      fd_set_info->max_fd = fd;
    }
  }
  else
  {
    LOG_MSG_ERROR("Exceeds maximum num of FD",0,0,0);
    return QCMAP_UTIL_FAILURE;
  }

  return QCMAP_UTIL_SUCCESS;
}

/*===========================================================================
FUNCTION qcmap_nl_query_if()

DESCRIPTION

  This function
  - sends RTM message to kernel

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_nl_query_if
(
   qcmap_sk_info_t *sk_info,
   unsigned int    nl_groups
)
{
  struct msghdr        *nl_msg_hdr = NULL;
  qcmap_nl_req_type    *nl_req = NULL;
  ssize_t              sndbytes=0;
  char                 logmsg[LOG_MSG_LEN];
/*------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
  Send RTM_GETNEIGH to find if RNDIS/ECM interfaces are running
-------------------------------------------------------------------------*/
  nl_msg_hdr = qcmap_nl_alloc_msg( QCMAP_NL_MSG_MAX_LEN );
  if(nl_msg_hdr == NULL)
  {
    LOG_MSG_ERROR("qcmap_nl_query_if(): Failed in qcmap_nl_alloc_msg",0, 0, 0);
    return QCMAP_UTIL_FAILURE;
  }
  nl_req = (qcmap_nl_req_type *)(nl_msg_hdr->msg_iov->iov_base);

  /*-------------------------------------------------------------------------
    Populate the required parameters in the netlink message
  ---------------------------------------------------------------------------*/
  nl_req->hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
  if ( nl_groups & RTMGRP_NEIGH )
  {
    nl_req->hdr.nlmsg_type = RTM_GETNEIGH;
  }
  if ( nl_groups & RTMGRP_IPV6_ROUTE )
  {
    nl_req->hdr.nlmsg_type |= RTM_GETROUTE;
  }
  if ( nl_groups & RTMGRP_LINK )
  {
    nl_req->hdr.nlmsg_type |= RTM_GETLINK;
  }
  /* NLM_F_REQUEST - has to be set for request messages
     NLM_F_DUMP -  equivalent to NLM_F_ROOT|NLM_F_MATCH */
  nl_req->hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  nl_req->hdr.nlmsg_seq = NLMSG_SEQ;
  nl_req->hdr.nlmsg_pid = sk_info->sk_addr_loc.nl_pid;
  nl_req->gen.rtgen_family = AF_PACKET;

  if (sendmsg(sk_info->sk_fd, (struct msghdr *)nl_msg_hdr, 0) <= 0)
  {
     LOG_MSG_ERROR("IOCTL Send Failed errno:%d",errno,0,0);
     snprintf((char *)logmsg,LOG_MSG_LEN,"echo USB Qcmap NL IOCTL"
              " Snd GETNEIGH Failed %d > /dev/kmsg",errno);
     ds_system_call((char *)logmsg,strlen(logmsg));
     return QCMAP_UTIL_FAILURE;
  }

  snprintf((char *)logmsg,LOG_MSG_LEN,"echo USB QCMAP NL IOCTL"
           " Snd GETNEIGH Succ> /dev/kmsg");
  ds_system_call((char *)logmsg,strlen(logmsg));

  free(nl_msg_hdr->msg_iov->iov_base);
  free(nl_msg_hdr->msg_iov);
  free(nl_msg_hdr->msg_name);
  free(nl_msg_hdr);

  return QCMAP_UTIL_SUCCESS;
}


/*===========================================================================
FUNCTION QCMAP_NL_LISTENER_INIT()

DESCRIPTION

  This function initializes netlink sockets and also performs a query to find
  any netlink events that could happened before netlink socket
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None
=========================================================================*/
int qcmap_nl_listener_init
(
  unsigned int              nl_type,
  unsigned int              nl_groups,
  qcmap_sk_fd_set_info_t    *sk_fdset,
  qcmap_sock_thrd_fd_read_f read_f,
  qcmap_sk_info_t           *sk_info,
  int                       max_fds
)
{
  int                    ret_val;

  memset(sk_info, 0, sizeof(qcmap_sk_info_t));

  /*---------------------------------------------------------------------------
    Open netlink sockets
  ----------------------------------------------------------------------------*/

  if( qcmap_nl_open_socket( sk_info, nl_type, nl_groups ) != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("Open: QCMAP Netlink socket failed",0,0,0);
    return QCMAP_UTIL_FAILURE;
  }

  /*--------------------------------------------------------------------------
    Add the NETLINK socket to the list of sockets that the listener
    thread should listen on.
  ---------------------------------------------------------------------------*/
  if( qcmap_nl_addfd_map(sk_fdset,sk_info->sk_fd,read_f, max_fds) !=
                                                            QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("cannot add Netlink routing sock for reading",0,0,0);
    close(sk_info->sk_fd);
    return QCMAP_UTIL_FAILURE;
  }

  /* -------------------------------------------------------------------------
     Query the kernel about the current Neighbors by sending RTM_GETNEIGH.
     This is useful for the interfaces if the USB is plugged in and then powered up
     Its Just a safe guard, since DHCP addresses wouldnt be assigned until DHCP server is bought up
  --------------------------------------------------------------------------*/
  ret_val = qcmap_nl_query_if(sk_info, nl_groups);
  if(ret_val != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("Failed sending RTM_GETNEIGH to kernel",0,0,0);
  }
  return QCMAP_UTIL_SUCCESS;
}

/*===========================================================================

FUNCTION QCMAP_NL_RELEASE_MSG()

DESCRIPTION

  This function
  - releases memory which was alloacted to receive the netlink message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

============================================================================*/
void qcmap_nl_release_msg
(
  struct msghdr * msgh
)
{
  unsigned char * buf = NULL;
  struct sockaddr_nl * nladdr = NULL;
  struct iovec * iov = NULL;

  if( NULL == msgh )
  {
    return;
  }

  nladdr = msgh->msg_name;
  iov = msgh->msg_iov;
  if( msgh->msg_iov )
  {
    buf = msgh->msg_iov->iov_base;
  }

  free(buf);
  free(iov);
  free(nladdr);
  free(msgh);
  return;
}

/*===========================================================================

FUNCTION QCMAP_NL_RECV()

DESCRIPTION

  This function
  - retrieves the netlink message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_nl_recv
(
  int              fd,
  struct msghdr    **msg_pptr,
  unsigned int     *msglen_ptr
)
{
  struct msghdr    *msgh = NULL;
  int              rmsgl;

  /*--------------------------------------------------------------------------
    Allocate the memory to receive the netlink message
  ---------------------------------------------------------------------------*/
  if( NULL == (msgh = qcmap_nl_alloc_msg( QCMAP_NL_MSG_MAX_LEN )) )
  {
    LOG_MSG_ERROR("Failed to allocate NL message",0,0,0);
    goto error;
  }

  /*--------------------------------------------------------------------------
    Receive message over the socket
  ---------------------------------------------------------------------------*/
  rmsgl = recvmsg(fd, msgh, 0);

  /*--------------------------------------------------------------------------
    Verify that something was read
  ---------------------------------------------------------------------------*/
  if (rmsgl <= 0)
  {
    LOG_MSG_ERROR("Received nl_msg, recvmsg failed:",0,0,0);
    perror("NL recv error");
    goto error;
  }

  /*--------------------------------------------------------------------------
    Verify that NL address length in the received message is expected value
  ---------------------------------------------------------------------------*/
  if (msgh->msg_namelen != sizeof(struct sockaddr_nl))
  {
    LOG_MSG_ERROR("rcvd msg with namelen != sizeof sockaddr_nl",0,0,0);
    goto error;
  }

  /*--------------------------------------------------------------------------
    Verify that message was not truncated. This should not occur
  ---------------------------------------------------------------------------*/
  if (msgh->msg_flags & MSG_TRUNC)
  {
    LOG_MSG_ERROR("Rcvd msg truncated!",0,0,0);
    goto error;
  }

  *msg_pptr    = msgh;
  *msglen_ptr = rmsgl;

  return QCMAP_UTIL_SUCCESS;

error:
  /*--------------------------------------------------------------------------
    An error occurred while receiving the message. Free all memory before
    returning.
  ---------------------------------------------------------------------------*/

  qcmap_nl_release_msg( msgh );
  *msg_pptr    = NULL;
  *msglen_ptr  = 0;
  return QCMAP_UTIL_FAILURE;
}
/*===========================================================================

FUNCTION QCMAP_QTI_NL_DECODE_RTM_LINK()

DESCRIPTION

  This function
  - decodes the RTM link type netlink message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=============================================================================*/
 int qcmap_nl_decode_rtm_link
(
  const char              *buffer,
  unsigned int            buflen,
  qcmap_nl_link_info_t    *link_info
)
{
  struct nlmsghdr * nlh = (struct nlmsghdr*)buffer;    /* NL message header */
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Extract the header data
-----------------------------------------------------------------------------*/
  link_info->metainfo = *(struct ifinfomsg*)NLMSG_DATA(nlh);
  LOG_MSG_INFO3("metainfo:  index = %d, family = %d, type = %d",
                link_info->metainfo.ifi_index,
                link_info->metainfo.ifi_family,
                link_info->metainfo.ifi_type);
  LOG_MSG_INFO3("metainfo: link up/down = %d",
                link_info->metainfo.ifi_change,0,0);

  return QCMAP_UTIL_SUCCESS;
}
/*===========================================================================

FUNCTION QCMAP_NL_DECODE_NLMSG()

DESCRIPTION

  This function
  - decodes the netlink link message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=============================================================================*/
int qcmap_nl_decode_link_nlmsg
(
  const char     *buffer,
  unsigned int   buflen,
  qcmap_nl_msg_t *msg_ptr,
  int            nlmsg_type
)
{

/*-------------------------------------------------------------------------
  Decode the message type
-------------------------------------------------------------------------*/
  switch(nlmsg_type)
  {
    case RTM_NEWLINK:
    case RTM_DELLINK:
      msg_ptr->type = nlmsg_type;
      msg_ptr->link_event = true;
/*-------------------------------------------------------------------------
  Decode the link message type
-------------------------------------------------------------------------*/
      if(QCMAP_UTIL_SUCCESS != qcmap_nl_decode_rtm_link(buffer,
                                                 buflen,
                                                 &(msg_ptr->nl_link_info)))
      {
        LOG_MSG_ERROR("Failed to decode rtm link message",0,0,0);
        return QCMAP_UTIL_FAILURE;
      }
    break;

    default:
      return QCMAP_UTIL_FAILURE;
    break;
  }
  return QCMAP_UTIL_SUCCESS;
}



/*===========================================================================

FUNCTION QCMAP_LISTENER_START()

DESCRIPTION

  This function
  - calls the select system call and listens data coming on
    various sockets and call the appropriate handler functions.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qcmap_listener_start
(
  qcmap_sk_fd_set_info_t    * global_sk_fd_set
)
{
  int            i,ret;
  qcmap_sk_fd_set_info_t    sk_fd_array;
  qcmap_sk_fd_set_info_t    *sk_fd_set = NULL;

/*-------------------------------------------------------------------------*/

  while(TRUE)
  {
    memset(&sk_fd_array, 0, sizeof(qcmap_sk_fd_set_info_t));
    sk_fd_array = *global_sk_fd_set;
    sk_fd_set = &sk_fd_array;
    FD_ZERO(&sk_fd_set->fdset);
    for (i = 0; i < sk_fd_set->num_fd; i++ )
    {
/*--------------------------------------------------------------------------
Check if the fd is valid before setting it in FD_SET
---------------------------------------------------------------------------*/
     if (fcntl(sk_fd_set->sk_fds[i].sk_fd, F_GETFL) < 0 && errno == EBADF)
       continue;
      FD_SET(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
    }

/*--------------------------------------------------------------------------
    Call select system function which will listen to netlink events
    coming on socket which we would have opened during
    initialization
--------------------------------------------------------------------------*/
    if ((ret = select(sk_fd_set->max_fd+1,
                     &(sk_fd_set->fdset),
                     NULL,
                     NULL,
                     NULL)) < 0)
    {
      LOG_MSG_ERROR("qcmap select failed %d", errno, 0, 0);
    }
    else
    {
      for (i = 0; i < sk_fd_set->num_fd; i++ )
      {
        if (FD_ISSET(sk_fd_set->sk_fds[i].sk_fd,
                     &(sk_fd_set->fdset)))
        {
          if(sk_fd_set->sk_fds[i].read_func)
          {
            if (QCMAP_UTIL_SUCCESS !=
               ((sk_fd_set->sk_fds[i].read_func)(sk_fd_set->sk_fds[i].sk_fd)))
            {
              LOG_MSG_ERROR("Error on read callback[%d] fd=%d",
                            i,
                            sk_fd_set->sk_fds[i].sk_fd,
                            0);
              FD_CLR(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
            }
            else
            {
              FD_CLR(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset));
            }
          }
          else
          {
            LOG_MSG_ERROR("No read function",0,0,0);
          }
        }
      }
    }
  }
  return QCMAP_UTIL_SUCCESS;
}


/*=====================================================================

FUNCTION QCMAP_NL_RECV_ON_DEV()

DESCRIPTION

  This function
  - compares the passed device names and returns accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=====================================================================*/

int qcmap_nl_is_recv_on_dev(const char *dev_type, char * dev_name)
{
  if (strncmp(dev_name, dev_type,strlen(dev_type)) == 0 )
      return QCMAP_UTIL_SUCCESS;
  return QCMAP_UTIL_FAILURE;
}

/*=====================================================================

FUNCTION QCMAP_NL_GET_INTERFACE_DETAILS()

DESCRIPTION

  This function
  - calls a system ioctl to get the interface details for the corresponding
  passed interface name

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=====================================================================*/
int qcmap_nl_get_interface_details
(
  char *   if_name,
  int *    if_index,
  int      interface_attribute
)
{
  int      fd;
  struct   ifreq ifr;

/*-------------------------------------------------------------------------
  Open a socket which is required to call an ioctl
--------------------------------------------------------------------------*/
  if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("get interface index socket create failed",0,0,0);
    return QCMAP_UTIL_FAILURE;
  }

  memset(&ifr, 0, sizeof(struct ifreq));

  if (interface_attribute == QCMAP_GET_IFINDEX)
  {
    /*-------------------------------------------------------------------------
        Populate the passed interface name
      --------------------------------------------------------------------------*/
    strlcpy((char *)ifr.ifr_name, if_name, sizeof(ifr.ifr_name));
    /*-------------------------------------------------------------------------
        Call the ioctl to get the interface index
       --------------------------------------------------------------------------*/
    if (ioctl(fd,SIOCGIFINDEX , &ifr) < 0)
    {
      LOG_MSG_ERROR("call_ioctl_on_dev; ioctl failed\
                     error:%s\n",strerror(errno),0,0);

      close(fd);
      return QCMAP_UTIL_FAILURE;
    }
    *if_index = ifr.ifr_ifindex;
  }
  else
  {
    /*-------------------------------------------------------------------------
        Populate the passed interface name
       --------------------------------------------------------------------------*/
    ifr.ifr_ifindex = *if_index;
    /*-------------------------------------------------------------------------
        Call the ioctl to get the interface index
       --------------------------------------------------------------------------*/
    if (ioctl(fd,SIOCGIFNAME , &ifr) < 0)
    {
      LOG_MSG_ERROR("call_ioctl_on_dev; ioctl failed\
                     error:%s\n",strerror(errno),0,0);

      close(fd);
      return QCMAP_UTIL_FAILURE;
    }
    (void)strlcpy(if_name, ifr.ifr_name, sizeof(ifr.ifr_name));
  }

  close(fd);
  return QCMAP_UTIL_SUCCESS;
}

#ifdef FEATURE_MDM_LE
/*=====================================================================

FUNCTION QCMAP_NL_DECODE_RTM_NEIGH()

DESCRIPTION

  This function
  - decodes the Netlink Neighbour message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=====================================================================*/
int qcmap_nl_decode_rtm_neigh
(
  const char              *buffer,
  unsigned int             buflen,
  qcmap_nl_neigh_info_t   *neigh_info
)
{
  struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;  /* NL message header */
  struct rtattr *rtah = NULL;

  /* Extract the header data */
  neigh_info->metainfo = *((struct ndmsg *)NLMSG_DATA(nlh));
  /* Subtracting NL Header to decode RT MSG*/
  buflen -= sizeof(struct nlmsghdr);

  /* Extract the available attributes */
  neigh_info->attr_info.param_mask = QCMAP_NLA_PARAM_NONE;

  rtah = QCMAP_NDA_RTA(NLMSG_DATA(nlh));

  while(RTA_OK(rtah, buflen))
  {
    switch(rtah->rta_type)
    {
      case NDA_DST:
        neigh_info->attr_info.local_addr.ss_family =
                                                neigh_info->metainfo.ndm_family;
        memcpy(&neigh_info->attr_info.local_addr.__ss_padding,
               RTA_DATA(rtah),
               sizeof(neigh_info->attr_info.local_addr.__ss_padding));
        break;

      case NDA_LLADDR:
        memcpy(neigh_info->attr_info.lladdr_hwaddr.sa_data,
               RTA_DATA(rtah),
               sizeof(neigh_info->attr_info.lladdr_hwaddr.sa_data));
        break;

      default:
        break;

    }

    /* Advance to next attribute */
    rtah = RTA_NEXT(rtah, buflen);
  }

  return QCMAP_UTIL_SUCCESS;
}
#endif //FEATURE_MDM_LE


/*=====================================================================

FUNCTION QCMAP_NL_DECODE_RTM_ADDR()

DESCRIPTION

  This function
  - decodes the Netlink Address message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=====================================================================*/

int qcmap_nl_decode_rtm_addr
(
  const char              *buffer,
  unsigned int             buflen,
  qcmap_nl_if_addr_t      *if_addr_info
)
{
  struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;  /* NL message header */
  struct rtattr *rtah = NULL;

  if (buffer == NULL)
  {
    LOG_MSG_ERROR("qcmap_nl_decode_rtm_addr : NULL buffer passed",0,0,0);
    return QCMAP_UTIL_FAILURE;
  }

  if (if_addr_info == NULL)
  {
    LOG_MSG_ERROR("qcmap_nl_decode_rtm_addr : NULL if_addr_info passed",0,0,0);
    return QCMAP_UTIL_FAILURE;
  }

  /* Extract the header data */
  if_addr_info->metainfo = *((struct ifaddrmsg *)NLMSG_DATA(nlh));

  if (if_addr_info->metainfo.ifa_family == AF_INET)
  {
    LOG_MSG_INFO1("RTM_NEWADDR received for IPv4",0,0,0);
  }
  else if (if_addr_info->metainfo.ifa_family == AF_INET6)
  {
    LOG_MSG_INFO1("Received RTM_NEWADDR for IPv6",0,0,0);
  }
  else
  {
    LOG_MSG_INFO1("Received RTM_NEWADDR for unknown family",0,0,0);
    return QCMAP_UTIL_FAILURE;
  }

  /* Subtracting NL Header to decode RT MSG*/
  buflen -= sizeof(struct nlmsghdr);
  rtah = QCMAP_IFA_RTA(NLMSG_DATA(nlh));
  /*Setting ifa_local to zero.*/
  memset(&if_addr_info->attr_info, 0, sizeof(if_addr_info->attr_info));

  while(RTA_OK(rtah, buflen))
  {
    switch(rtah->rta_type)
    {
      case IFA_LOCAL:
        if (if_addr_info->metainfo.ifa_family == AF_INET)
          if_addr_info->attr_info.ifa_local_v4 = *((struct in_addr *)RTA_DATA(rtah));
        break;
      case IFA_ADDRESS: /* For Global IPv6 Address */
        if ((if_addr_info->metainfo.ifa_family != AF_INET6) ||
            (if_addr_info->metainfo.ifa_scope != RT_SCOPE_UNIVERSE))
           break;
        else if (if_addr_info->metainfo.ifa_family == AF_INET6)
           memcpy(&if_addr_info->attr_info.ifa_local_v6,
                   RTA_DATA(rtah), sizeof(struct in6_addr));
        break;
      default:
        break;
    }
    /* Advance to next attribute */
    rtah = RTA_NEXT(rtah, buflen);
  }
  return QCMAP_UTIL_SUCCESS;
}
/*=====================================================================

FUNCTION QCMAP_NL_DECODE_RTM_ROUTE()

DESCRIPTION

  This function
  - decodes the Netlink Route message

DEPENDENCIES
  None.

RETURN VALUE
  QCMAP_UTIL_SUCCESS on success
  QCMAP_UTIL_FAILURE on failure


SIDE EFFECTS
  None

=====================================================================*/
int qcmap_nl_decode_rtm_route
(
  const char              *buffer,
  unsigned int             buflen,
  struct qcmap_nl_route_info_s    *route_info
)
{
  struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;  /* NL message header */
  struct rtattr *rtah = NULL;

  /* Extract the header data */
  route_info->metainfo = *((struct rtmsg *)NLMSG_DATA(nlh));
  /* Subtracting NL Header to decode RT MSG*/
  buflen -= sizeof(struct nlmsghdr);

  route_info->attr_info.param_mask = 0;

  if ( route_info->metainfo.rtm_protocol != RTPROT_STATIC )
  {
    /* Ignore the route silently. */
    return QCMAP_UTIL_SUCCESS;
  }

  rtah = QCMAP_RTN_RTA(NLMSG_DATA(nlh));

  while(RTA_OK(rtah, buflen))
  {
    switch(rtah->rta_type)
    {
      case RTA_DST:
        memcpy(&route_info->attr_info.dst_addr,
               RTA_DATA(rtah),sizeof(route_info->attr_info.dst_addr));
        route_info->attr_info.param_mask |= QCMAP_NL_ROUTE_INFO_DST_ADDR;
        break;
      case RTA_OIF:
        memcpy(&route_info->attr_info.ifindex,
               RTA_DATA(rtah),sizeof(route_info->attr_info.ifindex));
        route_info->attr_info.param_mask |= QCMAP_NL_ROUTE_INFO_IFINDEX;
        break;
      default:
        break;

    }

    /* Advance to next attribute */
    rtah = RTA_NEXT(rtah, buflen);
  }

  return QCMAP_UTIL_SUCCESS;
}
