/*========================================================================*/
/**
 @file adsp_audproc_api.h

 This file contains the structure definitions for Audio Post Processing
 Topology IDs, Module IDs, Parameter IDs.
 */
/*===========================================================================
 NOTE: The @brief description above does not appear in the PDF.
 The description that displays in the PDF is maintained in the
 audio_mainpage.dox file. Contact Tech Pubs for assistance.
 ===========================================================================*/
/*===========================================================================
  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
============================================================================ */
/*========================================================================
 Edit History

 $Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_audproc_api.h#1 $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 08/20/17   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8 (SDM845).
 03/21/17   pag     Removed Doxygen comments and added h2xml comments for modules.
                    Kept those modules in separate file and corresponding header
                    files are included here.
 07/23/16   HB       Moved properiatery information.
 07/18/16   sw      (Tech Pubs) Merged Doxygen comments from 8953.2.7;
                    edited Doxygen comments for AVS 2.8 (MSM8998(.
 10/22/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
 05/08/15   sw      (Tech Pubs) Edited Doxygen comments for 8952.2.6.
 09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
 09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0; edited
                    new Doxygen comments for 2.2.
 08/03/12   sw      (Tech Pubs) Edited Doxygen comments for 2.0 updates;
                    updated Doxygen markup to Dox 1.7.6.1.
 02/08/12   sw      (Tech Pubs) Updated Doxygen comments based on 06.110 branch.
 05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
 04/22/11   ss      Introducing Audproc API file that would hold all post processing Module IDs,
					Parameter IDs and Topology IDs
 ========================================================================== */

#ifndef _ADSP_AUDPROC_API_H_
#define _ADSP_AUDPROC_API_H_

#include "mmdefs.h"
#include "adsp_audproc_api_prop.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "chmixer_calib.h"
#include "mfc_calib.h"
#include "resampler_calib.h"
#include "rx_codec_gain_ctrl_calib.h"
#include "sampleslip_calib.h"
#include "tx_mic_gain_ctrl_calib.h"
#include "vol_ctrl_calib.h"
#include "audio_common_enable_calib.h"

/** @addtogroup audiopp_topos_asm
@{ */

/** ID of the no-default audio postprocessing topology.

    @inputfig{1,topo_id_asm_0x00010C68_postproc.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL)
 */
#define ASM_STREAM_POSTPROC_TOPO_ID_NONE                            0x00010C68

/** ID of the no-default audio preprocessing topology.

    @inputfig{1,topo_id_asm_0x00010C68_preproc.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL)
 */
#define ASM_STREAM_PREPROC_TOPO_ID_NONE      ASM_STREAM_POSTPROC_TOPO_ID_NONE

/** @} *//* end_addtogroup audiopp_topos_asm */

/** @addtogroup audiopp_topos_adm
@{ */
/** ID of the no-default COPP audio postprocessing device topology
    (indicates None).

    @inputfig{1,topo_id_adm_0x00010312.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL)
    - Sample Slip               (AUDPROC_MODULE_ID_SAMPLESLIP)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP              0x00010312

/** @} */ /* end_addtogroup audiopp_topos_adm */

typedef struct audproc_module_info_t audproc_module_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_module_info_t
@{ */
/* Defines the modules used by the ASM_CMD_ADD_TOPOLOGIES and
 ADM_CMD_ADD_TOPOLOGIES_V5 commands.
 */
struct audproc_module_info_t
{
   uint32_t module_id;
   /**< ID of the module to be created.

        @values Any valid module ID */

   uint32_t use_lpm;
   /**< Specifies whether to put this module in low power memory.

        @values
        - 0 -- Do not put this module in low power memory
        - 1 -- Put this module in low power memory

        If there is insufficient low-power memory, this module is placed in
        the default memory pool.*/

   uint8_t init_params[16];
   /**< Array of initialization parameters passed to the module when it is
        created.

        This array of 16 bytes is sent to the getsize() and init() methods of
        the module. The interpretation of this information is module specific. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_audproc_module_info_t */

typedef struct audproc_topology_definition_t audproc_topology_definition_t;

#include "adsp_begin_pack.h"

/** @ingroup asmsvc_cmd_add_topos
 Maximum number of modules supported in a custom topology.
 */
#define AUDPROC_MAX_MODULES_PER_TOPOLOGY 16

/** @weakgroup weak_audproc_topology_definition_t
@{ */
/* Defines a custom topology that is to be added to the aDSP. This structure
 is followed by num_modules of audproc_module_info_t.
 */
struct audproc_topology_definition_t
{
   uint32_t topology_id;
   /**< ID of this custom topology.

        @values 0x10000000 through 0x1FFFFFFF */

   uint32_t num_modules;
   /**< Number of modules in this topology.

        @values 0 to #AUDPROC_MAX_MODULES_PER_TOPOLOGY */

   audproc_module_info_t module_info[AUDPROC_MAX_MODULES_PER_TOPOLOGY];
   /**< Array of module information structures with 16 elements.

        The order of the elements in this array specifies the order in which the
        modules are connected in the topology, from source to sink.

        Only the first num_modules elements of this array are required.
        The rest of the elements are ignored. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_audproc_topology_definition_t */

typedef struct audproc_custom_topologies_t audproc_custom_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_custom_topologies_t
@{ */
/* Header structure for the custom topologies command buffer. This structure
 is followed by num_topologies of audproc_topology_definition_t.
 */
struct audproc_custom_topologies_t
{
   uint32_t num_topologies;
   /**< Specifies the number of topologies in this command.

        @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_audproc_custom_topologies_t */

/* Payload format for pp topology info data. */
typedef struct audproc_topology_info_t audproc_topology_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_topology_info_t
@{ */
/* Payload of the ADM_CMD_GET_PP_TOPO_MODULE_LIST, ADM_CMD_GET_PP_TOPO_MODULE_LIST_V2
    and ASM_CMD_GET_PP_TOPO_MODULE_LIST, ASM_CMD_GET_PP_TOPO_MODULE_LIST_V2 commands.
 */
struct audproc_topology_info_t
{
   uint32_t num_modules;
   /**< Number of modules in the topology. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_audproc_topology_info_t */

/* Payload format for pp topology module id info data. */
typedef struct audproc_topology_module_instance_info_t audproc_topology_module_instance_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_topology_module_instance_info_t
@{ */
/* Payload of the ADM_CMD_GET_PP_TOPO_MODULE_LIST_V2
    and ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2 commands.
*/
struct audproc_topology_module_instance_info_t
{
    uint32_t module_id;
    /**< Valid ID of the module. */

    uint16_t instance_id;
    /**< Valid ID of the module instance. */

    uint16_t reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_audproc_topology_module_instance_info_t */


typedef struct audproc_event_response_payload_header_t audproc_event_response_payload_header_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_audproc_event_response_payload_header_t
@{ */
/* Response payload related to any event registered with PP module.
    An event is prefixed with this structure before it is sent to client.
    Immediately following this payload are event-specific payloads.
*/
struct audproc_event_response_payload_header_t
{
   uint32_t event_id;
   /**< Event ID for the payload that is generated. */

   uint32_t module_id;
   /**< ID of the module that raised the response. */

   uint16_t instance_id;
   /**< Instance ID of the module that raised the response. */

   uint16_t reserved;
   /**< Used for alignment; must be set to 0. */

   uint32_t payload_size;
   /**< Size of the payload that is generated. */

}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_audproc_event_response_payload_header_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AUDPROC_API_H_ */
