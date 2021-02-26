#ifndef __4GV_ENC_API_H
#define __4GV_ENC_API_H

/*========================================================================

Copyright (c) 2008-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

#define FOURGV_ENC_UNSUPPORTED 0xFFFFFFFFLL
#define LOOKAHEAD_LEN            80
#define SPEECH_BUFFER_LEN        160
#define FrameSize   160     /* CELP frame size */
#define FSIZE FrameSize

#define TTY_BIT_HIST_LEN        9

#define MIN_BIT_LEN         8   /* min # of dits, good and bad, to make a bit */
#define MAX_BIT_LEN         13  /* max # of dits, good and bad, to make a bit */

#define DITS_PER_FRAME      10  /* 1 dit = 16 samples */
#define DITBUF_LEN          MAX_BIT_LEN + DITS_PER_FRAME + 1

typedef struct fourgv_tty_enc_struct_t {
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
   unsigned long dit2a_char_count; /* for debugging only */
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
} fourgv_tty_enc_struct_t;

class DTMF_fx;
class fgvEncoder;

class FourGVEncode
{

   public:

   FourGVEncode(int &result);
   FourGVEncode();
   ~FourGVEncode();
   void FGVEncodeDestructor(); //this member function is called from Venc wrapper, which compliments the placement new functionality in destructor
   void encoder(short int *inbuf16, short int *outbuf16,short int outbufsize, short int ttyEncflag,fourgv_tty_enc_struct_t* pTTYEncStruct);
   void init_encoder();
   void set_encoder_params();

   short int buf_fx[SPEECH_BUFFER_LEN*2+LOOKAHEAD_LEN];
   short int*     buf16P;
   short int     *bufP_fx;
   char dim;
   fgvEncoder       *pFgvEncoderObj;

   short operatingpoint_bck;
   short NS;
   short first_time;
   short int   max_rate;
   short int   min_rate;
   short int   ibuf_len;
   short int   obuf_len;
   short int avg_rate_target;
   short int avg_rate_control;
   int Fsop;
   int Fsinp;
   int operating_point;
   int dtx;
   int joint_source_modem_dtx;
};
extern int fourgv_repacking_dsp_to_mvs(char *iPacket);
extern int get_fourgv_enc_static_mem_size();
void init_fourgv_enc_static_mem(void *mem_ptr, FourGVEncode **four_gv_encode_ptr);

#endif  //__4GV_ENC_API_H

