
#ifndef DIAGCOMM_CMD_H 
#define DIAGCOMM_CMD_H
/*==========================================================================

              Diagnostic Communications for Diag Command/Response Channel

General Description
  
Copyright (c) 2013,2016-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_cmd.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/29/17   gn      Changes to handle remote socket close
05/12/16   ph      SMD references removal and code clean up.
08/08/13   sg      Command/response separation

===========================================================================*/

#include "diagcomm_io.h"     /* For diagcomm_io_conn_type, diagcomm_enum_port_type */


/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

boolean diagcomm_cmd_open( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
boolean diagcomm_cmd_status (diagcomm_enum_port_type port);
void diagcomm_cmd_close( diagcomm_port_type port_type, diagcomm_enum_port_type channel );
void diagcomm_cmd_init( diagcomm_port_type port_type, diagcomm_enum_port_type channel );

#endif //DIAGCOMM_CMD_H
