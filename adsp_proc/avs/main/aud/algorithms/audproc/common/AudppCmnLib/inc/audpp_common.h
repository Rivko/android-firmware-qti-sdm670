/*======================= COPYRIGHT NOTICE ==================================*]
Copyright (c) 2008-2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   audpp_common.h                    TYPE: C-header file              *]
[* DESCRIPTION: Common types and constants for post processor.               *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   04/04/06   xyp      Initial revision                                    *]
[*   10/22/07   sg       Clean up. Removed File handling, config file related*]
[*                       variables.                                          *]
[*****************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

/*=============================================================================
      Constants
=============================================================================*/
/* main */
#define BIT_EXACT_WITH_QSOUND 0  // to match QSound or not   
#define REAL_TIME 0              // simulate changing parameters on the fly
#define FIFO_SIZE   (2*1920)     // FIFO buffer size (16-bit words), 
                                 // set to even for stereo!!

/* untouched interface value */
#define EMPTY_VALUE (0xFFFFFFFF) // invalid interface value to mark init state

/* block size */                 // currently, every module uses same size
#define BLOCKSIZE   480

/* chorus */
#define CHORUS_GAME_AUDIO 1     // per-object chorus for game audio
#define CHORUS_MIDI (!CHORUS_GAME_AUDIO) // global chorus for MIDI

/* reverb */
#define REVERB_HARDCODE_DELAYS 0 // load comb and all-pass delays from table
#define REVERB_LP_POST_BIQUAD 0  // low-power reverb post filter  

/*=============================================================================
      Typedefs 
=============================================================================*/
typedef enum 
{ 
   MONO= 1,
   STEREO = 2,
   SUB_SURROUND = 4
} ChannelMode;

typedef enum 
{ 
   PPSUCCESS=0,
   PPFAILURE,
   PPERR_DELAY_NEGATIVE, 
   PPERR_DELAY_INVALID,
   PPERR_FILTER_INVALID_TAPCOUNT,
   PPERR_FILTER,
   PPERR_RATECONVERTER,
   PPERR_PANNERCONVERT_OVERFLOW,
   PPERR_PANNERCONVERT_INVALID,
   PPERR_RATECONVERTER_INVALID_INDEX,
   PPERR_STEREOCONVERTER,
   PPERR_IIR_OVERFLOW,
   PPERR_EQ_INVALID_NUMBANDS,
} PPStatus;

// output type
typedef enum 
{
    OUTPUT_NULL     = -1,
    OUTPUT_SPEAKERS = 0,
    OUTPUT_HEADPHONE
} outputtype;

// geometries for speaker mode
typedef enum
{
    SPEAKER_GEOMETRY_NULL    = -1,
    SPEAKER_GEOMETRY_DESKTOP = 0,
    SPEAKER_GEOMETRY_FRONT,
    SPEAKER_GEOMETRY_SIDE
} spkgeometry;

/*=============================================================================
      Interface Parameters
=============================================================================*/

#endif /* _COMMON_H_ */
