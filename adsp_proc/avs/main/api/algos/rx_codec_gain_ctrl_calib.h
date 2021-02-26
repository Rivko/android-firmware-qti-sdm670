#ifndef RX_CODEC_GAIN_CTRL_CALIB_H
#define RX_CODEC_GAIN_CTRL_CALIB_H
/*==============================================================================
  @file rx_codec_gain_ctrl_calib.h
  @brief This file contains RX_CODEC_GAIN_CTRL API
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



#include "mmdefs.h"


/** @h2xml_title1           {Rx Codec Gain Control}
    @h2xml_title_agile_rev  {Rx Codec Gain Control}
    @h2xml_title_date       {February 9, 2017} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/



/* ID of the Rx Codec Gain Control module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_RX_CODEC_GAIN
    - #AUDPROC_PARAM_ID_ENABLE
 */
#define AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL                        0x00010C37
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL", AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL}
    @h2xmlm_displayName  {"AUDIO RX CODEC GAIN"}
	@h2xmlm_description {ID of the Rx Codec Gain Control module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_RX_CODEC_GAIN\n
    - #AUDPROC_PARAM_ID_ENABLE\n}
    @h2xmlm_toolPolicy              {Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->  */
/* ID of the Rx Codec Gain Control parameter used by
    AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL.
 */

#define AUDPROC_PARAM_ID_RX_CODEC_GAIN                              0x00010C38


/* Structure for the Rx common codec gain control module. */
typedef struct audproc_rx_codec_gain_t audproc_rx_codec_gain_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_RX_CODEC_GAIN", AUDPROC_PARAM_ID_RX_CODEC_GAIN}
    @h2xmlp_description {Structure for the Rx common codec gain control module.}  */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_RX_CODEC_GAIN parameter in the
 Rx Codec Gain Control module.
 */
struct audproc_rx_codec_gain_t
{
   uint16_t rx_codec_gain;
   /**< @h2xmle_description {Linear gain (in Q13 format)}
        @h2xmle_dataFormat  {Q13}
		@h2xmle_default {0x2000}
        */


   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
   @h2xmle_rangeList {"0"=0}
   @h2xmle_visibility	{hide}
        */
}
#include "adsp_end_pack.h"
;
/**
   @}                   <-- End of the Module --> */
#endif
