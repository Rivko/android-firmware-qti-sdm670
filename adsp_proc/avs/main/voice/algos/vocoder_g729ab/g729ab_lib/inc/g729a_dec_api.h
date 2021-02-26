#ifndef G729A_DECODER_H
#define G729A_DECODER_H
/*============================================================================
  @file  g729a_decoder.h

  Basic function protptypes used for decoder function

        Copyright (c) 2009 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header  : g729a//G729AVocoderLib//src//g729a_decoder.h */

/* renamed definitions */
                             
#define  G729_DEC_L_FRAME      80          ///<Frame size.                                                             
#define  G729_DEC_M            10          ///<Order of LP filter.                                            
#define  G729_DEC_PIT_MAX      143         ///<Maximum pitch lag.                         
#define  G729_DEC_L_INTERPOL   (10+1)      ///<Length of filter for 
#define  G729_DEC_MA_NP        4           ///<MA prediction order for LSP 
#define  G729_DEC_L_SUBFR      40          ///<Subframe size. 
#define  G729_DEC_INPUT_BYTES  10          ///<Final output bytes of Encoder

 
/**
   @brief g729a_decode
   
   Main Decoder routine

   @param *pstG729DecGlobal : [in,out] Common  global structure containing
                                       common global variables for decoder
   @param *pstG729ComGlobal : [in,out] Common  global structure containing
                                       common global variables for encoder
                                       and decoder
   @param  *psIbuffer       : [in]    input speech buffer
   @param  *psObuffer       : [out]   output speech buffer
   @param  *psSynth         : [out]   Synthesis parameter
*/
extern int32 g729a_decode(void  *pstG729DecGlobal, void *pstG729ComGlobal, 
                    int16 * psIbuffer, int16 * psObuffer,int16 *psSynth, int16 fErasure );

/**
   @brief g729_initDec
   
   Initialises Main Decoder routine

   @param *pstG729DecGlobal : [in,out] Common  global structure containing
                                       common global variables for decoder
   @param *pstG729ComGlobal : [in,out] Common  global structure containing
                                       common global variables for encoder
                                       and decoder
*/
extern int32 g729_initDec(void *pG729Decoder, void *pG729ComDecoder);

/**
   @brief g729a_get_g729a_dec_struct_size
   
   returns the size of the global decoder struct
*/
extern int32 g729a_get_g729a_dec_struct_size(void);

/**
   @brief g729a_get_g729a_dec_com_struct_size
   
   returns the size of the global common struct
*/
extern int32 g729a_get_g729a_dec_com_struct_size(void);

/**
   @brief g729_dec_deframing
   
   Formats buffer from packed buffer to DSP library format
*/
extern void g729_dec_deframing( int8 *srcPacked, int16 *dstUnpacked);



#endif // #define G729A_DECODER_H

