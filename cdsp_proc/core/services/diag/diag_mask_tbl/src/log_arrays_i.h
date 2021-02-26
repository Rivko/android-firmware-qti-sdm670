#ifndef LOG_ARRAYS_I_H
#define LOG_ARRAYS_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
        DIAGNOSTIC LOG SERVICE: ARRAYS FOR LOG EQUIPMENT IDS


GENERAL DESCRIPTION
   Internal Header File. Contains structures/definitions for log equipment ids 
  
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

  $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/diag_mask_tbl/src/log_arrays_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/10   sg      Fixed compiler warnings
04/13/10   sg      Created Initial Version of the file.
                   Moved log_last_item_tbl from diaglogi.h to here 
===========================================================================*/

#include "comdef.h"
#include "diaglogi.h"

extern const uint16 log_last_item_tbl[LOG_EQUIP_ID_MAX + 1];

#endif /* LOG_ARRAYS_I_H */

