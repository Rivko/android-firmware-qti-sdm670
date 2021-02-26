/*========================================================================*/
/**
@file Interleaver.h

@brief Interleaver and deinterleaver utility functions.

This file contains ASM data commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All rights reserved.
Qualcomm Confidential and Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/audio_cmn_lib/Interleaver.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/09/10   rkc      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef _INTERLEAVER_H_
#define _INTERLEAVER_H_

#include "mmdefs.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @addtogroup dsp_algorithms
@{ */

/**
  Interleaves 8-bit stereo data. The input and output buffers must be 
  different.

  @param [in]  inBuf_L                  Input buffer, left channel.
  @param [in]  inBuf_R                  Input buffer, right channel.
  @param [out] outBuf                   Output buffer, data is interleaved.
  @param [in]  numSamplesToInterleave   Number of stereo samples to interleave.

  @return
  Indication of success or failure.
  
  @dependencies
  None.
*/
ADSPResult Interleave_8(int8_t* inBuf_L,
                        int8_t* inBuf_R,
                        int8_t* outBuf,
                        uint32_t numSamplesToInterleave);

/**
  Interleaves 16-bit stereo data. The input and output buffers must be 
  different.

  @param [in]  inBuf_L                  Input buffer, left channel.
  @param [in]  inBuf_R                  Input buffer, right channel.
  @param [out] outBuf                   Output buffer, data is interleaved.
  @param [in]  numSamplesToInterleave   Number of stereo samples to interleave.

  @return
  Indication of success or failure.
  
  @dependencies
  None.
*/
ADSPResult Interleave_16(int16_t* inBuf_L,
                        int16_t* inBuf_R,
                        int16_t* outBuf,
                        uint32_t numSamplesToInterleave);

/**
  Interleaves 32-bit stereo data. The input and output buffers must be 
  different.

  @param [in]  inBuf_L                  Input buffer, left channel.
  @param [in]  inBuf_R                  Input buffer, right channel.
  @param [out] outBuf                   Output buffer, data is interleaved.
  @param [in]  numSamplesToInterleave   Number of stereo samples to interleave.

  @return
  Indication of success or failure.
  
  @dependencies
  None.
*/
ADSPResult Interleave_32(int32_t* inBuf_L,
                        int32_t* inBuf_R,
                        int32_t* outBuf,
                        uint32_t numSamplesToInterleave);

/**
  Deinterleaves 8-bit stereo data. The input and output buffers must be 
  different.

  @param [in]  inBuf                      Input buffer, data is interleaved.
  @param [out] outBuf_L                   Output buffer, left channel.
  @param [out] outBuf_R                   Output buffer, right channel.
  @param [in]  numSamplesToDeInterleave   Number of stereo samples to
                                          deinterleave.

  @return
  Indication of success or failure.
  
  @dependencies
  None.
*/
ADSPResult DeInterleave_8(int8_t* inBuf,
                          int8_t* outBuf_L,
                          int8_t* outBuf_R,
                          uint32_t numSamplesToDeInterleave);

/**
  Deinterleaves 16-bit stereo data. The input and output buffers must be 
  different.

  @param [in]  inBuf                      Input buffer, data is interleaved.
  @param [out] outBuf_L                   Output buffer, left channel.
  @param [out] outBuf_R                   Output buffer, right channel.
  @param [in]  numSamplesToDeInterleave   Number of stereo samples to
                                          deinterleave.

  @return
  Indication of success or failure.
  
  @dependencies
  None.
*/
ADSPResult DeInterleave_16(int16_t* inBuf,
                          int16_t* outBuf_L,
                          int16_t* outBuf_R,
                          uint32_t numSamplesToDeInterleave);

/**
  Deinterleaves 32-bit stereo data. The input and output buffers must be 
  different.

  @param [in]  inBuf                      Input buffer, data is interleaved.
  @param [out] outBuf_L                   Output buffer, left channel.
  @param [out] outBuf_R                   Output buffer, right channel.
  @param [in]  numSamplesToDeInterleave   Number of stereo samples to
                                          deinterleave.

  @return
  Indication of success or failure.
  
  @dependencies
  None.
*/
ADSPResult DeInterleave_32(int32_t* inBuf,
                          int32_t* outBuf_L,
                          int32_t* outBuf_R,
                          uint32_t numSamplesToDeInterleave);


/** \brief Function for interleaving 8-bit multi-channel (num
 *         channel >= 1) data. Caller must ensure input and
 *         output buffers are all non-overlapping */
/**
 * @param [in] inBuf Array of input buffers 
 * @param [out] outBuf Output buffer, data is interleaved 
 * @param [in] numChannels Number of channels 
 * @param [in] numSamplesToInterleave # of multi-channel 
 *        samples to interleave. For example, in stereo, a
 *        pair of left and right channel samples constitute one
 *        stereo sample.
 * 
 * @return ADSPResult Success or Failure
 */
ADSPResult Interleave_Multich_8(int8_t *inBuf[],
                                int8_t *outBuf,
                                uint32_t numChannels,
                                uint32_t numSamplesToInterleave);

/** \brief Function for interleaving 16-bit multi-channel
 *         (num channel >= 1) data. Caller must ensure input and
 *         output buffers are all non-overlapping */
/**
 * @param [in] inBuf Array of input buffers 
 * @param [out] outBuf Output buffer, data is interleaved 
 * @param [in] numChannels Number of channels 
 * @param [in] numSamplesToInterleave # of multi-channel 
 *        samples to interleave. For example, in stereo, a
 *        pair of left and right channel samples constitute one
 *        stereo sample.
 * 
 * @return ADSPResult Success or Failure
 */
ADSPResult Interleave_Multich_16(int16_t *inBuf[],
                                 int16_t *outBuf,
                                 uint32_t numChannels,
                                 uint32_t numSamplesToInterleave);

/** \brief Function for interleaving 32-bit multi-channel
 *         (num channel >= 1) data. Caller must ensure input and
 *         output buffers are all non-overlapping */
/**
 * @param [in] inBuf Array of input buffers 
 * @param [out] outBuf Output buffer, data is interleaved 
 * @param [in] numChannels Number of channels 
 * @param [in] numSamplesToInterleave # of multi-channel 
 *        samples to interleave. For example, in stereo, a
 *        pair of left and right channel samples constitute one
 *        stereo sample.
 * 
 * @return ADSPResult Success or Failure
 */
ADSPResult Interleave_Multich_32(int32_t *inBuf[],
                                 int32_t *outBuf,
                                 uint32_t numChannels,
                                 uint32_t numSamplesToInterleave);
/** @} */ /* end_addtogroup dsp_algorithms */

/** \brief Function for de-interleaving 8-bit multi-channel (num
 *         channel >= 1) data. Caller must ensure input and
 *         output buffers are all non-overlapping */
/**
 * @param [in] inBuf Input buffer, data is interleaved 
 * @param [out] outBuf Array of output buffers 
 * @param [in] numChannels Number of channels 
 * @param [in] numSamplesToDeInterleave # of multi-channel 
 *        samples to deinterleave. For example, in stereo, a
 *        pair of left and right channel samples constitute one
 *        stereo sample.
 * 
 * @return ADSPResult Success or Failure
 */
ADSPResult DeInterleave_Multich_8(int8_t *inBuf,
                                 int8_t *outBuf[],
                                 uint32_t numChannels,
                                 uint32_t numSamplesToDeInterleave);

/** \brief Function for de-interleaving 16-bit multi-channel
 *         (num channel >= 1) data. Caller must ensure input and
 *         output buffers are all non-overlapping */
/**
 * @param [in] inBuf Input buffer, data is interleaved 
 * @param [out] outBuf Array of output buffers 
 * @param [in] numChannels Number of channels 
 * @param [in] numSamplesToDeInterleave # of multi-channel 
 *        samples to deinterleave. For example, in stereo, a
 *        pair of left and right channel samples constitute one
 *        stereo sample.
 * 
 * @return ADSPResult Success or Failure
 */
ADSPResult DeInterleave_Multich_16(int16_t *inBuf,
                                 int16_t *outBuf[],
                                 uint32_t numChannels,
                                 uint32_t numSamplesToDeInterleave);

/** \brief Function for de-interleaving 32-bit multi-channel
 *         (num channel >= 1) data. Caller must ensure input and
 *         output buffers are all non-overlapping */
/**
 * @param [in] inBuf Input buffer, data is interleaved 
 * @param [out] outBuf Array of output buffers 
 * @param [in] numChannels Number of channels 
 * @param [in] numSamplesToDeInterleave # of multi-channel 
 *        samples to deinterleave. For example, in stereo, a
 *        pair of left and right channel samples constitute one
 *        stereo sample.
 * 
 * @return ADSPResult Success or Failure
 */
ADSPResult DeInterleave_Multich_32(int32_t *inBuf,
                                 int32_t *outBuf[],
                                 uint32_t numChannels,
                                 uint32_t numSamplesToDeInterleave);
/** @} */ /* end_addtogroup dsp_algorithms */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_INTERLEAVER_H_*/

