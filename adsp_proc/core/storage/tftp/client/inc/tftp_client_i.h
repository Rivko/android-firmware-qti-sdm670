/***********************************************************************
 * tftp_client_i.h
 *
 * Short description
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/client/inc/tftp_client_i.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-20   dks   Remove rfs and tftp dependency
2014-07-18   rp    Create

===========================================================================*/

#ifndef __TFTP_CLIENT_I_H__
#define __TFTP_CLIENT_I_H__


#include "tftp_client_config.h"
#include "tftp_client.h"
#include "tftp_comdef.h"
#include "tftp_connection_addr.h"

/*----------------------------------------------------------------------------
  tftp-client : Configuration Items.
----------------------------------------------------------------------------*/
struct tftp_client_internal_config_type
{
  struct tftp_client_config_type config;
  struct tftp_connection_ipcr_addr server_addr;
};

int tftp_client_internal_config_set (
  struct tftp_client_internal_config_type *config);
int tftp_client_internal_config_get (
  struct tftp_client_internal_config_type *config);
int tftp_client_is_partial_files_supported (void);

#endif /* not __TFTP_CLIENT_I_H__ */
