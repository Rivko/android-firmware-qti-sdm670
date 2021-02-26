/*========================================================================

Copyright (c) 2008-2009, 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/**********************************************************************
Each of the companies; Lucent, Motorola, Nokia, and Qualcomm (hereinafter
referred to individually as "Source" or collectively as "Sources") do
hereby state:

To the extent to which the Source(s) may legally and freely do so, the
Source(s), upon submission of a Contribution, grant(s) a free,
irrevocable, non-exclusive, license to the Third Generation Partnership
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA,
and TTC, under the Source's copyright or copyright license rights in the
Contribution, to, in whole or in part, copy, make derivative works,
perform, display and distribute the Contribution and derivative works
thereof consistent with 3GPP2's and each Organizational Partner's
policies and procedures, with the right to (i) sublicense the foregoing
rights consistent with 3GPP2's and each Organizational Partner's  policies
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name
or each Organizational Partner's name any 3GPP2 or transposed Publication
even though this Publication may contain the Contribution or a derivative
work thereof.  The Contribution shall disclose any known limitations on
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the
formulating groups of 3GPP2 or any of its Organizational Partners, it
is proposed to the Committee as a basis for discussion and is not to
be construed as a binding proposal on the Source(s).  The Source(s)
specifically reserve(s) the right to amend or modify the material
contained in the Contribution. Nothing contained in the Contribution
shall, except as herein expressly provided, be construed as conferring
by implication, estoppel or otherwise, any license or right under (i)
any existing or later issuing patent, whether or not the use of
information in the document necessarily employs an invention of any
existing or later issued patent, (ii) any copyright, (iii) any
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or
all Normative portions of the Enhanced Variable Rate Codec (EVRC) as
it exists on the date of submittal of this form, should the EVRC be
approved as a Specification or Report by 3GPP2, or as a transposed
Standard by any of the 3GPP2's Organizational Partners, the Source(s)
state(s) that a worldwide license to reproduce, use and distribute the
Software, the license rights to which are held by the Source(s), will
be made available to applicants under terms and conditions that are
reasonable and non-discriminatory, which may include monetary compensation,
and only to the extent necessary for the practice of any or all of the
Normative portions of the EVRC or the field of use of practice of the
EVRC Specification, Report, or Standard.  The statement contained above
is irrevocable and shall be binding upon the Source(s).  In the event
the rights of the Source(s) in and to copyright or copyright license
rights subject to such commitment are assigned or transferred, the
Source(s) shall notify the assignee or transferee of the existence of
such commitments.
*******************************************************************/

/*======================================================================*/
/*     Enhanced Variable Rate Codec - Bit-Exact C Specification         */
/*     Copyright (C) 1997-1998 Telecommunications Industry Association. */
/*     All rights reserved.                                             */
/*----------------------------------------------------------------------*/
/* Note:  Reproduction and use of this software for the design and      */
/*     development of North American Wideband CDMA Digital              */
/*     Cellular Telephony Standards is authorized by the TIA.           */
/*     The TIA does not authorize the use of this software for any      */
/*     other purpose.                                                   */
/*                                                                      */
/*     The availability of this software does not provide any license   */
/*     by implication, estoppel, or otherwise under any patent rights   */
/*     of TIA member companies or others covering any use of the        */
/*     contents herein.                                                 */
/*                                                                      */
/*     Any copies of this software or derivative works must include     */
/*     this and all other proprietary notices.                          */
/*======================================================================*/
/*******************************************************************
 *
 * typedef statements of function types required for EVRC Encoder
 *
 ******************************************************************/
#ifndef __EVRC_ENC_API_H
#define __EVRC_ENC_API_H

#define TTY

#define NUM_TTY_RATES           2

#define MAX_TTY_BAUD_COUNT  3

#define TTY_FER                 0x0040
#define TTY_NO_GAIN             1

#define TTY_BIT_HIST_LEN        9

#define MIN_BIT_LEN         8   /* min # of dits, good and bad, to make a bit */
#define MAX_BIT_LEN         13  /* max # of dits, good and bad, to make a bit */

#define DITS_PER_FRAME      10  /* 1 dit = 16 samples */
#define DITBUF_LEN          MAX_BIT_LEN + DITS_PER_FRAME + 1

typedef struct evrc_tty_enc_struct_t {
   short int tty_option;
   short int tty_enc_flag;
   short int tty_enc_header;
   short int tty_enc_char;
   short int tty_enc_baud_rate;
   short int data_flag;
   short int speech_data_flag;
   short int framingCount;
   short int ttyState;
   short int last_char_counter;
   short int first_tty_char_flag;
   short int last_tty_char;
   short int tty_bit_hist[TTY_BIT_HIST_LEN];
   short int tty_bit_len_hist[TTY_BIT_HIST_LEN];
   unsigned long  int dit2a_char_count; /* for debugging only */
   short int ditbuf[DITBUF_LEN];
   short int tty_baud_count;
   short int fFigure;           // flag to indicate FIGS or LTRS mode
   short int AsciiChar;
   short int vocoderType;
   short int TTY_SILENCE_HDR;   // vocoder type dependent macros
   short int TTY_ONSET_HDR;
   short int TTY_CHAR_HDR_STOP;
   short int TTY_COUNTER_STOP;
   short int COUNTER_BETWEEN_START_STOP;
   short int TTY_MIN_INPUT_THRESH;
} evrc_tty_enc_struct_t;

extern unsigned int get_evrc_tty_enc_input_size(void);

extern void evrc_init_enc(void *pEvrcEncStructInstance);


extern void evrc_set_reset_variables_options(short int max_rate,
                                        short int min_rate,
                                        short int tty_option,
                                        void *pEvrcEncStructInstance
                                        );

extern void evrc_init_tty_enc(
    short int   *tty_char,
    short int   *counter,
    short int   *tty_baud_rate,
    void *pEvrcTTYEncStructInstance
);

extern short int evrc_tty_enc(
    short int   *tty_char,      /* (o): tty character           */
    short int   *counter,       /* (o): tty character counter   */
    short int   *tty_baud_rate, /* (i/o): prev/new character counter    */
    short int   pcm_buf[],      /* (i): input pcm               */
    short int   len,             /* (i): length of pcm buffer (FRAMESIZE)    */
    void *pEvrcTTYEncStructInstance
);

extern void evrc_set_reset_TTY_variables_options(short int tty_enc_char,
                            short int tty_enc_header,
                            short int tty_enc_baud_rate,
                            short int tty_enc_flag,
                            void *pEvrcTTYEncStructInstance
                            );

extern void evrc_init_enc(void* pEVRCEncInitObjectStruct);
extern void evrc_set_enc_params(void *encStructPtr,unsigned short int maxRate,
                         unsigned short int minRate,unsigned short int rateModulationCmd,
                         unsigned short int comfortNoiseEnable,unsigned short int reinitVocBNE,short int nrOccurredVocoder);
extern long int evrc_encoder(void* pEVRCEncObjectStruct, short int* psInputStream, short int* psOutputPacket,
                          short int ttyEncflag, evrc_tty_enc_struct_t *pTTYEncStruct);
extern short int get_sizeof_evrc_enc_struct(void);
extern long int evrc_repacking_dsp_to_mvs(char *iPacket);

#endif //__EVRC_ENC_API_H
