/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import java.util.ArrayList;
import java.util.Collections;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ListView;

import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.SoundModel;
import com.qualcomm.qti.sva.ux.dev.VoiceRequestListAdapter.VoiceRequestItem;


public class VoiceRequestsActivity extends Activity {
	private final static String TAG = "ListenLog.VoiceRequestsActivity";

    private ArrayList<VoiceRequestItem> voiceRequestList = new ArrayList<VoiceRequestItem>();
    private VoiceRequestListAdapter voiceRequestListAdapter;
    private ListView uiVoiceRequestsListView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "onCreate");
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_dev_voicerequests);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");
        initializeUserInterface();
    }

    private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");
        uiVoiceRequestsListView = (ListView)findViewById(R.id.list_voicerequests);

        fillVoiceRequestListAdapter();
        //soundModelListAdapter = new SoundModelListAdapter(this, voiceRequestList, onItemActionListener);
        /*uiSoundModelsListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        uiSoundModelsListView.setAdapter(soundModelListAdapter);*/
        //TODO add onItemListClick
        voiceRequestListAdapter = new VoiceRequestListAdapter(this, voiceRequestList);

        uiVoiceRequestsListView.setAdapter(voiceRequestListAdapter);
    }

    private void fillVoiceRequestListAdapter() {
        Log.v(TAG, "fillVoiceRequestListAdapter");

        voiceRequestList.clear();
        //TODO uncomment this when we have real voiceRequests
        /*ArrayList<VoiceRequestItem> voiceRequestItems = getAllVoiceRequestItems();
        if (null == voiceRequestItems) {
            Log.v(TAG, "fillVoiceRequestListAdapter: no voice request items");
            return;
        }

        for(VoiceRequestItem voiceRequestItem : voiceRequestItems) {
            boolean checked = Global.getInstance().getSmRepo().
                    isSmNameSelected(getApplicationContext(), soundModel.getName()) ? true : false;
            Log.v(TAG, "fillSoundModelListAdapter: soundModelName= " + soundModel.getName() +
                    ", checked= " + checked);
            SoundModelListAdapter.SoundModelItem listItem = new SoundModelListAdapter.SoundModelItem(
                    checked, soundModel.getName(), soundModel.getPrettyName());
            voiceRequestList.add(listItem);
        }*/

        //TODO remove this fudge when we have real voiceRequests
        String[] smNames = {"SM1", "SM2", "SM3"};
        String[] recordTimes = {"1/1/14 12:00", "1/2/14 13:30", "1/3/14 15:00"};
        for (int i=0; i<3; i++) {
            Log.v(TAG, "fillVoiceRequestListAdapter: soundModelName= " + smNames[i] +
                    ", recordTime= " + recordTimes[i]);
            voiceRequestList.add(new VoiceRequestItem(smNames[i], recordTimes[i]));
        }
        Collections.sort(voiceRequestList);
        return;
    }

    private ArrayList<SoundModel> getAllVoiceRequestItems() {
        // TODO Auto-generated method stub
        return null;
    }
}