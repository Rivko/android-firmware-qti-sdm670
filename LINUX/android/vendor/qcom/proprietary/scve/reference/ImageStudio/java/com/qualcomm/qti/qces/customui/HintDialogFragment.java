/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.customui;

import android.app.DialogFragment;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public class HintDialogFragment extends DialogFragment {

    HintPosition hintLayoutType;
    String dialogText;

    public enum HintPosition {
        TOP_TOAST, TOP_BUBBLE, BOTTOM_BUBBLE, BOTTOM_RIGHT_BUBBLE
    }

    public static HintDialogFragment newInstance(HintPosition hintPos, String dialogString) {
        HintDialogFragment fragment = new HintDialogFragment();

        Bundle args = new Bundle();
        args.putSerializable("hintPosition", hintPos);
        args.putString("dialogString", dialogString);

        fragment.setArguments(args);

        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        hintLayoutType = (HintPosition)getArguments().getSerializable("hintPosition");
        dialogText = getArguments().getString("dialogString");

        setStyle(STYLE_NO_FRAME, android.R.style.Theme_Dialog);
    }

    @Override
    public void onStart() {
        super.onStart();
        Window window = getDialog().getWindow();
        WindowManager.LayoutParams params = window.getAttributes();
        params.dimAmount = 0.0f;

        // set FLAG_NOT_FOCUSABLE so that dialog won't consume the touch event
        params.flags += WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;

        window.setAttributes(params);
        window.setBackgroundDrawableResource(android.R.color.transparent);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = null;

        switch (hintLayoutType) {
        case TOP_TOAST:
            view = inflater.inflate(R.layout.hint_top_toast_layout, container, false);
            break;
        case TOP_BUBBLE:
            view = inflater.inflate(R.layout.hint_top_bubble_layout, container, false);
            break;
        case BOTTOM_BUBBLE:
            view = inflater.inflate(R.layout.hint_bottom_bubble_layout, container, false);
            break;
        case BOTTOM_RIGHT_BUBBLE:
            view = inflater.inflate(R.layout.hint_bottom_right_bubble_layout, container, false);
            break;
        }

        TextView dialogTextView = (TextView)view.findViewById(R.id.popupTextView);
        dialogTextView.setText(dialogText);

        setDialogPosition();

        view.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                /*
                 * Commenting below because onTouch will dispatch event to parent & parent
                 * activities's dispatchTouchEvent will handle the actual dismiss of this fragment
                 */
                // getDialog().dismiss();
            }
        });

        view.setOnTouchListener(new OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                getActivity().dispatchTouchEvent(event);
                return false;
            }
        });
        return view;
    }

    private void setDialogPosition() {
        Window window = getDialog().getWindow();
        WindowManager.LayoutParams params = window.getAttributes();

        switch (hintLayoutType) {
        case TOP_BUBBLE:
            window.setGravity(Gravity.TOP | Gravity.CENTER);
            break;
        case TOP_TOAST:
            window.setGravity(Gravity.TOP | Gravity.CENTER);
            break;
        case BOTTOM_BUBBLE:
            window.setGravity(Gravity.BOTTOM | Gravity.CENTER);
            break;
        case BOTTOM_RIGHT_BUBBLE:
            window.setGravity(Gravity.BOTTOM | Gravity.RIGHT);
            break;
        }
        params.y = getResources().getDimensionPixelSize(R.dimen.topBarHeight);
        window.setAttributes(params);
    }

}
