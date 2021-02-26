#ifndef __VSS_PARAMS_PUBLIC_IF_H__
#define __VSS_PARAMS_PUBLIC_IF_H__

/**
  @file vss_params_public_if.h
  @brief This file contains module and parameter IDs supported by Core Voice Drivers.
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

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_params_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
05/09/17     sw    (Tech Pubs) Edited Doxygen comments for AVS 2.8 (845).
07/16/16     sw    (Tech Pubs) Edited Doxygen comments for AVS 2.8 (8998).
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


/****************************************************************************
 * MODULE ID DEFINES                                                        *
 ****************************************************************************/

/** @ingroup cvd_common_module_rx_volume
  ID of the Rx volume module.
  This module uses one parameter: VSS_PARAM_VOLUME.
*/
#define VSS_MODULE_RX_VOLUME ( 0x000112D6 )

/** @ingroup cvd_common_module_feature_cfg
  ID of the Feature Configuration module.

  This module contains feature configurations for enablement, operating Rx
  sampling rate, and the list of applicable system configurations.

  This module uses one parameter: VSS_PARAM_HDVOICE_CONFIG.
*/
#define VSS_MODULE_FEATURE_CONFIG ( 0x000130A5 )

/** @ingroup cvd_common_module_cust_feature_cfg
  ID of the Custom Feature Configuration module.

  This module contains feature configurations for enablement, operating Tx and
  Rx sampling rates, and the list of applicable system configurations.
  It is used with the vocproc configuration table to be registered with the CVP.

  This module uses one parameter: VSS_PARAM_CUSTOM_BWE_CONFIG.
*/
#define VSS_MODULE_CUSTOM_FEATURE_CONFIG ( 0x00013259 )

/** @ingroup cvd_common_module_clock_ctrl
  ID of the Clock Control module that controls voice clock voting for custom
  topology voice use cases.

  This module uses one parameter: VSS_PARAM_CLOCK_CONTROL.
*/
#define VSS_MODULE_CLOCK_CONTROL ( 0x0001319C )


/** @ingroup cvd_common_module_generic
  ID of the Generic CVD module, which is used for parameters that are not
  associated with a specific module (such as vocproc configuration parameters).

  @inputfile{group__weakb__cvd__common__mod__generic__num__dev__channels.tex}
*/
#define VSS_MODULE_CVD_GENERIC ( 0x0001316E )

/****************************************************************************
 * FEATURE CONFIG DEFINES                                                   *
 ****************************************************************************/

/*---------------------------------------------------------------------------*
 * Feature Enablement Modes                                                  *
 *---------------------------------------------------------------------------*/

/** @weakgroup weakb_cvd_common_mod_feature_enable_modes
@{ */
/** Sets the feature Enable mode to Default Off. */
#define VSS_PARAM_FEATURE_DEFAULT_OFF ( 0 )

/** Sets the feature Enable mode to Default On. */
#define VSS_PARAM_FEATURE_DEFAULT_ON ( 1 )

/** Sets the feature Enable mode to Forced Off. */
#define VSS_PARAM_FEATURE_FORCED_OFF ( 2 )

/** Sets the feature Enable mode to Forced On. */
#define VSS_PARAM_FEATURE_FORCED_ON ( 3 )

/** @} */ /* end_weakgroup weakb_feature_enable_modes */

/*---------------------------------------------------------------------------*
 * Number of Device Channels Defines                                                *
 *---------------------------------------------------------------------------*/

/** @weakgroup weakb_cvd_common_mod_generic_num_dev_channels
@{ */

/** One device channel. */
#define VSS_NUM_DEV_CHANNELS_1 ( 1 )

/** Two device channels. */
#define VSS_NUM_DEV_CHANNELS_2 ( 2 )

/** Three device channels. */
#define VSS_NUM_DEV_CHANNELS_3 ( 3 )

/** Four device channels. */
#define VSS_NUM_DEV_CHANNELS_4 ( 4 )

/** Five device channels. */
#define VSS_NUM_DEV_CHANNELS_5 ( 5 )

/** Six device channels. */
#define VSS_NUM_DEV_CHANNELS_6 ( 6 )

/** Seven device channels. */
#define VSS_NUM_DEV_CHANNELS_7 ( 7 )

/** Eight device channels. */
#define VSS_NUM_DEV_CHANNELS_8 ( 8 )

/** Maximum number of supported channels. */
#define VSS_MAX_DEV_CHANNELS ( VSS_NUM_DEV_CHANNELS_8 )
/** @} */ /* end_weakgroup weakb_cvd_common_mod_generic_num_dev_channels */

/*---------------------------------------------------------------------------*
 * Voice sampling rate Defines                                                *
 *---------------------------------------------------------------------------*/

/** @weakgroup weakb_cvd_common_mod_clock_sample_rates
@{ */

/** Voice sampling rate is any value. */
#define VSS_PARAM_SAMPLING_RATE_ANY ( 0 )

/** Voice sampling rate is 8 kHz. */
#define VSS_PARAM_SAMPLING_RATE_8K ( 8000 )

/** Voice sampling rate is 16 kHz. */
#define VSS_PARAM_SAMPLING_RATE_16K ( 16000 )

/** Voice sampling rate is 32 kHz. */
#define VSS_PARAM_SAMPLING_RATE_32K ( 32000 )

/** Voice sampling rate is 48 kHz. */
#define VSS_PARAM_SAMPLING_RATE_48K ( 48000 )

/** @} */ /* end_weakgroup weakb_cvd_common_mod_clock_sample_rates */

/****************************************************************************
 * PARAM ID DEFINES                                                         *
 ****************************************************************************/

/** @ingroup cvd_common_module_rx_volume
  ID of the parameter used by VSS_MODULE_RX_VOLUME to set the volume.
  On the Rx vocproc, the parameter data is a 32-bit value in millibels.
*/
#define VSS_PARAM_VOLUME ( 0x000112D7 )

/** @ingroup cvd_common_module_feature_cfg
  ID of the parameter used by VSS_MODULE_FEATURE_CONFIG to apply
  configurations for HD Voice features.

  The data in this parameter is a table that is indexed by columns.
  Each set of columns is a system configuration for which an HD voice
  feature can be enabled and configured.

  The parameter data is specified by six values:
  - version -- Specifies the data structure used in the entries. As the
    version increases, values are appended to the original data structure.
  - size -- Indicates the data size in bytes required for specifying the
    number of columns, columns, number of entries, and entries.
  - num_columns -- Indicates the number of columns in the parameter.
  - columns -- Data structure of type %vss_param_cal_column_t, which defines
    the columns to be used for finding entries.
       - The order and IDs of the columns specified in this structure must be
         the same for each individual entry.
       - The list of applicable column IDs and types must be the same as
         the static calibration table defined in
         #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.
  - num_entries -- Indicates the number of entries in the parameter.
  - entries -- Data structure of type dynamic_syscfg_entry_t, which
    specifies the column values for a data entry. The data entry is of
    type %vss_param_hdvoice_config_data_t.
       - The column values for each entry must match the definition (ID and
         type) and order defined in the columns section.
       - %vss_param_cal_na_value_t indicates a varying size for the column
         value described by the column type.
       - The possible data types for a column value are uint8, uint16, uint32,
         and uint64_t.

  When calibration is applied, the calibration table is checked for a matching
  data entry for the current system configuration.
  - If no entry is found, all features are disabled.
  - If an entry is found, the feature specified in the entry is configured to
    its enablement mode and Rx sampling rate.
  @newpage

  @par Data structures for this parameter
@verbatim
                                <-------- 32 bits -------->
 -------------------------
| version                 |
 -------------------------
| size                    | ----> size of the data below
 -------------------------
| num_columns             |
 ------------------------- 
| columns_1               | ----> -------------------------
|                         |      | column_id               | --> e.g., network_id
 -------------------------        -------------------------
| columns_2               |      | column_type             | --> e.g., uint32
|                         |       -------------------------
 -------------------------       | na_value                |
|  .                      |       -------------------------
|  .                      |
 -------------------------
| columns_n               |
-------------------------
| num_entries             |
 -------------------------  --> entry start
| column_1_value          | --> e.g., CDMA (network_id)
 -------------------------
| column_2_value          | --> e.g., FR (media_id)
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n_value          |
 -------------------------
| config_data             |
|                         |
|                         |
| ...                     |
 -------------------------  --> entry start
| column_1_value          | 
 ------------------------- 
| column_2_value          |
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n_value          |
 -------------------------- ----> -------------------------
| config_data             |      | feature_id              | --> e.g., Wide Voice
|                         |       -------------------------
|                         |      | enable_mode             | --> e.g., Default ON
| ...                     |       -------------------------
 -------------------------       | rx_sampling_rate        | --> e.g., 16000 Hz
| column_1_value          |       -------------------------
 -------------------------  
.                         .
.                         .
@endverbatim
  @newpage

  @par Expected data format for the entire table
@verbatim
Type                                                       Parameter
----                                                       ---------
uint32                                                     version
uint32                                                     size
uint32                                                     num_columns
vss_param_cal_column_t[num_columns]                        columns
uint32                                                     num_entries
dynamic_syscfg_entry_t[num_sys_config_entries]             entries


@endverbatim

  @par Expected data format for the vss_param_cal_column_t type
  @table{weak__vss__param__cal__column__t}

  @par Expected data format for the dynamic_syscfg_entry_t type
@verbatim
Type                                                       Parameter
----                                                       ---------
vss_param_cal_na_value_t[num_columns]                      column_value
vss_param_hdvoice_config_data_t                            config_data


@endverbatim

  @par Expected data format for the %vss_param_cal_na_value_t type
  @table{weak__vss__param__cal__na__value__t}

  @par Expected data format for the %vss_param_hdvoice_config_data_t type
  @table{weak__vss__param__hdvoice__config__data__t}

  @par Enable mode configuration
  For enable_mode, the data is a 32-bit value with the following supported
  modes:
  - 0 -- Default Off (Accepts BBWE UI commands)
  - 1 -- Default On  (Accepts BBWE UI commands)
  - 2 -- Forced Off  (Ignores BBWE UI commands)
  - 3 -- Forced On   (Ignores BBWE UI commands)
  @par
  Default mode allows feature enablement to be overwritten by BBWE UI commands.
  Forced mode is not affected by BBWE UI commands.
  @par
  Use the following table to determine if a feature is enabled based on
  enable_mode or the UI command:
@verbatim
                        BBWE UI command
             ------------------------------------------
Enable mode  |  Enable     |  Disable    | No Command |
-------------|-------------|-------------|------------|
Default Off  |  Enabled    |  Disabled   |  Disabled  |
Default On   |  Enabled    |  Disabled   |  Enabled   |
Forced Off   |  Disabled   |  Disabled   |  Disabled  |
Forced On    |  Enabled    |  Enabled    |  Enabled   |
-------------------------------------------------------


@endverbatim

  @par
  If an enable_mode value is not supported, the table data registration
  associated with this parameter is rejected.

  @inputfile{group__weakb__cvd__common__mod__feature__enable__modes.tex}
*/
#define VSS_PARAM_HDVOICE_CONFIG ( 0x000130A6 )

/** @ingroup cvd_common_module_cust_feature_cfg
  ID of the parameter used by VSS_MODULE_CUSTOM_FEATURE_CONFIG to apply
  configurations for custom bandwidth extension (BWE) features.

  The data in this parameter is a table that is indexed by columns.
  Each set of columns is a system configuration for which a custom BWE
  feature can be enabled and configured.

  The parameter data is specified by six values:
  - version -- Specifies the data structure used in the entries. As the
    version increases, values are appended to the original data structure.
  - size -- Indicates the data size in bytes required for specifying the
    number of columns, columns, number of entries, and entries.
  - num_columns -- Indicates the number of columns in the parameter.
  - columns -- Data structure of type %vss_param_cal_column_t, which defines
    the columns to be used for finding entries.
     - The order and IDs of the columns specified in this structure must be
       the same for each individual entry. @vertspace{-3}
     - The list of applicable column IDs and types must be limited to those
       used for calibration tables defined in
       #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.
       #VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA.
  - num_entries -- Indicates the number of entries in the parameter.
  - entries -- Data structure which specifies the column values for a data
    entry. The data entry is of type %vss_param_custom_bwe_config_data_t.
       - The column values for each entry must match the definition (ID and
         type) and order defined in the columns section. @vertspace{-3}
       - %vss_param_cal_na_value_t indicates a varying size for the column
         value described by the column type. @vertspace{-3}
       - The possible data types for a column value are uint8, uint16, uint32,
         and uint64_t.

  When calibration is applied, the calibration table is checked for a matching
  data entry for the current system configuration.
  - If no entry is found, all features are disabled. @vertspace{-2}
  - If an entry is found, the feature specified in the entry is configured to
    its enablement mode and Tx and Rx sampling rates.

  @par Data structures for this parameter
@verbatim
                                <-------- 32 bits -------->
 -------------------------
| version                 |
 -------------------------
| size                    | ----> size of the data below
 -------------------------
| num_columns             |
 ------------------------- 
| columns_1               | ----> --------------
|                         |      | column_id    | --> e.g., network_id
 -------------------------        --------------
| columns_2               |      | column_type  | --> e.g., uint32
|                         |       --------------
 -------------------------       | na_value     |
|  .                      |       --------------
|  .                      |
 -------------------------
| columns_n               |
-------------------------
| num_entries             |
 -------------------------  --> entry start
| column_1_value          | --> e.g., GSM (network_id)
 -------------------------
| column_2_value          | --> e.g., AMR (media_id)
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n_value          |
 -------------------------
| config_data             |
|                         |
|                         |
| ...                     |
 -------------------------  --> entry start
| column_1_value          | 
 ------------------------- 
| column_2_value          |
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n_value          |
 -------------------------- - > -------------
| config_data             |   | feature_id       |-> e.g., Custom BWE
|                         |    -------------------------
|                         |   | enable_mode      | --> e.g., Default ON
| ...                     |   -------------------------
 -------------------------    | rx_sampling_rate | --> e.g., 16000 Hz
| column_1_value          |       -------------------------
 -------------------------    | tx_sampling_rate | --> e.g., 16000 Hz

.                         .
.                         .
@endverbatim
  @newpage

  @par Expected data format for the entire table
@verbatim
Type                                                       Parameter
----                                                       ---------
uint32                                                     version
uint32                                                     size
uint32                                                     num_columns
vss_param_cal_column_t[num_columns]                        columns
uint32                                                     num_entries
dynamic_syscfg_entry_t[num_sys_config_entries]             entries


@endverbatim

  @par Expected data format for the vss_param_cal_column_t type
  @table{weak__vss__param__cal__column__t}

  @par Expected data format for the dynamic_syscfg_entry_t type
@verbatim
Type                                                       Parameter
----                                                       ---------
vss_param_cal_na_value_t[num_columns]                      column_value
vss_param_custom_bwe_config_data_t                         config_data


@endverbatim

  @par Expected data format for the %vss_param_cal_na_value_t type
  @table{weak__vss__param__cal__na__value__t}

  @par Expected data format for the %vss_param_custom_bwe_config_data_t type
  @table{weak__vss__param__custom__bwe__config__data__t}

  @par Enable mode configuration
  For enable_mode, the data is a 32-bit value with the following supported
  modes:
  - 0 -- Default Off (Accepts Custom BWE UI commands)
  - 1 -- Default On  (Accepts Custom BWE UI commands)
  - 2 -- Forced Off  (Ignores Custom BWE UI commands)
  - 3 -- Forced On   (Ignores Custom BWE UI commands)
  @par
  Default mode allows feature enablement to be overwritten by HD Voice UI
  commands. Forced mode is not affected by HD Voice UI commands.
  @par
  Use the following table to determine if a feature is enabled based on
  enable_mode or the UI command:
@verbatim
                        HD Voice UI command
             ------------------------------------------
Enable mode  |  Enable     |  Disable    | No Command |
-------------|-------------|-------------|------------|
Default Off  |  Enabled    |  Disabled   |  Disabled  |
Default On   |  Enabled    |  Disabled   |  Enabled   |
Forced Off   |  Disabled   |  Disabled   |  Disabled  |
Forced On    |  Enabled    |  Enabled    |  Enabled   |
-------------------------------------------------------


@endverbatim

  @par
  If an enable_mode value is not supported, the table data registration
  associated with this parameter is rejected.
*/
#define VSS_PARAM_CUSTOM_BWE_CONFIG ( 0x0001325A )

/* Union for specifying a system configuration column N/A value.*/
typedef union vss_param_cal_na_value_t vss_param_cal_na_value_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_cal_na_value_t
@{ */
/* Union for specifying a system configuration column N/A value.
*/
union vss_param_cal_na_value_t
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
/** @} */ /* end_weakgroup weak_vss_param_cal_na_value_t */

/* Structure for specifying a single column for system configuration */
typedef struct vss_param_cal_column_t vss_param_cal_column_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_cal_column_t
@{ */
struct vss_param_cal_column_t
{
  uint32_t id;
  /**< Column ID.

       @values See @xhyperref{hdr:CalibrationColumns,Calibration columns} */

  uint32_t type;
  /**< Column type.

       @values
       See @xhyperref{hdr:CalibrationColumnTypes,Calibration column types} */

  vss_param_cal_na_value_t na_value;
  /**< N/A value associated with this column ID and type.
       The N/A value is reserved and indicates that the entry is independent
       of this column.

       The size of this na_value field is dependent on the column type. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_cal_column_t */

/* Structure for specifying a configuration data in #VSS_PARAM_HDVOICE_CONFIG. */
typedef struct vss_param_hdvoice_config_data_t vss_param_hdvoice_config_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_hdvoice_config_data_t
@{ */
struct vss_param_hdvoice_config_data_t
{
  uint32_t feature_id;
  /**< Unique ID of the feature.

       @values See @xhyperref{hdr:CalColFeature,Calibration features} */

  uint32_t enable_mode;
  /**< Enable mode for a feature. For details, see <b>Enable mode
       configuration</b>.

       @values
       - #VSS_PARAM_FEATURE_DEFAULT_OFF
       - #VSS_PARAM_FEATURE_DEFAULT_ON
       - #VSS_PARAM_FEATURE_FORCED_OFF
       - #VSS_PARAM_FEATURE_FORCED_ON @tablebulletend */

  uint32_t rx_sampling_rate;
  /**< Sampling rate on the Rx path.

       @values 16 kHz */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_weak_vss_param_hdvoice_config_data_t */

/* Structure for specifying a configuration data in VSS_PARAM_CUSTOM_BWE_CONFIG. */
typedef struct vss_param_custom_bwe_config_data_t vss_param_custom_bwe_config_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_custom_bwe_config_data_t
@{ */
struct vss_param_custom_bwe_config_data_t
{
  uint32_t feature_id;
  /**< Unique ID of the custom feature.

       @values
       - #VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE
       - #VSS_ICOMMON_CAL_FEATURE_NONE @tablebulletend */

  uint32_t enable_mode;
  /**< Enable mode for the custom feature.

       @values
       - #VSS_PARAM_FEATURE_DEFAULT_OFF
       - #VSS_PARAM_FEATURE_DEFAULT_ON
       - #VSS_PARAM_FEATURE_FORCED_OFF
       - #VSS_PARAM_FEATURE_FORCED_ON @tablebulletend */

  uint32_t rx_sampling_rate;
  /**< Sampling rate on the Rx path.

       @values 8 kHz, 16 kHz, 32 kHz, and 48 kHz */

  uint32_t tx_sampling_rate;
  /**< Sampling rate on the Tx path.

       @values 8 kHz, 16 kHz, 32 kHz, and 48 kHz */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_vss_param_custom_bwe_config_data_t */

/** @ingroup cvd_common_module_clock_ctrl
  ID of the parameter used by VSS_MODULE_CLOCK_CONTROL to control voice clock
  voting for MPSS and bus bandwidth in custom topology voice use cases.

  This parameter applies MPPS and bus bandwidth scale factors on the voice
  voting numbers based on the use case parameters.

  @par Expected data format
@verbatim
<-------- 32 bits -------->
 -------------------------
| version                 |
 -------------------------
| size                    | -------> Size of the data below
 -------------------------
| num_entries             | -------> Number of clock control parameters below
 ------------------------- 
| clk_ctrl_params_entry_1 | ------->   -------------------------
|                         |           | direction               |
 -------------------------             -------------------------
| clk_ctrl_params_entry_2 |           | sampling_rate           |
|                         |            -------------------------
 -------------------------            | network_id              |
|  .                      |            -------------------------
   .                                  | media_type_id           |
|  .                      |            -------------------------
   .                                  | mpps_scale_factor       |
|                         |            -------------------------
| bus_bw_scale_factor     |
 -------------------------


@endverbatim

  @par Expected data format for the entire table
@verbatim
TYPE                                                      PARAMETER
----                                                      ---------
uint32                                                    version
uint32                                                    size
uint32                                                    num_entries
vss_param_clock_control_params_t[num_entries]             clk_ctrl_params
@endverbatim
  @newpage

  @apr_msgpayload{vss_param_clock_control_params_t}
  @table{weak__vss__param__clock__control__params__t}

  @inputfile{group__weakb__cvd__common__mod__clock__sample__rates.tex}
*/
#define VSS_PARAM_CLOCK_CONTROL ( 0x0001319D )

/* Type definition for #vss_param_clock_control_params_t. */
typedef struct vss_param_clock_control_params_t vss_param_clock_control_params_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_clock_control_params_t
@{ */
/* Structure for specifying clock control use case parameters in 
    #VSS_PARAM_CLOCK_CONTROL.
*/
struct vss_param_clock_control_params_t
{
   uint32_t direction;
   /**< Specifies the path.

        @values
         - #VSS_IVOCPROC_DIRECTION_RX
         - #VSS_IVOCPROC_DIRECTION_TX
         - #VSS_IVOCPROC_DIRECTION_RX_TX @tablebulletend */

  uint32_t sampling_rate;
  /**< Voice sampling rate.

       @values
        - #VSS_PARAM_SAMPLING_RATE_ANY
        - #VSS_PARAM_SAMPLING_RATE_8K
        - #VSS_PARAM_SAMPLING_RATE_16K
        - #VSS_PARAM_SAMPLING_RATE_32K
        - #VSS_PARAM_SAMPLING_RATE_48K @tablebulletend */

  uint32_t network_id;
  /**< Type of network.

       @values
        - See @xhyperref{hdr:CalNetworkID,Network ID column values}
        - If a network ID is not supplied, set this field to
          #VSS_ICOMMON_CAL_NETWORK_ID_NONE @tablebulletend */

  uint32_t media_type_id;
  /**< Type of vocoder.

       @values
       - See Section @xref{dox:MediaDefinitions}
       - If a vocoder media format ID is not supplied, set this field to
         #VSS_MEDIA_ID_NONE @tablebulletend */

  uint32_t mpps_scale_factor;
  /**< Scale factor used to control the voice use case MPPS.

        @values
         - Range -- 1.0, 1.1 ... 5.0
         - Default value -- 2.2 
         - Represented in integer format as 10, 11 ... 50 @tablebulletend */

  uint32_t bus_bw_scale_factor;
  /**< Scale factor used to control the voice use case bus bandwidth.

        @values
         - Range -- 0.5, 0.6 ... 5.0
         - Default value -- 2.0
         - Represented in integer format as 5, 6 ... 50 @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_clock_control_params_t */

/** @ingroup cvd_common_module_generic
  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the Tx AFE
  port media format.

  Set this parameter using in-band or out-of-band memory in
  #VSS_ICOMMON_CMD_SET_PARAM_V2.

  @msgpayload{vss_param_port_endpoint_media_info_t}
  @table{weak__vss__param__port__endpoint__media__info__t} @newpage
*/
#define VSS_PARAM_TX_PORT_ENDPOINT_MEDIA_INFO ( 0x00013253 )

/** @ingroup cvd_common_module_generic
  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the Rx AFE
  port media format.

  Set this parameter using in-band or out-of-band memory in
  #VSS_ICOMMON_CMD_SET_PARAM_V2.

  @msgpayload{vss_param_port_endpoint_media_info_t}
  @table{weak__vss__param__port__endpoint__media__info__t} @newpage
*/
#define VSS_PARAM_RX_PORT_ENDPOINT_MEDIA_INFO ( 0x00013254 )

/** @ingroup cvd_common_module_generic
  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the EC reference
  AFE port media format for the echo canceller reference signal.

  Set this parameter using in-band or out-of-band memory in
  #VSS_ICOMMON_CMD_SET_PARAM_V2.

  @msgpayload{vss_param_port_endpoint_media_info_t}
  @table{weak__vss__param__port__endpoint__media__info__t} @newpage
*/
#define VSS_PARAM_EC_REF_PORT_ENDPOINT_MEDIA_INFO ( 0x00013255 )

/* Type definition for #vss_param_port_endpoint_media_info_t. */
typedef struct vss_param_port_endpoint_media_info_t vss_param_port_endpoint_media_info_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_port_endpoint_media_info_t
@{ */
/* Structure for specifying payload in #vss_param_port_endpoint_media_info_t.
*/
struct vss_param_port_endpoint_media_info_t
{
   uint32_t port_id;
   /**< ID of the AFE port to which this media format corresponds.

        @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE @tablebulletend */

   uint16_t num_channels;
   /**< Number of port channels.

        @values 1 to 8 */

   uint16_t bits_per_sample;
   /**< Number of bits per sample that are rendered out of the AFE port
        (Rx path) or into the AFE port (Tx path).

        @values 16, 24, 32 */

   uint32_t sampling_rate;
   /**< Sampling rate of the AFE port.

        @values 8, 11.025, 16, 22.05, 24, 32, 44.1, 48, 88.2, 96, 176.4,
                192 kHz */

   uint8_t  channel_mapping[8];
   /**< Channel mapping array for the AFE port.

        Channel[i] mapping describes the position of channel i, where
        0 @le i < num_channels. Each entry in the array can have the following
        values.

        @values Valid channel map for AFE; e.g., 1,0,0,0,0,0,0,0 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_port_endpoint_media_info_t */

/** @ingroup cvd_common_module_generic
  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the Tx
  channel information.

  Set this parameter using in-band or out-of-band memory in
  #VSS_ICOMMON_CMD_SET_PARAM_V3.

  @msgpayload{vss_param_vocproc_dev_channel_info_t}
  @table{weak__vss__param__vocproc__dev__channel__info__t} @newpage
*/
#define VSS_PARAM_VOCPROC_TX_CHANNEL_INFO ( 0x0001328E )

/** @ingroup cvd_common_module_generic
  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the 
  Rx channel information.

  Set this parameter using in-band or out-of-band memory in
  #VSS_ICOMMON_CMD_SET_PARAM_V3.

  @msgpayload{vss_param_vocproc_dev_channel_info_t}
  @table{weak__vss__param__vocproc__dev__channel__info__t} @newpage
*/
#define VSS_PARAM_VOCPROC_RX_CHANNEL_INFO ( 0x0001328F )

/** @ingroup cvd_common_module_generic
  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the EC
  reference channel information.

  Set this parameter using in-band or out-of-band memory in
  #VSS_ICOMMON_CMD_SET_PARAM_V3.

  @msgpayload{vss_param_vocproc_dev_channel_info_t}
  @table{weak__vss__param__vocproc__dev__channel__info__t}
*/
#define VSS_PARAM_VOCPROC_EC_REF_CHANNEL_INFO ( 0x00013290 )

/* Type definition for #vss_param_vocproc_dev_channel_info_t . */
typedef struct vss_param_vocproc_dev_channel_info_t  vss_param_vocproc_dev_channel_info_t ;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_vocproc_dev_channel_info_t 
@{ */
/* Structure for specifying payload in #vss_param_vocproc_dev_channel_info_t .
*/
struct vss_param_vocproc_dev_channel_info_t
{
   uint32_t num_channels;
   /**< Number of channels.

        @values
         - #VSS_NUM_DEV_CHANNELS_1
         - #VSS_NUM_DEV_CHANNELS_2
         - #VSS_NUM_DEV_CHANNELS_3
         - #VSS_NUM_DEV_CHANNELS_4
         - #VSS_NUM_DEV_CHANNELS_5
         - #VSS_NUM_DEV_CHANNELS_6
         - #VSS_NUM_DEV_CHANNELS_7
         - #VSS_NUM_DEV_CHANNELS_8 @tablebulletend */

   uint32_t bits_per_sample;
   /**< Number of bits per sample per channel.

        @values 16 */

   uint8_t  channel_mapping[ VSS_MAX_DEV_CHANNELS ];
   /**< Mapping for each channel.

        Channel[i] mapping describes the position of channel i,
        where 0 @le i < num_channels. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_vocproc_dev_channel_info_t */

#endif /* __VSS_PARAMS_PUBLIC_IF_H__ */

