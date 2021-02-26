/*========================================================================*/
/**
@file adsp_usm_service_commands.h

@brief This file contains USM service commands and events structures
definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Ultrasound_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Ultrasound_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/api/adsp_usm_service_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/01/13   NR      Add support for family - B memory APIs 
10/06/12   sw      (Tech Pubs) Updated legal statements (lines 20 & 22) for QTI;
                   updated Doxygen markup for D.2 templates.
02/14/12   sw      (Tech Pubs) Edited Doxygen markup for Rev B of the IS.
10/18/11   sw      (Tech Pubs) Edited Doxygen markup/comments for new Interface Spec.
10/13/10   NR      File created

========================================================================== */
 
#ifndef _ADSP_USM_SERVICE_COMMANDS_H_
#define _ADSP_USM_SERVICE_COMMANDS_H_

#include "mmdefs.h"
#include "adsp_audio_memmap_api.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup usm_macros
@{ */
/** Stream ID bitmask. */
#define USM_BIT_MASK_STREAM_ID                                  (0x000000FFUL)

/** Stream ID shift value. */
#define USM_SHIFT_STREAM_ID                                     (0)

/** Session ID bitmask. */
#define USM_BIT_MASK_SESSION_ID                                 (0x0000FF00UL)

/** Session ID shift value. */
#define USM_SHIFT_SESSION_ID                                    (8)

/** Service ID bitmask. */
#define USM_BIT_MASK_SERVICE_ID                                 (0x00FF0000UL)

/** Service ID shift value. */
#define USM_SHIFT_SERVICE_ID                                    (16)

/** Domain ID bitmask. */
#define USM_BIT_MASK_DOMAIN_ID                                 (0xFF000000UL)

/** Domain ID shift value. */
#define USM_SHIFT_DOMAIN_ID                                     (24)

/** @} */ /* end_addtogroup usm_macros */

/** @ingroup usm_svc_cmd_mem_map_regions
    Commands the USM to map multiple shared memory regions.
    All the mapped regions must be from the same memory pool.

    @apr_hdr_fields
      Opcode -- USM_CMD_SHARED_MEM_MAP_REGIONS\n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      The USM map multiple shared region command uses the same
      message payload structure as the audio memory map regions
      command. For more information, refer to
      @xhyperref{Q6,[Q6]}.\n
      @tablespace
      @structure{avs_cmd_shared_mem_map_regions_t}
      @tablespace
      @inputtable{USM_CMD_MEM_MAP_REGIONS_APR_msg_payload.tex}
 
    @mappayload
      @structure{avs_cmd_shared_mem_map_regions_t}
      @tablespace
      @inputtable{USM_CMD_MEM_MAP_REGIONS_MapPayload.tex}

    @prerequisites
      All regions to be mapped must be from the same memory pool.

    @return
      USM_CMDRSP_SHARED_MEM_MAP_REGIONS

    API_developer_note: A-family APIs USM_CMD_MEMORY_MAP and 
    USM_CMD_MEMORY_UNMAP are replaced by
    USM_CMD_SHARED_MEM_MAP_REGIONS &
    USM_CMD_SHARED_MEM_UNMAP_REGIONS in B family. Also
    USM_CMDRSP_SHARED_MEM_MAP_REGIONS is introduced to return
    the mmhandle to the clients.

    @newpage
*/
/* Q6 = Hexagon Multimedia: Audio API Interface Spec (80-N3007-1)
   Q3 = Asynchronous Packet Router API (for MDM9x15 and Newer) Interface Spec
   (80-N1463-2) */
#define USM_CMD_SHARED_MEM_MAP_REGIONS                    0x00012728

/* USM memory map regions command structure is the same as the
   AVS memory map regions command structure. */
typedef struct  avs_cmd_shared_mem_map_regions_t usm_cmd_shared_mem_map_regions_t;

/**
 * This command returns the memory map handle for an USM_CMD_SHARED_MEM_MAP_REGIONS command
 * @relevant_apr_hdr_fields
      Opcode -- USM_CMD_SHARED_MEM_UNMAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmdrsp_shared_mem_map_regions_t}

    @prerequisites
      USM_CMD_SHARED_MEM_MAP_REGIONS.

    @return
      none
 */
#define USM_CMDRSP_SHARED_MEM_MAP_REGIONS     0x00012729

/* USM memory map regions response payload is same as AVS memory map regions payload. */
typedef struct avs_cmdrsp_shared_mem_map_regions_t usm_cmdrsp_shared_mem_map_regions_t;

/** @ingroup usm_svc_cmd_mem_unmap_regions
    Commands the USM to unmap multiple shared memory regions
    that were previously mapped.

    @relevant_apr_hdr_fields
      Opcode -- USM_CMD_SHARED_MEM_UNMAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmd_shared_mem_unmap_regions_t}
      @tablespace
      @inputtable{USM_CMD_MEM_UNMAP_REGIONS_APR_msg_payload.tex}

    @vertspace{9}
    @unmappayload
      @structure{avs_cmd_shared_mem_unmap_regions_t}
      @tablespace
      @inputtable{USM_CMD_MEM_UNMAP_REGIONS_Unmap_payload.tex}

    @prerequisites
      USM_CMD_SHARED_MEM_MAP_REGIONS.

*/
#define USM_CMD_SHARED_MEM_UNMAP_REGIONS              0x0001272A

/* USM memory unmap regions command structure is the same as the
    AVS memory unmap regions command structure. */
typedef struct avs_cmd_shared_mem_unmap_regions_t usm_cmd_shared_mem_unmap_regions_t;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_USM_SERVICE_COMMANDS_H_ */
