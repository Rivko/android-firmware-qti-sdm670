/** @file SmemLibNull.c
  
  Stub functions for SmemLib

  Copyright (c) 2013, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/09/13   vk      Initial revision

=============================================================================*/

typedef unsigned long uint32;
typedef unsigned int smem_mem_type;

void smem_init (void)
{
  return;
}

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
void *smem_alloc
(
  smem_mem_type         smem_type,
  uint32                buf_size
)
{
  return NULL;
}

void *smem_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
)
{
  return NULL;
}

/*===========================================================================
FUNCTION      smem_boot_init
===========================================================================*/
/**
@brief        Called by BOOT to initialize smem
                  
@dependencies None
      
@return       None
*/
/*=========================================================================*/
void smem_boot_init( void )
{
}


/*===========================================================================
FUNCTION      smem_boot_debug_init
===========================================================================*/
/**
@brief        Initialize shared memory for crash debugging.
 
              This function may be called at boot to recover enough state from
              shared memory to allow debug to recover state information from
              shared memory that may be used for offline debugging. This
              function will:

              (1) Zero the spinlocks
              (2) Configure the local data structures to allow access to
                  shared memory.

              Debug may call smem_get_addr() to determine the address and size
              of shared memory items. Debug should not write to the shared
              memory items.

              After calling this function, and recovering the relevant
              debugging information, boot must call smem_boot_init() to
              reinitialize shared memory before continuing.

              This function is supported only in the first system bootloader.

@dependencies None

@return       None

@sideeffect   None
*/
/*===========================================================================*/
void smem_boot_debug_init( void )
{
}
