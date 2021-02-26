/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.customui;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public abstract class AbstractCancelDialogFragment extends DialogFragment {

    private final int cancelDialogStringId;
    private final int cancelStayStringId;
    private final int cancelLeaveStringId;

    protected AbstractCancelDialogFragment(int cancelDialogStringId, int cancelStayStringId,
            int cancelLeaveStringId) {
        super();

        this.cancelDialogStringId = cancelDialogStringId;
        this.cancelStayStringId = cancelStayStringId;
        this.cancelLeaveStringId = cancelLeaveStringId;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        LayoutInflater inflater = getActivity().getLayoutInflater();
        AlertDialog dialog = builder.create();

        View view = inflater.inflate(R.layout.alert_dialog_custom, null);
        TextView dialogTextView = (TextView)view.findViewById(R.id.alert_dialog_title_text);
        dialogTextView.setText(getString(cancelDialogStringId));
        Button button1 = (Button)view.findViewById(R.id.alert_dialog_btn1);
        button1.setText(getString(cancelStayStringId));
        Button button2 = (Button)view.findViewById(R.id.alert_dialog_btn2);
        button2.setText(getString(cancelLeaveStringId));
        dialog.setView(view, 0, 0, 0, 0);

        button1.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                getDialog().dismiss();
            }
        });

        button2.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                getActivity().finish();
            }
        });

        return dialog;

    }

}
