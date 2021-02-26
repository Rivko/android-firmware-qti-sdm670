#ifndef _SMEM_H_
#define _SMEM_H_

/*===========================================================================

                    S H A R E D   M E M O R Y

===========================================================================*/
/**
  @file smem.h
*/
/* Defines the public API that is used to allocate memory in physical
  Shared Memory.
  
  This API deals strictly with void * to reduce dependencies.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMEM_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMEM_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smem" group description 
         in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/13   pa      Moving SMEM_ALLOC_FLAG* to smem_v.h.
07/29/13   pa      Make SMD use secure SMEM partitions, with BW compatibility.
06/26/13   pa      Updated comments and added ext_status to smem_alloc_params_type
04/24/13   pa      Added SMEM_STATUS_*, smem_alloc_ex, and smem_get_addr_ex.
04/15/11   tl      Moved smem_mem_type to its own header file
03/24/11   bt      Added SMEM_VERSION_SMSM and SMEM_SMSM_SIZE_INFO.
08/12/10   bt      Added mask argument to smem_version_set for generalization.
08/12/10   sw      (Tech Pubs) Final edits on new Doxygen comments.
07/27/10   bt      Updated function headers to include comments from smem.c.
07/13/10   hwu     Added SMEM_BOOT_INFO_FOR_APPS for keypad driver in OSBL. 
06/25/10   hwu     Added SMEM_SMEM_LOG_MPROC_WRAP so we can configure the 
                   shared memory as readable and writeable. 
06/25/10   hwu     Added SMEM_MODEM_SW_BUILD_ID for WM7 to query
                   Modem SW version.
06/22/10   esh     Updated Doxygen documentation
03/02/10   tl      Added items for debugging smem at boot
02/16/10   tl/esh  Added Doxygen tags
12/01/09   rs      CMI updates
11/19/09   sa      warning cleanup, sync-up with symbian
11/19/09   hwu     Added SMEM_SEFS_INFO. 
07/06/09   hwu     Added SMEM_SD_IMG_UPGRADE_STATUS for boot team.
06/10/09   bfc     CMI updates.
05/01/09   tl      Added SMEM_SMDLITE_TABLE
04/20/09   bfc     Only declare AU public items here.
03/06/09   zap     Added crash debug smem items
01/09/09   tl      Added entries for reading smem log
01/07/09   hwu     Added SMEM_DAL_AREA for DAL.
12/18/08   jlk     Added smd bridge port allocation table to smem type enum
12/16/08   hwu     Added SMEM_POWER_ON_STATUS_INFO for boot.
12/10/08   hwu     Added SMEM_USABLE_RAM_PARTITION_TABLE for boot.
09/26/08   jlk     Added SMEM_VERSION_SMD
09/25/08   hwu     Added DEM releated to smem_mem_type
09/22/08   hwu     Added clkregim related to the smem_mem_type.
07/01/08   hwu     Added SMEM_I2C_MUTEX.
06/19/08   bfc     Support variable sized SMD FIFOs.
04/15/08   hn      Merged. (02/21/08, bfc, First SMD Block port check in.)
04/15/08   hn      Merged. (02/10/08, bfc, Added new SMEM items for SMD block
                            ports.)
03/15/08   bfc     Added items to the SMEM heap.
01/28/08   bfc     Fixed a enum issue.
01/22/08   hn      Added SMEM_OSS_RRCASN1_BUF1 and SMEM_OSS_RRCASN1_BUF2.
12/06/07   jlk     Added SMEM_APPS_BOOT_MODE
11/15/07   taw     Added SMEM_BATT_INFO.
10/15/07   jlk     added SMEM_SMD_PROFILE for smd profiling
10/08/07   hwu     Added SMEM_CHANNEL_ALLOC_TBL. 
09/26/07   jhs     Added SMEM_HS_SUSPEND_FILTER_INFO for HS to support RPC
                   filtering.
08/16/07   ebb     Added enums SMEM_SMEM_LOG_EVENTS and 
                   SMEM_SMEM_STATIC_LOG_IDX.
08/14/07   ebb     Added new enum type SMEM_SLEEP_STATIC
08/14/07   bfc     Moved the heap info and allocations table up in SMEM.
                   Removed internal include from this public header.
07/09/07   hwu     Added SMEM_SCORPION_FREQUENCY.
06/11/07   ptm     Added smem get addr and smem free APIs.
06/05/07   hwu     Added SMEM_WM_UUID for boot.
04/26/07   ptm     Allocate 64 data structures for SMD channels.
04/08/07   ptm     Change smem_alloc to include smem_get_static_addr.
04/05/07   hwu     Fixed the order of SMEM_VERSION_INFO in smem_mem_type to
                   match smem_buf_data_tbl.
04/04/07   hwu     Moved SMEM_AARM_WARM_BOOT to the static region.
03/20/07   taw     Include smem_target.h instead of comdef.h.
03/14/07   bfc     Rearchitectured how SMEM is managed and accessed
02/07/07   ptm     Add HW Reset Detect field.
02/06/07   ptm     Added smem_get_static_addr API.
01/23/07   mmg     Added TSSC busy
01/15/07   ddh     Added support for limited sleep
01/04/07   bfc     Added a new api for boot
12/15/06   bfc     Fixed a type-o in the enum list
11/27/06   ptm     Move smem_get/set_diag_message to smem_diag.h.
09/06/06   taw     Removed smem_proc_comm typedefs and moved to
                   smem_proc_comm.h.
08/27/06   ptm     Add smem_get/set_diag_message APIs.
08/15/06   taw     Initial version.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "smem_type.h"

/** @addtogroup smem
@{ */

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** @brief Used by the AMSS to enable reset detection.\n
  The actual values used are defined by the AMSS core code.
*/
typedef struct
{
  uint32 magic_1;  /**< Most-significant word of the predefined magic value. */
  uint32 magic_2;  /**< Least-significant word of the predefined magic value. */
} smem_hw_reset_id_type;

/** SMEM Status return codes */
#define SMEM_STATUS_SUCCESS            0
#define SMEM_STATUS_FAILURE           -1
#define SMEM_STATUS_INVALID_PARAM     -2
#define SMEM_STATUS_OUT_OF_RESOURCES  -3
#define SMEM_STATUS_NOT_FOUND         -4
#define SMEM_STATUS_DUPLICATE         -5
#define SMEM_STATUS_SIZE_MISMATCH     -6

/** Flags for use with smem_alloc_ex() */
#define SMEM_ALLOC_FLAG_NONE            0 /**< Default behavior */

/** Parameters structure for smem_alloc_ex and smem_get_addr_ex */
typedef struct 
{
  /** Remote endpoint with which this SMEM item will be shared.  Input param.
   ** This field tells the allocation routine which partition to allocate from.
   ** If set to SMEM_INVALID_HOST, then the allocation will be from the default,
   ** unprotected partition. */
  smem_host_type remote_host;

  /** Identifier for the allocation.  Input param.
   ** Allowed types are in the smem_mem_type enum. */
  smem_mem_type smem_type;

  /** Requested size of the allocation.  This is an in/out parameter.
   ** Note that allocated size may be larger than requested.
   ** Actual allocated size is updated here upon successful
   ** allocation or address lookup. 
   ** Caller should always check the size param after the call to determine 
   ** the actual size, which may not match what was requested. */
  uint32 size;

  /** Pointer to allocated buffer.  This is an in/out parameter.
   ** Allocation routine will set the pointer to the 
   ** allocated buffer upon successful allocation or address lookup. */
  void * buffer;

  /** Flags for the allocation.  Input param.
   ** See SMEM_ALLOC_FLAG for options. */
  uint32 flags;

} smem_alloc_params_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smem_init
===========================================================================*/
/**
  Initializes the shared memory allocation structures.

  @return
  None.

  @dependencies
  Shared memory must have been cleared and initialized by the first system 
  bootloader, typically running on the modem processor. Even though calls to
  this function are gated by the smem_init_finished flag, this function
  must still be implemented to handle simultaneous calls
  from multiple threads.
*/
/*=========================================================================*/
void smem_init( void );

/*===========================================================================
FUNCTION      smem_alloc
===========================================================================*/
/**
  Requests a pointer to a buffer in shared memory.

  @param[in] smem_type   Type of memory.
  @param[in] buf_size    Size of the buffer requested. For pre-allocated 
                         buffers, an ERR_FATAL occurs if the buffer size 
                         requested does not match the size of the existing 
                         buffer. If the buf_size is not 64-bit aligned, this 
                         function increases the size until it is 64-bit aligned.

  @return
  NULL if smem_boot_init has not been called yet.\n
  Otherwise, returns either a valid SMEM address to the requested buffer or a 
  fatal error.

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void *smem_alloc
(
  smem_mem_type         smem_type,
  uint32                buf_size
);

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
int32 smem_alloc_ex
(
  smem_alloc_params_type * params
);

/*===========================================================================
MACRO         smem_get_static_addr
===========================================================================*/
/**
  Requests a pointer to a statically-allocated buffer in shared memory.

  This macro calls smem_alloc() directly and is intended as a backward-compatible 
  function for clients that still call the function directly.
  
  After all clients have converted back to smem_alloc() for all data items, 
  remove this macro. New code must call smem_alloc() directly, regardless of the 
  allocation class of the shared memory item.

  @param[in] smem_type   Type of memory for which to get a pointer.
  @param[in] buf_size    Size of the buffer requested. For pre-allocated
                         buffers, an ERR_FATAL occurs if the buffer
                         size requested does not match the size of the
                         existing buffer.

  @return
  Pointer to the requested buffer.

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
#define smem_get_static_addr smem_alloc

/*===========================================================================
FUNCTION      smem_get_addr
===========================================================================*/
/**
  Requests the address and size of an allocated buffer in shared memory. Newly 
  allocated shared memory buffers, which have never been allocated on any 
  processor, are guaranteed to be zero-initialized.

  @param[in]  smem_type   Type of memory for which to get a pointer.
  @param[out] buf_size    Size of the buffer allocated in shared memory.

  @return
  Pointer to the requested buffer, or NULL if the buffer has not been allocated.

  @dependencies
  The buffer must already have been allocated.

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void *smem_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
);

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
int32 smem_get_addr_ex
(
  smem_alloc_params_type * params
);

/*===========================================================================
FUNCTION      smem_free
===========================================================================*/
/**
  Frees a pointer in shared memory.
  
  @note This function is not actually implemented, but if it were, it would 
  allow other functions to reuse the memory previously allocated in shared 
  memory.

  @param[in] addr    Pointer to the shared memory block to be freed.

  @return
  None.

  @dependencies
  smem_init() must have been called on this processor.\n
  The pointer must have been allocated using smem_alloc() or smem_alloc_anon().
*/
/*=========================================================================*/
void smem_free
(
  void *addr
);

/*===========================================================================
FUNCTION      smem_version_set
===========================================================================*/
/**
  Sets the version number for this processor and a given object.
               
  The version number is compared to all previously set version numbers for this 
  object. The last processor to register checks against all other processors.

  @param[in] type       Type of object being version checked. It must be 
                        between the #SMEM_VERSION_FIRST and #SMEM_VERSION_LAST 
                        in the #smem_mem_type enum, unless it is of type 
                        #SMEM_VERSION_INFO (for internal use only).
  @param[in] version    Local version number for this memory object.
  @param[in] mask       Bitwise AND mask used to specify either SMEM or PROC_COMM
                        version when comparing against other processor version
                        numbers. To compare both (all 32 version bits), this
                        argument must be 0xFFFFFFFF.
   
  @return 
  TRUE -- Version number of the local processor matches all previously 
  registered versions for this object type.\n
  FALSE -- There is a mismatch.

  @dependencies
  smem_init() must have been called on this processor.

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
boolean smem_version_set
(
 smem_mem_type type,
 uint32 version,
 uint32 mask
);

/** @} */ /* end_addtogroup smem */

#ifdef __cplusplus
}
#endif

#endif /* _SMEM_H_ */
