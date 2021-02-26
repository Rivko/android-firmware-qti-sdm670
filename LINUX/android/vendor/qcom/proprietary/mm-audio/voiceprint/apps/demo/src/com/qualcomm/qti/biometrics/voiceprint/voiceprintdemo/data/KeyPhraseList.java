/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data;

import android.content.Context;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

public class KeyPhraseList extends Hashtable<String, KeyPhrase> {
    private final static String TAG = "VoicePrintDemo";
    private final static String DELIMITER = "_";

    private Context context;
    private KeyPhrase selected = null;


    public KeyPhraseList(Context context) {
        this.context = context;
    }

    public KeyPhraseList(Context context, List<KeyPhrase> keyphrases) {
        this.context = context;

        setKeyPhrases(keyphrases);

    }


   /* public void setKeyPhrases(Set<String> list) {
        if (list == null) {
            Log.d(TAG, "List cannot be null");
            return;
        }
        // format is id_phrase_name
        for (String entry : list) {
            if (entry != null) {
                String[] tokens = entry.split(DELIMITER);

                if (tokens.length > 1) {
                    // duplicates will be overwritten
                    KeyPhrase keyphrase = new KeyPhrase(tokens[0], tokens[1]);
                    put(keyphrase.getId(), keyphrase);
                }
            }
        }
    }*/


    public List<KeyPhrase> getList() {
        List<KeyPhrase> list = new ArrayList<KeyPhrase>(values());
        return list;
    }

    public void add(String phraseName, String phrase) {
        // duplicates are overwritten
        add(new KeyPhrase(phraseName, phrase));
    }

    public void add(KeyPhrase keyphrase) {
        put(keyphrase.getId(), keyphrase);
        //     VoicePrintPreferences.saveKeyPhrase(context, keyphrase);
    }

    @Override
    public synchronized KeyPhrase put(String key, KeyPhrase value) {
        return super.put(key, value);
    }


    public void select(KeyPhrase newSelected, boolean isChecked) {

        if (isChecked && newSelected != null) {
            // Uncheck others
            for (KeyPhrase entry : values()) {
                entry.setSelected(false);
            }

            // Check new selection
            newSelected.setSelected(true);


        } else {
            // Save new selection
            VoicePrintPreferences.saveLastSelectedKeyPhrase(context, null);
        }

        this.selected = newSelected;
    }

    public KeyPhrase getSelected() {
        return selected;
    }

    public boolean hasSelection() {
        return (selected != null);
    }

    public void clearSelected() {
        if (selected != null) {
            selected.setSelected(false);
        }
    }

    public void select(String lastSelectedKeyPhrase) {
        if (lastSelectedKeyPhrase == null) return;

        // find entry
        KeyPhrase item = this.get(lastSelectedKeyPhrase);
        if (item != null) {
            select(item, true);
        }

    }

    public Context getContext() {
        return context;
    }

    public static KeyPhraseList obtain(KeyPhraseList src) {
        KeyPhraseList dst = new KeyPhraseList(src.getContext());
        dst.putAll(src);
        return dst;
    }

    public void setKeyPhrases(List<KeyPhrase> keyphrases) {
        for (KeyPhrase kp : keyphrases) {
            add(kp);
        }
    }


}