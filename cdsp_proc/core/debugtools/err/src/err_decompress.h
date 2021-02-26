#ifndef ERR_DECOMPRESS_H
#define ERR_DECOMPRESS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR Service Decompression Module
                
GENERAL DESCRIPTION
  implements code required to zlib decompress err data compressed in elf image

Copyright (c) 2013 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_decompress.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/11/13   mcg     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err.h"
#include "errlog.h"

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
  err_const_type msg_blk;
  char fmt_buf[ERR_LOG_MAX_MSG_LEN]; // Only used on successful decompress
} err_decompress_struct_type;

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
void err_decompress_msg_const(const err_const_type* const_blk_ptr, err_decompress_struct_type* dest_ptr);

#endif /* ERR_DECOMPRESS_H */
