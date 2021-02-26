#ifndef __A_NODE_TEST_MSG_H__
#define __A_NODE_TEST_MSG_H__

/*==============================================================================
  @file a_node_msg.h
  @brief Specifies the Message format used to communicate between a_nodes

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/test/inc/test_msg.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  02/25/16    Unni     Creating a_node using Toy Server/Client as reference
==============================================================================*/

#include "apr_comdef.h"

/*==============================================================================
  Message Definitions
==============================================================================*/

/*------------------------------------------------------------------------------
 * ADD Command
 *
 * This command adds two values and returns the sum. The command response is
 * RSP_CALC is returned to client when finished.
 *----------------------------------------------------------------------------*/

/* Unique ID to identify ADD Command */
#define CMD_ADD ( 0x10 )

/* Payload structure definition for ADD Command */
typedef struct cmd_add cmd_add_t;

#include "apr_pack_begin.h" /* Used for Structure Packing */
struct cmd_add
{
  uint16_t a;
  uint16_t b;
}
#include "apr_pack_end.h"   /* Used for Structure Packing */
;
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * DIFF Command
 *
 * This command adds two values and returns the sum. The command response is
 * RSP_CALC is returned to client when finished.
 *----------------------------------------------------------------------------*/

/* Unique ID to identify DIFF Command */
#define CMD_DIFF ( 0x11 )

/* Payload structure definition for SUBSTRACT Command */
typedef struct cmd_diff cmd_diff_t;

#include "apr_pack_begin.h" /* Used for Structure Packing */
struct cmd_diff
{
  uint16_t a;
  uint16_t b;
}
#include "apr_pack_end.h"   /* Used for Structure Packing */
;
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Response
 *
 * Generic Response for Calculations
 *----------------------------------------------------------------------------*/
/* Unique ID to identify ADD Response */
#define RSP_CALC ( 0x20 )

/* Payload structure definition for ADD Response */
typedef struct rsp_calc rsp_calc_t;

#include "apr_pack_begin.h" /* Used for Structure Packing */
struct rsp_calc
{
  uint32_t result;
}
#include "apr_pack_end.h"   /* Used for Structure Packing */
;
/*----------------------------------------------------------------------------*/

#endif /* __A_NODE_TEST_MSG_H__ */

