#ifndef _ADSP_AVCS_API_H_
#define _ADSP_AVCS_API_H_

/**
@file adsp_core_api.h

@brief This file contains the structure definitions for the AVCS
commands used in client-AVCS communications.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      AV_Core_Service_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/avcs/inc/adsp_core_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/31/17   sw      (Tech Pubs) Updated 845 Doxygen comments for AVS 2.8
10/26/16   sw      (Tech Pubs) Updated Doxygen comments for AVS 2.8
03/02/16   rr      Merged doxy markup/comments from 2.7
12/08/14   rr      Merged Doxy markup/comments from 2.6
02/27/13   sw      (Tech Pubs) Updated doxy markup/comments for 2.2 PDF.
01/03/12   AP      Created the file
11/30/11   sw      (Tech Pubs) Updated doxy markup/comments for PDF (Rev D)
08/01/11   AP       Added ADSP_CMD_SET_PERIODIC_PROFILING_EVENTS_3 and
                    ADSP_EVENT_PERIODIC_PROFILING_3 API
06/27/11   AZ       Added ADSP_EVENT_GET_STATE and ADSP_EVENT_STATE API
04/27/11   leo     (Tech Pubs) Updated doxy comments for Interface Spec.
12/08/10   VI       Added Remote Bus BW and Power Collapse API
09/22/10   SD      (Tech Pubs) Edited Doxygen markup and comments.
05/21/10   KK      Created file.
========================================================================== */

#include "mmdefs.h"
#include "adsp_core_version.h"
#include "adsp_avs_build_version.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @addtogroup avcs_deprec_versions
@{ */
/** Deprecated. Use #AVS_BUILD_MAJOR_VERSION, #AVS_BUILD_MINOR_VERSION,
  #AVS_BUILD_BRANCH_VERSION, and #AVS_BUILD_SUBBRANCH_VERSION.

  Queries the AVS for the build version and versions of static services.

  @apr_hdr_fields
    Opcode -- AVCS_GET_VERSIONS

  @apr_msgpayload
    None.

  @return
    #AVCS_GET_VERSIONS_RSP

  @dependencies
    None. @newpage
*/
#define AVCS_GET_VERSIONS       0x00012905

/** Deprecated. Use #AVS_BUILD_MAJOR_VERSION, #AVS_BUILD_MINOR_VERSION
  #AVS_BUILD_BRANCH_VERSION, and #AVS_BUILD_SUBBRANCH_VERSION.

  Used by the AVCS to respond to an #AVCS_GET_VERSIONS request.

  @apr_hdr_fields
    Opcode -- AVCS_GET_VERSIONS_RSP

  @apr_msgpayload{avcs_service_cmd_rsp_get_api_versions_t}
    @tablens{weak__avcs__service__cmd__rsp__get__api__versions__t}

  @par Service information (avcs_svc_info)
    @table{weak__avcs__svc__info}

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_GET_VERSIONS_RSP   0x00012906

/** Deprecated. Use #AVCS_API_VERSION and #AVCS_API_BRANCH_VERSION.

  Version of the AVCS core module.
*/
#define AVCS_CORE_VERSION   0x00040002

typedef struct avcs_service_cmd_rsp_get_api_versions_t avcs_service_cmd_rsp_get_api_versions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_service_cmd_rsp_get_api_versions_t
@{ */
/* Deprecated. Payload of the AVCS_GET_VERSIONS_RSP command response, which
    provides a listing of service IDs and their API version numbers. */
/** Immediately following this structure are num_services instances of
    %avcs_svc_info (see <b>Service information</b>).
*/
struct avcs_service_cmd_rsp_get_api_versions_t
{

    uint32_t                  build_id;
    /**< Must be set to zero. */
    /* This field might have some meaningful nonzero value once the
       multiprocess design is implemented. */

    uint32_t                  num_services;
    /**< Number of supported services in the current build. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_service_cmd_rsp_get_api_versions_t */

typedef struct avcs_svc_info avcs_svc_info;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_svc_info
@{ */
/* Deprecated. For each instance in the AVCS_GET_VERSIONS_RSP payload, this
  structure contains per-service information.
*/
struct avcs_svc_info
{

    uint32_t                  service_id;
    /**< Service ID.

         @values 0 to 255 */

    uint32_t                  version;
    /**< Service API version.

         @values
         - Bits 0 to 15 -- Minor version
         - Bits 16 to 31 -- Major version @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_svc_info */
/** @} *//* end_addtogroup avcs_deprec_versions */


/** @ingroup core_evt_get_state
  Used by AVS clients to query the AVCS for the readiness of AVS static
  services and availability of modules. If the AVCS itself is not ready (it is
  not registered with the APR), the event is discarded in the APR layer.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_GET_STATE

  @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_EVENT_GET_STATE   0x0001290C

/** @ingroup core_resp_evt_state
  Used by the AVCS to respond to an #AVCS_EVENT_GET_STATE request.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_STATE

  @apr_msgpayload{avcs_event_state_t}
    @table{weak__avcs__event__state__t}

  @description
    When all static services are ready, the payload is set to 0x1. If a static
    service is not ready, the AVCS either does not respond or it responds with
    a payload other than 0x1.
    @par
    AVS clients can periodically send #AVCS_EVENT_GET_STATE to the AVCS at
    start up. If the AVS services are up and ready, the AVCS must respond
    with AVCS_EVENT_STATE that has a payload of 0x1.
    @par
    If AVS services are not up and ready, the AVCS cannot respond or it
    might respond with AVCS_EVENT_STATE that has a payload other than 0x1 or
    0x5. The clients must be able to handle this behavior appropriately.
    @par
    If the AVCS service (static service) is not ready (it is not registered
    with the APR), the APR discards the AVCS_EVENT_GET_STATE message.
    @par
    After receiving AVCS_EVENT_STATE with a payload of 0x1, the clients
    do the following:
    - Stop sending AVCS_EVENT_GET_STATE
    - Ignore any further responses (which might be latent responses for
      previous requests)
    - Execute the AVS static services
    However, if modules are required, the client must continue to query until
    the status becomes 0x5.
    @par
    Currently, the interface supports a query of the overall readiness of all
    AVS static services. The interface does not support a query of individual
    service readiness (future enhancement). @newpage

  @return
    None.

  @dependencies
    The client must have sent #AVCS_EVENT_GET_STATE.
*/
#define AVCS_EVENT_STATE   0x0001290D

typedef struct avcs_event_state_t avcs_event_state_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_event_state_t
@{ */
/* Payload of the AVCS_EVENT_STATE event, which responds to an
    AVCS_EVENT_GET_STATE event sent by clients.
*/
struct avcs_event_state_t
{

    uint32_t              state;
    /**< Indicates the AVS state.

         @values
         - 0x1 -- AVS services are up and ready, and static modules are
           available for use
         - 0x5 -- AVS services are up and ready, and all modules are available
           for use
         - Other values -- Reserved for future use; ignore any other values
         @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_event_state_t */

/** @ingroup core_cmd_request_remote_avt_vote
  Handles audio-video timer (avtimer) vote requests from clients.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST

  @apr_msgpayload{avcs_cmd_remote_avtimer_vote_request}
    @table{weak__avcs__cmd__remote__avtimer__vote__request}

  @description
    Clients can use this command to request the avtimer be enabled.
    Remote clients can access the avtimer only after this vote request.
    @par
    The AVS returns a handle to the avtimer in response to this command.
    The client must ensure that it votes the avtimer only once. 
    @par
    After the use case finishes, the client must request
    #AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST to release the avtimer.
    @par
    The client must not send multiple vote request commands. If the client
    sends multiple vote commands, the same avtimer handle is sent in
    response.

  @return
    #AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST

  @dependencies
    None.
*/
#define AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST              0x00012914

typedef struct avcs_cmd_remote_avtimer_vote_request avcs_cmd_remote_avtimer_vote_request_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_remote_avtimer_vote_request
@{ */
/* Payload of the AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST command.
*/
struct avcs_cmd_remote_avtimer_vote_request
{
   char client_name[8];
   /**< Client name, up to a maximum of eight characters.
        The avtimer driver takes the first six characters. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_remote_avtimer_vote_request */

/** @ingroup core_cmd_request_remoteavt_vote
  Sends the handle of the avtimer in response to an
  #AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST command.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST

  @apr_msgpayload{avcs_cmd_rsp_remote_avtimer_vote_request}
    @table{weak__avcs__cmd__rsp__remote__avtimer__vote__request}

  @description
    After the use case finishes, the client must request
    #AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST to release the avtimer.

  @return
    None.

  @dependencies
    The client must have sent #AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST.
*/
#define AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST              0x00012915

typedef struct avcs_cmd_rsp_remote_avtimer_vote_request avcs_cmd_rsp_remote_avtimer_vote_request_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_rsp_remote_avtimer_vote_request
@{ */
/* Payload of the AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST command.
*/
struct avcs_cmd_rsp_remote_avtimer_vote_request
{
   uint32_t avtimer_handle;
   /**< Handle of the avtimer. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_rsp_remote_avtimer_vote_request */

/** @ingroup core_cmd_request_remavt_release
  Handles client requests to disable the avtimer. Clients must send the valid
  avtimer handle.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST

  @apr_msgpayload{avcs_cmd_remote_avtimer_release_request}
    @table{weak__avcs__cmd__remote__avtimer__release__request}

  @return
    None.

  @dependencies
    The client must have sent #AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST.
*/
#define AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST              0x00012916

typedef struct avcs_cmd_remote_avtimer_release_request avcs_cmd_remote_avtimer_release_request_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_remote_avtimer_release_request
@{ */
/* Payload of the AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST command.
*/
struct avcs_cmd_remote_avtimer_release_request
{
   uint32_t avtimer_handle;
   /**< Handle of the avtimer. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_remote_avtimer_release_request */

/** @ingroup core_cmd_register_topos
  Registers custom topologies in the aDSP for use in the Audio, Voice, AFE,
  and LSM services.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_REGISTER_TOPOLOGIES
    @par
    Dst_port -- 0

  @apr_msgpayload{avcs_cmd_register_topologies_t}
    @table{weak__avcs__cmd__register__topologies__t}

  @par Topology information (avcs_topology_definition_t)
    @table{weak__avcs__topology__definition__t}

  @par Number of topologies (avcs_topology_buffer_t)
    @table{weak__avcs__topology__buffer__t} @vertspace{.5}
    @par
    In memory, the following order must be maintained:
    @par
    @indent{12} avcs_topology_buffer_t (num_topologies provided here) \n
    @indent{12} (avcs_topology_definition_t (num_modules provided here)) -- for
    topology 1 \n
    @indent{12} (avcs_module_info_t (list of num_modules modules provided
    here)) -- for topology 1 \n
    @indent{12} (avcs_topology_definition_t (num_modules provided here)) -- for
    topology 2 \n
    @indent{12} (avcs_module_info_t (list of num_modules modules provided
    here)) -- for topology 2 \n
    @indent{12}  ... \n
    @indent{12} (avcs_topology_definition_t (num_modules provided here)) -- for
    topology num_topologies \n
    @indent{12} (avcs_module_info_t (list of num_modules modules provided
    here)) -- for topology num_topologies
    @par
    General buffer_size calculation formula:
   @verbatim
   sizeof(avcs_topology_buffer_t) + ((sizeof(avcs_topology_definition_t) +
   sizeof(avcs_module_info_t)*num_modules) for topology "1") +
   ((sizeof(avcs_topology_definition_t) +
   sizeof(avcs_module_info_t)*num_modules) for topology "2") +
       ...
   ((sizeof(avcs_topology_definition_t) +
   sizeof(avcs_module_info_t)*num_modules)for topology "num_topologies")


@endverbatim

  @par Module information (avcs_module_info_t)
    @table{weak__avcs__module__info__t}

  @detdesc
    This command must be issued before the topology is used in an appropriate
    area, which is defined by avcs_topology_definition_t::topo_config.
    @par
    The command can be sent multiple times to add new topologies. If a
    subsequent command redefines a previously defined topology ID, an error
    is reported.
    @par
    If the topology has a module that is flagged as not needed, that module is
    bypassed. For more information, see amdb_cfg_flags_t::needed.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
 */
#define AVCS_CMD_REGISTER_TOPOLOGIES                                0x00012923

/* The payload for the AVCS_CMD_REGISTER_TOPOLOGIES command */
typedef struct avcs_cmd_register_topologies_t avcs_cmd_register_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_register_topologies_t
@{ */
/* Payload of the AVCS_CMD_REGISTER_TOPOLOGIES message.
 */
struct avcs_cmd_register_topologies_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the 64-bit topology buffer address. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the 64-bit topology buffer address. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for an address. The aDSP returns this memory map
         handle through the #AVCS_CMD_SHARED_MEM_MAP_REGIONS command.

         @values
         - NULL -- Parameter data payloads are within the message payload
           (in-band).
         - Non-NULL -- Parameter data payloads begin at the address specified
           in the data_payload_addr_lsw and data_payload_addr_msw fields
           (out-of-band).

         The client can choose in-band or out-of-band, however, we recommend
         out-of-band. */

    uint32_t                  buffer_size;
    /**< Actual size in bytes of the valid data in the topology buffer. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_register_topologies_t */

typedef struct avcs_topology_buffer_t avcs_topology_buffer_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_topology_buffer_t
@{ */
/* Header structure for the custom topologies command buffer.
 * This provides detail about the number of topologies that are defined in this command.
 */
struct avcs_topology_buffer_t
{
    uint32_t num_topologies;
    /**< Specifies the number of topologies in this command.

         @values > 0*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_topology_buffer_t */

/** @addtogroup core_cmd_register_topos
@{ */

/** Macro that defines the area as audio. */
#define AVCS_TOPO_CFG_AREA_AUDIO_BIT 0

/** Macro that defines the area as AFE. */
#define AVCS_TOPO_CFG_AREA_AFE_BIT       1

/** Macro that defines the area as LSM. */
#define AVCS_TOPO_CFG_AREA_LSM_BIT      2

/** Macro that defines the area as voice. */
#define AVCS_TOPO_CFG_AREA_VOICE_BIT   3

/** @} */ /* end_addtogroup core_cmd_register_topos */

typedef struct avcs_topology_definition_t avcs_topology_definition_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_topology_definition_t
@{ */
/* Defines a topology that is to be added to the aDSP.
 * This structure provides details such as the topoID, area bit mask and number of module-instances in the topology.
 */
struct avcs_topology_definition_t
{
    uint32_t version;
    /**< Versioning information.

         @values
         - 0 -- avcs_module_info_t is used for module information.
           There should not be any repeating modules in topology.
         - 1 -- avcs_module_instance_info_t is used for module/instance
           information.
           There should not be any repeating module/instances in the
           topology. @tablebulletend */

    uint32_t topology_id;
    /**< ID of the topology being added.

         @values Any valid topology ID */

    uint32_t topo_config;
    /**< Bits 0 to 3 indicate the area to which this topology. Set bits 4 to
         31 to 0.

         @values{ for bits 0 to 3}
         - 1 << #AVCS_TOPO_CFG_AREA_AUDIO_BIT
         - 1 << #AVCS_TOPO_CFG_AREA_AFE_BIT
         - 1 << #AVCS_TOPO_CFG_AREA_LSM_BIT
         - 1 << #AVCS_TOPO_CFG_AREA_VOICE_BIT
         - Or any combination @vertspace{3}

         For example: @vertspace{-3}
@verbatim
"Audio + Voice": 1 << #AVCS_TOPO_CFG_AREA_AUDIO_BIT |
1 << #AVCS_TOPO_CFG_AREA_VOICE_BIT
@endverbatim */

    uint32_t num_modules;
    /**< Total number of instances of all modules in this topology.

         For example, if there are three instance of module A, two instances
         of module B, and one instance of module C, this value is 6.

         @values @ge 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_topology_definition_t */

typedef struct avcs_module_info_t avcs_module_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_module_info_t
@{ */
/* Defines the module IDs that are used in this topology.
 */
struct avcs_module_info_t
{
    uint32_t module_id;
    /**< ID of the module to be created.

         @values Any valid module ID */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_module_info_t */

typedef struct avcs_module_instance_info_t avcs_module_instance_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_module_instance_info_t
@{ */
/* Defines a module-instance with the module ID-instance ID pair that are used in this topology.
 */
struct avcs_module_instance_info_t
{
    uint32_t module_id;
    /**< ID of the module to be created.

         @values Any valid module ID */

    uint16_t instance_id;
    /**< ID of the module instance to be created.

         @values Any valid instance ID */

    uint16_t reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_module_instance_info_t */

/** @addtogroup core_cmd_sh_mem_map_regions
@{ */
/** Commands the AVCS to map multiple shared memory regions.
    All mapped regions must be from the same memory pool.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_SHARED_MEM_MAP_REGIONS
    @par
    Dst_port -- 0000 (Session ID/Stream ID = 0/0)

  @apr_msgpayload{avcs_cmd_shared_mem_map_regions_t}
    This AVCS structure is the same as the AVS structure
    (avs_cmd_shared_mem_map_regions_t).
    @par
    Immediately following avs_cmd_shared_mem_map_regions_t are num_regions of
    avs_shared_map_region_payload_t.
    @par
    For more information on the AVS structures, refer to
    @xrefcond{Q6,80-NF774-1,80-NA610-1}.

  @return
    #AVCS_CMDRSP_SHARED_MEM_MAP_REGIONS.

  @dependencies
    None.
 */
#define AVCS_CMD_SHARED_MEM_MAP_REGIONS               0x00012924

typedef struct  avs_cmd_shared_mem_map_regions_t avcs_cmd_shared_mem_map_regions_t;

/** @} */ /* end_addtogroup core_cmd_sh_mem_map_regions */


/** @addtogroup core_resp_sh_mem_map_regions
@{ */
/** Returns the memory map handle in response to the
    #AVCS_CMD_SHARED_MEM_MAP_REGIONS command.

  @apr_hdr_fields
    Opcode -- AVCS_CMDRSP_SHARED_MEM_MAP_REGIONS
    @par
    Dst_port -- Ignored

  @apr_msgpayload{avcs_cmdrsp_shared_mem_map_regions_t}
    This AVCS structure is the same as the AVS structure
    (avs_cmdrsp_shared_mem_map_regions_t).
    @par
    For more information on avs_cmdrsp_shared_mem_map_regions_t, refer to
    @xrefcond{Q6,80-NF774-1,80-NA610-1}.

  @return
    None.

  @dependencies
    An #AVCS_CMD_SHARED_MEM_MAP_REGIONS command must be issued.
 */
#define AVCS_CMDRSP_SHARED_MEM_MAP_REGIONS              0x00012925

typedef struct  avs_cmdrsp_shared_mem_map_regions_t avcs_cmdrsp_shared_mem_map_regions_t;

/** @} */ /* end_addtogroup core_resp_sh_mem_map_regions */

/** @addtogroup core_cmd_sh_mem_unmap_regions
@{ */
/** Commands the AVCS to unmap multiple shared memory regions that were
    previously mapped.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_SHARED_MEM_UNMAP_REGIONS
    @par
    Dst_port -- 0000 (Session ID/Stream ID = 0/0)

  @apr_msgpayload{avcs_cmd_shared_mem_unmap_regions_t}
    This AVCS structure is the same as the AVS structure
    (avs_cmd_shared_mem_unmap_regions_t).
    @par
    For more information on avs_cmd_shared_mem_unmap_regions_t, refer to
    @xrefcond{Q6,80-NF774-1,80-NA610-1}.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    A corresponding #AVCS_CMD_SHARED_MEM_MAP_REGIONS command must have been
    issued.
 */
#define AVCS_CMD_SHARED_MEM_UNMAP_REGIONS              0x00012926

typedef struct avs_cmd_shared_mem_unmap_regions_t avcs_cmd_shared_mem_unmap_regions_t;

/** @} */ /* end_addtogroup core_cmd_sh_mem_unmap_regions */

/** @weakgroup weakb_core_register_amdb_mods_values
@{ */

/** Default value for the AMDB client ID. */
#define AVCS_AMDB_CLIENT_ID_NONE       0x0

/** AMDB client ID is all clients. */
#define AVCS_AMDB_CLIENT_ID_ALL        0x1

/** AMDB client ID is HLOS clients. */
#define AVCS_AMDB_CLIENT_ID_HLOS       0x2

/** AMDB client ID is AMDB Manager clients. */
#define AVCS_AMDB_CLIENT_ID_AMDB_MGR   0x3

/** AMDB client ID is QACT clients. */
#define AVCS_AMDB_CLIENT_ID_QACT       0x4

/** @} */ /* end_weakgroup weakb_core_register_amdb_mods_values */


/** @ingroup core_register_amdb_mods
  Registers modules with the AMDB. The payload structure is similar to
  amdb_cfg_module_info_t.

  @msgpayload{avcs_register_amdb_module_t}
    @table{weak__avcs__register__amdb__module__t}

  @detdesc
    For in-band, an error code chunk (amdb_cfg_error_code_t) is not supported.
    @par
    Before registering the same module again, #AVCS_DEREGISTER_AMDB_MODULES
    must be called.
    @par
    Modules registered using this command are preferred over built-in modules
    or custom modules that come from adsp_avs_config.acdb.
    @par
    Built-in modules can be overridden. That is, the same module ID can be
    reused for different source code. However, be sure to implement the same
    parameter IDs that are implemented in the original module.
    @par
    @newpage If any module fails, the overall result is a failure.
    @par
    When preload==ON_USE for a module, registration can be successful, but when
    the module is used, it might fail (e.g., the file is absent or dynamic
    linking failed).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    Memory must be mapped using #AVCS_CMD_SHARED_MEM_MAP_REGIONS, and a memory
    map handle must be obtained.

  @inputfile{group__weakb__core__register__amdb__mods__values.tex}
 */
#define AVCS_REGISTER_AMDB_MODULES       0x00012927

typedef struct avcs_register_amdb_module_t avcs_register_amdb_module_t;
#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_register_amdb_module_t
@{ */
struct avcs_register_amdb_module_t
{
   uint32_t                  client_id;
   /**< Identifier for the client.

        @values
        - #AVCS_AMDB_CLIENT_ID_HLOS
        - #AVCS_AMDB_CLIENT_ID_AMDB_MGR
        - #AVCS_AMDB_CLIENT_ID_QACT @tablebulletend */

   uint32_t                  data_payload_addr_lsw;
   /**< Lower 32 bits of the 64-bit data payload address. */

   uint32_t                  data_payload_addr_msw;
  /**< Upper 32 bits of the 64-bit data payload address.

       If the address is not sent (NULL), the message is in the payload.

       If the address is sent (non-NULL), the parameter data payloads begin
       at the specified address.

       This memory is treated as an array of module information. 
       For error codes of individual modules, the module information must have
       an error code chunk (amdb_cfg_error_code_t). */

   uint32_t                  mem_map_handle;
   /**< Unique identifier for an address. The aDSP returns this memory map
        handle through #AVCS_CMD_SHARED_MEM_MAP_REGIONS.

        @values
        - NULL -- Parameter data payloads are within the message payload
          (in-band).
        - Non-NULL -- Parameter data payloads begin at the address specified
          in the data_payload_addr_lsw and data_payload_addr_msw fields
          (out-of-band). @tablebulletend */

   uint32_t                  size;
   /**< Size in bytes of the valid data. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_register_amdb_module_t */


/** @ingroup core_deregister_amdb_mods
  Commands the AVCS to deregister modules. The payload structure is
  similar to amdb_cfg_module_info_t.

  @msgpayload{deregister_amdb_module_t}
    @table{weak__avcs__deregister__amdb__module__t}

  @detdesc
    For in-band, an error code chunk (amdb_cfg_error_code_t) is not supported.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The module must be registered using #AVCS_REGISTER_AMDB_MODULES.
 */
#define AVCS_DEREGISTER_AMDB_MODULES       0x00012928

typedef struct avcs_deregister_amdb_module_t avcs_deregister_amdb_module_t;
#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_deregister_amdb_module_t
@{ */
struct avcs_deregister_amdb_module_t
{
   uint32_t                  client_id;
   /**< Identifier for the client.

        @values
        - #AVCS_AMDB_CLIENT_ID_HLOS
        - #AVCS_AMDB_CLIENT_ID_AMDB_MGR
        - #AVCS_AMDB_CLIENT_ID_QACT @tablebulletend */

   uint32_t                  data_payload_addr_lsw;
   /**< Lower 32 bits of the 64-bit data payload address. */

   uint32_t                  data_payload_addr_msw;
  /**< Upper 32 bits of the 64-bit data payload address.

       If the address is not sent (NULL), the message is in the payload.

       If the address is sent (non-NULL), the parameter data payloads begin
       at the specified address.

       This memory is treated as an array of module information. 
       For error codes of individual modules, the module information must have
       an error code chunk (amdb_cfg_error_code_t). */

   uint32_t                  mem_map_handle;
   /**< Unique identifier for an address. The aDSP returns this memory map
        handle through #AVCS_CMD_SHARED_MEM_MAP_REGIONS.

        @values
        - NULL -- Parameter data payloads are within the message payload
          (in-band).
        - Non-NULL -- Parameter data payloads begin at the address specified
          in the data_payload_addr_lsw and data_payload_addr_msw fields
          (out-of-band). @tablebulletend */

   uint32_t                  size;
   /**< Size in bytes of the valid data. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_deregister_amdb_module_t */


/** @ingroup core_print_amdb_mods
  Prints all registered modules to logs, typically in the QXDM
  Professional@tm tool.

  @msgpayload{avcs_print_amdb_modules_t}
    @table{weak__avcs__print__amdb__modules__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
 */
#define AVCS_AMDB_PRINT_ALL_MODULES    0x00012929

typedef struct avcs_print_amdb_modules_t avcs_print_amdb_modules_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_print_amdb_modules_t
@{ */
struct avcs_print_amdb_modules_t
{
   uint32_t                  client_id;
   /**< Identifier for the client.

        @values
        - #AVCS_AMDB_CLIENT_ID_ALL
        - #AVCS_AMDB_CLIENT_ID_HLOS
        - #AVCS_AMDB_CLIENT_ID_AMDB_MGR
        - #AVCS_AMDB_CLIENT_ID_QACT

       Only modules registered by a specified client ID are printed. However,
       when client_id=AVCS_AMDB_CLIENT_ID_ALL, all modules in the AMDB are
       printed. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_print_amdb_modules_t */


/** @ingroup core_cmd_deregister_topos
  Deregisters custom topologies that were previously registered through
  #AVCS_CMD_REGISTER_TOPOLOGIES for use in the Audio, Voice, AFE, and LSM
  services.

  @note1hang This command cannot be used to deregister static topologies or
             topologies that were added using other commands.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_DEREGISTER_TOPOLOGIES
    @par
    Dst_port -- 0

  @apr_msgpayload{avcs_cmd_deregister_topologies_t}
    @table{weak__avcs__cmd__deregister__topologies__t} @newpage

  @par Topology information (avcs_topology_deregister_t)
    @table{weak__avcs__topology__deregister__t}

  @par Number of topologies (avcs_topology_buffer_t)
    @table{weak__avcs__topology__buffer__t} @vertspace{.5}
    @par
    In memory, the following order must be maintained:
    @par
    @indent{12} avcs_topology_buffer_t (num_topologies provided here) \n
    @indent{12} (avcs_topology_deregister_t) -- for topology 1 \n
    @indent{12} (avcs_topology_deregister_t) -- for topology 2 \n
    @indent{12} . . .

  @detdesc
    This command can remove all custom topologies (mode parameter =
    #AVCS_CMD_DEREGISTER_ALL_CUSTOM_TOPOLOGIES) or only selected topologies
    (mode parameter = #AVCS_CMD_DEREGISTER_SELECTED_TOPOLOGIES).
    It can be sent multiple times to remove different topologies.
    @par
    The command is successful only for topologies that were previously
    registered and have not yet been deregistered. If a topology was not
    registered or if it is already deregistered (as noted through the topology
    ID), an error is reported.
    @par
    Also, if the command sends multiple topologies to deregister, all of the
    checks are performed before it deregisters any of them.

  @return
    #ADSP_EOK -- Success.
    @par
    #ADSP_ENOMEMORY -- Not enough memory in the aDSP for API validation.
    @par
    #ADSP_EBADPARAM -- Failure. Typical examples:
      - The topology does not exist or it is not active in the topology
        database.
      - The topology is already deregistered.
      - The buffer size is not large enough to hold the number of topologies.

  @dependencies
    None.
 */
#define AVCS_CMD_DEREGISTER_TOPOLOGIES                                0x0001292a

/* The payload for the AVCS_CMD_REGISTER_TOPOLOGIES command */
typedef struct avcs_cmd_deregister_topologies_t avcs_cmd_deregister_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_deregister_topologies_t
@{ */
/* Payload of the AVCS_CMD_DEREGISTER_TOPOLOGIES message.
 */
struct avcs_cmd_deregister_topologies_t
{
  uint32_t                  data_payload_addr_lsw;
  /**< Lower 32 bits of the 64-bit topology buffer address. */

  uint32_t                  data_payload_addr_msw;
  /**< Upper 32 bits of the 64-bit topology buffer address. */

  uint32_t                  mem_map_handle;
  /**< Unique identifier for an address. The aDSP returns this memory map
       handle through #AVCS_CMD_SHARED_MEM_MAP_REGIONS.

       @values
       - NULL -- Parameter data payloads are within the message payload
         (in-band).
       - Non-NULL -- Parameter data payloads begin at the address specified
         in the data_payload_addr_lsw and data_payload_addr_msw fields
         (out-of-band). @tablebulletend */

  uint32_t                  buffer_size;
  /**< Actual size in bytes of the valid data in the topology buffer. */

  uint32_t                  mode;
  /**< Mode of operation.

       @values
       - #AVCS_CMD_DEREGISTER_SELECTED_TOPOLOGIES (Default) -- The AVCS checks
         the parameter payload of this command, and deregisters each topology
         after the required checks have been performed
       - #AVCS_CMD_DEREGISTER_ALL_CUSTOM_TOPOLOGIES -- The AVCS ignores all
         other fields in the payload when it deregisters the topologies.
         @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_weak_avcs_cmd_deregister_topologies_t */

/** @addtogroup core_cmd_deregister_topos
@{ */

/** Mode of operation is to deregister selected topologies. */
#define AVCS_CMD_DEREGISTER_SELECTED_TOPOLOGIES            1

/** Mode of operation is to deregister all custom topologies that were added
    through the #AVCS_CMD_REGISTER_TOPOLOGIES command.
 */
#define AVCS_CMD_DEREGISTER_ALL_CUSTOM_TOPOLOGIES          2

/** @} */ /* end_addtogroup core_cmd_deregister_topos */

/** @weakgroup weak_avcs_topology_deregister_t
@{ */
/* Defines a topology that is to be removed from the aDSP.
 * This structure provides details such as the topoID, area bit mask and num. of modules in the topology.
 */
#include "adsp_begin_pack.h"
struct avcs_topology_deregister_t
{
  uint32_t topology_id;
  /**< ID of the topology being removed.

       @values
       Any valid topology ID that was registered using
       #AVCS_CMD_REGISTER_TOPOLOGIES and is active in the aDSP. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_topology_deregister_t */


/** @ingroup core_cmd_get_fwk_version
  Queries the core service for the build version and API versions of
  the static services.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_GET_FWK_VERSION

  @apr_msgpayload
    None.

  @return
    #AVCS_CMDRSP_GET_FWK_VERSION

  @dependencies
    None.
*/
#define AVCS_CMD_GET_FWK_VERSION         0x0001292c

/** @ingroup core_resp_get_fwk_version
    Response to an #AVCS_CMD_GET_FWK_VERSION request.

  @apr_hdr_fields
    Opcode -- AVCS_CMDRSP_GET_FWK_VERSION

  @apr_msgpayload{avcs_cmdrsp_get_fwk_version_t}
    @tablens{weak__avcs__cmdrsp__get__fwk__version__t}

  @par Service information (avs_svc_api_info_t)
    @tablens{weak__avs__svc__api__info__t}

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_CMDRSP_GET_FWK_VERSION   0x0001292d

typedef struct avcs_cmdrsp_get_fwk_version_t avcs_cmdrsp_get_fwk_version_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmdrsp_get_fwk_version_t
@{ */
/* Payload of the AVCS_CMDRSP_GET_FWK_VERSION command response, which
    provides the build version information and a listing of service IDs 
    and their API version numbers. */
/** Immediately following this structure are num_services instances of
    %avs_svc_api_info_t (see <b>Service information</b>).
*/
struct avcs_cmdrsp_get_fwk_version_t
{

    uint32_t                  build_major_version;
    /**< Indicates the major version of the AVS build.

         This value is incremented on chipset family boundaries. */

    uint32_t                  build_minor_version;
    /**< Minor version of the AVS build.

         This value represents the mainline to which the AVS build belongs. */

    uint32_t                  build_branch_version;
    /**< Indicates the AVS branch version to which the image belongs. */

    uint32_t                  build_subbranch_version;
    /**< Indicates the AVS sub-branch or customer product line information. */

    uint32_t                  num_services;
    /**< Number of supported AVS services in the current build. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmdrsp_get_fwk_version_t */

typedef struct avs_svc_api_info_t avs_svc_api_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_svc_api_info_t
@{ */
/** For each instance in the payload, this structure contains per-service
  information.
*/
struct avs_svc_api_info_t
{

    uint32_t                  service_id;
    /**< APRV2 service IDs for the individual static services.

         @values
         - APRV2_IDS_SERVICE_ID_ADSP_CORE_V
         - APRV2_IDS_SERVICE_ID_ADSP_AFE_V
         - APRV2_IDS_SERVICE_ID_ADSP_ASM_V
         - APRV2_IDS_SERVICE_ID_ADSP_ADM_V
         - APRV2_IDS_SERVICE_ID_ADSP_MVM_V
         - APRV2_IDS_SERVICE_ID_ADSP_CVS_V
         - APRV2_IDS_SERVICE_ID_ADSP_CVP_V
         - APRV2_IDS_SERVICE_ID_ADSP_LSM_V

         For more information, refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}. */

    uint32_t                  api_version;
    /**< Indicates the API version of the service.

         Each new API update that warrants a change on the HLOS side triggers
         an increment in the version. */

    uint32_t                  api_branch_version;
    /**< Indicates the API increments on a sub-branch (not on the mainline).

         API branch version numbers can increment independently on different
         sub-branches*/

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_svc_api_info_t */

/** @addtogroup core_cmd_add_pool_pages
@{ */
/** Adds pages to a physical memory pool.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_ADD_POOL_PAGES

  @apr_msgpayload{avcs_cmd_add_pool_pages}
    @table{weak__avcs__cmd__add__pool__pages}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
*/
#define AVCS_CMD_ADD_POOL_PAGES              0x0001292E

/** ID of the physical pool. */
#define ADSP_MEMORY_MAP_HLOS_PHYSPOOL 4

typedef struct avcs_cmd_add_pool_pages_t avcs_cmd_add_pool_pages_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_add_pool_pages
@{ */
/* Payload of the AVCS_CMD_ADD_POOL_PAGES command.
*/
struct avcs_cmd_add_pool_pages_t
{
   uint32_t pool_id;
   /**< Valid pool ID.

        @values #ADSP_MEMORY_MAP_HLOS_PHYSPOOL */

   uint32_t size;
   /**< Size of the page, in bytes. The size must be 4 KB aligned. */

   uint32_t phy_addr_lsw;
   /**< Lower 32 bits of the address of the first page. */

   uint32_t phy_addr_msw;
   /**< Upper 32 bits of the address of the first page.

        The 64-bit address formed by phy_addr_lsw and phy_addr_msw must must
        be 4 KB aligned.*/

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_add_pool_pages */
/** @} *//* end_addtogroup core_cmd_add_pool_pages */

/** @addtogroup core_cmd_remove_pool_pages
@{ */
/** Removes pages from a physical memory pool.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_REMOVE_POOL_PAGES

  @apr_msgpayload{avcs_cmd_remove_pool_pages}
    @table{weak__avcs__cmd__remove__pool__pages}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    None.
*/
#define AVCS_CMD_REMOVE_POOL_PAGES              0x0001292F

typedef struct avcs_cmd_remove_pool_pages_t avcs_cmd_remove_pool_pages_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_remove_pool_pages
@{ */
/* Payload of the AVCS_CMD_REMOVE_POOL_PAGES command.
*/
struct avcs_cmd_remove_pool_pages_t
{
   uint32_t pool_id;
   /**< Valid ID of the pool.

        @values #ADSP_MEMORY_MAP_HLOS_PHYSPOOL */

   uint32_t size;
   /**< Size of the page (in bytes). The size must be 4 KB aligned. */

   uint32_t phy_addr_lsw;
   /**< Lower 32 bits of the address of the first page. */

   uint32_t phy_addr_msw;
   /**< Upper 32 bits of the address of the first page.

        The 64-bit address formed by phy_addr_lsw and phy_addr_msw must must
        be 4 KB aligned.*/

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_remove_pool_pages */
/** @} *//* end_addtogroup core_cmd_remove_pool_pages */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ACS_API_H_ */
