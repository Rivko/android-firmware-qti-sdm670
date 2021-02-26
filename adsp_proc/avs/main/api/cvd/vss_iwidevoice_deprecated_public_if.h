#ifndef __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_iwidevoice_deprecated_public_if.h
  @brief This file contains the APR API definitions for WideVoice.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_iwidevoice_deprecated_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
04/01/11    leo    (Tech pubs) Minor updates for new template.
01/12/11    SCD    (Tech Pubs) Edited Doxygen markup and comments.
01/11/11     AT    Initial revision.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * WIDEVOICE APIS                                                           *
 ****************************************************************************/

/** @addtogroup cvd_widevoice_deprecated
@{ */
/** Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG to register the
  device configuration table containing the Wide Voice sample rate with the
  vocproc.

  Enables/disables Wide Voice.

  @apr_msgpayload{vss_iwidevoice_cmd_set_widevoice_t}
  @table{weak__vss__iwidevoice__cmd__set__widevoice__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IWIDEVOICE_CMD_SET_WIDEVOICE ( 0x00011243 )

/* Type definition for vss_iwidevoice_cmd_set_widevoice_t. */
typedef struct vss_iwidevoice_cmd_set_widevoice_t vss_iwidevoice_cmd_set_widevoice_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iwidevoice_cmd_set_widevoice_t 
@{ */
/* Payload of the VSS_IWIDEVOICE_CMD_SET_WIDEVOICE command.
*/
struct vss_iwidevoice_cmd_set_widevoice_t
{
  uint32_t enable;
  /**< Specifies whether Wide Voice is enabled.

       @values
       - 0 -- Disable Wide Voice
       - 1 -- Enable Wide Voice @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iwidevoice_cmd_set_widevoice_t */
/** @} */ /* end_addtogroup cvd_widevoice_deprecated */


#endif /* __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__ */

