#ifndef __VSS_ISTREAM_PUBLIC_IF_H__
#define __VSS_ISTREAM_PUBLIC_IF_H__

/**
  @file vss_istream_public_if.h
  @brief This file contains the APR API definitions for Core Voice Stream (CVS)
  module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2015-2016 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_istream_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
07/16/16     sw    (Tech Pubs) Edited Doxygen comments/markup for 8998.2.8.
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * CORE STREAM APIS                                                         *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_create_passive_ctrl 
  Creates a new passive control stream session.

  @apr_msgpayload
  The payload is the session name string used to identify the full control
  stream session for which a handle is to be obtained. The string size,
  including the NULL termination character, is limited to 31 characters.

  @detdesc
  Passive stream sessions allow clients to manage UI controls such as muting
  and volume levels while the corresponding full control sessions manage data
  exchange.
  @par
  Use the session name, <i>default modem voice</i>, to obtain a handle to the
  default stream session for a circuit-switched voice call.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION ( 0x00011140 )

/** @ingroup cvd_cvs_cmd_create_full_ctrl
  Creates a new full control stream session.

  @apr_msgpayload{vss_istream_cmd_create_full_control_session_t}
  @table{weak__vss__istream__cmd__create__full__control__session__t}

  @detdesc
  Full stream sessions give clients full control over the stream, including
  data exchange and UI controls.
  @par
  If the client sets the direction to both Tx and Rx (bidirectional), the same
  media type must be used on each path.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The session name, <i>default modem voice</i>, is reserved for the default
  stream session for circuit-switched voice calls and cannot be given to
  any new full control sessions.
  @par
  Multiple full control stream sessions cannot be created with the same name.
*/
#define VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000110F7 )

/* Type definition for vss_istream_cmd_create_full_control_session_t. */
typedef struct vss_istream_cmd_create_full_control_session_t vss_istream_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_create_full_control_session_t 
@{ */
/* Payload structure for the
    VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_istream_cmd_create_full_control_session_t
{
   uint16_t direction;
   /**< Specifies the path on which the session is created.

        @values
        - 0 -- Tx only
        - 1 -- Rx only
        - 2 -- Tx and Rx
        - 3 -- Tx and Rx loopback @tablebulletend */

   uint32_t enc_media_type;
   /**< Tx vocoder type (see Section @xref{dox:MediaDefinitions}). */

   uint32_t dec_media_type;
   /**< Rx vocoder type (see Section @xref{dox:MediaDefinitions}). */

   uint32_t network_id;
   /**< Type of network.

        @values See @xhyperref{hdr:CalNetworkID,Network ID column values} */

#if __STDC_VERSION__ >= 199901L
   char_t name[];
   /**< Variable-sized stream name (optional).

        The stream name size is the payload size minus the size of the other
        fields. The string size, including the NULL termination character, is
        limited to 31 characters.

        The string must be NULL terminated if it is supplied.

        A size 0 name is interpreted as a nameless session where only the
        handle is used to access to the session. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_create_full_control_session_t */

/** @ingroup cvd_cvs_cmd_set_media_type
  Sets the media type on the stream. This command is supported for full control
  clients only.

  @apr_msgpayload{vss_istream_cmd_set_media_type_t}
  @table{weak__vss__istream__cmd__set__media__type__t}

  @detdesc
  If the client sets the direction to both Tx and Rx (bidirectional), the same
  media type must be used on each path.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_MEDIA_TYPE ( 0x00011186 )

/* Type definition for vss_istream_cmd_set_media_type_t. */
typedef struct vss_istream_cmd_set_media_type_t vss_istream_cmd_set_media_type_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_media_type_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_MEDIA_TYPE command.
*/
struct vss_istream_cmd_set_media_type_t
{
   uint32_t rx_media_id;
   /**< Specifies the Rx vocoder type.

        @values See Section @xref{dox:MediaDefinitions} */

   uint32_t tx_media_id;
   /**< Specifies the Tx vocoder type.

        @values See Section @xref{dox:MediaDefinitions} */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_media_type_t */


/** @ingroup cvd_cvs_cmd_attach_vocproc
  Attaches a vocproc to the stream.

  @apr_msgpayload{vss_istream_cmd_attach_vocproc_t}
  @table{weak__vss__istream__cmd__attach__vocproc__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_ATTACH_VOCPROC ( 0x000110F8 )

/* Type definition for vss_istream_cmd_attach_vocproc_t. */
typedef struct vss_istream_cmd_attach_vocproc_t vss_istream_cmd_attach_vocproc_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_attach_vocproc_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_ATTACH_VOCPROC command.
*/
struct vss_istream_cmd_attach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to attach to the stream. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_attach_vocproc_t */


/** @ingroup cvd_cvs_cmd_detach_vocproc
  Detaches a vocproc from the stream.

  @apr_msgpayload{vss_istream_cmd_detach_vocproc_t}
  @table{weak__vss__istream__cmd__detach__vocproc__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DETACH_VOCPROC ( 0x000110F9 )

/* Type definition for vss_istream_cmd_detach_vocproc_t. */
typedef struct vss_istream_cmd_detach_vocproc_t vss_istream_cmd_detach_vocproc_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_detach_vocproc_t 
@{ */
/* Payload stucture for the VSS_ISTREAM_CMD_DETACH_VOCPROC command.
*/
struct vss_istream_cmd_detach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to detach from the stream. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weak_vss_istream_cmd_detach_vocproc_t */

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/* Type definition for vss_istream_cal_na_value_t. */
typedef union vss_istream_cal_na_value_t vss_istream_cal_na_value_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cal_na_value_t
@{ */
/* Union for specifying a calibration column N/A value.
*/
union vss_istream_cal_na_value_t
{
  uint8_t uint8_val;
    /**< UINT8 N/A value. */
  uint16_t uint16_val;
    /**< UINT16 N/A value. */
  uint32_t uint32_val;
    /**< UINT32 N/A value. */
  uint64_t uint64_val;
    /**< UINT64 N/A value. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cal_na_value_t */

/* Type definition for vss_istream_cal_column_t. */
typedef struct vss_istream_cal_column_t vss_istream_cal_column_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cal_column_t 
@{ */
/* Structure for specifying a calibration column.
*/
struct vss_istream_cal_column_t
{
  uint32_t id;
  /**< Column ID.

       @values See @xhyperref{hdr:CalibrationColumns,Calibration columns} */

  uint32_t type;
  /**< Column type.

       @values
       See @xhyperref{hdr:CalibrationColumnTypes,Calibration column types} */

  vss_istream_cal_na_value_t na_value;
  /**< N/A value associated with this column ID and type.

       N/A is a reserved value indicating that the calibration data entry
       corresponding to it in the table is independent of this column.
       Therefore, the data is applied regardless of the current system
       configuration for this column. 

       The size of na_value is dependent on the column type. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cal_column_t */


/** @ingroup cvd_cvs_cmd_reg_stat_cal_data
  Registers the memory that contains the static calibration data table with the
  stream.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}. For the expected calibration
             data buffer format, see Section @xref{dox:CalibrationDataFormat}.

  @apr_msgpayload{vss_istream_cmd_register_static_calibration_data_t}
  @table{weak__vss__istream__cmd__register__static__calibration__data__t}

  @par Calibration column payload (vss_istream_cal_column_t)
  @table{weak__vss__istream__cal__column__t} @newpage

  @par Calibration column N/A values (vss_istream_cal_na_value_t)
  @table{weak__vss__istream__cal__na__value__t}

  @detdesc
  The static calibration data table captures calibration data for system
  configurations that do not change while the stream is running (such as
  network sampling rates or Tx and Rx pre/postprocessing sampling rates).
  Static calibration data is applied when the stream transitions to the Run
  state.
  @par
  During registration, the client provides the address, size, and handle of the
  memory containing the calibration table. The client also provides the columns
  and associated column types that the stream uses to form a key. This key is
  used to index into the calibration table and look up a matching entry.
  @par
  The columns and types must be provided in this command in the same order
  that they exist in the calibration table.
  - If no matching entry is found, calibration data is not applied. 
  - If multiple matching entries are found, the order in which they are applied
    is specified in Section&nbsp;@xref{dox:OrderMultiMatchingCalibEntries}.
  @par
  The currently supported order for the columns provided in this command is:
  -# #VSS_ICOMMON_CAL_COLUMN_NETWORK
  -# #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE
  -# #VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE
  -# #VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE
  -# #VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE
  -# #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID
  -# #VSS_ICOMMON_CAL_COLUMN_FEATURE
  @par
  The client cannot modify the calibration data memory while it is registered,
  and the stream does not modify this memory while it is registered. System
  stability problems might occur if the client modifies the calibration data
  memory while it is registered.
  @par
  The calibration data at the registered memory is valid until the memory is
  deregistered.
  @par
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  Any memory registered here must first be mapped with the CVD by following
  the guidelines in Section @xref{dox:ShMemGuidelines}. 
*/
#define VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA ( 0x0001307D )

/* Type definition for #vss_istream_cmd_register_static_calibration_data_t. */
typedef struct vss_istream_cmd_register_static_calibration_data_t vss_istream_cmd_register_static_calibration_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_register_static_calibration_data_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA command.
*/
struct vss_istream_cmd_register_static_calibration_data_t
{
  uint32_t cal_mem_handle;
  /**< Unique identifier for the shared memory address of the calibration data
       payload. */

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
  vss_istream_cal_column_t columns[];
  /**< Array of columns of size num_columns.

       The order in which the columns are provided here must match the order
       in which they exist in the calibration table. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgoup weak_vss_istream_cmd_register_static_calibration_data_t */

/** @ingroup cvd_cvs_cmd_dereg_stat_cal_data
  Deregisters the memory that contains static calibration data from the stream.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA ( 0x0001307E )


/****************************************************************************
 * COMMON VOCODER APIS                                                      *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_set_enc_dtx_mode
  Sets the encoder Discontinuous Transmission (DTX) mode.

  @apr_msgpayload{vss_istream_cmd_set_enc_dtx_mode_t}
  @table{weak__vss__istream__cmd__set__enc__dtx__mode__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_ENC_DTX_MODE ( 0x0001101D )

/* Type definition for vss_istream_cmd_set_enc_dtx_mode_t. */
typedef struct vss_istream_cmd_set_enc_dtx_mode_t vss_istream_cmd_set_enc_dtx_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_enc_dtx_mode_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_ENC_DTX_MODE command.
*/
struct vss_istream_cmd_set_enc_dtx_mode_t
{
   uint32_t enable;
   /**< Specifies whether DTX is on or off.

        @values
        - 0 -- Disable DTX
        - 1 -- Enable DTX @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_enc_dtx_mode_t */


/****************************************************************************
 * CDMA SPECIFIC VOCODER APIS                                               *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_cdma_set_enc_mnmx_rate
  Sets the minimum and maximum rates on the encoder.

  @apr_msgpayload{vss_istream_cmd_cdma_set_enc_minmax_rate_t}
  @table{weak__vss__istream__cmd__cdma__set__enc__minmax__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE ( 0x00011019 )

/* Type definition for vss_istream_cmd_cdma_set_enc_minmax_rate_t. */
typedef struct vss_istream_cmd_cdma_set_enc_minmax_rate_t vss_istream_cmd_cdma_set_enc_minmax_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_cdma_set_enc_minmax_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE
    command.
*/
struct vss_istream_cmd_cdma_set_enc_minmax_rate_t
{
   uint16_t min_rate;
   /**< Specifies the lower boundary of the encoder rate.

        @values
        - 0x0000 -- Blank frame
        - 0x0001 -- Eighth rate
        - 0x0002 -- Quarter rate
        - 0x0003 -- Half rate
        - 0x0004 -- Full rate @tablebulletend */

   uint16_t max_rate;
   /**< Specifies the upper boundary of the encoder rate.

        @values
        - 0x0000 -- Blank frame
        - 0x0001 -- Eighth rate
        - 0x0002 -- Quarter rate
        - 0x0003 -- Half rate
        - 0x0004 -- Full rate @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_cdma_set_enc_minmax_rate_t */


/** @ingroup cvd_cvs_cmd_cdma_set_enc_rate_mod
  Sets the rate modulation parameter on the encoder.

  @apr_msgpayload{vss_istream_cmd_cdma_set_enc_rate_modulation_t}
  @table{weak__vss__istream__cmd__cdma__set__enc__rate__modulation__t}

  @detdesc
  In the mode parameter, the equation for the vocoder rate modulation 
  ratio is: \n
  @verbatim
                2X+1
     ratio = --------
              2(X+1)


  @endverbatim
  @par
  Where:
  - X is S or 1/S, depending on the selected option
  - S is the rate limit factor
  @par
  S is an integer that causes the ratio, when multiplied by 14.4 kbps (full
  rate), to become the average bitrate.
  @par
  For convenience, following are some precalculated modes for EVRC:
  @par
@verbatim
0x00000000    Target 14.4 kbps (8/8 rate) on the average
              Bit values ORed:
              0x00000000    Vocoder rate modulation disabled

0x0000000F    Target 12.2 kbps (7/8 rate) on the average
              Bit values ORed:
              0x00000001    Vocoder rate modulation enabled
              0x00000002    Select X=S
              0x0000000C    Rate limit factor S=3
@endverbatim
@newpage
@verbatim
0x00000007    Target 11.2 kbps (6/8 rate) on the average
              Bit values ORed:
              0x00000001    Vocoder rate modulation enabled
              0x00000002    Select X=S
              0x00000004    Rate limit factor S=1

0x00000005    Target 9.0 kbps (5/8 rate) on the average
              Bit values ORed:
              0x00000001    Vocoder rate modulation enabled
              0x00000000    Select X=1/S
              0x00000004    Rate limit factor S=3

0x00000003    Target 7.2 kbps (4/8 rate) on the average
              (1/2 rate is not supported for EVRC; must use 0x0000)
              Bit values ORed:
              0x00000001    Vocoder rate modulation enabled
              0x00000001    Select X=S
              0x00000000    Rate limit factor S=0


@endverbatim

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION ( 0x0001101C )

/* Type definition for #vss_istream_cmd_cdma_set_enc_rate_modulation_t. */
typedef struct vss_istream_cmd_cdma_set_enc_rate_modulation_t vss_istream_cmd_cdma_set_enc_rate_modulation_t;

/* For struct vss_istream_cmd_cdma_set_enc_rate_modulation_t, 'mode' is
  equivalent to the 'rateModulationCmd' in the "6550 VDSP Firmware Interface
  Specification". */

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_cdma_set_enc_rate_modulation_t
@{ */
/* Payload structure for the
    VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION command.
*/
struct vss_istream_cmd_cdma_set_enc_rate_modulation_t
{
   uint32_t mode;
   /**< Sets the vocoder Reduced Rate Modulation mode.

        @values{for bit 0}
        - 0 -- Disables vocoder rate modulation
        - 1 -- Enables vocoder rate modulation

        @values{for bit 1}
        - 0 -- Selects X=1/S
        - 1 -- Selects X=S

        @values{for bits 9 to 2}
        - Rate limit factor is the value of S (8-bit unsigned integer)

        Bits 31 to 10 are reserved; they must be set to 0. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_cdma_set_enc_rate_modulation_t */


/****************************************************************************
 * QCELP-13K VOCODER APIS                                                   *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_qcelp13k_set_rate
  Sets the Qualcomm Code Excited Linear Prediction 13K (QCELP-13K) average
  encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_qcelp13k_set_rate_t}
  @table{weak__vss__istream__cmd__voc__qcelp13k__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE ( 0x0001113B )

/* Type definition for vss_istream_cmd_voc_qcelp13k_set_rate_t. */
typedef struct vss_istream_cmd_voc_qcelp13k_set_rate_t vss_istream_cmd_voc_qcelp13k_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_qcelp13k_set_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_qcelp13k_set_rate_t
{
   uint32_t rate;
   /**< Sets the average vocoder rate.

        @values
        - 0x00000000 -- 14.4 kbps
        - 0x00000001 -- 12.2 kbps
        - 0x00000002 -- 11.2 kbps
        - 0x00000003 --  9.0 kbps
        - 0x00000004 --  7.2 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_qcelp13k_set_rate_t */


/****************************************************************************
 * 4GV-NB VOCODER APIs                                                      *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_4gvnb_set_rate
  Sets the 4GV-NB average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvnb_set_rate_t}
  @table{weak__vss__istream__cmd__voc__4gvnb__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE ( 0x0001113C )

/* Type definition for vss_istream_cmd_voc_4gvnb_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvnb_set_rate_t vss_istream_cmd_voc_4gvnb_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvnb_set_rate_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_4gvnb_set_rate_t
{
   uint32_t rate;
   /**< Sets the average vocoder rate.

        @values
        - 0x00000000 -- 10.0 kbps
        - 0x00000001 --  8.5 kbps
        - 0x00000002 --  7.5 kbps
        - 0x00000003 --  7.0 kbps
        - 0x00000004 --  6.6 kbps
        - 0x00000005 --  6.2 kbps
        - 0x00000006 --  5.8 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvnb_set_rate_t */


/****************************************************************************
 * 4GV-WB VOCODER APIS                                                      *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_4gvwb_set_rate
  Sets the 4GV-WB average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvwb_set_rate_t}
  @table{weak__vss__istream__cmd__voc__4gvwb__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE ( 0x0001113D )

/* Type definition for vss_istream_cmd_voc_4gvwb_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvwb_set_rate_t vss_istream_cmd_voc_4gvwb_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvwb_set_rate_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_4gvwb_set_rate_t
{
   uint32_t rate;
   /**< Sets the average vocoder rate.

        @values
        - 0x00000000 --  8.5 kbps
        - 0x00000004 -- 10.0 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvwb_set_rate_t */


/****************************************************************************
 * 4GV-NW VOCODER APIS                                                      *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_4gvnw_set_rate
  Sets the 4GV-NW average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvnw_set_rate_t}
  @table{weak__vss__istream__cmd__voc__4gvnw__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE ( 0x00011281 )

/* Type definition for vss_istream_cmd_voc_4gvnw_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvnw_set_rate_t vss_istream_cmd_voc_4gvnw_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvnw_set_rate_t
@{ */
/* Payload structure for the
    VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE command.
*/
struct vss_istream_cmd_voc_4gvnw_set_rate_t
{
   uint32_t rate;
   /**< Sets the average vocoder rate.

        @values
        - 0x00000000 --  8.5 kbps
        - 0x00000001 --  9.3 kbps
        - 0x00000002 --  7.5 kbps
        - 0x00000003 --  7.0 kbps
        - 0x00000004 --  6.6 kbps
        - 0x00000005 --  6.2 kbps
        - 0x00000006 --  5.8 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvnw_set_rate_t */


/****************************************************************************
 * 4GV-NW-2K VOCODER APIS                                                   *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_4_set_rate
  Sets the 4th Generation Narrow-Wide-2K 2K (4GV-NW-2K) average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvnw2k_set_rate_t}
  @table{weak__vss__istream__cmd__voc__4gvnw2k__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE ( 0x00012E5F )

/* Type definition for vss_istream_cmd_voc_4gvnw2k_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvnw2k_set_rate_t vss_istream_cmd_voc_4gvnw2k_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvnw2k_set_rate_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE command.
*/
struct vss_istream_cmd_voc_4gvnw2k_set_rate_t
{
   uint32_t rate;
   /**< Sets the average vocoder rate.

        @values
        - 0x00000000 --  8.5 kbps
        - 0x00000001 --  9.3 kbps
        - 0x00000002 --  7.5 kbps
        - 0x00000003 --  2.4 kbps
        - 0x00000004 --  6.6 kbps
        - 0x00000005 --  6.2 kbps
        - 0x00000006 --  5.8 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvnw2k_set_rate_t */


/****************************************************************************
 * AMR VOCODER APIS                                                         *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_amr_set_enc_rate
  Sets the Adaptive Multirate Codec (AMR) encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_amr_set_enc_rate_t}
  @table{weak__vss__istream__cmd__voc__amr__set__enc__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE ( 0x0001113E )

/* Type definition for vss_istream_cmd_voc_amr_set_enc_rate_t. */
typedef struct vss_istream_cmd_voc_amr_set_enc_rate_t vss_istream_cmd_voc_amr_set_enc_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_amr_set_enc_rate_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE
    command.
*/
struct vss_istream_cmd_voc_amr_set_enc_rate_t
{
   uint32_t mode;
   /**< Sets the AMR encoder rate.

        @values
        - 0x00000000 -- 4.75 kbps
        - 0x00000001 -- 5.15 kbps
        - 0x00000002 -- 5.90 kbps
        - 0x00000003 -- 6.70 kbps
        - 0x00000004 -- 7.40 kbps
        - 0x00000005 -- 7.95 kbps
        - 0x00000006 -- 10.2 kbps
        - 0x00000007 -- 12.2 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_amr_set_enc_rate_t */


/****************************************************************************
 * AMR-WB VOCODER APIS                                                      *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_amrwb_set_enc_rate
  Sets the Adaptive Multirate Codec Wideband (AMR-WB) encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_amrwb_set_enc_rate_t}
  @table{weak__vss__istream__cmd__voc__amrwb__set__enc__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE ( 0x0001113F )

/* Type definition for vss_istream_cmd_voc_amrwb_set_enc_rate_t. */
typedef struct vss_istream_cmd_voc_amrwb_set_enc_rate_t vss_istream_cmd_voc_amrwb_set_enc_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_amrwb_set_enc_rate_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE
    command.
*/
struct vss_istream_cmd_voc_amrwb_set_enc_rate_t
{
   uint32_t mode;
   /**< Sets the AMR-WB encoder rate.

        @values
        - 0x00000000 --  6.60 kbps
        - 0x00000001 --  8.85 kbps
        - 0x00000002 -- 12.65 kbps
        - 0x00000003 -- 14.25 kbps
        - 0x00000004 -- 15.85 kbps
        - 0x00000005 -- 18.25 kbps
        - 0x00000006 -- 19.85 kbps
        - 0x00000007 -- 23.05 kbps
        - 0x00000008 -- 23.85 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_amrwb_set_enc_rate_t */


/****************************************************************************
 * EVS VOCODER APIS                                                      *
 ****************************************************************************/

/** @addtogroup cvd_cvs_cmd_evs_set_enc_op_mode
@{ */
/** Sets the Enhanced Voice Services (EVS) encoder operating mode.

  @apr_msgpayload{vss_istream_cmd_set_evs_voc_enc_operating_mode_t}
  @table{weak__vss__istream__cmd__set__evs__voc__enc__operating__mode__t}
  @newpage

  @detdesc
  The operating mode consists of the bit rate and the associated bandwidth of
  the bit rate.
  @par
  This command has no effect and returns an error for one or more of the
  following conditions:
   - The stream is not using the EVS vocoder
   - The payload contains an invalid bit rate or bandwidth
   - The payload contains an invalid EVS bit rate with respect to the
     associated bandwidth
   - The bandwidth is higher than the sampling rates set on the postprocessing
     blocks by default values or from
     #VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE ( 0x00013166 )

/** Narrowband operating audio bandwidth for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB ( 0 )

/** Wideband operating audio bandwidth for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB ( 1 )

/** Super wideband operating audio bandwidth for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB ( 2 )

/** Fullband operating audio bandwidth for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB ( 3 )

/** @} */ /* end_addtogroup cvd_cvs_cmd_evs_set_enc_op_mode */

/* Type definition for vss_istream_cmd_set_evs_voc_enc_operating_mode_t. */
typedef struct vss_istream_cmd_set_evs_voc_enc_operating_mode_t vss_istream_cmd_set_evs_voc_enc_operating_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_evs_voc_enc_operating_mode_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE
    command.
*/
struct vss_istream_cmd_set_evs_voc_enc_operating_mode_t
{
   uint8_t mode;
   /**< Sets the operating bit rate on the vocoder encoder.

        @values
        - 0x00000000 --  6.60 kbps, @if OEM_only AMR-WB @endif
                         IO (WB)
        - 0x00000001 --  8.85 kbps, @if OEM_only AMR-WB @endif
                         IO (WB)
        - 0x00000002 -- 12.65 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000003 -- 14.25 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000004 -- 15.85 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000005 -- 18.25 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000006 -- 19.85 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000007 -- 23.05 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000008 -- 23.85 kbps, @if OEM_only AMR-WB @endif
                        IO (WB)
        - 0x00000009 -- 5.90 kbps, EVS (NB, WB)
        - 0x0000000A -- 7.20 kbps, EVS (NB, WB)
        - 0x0000000B -- 8.00 kbps, EVS (NB, WB)
        - 0x0000000C -- 9.60 kbps, EVS (NB, WB, SWB)
        - 0x0000000D -- 13.20 kbps, EVS (NB, WB, SWB)
        - 0x0000000E -- 16.40 kbps, EVS (NB, WB, SWB, FB)
        - 0x0000000F -- 24.40 kbps, EVS (NB, WB, SWB, FB)
        - 0x00000010 -- 32.00 kbps, EVS (WB, SWB, FB)
        - 0x00000011 -- 48.00 kbps, EVS (WB, SWB, FB)
        - 0x00000012 -- 64.00 kbps, EVS (WB, SWB, FB)
        - 0x00000013 -- 96.00 kbps, EVS (WB, SWB, FB)
        - 0x00000014 -- 128.00 kbps, EVS (WB, SWB, FB) @tablebulletend */

  uint8_t bandwidth;
  /**< Specifies the operating audio bandwidth on the vocoder encoder.

        @values
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB 
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB 
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB @tablebulletend */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_evs_voc_enc_operating_mode_t */

/** @ingroup cvd_cvs_cmd_evs_set_enc_chmode_en
  Enables the encoder Channel Aware mode for EVS.

  @apr_msgpayload{vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t}
  @table{weak__vss__istream__cmd__set__evs__enc__channel__aware__mode__enable__t}

  @detdesc
  This command has no effect and returns an error in either of the following
  conditions:
  - The stream is not using the EVS vocoder.
  - The payload parameters are invalid.
  @par
  Channel Aware mode operates only at 13.2 kbps on WB or SWB bandwidths.
  @par
  To start the feature before the bit rate changes to 13.2 kbps, Qualcomm
  recommends enabling Channel Aware mode before
  #VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE.
  @par
  For more information on Channel Aware mode, refer to
  @xhyperref{3GPPTS26445,3GPP TS 26.445}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE ( 0x00013168 )

/* Type definition for vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t. */
typedef struct vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE command.
*/
struct vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t
{
   uint8_t fec_offset;
   /**< Specifies the forward error correction offset.

        @values 2, 3, 5, 7 */

   uint8_t fer_rate;
   /**< Specifies the frame error rate threshold.

        @values
        - 0 -- Low
        - 1 -- High @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t */


/** @ingroup cvd_cvs_cmd_evs_set_enc_chmode_dis
  Disables the encoder Channel Aware mode for EVS.

  @apr_msgpayload
  None.

  @detdesc
  This command has no effect and returns an error in either of the following
  conditions:
  - The stream is not using the EVS vocoder 
  - Channel Aware mode is already disabled

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE ( 0x0001316C )


/****************************************************************************
 * DTMF GENERATION APIS                                                     *
 ****************************************************************************/

/** @addtogroup cvd_cvs_cmd_set_dtmf_generation 
@{ */
/** Starts and stops Dual-tone Multifrequency (DTMF) generation.

  @apr_msgpayload{vss_istream_cmd_set_dtmf_generation_t}
  @table{weak__vss__istream__cmd__set__dtmf__generation__t}

  @detdesc
  Completion of DTMF generation (either due to a Stop command or because the
  requested duration has elapsed) is indicated to the client via the
  #VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED event.
  @par
  A new DTMF generation command replaces a previously unfinished command in the
  same direction. If the generation of a single tone is needed, set the tone_1
  and tone_2 parameters to the same value. If the stream is to be closed before
  the requested tone duration has elapsed, first stop the tone that is
  currently playing, thus ensuring proper rampdown and avoiding audio
  artifacts (pops).

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_DTMF_GENERATION ( 0x00011023 )

/** Direction of DTMF generation is on the Tx path. */
#define VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX ( 0 )

/** Disables DTMF generation mixing. The generated DTMF replaces the speech. */
#define VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE ( 0 )

/** Enables DTMF generation mixing with the speech. */
#define VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE ( 1 )

/** @} */ /* end_addtogroup cvd_cvs_cmd_set_dtmf_generation */

/* Type definition for vss_istream_cmd_set_dtmf_generation_t. */
typedef struct vss_istream_cmd_set_dtmf_generation_t vss_istream_cmd_set_dtmf_generation_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_dtmf_generation_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_DTMF_GENERATION
    command.
*/
struct vss_istream_cmd_set_dtmf_generation_t
{
   uint16_t direction;
   /**< Specifies the path on which the DTMF tone is generated. Currently,
        only Tx is supported.

        @values #VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX */

   uint16_t mix_flag;
   /**< Specifies whether DTMF is mixed with a speech flag.

        @values
        - #VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE
        - #VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE @tbend2 */

   uint16_t tone_1;
   /**< Frequency of tone 1 for DTMF.

        @values 100 Hz to 4 kHz */

   uint16_t tone_2;
   /**< Frequency of tone 2 for DTMF.

        @values 100 Hz to 4 kHz */

   uint16_t gain;
   /**< DTMF tone linear gain. Because the level of tone generation is fixed
        at 0 dBFS, this parameter must be set to a negative value.

        @values Negative linear value in Q13 format */

   int16_t duration;
   /**< Duration of the tone with a 5 ms resolution. The duration includes
        ramp-up and ramp-down periods of 1 ms and 2 ms, respectively.

        @values
        - -1 -- Infinite duration
        -  0 -- Stop infinite tone
        - >0 -- Finite duration in milliseconds @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_istream_cmd_set_dtmf_generation_t */


/** @ingroup cvd_cvs_evt_dtmf_generation_ended
  Sent by the stream to the client that enabled DTMF generation. This event
  indicates that tone generation has ended either because the client issued a
  Stop command or the duration requested by the client has elapsed.

  @apr_msgpayload{vss_istream_evt_dtmf_generation_ended_t}
  @table{weak__vss__istream__evt__dtmf__generation__ended__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED ( 0x00011032 )

/* Type definition for vss_istream_evt_dtmf_generation_ended_t. */
typedef struct vss_istream_evt_dtmf_generation_ended_t vss_istream_evt_dtmf_generation_ended_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_dtmf_generation_ended_t 
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED
    event.
*/
struct vss_istream_evt_dtmf_generation_ended_t
{
   uint16_t direction;
   /**< Specifies the path on which the DTMF tone was generated. Currently,
        only Tx is supported.

        @values #VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_dtmf_generation_ended_t */


/****************************************************************************
 * DTMF DETECTION APIS                                                      *
 ****************************************************************************/

/** @addtogroup cvd_cvs_cmd_set_rx_dtmf_detect
@{ */
/** Configures DTMF detection on the Rx path.

  @apr_msgpayload{vss_istream_cmd_set_rx_dtmf_detection_t}
  @table{weak__vss__istream__cmd__set__rx__dtmf__detection__t}

  @detdesc
  The DTMF detection status is sent to the client sending this command through
  #VSS_ISTREAM_EVT_RX_DTMF_DETECTED events.
  @par
  Only one client can request Rx DTMF detection on a given stream at a time.
  If a client requests Rx DTMF detection while the previous client's Rx DTMF
  detection is still active, the request fails.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  A valid session must have been created.
*/
#define VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION ( 0x00011027 )

/** DTMF detection enable flag. */
#define VSS_ISTREAM_RX_DTMF_DETECTION_ENABLE ( 1 )

/** DTMF detection disable flag. */
#define VSS_ISTREAM_RX_DTMF_DETECTION_DISABLE ( 0 )

/** @} */ /* end_addtogroup cvd_cvs_cmd_set_rx_dtmf_detect */

/* Type definition for vss_istream_cmd_set_rx_dtmf_detection_t. */
typedef struct vss_istream_cmd_set_rx_dtmf_detection_t vss_istream_cmd_set_rx_dtmf_detection_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_rx_dtmf_detection_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION
    command.
*/
struct vss_istream_cmd_set_rx_dtmf_detection_t
{
   uint32_t enable;
   /**< Specifies whether Rx DTMF detection is enabled.

        @values
        - #VSS_ISTREAM_RX_DTMF_DETECTION_ENABLE
        - #VSS_ISTREAM_RX_DTMF_DETECTION_DISABLE @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_istream_cmd_set_rx_dtmf_detection_t */


/** @ingroup cvd_cvs_evt_rx_dtmf_detected
  Sent by the stream to the client that enables Rx DTMF detection whenever DTMF
  is detected on the Rx path.

  @apr_msgpayload{vss_istream_evt_rx_dtmf_detected_t}
  @table{weak__vss__istream__evt__rx__dtmf__detected__t}

  @detdesc
  The DTMF detection feature can only be used to detect DTMF frequencies as
  listed in Section 6 of the @xhyperref{S2,ITU-T Recommendation Q.23}).
  Therefore, the possible values for the low_freq and high_freq parameters are
  as listed in the message payload (vss_istream_evt_rx_dtmf_detected_t).

  @return
  None.

  @dependencies
  A valid session must have been created.
*/
#define VSS_ISTREAM_EVT_RX_DTMF_DETECTED ( 0x0001101A )

/* Type definition for vss_istream_evt_rx_dtmf_detected_t. */
typedef struct vss_istream_evt_rx_dtmf_detected_t vss_istream_evt_rx_dtmf_detected_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_rx_dtmf_detected_t 
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_RX_DTMF_DETECTED event.
*/
struct vss_istream_evt_rx_dtmf_detected_t
{
   uint16_t low_freq;
   /**< Low frequency detected in the DTMF tone.

        @values 697, 770, 852, 941 Hz */

   uint16_t high_freq;
   /**< Detected high frequency.

        @values 1209, 1336, 1477, 1633 Hz */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_rx_dtmf_detected_t */


/****************************************************************************
 * EAMR EVENTS                                                              *
 ****************************************************************************/

/** @ingroup cvd_cvs_evtclass_eamr_mode_chg
  Class that identifies enhanced AMR (eAMR) mode change events.
  This class comprises one event: #VSS_ISTREAM_EVT_EAMR_MODE_CHANGED. 
*/
#define VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE ( 0x00012E58 )

/** @addtogroup cvd_cvs_evt_eamr_mode_chgd
@{ */
/** Sent by the stream to the client to notify the client that the eAMR
  mode changed.

  @apr_msgpayload{vss_istream_evt_eamr_mode_changed_t}
  @table{weak__vss__istream__evt__eamr__mode__changed__t}

  @detdesc
  To receive this event, the client must first use
  #VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS to register to listen for the
  #VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE event class with the CVS module.
  @par
  To stop receiving this event, the client must use
  #VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS to cancel the
  VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE event class.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_EAMR_MODE_CHANGED ( 0x00012E59 )

/** eAMR narrowband mode. */
#define VSS_ISTREAM_EAMR_MODE_NARROWBAND ( 0 )

/** eAMR wideband mode. */
#define VSS_ISTREAM_EAMR_MODE_WIDEBAND ( 1 )

/** @} */ /* end_addtogroup cvd_cvs_evt_eamr_mode_chgd */

/* Type definition for vss_istream_evt_eamr_mode_changed_t. */
typedef struct vss_istream_evt_eamr_mode_changed_t vss_istream_evt_eamr_mode_changed_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_eamr_mode_changed_t
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_EAMR_MODE_CHANGED event.
*/
struct vss_istream_evt_eamr_mode_changed_t
{
   uint32_t mode;
   /**< Specifies the eAMR mode.

        @values
        - #VSS_ISTREAM_EAMR_MODE_NARROWBAND
        - #VSS_ISTREAM_EAMR_MODE_WIDEBAND @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_eamr_mode_changed_t */


/****************************************************************************
 * EVS EVENTS                                                              *
 ****************************************************************************/

/** @ingroup cvd_cvs_evtclass_evs_bandwidth_chg
  Event class that identifies EVS bandwidth change events. This event class
  comprises the #VSS_ISTREAM_EVT_EVS_RX_BANDWIDTH_CHANGED event.
*/
#define VSS_ISTREAM_EVENT_CLASS_EVS_BANDWIDTH_CHANGE ( 0x000131DC )


/** @ingroup cvd_cvs_evt_evs_rx_bandwidth_chg
  Event sent by the stream to the client to notify the client that the
  EVS Rx bandwidth changed.

  @apr_msgpayload{vss_istream_evt_evs_rx_bandwidth_changed_t}
  @table{weak__vss__istream__evt__evs__rx__bandwidth__changed__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_EVS_RX_BANDWIDTH_CHANGED ( 0x000131DD )

/* Type definition for #vss_istream_evt_evs_rx_bandwidth_changed_t. */
typedef struct vss_istream_evt_evs_rx_bandwidth_changed_t vss_istream_evt_evs_rx_bandwidth_changed_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_evs_rx_bandwidth_changed_t
@{ */
/* Payload structure for the
 *         #VSS_ISTREAM_EVT_EVS_RX_BANDWIDTH_CHANGED event.
*/
struct vss_istream_evt_evs_rx_bandwidth_changed_t
{
   uint8_t bandwidth;
   /**< EVS bandwidth.

        @values
         - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB
         - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB
         - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB
         - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_evs_rx_bandwidth_changed_t */


/****************************************************************************
 * NOTIFICATIONS ISSUED BY THE STREAM TO ITS CLIENTS                        *
 ****************************************************************************/

/** @ingroup cvd_cvs_evt_ready
  Sent by the stream session to the client when streaming is active.

  @apr_msgpayload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_READY ( 0x000110FC )

/** @ingroup cvd_cvs_evt_not_ready
  Sent by the stream session to the client when streaming is inactive.

  @apr_msgpayload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_NOT_READY ( 0x000110FD )

/****************************************************************************
 * Interface for setting Vocoder parameters to the Stream.                  *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_set_voc_param
  Configures a vocoder parameter for a stream. A single command contains
  only one vocoder parameter.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_istream_cmd_set_voc_param_t}
  @table{weak__vss__istream__cmd__set__voc__param__t} @newpage

  @par Parameter data variable payload (vss_istream_voc_param_data_t)
  @tablens{weak__vss__istream__voc__param__data__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD. Follow the shared memory usage guidelines in
  Section @xref{dox:ShMemGuidelines}.
  @par
  The client must not modify or unmap the memory until a result response is
  received from the CVD.
*/
#define VSS_ISTREAM_CMD_SET_VOC_PARAM ( 0x000131EE )

typedef struct vss_istream_cmd_set_voc_param_t vss_istream_cmd_set_voc_param_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_voc_param_t
@{ */
struct vss_istream_cmd_set_voc_param_t
{
  uint32_t mem_handle;
    /**< Unique identifier for the shared memory address (physical/virtual) of
         the parameter data payload.
         The aDSP returns this memory map handle through #VSS_IMEMORY_RSP_MAP.

         This field is used for out-of-band messages only. If the parameter
         data is in-band, set this field to 0.

         @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %vss_istream_voc_param_data_t structures immediately
              follow, whose total size is mem_size bytes.
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is mem_size bytes. @tablebulletend */

  uint64_t mem_address;
    /**< Shared memory address of the out-of-band parameter data payload.

         If the mem_handle is 0 (in-band), this field is ignored. */

  uint32_t mem_size;
    /**< Size of the memory allocated for the parameter data payload. This
         field is used for both in-band and out-of-band data.

         @values > 0 bytes

         For out-of-band parameter data, the amount of memory allocated at
         mem_address must meet the requirement as specified in
         Section&nbsp;@xref{dox:ShMemGuidelines}. */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  vss_istream_voc_param_data_t param_data[];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif

}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_istream_cmd_set_voc_param_t */

typedef struct vss_istream_voc_param_data_t vss_istream_voc_param_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_voc_param_data_t
@{ */
/* Parameter data structure used by the
    #VSS_ISTREAM_CMD_SET_VOC_PARAM command. */
/** When parameter data is in-band, immediately following this structure are
    param_size bytes of parameter data.
*/
struct vss_istream_voc_param_data_t
{
  uint32_t media_id;
    /**< Valid ID of the media format.

         @values
         - See Section @xref{dox:MediaDefinitions}
         - If a vocoder media format ID is not supplied, set this field to
           #VSS_MEDIA_ID_NONE @tablebulletend */

  uint32_t param_id;
    /**< Valid ID of the parameter.

         @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t direction_id;
    /**< Unique ID that indicates the direction on which the parameter is set.

         @values
         - #VSS_ISTREAM_DIRECTION_TX
         - #VSS_ISTREAM_DIRECTION_RX 
         - #VSS_ISTREAM_DIRECTION_TX_RX @tablebulletend */

  uint32_t param_size;
    /**< Size of the parameter data payload. This value is a multiple of
         4&nbsp;bytes.

         @values > 0 bytes */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint32_t param_data[param_size];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_istream_voc_param_data_t */

/** @addtogroup cvd_cvs_cmd_set_voc_param
@{ */
/** ID of the packet loss concealment (PLC) vocoder parameter. @newpage */
#define VSS_ISTREAM_PARAM_PLC ( 0x00010E9B ) 

/** Valid directions for the parameter data. */
/* part of #vss_istream_cmd_set_voc_param_t.
*/
typedef enum vss_istream_param_direction_t
{
   VSS_ISTREAM_DIRECTION_TX,     /**< Direction is the Tx path only. */
   VSS_ISTREAM_DIRECTION_RX,     /**< Direction is the Rx path only. */
   VSS_ISTREAM_DIRECTION_TX_RX   /**< Direction is both Tx and Rx paths. */
} /** @cond */vss_istream_param_direction_t/** @endcond */;

/** @} *//* end_addtogroup cvd_cvs_cmd_set_voc_param */

#endif /* __VSS_ISTREAM_PUBLIC_IF_H__ */

