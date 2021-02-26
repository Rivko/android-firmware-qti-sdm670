/***************************************************************************
* Copyright (c) 2013 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 


/*! \file rfft16.h
    \brief Brief description of file
*/

/**
 Calls us_rfft with hard-coded Wt1 and Wt2 arguments 
 supports N=256,512,1024,2048 
*/
void us_rfft16(Word16 *input, int N, CWord2x16 *output);




