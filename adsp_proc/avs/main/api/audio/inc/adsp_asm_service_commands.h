/*======================================================================== */
/**
@file adsp_asm_service_commands.h

This file contains ASM service commands and events structures definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_asm_service_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/22/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0.
08/06/12   sw      (Tech Pubs) Edited comments; updated Doxygen markup to
                   Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.
========================================================================== */

#ifndef _ADSP_ASM_SERVICE_COMMANDS_H_
#define _ADSP_ASM_SERVICE_COMMANDS_H_

#include "mmdefs.h"
#include "adsp_audio_memmap_api.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup asmsvc_cmd_mem_map
    Commands the ASM to map multiple shared memory regions.
    All the mapped regions must be from the same memory pool.

  @apr_hdr_fields
    Opcode -- ASM_CMD_SHARED_MEM_MAP_REGIONS
    @par
    Dst_port -- 0000 (Session ID/Stream ID = 0/0)

  @apr_msgpayload{avs_cmd_shared_mem_map_regions_t}
    @tablens{weak__avs__cmd__shared__mem__map__regions__t}

  @mappayload{avs_shared_map_region_payload_t}
    @table{weak__avs__shared__map__region__payload__t}

  @return
    #ASM_CMDRSP_SHARED_MEM_MAP_REGIONS.

  @dependencies
    None. @newpage
*/
/* API_developer_note: A family APIs ASM_CMD_MEMORY_MAP, ASM_CMD_MEMORY_UNMAP,
   ASM_CMD_MEMORY_MAP_REGIONS, ASM_CMD_MEMORY_UNMAP_REGIONS are replaced by
   ASM_CMD_SHARED_MEM_MAP_REGIONS & ASM_CMD_SHARED_MEM_UNMAP_REGIONS in
   B family.
   Also, ASM_CMDRSP_SHARED_MEM_MAP_REGIONS is introduced to return the mmhandle
   to the clients.
*/
#define ASM_CMD_SHARED_MEM_MAP_REGIONS               0x00010D92

/* ASM memory map regions command structure is the same as the
    AVS memory map regions command structure. */
typedef struct  avs_cmd_shared_mem_map_regions_t asm_cmd_shared_mem_map_regions_t;

/** @ingroup asmsvc_resp_mem_map
    Returns the memory map handle in response to an
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.

   @apr_hdr_fields
    Opcode -- ASM_CMD_SHARED_MEM_UNMAP_REGIONS
    @par
    Dst_port -- 0000 (Session ID/Stream ID = 0/0)

  @apr_msgpayload{avs_cmdrsp_shared_mem_map_regions_t}
    @table{weak__avs__cmdrsp__shared__mem__map__regions__t}

  @return
    None.

  @dependencies
    An #ASM_CMD_SHARED_MEM_MAP_REGIONS command must have been issued.
*/
#define ASM_CMDRSP_SHARED_MEM_MAP_REGIONS     0x00010D93

/* ASM memory map regions response payload is same as AVS memory map regions payload. */
typedef struct avs_cmdrsp_shared_mem_map_regions_t asm_cmdrsp_shared_mem_map_regions_t;

/** @ingroup asmsvc_cmd_mem_unmap
    Commands the ASM to unmap multiple shared memory regions that were
    previously mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.

  @apr_hdr_fields
    Opcode -- ASM_CMD_SHARED_MEM_UNMAP_REGIONS
    @par
    Dst_port -- 0000 (Session ID/Stream ID = 0/0)

  @apr_msgpayload{avs_cmd_shared_mem_unmap_regions_t}
    @table{weak__avs__cmd__shared__mem__unmap__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    A corresponding #ASM_CMD_SHARED_MEM_MAP_REGIONS command must have been
    issued.
*/
#define ASM_CMD_SHARED_MEM_UNMAP_REGIONS              0x00010D94

/* ASM memory unmap regions command structure is the same as the
    AVS memory unmap regions command structure. */
typedef struct avs_cmd_shared_mem_unmap_regions_t asm_cmd_shared_mem_unmap_regions_t;

/** @ingroup asmsvc_cmd_add_topos
    Defines custom topologies in the aDSP for use in POPP and POPreP. These
    topologies are created by specifying the topology IDs in the
    #ASM_STREAM_CMD_OPEN_READ_V3 command.

  @apr_hdr_fields
    Opcode -- ASM_CMD_ADD_TOPOLOGIES
    @par
    Dst_port -- 0

  @apr_msgpayload{asm_cmd_add_topologies_t}
    @table{weak__asm__cmd__add__topologies__t}

  @par Buffer format (audproc_custom_topologies_t)
    Immediately following this structure are num_topologies of the
    <b>Topology definitions</b> (%audproc_topology_definition_t) that are to
    be added to the aDSP.
    @tablens{weak__audproc__custom__topologies__t}
    Calculation of the buffer size:
    - Size of the module information structure = 4 + 4 + 16 = 24 bytes
    - Size of the topology definition structure = 4 + 4 + 16 * (size of the
      module information structure) = 392 bytes
    - Size of the buffer = 4 + num_topologies * 392 @newpage

  @par Topology definition (audproc_topology_definition_t)
    Immediately following this structure are num_modules of the Module
    information structure (%audproc_module_info_t).
    @tablens{weak__audproc__topology__definition__t}

  @par Module information (audproc_module_info_t)
    @table{weak__audproc__module__info__t}

  @detdesc 
    The aDSP stores custom topology definitions internally. The
    definitions sent to the ASM are available only for the POPP and POPreP.
    @par
    This command must be sent before opening a session with the custom
    topologies. It can be sent multiple times to add new topologies; the ASM
    supports up to a total of 128 custom topologies. If a subsequent command
    redefines a previously defined topology ID, the previous topology
    definition is replaced with the new one. However, the changes take effect
    only when the next session is created. @newpage

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client as input.

  @dependencies
    None.
*/
#define ASM_CMD_ADD_TOPOLOGIES                                0x00010DBE

/* The payload for the ASM_CMD_ADD_TOPOLOGIES command */
typedef struct asm_cmd_add_topologies_t asm_cmd_add_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_cmd_add_topologies_t
@{ */
/* Payload of the ASM_CMD_ADD_TOPOLOGIES command.
*/
struct asm_cmd_add_topologies_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the 64-bit topology buffer address. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the 64-bit topology buffer address. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  buffer_size;
    /**< Size in bytes of the valid data in the topology buffer. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_cmd_add_topologies_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_ASM_SERVICE_COMMANDS_H_ */
