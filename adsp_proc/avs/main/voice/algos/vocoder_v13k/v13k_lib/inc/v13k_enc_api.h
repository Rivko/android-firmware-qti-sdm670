
/*========================================================================

Copyright (c) 2008-2009, 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/*======================= COPYRIGHT NOTICE ======================*/
/* Copyright (c) 2008-2009, 2012-2015 by Qualcomm Technologies, Inc.          */
/* All rights reserved.                                          */
/* All data and information contained in or disclosed by this    */
/* document is confidential and proprietary information of       */
/* Qualcomm Technologies, Inc and all rights therein are expressly reserved.  */
/* By accepting this material the recipient agrees that this     */
/* material and the information contained therein is held in     */
/* confidence and in trust and will not be used, copied,         */
/* reproduced in whole or in part, nor its contents revealed in  */
/* any manner to others without the express written permission   */
/* of Qualcomm Technologies, Inc.                                             */
/*===============================================================*/

#ifndef __V13K_ENC_API_H
#define __V13K_ENC_API_H

#define TTY_BIT_HIST_LEN        9

#define MIN_BIT_LEN         8   /* min # of dits, good and bad, to make a bit */
#define MAX_BIT_LEN         13  /* max # of dits, good and bad, to make a bit */

#define DITS_PER_FRAME      10  /* 1 dit = 16 samples */
#define DITBUF_LEN          MAX_BIT_LEN + DITS_PER_FRAME + 1

typedef struct v13k_tty_enc_struct_t {
   short tty_option;
   short tty_enc_flag;
   short tty_enc_header;
   short tty_enc_char;
   short tty_enc_baud_rate;
   short data_flag;
   short speech_data_flag;
   short framingCount;
   short ttyState;
   short last_char_counter;
   short first_tty_char_flag;
   short last_tty_char;
   short tty_bit_hist[TTY_BIT_HIST_LEN];
   short tty_bit_len_hist[TTY_BIT_HIST_LEN];
   unsigned long int dit2a_char_count; /* for debugging only */
   short ditbuf[DITBUF_LEN];
   short tty_baud_count;
   short fFigure;           // flag to indicate FIGS or LTRS mode
   short AsciiChar;
   short vocoderType;
   short TTY_SILENCE_HDR;   // vocoder type dependent macros
   short TTY_ONSET_HDR;
   short TTY_CHAR_HDR_STOP;
   short TTY_COUNTER_STOP;
   short COUNTER_BETWEEN_START_STOP;
   short TTY_MIN_INPUT_THRESH;
} v13k_tty_enc_struct_t;

extern short int v13k_get_enc_struct_size(void);

extern void v13k_init_encoder13(void *encStructPtr);

extern void v13k_set_enc_params(void *encStructPtr,
                         unsigned short int maxRate,
                         unsigned short int minRate,
                         unsigned short int rateModulationCmd,
                         unsigned short int comfortNoiseEnable,
                         unsigned short int reinitVocBNE,
                         unsigned short int reducedRateLevel,
                         short int nrOccurredVocoder);

extern void v13k_lvb_enc(void *sEncoderPtr,
                  short int *inpSamplesPtr,
                  short int *pktPtr,
                  short int ttyEncflag,
                  v13k_tty_enc_struct_t *pTTYEncStruct
                  );

extern signed long int v13k_repacking_dsp_to_mvs(signed char *iPacket);

#endif //__v13k_ENC_API_H
