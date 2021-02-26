
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications for Diag Command/Response Channel

General Description

Copyright (c) 2013,2015-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_cmd.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/17   gn      Changes to handle remote socket close
05/12/16   ph      SMD references removal and code clean up.
04/08/15   ps      Diag over sockets and dsm removal 
08/08/13   is      Command/response separation

===========================================================================*/

#include "diagcomm_cmd.h"
#include "msg.h"             /* For MSG_1, etc. */
#include "assert.h"          /* For ASSERT() */
#include "diagcomm_v.h"

extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_cfg.c */
/*===========================================================================

FUNCTION DIAGCOMM_CMD_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer
  for the command/response channel.

PARAMETERS
  port_type - Cmd channel port type
  port_num - Cmd channel to initilize
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_cmd_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num )
{  
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( port_type == DIAGCOMM_PORT_REMOTE_PROC );
  ASSERT( port_num <= DIAGCOMM_PORT_NUM_LAST );
  
  conn = &diagcomm_io_cmd_conn[port_type][port_num];
      
  /* Make sure we are closed */
  diagcomm_cmd_close( port_type, port_num );
  
} /* diagcomm_cmd_init */

  
/*===========================================================================

FUNCTION DIAGCOMM_CMD_OPEN

DESCRIPTION
  Opens the communcications stream for the command/response channel.

PARAMETERS
  port_type - Cmd channel port type
  channel - Cmd channel to open
  
RETURN VALUE
  True if control port opened successfully.
  False otherwise.
  
===========================================================================*/
boolean 
diagcomm_cmd_open( diagcomm_port_type port_type, diagcomm_enum_port_type channel )
{
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( port_type == DIAGCOMM_PORT_REMOTE_PROC );
  ASSERT( channel <= DIAGCOMM_PORT_NUM_LAST );
  
  conn = &diagcomm_io_cmd_conn[port_type][channel];
  
  if( conn->connected )
  {
    return conn->connected;
  }
  
  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
    }
    else
    {
	  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_cmd_open - Could not open stream (%d, %d)", port_type, channel);
    }
  }
  
  return conn->connected;
  
} /* diagcomm_cmd_open */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_CLOSE

DESCRIPTION
  Closes the communcications stream for the command/response channel, if any.

PARAMETERS
  port_type - Cmd channel port type
  channel - Cmd channel to close
 
RETURN VALUE
  None
  
===========================================================================*/
void 
diagcomm_cmd_close( diagcomm_port_type port_type, diagcomm_enum_port_type channel )
{
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( port_type == DIAGCOMM_PORT_REMOTE_PROC);
  ASSERT( channel <= DIAGCOMM_PORT_NUM_LAST );
  
  conn = &diagcomm_io_cmd_conn[port_type][channel];
  
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
    
  }

  /* Success or not, consider close operation successful */
  conn->connected = FALSE;
  
} /* diagcomm_cmd_close */

/*===========================================================================

FUNCTION DIAGCOMM_CMD_STATUS

DESCRIPTION
    This function returns TRUE if the port passed in is open.

===========================================================================*/
boolean
diagcomm_cmd_status (diagcomm_enum_port_type port)
{
  return (diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port].connected \
  	       && diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port].channel_connected);   
}
