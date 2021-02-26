/*========================================================================

Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.5.0   March 2, 2001
*                                R99   Version 3.2.0
*                                REL-4 Version 4.0.0
*      REL        WHO               DATE             CR/FR NO
       1.0.3      ktv               9/17/2012        396040
       2.1.0      ktv               4/18/2013        CR 451999/FR 9246
       2.2.7      Krupesh           6/20/2013        CR 485214
       2.3.1      Krupesh           9/10/2013        CR 546689, 546131, 546314
       2.4.1      SA                xxxxx
********************************************************************************
*
*      File             : amr_dec_api.h
*      Purpose          : typedef statements of function types required for amr Decoder
*
********************************************************************************
*/

#ifndef __EAMR_DEC_API_H
#define __EAMR_DEC_API_H

#define CODEC_TYPE_AMR_NB 0
#define CODEC_TYPE_AMR_WB 1
enum RXFrameType { RX_SPEECH_GOOD = 0,
                   RX_SPEECH_DEGRADED,
                   RX_ONSET,
                   RX_SPEECH_BAD,
                   RX_SID_FIRST,
                   RX_SID_UPDATE,
                   RX_SID_BAD,
                   RX_NO_DATA,
                   RX_N_FRAMETYPES     /* number of frame types */
};

extern int get_eamr_dec_struct_size(void);
extern unsigned int get_eamr_bw_det_state(void *pAmrDecStructInstance);

extern int eamr_decoder_frame_init(void *pAmrDecStructInstance,// Structure ponter (i/o)
                                  short int rxframetypeMode,      // Send zero value to intially
                                  char *id                    // Decoder ID
                                  );

extern int eamr_decode(void *pAmrDecStructInstance,       // Structure ponter (i/o)
                      short int *amrDecPktIn,                // Input packet (i)
                      short int *amrDecSynthOut,              // Output of Decoder (o)
                       short int *amrDecSynthOutWB,              // Output of Decoder (o)
                       short int enable_WM,
                       short int enable_BeAMR
                      );

extern void eamrsup_modem_if1_de_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(o)
                                      unsigned char  *if1_frame,// if1 packet (i)
                                      unsigned char *last_mode, //previous mode
                                      unsigned char codec_type //NB or WB
                                      );
extern void eamrsup_if1_de_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(o)
                                      unsigned char  *if1_frame, // if1 packet (i)
                                      unsigned char *last_mode, //previous mode
                                      unsigned char codec_type //NB or WB
                                      );

extern void eamrsup_if2_de_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(o)
                                      unsigned char  *if2_frame,// if2 packet (o)
                                      unsigned char *last_mode
                                      );

extern void eamrsup_fs_de_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(o)
                                     unsigned char  *fs_frame,// fs packet (o)
                                     unsigned char *last_mode, //previous mode
                                     unsigned char codec_type //NB or WB
                                      );
#endif //_EAMR_DEC_API_H

