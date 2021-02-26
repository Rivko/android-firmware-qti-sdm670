/*==============================================================================
@file  DALSYSMem.c

DALSYS memory region implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include <stdlib.h>
#include <string.h>
#include <qurt.h>
#include "stringl/stringl.h"

/*==============================================================================
          Structures
 ==============================================================================*/
typedef struct _DALSYSMemObj _DALSYSMemObj;
struct _DALSYSMemObj
{
   INHERIT_DALSYS_BASE_OBJ();
   qurt_mem_region_t qmem_region;
   DALSYSMemInfoEx memInfo;
};

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS
==============================================================================*/
extern DALResult DALSYS_MemAllocQurtRegion( const DALSYSMemReq *pMemReq,
      DALSYSMemInfoEx *pMemInfo, qurt_mem_region_t *p_qmem_region );

extern void DALSYS_MemFreeQurtRegion( DALSYSMemInfoEx const *pMemInfo, 
      qurt_mem_region_t qmem_region );

/*============================================================================n
      Constants and Macros
==============================================================================*/

/*==============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static _DALSYSMemObj *
AllocAndInitMemObj( _DALSYSMemObj *pMemObj )
{
   if (NULL == pMemObj)
   {
      if (DAL_SUCCESS != DALSYS_Malloc (DALSYS_MEM_OBJ_SIZE_BYTES, 
               (void **)&pMemObj))
      {
         // ensure the advertised memory object size is not smaller
         // than the actual size
         DALSYSCMN_COMPILER_ASSERT(
               (sizeof(_DALSYSMemObj) > DALSYS_MEM_OBJ_SIZE_BYTES));
         return NULL;
      }
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
      DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(pMemObj->dwObjInfo); 
   }
   else
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
   }

   DALSYSCMN_SET_OBJECT_ID(pMemObj->dwObjInfo, DALSYSCMN_MEM_OBJECT_ID);
   DALSYSCMN_RESET_MEM_COPY_BIT(pMemObj->dwObjInfo);
   pMemObj->thisVirtualAddr = (DALSYSMemAddr)pMemObj;
   return pMemObj;
}

/*------------------------------------------------------------------------------
* Users must invoke this API to construct a Mem Object. Users have the option
  of pre-allocating memory: required for this object, or rely upon dynamic
  memory allocation.
 -----------------------------------------------------------------------------*/
DALResult
DALSYS_MemRegionAllocEx( DALSYSMemHandle *phMem, const DALSYSMemReq *pMemReq, 
      DALSYSMemInfoEx *pMemInfo )
{
   _DALSYSMemObj *pMemObj;
   *phMem = NULL;

   pMemObj = AllocAndInitMemObj((_DALSYSMemObj *)pMemReq->pObj);

   if (!pMemReq->prealloc)
   {
      DALResult result;

      // create underlying kernel memory region
      // QURT will check and update the virtual addr, phys addr, and size
      // to ensure they match the alignment and size of memory mappings
      result = DALSYS_MemAllocQurtRegion( pMemReq, &pMemObj->memInfo, 
            &pMemObj->qmem_region );
      if (DAL_SUCCESS != result)
      {
         if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pMemObj->dwObjInfo))
         {
            DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
            DALSYS_Free(pMemObj);
         }
         else
         {
            DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
         }
         return result;
      }
   }
   else
   {
      pMemObj->qmem_region = 0;
      pMemObj->memInfo = pMemReq->memInfo;
   }

   *phMem = (DALSYSMemHandle)pMemObj;
   if (pMemInfo)
   {
      *pMemInfo = pMemObj->memInfo;
   }
   return DAL_SUCCESS;
}

DALResult
DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr virtualAddr,
                      DALSYSMemAddr physicalAddr, uint32 size, 
                      DALSYSMemHandle *phMem, DALSYSMemObj *pObj)
{
   DALResult ret;
   DALSYSMemReq memReq;
   memReq.memInfo.physicalAddr = (DALSYSPhyAddr)physicalAddr;
   memReq.memInfo.virtualAddr = virtualAddr;
   memReq.memInfo.size = (DALSYSMemAddr)size;
   memReq.prealloc = (dwAttribs & DALSYS_MEM_PROPS_PHYS_PREALLOC);
   memReq.pObj = pObj;

   // determine permission
   switch (dwAttribs & DALSYS_MEM_PROPS_EXT_PERM_MASK)
   {
      case DALSYS_MEM_PROPS_EXT_PERM_RD:
         memReq.memInfo.permission = DALSYS_MEM_PERM_R;
         break;
      case DALSYS_MEM_PROPS_EXT_PERM_FULL:
         memReq.memInfo.permission = DALSYS_MEM_PERM_RW;
         break;
      default:
      case DALSYS_MEM_PROPS_EXT_PERM_NONE:
         memReq.memInfo.permission = DALSYS_MEM_PERM_NONE;
         break;
   }

   // determine share type
   if (dwAttribs & DALSYS_MEM_PROPS_ARCH_HW_NORM_MEM_SHARE)
   {
      memReq.memInfo.shareType = DALSYS_MEM_SHARE_ALLOWED;
   }
   else 
   { 
      memReq.memInfo.shareType = DALSYS_MEM_SHARE_NOT_ALLOWED; 
   } 

   // determine cache attributes
   if (dwAttribs & DALSYS_MEM_PROPS_EXT)
   {
      if (dwAttribs & DALSYS_MEM_PROPS_EXT_NORM_MEM)
      {
         // Hexagon does not support independent configuration of outer 
         // and inner cache types
         memReq.memInfo.cacheType = DALSYS_MEM_CACHE_NONE;
      }
      else
      {
         switch (dwAttribs & DALSYS_MEM_PROPS_EXT_ATTRIBS_MASK)
         {
            case DALSYS_MEM_PROPS_EXT_OUTER_INNER_UNCACHED:
               memReq.memInfo.cacheType = DALSYS_MEM_CACHE_NONE;  
               break;

            case DALSYS_MEM_PROPS_EXT_SHARED_DEVICE:
               memReq.memInfo.shareType = DALSYS_MEM_SHARE_ALLOWED;
            case DALSYS_MEM_PROPS_EXT_STRONGLY_ORDERED:
            case DALSYS_MEM_PROPS_EXT_NON_SHARED_DEVICE:       
               memReq.memInfo.cacheType = DALSYS_MEM_CACHE_DEVICE;
               break;

            case DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_NO_ALLOC:
            case DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_ALLOC:
               memReq.memInfo.cacheType = DALSYS_MEM_CACHE_WRITEBACK;
               break;

            case DALSYS_MEM_PROPS_EXT_OUTER_INNER_WT_NO_ALLOC:
               memReq.memInfo.cacheType = DALSYS_MEM_CACHE_WRITETHROUGH;
               break;

            default:
               memReq.memInfo.cacheType = DALSYS_MEM_CACHE_DEVICE;
               break;  
         }
      }
   }
   else
   {
      memReq.memInfo.cacheType = DALSYS_MEM_CACHE_DEVICE;
   }

   ret = DALSYS_MemRegionAllocEx( phMem, &memReq, NULL);
   if (DAL_NO_ERROR_RETURNED(ret))
   {
      _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)(*phMem);
      DALSYSCMN_SET_ATTRIB( pMemObj->dwObjInfo, dwAttribs );
   }
   return ret;
}

DALResult
DALSYS_MemInfoEx(DALSYSMemHandle hMem, DALSYSMemInfoEx *pMemInfo)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);
    if(pMemObj)
    {
       *pMemInfo = pMemObj->memInfo;
       return DAL_SUCCESS;
    }
    DALSYS_LOG_ERROR_EVENT("Failed- pMemInfo:0x%x", pMemInfo);
    return DAL_ERROR;
} 

DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *) DALSYSCMN_HANDLE_TO_OBJECT(hMem);
    if(pMemObj)
    {
        pMemInfo->VirtualAddr  = pMemObj->memInfo.virtualAddr; 
        pMemInfo->PhysicalAddr = (DALSYSMemAddr)pMemObj->memInfo.physicalAddr;
        pMemInfo->dwLen        = (uint32)pMemObj->memInfo.size;
        pMemInfo->dwMappedLen  = (uint32)pMemObj->memInfo.size;
        DALSYSCMN_GET_ATTRIB( pMemObj->dwObjInfo, pMemInfo->dwProps );
        return DAL_SUCCESS;
    }
    DALSYS_LOG_ERROR_EVENT("Failed- pMemInfo:0x%x", pMemInfo);
    return DAL_ERROR;
} 

DALSYSMemHandle
DALSYSCMN_MemObjCopy(DALSYSMemObj * pInObj, DALSYSMemObj * pOutObj)
{
   _DALSYSMemObj * pInMemObj = (_DALSYSMemObj *)pInObj;
   _DALSYSMemObj * pOutMemObj;

   if (!pInObj) return NULL;

   pOutMemObj = AllocAndInitMemObj( (_DALSYSMemObj *)pOutObj );
   if (!pOutMemObj) return NULL;

   DALSYSCMN_SET_MEM_COPY_BIT(pOutMemObj->dwObjInfo);
   pOutMemObj->qmem_region = pInMemObj->qmem_region;
   pOutMemObj->memInfo = pInMemObj->memInfo;
   return (DALSYSMemHandle)DALSYSCMN_HANDLE_SET_COPY(pOutMemObj);
}

DALResult DALSYS_DestroyMemObject(DALSYSMemHandle hMem)
{
   _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);

   if( !pMemObj || (DALSYSCMN_MEM_OBJECT_ID != 
            DALSYSCMN_GET_OBJECT_ID(pMemObj->dwObjInfo))) return DAL_ERROR;

   if(!DALSYSCMN_HANDLE_IS_COPY(hMem))
   {
      DALSYS_MemFreeQurtRegion( &pMemObj->memInfo, pMemObj->qmem_region );
   }

   /*----------------------------------------------------------------------
    If DALSYS Mem object dynamically alloced free structure memory,
    otherwise reset the memory area to ZERO
    ----------------------------------------------------------------------*/
   if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pMemObj->dwObjInfo))
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
      return DALSYS_Free(pMemObj);
   }
   else
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
      return DAL_SUCCESS;
   }
}

DALResult
DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen)
{
   qurt_mem_cache_op_t opCode;
    if (0 == dwLen)
    {
        DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR,
            "line length cannot be empty in DALSYS_CacheCommand- VirtualAddr:0x%x",
            VirtualAddr);
        return DAL_ERROR;
    }
     
    switch(CacheCmd)
    {
      case DALSYS_CACHE_CMD_INVALIDATE:
         opCode = QURT_MEM_CACHE_INVALIDATE;
         break;

      case DALSYS_CACHE_CMD_FLUSH:
         opCode = QURT_MEM_CACHE_FLUSH_INVALIDATE;
         break;

      case DALSYS_CACHE_CMD_CLEAN:
         opCode = QURT_MEM_CACHE_FLUSH;
         break;

      default: 
        DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR,
            "Unsupported cache command 0x%x in DALSYS_CacheCommand- VirtualAddr:0x%x",
            CacheCmd, VirtualAddr);
         return DAL_ERROR;
   }

   if (QURT_EOK == qurt_mem_cache_clean( (qurt_addr_t)VirtualAddr, 
            (qurt_size_t)dwLen, opCode, QURT_MEM_DCACHE ))
   {
      return DAL_SUCCESS;
   }
   else
   {
      DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR,
         "qurt_mem_cache_clean failed in DALSYS_CacheCommand- VirtualAddr:0x%x",
            VirtualAddr);
      return DAL_ERROR;
   }
}

DALResult
DALSYS_Malloc(uint32 dwSize, void **ppMem)
{
    if (0 == dwSize)
    {
        //treat zero size as fatal error
        DALSYS_LOG_FATAL_EVENT("DALSYS_Malloc: size cannot be zero");
        return DAL_ERROR;
    }

    *ppMem = qurt_malloc(dwSize);
    if (NULL != *ppMem)
    {
        return DAL_SUCCESS;
    }
    // under RTOS environments treat this as fatal error
    DALSYS_LOG_FATAL_EVENT("OUT OF MEMORY");
    return DAL_ERROR;
}

DALResult
DALSYS_Free(void *pMem)
{
    qurt_free(pMem);
    return DAL_SUCCESS;
}

uint32 _DALSYS_memscpy(void * pDest, uint32 iDestSz, 
      const void * pSrc, uint32 iSrcSize)
{
   return memscpy(pDest, iDestSz, pSrc, iSrcSize);
}

int DalSys_CopyFromUser(int client_handle, void *dest, const void *src, unsigned len)
{
    return qurt_qdi_copy_from_user( client_handle,  dest,   src,  len);

}

int DalSys_CopyToUser(int client_handle, void *dest, const void *src, unsigned len)
{
    return qurt_qdi_copy_to_user( client_handle,  dest,   src,  len);
}
