#ifndef __VSS_IAVTIMER_PUBLIC_IF_H__
#define __VSS_IAVTIMER_PUBLIC_IF_H__

/**
  @file vss_iavtimer_public_if.h
  @brief This file contains the APR API definitions for the CVD supported
  AVTimer interfaces.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2013, 2015, 2017 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_iavtimer_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/16/17     sw    (Tech Pubs) Edited Doxygen comments for AVS 2.8 (845).
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * AVTimer APIS                                                             *
 ****************************************************************************/

/** @ingroup cvd_avtimer_cmd_get_time
  Gets the current AVTimer timestamp in microseconds.

  @apr_msgpayload
  None.

  @detdesc
  The dst_addr in the APR command must be set to the MVM address, and the
  dst_port must be set to APR_NULL_V.
  @par
  The client is responsible for handling any unsigned 64-bit overflow of the
  AVTimer timestamp.
  @par
  There are inherent delays associated with receiving, processing, and
  responding to this command.

  @return
  #VSS_IAVTIMER_RSP_GET_TIME -- Success.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to
  @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The client must start the MVM service to have a valid AVTimer value.
*/
#define VSS_IAVTIMER_CMD_GET_TIME ( 0x00012EF0 )


/** @ingroup cvd_avtimer_rsp_get_time
  Response to the #VSS_IAVTIMER_CMD_GET_TIME command.

  @apr_msgpayload{vss_iavtimer_rsp_get_time_t}
  @table{weak__vss__iavtimer__rsp__get__time__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IAVTIMER_RSP_GET_TIME ( 0x00012EF1 )

/* Type definition for vss_iavtimer_rsp_get_time_t. */
typedef struct vss_iavtimer_rsp_get_time_t vss_iavtimer_rsp_get_time_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iavtimer_rsp_get_time_t
@{ */
/* Payload structure for the VSS_IAVTIMER_RSP_GET_TIME command
    response.
*/
struct vss_iavtimer_rsp_get_time_t
{
  uint64_t timestamp_us;
    /**< AVTimer timestamp in microseconds. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iavtimer_rsp_get_time_t */

/** @ingroup cvd_avtimer_cmd_get_offset
  Gets the AVTimer offset (in microseconds) with respect to the QTimer.

  @apr_msgpayload
  None.

  @detdesc
  The destination address in the APR command is set to the MVM address, and the
  destination port is set to APR_NULL_V.
  @par
  The AVTimer offset can be both positive and negative, which the client is to
  handle.

  @return
  #VSS_IAVTIMER_RSP_GET_OFFSET -- Success. \n
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to
  @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The client must create a full control MVM session to have a valid AVTimer
  offset value.
*/
#define VSS_IAVTIMER_CMD_GET_OFFSET ( 0x0001325B )

/** @ingroup cvd_avtimer_rsp_get_offset
  Response to the #VSS_IAVTIMER_CMD_GET_OFFSET command.

  @apr_msgpayload{vss_iavtimer_rsp_get_offset_t}
  @table{weak__vss__iavtimer__rsp__get__offset__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IAVTIMER_RSP_GET_OFFSET ( 0x0001325C )

/* Type definition for vss_iavtimer_rsp_get_offset_t. */
typedef struct vss_iavtimer_rsp_get_offset_t vss_iavtimer_rsp_get_offset_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iavtimer_rsp_get_offset_t
@{ */
/* Payload structure for the VSS_IAVTIMER_RSP_GET_OFFSET command
   response.
*/
struct vss_iavtimer_rsp_get_offset_t
{
  int64_t offset_us;
    /**< Current avtimerTimestamp -- qtimerTimestamp in microseconds. */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_weak_vss_iavtimer_rsp_get_offset_t */

#endif /* __VSS_IAVTIMER_PUBLIC_IF_H__ */
