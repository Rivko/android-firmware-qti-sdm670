/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.orbbec;

public class OBCameraParams {

    /*Native orbbec/include/cmd.hpp structure

    typedef struct OBCameraParams
    {
        float l_intr_p[4];//[fx,fy,cx,cy]
        float r_intr_p[4];//[fx,fy,cx,cy]
        float r2l_r[9];//[r00,r01,r02;r10,r11,r12;r20,r21,r22]
        float r2l_t[3];//[t1,t2,t3]
        float l_k[5];//[k1,k2,k3,p1,p2]
        float r_k[5];//[k1,k2,k3,p1,p2]
        int is_m;
    }OBCameraParams;
    */
    float l_intr_p[];//[fx,fy,cx,cy]
    float r_intr_p[];//[fx,fy,cx,cy]
    float r2l_r[];//[r00,r01,r02;r10,r11,r12;r20,r21,r22]
    float r2l_t[];//[t1,t2,t3]
    float l_k[];//[k1,k2,k3,p1,p2]
    float r_k[];//[k1,k2,k3,p1,p2]
    int is_m;

    public OBCameraParams(float[] l_intr_p, float[] r_intr_p, float[] r2l_r, float[] r2l_t, float[] l_k, float[] r_k, int is_m) {
        this.l_intr_p = l_intr_p;
        this.r_intr_p = r_intr_p;
        this.r2l_r = r2l_r;
        this.r2l_t = r2l_t;
        this.l_k = l_k;
        this.r_k = r_k;
        this.is_m = is_m;
    }

}
