#ifndef __VSS_IMEMORY_PUBLIC_IF_H__
#define __VSS_IMEMORY_PUBLIC_IF_H__

/**
  @file vss_imemory_public_if.h
  @brief This file contains the APR API definitions for CVD shared memory
  management.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2012, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_imemory_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
04/12/12     HL    Initial version.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * MEMORY MAP/UNMAP APIS                                                    *
 ****************************************************************************/

/* Shared Memory Usage Guidelines *******************************************/
/** @weakgroup weakf_shared_mem_usage_guidelines
  This section describes the guidelines for using shared memory to share
  data between the client and the CVD.

  Before shared memory can be used, it must first be mapped with the MVM. The
  mapped memory is accessible by all sessions of all CVD modules.
  The client must not map the same piece of memory twice with the MVM.

  The client specifies the memory attributes in the memory mapping commands.
  The attributes are: 
  - Cached or uncached -- Whether the memory is cached or uncached
  - Cache line size if cached -- Minimum amount of data in bytes that can be
    affected by a single cache maintenance operation
  - Access permission -- Permission to access the memory after it is mapped,
    (the memory becomes readable and writable)
  - Page alignment -- Page frame alignment
  - Minimum native data type access width -- Minimum native data type width
    in bits that can be accessed
  - Maximum native data type access width -- Maximum native data type width
    in bits that can be accessed

  The CVD supports three types of shared memory:
  - Virtual memory -- Useful when the client and the CVD both see a unified
    virtual address space (such as when both the client and the CVD are on the
    same processor). For information on how to map virtual memory with the CVD,
    see #VSS_IMEMORY_CMD_MAP_VIRTUAL.
  - Contiguous physical memory -- Useful when enough contiguous physical memory
    is available. For information on how to map contiguous physical memory
    with the CVD, see #VSS_IMEMORY_CMD_MAP_PHYSICAL.
  - Non-contiguous physical memory -- Useful when enough contiguous memory is
    not available or not efficient (in terms of memory utilization) to
    allocate and multiple memory blocks are needed. For information on how to
    map non-contiguous physical memory with the CVD, see
    #VSS_IMEMORY_CMD_MAP_PHYSICAL.

  The CVD provides a handle to the client in response to the memory mapping
  commands. The handle encapsulates the shared memory attributes.

  After the memory is mapped, the client can use it for sharing data with the
  CVD by providing the location and size of the data in shared memory as well
  as the handle returned by the memory mapping command. The location of the
  data is specified as a memory address or an address offset depending on the
  type of the shared memory used:
  - Virtual memory 
    - The client specifies the virtual address to indicate the data location.
    - The CVD uses the virtual address directly to access the memory.
  - Contiguous physical memory.
    - The client specifies the physical address to indicate the data location.
    - The CVD does the physical-to-virtual address translation before
      accessing the memory. @newpage
  - Non-contiguous physical memory.
    - The client specifies an address offset to indicate the data location.
    - The offset begins from the client's starting virtual address of the
      memory. It is possible to use an offset because both the client and the
      CVD map non-contiguous physical memory to contiguous virtual memory.
      When the CVD first maps the memory, it obtains a virtual address,
      cvd_start_virt_addr. 
    - When an address offset is provided to indicate the
      location of the data, the CVD uses the cvd_start_virt_addr + offset to
     access the shared memory.

  When specifying the data location, any memory address or address
  offset provided to the CVD must be aligned to the lcm_align address.
  The address or address offset must also adhere to the memory
  attributes required for the region in which the memory address or offset was
  mapped, where lcm_align = LCM (cache line size, maximum native data type
  access width) and LCM = least common multiple. Also, the
  memory must be large enough to hold the data, and it must be a multiple of
  the cache line size.

  For the provided memory, the client must not use the
  subsequent padding bytes (if there are any) after the data for other
  purposes because cache maintenance operations, such as flushing, can affect
  the padding bytes.
*****************************************************************************/

/* Type definition for vss_imemory_block_t. */
typedef struct vss_imemory_block_t vss_imemory_block_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_block_t 
@{ */
/* Structure for specifying a single block of contiguous memory.
*/
struct vss_imemory_block_t
{
  uint64_t mem_address;
  /**< Shared memory address of the memory block. The address is virtual
       for virtual memory and physical for physical memory.

       The address must be aligned to LCM (cache_line_size, page_align,
       max_data_width), where the attributes are specified in this command or
       #VSS_IMEMORY_CMD_MAP_PHYSICAL, and LCM = least common multiple. */

  uint32_t mem_size;
  /**< Size of the memory allocated for the memory block.

       @values > 0 bytes

       The amount of memory allocated at mem_address must meet the
       requirement as specified in Section @xref{dox:ShMemGuidelines}.

       The size must be a multiple of page_align, where page_align is
       specified in this command or in #VSS_IMEMORY_CMD_MAP_PHYSICAL. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_block_t */


/** @ingroup cvd_shmem_cmd_map_virtual
  Maps contiguous virtual memory with the CVD. The mapped memory is accessible 
  by all sessions of all CVD modules.

  @apr_msgpayload{vss_imemory_cmd_map_virtual_t}
  @table{weak__vss__imemory__cmd__map__virtual__t}

  @par Memory block (%vss_imemory_block_t)
  @table{weak__vss__imemory__block__t}

  @detdesc 
  The dst_addr in the APR command must be set to the MVM address, and the
  dst_port must be set to APR_NULL_V.

  @return
  #VSS_IMEMORY_RSP_MAP -- Success.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to
  @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMEMORY_CMD_MAP_VIRTUAL ( 0x00011333 )

/* Type definition for vss_imemory_cmd_map_virtual_t. */
typedef struct vss_imemory_cmd_map_virtual_t vss_imemory_cmd_map_virtual_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_cmd_map_virtual_t
@{ */
/* Payload structure for VSS_IMEMORY_CMD_MAP_VIRTUAL.
*/
struct vss_imemory_cmd_map_virtual_t
{
  vss_imemory_block_t block;
  /**< Specifies a single block of contiguous virtual memory. */

  bool_t is_cached;
  /**< Specifies whether memory is cached.

       @values
       - FALSE -- Uncached
       - TRUE  -- Cached @tablebulletend */

  uint16_t cache_line_size;
  /**< Cache line size in bytes. 

       @values 128 */

  uint32_t access_mask;
  /**< Access permission to the memory while it is mapped. 

       @values
       - Bit 0 -- If set, the memory is readable.
       - Bit 1 -- If set, the memory is writable. @tablebulletend */

  uint32_t page_align;
  /**< Page frame alignment in bytes. 

       @values 4096 */

  uint8_t min_data_width;
  /**< Minimum native data type width in bits that can be accessed.

       @values 8 */

  uint8_t max_data_width;
  /**< Maximum native data type width in bits that can be accessed.

       @values 64 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_cmd_map_virtual_t */


/** @ingroup cvd_shmem_cmd_map_physical
  Maps physical memory with the CVD. The mapped memory is accessible by all
  sessions of all CVD modules.

  @apr_msgpayload{vss_imemory_cmd_map_physical_t}
  @table{weak__vss__imemory__cmd__map__physical__t}

  @par Table descriptor (%vss_imemory_table_descriptor_t)
  @tablens{weak__vss__imemory__table__descriptor__t} @newpage

  @par Table format (%vss_imemory_table_t)
  @tablens{weak__vss__imemory__table__t}

  @detdesc
  The dst_addr in the APR command must be set to the MVM address, and the
  dst_port must be set to APR_NULL_V.
  @par
  This command can be used for mapping either contiguous or non-contiguous
  physical memory.
  @par
  The table at the address must have the format specified by
  %vss_imemory_table_t.

  @par Use case 1 -- Map contiguous physical memory
  The table_descriptor field in the command payload specifies the location and
  size of a table that contains information on a single memory block.

  @par Use case 2 -- Map non-contiguous physical memory
  The table_descriptor field in the command payload specifies the location and
  size of a table that contains information on multiple memory blocks. The
  order of the memory blocks provided to the CVD must be the same as when the
  client maps the blocks on its local processor.

  @return
  #VSS_IMEMORY_RSP_MAP -- Success.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to
  @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMEMORY_CMD_MAP_PHYSICAL ( 0x00011334 )

/* Type definition for vss_imemory_table_descriptor_t. */
typedef struct vss_imemory_table_descriptor_t vss_imemory_table_descriptor_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_table_descriptor_t 
@{ */
/** Specifies the location and size of a table that contains information on
  memory blocks.
*/
struct vss_imemory_table_descriptor_t
{
  uint64_t mem_address;
  /**< Shared memory address of the table.

       The address must be aligned to LCM (cache_line_size, page_align,
       max_data_width), where the attributes are specified in
       this command, and LCM = least common multiple. */

  uint32_t mem_size;
  /**< Size of the memory allocated for the table.

       @values > 0 bytes

       The amount of memory allocated at mem_address must meet the
       requirement as specified in Section @xref{dox:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_table_descriptor_t */

/* Type definition for vss_imemory_table_t. */
typedef struct vss_imemory_table_t vss_imemory_table_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_table_t 
@{ */
/** Specifies the format of a table that contains information on memory blocks.
*/
struct vss_imemory_table_t
{
  vss_imemory_table_descriptor_t next_table_descriptor;
  /**< Specifies the next table.
       All of the memory blocks to be mapped are indicated.

       If there is no subsequent table, clear the size of the table to zero;
       in this case, the table address is ignored. */

#if __STDC_VERSION__ >= 199901L
  vss_imemory_block_t blocks[];
    /**< Specifies one or more memory blocks. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_table_t */

/* Type definition for vss_imemory_cmd_map_physical_t. */
typedef struct vss_imemory_cmd_map_physical_t vss_imemory_cmd_map_physical_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_cmd_map_physical_t
@{ */
/* Payload structure for VSS_IMEMORY_CMD_MAP_PHYSICAL.
*/
struct vss_imemory_cmd_map_physical_t
{
  vss_imemory_table_descriptor_t table_descriptor;
  /**< Specifies the location and size of a table that contains information
       for one or more memory blocks. */

  bool_t is_cached;
  /**< Indicates cached or uncached memory.
    
       @values 
       - FALSE -- Uncached
       - TRUE -- Cached @tablebulletend */

  uint16_t cache_line_size;
  /**< Cache line size in bytes.

       @values 128 */

  uint32_t access_mask;
  /**< Permission for the CVD to access the memory while it is mapped.

       @values 
       - Bit 0 -- If set, the memory is readable
       - Bit 1 -- If set, the memory is writable @tablebulletend */

  uint32_t page_align;
  /**< Page frame alignment in bytes.

       @values 4096 */

  uint8_t min_data_width;
  /**< Minimum native data type width in bits that can be accessed.

       @values 8 */

  uint8_t max_data_width;
  /**< Maximum native data type width in bits that can be accessed.

       @values 64 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_cmd_map_physical_t */


/** @ingroup cvd_shmem_resp_map
  Response to the #VSS_IMEMORY_CMD_MAP_VIRTUAL and #VSS_IMEMORY_CMD_MAP_PHYSICAL
  commands.

  @apr_msgpayload{vss_imemory_rsp_map_t}
  @table{weak__vss__imemory__rsp__map__t}

  @return
  None.

  @dependencies
  None.
 */
#define VSS_IMEMORY_RSP_MAP ( 0x00011336 )

/* Type definition for vss_imemory_rsp_map_t. */
typedef struct vss_imemory_rsp_map_t vss_imemory_rsp_map_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_rsp_map_t 
@{ */
/* Payload structure for VSS_IMEMORY_RSP_MAP.
*/
struct vss_imemory_rsp_map_t
{
  uint32_t mem_handle;
  /**< Unique identifier for the shared memory address of the memory
       attributes.

       The client must provide this handle to the CVD when a memory address or
       offset is passed to the CVD for sharing data. Any other value, including
       NULL, cannot be passed. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_rsp_map_t */


/** @ingroup cvd_shmem_cmd_unmap
  Unmaps memory with the CVD.

  @apr_msgpayload{vss_imemory_cmd_unmap_t}
  @table{weak__vss__imemory__cmd__unmap__t}

  @detdesc
  The dst_addr in the APR command must be set to the MVM address and the
  dst_port must be set to APR_NULL_V.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMEMORY_CMD_UNMAP ( 0x00011337 )

/* Type definition for vss_imemory_cmd_unmap_t. */
typedef struct vss_imemory_cmd_unmap_t vss_imemory_cmd_unmap_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imemory_cmd_unmap_t 
@{ */
/* Payload structure for VSS_IMEMORY_CMD_UNMAP.
*/
struct vss_imemory_cmd_unmap_t
{
  uint32_t mem_handle;
  /**< Unique identifier for the shared memory address of the memory attributes
       returned by #VSS_IMEMORY_RSP_MAP. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imemory_cmd_unmap_t */


#endif /* __VSS_IMEMORY_PUBLIC_IF_H__ */

