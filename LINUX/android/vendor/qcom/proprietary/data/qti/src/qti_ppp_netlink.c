/******************************************************************************

                        QTI_PPP_NETLINK.C

******************************************************************************/

/******************************************************************************

  @file    qti_ppp_netlink.c
  @brief   Qualcomm Tethering Interface PPP Netlink Messaging Implementation File

  DESCRIPTION
  Implementation file for QTI Netlink messaging functions related to PPP interface.

  ---------------------------------------------------------------------------
   Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        --------------------------------------------------------
02/19/14   cp         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include "qti_ppp.h"

/*===========================================================================

FUNCTION QTI_PPP_NL_RECV_MSG()

DESCRIPTION

  This function
  - receives the netlink message associated with the netlink event which
  was received on select call
  - decodes the netlink message to find if the netlink message was found
  on the desired interface and if it was a link up or link down event
  - sends a command to command queue to process the link up or link down
    event

DEPENDENCIES
  None.

RETURN VALUE
  QTI_PPP_SUCCESS on success
  QTI_PPP_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qti_ppp_nl_recv_msg(int fd)
{
  struct msghdr                    * msghdr = NULL;
  struct sockaddr_nl               * nladdr = NULL;
  struct iovec                     * iov = NULL;
  unsigned int                     msglen = 0;
  qcmap_nl_msg_t                   * nlmsg = NULL;
  int                              interface_num,interface;
  int                              ret_val = QTI_PPP_SUCCESS;
  struct nlmsghdr                  *nlh = NULL;
/*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
  Allocate memory to decode the netlink message
---------------------------------------------------------------------------*/
  nlmsg = malloc(sizeof(qcmap_nl_msg_t));
  if(NULL == nlmsg)
  {
    LOG_MSG_ERROR("Failed alloc of nlmsg",0,0,0);
    goto error;
  }
  else
  {
    memset(nlmsg, 0, sizeof(qcmap_nl_msg_t));
/*--------------------------------------------------------------------------
  Receive the netlink message
---------------------------------------------------------------------------*/
    if(QCMAP_UTIL_SUCCESS != qcmap_nl_recv(fd, &msghdr, &msglen))
    {
      LOG_MSG_ERROR("Failed to receive nl message",0,0,0);
      goto error;
    }

    nladdr = msghdr->msg_name;
    iov = msghdr->msg_iov;

/*--------------------------------------------------------------------------
  Decode the received netlink message
---------------------------------------------------------------------------*/
    nlh = (struct nlmsghdr *)iov->iov_base;

    while(NLMSG_OK(nlh, msglen))
    {
      if(QCMAP_UTIL_SUCCESS != qcmap_nl_decode_link_nlmsg((char*)iov->iov_base,
                                                          msglen,
                                                          nlmsg,
                                                          nlh->nlmsg_type))
      {
        LOG_MSG_ERROR("Failed to decode nl message",0,0,0);
        /* Move to the NEXT Message */
        nlh = NLMSG_NEXT(nlh, msglen);
        continue;
      }

      LOG_MSG_INFO1("Netlink event got on unknow interface, check if PPP",0,0,0);
      if ((nlmsg->type == RTM_DELLINK) &&
          (nlmsg->nl_link_info.metainfo.ifi_flags & IFF_POINTOPOINT) &&
          !(nlmsg->nl_link_info.metainfo.ifi_flags & IFF_UP))
      {
        LOG_MSG_INFO1("Netlink event got on PPP interface, IF Down",0,0,0);

        if ( usb_tty_config_info.is_ppp_active )
        {
/*--------------------------------------------------------------------------
   Bring down PPP tethering
 ---------------------------------------------------------------------------*/
        qti_ppp_tethered_link_down();

/*--------------------------------------------------------------------------
   Disable QC Mobile AP
---------------------------------------------------------------------------*/
          qti_ppp_disable_mobile_ap();
        }

        /* Update the PPP active flag. */
        usb_tty_config_info.is_ppp_active = 0;

/*---------------------------------------------------------------------
   Call into the USB TTY listener init function which sets up QTI to
   listen to AT Commands coming in from the USB device file for DUN
---------------------------------------------------------------------*/
        ret_val = qti_usb_tty_listener_init(&usb_tty_config_info,
                                            &sk_fdset,
                                            qti_usb_tty_recv_msg);
        if (ret_val != QTI_PPP_SUCCESS)
        {
          LOG_MSG_ERROR("Failed to initialize QTI USB TTY listener",0,0,0);
        }
      }
      nlh = NLMSG_NEXT(nlh, msglen);
    }
    qcmap_nl_release_msg(msghdr);
    free(nlmsg);
    msghdr = NULL;
    nlmsg = NULL;
    return QCMAP_UTIL_SUCCESS;
  }
error:
  if(msghdr)
  {
    qcmap_nl_release_msg(msghdr);
    msghdr = NULL;
  }
  if(nlmsg)
  {
    free(nlmsg);
    nlmsg = NULL;
  }

  return QCMAP_UTIL_FAILURE;
}

