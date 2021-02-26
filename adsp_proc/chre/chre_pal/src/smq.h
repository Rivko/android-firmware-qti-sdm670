/*======================================================

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
=====================================================*/
#ifndef _SMQ_H_
#define _SMQ_H_

#include "AEEStdErr.h"
#include "AEEstd.h"
#include "AEEIEnv.h"
#include "AEEIMemMap.h"
#include "AEEatomic.h"
#include "AEEICritSect.h"
#include "AEECCritSect.h"

#define SM_BLOCKSIZE 128

#define SMQ_ALIGN_PTR(x, a) ((byte*)(((uint32)(x) + (a)-1) & ~((a)-1)))
#define SMQ_ROUND_UP(x, a) (((x) + ((a)-1)) & ~((a)-1))

/*==============================================================================
                       Shared Memory Queue
==============================================================================*/

typedef struct SMQBlockMap {
   IEnv* piEnv;
   uint32 iRead;
   uint32 nBlocks;
   byte* pMap;
} SMQBlockMap;

typedef struct SMQNode {
   uint16 iBlock;
   uint16 nBlocks;
} SMQNode;

typedef struct SMQOutState {
   uint32 iSentW;
   uint32 iFreeR;
} SMQOutState;

typedef struct SMQOut {
   SMQOutState s;
   SMQNode Sent[1];
} SMQOut;

typedef struct SMQInState {
   uint32 iSentR;
   uint32 iFreeW;
} SMQInState;

typedef struct SMQIn {
   SMQInState s;
   SMQNode Free[1];
} SMQIn;

typedef struct SMQ {
   IEnv* piEnv;
   uint32 dwRefs;

   ICritSect* pics;
   
   IMemMap* pimm;
   SMQOut* pOut;
   SMQIn* pIn;
   byte* pBlocks;
   SMQBlockMap smqbm;
   uint32 nBlocks;
} SMQ;

static __inline int SMQBlockMap_Get(SMQBlockMap* me, uint32* piBlock, uint32 n)
{
   uint32 iStart;

   iStart = me->iRead;

   /* optimized for one block gets */
   if (1 == n) {
      
      do {
         if (0 == me->pMap[me->iRead]) {
            *piBlock = me->iRead;
            me->pMap[me->iRead] = 1;
            return AEE_SUCCESS;
         }
      } while (iStart != (me->iRead = (++me->iRead) % me->nBlocks));
      
   } else {
      uint32 iMark = me->nBlocks;
      uint32 nFound = 0;

      do {
         if (0 == me->pMap[me->iRead]) {
            if (iMark > me->iRead) {
               iMark = me->iRead;
               iStart = me->iRead;
               nFound = 0;
            }

            nFound++;
            if (nFound == n) {
               uint32 i;
                  
               *piBlock = iMark;
               for (i = 0; i < n; i++) {
                  me->pMap[iMark + i] = (byte)(n - i);
               }
               return AEE_SUCCESS;
            }
         } else {
            nFound = 0;
            me->iRead += me->pMap[me->iRead] - 1;
            iMark = me->nBlocks;
         }
      } while (iStart != (me->iRead = (++me->iRead) % me->nBlocks));
      
   }

   return AEE_ENOMEMORY;
}

static __inline void SMQBlockMap_Put(SMQBlockMap* me, uint32 i)
{
   uint32 nBlocks;
   
   nBlocks = me->pMap[i];
   while (nBlocks--) {
      me->pMap[i] = 0;
      i++;
   }
}

static __inline int SMQBlockMap_QuerySize(SMQBlockMap* me, uint32 i)
{
   byte nBlocks = me->pMap[i];
   return nBlocks;
}

static __inline int SMQBlockMap_New(SMQBlockMap* me, IEnv* piEnv, uint32 nBlocks)
{
   int nErr;

   if (1 >= nBlocks) {
      return AEE_ENOMEMORY;
   }
   
   STD_ZEROAT(me);

   nErr = IEnv_ErrRealloc(piEnv, nBlocks, (void**)&me->pMap);
   if (AEE_SUCCESS != nErr) {
      return nErr;
   }
   
   IEnv_AddRef(piEnv);
   me->piEnv = piEnv;
   me->iRead = 0;
   me->nBlocks = nBlocks - 1; /* due to send/free index overlap */

   return AEE_SUCCESS;
}

static __inline void SMQBlockMap_Delete(SMQBlockMap* me)
{
   IENV_FREEIF(me->piEnv, me->pMap);
   IQI_RELEASEIF(me->piEnv);
}

static __inline int SMQ_Free(SMQ* me, void* pData)
{
   SMQNode Node;
   uint32 iBlock;

   ICritSect_Enter(me->pics);
   
   /* calc and validate block index */
   iBlock = ((byte*)pData - me->pBlocks) / SM_BLOCKSIZE;
   if (iBlock >= me->nBlocks) {
      ICritSect_Leave(me->pics);
      return AEE_EBADPARM;
   }

   /* write block index */
   Node.iBlock = (uint16)iBlock;
   Node.nBlocks = 0;
   *((SMQNode*)(me->pIn->Free + me->pIn->s.iFreeW)) = Node;

   /* update write index */
   me->pIn->s.iFreeW = (me->pIn->s.iFreeW + 1) % me->nBlocks;

   ICritSect_Leave(me->pics);
   
   return AEE_SUCCESS;
}

static __inline int SMQ_Receive(SMQ* me, void** pp, int* pnSize, boolean* pbMore)
{
   SMQNode* pNode;
   int nErr = AEE_SUCCESS;

   ICritSect_Enter(me->pics);
   
   /* is empty? */
   if (me->pIn->s.iSentR == me->pOut->s.iSentW) {
      nErr = AEE_ENOMORE;
      goto bail;
   }

   /* validate block index */
   pNode = (SMQNode*)(me->pOut->Sent + me->pIn->s.iSentR);
   if (pNode->iBlock >= me->nBlocks) {
      nErr = AEE_EBADPARM;
      goto bail;
   }

   /* increment read index */
   me->pIn->s.iSentR = (me->pIn->s.iSentR + 1) % me->nBlocks;

   *pp = me->pBlocks + (pNode->iBlock * SM_BLOCKSIZE);
   *pnSize = SM_BLOCKSIZE * pNode->nBlocks;

bail:
   if (me->pIn->s.iSentR != me->pOut->s.iSentW) {
      *pbMore = TRUE;
   } else {
      *pbMore = FALSE;
   }

   ICritSect_Leave(me->pics);

   return nErr;
}

static __inline void SMQ_Abort(SMQ* me, void* pData)
{
   int iBlock;
   
   /* get block index and free */
   iBlock = ((byte*)pData - me->pBlocks) / SM_BLOCKSIZE;

   ICritSect_Enter(me->pics);
   SMQBlockMap_Put(&me->smqbm, iBlock);
   ICritSect_Leave(me->pics);
}

static __inline int SMQ_Alloc_Send(SMQ* me, const byte* pcb, int nSize)
{
   void* pv = 0;
   int nBlocks;
   uint32 iBlock = 0;
   int nErr = AEE_SUCCESS;
   
   ICritSect_Enter(me->pics);
   
   /* empty? */
   while (me->pOut->s.iFreeR != me->pIn->s.iFreeW) {
      SMQNode* pNode;
      
      /* validate block index */
      pNode = (SMQNode*)(me->pIn->Free + me->pOut->s.iFreeR);
      if (pNode->iBlock >= me->nBlocks) {
         nErr = AEE_EBADPARM;
         goto bail;
      }

      /* update read index */
      me->pOut->s.iFreeR = (me->pOut->s.iFreeR + 1) % me->nBlocks;

      SMQBlockMap_Put(&me->smqbm, pNode->iBlock);
   }

   nBlocks = (nSize + SM_BLOCKSIZE - 1) / SM_BLOCKSIZE;
   ERR_BAIL(SMQBlockMap_Get(&me->smqbm, &iBlock, nBlocks));
   
   pv = me->pBlocks + (SM_BLOCKSIZE * iBlock);

   /* copy data */
   std_memmove(pv, pcb, nSize);

   ((SMQNode*)(me->pOut->Sent + me->pOut->s.iSentW))->iBlock = (uint16)iBlock;
   ((SMQNode*)(me->pOut->Sent + me->pOut->s.iSentW))->nBlocks = (uint16)nBlocks;

   /* update write index */
   me->pOut->s.iSentW = (me->pOut->s.iSentW + 1) % me->nBlocks;
   
bail:
   if (AEE_SUCCESS != nErr) {
      if (0 != pv) {
         SMQBlockMap_Put(&me->smqbm, iBlock);
      }
   }
   ICritSect_Leave(me->pics);   
   return nErr;
}

static __inline uint32 SMQ_AddRef(SMQ* me)
{
   return atomic_Add(&me->dwRefs, 1);
}

static __inline void SMQ_Dtor(SMQ* me)
{
   SMQBlockMap_Delete(&me->smqbm);
   IQI_RELEASEIF(me->pimm);
   IQI_RELEASEIF(me->pics);
   IQI_RELEASEIF(me->piEnv);
}

static __inline void SMQ_Delete(SMQ* me)
{
      IEnv* piEnv = me->piEnv;
      IEnv_AddRef(me->piEnv);
      
      SMQ_Dtor(me);

      IENV_FREEIF(piEnv, me);
      IEnv_Release(piEnv);
}

static __inline uint32 SMQ_Release(SMQ* me)
{
   uint32 dwRefs = atomic_Add(&me->dwRefs, -1);

   if (0 == dwRefs) {
      SMQ_Delete(me);
   }
   
   return dwRefs;
}

static __inline int SMQ_New(IEnv* piEnv, IMemMap* pimm, boolean bOut, SMQ** pp)
{
   SMQ* me = 0;
   MemMapInfo mmi;
   int nBlocks;
   byte* pbOrig;
   byte* pb;
   int nSize;
   uint32 i;
   int nErr;

   ERR_RETFARF(IENV_ERRMALLOCREC(piEnv, SMQ, &me), "Malloc me failed");

   IEnv_CreateInstance(piEnv, AEECLSID_CCritSect, (void**)&me->pics);
   
   IEnv_AddRef(piEnv);
   me->piEnv = piEnv;

   me->dwRefs = 1;

   IMemMap_AddRef(pimm);
   me->pimm = pimm;

   (void)IMemMap_GetInfo(pimm, &mmi);

   pbOrig = (byte*)mmi.ulMapStart;
   nSize = mmi.dwMapSize;
   
   /* need to make this more cache friendly, maybe defined by creator */
   pb = pbOrig;
   pb = SMQ_ALIGN_PTR(pb, 8);
   nSize -= pb - (byte*)pbOrig;
   nBlocks = (int)((nSize - sizeof(SMQOutState) - sizeof(SMQInState)) /
                   (SM_BLOCKSIZE + sizeof(SMQNode) * 2));
   if (0 >= nBlocks) {
      nErr = AEE_ENOMEMORY;
      goto bail;
   }

   me->pBlocks = pb; // blocks first so they retain 64 bit alignment
   me->nBlocks = nBlocks;
   pb += nBlocks * SM_BLOCKSIZE;
   me->pOut = (SMQOut*)pb;
   pb += sizeof(SMQOutState) + (nBlocks * sizeof(SMQNode));
   me->pIn = (SMQIn*)pb;

   me->pOut->s.iSentW = 0;
   me->pOut->s.iFreeR = 0;
   me->pIn->s.iSentR = 0;
   me->pIn->s.iFreeW = 0;
   for (i = 0; i < me->nBlocks; i++) {
      (me->pOut->Sent + i)->iBlock = 0xFFFF;
   }
   for (i = 0; i < me->nBlocks; i++) {
      (me->pIn->Free + i)->iBlock = 0xFFFF;
   }

   if (bOut) {
      SMQBlockMap_Delete(&me->smqbm);
      nErr = SMQBlockMap_New(&me->smqbm, piEnv, me->nBlocks);
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }
   }

   *pp = me;

bail:
   if (AEE_SUCCESS != nErr) {
      SMQ_Delete(me);
   }
   return nErr;
}

#endif // _SMQ_H_
