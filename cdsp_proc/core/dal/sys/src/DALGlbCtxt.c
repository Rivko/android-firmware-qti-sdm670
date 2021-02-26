/*==============================================================================
@file  DALSYSWorkLoop_rex.c

DALSYS Global Context implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "stringl/stringl.h"
#include "qurt.h"

#include "dalconfig.h"
#include "DALSysInt.h"
#include "DALFramework.h"
#include "DALGlbCtxt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALSysCmn.h"

#include "DDIChipInfo.h"
#include "DDIHWIO.h"

#include "msmhwiobase.h"
#include "DALHwio.h"
#include "HALhwio.h"
#include "smem.h"

// pages size and lock area size in page units
#define DALGLBCTXT_PAGE_SIZE                    0x1000
#define DALGLBCTXT_SMEM_AREA_SIZE               0x1000
#define DALGLBCTXT_CACHE_LINE_SIZE_BYTES        16
#define DALGLBCTXT_SLEEP_MS                     1
#define DALGLBCTXT_MAX_SLEEP_MS                 500
#define DALGLBCTXT_strlen(name) ((strlen(name)<DALGLBCTXT_GLB_HDR_NAME_LEN)?strlen(name):(DALGLBCTXT_GLB_HDR_NAME_LEN-1))

uint8  *HWMutexBase        = NULL;
static DALGLB_HEADER * g_shared_token_hdr = NULL;
static DALSYSEventHandle hTimeoutEvent    = NULL;
static DALSYSSyncHandle  hGlbCtxtSync     = NULL;
static uint32 HWMutexId    = 0;
static int GlbCtxtIsInit   = 0;
static DALResult (*InterProcSpinLock)(void);
static void (*InterProcSpinUnLock)(void);

static inline
uint32 _AllocGlbCtxtMem(void)
{
   // Add page size so as to always align it to page start
   return (uint32) smem_alloc(SMEM_DAL_AREA,(DALGLBCTXT_SMEM_AREA_SIZE+DALGLBCTXT_PAGE_SIZE));
}

static
void _LockYield(int times)
{
   if(times)
   {
      DALSYSEventHandle hEvent[] = {hTimeoutEvent};
      uint32 dwIdx = 0;
      DALSYS_EventMultipleWait(hEvent,1,times*1000,&dwIdx);
   }
}

static
void _PopulateLockHdr(DALGLB_HEADER * lock_hdr, uint32 size, const char *name )
{
   //populate the global header
   lock_hdr->size = size;
   memset(lock_hdr->name,0,DALGLBCTXT_GLB_HDR_NAME_LEN);
   strlcpy(lock_hdr->name,name,DALGLBCTXT_GLB_HDR_NAME_LEN);
   lock_hdr->lock.lock = 0;
   lock_hdr->lock.version = 0;
}

static
DALGLB_HEADER * _FindOrAllocHeader(const char *name, uint32 size_req)
{
   // initialize to the smem region start
   DALGLB_HEADER *glb_hdr = g_shared_token_hdr;
   uint32 size_avail = DALGLBCTXT_SMEM_AREA_SIZE;

   if( NULL == glb_hdr)
      return NULL;

   // Get to next free location
   while(glb_hdr->size && size_avail)
   {
     // are we searching for specific name
     if(name && (0 == memcmp(glb_hdr->name,name,DALGLBCTXT_strlen(name))))
          return glb_hdr;

     size_avail -= glb_hdr->size;
     glb_hdr = (DALGLB_HEADER *)(((uint32) glb_hdr) + glb_hdr->size);
   }
   // return null if search for existing ctxt, or if nothing
   //   available for new ctxt
   return ((name || (size_avail < size_req))?NULL:glb_hdr);
}

extern uint32 DALFW_LockedGetW(volatile uint32 *pTarget);
extern DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget, uint32 value);

static void InterProcSWSpinUnLock(void)
{
   // TODO: below is not a correct implementation of atomic unlock, 
   // but a temporary workaround for a HW issue
   do
   {
      DALFW_LockedGetW((void *)&(g_shared_token_hdr->lock.lock));
      DALFW_LockedConditionalSetW((void *)&(g_shared_token_hdr->lock.lock), 0);
   }
   while (0 != g_shared_token_hdr->lock.lock);
}

static DALResult InterProcSWSpinLock(void)
{
   // TODO: below is not a correct implementation of atomic lock, 
   // but a temporary workaround for a HW issue
   int waitTime = 0;
   while (1)
   {
      while (g_shared_token_hdr->lock.lock == 0)
      {
         DALFW_LockedGetW((void *)&(g_shared_token_hdr->lock.lock));
         DALFW_LockedConditionalSetW((void *)&(g_shared_token_hdr->lock.lock), 
            DALGLBCTXT_SPINLOCK_PID);
         if (DALGLBCTXT_SPINLOCK_PID == g_shared_token_hdr->lock.lock)
         {
            return DAL_SUCCESS;
         }
      }     
      
      if (waitTime > DALGLBCTXT_MAX_SLEEP_MS)
      {
         // check for time-out
         return DAL_ERROR;
      }

      // sleep and retry
      waitTime += DALGLBCTXT_SLEEP_MS;
      _LockYield(DALGLBCTXT_SLEEP_MS);
  }
  return DAL_SUCCESS;
}

static void InterProcHWSpinUnLock(void)
{
   // TODO: once the capability to use HW spinlock is available
   // on all processors, there is no need to unlock SW spinlock before
   // HW spinlock
   InterProcSWSpinUnLock();

   /* Write '0' to the hardware mutex to unlock */
   HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, 0 );
}

static DALResult InterProcHWSpinLock(void)
{
   int waitTime = 0;
   DALResult ret;

   /* Attempt to write PID */
   HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, DALGLBCTXT_SPINLOCK_PID );

   while (HWIO_INI( TCSR_MUTEX_REG_n, HWMutexId) != 
          DALGLBCTXT_SPINLOCK_PID )
   {
      // unable to acquire spinlock
      if (waitTime > DALGLBCTXT_MAX_SLEEP_MS)
      {
         // check for time-out
         return DAL_ERROR;
      }
      waitTime += DALGLBCTXT_SLEEP_MS;
      _LockYield(DALGLBCTXT_SLEEP_MS);

      /* Attempt to write PID again */
      HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, DALGLBCTXT_SPINLOCK_PID );
   }

   // lock SW spinlock
   // TODO: once the capability to use HW spinlock is available
   // on all processors, there is no need to lock SW spinlock after
   // HW spinlock
   ret = InterProcSWSpinLock();
   if (DAL_SUCCESS != ret)
   {
      HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, 0 );
   }
   return ret;
}

static void InterProcSpinInit(void)
{
   DalDeviceHandle * hChipinfo;
   DalDeviceHandle * hHWIO;
   DalChipInfoFamilyType family;
   DalChipInfoVersionType version;
   DALResult ret;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar glbCtxtProp;

   // determine if HW spin lock can be used based on chip info
   // this check is unnecessary if all chipsets for this PW are using one type of spinlock
   ret = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipinfo);
   if (DAL_SUCCESS != ret || NULL == hChipinfo)
   {
      DALSYS_LOG_FATAL_EVENT("Failed to attach chipinfo device");
      return;
   }

   DalChipInfo_GetChipFamily(hChipinfo, &family);
   DalChipInfo_GetChipVersion(hChipinfo, &version);

   if ((DALCHIPINFO_FAMILY_MDM9x25 == family && version < 0x00020001) ||
       (DALCHIPINFO_FAMILY_MSM8974 == family && version < 0x00020000))
   {
      InterProcSpinLock = InterProcSWSpinLock;
      InterProcSpinUnLock = InterProcSWSpinUnLock;
      DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_WARNING, 
         "InterProcSpinInit: using SW spin lock");
      return;
   }

   // get the HW mutex index
   ret = DALSYS_GetDALPropertyHandle(0,hProp);
   if (DAL_SUCCESS != ret)
   {
      DALSYS_LOG_FATAL_EVENT("Failed to get prop handle");
      return;
   }

   if (DAL_SUCCESS != DALSYS_GetPropertyValue(hProp,"GlbCtxtHWMutexNumber",0,&glbCtxtProp))
   {
      // if HW mutex number is not defined then SW spin lock is used
      InterProcSpinLock = InterProcSWSpinLock;
      InterProcSpinUnLock = InterProcSWSpinUnLock;
      DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_WARNING, 
         "InterProcSpinInit: GlbCtxtHWMutexNumber property is not defined");
      return;
   }

   HWMutexId = glbCtxtProp.Val.dwVal;
   InterProcSpinLock = InterProcHWSpinLock;
   InterProcSpinUnLock = InterProcHWSpinUnLock;

   // map memory region for HW mutex HWIO
   ret = DAL_DeviceAttach(DALDEVICEID_HWIO, &hHWIO);
   if (DAL_SUCCESS != ret || NULL == hHWIO)
   {
      DALSYS_LOG_FATAL_EVENT("Failed to attach HWIO device");
      return;
   }

   ret = DalHWIO_MapRegion(hHWIO, HW_MUTEX_BASE_NAME, &HWMutexBase);
   if (DAL_SUCCESS != ret || NULL == HWMutexBase)
   {
      DALSYS_LOG_FATAL_EVENT("Failed to map region");
      return;
   }
}

static DALResult GlbCtxtLock(void)
{
   DALResult ret;

   // lock SW mutex
   DALSYS_SyncEnter(hGlbCtxtSync);

   // lock inter-processor spinlock
   ret = InterProcSpinLock();
   if (DAL_SUCCESS != ret)
   {
      DALSYS_SyncLeave(hGlbCtxtSync);
   }
   return ret;
}

static void GlbCtxtUnLock(void)
{
   // unlock inter-processor spinlock
   InterProcSpinUnLock();
  
   // unlock SW mutex
   DALSYS_SyncLeave(hGlbCtxtSync);
}

DALResult
DALGLBCTXT_Init()
{
   DALResult ret          = DAL_ERROR;
   uint32 DALSmemBaseAddr = 0;
   DALGLB_HEADER *glb_hdr = 0;

   if(!hGlbCtxtSync)
   {
      DALSYSSyncHandle hSync;
      DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,&hSync,NULL);

      if (!hSync)
      { 
         DALSYS_LOG_ERROR_EVENT("Cannot create sync object");
         return DAL_ERROR;
      }

      if ((uint32)hSync != DALFW_LockedCompareExchangeW((volatile uint32 *)&hGlbCtxtSync, 0, 
               (uint32)hSync))
      {
         // some context has already updated hGlbCtxtSync
         DALSYS_DestroyObject(hSync);
      }
   }

   DALSYS_SyncEnter(hGlbCtxtSync);

   if (GlbCtxtIsInit)
   {
      // no re-initialization if last run was successful
      ret = DAL_SUCCESS;
      goto exit_dalglbctxt_init;
   }

   DALSmemBaseAddr = _AllocGlbCtxtMem();
   glb_hdr = (DALGLB_HEADER *)DALSmemBaseAddr;
   
   // return DAL_ERROR if we cannot alloc
   if(0 == DALSmemBaseAddr)
      goto exit_dalglbctxt_init;

   //Create a timeout event to be used for wait
   if( 0 == hTimeoutEvent)
      DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,&hTimeoutEvent,0);

   // Round the next avail page boundary, if not on page boundary
   //set the size of the un-aligned location to point to page boundary
   if(DALSmemBaseAddr&(DALGLBCTXT_PAGE_SIZE-1))
      glb_hdr->size = ((DALSmemBaseAddr+DALGLBCTXT_PAGE_SIZE) & 
            (~(DALGLBCTXT_PAGE_SIZE-1))) - DALSmemBaseAddr;

   if(!g_shared_token_hdr)
   {
      g_shared_token_hdr = (DALGLB_HEADER *)((uint32)DALSmemBaseAddr+ glb_hdr->size);
   }

   // initialize interprocessor spin-lock
   InterProcSpinInit();

   //check to see if the dal global spin lock should be created or not
   if(DAL_SUCCESS == InterProcSpinLock())
   {
      if(0 != memcmp(g_shared_token_hdr->name,"dalspinlock",11))
      {
         // zero out memory
         memset((void *)g_shared_token_hdr,0,DALGLBCTXT_SMEM_AREA_SIZE);

         // populate and init the dal specific region
         _PopulateLockHdr(g_shared_token_hdr,sizeof(DALGLB_HEADER),"dalspinlock");

      }
      //release spin lock
      InterProcSpinUnLock();
      GlbCtxtIsInit = 1;
      ret = DAL_SUCCESS;
   }
   else
   {
      //error, so memory must be in non zero state....
      DALSYS_LOG_ERROR_EVENT("failed- lock:0x%x",
         g_shared_token_hdr->lock.lock);
   }

exit_dalglbctxt_init:
   DALSYS_SyncLeave(hGlbCtxtSync);
   return ret;
}

DALResult
DALGLBCTXT_AllocCtxt(const char * name, uint32 size_req, uint32 lock_type, void ** ctxt)
{
   // search for an existing header
   DALGLB_HEADER * _hdr = 0;
   DALResult ret        = DAL_ERROR;

   if (!GlbCtxtIsInit)
   {
      if (DAL_SUCCESS != DALGLBCTXT_Init())
      {
         // failed to initialize
         return DAL_ERROR;
      }
   }

   // round size_req to cache line size
   if(size_req&(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1))
   {
      size_req = (size_req+DALGLBCTXT_CACHE_LINE_SIZE_BYTES) & ~(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1);
   }

   //grab the global lock to prevent other cores from accessing.
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
            goto exit_dal_alloc_ctxt;

         // populate and init the dal specific region
         _PopulateLockHdr(_hdr,size_req,name);

      }

      if(ctxt)
         *ctxt = (void *)_hdr;

      ret = DAL_SUCCESS;

exit_dal_alloc_ctxt:
      //release this lock for other cores
      GlbCtxtUnLock();
   }
   else
   {
      DALSYS_LOG_ERROR_EVENT("failed- lock:0x%x",
         g_shared_token_hdr->lock.lock);
   }

   return ret;
}

DALResult
DALGLBCTXT_FindCtxt(const char * name, void ** ctxt)
{
   // search for an existing header
   DALGLB_HEADER * _hdr = 0;

   if (!GlbCtxtIsInit)
   {
      if (DAL_SUCCESS != DALGLBCTXT_Init())
      {
         // failed to initialize
         return DAL_ERROR;
      }
   }

   //grab the global lock to prevent other cores from accessing.
   if(DAL_SUCCESS == GlbCtxtLock())
   {
      _hdr = _FindOrAllocHeader(name,0);

      if(ctxt)
         *ctxt = (void *)_hdr;

      //release this lock for other cores
      GlbCtxtUnLock();
   }
   else
   {
      DALSYS_LOG_ERROR_EVENT("failed- lock:0x%x",
         g_shared_token_hdr->lock.lock);
   }
   return (_hdr?DAL_SUCCESS:DAL_ERROR);
}
