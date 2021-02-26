#ifndef __VSS_IVERSION_PUBLIC_IF_H__
#define __VSS_IVERSION_PUBLIC_IF_H__

/**
  @file vss_iversion_public_if.h
  @brief This file contains the APR API definitions for Core Voice Driver
  version information.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_iversion_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * CVD VERSION APIS                                                         *
 ****************************************************************************/

/** @ingroup cvd_version_cmd_get
  Retrieves the CVD version string.

  @apr_msgpayload
  None.

  @detdesc
  The dst_addr in the APR command must be set to the MVM address, and the
  dst_port must be set to APR_NULL_V.

  @return
  #VSS_IVERSION_RSP_GET -- On success.
  @par
  APRV2_IBASIC_RSP_RESULT -- On failure (refer to
  @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IVERSION_CMD_GET ( 0x00011378 )


/** @ingroup cvd_version_resp_get
  Response to the #VSS_IVERSION_CMD_GET command. The response contains the CVD
  version string.

  @apr_msgpayload{vss_iversion_rsp_get_t}
  @table{weak__vss__iversion__rsp__get__t}

  @returns
  None.

  @dependencies
  None.
*/
#define VSS_IVERSION_RSP_GET ( 0x00011379 )

/* Type definition for #vss_iversion_rsp_get_t. */
typedef struct vss_iversion_rsp_get_t vss_iversion_rsp_get_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iversion_rsp_get_t
@{ */
/* Payload structure for the #VSS_IVERSION_RSP_GET command response.
*/
struct vss_iversion_rsp_get_t
{
  char_t version[31];
    /**< NULL-terminated version string. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iversion_rsp_get_t */


#endif /* __VSS_IVERSION_PUBLIC_IF_H__ */

