#ifndef USLEEP_TRANS_H
#define USLEEP_TRANS_H
/*==============================================================================
  FILE:         uSleep_trans.h

  OVERVIEW:     Provides main transition API intface functions and types for uSleep

  DEPENDENCIES: None
 
                Copyright (c) 2014-2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/inc/uSleep_trans.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * uSleepTrans_notifyComplete
 * 
 * @brief Notifies uSleep that the island transition has been completed.
 *  
 * @param entry: TRUE   -> Island entry is complete
 *               FALSE  -> Island exit is complete
 */
void uSleepTrans_notifyComplete(uint8 entry);

#endif /* USLEEP_OS_H */

