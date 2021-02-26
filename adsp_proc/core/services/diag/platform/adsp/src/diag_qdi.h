#ifndef DIAG_QDI_H
#define DIAG_QDI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diag Interface

GENERAL DESCRIPTION
  Contains diag qdi structure defination.

EXTERNALIZED FUNCTIONS 
None 

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2012-2015,2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/platform/adsp/src/diag_qdi.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   gn      Added support for diag services to uImage
11/11/15   as      File Created 
===========================================================================*/

#include "qurt_qdi_driver.h"
#include "qurt_signal.h"
#include "diagbuf_mpd_type.h"
#ifdef DIAG_UIMAGE_MODE
#include "micro_diagbuf_mpd_type.h"
#endif

typedef struct diag_qdi_opener_type
{
   qurt_qdi_obj_t    qdiobj;
   qurt_signal_t     signal;
   int               qdi_handle;
   diagbuf_mpd_type  *user_pd_node_ptr;
   #ifdef DIAG_UIMAGE_MODE
   micro_diagbuf_mpd_type  *user_pd_micro_node_ptr;
   #endif
} diag_qdi_opener; 

#endif //DIAG_QDI_H
