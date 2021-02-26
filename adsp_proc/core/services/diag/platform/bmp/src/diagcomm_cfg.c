/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Configuration

General Description

Copyright (c) 2013, 2015-2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/platform/bmp/src/diagcomm_cfg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/16   ph      SMD references removal and code clean up.
08/18/15   as      Fixed backwards compatibility issues with socket features 
04/08/15   ps      Diag over sockets and dsm removal 
08/08/13   sg      Command/response separation

===========================================================================*/

#include "diagcomm_io.h"
#include "qsocket_ids.h"
#include "stdio.h"               /* For snprintf */
#include <string.h>
#include "osal.h"

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; 

char diag_ctrl_read_buf[DIAG_CTRL_READ_BUF_SIZE];
char diag_cmd_read_buf[DIAG_CMD_READ_BUF_SIZE];


/*===========================================================================

FUNCTION DIAGCOMM_IO_INIT

DESCRIPTION
  Initilizes Diag variables for interprocess communication and I/O
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_io_init( void )
{
  int ret;
  uint8 i, j = 0;
  
  for( i=0; i < NUM_PORT_TYPES; i++ )
  {
    for( j=0; j < NUM_REMOTE_PORTS; j++ )
    {
      (void)memset( &diagcomm_io_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      (void)memset( &diagcomm_io_ctrl_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
      (void)memset( &diagcomm_io_cmd_conn[i][j], 0, sizeof(diagcomm_io_conn_type) );
	  
      diagcomm_io_conn[i][j].port_num = j;
      diagcomm_io_conn[i][j].connected = FALSE;
      diagcomm_io_conn[i][j].channel_connected = FALSE;
      diagcomm_io_conn[i][j].data_pending = FALSE;
      diagcomm_io_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_conn[i][j].channel_type = DIAGCOMM_IO_DATA;
      diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][j].port_type = DIAGCOMM_PORT_REMOTE_PROC;
      diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][j].allow_flow = TRUE;     

      diagcomm_io_ctrl_conn[i][j].port_num = j;
      diagcomm_io_ctrl_conn[i][j].connected = FALSE;
      diagcomm_io_ctrl_conn[i][j].channel_connected = FALSE;
      diagcomm_io_ctrl_conn[i][j].data_pending = FALSE;
      diagcomm_io_ctrl_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_ctrl_conn[i][j].channel_type = DIAGCOMM_IO_CTRL;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][j].port_type = DIAGCOMM_PORT_REMOTE_PROC;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][j].allow_flow = TRUE;

      diagcomm_io_cmd_conn[i][j].port_num = j;
      diagcomm_io_cmd_conn[i][j].connected = FALSE;
      diagcomm_io_cmd_conn[i][j].channel_connected = FALSE;
      diagcomm_io_cmd_conn[i][j].data_pending = FALSE;
      diagcomm_io_cmd_conn[i][j].io_type = DIAGCOMM_IO_TYPE_INVALID;
      diagcomm_io_cmd_conn[i][j].channel_type = DIAGCOMM_IO_CMD;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][j].port_type = DIAGCOMM_PORT_REMOTE_PROC;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][j].allow_flow = TRUE;

    }
  }
 
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].isServer = FALSE;
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_DATA;
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;
//No incoming data over the data channel. 
  diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_length = 0;

//CTRL Channel
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].isServer = FALSE;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_CTRL;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
//  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;

  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf = &diag_ctrl_read_buf;
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_length = sizeof(diag_ctrl_read_buf);
  diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_CTRL_BUF";
  ret = osal_init_mutex(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_mutex);
  ASSERT(OSAL_SUCCESS == ret);

  //Settings to be used when using sockets
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].isServer = TRUE;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.sa_family = AF_IPC_ROUTER;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addrtype = IPCR_ADDR_NAME;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addr.port_name.instance= DIAG_SOCKET_INST_ID_CMD;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].send_flags = QMSG_DONTWAIT;

  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf = &diag_cmd_read_buf;
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_length = sizeof(diag_cmd_read_buf);
  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_mutex.name = "MUTEX_DIAG_CMD_BUF";
  ret = osal_init_mutex(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].buf_mutex);
  ASSERT(OSAL_SUCCESS == ret);
		
} /* diagcomm_io_init */

void diagcomm_update_io_type(void)
{
   diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type = diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].change_to_type;
   diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type = diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].change_to_type;
   diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type = diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].change_to_type;
}
