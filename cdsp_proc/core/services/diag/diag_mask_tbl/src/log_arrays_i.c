/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
        DIAGNOSTIC LOG SERVICE: ARRAYS FOR LOG EQUIPMENT IDS


GENERAL DESCRIPTION
Contains structures/definitions for log equipment ids 
  
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

  $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/diag_mask_tbl/src/log_arrays_i.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/10   sg      Created Initial Version of the file.
                   Moved log_last_item_tbl from diaglogi.h to here 
===========================================================================*/
#include "comdef.h"
#include "log_arrays_i.h"

/* 'log_last_item_tbl' is an array of offsets into log_mask indexed by
 * equipment ID.*/

/* Lookup table for the number of bits in a given equip ID's mask. */
const uint16 log_last_item_tbl[LOG_EQUIP_ID_MAX + 1] = {
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_0_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_1_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_2_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_3_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_4_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_5_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_6_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_7_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_8_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_9_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_10_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_11_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_12_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_13_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_14_LAST_CODE),
  LOG_GET_ITEM_NUM(LOG_EQUIP_ID_15_LAST_CODE)
};

