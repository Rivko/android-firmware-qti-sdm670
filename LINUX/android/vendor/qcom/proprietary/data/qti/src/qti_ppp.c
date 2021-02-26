/******************************************************************************

                        QTI_PPP.C

******************************************************************************/

/******************************************************************************

  @file    qti_ppp.c
  @brief   Qualcomm Tethering Interface Module for DUN connections.

  DESCRIPTION
  Implementation of Qualcomm Tethering Interface.

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/19/14   cp         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include "qti_ppp.h"

/*===========================================================================
                              VARIABLE DECLARATIONS
===========================================================================*/
static qti_ppp_conf_t                qti_ppp_conf;
qcmap_sk_fd_set_info_t   sk_fdset;
qti_usb_tty_config   usb_tty_config_info;
qti_usb_line_state_config usb_line_state_config_info;
qti_smd_tty_config   smd_tty_config_info;

qcmap_sk_info_t  sk_info;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION IsDUNSoftAPMode
==========================================================================*/
/*!
@brief
  Checks if we are in DUN+SoftAP Mode or not.

@parameters
  None.

@return
  true  - If we are in DUN+SoftAP mode.
  flase - if we are not in DUN+SoftAP mode.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static boolean IsDunSoftAPMode(void)
{
  int i = 0;
  FILE *cmd = NULL;
  char mode = 0;
  const char process[] = "cat /sbin/usb/softap_w_dun";

  cmd = popen(process, "r");
  if(cmd)
    mode = fgetc(cmd);

  if ( mode == 'Y' )
  {
    LOG_MSG_INFO1("In DUN+SoftAP mode.", 0, 0, 0);
    return TRUE;
  }
  else
  {
    LOG_MSG_ERROR(" Not in DUN+SoftAP mode.", 0, 0, 0);
    return FALSE;
  }
}

/*===========================================================================

FUNCTION QTI_PPP_CLEAR_FD()

DESCRIPTION

  This function
  - Removes the fd to the list of FD on which select call listens.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int qti_ppp_clear_fd
(
   qcmap_sk_fd_set_info_t *fd_set,
   int                     fd
)
{
  int i=0, j=0;
/*--------------------------------------------------------------------------
  Remove fd to fdmap array.
-------------------------------------------------------------------------- */
  for ( i = 0; i < fd_set->num_fd; i++ )
  {
     if ( fd_set->sk_fds[i].sk_fd == fd )
       break;
  }

  if ( i == fd_set->num_fd )
  {
    LOG_MSG_ERROR("Something wrong FD %d not found", fd, 0, 0);
    return QTI_PPP_FAILURE;
  }

  /* Clear the fd. */
  for ( j = i; j < ( fd_set->num_fd - 1); j++ )
  {
    fd_set->sk_fds[j].sk_fd = fd_set->sk_fds[j+1].sk_fd;
    fd_set->sk_fds[j].read_func = fd_set->sk_fds[j+1].read_func;
  }

  fd_set->sk_fds[fd_set->num_fd - 1].sk_fd = 0;
  fd_set->sk_fds[fd_set->num_fd - 1].read_func = NULL;
  fd_set->num_fd--;

/*--------------------------------------------------------------------------
  Re-calculate max_fd
--------------------------------------------------------------------------*/
  fd_set->max_fd = 0;
  for ( i = 0; i < fd_set->num_fd; i++ )
  {
     if ( fd_set->max_fd < fd_set->sk_fds[i].sk_fd )
       fd_set->max_fd = fd_set->sk_fds[i].sk_fd;
  }

  return QTI_PPP_SUCCESS;
}


/*==========================================================================

FUNCTION MAIN()

DESCRIPTION

  The main function for QTI which is first called when QTI gets started on
  boot up.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

==========================================================================*/
int main(int argc, char ** argv)
{
  int                       retval = QTI_PPP_SUCCESS;

/*------------------------------------------------------------------------*/
    /* Initializing Diag for QXDM loga*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
     LOG_MSG_ERROR("Diag_LSM_Init failed !!", 0, 0, 0);
  }

  /* Check if we are in DUN+SoftAP mode or not. */
  if ( !IsDunSoftAPMode() )
  {
    exit(0);
  }

  LOG_MSG_INFO1("Start QTI PPP", 0, 0, 0);

/*-----------------------------------------------------------------------
    Initialize QTI variables
------------------------------------------------------------------------*/
  memset(&sk_fdset, 0, sizeof(qcmap_sk_fd_set_info_t));
  memset(&usb_tty_config_info, 0, sizeof(qti_usb_tty_config));
  memset(&smd_tty_config_info, 0, sizeof(qti_smd_tty_config));

/*-----------------------------------------------------------------------
    Initialize QTI_PPP to be a client of QCMAP
------------------------------------------------------------------------*/
  retval = qti_ppp_qcmap_init(&qti_ppp_conf);
  if(retval != QTI_PPP_SUCCESS)
  {
    LOG_MSG_ERROR("QTI %d: Failed to initialize QTI as a QCMAP client",
                   0, 0, 0);
    return QTI_PPP_FAILURE;
  }

/*---------------------------------------------------------------------
  Call into the netlink listener init function which sets up QTI to
  listen to netlink events
---------------------------------------------------------------------*/
  if (qcmap_nl_listener_init(
                                 NETLINK_ROUTE,
                                 RTMGRP_LINK,
                                 &sk_fdset,
                                 qti_ppp_nl_recv_msg,
                                 &sk_info,
                                 MAX_NUM_OF_FD) == QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_INFO1("Open netlink socket succeeds",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Netlink socket open failed",0,0,0);
    return QTI_PPP_FAILURE;
  }

/*---------------------------------------------------------------------
    Call into the USB TTY listener init function which sets up QTI to
    listen to AT Commands coming in from the USB device file for DUN
---------------------------------------------------------------------*/
  retval = qti_usb_tty_listener_init(&usb_tty_config_info,
                                      &sk_fdset,
                                      qti_usb_tty_recv_msg);
  if(retval != QTI_PPP_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize QTI USB TTY listener.", 0, 0, 0);
  }

/*---------------------------------------------------------------------
    Create a Notifier fd on the DUN TTY connected status flag.
---------------------------------------------------------------------*/
  retval = qti_usb_line_state_init(&usb_line_state_config_info,
                                      &sk_fdset,
                                      qti_usb_line_state_recv_msg);
  if(retval != QTI_PPP_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to initialize QTI USB TTY Notifier.", 0, 0, 0);
    return QTI_PPP_FAILURE;
  }

/*---------------------------------------------------------------------
    Call into the SMD TTY listener init function which sets up QTI to
    listen to AT Commands coming in from the Modem for DUN
---------------------------------------------------------------------*/
    retval = qti_smd_tty_listener_init(&smd_tty_config_info,
                                      &sk_fdset,
                                      qti_smd_tty_recv_msg);
    if(retval != QTI_PPP_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to initialize QTI SMD TTY listener",0,0,0);
      return QTI_PPP_FAILURE;
    }

/*--------------------------------------------------------------------
  Start the listener which listens to netlink events and AT commands.
---------------------------------------------------------------------*/

  retval = qcmap_listener_start(&sk_fdset);

  if(retval != QCMAP_UTIL_SUCCESS)
  {
    LOG_MSG_ERROR("QTI PPP:Failed to start NL listener",
                   0, 0, 0);
  }
  qti_ppp_qcmap_exit();
  return QTI_PPP_SUCCESS;
}
