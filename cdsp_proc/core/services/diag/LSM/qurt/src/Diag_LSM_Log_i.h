#ifndef DIAG_LSM_LOG_I_H
#define DIAG_LSM_LOG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
EXTENDED DIAGNOSTIC LOG LEGACY SERVICE MAPPING HEADER FILE
(INTERNAL ONLY)

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support the reporting 
  of messages.  This includes support for the 
  extended capabilities as well as the legacy messaging scheme.

Copyright (c) 2007-2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_Log_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/12   sg     Changes to bringup Diag MultiPD
12/02/08   mad     Changed signature for log_update_mask()
11/26/07   JV      Created File
===========================================================================*/

/* Initializes legacy service mapping for Diag log service */
boolean Diag_LSM_Log_Init(void);

/* clean up log service before exiting legacy service mapping layer */
boolean Diag_LSM_Log_DeInit (void);

/* updates the copy of log_mask */
boolean log_update_mask(void);

#endif /* DIAG_LSM_LOG_I_H */
