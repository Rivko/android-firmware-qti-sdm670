/********************************************************************************
 * GSM AMR-WB speech codec  REL    Version 7.1.0
 *                          REL    ICN fix
 *                                  Who:  pallavim
 *				    Date: 12/4/2012
 *				    CR No's: 420333
 * File             : amrwb_dec_api.h
 * Purpose          : API functions for AMR WB Decoder 
 *
 ********************************************************************************
 */
#ifndef __AMRWB_DECODER_API_H
#define __AMRWB_DECODER_API_H

#include "amrwb_stddef.h"

typedef enum {
   AMRSUP_CODEC_AMR_NB,
   AMRSUP_CODEC_AMR_WB,
   AMRSUP_CODEC_MAX
} amrsup_codec_type;

typedef enum {
   AMRSUP_SPEECH_GOOD,          /* Good speech frame              */
   AMRSUP_SPEECH_DEGRADED,      /* Speech degraded                */
   AMRSUP_ONSET,                /* onset                          */
   AMRSUP_SPEECH_BAD,           /* Corrupt speech frame (bad CRC) */
   AMRSUP_SID_FIRST,            /* First silence descriptor       */
   AMRSUP_SID_UPDATE,           /* Comfort noise frame            */
   AMRSUP_SID_BAD,              /* Corrupt SID frame (bad CRC)    */
   AMRSUP_NO_DATA,              /* Nothing to transmit            */
   AMRSUP_SPEECH_LOST,          /* Lost speech in downlink        */
   AMRSUP_FRAME_TYPE_MAX
} amrsup_frame_type;

void amrwb_init_decoder(void *spd_state);

void amrwb_close_decoder(void *spd_state);

int amrwb_dec_api(
      int16 prms[],
      int16 synth[],
      void *st
      );

int32 get_amr_wb_dec_struct_size (void);

int32 amrwb_sup_modem_if1_de_framing_main(uint8 *vocPacket,
      uint8  *if1_frame,
      uint8  *last_mode,
      uint8  codec_type
      );

int32 amrwb_sup_if1_de_framing_main(uint8 *vocPacket,
      uint8  *if1_frame,
      uint8 *last_mode,
      uint8 codec_type
      );

int32 amrwb_sup_if2_de_framing_main(uint8 *vocPacket,
      uint8 *if2_frame,
      uint8 *last_mode
      );

int32 amrwb_sup_fs_de_framing_main(uint8 *vocPacket,
      uint8 *fs_frame,
      uint8 *last_mode,
      uint8 codec_type
      );

#endif
