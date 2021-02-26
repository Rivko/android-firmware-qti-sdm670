/*===========================================================================* 
Copyright (c) 2007-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*===========================================================================*/
/***********************************************************************
*
*   FILE : basic_op.h
*
*   PURPOSE:
*
*     Bit-exact simulation of a
*     generic 32 bit accumulator DSP chip (fractional math).  This
*     version has a latching overflow bit (giOverflow) and
*     non-compound MAC's (One where initial mult does not saturate)
*
***********************************************************************/

#ifndef __BASICOP_H_
#define __BASICOP_H_

#include "AEEStdDef.h"

#ifdef round
#undef round
#endif // round

#if ((defined __hexagon__) || (defined __qdsp6__))
#include <hexagon_protos.h>
#include <hexagon_types.h>
#endif

static const int32 MAX_32 = 0x7FFFFFFFL;
static const uint32 UMAX_32 = 0xFFFFFFFFL;
static const int32 MIN_32 = 0x80000000L;
static const int32 LONGWORD_HALF = 0x40000000L;
#define LONGWORD_SIGN  MIN_32       /* sign bit */
#define LONGWORD_MIN  MIN_32
#define LONGWORD_MAX  MAX_32

static const int16 MAX_16 = 0x7FFF;
static const uint16 UMAX_16 = 0xFFFF;
static const int16 MIN_16 = 0x8000;
static const int16 SHORTWORD_HALF = 0x4000;
#define SHORTWORD_SIGN  MIN_16          /* sign bit for Word16 type */
#define SHORTWORD_MIN  MIN_16           /* smallest Ram */
#define SHORTWORD_MAX  MAX_16           /* largest Ram */
/*_________________________________________________________________________
|                                                                         |
|                            Function Prototypes                          |
|_________________________________________________________________________|
*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#if ((defined __hexagon__) || (defined __qdsp6__))
    /* addition */
    /************/
#define s16_add_s16_s16(var1, var2)  (int16) Q6_R_add_RlRl(var1, var2)  /* 1 ops */
#define s16_add_s16_s16_sat(var1, var2)  (int16) Q6_R_add_RlRl_sat(var1, var2)
#define s32_add_s32_s32(var1, var2)  Q6_R_add_RR(var1, var2)  /* 2 ops */
#define s32_add_s32_s32_sat(var1, var2)  Q6_R_add_RR_sat(var1, var2)
#define s64_add_s32_s32(var1, var2)  Q6_P_add_PP((int64) var1, (int64) var2)
#define s64_add_s32_u32(var1, var2)  Q6_P_add_PP((int64) var1, (uint32) var2)
#define s64_add_s64_s32(var1, var2)  Q6_P_add_PP((int64) var1, (int64) var2)
#define s64_add_s64_s64(var1, var2)  Q6_P_add_PP((int64) var1, (int64) var2)

#define s16_sub_s16_s16(var1, var2)  (int16) Q6_R_sub_RlRl(var1, var2)
#define s16_sub_s16_s16_sat(var1, var2)  (int16) Q6_R_sub_RlRl_sat(var1, var2)
#define s32_sub_s32_s32(var1, var2)  Q6_R_sub_RR(var1, var2)
#define s32_sub_s32_s32_sat(var1, var2)  Q6_R_sub_RR_sat(var1,var2)
#define s64_sub_s64_s64(var1, var2)  Q6_P_sub_PP((int64) var1, (int64) var2)

    /* multiplication */
    /******************/
#define s32_mult_s16_s16(var1, var2)  Q6_R_mpy_RlRl(var1, var2)
#define u32_mult_u16_u16(var1, var2)  (uint32) Q6_R_mpyu_RlRl(var1, var2)
#define s32_mult_s16_u16(var1, var2)  (int32) Q6_P_mpy_RR((int32)var1, Q6_R_zxth_R(var2))
#define s32_mult_s16_s16_shift_sat(var1, var2)  Q6_R_mpy_RlRl_s1_sat(var1, var2)
#define s64_mult_u16_u16_shift(var1,var2,shift)  Q6_P_asl_PR(u32_mult_u16_u16(var1, var2), shift)
#define s64_mult_s16_u16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_P_mpy_RR((int32)var1, Q6_R_zxth_R(var2)),shift)
#define s64_mult_s16_s16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_R_mpy_RlRl(var1, var2), shift)

#define s32_mult_s32_s16_rnd_sat(var1, var2) Q6_R_mpy_RRl_s1_rnd_sat(var1, var2)
#define s64_mult_s32_s16(var1,var2)  Q6_P_mpy_RR(var1,(int32) var2)
#define s64_mult_s32_u16(var1,var2)  Q6_P_mpy_RR(var1, var2)
#define s64_mult_u32_s16(var1,var2)  s64_mult_u32_s16_shift(var1, var2, 0)
#define s64_mult_u32_s16_shift(var1,var2,shift)  ((var2 >= 0)? (Q6_P_asl_PR(Q6_P_mpyu_RR(var1,var2),(shift-16))) : \
    (Q6_P_asl_PR(Q6_P_neg_P(Q6_P_mpyu_RR(var1,-var2)),(shift-16))))
#define s64_mult_s32_u16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_P_mpy_RR(var1, Q6_R_zxth_R(var2)),shift-16)
#define s64_mult_s32_s16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_P_mpy_RR(var1,(int32) var2), shift-16)

#define s32_mult_s32_s32_rnd_sat(var1, var2)  Q6_R_mpy_RR_rnd(var1, var2)
#define s64_mult_s32_s32(var1,var2)  Q6_P_mpy_RR(var1 , var2 )
#define s64_mult_s32_s32_shift(var1, var2, shift)  Q6_P_asl_PR(Q6_P_mpy_RR(var1,var2), shift-32)
#define s64_mult_s32_u32_shift(var1, var2, shift)  ((var1 >= 0) ? (Q6_P_asl_PR(Q6_P_mpyu_RR(var1,var2),(shift-32))) : \
    (Q6_P_asl_PR(Q6_P_neg_P(Q6_P_mpyu_RR(-var1,var2)),(shift-32))))

#define s64_mult_lp_s32_s32_shift(var1, var2, shift)  (Q6_P_asl_PR(Q6_P_mpy_RR(var1,var2), shift-32))
#define s64_mult_lp_s32_u32_shift(var1, var2, shift)  s64_mult_s32_u32_shift(var1, var2, shift)

    /* arithmetic shifts */
    /*********************/
#define s16_shr_s16_sat(var1, shift)  (int16) Q6_R_asr_RR_sat(var1, shift)
#define s16_shl_s16_sat(var1, shift)  (int16) Q6_R_asl_RR_sat(var1, shift)
#define s32_shr_s32_sat(var1, shift)  Q6_R_asr_RR_sat(var1, shift)
#define s32_shl_s32_sat(var1, shift)  Q6_R_asl_RR_sat(var1, shift)
#define s32_shl_s32_rnd_sat(var1,shift)  ((shift >= 0) ?  \
                         (s32_shl_s32_sat(var1, shift)) : \
                         (s32_add_s32_s32_sat(s32_shl_s32_sat(var1, shift),s32_and_s32_s32(s32_shl_s32_sat(var1, (shift + 1)), 0x1))))
#define s64_shl_s64(var1, shift)  Q6_P_asl_PR(var1, shift)

    /* logical operations */
    /**********************/
#define s16_and_s16_s16(var1, var2)  (int16) Q6_R_and_RR((int32) var1, (int32) var2)
#define s32_and_s32_s32(var1, var2)  Q6_R_and_RR(var1, var2)

    /* absolute value  */
    /*******************/
#define u16_abs_s16_sat(var1)  (uint16)    Q6_R_asrh_R(Q6_R_abs_R_sat(Q6_R_aslh_R(var1)))//more efficient intrinsic?
#define u32_abs_s32_sat(var1)  (uint32)    Q6_R_abs_R_sat(var1)

    /* multiply accumulate  */
    /************************/
#define s16_mac_s32_s16_s16_sat_rnd(var3,var1,var2)  (int16) Q6_R_asrh_R(Q6_R_add_RR_sat(Q6_R_mpyacc_RlRl_sat(var3,var1,var2),0x08000))
#define s16_msu_s32_s16_s16_sat_rnd(var3,var1,var2)  (int16) Q6_R_asrh_R(Q6_R_add_RR_sat(Q6_R_mpynac_RlRl_s1_sat(var3,var1,var2),0x08000))
#define s32_msu_s32_s16_s16_sat(var3,var1,var2)  Q6_R_mpynac_RlRl_s1_sat(var3, var1, var2)
#define s64_mac_s64_s16_s16_shift_sat(var3,var1,var2,shift)  Q6_R_add_RR_sat(var3, Q6_R_asl_RR_sat(Q6_R_mpy_RlRl(var1,var2), shift))
#define s64_mac_s64_s16_s16_shift  s64_mac_s64_s16_s16_shift_sat
#define s64_mac_s64_s16_s16_s1(var3, var1, var2)  Q6_P_mpyacc_RlRl_s1(var3, var1, var2)

    /*Additional operators*/
    /**********************/

    /* negation  */
    /*************/
#define s16_neg_s16_sat(var1)  (int16) Q6_R_sath_R(Q6_R_neg_R( Q6_R_sxth_R(var1)))
#define s32_neg_s32_sat(var1)  Q6_R_neg_R_sat(var1)

    /* ALU operations */
    /******************/
#define s16_complement_s16(var1)  (int16) Q6_R_not_R(var1)
#define s16_max_s16_s16(var1, var2)  (int16) Q6_R_max_RR(var1,var2)
#define s16_min_s16_s16(var1, var2)  (int16) Q6_R_min_RR(var1,var2)
#define s32_min_s32_s32(var1, var2)  Q6_R_min_RR(var1,var2)
#define s32_max_s32_s32(var1, var2)  Q6_R_max_RR(var1,var2)

    /* Accumulator manipulation */
    /****************************/
#define s32_deposit_s16_l(var1)  Q6_R_sxth_R(var1)
#define s32_deposit_s16_h(var1)  Q6_R_aslh_R(var1)

#define s16_extract_s32_l(var1)  (int16) Q6_R_sxth_R(var1)
#define u16_extract_s32_l(var1)  (uint16) Q6_R_zxth_R(var1)
#define s16_extract_s32_h(var1)  (int16) Q6_R_asrh_R(var1)
#define s16_extract_s64_h(var1)  (int16)(Q6_P_asr_PR(var1, 16))
#define s16_extract_s64_l(var1)  (int16)(var1)
#define s16_extract_s64_h_sat(var1)  (int16)Q6_R_asrh_R(Q6_R_sat_P(var1))
#define s32_extract_s64_l(var1)  (int32)(var1)
#define u32_extract_s64_l(var1)  (uint32)(var1)

    /* Round */
    /*********/
#define s16_round_s32_sat(var1)  (int16) Q6_R_asrh_R(Q6_R_add_RR_sat(var1,0x00008000L))

    /* Normalization */
    /*****************/
#define s16_norm_s64(var1)  (int16) ((var1 == 0) ? (0) : (Q6_R_sub_RR(Q6_R_clb_P(var1), 33)))
#define s16_norm_s32(var1)  (int16) ((var1 == 0) ? (0) : (Q6_R_sub_RR(Q6_R_clb_R(var1),1)))
#define s16_norm_s16(var1)  s16_norm_s32(s32_deposit_s16_h(var1))
#define s32_cl1_s32(var1)  (Q6_R_cl1_R(var1))

    /* Saturation manipulation routines */
    /************************************/
#define s32_saturate_s64(var1)  Q6_R_sat_P(var1)
#define s16_saturate_s32(var1)  (int16) Q6_R_sath_R(var1)

    /* Non-saturating instructions */

    /*get Most significant bit '1' position (i.e. count leading 0's)*/
    /****************************************************************/
#define s32_get_msb_s32(var1) Q6_R_clb_R(var1)

    /*get Most significant bit '1' position (i.e. count leading 0's)*/
    /****************************************************************/
#define s32_cl0_s32(var1)     Q6_R_cl0_R(var1)

    /*get Least significant bit '1' position (i.e. count trailing 0's)*/
    /******************************************************************/
#define s32_get_lsb_s32(var1) Q6_R_ct0_R(var1)

	/* arithmetic shifts without saturation */
    /****************/
//    int16 s16_shr_s16(int16 var1, int16 shift);             /* 1 ops */
//    int16 s16_shl_s16(int16 var1, int16 shift);             /* 1 ops */
//    int32 s32_shr_s32(int32 var1, int16 shift);             /* 2 ops */
//    int32 s32_shl_s32(int32 var1, int16 shift);             /* 2 ops */    
	
#define s16_shr_s16(var1, shift)  (int16) Q6_R_asr_RR(var1, shift)
#define s16_shl_s16(var1, shift)  (int16) Q6_R_asl_RR(var1, shift)
#define s32_shr_s32(var1, shift)  Q6_R_asr_RR(var1, shift)
#define s32_shl_s32(var1, shift)  Q6_R_asl_RR(var1, shift)

#else // #if !defined(__qdsp6__)

    /* addition */
    /************/
    int16 s16_add_s16_s16(int16 var1, int16 var2);          /* 1 ops */
    int16 s16_add_s16_s16_sat(int16 var1, int16 var2);
    int32 s32_add_s32_s32(int32 var1, int32 var2);          /* 2 ops */
    int32 s32_add_s32_s32_sat(int32 var1, int32 var2);
    int64 s64_add_s64_s64(int64 var1, int64 var2);
    int64 s64_add_s64_s32(int64 var1, int32 var2);
    int64 s64_add_s32_s32(int32 var1, int32 var2);
    int64 s64_add_s32_u32(int32 var1, uint32 var2);

    int16 s16_sub_s16_s16(int16 var1, int16 var2);          /* 1 ops */
    int16 s16_sub_s16_s16_sat(int16 var1, int16 var2);
    int32 s32_sub_s32_s32(int32 var1, int32 var2);
    int32 s32_sub_s32_s32_sat(int32 var1, int32 var2);        /* 2 ops */
    int64 s64_sub_s64_s64(int64 var1, int64 var2);
    int64 s64_sub_s64_s64(int64 var1, int64 var2);


    /* multiplication */
    /******************/
    int32 s32_mult_s16_s16(int16 var1, int16 var2);
    int32 s32_mult_s16_u16(int16 var1, uint16 var2);
    int32 s32_mult_s32_s32_rnd_sat(int32 var1, int32 var2);
    uint32 u32_mult_u16_u16(uint16 var1, uint16 var2);
    int64 s64_mult_s32_s16(int32 var1, int16 var2);
    int64 s64_mult_u32_s16(uint32 var1, int16 var2);
    int64 s64_mult_s32_u16(int32 var1, uint16 var2);
    int64 s64_mult_s32_s32(int32 var1, int32 var2);
    int64 s64_mult_s16_s16_shift(int16 var1, int16 var2, int16 shift);
    int64 s64_mult_s16_u16_shift(int16 var1, uint16 var2, int16 shift);
    int64 s64_mult_u16_u16_shift(uint16 var1, uint16 var2, int16 shift);
    int64 s64_mult_u32_s16_shift( uint32 var1, int16 var2, int16 shift);
    int64 s64_mult_s32_u16_shift( int32 var1, uint16 var2, int16 shift);
    int64 s64_mult_s32_s16_shift( int32 var1, int16 var2, int16 shift);
    int64 s64_mult_s32_s32_shift(int32 var1, int32 var2, int16 shift);
    int64 s64_mult_s32_u32_shift(int32 var1, uint32 var2, int16 shift);
    int64 s64_mult_lp_s32_s32_shift(int32 var1, int32 var2, int16 shift);
    int64 s64_mult_lp_s32_u32_shift(int32 var1, uint32 var2, int16 shift);
    int32 s32_mult_s16_s16_shift_sat(int16 var1, int16 var2);
    int32 s32_mult_s32_s16_rnd_sat(int32 var1, int16 var2);

    /* arithmetic shifts with saturation */
    /*********************/

    int16 s16_shr_s16_sat(int16 var1, int16 shift);          /* 1 ops */
    int16 s16_shl_s16_sat(int16 var1, int16 shift);          /* 1 ops */
    int16 s16_shl_s16_sat_rnd(int16 var, int16 var2);       /* 2 ops */
    int32 s32_shr_s32_sat(int32 var1, int16 shift);            /* 2 ops */
    int32 s32_shl_s32_sat(int32 var1, int16 shift);            /* 2 ops */
    int32 s32_shl_s32_rnd_sat(int32 L_var, int16 shift);    /* 3 ops */

    /* arithmetic shifts without saturation */
    /****************/
    int16 s16_shr_s16(int16 var1, int16 shift);             /* 1 ops */
    int16 s16_shl_s16(int16 var1, int16 shift);             /* 1 ops */
    int32 s32_shr_s32(int32 var1, int16 shift);             /* 2 ops */
    int32 s32_shl_s32(int32 var1, int16 shift);             /* 2 ops */
    int64 s64_shl_s64(int64 var1, int16 shift);

    /* logical operations */
    /**********************/
    int16 s16_and_s16_s16(int16 var1, int16 var2);
    int32 s32_and_s32_s32(int32 var1, int32 var2);

    /* absolute value  */
    /*******************/
    uint16 u16_abs_s16_sat(int16 var1);        /* 1 ops */
    uint32 u32_abs_s32_sat(int32 var1);        /* 3 ops */

    /* multiply accumulate  */
    /************************/

    int16 s16_mac_s32_s16_s16_sat_rnd(int32 var3,
        int16 var1, int16 var2); /* 2 op */
    int32 s32_msu_s32_s16_s16_sat(int32 var3,
        int16 var1, int16 var2);  /* 1 op */
    int16 s16_msu_s32_s16_s16_sat_rnd(int32 var3,
        int16 var1, int16 var2); /* 2 op */
    int64 s64_mac_s64_s16_s16_shift(int64 var3,
        int16 var1, int16 var2, int16 shift);
    int64 s64_mac_s64_s16_s16_shift_sat(int64 var3,
        int16 var1, int16 var2, int16 shift);
    int64 s64_mac_s64_s16_s16_s1(int64 var3,
        int16 var1, int16 var2);
    /*
    Additional operators
    */
    int32   s32_mls_s32_s16_sat(int32 var1, int16 var2);

    /* negation  */
    /*************/
    int16 s16_neg_s16_sat(int16 var1);        /* 1 ops */
    int32 s32_neg_s32_sat(int32 var1);        /* 2 ops */

    /* ALU operations */
    /******************/
    int16 s16_complement_s16(int16 var1);
    int16 s16_max_s16_s16(int16 var1, int16 var2);
    int16 s16_min_s16_s16(int16 var1, int16 var2);
    int16 s16_norm_s64(int64 var1);
    int32 s32_min_s32_s32(int32 var1, int32 var2);
    int32 s32_max_s32_s32(int32 var1, int32 var2);


    /* Accumulator manipulation */
    /****************************/
    int16 s16_extract_s32_l(int32 var1);    /* 1 ops */
    uint16 u16_extract_s32_l(int32 var1);
    int16 s16_extract_s32_h(int32 var1);    /* 1 ops */
    int16 s16_extract_s64_l(int64 var1);
    int16 s16_extract_s64_h(int64 var1);
    int16 s16_extract_s64_h_sat(int64 var1);
    int32 s32_extract_s64_l(int64 var1);
    uint32 u32_extract_s64_l(int64 var1);
    int32 L_deposit_l(int16 var1);            /* 1 ops */
    int32 s32_deposit_s16_h(int16 var1);    /* 1 ops */


    /* Round */
    /*********/
    int16 s16_round_s32_sat(int32 var1);    /* 1 ops */

    /* Normalization */
    /*****************/
    int16 s16_norm_s32(int32 var1);            /* 30 ops */
    int16 s16_norm_s16(int16 var1);            /* 15 ops */
    int32 s32_cl1_s32(int32 var1);


    /* Saturation manipulation routines */
    /************************************/
    int  clearOverflow(void);
    int  isOverflow(void);
    int  popOverflow(void);
    int  setOverflow(void);

    int16 s16_saturate_s32(int32 var1);
    int32 s32_saturate_s64(int64 var1);


    /* Non-saturating instructions */
    /*******************************/
    int32 L_add_c(int32 var1, int32 var2);    /* 2 ops */
    int32 L_sub_c(int32 var1, int32 var2);  /* 2 ops */
    int32 L_macNs(int32 var3,
        int16 var1, int16 var2);            /* 1 ops */
    int32 L_msuNs(int32 var3,
        int16 var1, int16 var2);            /* 1 ops */

#endif // #if !defined(__qdsp6__)

boolean changed(int value1, int value2);

int change_if_valid
(
    int             originalValue,          /* value to be changed           */
    int             newValue                /* new value, could be invalid   */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

// FOR BACKWARDS COMPATIBILITY of certain modules
// the follow header file is included.
#include "legacy_map.h"
#endif /*__BASICOP_H */
