/***************************************************************************
* Copyright (c) Date: Mon Nov 24 16:25:59 CST 2008 Qualcomm Technologies INCORPORATED 
* All Rights Reserved 
* Modified by Qualcomm Technologies INCORPORATED on Mon Nov 24 16:25:59 CST 2008 
****************************************************************************/ 


/*! \file dsp_libs.h
    \brief Brief description of file
*/

typedef struct
{
  Word32 result;    /*!< Result value */
  Word32 scale;     /*!< Scale factor */
} us_result_scale_t;


/*! 
Approximates inversion of a 32-bit positive number

\param x number to be inverted

\details
\return
  64-bit integer where the high 32-bit is shift factor
  and the low 32-bit is Q-shifted inverse

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 6 

\b Notes
 - Approximation done with a linearly interpolated lookup table.

 - Data format 

\par
   IF input is in Qn and output in Qm, then
   m = 30 - SF - n .
\par
   For example, 
\par
   input is 0x0F000000 in Q26, i.e., 3.75
\par
   return values are
   result = 0x44730000, SF = -28
\par
   Thus, m = 30-(-28)-26 = 32, i.e.,
   inversion is  (0x44730000)/2^32 = 0.266464

*/

Word64 us_dsplib_approx_invert(Word32 x);



/*! 
Approximates inversion of a 32-bit positive number(fast version)

\param x number to be inverted

\details
\return
  64-bit integer where the high 32-bit is shift factor
  and the low 32-bit is Q-shifted inverse

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 5 

\b Notes
 - Modified version of dsplib_approx_invert

 - Data format (ref. to dsplib_approx_invert) 

*/

us_result_scale_t us_dsplib_approx_invert_simple(Word32 x);



/*! 
Approximate division

\param numer numerator
\param denom denominator

\details

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 7

\b Notes
 - Inversion is approximated by linear interpolation of LUT.

 - Data format 

\par
   IF inputs are in Q0, then output is in Q(-SF)
\par
   For example, 
\par
   numer = 5297, denom = 13555
\par
   return values are
   result = 839214304, SF = -31
\par
   Thus, division value is
     839214304/2^31 = 0.3907896

*/

Word64 us_dsplib_approx_divide(Word32 numer,Word32 denom);



/*! 
Inverts a 32-bit positive number using Newton's method 

\param x number to be inverted

\details

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 18

\b Notes

 - Data format 

\par
   IF input is in Q0, then output is in Q(45-SF)
\par
   For example, 
\par
   input is 14767,
\par
   return values are
   result = 18179, SF = 17
\par
   Thus, inversion value is
     18179/2^(45-17) = 6.7722 x 10^-5
*/

Word64 us_dsplib_newtons_invert(Word32 x);



/*! 
Inversion of a 16-bit positive number

\param x number to be inverted

\details
\return
  64-bit integer where the high 32-bit is shift factor
  and the low 16-bit is Q-shifted inverse

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 18 

\b Notes
 - Inversion has 15-bit full precision.

 - Data format 

\par
   IF input is in Qn and output in Qm, then
   m = SF - n .
\par
   For example, 
\par
   input is 0x0F00 in Q10, i.e., 3.75
\par
   return values are
   result = 17476, SF = 26 
\par
   Thus, m = 26 - 10 = 16, i.e.,
   inversion is  17476/2^16 = 0.2666626

*/

us_result_scale_t us_dsplib_invert(Word16 x);		


/*! 
Approximates sqrt(x)

\param x input number in Q0
\param rnd_fac rounding factor


\details

\return
sqrt(x) in Q16

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 8

\b Notes
 - ref. to QDSP4000 library for implementation

*/

Word32 dsplib_sqrt_lut(Word32 x, Word32 rnd_fac);



/*! 
Approximates 1/sqrt(x)

\param x input number in Q0
\param rnd_fac rounding factor


\details

\return
1/sqrt(x) in Q30

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 8

\b Notes
 - ref. to QDSP4000 library for implementation

*/

Word32 dsplib_inv_sqrt_lut(Word32 x, Word32 rnd_fac);


/*! 
Approximates 10*log10(x)

\param x input 

\details

\return
10log10(x) in unsigned Q23 

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 7

\b Notes
 - ref. to QDSP4000 library for implementation

*/

Word32 dsplib_log10(Word32 x);



/*! 
Approximates 10^x for x in [-1, 1]

\param x input in Q26

\details

\return
10^x in Q15

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 8

\b Notes
 - ref. to QDSP4000 library for implementation

*/

Word32 dsplib_exp10(Word32 x);



/*! 
Approximates sin(2*PI*x)

\param x input in Q24

\details

\return
sin(2*PI*x) in Q31

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 6 (x<0)
 - 5 (x >=0)

\b Notes
 - Use 512-entry LUT with interpolation for approximation

*/

Word32 us_dsplib_sin(Word32 x);


/*! 
Approximates cos(2*PI*x)

\param x input in Q24

\details

\return
cos(2*PI*x) in Q31

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 6 (x<0)
 - 5 (x >=0)

\b Notes
 - Use 512-entry LUT with interpolation for approximation

*/

Word32 us_dsplib_cos(Word32 x);



/*! 
Approximates arctan(x)/PI for x in [-1,1)

\param x input in Q15

\details

\return
arctan(x)/PI in Q31

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - 4

\b Notes
 - Use polynomial approximation with MSE

*/

Word32 us_dsplib_atan(Word16 x);




/*! 
Approximate unsigned-inverse using linear 8-entry lookup table.

\param input unsigned number to be inverted

\details

\return
    A result/scale pair corresponding to 1/input.
    If input is 0, the output returned is result=0xFFFFFFFF, scale=0.
    Valid input range is 0-0xFFFFFFFF

\b Assembly \b Assumptions
 - LUT aligned by 32-bytes

\b Cycle-Count
 - The optimized assembly function is 5 cycles.

\b Notes
 -  Maximum absolute relative error is 1.8e-3.

 - Data format 

\par
   With the input in Qi, the output is Qo=32-Qi. Right-shifting the result by
   the scale, converts the result to Qo.
\par
   For example, 
\par
   input: 3.5625 (57 in Q4)
\par
   output: scale=4, result=0x47dd8000=1205698560 (Q30)
\par
   Thus, 
   shift right by 4 => 0x47dd800=75356160 (Q28=32-4)=0.280724

*/

extern us_result_scale_t approx_uinv_linlut8(UWord32 input);






/*! 
Approximate unsigned-inverse using cubic polynomial approximation.

\param input unsigned number to be inverted

\details


\return
    A result/scale pair corresponding to 1/input.
    If input is 0, the output returned is result=0xFFFFFFFF, scale=0.
    Valid input range is 0-0xFFFFFFFF.

\b Assembly \b Assumptions
 - LUT aligned by 64-bytes

\b Cycle-Count
 - The optimized assembly function is 7 cycles.

\b Notes
 - Maximum absolute relative error is 1.525879e-5.

 - Data format 

\par
    With the input in Qi, the output is Qo=32-Qi. Right-shifting the result by
    the scale, converts the result to Qo.
\par
   For example, 
\par
   input: 3.5625 (57 in Q4)
\par
   output: scale=4, result=0x47dc5800=1205622784 (Q30)
\par
   Thus, 
   shift right by 4 => 0x47dc580=75351424 (Q28=32-4)=0.280706

*/

extern us_result_scale_t approx_uinv_cubelut8(UWord32 input);





/*! 
Approximate unsigned-inverse using Newton-Raphson iterative method.

\param input unsigned number to be inverted
\param iter number of iterations to be used. The iteration count must be in [1,3].

\details

\return
    A result/scale pair corresponding to 1/input.
    If input is 0, the output returned is result=0xFFFFFFFF, scale=0.

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - The optimized assembly function is 4+2*(iterations) cycles.

\b Notes
 - Maximum absolute relative error : 7.3e-3(iter=1), 6e-5(iter=2)  5.5e-9(iter=3)
 - Quantization noise makes error worse at more than 3 iterations.

 - Data format 

\par
    With the input in Qi, the output is Qo=32-Qi. Right-shifting the result by
    the scale, converts the result to Qo.
\par
   For example, 
\par
   input: 3.5625 (57 in Q4)
\par
   output: scale=4, result=0x47dc11f8=1205604856 (Q30)
\par
   Thus, 
   shift right by 4 => 0x47dc120=75350304 (Q28=32-4)=0.280702

*/

extern us_result_scale_t approx_uinv_newt(UWord32 input, UWord32 iter);






/*! 
Approximate unsigned-inverse using Taylor's series expansion.

\param input unsigned number to be inverted
\param hi_pow the degree of the Taylor polynomial to be used (must be in [3,8]). 

\details

\return
  A result/scale pair corresponding to 1/input.
  If input is 0, the output returned is result=0xFFFFFFFF, scale=0.


\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - The optimized assembly function is 6+(hi_pow) cycles.

\b Notes

 - Max absolute relative errors are: 1.6-4, 1.8e-5, 2.0e-6, 2.5e-7, 3.5e-8, 3.5e-9
   for highest power = 3,...,8 respectively.

 - Competitive only for hi_pow=4 but 2 cycles more than newt_2iter and 3 more 
   than cube8fit

 - Data format 

\par
   With the input in Qi, the output is Qo=32-Qi. Right-shifting the result by
   the scale, converts the result to Qo.
\par
   For example, 
\par
   input: 3.5625 (57 in Q4)
\par
   output: scale=5, result=0x8fb8235d=2411209565 (Q31)
\par
   Thus, 
   result shift right by 5 => 0x47dc11b=75350299 (Q28=32-4)=0.280702
   (NOTE: The result is twice the magnitude, compared to other 
           algorithms and so the scale is also 1 more)

*/

extern us_result_scale_t approx_uinv_tylr(UWord32 input, UWord32 hi_pow);






/*! 
Approximate dB-to-linear conversion using linear 8-segment lookup table.

\param input number in dB (in Q24) to be converted

\details

\return
  A result/scale pair corresponding to 10^(input/10).

\b Assembly \b Assumptions
 - LUT aligned by 32-bytes.

\b Cycle-Count
 - The optimized assembly function is 5 cycles.

\b Notes
 - Maximum absolute relative error is 5.8121e-04

 - Data format 

\par
   The scale_factor is signed and lies in the range [-32,31].
   The scale_factor corresponds to the right shift amount in 
   a 64-bit shift operation on the result & interpreting the 
   resultant 64 bits as a Q32 number. For other output Q-factors,
   the scale_factor is to be adjusted appropriately (ensure no 
   overflow/underflow).
   For e.g. Q16 result can be obtained w/o underflow for inputs in range
   -48.1647dB:127dB with right shift amount given by:
   right shift = scale_factor + 16.

\par
   Considering only the positive input dB range upto 96.32dB, no shift is
   required if we treat the result as floating point. The Qo is given by:
   Qo = 32 + scale_factor..Qo is in the range [0,31]

\par
   Valid input range is Q24 numbers corresponding to real values within
   +/-96.32 (dB range corresponding to linear 32-bit no. range). Inputs 
   between 96.33dB:127dB or -128dB:-96.33dB can be used, but the right 
   shift for the result in 64 bits, now interpreted as Q21, is given by:
   right shift =scale_factor + sign(scale_factor)*11

\par
   For example, 
\par
   input is -16.00dB =0xf0000000 in Q24
\par
   return (scale, result) = (5, 0xcddfb7c0)   
\par
   Thus, Qo = 32 + 5 = 37, and the value is (0xcddfb7c0)*2^-37 = 2.513109e-02

*/

extern us_result_scale_t approx_dB_to_ulin_linlut8( Word32 input );






/*! 
Approximate dB-to-linear conversion using a single cubic polynomial fit.

\param input number in dB to be converted

\details
   
\return
  A result/scale pair corresponding to 10^(input/10).
  

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - The optimized assembly function is 6 cycles.

\b Notes
 - Maximum absolute relative error is 1.3463e-04

 - Data format 

\par
   Please refer to approx_db_to_ulin_linlut8().

*/

extern us_result_scale_t approx_dB_to_ulin_cubefit( Word32 input );







/*! 
Approximate dB-to-linear conversion using 2nd degree 8-segment lookup table.

\param x number to be inverted

\details

\return
  A result/scale pair corresponding to 10^(input/10).

\b Assembly \b Assumptions
 - LUT is aligned by 64-bytes

\b Cycle-Count
 - The optimized assembly function is 7 cycles.

\b Notes
 - Maximum absolute relative error is 4.4531e-06

 - Data format 

\par
   Please refer to approx_db_to_ulin_linlut8().

*/

extern us_result_scale_t approx_dB_to_ulin_quadlut8( Word32 input );









/*! 
Approximate linear range to dB conversion using linear 8-segment lookup table.

\param input number to be converted into dB
\param Qi Q-factor of the input number (Q-factor must be in [0,32]).

\details

\return
  A result corresponding to 10*log10(input).
  If input is 0, the output returned is 0x80000000=-256dB in Q24.

\b Assembly \b Assumptions
 - LUT aligned by 32-bytes

\b Cycle-Count
 - The optimized assembly function is 6 cycles.

\b Notes
 -  Maximum absolute error is less than 0.004012 dB

 - Data format 

\par
    The output Q-factor is fixed at Q24. Valid output is in the range
    +/-96.32dB.

\par
   IF input is in Qn and output in Qm, then
   m = 30 - SF - n .
\par
   For example, 
\par
    input: 0xffffffff..Q0 = 4.29e+09,  output: 0x6054bd9b = 96.33102dB (Q24)
\par
    input: 0xffffffff..Q32= 1.00e+00,  output: 0x00005cfb = 0.00142dB (Q24)
*/

extern Word32 approx_ulin_to_dB_linlut8( UWord32 input, Word32 Qi);

       





/*! 
Approximate linear range to dB conversion using cubic 32-segment lookup table.

\param input number to be converted into dB
\param Qi Q-factor of the input number (Q-factor must be in [0,32]).

\details


\return
  A result corresponding to 10*log10(input).
  If input is 0, the output returned is 0x80000000=-256dB in Q24.

\b Assembly \b Assumptions
 - LUT aligned by 64-bytes

\b Cycle-Count
 - The optimized assembly function is 7 cycles.

\b Notes
 - Maximum absolute error is less than 0.0000483 dB

 - Data format 

\par
    The output Q-factor is fixed at Q24. Valid output is in the range
    +/-96.32dB.

\par
   IF input is in Qn and output in Qm, then
   m = 30 - SF - n .
\par
   For example, 
\par
    input: 0xffffffff..Q0 = 4.29e+09,  output: 0x60546034 = 96.32959 dB (Q24)
\par
    input: 0xffffffff..Q32= 1.00e+00,  output: 0xffffff94 = -0.00001 dB (Q24)
*/

extern Word32 approx_ulin_to_dB_cubelut8( UWord32 input, Word32 Qi);






/*! 
Approximate atan2(y,x)/pi using 7th degree polynomial approximation.

\param x x-coordinate of the input point
\param y y-coordinate of the input point 

\details
   
\return
   A result corresponding to atan2(y,x)/pi.
   If the input pair is (0,0), the output returned is 0.

\b Assembly \b Assumptions
 - None

\b Cycle-Count
 - The optimized assembly function is 22 cycles.

\b Notes
 - Maximum absolute relative error is 3.9419e-007.

 - Data format 

\par
   The output is in Q31.

\par
   For example, 
\par
   input:y=-2147483648 x=-2147483648
\par
   output: -1610611935 = 0xA0000321 =-7.499996e-01..Q31 (pi * -.75)

*/

extern Word32 approx_atan2_poly7( Word32 y, Word32 x);



