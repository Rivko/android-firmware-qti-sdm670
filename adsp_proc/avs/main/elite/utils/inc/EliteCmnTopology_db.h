/*========================================================================*/
/**
@file EliteCmnTopology_db.h
@brief This file declares the interface for the common global topology database (registered via AVCS).
This allows storage, query of topologies and modules in a topology.
 */

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*========================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc. All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
========================================================================== */

/*========================================================================
Edit History

when         who   what, where, why
--------     ---   -------------------------------------------------------
08/19/16    sw    (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/25/15    sw    (Tech Pubs) Edited Doxygen markup/comments for 8996.2.7.
03/12/15    KR    Created file.
========================================================================== */
#ifndef ELITE_CMN_TOPOLOGY_DB_H
#define ELITE_CMN_TOPOLOGY_DB_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#include "adsp_core_api.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

  /**  @ingroup elite_topodb_datatypes
    Valid types of common topologies.
  */
  typedef enum elite_cmn_topo_t
  {
    ELITE_CMN_STATIC_TOPOLOGIES = 0,  /** Topology type is static. */
    ELITE_CMN_CUSTOM_TOPOLOGIES       /** Topology type is custom. */
  }elite_cmn_topo_type_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/** @ingroup elite_topodb_db_init_cmn
  Initializes the common global topology database.

  @return
  None.

  @dependencies
  None.
 */
void elite_cmn_topo_db_init(void);

/** @ingroup elite_topodb_db_deinit_cmn
  De-initializes the common global topology database.

  @return
  None.

  @dependencies
  None.
 */
void elite_cmn_topo_db_deinit(void);

/** @ingroup elite_topodb_db_add_topo_cmn
  Adds a topology to the common global database.

  @datatypes
  avcs_topology_definition_t (refer to @xrefcond{Q12,80-NF774-4,80-NA610-4}) \n
  avcs_module_info_t (refer to @xrefcond{Q12,80-NF774-4,80-NA610-4}) \n
  #elite_cmn_topo_type_t

  @param[in] topology_definition_ptr  Pointer to the topology definition.
  @param[in] module_definition_ptr    Pointer to the module instance definition.
  @param[in] topo_type                Type of topology, static or custom.
  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Topology addition failed.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
 ADSPResult elite_cmn_topo_db_add_topology(avcs_topology_definition_t *topology_definition_ptr, void *module_definition_ptr, elite_cmn_topo_type_t topo_type);

/** @ingroup elite_topodb_db_mod_present
  Determines if a module is present in a specified topology.

  @param[in] area_bit_mask  Area bitmask of the client making this request.
  @param[in] topology_id    ID of the topology in which the module is being
                            searched.
  @param[in] module_id      ID of the module to be checked.

  @return
  ADSP_EOK -- Module exists in the topology.
  @par
  Error code -- Module does not exist in the topology, or the topology does not exist.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
ADSPResult elite_cmn_topo_db_is_module_present(uint32_t area_bit_mask, uint32_t topology_id, uint32_t module_id);

/** @ingroup elite_topodb_db_act_topo_present
  Determines if an active topology with a provided bitmask is present
  in the database.

  @param[in] area_bit_mask  Area bitmask of the client that is making this
                            request.
  @param[in] topology_id    ID of the topology in which the module is being
                            searched.

  @return
  ADSP_EOK -- Active topology exists with the bitmask.
  @par
  Error code -- Active topology does not exist with the bitmask.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
ADSPResult elite_cmn_topo_db_is_active_topology_present(uint32_t area_bit_mask, uint32_t topology_id);

/** @ingroup elite_topodb_db_topo_unique
  Determines if an incoming topology ID is unique (it does not exist
  in the common global topology database).

  @param[in] topology_id   ID of the topology to be checked.

  @return
  ADSP_EOK -- Topology ID is unique.
  @par
  Error code -- Topology ID is not unique.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
ADSPResult elite_cmn_topo_db_is_topology_unique(uint32_t topology_id);

/** @ingroup elite_topodb_db_act_topo_found
  Determines if an active custom topology ID is present in the common global
  database and if the topology ID is unique.

  @param[in] topology_id  ID of the topology to be checked.

  @return
  ADSP_EOK -- Active custom topology is found in the database.
  @par
  Error code -- Active custom topology is not found in the database.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
ADSPResult elite_cmn_topo_db_is_active_custom_topology_found(uint32_t topology_id);

  /** @ingroup elite_topodb_db_dereg_sgl_cust_topo
  Deregisters one custom topology from the common global database.

  @param[in] topology_id    ID of the topology to be removed.

  @return
  ADSP_EOK -- Success. Topology is found, is active, and is a custom topology.
  @par
  Error code -- Otherwise.

  @dependencies
  The topology ID must have been added using elite_cmn_topo_db_add_topology(),
  it must be custom, and it must be active at the time of removal.
 */
ADSPResult elite_cmn_topo_db_deregister_single_topology(uint32_t topology_id);

/** @ingroup elite_topodb_db_dereg_all_cust_topo
  Deregisters all custom topologies from the common global database.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Topology deregistration failed.

  @dependencies
  All topology IDs must have been added using elite_cmn_topo_db_add_topology(),
  and they must be active at the time of removal.
*/
ADSPResult elite_cmn_topo_db_deregister_all_custom_topologies(void);

/** @ingroup elite_topodb_deinit_db_cmn
  De-initializes the common topology and module databases.

  @note1hang This function is maintained for backward compatibility.

  @return
  None.

  @dependencies
  elite_cmn_db_init() was called, and it returned ADSP_EOK.
*/
void elite_cmn_db_deinit(void);

/** @ingroup elite_topodb_init_db_cmn
  Initializes the common topology and module databases used by the Elite
  services.

  This function internally calls the following functions to populate the
  databases with statically defined modules and topologies:
  - aud_add_static_topos_and_mods_to_cmn_db()
  - afe_add_static_topos_and_mods_to_cmn_db()
  - voice_add_static_topologies_and_modules_v2()

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- None of the databases were initialized.

  @dependencies
  None.
*/
ADSPResult elite_cmn_db_init(void);

/** @ingroup elite_topodb_reset_db_cmn
  Resets the common topology and module databases.

  @return
  None.

  @dependencies
  elite_cmn_db_init() was called, and it returned ADSP_EOK.
*/
ADSPResult elite_cmn_topo_db_reset(void);

/* Audio specific functions, implemented in Audio PP svc */
/** @ingroup elite_topodb_audpp_add_topo_mod_cmn
  Audio-specific function that is used to add topologies required
  by the audio pre/postprocessor services to the common databases.

  @return
  ADSP_EOK -- All topologies and modules were added successfully.
  @par
  Error code -- Topology or module addition failed. Any topologies and modules
  that were added successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult aud_add_static_topos_and_mods_to_cmn_db(void);

/** @ingroup elite_topodb_audpp_add_topo_cmn
  Audio-specific function that is used to add topologies required by the
  audio pre/postprocessor services to the common databases.

  @note1hang This function is maintained for backward compatibility.

  @return
  ADSP_EOK -- All topologies were added successfully.
  @par
  Error code -- Topology addition failed. Any topologies that were added
  successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult aud_add_static_topos_to_cmn_db(void);


/* AFE specific functions, implemented in AFE svc */
/** @ingroup elite_topodb_afe_add_topo_mod_cmn
  AFE-specific function that is used to add topologies and modules required
  by the AFE service to the common databases.

  @return
  ADSP_EOK -- All topologies and modules were added successfully.
  @par
  Error code -- Topology or module addition failed. Any topologies and modules
  that were added successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult afe_add_static_topos_and_mods_to_cmn_db(void);

/** @ingroup elite_topodb_afe_add_topo_cmn
  AFE-specific function that is used to add topologies required by the AFE
  service to the common databases.

  @note1hang This function is maintained for backward compatibility.

  @return
  ADSP_EOK -- All topologies were added successfully.
  @par
  Error code -- Topology addition failed. Any topologies that were added
  successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult afe_add_static_topos_to_cmn_db(void);


/** @ingroup elite_topodb_lsm_add_topo_cmn
  LSM-specific function that is used to add topologies required by the LSM
  service to the common databases.

  @note1hang This function is maintained for backward compatibility.

  @return
  ADSP_EOK -- All topologies were added successfully.
  @par
  Error code -- Topology addition failed. Any topologies that were added
  successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult lsm_add_static_topos_to_cmn_db(void);


/* Voice specific functions, implemented in Voice Cmn svc */
/** @ingroup elite_topodb_voice_add_topo_mod_cmn
  Voice-specific function that is used to add modules required by the voice
  service to the common databases.

  @return
  ADSP_EOK -- All modules were added successfully.
  @par
  Error code -- Module addition failed. Any modules that were added
                successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult voice_add_static_topologies_and_modules_v2(void);

/** @ingroup elite_topodb_voice_add_topo_cmn
  Voice-specific function that is used to add topologies required by the
  voice service to the common databases.

  @note1hang This function is maintained for backward compatibility.

  @return
  ADSP_EOK -- All topologies were added successfully.
  @par
  Error code -- Topology addition failed. Any topologies that were added
  successfully are still usable.

  @dependencies
  The common databases were initialized using elite_cmn_topo_db_init().
*/
ADSPResult voice_add_static_topos_to_cmn_db(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_CMN_TOPOLOGY_DB_H
