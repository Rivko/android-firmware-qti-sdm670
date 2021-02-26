#ifndef __VSS_IHDVOICE_PUBLIC_IF_H__
#define __VSS_IHDVOICE_PUBLIC_IF_H__

/**
  @file vss_ihdvoice_public_if.h
  @brief This file contains the API definitions for HD Voice features.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * HD Voice APIS                                    *
 ****************************************************************************/

/** @ingroup cvd_hdvoice_cmd_enable
  Enables HD Voice for a voice session.

  @apr_msgpayload
  None.

  @detdesc
  The client must set dst_addr in the APR command to the MVM address.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_ENABLE ( 0x000130A2 )

/** @ingroup cvd_hdvoice_cmd_disable
  Disables HD Voice for a voice session.

  @apr_msgpayload
  None.

  @detdesc
  The client must set dst_addr in the APR command to the MVM address.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_DISABLE ( 0x000130A3 )

/** @ingroup cvd_hdvoice_cmd_enable_beamr
  Enables blind enhanced AMR (BeAMR) for subsequent sessions.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_BEAMR_ENABLE ( 0x00013157 )

/** @ingroup cvd_hdvoice_cmd_disable_beamr
  Disables BeAMR for subsequent sessions.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_BEAMR_DISABLE ( 0x00013159 )

/** @ingroup cvd_hdvoice_cmd_enable_wv2
  Enables Wide Voice 2.0 for subsequent sessions.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_WV2_ENABLE ( 0x00013158 )

/** @ingroup cvd_hdvoice_cmd_disable_wv2
  Disables Wide Voice 2.2 for subsequent sessions.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_WV2_DISABLE ( 0x0001315A )

#endif /* __VSS_IHDVOICE_PUBLIC_IF_H__ */

