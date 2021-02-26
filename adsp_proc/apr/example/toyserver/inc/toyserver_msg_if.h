#ifndef __TESTSERVER_MSG_IF_H__
#define __TESTSERVER_MSG_IF_H__

/*
  Copyright (C) 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/example/toyserver/inc/toyserver_msg_if.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/*****************************************************************************
 * Messages                                                                  *
 ****************************************************************************/

#define TOYSERVER_ITEST_CMD_ADD ( 0x000112C8 )
  /**<
   * This command adds two values and returns the sum. The command response
   * TOYSERVER_ITEST_RSP_ADD is returned to the client when finished.
   */

typedef struct toyserver_itest_cmd_add_t toyserver_itest_cmd_add_t;

#include "apr_pack_begin.h"
struct toyserver_itest_cmd_add_t
{
  uint32_t a;
  uint32_t b;
}
#include "apr_pack_end.h"
;


#define TOYSERVER_ITEST_RSP_ADD ( 0x000112C9 )
  /**<
   * This command response returns the sum of the two values provided from
   * the command TOYSERVER_ITEST_CMD_ADD.
   */

typedef struct toyserver_itest_rsp_add_t toyserver_itest_rsp_add_t;

#include "apr_pack_begin.h"
struct toyserver_itest_rsp_add_t
{
  uint32_t sum;
}
#include "apr_pack_end.h"
;

#define TOYCLIENT_ITEST_CMD_START_TEST ( 0x00011302 )
  /**<
   * This command starts APR test. The command response
   * APR_EOK is returned to the client.
   */

#endif /* __TESTSERVER_MSG_IF_H__ */

