#ifndef __VSS_COMMON_PUBLIC_IF_H__
#define __VSS_COMMON_PUBLIC_IF_H__

/**
  @file vss_common_public_if.h
  @brief This file contains the APR API definitions that are common among
  modules.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2015-2017 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_common_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/16/17     sw    (Tech Pubs) Edited Doxygen comments for AVS 2.8 (SDM845).
07/16/16     sw    (Tech Pubs) Merged Doxygen comments from 8953.2.7. Edited
                    Doxygen comments for AVS 2.8 (MSM8998).
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/

#include "apr_comdef.h"

/*--------------------------------------------------------------------------*
 * MEDIA TYPE DEFINITIONS                                                   *
 *--------------------------------------------------------------------------*/
/** @addtogroup cvd_common_media_defs
@{ */
/** @name &nbsp;
@{ */

/** Default media type ID. */
#define VSS_MEDIA_ID_NONE ( 0x00010FC0 )

/** @cond OEM_only */
/** CDMA variable 13K vocoder format.
  This format is used by the CDMA network as Service Option 17.

  @inputfile{group__weakf__media__fmt__13k.tex}
*/
#define VSS_MEDIA_ID_13K ( 0x00010FC1 )

/** CDMA Enhanced Variable Rate Vocoder (EVRC) format.
  This format is used by the CDMA network as Service Option 3.

  @inputfile{group__weakf__media__fmt__evrc.tex}
*/
#define VSS_MEDIA_ID_EVRC ( 0x00010FC2 )

/** CDMA 4th generation narrowband (EVRC-B) vocoder format.
  This format is used by the CDMA network as Service Option 68.

  @inputfile{group__weakf__media__fmt__4gvnb.tex}
*/
#define VSS_MEDIA_ID_4GV_NB ( 0x00010FC3 )

/** CDMA 4th generation wideband (EVRC-WB) vocoder format.
  This format is used by the CDMA network as Service Option 70.

  @inputfile{group__weakf__media__fmt__4gvwb.tex}
*/
#define VSS_MEDIA_ID_4GV_WB ( 0x00010FC4 )

/** CDMA 4th generation narrow-wide (EVRC-NW) vocoder format.
  This format is used by the CDMA network as Service Option 73.

  @inputfile{group__weakf__media__fmt__4gvnw.tex}
*/
#define VSS_MEDIA_ID_4GV_NW ( 0x00010FC5 )

/** CDMA 4th generation narrow-wide 2K (EVRC-NW2K) vocoder format.
  This format is used by the CDMA network as Service Option 77.

  @inputfile{group__weakf__media__fmt__4gvnw2k.tex}
*/
#define VSS_MEDIA_ID_4GV_NW2K ( 0x00010FD7 )

/** Universal Mobile Telecommunications System (UMTS) AMR-NB vocoder format
  (IF1).

  @inputfile{group__weakf__media__fmt__amrnb.tex}
*/
#define VSS_MEDIA_ID_AMR_NB ( 0x00010FC6 )

/** UMTS AMR-WB vocoder format (IF1).

  @inputfile{group__weakf__media__fmt__amrwb.tex}
*/
#define VSS_MEDIA_ID_AMR_WB ( 0x00010FC7 )

/** Enhanced AMR vocoder format.

  @inputfile{group__weakf__media__fmt__eamr.tex}
*/
#define VSS_MEDIA_ID_EAMR ( 0x00010FD4 )

/** GSM enhanced full rate vocoder format.

  @inputfile{group__weakf__media__fmt__efr.tex}
*/
#define VSS_MEDIA_ID_EFR ( 0x00010FC8 )

/** GSM full rate vocoder format.

  @inputfile{group__weakf__media__fmt__fr.tex}
*/
#define VSS_MEDIA_ID_FR ( 0x00010FC9 )

/** GSM half rate vocoder format.

  @inputfile{group__weakf__media__fmt__hr.tex}
*/
#define VSS_MEDIA_ID_HR ( 0x00010FCA )

/** Enhanced Voice Services (EVS) vocoder format.

  @inputfile{group__weakf__media__fmt__evs.tex}
*/
#define VSS_MEDIA_ID_EVS ( 0x00010FD8 )

/** @endcond */

/** Linear Pulse Code Modulation (PCM) vocoder format; mono, 16 bits,
  little endian, 8 kHz.

  @inputfile{group__weakf__media__fmt__pcm8khz.tex}
*/
#define VSS_MEDIA_ID_PCM_8_KHZ ( 0x00010FCB )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 16 kHz.

  @inputfile{group__weakf__media__fmt__pcm16khz.tex}
*/
#define VSS_MEDIA_ID_PCM_16_KHZ ( 0x00010FCC )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 32 kHz.

  @inputfile{group__weakf__media__fmt__pcm32khz.tex}
*/
#define VSS_MEDIA_ID_PCM_32_KHZ ( 0x00010FD9 )

/** @} */ /* end_name &nbsp; */

/* Linear PCM vocoder format; mono, 16 bits, little endian, 44.1 kHz.

  @inputfile{group__weakf__media__fmt__pcm441khz.tex}
*/
#define VSS_MEDIA_ID_PCM_44_1_KHZ ( 0x00010FD5 )

/** @name &nbsp;
@{ */
/** Linear PCM vocoder format; mono, 16 bits, little endian, 48 kHz.

  @inputfile{group__weakf__media__fmt__pcm48khz.tex}
*/
#define VSS_MEDIA_ID_PCM_48_KHZ ( 0x00010FD6 )

/** @cond OEM_only */

/** G.711 a-law version 2 vocoder format; contains a 20 ms vocoder frame.

  @inputfile{group__weakf__media__fmt__g711alawv2.tex}
*/
#define VSS_MEDIA_ID_G711_ALAW_V2 ( 0x000131A1 )

/** G.711 @mu-law version 2 vocoder format; contains a 20 ms vocoder frame.

  @inputfile{group__weakf__media__fmt__g711mulawv2.tex}
*/
#define VSS_MEDIA_ID_G711_MULAW_V2 ( 0x000131A2 )

/** G.729AB vocoder format; produces two 10 ms vocoder frames every 20 ms.
  For more information, refer to Figure 4 in @xhyperref{S1,RFC 3551}.

  @inputfile{group__weakf__media__fmt__G729.tex}
*/
#define VSS_MEDIA_ID_G729 ( 0x00010FD0 )

/** G.722 vocoder format; contains one 20 ms vocoder frame.

  @inputfile{group__weakf__media__fmt__G722.tex}
*/
#define VSS_MEDIA_ID_G722 ( 0x00010FD1 )

/** DTMF format that contains one DTMF tone payload.

  @inputfile{group__weakf__media__fmt__dtmf.tex}
*/
#define VSS_MEDIA_ID_DTMF ( 0x00013295 )

/** @endcond */
/** @} */ /* end_name &nbsp; */
/** @} */ /* end_addtogroup cvd_common_media_defs */


/****************************************************************************
 * CALIBRATION FRAMEWORK DEFINITIONS                                        *
 ****************************************************************************/

/* Order of Applying Multiple Matching Calibration Entries------------------*/
/** @weakgroup weakf_order_multi_match_cal_entries
  A system configuration is determined by the network, Tx and Rx
  pre/postprocessing (PP) sampling rates, etc.

  When multiple matching calibration entries in the table must
  be applied to a system configuration, the order in which the matching entries
  are applied is determined using a rule called Specific Overwrites N/A, which
  is explained as follows.

  @par Specific Overwrites N/A rule
  A matching calibration entry that contains a non-N/A value in a column is
  applied after all other matching entries that contain N/A values in the same
  column.
  @par
  For all matching entries found during calibration application, this rule is
  applied in reverse order of the columns that were registered.
  Starting with the last column, the
  Specific Overwrites N/A rule is applied on all matching entries. Then the
  rule is applied on the next-to-last column, and so on.
  @par
  Thus, if N columns are provided as part of the calibration registration,
  start from the Nth column:
    -# For all matching entries that contain the non-N/A value in this
       column, apply the Specific Overwrites N/A rule.
    -# After the rule is applied, the matching entries are separated into two
       groups: one group with non-N/A values (group A) and one group with
       N/A values (group B). Then the group order is determined (group A
       is applied after group B).
    -# For each group, apply the rule on the preceding (Nth-1) column.
       Again, two groups are created and their order is determined.
       This process continues on all preceding columns until the calibration
       order for all matching entries is determined.

  @par Example
  Assumptions:
  - Following is the order of columns in the dynamic calibration
    table registered with a specific vocproc session:
     -# #VSS_ICOMMON_CAL_COLUMN_NETWORK
     -# #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE
     -# #VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE
     -# #VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX
     -# #VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE
     -# #VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE
     -# #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID
     -# #VSS_ICOMMON_CAL_COLUMN_FEATURE
  - The N/A column value for column 2 is 0.
  - Three matching entries are found in the vocproc dynamic calibration table
    based on the current system configuration, and they must be applied. Each
    matching entry is represented by a composition of column values for the
    columns listed above. @newpage
     -# Calibration entry 1 is composed by:
@code
<
 #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0 (i.e. N/A Tx PP sampling rate),
       16000 (i.e. 16000 Hz Rx PP sampling rate),
       1 (i.e. volume index 1),
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_MEDIA_ID_NONE,
 VSS_ICOMMON_CAL_FEATURE_NONE
>

@endcode
     -# Calibration entry 2 is composed by:
@if OEM_only
@code
<
 VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0,
       16000,
       1,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
 VSS_MEDIA_ID_EAMR,
 VSS_ICOMMON_CAL_FEATURE_NONE
>

@endcode
@endif
@if ISV_only
@code
<
 VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0,
       16000,
       1,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
 VSS_ICOMMON_CAL_FEATURE_NONE
>

@endcode
@endif
     -# Calibration entry 3 is composed by:
@if OEM_only
@code
<
 #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0,
       16000,
       1,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
 VSS_MEDIA_ID_NONE,
 VSS_ICOMMON_CAL_FEATURE_BEAMR
>


@endcode
@endif
@if ISV_only
@code
<
 #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0,
       16000,
       1,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
 VSS_MEDIA_ID_NONE,
>


@endcode
@endif
  @par
  The Specific Overwrites N/A rule is first applied on column 8
  (VSS_ICOMMON_CAL_COLUMN_FEATURE). As a result, calibration entry 3
  is applied after calibration entries 1 and 2 are applied.
  @par
  Next, to determine the order of applying calibration entries 1 and 2, the
  Specific Overwrites N/A rule is applied on column 7
  (VSS_ICOMMON_CAL_COLUMN_MEDIA_ID). As a result, calibration entry 2
  is applied after calibration entry 1 is applied.
  @par
  Therefore, the final order in which the three calibration entries are
  applied is:
  -# Calibration entry 1 is applied first.
  -# Calibration entry 2 is applied next.
  -# Calibration entry 3 is applied last.
*/
/*--------------------------------------------------------------------------*/

/* Calibration Data Format -------------------------------------------------*/
/** @weakgroup weakf_calib_data_format
  The client provides shared memory that contains calibration data when
  registering calibration data with CVD. The calibration data is in a
  specific format, which is specified as follows. 

  The calibration data buffer contains the following fields at its beginning:

  - Major version -- Calibration data format major version. The currently
    supported major version is 1.
  - Minor version -- Calibration data format minor version. The currently
    supported minor version is 0.

  In Version 1.0 (major version 1, minor version 0) of the calibration data
  format, the calibration data is represented by four tables:
  - Lookup table (LUT) -- Contains the calibration profiles, where each profile
    comprises a list of column values along with a byte offset to the
    content definition table as well as a byte offset to the content data
    offsets table.
  - Content Definition Table (CDFT) -- Contains the lists of <mid, pid> (Module
    ID, Parameter ID) pairs for calibration profiles in the LUT.
  - Content Data Offsets Table (CDOT) -- Contains the location (offsets in
    bytes) of the data table where the calibration data resides for each
    <mid, pid> pair for the lists of <mid, pid> pairs in the CDFT.
  - Data table -- Contains calibration data parameters.

  @par Expected data format
@verbatim
 ---------------------------  --> Calibration data buffer start
| Major version             | --> 32-bit field
 ---------------------------
| Minor version             | --> 32-bit field
 --------------------------- 
| Size in bytes of the LUT  | --> 32-bit field
| that follows              |
 ---------------------------  --> Beginning of the LUT
| Number of LUT entries     | --> 32-bit field
 ---------------------------  --> Entry 1 start
| column_1                  | --> e.g., network_id
 ---------------------------
| column_2                  | --> e.g., tx_pp_sampling_rate
 ---------------------------
|  .                        |
|  .                        |
 ---------------------------
| column_n                  |
 ---------------------------
| CDFT OFFSET               | --> Byte offset from the beginning of the CDFT
|                           |     32-bit field
 ---------------------------
| CDOT OFFSET               | --> Byte offset from the beginning of CDOT
|                           |     32-bit field
 ---------------------------  --> Entry 2 start
| column_1                  |
 ---------------------------
| column_2                  |
|  .                        |
|  .                        |
|  ...                      |
 --------------------------- 
| Size in bytes of the CDFT | --> 32-bit field
| that follows              |
 ---------------------------  --> Entry 1 start (beginning of the CDFT)
| Number of <mid, pid> pairs| --> 32-bit field
 ---------------------------
| mod_id                    | --> 32-bit field
 ---------------------------
| param_id                  | --> 32-bit field
 ---------------------------
| mod_id                    |
 ---------------------------
| param_id                  |
|                           |
|                           |
| ...                       |
 ---------------------------  --> Entry 2 start
| Number of <mid, pid> pairs|
----------------------------
| mod_id                    |
----------------------------
| param_id                  |
----------------------------
| mod_id                    |
----------------------------
| param_id                  |
|                           |
|                           |
| ...                       |
---------------------------- 
| Size in bytes of the CDOT | --> 32-bit field
| that follows              |
 ---------------------------  --> Entry 1 start (beginning of the CDOT)
| Number of data offset     | --> 32-bit field
| entries                   |
----------------------------
| data table offset         | --> 32-bit field
----------------------------
| data table offset         |
----------------------------
|                           |
|                           |
| ...                       |
 ---------------------------  --> Entry 2 start
| Number of data offset     |
| entries                   |
 ---------------------------
| data table offset         |
 ---------------------------
| data table offset         |
 ---------------------------
|                           |
|                           |
| ...                       |
 --------------------------- 
| Size in bytes of the data | --> 32-bit field
| table that follows        |
 ---------------------------  --> Entry 1 start (beginning of the data table)
| data_size                 | --> Size in bytes of the calibration data for
|                           |     a specific <mid, pid> pair; 32-bit field
 ---------------------------
| data                      | --> Calibration data for a specific <mid, pid>
|                           |     pair; field is a multiple of 4 bytes.
|                           |
|                           |
|  ...                      |
 ---------------------------  --> Entry 2 start
| data_size                 |
 ---------------------------
| data                      |
|                           |
|                           |
| ...                       |

@endverbatim
*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN IDS                                                   *
 *--------------------------------------------------------------------------*/
/** @addtogroup cvd_common_module_cal_framework 
@{ */

/** @name Calibration columns
@xreflabel{hdr:CalibrationColumns}
@{ */

/** Calibration column for the network ID (IDs are listed in
  @xhyperref{hdr:CalNetworkID,Network ID column values}).
*/
#define VSS_ICOMMON_CAL_COLUMN_NETWORK ( 0x00011350 )

/** Calibration column for the Rx pre/postprocessing sampling rate. */
#define VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE ( 0x00011351 )

/** Calibration column for the Tx pre/postprocessing sampling rate. */
#define VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE ( 0x00011352 )

/** Calibration column for the Rx volume index.

  Given a specific set of system configurations (composed by network, Tx and
  Rx PP sampling rates, feature, etc., but excluding the Rx volume index), the
  list of <mid, pid> pairs to be calibrated must be the same between the
  different Rx volume indices (except the N/A Rx volume index). The Rx
  volume calibration data is placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX ( 0x00011358 )

/** Calibration column for the media ID (see Section @xref{dox:MediaDefinitions}).
*/
#define VSS_ICOMMON_CAL_COLUMN_MEDIA_ID ( 0x00013081 )

/** Calibration column for the Tx vocoder operating mode
  (modes are listed in @xhyperref{hdr:CalVocClassCol,Vocoder class column
  values}).

  The Tx operating mode for some vocoders can change dynamically during
  runtime. Given a specific set of system configurations (composed by network,
  Tx and Rx PP sampling rates, feature, etc., but excluding the Tx vocoder
  operating mode), the list of <mid, pid> pairs to be calibrated must be the
  same between the different Tx vocoder operating modes (except the N/A Tx
  vocoder operating mode). The Tx vocoder operating mode-specific calibration
  data is placed in the dynamic calibration data table. @newpage
*/
#define VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE ( 0x00013082 )

/** Calibration column for the Rx vocoder operating mode
  (see @xhyperref{hdr:CalVocClassCol,Vocoder class column values}).

  The Rx operating mode for some vocoders can change dynamically during
  runtime. Given a specific set of system configurations (composed by network,
  Tx and Rx PP sampling rates, feature, etc., but excluding the Rx vocoder
  operating mode), the list of <mid, pid> pairs to be calibrated must be the
  same between the different Rx vocoder operating modes (except the N/A Rx
  vocoder operating mode). The Rx vocoder operating mode specific calibration
  data is placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE ( 0x00013083 )

/** Calibration column for the feature
  (see @xhyperref{hdr:CalColFeature,Calibration features}).

  Some features can be enabled or disabled dynamically during runtime.
  Given a specific set of system configurations (composed by network, Tx and
  Rx PP sampling rates, feature, etc., but excluding the feature), the list of
  <mid, pid> pairs to be calibrated must be the same when a feature is
  enabled and when a feature is disabled. The feature specific calibration
  data is placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_FEATURE ( 0x00013084 )

/** @} */ /* end_name Calibration columns */


/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN TYPES                                                 *
 *--------------------------------------------------------------------------*/
/** @name Calibration column types
@xreflabel{hdr:CalibrationColumnTypes}
@{ */

/** 8-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8 ( 0x0001135A )

/** 16-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16 ( 0x0001135B )

/** 32-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32 ( 0x0001135C )

/** 64-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64 ( 0x0001135D )

/** @} */ /* end_name Column types */


/*--------------------------------------------------------------------------*
 * NETWORK ID COLUMN VALUES                                                 *
 *--------------------------------------------------------------------------*/
/** @name Network ID column values
@xreflabel{hdr:CalNetworkID}
@{ */

/** Network ID is not applicable. */
#define VSS_ICOMMON_CAL_NETWORK_ID_NONE ( 0x0001135E )

/** CDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_CDMA ( 0x0001135F )

/** GSM network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_GSM ( 0x00011360 )

/** WCDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_WCDMA ( 0x00011361 )

/** Voice over Internet Protocol (VoIP) network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_VOIP ( 0x00011362 )

/** LTE network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_LTE ( 0x00011363 )

/** @} */ /* end_name Network ID column values */


/*--------------------------------------------------------------------------*
 * TX/RX VOCODER OPERATING MODE COLUMN VALUES                               *
 *--------------------------------------------------------------------------*/
/** @name Vocoder class column values
@xreflabel{hdr:CalVocClassCol}
@{ */

/** Vocoder operating mode is not applicable. */
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE ( 0x00013085 )

/** Common narrowband vocoder operating mode. Represents audio bandwidth up to
    4 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB ( 0x00013086 )

/** Common wideband vocoder operating mode. Represents audio bandwidth up to
    8 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB ( 0x00013087 )

/** Common super wideband vocoder operating mode. Represents audio bandwidth
    up to 16 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB ( 0x00013088 )

/** Common fullband vocoder operating mode. Represents audio bandwidth up to
    24 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB ( 0x00013089 )

/** @} */ /* end_name Vocoder class column values */


/*--------------------------------------------------------------------------*
 * FEATURE COLUMN VALUES                                                    *
 *--------------------------------------------------------------------------*/
/** @name Calibration features
@xreflabel{hdr:CalColFeature}
@{ */

/** Feature is not applicable. */
#define VSS_ICOMMON_CAL_FEATURE_NONE ( 0x0001308A )

/** Wide Voice feature. */
#define VSS_ICOMMON_CAL_FEATURE_WIDEVOICE ( 0x0001308B )

/** @cond OEM_only */
/** Blind enhanced AMR feature. @newpage */
#define VSS_ICOMMON_CAL_FEATURE_BEAMR ( 0x0001308C )

/** Wide Voice 2.0 feature. */
#define VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2 ( 0x000130A7 )
/** @endcond */

/** Custom bandwidth extension feature. */
#define VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE ( 0x00013258 )

/** @} */ /* end_name Calibration features */


/****************************************************************************
 * STREAM/VOCPROC CALIBRATION APIS                                          *
 ****************************************************************************/

/* Type definition for vss_icommon_param_data_t. */
typedef struct vss_icommon_param_data_t vss_icommon_param_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_param_data_t
@{ */
/* Parameter data structure used by the VSS_ICOMMON_CMD_SET_PARAM_V2
   VSS_ICOMMON_RSP_GET_PARAM commands. */
/** Immediately following this structure are param_size bytes of parameter
    data. The structure and size depend on the module_id/param_id combination.
*/
struct vss_icommon_param_data_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

         @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t param_id;
  /**< Valid ID of the parameter.

         @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint16_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

  uint16_t reserved;
    /**< This field must be set to 0. */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint8_t param_data[param_size];
    /**< Array of in-band parameter data

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */

     /*  Additional repetitions of the module_id, param_id, param_size,
         reserved sequence can continue below:
         uint32_t nextmodule_id;
         .
         .
         . */

#endif /* 0 (DOXYGEN_ONLY) */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_param_data_t */
/** @} *//* end_addtogroup cvd_common_module_cal_framework */

/** @ingroup cvd_common_cmd_set_paramv2
  Sets one or more calibration parameters.

  @note1hang This command is intended for private use. Clients must not use
             this command for calibration or UI control purposes.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_icommon_cmd_set_param_v2_t}
  @tablens{weak__vss__icommon__cmd__set__param__v2__t} @newpage

  @par Parameter data variable payload (vss_icommon_param_data_t)
  @tablens{weak__vss__icommon__param__data__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD by following the shared memory usage guidelines
  in Section @xref{dox:ShMemGuidelines}.
  @par
  The parameter data in the memory must be valid and read only until a
  response to this command is received.
*/
#define VSS_ICOMMON_CMD_SET_PARAM_V2 ( 0x0001133D )

/* Type definition for vss_icommon_cmd_set_param_v2_t. */
typedef struct vss_icommon_cmd_set_param_v2_t vss_icommon_cmd_set_param_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_param_v2_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_PARAM_V2 command. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%vss_icommon_param_data_t).
*/
struct vss_icommon_cmd_set_param_v2_t
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
            - A series of %vss_icommon_param_data_t structures immediately
              follow, whose total size is mem_size bytes.
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is mem_size bytes. @tablebulletend */

  uint64_t mem_address;
    /**< Shared memory address of the out-of-band parameter data payload.

         If the mem_handle is 0 (in-band data payload), this field is ignored. */

  uint32_t mem_size;
    /**< Size of the memory allocated for the payload. This field is used
         for both in-band and out-of-band data.

         @values > 0 bytes

         For out-of-band parameter data, the amount of memory allocated at
         mem_address must meet the requirement as specified in
         Section&nbsp;@xref{dox:ShMemGuidelines}. */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  vss_icommon_param_data_t param_data[];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY) */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_param_t */

/** @ingroup cvd_common_cmd_get_paramv2
  Gets a calibration parameter.

  @note1hang This command is intended for private use. Clients must not use
             this function for calibration or UI control purposes.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}. @vertspace{-6}

  @apr_msgpayload{vss_icommon_cmd_get_param_v2_t}
   @tablens{weak__vss__icommon__cmd__get__param__v2__t}

  @return
  #VSS_ICOMMON_RSP_GET_PARAM -- Success, invalid module ID, invalid
  parameter ID.
  @par
  APRV2_IBASIC_RSP_RESULT -- Any failure other than an invalid module
  ID or invalid parameter ID (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD by following the guidelines in
  Section @xref{dox:ShMemGuidelines}.
  @par
  Clients must not modify the memory and the memory must be valid and writable
  until a response to this command is received.
*/
#define VSS_ICOMMON_CMD_GET_PARAM_V2 ( 0x0001133E )

/* Type definition for vss_icommon_cmd_get_param_t. */
typedef struct vss_icommon_cmd_get_param_v2_t vss_icommon_cmd_get_param_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_get_param_v2_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_GET_PARAM_V2 command. */
/** For in-band data, immediately following this structure are mem_size bytes
    of parameter data. The structure and size depend on the module_id/param_id
    combination.
*/
struct vss_icommon_cmd_get_param_v2_t
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
          - A series of %vss_icommon_param_data_t structures immediately
            follow, whose total size is mem_size bytes.
       - Non-NULL -- Out-of-band
          - The payload begins at the specified physical address.
          - The total size is mem_size bytes. @tablebulletend */

  uint64_t mem_address;
  /**< Shared memory address of the out-of-band parameter data payload.

       A single %vss_icommon_param_data_t, which contains the header and the
       algorithm parameter data, is placed at this location.

       If the mem_handle is 0 (in-band), this field is ignored. */

  uint16_t mem_size;
  /**< Size of the memory allocated for the payload. This field is used
       for both in-band and out-of-band data.

       @values > 0 bytes

       For out-of-band parameter data, the amount of memory allocated at
       mem_address must meet the requirement as specified in
       Section&nbsp;@xref{dox:ShMemGuidelines}. */

  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_icommon_cmd_get_param_t */


/** @ingroup cvd_common_resp_get_param
  Returns a calibration parameter in response to the
  #VSS_ICOMMON_CMD_GET_PARAM_V2 command.

  @apr_msgpayload{vss_icommon_rsp_get_param_t}
  @tablens{weak__vss__icommon__rsp__get__param__t}

  @par Parameter data variable payload (vss_icommon_param_data_t)
  @tablens{weak__vss__icommon__param__data__t} @newpage

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ICOMMON_RSP_GET_PARAM ( 0x00011008 )

/* Type definition for vss_icommon_rsp_get_param_t. */
typedef struct vss_icommon_rsp_get_param_t vss_icommon_rsp_get_param_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_rsp_get_param_t
@{ */
/* Payload structure for the VSS_ICOMMON_RSP_GET_PARAM command
   response. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%vss_icommon_param_data_t). The payload depends on the size of the
    parameter.
*/
struct vss_icommon_rsp_get_param_t
{
  uint32_t status;
  /**< Status message (error code).

       @values
       - 0 -- Success
       - 1 -- Invalid module ID
       - 2 -- Invalid parameter ID

       For more information, refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}). */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  vss_icommon_param_data_t param_data[1];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY) */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_rsp_get_param_t */

/* Type definition for vss_icommon_param_data_v3_t. */
typedef struct vss_icommon_param_data_v3_t vss_icommon_param_data_v3_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_param_data_v3_t
@{ */
/* Parameter data structure used by the VSS_ICOMMON_CMD_SET_PARAM_V3
   VSS_ICOMMON_RSP_GET_PARAM_V3 commands. */
/** Immediately following this structure are param_size bytes of parameter
    data. The structure and size depend on the module_id/param_id combination.
*/
struct vss_icommon_param_data_v3_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint16_t instance_id;
    /**< Valid ID of the module instance. */

  uint16_t reserved;
  /**< This field must be set to zero. */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint32_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint8_t param_data[param_size];
  /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */

     /*  Additional repetitions of the module_id, param_id, instance_id,
         reserved, param_size sequence can continue below: \n
         uint32_t nextmodule_id; \n
         . \n
         . \n
         . */

#endif /* 0 (DOXYGEN_ONLY */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_param_data_v3_t */


/** @ingroup cvd_common_cmd_set_paramv3
  Sets one or more calibration parameters to the specified instance of the
  module.

  This command supports module instances. It can be called on different
  instances of the same module.

  @note1hang This command is intended for private use. Clients must not use
             this command for calibration or UI control purposes.

  @note1hang For guidelines on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_icommon_cmd_set_param_v3_t}
  @tablens{weak__vss__icommon__cmd__set__param__v3__t}

  @par Parameter data variable payload (vss_icommon_param_data_v3_t)
  @tablens{weak__vss__icommon__param__data__v3__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD by following the guidelines in
  Section @xref{dox:ShMemGuidelines}.
  The parameter data in the memory must be valid and read-only until a
  response to this command is received.
*/
#define VSS_ICOMMON_CMD_SET_PARAM_V3 ( 0x00013245 )

/* Type definition for vss_icommon_cmd_set_param_v3_t. */
typedef struct vss_icommon_cmd_set_param_v3_t vss_icommon_cmd_set_param_v3_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_param_v3_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_PARAM_V3 command. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%vss_icommon_param_data_v3_t).
*/
struct vss_icommon_cmd_set_param_v3_t
{
  uint32_t mem_handle;
    /**< Pointer to the unique identifier for an address (physical/virtual).

         If the parameter data payload is within the message payload
         (in-band), set this field to 0. The parameter data begins at the
         specified data payload address.

         If the parameter data is out-of-band, this field is the handle to the
         physical address in the shared memory that holds the parameter data. */

  uint64_t mem_address;
    /**< Location of the parameter data payload.

         The payload is an array of %vss_icommon_param_data_v3_t. If the
         mem_handle is 0, this field is ignored. */

  uint32_t mem_size;
    /**< Size of the parameter data payload in bytes.
         This field is applicable to both in-band and out-of-band parameter
         data.

         For an out-of-band parameter, the amount of memory allocated at
         mem_address must meet the requirement as specified in
         Section&nbsp;@xref{dox:ShMemGuidelines}. */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  vss_icommon_param_data_v3_t param_data[];
  /**< Parameter data is present when the data is in-band.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY) */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_param_v3_t */


/** @ingroup cvd_common_cmd_get_paramv3
  Gets a calibration parameter from the specified instance of 
  a module.

  This command can be called on different instances of the same module.

  @note1hang This command is intended for private use. Clients must not use
             this function for calibration or UI control purposes.

  @note1hang For guidelines on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_icommon_cmd_get_param_v3_t}
   @tablens{weak__vss__icommon__cmd__get__param__v3__t}

  @return
  #VSS_ICOMMON_RSP_GET_PARAM_V3 -- Success, invalid module ID, invalid
  parameter ID.
  @par
  APRV2_IBASIC_RSP_RESULT -- Any failure other than an invalid module
  ID or invalid parameter ID (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD by following the guidelines in
  Section @xref{dox:ShMemGuidelines}.
  @par
  Clients must not modify the memory and the memory must be valid and writable
  until a response to this command is received.
*/
#define VSS_ICOMMON_CMD_GET_PARAM_V3 ( 0x00013246 )

/* Type definition for vss_icommon_cmd_get_param_v3_t. */
typedef struct vss_icommon_cmd_get_param_v3_t vss_icommon_cmd_get_param_v3_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_get_param_v3_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_GET_PARAM_V3 command. */
/** For in-band data, immediately following this structure are mem_size bytes
    of parameter data. The structure and size depend on the module_id/param_id
    combination.
*/
struct vss_icommon_cmd_get_param_v3_t
{
  uint32_t mem_handle;
  /**< Pointer to the unique identifier for an address (physical/virtual).

       If the parameter data payload is within the message payload
       (in-band), set this field to 0. The parameter data begins at the
       specified data payload address.

       If the parameter data is out-of-band, this field is the handle to the
       physical address in the shared memory that holds the parameter data. */

  uint64_t mem_address;
  /**< Location to hold the parameter data.

       A single %vss_icommon_param_data_v3_t that contains the header and the
       algorithm's parameter data is placed at this location. If mem_handle
       is 0, this field is ignored. */

  uint16_t mem_size;
  /**< Size of the memory in bytes to hold the parameter.

       This field is applicable to both in-band and out-of-band parameters.
       The size must be large enough to hold the algorithm's parameter data
       plus the header %vss_icommon_param_data_v3_t.

       For an out-of-band parameter, the size must also meet the requirement
       as specified in Section @xref{dox:ShMemGuidelines}. */

  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint16_t instance_id;
    /**< Valid ID of the module instance. */

  uint16_t reserved;
  /**< This field must be set to zero. */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_icommon_cmd_get_param_v3_t */


/** @ingroup cvd_common_resp_get_paramv3
  Returns a calibration parameter in response to the
  #VSS_ICOMMON_CMD_GET_PARAM_V3 command.

  This command can be called on different instances of the same module.

  @apr_msgpayload{vss_icommon_rsp_get_param_v3_t}
  @tablens{weak__vss__icommon__rsp__get__param__v3__t}

  @par Parameter data variable payload (vss_icommon_param_data_v3_t)
  @tablens{weak__vss__icommon__param__data__v3__t} @newpage

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ICOMMON_RSP_GET_PARAM_V3 ( 0x00013247 )

/* Type definition for vss_icommon_rsp_get_param_t. */
typedef struct vss_icommon_rsp_get_param_v3_t vss_icommon_rsp_get_param_v3_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_rsp_get_param_v3_t
@{ */
/* Payload structure for the VSS_ICOMMON_RSP_GET_PARAM_V3 command
   response. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%vss_icommon_param_data_v3_t). The payload depends on the size of the
    parameter.
*/
struct vss_icommon_rsp_get_param_v3_t
{
  uint32_t status;
  /**< Status message (error code). For more information, refer to
       @xrefcond{Q2,80-N1463-2,80-NA601-5}).

       @values
       - 0 -- Success
       - 1 -- Invalid module ID
       - 2 -- Invalid parameter ID @tablebulletend */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  vss_icommon_param_data_v3_t param_data[1];
  /**< Parameter data is present when the data is in-band. */
#endif /* 0 (DOXYGEN_ONLY) */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_rsp_get_param_v3_t */



/****************************************************************************
 * STREAM/VOCPROC UI PROPERTY APIS                                          *
 ****************************************************************************/

/** @ingroup cvd_common_cmd_set_ui_prop
  Sets a UI property.

  @apr_msgpayload{vss_icommon_cmd_set_ui_property_t}
  @tablens{weak__vss__icommon__cmd__set__ui__property__t}

  @detdesc
  This command caches specific UI settings like volume, mute, etc.
  Calibration and other controls are not to use this command.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_UI_PROPERTY ( 0x00011103 )

/* Type definition for vss_icommon_cmd_set_ui_property_t. */
typedef struct vss_icommon_cmd_set_ui_property_t vss_icommon_cmd_set_ui_property_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_ui_property_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_UI_PROPERTY command. */
/** When parameter data is received in-band, immediately following this
    structure are param_size bytes of parameter data.
*/
struct vss_icommon_cmd_set_ui_property_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

         @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t param_id;
  /**< Valid ID of the parameter.

         @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint16_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

  uint16_t reserved;
    /**< This field must be set to 0. */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint8_t param_data[param_size];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY) */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_ui_property_t */


/** @ingroup cvd_common_cmd_get_ui_prop
  Gets a UI property.

  @apr_msgpayload{vss_icommon_cmd_get_ui_property_t}
  @tablens{weak__vss__icommon__cmd__get__ui__property__t}

  @return
  #VSS_ICOMMON_RSP_GET_UI_PROPERTY -- Success; the UI property was retrieved.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure; the UI property was not retrieved (refer
  to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_GET_UI_PROPERTY ( 0x00011104 )

/* Type definition for vss_icommon_cmd_get_ui_property_t. */
typedef struct vss_icommon_cmd_get_ui_property_t vss_icommon_cmd_get_ui_property_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_get_ui_property_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_GET_UI_PROPERTY command. */
/** When parameter data is received in-band, immediately following this
    structure are param_size bytes of parameter data.
*/
struct vss_icommon_cmd_get_ui_property_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t param_id;
  /**< Valid ID of the parameter

       @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_get_ui_property_t */


/** @ingroup cvd_common_resp_get_ui_prop
  Returns a UI property in response to the #VSS_ICOMMON_CMD_GET_UI_PROPERTY
  command.

  @apr_msgpayload{vss_icommon_rsp_get_ui_property_t}
  @tablens{weak__vss__icommon__rsp__get__ui__property__t}

  @return
  None.

  @dependencies
  None.
 */
#define VSS_ICOMMON_RSP_GET_UI_PROPERTY ( 0x00011105 )

/* Type definition for vss_icommon_rsp_get_ui_property_t. */
typedef struct vss_icommon_rsp_get_ui_property_t vss_icommon_rsp_get_ui_property_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_rsp_get_ui_property_t
@{ */
/* Payload structure for the VSS_ICOMMON_RSP_GET_UI_PROPERTY command
    response. */
/** When parameter data is received in-band, immediately following this
    structure are param_size bytes of parameter data.
*/
struct vss_icommon_rsp_get_ui_property_t
{
  uint32_t status;
    /**< Status of the GET_UI_PROPERTY command.

       @values
       - 0 -- Success
       - 1 -- Invalid module ID
       - 2 -- Invalid parameter ID @tablebulletend */

  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xrefcond{Q4,80-NF774-2,80-NA610-2} */

  uint16_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

  uint16_t reserved;
  /**< This field must be set to 0. */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint8_t param_data[param_size];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY) */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_rsp_get_ui_property_t */


/** @ingroup cvd_common_cmd_set_ui_propv2
  Sets a UI property to the specified instance of a module.

  This command can be called on different instances of the same module.

  @apr_msgpayload{vss_icommon_cmd_set_ui_property_v2_t}
  @tablens{weak__vss__icommon__cmd__set__ui__property__v2__t}

  @detdesc
  This command caches specific UI settings like volume, mute, etc.
  Calibration and other controls are not to use this command.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_UI_PROPERTY_V2 ( 0x00013248 )

/* Type definition for vss_icommon_cmd_set_ui_property_v2_t. */
typedef struct vss_icommon_cmd_set_ui_property_v2_t vss_icommon_cmd_set_ui_property_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_ui_property_v2_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_UI_PROPERTY_V2 command. */
/** When parameter data is sent in-band, immediately following this structure
  are param_size bytes of parameter data.
  The structure and size depend on the module_id/param_id/ combination.
*/
struct vss_icommon_cmd_set_ui_property_v2_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint16_t instance_id;
    /**< Valid ID of the module instance. */

  uint16_t reserved;
  /**< This field must be set to zero. */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint32_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint8_t param_data[param_size];
    /**< Array of in-band parameter data.

         For out-of-band data, this field is ignored because the data is part
         of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_ui_property_v2_t */


/** @ingroup cvd_common_cmd_get_ui_propv2
  Gets a UI property from the specified instance of a module.

  This command can be called on different instances of the same module.

  @apr_msgpayload{vss_icommon_cmd_get_ui_property_v2_t}
  @table{weak__vss__icommon__cmd__get__ui__property__v2__t}

  @return
  #VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2 -- Success; the UI property was retrieved.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure; the UI property was not retrieved (refer
  to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2 ( 0x00013249 )

/* Type definition for vss_icommon_cmd_get_ui_property_v2_t. */
typedef struct vss_icommon_cmd_get_ui_property_v2_t vss_icommon_cmd_get_ui_property_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_get_ui_property_v2_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2 command.
*/
struct vss_icommon_cmd_get_ui_property_v2_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint16_t instance_id;
    /**< Valid ID of the module instance. */

  uint16_t reserved;
  /**< This field must be set to zero. */

  uint32_t param_id;
  /**< Valid ID of the parameter

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint32_t param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_get_ui_property_v2_t */


/** @ingroup cvd_common_resp_get_ui_propv2
  Returns a UI property in response to the
  #VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2 command.

  @apr_msgpayload{vss_icommon_rsp_get_ui_property_v2_t}
  @tablens{weak__vss__icommon__rsp__get__ui__property__v2__t}

  @return
  None.

  @dependencies
  None.
 */
#define VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2 ( 0x0001324A )

/* Type definition for vss_icommon_rsp_get_ui_property_v2_t. */
typedef struct vss_icommon_rsp_get_ui_property_v2_t vss_icommon_rsp_get_ui_property_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_rsp_get_ui_property_v2_t
@{ */
/* Payload structure for the VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2 command
    response. */
/** When a UI property is received in-band, it immediately follows the reserved
    field, and its size is indicated in param_size.
*/
struct vss_icommon_rsp_get_ui_property_v2_t
{
  uint32_t status;
    /**< Status of the GET_UI_PROPERTY_V2 command.

       @values
       - 0 -- Success
       - 1 -- Invalid module ID
       - 2 -- Invalid parameter ID @tablebulletend */

  uint32_t module_id;
  /**< Valid ID of the module 

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint16_t instance_id;
  /**< Valid ID of the module instance. */

  uint16_t reserved;
  /**< This field must be set to zero. */

  uint32_t param_id;
  /**< Valid ID of the parameter

       @values Refer to @xrefcond{Q4,80-NF901-2,80-NA690-2} */

  uint32_t param_size;
  /**< Size of the parameter data based on the module_id/param_id
       combination.

       @values > 0 bytes, in multiples of 4 bytes */

#if DOXYGEN_ONLY /* Changed this from 0 so Doxygen will parse this in the PDF */
  uint8_t param_data[param_size];
  /**< Array of in-band parameter data.

       For out-of-band data, this field is ignored because the data is part
       of the shared memory. */
#endif /* 0 (DOXYGEN_ONLY) */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_rsp_get_ui_property_v2_t */



/****************************************************************************
 * VOICE TIMING APIS                                                        *
 ****************************************************************************/

/** @weakgroup weakb_cvd_common_vfr_modes
@{ */
 /** Soft VFR mode flag that indicates the voice data path is driven
    by the software timer reference. */
#define VSS_ICOMMON_VFR_MODE_SOFT ( 0 )

/** Hard VFR mode flag that indicates the voice data path is driven
    by hardware interrupts aligned to the network timing reference. */
#define VSS_ICOMMON_VFR_MODE_HARD ( 1 )
/** @} *//* end_weakgroup weakb_cvd_common_vfr_modes */

/** @addtogroup cvd_common_cmd_set_voice_tmg
@{ */
/**Sets the voice timing parameters and Vocoder Frame Reference (VFR).
  This command is for full control stream clients only.

  @apr_msgpayload{vss_icommon_cmd_set_voice_timing_v2_t}
  @table{weak__vss__icommon__cmd__set__voice__timing__v2__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).
  @par
  Offset values greater than 20000 microseconds result in failures.

  @dependencies
  None.

  @inputfile{group__weakb__cvd__common__vfr__modes.tex}
*/
#define VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 ( 0x00012E66 )

/** @} */ /* end_addtogroup cvd_common_cmd_set_voice_tmg */

/* Type definition for vss_icommon_cmd_set_voice_timing_v2_t. */
typedef struct vss_icommon_cmd_set_voice_timing_v2_t vss_icommon_cmd_set_voice_timing_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_voice_timing_v2_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 command.
*/
struct vss_icommon_cmd_set_voice_timing_v2_t
{
  uint16_t mode;
  /**< Vocoder frame synchronization mode.

       @values
       - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization
       - #VSS_ICOMMON_VFR_MODE_HARD -- 20 ms VFR interrupt
         @tablebulletend */

  uint32_t vsid;
  /**< Voice System ID as defined by @xrefcond{Q4,80-NF774-2,80-NA610-2}. */

  uint16_t enc_offset;
  /**< Offset in microseconds from the VFR to deliver a Tx vocoder packet.

       @values < 20000 microseconds. */

  uint16_t dec_req_offset;
  /**< Offset in microseconds from the VFR to request for an Rx vocoder packet.

       @values < 20000 microseconds. */

  uint16_t dec_offset;
  /**< Offset in microseconds from the VFR to indicate the deadline to
       receive an Rx vocoder packet.

       @values < 20000 microseconds.

       Rx vocoder packets received after this deadline are not guaranteed to
       be processed. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_voice_timing_v2_t */

/** @ingroup cvd_common_cmd_set_vfr_cfg
  Sets the Vocoder Frame Reference (VFR) configuration for a voice call.
  This command is for full control stream clients only.

  @msgpayload{vss_icommon_cmd_set_vfr_config_t}
  @table{weak__vss__icommon__cmd__set__vfr__config__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_VFR_CONFIG ( 0x0001325D )

/* Type definition for vss_icommon_cmd_set_vfr_config_t. */
typedef struct vss_icommon_cmd_set_vfr_config_t vss_icommon_cmd_set_vfr_config_t;

#include "apr_pack_begin.h"
/** @weakgroup weak_vss_icommon_cmd_set_vfr_config_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_VFR_CONFIG command.
*/
struct vss_icommon_cmd_set_vfr_config_t
{
  uint16_t vfr_mode;
  /**< Vocoder frame synchronization mode.

       @values
       - #VSS_ICOMMON_VFR_MODE_SOFT -- Periodicity vfr_cycle_ms
       - #VSS_ICOMMON_VFR_MODE_HARD -- A 20 ms VFR interrupt
       @tablebulletend */

  uint16_t vfr_cycle_duration_ms;
  /**< Vocoder frame reference duration in milliseconds.
       If vfr_mode is #VSS_ICOMMON_VFR_MODE_HARD, it is ignored.

       @values N*20, where N is an integer */

  uint32_t vsid;
  /**< Voice system ID as defined by @xrefcond{Q4,80-NF774-2,80-NA610-2}. */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_icommon_cmd_set_vfr_config_t */

/** @ingroup cvd_common_cmd_set_voice_tmg_ref
  Sent by the client to provide its vocoder packet exchange time reference.

  @msgpayload{vss_icommon_cmd_set_voice_timing_reference_t}
  @table{weak__vss__icommon__cmd__set__voice__timing__reference__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  The Vocoder frame reference must be set via #VSS_ICOMMON_CMD_SET_VFR_CONFIG.
*/
#define VSS_ICOMMON_CMD_SET_VOICE_TIMING_REFERENCE ( 0x0001325E )

/* Type definition for vss_icommon_cmd_set_voice_timing_reference_t. */
typedef struct vss_icommon_cmd_set_voice_timing_reference_t vss_icommon_cmd_set_voice_timing_reference_t;

#include "apr_pack_begin.h"
/** @weakgroup weak_vss_icommon_cmd_set_voice_timing_reference_t
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_VOICE_TIMING_REFERENCE command.
*/
struct vss_icommon_cmd_set_voice_timing_reference_t
{
  uint64_t tx_ref_timstamp_us;
  /**< Client's encoder packet exchange time reference (in microseconds).

       The time reference is used to align the encoder's timeline to the
       client's timeline at the beginning of the vocoder packet exchange
       operation.

       The clock source to be used for interpreting the timestamp is
       product-specific. Currently, the AVTimer is used as the timing reference
       for all products.

       If the stream direction is Rx only, this field is set to 0 and ignored. */

  uint64_t rx_ref_timstamp_us;
  /**< Client's decoder packet exchange time reference (in microseconds).

       The time reference is used to align the decoder's timeline to the
       client timeline at the beginning of the vocoder's packet exchange
       operation.

       The clock source to be used for interpreting the timestamp is
       product-specific. Currently, the AVTimer is used as the timing reference
       for all products.

       If the stream direction is Tx only, this field is set to 0 and ignored. */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_icommon_cmd_set_voice_timing_reference_t */


/****************************************************************************
 * VOICE ACTIVITY EVENTS                                                    *
 ****************************************************************************/

/** @addtogroup cvd_common_evtclass_voice_act
@{ */
/** Class that identifies voice activity events generated during a voice call.
  This class comprises one event: VSS_ICOMMOM_EVT_VOICE_ACTIVITY_UPDATE. */
/* This class of events is for OEMs only. */
#define VSS_ICOMMON_EVENT_CLASS_VOICE_ACTIVITY_UPDATE ( 0x000131EF )

/** Event sent by the MVM to notify a client of ongoing voice activites during
  a voice call.

  @apr_msgpayload{vss_icommon_evt_voice_activity_update_t}
  @table{weak__vss__icommon__evt__voice__activity__update__t}

  @detdesc
  To start receiving this class of events, the client must first use
  #VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS to register to listen for the
  #VSS_ICOMMON_EVENT_CLASS_VOICE_ACTIVITY_UPDATE event class with the MVM module.
  @par
  The dst_addr in the APR command must be set to the MVM address, and the
  dst_port must be set to APR_NULL_V.
  @par
  Multiple voice activity notifications can be sent to the client. The client
  decides how to handle these notifications.
  @par
  To stop receiving this class of events, the client must use
  #VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ICOMMON_EVT_VOICE_ACTIVITY_UPDATE ( 0x000131F0 )

/* Follwing supported voice activities be notified to the client. */

/** Mute is detected during postprocessing on the Tx path. */
#define VSS_ICOMMON_VOICE_ACTIVITY_VPTX_MUTE ( 0x000131F1 )

/** Unmute is detected during postprocessing on the Tx path. */
#define VSS_ICOMMON_VOICE_ACTIVITY_VPTX_UNMUTE ( 0x000131F2 )

/** Mic path is broken. */
#define VSS_ICOMMON_VOICE_ACTIVITY_MIC_BREAK ( 0x000131F3 )

/** Mic path is restored. */
#define VSS_ICOMMON_VOICE_ACITIVTY_MIC_UNBREAK ( 0x000131F4 )

/** UI mute for uplink is applied on the streams. */
#define VSS_ICOMMON_VOICE_ACTIVITY_UI_STREAM_TX_MUTE ( 0x000131F5 )

/** UI unmute for uplink is applied on the streams. */
#define VSS_ICOMMON_VOICE_ACTIVITY_UI_STREAM_TX_UNMUTE ( 0x000131F6 )

/** UI mute for uplink is applied on the vocproc. */
#define VSS_ICOMMON_VOICE_ACTIVITY_UI_VOCPROC_TX_MUTE ( 0x000131F7 )

/** UI unmute for uplink is applied on the vocproc. */
#define VSS_ICOMMON_VOICE_ACTIVITY_UI_VOCPROC_TX_UNMUTE ( 0x000131F8 )

/** @} *//* end_addtogroup cvd_common_evtclass_voice_act */

/* Type definition for vss_icommon_evt_voice_activity_update_t. */
typedef struct vss_icommon_evt_voice_activity_update_t vss_icommon_evt_voice_activity_update_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_evt_voice_activity_update_t
@{ */
/* Payload structure for the VSS_ICOMMOM_EVT_VOICE_ACTIVITY_UPDATE event. */
struct vss_icommon_evt_voice_activity_update_t
{
   uint32_t activity;
   /**< Specifies the voice acitivity.

        @values
        - #VSS_ICOMMON_VOICE_ACTIVITY_VPTX_MUTE
        - #VSS_ICOMMON_VOICE_ACTIVITY_VPTX_UNMUTE
        - #VSS_ICOMMON_VOICE_ACTIVITY_MIC_BREAK
        - #VSS_ICOMMON_VOICE_ACITIVTY_MIC_UNBREAK
        - #VSS_ICOMMON_VOICE_ACTIVITY_UI_STREAM_TX_MUTE
        - #VSS_ICOMMON_VOICE_ACTIVITY_UI_STREAM_TX_UNMUTE
        - #VSS_ICOMMON_VOICE_ACTIVITY_UI_VOCPROC_TX_MUTE
        - #VSS_ICOMMON_VOICE_ACTIVITY_UI_VOCPROC_TX_UNMUTE @vertspace{-28} */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_icommon_evt_voice_activity_update_t */

#endif /* __VSS_COMMON_PUBLIC_IF_H__ */

