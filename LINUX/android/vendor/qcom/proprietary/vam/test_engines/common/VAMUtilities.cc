/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <VAM/VAMUtilities.h>
#include <VAM/vaapi_errors.h>
#include <VAM/vaapi_config.h>

#include <sys/time.h>
#include <ctime>
#include <cstdio>
#include <cerrno>

uint64_t getTimeNow() {
    struct timespec res;
    // clock_gettime(CLOCK_REALTIME, &res);
    clock_gettime(CLOCK_MONOTONIC, &res);
    return 1000000LL * (int64_t)res.tv_sec + res.tv_nsec / 1000;
}

///////////////////////////////////////////////////////
// Calibrition utility - start

int OCMatrixInverse3x3_1(float *mat, float *mat_inv) {
    float a[3][3];
    float *mat_inv1 = mat_inv;
    int i, j;
    float determinant = 0;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            a[i][j] = mat[i * 3 + j];
        }
    }


    for (i = 0; i < 3; i++) {
        determinant = determinant + (a[0][i] *
                                    (a[1][(i + 1) % 3] * a[2][(i + 2) % 3] -
                                     a[1][(i + 2) % 3] * a[2][(i + 1) % 3]));
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            mat_inv1[i * 3 + j] = ((a[(i + 1) % 3][(j + 1) % 3] *
                                    a[(i + 2) % 3][(j + 2) % 3]) -
                                    (a[(i + 1) % 3][(j + 2) % 3] *
                                     a[(i + 2) % 3][(j + 1) % 3])) /
                                     determinant;
        }
    }

    return 0;
}

int getCalibratedParams(const vaapi_scene_calibration *caliInfo,
                        int64_t *params) {
    float sx2 = 0;
    float sxy = 0;
    float sx = 0;
    float sy2 = 0;
    float sy = 0;
    float sxz = 0;
    float syz = 0;
    float sz = 0;
    float fixed_height_ = 1;
    for (unsigned int i = 0; i < caliInfo->object_size; i++) {
        vaapi_scene_calibration_object object = caliInfo->objects[i];
        float height = object.pos.height * fixed_height_ /
                            object.physical_height;
        sx2 += object.pos.x * object.pos.x;
        sxy += object.pos.x * (object.pos.y + object.pos.height);
        sx += object.pos.x;
        sy2 += (object.pos.y + object.pos.height) *
                (object.pos.y + object.pos.height);
        sy += (object.pos.y + object.pos.height);
        sxz += object.pos.x * height;
        syz += (object.pos.y + object.pos.height) * height;
        sz += height;
    }
    float h_mat[9] = { sx2, sxy, sx, sxy, sy2, sy, sx, sy,
                        static_cast<float>(caliInfo->object_size) };
    float h_inv_mat[9];
    float p[3] = { sxz, syz, sz };
    OCMatrixInverse3x3_1(reinterpret_cast<float *>(h_mat),
                         reinterpret_cast<float *>(h_inv_mat));
    float a = 0;
    float b = 0;
    float c = 0;
    float * abc = reinterpret_cast<float *>(params);
    for (int i = 0; i < 3; i++) {
        a += h_inv_mat[i] * p[i];
        b += h_inv_mat[3 + i] * p[i];
        c += h_inv_mat[6 + i] * p[i];
    }
    abc[0] = a;
    abc[1] = b;
    abc[2] = c;
    return 0;
}
// Calibrition utility - end
///////////////////////////////////////////////////////
