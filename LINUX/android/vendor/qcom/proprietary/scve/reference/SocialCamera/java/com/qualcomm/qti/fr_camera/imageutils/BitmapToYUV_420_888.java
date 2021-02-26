/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.imageutils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.media.Image;
import android.media.ImageAllocationYUV_420_888;
import android.renderscript.Allocation;
import android.renderscript.Float3;
import android.renderscript.RenderScript;

import java.util.Objects;

/**
 * This class converts a {@link Bitmap} to a {@link ImageFormat#YUV_420_888} {@link Image}
 */
public class BitmapToYUV_420_888 {
    private static final Float3 RGB_TO_Y_BT_601 = new Float3(0.257f, 0.504f, 0.098f);
    private static final Float3 RGB_TO_CB_BT_601 = new Float3(-0.148f, -0.291f, 0.439f);
    private static final Float3 RGB_TO_CR_BT_601 = new Float3(0.439f, -0.368f, -0.071f);

    private static final Float3 RGB_TO_Y_BT_709 = new Float3(0.183f, 0.614f, 0.062f);
    private static final Float3 RGB_TO_CB_BT_709 = new Float3(-0.101f, -0.339f, 0.439f);
    private static final Float3 RGB_TO_CR_BT_709 = new Float3(0.439f, -0.399f, -0.040f);

    private static final int BT_601_HEIGHT_LIMIT = 576;

    private final RenderScript rs;
    private final ScriptC_BitmapToYUV_420_888 script;

    /**
     * Construct a new {@link BitmapToYUV_420_888} instance
     * @param context the active {@link Context}
     * @throws NullPointerException if context are null
     */
    public BitmapToYUV_420_888(Context context) {
        Objects.requireNonNull(context, "context may not be null");

        rs = RenderScript.create(context);
        script = new ScriptC_BitmapToYUV_420_888(rs);
    }

    /**
     * Convert the specified {@link Bitmap} to a {@link ImageFormat#YUV_420_888} {@link Image}
     * @param b the {@link Bitmap} to convert
     * @return an {@link Image} containing the {@link ImageFormat#YUV_420_888} encoded {@link Image}
     * @throws NullPointerException if b is null
     */
    public Image convert(Bitmap b) {
        Objects.requireNonNull(b, "b may not be null");

        final int width = b.getWidth();
        final int height = b.getHeight();

        if (height <= BT_601_HEIGHT_LIMIT) {
            script.set_y_transform(RGB_TO_Y_BT_601);
            script.set_cb_transform(RGB_TO_CB_BT_601);
            script.set_cr_transform(RGB_TO_CR_BT_601);
        } else {
            script.set_y_transform(RGB_TO_Y_BT_709);
            script.set_cb_transform(RGB_TO_CB_BT_709);
            script.set_cr_transform(RGB_TO_CR_BT_709);
        }

        ImageAllocationYUV_420_888 im = new ImageAllocationYUV_420_888(rs, width, height);
        Allocation[] a = im.getAllocations();

        Allocation rgb = Allocation.createFromBitmap(rs, b);

        script.set_rgb_alloc(rgb);
        script.set_y_alloc(a[0]);
        script.set_v_alloc(a[2]);

        script.invoke_setup();
        script.forEach_convertRGBToYUV(a[1]);

        im.syncAllocations();

        return im;
    }
}
