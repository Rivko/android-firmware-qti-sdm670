#ifndef EANS_CALIB_H
#define EANS_CALIB_H
/*==============================================================================
  @file eans_calib.h
  @brief This file contains EANS API
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

/** @h2xml_title1           {Enhanced Audio Noise Suppression (EANS)}
    @h2xml_title_agile_rev  {Enhanced Audio Noise Suppression (EANS)}
    @h2xml_title_date       {February 24, 2017} */


#include "mmdefs.h"
#include "eans_param_calib.h"

#define AUDPROC_MODULE_ID_EANS                            0x00010C4A
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_EANS",
                          AUDPROC_MODULE_ID_EANS}
    @h2xmlm_displayName  {"AUDIO EANS"}
    @h2xmlm_description  {ID of the Enhanced Audio Noise Suppression (EANS) version 1 module.\n

    This module supports the following parameter IDs:\n
     - #AUDPROC_PARAM_ID_EANS_ENABLE\n
     - #AUDPROC_PARAM_ID_EANS_PARAMS\n
     - #AUDPROC_PARAM_ID_ENABLE\n

    All parameter IDs are device independent.\n}

     @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/* ID of the EANS Configuration parameters used by AUDPROC_MODULE_ID_EANS. */
#define AUDPROC_PARAM_ID_EANS_PARAMS                      0x00010C4C

/**
	@h2xml_Select					{audproc_eans_enable_t}
   @h2xmlm_InsertParameter
*/

/* Structure for EANS configuration parameters. */
typedef struct audproc_eans_params_t audproc_eans_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EANS_PARAMS", AUDPROC_PARAM_ID_EANS_PARAMS}
    @h2xmlp_description {Configures the module} */

#include "adsp_begin_pack.h"

struct audproc_eans_params_t
{
   int16_t eans_mode;
   /**< @h2xmle_description   { Mode word for enabling/disabling submodules.\n  Byte offset: 0\n }
        @h2xmle_default {0x3072} */


   int16_t eans_input_gain;
    /**< @h2xmle_description   { Input gain to the EANS module.\n  Byte offset: 2\n }
         @h2xmle_dataFormat { Q2.13}
	 @h2xmle_default {8192} */



   int16_t eans_output_gain;
    /**< @h2xmle_description   {  Output gain to the EANS module.\n  Byte offset: 4\n }
         @h2xmle_dataFormat { Q2.13}
	 @h2xmle_default {8192} */


   int16_t eans_target_ns;
    /**< @h2xmle_description   { Target noise suppression level in dB.\n  Byte offset: 6\n }
         @h2xmle_dataFormat { Q7.8}
          @h2xmle_default {5120}*/


   int16_t eans_s_alpha;
    /**< @h2xmle_description   {Over-subtraction factor for nonstationary noise suppression.\n  Byte offset: 8\n }
         @h2xmle_dataFormat { Q3.12}
	 @h2xmle_default {5120} */


   int16_t eans_n_alpha;
   /**< @h2xmle_description   {Over-subtraction factor for stationary noise suppression.\n  Byte offset: 10\n }
         @h2xmle_dataFormat { Q3.12}
	 @h2xmle_default {4096} */

   int16_t eans_n_alphamax;
   /**< @h2xmle_description   {Maximum over-subtraction factor for nonstationary noise suppression.\n  Byte offset: 12\n }
         @h2xmle_dataFormat { Q3.12}
	 @h2xmle_default {5120} */

   int16_t eans_e_alpha;
   /**< @h2xmle_description   {Scaling factor for excess noise suppression.\n  Byte offset: 14\n }
         @h2xmle_dataFormat { Q15}
	 @h2xmle_default {256} */

   int16_t eans_ns_snrmax;
    /**< @h2xmle_description   { Upper boundary in dB for SNR estimation.\n  Byte offset: 16\n }
         @h2xmle_dataFormat { Q7.8}
         @h2xmle_default {5120} */

   int16_t eans_sns_block;
    /**< @h2xmle_description   {Quarter block size for stationary noise suppression.\n  Byte offset: 18\n }
         @h2xmle_dataFormat { Q0}
         @h2xmle_default {100} */


   int16_t eans_ns_i;
    /**< @h2xmle_description   {Initialization block size for noise suppression.\n  Byte offset: 20\n }
         @h2xmle_dataFormat { Q0}
         @h2xmle_default {100} */


   int16_t eans_np_scale;
    /**< @h2xmle_description   {Power scale factor for nonstationary noise update.\n  Byte offset: 22\n }
         @h2xmle_dataFormat { Q7.8}
         @h2xmle_default {2560} */



   int16_t eans_n_lambda;
    /**< @h2xmle_description   {Smoothing factor for higher level nonstationary noise update.\n  Byte offset: 24\n }
         @h2xmle_dataFormat { Q15}
         @h2xmle_default {32440} */


   int16_t eans_n_lambdaf;
    /**< @h2xmle_description   {Medium averaging factor for noise update.\n  Byte offset: 26\n }
         @h2xmle_dataFormat { Q15}
         @h2xmle_default {32736} */


   int16_t eans_gs_bias;
    /**< @h2xmle_description   {Bias factor in dB for gain calculation.\n  Byte offset: 28\n }
         @h2xmle_dataFormat { Q7.8}
         @h2xmle_default {0} */


   int16_t eans_gs_max;
    /**< @h2xmle_description   {SNR lower boundary in dB for aggressive gain calculation.\n  Byte offset: 30\n }
         @h2xmle_dataFormat { Q7.8}
         @h2xmle_default {2560} */


   int16_t eans_s_alpha_hb;
    /**< @h2xmle_description   {Over-subtraction factor for high-band stationary noise suppression.\n  Byte offset: 32\n }
        @h2xmle_dataFormat    {Q3.12}
	  @h2xmle_default {5120} */


   int16_t eans_n_alphamax_hb;
    /**< @h2xmle_description   {Maximum over-subtraction factor for high-band nonstationary noise
        suppression.\n  Byte offset: 34\n }
          @h2xmle_dataFormat    {Q3.12}
	   @h2xmle_default {4096} */

   int16_t eans_e_alpha_hb;
    /**< @h2xmle_description   {Scaling factor for high-band excess noise suppression.\n  Byte offset: 36\n }
         @h2xmle_dataFormat    {Q3.12}
	 @h2xmle_default       {256}*/


   int16_t eans_n_lambda0;
    /**< @h2xmle_description   {Smoothing factor for nonstationary noise update during speech activity.\n  Byte offset: 38\n }
        @h2xmle_dataFormat { Q15}
         @h2xmle_default {32767} */


   int16_t thresh;
    /**< @h2xmle_description   {Threshold for generating a binary voice activity detection (VAD)
        decision.\n  Byte offset: 40\n }

         @h2xmle_default {0} */


   int16_t pwr_scale;
    /**< @h2xmle_description   {Indirect lower boundary of the noise level estimate.\n  Byte offset: 42\n }

         @h2xmle_default {256} */


   int16_t hangover_max;
    /**< @h2xmle_description   { Avoids mid-speech clipping and reliably detects weak speech bursts at
        the end of speech activity.\n  Byte offset: 44\n }

         @h2xmle_default {50} */


   int16_t alpha_snr;
    /**< @h2xmle_description   {Controls responsiveness of VAD.\n  Byte offset: 46\n }

         @h2xmle_default {3278} */


   int16_t snr_diff_max;
    /**< @h2xmle_description   {Maximum SNR difference.\n

        Decreasing this parameter value can help in making correct decisions
        during abrupt changes. However, decreasing the value too much can
        increase false alarms during long pauses or silences.\n  Byte offset: 48\n }

         @h2xmle_default {3072} */



   int16_t snr_diff_min;
    /**< @h2xmle_description   {Minimum SNR difference.\n

        Decreasing this parameter value can help in making correct decisions
        during abrupt changes. However, decreasing the value too much can
        increase false alarms during long pauses or silences.\n  Byte offset: 50\n }

         @h2xmle_default {2560} */


   int16_t init_length;
    /**< @h2xmle_description   {Defines the number of frames for which a noise level estimate is set
        to a fixed value.\n  Byte offset: 52\n }

         @h2xmle_default {50} */


   int16_t max_val;
    /**< @h2xmle_description   {Defines the upper limit of the noise level.\n  Byte offset: 54\n }
          @h2xmle_default {458} */


   int16_t init_bound;
    /**< @h2xmle_description   {Defines the initial bounding value for the noise level estimate. This
        value is used during the initial segment defined by the init_length
        parameter.\n  Byte offset: 56\n }

        @h2xmle_default {145} */


   int16_t reset_bound;
    /**< @h2xmle_description   {Reset boundary for noise tracking.\n  Byte offset: 58\n }

         @h2xmle_default {290} */


   int16_t avar_scale;
    /**< @h2xmle_description   {Defines the bias factor in noise estimation.\n  Byte offset: 60\n }

         @h2xmle_default {12042} */


   int16_t sub_nc;
    /**< @h2xmle_description   {Defines the window length for noise estimation.\n  Byte offset: 62\n }

         @h2xmle_default {50} */


   int16_t spow_min;
    /**< @h2xmle_description   {Defines the minimum signal power required to update the boundaries
        for the noise floor estimate\n  Byte offset: 64\n }

         @h2xmle_default {580} */


   int16_t eans_gs_fast;
    /**< @h2xmle_description   {Fast smoothing factor for postprocessor gain.\n  Byte offset: 66\n }
         @h2xmle_dataFormat { Q0.15}
         @h2xmle_default {19660} */


   int16_t eans_gs_med;
    /**< @h2xmle_description   { Medium smoothing factor for postprocessor gain.\n  Byte offset: 68\n }
         @h2xmle_dataFormat { Q0.15}
         @h2xmle_default {31130} */


   int16_t eans_gs_slow;
    /**< @h2xmle_description   {Slow smoothing factor for postprocessor gain.\n  Byte offset: 70\n }
         @h2xmle_dataFormat { Q0.15}
         @h2xmle_default {31130} */


   int16_t eans_swb_salpha;
    /**< @h2xmle_description   {Super wideband aggressiveness factor for stationary noise suppression.\n  Byte offset: 72\n }

         @h2xmle_default {0} */


   int16_t eans_swb_nalpha;
    /**< @h2xmle_description   {Super wideband aggressiveness factor for nonstationary noise suppression.\n  Byte offset: 74\n

         @h2xmle_default {4096} */

}
#include "adsp_end_pack.h"
;

/** @}                   <-- End of the Module -->*/

#endif

