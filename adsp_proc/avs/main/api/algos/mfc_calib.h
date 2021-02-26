#ifndef MFC_CALIB_H
#define MFC_CALIB_H

#include "mmdefs.h"

/*==============================================================================
  @file mfc_calib.h
  @brief This file contains MFC API
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


/* ID of the Media Format Converter (MFC) module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT
    - #AUDPROC_CHMIXER_PARAM_ID_COEFF
 */
/*  Refer to topology recommendation TR3. */

/** @h2xml_title1           {Media Format Converter}
    @h2xml_title_agile_rev  {Media Format Converter}
    @h2xml_title_date       {March 3, 2017} */
/**
   @h2xmlx_xmlNumberFormat {int}
*/

#define AUDPROC_MODULE_ID_MFC                               0x00010912
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_MFC",
                          AUDPROC_MODULE_ID_MFC}
    @h2xmlm_displayName  {"AUDIO MFC"}
    @h2xmlm_description  {ID of the Media Format Converter (MFC) module.\n
    This module supports the following parameter IDs:\n
AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT\n
AUDPROC_CHMIXER_PARAM_ID_COEFF\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/


/* ID of the Output Media Format parameters used by AUDPROC_MODULE_ID_MFC.
 */
#define AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT            0x00010913
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT", AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT}
    @h2xmlp_description {Structure for the output media format parameter used by the Media
 Format Converter module.\n } */

/* Structure for the output media format parameter used by the Media
 Format Converter module. */
typedef struct audproc_mfc_output_media_fmt_t audproc_mfc_output_media_fmt_t;

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT parameter in the
 Media Format Converter Module.
 */
struct audproc_mfc_output_media_fmt_t
{
   uint32_t sampling_rate;
   /**< @h2xmle_description  {Sampling rate in samples per second.}

         @h2xmle_range        {0..384000}  */

   uint16_t bits_per_sample;
   /**< @h2xmle_description  {Number of bits used to store each sample.}
        @h2xmle_rangeList   {"16 bits per sample (Q15 format)"= 16;"24 bits per sample (Q27 format)"=24;"32 bits per sample (Q31 format)"=32}
    */

   uint16_t num_channels;
   /**< @h2xmle_description  {Number of channels.}
         @h2xmle_default      {1}
         @h2xmle_range        {1..8}  */

   uint16_t channel_type[8];
   /**< @h2xmle_description  {Channel mapping array. Specify a channel mapping for each output channel.If the number of channels is not a multiple of four, zero padding must be added to the channel type array to align the packet to a multiple of 32 bits.}
   @h2xmle_range        {0..7}  */
}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->*/
#endif
