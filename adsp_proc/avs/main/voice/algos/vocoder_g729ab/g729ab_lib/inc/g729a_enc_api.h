#ifndef G729A_CODER_H
#define G729A_CODER_H

/*============================================================================
  @file  g729a_coder.h

  Basic function protptypes used for encoder function

        Copyright (c) 2009 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header  : g729a//G729AVocoderLib//src/g729a_coder.h */


/* renamed definitions */
#define  G729_ENC_L_TOTAL      240         ///<Total size of speech buffer.               
#define  G729_ENC_L_WINDOW     240         ///<Window size in LP analysis.                                 
#define  G729_ENC_L_FRAME      80          ///<Frame size.                                                             
#define  G729_ENC_M            10          ///<Order of LP filter.
#define  G729_ENC_MP1          (G729_ENC_M + 1)    ///<M+1                                            
#define  G729_ENC_PIT_MAX      143         ///<Maximum pitch lag.                         
#define  G729_ENC_L_INTERPOL   (10+1)      ///<Length of filter for 
#define  G729_ENC_MA_NP        4           ///<MA prediction order for LSP 
#define  G729_ENC_OUTPUT_BYTES 10          ///<Final output bytes of Encoder
#define  G729_ENC_NB_SUMACF       3
#define  G729_ENC_NB_CURACF       2
#define  G729_ENC_NB_GAIN         2
#define  G729_ENC_SIZ_SUMACF      (G729_ENC_NB_SUMACF * G729_ENC_MP1)
#define  G729_ENC_SIZ_ACF         (G729_ENC_NB_CURACF * G729_ENC_MP1)


/**
   @brief g729a_encode
   
   Main Encoder routine

   @param *pEncGlobalVar   : [in,out] Common  global structure containing
                                      common global variables for encoder
   @param *pstG729ComGlobal: [in,out] Common  global structure containing
                                      common global variables for encoder
                                      and decoder
   @param  *psTemp          : [in]    input speech buffer
   @param  *psSerial        : [out]   output speech buffer
*/
extern int32  g729a_encode(void *pEncGlobalVar,void *pstG729ComGlobal,int16 frame,
                            int16 vad_enable, 
                            int16 * psTemp,int16 * psSerial);

/**
   @brief g729_initEnc
   
   Initialses the main Encoder routine

   @param *pEncGlobalVar   : [in,out] Common  global structure containing
                                      common global variables for encoder
*/
extern int32  g729_initEnc(void *pEncGlobalVar, void *pComGlobalVar);

/**
   @brief g729a_get_g729a_enc_struct_size
   
   returns the size of the global encoder struct
*/
extern int32  g729a_get_g729a_enc_struct_size(void);

/**
   @brief g729a_get_g729a_enc_com_struct_size
   
   returns the size of the global common struct
*/
extern int32  g729a_get_g729a_enc_com_struct_size(void);

/**
   @brief g729_enc_framing
   
   formats the buffer from DSP library format to packed format
*/
extern void g729_enc_framing( int16 *srcParams, int8 *dstPacked);   



#endif //#if deined G729A_CODER_H

