/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.widget;

import android.app.DialogFragment;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import com.qualcomm.qti.threedr.R;

/**
 * Created by mithun on 9/27/16.
 */

public class HintDialogFragment extends DialogFragment {

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(STYLE_NO_FRAME, android.R.style.Theme_Dialog);
    }

    @Override
    public void onStart() {
        super.onStart();
        Window window = getDialog().getWindow();
        window.setGravity(Gravity.CENTER_VERTICAL | Gravity.END);
        WindowManager.LayoutParams params = window.getAttributes();
        params.y = -145;
        params.dimAmount = 0.0f;

        // set FLAG_NOT_FOCUSABLE so that dialog won't consume the touch event
        params.flags += WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;

        window.setAttributes(params);
        window.setBackgroundDrawableResource(android.R.color.transparent);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {

        //return super.onCreateView(inflater, container, savedInstanceState);
        View view = inflater.inflate(R.layout.hint_bubble_layout, container, false);
        view.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                /*
                 * Commenting below because onTouch will dispatch event to parent & parent
                 * activities's dispatchTouchEvent will handle the actual dismiss of this fragment
                 */
                // getDialog().dismiss();
            }
        });

        view.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                dismiss();
                return false;
            }
        });
        return view;

    }


}
