#ifndef __TESTCLIENT_MSG_IF_H__
#define __TESTCLIENT_MSG_IF_H__

/*
  Copyright (C) 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/example/toyclient/inc/toyclient_msg_if.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/*****************************************************************************
 * Messages                                                                  *
 ****************************************************************************/

#define TOYCLIENT_ITEST_CMD_START_TEST ( 0x00011302 )
  /**<
   * This command starts APR test. The command response
   * APR_EOK is returned to the client.
   */

#endif /* __TESTCLIENT_MSG_IF_H__ */

