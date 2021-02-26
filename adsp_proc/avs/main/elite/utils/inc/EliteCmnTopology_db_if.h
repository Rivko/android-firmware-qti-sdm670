/*========================================================================*/
/**
@file EliteCmnTopology_db_if.h
@brief This file declares the interface for the common global topology database (registered via AVCS).
This allows query of topologies and modules in a topology.
 */
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc. All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

when           who     what, where, why
-------------      ---       -----------------------
03/12/15      KR      Created file.
========================================================================== */
#ifndef ELITE_CMN_TOPOLOGY_DB_IF_H
#define ELITE_CMN_TOPOLOGY_DB_IF_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "adsp_core_api.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup elite_topodb_datatypes_cmn
@{ */

/** Invalid handle ID returned by the database in case of an error. */
#define ELITE_CMN_TOPO_DB_INVALID_HANDLE             0xFFFFFFFF

typedef struct elite_cmn_topo_db_entry_t elite_cmn_topo_db_entry_t;

/** Topology database entry nodes.
*/
struct elite_cmn_topo_db_entry_t
{
    avcs_topology_definition_t topo_def;
    /**< Topology definition information. */

    avcs_module_instance_info_t module_info[1];
    /**< Variable-sized list of module instance information starting from
         this entry + (num_modules - 1) entries following this element in
         memory (avcs_module_instance_info_t module_info[num_modules]). */
};

/** @} */ /* end_addtogroup elite_topodb_datatypes_cmn */


/** @ingroup elite_topodb_db_get_topo_cmn
  Gets a topology definition from the common global topology database.

  @datatypes
  elite_cmn_topo_db_entry_t

  @param[in]     area_bit_mask     Area bitmask of the client making this
                                   request.
  @param[in]     topology_id       ID of the topology for which the definition
                                   is requested.
  @param[in,out] topology_ptr_ptr  Double pointer to the topology definition
                                   (which includes its module properties).
  @param[out]    actual_size_ptr   Pointer to the actual size (in bytes) of the
                                   data pointed by topology_ptr_ptr.
  @param[out]    handle_ptr        Pointer to a handle that the client uses for
                                   future transactions (such as release).

  @return
  ADSP_EOK -- Success.
  @par
  Error code:
  - Topology definition could not be retrieved.
  - Provided topology_id does not have the area_bit_mask.
  - Topology is not active (it is marked for deregister).
  @par
  For all three error codes, the topology database returns NULL, the actual
  size is set to zero, and the handle is set to
  #ELITE_CMN_TOPO_DB_INVALID_HANDLE.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
ADSPResult elite_cmn_topo_db_get_topology(uint32_t area_bit_mask, uint32_t topology_id, elite_cmn_topo_db_entry_t **topology_ptr_ptr, uint32_t *actual_size_ptr, uint32_t *handle_ptr);

/** @ingroup elite_topodb_db_rel_topo_cmn
  Notifies the common global topology database that the client released a
  topology.

  The client must call this function once it is finished using the topology
  definition pointer.

  @param[in] handle   Handle to be released.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Specified handle does not exist.

  @dependencies
  The elite_cmn_topo_db_init() function was called successfully.
 */
ADSPResult elite_cmn_topo_db_release_topology(uint32_t handle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_CMN_TOPOLOGY_DB_IF_H
