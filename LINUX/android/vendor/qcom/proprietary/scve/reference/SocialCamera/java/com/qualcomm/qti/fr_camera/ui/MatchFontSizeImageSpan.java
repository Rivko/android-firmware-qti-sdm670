/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.ui;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.text.style.ImageSpan;

/**
 * This class represents a variant of {@link ImageSpan} which sizes the wrapped image according to
 * the size of the font applied to this span
 */
public class MatchFontSizeImageSpan extends ImageSpan {
    /**
     * See {@link ImageSpan#ImageSpan(Drawable)}
     */
    public MatchFontSizeImageSpan(Drawable d) {
        super(d);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Drawable, int)}
     */
    public MatchFontSizeImageSpan(Drawable d, int verticalAlignment) {
        super(d, verticalAlignment);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Drawable, String)}
     */
    public MatchFontSizeImageSpan(Drawable d, String source) {
        super(d, source);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Drawable, String, int)}
     */
    public MatchFontSizeImageSpan(Drawable d, String source, int verticalAlignment) {
        super(d, source, verticalAlignment);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Bitmap)}
     */
    @Deprecated
    @SuppressWarnings("deprecation")
    public MatchFontSizeImageSpan(Bitmap b) {
        super(b);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Bitmap, int)}
     */
    @Deprecated
    @SuppressWarnings("deprecation")
    public MatchFontSizeImageSpan(Bitmap b, int verticalAlignment) {
        super(b, verticalAlignment);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Context, Bitmap)}
     */
    public MatchFontSizeImageSpan(Context context, Bitmap b) {
        super(context, b);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Context, Bitmap, int)}
     */
    public MatchFontSizeImageSpan(Context context, Bitmap b, int verticalAlignment) {
        super(context, b, verticalAlignment);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Context, Uri)}
     */
    public MatchFontSizeImageSpan(Context context, Uri uri) {
        super(context, uri);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Context, Uri, int)}
     */
    public MatchFontSizeImageSpan(Context context, Uri uri, int verticalAlignment) {
        super(context, uri, verticalAlignment);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Context, int)}
     */
    public MatchFontSizeImageSpan(Context context, int resourceId) {
        super(context, resourceId);
    }

    /**
     * See {@link ImageSpan#ImageSpan(Context, int, int)}
     */
    public MatchFontSizeImageSpan(Context context, int resourceId, int verticalAlignment) {
        super(context, resourceId, verticalAlignment);
    }

    @Override
    public int getSize(Paint paint, CharSequence text, int start, int end,
                       Paint.FontMetricsInt fm) {
        final Paint.FontMetricsInt paintFm = paint.getFontMetricsInt();
        Drawable d = getDrawable();
        final int intrinsicWidth = d.getIntrinsicWidth();
        final int intrinsicHeight = d.getIntrinsicHeight();

        if (fm != null) {
            mergeFontMetrics(paintFm, fm);
        }

        if (intrinsicWidth == -1) {
            // No intrinsic width. Render as 0-sized.
            d.setBounds(0, 0, 0, 0);
            return 0;
        }

        // Use the paint font metrics to pick the appropriate desired render height, based on
        // vertical alignment
        final int height;
        final int verticalAlign = getVerticalAlignment();
        switch (verticalAlign) {
            case MatchFontSizeImageSpan.ALIGN_BASELINE:
                height = -paintFm.ascent + paintFm.descent;
                break;

            case MatchFontSizeImageSpan.ALIGN_BOTTOM:
                height = -paintFm.top + paintFm.bottom;
                break;

            default:
                throw new IllegalStateException("Unknown vertical alignment: " + verticalAlign);
        }

        // If we have an intrinsic height, scale the intrinsic width to maintain aspect ratio. If
        // not, maintain the intrinsic width.
        final int scaledWidth;
        if (intrinsicHeight != -1) {
            scaledWidth = (int)((float)intrinsicWidth * (float)height / (float)intrinsicHeight);
        } else {
            scaledWidth = intrinsicWidth;
        }

        d.setBounds(0, 0, scaledWidth, height);
        return scaledWidth;
    }

    // Use the current paint's font metrics to grow the provided font metrics, if applicable
    private static void mergeFontMetrics(Paint.FontMetricsInt paintFm, Paint.FontMetricsInt fm) {
        if (paintFm.ascent < fm.ascent) {
            fm.ascent = paintFm.ascent;
        }

        if (paintFm.top < fm.top) {
            fm.top = paintFm.top;
        }

        if (paintFm.descent > fm.descent) {
            fm.descent = paintFm.descent;
        }

        if (paintFm.bottom > fm.bottom) {
            fm.bottom = paintFm.bottom;
        }

        if (paintFm.leading > fm.leading) {
            fm.leading = paintFm.leading;
        }
    }
}
