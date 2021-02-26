/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2006 by Qualcomm Technologies, Inc.  All rights reserved. All data and   *]
[* information contained in or disclosed by this document is confidental and *]
[* proprietary information of Qualcomm Technologies, Inc and all rights therein are       *]
[* expressly reserved. By accepting this material the recipient agrees that  *]
[* this  material and the information contained therein is held in confidence*]
[* and in trust and will not be used, copied, reproduced in whole or in part,*]
[* nor its contents revealed in  any manner to others without the express    *]
[* written permission of Qualcomm Technologies, Inc.                                      *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   voice_iir_api.h               TYPE: C-header file            *]
[* DESCRIPTION: Contains global variables and fn declarations for equalizer. *]
[*   when         who     what, where, why                                   *]
[*   --------     ---     ---------------------------------------------------*]
[*   06/24/2010    pc     Initial revision                                   *]
[*****************************************************************************/
#ifndef _VOICE_IIRTDF2_H_
#define _VOICE_IIRTDF2_H_


/*=============================================================================
      Function Declarations
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int Voice_IirTdf2( signed short int  *inp,
			signed short int *out,
			unsigned short int samples,
			signed long int *numcoefs,
			signed long int *dencoefs,
			signed long int *mem,
			signed short int shiftn,
			signed short int shiftd);

void Voice_ApplyPreGain (signed short int  *pIn, unsigned short int nPreGain,
                         signed short int nInSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _FNS_IIRTDF2_H_ */
