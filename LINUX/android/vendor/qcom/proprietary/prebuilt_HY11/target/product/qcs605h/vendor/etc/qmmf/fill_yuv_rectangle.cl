/****************************************************************************
 * Copyright (c) 2018 Qualcomm Technologies, Inc.                            *
 * All Rights Reserved.                                                      *
 * Confidential and Proprietary - Qualcomm Technologies, Inc.                *
 ****************************************************************************/

kernel void fill_yuv_rectangle(
#ifdef KERNEL_DEBUG_MODE
    __global float *debug_buf,      // 0
#endif
    __global uchar *output_frame,   // 1
               uint y_offset,       // 2
               uint nv_offset,      // 3
               uint stride,         // 4
             uchar2 y_out,          // 5
             uchar2 uv_out          // 6
)
{
  uint x = get_global_id(0);
  uint y = get_global_id(1);
#ifdef KERNEL_DEBUG_MODE
  uint dbg_idx = 1;
  bool dbg_en = false;
  if (x == 0 && y == 0) {
    dbg_en = true;
  }
#endif
  uint offset = 2 * (stride * y + x);
  *(__global uchar2*)(output_frame + y_offset + offset) = y_out;
  *(__global uchar2*)(output_frame + y_offset + offset + stride) = y_out;

  offset = stride * y + x * 2;
  *(__global uchar2*)(output_frame + nv_offset + offset) = uv_out;

#ifdef KERNEL_DEBUG_MODE
  if (dbg_en) {
    debug_buf[0] = dbg_idx - 1;
  }
#endif
}
