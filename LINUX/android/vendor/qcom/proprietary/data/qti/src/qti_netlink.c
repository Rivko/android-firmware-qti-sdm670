/******************************************************************************

                        QTI_NETLINK.C

******************************************************************************/

/******************************************************************************

  @file    qti_netlink.c
  @brief   Tethering Interface module Netlink Messaging Implementation File

  DESCRIPTION
  Implementation file for Tethering Interface module Netlink messaging functions.

  ---------------------------------------------------------------------------
   Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        --------------------------------------------------------
04/04/13   sb         Add support for dynamic switching between USB PIDs
11/15/12   sb         Initial version

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

#include "qti.h"
#include "qti_cmdq.h"

/*===========================================================================
                              VARIABLE DEFINITIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   QTI configuration file
---------------------------------------------------------------------------*/
static qti_conf_t * qti_netlink_conf;

extern qcmap_sk_info_t     sk_info;


/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION QTI_NETLINK_INIT()

DESCRIPTION

  This function initializes QTI:
  - obtains the interface index and name mapping
  - initializes the QTI configuration varaible

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/

int qti_netlink_init(qti_conf_t * qti_conf)
{
  int    i;
  int    ret_val;

/*------------------------------------------------------------------------*/
  //ds_assert(qti_conf != NULL);

  LOG_MSG_INFO1("qti_netlink_init()", 0, 0, 0);

/*-----------------------------------------------------------------------------
  Static pointer to QTI configuration variable
------------------------------------------------------------------------------*/
  qti_netlink_conf = qti_conf;

/*-------------------------------------------------------------------------
   Initialize QTI netlink state
--------------------------------------------------------------------------*/
  qti_netlink_conf->state = QTI_INIT;

/*-------------------------------------------------------------------------
   Initialize interface names
--------------------------------------------------------------------------*/
  strlcpy(qti_netlink_conf->if_dev[0].dev_name,
              RNDIS_INTERFACE,
              IF_NAME_LEN);
  strlcpy(qti_netlink_conf->if_dev[1].dev_name,
              ECM_INTERFACE,
              IF_NAME_LEN);
  strlcpy(qti_netlink_conf->if_dev[2].dev_name,
              ETH_INTERFACE,
              IF_NAME_LEN);

/*-------------------------------------------------------------------------
   Initialize interface indices to default and then obtain the right
   interface indices.
--------------------------------------------------------------------------*/
  for(i=0; i < QTI_INTERFACES; i++)
  {
    qti_netlink_conf->if_dev[i].if_index = QTI_DEFAULT_INTERFACE_ID;
    qti_netlink_conf->if_dev[i].prev_if_index = QTI_DEFAULT_INTERFACE_ID;

    ret_val = qcmap_nl_get_interface_details(
                                       qti_netlink_conf->if_dev[i].dev_name,
                                       &(qti_netlink_conf->if_dev[i].if_index),
                                       QCMAP_GET_IFNAME);
    if(ret_val != QCMAP_UTIL_SUCCESS)
    {
      LOG_MSG_ERROR("Error while getting interface index for %d device", i, 0, 0);
    }

    LOG_MSG_INFO1("Device names and index: %s = %d",
                  qti_netlink_conf->if_dev[i].dev_name,
                  qti_netlink_conf->if_dev[i].if_index, 0);
  }

  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_NL_QUERY_IF()

DESCRIPTION

  This function
  - brings up RNDIS and ECM interfaces to listen to netlink events
    coming up on those interfaces
  - sends RTM_GETLINK to kernel to check if RNDIS/ECM interfaces
    are running

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qti_nl_query_if()
{
  char                 command[QTI_MAX_COMMAND_STR_LEN];

/*------------------------------------------------------------------------*/
  if(sk_info.sk_fd < 0)
  {
    LOG_MSG_ERROR("qti_nl_query_if(): Invalid netlink fd",
                  0, 0, 0);
    return QTI_FAILURE;
  }

/*-------------------------------------------------------------------------
  Bring up rndis0, ecm0, and eth iface interfaces to listen to netlink events on them.
  This was moved from QTI initializaton to here to allow USB to complete
  rndis0, ecm0, eth iface enumeration.
--------------------------------------------------------------------------*/
  snprintf( command, QTI_MAX_COMMAND_STR_LEN,
            "ifconfig %s up", RNDIS_INTERFACE);
  ds_system_call( command, strlen(command));
  LOG_MSG_INFO1("ifconfig rndis0 up", 0, 0, 0);

  snprintf( command, QTI_MAX_COMMAND_STR_LEN,
                    "ifconfig %s up", ECM_INTERFACE);
  ds_system_call( command, strlen(command));
  LOG_MSG_INFO1("ifconfig ecm0 up", 0, 0, 0);

  snprintf( command, QTI_MAX_COMMAND_STR_LEN,
            "ifconfig %s down", ETH_INTERFACE);
  ds_system_call( command, strlen(command));
  snprintf( command, QTI_MAX_COMMAND_STR_LEN,
            "ifconfig %s up", ETH_INTERFACE);
  ds_system_call( command, strlen(command));
  LOG_MSG_INFO1("ifconfig eth iface up", 0, 0, 0);

  if( qcmap_nl_query_if(&sk_info, RTMGRP_LINK) == QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_INFO1("Netlink Query succeeds",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Netlink Query failed",0,0,0);
    return QTI_FAILURE;
  }

  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_NL_DECODE_RTM_LINK()

DESCRIPTION

  This function
  - Iterate over the array of interfaces, and return the link id for
    the interface matching the specified if index.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=============================================================================*/
static int qti_get_link_for_ifindex (int ifindex)
{
  int link = -1;
  int i;

  for( i = 0; i < QTI_INTERFACES; i++ )
  {
    if( qti_netlink_conf->if_dev[i].if_index == ifindex )
    {
      link = i;
      break;
    }
  }

  return link;
}

/*===========================================================================

FUNCTION QTI_GET_LINK_FOR_PREVIFINDEX

DESCRIPTION

  This function
  - Iterate over the array of interfaces, and return the link id for
    the interface matching the specified  previous if index.

DEPENDENCIES
  None.

RETURN VALUE
  valid interface number on success
  -1 on failure


SIDE EFFECTS
  None

=============================================================================*/
static int qti_get_link_for_previfindex (int ifindex)
{
  int link = -1;
  int i;

  for( i = 0; i < QTI_INTERFACES; i++ )
  {
    if( qti_netlink_conf->if_dev[i].prev_if_index == ifindex )
    {
      link = i;
      break;
    }
  }

  return link;
}

/*===========================================================================
FUNCTION QTI_GET_ALL_IF_INDEX()

DESCRIPTION

  This function
  - Gets the indices of the tethering interfaces.

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None

=========================================================================*/
static void qti_get_all_if_index()
{
  int        i;
  int        ret_val;
  char       command[100];

/*----------------------------------------------------------------------------*/
  strlcpy(qti_netlink_conf->if_dev[0].dev_name, RNDIS_INTERFACE, IF_NAME_LEN);
  strlcpy(qti_netlink_conf->if_dev[1].dev_name, ECM_INTERFACE, IF_NAME_LEN);
  strlcpy(qti_netlink_conf->if_dev[2].dev_name, ETH_INTERFACE, IF_NAME_LEN);
  LOG_MSG_INFO1("Assigned device names %s,%s",
                qti_netlink_conf->if_dev[0].dev_name,
                qti_netlink_conf->if_dev[1].dev_name,
                qti_netlink_conf->if_dev[2].dev_name);

  for(i=0; i < QTI_INTERFACES; i++)
  {
    if ( (qti_netlink_conf->if_dev[i].if_index != QTI_DEFAULT_INTERFACE_ID) &&
         (qti_netlink_conf->if_dev[i].prev_if_index != qti_netlink_conf->if_dev[i].if_index))
    {
      LOG_MSG_ERROR("Storing the interface index for %d device with interface"
                    " index %d", i, qti_netlink_conf->if_dev[i].if_index, 0);
      qti_netlink_conf->if_dev[i].prev_if_index = qti_netlink_conf->if_dev[i].if_index;
    }
/*----------------------------------------------------------------------------
  Initialize interface index to default
----------------------------------------------------------------------------*/
    qti_netlink_conf->if_dev[i].if_index = QTI_DEFAULT_INTERFACE_ID;

    ret_val = qcmap_nl_get_interface_details(
                                       qti_netlink_conf->if_dev[i].dev_name,
                                       &(qti_netlink_conf->if_dev[i].if_index),
                                       QCMAP_GET_IFINDEX);
    if(ret_val!=QCMAP_UTIL_SUCCESS)
    {
      LOG_MSG_ERROR("Error while getting interface index for %d device",
                     i, 0, 0);
    }
  }
}


/*===========================================================================

FUNCTION RETRY_LINK_BRINGUP()

DESCRIPTION

  This function
  - enables QTI to support dynamic composition switch without rebooting
  - if we get netlink events on RNDIS/ECM interfaces and they are not
  enabled this function enables them and sends getlink message to
  re-trigger netlink events on those interfaces.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
void retry_link_bringup(int fd, int interface)
{
  char command[QTI_MAX_COMMAND_STR_LEN];
/*------------------------------------------------------------------------*/

  LOG_MSG_INFO1("Re-enable RNDIS/ECM/ETH interfaces", 0, 0, 0);
  if(interface == 0)
  {
    snprintf( command, QTI_MAX_COMMAND_STR_LEN, "ifconfig %s up", RNDIS_INTERFACE);
    ds_system_call( command, strlen(command));
  }
  else if(interface == 1)
  {
    snprintf( command, QTI_MAX_COMMAND_STR_LEN, "ifconfig %s up", ECM_INTERFACE);
    ds_system_call( command, strlen(command));
  }
  else if(interface == 2)
  {
    snprintf( command, QTI_MAX_COMMAND_STR_LEN, "ifconfig %s up", ETH_INTERFACE);
    ds_system_call( command, strlen(command));
  }

  if( qcmap_nl_query_if(&sk_info, RTMGRP_LINK) == QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_INFO1("Open netlink socket succeeds",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Netlink socket open failed",0,0,0);
    return QTI_FAILURE;
  }
}
/*===========================================================================

FUNCTION QTI_NL_RECV_MSG()

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
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qti_nl_recv_msg(int fd)
{
  struct msghdr                    * msghdr = NULL;
  struct sockaddr_nl               * nladdr = NULL;
  struct iovec                     * iov = NULL;
  unsigned int                     msglen = 0;
  qcmap_nl_msg_t                     * nlmsg = NULL;
  qti_cmdq_cmd_t                   * cmd_buf = NULL;
  int                              interface_num,interface;
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
    memset(nlmsg, 0, sizeof(qcmap_nl_msg_t));

    nlh = (struct nlmsghdr *)iov->iov_base;

    while(NLMSG_OK(nlh, msglen))
    {

      if(QCMAP_UTIL_SUCCESS != qcmap_nl_decode_link_nlmsg((char*)iov->iov_base,
                                                          msglen,
                                                          nlmsg,
                                                          nlh->nlmsg_type))
      {
        /* Move to the NEXT Message */
        nlh = NLMSG_NEXT(nlh, msglen);
        continue;
      }
/*--------------------------------------------------------------------------
  Update indices. They can change if the enumeration has happened again.
--------------------------------------------------------------------------*/
      qti_get_all_if_index();

/*--------------------------------------------------------------------------
  Get the interface on which we got the netlink event
---------------------------------------------------------------------------*/
      interface_num = qti_get_link_for_ifindex(nlmsg->nl_link_info.metainfo.ifi_index);

/*-------------------------------------------------------------------------------
  In case network interface index is not found, try to fetch using previous index
--------------------------------------------------------------------------------*/

    if ( interface_num == -1)
    {
      interface_num = qti_get_link_for_previfindex(nlmsg->nl_link_info.metainfo.ifi_index);
      if ( interface_num != -1)
      {
        qti_netlink_conf->if_dev[interface_num].prev_if_index = QTI_DEFAULT_INTERFACE_ID;
      }
    }

/*--------------------------------------------------------------------------
  We got the netlink event on RNDIS interface
---------------------------------------------------------------------------*/
      if(interface_num == 0)
      {
        interface = RNDIS_IF;
        LOG_MSG_INFO1("Netlink event got on RNDIS interface",0,0,0);

/*-------------------------------------------------------------------------
  Ignore netlink event on RNDIS since ECM is enabled
-------------------------------------------------------------------------*/
      if(qti_netlink_conf->if_dev[1].enabled)
        {
        LOG_MSG_INFO1("Ignoring Netlink event on RNDIS since ECM is enabled",
                        0, 0, 0);
          qcmap_nl_release_msg(msghdr);
          free(nlmsg);
          return QCMAP_UTIL_SUCCESS;
        }
      }
/*--------------------------------------------------------------------------
 We got the netlink event on ECM interface
---------------------------------------------------------------------------*/
      else if(interface_num ==1)
      {
        interface= ECM_IF;
        LOG_MSG_INFO1("Netlink event got on ECM interface",0,0,0);
/*-------------------------------------------------------------------------
  Ignore netlink event on ECM since RNDIS is enabled
-------------------------------------------------------------------------*/
      if(qti_netlink_conf->if_dev[0].enabled)
        {
        LOG_MSG_INFO1("Ignoring Netlink event on ECM since RNDIS is enabled",
                         0, 0, 0);
          qcmap_nl_release_msg(msghdr);
          free(nlmsg);
          return QCMAP_UTIL_SUCCESS;
        }
      }
/*--------------------------------------------------------------------------
 We got the netlink event on ETH interface
---------------------------------------------------------------------------*/
      else if(interface_num ==2)
      {
        interface= ETH_IF;
        LOG_MSG_INFO1("Netlink event got on ETH interface",0,0,0);
      }
      else
      {
        qcmap_nl_release_msg(msghdr);
        free(nlmsg);
        return QCMAP_UTIL_SUCCESS;
      }
/*--------------------------------------------------------------------------
  QTI was checking for interface RUNNING flag to determine if the interface is
  up or not. The latest version of kernel does not set RUNNING flag when the
  interface is up and only sets UP flag when the interface is up.
  This change checks for UP flag and avoid doing GETLINK on that interface
  repeatedly, there by avoides the CPU utilization issue.
---------------------------------------------------------------------------*/
      if(nlmsg->type == RTM_NEWLINK &&
                             !(nlmsg->nl_link_info.metainfo.ifi_flags & IFF_UP))
      {
        retry_link_bringup(fd,interface_num);
      }
/*--------------------------------------------------------------------------
  Check if its a RTM link event
---------------------------------------------------------------------------*/
      if(nlmsg->link_event)
      {
        LOG_MSG_INFO1("flag: nl_link_info.metainfo.ifi_change %d IFF_UP %d",
                      nlmsg->nl_link_info.metainfo.ifi_change,
                      IFF_UP,
                      0);
/*--------------------------------------------------------------------------
  Check if the interface is running.If its a RTM_NEWLINK and the interface
  is running then it means that its a link up event
---------------------------------------------------------------------------*/
        if((nlmsg->nl_link_info.metainfo.ifi_flags & IFF_UP) &&
           (nlmsg->nl_link_info.metainfo.ifi_flags & IFF_LOWER_UP) &&
           (nlmsg->type == RTM_NEWLINK))
        {
          LOG_MSG_INFO1("Got a new link event",0,0,0);

          if(qti_netlink_conf->if_dev[interface_num].enabled)
          {
            LOG_MSG_ERROR("Interface already enabled. Ignorning LINK_UP",
                          0, 0, 0);
          }
          else
          {
/*--------------------------------------------------------------------------
  Post a link up command
---------------------------------------------------------------------------*/
            cmd_buf = qti_cmdq_get_cmd();
            if(cmd_buf == NULL)
            {
              LOG_MSG_ERROR("qti_cmdq: failed to allocate memeory for cmd",
                            0, 0, 0);
              goto error;
            }
            cmd_buf->data.event = QTI_LINK_UP_EVENT;
            cmd_buf->data.interface = interface;
            if( QTI_CMDQ_SUCCESS != qti_cmdq_put_cmd( cmd_buf ) )
            {
              qti_cmdq_release_cmd(cmd_buf);
              LOG_MSG_ERROR("qti_cmdq: failed to put commmand",0,0,0);
              goto error;
            }

            qti_netlink_conf->if_dev[interface_num].enabled = TRUE;
            LOG_MSG_INFO1("Sucessfully put cmd into command buffer",0,0,0);

/*--------------------------------------------------------------------------
 Required for KPI logs
---------------------------------------------------------------------------*/
            if(interface == ECM_IF)
            {
              ds_system_call("echo QTI:ECM mode > /dev/kmsg",
                             strlen("echo QTI:ECM mode > /dev/kmsg"));
            }
            else if (interface == RNDIS_IF)
            {
              ds_system_call("echo QTI:RNDIS mode > /dev/kmsg",
                             strlen("echo QTI:RNDIS mode > /dev/kmsg"));
            }
          else if (interface == ETH_IF)
          {
            ds_system_call("echo QTI:ETH mode > /dev/kmsg",
                           strlen("echo QTI:ETH mode > /dev/kmsg"));
          }
/*--------------------------------------------------------------------------
  Write to dmesg log. It will help in debugging customer issues quickly.
  But we need to make sure we dont write too many messages to dmesg.
---------------------------------------------------------------------------*/
            ds_system_call("echo QTI:LINK_UP message posted > /dev/kmsg",
                           strlen("echo QTI:LINK_UP message posted > /dev/kmsg"));
          }
        }
/*--------------------------------------------------------------------------
  If the interface is not connected and its a RTM_NEWLINK message then it
  means the link has gone down.
---------------------------------------------------------------------------*/
        else if((nlmsg->type == RTM_NEWLINK) &&
               !(nlmsg->nl_link_info.metainfo.ifi_flags & IFF_LOWER_UP))
        {
          LOG_MSG_INFO1("Got a del link event",0,0,0);
          if(!qti_netlink_conf->if_dev[interface_num].enabled)
          {
            LOG_MSG_ERROR("Interface not enabled. Ignorning LINK_DOWN", 0, 0, 0);
          }
          else
          {
/*--------------------------------------------------------------------------
  Post a link down command
---------------------------------------------------------------------------*/
            cmd_buf = qti_cmdq_get_cmd();
            if(cmd_buf == NULL)
            {
              LOG_MSG_ERROR("qti_cmdq: failed to allocate memeory for cmd",
                            0, 0, 0);
              goto error;
            }
            cmd_buf->data.event = QTI_LINK_DOWN_EVENT;
            cmd_buf->data.interface = interface;
            if( QTI_CMDQ_SUCCESS != qti_cmdq_put_cmd( cmd_buf ) )
            {
              qti_cmdq_release_cmd(cmd_buf);
              LOG_MSG_ERROR("qti_cmdq: failed to put commmand",0,0,0);
              goto error;
            }

            qti_netlink_conf->if_dev[interface_num].enabled = FALSE;
            LOG_MSG_INFO1("Sucessfully put cmd into command buffer",0,0,0);
/*--------------------------------------------------------------------------
  Write to dmesg log. It will help in debugging customer issues quickly.
  But we need to make sure we dont write too many messages to dmesg.
---------------------------------------------------------------------------*/
            ds_system_call("echo QTI:LINK_DOWN message posted > /dev/kmsg",
                           strlen("echo QTI:LINK_DOWN message posted > /dev/kmsg"));
          }
        }
      }
      else
      {
        LOG_MSG_INFO1("Ignoring event %d", nlmsg->type,0,0);
      }
      nlh = NLMSG_NEXT(nlh, msglen);
    }
    qcmap_nl_release_msg(msghdr);
    free(nlmsg);
    msghdr = NULL;
    nlmsg = NULL;
  }

  return QCMAP_UTIL_SUCCESS;
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

