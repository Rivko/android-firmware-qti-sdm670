#ifndef DIAG_LSM_MSG_I_H
#define DIAG_LSM_MSG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
EXTENDED DIAGNOSTIC MESSAGE SERVICE LEGACY MAPPING
INTERNAL HEADER FILE

GENERAL DESCRIPTION
Internal header file
  
Copyright (c) 2007-2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_Msg_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/08   mad     Changed signature of msg_update_mask()
12/03/07   mad     Created File
===========================================================================*/

/* Initializes Mapping layer for message service*/
boolean Diag_LSM_Msg_Init (void);

/* clean up before exiting legacy service mapping layer.
Does nothing as of now, just returns TRUE. */
boolean Diag_LSM_Msg_DeInit (void);

/* updates the copy of the run-time masks for messages */
boolean msg_update_mask(void);

#endif /* DIAG_LSM_MSG_I_H */
