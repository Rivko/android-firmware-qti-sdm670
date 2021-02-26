/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.effects;

import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.Deque;
import java.util.HashMap;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicBlur;

import com.qualcomm.qti.qces.imageedit.effects.EffectsController.HintType;

/*package*/class EffectsModel {

    public HashMap<HintType, Boolean> hintShown;
    public boolean hintActive;
    public boolean helpActive;
    public boolean enable_help_popup;

    public enum Layer {
        BACKGROUND(0), FOREGROUND(1);

        private final int index;
        private final EffectRecord defaultEffect;

        private Layer(int index) {
            this.index = index;
            this.defaultEffect = new EffectRecord(this, IdentityEffect.DEFAULT, null);
        }
    }

    //Y=0.299R+0.587G+0.114B
    private float[] y = {0.299f,0.587f,0.114f};

    /* [a, b, c, d, e,
        f, g, h, i, j,
        k, l, m, n, o,
        p, q, r, s, t ]

        R� = a*R + b*G + c*B + d*A + e;
        G� = f*R + g*G + h*B + i*A + j;
        B� = k*R + l*G + m*B + n*A + o;
        A� = p*R + q*G + r*B + s*A + t  */
   float[][] rgbRecoloringPattern = new float[][]{
            //RGB to RBG
                    {1,0,0,0,0,
                    0,0,1,0,0,
                    0,1,0,0,0,
                    0,0,0,1,0},
           //RGB to BRG
                    {0,0,1,0,0,
                    1,0,0,0,0,
                    0,1,0,0,0,
                    0,0,0,1,0},
           //RGB to BGR
                    {0,0,1,0,0,
                    0,1,0,0,0,
                    1,0,0,0,0,
                    0,0,0,1,0},
           //RGB to GRB
                    {0,1,0,0,0,
                    1,0,0,0,0,
                    0,0,1,0,0,
                    0,0,0,1,0},
           //RGB to GBR
                  {0,1,0,0,0,
                   0,0,1,0,0,
                   1,0,0,0,0,
                   0,0,0,1,0},

          // RGB to 00Y
                   {0,0,0,0,0,
                    0,0,0,0,0,
                    y[0],y[1],y[2],0,0,
                    0,0,0,1,0},
          // RGB to 0Y0
                    {0,0,0,0,0,
                    y[0],y[1],y[2],0,0,
                    0,0,0,0,0,
                    0,0,0,1,0},
          // RGB to Y00
                    {y[0],y[1],y[2],0,0,
                    0,0,0,0,0,
                    0,0,0,0,0,
                    0,0,0,1,0},
          // RGB to YY0
                    {y[0],y[1],y[2],0,0,
                    y[0],y[1],y[2],0,0,
                    0,0,0,0,0,
                    0,0,0,1,0},
          // RGB to Y0Y
                    {y[0],y[1],y[2],0,0,
                    0,0,0,0,0,
                    y[0],y[1],y[2],0,0,
                    0,0,0,1,0},
          // RGB to 0YY
                    {0,0,0,0,0,
                    y[0],y[1],y[2],0,0,
                    y[0],y[1],y[2],0,0,
                    0,0,0,1,0},
    };

    /**
     * The supertype of all types of {@link Effect}s
     */
    public static interface Effect extends Parcelable {

    };

    /**
     * {@link Effect}s is an effect that copies the input to the output
     */
    public enum IdentityEffect implements Effect {
        DEFAULT;

        // was complaining about having a static class
        public static final Parcelable.Creator<Effect> CREATOR = new Parcelable.Creator<Effect>() {
            public Effect createFromParcel(Parcel in) {
                return IdentityEffect.DEFAULT;
            }

            public Effect[] newArray(int size) {
                return new Effect[size];
            }
        };

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
        }
    };

    /**
     * {@link Effect}s that operate on the color channels of the image
     */
    public enum ColorEffect implements Effect {
        BLUR(0), DARKEN(1), LIGHTEN(2), GRAYSCALE(3), SEPIA(4), RECOLOR(5);

        public static final Parcelable.Creator<Effect> CREATOR = new Parcelable.Creator<Effect>() {
            public Effect createFromParcel(Parcel in) {
                return ColorEffect.values()[(in.readInt())];
            }

            public Effect[] newArray(int size) {
                return new Effect[size];
            }
        };

        private final int index;

        private ColorEffect(int index) {
            this.index = index;
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(this.index);
        }
    }

    /**
     * {@link Effect}s that operate via an input {@link Bitmap}
     */
    public enum BitmapEffect implements Effect {
        REPLACE(0);

        public static final Parcelable.Creator<Effect> CREATOR = new Parcelable.Creator<Effect>() {
            public Effect createFromParcel(Parcel in) {
                return BitmapEffect.values()[(in.readInt())];
            }

            public Effect[] newArray(int size) {
                return new Effect[size];
            }
        };

        private final int index;

        private BitmapEffect(int index) {
            this.index = index;
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(this.index);
        }
    }

    private static final float LIGHTEN = 128f;
    private static final float DARKEN = -128f;
    private static final int DEFAULT_LIGHTEN_DARKEN_BLUR_INTENSITY = 50;
    private static final int DEFAULT_COLOR_EFFECT_INTENSITY = 100;

    private final RenderScript renderscript;
    private final EffectsActivity activity;
    private final Bitmap[] inputLayer = new Bitmap[2];
    private final Bitmap[] effectLayer = new Bitmap[2];
    private final Allocation[] inputAllocation = new Allocation[2];
    private final Allocation[] effectAllocation = new Allocation[2];
    private Deque<EffectRecord> undoStack = new ArrayDeque<EffectRecord>();
    private Deque<EffectRecord> redoStack = new ArrayDeque<EffectRecord>();
    private EffectRecord[] effect = new EffectRecord[2];

    public EffectsModel(EffectsActivity activity) {
        this.activity = activity;
        renderscript = RenderScript.create(activity);

        // initialize the effect layers
        effect[Layer.BACKGROUND.index] = Layer.BACKGROUND.defaultEffect;
        effect[Layer.FOREGROUND.index] = Layer.FOREGROUND.defaultEffect;
        undoStack.clear();
        redoStack.clear();

        hintShown = new HashMap<HintType, Boolean>();
        hintShown.put(HintType.SEPARATE_AWARE, false);
        hintActive = false;
        helpActive = false;
    }

    public void saveState(Bundle state) {

        // need to save all EffectRecord
        EffectRecord[] undoStackArray = null;
        EffectRecord[] redoStackArray = null;
        EffectRecord[] effectArray = null;

        if (undoStack.size() > 0) {
            undoStackArray = Arrays.copyOf(undoStack.toArray(), undoStack.toArray().length,
                    EffectRecord[].class);
        }
        if (redoStack.size() > 0) {
            redoStackArray = Arrays.copyOf(redoStack.toArray(), redoStack.toArray().length,
                    EffectRecord[].class);
        }
        if (effect.length > 0) {
            effectArray = effect;
        }

        state.putParcelableArray("undoStack", undoStackArray);
        state.putParcelableArray("redoStack", redoStackArray);
        state.putParcelableArray("effect", effectArray);
        state.putBoolean("hintActive", hintActive);
        state.putSerializable("hintMap", hintShown);
        state.putBoolean("helpActive", helpActive);
    }

    @SuppressWarnings("unchecked")
    public void restoreState(Bundle state) {

        EffectRecord[] undoStackArray = null;
        EffectRecord[] redoStackArray = null;
        EffectRecord[] effectArray = null;

        Parcelable[] effectRecordArray = state.getParcelableArray("undoStack");
        // can't cast directly from Parcelable to EffectRecord so need to a Java style memcpy
        if (effectRecordArray != null) {
            undoStackArray = Arrays.copyOf(effectRecordArray, effectRecordArray.length,
                    EffectRecord[].class);
            undoStack = convertArrayToArrayDeque(undoStackArray);
        }

        effectRecordArray = state.getParcelableArray("redoStack");
        // can't cast directly from Parcelable to EffectRecord so need to a Java style memcpy
        if (effectRecordArray != null) {
            redoStackArray = Arrays.copyOf(effectRecordArray, effectRecordArray.length,
                    EffectRecord[].class);
            redoStack = convertArrayToArrayDeque(redoStackArray);
        }

        effectRecordArray = state.getParcelableArray("effect");
        // can't cast directly from Parcelable to EffectRecord so need to a Java style memcpy
        if (effectRecordArray != null) {
            effectArray = Arrays.copyOf(effectRecordArray, effectRecordArray.length,
                    EffectRecord[].class);
            effect = effectArray;
        }

        hintActive = state.getBoolean("hintActive");
        hintShown = (HashMap<HintType, Boolean>)state.getSerializable("hintMap");
        helpActive = state.getBoolean("helpActive");
    }

    private Deque<EffectRecord> convertArrayToArrayDeque(EffectRecord[] stackArray) {
        if (stackArray == null || stackArray.length <= 0) {
            return null;
        }

        Deque<EffectRecord> stack = new ArrayDeque<EffectRecord>();
        for (int i = 0; i < stackArray.length; i++) {
            stack.push(stackArray[i]);
        }

        return stack;
    }

    public void setBackgroundForegroundImages(Bitmap background, Bitmap foreground) {

        inputLayer[Layer.BACKGROUND.index] = background;
        inputLayer[Layer.FOREGROUND.index] = foreground;

        if (effectLayer[Layer.BACKGROUND.index] != null) {
            effectLayer[Layer.BACKGROUND.index].recycle();
        }
        effectLayer[Layer.BACKGROUND.index] = Bitmap.createBitmap(
                inputLayer[Layer.BACKGROUND.index].getWidth(),
                inputLayer[Layer.BACKGROUND.index].getHeight(), Config.ARGB_8888);

        if (effectLayer[Layer.FOREGROUND.index] != null) {
            effectLayer[Layer.FOREGROUND.index].recycle();
        }
        effectLayer[Layer.FOREGROUND.index] = Bitmap.createBitmap(
                inputLayer[Layer.FOREGROUND.index].getWidth(),
                inputLayer[Layer.FOREGROUND.index].getHeight(), Config.ARGB_8888);

        inputAllocation[Layer.BACKGROUND.index] = Allocation.createFromBitmap(renderscript,
                inputLayer[Layer.BACKGROUND.index]);
        inputAllocation[Layer.FOREGROUND.index] = Allocation.createFromBitmap(renderscript,
                inputLayer[Layer.FOREGROUND.index]);
        effectAllocation[Layer.BACKGROUND.index] = Allocation.createFromBitmap(renderscript,
                effectLayer[Layer.BACKGROUND.index]);
        effectAllocation[Layer.FOREGROUND.index] = Allocation.createFromBitmap(renderscript,
                effectLayer[Layer.FOREGROUND.index]);

        updateEffectsForLayer(Layer.BACKGROUND);
        updateEffectsForLayer(Layer.FOREGROUND);
    }

    private void updateEffectsForLayer(Layer layer) {

        Effect effect = getEffect(layer);
        if (effect instanceof ColorEffect) {
            setColorEffect(layer, (ColorEffect)effect);
        } else if (effect instanceof BitmapEffect) {
            // TODO: Rename this.effect - this is confusing since it is EffectRecord and not Effect
            setBitmapEffect(layer, (BitmapEffect)effect,
                    this.effect[layer.index].bitmapEffectBitmap);
        } else {
            clearEffect(layer);
        }
    }

    /**
     * Clear any currently set {@link Effect} for the specified layer
     * @param layer {@link Layer} for which the effect should be cleared
     */
    void clearEffect(Layer layer) {
        if (effect[layer.index].effect instanceof IdentityEffect) {
            bringToTopOfUndoStack(effect[layer.index]);
            return;
        }

        effect[layer.index] = new EffectRecord(layer, IdentityEffect.DEFAULT, null);
        undoStack.push(effect[layer.index]);
    }

    /**
     * Set the {@link ColorEffect} to apply to the specified layer
     * @param layer {@link Layer} to which the effect should be applied
     * @param colorEffect a {@link ColorEffect} to apply to the specified layer
     */
    void setColorEffect(Layer layer, ColorEffect colorEffect) {
        int intensity;
        if (effect[layer.index].effect == colorEffect) {
            bringToTopOfUndoStack(effect[layer.index]);
            return;
        }

        effect[layer.index] = new EffectRecord(layer, colorEffect, null);
        undoStack.push(effect[layer.index]);
        redoStack.clear();


        switch (colorEffect) {
        case GRAYSCALE:
        case SEPIA:
            intensity = DEFAULT_COLOR_EFFECT_INTENSITY;
            break;

        case BLUR:
        case LIGHTEN:
        case DARKEN:
            intensity = DEFAULT_LIGHTEN_DARKEN_BLUR_INTENSITY;
            break;

        case RECOLOR:
                intensity = 0;
                break;
        default:
            throw new RuntimeException("Unexpected ColorEffect type");
        }

        setColorEffectIntensity(layer, intensity);
    }

    /**
     * Set the intensity of the currently active {@link ColorEffect} (if one is set)
     * @param layer {@link Layer} to which the intensity should be applied
     * @param intensity an integer, from 0 to 100, representing the intensity at which to apply the
     *        {@link ColorEffect}
     */
    void setColorEffectIntensity(Layer layer, int intensity) {
        if (intensity < 0) {
            intensity = 0;
        } else if (intensity > 100) {
            intensity = 100;
        }

        effect[layer.index].colorEffectIntensity = intensity;
        redoStack.clear();
    }

    void setNextRecolorPatternForLayer(Layer layer){
        //for RECOLOR, intensity field holds the pattern index
        int intensity = (getColorEffectIntensity(layer)+1)%rgbRecoloringPattern.length;
        setColorEffectIntensity(layer, intensity);
    }

    /**
     * Get the intensity of the {@link ColorEffect} for the specified layer
     * @param layer {@link Layer} for which the intensity should be retrieved
     * @return an integer, from 0 to 100, representing the intensity of the {@link ColorEffect}
     */
    int getColorEffectIntensity(Layer layer) {
        return effect[layer.index].colorEffectIntensity;
    }

    /**
     * Set the {@link BitmapEffect} to apply to the specified layer
     * @param layer {@link Layer} to which the effect should be applied
     * @param bitmapEffect a {@link BitmapEffect} to apply to the specified layer
     * @param bitmap a {@link Bitmap} to use for the {@link BitmapEffect}
     */
    void setBitmapEffect(Layer layer, BitmapEffect bitmapEffect, Bitmap bitmap) {
        if (effect[layer.index].effect == bitmapEffect &&
                effect[layer.index].bitmapEffectBitmap.sameAs(bitmap)) {
            bringToTopOfUndoStack(effect[layer.index]);
            return;
        }

        effect[layer.index] = new EffectRecord(layer, bitmapEffect, bitmap);
        undoStack.push(effect[layer.index]);
        redoStack.clear();
    }

    /**
     * Get a {@link Bitmap} for the specified layer with any active {@link Effect} applied
     * @param layer {@link Layer} for which {@link Bitmap} should be retrieved
     * @return a {@link Bitmap} with effect applied for the specified layer
     */
    Bitmap getEffectBitmap(Layer layer) {

        if (effect[layer.index].effect instanceof ColorEffect) {
            switch ((ColorEffect)effect[layer.index].effect) {
            case BLUR:
                applyBlurToBitmap(inputAllocation[layer.index], effectAllocation[layer.index],
                        getBlurRadius(effect[layer.index].colorEffectIntensity));
                break;

            case DARKEN:
                applyColorMatrixToBitmap(inputLayer[layer.index], effectLayer[layer.index],
                        getDarkenColorMatrix(effect[layer.index].colorEffectIntensity));
                break;

            case LIGHTEN:
                applyColorMatrixToBitmap(inputLayer[layer.index], effectLayer[layer.index],
                        getLightenColorMatrix(effect[layer.index].colorEffectIntensity));
                break;

            case GRAYSCALE:
                applyColorMatrixToBitmap(inputLayer[layer.index], effectLayer[layer.index],
                        getGrayscaleColorMatrix(effect[layer.index].colorEffectIntensity));
                break;

            case SEPIA:
                applyColorMatrixToBitmap(inputLayer[layer.index], effectLayer[layer.index],
                        getSepiaColorMatrix(effect[layer.index].colorEffectIntensity));
                break;

            case RECOLOR:
                //using colorEffectIntensity to store patternIdx
                applyColorMatrixToBitmap(inputLayer[layer.index], effectLayer[layer.index],
                        getRGBRemappingColorMatrix(effect[layer.index].colorEffectIntensity));
                    break;

            default:
                throw new RuntimeException("Unexpected ColorEffect");
            }
        } else if (effect[layer.index].effect instanceof BitmapEffect) {
            switch ((BitmapEffect)effect[layer.index].effect) {
            case REPLACE:
                applyReplaceToBitmap(effect[layer.index].bitmapEffectBitmap,
                        effectLayer[layer.index]);
                break;

            default:
                throw new RuntimeException("Unexpected BitmapEffect");
            }
        } else if (effect[layer.index].effect instanceof IdentityEffect) {
            copyBitmap(inputLayer[layer.index], effectAllocation[layer.index]);
        } else {
            throw new RuntimeException("Unexpected effect type [" + effect[layer.index] +
                    "] for layer [" + layer + "]");
        }

        return effectLayer[layer.index];
    }

    /**
     * Get the "active" {@link Layer}, the one to which an effect was most recently applied
     * @return the last {@link Layer} to which an effect was applied
     */
    Layer getActiveLayer() {
        if (undoStack.isEmpty()) {
            return Layer.FOREGROUND;
        }
        return undoStack.peek().layer;
    }

    /**
     * Get the {@link Effect} applied to the specified {@link Layer}
     * @return the {@link Effect} applied to the specified {@link Layer}
     */
    Effect getEffect(Layer layer) {
        return effect[layer.index].effect;
    }

    /**
     * Undoes the most recent applied effect, and restores the effect active before it
     * @return the {@link Layer} on which the most recent effect was just discarded, or null if
     *         there was no previous effect to undo
     */
    Layer undoEffect() {
        if (undoStack.isEmpty()) {
            return null;
        }

        EffectRecord undoEffect = undoStack.pop();
        redoStack.push(undoEffect);

        effect[undoEffect.layer.index] = undoEffect.layer.defaultEffect;
        for (EffectRecord record : undoStack) {
            if (record.layer == undoEffect.layer) {
                effect[undoEffect.layer.index] = record;
                break;
            }
        }

        return undoEffect.layer;
    }

    /**
     * Redoes the most recently undone effect (via {@link #undoEffect()})
     * @return the {@link Layer} on which the redone effect is applied, or null if there was no
     *         previously undone effect to redo
     */
    Layer redoEffect() {
        if (redoStack.isEmpty()) {
            return null;
        }

        EffectRecord redoEffect = redoStack.pop();
        undoStack.push(redoEffect);
        effect[redoEffect.layer.index] = redoEffect;

        return redoEffect.layer;
    }

    /**
     * Test whether there are any previous effects that can be undone via {@link #undoEffect()}
     * @return true if invoking {@link #undoEffect()} will have an effect, else false
     */
    boolean canUndo() {
        return !undoStack.isEmpty();
    }

    /**
     * Test whether there are any previously undone effects that can be redone via
     * {@link #redoEffect()}
     * @return true if invoking {@link #redoEffect()} will have an effect, else false
     */
    boolean canRedo() {
        return !redoStack.isEmpty();
    }

    /**
     * Get a {@link Bitmap} containing a flat image with the current effects applied
     */
    Bitmap getFlattenedImage() {
        Bitmap flattenBitmap = effectLayer[0].copy(Config.ARGB_8888, true);
        Canvas flattenCanvas = new Canvas(flattenBitmap);
        for (int i = 1; i < effectLayer.length; i++) {
            flattenCanvas.drawBitmap(effectLayer[i], 0, 0, null);
        }
        return flattenBitmap;
    }

    private void bringToTopOfUndoStack(EffectRecord effect) {
        if (undoStack.remove(effect)) {
            undoStack.push(effect);
        }
    }

    private ColorMatrixColorFilter getDarkenColorMatrix(int intensity) {
        final float darken = DARKEN * intensity / 100f;
        return new ColorMatrixColorFilter(new ColorMatrix(new float[] { 1, 0, 0, 0, darken, 0, 1,
                0, 0, darken, 0, 0, 1, 0, darken, 0, 0, 0, 1, 0 }));
    }

    private ColorMatrixColorFilter getLightenColorMatrix(int intensity) {
        final float lighten = LIGHTEN * intensity / 100f;
        return new ColorMatrixColorFilter(new ColorMatrix(new float[] { 1, 0, 0, 0, lighten, 0, 1,
                0, 0, lighten, 0, 0, 1, 0, lighten, 0, 0, 0, 1, 0 }));
    }

    private ColorMatrixColorFilter getGrayscaleColorMatrix(int intensity) {
        ColorMatrix cm = new ColorMatrix();
        cm.setSaturation((100 - intensity) / 100f);
        return new ColorMatrixColorFilter(cm);
    }

    private ColorMatrixColorFilter getSepiaColorMatrix(int intensity) {
        ColorMatrix matrixA = new ColorMatrix();
        matrixA.setSaturation((100 - intensity) / 100f);

        ColorMatrix matrixB = new ColorMatrix();
        matrixB.setScale(1f, 1f - 0.05f * (intensity / 100f), 1f - 0.18f * (intensity / 100f), 1f);
        matrixA.setConcat(matrixB, matrixA);

        ColorMatrixColorFilter filter = new ColorMatrixColorFilter(matrixA);
        return filter;
    }

    private ColorMatrixColorFilter getRGBRemappingColorMatrix(int rgbRecoloringPatternIdx) {
        ColorMatrix matrix = new ColorMatrix(rgbRecoloringPattern[rgbRecoloringPatternIdx%rgbRecoloringPattern.length]);

        ColorMatrixColorFilter filter = new ColorMatrixColorFilter(matrix);
        return filter;
    }

    private float getBlurRadius(int intensity) {
        return intensity / 4f;
    }

    private static void applyColorMatrixToBitmap(Bitmap inputBitmap, Bitmap outputBitmap,
            ColorFilter colorFilter) {
        Paint paint = new Paint();
        paint.setColorFilter(colorFilter);
        new Canvas(outputBitmap).drawBitmap(inputBitmap, 0, 0, paint);
    }

    // radius must be between 0.0f and 25.0f
    private void applyBlurToBitmap(Allocation input, Allocation output, float radius) {
        ScriptIntrinsicBlur blur = ScriptIntrinsicBlur.create(renderscript,
                Element.U8_4(renderscript));
        blur.setRadius(radius == 0f ? Float.MIN_NORMAL : radius);
        input.syncAll(Allocation.USAGE_SCRIPT);
        blur.setInput(input);
        blur.forEach(output);
        output.syncAll(Allocation.USAGE_SHARED);
    }

    private static void applyReplaceToBitmap(Bitmap inputBitmap, Bitmap outputBitmap) {

        final Matrix m = new Matrix();
        float scale, dx, dy;
        if (inputBitmap.getWidth() * outputBitmap.getHeight() >= outputBitmap.getWidth() *
                inputBitmap.getHeight()) {
            // Input has a wider aspect ratio than output
            scale = (float)outputBitmap.getHeight() / inputBitmap.getHeight();
            dx = (inputBitmap.getWidth() * scale - outputBitmap.getWidth()) * 0.5f;
            dy = 0;
        } else {
            // Input has a taller aspect ratio than output
            scale = (float)outputBitmap.getWidth() / inputBitmap.getWidth();
            dx = 0;
            dy = (inputBitmap.getHeight() * scale - outputBitmap.getHeight()) * 0.5f;
        }
        m.setScale(scale, scale);
        m.postTranslate(-dx, -dy);

        new Canvas(outputBitmap).drawBitmap(inputBitmap, m, new Paint(Paint.FILTER_BITMAP_FLAG));
    }

    private static void copyBitmap(Bitmap input, Allocation output) {
        output.copyFrom(input);
        output.syncAll(Allocation.USAGE_SHARED);
    }

    private static class EffectRecord implements Parcelable {
        final Layer layer;
        final Effect effect;
        int colorEffectIntensity;
        final Bitmap bitmapEffectBitmap;

        public EffectRecord(Layer layer, Effect effect, Bitmap bitmapEffectBitmap) {
            this.layer = layer;
            this.effect = effect;
            this.bitmapEffectBitmap = bitmapEffectBitmap;
        }

        public static final Parcelable.Creator<EffectRecord> CREATOR = new Parcelable.Creator<EffectRecord>() {
            public EffectRecord createFromParcel(Parcel in) {
                Layer pLayer = Layer.values()[in.readInt()];
                Effect pEffect = (Effect)(in.readParcelable(getClass().getClassLoader()));

                // Bitmap is Parcelable
                Bitmap bitmap = null;
                int isBitmapExist = in.readInt();
                // this needs to be done because there can be a crash if we try to read null
                // parcelable bitmap
                if (isBitmapExist == 1) {
                    // an alternative way to read a parcelable bitmap:
                    // bitmap = (Bitmap)(in.readParcelable(getClass().getClassLoader()));
                    bitmap = Bitmap.CREATOR.createFromParcel(in);
                }

                return new EffectRecord(pLayer, pEffect, bitmap);
            }

            public EffectRecord[] newArray(int size) {
                return new EffectRecord[size];
            }
        };

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(layer.index);
            dest.writeParcelable(effect, 0);
            // this needs to be done because there can be a crash if we try to read null parcelable
            // bitmap
            if (bitmapEffectBitmap != null) {
                dest.writeInt(1);
                bitmapEffectBitmap.writeToParcel(dest, 0);
            } else {
                dest.writeInt(0);
            }
        }
    }
}
