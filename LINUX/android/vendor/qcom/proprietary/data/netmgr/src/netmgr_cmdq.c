/**
  @file
  netmgr_cmdq.h

  @brief
  Netmgr command executor component.

  @details
  The netmgr command executor component is used to execute
  system commands.
*/
/*===========================================================================

  Copyright (c) 2014,2016,2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/14   kannana   Initial version

===========================================================================*/
#include <unistd.h>

#include "netmgr_cmdq.h"

/*===========================================================================
                         LOCAL FUNCTION DEFINITIONS
===========================================================================*/

static int
netmgr_cmdq_update_system_config
(
  const char    *filename,
  netmgr_cmdq_procsys_cgf_t *cfg
)
{
  FILE *fp = NULL;
  int rc   = NETMGR_FAILURE;
  int reti = NETMGR_SUCCESS;

  if (!filename || !cfg)
  {
    netmgr_log_err("%s(): invalid params!", __func__);
    goto bail;
  }

  /* Check if we can access the procfs/sysfs file */
  if (access(filename, F_OK))
  {
    netmgr_log_err("%s(): unable to access %s file. Err [%d:%s]",
                   __func__, filename, errno, strerror(errno));
    goto bail;
  }

  fp = fopen(filename, "we");
  if (!fp)
  {
    netmgr_log_err("%s(): unable to open %s file. Err [%d:%s]",
                   __func__, filename, errno, strerror(errno));
    goto bail;
  }

  switch (cfg->config_type)
  {
  case NETMGR_CMDQ_TYPE_INT:
    netmgr_log_low("%s(): setting %d in %s", __func__, cfg->config_val.intval, filename);
    fprintf(fp, "%d", cfg->config_val.intval);
    break;
  case NETMGR_CMDQ_TYPE_HEX:
    netmgr_log_low("%s(): setting %x in %s", __func__, cfg->config_val.intval, filename);
    fprintf(fp, "%x", cfg->config_val.intval);
    break;
  case NETMGR_CMDQ_TYPE_LONG:
    netmgr_log_low("%s(): setting %lu in %s", __func__, cfg->config_val.longval, filename);
    fprintf(fp, "%lu", cfg->config_val.longval);
    break;
  case NETMGR_CMDQ_TYPE_STR:
    netmgr_log_low("%s(): setting %s in %s", __func__, cfg->config_val.strval, filename);
    fprintf(fp, "%s", cfg->config_val.strval);
    break;
  case NETMGR_CMDQ_TYPE_INVALID:
  case NETMGR_CMDQ_TYPE_MAX:
  default:
    netmgr_log_err("%s(): unknown value type!", __func__);
    reti = NETMGR_FAILURE;
    break;
  }

  if (NETMGR_SUCCESS != reti)
  {
    goto bail;
  }

  rc = NETMGR_SUCCESS;

bail:
  if (fp)
  {
    if (fclose(fp))
    {
      netmgr_log_err("%s(): unable to close %s file! Err [%d:%s]",
                     __func__, filename, errno, strerror(errno));
    }
  }

  return rc;
}

/*===========================================================================
  FUNCTION:  netmgr_cmdq_cmd_exec
===========================================================================*/
/*!
    @brief
    Virtual function registered with the command thread to process
    a command

    @return None.
*/
/*=========================================================================*/
static void netmgr_cmdq_cmd_exec(ds_cmd_t *cmd, void *data)
{
  netmgr_cmdq_cmd_t         *cmd_buf;
  netmgr_cmdq_cmd_info_t    *cmd_info = NULL;
  struct ds_exec_opts       *opts = NULL;
  netmgr_cmdq_procsys_cgf_t *procsys_cfg_opts;
  int                       ret = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if ( NULL == cmd || NULL == data )
  {
    netmgr_log_err("%s(): Input parameters invalid", __func__);
    goto bail;
  }

  cmd_buf = (netmgr_cmdq_cmd_t*) data;
  cmd_info = (netmgr_cmdq_cmd_info_t*) cmd_buf->userdata;

  if (!cmd_info)
  {
    netmgr_log_err("%s(): command is invalid!", __func__);
    goto bail;
  }

  switch (cmd_info->cmd_type)
  {
  case CMD_TYPE_SYSCALL:
    opts = (struct ds_exec_opts*) cmd_info->cmd_opts;
    if (!opts)
    {
      netmgr_log_err("%s(): command options are invalid!", __func__);
      cmd_info->ret_code = DS_EXEC_BAD_ARGS;
      goto bail;
    }

    ret = ds_exec(cmd_info->cmd_str, opts);
    cmd_info->ret_code = ret;
    break;

  case CMD_TYPE_PROCSYS_CFG:
    procsys_cfg_opts = (netmgr_cmdq_procsys_cgf_t*) cmd_info->cmd_opts;

    if (!procsys_cfg_opts)
    {
      netmgr_log_err("%s(): command options are invalid!", __func__);
      cmd_info->ret_code = NETMGR_FAILURE;
      goto bail;
    }

    ret = netmgr_cmdq_update_system_config(cmd_info->cmd_str, procsys_cfg_opts);
    cmd_info->ret_code = ret;
    break;

  default:
    netmgr_log_err("%s(): unknown command type [%d]", __func__, cmd_info->cmd_type);
    cmd_info->ret_code = NETMGR_FAILURE;
    break;
  }

bail:
  /* Signal the waiting thread to resume execution */
  if (NETMGR_SUCCESS != sem_post(&cmd_info->cmd_sem))
  {
    netmgr_log_err("%s(): failed to post semaphore [%d:%s]",
                   __func__, errno, strerror(errno));
  }

  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION:  netmgr_cmdq_cmd_free
===========================================================================*/
/*!
    @brief
    Virtual function registered with the command thread to free command
    buffer after execution of the command is complete.

    @return None.
*/
/*=========================================================================*/
static void netmgr_cmdq_cmd_free(ds_cmd_t *cmd, void *data)
{
  netmgr_cmdq_cmd_t *cmd_buf = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  if (NULL == data || NULL == cmd)
  {
    netmgr_log_err("%s(): Received invalid data!", __func__);
    goto bail;
  }

  cmd_buf = (netmgr_cmdq_cmd_t*) data;

  if (!cmd_buf || (&cmd_buf->cmd != cmd))
  {
    netmgr_log_err("%s(): trying to free an invalid command buffer!", __func__);
    goto bail;
  }

  free(cmd_buf);

bail:
  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
                         GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*=========================================================================
  FUNCTION:  netmgr_cmdq_init
===========================================================================*/
/*!
    @brief
    Initializes netmgr command queue

    @details
    For handling use-cases which do not fit into the main netmgr state
    machine, we will use a separate command executor thread.

    @return
    NETMGR_SUCCESS
    NETMGR_FAILURE
*/
/*=========================================================================*/
int netmgr_cmdq_init()
{
  int rc;
  int ret = NETMGR_SUCCESS;

  netmgr_log_med("%s(): Initiliazing netmgr command queue", __func__);
  rc = ds_cmdq_init(&cmdq, NETMGR_CMDQ_MAX_CMDS);
  if (0 != rc)
  {
    netmgr_log_err("%s(): Failed to initialize netmgr command queue [%d]", __func__, rc);
    ret = NETMGR_FAILURE;
  }

  return ret;
}

/*=========================================================================
  FUNCTION:  netmgr_cmdq_get_cmd
===========================================================================*/
/*!
    @brief
    Function to get a command object

    @return
    netmgr_cmdq_cb_cmd_t* type object on success
    NULL on failure
*/
/*=========================================================================*/
netmgr_cmdq_cmd_t* netmgr_cmdq_get_cmd( void )
{
  netmgr_cmdq_cmd_t *cmd_buf = NULL;

  /* Allocate command buffer */
  cmd_buf = (netmgr_cmdq_cmd_t*) malloc( sizeof(netmgr_cmdq_cmd_t) );
  if ( NULL == cmd_buf )
  {
    netmgr_log_err("%s(): Command buffer allocation failed!", __func__);
    goto bail;
  }

  memset(cmd_buf, 0x0, sizeof(netmgr_cmdq_cmd_t));

  /* Assign default execution and free handlers */
  cmd_buf->cmd.execute_f = netmgr_cmdq_cmd_exec;
  cmd_buf->cmd.free_f    = netmgr_cmdq_cmd_free;
  cmd_buf->cmd.data      = (void*) cmd_buf;

bail:
  return cmd_buf;
}

/*=========================================================================
  FUNCTION:  netmgr_cmdq_release_cmd
===========================================================================*/
/*!
    @brief
    Function to get a command object

    @return
    None
*/
/*=========================================================================*/
void netmgr_cmdq_release_cmd( netmgr_cmdq_cmd_t *cmd_buf )
{
  if (NULL != cmd_buf && cmd_buf->cmd.free_f )
  {
    cmd_buf->cmd.free_f(&cmd_buf->cmd, cmd_buf->cmd.data);
  }
  else
  {
    netmgr_log_err("%s(): Specified buffer not valid, ignoring", __func__);
  }
}

/*=========================================================================
  FUNCTION:  netmgr_cmdq_put_cmd
===========================================================================*/
/*!
    @brief
    Function to post to command queue

    @return
    NETMGR_SUCCESS
    NETMGR_FAILURE
*/
/*=========================================================================*/
int netmgr_cmdq_put_cmd( netmgr_cmdq_cmd_t *cmd_buf )
{
  int result = NETMGR_FAILURE;

  if ( NULL == cmd_buf )
  {
    netmgr_log_err("%s(): Command buffer is NULL!", __func__);
    goto bail;
  }

  /* Add to command queue */
  result = ds_cmdq_enq(&cmdq, &cmd_buf->cmd);

bail:
  return result;
}
