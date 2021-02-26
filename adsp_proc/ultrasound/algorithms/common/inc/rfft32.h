/***************************************************************************
* Copyright (c) 2013 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 


/*! \file rfft32.h
    \brief Brief description of file
*/

/**
 Calls us_rfft32x16 with hard-coded Wt1 and Wt2 arguments 
 supports N=256,512,1024,2048 
*/
void us_rfft32(CWord2x32 *input, int N, CWord2x32 *output);




