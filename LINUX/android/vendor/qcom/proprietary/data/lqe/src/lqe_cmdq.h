/******************************************************************************

                                L Q E _  C M D Q . H

******************************************************************************/

/******************************************************************************
  @file    lqe_cmdq.h
  @brief   Command thread to handle QMI indications

  DESCRIPTION
  QMI callbacks needs to be released quickly, as the QMI has
  other clients to service. This command thread helps switch
  context. The QMI indications are relayed to the client in
  this command thread context.

*****************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __LQE_CMDQ_H__
#define __LQE_CMDQ_H__

#include "lqe.h"
#include "lqei.h"
#include "ds_util.h"
#include "ds_cmdq.h"

typedef enum
{
  LQE_CB_CMD_QMI_SYS_IND,
  LQE_CB_CMD_QMI_OTT_IND,
  LQE_CB_CMD_QMI_WDS_IND,
  LQE_CB_CMD_MAX
} lqe_cmdq_evt_type_t;

typedef union lqe_ind_event_info_s {
  lqe_qmi_sys_event_ind_t qmi_sys_ind;
  lqe_qmi_ott_event_ind_t qmi_ott_ind;
  lqe_qmi_wds_event_ind_t qmi_wds_ind;
} lqe_cmdq_evt_info_t;

/* Typedef for lqe command type */
typedef struct
{
  ds_cmd_t                   ds_cmd;   /* Should be the first entry in the structure */
  int                        lqe_handle;
  lqe_cmdq_evt_type_t        event;
  lqe_cmdq_evt_info_t        cmd_data;
} lqe_cmdq_cmd_type;

/*===========================================================================
  FUNCTION  lqe_cmdq_alloc_cmd
===========================================================================*/
lqe_cmdq_cmd_type *lqe_cmdq_alloc_cmd(void);


/*===========================================================================
  FUNCTION  lqe_cmdq_free_cmd
===========================================================================*/
void lqe_cmdq_free_cmd
(
  lqe_cmdq_cmd_type *cmd
);

/*===========================================================================
  FUNCTION  lqe_cmdq_enqueue_cmd
===========================================================================*/
int lqe_cmdq_enqueue_cmd
(
  const lqe_cmdq_cmd_type *cmd
);

/*===========================================================================
  FUNCTION  lqe_cmdq_init
===========================================================================*/
int lqe_cmdq_init(void);

/*===========================================================================
  FUNCTION  lqe_cmdq_deinit
===========================================================================*/
int lqe_cmdq_deinit(void);

#endif
