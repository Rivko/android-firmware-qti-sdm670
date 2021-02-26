/***************************************************************************
* Copyright (c) Date: Mon Nov 24 16:19:58 CST 2008 Qualcomm Technologies INCORPORATED 
* All Rights Reserved 
* Modified by Qualcomm Technologies INCORPORATED on Mon Nov 24 16:19:58 CST 2008 
****************************************************************************/ 
#ifndef __FFT_H__
#define __FFT_H__

/*! \file fft.h
    \brief Brief description of file
*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


    /*[*****************************************************************************]*/
    /*[  Function   : void us_sfft16x16()                                              ]*/
    /*[*****************************************************************************]*/
    /*[  Description: perform N-point FFT transform with a scaling factor of 1/N    ]*/
    /*[=============================================================================]*/
    /*[  Assumptions:                                                               ]*/
    /*[           - complex number: interleaved real/image with real part           ]*/
    /*[             at low memory address                                           ]*/
    /*[           - input  is aligned by size of the array                          ]*/
    /*[           - output is aligned by 8bytes                                     ]*/
    /*[           - N >  4                                                          ]*/
    /*[=============================================================================]*/
    /*[  Inputs     : R0 : CWord2x16 *input                                         ]*/
    /*[               R1 : int       N                                              ]*/
    /*[               R2 : CWord2x16 *twiddles                                      ]*/
    /*[               R3 : CWord2x16 *output                                        ]*/
    /*[=============================================================================]*/
    /*[  Register Usage: R0-R27, R28                                                ]*/
    /*[  Hardware Loops affected: Loop0, Loop1                                      ]*/
    /*[                                                                             ]*/
    /*[  Stack Memory Frame Allocated (in Bytes): 48                                ]*/
    /*[=============================================================================]*/
    /*[  Implementation:                                                            ]*/
    /*[           - Radix-4 DIF                                                     ]*/
    /*[           - INPUT: LOAD IN BIT-REVERSE ORDER                                ]*/
    /*[           - TWIDDLE FACTOR ARRAY: IN BIT-REVERSE ORDER & INTERLEAVED        ]*/
    /*[=============================================================================]*/
    /*[  Cycle Count:                                                               ]*/
    /*[           - N =    4^k    (7N/8 + 5)*K - (N+32)/24   +  10                  ]*/
    /*[           - N = 2*(4^K)   (7N/8 + 5)*K + (17N-16)/24 +  13                  ]*/
    /*[*****************************************************************************]*/

void us_sfft16x16(CWord2x16 *input, int N, CWord2x16 *w, CWord2x16 *output);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
