#ifndef __APR_DIAG_H__
#define __APR_DIAG_H__

/*
   Copyright (C) 2010,2016 QUALCOMM Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - QUALCOMM Technologies, Inc.

   $Header: //components/rel/apr.common/2.0/datalink/diag/inc/aprv2_diag.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_packet.h"

APR_INTERNAL int32_t aprv2_diag_init ( void );
APR_INTERNAL int32_t aprv2_diag_deinit ( void );

APR_INTERNAL int32_t aprv2_diag_send ( aprv2_packet_t* packet );

#endif /* __APR_DIAG_H__ */

