/**
@file adsp_lsm_service_commands.h

@brief This file contains LSM service commands and events structures
definitions.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      Any descriptions that display in the PDF are located in the
      LSM_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2014,2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
======================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/adsp_lsm_service_commands.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  07/16/2016 sw      (Tech Pubs) Edited Doxygen comments/markup for AVS 2.8.
  12/09/2014 sw      (Tech Pubs) Edited Doxygen comments for 2.6.
  07/14/2014 Unni    Initial version
==============================================================================*/

#ifndef _ADSP_LSM_SERVICE_COMMANDS_H_
#define _ADSP_LSM_SERVICE_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "mmdefs.h"

/*
 * Static Topologies are topologies pre-defined in the LSM service and
 * available built-in the ADSP. They can be directly used in
 * LSM_SESSION_CMD_OPEN_TX_V2 without using LSM_CMD_ADD_TOPOLOGIES
 */

/** @ingroup lsmtopo_ids
    ID of the topology used for LSM Tx sessions that have no modules.
    A session that opens with this topology creates an empty topology
    chain.
 */
#define LSM_TX_TOPOLOGY_ID_NONE               0x00012E1B

/** @ingroup lsmdepr_command
    Deprecated. Use AVCS_CMD_REGISTER_TOPOLOGIES (refer to
    @xrefcond{Q5,80-NF774-4,80-NA610-4}).

    Defines custom topologies in the aDSP for use in an LSM session. To
    create a custom topology, specify the topology ID in the
    #LSM_SESSION_CMD_OPEN_TX_V2 command.

  @apr_hdr_fields
    Opcode -- LSM_CMD_ADD_TOPOLOGIES
    @par
    Dst_port -- 0

  @apr_msgpayload{lsm_cmd_add_topologies_t}
    @table{weak__lsm__cmd__add__topologies__t}

  @par Buffer format (lsm_custom_topologies_t)
    @tablens{weak__lsm__custom__topologies__t}
    @par
    The buffer size is calculated as follows:
    - Size of the module information structure = 4 + 4 + 16 = 24 bytes
    - Size of the topology definition structure = 4 + 4 + 16 * (size of the
      module information structure) = 392 bytes
    - Size of the buffer = 4 + num_topologies * 392

  @par Topology definition (lsm_topology_definition_t)
    @tablens{weak__lsm__topology__definition__t}

  @par Module information (lsm_module_info_t)
    @table{weak__lsm__module__info__t}

  @detdesc
    The aDSP internally stores custom topology definitions. The
    definitions sent to the LSM are only available for LSM sessions.
    @par
    This command must be sent before opening a session with the custom
    topologies. The command can be sent multiple times to add new topologies;
    the LSM supports up to a total of 128 custom topologies.
    @par
    If a subsequent command redefines a previously defined topology ID, the
    new topology definition replaces the previous topology. However, the
    changes only take effect when the next session is created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).
    The client processor defines the handle as input.

  @dependencies
    None.
*/
#define LSM_CMD_ADD_TOPOLOGIES            0x00012A8C

/* The payload for the LSM_CMD_ADD_TOPOLOGIES command */
typedef struct lsm_cmd_add_topologies_t lsm_cmd_add_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_cmd_add_topologies_t
@{ */
/* Payload of the LSM_CMD_ADD_TOPOLOGIES command.
*/
struct lsm_cmd_add_topologies_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the 64-bit topology buffer address. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the 64-bit topology buffer address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data, this field must be set to 0.
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         The mem_map_handle is to be a nonzero value because the topology
         buffer is always out-of-band. */

    uint32_t                  buffer_size;
    /**< Size in bytes of the valid data in the topology buffer. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_cmd_add_topologies_t */

/** @ingroup lsmdepr_command
    Deprecated. Maximum number of init_params supported in each module.
*/
#define LSM_MAX_INIT_PARAMS 16

typedef struct lsm_module_info_t lsm_module_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_module_info_t
@{ */
/* Defines the modules used by the LSM_CMD_ADD_TOPOLOGIES commands.
*/
struct lsm_module_info_t
{
   uint32_t module_id;
   /**< ID of the module to create.

        @values Any valid module ID */

   uint32_t use_lpm;
   /**< Specifies whether to put this module in low-power memory.

        @values
        - 0 -- Do not put this module in low-power memory
        - 1 -- Put this module in low-power memory [Not supported]

        If there is insufficient low-power memory, this module is placed in
        the default memory pool. */

   uint8_t  init_params[LSM_MAX_INIT_PARAMS];
   /**< Array of initialization parameters passed to the module when it is
        created.

        This array of 16 bytes is sent to the getsize() and init() methods of
        the APPI module. The interpretation of this information is
        module-specific. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_module_info_t */

typedef struct lsm_topology_definition_t lsm_topology_definition_t;

#include "adsp_begin_pack.h"

/** @ingroup lsmdepr_command
    Deprecated. Maximum number of modules supported in a custom topology.
*/
#define LSM_MAX_MODULES_PER_TOPOLOGY 16

/** @weakgroup weak_lsm_topology_definition_t
@{ */
/* Defines a custom topology that is to be added to the aDSP. */
/** Immediately following this structure are num_modules of the
    <b>Module information</b> (%lsm_module_info_t).
*/
struct lsm_topology_definition_t
{
   uint32_t topology_id;
   /**< ID of the custom topology.

        @values 0x10000000 through 0x1FFFFFFF */

   uint32_t num_modules;
   /**< Number of modules in this topology.

        @values 1 through #LSM_MAX_MODULES_PER_TOPOLOGY */

   lsm_module_info_t module_info[LSM_MAX_MODULES_PER_TOPOLOGY];
   /**< Array of module information structures with 16 elements.

        The order of the elements in this array specifies the order in which
        the modules are connected in the topology, from source to sink. Only
        the first num_modules elements of this array are required. The rest of
        the elements are ignored. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_topology_definition_t */

typedef struct lsm_custom_topologies_t lsm_custom_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_custom_topologies_t
@{ */
/* Header structure for the custom topologies command buffer.  */
/** Immediately following this structure are num_topologies of the
    <b>Topology definition</b> (%lsm_topology_definition_t) that are to be
    added to the aDSP.
*/
struct lsm_custom_topologies_t
{
   uint32_t num_topologies;
   /**< Specifies the number of topologies in this command.

        @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_custom_topologies_t */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_LSM_SERVICE_COMMANDS_H_ */
