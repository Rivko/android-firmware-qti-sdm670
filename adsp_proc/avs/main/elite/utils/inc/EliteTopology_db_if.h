/*========================================================================*/
/**
@file EliteTopology_db_if.h
@brief This file declares the interface for the Common topology database.

This file has the interface for the Common topology database module. This module
allows storage and query of Common topologies.

*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*========================================================================
Copyright (c) 2012, 2015 Qualcomm Technologies, Inc. All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/11/14   sw      (Tech Pubs) Added Doxygen markup/comments for 2.6.
03/27/12   DG      Created file.
========================================================================== */
#ifndef ELITE_TOPOLOGY_DB_IF_H
#define ELITE_TOPOLOGY_DB_IF_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifndef TST_TOPOLOGY_DB
#include "adsp_audproc_api.h"
#include "adsp_error_codes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/
/** @addtogroup elite_topodb_datatypes
@{ */
/** Valid processing service IDs.
*/
enum elite_topo_db_client_type
{
   TOPO_DB_CLIENT_TYPE_POPP = 0,
   /**< Per object (stream leg) postprocessing. */

   TOPO_DB_CLIENT_TYPE_COPP,
   /**< Common object (common leg or device leg) postprocessing. */

   TOPO_DB_CLIENT_TYPE_POPREP,
   /**< Per object (stream leg) preprocessing. */

   TOPO_DB_CLIENT_TYPE_COPREP,
   /**< Common object (common leg or device leg) preprocessing. */

   TOPO_DB_CLIENT_TYPE_AFE,
   /**< Audio Front End processing. */

   TOPO_DB_CLIENT_TYPE_LSM,
   /**< Listen Stream Manager processing. */

   TOPO_DB_CLIENT_MAX_NUM_TYPES ,
   /**< Number of audio processor service types. */

   TOPO_DB_CLIENT_TYPE_INVALID = 0xFFFFFFFF,
   /**< Invalid client type. */
};
/** @} */ /* end_addtogroup elite_topodb_datatypes */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/** @ingroup elite_topodb_init_global
  Initializes the global topology database.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- There was an error in initialization.

  @dependencies
  None.
*/
ADSPResult topo_db_init(void);


/** @ingroup elite_topodb_deinit_global
  De-initializes the global topology database. None of the topology database
  functions can be used after this function is called.

  @return
  None.

  @dependencies
  The topo_db_init() function was called, and it returned ADSP_EOK.
*/
void topo_db_deinit(void);


/** @ingroup elite_topodb_add_topo
  Adds custom topologies created by the aDSP client to the global database.

  @datatypes
  #elite_topo_db_client_type \n
  audproc_topology_definition_t (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})

  @param[in] client_type           Type of Elite service that will use the
                                   topologies being added.
  @param[in] num_topologies        Number of topologies being added.
  @param[in] topology_definitions  Array of topology definitions to add. \n
                                   The array must have num_topologies
                                   elements. \n
                                   Each topology ID must be in the range of
                                   0x10000000 to 0x1FFFFFFF (inclusive) to
                                   ensure that it does not conflict with
                                   internal topologies.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Topology addition failed. Any successfully added topologies are
  still usable.

  @dependencies
  The topo_db_init() function was called, and it returned ADSP_EOK.
*/
ADSPResult topo_db_add_topologies(elite_topo_db_client_type client_type, uint32_t num_topologies, const audproc_topology_definition_t topology_definitions[]);


/** @ingroup elite_topodb_add_topo_intern
  Adds topologies that are internally defined in the aDSP to the global
  database.

  @datatypes
  #elite_topo_db_client_type \n
  audproc_topology_definition_t (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})

  @param[in] client_type           Type of Elite service that will use the
                                   topologies being added.
  @param[in] num_topologies        Number of topologies being added.
  @param[in] topology_definitions  Array of topology definitions to add. \n
                                   The array must have num_topologies
                                   elements.\n
                                   There is no restriction on the range of
                                   topology IDs.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Topology addition failed. Any successfully added topologies are
  still usable.

  @dependencies
  The topo_db_init() function was called, and it returned ADSP_EOK.
*/
ADSPResult topo_db_add_internal_topologies(elite_topo_db_client_type client_type, uint32_t num_topologies, const audproc_topology_definition_t topology_definitions[]);


/** @ingroup elite_topodb_get_topo
  Gets a topology definition from the global database.

  @datatypes
  #elite_topo_db_client_type \n
  audproc_topology_definition_t (refer to @xrefcond{Q9,80-NF774-1,80-NA610-1})

  @param[in]  client_type   Type of Elite service requesting the topology
                            definition.
  @param[in]  topology_id   ID of the topology.
  @param[out] topology_ptr  Double pointer that will be filled with a pointer
                            to the topology definition corresponding to
                            topology_id.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Topology definition was not retrieved. In this case,
  the topology_ptr value must be considered invalid.

  @dependencies
  The topo_db_init() function was called, and it returned ADSP_EOK.
*/
ADSPResult topo_db_get_topology(elite_topo_db_client_type client_type, uint32_t topology_id, const audproc_topology_definition_t **topology_ptr);


/** @ingroup elite_topodb_mod_pres
  Determines if a module is present in a specified topology.

  @datatypes
  #elite_topo_db_client_type

  @param[in] client_type    Type of Elite service requesting the topology
                            definition.
  @param[in] topology_id    ID of the topology.
  @param[in] module_id      ID of the module.

  @return
  ADSP_EOK -- Module exists in the topology.
  @par
  Error code -- Module does not exist in the topology, or the topology does not
  exist.

  @dependencies
  The topo_db_init() function was called, and it returned ADSP_EOK.
*/
ADSPResult topo_db_is_module_present(elite_topo_db_client_type client_type, uint32_t topology_id, uint32_t module_id);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_TOPOLOGY_DB_IF_H


