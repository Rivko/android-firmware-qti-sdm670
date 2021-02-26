#ifndef VOICE_SS_H
#define VOICE_SS_H

/****************************************************************************
Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/
/*====================================================================== */

/*========================================================================
                             Edit History
when       who     what, where, why
--------   ---     -------------------------------------------------------
08/30/10   ss      Created file for sample slipiing wrapper
========================================================================== */

/* =======================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */


#include "qurt_elite.h"

//Number of frame for multi frame solution.
#define VOICE_SS_MULTI_FRAME_4 (4)         //Distribute drift correction over 4 frames
#define VOICE_SS_MULTI_FRAME_2 (2)         //Distribute drift correction over 2 frames
#define VOICE_SS_MULTI_FRAME_1 (1)         //Distribute drift correction over 1 frames

/* =======================================================================
                        DATA DECLARATIONS
========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/** @brief Structure for sample slipping module
*/
typedef struct
{
   void *ss_cfg_ptr;
   /**< sample slipping configaration structure */
   void *ss_data_ptr;
   /**< sample slipping static data structure */
   int32_t sample_slip_stuff_sum;
   /**< sum of the number of samples slipped or stuffed */
   uint32_t kpps;
   /**< IIR mpps information in killo packets per second (kpps) unit */
   uint32_t sampling_rate;
   /**< configured sampling rate */
   uint32_t nb_sampling_rate_factor;
   /**< configured sampling rate/8000 */
} ss_struct_type_t;

/* =======================================================================
**                          Function Declarations
** ======================================================================= */
/*************************************************************************
FUNCTION: voice_ss_config
*************************************************************************/
/**
This function configures the sampling rate of the algorithm
@param ss_struct_ptr [in/out] Pointer to the SS Structure
@param sampling_rate [in] 
@return Success or failure.
*/
ADSPResult voice_ss_config(ss_struct_type_t  *ss_struct_ptr, uint32_t sampling_rate);

/*************************************************************************
FUNCTION:voice_ss_get_kpps
*************************************************************************/
/**
This function gives the number of kpps at nb input rate.

@param ss_struct_ptr [in] Pointer to the SS Structure.
@param kpps_ptr[out]  Pointer to the kpps value
@return Success or failure of the instance creation.
*/
ADSPResult voice_ss_get_kpps(ss_struct_type_t  *ss_struct_ptr,
				 uint32_t *kpps_ptr
             );

/*************************************************************************
FUNCTION:voice_ss_get_delay
*************************************************************************/
/**
This function gives the delay introduced in us.

@param ss_struct_ptr [in] Pointer to the SS Structure.
@param delay_ptr[out]  Pointer to the delay value
@return Success or failure of the instance creation.
*/
ADSPResult voice_ss_get_delay(ss_struct_type_t  *ss_struct_ptr,
				 uint32_t *delay_ptr
             );

/** Function that gets the data memory size required for sample slipping algorithm
    @param [out] nSize data memory size
*/
void voice_ss_get_size(uint32_t *nSize);

/** Function that initializes the memory pointers for config and data structures
    @param [out] pSsStruct pointer to sample slipping structure.
    @param [in] pMemAddr pointer to the memory allocated for sample slipping module
    @param [in] nSize total data memory size for sample slipping module
*/
void voice_ss_set_mem(ss_struct_type_t  *pSsStruct, int8_t *pMemAddr, uint32_t nSize);

/** Function that initializes the sample slipping algorithm
    @param [in] pSsStruct pointer to sample slipping structure.
    @param [in] nSsFrameSize input frame size for sample slipping module
    @return An indication of success or failure.
*/
ADSPResult voice_sample_slip_init(ss_struct_type_t  *pSsStruct, int16_t nSsFrameSize, uint8_t multi_frame);

/** Core processing function that performs sample slipping/stuffing
    @param [in] pSsStruct pointer to sample slipping structure.
    @param [pOut] pOut pointer to output buffer.
    @param [pIn] pIn pointer to input buffer.
    @param [in] nInSize input frame size for sample slipping module
    @param [out] nOutSize output frame size for sample slipping module
    @return An indication of success or failure.
*/
ADSPResult voice_sample_slip_process (ss_struct_type_t  *pSsStruct,int16_t *pOut, int16_t *pIn, int16_t nInSize,int16_t nOutSize);

/** Function that destroys the pointers of sample slipping module.
    @param [in] pSsStruct pointer to sample slipping structure.
*/
void voice_ss_end(ss_struct_type_t  *pSsStruct);

#endif /* VOICE_SS_H */

