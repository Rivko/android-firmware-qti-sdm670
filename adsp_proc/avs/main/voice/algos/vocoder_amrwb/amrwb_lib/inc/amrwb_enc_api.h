/********************************************************************************
 * GSM AMR-WB speech codec  REL    Version 7.1.0
 *                          REL    ICN fix
 *                                  Who:  pallavim
 *				    Date: 12/4/2012
 *				    CR No's: 420333
 * File             : amrwb_enc_api.h
 * Purpose          : API functions for AMR WB Encoder
 *
 ********************************************************************************
 */
#ifndef __AMRWB_ENCODER_API_H
#define __AMRWB_ENCODER_API_H
#include "amrwb_stddef.h"

enum AmrWBModeRate { MMRWB660 = 0,
   MMRWB885,
   MMRWB1265,
   MMRWB1425,
   MMRWB1585,
   MMRWB1825,
   MMRWB1985,
   MMRWB2305,
   MMRWB2385,
   MMRWBDTX,
   MNWB_MODES     /* number of (SPC) modes */
};

void amrwb_init_encoder(void *spe_state);

void amrwb_close_encoder(void /* *spe_state */);


int16 amrwb_enc_api(
      int16 signal[],
      int16 stream[],
      void *st,
      int16 allow_dtx,
      int16 mode
      );

int32 get_amr_wb_enc_struct_size (void);

int32 amrwb_modem_if1_framing_main(uint8 *vocPacket,
      uint8  *modem_if1_frame,
      uint16 codec_type
      );

int32 amrwb_if1_framing_main(uint8 *vocPacket,
      uint8  *if1_frame,
      uint16 codec_type
      );


int32 amrwb_if2_framing_main(uint8 *vocPacket,
      uint8  *if2_frame
      );

int32 amrwb_fs_framing_main(uint8 *vocPacket,
      uint8  *fs_frame,
      uint16 codec_type
      );


#endif
