#ifndef __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_ivocproc_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Core VocProc
  (CVP) module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2015, 2017 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_ivocproc_deprecated_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/16/17     sw    (Tech Pubs) Edited Doxygen comments for AVS 2.8 (845).
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"
#include "vss_ivocproc_public_if.h"

/** @addtogroup cvd_cvp_deprecated
@{ */

/*--------------------------------------------------------------------------*
 * VOCPROC TOPOLOGIES                                                       *
 *--------------------------------------------------------------------------*/

/* This topology is deprecated as it is never PORed/tested. */

/** Deprecated. Use #VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCE.

  Tx topology ID for the tri-mic Fluence algorithm.
*/
#define VSS_IVOCPROC_TOPOLOGY_ID_TX_TM_FLUENCE ( 0x00010F80 )


/****************************************************************************
 * CORE VOCPROC APIS                                                        *
 ****************************************************************************/

/** Deprecated. Use #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2.

  Creates a new full control vocproc session.

  @apr_msgpayload{vss_ivocproc_cmd_create_full_control_session_t}
  @table{weak__vss__ivocproc__cmd__create__full__control__session__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000100C3 )

/* Type definition for vss_ivocproc_cmd_create_full_control_session_t. */
typedef struct vss_ivocproc_cmd_create_full_control_session_t vss_ivocproc_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_create_full_control_session_t 
@{ */
/* Payload structure for the 
    VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_ivocproc_cmd_create_full_control_session_t
{
  uint16_t direction;
  /**< Specifies the path on which the session is created.
    
       @values
       - 0 -- Tx only
       - 1 -- Rx only
       - 2 -- Tx and Rx @tablebulletend */

  uint32_t tx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t tx_topology_id;
  /**< ID of the Tx topology that identifies the predefined algorithm required
       for the session.

       @values
       - See Section @xref{dox:TopologyIds}
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE @tablebulletend */

  uint32_t rx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t rx_topology_id;
  /**< ID of the Rx topology that identifies the predefined algorithm required
       for the session.

       @values
       - See Section @xref{dox:TopologyIds}
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE @tablebulletend */

  uint32_t network_id;
  /**< Type of network.

       @values
       - See @xhyperref{hdr:DeprecatedNetworkIDs,Deprecated Network IDs}
       - If a network ID is not supplied, set this field to
         #VSS_NETWORK_ID_DEFAULT @tablebulletend */

#if __STDC_VERSION__ >= 199901L
  char_t session_name[];
    /**< Session name string used to identify a session that can be shared with
       passive controllers (optional).

       The string size, including the NULL termination character, is limited
       to 31 characters. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_create_full_control_session_t */

/** Deprecated. Use #VSS_IVOCPROC_CMD_SET_DEVICE_V2.

  Sets a new voice device configuration on the vocproc.

  @apr_msgpayload{vss_ivocproc_cmd_set_device_t}
  @table{weak__vss__ivocproc__cmd__set__device__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The configuration must match the direction of the vocproc. For example, when
  a vocproc is created as a Tx only direction, the Rx parameters are ignored.
  @newpage
*/
#define VSS_IVOCPROC_CMD_SET_DEVICE ( 0x000100C4 )

/* Type definition for vss_ivocproc_cmd_set_device_t. */
typedef struct vss_ivocproc_cmd_set_device_t vss_ivocproc_cmd_set_device_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_device_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_DEVICE command.
*/
struct vss_ivocproc_cmd_set_device_t
{
  uint32_t tx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t tx_topology_id;
  /**< ID of the Tx topology that identifies the predefined algorithm required
       for the session.

       @values
       - See Section @xref{dox:TopologyIds}
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE

       If this field is set to #VSS_IVOCPROC_TOPOLOGY_ID_NONE, the vocproc
       does not contain any pre/postprocessing blocks, and the data is passed
       through unmodified. */

  uint32_t rx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t rx_topology_id;
  /**< ID of the Rx topology that identifies the predefined algorithm required
       for the session.

       @values
       - See Section @xref{dox:TopologyIds}
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE

       If this field is set to #VSS_IVOCPROC_TOPOLOGY_ID_NONE, the vocproc
       does not contain any pre/postprocessing blocks and is passed through
       unmodified. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_set_device_t */

/** Deprecated. Use #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V3.

  Creates a new full control vocproc session.

  @apr_msgpayload{vss_ivocproc_cmd_create_full_control_session_v2_t}
  @table{weak__vss__ivocproc__cmd__create__full__control__session__v2__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 ( 0x000112BF )

/* Type definition for vss_ivocproc_cmd_create_full_control_session_v2_t. */
typedef struct vss_ivocproc_cmd_create_full_control_session_v2_t vss_ivocproc_cmd_create_full_control_session_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_create_full_control_session_v2_t 
@{ */
/* Payload for the VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 command.
*/
struct vss_ivocproc_cmd_create_full_control_session_v2_t
{
  uint16_t direction;
  /**< Vocproc direction; the path on which the session is created.

       @values
       - #VSS_IVOCPROC_DIRECTION_RX
       - #VSS_IVOCPROC_DIRECTION_TX
       - #VSS_IVOCPROC_DIRECTION_RX_TX @tablebulletend */

  uint16_t tx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t tx_topology_id;
  /**< ID of the Tx topology that identifies the predefined algorithm required
       for the session.

       @values
       - See Section @xref{dox:TopologyIds}
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE @tablebulletend */

  uint16_t rx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t rx_topology_id;
  /**< ID of the Rx topology that identifies the predefined algorithm required
       for the session.

       @values
       - #VSS_IVOCPROC_TOPOLOGY_ID_RX_DEFAULT
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE @tablebulletend */

  uint32_t profile_id;
  /**< Voice calibration profile ID. */

  uint32_t vocproc_mode;
  /**< Specifies whether the vocproc mode is set to internal or external
       mixing.
    
       @values
       - #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING
       - #VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING @tablebulletend */

  uint16_t ec_ref_port_id;
  /**< ID of the port to which the vocproc connects for receiving an echo
       canceller reference signal.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE

       This value is ignored when the vocproc_mode parameter is set to
       #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING. */

#if __STDC_VERSION__ >= 199901L
  char_t session_name[];
  /**< Session string used to identify a session that can be shared with
       passive controllers (optional).

       The string size, including the NULL termination character, is limited
       to 31 characters. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_ivocproc_cmd_create_full_control_session_v2_t */

/** Deprecated. Use #VSS_IVOCPROC_CMD_SET_DEVICE_V3.

  Sets a new voice device configuration on the vocproc.

  @apr_msgpayload{vss_ivocproc_cmd_set_device_v2_t}
  @table{weak__vss__ivocproc__cmd__set__device__v2__t}

  @detdesc
  The configuration provided must match the direction of the vocproc. For
  example, when a vocproc is created as a Tx only direction, the Rx
  parameters are ignored.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_SET_DEVICE_V2 ( 0x000112C6 )

/* Type definition for vss_ivocproc_cmd_set_device_v2_t. */
typedef struct vss_ivocproc_cmd_set_device_v2_t vss_ivocproc_cmd_set_device_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_device_v2_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_DEVICE_V2 command.
*/
struct vss_ivocproc_cmd_set_device_v2_t
{
  uint16_t tx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t tx_topology_id;
  /**< ID of the Tx topology that identifies the predefined algorithm required
       for the session.

       @values
       - See Section @xref{dox:TopologyIds}
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE @tablebulletend */

  uint16_t rx_port_id;
  /**< ID of the device port to which the vocproc connects.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

  uint32_t rx_topology_id;
  /**< ID of the Rx topology that identifies the predefined algorithm required
       for the session.

       @values
       - #VSS_IVOCPROC_TOPOLOGY_ID_RX_DEFAULT
       - If a topology ID is not supplied, set this field to
         #VSS_IVOCPROC_TOPOLOGY_ID_NONE @tablebulletend */

  uint32_t vocproc_mode;
  /**< Specifies whether the vocproc mode is set to internal or external
       mixing.
    
       @values
       - #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING
       - #VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING @tablebulletend */

  uint16_t ec_ref_port_id;
  /**< ID of the port to which the vocproc connects for receiving the echo
       canceller reference signal.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE

       This value is ignored when vocproc_mode is set to
       #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_set_device_v2_t */


/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/** Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.

  Registers the memory that contains common calibration data table with
  the vocproc.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_ivocproc_cmd_register_calibration_data_v2_t}
  @table{weak__vss__ivocproc__cmd__register__calibration__data__v2__t}

  @par Expected buffer format
@verbatim
                                      <-------- 32 bits -------->
 -------------------------
| column_1                | --> e.g., network_id
 -------------------------
| column_2                | --> e.g., rx_sampling_rate
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n                |
 -------------------------
| data_size               | --> byte length of data block that follows
 -------------------------
| data                    | -------->  -------------------------
|                         |           | mod_id                  |
|                         |            -------------------------
| ...                     |           | param_id                |
 -------------------------             -------------------------
| column_1                |           | param_size | reserved   |
 -------------------------             -------------------------
| column_2                |           | param_data              |
 -------------------------            |                         |
|  .                      |           | ...                     |
|  .                      |            -------------------------
 -------------------------            | mod_id                  |
| column_n                |           .                         .
 -------------------------            .                         .
| data_size               |           .                         .
 -------------------------
| data                    |
|                         |
|                         |
| ...                     |
 -------------------------
|                         |
.                         .
.                         .

@endverbatim

  @detdesc
  As part of this registration, in addition to the address, size, and handle
  of the memory containing the calibration table, the client also provides
  the columns and associated column types that the vocproc uses to form a
  key that is used to index into the calibration table and look up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.
  @par
  Calibration data is applied when the vocproc transitions to the Run state.
  @par
  The client is not allowed to modify the calibration data memory while it
  is registered, and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail. 
  @par
  The supported column IDs that can be registered in this command are:
  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  Any memory registered here must first be mapped with the CVD as specified in
  Section @xref{dox:ShMemGuidelines}.
  @par
  The client cannot modify the calibration data memory while it is registered,
  and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until it is
  unregistered. Registering a memory region for calibration data while another
  region is still registered causes the command to fail. @newpage
*/
#define VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 ( 0x00011373 )

/* Type definition for vss_ivocproc_cmd_register_calibration_data_v2_t. */
typedef struct vss_ivocproc_cmd_register_calibration_data_v2_t vss_ivocproc_cmd_register_calibration_data_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_register_calibration_data_v2_t
@{ */
/* Payload structure for the
    VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 command.
*/
struct vss_ivocproc_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;
  /**< Unique identifier for the shared memory address of the per-network
       calibration data payload. */

  uint64_t cal_mem_address;
  /**< Shared memory address of the payload. */

  uint32_t cal_mem_size;
  /**< Size of the memory allocated for the payload.

       @values > 0 bytes

       The amount of memory allocated at cal_mem_address must meet the
       requirement as specified in Section @xref{dox:ShMemGuidelines}. */

  uint32_t num_columns;
  /**< Number of columns used to form the key for looking up a calibration 
       data entry. */

#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
  /**< Array of columns of size num_columns. The order in which the columns
       are provided here must match the order in which they exist in the
       calibration table. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_register_calibration_data_v2_t */

/** Deprecated. Use #VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA.

  Deregisters the memory that contains the common calibration data table from
  the vocproc and any cal-indexing memory provided.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA ( 0x00011276 )


/** Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA to send
  dynamic calibration data to the vocproc.
  
  Registers the memory that contains the volume calibration data table with
  the vocproc.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_ivocproc_cmd_register_volume_calibration_data_t}
  @table{weak__vss__ivocproc__cmd__register__volume__calibration__data__t}

  @par Expected buffer format
@verbatim
                                      <-------- 32 bits -------->
 -------------------------
| column_1                | --> e.g., network_id
 -------------------------
| column_2                | --> e.g., rx_sampling_rate
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n                | --> e.g., volume_index
 -------------------------
| data_size               | --> byte length of data block that follows
 -------------------------
| data                    | -------->  -------------------------
|                         |           | mod_id                  |
|                         |            -------------------------
| ...                     |           | param_id                |
 -------------------------             -------------------------
| column_1                |           | param_size | reserved   |
 -------------------------             -------------------------
| column_2                |           | param_data              |
 -------------------------            |                         |
|  .                      |           | ...                     |
|  .                      |            -------------------------
 -------------------------            | mod_id                  |
| column_n                |           .                         .
 -------------------------            .                         .
| data_size               |           .                         .
 -------------------------
| data                    |
|                         |
|                         |
| ...                     |
 -------------------------
|                         |
.                         .
.                         .

@endverbatim

  @detdesc
  As part of this registration, in addition to the address, size, and handle
  of the memory containing the calibration table, the client also provides
  the columns and associated column types that the vocproc uses to form a
  key that is used to index into the calibration table and look up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.
  @par
  Calibration data is applied when the vocproc transitions to the Run state
  as well as when the volume step changes during the Run state.
  @par
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.
  @par
  The supported column IDs that can be registered in this command are:
  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  Any memory registered here must first be mapped with the CVD as specified in
  Section @xref{dox:ShMemGuidelines}. @newpage
*/
#define VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA ( 0x00011374 )

/* Type definition for vss_ivocproc_cmd_register_volume_calibration_data_t. */
typedef struct vss_ivocproc_cmd_register_volume_calibration_data_t vss_ivocproc_cmd_register_volume_calibration_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_register_volume_calibration_data_t
@{ */
/* Payload structure for the
    #VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA command.
*/
struct vss_ivocproc_cmd_register_volume_calibration_data_t
{
  uint32_t cal_mem_handle;
  /**< Unique identifier for the shared memory address of the volume
       calibration data payload. */

  uint64_t cal_mem_address;
  /**< Shared memory address of the payload. */

  uint32_t cal_mem_size;
  /**< Size of the memory allocated for the payload.

       @values > 0 bytes

       The amount of memory allocated at cal_mem_address must meet the
       requirement as specified in Section @xref{dox:ShMemGuidelines}. */

  uint32_t num_columns;
  /**< Number of columns used to form the key for looking up a calibration
       data entry. */

#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
  /**< Array of columns of size num_columns. The order in which the
       columns are provided here must match the order in which they exist
       in the calibration table. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_register_volume_calibration_data_t */


/** Deprecated. Use #VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA.

  Deregisters the memory that contains the volume calibration data table from
  the vocproc and any provided calibration-indexing memory.

  @apr_msgpayload
  None.

  @par Payload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA ( 0x00011375 )


/****************************************************************************
 *  VOCPROC VOLUME/MUTE APIS                                                *
 ****************************************************************************/

/** Deprecated. Use #VSS_IVOLUME_CMD_SET_STEP.

  Changes the volume and recalibrates for the new volume.

  @apr_msgpayload{vss_ivocproc_cmd_set_rx_volume_index_t}
  @table{weak__vss__ivocproc__cmd__set__rx__volume__index__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX ( 0x000110EE )

/* Type definition for vss_ivocproc_cmd_set_rx_volume_index_t. */
typedef struct vss_ivocproc_cmd_set_rx_volume_index_t vss_ivocproc_cmd_set_rx_volume_index_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_rx_volume_index_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX
    command.
*/
struct vss_ivocproc_cmd_set_rx_volume_index_t
{
  uint16_t vol_index;
  /**< Volume index used by the vocproc to index the volume table and set the
       volume on the Voice Digital Signal Processor (VDSP). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_set_rx_volume_index_t */


/** Deprecated. Use #VSS_IVOLUME_CMD_MUTE_V2.

  Changes the mute setting.

  @apr_msgpayload{vss_ivocproc_cmd_set_mute_t}
  @table{weak__vss__ivocproc__cmd__set__mute__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_SET_MUTE ( 0x000110EF )

/* Type definition for vss_ivocproc_cmd_set_mute_t. */
typedef struct vss_ivocproc_cmd_set_mute_t vss_ivocproc_cmd_set_mute_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_mute_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_MUTE command.
*/
struct vss_ivocproc_cmd_set_mute_t
{
  uint16_t direction;
  /**< Specifies the path on which mute is configured.

       @values
       - 0 -- Tx only
       - 1 -- Rx only
       - 2 -- Tx and Rx @tablebulletend */

  uint16_t mute_flag;
  /**< Turn mute on or off.

       @values
       - 0 -- Disable mute
       - 1 -- Enable mute @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_ivocproc_cmd_set_mute_t */


/** @ingroup cvd_cvp_deprecated
  Deprecated. Use #VSS_ICOMMON_CMD_SET_PARAM_V3.

  Registers the memory that contains one or more topology parameters.

  @note1hang For guidelines on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_ivocproc_cmd_topology_register_params_t}
  @table{weak__vss__ivocproc__cmd__topology__register__params__t}

  @par Expected buffer format for Vocproc topology parameters
  In this format, mod_id must be set to #VSS_MODULE_CVD_GENERIC.
  @par
@verbatim
<-------- 32 bits -------->

-------------------------
| mod_id                  |
-------------------------
| param_id                |
-------------------------
| param_size | reserved   |
-------------------------
| param_data              |
|                         |
| ...                     |
-------------------------
| mod_id                  |
.                         .
.                         .
.                         .

@endverbatim

  @detdesc
  Vocproc topology parameters can be included in the payload using
  out-of-band shared memory. Topology parameters can also be set separately
  using in-band commands. Parameters are cached and applied on the vocproc
  session in the following order when #VSS_IVOCPROC_CMD_TOPOLOGY_COMMIT is
  received:
   -# Out-of-band parameters
   -# In-band parameters
  @par
  If the same parameter is set using both in-band and out-of-band commands,
  the in-band parameter takes effect upon commit. @newpage
  @par
  The client cannot modify the shared memory while it is registered, and the
  vocproc does not modify this memory while it is registered. The topology
  parameter data in the registered memory is valid until it is unregistered.
  @par
  This command fails if a memory region for topology parameter data is
  registered while another region is still registered.
  @par
  Registered topology parameter data is deregistered after the vocproc session
  is stopped or disabled.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The topology parameter data memory registered with this command must first be
  mapped with the CVD (see Section @xref{dox:ShMemGuidelines}).
*/
#define VSS_IVOCPROC_CMD_TOPOLOGY_REGISTER_PARAMS ( 0x0001319A )

/* Type definition for #vss_ivocproc_cmd_topology_register_params_t. */
typedef struct vss_ivocproc_cmd_topology_register_params_t vss_ivocproc_cmd_topology_register_params_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_topology_register_params_t
@{ */
/* Payload structure for the #VSS_IVOCPROC_CMD_TOPOLOGY_REGISTER_PARAMS command.
*/
struct vss_ivocproc_cmd_topology_register_params_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory that holds the vocproc topology parameter
       data. */

  uint64_t mem_address;
  /**< Location of the vocproc topology payload. */

  uint32_t mem_size;
  /**< Size in bytes of the vocproc topology payload.

       The amount of memory allocated at mem_address must meet the requirement
       as specified in Section @xref{dox:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_topology_register_params_t */


/** @ingroup cvd_cvp_deprecated
  Deprecated.

  Deregisters the memory that holds vocproc topology parameter data.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The vocproc session must be stopped or disabled. @newpage
*/
#define VSS_IVOCPROC_CMD_TOPOLOGY_DEREGISTER_PARAMS ( 0x0001319B )

/** @ingroup cvd_cvp_deprecated
  Deprecated. Use #VSS_ICOMMON_CMD_SET_PARAM_V3.

  Sets the vocproc sampling rates.

  @apr_msgpayload{vss_ivocproc_cmd_set_sampling_rate_t}
  @table{weak__vss__ivocproc__cmd__set__sampling__rate__t}

  @detdesc
  If no sampling rates are provided to the vocproc (i.e., this command is
  not called), the operating and calibration sampling rates the
  vocproc uses are those it receives from the MVM (provided to the MVM via
  #VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE or
  #VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE).
  @par
  If sampling rates are provided to the vocproc (that is, this command is
  called), the operating and calibration sample rates the vocproc
  uses are those provided in this command.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_SET_SAMPLING_RATE ( 0x00011370 )

/* Type definition for vss_ivocproc_cmd_set_sampling_rate_t. */
typedef struct vss_ivocproc_cmd_set_sampling_rate_t vss_ivocproc_cmd_set_sampling_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_sampling_rate_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_SAMPLING_RATE command. */
struct vss_ivocproc_cmd_set_sampling_rate_t
{
  uint32_t rx;
  /**< Rx sampling rate.

       @values 8, 16, 48 kHz */

  uint32_t tx;
  /**< Tx sampling rate.

       @values 8, 16, 48 kHz */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_set_sampling_rate_t */

/** @ingroup cvd_cvp_deprecated
  Deprecated. Use #VSS_ICOMMON_CMD_SET_PARAM_V3.

  Sets the number of device channels on a vocproc topology to complete
  creation of a vocproc session.

  @apr_msgpayload{vss_ivocproc_cmd_topology_set_dev_channels_t}
  @table{weak__vss__ivocproc__cmd__topology__set__dev__channels__t}

  @detdesc
  This command is optional for internal predefined topologies.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  This command must be issued as follows:
  - Between #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V3 and
    #VSS_IVOCPROC_CMD_TOPOLOGY_COMMIT
  - Or between #VSS_IVOCPROC_CMD_SET_DEVICE_V3 and
    #VSS_IVOCPROC_CMD_TOPOLOGY_COMMIT
*/
#define VSS_IVOCPROC_CMD_TOPOLOGY_SET_DEV_CHANNELS ( 0x00013199 )

/* Type definition for #vss_ivocproc_cmd_topology_set_dev_channels_t. */
typedef struct vss_ivocproc_cmd_topology_set_dev_channels_t vss_ivocproc_cmd_topology_set_dev_channels_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_topology_set_dev_channels_t
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_TOPOLOGY_SET_DEV_CHANNELS command.
*/
struct vss_ivocproc_cmd_topology_set_dev_channels_t
{
  uint16_t tx_num_channels;
  /**< Number of microphones.

       @values
        - #VSS_NUM_DEV_CHANNELS_1
        - #VSS_NUM_DEV_CHANNELS_2
        - #VSS_NUM_DEV_CHANNELS_3
        - #VSS_NUM_DEV_CHANNELS_4 @tablebulletend */

  uint16_t rx_num_channels;
  /**< Number of speaker channels.

       @values #VSS_NUM_DEV_CHANNELS_1 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_topology_set_dev_channels_t */

/** @} */ /* end_addtogroup cvd_cvp_deprecated */

#endif /* __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__ */

