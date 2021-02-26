/*========================================================================
Copyright (c) 2010-2015, Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_adsppm_wrapper.h

@brief Lite Wrapper for ADSPPM. Mainly to serve profiling.
 goal is not to hide MMPM/ADSPPM details
 */
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_adsppm_wrapper.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/12/15   rbhatnk     Created file.
========================================================================== */

#ifndef QURT_ADSPPM_WRAPPER_H
#define QURT_ADSPPM_WRAPPER_H

#if defined(SIM)
#define QURT_ELITE_ADSPPM_DEBUG
#endif

#ifdef QURT_ELITE_ADSPPM_DEBUG
#undef ADSPPM_INTEGRATION
#define ADSPPM_INTEGRATION 1
#endif

#if (ADSPPM_INTEGRATION==1)
#include "mmpm.h"
#endif //(ADSPPM_INTEGRATION==1)

#include "AdspCoreSvc.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */


/* -----------------------------------------------------------------------
 ** Global definitions/forward declarations
 ** ----------------------------------------------------------------------- */
typedef void* qurt_elite_adsppm_client_t;

/** @ingroup qurt_elite_adsppm_wrapper
  Creates qurt_elite_adsppm_wrapper

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_create();

/** @ingroup qurt_elite_adsppm_wrapper
  destroys qurt_elite_adsppm_wrapper

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_destroy();

/** @ingroup qurt_elite_adsppm_wrapper
  begin profiling

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_begin_profiling();

/** @ingroup qurt_elite_adsppm_wrapper
  ends profiling

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_end_profiling();

#if (ADSPPM_INTEGRATION==1)
/** @ingroup qurt_elite_adsppm_wrapper
  Registers with ADSPPM.
  returned client_ptr_ptr is valid only when profiling. Client shouldnt use client_ptr_ptr for any purpose.
  At the time of register, profiling may not be enabled yet. In such cases handle will be allocated at next request call.

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_register(MmpmRegParamType *regParam, uint32_t *client_id_ptr, qurt_elite_adsppm_client_t **client_ptr_ptr);


/** @ingroup qurt_elite_adsppm_wrapper
  Sends request to ADSPPM
  returned client_ptr_ptr is valid only when profiling. Client shouldnt use client_ptr_ptr for any purpose.
  At the time of register, profiling may not be enabled yet. In such cases handle will be allocated at next request call.

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_request(uint32_t client_id, qurt_elite_adsppm_client_t **client_ptr_ptr, MmpmRscExtParamType *pRscExtParam);

/** @ingroup qurt_elite_adsppm_wrapper
  Sends release to ADSPPM
  returned client_ptr_ptr is valid only when profiling. Client shouldnt use client_ptr_ptr for any purpose.
  At the time of register, profiling may not be enabled yet. In such cases handle will be allocated at next request call.

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_release(uint32_t client_id, qurt_elite_adsppm_client_t **client_ptr_ptr, MmpmRscExtParamType *pRscExtParam);

#endif //#if (ADSPPM_INTEGRATION==1)

/** @ingroup qurt_elite_adsppm_wrapper
  Deregisters with ADSPPM

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_deregister(uint32_t *client_id_ptr, qurt_elite_adsppm_client_t **client_ptr_ptr);

/**
 * Aggregates all client votes.
 *
 */
void qurt_elite_adsppm_wrapper_aggregate(avcs_component_adsppm_voting_info_t *info_ptr, avcs_component_adsppm_voting_info_t *voc_client_ptr, uint32_t *aggr_bw, uint32_t *aggr_mpps);

/**
 * returns true if the client is registered.
 */
bool_t qurt_elite_adsppm_wrapper_is_registered(uint32_t client_id);

/**
 * when is_max_out is TRUE - bumps up the bus and Q6 clocks.
 * when is_max_out is FALSE - releases the bus and Q6 clocks.
 */
ADSPResult qurt_elite_adsppm_wrapper_max_out_pm(uint32_t client_id, qurt_elite_adsppm_client_t **client_ptr_ptr, bool_t is_max_out);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ADSPPM_WRAPPER_H

