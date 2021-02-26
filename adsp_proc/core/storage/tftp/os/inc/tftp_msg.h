/***********************************************************************
 * tftp_msg.h
 *
 * Short description
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/inc/tftp_msg.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-12-03   rp    Create

===========================================================================*/

#ifndef __TFTP_MSG_H__
#define __TFTP_MSG_H__

#include "tftp_config_i.h"
#include "tftp_log.h"


void tftp_msg_init (void);

void tftp_msg_print (enum tftp_log_msg_level_type, const char *msg);



#endif /* not __TFTP_MSG_H__ */
