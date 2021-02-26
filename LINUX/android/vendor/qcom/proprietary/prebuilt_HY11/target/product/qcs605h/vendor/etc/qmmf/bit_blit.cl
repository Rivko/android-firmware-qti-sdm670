/****************************************************************************
 * Copyright (c) 2018 Qualcomm Technologies, Inc.                            *
 * All Rights Reserved.                                                      *
 * Confidential and Proprietary - Qualcomm Technologies, Inc.                *
 ****************************************************************************/

__kernel void bit_blit(
#ifdef KERNEL_DEBUG_MODE
    __global float *debug_buf,  // 0
#endif
    __global unsigned char *overlayYUVimg,  // 1
    __global unsigned char *aplhaMask,      // 2
    __global unsigned char *dstYUVimg,      // 3
    int offsetUVOverlay,                    // 4
    int offsetUVDst,                        // 5
    int offsetMaskY,                        // 6
    int offsetMaskUV,                       // 7
    int overlayStride,                      // 8
    int dstStride                           // 9

    ) {
  // get horizontal and vertical limitations
  int XSize = get_global_size(0);
  int YSize = get_global_size(1);

  uint offsetSrc, offsetDst;

  // Get output dimensions
  int srcX = get_global_id(0);
  int srcY = get_global_id(1);

#ifdef KERNEL_DEBUG_MODE
  uint dbg_idx = 1;
  bool dbg_en = false;
  if (srcX == ((908 - 700) / 2) && srcY == ((568 - 500) / 2)) {
    dbg_en = true;
  }
#endif

  __global unsigned char *srcUVimg = overlayYUVimg + offsetUVOverlay;
  __global unsigned char *dstUVimg = dstYUVimg + offsetUVDst;

  float2 srcData, maskData, OneMinusmaskData, dstData, fuseData;
  float uvMask;

  offsetSrc = srcY * overlayStride + srcX;
  offsetDst = srcY * dstStride + srcX;

  srcData = convert_float2(vload2(offsetSrc, overlayYUVimg));
  maskData = convert_float2(vload2(offsetSrc, aplhaMask));
  maskData = maskData / 256.0f;
  uvMask = maskData.s0 + maskData.s1;
  OneMinusmaskData = 1.0f - maskData;
  dstData = convert_float2(vload2(offsetDst + offsetMaskY / 2, dstYUVimg));
  fuseData = (srcData * maskData + dstData * OneMinusmaskData);
  vstore2(convert_uchar2(fuseData), offsetDst + offsetMaskY / 2, dstYUVimg);

  srcData =
      convert_float2(vload2(offsetSrc + overlayStride / 2, overlayYUVimg));
  maskData = convert_float2(vload2(offsetSrc + overlayStride / 2, aplhaMask));
  maskData = maskData / 256.0f;
  uvMask = uvMask + maskData.s0 + maskData.s1;
  OneMinusmaskData = 1.0f - maskData;
  dstData = convert_float2(
      vload2(offsetDst + (offsetMaskY + dstStride) / 2, dstYUVimg));
  fuseData = (srcData * maskData + dstData * OneMinusmaskData) / 1.0f;
  vstore2(convert_uchar2(fuseData), offsetDst + (offsetMaskY + dstStride) / 2,
          dstYUVimg);

  offsetSrc = srcY * overlayStride + 2 * srcX;
  offsetDst = srcY * dstStride + 2 * srcX;
  uvMask = uvMask / 4.0f;
  srcData = convert_float2(vload2(offsetSrc / 2, srcUVimg));
  dstData = convert_float2(vload2((offsetMaskUV + offsetDst) / 2, dstUVimg));
  fuseData = (srcData * uvMask + dstData * (1.0f - uvMask));
  vstore2(convert_uchar2(fuseData), (offsetMaskUV + offsetDst) / 2, dstUVimg);

#ifdef KERNEL_DEBUG_MODE
  if (dbg_en) {
    debug_buf[0] = dbg_idx - 1;
  }
#endif
}