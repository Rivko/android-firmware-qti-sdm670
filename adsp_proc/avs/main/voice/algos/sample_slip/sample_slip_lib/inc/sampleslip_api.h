#ifndef SAMPLESLIP_API_H
#define SAMPLESLIP_API_H

/*============================================================================
  @file sampleslip_api.h

  Common public header file for the sample slipping solution.

Copyright (c) 2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/voice/algos/sample_slip/sample_slip_lib/inc/sampleslip_api.h#1 $    */
/*===========================================================================*
 * REVISION HISTORY                                                          *
 *when       who     what, where, why                                        *
 *---------- ------- --------------------------------------------------------* 
 *08/30/10   SS      public heade file for sample slipping algorithm    
 *===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Initializes the sample slipping configuration structure.

  @param ssCfg: [in] Pointer to sample slipping config structure
  @param frameSize: [in] Nominal frame size of the data
  @param multi_frames: [in] Number of multiple frames
 */

void voice_ss_cfg_init( void *ssCfg,
                  int16 frameSize,
                  int16 multi_frames );

/**
  @brief Initializes the sample slipping data structure.

  @param ssCfg: [in] Pointer to sample slipping config structure
  @param ssData: [in] Pointer to sample slipping data structure
 */

void voice_ss_init( const void *ssCfg,
              void *ssData );


/**
  @brief Process one frame of sample slipping algorithm.

  @param ssCfg: [in] Pointer to sample slipping config structure
  @param ssData: [in] Pointer to sample slipping data structure
  @param pInpL16: [in] Pointer to input frame data
  @param pOutL16: [out] Pointer to output frame data
  @param inp_frame: [in] Input frame size
  @param out_frame: [in] Output frame size
  @param ss_mode: [in] Sample slip mode
 */
void voice_samp_slip_process ( const void *ssCfg,
                         void *ssData,
                         int16 *pInpL16,
                         int16 *pOutL16,
                         int16 inp_frame,
                         int16 out_frame,
                         int16 ss_mode );

/**
  @to get the configaration structure size
 */
int ss_cfg_params_size(void);

/**
  @to get the static data structure size
 */
int ss_static_data_size(void);

#endif /* #ifndef SAMPLESLIP_API_H */

