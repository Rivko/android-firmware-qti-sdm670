/*========================================================================*/
/**
@file EliteTopology_db.h
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
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
========================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments; added
                   comments missing from some functions.
09/11/14   sw      (Tech Pubs) Added Doxygen markup/comments for 2.6.
07/25/12   Unni    Added lsm_add_static_topologies_and_modules
03/27/12   DG      Created file.
========================================================================== */
#ifndef ELITE_TOPOLOGY_DB_H
#define ELITE_TOPOLOGY_DB_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Class Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/** @ingroup elite_topodb_init_db
  Initializes the global topology and module databases used by the Elite
  services.
  
  This function internally calls AudPP_AddStaticTopologiesAndModules() to
  populate the databases with statically defined modules and topologies.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- None of the databases were initialized.

  @dependencies
  None.
*/
ADSPResult Elite_InitializeDataBases(void);

/** @ingroup elite_topodb_deinit_db
  De-initializes the global topology and module databases.

  @return
  None.

  @dependencies
  Elite_InitializeDataBases() was called, and it returned ADSP_EOK.
*/
void Elite_DeinitializeDataBases(void);

/** @ingroup elite_topodb_reset_topodb
  Resets the global topology database.

  @return
  None.

  @dependencies
  Elite_InitializeDataBases() was called, and it returned ADSP_EOK.
*/
ADSPResult Elite_ResetTopologyDatabase(void);

/** @ingroup elite_topodb_audpp_add_topo_mod
  Adds topologies and modules required by the audio pre/postprocessor services
  to the global databases.

  @return
  ADSP_EOK -- All topologies and modules were added successfully.
  @par
  Error code -- Topology or module addition failed. Any topologies and modules
  that were added successfully are still usable.

  @dependencies
  The global databases were initialized using Elite_InitializeDataBases().
*/
ADSPResult AudPP_AddStaticTopologiesAndModules(void);

/** @ingroup elite_topodb_audpp_add_topo
  Adds topologies required by the audio pre/postprocessor services to the
  global databases.

  @return
  ADSP_EOK -- All topologies were added successfully.
  @par
  Error code -- Topology addition failed. Any topologies
  that were added successfully are still usable.

  @dependencies
  The global databases were initialized using Elite_InitializeDataBases().
*/
ADSPResult AudPP_AddStaticTopologies(void);

/* not required for doc */
ADSPResult voice_add_static_topologies_and_modules(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_TOPOLOGY_DB_H


