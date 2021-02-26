#ifndef BASIC_SIMD_OP_H_
#define BASIC_SIMD_OP_H_

/*========================================================================

B A S I C   S I M D   O P

basic_simd_op is an extension of audio_basic_op.h, with additional intrinsics
for vector operation.

  Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/03/12   minsubl      Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "audio_basic_op.h"

#ifdef __qdsp6__
#include <hexagon_protos.h>
#include <hexagon_types.h>
#else /* qdsp6 */
typedef int64 HEXAGON_Vect64;
typedef int32 HEXAGON_Vect32;

#define HEXAGON_V64_GET_H0(v) \
	((int16) ((v)))
#define HEXAGON_V64_GET_H1(v) \
	((int16) (((v)) >> 16LL))
#define HEXAGON_V64_GET_H2(v) \
	((int16) ((v) >> 32LL))
#define HEXAGON_V64_GET_H3(v) \
	((int16) ((v) >> 48LL))

#define HEXAGON_V64_GET_W0(v) \
	((int32) (v))
#define HEXAGON_V64_GET_W1(v) \
	((int32) ((v) >> 32LL))

#define HEXAGON_V32_GET_H0(v) \
	((int16) (v))

#define HEXAGON_V32_GET_H1(v) \
	((int16) ((v) >> 16))

#define HEXAGON_V_CREATE_H(h3, h2, h1, h0) \
	((((HEXAGON_Vect64) (h3)) << 48LL) | \
	(((HEXAGON_Vect64) ((h2) & 0xffff)) << 32LL) | \
	(((HEXAGON_Vect64) ((h1) & 0xffff)) << 16LL) | \
	((HEXAGON_Vect64) ((h0) & 0xffff)))

#define HEXAGON_V64_CREATE_W(w1, w0) \
	((((HEXAGON_Vect64) (w1)) << 32LL) | ((HEXAGON_Vect64) ((w0) & 0xffffffff)))


#endif /* qdsp6 */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#if defined(__qdsp6__)
#define v64_vaddh_v64_v64_sat(varr, vari)				(Q6_P_vaddh_PP_sat((varr), (vari)))
#define v64_vsubh_v64_v64_sat(varr, vari)				(Q6_P_vsubh_PP_sat((varr), (vari)))
#define v64_vsplath_s32(vari)							(Q6_P_vsplath_R((vari)))
#define v64_vmpyh_v32_v32_sat(varr, vari)				(Q6_P_vmpyh_RR_sat((varr), (vari)))
#define v64_vasrh_v64(varr, shift)						(Q6_P_vasrh_PI((varr), (shift)))
#define s64_vrmpywohacc_v64_v64(acc, varr, vari)		(Q6_P_vrmpywohacc_PP((acc), (varr), (vari)))
#define s64_vrmpywehacc_v64_v64(acc, varr, vari)		(Q6_P_vrmpywehacc_PP((acc), (varr), (vari)))
#define s64_vrmpyhacc_v64_v64(acc, varr, vari)			(Q6_P_vrmpyhacc_PP((acc), (varr), (vari)))
#else //define(qdsp6)
HEXAGON_Vect64 v64_vaddh_v64_v64_sat(HEXAGON_Vect64 varr, HEXAGON_Vect64 vari);
HEXAGON_Vect64 v64_vsubh_v64_v64_sat(HEXAGON_Vect64 varr, HEXAGON_Vect64 vari);
HEXAGON_Vect64 v64_vsplath_s32(int32 vari);
HEXAGON_Vect64 v64_vmpyh_v32_v32_sat(HEXAGON_Vect32 varr, HEXAGON_Vect32 vari);
HEXAGON_Vect64 v64_vasrh_v64(HEXAGON_Vect64 varr, int16 shift);
int64 s64_vrmpywehacc_v64_v64(int64 acc, HEXAGON_Vect64 varr, HEXAGON_Vect64 vari);
int64 s64_vrmpywohacc_v64_v64(int64 acc, HEXAGON_Vect64 varr, HEXAGON_Vect64 vari);
int64 s64_vrmpyhacc_v64_v64(int64 acc, HEXAGON_Vect64 varr, HEXAGON_Vect64 vari);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //define(qdsp6)
#endif /* BASIC_SIMD_OP_H_ */
