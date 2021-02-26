#ifndef _fft16_h
#define _fft16_h

/***************************************************************************
* Copyright (c) 2008-2014 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 


/*! \file fft16.h
    \brief Brief description of file
*/
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 Calls us_sfft16x16 with hard-coded "twiddles" argument that is 
 good for up to N=4096 
*/
void us_fft16(CWord2x16 *input, int N, CWord2x16 *output);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // _fft16_h
