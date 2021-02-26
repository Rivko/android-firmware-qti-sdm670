/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;

import java.util.List;

/**
 * Adapter class for the Phrase List View. Pulls data from the database and converts
 * them into the row view.
 */
public class KeyPhraseSelectAdapter extends ArrayAdapter<KeyPhrase> {

    private Context context;
    //private OnKeyPhraseSelectionChange listener;
    //private int selectedPosition = -1;
    public static final String TAG = "VoicePrintDemo";

    // Should match style
    public final static int SELECTION_ONLY_MODE = 0;
    public final static int EDIT_MODE = 1;

    private int mode = SELECTION_ONLY_MODE;


    public interface OnKeyPhraseSelectionChange {
        public void onSelectionChange(int position, KeyPhrase newSelected, boolean isChecked);
    }

    public KeyPhraseSelectAdapter(Context context, List<KeyPhrase> objects) {
        this(context, objects, SELECTION_ONLY_MODE);
    }


    public KeyPhraseSelectAdapter(Context context, List<KeyPhrase> objects, int mode) {
        super(context, R.layout.two_item_row, objects);

        this.context = context;
        this.mode = mode;
    }

    /*
     * (non-Javadoc)
 *
 * @see android.widget.ArrayAdapter#getView(int, android.view.View,
 * android.view.ViewGroup)
 */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        // Gets a view that displays the data at the particular position in the
        // data set. Return a custom row view.
        return (mode == SELECTION_ONLY_MODE ? drawSelectOnlyDataRow(position, parent, inflater) :
                drawEditDataRow(position, parent, inflater));

    }

    /**
     * Draw data row.
     *
     * @param position the position
     * @param parent   the parent
     * @param inflater the inflater
     * @return the view
     */
    private View drawSelectOnlyDataRow(final int position, ViewGroup parent,
                                       LayoutInflater inflater) {

        // Inflate a custom row view.
        final View rowView = inflater
                .inflate(R.layout.two_item_row, parent, false);

        final KeyPhrase item = (KeyPhrase) getItem(position);

        if (item == null) {
            return null;
        }


        TextView phraseName = (TextView) rowView.findViewById(R.id.text2);
        if (item.getName() != null) {
            phraseName.setText(item.getName());
        } else {
            phraseName.setVisibility(View.GONE);
        }

        TextView phrase = (TextView) rowView.findViewById(R.id.text1);
        if (item.getPhrase() != null) {
            phrase.setText(item.getPhrase());
        } else {
            phrase.setVisibility(View.GONE);
        }

        final CheckBox radioButton = (CheckBox) rowView.findViewById(R.id.selectButton);
        radioButton.setChecked(item.isSelected());
        radioButton.setVisibility(View.VISIBLE);
        radioButton.setFocusable(false);
        radioButton.setClickable(false);

        return rowView;
    }


    /**
     * Draw data row.
     *
     * @param position the position
     * @param parent   the parent
     * @param inflater the inflater
     * @return the view
     */
    private View drawEditDataRow(final int position, ViewGroup parent,
                                 LayoutInflater inflater) {

        // Inflate a custom row view.
        final View rowView = inflater
                .inflate(R.layout.two_item_row, parent, false);

        final KeyPhrase item = (KeyPhrase) getItem(position);

        if (item == null)
            return null;

        TextView phraseName = (TextView) rowView.findViewById(R.id.text2);
        phraseName.setText(item.getName());

        TextView phraseText = (TextView) rowView.findViewById(R.id.text1);
        phraseText.setText(item.getPhrase());

        final CheckBox button = (CheckBox) rowView.findViewById(R.id.settingsButton);
        button.setVisibility(View.VISIBLE);
        button.setFocusable(false);
        button.setClickable(false);

        return rowView;
    }
}
