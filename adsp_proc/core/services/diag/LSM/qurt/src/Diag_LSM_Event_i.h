#ifndef DIAG_LSM_EVENT_I_H
#define DIAG_LSM_EVENT_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                Internal Header File for Event Legacy Service Mapping

GENERAL DESCRIPTION
  
Copyright (c) 2007-2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/Diag_LSM_Event_i.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/05/12   sg     Changes to bringup Diag MultiPD
12/03/08   mad     Changed signature of event_update_mask()
11/26/07   mad     Created File
===========================================================================*/
/* Initializes legacy service mapping for Diag event service */
boolean Diag_LSM_Event_Init(void);

/* clean up event service before exiting legacy service mapping layer.
Currently does nothing, just returns TRUE */
boolean Diag_LSM_Event_DeInit(void);

/* updates the copy of event_mask */
boolean event_update_mask(void);


#endif /* DIAG_LSM_EVENT_I_H */
