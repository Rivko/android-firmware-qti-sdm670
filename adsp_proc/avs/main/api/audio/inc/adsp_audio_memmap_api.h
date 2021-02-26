/*========================================================================*/
/**
@file adsp_audio_memmap_api.h

This file contains the structure definitions for the memory map 
commands used in client-ASM,ADM communications.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
Copyright (c) 2010-2014, 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
======================================================================== */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_audio_memmap_api.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4.
08/06/12   sw      (Tech Pubs) Edited comments for Badger; updated Doxygen markup
                   to Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
07/06/10   kk      Created file.
========================================================================== */

#ifndef _ADSP_AUDIO_MEMMAP_API_H_
#define _ADSP_AUDIO_MEMMAP_API_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @addtogroup asm_common
@{ */
/** Enumeration for the shared memory 8_4K pool ID. */
#define ADSP_MEMORY_MAP_SHMEM8_4K_POOL      3

/** Enumeration for the mDSP shared memory 4K pool ID. */
#define ADSP_MEMORY_MAP_MDF_SHMEM_4K_POOL      4

/** @} */ /* end_addtogroup asm_common */

/** @addtogroup asmsvc_cmd_mem_map
@{ */

/** Definition of the is_virtual flag shift value. */
#define ADSP_MEMORY_MAP_SHIFT_IS_VIRTUAL                          0

/** Definition of the is_virtual flag bitmask. */
#define ADSP_MEMORY_MAP_BIT_MASK_IS_VIRTUAL                       (0x00000001UL)

/** Definition of the virtual memory flag. */
#define ADSP_MEMORY_MAP_VIRTUAL_MEMORY                         1

/** Definition of the physical memory flag. */
#define ADSP_MEMORY_MAP_PHYSICAL_MEMORY                        0

/** @} */ /* end_addtogroup asmsvc_cmd_mem_map */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_cmd_shared_mem_map_regions_t
@{ */
/* Memory map regions command payload used by the
    ASM_CMD_SHARED_MEM_MAP_REGIONS and ADM_CMD_SHARED_MEM_MAP_REGIONS commands
    that allow clients to map multiple shared memory regions in a single command. */
/** Immediately following this structure are num_regions of the
    <b>Map payload</b> (%avs_shared_map_region_payload_t).
*/
struct avs_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory ID implicitly defines the characteristics of the memory.
         Characteristics can include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and
         4 KB aligned. */

    uint16_t                  num_regions;
    /**< Number of regions to map.

         @values Any value greater than zero */

    uint32_t                  property_flag;
    /**< Configures one common property for all regions in the payload. No
         two regions in the same #ASM_CMD_SHARED_MEM_MAP_REGIONS or
         #ADM_CMD_SHARED_MEM_MAP_REGIONS can have different properties.

         @values 0x00000000 to 0x00000001

         Bit 0 is the IsVirtual flag that indicates physical or virtual
         mapping:
           - 0 -- The shared memory address provided in
                  avs_shared_map_region_payload_t is a physical address. The
                  shared memory must be mapped (a hardware TLB entry), and a
                  software entry must be added for internal bookkeeping.

         @contcell
         Bit 0 (cont.)
           - 1 -- The shared memory address provided in the map
                  payload[usRegions] is a virtual address. The shared memory
                  must not be mapped (because the hardware TLB entry is already
                  available), but a software entry must be added for internal
                  bookkeeping. \n
                  This bit value (1) is useful if two services within the aDSP
                  are communicating via the APR. They can directly communicate
                  via the virtual address instead of the physical address. The
                  virtual regions must be contiguous.
                
         Bits 31 to 1 are reserved and must be set to 0. */
} 
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_cmd_shared_mem_map_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_shared_map_region_payload_t
@{ */
/* Payload of the map region; used by the avs_cmd_shared_mem_map_regions_t
    structure.
*/
struct avs_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map. This word must be contiguous memory, and it must be
         4&nbsp;KB aligned.

         @values Any 32-bit value */

    uint32_t                  shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map. This word must be contiguous memory, and it must be
         4&nbsp;KB aligned.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit address, bits 31 to 4 must be set to
           0.
         - For a 64-bit shared memory address, any 32-bit value.
         @tablebulletend */
         
    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_shared_map_region_payload_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_cmd_shared_mem_unmap_regions_t
@{ */
/* Payload of the memory unmap regions command used by
    ASM_CMD_SHARED_MEM_UNMAP_REGIONS and ADM_CMD_SHARED_MEM_UNMAP_REGIONS.
*/
struct avs_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through either
         #ASM_CMD_SHARED_MEM_MAP_REGIONS or #ADM_CMD_SHARED_MEM_MAP_REGIONS.

         @values Any 32-bit value

         The aDSP uses this handle to retrieve the associated shared
         memory regions and unmap them. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_cmd_shared_mem_unmap_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_cmdrsp_shared_mem_map_regions_t
@{ */
/* Payload of the memory map command response used by
    ASM_CMDRSP_SHARED_MEM_MAP_REGIONS and ADM_CMDRSP_SHARED_MEM_MAP_REGIONS,
    which allow clients to unmap multiple shared memory regions in a single
    command. The mem_map_handle field can later be used by the client to unmap
    regions that were previously mapped.
*/
struct avs_cmdrsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through either
         #ASM_CMD_SHARED_MEM_MAP_REGIONS or #ADM_CMD_SHARED_MEM_MAP_REGIONS.

         @values Any 32-bit value

         The aDSP uses this handle to retrieve the shared memory attributes.
         This handle can be an abstract representation of the shared memory
         regions that are being mapped. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_cmdrsp_shared_mem_map_regions_t */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AUDIO_MEMMAP_API_H_ */
