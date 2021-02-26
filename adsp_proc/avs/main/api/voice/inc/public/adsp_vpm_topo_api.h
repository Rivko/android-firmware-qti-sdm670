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

/** Topology ID for Rx pass through. */
#define VPM_RX_NONE              ( 0x00010F70 )



/**
* These macros are present only for successful build.
**/
#define VPM_TX_SM_ECNS    ( 0x00010F71 )
#define VPM_TX_SM_ECNS_V2 ( 0x00010F89 )
#define VPM_TX_DM_FLUENCE ( 0x00010F72 )
#define VPM_TX_SM_FLUENCEV5 ( 0x00010F73 )
#define VPM_TX_DM_FLUENCEV5 ( 0x00010F74 )
#define VPM_TX_QM_FLUENCE_PROV2 ( 0x00010F87 )
#define VPM_TX_DM_FLUENCEV5_BROADSIDE ( 0x00010F88 )
#define VPM_TX_DM_VPECNS ( 0x00010F86 )
#define VPM_RX_DEFAULT           ( 0x00010F77 )
#define VPM_RX_DEFAULT_V2        ( 0x00010F8A )
#define VPM_RX_DEFAULT_V3        ( 0x00010F8B )

#endif /* __ADSP_VPM_API_TOPO_H__ */
