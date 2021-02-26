/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Sim;

import android.content.Intent;
import android.os.Bundle;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;

public class SimActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected int getLayoutId(){
        return R.layout.case_act;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
//        if (requestCode == REQUEST_EDIT_TASK) {
//            // If the task was edited successfully, go back to the list.
//            if (resultCode == ADD_EDIT_RESULT_OK) {
//                // If the result comes from the add/edit screen, it's an edit.
//                setResult(EDIT_RESULT_OK);
//                finish();
//            }
//        }
    }

}
