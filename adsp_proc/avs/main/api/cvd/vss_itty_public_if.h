#ifndef __VSS_ITTY_PUBLIC_IF_H__
#define __VSS_ITTY_PUBLIC_IF_H__

/**
  @file vss_itty_public_if.h
  @brief This file contains the APR API definitions for Teletypewriter (TTY).
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

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_itty_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
04/01/11    leo    (Tech pubs) Minor updates for new template.
01/12/11    SCD    (Tech Pubs) Edited Doxygen markup and comments.
01/11/11     AT    Initial version.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * TELETYPEWRITER (TTY) APIS                                                *
 ****************************************************************************/


/** @ingroup cvd_tty_inb_cmd_set_tty
  Sets the TTY mode. The client must sent this command to the MVM session in
  the CVD module.

  @apr_msgpayload{vss_itty_cmd_set_tty_mode_t}
  @table{weak__vss__itty__cmd__set__tty__mode__t}

  @return 
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ITTY_CMD_SET_TTY_MODE ( 0x00011196 )
  
/* Type definition for vss_itty_cmd_set_tty_mode_t. */
typedef struct vss_itty_cmd_set_tty_mode_t vss_itty_cmd_set_tty_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_itty_cmd_set_tty_mode_t
@{ */
/* Payload structure for the VSS_ITTY_CMD_SET_TTY_MODE command.
*/
struct vss_itty_cmd_set_tty_mode_t
{
  uint32_t mode;
  /**< Sets the TTY mode.

       @values
       - 0 -- Disable TTY
       - 1 -- Hearing Carry Over (HCO)
       - 2 -- Voice Carry Over (VCO)
       - 3 -- FULL @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_itty_cmd_set_tty_mode_t */

#endif /* __VSS_ITTY_PUBLIC_IF_H__ */

