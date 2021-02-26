/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/
#include "systemcachemgr.h"
#include "qurt_timer.h"

#define  LPASS_AUDIO_CORE_SCID_ADDR         (0x1709E000)
#define  LPASS_AUDIO_CORE_SCID_VALIDBITS    (0x1F)
#define  SYS_CACHE_SUCCESS                   0

typedef struct
{
   DALSYSSyncHandle           lock;
   uint32                     currentScidCountIndex;
   uint32                     lpassCoreScidVirtualAddress;
   SysCacheMgrScidRefCount    scidCountArray[MAX_SCID_COUNT];
   uint32                     streamIdCountArray[BUSMgr_Scid_Max][MAX_STREAM_ID];

}SysCacheMgrCtxt;


SysCacheMgrCtxt   gSyscacheMgrScCtxt;

Adsppm_Status QurtGetVirtAddress(qurt_mem_region_t *pQmem_region, uint32 phyAddr, uint32 regSize, uint32 *pVirtAddr);


Adsppm_Status scpmInit(void)
{
    SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;

    memset(self, 0, sizeof(SysCacheMgrCtxt));

    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock, NULL))
    {
        return Adsppm_Status_Failed;
    }

    return Adsppm_Status_Success;
}


/**
* Description: This function is used to increment Scid count in system cache mgr's internal context
* Input: System cache Id
* Output: Adsppm_Status
*/
Adsppm_Status syscacheMgrIncrementScidCount (uint32 scid)
{
   uint32   index = 0;
   uint32   count = 0;
   boolean  matchfound = FALSE;

   SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;

   if(!self)
      return Adsppm_Status_BadMemPtr;

   adsppm_lock(self->lock);

   if (self->currentScidCountIndex >= MAX_SCID_COUNT)
   {
      adsppm_unlock(self->lock);
      return Adsppm_Status_BadParm;
   }


   for(index = 0; index < MAX_SCID_COUNT; index ++)
   {
      if(scid == self->scidCountArray[index].scid )
      {
          self->scidCountArray[index].count++;
          matchfound = TRUE;
          ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,"Count is %u incremented on scid %u at index %u",self->scidCountArray[index].count,
                                                                                          scid, index);
          break;
       }
   }

   if(matchfound == FALSE)
   {
      self->scidCountArray[self->currentScidCountIndex].scid = scid;
      self->scidCountArray[self->currentScidCountIndex].count ++;

      ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,"Match failed; Count is %u incremented on new scid %u at new index %u",self->scidCountArray[self->currentScidCountIndex].count,
                                                                                          scid, self->currentScidCountIndex);
      self->currentScidCountIndex++;
   }

   adsppm_unlock(self->lock);
   //Update internal System Cache Ctxt for new scid and activate it if count > 0

   if (syscacheMgrGetScidCount(scid, &count)== Adsppm_Status_Success)
   {
      if (count == 1)
      {
          SysCacheActDeactSCID(scid, TRUE);
      }
   }
   else
   {
       ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"Get count for Scid %u is failed",scid);
   }


   return Adsppm_Status_Success;

}

/**
* Description: This function is used to decrement Scid count in system cache mgr's internal context
* Input: System cache Id
* Output: Adsppm_Status
*/

Adsppm_Status syscacheMgrDecrementScidCount (uint32 scid)
{
   uint32   index = 0;
   uint32   count = 0;
   Adsppm_Status status = Adsppm_Status_Failed;
   boolean  matchfound = FALSE;

   SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;

   if(!self)
      return Adsppm_Status_BadMemPtr;

   adsppm_lock(self->lock);

   for(index = 0; index < MAX_SCID_COUNT; index ++)
   {
       if(scid == self->scidCountArray[index].scid )
       {
          self->scidCountArray[index].count--;
          matchfound = TRUE;
          ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,"Count is %u on decremented scid %u at index %u",self->scidCountArray[index].count,
                                                                                          scid, index);
          status = Adsppm_Status_Success;
          break;
       }
   }
   if(matchfound == FALSE)
   {
      status = Adsppm_Status_BadState;
   }

   adsppm_unlock(self->lock);

   if(syscacheMgrGetScidCount(scid, &count)== Adsppm_Status_Success)
   {
      if(count == 0)
      {
          // No more scid left. Now Deactivate.
          SysCacheActDeactSCID(scid, FALSE);
          ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO," Scid %u deactivated",scid);
      }
   }
   else
   {
      ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"Get count for Scid %u failed",scid);

   }

   return status;

}

/**
* Description: This function is used to get Scid count from system cache mgr's internal context
* Input: System cache Id, pointer to get count
* Output: Adsppm_Status
*/

Adsppm_Status syscacheMgrGetScidCount (uint32 scid, uint32 *count)
{
   uint32   index = 0;
   Adsppm_Status status = Adsppm_Status_Success;

   SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;

   if(!self)
      return Adsppm_Status_BadMemPtr;

   if(count == NULL)
   {
      status = Adsppm_Status_BadParm;
      return status;
   }

   *count = 0;

   adsppm_lock (self->lock);

   for(index = 0; index < MAX_SCID_COUNT; index ++)
   {
     if(scid == self->scidCountArray[index].scid )
     {
        *count = self->scidCountArray[index].count;
         break;
     }
   }

   adsppm_unlock (self->lock);

   return status;

}

/**
* Description: This function is used to program sub-system's internal System cache Id
* Input: System cache Id, Stream id
* Output: Adsppm_Status
*/

void syscacheMgrProgramInternalScid (uint32 scid, uint32 index)
{
  uint32 phyAddress = LPASS_AUDIO_CORE_SCID_ADDR;
  uint32 virtAddress = 0x0;
  uint32 regSize = 4;
  uint32 currentScid = 0;

  qurt_mem_region_t memRegion;

  SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;


  if((Adsppm_Status_Success == QurtGetVirtAddress(&memRegion, phyAddress, regSize, &virtAddress)) &&
                                virtAddress > 0)
  {
    self->lpassCoreScidVirtualAddress = virtAddress;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,"Virtual Add is 0x%x for LPASS core",self->lpassCoreScidVirtualAddress);

  }
  if(self->lpassCoreScidVirtualAddress != NULL)
  {
    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO_EXT,"Before reading current Scid on LPASS core");
    currentScid = (*(uint32 *)(self->lpassCoreScidVirtualAddress));
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,"After reading current Scid 0x%x on LPASS core",currentScid);
    (*(uint32 *)(self->lpassCoreScidVirtualAddress)) = (currentScid & ~LPASS_AUDIO_CORE_SCID_VALIDBITS)|(scid & LPASS_AUDIO_CORE_SCID_VALIDBITS);
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,"Programmed Scid 0x%x on LPASS core",(*(uint32 *)(self->lpassCoreScidVirtualAddress)));
  }

}

/**
* Description: This function is used to reset Scid value to 0 and deactivate scid
* Input: System cache Id, streeam index
* Output: Adsppm_Status
*/

Adsppm_Status sysCacheMgrResetScid (uint32 index, BUSMgrScidType scidType)
{
   uint32 currentScid = 0;

   SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;

   if(!self)
      return Adsppm_Status_BadMemPtr;

   adsppm_lock (self->lock);
   currentScid = self->streamIdCountArray[scidType][index];
   adsppm_unlock (self->lock);


   if (currentScid > 0)
   {

      if(scidType == BUSMgr_Scid_Internal)
      {
          syscacheMgrProgramInternalScid (0x0, index );
      }
      else if (scidType == BUSMgr_Scid_External)
      {
          SysCacheProgramSCID(0x0, index);
      }
      else
      {
          ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"Unknown scidType received  %u ",scidType);
          return Adsppm_Status_Failed;
      }

      if (syscacheMgrDecrementScidCount (currentScid) == Adsppm_Status_Success)
      {
          ADSPPM_LOG_PRINTF_1 (ADSPPM_LOG_LEVEL_ERROR,"Count decremented for scid %u",currentScid);
      }
      else
      {
          ADSPPM_LOG_PRINTF_1 (ADSPPM_LOG_LEVEL_ERROR,"Decrement count failed for scid %u",currentScid);
          return Adsppm_Status_Failed;
      }
      adsppm_lock (self->lock);
      self->streamIdCountArray[scidType][index] = 0;
      adsppm_unlock (self->lock);
   }
   else
      ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"Scid is already reset");

   return Adsppm_Status_Success;
}

/**
* Description: This function is used to program Scid and activate scid
* Input: System cache Id, streeam index, Type of address (Internal/Q6)
* Output: Adsppm_Status
*/

Adsppm_Status sysCacheMgrProgramScid (uint32 usecaseId,uint32 index, BUSMgrScidType scidType )
{
    uint32 scid = 0,  maxsize = 0;
    uint32 currentScid = 0;

    SysCacheMgrCtxt* self = &gSyscacheMgrScCtxt;

    if(!self)
       return Adsppm_Status_BadMemPtr;


    //Get SCID
    if(SysCacheGetSCID(usecaseId, &scid, &maxsize) != SYS_CACHE_SUCCESS)
    {
       return Adsppm_Status_Failed;
    }

    adsppm_lock (self->lock);
    currentScid = self->streamIdCountArray[scidType][index];
    adsppm_unlock (self->lock);

    //Is currentactivatedinternal SCID different
    if (currentScid != scid)
    {

       adsppm_lock (self->lock);
       self->streamIdCountArray[scidType][index] = scid;
       adsppm_unlock (self->lock);

       syscacheMgrIncrementScidCount (scid);

       if(scidType == BUSMgr_Scid_Internal)
       {
          syscacheMgrProgramInternalScid (scid, index);
       }
       else if (scidType == BUSMgr_Scid_External)
       {
          SysCacheProgramSCID(scid, index);
          ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,"Scid %u is programmed at %u ",scid, index);

       }
       else
       {
          ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"Unknown scidType received  %u ",scidType);
          return Adsppm_Status_Failed;
       }


       //Update internal System Cache for OLD scid and deactivate it if count == 0
       if(currentScid > 0)
       {
          syscacheMgrDecrementScidCount (currentScid);
       }
    }
    else
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,"Correct Scid %u is already programmed",currentScid);

    return Adsppm_Status_Success;
}



Adsppm_Status QurtGetVirtAddress(qurt_mem_region_t *pQmem_region, uint32 phyAddr, uint32 regSize, uint32 *pVirtAddr)
{
   Adsppm_Status status = Adsppm_Status_Success;
   qurt_mem_pool_t hwio_pool;

   memset((void *)pQmem_region,0,sizeof(qurt_mem_region_t));

   size_t   page_size = (size_t) regSize;
   qurt_mem_cache_mode_t cache_attribs =  QURT_MEM_CACHE_NONE;

   qurt_perm_t perm = (qurt_perm_t) (QURT_PERM_READ | QURT_PERM_WRITE);
   if ( Adsppm_Status_Success !=  (status = qurt_mem_map_static_query((qurt_addr_t *)pVirtAddr,
                                                                                     phyAddr,
                                                                                     page_size,
                                                                                     cache_attribs,
                                                                                     perm)))
   {
      if(Adsppm_Status_Success != (status = qurt_mem_pool_attach("ADSPPM_DRIVER_POOL", &hwio_pool)))
      {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"attach to driver pool failed");
      }
      else
      {
         qurt_mem_region_attr_t hwio_attr;

         qurt_mem_region_attr_init( &hwio_attr);
         qurt_mem_region_attr_set_cache_mode( &hwio_attr, QURT_MEM_CACHE_NONE);
         qurt_mem_region_attr_set_mapping  ( &hwio_attr, QURT_MEM_MAPPING_PHYS_CONTIGUOUS );
         qurt_mem_region_attr_set_physaddr ( &hwio_attr, phyAddr );

         //#ifdef SIM
         /*
            Without QURT_MEM_REGION_SHARED, memory is not global
            Then, qurt_mem_map_static_query() API cannot locate that in MPD build.
            Once qurt_mem_map_static_query() is fixed, this could be removed
         */
         qurt_mem_region_attr_set_type(&hwio_attr, QURT_MEM_REGION_SHARED);
         //#endif

         /* create the memory region */
         if (Adsppm_Status_Success != (status = qurt_mem_region_create(pQmem_region,
                               regSize,
                               hwio_pool,
                              &hwio_attr )))
         {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,"create mem region failed (addr 0x%x) (size 0x%x)",phyAddr, page_size);
            return status;
         }

         if (Adsppm_Status_Success != (status = qurt_mem_region_attr_get(*pQmem_region, &hwio_attr)))
         {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,"get mem region attr failed (addr 0x%x) (size 0x%x)",phyAddr, page_size);
         }
         else
         {
            unsigned int addr = 0;
            qurt_mem_region_attr_get_virtaddr(&hwio_attr, &addr);
            *pVirtAddr = (uint32) addr;
         }
      }
   }
   else
   {
      ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,"already mampped statically ph = 0x%x, virt = 0x%x",(unsigned int)phyAddr, *pVirtAddr);

   }
   return status;
}


Adsppm_Status QurtDestoryVirtAddress(qurt_mem_region_t qmem_region)
{
   Adsppm_Status status = Adsppm_Status_Success;

   if(qmem_region)
   {
      if(Adsppm_Status_Success != (status = qurt_mem_region_delete(qmem_region)))
      {
         ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"Failed to delete memory region [status:%d]",status);
      }
   }
   else
   {
      ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"no qmem_region to destory");
   }

   return status;
}

