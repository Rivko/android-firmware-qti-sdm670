/*========================================================================*/
/**
@file EliteMem_Util.h

@brief This file declares utility functions to manage shared memory between
the Scorpion and Hexagon processors, including physical to virtual address mapping, etc.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
Copyright (c) 2009-2010, 2012-2015 Qualcomm Technologies, Inc. All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup/comments from 2.4.
03/30/13   sw      (Tech Pubs) Updated Doxygen comments and markup for 2.0.
03/20/12   sw      (Tech Pubs) Updated Doxygen comments and markup for 1.2.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/09   DC      Created file.

========================================================================== */
#ifndef _ELITEMEM_UTIL_H_
#define _ELITEMEM_UTIL_H_


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

// Shared Memeory
#if defined(__qdsp6__)
//System
#include "qube.h"
#endif

#include "EliteAprIf.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** @ingroup eliteshm_datatypes
  Defines a 32-byte-aligned portion of a shared memory region.
 */
typedef struct /** @cond */ shared_memory_map_struct /** @endcond */
{
   qurt_mem_region_t    unMemMapClient;
   /**< QuRT_Elite memory map client. */

   uint32_t           unMemMapHandle;
   /**< QuRT_Elite memory map handle. */

   uint32_t           unPhysAddrLsw;
   /**< Lower 32 bits of the physical shared memory address of this region. */
   /* ToDo: change it to unShmAddrLsw*/

   uint32_t           unPhysAddrMsw;
   /**< Upper 32 bits of the physical shared memory address of this region.
         
        The 64-bit address formed by unPhysAddrLsw and unPhysAddrMsw must be
        aligned to 32 bytes. */
   /* ToDo: change it to unShmAddrMsw*/

   uint32_t           unVirtAddr;
   /**< Virtual address of this region. */

   uint32_t           unMemSize;
   /**< Memory size of this region. */

} elite_mem_shared_memory_map_t;




/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/** @ingroup eliteshm_func_get_attrib
  Maps the physical address of a shared memory buffer to a virtual address.
  The memory must be physically contiguous.

  @datatypes
  #elite_mem_shared_memory_map_t

  @param[in]  unPhysAddrLsw        Physical address(LSW) of the shared memory.
  @param[in]  unPhysAddrMsw        Physical address(MSW) of the shared memory.
  @param[in]  unMemSize            Size of the shared memory.
  @param[out] pShareMemMapTypeNode Pointer to the virtual memory address and
                                   other memory-related attributes that are
                                   needed for later memory management, such
                                   as release.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_map_get_shm_attrib(uint32_t unPhysAddrLsw,
                                    uint32_t unPhysAddrMsw,
                                    uint32_t unMemSize,
                                    elite_mem_shared_memory_map_t *pShareMemMapTypeNode );


/** @ingroup eliteshm_func_get_attrib_ref_ct
  Maps the physical address of a shared memory buffer to a virtual address, and
  increments the reference counter to the mapped shared memory (memory map
  handle). The memory must be physically contiguous.

  @datatypes
  #elite_mem_shared_memory_map_t

  @param[in]  unPhysAddrLsw        Physical address(LSW) of the shared memory.
  @param[in]  unPhysAddrMsw        Physical address(MSW) of the shared memory.
  @param[in]  unMemSize            Size of the shared memory.
  @param[out] pShareMemMapTypeNode Pointer to the virtual memory address and
                                   other memory-related attributes that are
                                   needed for later memory management, such
                                   as release.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_map_get_shm_attrib_ref_counted(uint32_t unPhysAddrLsw,
                                    uint32_t unPhysAddrMsw,
                                    uint32_t unMemSize,
                                    elite_mem_shared_memory_map_t *pShareMemMapTypeNode );

/** @ingroup eliteshm_func_release
  Releases the shared memory owned by decrementing the reference counter.

  @datatypes
  #elite_mem_shared_memory_map_t

  @param[in] pShareMemMapTypeNode   Pointer to the memory region.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_map_release_shm(elite_mem_shared_memory_map_t *pShareMemMapTypeNode );


/** @ingroup eliteshm_func_flush_cache
  Flushes the memory region from the cache.

  @datatypes
  #elite_mem_shared_memory_map_t

  @param[in] pSharedMemMapTypeNode  Pointer to the memory region.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_flush_cache( elite_mem_shared_memory_map_t *pSharedMemMapTypeNode );


/** @ingroup eliteshm_func_invalidate_cache
  Invalidates the memory region from the cache.

  @datatypes
  #elite_mem_shared_memory_map_t

  @param[in] pSharedMemMapTypeNode  Pointer to the memory region.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_invalidate_cache( elite_mem_shared_memory_map_t *pSharedMemMapTypeNode );

/** @ingroup eliteshm_func_map_cmd_hdlr
  Processes the following commands:
  - ASM_CMD_SHARED_MEM_MAP_REGIONS
    (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})
  - ADM_CMD_SHARED_MEM_MAP_REGIONS
    (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})
  - AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS
    (refer to @xrefcond{Q11,80-NF774-3,80-NA610-3})
  - VOICE_CMD_SHARED_MEM_MAP_REGIONS
    (refer to @xrefcond{Q10,80-NF774-2,80-NA610-2})
                                     
  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] nMemMapClient   QuRT_Elite memory map client.
  @param[in] hAprHandle      APR handle the caller uses to send the command
                             response.
  @param[in] pPkt            Pointer to the APR packet. 
  @param[in] RespOpcode      Opcode of the shared map regions command response.

  @return
  ADSPResult -- Error code.

  @dependencies
  None.
*/
ADSPResult elite_mem_shared_memory_map_regions_cmd_handler(uint32_t nMemMapClient,
                                      elite_apr_handle_t hAprHandle,
                                      elite_apr_packet_t *pPkt,
                                      uint32_t RespOpcode);


/** @ingroup eliteshm_func_unmap_cmd_hdlr
  Processes the following commands:
  - ASM_CMD_SHARED_MEM_UNMAP_REGIONS
    (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})
  - ADM_CMD_SHARED_MEM_UNMAP_REGIONS
    (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})
  - AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS
    (refer to @xrefcond{Q11,80-NF774-3,80-NA610-3})
  - VOICE_CMD_SHARED_MEM_UNMAP_REGIONS
    (refer to @xrefcond{Q10,80-NF774-2,80-NA610-2})

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] nMemMapClient   QuRT_Elite memory map client.
  @param[in] hAprHandle      APR handle the caller uses to send the command
                             response.
  @param[in] pPkt            Pointer to the APR packet.

  @return
  ADSPResult -- Error code.

  @dependencies
  None.
*/
ADSPResult elite_mem_shared_memory_un_map_regions_cmd_handler(uint32_t nMemMapClient,
                                        elite_apr_handle_t hAprHandle,
                                        elite_apr_packet_t *pPkt);


/** @ingroup eliteshm_func_reg_lpm_core
  Registers the LPM core with aDSP Power Management (ADSPPM).
       
  @param[in,out] lpm_core_id_ptr  Pointer to the LPM core ID returned by
                                  MMPM_Register_Ext.
   
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_register_lpm_core(uint32_t* lpm_core_id_ptr );
                                        

/** @ingroup eliteshm_func_unreg_lpm_core
  Unregisters the LPM core with ADSPPM.
   
  @param[in,out] lpm_core_id_ptr  Pointer to the LPM core ID returned by
                                  MMPM_Register_Ext.
   
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_unregister_lpm_core(uint32_t* lpm_core_id_ptr );
                               
                               
/** @ingroup eliteshm_func_req_rel_lpm_pwr
  Requests or releases the LPM power.
   
  @param[in] core_id_ptr       Pointer to LPM core ID returned by
                               MMPM_Register_Ext.
  @param[in] is_release_flag   Flag that indicates if power is to be released
                               or requested.
                               - FALSE -- Request the LPM power resource.
                               - TRUE -- Release the power resource.
                               @tablebulletend
   
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_req_rel_lpm_power(uint32_t *core_id_ptr, bool_t is_release_flag);


/** @ingroup eliteshm_func_req_rel_lpm_bw
  Requests or relinquishes the LPM bandwidth.

  @param[in] core_id_ptr       Pointer to the LPM core ID returned by
                               MMPM_Register_Ext.
  @param[in] is_release_flag   Flag that indicates if power is to be released
                               or requested.
                               - FALSE -- Request the LPM power resource.
                               - TRUE -- Release the power resource.
                               @tablebulletend

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_req_rel_lpm_bw(uint32_t *core_id_ptr,bool_t is_release_flag);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _ELITEMEM_UTIL_H_
