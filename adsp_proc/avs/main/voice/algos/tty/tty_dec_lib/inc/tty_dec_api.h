/*-----------------------------------------------------------------------
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/
#ifndef __TTY_DECAPI_H
#define __TTY_DECAPI_H

#define TTY

#ifndef __TTY_ENCAPI_H

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
/*--------------------------------------------------------------------
* The following are the dit buffer sizes in the decoder.  It is
* placed in this header file in case it needs to be tailored for
* a specific implementation.  The minimum values recommended are:
*
*   #define TTY_BUF_SIZE            11   (9 lookahead + current frame + 1 lookback)
*   #define CURRENT_FRAME            9
*---------------------------------------------------------------------*/

#define TTY_BUF_SIZE            11   /* 9 lookahead + current frame + 1 lookback */
#define CURRENT_FRAME            9


typedef struct TTYDecStruct {
short   tty_dec_flag;
short   tty_dec_header;
short   tty_dec_char;
short   tty_dec_baud_rate;
short   tty_option;
short   counter_hist[TTY_BUF_SIZE];
short   char_hist[TTY_BUF_SIZE];
short   tty_rate_hist[TTY_BUF_SIZE];
short   stop_bit_len[NUM_TTY_RATES];    /* initialized in init_tty_dec() & init_tty_rxtx() */
short   data_bit_len[NUM_TTY_RATES];    /* initialized in init_tty_dec() & init_tty_rxtx() */
short   bit_num;
short   bit_size;
short   current_counter;
short   current_char;
short   prev_bit;
short   tone_param[3];
short   fFigure;          // flag to indicate FIGS or LTRS mode
short   AsciiChar;
short   vocoderType;
short   TTY_SILENCE_HDR;  // vocoder type dependent macros
short   TTY_ONSET_HDR;
short   TTY_CHAR_HDR_STOP;
short   TTY_COUNTER_STOP;
short   COUNTER_BETWEEN_START_STOP;
short   TTY_MIN_INPUT_THRESH;
short   tty_sin_tab_idx;   /* used by tty_continuous_phase_tone_gen() */
} TTYDecStruct;

void init_tty_dec(TTYDecStruct *TTYDecPtr, short ttyOption, short vocoderType);

short tty_dec(
    short   buf[],
    short   acb_gain,
    short   counter,
    short   tty_char,
    short   tty_baud_rate,
    short   fer_flag,
    short   subframe,
    short   num_subfr,
    short   length
   , TTYDecStruct *TTYDecPtr
);
#endif
