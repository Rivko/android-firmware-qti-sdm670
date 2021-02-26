/** @file Elite_CAPI_V2_private_params.h
This file contains private parameters (those not defined by elite public APIs),
to be used between Encoder, Decoder and PP services.
Unless otherwise defined each parameter has a uint32_t value.

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/** 
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_CAPI_V2_private_params.h#1 $


when       who     what, where, why 
--------   ---     -------------------------------------------------------
02/13/14   rbhatnk     Created file.

========================================================================== 
*/ 

#ifndef ELITECAPIV2PRIVATEPARAMS_H
#define ELITECAPIV2PRIVATEPARAMS_H

/**
 * DTMF parameters. Each parameter has a uint32_t value.
 */
#define CAPI_PARAM_DTMF_RUNNING     0x12F2A
#define CAPI_PARAM_DTMF_HIGH_TONE   0x12F2B
#define CAPI_PARAM_DTMF_LOW_TONE    0x12F23
#define CAPI_PARAM_DTMF_DURATION    0x12F24
#define CAPI_PARAM_DTMF_GAIN        0x12F25
#define CAPI_PARAM_DTMF_SET         0x12F26

/* Used to get the number of samples per frame per channel.
 * Currently not used by encoder or decoder.
 *
 * It's usage is deprecated. */
#define CAPI_PARAM_SAMPLES_PER_FRAME   0x12F27
/**< Used to get decoder delay from decoder, used only with gapless playback.
 * zero delay assumed if no implementation in the CAPI. */
#define CAPI_PARAM_DECODER_DELAY       0x12F28
/**< Used to get trailing delay from decoder, used only with gapless playback.
 * zero delay assumed if no implementation in the CAPI. */
#define CAPI_PARAM_TRAILING_DELAY      0x12F29

/**< Used to set encoder config block on the encoder CAPIs.
 *  Encoders must implement this, however, the error is ignored if encoder doesn't implement.*/
#define CAPI_PARAM_ENC_CFG_BLK         0x12F2F

/**< Used to enable converter mode in decode-converter capi.
 *  Payload for this parameter is NULL.*/
#define CAPI_PARAM_ENABLE_CONVERTER 0x00013220
#endif //ELITECAPIV2PRIVATEPARAMS_H
