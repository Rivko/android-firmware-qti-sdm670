#ifndef __AUDIO_BASIC_OP_EXT_H_
#define __AUDIO_BASIC_OP_EXT_H_
/*========================================================================

B A S I C   O P   E X T

*/

/**
@file audio_basic_op_ext.h

basic_op_ext is an extension of basic_op.h, with additional intrinsics
used by linking code.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2008-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/22/08   mc      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "shared_aud_cmn_lib.h"
#include "audio_basic_op.h"

#if ((defined __hexagon__) || (defined __qdsp6__))
#include <hexagon_protos.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup math_operations
@{ */

/* -----------------------------------------------------------------------
** Macro Definitions
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Function Declarations
** ----------------------------------------------------------------------- */

#if ((defined __hexagon__) || (defined __qdsp6__))

#define s16_imag_s32(vari) 								(s16_extract_s32_h(vari))
#define s32_imag_s64(vari) 								((int32)((vari) >> 32))
#define s16_real_s32(varr) 								(s16_extract_s32_l(varr))
#define s32_real_s64(varr) 								((int32)varr)

#define s32_complex_s16_s16(varr,vari)					(Q6_R_combine_RlRl((vari),(varr)))
#define s64_complex_s32_s32(varr,vari)					(Q6_P_combine_RR( (vari), (varr) ))

#define s64_complex_add_s64_s64(x, y)					(Q6_P_vaddw_PP_sat(x,y))
#define s64_complex_sub_s64_s64(x, y)					(Q6_P_vsubw_PP_sat(x,y))

#define s32_complex_mult_rnd_sat_s32_s32(var1, var2)	(Q6_R_cmpy_RR_s1_rnd_sat(var1, var2))

/* TODO : Need to check qdsp6 v5 or 6 support this. */
int64 	s64_complex_mult_rnd_sat_s64_s32(int64, int32);

#define s32_complex_mult_s32_s16_sat(x, y) 				((int32) s32_saturate_s64(s32_mult_s32_s16_rnd_sat( (x), (y) )))

#define s32_complex_conjugate_sat_s32(var1)				((int32)Q6_P_vconj_P_sat(var1))
#define s64_complex_conjugate_sat_s64(var1)				( s64_complex_s32_s32( s32_real_s64(var1), s32_neg_s32_sat(s32_imag_s64(var1)) ) )

#define s64_complex_average_s64_s64(x,y)				(Q6_P_vavgw_PP_crnd(x,y))
#define s64_complex_neg_average_s64_s64_sat(x,y)        (Q6_P_vnavgw_PP_crnd_sat(x,y))

#define s32_bitrev_s32(x,y) 							(Q6_R_lsr_RR(Q6_R_brev_R(x), 32-y))

/**
  Multiplies an unsigned 32-bit integer and another unsigned 32-bit integer,
  and returns the unsigned 64-bit result.
*/
#define u64_mult_u32_u32(var1,var2) Q6_P_mpyu_RR(var1,var2)

/** 
  Counts both leading ones and leading zeros starting from the MSB, and then 
  it selects the maximum.
*/
#define s32_get_msb_s32(var1) Q6_R_clb_R(var1)

/** Counts the leading zeros starting from the MSB.
*/
#define s32_cl0_s32(var1)     Q6_R_cl0_R(var1)

/**
  Gets the least significant bit 1 position (i.e., counts the trailing 
  zeros).
*/
#define s32_get_lsb_s32(var1) Q6_R_ct0_R(var1)

/** Sets ind (0 to 31) bit in var1 
*/
#define s32_set_bit_s32_s32(var1, ind) Q6_R_setbit_RR(var1, ind)

/** Clears ind (0 to 31) bit in var1 
*/
#define s32_clr_bit_s32_s32(var1, ind) Q6_R_clrbit_RR(var1, ind)

/** Tests ind (0 to 31) bit in var1 and result in predicate reg
*/
#define s8_tst_bit_s32_s32(var1, ind) Q6_p_tstbit_RR(var1, ind)

	/** Wrap the var1 into the modulo range from 0 to var2
	*/
#define s32_modwrap_s32_u32(var1, var2) Q6_R_modwrap_RR(var1, var2)

#define s64_mac_s32_s32(mac64, var1, var2)    Q6_P_mpyacc_RR(mac64, var1, var2)

#define s32_mac_s32_s32_s1_sat(mac32, var1, var2)    Q6_R_mpyacc_RR_s1_sat(mac32, var1, var2)

#define s32_accu_asr_s32_imm5(accu32, var, u5)  Q6_R_asracc_RI(accu32, var, u5)

#define s32_accu_asl_s32_imm5(accu32, var, u5)  Q6_R_aslacc_RI(accu32, var, u5)

#define s32_shr_s32_imm5_rnd(var, u5)    Q6_R_asr_RI_rnd(var, u5)

#define s64_shr_s64_imm6_rnd(var, u6)    Q6_P_asr_PI_rnd(var, u6)

#else //define(qdsp6)

#define s32_complex_mult_s32_s16_sat(x, y) 				((int32) s32_saturate_s64(s32_mult_s32_s16_rnd_sat( (x), (y) )))
	int32 s32_cl0_s32(int32 var);
    int16 s16_imag_s32(int32 vari);
    int32 s32_imag_s64(int64 vari);

    int16 s16_real_s32(int32 varr);
    int32 s32_real_s64(int64 varr);

    int32 s32_complex_s16_s16(int16 varr, int16 vari);
    int64 s64_complex_s32_s32(int32 varr, int32 vari);

    int64 s64_complex_add_s64_s64(int64, int64);
    int64 s64_complex_sub_s64_s64(int64, int64);

    int32 s32_complex_mult_rnd_sat_s32_s32(int32 var1, int32 var2);
    int64 s64_complex_mult_rnd_sat_s64_s32(int64 var1, int32 var2);

    int32 s32_complex_conjugate_sat_s32(int32 var1);
    int64 s64_complex_conjugate_sat_s64(int64 var1);

    int64 s64_complex_average_s64_s64(int64, int64);
    int64 s64_complex_neg_average_s64_s64_sat(int64, int64);
    int32 s32_bitrev_s32(int32, int32);

/**
  Multiplies two unsigned 32-bit integers and returns the unsigned 64-bit
  result.
*/
uint64 u64_mult_u32_u32(uint32 var1,uint32 var2);

int32 s32_modwrap_s32_u32(int32 var1, uint32 var2);

int64 s64_mac_s32_s32(int64 mac64, int32 var1, int32 var2);

int32 s32_mac_s32_s32_s1_sat(int32 mac32, int32 var1, int32 var2);

int32 s32_accu_asr_s32_imm5(int32 accu32, int32 var, uint32 u5);

int32 s32_accu_asl_s32_imm5(int32 accu32, int32 var, uint32 u5);


#endif //define(qdsp6)

int32 L_shift_r(int32 L_var1, int16 var2);

int32 s32_mac_s32_s32_s1_rnd_sat(int32 mac32, int32 var1, int32 var2); // TODO: add intrinsic

/** @} */ /* end_addtogroup math_operations */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*__BASIC_OP_EXT_H_*/
