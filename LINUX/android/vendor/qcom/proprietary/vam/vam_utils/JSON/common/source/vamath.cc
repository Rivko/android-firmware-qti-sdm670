/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "vamath.h"

int MatrixInverse3x3(float *mat, float *mat_inv){

    float a[3][3];
    float *mat_inv1 = mat_inv;
    int i, j;
    float determinant = 0;

    for (i = 0; i<3; i++)
        for (j = 0; j<3; j++)
            a[i][j] = mat[i * 3 + j];


    for (i = 0; i<3; i++)
        determinant = determinant + (a[0][i] * (a[1][(i + 1) % 3] * a[2][(i + 2) % 3] - a[1][(i + 2) % 3] * a[2][(i + 1) % 3]));

    for (i = 0; i<3; i++){
        for (j = 0; j<3; j++)
            mat_inv1[i * 3 + j] = ((a[(i + 1) % 3][(j + 1) % 3] * a[(i + 2) % 3][(j + 2) % 3]) - (a[(i + 1) % 3][(j + 2) % 3] * a[(i + 2) % 3][(j + 1) % 3])) / determinant;
    }

    return 0;
}
