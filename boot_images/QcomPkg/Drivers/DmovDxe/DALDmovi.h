#ifndef DALDmovi_H
#define DALDmovi_H
/*
===============================================================================

 FILE:         DALDmovi.h

 DESCRIPTION:  
   DMOV internal function and data structure declarations.

===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/06/09   an      renamed Dmovi_NotifyClient to Dmovi_MoveToNotifyQueue
09/24/09   an      added helper function Dmovi_NotifyClient
06/02/09   an      moved DMOV_LEGACY_SUPPORT into compilation files
04/29/09   an      fix compilation warnings 
03/27/09   MK      Added clock management 
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

/* ----------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------- */

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALFramework.h"

/*-----------------------------------------------------------------------------
** Constants
**---------------------------------------------------------------------------*/

/* Flush polling loop count used by dmov_domain_flush() */
#define DMOV_FLUSH_POLL_MAX_COUNT       64

/*-----------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------*/

/* Determine whether channel is active either local or remote */
#define DMOVI_CHAN_IS_ACTIVE(state) \
  (((state) >= DMOV_CHAN_STATE_RUN_NO_ACCESS) && ((state) <= DMOV_CHAN_STATE_FLUSHING_ALL))

/* Determine whether channel is active on local processor */
#define DMOVI_CHAN_IS_ACTIVE_LOCAL(state) \
  (((state) >= DMOV_CHAN_STATE_RUN) && ((state) <= DMOV_CHAN_STATE_FLUSHING_ALL))

/* ----------------------------------------------------------------------------
** Interface 
** ------------------------------------------------------------------------- */
void Dmovi_DevInit(  DmovDevCtxt * pDevCtxt );
void Dmovi_DevReset(  DmovDevCtxt * pDevCtxt );
void Dmovi_DevEnable(  DmovDevCtxt * pDevCtxt );
void Dmovi_DevDisable(  DmovDevCtxt * pDevCtxt );
DALResult Dmovi_GetClockIDs(  DmovDevCtxt * pDevCtxt );

void Dmovi_DevSave(DmovDevCtxt * pDevCtxt);
void Dmovi_DevRestore(DmovDevCtxt * pDevCtxt);
DALBOOL Dmovi_IssueTransfer(DmovDevCtxt * pDevCtxt, Dmov_ChanType *pChan);
DALResult Dmovi_QueueTransfer(DmovClientCtxt * pCtxt, 
                              const DmovTransferRequest * TransferRequest, 
                              uint32 * CommandList, 
                              DmovReqStatus * RequestStatus);
DALResult Dmovi_RequestIsValid(DmovClientCtxt * pCtxt, 
                               const DmovTransferRequest* TransferRequest, 
                               uint32 * CommandList, 
                               DmovReqStatus * RequestStatus);
void Dmovi_MoveToNotifyQueue(DmovDevCtxt * pDevCtxt, 
                              Dmov_ChanType* pChan,
                              DmovTransferQItem* pTransferDescriptor, 
                              DALBOOL bFreeTransfer);
void Dmovi_FlushQueues(DmovDevCtxt * pDevCtxt, Dmov_ChanType *pChan);
void Dmovi_GetChanError(DmovDevCtxt * pDevCtxt, Dmov_ChanType * pChan);
void Dmovi_FlushHandler(DmovDevCtxt * pDevCtxt, Dmov_ChanType *pChan,uint32 dwResult);
void Dmovi_StopHandler(DmovDevCtxt * pDevCtxt, Dmov_ChanType * pChan);
DALBOOL Dmovi_ReadResult(DmovDevCtxt * pDevCtxt, Dmov_ChanType * pChan);
void Dmovi_ChanHandler(DmovDevCtxt * pDevCtxt, Dmov_ChanType *pChan);
int Dmovi_CompareToken( void *p_Item, void *p_CompareValue);
DALBOOL Dmovi_FlushRemoteChan(DmovDevCtxt * pDevCtxt, uint32 dwChan);
void Dmovi_ClearTransferDescriptor(DmovTransferQItem * pTransferDescriptor);

#endif /*DALDmovi_H*/


