/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.effects;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.DialogFragment;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.qualcomm.qti.qces.customui.HintDialogFragment;
import com.qualcomm.qti.qces.customui.HintDialogFragment.HintPosition;
import com.qualcomm.qti.qces.imageedit.effects.EffectsModel.BitmapEffect;
import com.qualcomm.qti.qces.imageedit.effects.EffectsModel.ColorEffect;
import com.qualcomm.qti.qces.imageedit.effects.EffectsModel.Effect;
import com.qualcomm.qti.qces.imageedit.effects.EffectsModel.IdentityEffect;
import com.qualcomm.qti.qces.imageedit.effects.EffectsModel.Layer;
import com.qualcomm.qti.qces.imageedit.segmentation.HardCutActivity;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.stage.StageActivity;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;
import com.qualcomm.qti.qces.util.ImageUtils;

/*package*/class EffectsController {
    private static final String TAG = EffectsController.class.getSimpleName();
    private static final String EFFECTS_FILE_NAME = "effects_flat.png";

    private EffectsActivity activity;
    private EffectsModel model;
    private EffectsView view;

    public enum HintType {
        SEPARATE_AWARE
    };

    public EffectsController(EffectsActivity activity, EffectsView view, EffectsModel model) {
        this.activity = activity;
        this.view = view;
        this.model = model;
    }

    void initialize(Uri backgroundUri, Uri foregroundUri) {
        view.hideEffectIntensitySlider();
        new LoadInputBitmapsTask().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR, backgroundUri,
                foregroundUri);
    }

    void onSaveInstanceState(Bundle bundle) {
        model.saveState(bundle);
    }

    void onRestoreInstanceState(Bundle bundle) {
        model.restoreState(bundle);
    }

    void onResume() {
        if (model.hintActive == true) {
            model.hintShown.put(HintType.SEPARATE_AWARE, false);
        }
        showHint(HintType.SEPARATE_AWARE);
    }

    void foregroundLightenClicked() {
        model.setColorEffect(Layer.FOREGROUND, ColorEffect.LIGHTEN);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void foregroundDarkenClicked() {
        model.setColorEffect(Layer.FOREGROUND, ColorEffect.DARKEN);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void foregroundSepiaClicked() {
        model.setColorEffect(Layer.FOREGROUND, ColorEffect.SEPIA);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void foregroundGrayscaleClicked() {
        model.setColorEffect(Layer.FOREGROUND, ColorEffect.GRAYSCALE);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void foregroundNoneClicked() {
        model.clearEffect(Layer.FOREGROUND);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void foregroundBlurClicked() {
        model.setColorEffect(Layer.FOREGROUND, ColorEffect.BLUR);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void foregroundRecolorClicked() {
        model.setNextRecolorPatternForLayer(Layer.FOREGROUND);
        model.setColorEffect(Layer.FOREGROUND, ColorEffect.RECOLOR);
        updateEffectBitmapForLayer(Layer.FOREGROUND);
        updateUIForEffectState();
    }

    void backgroundLightenClicked() {
        model.setColorEffect(Layer.BACKGROUND, ColorEffect.LIGHTEN);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void backgroundDarkenClicked() {
        model.setColorEffect(Layer.BACKGROUND, ColorEffect.DARKEN);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void backgroundSepiaClicked() {
        model.setColorEffect(Layer.BACKGROUND, ColorEffect.SEPIA);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void backgroundGrayscaleClicked() {
        model.setColorEffect(Layer.BACKGROUND, ColorEffect.GRAYSCALE);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void backgroundNoneClicked() {
        model.clearEffect(Layer.BACKGROUND);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void backgroundBlurClicked() {
        model.setColorEffect(Layer.BACKGROUND, ColorEffect.BLUR);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void backgroundRecolorClicked() {
        model.setNextRecolorPatternForLayer(Layer.BACKGROUND);
        model.setColorEffect(Layer.BACKGROUND, ColorEffect.RECOLOR);
        updateEffectBitmapForLayer(Layer.BACKGROUND);
        updateUIForEffectState();
    }

    void effectIntensityChanged(int intensity) {
        final Layer activeLayer = model.getActiveLayer();
        model.setColorEffectIntensity(activeLayer, intensity);
        updateEffectBitmapForLayer(activeLayer);
    }

    void doneClicked() {
        Bitmap flat = model.getFlattenedImage();
        new SaveTempImageTask().execute(flat);
    }

    void cancelClicked() {
        activity.setResult(Activity.RESULT_CANCELED);
        activity.finish();
    }

    void undoClicked() {
        Layer modifiedLayer = model.undoEffect();
        if (modifiedLayer != null) {
            updateEffectBitmapForLayer(modifiedLayer);
        }
        updateUIForEffectState();
    }

    void redoClicked() {
        Layer modifiedLayer = model.redoEffect();
        if (modifiedLayer != null) {
            updateEffectBitmapForLayer(modifiedLayer);
        }
        updateUIForEffectState();
    }

    private void flattenConfirmDialog(){

        final AlertDialog optDialog = new AlertDialog.Builder(activity).create();
        LayoutInflater inflater = activity.getLayoutInflater();
        //AlertDialog dialog = builder.create();

        View view = inflater.inflate(R.layout.alert_dialog_custom, null);
        TextView dialogTextView = (TextView)view.findViewById(R.id.alert_dialog_title_text);
        dialogTextView.setText(R.string.effects_segment_dialog);
        Button button1 = (Button)view.findViewById(R.id.alert_dialog_btn1);
        button1.setText(R.string.effects_segment_dialog_stay);
        Button button2 = (Button)view.findViewById(R.id.alert_dialog_btn2);
        button2.setText(R.string.effects_segment_dialog_go);
        optDialog.setView(view, 0, 0, 0, 0);

        button1.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                optDialog.dismiss();
            }
        });

        button2.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                optDialog.dismiss();
                Bitmap flat = model.getFlattenedImage();
                new SaveTempImageTask().execute(flat);
            }
        });

        optDialog.show();
    }

    void separateClicked() {

        if (activity.isOnlyForeground == false) {
            flattenConfirmDialog();
        }
        else {
            Intent hardCutActivity = new Intent(activity.getApplicationContext(), HardCutActivity.class);
            hardCutActivity.putExtra(ImageSharingIntentExtras.NEXT_ACTIVITY,
                    EffectsActivity.class.getSimpleName());
            hardCutActivity.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI, activity
                    .getBackgroundImageUri().toString());
            hardCutActivity.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
            activity.startActivity(hardCutActivity);
        }
    }

    void helpClicked() {
        Intent intent = new Intent(activity, EffectsIntroFragmentActivity.class);
        activity.startActivity(intent);
        model.helpActive = true;
    }

    boolean isHelpPressed() {
        return view.isHelpPressed();
    }

    private void updateEffectBitmapForLayer(Layer layer) {
        Bitmap bitmap = model.getEffectBitmap(layer);
        if (layer == Layer.FOREGROUND) {
            view.setForegroundBitmap(bitmap);
        } else {
            view.setBackgroundBitmap(bitmap);
        }
    }

    private void updateUIForEffectState() {
        final Layer currentLayer = model.getActiveLayer();
        final Effect currentEffect = model.getEffect(currentLayer);
        final Effect foregroundEffect = model.getEffect(Layer.FOREGROUND);
        final Effect backgroundEffect = model.getEffect(Layer.BACKGROUND);

        //2nd part if this IF condition is to disable intensity seekbar when RECOLOR is selected
        if ((currentEffect instanceof ColorEffect)&& (currentEffect != ColorEffect.RECOLOR)) {
            view.showEffectIntensitySlider();
            view.setEffectIntensity(model.getColorEffectIntensity(currentLayer));
        } else {
            view.hideEffectIntensitySlider();
        }

        if (model.canUndo()) {
            view.enableUndo();
        } else {
            view.disableUndo();
        }

        if (model.canRedo()) {
            view.enableRedo();
        } else {
            view.disableRedo();
        }

        String effectNamePrefix;
        if (currentLayer == Layer.FOREGROUND) {
            effectNamePrefix = activity.getString(R.string.effects_current_effect_foreground);
        } else {
            effectNamePrefix = activity.getString(R.string.effects_current_effect_background);
        }

        String effectName = null;
        if (currentEffect instanceof ColorEffect) {
            switch ((ColorEffect)currentEffect) {
            case BLUR:
                effectName = activity.getString(R.string.effects_effect_blur);
                break;

            case DARKEN:
                effectName = activity.getString(R.string.effects_effect_darken);
                break;

            case GRAYSCALE:
                effectName = activity.getString(R.string.effects_effect_grayscale);
                break;

            case LIGHTEN:
                effectName = activity.getString(R.string.effects_effect_lighten);
                break;

            case SEPIA:
                effectName = activity.getString(R.string.effects_effect_sepia);
                break;

            case RECOLOR:
                effectName = activity.getString(R.string.effects_effect_recolor);
                break;
            }
        } else if (currentEffect instanceof BitmapEffect) {
            switch ((BitmapEffect)currentEffect) {
            case REPLACE:
                effectName = activity.getString(R.string.effects_effect_replace_background);
                break;
            }
        } else if (currentEffect instanceof IdentityEffect) {
            effectName = activity.getString(R.string.effects_effect_none);
        }

        if (effectName == null) {
            throw new RuntimeException("Missing effect name");
        }

        view.setEffectName(effectNamePrefix + " " + effectName);

        updateEffectSelection(Layer.FOREGROUND, foregroundEffect);
        updateEffectSelection(Layer.BACKGROUND, backgroundEffect);

    }

    void updateEffectSelection(Layer layer, Effect effect) {
        if (effect instanceof ColorEffect) {
            switch ((ColorEffect)effect) {
            case BLUR:
                if (layer == Layer.FOREGROUND) {
                    view.selectForegroundEffectBlur();
                } else {
                    view.selectBackgroundEffectBlur();
                }
                break;

            case DARKEN:
                if (layer == Layer.FOREGROUND) {
                    view.selectForegroundEffectDarken();
                } else {
                    view.selectBackgroundEffectDarken();
                }
                break;

            case GRAYSCALE:
                if (layer == Layer.FOREGROUND) {
                    view.selectForegroundEffectGrayscale();
                } else {
                    view.selectBackgroundEffectGrayscale();
                }
                break;

            case LIGHTEN:
                if (layer == Layer.FOREGROUND) {
                    view.selectForegroundEffectLighten();
                } else {
                    view.selectBackgroundEffectLighten();
                }
                break;

            case SEPIA:
                if (layer == Layer.FOREGROUND) {
                    view.selectForegroundEffectSepia();
                } else {
                    view.selectBackgroundEffectSepia();
                }
                break;

                case RECOLOR:
                if (layer == Layer.FOREGROUND) {
                    view.selectForegroundEffectRecolor();
                } else {
                    view.selectBackgroundEffectRecolor();
                }
                break;
            }
        } else if (effect instanceof IdentityEffect) {
            if (layer == Layer.FOREGROUND) {
                view.selectForegroundEffectNone();
            } else {
                view.selectBackgroundEffectNone();
            }
        }
    }

    void showHint(HintType hintType) {
        DialogFragment newHintFragment = null;

        dismissHint(); // dismiss any previous hints still on the screen

        switch (hintType) {
        case SEPARATE_AWARE:
            if (model.hintShown.get(hintType) == false && view.isSeparateButtonEnabled()) {
                newHintFragment = HintDialogFragment.newInstance(HintPosition.BOTTOM_RIGHT_BUBBLE,
                        activity.getResources().getString(R.string.hint_separate));
            }
            break;
        }

        if (newHintFragment != null) {
            newHintFragment.show(activity.getFragmentManager(), "hint");
            model.hintActive = true;
            model.hintShown.put(hintType, true);
        }
    }

    void dismissHint() {
        DialogFragment df = (DialogFragment)activity.getFragmentManager().findFragmentByTag("hint");
        if (df != null && df.getDialog() != null) {
            df.getDialog().dismiss();
            model.hintActive = false;
        }
    }

    private class LoadInputBitmapsTask extends AsyncTask<Uri, Void, Void> {
        private Bitmap background;
        private Bitmap foreground;

        @Override
        protected void onPreExecute() {
            view.showSpinner();
        }

        @Override
        protected Void doInBackground(Uri... params) {
            try {
                background = ImageUtils.loadImage(activity, params[0], activity.isFullMP);
                foreground = ImageUtils.loadImage(activity, params[1], activity.isFullMP);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            if (activity.isOnlyForeground) {
                // enable only foreground effects and disable background effects
                activity.enableForegroundEffectsButtons(true);
                activity.enableBackgroundEffectsButtons(false);
            } else {
                // enable all the effects buttons
                activity.enableEffectsButtons(true);
            }

            if (background != null && foreground != null) {

                model.setBackgroundForegroundImages(background, foreground);
                view.enableTouchMaskImageView();

                updateEffectBitmapForLayer(Layer.FOREGROUND);
                updateEffectBitmapForLayer(Layer.BACKGROUND);

                updateUIForEffectState();

                /*
                 * After loading image need to initialize content size of image for scaling and
                 * translation
                 */
                view.updateContentSize();
            } else {
                Log.e(TAG, "Failed to load background & foreground Bitmaps");
                // TODO: handle failure gracefully
            }

            view.hideSpinner();
        }
    }

    private class SaveTempImageTask extends AsyncTask<Bitmap, Void, Uri> {
        @Override
        protected void onPreExecute() {
            view.showSpinner();
        }

        @Override
        protected Uri doInBackground(Bitmap... params) {

            File destination = new File(activity.getCacheDir(), EFFECTS_FILE_NAME);

            try {
                FileOutputStream out = new FileOutputStream(destination);
                params[0].compress(Bitmap.CompressFormat.PNG, 100, out);
                out.flush();
                out.close();
            } catch (IOException e) {
                Log.e(TAG, "Failed writing flat image to temporary storage", e);
                return null;
            }

            params[0].recycle();
            params[0] = null;
            return Uri.fromFile(new File(destination.getAbsolutePath()));

        }

        @Override
        protected void onPostExecute(Uri uri_result) {
            if (uri_result == null) {
                Log.e(TAG, "Failed to save temp flattened effects image");
                // TODO: handle failure gracefully
                return;
            }

            Intent nextActivity;
            if (activity.separateClicked)
                nextActivity = new Intent(activity, HardCutActivity.class);
            else
                nextActivity = new Intent(activity, StageActivity.class);

            nextActivity.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                    uri_result.toString());
            nextActivity.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
            // clear_top here will force segment and effects activities to close
            nextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            activity.startActivity(nextActivity);
            activity.finish();

            view.hideSpinner();
        }
    }

}
