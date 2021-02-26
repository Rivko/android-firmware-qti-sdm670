/*=======================================================================

Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */
/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.5.0   March 2, 2001
*                                R99   Version 3.2.0
*                                REL-4 Version 4.0.0
*      REL        WHO               DATE             CR/FR NO
*      1.0.3      ktv               9/17/2012        396040
*      2.1.0      ktv               4/18/2013        CR 451999/FR 9246
       2.2.7      Krupesh           6/20/2013        CR 485214
       2.3.1      Krupesh           9/10/2012        CR 546689, 546131, 546314
********************************************************************************
*
*      File             : amr_enc_api.h
*      Purpose          : typedef statements of function types required for Amr
*                         Encoder
*
********************************************************************************
*/

#ifndef __EAMR_ENC_API_H
#define __EAMR_ENC_API_H


int get_eamr_enc_struct_size(void);

short int eamr_encoder_frame_init (
                  void *pAmrEncStructInstance,// Structure ponter (i/o)
                  short int dtx,                 // dtx information
                  short int vadType,
                  short int sampleRate
                  );

int eamr_encoder(void *pAmrEncStructInstance,  // Structure ponter (i/o)
                        short int *speechEncInputSamples,// Speech Input Samples (i)
                        short int *amrEncPacketOut,      // AMR packet (o)
                        short int mode,                 // Mode (i)
                        int dtxFlag,                // dtx information (i)
                        short enableWM          // flag to enable WaterMarking (i)
                        );

short int eamr_sid_sync_init (void *pAmrEncStructInstance);
short int eamr_modem_if1_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(i)
                                  unsigned char  *if1_frame// if1 packet (o)
                                );
short int eamr_if1_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(i)
                                  unsigned char  *if1_frame// if1 packet (o)
                                );
short int eamr_if2_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(i)
                                  unsigned char  *if2_frame// if2 packet (o)
                                );

short int eamr_fs_framing_main(unsigned char *amrPacket,//amr packet in qdsp format(i)
                                 unsigned char  *fs_frame// fs packet (o)
                                );

#endif  //__EAMR_ENC_API_H

