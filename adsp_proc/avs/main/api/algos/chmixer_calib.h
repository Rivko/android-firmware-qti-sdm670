#ifndef CHMIXER_CALIB_H
#define CHMIXER_CALIB_H
/*==============================================================================
  @file chmixer_calib.h
  @brief This file contains CHMIXER API
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

/** @h2xml_title1           {Audio Channel Mixer}
    @h2xml_title_agile_rev  {Audio Channel Mixer}
    @h2xml_title_date       {February 9, 2017} */

#include  "mmdefs.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define AUDPROC_MODULE_ID_CHMIXER                                  0x00010341
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_CHMIXER",
                          AUDPROC_MODULE_ID_CHMIXER}
    @h2xmlm_displayName  {"AUDIO CHMIXER"}
    @h2xmlm_description  {ID of the Channel Mixer module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_CHMIXER_PARAM_ID_COEFF\n

    All parameter IDs are device independent.\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/* ID of the Coefficient parameter used by AUDPROC_MODULE_ID_CHMIXER to configure the channel mixer weighting coefficients. */
#define AUDPROC_CHMIXER_PARAM_ID_COEFF                                    0x00010342

/* Structure payload for: AUDPROC_CHMIXER_PARAM_ID_COEFF*/

typedef struct audproc_chmixer_param_id_coeff_t audproc_chmixer_param_id_coeff_t;
/** @h2xmlp_parameter   {"AUDPROC_CHMIXER_PARAM_ID_COEFF", AUDPROC_CHMIXER_PARAM_ID_COEFF}
    @h2xmlp_description {Configures the channel mixer weighting coefficients.\n
    Immediately following this structure are these variable-sized arrays in
    sequence:\n
    -# num_output_channels entries of output channel mapping.\n
       (Permitted values: 1 to 34\n)
    -# num_input_channels entries of input channel mapping.\n
       (Permitted values: 1 to 34\n)
    -# num_output_channels (rows) x num_input_channels (columns) entries of
       channel mixer weighting coefficients in Q14 format.\n
       (Permitted values: 0 to 16384)\n\n
       For example:\n

 Unity (1)       -> 0x4000 -> 16384\n
 -3 dB (0.707)   -> 0x2D44 -> 11588\n
 -6 dB (0.5)     -> 0x2000 -> 8192\n
 0               -> 0x0    -> 0\n

    -# Optional 16-bit zero padding if the entire combination of 1, 2, and 3
       is not a multiple of 32 bits. Padding is required so the entire
       payload is aligned to 32 bits. (Permitted value: 0)\n\n

    Rules for using Set parameters \n
    - The Set parameters are per-stream, per-device, or per-stream-per-device.\n
    - The Set parameters values are lost when the stream/device is closed. \n
    - For a given stream/device, the aDSP can cache up to eight Set
      parameters. \n
      \n
      For example, this means the client can configure eight Set parameters as
      follows:\n

 Input -- 2, FL, FR \n
 Output -- 2, FL, FR \n
 Coefficients set #0 on index #0 \n \n

 Input -- 5.1, FL, FR, FC, LS, RS, LFE \n
 Output -- 2, FL, FR\n
 Coefficients set #1 on index #1\n
 ..\n
 ..\n
 Input -- 2, FL, C\n
 Output: FL, FR\n
 Coefficients set #7 on index #7\n \n

    The client can overwrite any of the mapping rules by providing the index
    number.\n
    - If multiple rows contain duplicate entries, the higher index rule (which
      is supposedly set later) is chosen and applied, if applicable.\n
    - The aDSP reverts to default coefficients (built-in coefficients) if the
      client does not explicitly provide the rule (e.g., if the input
      or output media types do not match the Set parameter rule provided.\n
    - The Set parameter must be called after stream, device, and routing is
      established. For the Set parameter to take effect correctly, Qualcomm
      recommends calling the Set parameter before the Run command.\n }

    @h2xmlp_toolPolicy  {NO_SUPPORT}*/

#include "adsp_begin_pack.h"

struct audproc_chmixer_param_id_coeff_t
{
   uint32_t index;
   /**< @h2xmle_description  { Specifies index of the mapping rule the client is providing. }

         @h2xmle_range        {0..7}  */

   uint16_t num_output_channels;
   /**< @h2xmle_description  { Specifies number of output channels for this rule. }
         @h2xmle_range        {1..8}
	 @h2xmle_default       {1}	 */

   uint16_t num_input_channels;
   /**< @h2xmle_description  { Specifies number of input channels for this rule. }
         @h2xmle_default       {1}
         @h2xmle_range        {1..8}  */

}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->*/
#endif


