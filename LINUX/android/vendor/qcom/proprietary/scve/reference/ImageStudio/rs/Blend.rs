//Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
//All Rights Reserved.
//Confidential and Proprietary - Qualcomm Technologies, Inc.


#pragma version(1)
#pragma rs_fp_relaxed
#pragma rs java_package_name(com.qualcomm.qti.qces.imageedit.objectclone)


//2 global variables: blend_factor and a second allocation for the 2nd input image
float blend_factor;
rs_allocation a_in2;

void root(const uchar4 *v_in, uchar4 *v_out, uint32_t x, uint32_t y)
{
    //first allocation v_in is for the preciseMask
    float4 unpackedPixelA = rsUnpackColor8888(*v_in);

    //each time we do clone the blended results are allocated as a_in2
    float4 unpackedPixelB = rsUnpackColor8888(rsGetElementAt_uchar4(a_in2,x,y));

    //Let alpha be the per pixel soft segmentation value, beta be user input parameter
    //for tuning cloned object F_c and original object F_o.
    //The final result will be I = alpha * (beta* F_c + (1-beta) * F_o)
    float4 out = (1-blend_factor)*unpackedPixelA.rgba + (blend_factor)*unpackedPixelB.rgba;
    out *= unpackedPixelA.a;

    *v_out = rsPackColorTo8888(out);
}
