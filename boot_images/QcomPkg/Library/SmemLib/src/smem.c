/*=============================================================================

                          SMEM CORE

 Copyright (c)  2004-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/16   vr      SMEM Version 0x000C0000 changes.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem_os.h"
#include "smem.h"
#include "smem_v.h"
#include "smem_version.h"
#include "smem_target.h"
#include "spinlock.h"
#include "smem_internal.h"
#include "smem_partition.h"
#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT
#include "smem_legacy.h"
#endif /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/

/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/
/* Global SMEM info stucture  */
smem_info_type smem_info;

/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*===========================================================================
FUNCTION      smem_alloc
===========================================================================*/
/**
@brief        Request a pointer to an item in shared/external memory. If the
              space for the item has been allocated in either shared/external 
              memory, return the pointer to that location, otherwise allocate
              buffer for the specified item from the shared memory.

@param[in]    smem_type     Type of memory to get a pointer for.
@param[in]    buf_size      Size of the buffer requested.  For pre-allocated
                            buffers, an ERR_FATAL will occur if the buffer
                            size requested does not match the size of the
                            existing buffer.  If the buf_size
                            is not 64-bit aligned, this function will top
                            the size off to an aligned one.

@dependencies None

@return      return NULL if smem_boot_init is not called yet. Otherwise, either
             return a valid SMEM address to the requested buffer or error fatal

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void *smem_alloc(smem_mem_type smem_type, uint32 buf_size)
{
  void *smem_buf;
  
#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
  if (smem_info.state != SMEM_STATE_INITIALIZED)
  {
	smem_init();
  }
  
  /* Return NULL if the shared memory is not zero initialized */
  if ((smem_info.state        != SMEM_STATE_INITIALIZED) ||
      (smem_info.funcs        == NULL) ||
      (smem_info.funcs->alloc == NULL))
  {
    SMEM_OS_LOG_ERR(3, "smem_alloc: SMEM is not initialized state=%d! "
        "smem_type=%d, buf_size=%d.", smem_info.state, smem_type, buf_size);
    return NULL;
  }
  
  if (smem_type >= smem_info.max_items)
  {
    SMEM_OS_LOG_ERR(2, "smem_alloc: Trying to allocate smem item=%d which is >= max item=%d!",
        smem_type, smem_info.max_items);
    return NULL;
  }
  
  /* Allocate shared memory buffer */
  smem_buf = smem_info.funcs->alloc(smem_type, buf_size);

  if (smem_buf == NULL)
  {
    SMEM_OS_LOG_ERR(2, "smem_alloc: SMEM allocation failed! smem_type=%d, buf_size=%d",
          smem_type, buf_size);
  }
  else
  {
    SMEM_OS_LOG_INFO(3, "smem_alloc: SMEM allocation success. smem_type=%d, buf_size=%d and "
          "offset = 0x%08X.", smem_type, buf_size, ((uint8*)smem_buf - SMEM_INFO_SMEM_BASE_ADDR()));
  }
  
  return smem_buf;
#else /* !defined(SMEM_FEATURE_ALLOC_SUPPORT) */
  uint32 ret_size;

  smem_buf = smem_get_addr(smem_type, &ret_size);
  if (smem_buf == NULL)
  {
    SMEM_OS_LOG_ERR(1, "smem_alloc: is not supported only get address is supported. "
                    "smem_type=%d was not allocated already.", smem_type);
  }
  else if (ret_size != buf_size)
  {
    SMEM_OS_LOG_ERR(3, "smem_alloc: smem_type=%d was allocated with different size=%d already, expected=%d.", 
            smem_type, ret_size, buf_size);

    smem_buf = NULL;
  }
  
  return smem_buf;
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */   
}

/*===========================================================================
FUNCTION      smem_get_addr
===========================================================================*/
/**
@brief        Request a pointer to an already allocated buffer in shared
              memory. Returns the address and size of the allocated buffer.

              Newly-allocated shared memory buffers, which have never been
              allocated on any processor, are guaranteed to be zeroed.

@param[in]    smem_type     Type of memory to get a pointer for.
@param[out]   buf_size      (Output) Size of buffer allocated in shared
                            memory.

@dependencies The buffer must already have been allocated.

@return       Pointer to the requested buffer, or NULL if the buffer has
              not been allocated.

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void *smem_get_addr(smem_mem_type  smem_type, uint32 *buf_size)
{
#ifdef SMEM_FEATURE_GET_ADDRS_SUPPORT
  void *smem_buf;

  if (smem_info.state != SMEM_STATE_INITIALIZED)
  {
	smem_init();
  }
  /* Return NULL if the shared memory is not zero initialized */
  if ((smem_info.state           != SMEM_STATE_INITIALIZED) ||
      (smem_info.funcs           == NULL) ||
      (smem_info.funcs->get_addr == NULL))
  {
    SMEM_OS_LOG_ERR(2, "smem_get_addr: SMEM is not initialized state=%d! "
        "smem_type=%d.", smem_info.state, smem_type);
    return NULL;
  }
  if (smem_type >= smem_info.max_items)
  {
    SMEM_OS_LOG_ERR(2, "smem_get_addr: Trying to get addr smem item=%d which is >= max item=%d!",
        smem_type, smem_info.max_items);
    return NULL;
  }
  
  /* Get shared memory buffer address */
  smem_buf = smem_info.funcs->get_addr(smem_type, buf_size);
  
  if (smem_buf == NULL)
  {
    SMEM_OS_LOG_ERR(1, "smem_get_addr: SMEM get addr failed! smem_type=%d", smem_type);
  }
  else
  {
    SMEM_OS_LOG_INFO(3, "smem_get_addr: SMEM get addr success. smem_type=%d, buf_size=%d"
          " and offset = 0x%08X.", smem_type, buf_size ? *buf_size : 0xFFFFFFFF, 
          ((uint8*)smem_buf - SMEM_INFO_SMEM_BASE_ADDR()));
  }
  return smem_buf;
#else /* !SMEM_FEATURE_GET_ADDRS_SUPPORT */
  return NULL;
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */
}

/*===========================================================================
FUNCTION      smem_alloc_ex
===========================================================================*/
/**
  Requests a pointer to a buffer in shared memory.  Upon success, params.buffer
  points to the allocation in shared memory.  

  @param[inout] params  See definition of smem_alloc_params_type for details.

  @return
  SMEM_STATUS_SUCCESS          - Allocation was successful,
                                 or already exists with specified size.
                                 Pointer to SMEM buffer is saved in 
                                 params.buffer.
                                 params.size contains the requested size rounded
                                 up to 8 bytes.
                                 params.flags contains the status of the cached
                                 flag, which may not match what was 
                                 requested if the item has been allocated 
                                 previously.
  SMEM_STATUS_SIZE_MISMATCH    - Allocation exists, but has different size.
                                 This is possible when another processor has 
                                 already allocated an item with this SMEM ID and
                                 a different size.
                                 Pointer to SMEM buffer is saved in 
                                 params.buffer.
                                 params.size contains the originally allocated
                                 size rounded up to 8 bytes.
                                 params.flags contains the status of the cached
                                 flag, which may not match what was 
                                 requested if the item has been allocated 
                                 previously.
  SMEM_STATUS_INVALID_PARAM    - Invalid parameter
  SMEM_STATUS_OUT_OF_RESOURCES - Not enough SMEM to allocate this buffer
  SMEM_STATUS_FAILURE          - General failure

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
int32 smem_alloc_ex(smem_alloc_params_type *params)
{
  int32 ret;
  
  if (params == NULL)
  {
    SMEM_OS_LOG_ERR(0, "smem_alloc_ex: params should not be NULL!");
    return SMEM_STATUS_INVALID_PARAM;
  }

  if (smem_info.state != SMEM_STATE_INITIALIZED)
  {
	smem_init();
  }
  /* Return NULL if the shared memory is not zero initialized */
  if ((smem_info.state           != SMEM_STATE_INITIALIZED) ||
      (smem_info.funcs           == NULL) ||
      (smem_info.funcs->alloc_ex == NULL))
  {
    SMEM_OS_LOG_ERR(3, "smem_alloc_ex: SMEM is not initialized state=%d! "
        "smem_type=%d, remote=%d.", smem_info.state, params->smem_type, params->remote_host);
    return SMEM_STATUS_FAILURE;
  }
  if (params->smem_type >= smem_info.max_items)
  {
    SMEM_OS_LOG_ERR(2, "smem_alloc_ex: Trying to allocate smem item=%d which is >= max item=%d!",
        params->smem_type, smem_info.max_items);
    return SMEM_STATUS_INVALID_PARAM;
  }
  
  /* Allocate shared memory buffer */
  ret = smem_info.funcs->alloc_ex(params);
  
  if (ret == SMEM_STATUS_SUCCESS)
  {
    SMEM_OS_LOG_INFO(5, "smem_alloc_ex: SMEM alloc_ex success. smem_type=%d, remote=%d, "
        "size=%d, flags=0x%08X and offset = 0x%08X.", params->smem_type, params->remote_host,
        params->size, params->flags, ((uint8*)params->buffer - SMEM_INFO_SMEM_BASE_ADDR()));
  }
  else if (ret == SMEM_STATUS_SIZE_MISMATCH)
  {
    SMEM_OS_LOG_ERR(5, "smem_alloc_ex: SMEM size mismatch! smem_type=%d, remote=%d, "
        "size=%d, flags=0x%08X and offset = 0x%08X.", params->smem_type, params->remote_host,
        params->size, params->flags, ((uint8*)params->buffer - SMEM_INFO_SMEM_BASE_ADDR()));
  }
  else
  {
    if (!(params->flags & SMEM_ALLOC_FLAG_PARTITION_ONLY))
    {
      params->buffer = smem_alloc(params->smem_type, params->size);
      if (params->buffer)
      {
        params->flags &= ~(SMEM_ALLOC_FLAG_CACHED);
        return SMEM_STATUS_SUCCESS;
      }
      ret = SMEM_STATUS_FAILURE;
    }
    
    SMEM_OS_LOG_ERR(5, "smem_alloc_ex: SMEM alloc_ex failed with err=%d! smem_type=%d, remote=%d, "
        "size=%d, flags=0x%08X.", ret, params->smem_type, params->remote_host, params->size, params->flags);
  }
  return ret;
}
#endif /* SMEM_FEATURE_PARTITION_SUPPORT */

/*===========================================================================
FUNCTION      smem_get_addr_ex
===========================================================================*/
/**
  Requests the address and size of an allocated buffer in shared memory.  
  If found, sets the buffer and size fields of the params struct.

  @param[inout] params  params.smem_type must be set to the ID to search for.
                        params.remote_host must be set to the ID of the remote
                        host of the partition.
                        If params.flags has the SMEM_ALLOC_FLAG_PARTITION_ONLY 
                        flag set, then this function will not search the default
                        partition for the item.  This is useful when SMEM item
                        X is possible in both the default partition and an edge-
                        pair partition, and the caller wants to find out if the
                        item exists in the edge-pair partition only.

  @return
  SMEM_STATUS_SUCCESS       - Allocated buffer found
                              params.buffer contains a pointer to the allocation.
                              params.size contains the size of the allocation
                              as originally requested, rounded up to 8 bytes.
                              params.flags contains the status of the cached
                              flag.
  SMEM_STATUS_NOT_FOUND     - Allocated buffer not found
  SMEM_STATUS_INVALID_PARAM - Invalid parameter
  SMEM_STATUS_FAILURE       - Failure occurred

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
  This function updates the size_remaining field for the partition.
*/
/*=========================================================================*/
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
int32 smem_get_addr_ex(smem_alloc_params_type *params)
{
#ifdef SMEM_FEATURE_GET_ADDRS_SUPPORT
  int32 ret;
  
  if (params == NULL)
  {
    SMEM_OS_LOG_ERR(0, "smem_get_addr_ex: params should not be NULL!");
    return SMEM_STATUS_INVALID_PARAM;
  }

  if (smem_info.state != SMEM_STATE_INITIALIZED)
  {
	smem_init();
  }
  /* Return NULL if the shared memory is not zero initialized */
  if ((smem_info.state              != SMEM_STATE_INITIALIZED) ||
      (smem_info.funcs              == NULL) ||
      (smem_info.funcs->get_addr_ex == NULL))
  {
    SMEM_OS_LOG_ERR(3, "smem_get_addr_ex: SMEM is not initialized state=%d! "
        "smem_type=%d, remote=%d.", smem_info.state, params->smem_type, params->remote_host);
    return SMEM_STATUS_FAILURE;
  }
  
  if (params->smem_type >= smem_info.max_items)
  {
    SMEM_OS_LOG_ERR(2, "smem_get_addr_ex: Trying to get addr smem item=%d which is >= max item=%d!",
        params->smem_type, smem_info.max_items);
    return SMEM_STATUS_INVALID_PARAM;
  }
  
  /* Get shared memory buffer address */
  ret = smem_info.funcs->get_addr_ex(params);
  
  if (ret == SMEM_STATUS_SUCCESS)
  {
    SMEM_OS_LOG_INFO(5, "smem_get_addr_ex: SMEM get_addr_ex success. smem_type=%d, remote=%d, "
        "size=%d, flags=0x%08X and offset = 0x%08X.", params->smem_type, params->remote_host,
        params->size, params->flags, ((uint8*)params->buffer - SMEM_INFO_SMEM_BASE_ADDR()));
  }
  else if (ret == SMEM_STATUS_SIZE_MISMATCH)
  {
    SMEM_OS_LOG_ERR(5, "smem_get_addr_ex: SMEM size mismatch! smem_type=%d, remote=%d, "
        "size=%d, flags=0x%08X and offset = 0x%08X.", params->smem_type, params->remote_host,
        params->size, params->flags, ((uint8*)params->buffer - SMEM_INFO_SMEM_BASE_ADDR()));
  }
  else
  {
    if (!(params->flags & SMEM_ALLOC_FLAG_PARTITION_ONLY))
    {
      uint32 local_size;
      params->buffer = smem_get_addr(params->smem_type, &local_size);
      if (params->buffer)
      {
        params->size = local_size;
        params->flags &= ~(SMEM_ALLOC_FLAG_CACHED);
        return SMEM_STATUS_SUCCESS;
      }
      ret = SMEM_STATUS_NOT_FOUND;
    }
	
#if 0
	/* comment out following err to avoid err printing in case remote host is not boot and FIFO not allocated yet */
    SMEM_OS_LOG_ERR(5, "smem_get_addr_ex: SMEM get_addr_ex failed with err=%d! smem_type=%d, remote=%d, "
        "size=%d, flags=0x%08X.", ret, params->smem_type, params->remote_host, params->size, params->flags);
#endif
  }
  return ret;
#else /* !SMEM_FEATURE_GET_ADDRS_SUPPORT */
  return SMEM_STATUS_NOT_FOUND;
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */
}
#endif /* SMEM_FEATURE_PARTITION_SUPPORT */
/*===========================================================================
FUNCTION      smem_version_set
===========================================================================*/
/**
@brief        Sets the version number for this processor and a given object.

              The version number is compared against all previously set
              version numbers for this object.  So the last processor to
              register will check against all other processors.

@param[in]    type    the type of object being version checked.
                  must be between the #SMEM_VERSION_FIRST and
                  #SMEM_VERSION_LAST in the #smem_mem_type enum in smem.h,
                  unless it is of type SMEM_VERSION_INFO (for internal use).
 
@param[in]    version   the local version number for this memory
                 object

@param[in]    mask    The client may specify a mask in case the 32-bit #version
                  argument contains multiple protocols' versions, and not all
                  of them need to be checked.
                  
@dependencies smem_init() must have been called on this processor.
      
@return       boolean - true if the version number of the local processor
                 matches all previously registered versions for this object
                 type, false if there is a mismatch.

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
boolean smem_version_set
(
  smem_mem_type type,
  uint32        version,
  uint32        mask
)
{
  uint32  idx;
  uint32  my_version;
  uint32  other_version;
  boolean match = TRUE;
  uint32  *version_array;

  if ((type != SMEM_VERSION_INFO) && ((type < SMEM_VERSION_FIRST) ||
                                      (type > SMEM_VERSION_LAST)))
  {
    return FALSE;
  }
  
  if (type == SMEM_VERSION_INFO)
  {
    smem_static_allocs_type *static_allocs = 
        (smem_static_allocs_type *)SMEM_INFO_SMEM_BASE_ADDR();
        
    version_array = static_allocs->version;
  }
  else
  {
    version_array = (uint32 *)smem_alloc(type, 
              SMEM_VERSION_INFO_SIZE * sizeof(uint32));
    if (version_array == NULL)
    {
      SMEM_OS_LOG_FATAL(1, "smem_version_set: Unable to allocate version array: "
            "type %d.", type);
      return FALSE;
    }
  }
  
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  
  my_version = (version & mask);
  version_array[SMEM_VERSION_INFO_OFFSET] |= my_version;
  
  smem_os_mem_barrier();
  
  for(idx = 0; idx < SMEM_VERSION_INFO_SIZE; idx++)
  {
    other_version = version_array[idx] & mask;

    /* If a version exists then compare the major numbers, determined by the
     * value of mask. */
    if((other_version != 0) && (other_version != my_version) )
    {
      match = FALSE;
      idx = SMEM_VERSION_INFO_SIZE;  /* jump out of the loop */
    }
  }
  
  SMEM_ACCESS_CONFIGURE_RESTORE();

  return match;
}/* smem_version_set */

/*=============================================================================
  FUNCTION  <function>
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
SMEM_OS_INIT_FUNC_RECLAIM_SECTION
static uint32 smem_boot_version_get(void)
{
  volatile smem_static_allocs_type *static_allocs = 
            (volatile smem_static_allocs_type *)SMEM_INFO_SMEM_BASE_ADDR();
  uint32 version;

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  
  version = static_allocs->version[SMEM_VERSION_BOOT_OFFSET];
  
  SMEM_ACCESS_CONFIGURE_RESTORE();
  
  return version;
}

/*=============================================================================
  FUNCTION  <function>
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
SMEM_OS_INIT_FUNC_RECLAIM_SECTION
static boolean smem_boot_init_check(void)
{
  volatile smem_static_allocs_type *static_allocs = 
            (volatile smem_static_allocs_type *)SMEM_INFO_SMEM_BASE_ADDR();
            
  boolean init_status;

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  
  init_status = (static_allocs->heap_info.initialized == SMEM_HEAP_INFO_INIT);

  SMEM_ACCESS_CONFIGURE_RESTORE();
  
  return init_status;
}

/*===========================================================================
FUNCTION      smem_get_num_hosts
===========================================================================*/
/**
  Return the maximum nuber of hosts. It should not be less than
  SMEM_NUM_HOSTS.

  @return
  The maximum nuber of hosts.
*/
/*=========================================================================*/
uint32 smem_get_num_hosts( void )
{
  /* hard code the return value until bootloaders place it 
     in SMEM/IMEM where it can be read from */
  return SMEM_NUM_HOSTS;  
}

/*=============================================================================
  FUNCTION  smem_init
=============================================================================*/
/**
  Initializes the shared memory allocation structures.

  @return
  None.

  @dependencies
  Shared memory must have been cleared and initialized by the first system 
  bootloader.
 */
/*===========================================================================*/
SMEM_OS_INIT_FUNC_RECLAIM_SECTION
void smem_init(void)
{
  uint32 version;
  
  /* Prevent this function from doing anything if it has already been called.
   * Caller must call each initialization functions serially from a single 
   * initialization thread, so there is no need to be thread-safe.  
   * But we need to protect against the critical sections being re-init'ed. */
  if (smem_info.state == SMEM_STATE_INITIALIZED)
  {
    return;
  }

  /* Initialize the OS specific functionality */
  smem_os_init();
  
  /* Initialize the target specific functionality */
  smem_targ_init();
  
  /* Set the SMEM base physical address and size */
  smem_info.smem_phys_base_addr = smem_targ_base_addr_get();
  smem_info.smem_size           = smem_targ_size_get();
  
  /* Map the SMEM base physical address to virtual address */
  smem_info.smem_base_addr = smem_os_map_base_va(smem_info.smem_phys_base_addr,
                                                 smem_info.smem_size);
  
  if (smem_info.smem_base_addr == NULL)
  {
    SMEM_OS_LOG_FATAL(2, "SMEM base addr=0x%08X, size=%d memory mapping failed.",
                smem_info.smem_phys_base_addr, smem_info.smem_size);
    return;
  }
  /* Max item number allowed */
  smem_info.max_items = smem_targ_max_items_get();
  
  /* First map the SMEM legacy region */
  smem_os_map_page_attr(0, SMEM_LEGACY_SMEM_SIZE, FALSE);
  
  /* Check if SMEM is initialized by Boot */
  if (!smem_boot_init_check())
  {
    SMEM_OS_LOG_FATAL(0, "SMEM is not initialized by Boot.");
  }
  
  /* Initialize the spinlock */
  smem_spin_lock_init();
  
  /* Get the Boot SMEM version and do corresponding initialization and 
   * update the alloc/get function pointers based on version */
  version = smem_boot_version_get();
  
  if ((version & SMEM_MAJOR_VERSION_MASK) == SMEM_LEGACY_VERSION_ID)
  {
#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT
    /* Use old/legacy version */
    smem_info.version = version;
    
    /* SMEM is initialized by with old/legacy version, hence use implementation */
    smem_info.funcs = &smem_legacy_funcs;
    
    /* Initialize the legacy */
    smem_legacy_init();
#else /* !SMEM_FEATURE_LEGACY_VERSION_SUPPORT */
    SMEM_OS_LOG_FATAL(1, "SMEM version 0x%X is not supported.", version);
#endif /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */
  }
  else
  {
    /* Use actual version */
    smem_info.version = SMEM_VERSION_ID;
    
    smem_info.funcs = &smem_part_funcs;
  }

  /* Set up partition structures */
  smem_part_init();
    
  /*------------------------------------------------------------
   * Set SMEM version info and verify that everyone registered so
   * far is compatible.
   * Each call to smem_init verifies that all previous calls are
   * compatible. So, the last call to smem_init will be certain to
   * detect if anyone is incompatible.
   *------------------------------------------------------------*/
  smem_os_mem_barrier();

  /* Moved before the version check/set in order to fulfill that function's
   * dependency that smem has been initialized (and indicate so). */
  smem_info.state = SMEM_STATE_INITIALIZED;
  
  if (smem_version_set(SMEM_VERSION_INFO, smem_info.version, 
                       SMEM_MAJOR_VERSION_MASK) == FALSE)
  {
    SMEM_OS_LOG_FATAL(1, "smem_init: major version (%d) does not match all procs!", 
        smem_info.version);
    return;
  }
}
