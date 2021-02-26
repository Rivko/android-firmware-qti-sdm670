#ifndef __4GV_DEC_API_H
#define __4GV_DEC_API_H

/*========================================================================

Copyright (c) 2008, 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

#define LOOKAHEAD_LEN            80
#define SPEECH_BUFFER_LEN        160
#define FrameSize   160     /* CELP frame size */
#define FSIZE FrameSize
#define FOURGV_DEC_UNSUPPORTED -1

//TTY
extern "C" {
    #include "tty_dec_api.h"
}

typedef struct
{
   bool tw_enable;
   bool phase_matching_enable;
   short tw_output_len;
   short tw_fraction;
   int tw_phase_offset;
   int tw_run_length;

}FourGVTW;

class fgvDecoder;
class FourGVDecode
{

   public:

   FourGVDecode(int &result);
   FourGVDecode();
   ~FourGVDecode();
   void FGVDecodeDestructor(); //this member function is called from Vdec wrapper, which compliments the placement new functionality in destructor
   short int decoder(short int *inbuf16,short int *outbuf16, short int outbufsize, TTYDecStruct
         *pTTYDecStruct, FourGVTW *pFourGVTW);
   void init_decoder();
   // returns mode bit
   short int fourgv_get_mode_bit();

   short int buf_fx[SPEECH_BUFFER_LEN*2+LOOKAHEAD_LEN];
   short int     *buf16P;
   short int     *bufP_fx;
   short int      rate;
   fgvDecoder  *pFgvDecoderObj;

   short int   ibuf_len;
   short int   obuf_len;
   int Fsop;
   int Fsinp;
   short int serviceOption;
};
extern void fourgv_repacking_mvs_to_dsp(char *iPacket);

extern int get_fourgv_dec_static_mem_size();
void init_fourgv_dec_static_mem(void *mem_ptr, FourGVDecode **four_gv_decode_ptr);

#endif  //__4GV_DEC_API_H

