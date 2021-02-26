/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "cachemgr.h"
#include "observer.h"
#include "clientmgr.h"
#include "asic.h"
#include "qurt_timer.h"
#include "mipsmgr.h"

// All state info for the Cache Partition Manager
typedef struct
{
    DALSYSSyncHandle lock;
    AdsppmObserverStateType observerState;
    uint32 activeClientClasses;
    AsicCachePartitionConfigTableType* partitionConfigTable;
    qurt_cache_partition_size_t mainPartitionSize;
    uint32 activeLineLockCount;
    uint32 mppsthreshold[ADSPPM_NUM_OF_CLIENT_CLASSES];
} AdsppmCachePartitionManagerType;

static AdsppmCachePartitionManagerType gAdsppmCachePartitionManager;

///////////////////////////////////////////////////////////////////////////////
// Static functions 
///////////////////////////////////////////////////////////////////////////////

// Notify all registered observers of the specified event
static void cpmNotify(AdsppmCachePartitionManagerEventId eventId)
{
    AdsppmCachePartitionManagerType* self = &gAdsppmCachePartitionManager;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "updating observers on event %d", eventId);
    observer_notify(&self->observerState, eventId);
    return;
}

// Returns true if there was a change in the active client classes.
// Assume lock already obtained.
static boolean cpmUpdateActiveClientClasses(
    AdsppmCachePartitionManagerType* self)
{
    boolean updated = FALSE;
    uint32 currentClasses = CM_GetActiveUseCaseClasses();
    if(currentClasses != self->activeClientClasses)
    {
        self->activeClientClasses = currentClasses;
        updated = TRUE;
    }
    return updated;
}
/**
* Checks if current  clientclass MPPS vote is higher than threshold value or not.
* Returns true if it is otherwise false
*/
static boolean cpmCheckMPPSClientClassValue (AdsppmCachePartitionManagerType* self)
{
   boolean retVal = TRUE;
   uint32 mppsthreshold=0, currentmppsclientclassvalue=0;

   if(!self)
    {
        retVal = FALSE;
        return retVal;
    }
   
   for(uint32 index = 0; index < ADSPPM_NUM_OF_CLIENT_CLASSES; index++)
   {
           // 1 << index will give the different client class enums
           // Except MMPM_UNKNOWN_CLIENT_CLASS which is not needed
           MmpmClientClassType clientclassId = 1 << index;
           if(self->activeClientClasses & clientclassId)
             {
                 mppsthreshold = self->mppsthreshold[index];
                 ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                        "MPPS Threshold value: %d",
                        mppsthreshold);
                 if(!mppsthreshold)
                      continue;
                 else if(Adsppm_Status_Success == mipsmgr_GetMppsPerClientClass( clientclassId, &currentmppsclientclassvalue ))
                    {
                       if(currentmppsclientclassvalue <= mppsthreshold)
                         {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                                "Cache partition is rejected: current mpps value %d",
                                                 currentmppsclientclassvalue);
                             retVal = FALSE;
                             break;
                         }
                       ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                                "Current MPPS value: %d",
                                                 currentmppsclientclassvalue);
                    }
              }
    }
      
    return retVal;
}

// Returns  current cache partition size
// 
Adsppm_Status CPM_GetCachePartitionSize(uint32 *cachesize)
{
    AdsppmCachePartitionManagerType* self = &gAdsppmCachePartitionManager;
    if(self)
    	{
          //Aquire lock
          adsppm_lock(self->lock);
          //Read the main partition size from cpm context structure
          *cachesize = self->mainPartitionSize;
          adsppm_unlock(self->lock);	
          return Adsppm_Status_Success;		  
    	}
	else
        return Adsppm_Status_Failed;		

}


// Update the cache partition configuration based on the current mix of active
// client classes.
// Assume lock already obtained
static void cpmUpdatePartition(AdsppmCachePartitionManagerType* self)
{
    if(self->partitionConfigTable &&
        self->partitionConfigTable->pConfigEntries)
    {
        boolean foundConfig = FALSE;
        boolean partitionChanged = FALSE;
        const AsicCachePartitionConfigType* configEntry = NULL;
        // Search for entry in config table matching current aggregate
        // use case class
        for(int n = 0; n < self->partitionConfigTable->numEntries; n++)
        {
            configEntry =
                &self->partitionConfigTable->pConfigEntries[n];
            if(configEntry->aggregateClass == self->activeClientClasses)
            {
                //Check in table for each client class's threshold value
                //If mpps value is higher than threshold, move forward 
                if ((self->activeClientClasses == CLIENTCLASS_CHECK_MPPS_VOTE) && !cpmCheckMPPSClientClassValue(self) )
                    break;
                
                foundConfig = TRUE;
                if(configEntry->mainPartSize != self->mainPartitionSize)
                {
                    // Set new partition size
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                        "setting cache partition size: setting %d",
                        configEntry->mainPartSize);
                    if(qurt_mem_configure_cache_partition(
                        HEXAGON_L2_CACHE, configEntry->mainPartSize) ==
                        QURT_EOK)
                    {
                        self->mainPartitionSize =
                            configEntry->mainPartSize;
                        partitionChanged = TRUE;
                        break;
                    }
                    else
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "failed to set cache partition size: "
                            "setting %d", configEntry->mainPartSize);
                    }
                }
            }
        }
        if(!foundConfig && (self->mainPartitionSize != FULL_SIZE))
        {
            // If entry was not found in the table, default to no
            // paritioning (FULL_SIZE setting)
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                "setting cache partition size: default setting %d",
                FULL_SIZE);
            if(qurt_mem_configure_cache_partition(HEXAGON_L2_CACHE,
                FULL_SIZE) == QURT_EOK)
            {
                self->mainPartitionSize = FULL_SIZE;
                partitionChanged = TRUE;
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                    "failed to set cache partition size: FULL_SIZE");
            }
        }
        if(partitionChanged)
        {
            cpmNotify(CPM_CACHE_CONFIGURATION_CHANGE);
        }
    }
}

// Check for change in active client classes, and if there is a change, select
// a new cache partitioning configuration based on the new mix of client
// classes
static void cpmOnClientManagerUpdate(AdsppmClientManagerEventId eventId,
    void* param)
{

    AdsppmCachePartitionManagerType* self =
        (AdsppmCachePartitionManagerType*) param;

    adsppm_lock(self->lock);

    if(eventId == CM_ACTIVE_CLASS_CHANGE_EVENT)
    {
        // Skip possible cache partition configuration change if line locking
        // is in progress.  If skipped here due to line locking activity, the
        // partition config will be re-evaluated once the line locking
        // completes.
        if((self->activeLineLockCount == 0) &&
            cpmUpdateActiveClientClasses(self))
        {
            cpmUpdatePartition(self);
        }
    }

    adsppm_unlock(self->lock);

}

static Adsppm_Status cpmDoLineLocking(
    const MmpmL2CacheLineLockParameterType* lockingParams)
{

    Adsppm_Status status = Adsppm_Status_Success;
    uint32 totalSize = 0, blockSize = 0, lockedSize = 0, pauseUs = 0;
    uint8* p = NULL;
    boolean throttleEnabled = FALSE;

    if(lockingParams == NULL)
        return Adsppm_Status_Failed;

    throttleEnabled = (lockingParams->throttleBlockSize > 0) &&
        (lockingParams->throttlePauseUs > 0);

    if(throttleEnabled)
    {
        totalSize = lockingParams->size;
        blockSize = lockingParams->throttleBlockSize;
        pauseUs = lockingParams->throttlePauseUs;
    }
    else
    {
        totalSize = lockingParams->size;
        blockSize = lockingParams->size;
        pauseUs = 0;
    }

    p = lockingParams->startAddress;

    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_DEBUG,
        "throttle %d, totalSize %d, blockSize %d, pause %d",
        throttleEnabled, totalSize, blockSize, pauseUs);

    do
    {
        if(qurt_mem_l2cache_line_lock((qurt_addr_t) p, blockSize) == QURT_EOK)
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "locked cache lines: address 0x%x, size %d",
                (uint32) p, blockSize);

            lockedSize += blockSize;
            p += blockSize;

            if((pauseUs > 0) && (lockedSize < totalSize))
            {
                if(qurt_timer_sleep(pauseUs) != QURT_EOK)
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "pause of %d us failed", pauseUs);
                    status = Adsppm_Status_Failed;
                }
            }

            // Setup for next iteration - check for partial block
            if(blockSize > (totalSize - lockedSize))
                blockSize = totalSize - lockedSize;
        }
        else
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                "failed to lock cache lines: address 0x%x, size %d",
                (uint32) p, blockSize);
            status = Adsppm_Status_Failed;
        }

        ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_DEBUG,
            "status %d, lockedSize %d, totalSize %d",
            status, lockedSize, totalSize);
    }
    while((status != Adsppm_Status_Failed) && (lockedSize < totalSize));

    return status;

}

///////////////////////////////////////////////////////////////////////////////
// Public functions 
///////////////////////////////////////////////////////////////////////////////

uint32 CPM_GetMPPSThresholdValue (MmpmClientClassType *clientClassValue )
{
   uint32 mppsThreshold = 0;
   
   if(ACM_GetFeatureStatus(AsicFeatureId_CacheSizeMPPSThreshold) ==
                                           AsicFeatureState_Enabled)
    {
       ACM_GetMPPSThresholdfromClientClass (*clientClassValue, &mppsThreshold);

    }
   return mppsThreshold;
}
// Initialize the Cache Partition Manager.
// Requires that the Client Manager and ASIC Configuration Manager are already
// initialized.
Adsppm_Status CPM_Init(void)
{

    AdsppmCachePartitionManagerType* self = &gAdsppmCachePartitionManager;

    memset(self, 0, sizeof(AdsppmCachePartitionManagerType));

    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock, NULL))
    {
        return Adsppm_Status_Failed;
    }

    if(observer_init(&self->observerState) != Adsppm_Status_Success)
        return Adsppm_Status_Failed;

    self->partitionConfigTable = ACM_GetCachePartitionConfig();

    if(ACM_GetFeatureStatus(AsicFeatureId_CachePartitionControl) ==
        AsicFeatureState_Enabled)
    {
        CM_RegisterEvent(CM_ACTIVE_CLASS_CHANGE_EVENT, cpmOnClientManagerUpdate,
            (void*) self);
    }

    for(uint32 index = 0; index < ADSPPM_NUM_OF_CLIENT_CLASSES; index++)
       {
          MmpmClientClassType clientclassId  = 1 << index;
          self->mppsthreshold[index] = CPM_GetMPPSThresholdValue (&clientclassId);
       } 

    return Adsppm_Status_Success;

}

Adsppm_Status CPM_SetLineLocking(
    const MmpmL2CacheLineLockParameterType* oldParam,
    const MmpmL2CacheLineLockParameterType* newParam)
{
    AdsppmCachePartitionManagerType* self = &gAdsppmCachePartitionManager;
    Adsppm_Status status = Adsppm_Status_Failed;
    boolean tryLock = FALSE;
    boolean tryUnlock = FALSE;
    boolean requestLock = FALSE;
    boolean currentlyLocked;
    boolean newParamValid;

    if(oldParam == NULL || newParam == NULL)
    {
        return Adsppm_Status_BadParm;
    }

    adsppm_lock(self->lock);

    // Check if request is to lock (true) or unlock (false)
    requestLock = 
        (newParam->startAddress != 0) && (newParam->size != 0);

    // Check if client already has locking enabled
    currentlyLocked = (oldParam->startAddress != 0) &&
        (oldParam->size != 0);

    // Lock request is valid if address is aligned to 32 bytes, and the
    // address is aligned to the size
    newParamValid =
        (newParam->startAddress != 0) &&
        (newParam->size != 0) &&
        ((uint32) newParam->startAddress % 32 == 0) &&
        ((uint32) newParam->startAddress % newParam->size == 0);

    // Flags to determine if we'll attempt to lock or unlock
    tryUnlock = !requestLock && currentlyLocked;
    tryLock = requestLock && !currentlyLocked && newParamValid;

    // Log errors

    if(requestLock && !tryLock)
    {
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
            "cache line lock request failed: currently %s, params are %s",
            currentlyLocked ?  "locked" : "unlocked",
            newParamValid ? "valid" : "invalid");
    }
    if(!requestLock && !tryUnlock)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "cache line unlock request failed: currently %s",
            currentlyLocked ? "locked" : "unlocked");
    }

    // Try locking or unlocking

    if(tryLock)
    {
        // If the cache is currently not in half-half configuration then we'll
        // temporarily enable half partitioning while line locking is being
        // done.  This is to ensure the locked lines are in the current
        // partition, should partioning be enabled in the future.
        boolean tempPartitioning = (self->mainPartitionSize != HALF_SIZE);

        if(tempPartitioning)
        {
            if(qurt_mem_configure_cache_partition(HEXAGON_L2_CACHE,
                HALF_SIZE) == QURT_EOK)
            {
                self->mainPartitionSize = HALF_SIZE;
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                    "temporarily set cache partition size to half");
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                    "warning: failed to set cache partition size to half");
                tempPartitioning = FALSE;
            }
        }

        self->activeLineLockCount += 1;

        adsppm_unlock(self->lock);

        // Enable line locking; this is done outside of the lock, as to not
        // block other cache partition manager operations while line locking
        // is in progress.
        status = cpmDoLineLocking(newParam);

        adsppm_lock(self->lock);

        self->activeLineLockCount -= 1;

        // If all line locking requests are done, go back to the non-temporary
        // cache partition configuration.  This may be different than the
        // configuration before line locking started, if there was a change
        // in client class mix while locking was in progress.
        if(self->activeLineLockCount == 0)
        {
            cpmUpdateActiveClientClasses(self);
            cpmUpdatePartition(self);
        }

    }
    else if(tryUnlock)
    {
        // Disable line locking that was previously done by this client
        if(qurt_mem_l2cache_line_unlock(
            (qurt_addr_t) oldParam->startAddress,
            oldParam->size) == QURT_EOK)
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                "unlocked cache lines: address 0x%x, size %d",
                oldParam->startAddress, oldParam->size);
            status = Adsppm_Status_Success;
        }
        else
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                "failed to unlock cache lines: address 0x%x, size %d",
                oldParam->startAddress, oldParam->size);
        }
    }

    adsppm_unlock(self->lock);

    return status;
}

Adsppm_Status CPM_RegisterEvent(AdsppmCachePartitionManagerEventId event,
    AdsppmCacheEventHandler handler, void* param)
{

    AdsppmCachePartitionManagerType* self = &gAdsppmCachePartitionManager;
    // Assuming AdsppmCacheEventHandler can be cast to AdsppmEventHandler
    return observer_add(&self->observerState, event,
        (AdsppmEventHandler) handler, param);

}

Adsppm_Status CPM_UnregisterEvent(AdsppmCachePartitionManagerEventId event,
    AdsppmCacheEventHandler handler)
{

    AdsppmCachePartitionManagerType* self = &gAdsppmCachePartitionManager;
    // Assuming AdsppmCacheEventHandler can be cast to AdsppmEventHandler
    return observer_remove(&self->observerState, event,
        (AdsppmEventHandler) handler);

}

