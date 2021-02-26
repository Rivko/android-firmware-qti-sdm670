//Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
//All Rights Reserved.
//Confidential and Proprietary - Qualcomm Technologies, Inc.


#pragma version(1)
#pragma rs java_package_name(com.qualcomm.qti.qces.imageedit.segmentation)

void root(uchar4 *pixel)
{
    float4 unpackedPixel = rsUnpackColor8888(*pixel);
    unpackedPixel.a = ceil(unpackedPixel.a);
    *pixel = rsPackColorTo8888(unpackedPixel);
}
