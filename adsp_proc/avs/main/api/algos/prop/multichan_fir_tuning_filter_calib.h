#ifndef MULTICHAN_FIR_TUNING_FILTER_CALIB_H
#define MULTICHAN_FIR_TUNING_FILTER_CALIB_H
/*==============================================================================
  @file multichan_fir_tuning_filter_calib.h
  @brief This file contains FIR_TUNING_FILTER API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {FIR Tuning Filter}
    @h2xml_title_agile_rev  {FIR Tuning Filter}
    @h2xml_title_date       {December 14, 2016} */

#include "mmdefs.h"

/** ID of the FIR Tuning Filter module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_FIR_ENABLE
    - #AUDPROC_PARAM_ID_FIR_FILTER_MAX_TAP_LENGTH
    - #AUDPROC_PARAM_ID_FIR_FILTER_CONFIG
 */
#define AUDPROC_MODULE_ID_FIR_TUNING_FILTER                    0x00010921

/** ID of the Fir filter Enable parameter used by

    AUDPROC_MODULE_ID_FIR_TUNING_FILTER. */
#define AUDPROC_PARAM_ID_FIR_ENABLE                          0x00010922

/** ID of the FIR filter Maximum Tap Length parameter used by
    AUDPROC_MODULE_ID_FIR_TUNING_FILTER. */
#define AUDPROC_PARAM_ID_FIR_FILTER_MAX_TAP_LENGTH           0x00010923

/** ID of the FIR filter coefficient parameter used by
    AUDPROC_MODULE_ID_FIR_TUNING_FILTER. */
#define AUDPROC_PARAM_ID_FIR_FILTER_CONFIG                   0x00010924

/* Structure for the enable parameter of Fir filter module. */
typedef struct audproc_fir_enable_t audproc_fir_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_FIR_ENABLE", AUDPROC_PARAM_ID_FIR_ENABLE}
    @h2xmlp_description {Used to configure a device}
    @h2xmlp_version     {0}    */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_FIR_ENABLE parameter used by the
 Fir filter module.
 */
struct audproc_fir_enable_t
{
   uint32_t version;

   /**< @h2xmle_description  {Specifies the current version of this interface.}
        @h2xmle_default      {0}
        @h2xmle_isVersion    {true}
        @h2xmle_readOnly     {true} */


   uint32_t fir_feature_mode_t;
   /**< @h2xmle_description  {Specifies whether FIR module is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}
        @h2xmle_default      {0}  */
}
#include "adsp_end_pack.h"
;


/** @h2xmlp_subStruct */

typedef struct audproc_fir_max_tap_length_cfg_t
{
   uint32_t channel_mask_lsb;
   /**< @h2xmle_description  {Lower 32 bits of the mask that indicates the corresponding channel whose maximum tap length is to be set.\n - Set the bits corresponding to 1 to 31 channels of standard channel mapping (channels are mapped per standard channel mapping)\n - Position of the bit to set 1 (left shift)(channel_map) \n}

        @h2xmle_readOnly     {true}*/



   uint32_t channel_mask_msb;
   /**< @h2xmle_description  {Upper 32 bits of the mask that indicates the corresponding channel
                              whose maximum tap length is to be set.
                              - Set the bits corresponding to 32 to 63 channels of standard channel
                                mapping (channels are mapped per standard channel mapping)
                              - Position of the bit to set 1 (left shift)(channel_map - 32)}
        @h2xmle_readOnly     {true}                      */


   uint32_t fir_max_tap_length;
   /**< @h2xmle_description  { Specifies the maximum tap length of the FIR filter. This value is
                               limited by resources (memory, MIPS, etc.) }
        @h2xmle_range        {0..4294967295}
        @h2xmle_default      {512}
        @h2xmle_readOnly     {true}  */


}audproc_fir_max_tap_length_cfg_t
;



typedef struct audproc_fir_max_tap_cfg_t audproc_fir_max_tap_cfg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_FIR_FILTER_MAX_TAP_LENGTH", AUDPROC_PARAM_ID_FIR_FILTER_MAX_TAP_LENGTH }
    @h2xmlp_description { Structure for the max tap length parameter of Fir filter module.}
     @h2xmlp_version     {0}
     @h2xmlx_expandStructs	{false} */

#include "adsp_begin_pack.h"

/** Immediately following this structure are num_config structures of type
    audproc_fir_max_tap_length_cfg_t.
 */
struct audproc_fir_max_tap_cfg_t
{
   uint32_t version;
   /**< @h2xmle_description  {Specifies the current version of this interface.}
        @h2xmle_default      {0}
        @h2xmle_isVersion    {true}
        @h2xmle_readOnly {true} */

   uint32_t num_config;
   /**< @h2xmle_description  {Specifies the different sets of filter coefficient configurations.}
        @h2xmle_range        {1..63}
        @h2xmle_default      {1}
        @h2xmle_readOnly     {true}  */

   audproc_fir_max_tap_length_cfg_t configData[0];
   /**< @h2xmle_description {Specifies the different sets of filter coefficient configurations}
        @h2xmle_variableArraySize {num_config}
        @h2xmle_default      {0}   */

}
#include "adsp_end_pack.h"
;


/** @h2xmlp_subStruct */

typedef struct audproc_fir_filter_cfg_t
{
   uint32_t channel_mask_lsb;
   /**< @h2xmle_description  { Lower 32 bits of the mask that indicates the corresponding channel
                               whose filter coefficients are to be set.
                               - Set the bits corresponding to 1 to 31 channels of standard channel
                                 mapping (channels are mapped per standard channel mapping)
                               - Position of the bit to set 1 (left shift) (channel_map)                   }
        @h2xmle_readOnly     {true}                       */

   uint32_t channel_mask_msb;
   /**< @h2xmle_description  { Upper 32 bits of the mask that indicates the corresponding channel
                               whose filter coefficients are to be set.
                               - Set the bits corresponding to 32 to 63 channels of standard channel
                                 mapping (channels are mapped per standard channel mapping)
                               - Position of the bit to set  1 (left shift) (channel_map - 32)              }
        @h2xmle_readOnly     {true}                       */


    uint32_t coef_width;
    /**< @h2xmle_description  {Indicates the bit width of the filter coefficients.}
         @h2xmle_rangeList    {"16"=16;
                               "32"=32}
         @h2xmle_default      {16}
         @h2xmle_readOnly     {true}   */

    uint16_t coef_Q_Factor;
    /**< @h2xmle_description  {Indicates the Q factor of the coefficients.}
         @h2xmle_range        {1..31}
         @h2xmle_default      {1}
         @h2xmle_readOnly     {true}         */


    uint16_t num_taps;
    /**< @h2xmle_description  {Indicates the filter tap length. (num_taps should be lesser than fir_max_tap_length)}
         @h2xmle_range        {1..512}
         @h2xmle_default      {256}
         @h2xmle_readOnly     {true}         */


    uint32_t filter_delay_in_samples;
    /**< @h2xmle_description  {Indicates the delay of the filter in samples.
                               The client uses this field to configure the delay corresponding to
                               the current filter design. The configured delay is considered as the
                               final value unless it is the special value. If it is the special
                               value, the aDSP sets this field to the best approximate value.}
        @h2xmle_default      {0xFFFFFFFF}
        @h2xmle_readOnly     {true}*/


    int32_t filter_coefficients[0];
    /**< @h2xmle_description  {Array of filter coefficients. The array size depends on the number of
            						 taps. If coef_width = 16, store the filter coefficients in the
                               lower 16 bits of the 32-bit field, and sign extend it to
                               32 bits.}
         @h2xmle_variableArraySize {"num_taps"}
         @h2xmlx_expandStructs {false}
         @h2xmle_default      {0}
         @h2xmle_readOnly     {true}  */

} audproc_fir_filter_cfg_t;

/* Structure for the filter coefficients parameter of Fir filter module. */
typedef struct audproc_fir_filter_coeff_cfg_t audproc_fir_filter_coeff_cfg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_FIR_FILTER_CONFIG", AUDPROC_PARAM_ID_FIR_FILTER_CONFIG }
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_FIR_FILTER_CONFIG parameter used by the Fir filter module}
    @h2xmlp_version     {0}
    @h2xmlx_expandStructs	{false}  */

#include "adsp_begin_pack.h"


/** Immediately following this structure are num_config structures of type
    audproc_fir_filter_cfg_t.
 */
struct audproc_fir_filter_coeff_cfg_t
{
   uint32_t version;
   /**< @h2xmle_description  {Specifies the current version of this interface.}
        @h2xmle_readOnly     {true}
        @h2xmle_default      {0}
        @h2xmle_isVersion    {true}   */

   uint32_t num_config;
   /**< @h2xmle_description  {Specifies the different sets of filter coefficient configurations.}
        @h2xmle_range        {1..63}
        @h2xmle_default      {1}
        @h2xmle_readOnly     {true}        */

   audproc_fir_filter_cfg_t configData[0];
   /**< @h2xmle_description {Specifies the different sets of filter coefficient configurations.}
        @h2xmle_variableArraySize {num_config}
        @h2xmle_default      {0}
        @h2xmle_readOnly     {true} */

}
#include "adsp_end_pack.h"
;


/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_FIR_TUNING_FILTER",
                          AUDPROC_MODULE_ID_FIR_TUNING_FILTER}
	@h2xmlm_displayName  {"AUDIO MULTICHAN FIR"}
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @h2xmlm_description  {FIR Tuning Filter Module}
    @{                   <-- Start of the Module -->
*/

/**
	@h2xml_Select					{audproc_fir_enable_t}
   @h2xmlm_InsertParameter
*/
/**
	@h2xml_Select					{audproc_fir_max_tap_cfg_t}
   @h2xmlm_InsertParameter
*/

/**
	@h2xml_Select					{audproc_fir_max_tap_length_cfg_t}
   @h2xmlm_InsertParameter
*/

/**
	@h2xml_Select					{audproc_fir_filter_coeff_cfg_t}
   @h2xmlm_InsertParameter
*/

/**
	@h2xml_Select					{audproc_fir_filter_cfg_t}
   @h2xmlm_InsertParameter
*/



/** @}                   <-- End of the Module -->*/

#endif
