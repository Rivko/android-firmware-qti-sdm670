#ifndef __GSM_HR_DEC
#define __GSM_HR_DEC
/*========================================================================

Copyright (c) 2008, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */
/*_________________________________________________________________________
 |                                                                         |
 |                           Function Prototypes                           |
 |_________________________________________________________________________|
*/
extern int gsmhr_decode(void *pHrDecStruct,
      int16 *pHrPacket,
      int16 *pswDecodedSpeechFrame
      );
extern void gsmhr_resetDec(void *pHrDecStruct);
extern int gsmhr_get_hr_dec_struct_size(void);
extern void gsm_hr_repacking_mvs_to_dsp(int8 *iPacket);
#endif
