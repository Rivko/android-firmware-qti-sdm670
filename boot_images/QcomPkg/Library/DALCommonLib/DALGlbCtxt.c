/** @file DALGlbCtxt.c

  DAL Global Context

  Copyright (c) 2011,2014 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/08/14   sho     Move this implementation to DALCommonLib

=============================================================================*/

#include "DALFramework.h"
#include "DALGlbCtxt.h"
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>

// On other platforms the lock processor ID is defined as the processor 
// architecture.
// However the architecture is defined in dalconfig.h which cannot be
// included from this driver.  For the purpose of spin lock, it just needs
// to be any non-zero value
#define DALGLBCTXT_LOCK_PROCESSOR_ID            0x00EF10DA1 // UEFI DAL

// pages size and lock area size in page units
#define DALGLBCTXT_PAGE_SIZE                    0x1000
#define DALGLBCTXT_SMEM_AREA_SIZE               0x1000
#define DALGLBCTXT_CACHE_LINE_SIZE_BYTES        16

#define DALGLBCTXT_strlen(name)  ((AsciiStrLen(name)<DALGLBCTXT_GLB_HDR_NAME_LEN)?AsciiStrLen(name):(DALGLBCTXT_GLB_HDR_NAME_LEN-1))

static DALGLB_HEADER * g_shared_token_hdr = NULL;

extern EFI_STATUS _AllocGlbCtxtMem(DALSYSMemAddr *p_ctxt_addr, 
      UINT32 size);

static
void _PopulateLockHdr(DALGLB_HEADER * lock_hdr, uint32 size, 
      const char *name)
{
   //populate the global header
   lock_hdr->size = size;
   DALSYS_memset(lock_hdr->name,0,DALGLBCTXT_GLB_HDR_NAME_LEN);
   DALSYS_memscpy(lock_hdr->name,DALGLBCTXT_GLB_HDR_NAME_LEN, 
        (void *)name,DALGLBCTXT_strlen(name));
   lock_hdr->lock.lock = 0;
   lock_hdr->lock.version = 0;
}

static void GlbCtxtUnLock(void)
{
   g_shared_token_hdr->lock.lock = 0;
}

static DALResult GlbCtxtLock(void)
{
   if (g_shared_token_hdr->lock.lock == 0)
   {
      g_shared_token_hdr->lock.lock = DALGLBCTXT_LOCK_PROCESSOR_ID;
      return DAL_SUCCESS;
   }
   else
   {
      return DAL_ERROR;
   }
}

static
DALGLB_HEADER * _FindOrAllocHeader(const char *name, uint32 size_req)
{
   // initialize to the smem region start
   DALGLB_HEADER *glb_hdr = (DALGLB_HEADER *)g_shared_token_hdr;
   uint32 size_avail = DALGLBCTXT_SMEM_AREA_SIZE+DALGLBCTXT_PAGE_SIZE;

   if( NULL == glb_hdr)
      return NULL;

   // Get to next free location
   while(glb_hdr->size && size_avail)
   {
     // are we searching for specific name
     if(name && (0 == memcmp(glb_hdr->name,name,DALGLBCTXT_strlen(name))))
          return glb_hdr;

     size_avail -= glb_hdr->size;
     glb_hdr = (DALGLB_HEADER *)(((DALSYSMemAddr) glb_hdr) + glb_hdr->size);
   }
   // return null if search for existing ctxt, or if nothing
   //   available for new ctxt
   return ((name || (size_avail < size_req))?NULL:glb_hdr);
}

DALResult
DALGLBCTXT_Init()
{
   static int GlbCtxtIsInit = 0;
   EFI_STATUS status;
   DALSYSMemAddr DALSmemBaseAddr = 0;
   DALGLB_HEADER *glb_hdr = 0;

   // make sure we initialize only once
   if (GlbCtxtIsInit) return DAL_SUCCESS;
   GlbCtxtIsInit = 1;

   status = _AllocGlbCtxtMem(&DALSmemBaseAddr, 
         (DALGLBCTXT_SMEM_AREA_SIZE+DALGLBCTXT_PAGE_SIZE));
   if (EFI_SUCCESS != status)
   {
      DEBUG ((EFI_D_ERROR, 
         "Unable to locate shared memory; status=0x%x\n",
         status));

      return DAL_ERROR;
   }
   glb_hdr = (DALGLB_HEADER *)DALSmemBaseAddr;

   // Round the next avail page boundary, if not on page boundary
   //set the size of the un-aligned location to point to page boundary
   if(DALSmemBaseAddr&(DALGLBCTXT_PAGE_SIZE-1))
      glb_hdr->size = ((DALSmemBaseAddr+DALGLBCTXT_PAGE_SIZE) & (~(DALGLBCTXT_PAGE_SIZE-1))) - DALSmemBaseAddr;

    if(!g_shared_token_hdr)
    {
        g_shared_token_hdr = (DALGLB_HEADER *)
           ((DALSYSMemAddr)DALSmemBaseAddr+ glb_hdr->size);
    }

    if (DAL_SUCCESS == GlbCtxtLock())
    {
      if (0 != memcmp(g_shared_token_hdr->name,"dalspinlock",11))
      {
         // zero out memory
         DALSYS_memset((void *)g_shared_token_hdr,0,
                  DALGLBCTXT_SMEM_AREA_SIZE);

         // populate and init the dal specific region
         _PopulateLockHdr(g_shared_token_hdr,sizeof(DALGLB_HEADER),
                  "dalspinlock");

      }
      //release lock
      GlbCtxtUnLock();
      return DAL_SUCCESS;
   }
   return DAL_ERROR;
}

DALResult
DALGLBCTXT_AllocCtxt(const char * name, uint32 size_req, uint32 lock_type, void ** ctxt)
{
   // search for an existing header
   DALGLB_HEADER *   _hdr = 0;
   DALResult ret;

   ret = DALGLBCTXT_Init();
   if (DAL_SUCCESS != ret) return ret;
   
   // round size_req to cache line size
   if(size_req&(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1))
   {
      size_req = (size_req+DALGLBCTXT_CACHE_LINE_SIZE_BYTES) & 
         ~(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1);
   }

   ret  = DAL_ERROR;

   // grab the global lock to prevent other cores from accessing.
   if(DAL_SUCCESS == GlbCtxtLock())
   {
      //check to see if this previously exists
      _hdr = _FindOrAllocHeader(name,0);

      //if none exists, then allocate mem area
      if(!_hdr)
      {
         _hdr = _FindOrAllocHeader(NULL,size_req);

         //cannot allocate return error
         if(NULL == _hdr)
         {
            DEBUG ((EFI_D_ERROR, "Unable to allocate context\n"));
            goto exit_dal_alloc_ctxt;
         }

         // populate and init the dal specific region
         _PopulateLockHdr(_hdr,size_req,name);

      }
      else
      {
         // ensure the found context has the expected size
         if (_hdr->size != size_req)
         {
            DEBUG ((EFI_D_ERROR, 
               "Previously allocated context size=%d different from expected size=%d\n", 
                     _hdr->size, size_req));
            goto exit_dal_alloc_ctxt;
         }
      }

      if(ctxt) *ctxt = (void *)_hdr;

      ret = DAL_SUCCESS;

exit_dal_alloc_ctxt:
      //release this lock for other cores
      GlbCtxtUnLock();
   }

   return ret;
}

DALResult
DALGLBCTXT_FindCtxt(const char * name, void ** ctxt)
{
   // search for an existing header
   DALGLB_HEADER * _hdr = 0;
   DALResult ret;

   ret = DALGLBCTXT_Init();
   if (DAL_SUCCESS != ret) return ret;

   if(DAL_SUCCESS == GlbCtxtLock())
   {
      _hdr = _FindOrAllocHeader(name,0);

      if(ctxt) *ctxt = (void *)_hdr;

      GlbCtxtUnLock();
   }
   return (_hdr?DAL_SUCCESS:DAL_ERROR);
}
