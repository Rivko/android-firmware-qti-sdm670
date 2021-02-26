
/*========================================================================
Elite

This file contains Macros for Voice.

Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_common/inc/VoiceMacros.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/25/10   pg     Initial version
========================================================================== */
#ifndef VOICEMACROS_H
#define VOICEMACROS_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "VoiceMsgs.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define MAX_MUTE_RAMP_DURATION_MSEC 5000   

/***************************** start of macros *****************************/
#if defined(__qdsp6__) || defined(__GNUC__) || defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 220000)
#define VOICE_MEM_ALIGN(t, a)    t __attribute__((aligned(a)))
#elif defined(_MSC_VER)
#define VOICE_MEM_ALIGN(t, a)    __declspec(align(a)) t
#elif defined(__ARMCC_VERSION)
#define VOICE_MEM_ALIGN(t, a)    __align(a) t
#else
#error Unknown compiler
#endif

#define CHECKRETURNVAL( result ) \
   {\
      if ( ADSP_FAILED(( result )) ) \
      {\
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CHECKRETURNVAL [%d]",result);\
         return ( result );\
      }\
   }
#define CHECKRETURNVALERR( result, str ) \
   {\
      if ( ADSP_FAILED(( result )) ) \
      {\
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CHECKRETURNVALERR [%d] [%s]",result,str);\
         return ( result );\
      }\
   }
#define CHECKRETURN( result ) \
   {\
      if ( ADSP_FAILED(( result )) ) \
      {\
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CHECKRETURN [%d]",result);\
         return;\
      }\
   }
#define CHECKRETURNERR( result, str ) \
   {\
      if ( ADSP_FAILED(( result )) ) \
      {\
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CHECKRETURNERR [%d] [%s]",result,str);\
         return;\
      }\
   }
#define CHECKBREAK( result ) \
   {\
      if ( ADSP_FAILED(( result )) ) \
      {\
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CHECKBREAK [%d]",result);\
         break;\
      }\
   }
#define CHECKBREAKERR( result, str ) \
   {\
      if ( ADSP_FAILED(( result )) ) \
      {\
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CHECKBREAKERR [%d] [%s]",result,str);\
         break;\
      }\
   }

/***************************** end of macros *****************************/

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef VOICETIMERSVC_H

