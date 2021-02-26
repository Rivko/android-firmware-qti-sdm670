#ifndef __AXICFGINT_H__
#define __AXICFGINT_H__
/*=============================================================================

@file   axicfgint.h

@brief  Internal header file for bus configuration.

when         who     what, where, why
----------   ---     --------------------------------------------------------
2016/02/22   sds     Branch for Hoya restructure.
2011/12/05   dj      Created file

===========================================================================
            Copyright (c) 2011-2016 QUALCOMM Technologies, Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "ddiaxicfgint.h"
#include "ddiaxicfg.h"
#include "icb_soc_data.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Public Functions
** ----------------------------------------------------------------------- */
/**
  @brief Set a QoS policy for the given master, overriding default settings.

  @param[in] master      A pointer to the master information
  @param[in] qos_policy  A pointer to the qos policy to set (NULL to unset)
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfgInt_SetQosPolicy( icb_master_def_type * master,
                                              AxiCfg_QosPolicyType *qos_policy );

#ifdef __cplusplus
}
#endif

#endif /* __AXICFGINT_H__ */
