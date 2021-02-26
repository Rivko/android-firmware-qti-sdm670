#ifndef FFT_WIDDLES
#define FFT_WIDDLES
/***************************************************************************
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/ 

/********************************************/
/* interleaved twiddles with size of        */
/* 3*4096/4                                 */
/* + support up to 4096-point FFT           */
/* + table can be shortened for smaller     */
/*   size FFT                               */
/********************************************/

extern int32 AudWtwiddles[];     

#if ((defined __hexagon__) || (defined __qdsp6__))
extern int32 AudWt2_1024[];
#else
extern int32 Wt2_1024[];
#endif

#if ((defined __hexagon__) || (defined __qdsp6__))
extern int32 AudWt2_512[];
#else
extern int32 Wt2_512[];
#endif

extern int32 AudWt2_256[];
extern int32 AudWt2_64[]; 
extern int32 AudWt2_32[]; 



#endif //FFT_WIDDLES
