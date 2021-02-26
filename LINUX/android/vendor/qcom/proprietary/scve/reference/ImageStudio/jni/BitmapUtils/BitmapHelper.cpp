/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/bitmap.h>
#include <vector>
#include <utility>

typedef struct {
    int x;
    int y;
    int dir; // -1 for left, +1 for right
} segment_desc_t;

// Flood-fill a region of an 8-bit bitmap
static void floodFill(const AndroidBitmapInfo &info, uint8_t *p, int startX, int startY,
        uint8_t match, uint8_t fill)
{
    std::vector<segment_desc_t> segments;
    segments.push_back(segment_desc_t { startX, startY, -1 });
    segments.push_back(segment_desc_t { startX + 1, startY, 1 });

    while (!segments.empty()) {
        bool aboveAdded = false;
        bool belowAdded = false;
        segment_desc_t seg = segments.back();
        segments.pop_back();

        for (int c = seg.x; c >= 0 && c < (int)info.width && p[seg.y * info.stride + c] == match; c += seg.dir) {
            p[seg.y * info.stride + c] = fill;

            if (seg.y > 0 && (p[(seg.y - 1) * info.stride + c] == match) != aboveAdded) {
               if (!aboveAdded) {
                   aboveAdded = true;
                   segments.push_back(segment_desc_t { c, seg.y - 1, -1 });
                   segments.push_back(segment_desc_t { c + 1, seg.y - 1, 1 } );
               } else {
                   aboveAdded = false;
               }
            }

            if (seg.y < ((int)info.height - 1) && (p[(seg.y + 1) * info.stride + c] == match) != belowAdded) {
               if (!belowAdded) {
                   belowAdded = true;
                   segments.push_back(segment_desc_t { c, seg.y + 1, -1 });
                   segments.push_back(segment_desc_t { c + 1, seg.y + 1, 1 } );
               } else {
                   belowAdded = false;
               }
            }
        }
    }
}

#ifdef __cplusplus
extern "C" {
#endif

#define MASK_FILL_SHAPE_ALPHA 255L
#define MASK_FILL_BACKGROUND_ALPHA 128L

/*
 * Class:     com_qualcomm_qti_qces_imageedit_removal_RemovalModel
 * Method:    nFloodFillPath
 * Signature: (Landroid/graphics/Bitmap;)V
 */

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalModel_nFloodFillShape
  (JNIEnv *env, jclass cls, jobject b)
{
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, b, &info);

    uint8_t *p;
    AndroidBitmap_lockPixels(env, b, (void **)&p);

    // Walk the edges of the bitmap, and start a flood-fill at any pixels that are
    // MASK_FILL_BACKGROUND_ALPHA.
    for (int x = 0; x < (int)info.width; x++) {
        if (p[0 * info.stride + x] == MASK_FILL_BACKGROUND_ALPHA) {
            floodFill(info, p, x, 0, MASK_FILL_BACKGROUND_ALPHA, 0);
        }
        if (p[(info.height - 1) * info.stride + x] == MASK_FILL_BACKGROUND_ALPHA) {
            floodFill(info, p, x, info.height - 1, MASK_FILL_BACKGROUND_ALPHA, 0);
        }
    }
    for (int y = 0; y < (int)info.height; y++) {
        if (p[y * info.stride + 0] == MASK_FILL_BACKGROUND_ALPHA) {
            floodFill(info, p, 0, y, MASK_FILL_BACKGROUND_ALPHA, 0);
        }
        if (p[y * info.stride + (info.width - 1)] == MASK_FILL_BACKGROUND_ALPHA) {
            floodFill(info, p, info.width - 1, y, MASK_FILL_BACKGROUND_ALPHA, 0);
        }
    }

    // Now, any pixels left that are MASK_FILL_BACKGROUND_ALPHA are contained in the shape.
    // Change them to MASK_FILL_SHAPE_ALPHA.
    for (int y = 0; y < (int)info.height; y++) {
        for (int x = 0; x < (int)info.width; x++) {
            if (p[y * info.stride + x] == MASK_FILL_BACKGROUND_ALPHA) {
                p[y * info.stride + x] = MASK_FILL_SHAPE_ALPHA;
            }
        }
    }

    AndroidBitmap_unlockPixels(env, b);
}

#ifdef __cplusplus
}
#endif

