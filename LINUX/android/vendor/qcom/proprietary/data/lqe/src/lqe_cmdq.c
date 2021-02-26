/******************************************************************************

                         L Q E _  C M D Q . C

******************************************************************************/

/******************************************************************************

  @file    lqe_cmdq.c
  @brief   Command thread to handle QMI indications

  DESCRIPTION
  QMI callbacks needs to be released quickly, as the QMI has
  other clients to service. This command thread helps switch
  context. The QMI indications are relayed to the client in
  this command thread context.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include "ds_list.h"
#include "ds_cmdq.h"
#include "lqe_cmdq.h"
#include "lqei.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/* Max commands in the queue */
#define LQE_CMDQ_MAX_CMDS 100

/* LQE command queue */
static struct ds_cmdq_info_s lqe_cmdq;


/*===========================================================================
                            LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  lqe_cmdq_free_cmd_cb
===========================================================================*/
/*!
@brief
 Callback function registered with the Command Thread to free a command buffer
 after execution of the command is complete

@param
  ds_cmd - Command to be freed
  data   - Command data

@return
  void

*/
/*=========================================================================*/
static void lqe_cmdq_free_cmd_cb
(
  ds_cmd_t *ds_cmd,
  void *data
)
{
  (void)data;
  /* Free the memory allocated for the command */
  free((lqe_cmdq_cmd_type *)ds_cmd);
}

/*===========================================================================
  FUNCTION  lqe_cmdq_alloc_cmd
===========================================================================*/
lqe_cmdq_cmd_type *lqe_cmdq_alloc_cmd
(
  void
)
{
  lqe_cmdq_cmd_type *cmd = NULL;

  /* Allocate a new command */
  if (NULL == (cmd = malloc(sizeof(lqe_cmdq_cmd_type))))
  {
    LQE_LOG_ERROR("%s: alloc failed\n", __func__);
    return NULL;
  }

  memset(cmd, 0, sizeof(lqe_cmdq_cmd_type));

  /* Asssign default execution and free handlers */
  cmd->ds_cmd.execute_f = lqe_process_cmd_cb;
  cmd->ds_cmd.free_f    = lqe_cmdq_free_cmd_cb;

  return cmd;
}

/*===========================================================================
  FUNCTION  lqe_cmdq_free_cmd
===========================================================================*/
void lqe_cmdq_free_cmd
(
  lqe_cmdq_cmd_type *cmd
)
{
  if (NULL == cmd)
  {
    LQE_LOG_ERROR("%s bad params\n", __func__);
    return;
  }

  if (NULL != cmd->ds_cmd.free_f)
  {
    cmd->ds_cmd.free_f(&cmd->ds_cmd, cmd->ds_cmd.data );
  }
  else
  {
    LQE_LOG_ERROR("%s: free_f NULL\n", __func__);
  }
}

/*===========================================================================
  FUNCTION  lqe_cmdq_enqueue_cmd
===========================================================================*/
int lqe_cmdq_enqueue_cmd
(
  const lqe_cmdq_cmd_type *cmd
)
{
  if (NULL == cmd)
  {
    LQE_LOG_ERROR("%s: bad params\n", __func__);
    return LQE_FAILURE;
  }

  /* Enqueue the command */
  return ds_cmdq_enq(&lqe_cmdq, (const ds_cmd_t *)cmd);

}

/*===========================================================================
  FUNCTION  lqe_cmdq_init
===========================================================================*/
int lqe_cmdq_init
(
  void
)
{
  /* Initialize the command queue */
  return ds_cmdq_init(&lqe_cmdq, LQE_CMDQ_MAX_CMDS);
}

/*===========================================================================
  FUNCTION  lqe_cmdq_deinit
===========================================================================*/
int lqe_cmdq_deinit
(
  void
)
{
  /* Deinitialize the command queue */
  return ds_cmdq_deinit(&lqe_cmdq);
}
