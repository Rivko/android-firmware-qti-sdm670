/**
@file AudioStreamMgr_adsppm.h********
@brief This file declares some common data types used by*
*      AudioStreamMgr_adsppm.cpp (ADSP power manager).

*/

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/18/2012 RB      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#ifndef ASM_ADSPPM_H
#define ASM_ADSPPM_H

#include "AudioStreamMgr_Type.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

   /*-------------------------------------------------------------------------
   Preprocessor Definitions and Constants
   -------------------------------------------------------------------------*/
#define RELEASE_RESOURCE    TRUE
#define REQUEST_RESOURCE    FALSE

/**
 * max tolerable interrupt latency for AVsync/HWresampler/MIDI in microseconds: 1ms
 * TODO: to optimize per core.
 */
#define SLEEP_LATENCY_US   1000



   /*-------------------------------------------------------------------------
   Type Declarations
   -------------------------------------------------------------------------*/

   /*---------------------------------------------------------------------------
   Class Definitions
   ----------------------------------------------------------------------------*/


   /*---------------------------------------------------------------------------
   Function Declarations and Documentation
   ----------------------------------------------------------------------------*/
/*
   This function calls ADSPPM request API with the required MIPS, BW and other resources.

*/

/**
 *
 * register with Adsppm
   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */
   ADSPResult AudioStreamMgr_RegisterPm(AudioStreamMgr_t *pMe);

   /**
    *
      @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                        that associated with the given session might be turned on/off depending
                        on command processing.
      @return Success or Failure
    */
   ADSPResult AudioStreamMgr_UnRegisterPm(AudioStreamMgr_t *pMe);

/**
 * a generic request to adsppm
   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */
   ADSPResult AudioStreamMgr_RequestReleasePm(AudioStreamMgr_t *pMe);



#ifdef __cplusplus
}
#endif //__cplusplus


#endif //ASM_ADSPPM_H


