#ifndef __VSS_IVFR_PRIVATE_IF_H__
#define __VSS_IVFR_PRIVATE_IF_H__

/*
   Copyright (c) 2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/vss_ivfr_private_if.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "vss_common_public_if.h"

/****************************************************************************
 * VFR APIs                                                                 *
 ****************************************************************************/

/**
  Opens the CVD VFR driver. The handle to the opened driver is returned to the
  client via the src_port in the #VSS_IVFR_RSP_OPEN command response.

  @par Payload
  #vss_ivfr_cmd_open_t

  @return
  #VSS_IVFR_RSP_OPEN -- In case of success.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.

  @dependencies
  None.
*/
#define VSS_IVFR_CMD_OPEN ( 0x0001305F )

/** @brief Type definition for #vss_ivfr_cmd_open_t.
*/
typedef struct vss_ivfr_cmd_open_t vss_ivfr_cmd_open_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVFR_CMD_OPEN command.
*/
struct vss_ivfr_cmd_open_t
{
  uint16_t mode;
    /**< Vocoder frame synchronization mode. Possible values:\n
         - #VSS_ICOMMON_VFR_MODE_SOFT. */
}
#include "apr_pack_end.h"
;


/**
  Response to the #VSS_IVFR_CMD_OPEN command.

  @par Payload
  #vss_ivfr_rsp_open_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IVFR_RSP_OPEN ( 0x00013060 )

/** @brief Type definition for #vss_ivfr_rsp_open_t.
*/
typedef struct vss_ivfr_rsp_open_t vss_ivfr_rsp_open_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVFR_RSP_OPEN command response.
*/
struct vss_ivfr_rsp_open_t
{
  uint64_t ref_timestamp_us;
    /**< VFR reference timestamp in microseconds. The clock source to be used
         for interpreting the timestamp is product requirement specific. */
}
#include "apr_pack_end.h"
;


/**
  Closes the CVD VFR driver.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The client must have successfully opened the CVD VFR driver.
*/
#define VSS_IVFR_CMD_CLOSE ( 0x00013061 )

#endif /* __VSS_IVFR_PRIVATE_IF_H__ */

