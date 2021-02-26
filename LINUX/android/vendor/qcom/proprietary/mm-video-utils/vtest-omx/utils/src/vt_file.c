/*-------------------------------------------------------------------
Copyright (c) 2013-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
--------------------------------------------------------------------*/

/*========================================================================
  Include Files
 ==========================================================================*/
#include "vt_file.h"
#include "vt_debug.h"
#include "OMX_QCOMExtns.h"
#include <stdio.h>

#define  ALIGN(val, grid) ((!(grid)) ? (val) : \
        ((((val) + (grid) - 1) / (grid)) * (grid)))

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_file_open(void **handle, char *filename, int readonly) {

    int result = 0;
    FILE *f = NULL;

    if (handle != NULL && filename != NULL && (readonly == 0 || readonly == 1)) {
        if (readonly == 1) {
            f = fopen(filename, "rb");
        } else {
            f = fopen(filename, "wb");
        }

        if (f == NULL) {
            VTEST_MSG_ERROR("Unable to open file");
            result = 1;
        }

        *handle = f;
    } else {
        VTEST_MSG_ERROR("bad param");
        result = 1;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_file_close(void *handle) {

    int result = 0;

    if (handle != NULL) {
        fclose((FILE *)handle);
    } else {
        VTEST_MSG_ERROR("handle is null");
        result = 1;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_file_read(void *handle, void *buffer, int width, int height, int config,
        unsigned int format) {

    int bytes = width;

    int result = 0, color_format = 0;

    switch (format) {
    case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m:
        color_format = COLOR_FMT_NV12;
        break;
    case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed:
        color_format = COLOR_FMT_NV12_UBWC;
        break;
    case QOMX_COLOR_Format32bitRGBA8888:
        color_format = COLOR_FMT_RGBA8888;
        break;
    case QOMX_COLOR_Format32bitRGBA8888Compressed:
        color_format = COLOR_FMT_RGBA8888_UBWC;
        break;
    default:
        break;
    }

    if (buffer != NULL) {
        if (bytes > 0) {
            if (!config) {
                if (color_format == COLOR_FMT_NV12) {
                    char *yuv = (char *)buffer;
                    int i, lscanl, lstride, cscanl, cstride;

                    lstride = VENUS_Y_STRIDE(COLOR_FMT_NV12, width);
                    lscanl = VENUS_Y_SCANLINES(COLOR_FMT_NV12, height);
                    cstride = VENUS_UV_STRIDE(COLOR_FMT_NV12, width);
                    cscanl = VENUS_UV_SCANLINES(COLOR_FMT_NV12, height);
                    for (i = 0; i < height; i++) {
                        result += (int)fread(yuv, 1, width, (FILE *)handle);
                        yuv += lstride;
                    }
                    yuv = (char *)buffer + (lscanl * lstride);
                    for (i = 0; i < ((height + 1) >> 1); i++) {
                        result += (int)fread(yuv, 1, width, (FILE *)handle);
                        yuv += cstride;
                    }
                    buffer = ((char *)buffer) + VENUS_BUFFER_SIZE(color_format, width, height);
                } else if (color_format == COLOR_FMT_NV12_UBWC || color_format == COLOR_FMT_RGBA8888_UBWC) {
                    int bytes_to_read = 0;
                    char *yuv = (char *)buffer;
                    const unsigned int extra_size = VENUS_EXTRADATA_SIZE(width, height);

                    bytes_to_read = VENUS_BUFFER_SIZE(color_format, width, height);
                    if (color_format == COLOR_FMT_NV12_UBWC) {
                    /* In addition to extradata size of 8K, another extra 8k bytes is included by
                    * Venus firmware when reporting output buffer size requirements for NV12 UBWC.
                    * This shouldn't be accounted for while reading the frame data. Hence adjusting
                    * the frame size for NV12 Linear UBWC
                    */
                        bytes_to_read -= 2 * extra_size;
                    }
                    result = (int)fread(yuv, 1, bytes_to_read, (FILE *)handle);
                    buffer = ((char *)buffer) + VENUS_BUFFER_SIZE(color_format, width, height);
                } else if (color_format == COLOR_FMT_RGBA8888) {
                    char *rgb = (char *)buffer;
                    int i, rscanl, rstride;

                    rstride = VENUS_RGB_STRIDE(color_format, width);
                    rscanl = VENUS_RGB_SCANLINES(color_format, height);
                    for (i = 0; i < height; i++) {
                        result += (int)fread(rgb, 1, width * 4, (FILE *)handle);
                        rgb += rstride;
                    }
                    rgb = (char *)buffer + (rscanl * rstride);
                    buffer = ((char *)buffer) + VENUS_BUFFER_SIZE(color_format, width, height);
                }
            } else {
                result = (int)fread(buffer, 1, width, (FILE *)handle);
            }
        } else {
            VTEST_MSG_ERROR("Bytes must be > 0");
            result = -1;
        }
    } else {
        VTEST_MSG_ERROR("Null param");
        result = -1;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_file_write(void *handle, void *buffer, int bytes) {

    int result = 0;

    if (buffer != NULL) {
        if (bytes > 0) {
            result = (int)fwrite(buffer, 1, bytes, (FILE *)handle);
        } else {
            VTEST_MSG_ERROR("Bytes must be > 0");
            result = -1;
        }
    } else {
        VTEST_MSG_ERROR("Null param");
        result = -1;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_file_seek_start(void *handle, int bytes) {

    int result = 0;

    if (bytes >= 0) {
        if (fseek((FILE *)handle, bytes, SEEK_SET) != 0) {
            VTEST_MSG_ERROR("failed to seek");
            result = 1;
        }
    }
    return result;
}
