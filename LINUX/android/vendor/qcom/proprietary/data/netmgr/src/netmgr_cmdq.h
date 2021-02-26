#ifndef _NETMGR_CMDQ_H_
#define _NETMGR_CMDQ_H_
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

  Copyright (c) 2014,2016 Qualcomm Technologies, Inc.
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <semaphore.h>

#include "ds_util.h"
#include "ds_cmdq.h"
#include "netmgr_util.h"

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
#define NETMGR_CMDQ_MAX_CMDS (20)

typedef enum
{
  CMD_TYPE_UNKNWON = -1,
  CMD_TYPE_SYSCALL,
  CMD_TYPE_PROCSYS_CFG,
  CMD_TYPE_MAX
} netmgr_cmdq_cmd_type_t;

/*---------------------------------------------------------------------------
   Argument type which is used for setting procfs/sysfs entries
---------------------------------------------------------------------------*/
typedef union
{
  int intval;
  unsigned long longval;
  char strval[20];
} netmgr_cmdq_cfg_val_t;

typedef enum
{
  NETMGR_CMDQ_TYPE_INVALID = -1,
  NETMGR_CMDQ_TYPE_INT,
  NETMGR_CMDQ_TYPE_HEX,
  NETMGR_CMDQ_TYPE_LONG,
  NETMGR_CMDQ_TYPE_STR,
  NETMGR_CMDQ_TYPE_MAX
} netmgr_cmdq_cfg_val_type_t;

typedef struct
{
  netmgr_cmdq_cfg_val_t       config_val;
  netmgr_cmdq_cfg_val_type_t  config_type;
} netmgr_cmdq_procsys_cgf_t;

typedef struct
{
  sem_t                   cmd_sem;
  netmgr_cmdq_cmd_type_t  cmd_type;
  int                     ret_code;
  char                    cmd_str[NETMGR_MAX_COMMAND_LENGTH];
  void*                   *cmd_opts;
} netmgr_cmdq_cmd_info_t;

typedef struct
{
  ds_cmd_t   cmd;
  void*      userdata;
} netmgr_cmdq_cmd_t;

ds_cmdq_info_t  cmdq;

/*=========================================================================
  FUNCTION:  netmgr_cmdq_init

===========================================================================*/
/*!
    @brief
    Initializes netmgr command queue

    @return
    NETMGR_SUCCESS
    NETMGR_FAILURE
*/
/*=========================================================================*/
int netmgr_cmdq_init( void );

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
netmgr_cmdq_cmd_t* netmgr_cmdq_get_cmd( void );

/*=========================================================================
  FUNCTION:  netmgr_cmdq_release_cmd

===========================================================================*/
/*!
    @brief
    Function to release a command object

    @return
    None
*/
/*=========================================================================*/
void netmgr_cmdq_release_cmd( netmgr_cmdq_cmd_t *cmd_buf );

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
int netmgr_cmdq_put_cmd( netmgr_cmdq_cmd_t *cmd_buf );

#endif /* _NETMGR_CMDQ_H_ */
