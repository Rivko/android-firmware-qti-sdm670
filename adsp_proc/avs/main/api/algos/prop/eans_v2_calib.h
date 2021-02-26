#ifndef EANS_V2_CALIB_H
#define EANS_V2_CALIB_H
/*==============================================================================
  @file eans_v2_calib.h
  @brief This file contains EANS_V2 API
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
/** @h2xml_title1           {Enhanced Audio Noise Suppression (EANS) version 2}
    @h2xml_title_agile_rev  {Enhanced Audio Noise Suppression (EANS) version 2}
    @h2xml_title_date       {February 24, 2017} */


#include "mmdefs.h"
#include "eans_param_calib.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

#define AUDPROC_MODULE_ID_EANS_V2                           0x0001091F
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_EANS_V2",
                          AUDPROC_MODULE_ID_EANS_V2}
    @h2xmlm_displayName  {"AUDIO EANS V2"}
    @h2xmlm_description  {ID of the Enhanced Audio Noise Suppression (EANS) version 2 module.\n

    This module supports the following parameter IDs:\n
     - #AUDPROC_PARAM_ID_EANS_ENABLE\n
     - #AUDPROC_PARAM_ID_EANS_PARAMS_V2\n
     - #AUDPROC_PARAM_ID_ENABLE\n

    All parameter IDs are device independent.\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/
/**
	@h2xml_Select					{audproc_eans_enable_t}
   @h2xmlm_InsertParameter
*/

/* ID of the EANS configuration parameters used by AUDPROC_MODULE_ID_EANS_V2. */
#define AUDPROC_PARAM_ID_EANS_PARAMS_V2                      0x00010920

/*Structure for EANSV2 configuration parameters. */
typedef struct audproc_eans_v2_params_t audproc_eans_v2_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EANS_PARAMS_V2", AUDPROC_PARAM_ID_EANS_PARAMS_V2}
    @h2xmlp_description {Configures the module}
    @h2xmlp_version     {0}    */

#include "adsp_begin_pack.h"

struct audproc_eans_v2_params_t
{
   uint32_t version;
    /**< @h2xmle_description   { Specifies the version information of this parameter.\n }
         @h2xmle_readonly	{true}
          @h2xmle_isVersion    {true}*/

   uint32_t eans_v2_feature_mode;
   /**< @h2xmle_description   { Controls module functionality by enabling/disabling submodules.\n
        This mode can be enabled as a combination of one or more submodules.\n }

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"FNS_FEATURE_WNR_ENABLE"}
       @h2xmle_description {Enable WND and WNR feature.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"FNS_FEATURE_FREQUENCY_PROCESS_ENABLE"}
       @h2xmle_description {Enable Frequency Process.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {"FNS_FEATURE_VAD_ENABLE"}
       @h2xmle_description {Enable VAD.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000008}
       @h2xmle_bitName     {"FNS_FEATURE_VADNREF_ENABLE"}
       @h2xmle_description {Enable VAD based non-stationary noise.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000010}
       @h2xmle_bitName     {"FNS_FEATURE_SMRMT_ENABLE"}
       @h2xmle_description {Enable SMRMT.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000020}
       @h2xmle_bitName     {"FNS_FEATURE_POSTPROC_ENABLE"}
       @h2xmle_description {Enable Posprocessing.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000040}
       @h2xmle_bitName     {"FNS_FEATURE_HISSING_MODE_ENABLE"}
       @h2xmle_description {Enable Hiss mode.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x000000080}
       @h2xmle_bitName     {"FNS_FEATURE_LINKED_MODE_ENABLE"}
       @h2xmle_description {Enable Linked mode.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd  */


   uint16_t eans_v2_GT_delayMode;
    /**< @h2xmle_description  { Determines the FFT size based on the delay.\n}

         @h2xmle_rangeList    {"Low delay"=0;
                              "High delay"=1}
         @h2xmle_default      {1}  */


   uint16_t eans_v2_GT_resolutionMode;
   /**< @h2xmle_description  {Number of compressed bins used depends on the resolution mode selected.\n}

        @h2xmle_rangeList    {"Low"=0;
                              "High"=1;
			      "Super High" = 2}
        @h2xmle_default      {1} */


   int16_t eans_v2_GT_InputGain;
   /**< @h2xmle_description  {Input gain to the EANS module.\n}
        @h2xmle_default      {8192}
        @h2xmle_range        {0..32767}
        @h2xmle_dataFormat  {Q2.13} 	*/


   int16_t eans_v2_GT_OutputGain;
   /**< @h2xmle_description  {Output gain to the EANS module.\n}
        @h2xmle_default      {8192}
        @h2xmle_range        {0..32767}
        @h2xmle_dataFormat  {Q2.13} 	*/

   int16_t eans_v2_vad_Mode;
   /**< @h2xmle_description  {Enables voice activity detection (VAD).\n
      Currently, only enable is supported\n}
        @h2xmle_rangeList {"Enable" = 1}
        @h2xmle_default   {1}	*/


    int16_t eans_v2_vadnref_Ni;
    /**< @h2xmle_description  {Block length for initial non-stationary noise estimation.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default   {100}	*/


   int16_t eans_v2_vadnref_NPscale;
    /**< @h2xmle_description  {Speech level scale factor for non-stationary noise computation.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {2560}
        @h2xmle_dataFormat  {Q7.8} 	*/


   int16_t eans_v2_vadnref_NLambda;
    /**< @h2xmle_description  {Smoothing factor for non-stationary noise averaging used during speech activity.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {32440}
         @h2xmle_dataFormat  {Q15} 	*/


   int16_t eans_v2_vadnref_NLambdaf;
   /**< @h2xmle_description  {Higher smoothing factor for non-stationary noise averaging used during
        speech inactivity.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {32736}
        @h2xmle_dataFormat  {Q15} 	*/


   int16_t eans_v2_vadnref_NLambda0;
   /**< @h2xmle_description  {Highest smoothing factor for non-stationary noise averaging used during
        speech inactivity.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {32767}
        @h2xmle_dataFormat  {Q15} 	*/

   int16_t eans_v2_avad_thresh;
   /**< @h2xmle_description  {Threshold for generating a binary VAD decision.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {16384}
        @h2xmle_dataFormat  {Q15} 	*/


   int16_t eans_v2_avad_pwrScale;
   /**< @h2xmle_description  {Multiplication factor used while estimating the lower boundary of noise.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {200}
        @h2xmle_dataFormat  {Q15} 	*/


   int16_t eans_v2_avad_hangoverMax;
   /**< @h2xmle_description  {Length of the hangover window (in frames). This parameter reliably
        detects weak speech bursts at the end of speech activity.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {5} 	*/


   int16_t eans_v2_avad_alphaSNR;
   /**< @h2xmle_description  {Averaging factor for smoothing the SNR estimate.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      { 3278}
        @h2xmle_dataFormat  {Q15} 	*/

   int16_t eans_v2_avad_snrDiffMax;
 /**< @h2xmle_description  { Maximum log SNR difference for decision making.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {3072}
        @h2xmle_dataFormat  {Q7.8} 	*/


   int16_t eans_v2_avad_snrDiffMin;
   /**< @h2xmle_description  { Minimum log SNR difference for decision making.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {2560}
        @h2xmle_dataFormat  {Q7.8} 	*/


   int16_t eans_v2_avad_initLength;
   /**< @h2xmle_description  {Defines the number of frames for which a noise level estimate is set to
        a fixed value.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {200} 	*/


   int16_t eans_v2_avad_maxVal;
   /**< @h2xmle_description  {Defines the upper limit of the noise level.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {32767}
        @h2xmle_dataFormat  {Q15} 	*/

   int16_t eans_v2_avad_initBound;
   /**< @h2xmle_description  {Initialization value for the minimum noise bounding value.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {200}
        @h2xmle_dataFormat  {Q15} 	*/


   int16_t eans_v2_avad_resetBound;
   /**< @h2xmle_description  {Reset value for the minimum noise bounding value.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {32767}
        @h2xmle_dataFormat  {Q15} 	*/

   int16_t eans_v2_avad_avarScale;
   /**< @h2xmle_description  {Multiplication factor for speech standard deviation.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {8192}
        @h2xmle_dataFormat  {Q2.13} 	*/

   int16_t eans_v2_avad_sub_Nc;
   /**< @h2xmle_description  {Defines the window length for noise estimation.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {25} 	*/

   int16_t eans_v2_avad_spowMin;
   /**< @h2xmle_description  {Defines the minimum signal power required to update the boundaries for
        the noise floor estimate.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {32767}
        @h2xmle_dataFormat  {Q15} 	*/


   int16_t eans_v2_smrmt_Mode;
   /**< @h2xmle_description  { Single Mic Recursive Minimum Tracking submodule operation mode.\n
        The client can OR these values and set the result as SMRMT mode.\n}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"SMRMT_OP_CLEAN_DECAY"}
       @h2xmle_description {SMRMT_OP_CLEAN_DECAY.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"SMRMT_OP_APPLY_INIT_NFLOOR"}
       @h2xmle_description {SMRMT_OP_APPLY_INIT_NFLOOR.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd */

   int16_t eans_v2_smrmt_wlenInit;
   /**< @h2xmle_description  { Initial window length (in frames) for noise estimation in SMRMT mode.\n}
        @h2xmle_range        {0..32767}
	    @h2xmle_default      {2000} */


   int16_t eans_v2_smrmt_wlenInc;
   /**< @h2xmle_description  {Step size for the window length increment.\n}
        @h2xmle_range        {0..32767}
	    @h2xmle_default      {500}  */


   int16_t eans_v2_smrmt_wlenMax;
   /**< @h2xmle_description  {Maximum window size.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {6000} */


   int16_t eans_v2_smrmt_band1I;
    /**< @h2xmle_description  {Bin index for band 1.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {64}  */


   int16_t eans_v2_smrmt_band2I;
    /**< @h2xmle_description  {Bin index for band 2.\n}
         @h2xmle_range        {0..32767}
	     @h2xmle_default      {256}  */


   int16_t eans_v2_smrmt_alphaP;
      /**< @h2xmle_description  {Bin index for band 2.\n}
           @h2xmle_range        {0..32767}
	       @h2xmle_default      {1638}  */

   int16_t eans_v2_smrmt_alphaD;
   /**< @h2xmle_description  {Smoothing factor for the final noise reference.\n}
        @h2xmle_range        {0..32767}
	    @h2xmle_default      {32700}  */

   int16_t eans_v2_smrmt_overest_factL16Q12;
     /**< @h2xmle_description  { Overestimate factor used while calculating noise references.\n}
          @h2xmle_range        {0..32767}
	      @h2xmle_default      {12500}  */

   int16_t eans_v2_smrmt_nlinitQL16;
   /**< @h2xmle_description  { Initial noise power floor Q factor.\n}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {31}  */


   int16_t eans_v2_PP_Mode;
   /**< @h2xmle_description  { Postprocessing submodule mode bit. This bit is required to enable
        postprocessing modules. The mode can be enabled as a combination of
        one or more submodules.\n The client can OR these values and set the result as Postprocessing
        mode.\n}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"PP_ENABLE_SPECSUB"}
       @h2xmle_description {PP_ENABLE_SPECSUB.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"PP_ENABLE_ENSPP"}
       @h2xmle_description {PP_ENABLE_ENSPP.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd */


   int16_t eans_v2_PP1_alphaL16Q13;
     /**<@h2xmle_description  {Over-subtraction factor used in a spectral subtraction block (PP1)
        while calculating gain.\n}
         @h2xmle_range        {0..32767}

       	 @h2xmle_default      {16384}*/

   int16_t eans_v2_PP1_alpha_smLB_L16Q15;
     /**<@h2xmle_description  {Smoothing factor for the low band in PP1.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {28500}*/

   int16_t eans_v2_PP1_alpha_smNLB_L16Q15;
     /**<@h2xmle_description  {Smoothing factor for the high band in PP1.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {25000}*/


   int16_t eans_v2_PP1_minGainL16Q14;
     /**<@h2xmle_description  { Minimum gain floor for PP1.\n}
         @h2xmle_range        {0..32767}
	     @h2xmle_default      {1024} */

   int16_t eans_v2_PP1_envRestFactL16Q12;
     /**<@h2xmle_description  { Envelope restoration factor employed in PP1.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {8192}*/


   int16_t eans_v2_PP2_TargetNS;
     /**<@h2xmle_description  { Target noise suppression level (in dB) for PP2.\n}
         @h2xmle_dataFormat  {Q7.8}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {5120}*/


   int16_t eans_v2_PP2_Nalpha;
     /**<@h2xmle_description  { Over-subtraction factor used in PP2 for gain calculation.\n}
         @h2xmle_dataFormat  {Q3.12}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {4096}*/


   int16_t eans_v2_PP2_NalphaMax;
     /**<@h2xmle_description  { Maximum value of over-subtraction factor used in PP2 for gain
        calculation.\n}
         @h2xmle_dataFormat  { Q3.12}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {5120}*/


   int16_t eans_v2_PP2_NSNRmax;
      /**<@h2xmle_description  { Upper bounding value (in dB) for SNR estimation in PP2.\n}
          @h2xmle_dataFormat  { Q7.8}
          @h2xmle_range        {0..32767}
          @h2xmle_default      {4096}*/


   int16_t eans_v2_PP2_GsBias;
       /**<@h2xmle_description  { Bias factor (in dB) for gain calculation.\n}
           @h2xmle_dataFormat  { Q7.8}
           @h2xmle_range        {0..32767}
           @h2xmle_default      {0}*/



   int16_t eans_v2_PP2_GsMax;
     /**<@h2xmle_description  {Lower bounding value for SNR (in dB) for aggressive gain calculation in
        PP.\n}
        @h2xmle_dataFormat  { Q7.8}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {2560}*/


   int16_t eans_v2_PP2_submode;
     /**<@h2xmle_description  {Mode control flag that enables submodule functionality in PP2.\n
      values 0 (PP_NAL), currently, only this value is supported.\n}
         @h2xmle_range        {0..32767}

         @h2xmle_default    {0}  */

   int16_t eans_v2_PP2_envRestFactL16Q12;
     /**<@h2xmle_description  { Envelope restoration factor employed in PP2.\n}
         @h2xmle_dataFormat      {Q12}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {8192} */


   int16_t eans_v2_PP2_TargetNoiseFloor;
     /**<@h2xmle_description  { Comfort noise level in PP2.\n}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {100}
	     @h2xmle_dataFormat      {Q0.15}  */


   int16_t eans_v2_PP2_SlopeNoiseFloor;
    /**< @h2xmle_description  {Comfort noise slope to determine the shape of the comfort noise.\n}
         @h2xmle_dataFormat  { Q0.15}
         @h2xmle_range        {0..32767}
         @h2xmle_default      {32740}*/


   int16_t reserved1;
    /**< @h2xmle_description  { Client must set this parameter to 0.\n}
          @h2xmle_visibility  {hide}*/



   int32_t eans_v2_smrmt_thrB1;
     /**<@h2xmle_description  {SNR threshold for band 1.\n}
          @h2xmle_range        {0..2147483647}

	     @h2xmle_default    {80000000}		      */


   int32_t eans_v2_smrmt_thrB2;
     /**<@h2xmle_description  {SNR threshold for band 2.\n}
          @h2xmle_range        {0..2147483647}

	     @h2xmle_default    {100000000}		      */


   int32_t eans_v2_smrmt_thrB3;
     /**<@h2xmle_description  {SNR threshold for band 3.\n}
         @h2xmle_range        {0..2147483647}

	     @h2xmle_default    {120000000}		      */


   int32_t eans_v2_smrmt_nlinitL32;
     /**<@h2xmle_description  {Initial noise floor power.\n}

         @h2xmle_default    {214}   */

}
#include "adsp_end_pack.h"
;

/** @}                   <-- End of the Module -->*/

#endif
