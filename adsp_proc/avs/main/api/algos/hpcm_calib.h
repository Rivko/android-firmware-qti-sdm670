#ifndef HPCM_CALIB_H
#define HPCM_CALIB_H
/*==============================================================================
  @file hpcm_calib.h
  @brief This file contains HPCM
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc .All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/11/17   shrani     Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"


/** @h2xml_title1           {VOICE_MODULE_HPCM}
    @h2xml_title_agile_rev  {VOICE_MODULE_HPCM}
    @h2xml_title_date       {April 11, 2017} */


#define VOICE_MODULE_HPCM_STREAM_TX  0x00010F2F
/**
    @h2xmlm_module       {"VOICE_MODULE_HPCM_STREAM_TX",
                          VOICE_MODULE_HPCM_STREAM_TX}

    @h2xmlm_displayName  {"TX VOICE HPCM STREAM"}
	@h2xmlm_description {HPCM framework that is part of voice preprocessing on the stream Tx path.\n
This module does not support any parameter IDs.\n}
    @h2xmlm_toolPolicy              {Calibration}
    @{                   <-- Start of the Module -->

   @}                   <-- End of the Module -->*/


#define VOICE_MODULE_HPCM_STREAM_RX  0x00010F30
/**
    @h2xmlm_module       {"VOICE_MODULE_HPCM_STREAM_RX",
                          VOICE_MODULE_HPCM_STREAM_RX}

    @h2xmlm_displayName  {"RX VOICE HPCM STREAM"}
	@h2xmlm_description {HPCM framework that is part of voice postprocessing on the stream Rx path.\n
This module does not support any parameter IDs.\n}
    @h2xmlm_toolPolicy              {Calibration}
    @{                   <-- Start of the Module -->


   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_HPCM_DEVICE_TX  0x00010F1E
/**
    @h2xmlm_module       {"VOICE_MODULE_HPCM_DEVICE_TX",
                          VOICE_MODULE_HPCM_DEVICE_TX}

    @h2xmlm_displayName  {"TX VOICE HPCM DEVICE"}
	@h2xmlm_description {Host PCM (HPCM) framework that is part of voice preprocessing on the device Tx path.\n
This module does not support any parameter IDs.\n}

    @h2xmlm_toolPolicy              {Calibration}
    @{                   <-- Start of the Module -->


   @}                   <-- End of the Module -->*/

#define VOICE_MODULE_HPCM_DEVICE_RX  0x00010F2E
/**
    @h2xmlm_module       {"VOICE_MODULE_HPCM_DEVICE_RX",
                          VOICE_MODULE_HPCM_DEVICE_RX}

    @h2xmlm_displayName  {"RX VOICE HPCM DEVICE"}
	@h2xmlm_description {HPCM framework that is part of voice postprocessing on the device Rx path.\n
This module does not support any parameter IDs.\n}
    @h2xmlm_toolPolicy              {Calibration}
    @{                   <-- Start of the Module -->

   @}                   <-- End of the Module -->*/



#endif
