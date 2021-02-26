/*-----------------------------------------------------------------------
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/
#ifndef __TTY_ENCAPI_H
#define __TTY_ENCAPI_H

#define TTY

#ifndef __TTY_DECAPI_H

#define TTY_SILENCE             0x0001
#define NUM_TTY_RATES           2
#define TTY_BIT_HIST_LEN        9

#define MIN_BIT_LEN         8   /* min # of dits, good and bad, to make a bit */
#define MAX_BIT_LEN         13  /* max # of dits, good and bad, to make a bit */

#define DITS_PER_FRAME      10  /* 1 dit = 16 samples */
#define DITBUF_LEN          MAX_BIT_LEN + DITS_PER_FRAME + 1
#define MAX_TTY_BAUD_COUNT  3


#define TTY_FER                 0x0040
#define TTY_NO_GAIN             1

enum CDMA_VOCODER_TYPE {
   CDMA_V13K = 0,
   CDMA_EVRC,
   CDMA_FOURGV
};

#endif


typedef struct TTYEncStruct {
short   tty_option;
short   tty_enc_flag;
short   tty_enc_header;
short   tty_enc_char;
short   tty_enc_baud_rate;
short   data_flag;
short   speech_data_flag;
short   framingCount;
short   ttyState;
short   last_char_counter;
short   first_tty_char_flag;
short   last_tty_char;
short   tty_bit_hist[TTY_BIT_HIST_LEN];
short   tty_bit_len_hist[TTY_BIT_HIST_LEN];
unsigned long dit2a_char_count;   /* for debugging only */
short   ditbuf[DITBUF_LEN];
short   tty_baud_count;
short   fFigure;           // flag to indicate FIGS or LTRS mode
short   AsciiChar;
short   vocoderType;
short   TTY_SILENCE_HDR;   // vocoder type dependent macros
short   TTY_ONSET_HDR;
short   TTY_CHAR_HDR_STOP;
short   TTY_COUNTER_STOP;
short   COUNTER_BETWEEN_START_STOP;
short   TTY_MIN_INPUT_THRESH;
} TTYEncStruct;

void init_tty_enc(TTYEncStruct *TTYEncPtr, short ttyOption, short vocoderType);

short tty_enc(
    short   pcm_buf[],      /* (i): input pcm               */
    /*short   len,             (i): length of pcm buffer (FRAMESIZE)    */
    TTYEncStruct *TTYEncPtr
);

#endif
