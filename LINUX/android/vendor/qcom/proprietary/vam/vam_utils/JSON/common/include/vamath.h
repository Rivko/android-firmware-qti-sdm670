/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VAMATH_H_
#define VAMATH_H_

/**
* @brief
obtain the inverted matrix of a 3x3 matrix

@param mat a 3x3 input matrix
@param mat_inv a 3x3 output matrix
**/
int MatrixInverse3x3(float *mat, float *mat_inv);

#endif
