/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.ListView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseList;

/**
 * ListView to display phrase information.
 */
public class PhraseListView extends LinearLayout implements AdapterView.OnItemSelectedListener {

    private static final String TAG = "VoicePrintDemo";
    private ListView phraseListView;
    private KeyPhraseSelectAdapter mAdapter;
    private KeyPhraseSelectAdapter.OnKeyPhraseSelectionChange listener;
    private KeyPhraseList phraseList;
    private int mode = KeyPhraseSelectAdapter.SELECTION_ONLY_MODE;

    public PhraseListView(Context context) {
        super(context);


        init(context, null);
    }

    public PhraseListView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context, attrs);
    }

    public PhraseListView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context, attrs);
    }

    public PhraseListView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(context, attrs);

    }
    //  private VoicePrintDemoApp mApplication;

    private void init(Context context, AttributeSet attrs) {
        LayoutInflater inflater =
                (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        LayoutInflater layoutInflater = (LayoutInflater)
                context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);


        if (layoutInflater != null) {
            View view = layoutInflater.inflate(R.layout.phrase_list_view, this);
            View main = view.findViewById(R.id.mainLayout);
            phraseListView = (ListView) main.findViewById(R.id.phraseList);
            //  phraseListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
            phraseListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, final int position, long id) {
                    //     Log.d(TAG, "setOnItemClickListener item " + position);

                    // Manually call selection to avoid row color changes
                    if (mode == KeyPhraseSelectAdapter.SELECTION_ONLY_MODE) {
                        onItemSelected(parent, view, position, id);
                    } else {
                        phraseListView.requestFocusFromTouch();
                        phraseListView.smoothScrollToPosition(position);
                        phraseListView.setSelectionFromTop(position, phraseListView.getHeight() / 2);
                    }
                }
            });

            phraseListView.setOnItemSelectedListener(this);
        }

        if (attrs != null) {
            TypedArray a = context.getTheme().obtainStyledAttributes(
                    attrs,
                    R.styleable.PhraseListView,
                    0, 0);

            try {
                mode = a.getInteger(R.styleable.PhraseListView_mode,
                        KeyPhraseSelectAdapter.SELECTION_ONLY_MODE);
                if (mode != KeyPhraseSelectAdapter.SELECTION_ONLY_MODE) {
                    phraseListView.setSelector(R.drawable.list_selector);
                }

            } finally {
                a.recycle();
            }
        }
    }

    public void setPhraseList(KeyPhraseList list) {

        phraseList = list;
        mAdapter = new KeyPhraseSelectAdapter(this.getContext(), phraseList.getList(), mode);
        phraseListView.setClickable(true);
        phraseListView.setAdapter(mAdapter);
    }

    public void addHeaderPadding(int value) {
        if (phraseListView == null) {
            phraseListView = (ListView) findViewById(R.id.phraseList);
        }

        View padding = new View(this.getContext());
        padding.setMinimumHeight(value);

        phraseListView.addHeaderView(padding);

    }

    public void addFooterPadding(int value) {


        if (phraseListView == null) {
            phraseListView = (ListView) findViewById(R.id.phraseList);
        }

        View padding = new View(this.getContext());
        padding.setMinimumHeight(value);

        phraseListView.addFooterView(padding);

    }

    public KeyPhrase getSelected() {

        return phraseList.getSelected();
    }

    public boolean hasSelection() {

        return phraseList.hasSelection();
    }

    public ListView getListView() {
        return phraseListView;
    }

    public void setOnSelectionChangeListener(KeyPhraseSelectAdapter.OnKeyPhraseSelectionChange listener) {

        this.listener = listener;
    }

    public void select(String phrase) {

        if (phraseList != null) {
            phraseList.select(phrase);
            mAdapter.notifyDataSetChanged();
        }
    }

    public int getSelectedItemPosition() {

        return phraseListView.getSelectedItemPosition();
    }

    public void smoothScrollToPosition(int position) {

        phraseListView.smoothScrollToPosition(position);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        Log.d(TAG, "setOnItemSelectedListener item " + position);
        phraseList.select(mAdapter.getItem(position), true);
        mAdapter.notifyDataSetChanged();

        // Let observers do own processing
        if (listener != null) {
            listener.onSelectionChange(position, mAdapter.getItem(position), true);
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {
        //   Log.d(TAG, "onNothingSelected()");
    }
}
