/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     D E P E N D E N C Y    

GENERAL DESCRIPTION

This module provides support for external timestamp string err services.

Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_ts_string.c#1 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/15   abh     Created file for handling external timestamp string variable.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES
===========================================================================*/
#include "err_smem_log.h"

/*===========================================================================
                     EXTERN DECLARATION FOR THE MODULE
===========================================================================*/
/* External Timestamp Strings */
extern const char engg_timestampstring[];
extern const char qcom_timestampstring[];

/*===========================================================================
                     FUNCTION DEFINITION
===========================================================================*/

void err_write_timestamp_string(void)
{
  /* Call the smem function to write the timestamp string into SMEM */
  err_smem_write_timestamp_string(engg_timestampstring,qcom_timestampstring);
  return;
}

