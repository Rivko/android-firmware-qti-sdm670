/***************************************************************************
* Copyright (c) 2008-2013 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 


/*! \file fft32.h
    \brief Brief description of file
*/

/**
 Calls us_fft32x16 with hard-coded "twiddles" argument that is good for up
 to N=4096 
*/
void us_fft32(CWord2x32 *input, int N, CWord2x32 *output);




