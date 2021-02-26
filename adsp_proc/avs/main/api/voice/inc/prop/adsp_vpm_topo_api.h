#ifndef __ADSP_VPM_API_TOPO_H__
#define __ADSP_VPM_API_TOPO_H__

/**
  @file adsp_vpm_topo_api.h
  @brief This file contains Topo definitions for the Voice Processing Manager
  (VPM).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The Voice_mainpage.dox file contains all file/group
      descriptions that are in the output PDF generated using Doxygen and
      Latex. To edit or update any of the file/group text in the PDF, edit
      the Voice_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   HB      Created new file for topo definition.
===========================================================================*/

/** Topology ID for Tx pass through. */
#define VPM_TX_NONE       ( 0x00010F70 )

/** @cond OEM_only */
/** @xreflabel{hdr:ParamVpmTxSmEcnsTopo}
    Topology ID for Tx single-mic echo cancellation noise suppression.
    @newpage */
#define VPM_TX_SM_ECNS    ( 0x00010F71 )
/** @xreflabel{hdr:ParamVpmTxSmEcnsV2Topo}
    Topology ID for Tx single-mic ECNS V2.algorithm */
#define VPM_TX_SM_ECNS_V2 ( 0x00010F89 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceTopo}
    Topology ID for the Tx dual-mic Fluence&tm;&nbsp;algorithm. */
#define VPM_TX_DM_FLUENCE ( 0x00010F72 )

/** @xreflabel{hdr:ParamVpmTxSmFluenceV5Topo}
    Topology ID for the Tx single-mic Fluence V5 algorithm. */
#define VPM_TX_SM_FLUENCEV5 ( 0x00010F73 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceV5Topo}
    Topology ID for the Tx dual-mic Fluence V5 algorithm. */
#define VPM_TX_DM_FLUENCEV5 ( 0x00010F74 )

/** @xreflabel{hdr:ParamVpmTxQmFluenceProV2Topo}
    Topology ID for the Tx quad-mic Fluence Pro V2 algorithm. */
#define VPM_TX_QM_FLUENCE_PROV2 ( 0x00010F87 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceV5BroadSideTopo}
    Topology ID for the Tx dual-mic  Fluence V5 broadside spkphone algorithm. */
#define VPM_TX_DM_FLUENCEV5_BROADSIDE ( 0x00010F88 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceTopo}
    Topology ID for the Tx dual-mic Snapdragon Voice Plus&tm;&nbsp; ECNS algorithm. */
#define VPM_TX_DM_VPECNS ( 0x00010F86 )
/** @endcond */

/** Topology Ids for custom dynamic topologies. Up to 3 custom topologies
    are supported for each mic configuration.These topologies are mirror
    of existing Fluence V5 topologies with exception of ECNS module.

    ECNS module need to be developed using CAPI V2 framework and it need to
    support FWK_EXTN_ECNS framework extension.

    ECNS module will be dynamically loaded. Name of the ECNS shared object,
    entry functions and ECNS module Ids are predefined and same need to be used
    for implementation.

    All custom topologies run with 20msec frame size. Default clock voting for
    custom topology is same existing QCOM topology whose mirror it is. For instance
    single mic custom topology will have clock voting equivalent to VPM_TX_SM_FLUENCEV5
    topology.
 */

/** Topology ID for single mic custom dynamic topologies. */

#define VPM_TX_CUSTOM_SM_ECNS_1 (0x10027000)

#define VPM_TX_CUSTOM_SM_ECNS_2 (0x10027001)

#define VPM_TX_CUSTOM_SM_ECNS_3 (0x10027002)

/** Topology ID for dual mic custom dynamic topologies. */
#define VPM_TX_CUSTOM_DM_ECNS_1 (0x10027010)

#define VPM_TX_CUSTOM_DM_ECNS_2 (0x10027011)

#define VPM_TX_CUSTOM_DM_ECNS_3 (0x10027012)

/** Topology ID for quad mic custom dynamic topologies. */
#define VPM_TX_CUSTOM_QM_ECNS_1 (0x10027020)

#define VPM_TX_CUSTOM_QM_ECNS_2 (0x10027021)

#define VPM_TX_CUSTOM_QM_ECNS_3 (0x10027022)

/** Topology ID for Rx pass through. */
#define VPM_RX_NONE              ( 0x00010F70 )

/** Topology ID for Rx processing with all available algorithms. */
#define VPM_RX_DEFAULT           ( 0x00010F77 )

/** Topology ID for Rx processing with all supported algorithms.
    It supports VOICE_MODULE_MBDRC_V2 instead of VOICE_MODULE_MBDRC module.
 */
#define VPM_RX_DEFAULT_V2        ( 0x00010F8A )

/** The topologies VPM_RX_DEFAULT and VPM_RX_DEFAULT_V2 is being deprecated,
     and will be removed in future. */

/** Topology ID for Rx processing with all supported algorithms.
    It supports VOICE_MODULE_FNS_V2 instead of VOICE_MODULE_FNS module.
 */
#define VPM_RX_DEFAULT_V3        ( 0x00010F8B )

#endif /* __ADSP_VPM_API_TOPO_H__ */
