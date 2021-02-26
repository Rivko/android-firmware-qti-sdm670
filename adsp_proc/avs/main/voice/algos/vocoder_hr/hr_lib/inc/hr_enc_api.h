/*
*
*      File             : hr_enc_api.h
*      Purpose          : typedef statements of function types required for HR
*                         Encoder
*
********************************************************************************
*/
#ifndef __GSM_HR_ENC
#define __GSM_HR_ENC

/*========================================================================

Copyright (c) 2008, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/*_________________________________________________________________________
 |                                                                         |
 |                           Function Prototypes                           |
 |_________________________________________________________________________|
*/

// actual encoding routine
extern int gsmhr_encode(void *pHrEncStruct,    // (i/o) global var struct
                  int16 *pswSpeechBuff, // (i) input buffer 
                  int16 *hrPacket,      //(o) output packet
                  int16 dtx_mode        // (i) dtxmode switch
                       );

// reset the Encoder
extern void gsmhr_resetEnc(void *pHrEncStruct   // (i/o) global var struct
                          );

// returns the size of the global var struct
extern int gsmhr_get_hr_enc_struct_size(void);

//Re-packs the encoded packet as per MVS requirements
extern void gsm_hr_repacking_dsp_to_mvs(int8 *iPacket);

#endif
