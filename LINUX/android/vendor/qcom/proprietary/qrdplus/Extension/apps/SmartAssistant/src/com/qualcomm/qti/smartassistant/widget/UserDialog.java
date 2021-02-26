/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.widget;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.qualcomm.qti.smartassistant.R;

public class UserDialog extends AlertDialog {
    EditText mEditor;
    public UserDialog(Context context) {
        super(context);
    }

    public static class Builder {
        private Context mContext;
        private int mTitleRes;
        private int mPositiveTextRes;
        private int mNegativeTextRes;
        private DialogInterface.OnClickListener mPositiveListener;
        private DialogInterface.OnClickListener mNegativeListener;
        private int mHintRes;
        private String mEditText;
        private boolean mEditable;

        public Builder(Context context) {
            mContext = context;
            mEditable = true;
        }

        public Builder setTitle(int titleRes) {
            mTitleRes = titleRes;
            return this;
        }

        public Builder setPositiveButton(int textRes, DialogInterface.OnClickListener listener) {
            mPositiveListener = listener;
            mPositiveTextRes = textRes;
            return this;
        }

        public Builder setNegtiveButton(int textRes, DialogInterface.OnClickListener listener) {
            mNegativeListener = listener;
            mNegativeTextRes = textRes;
            return this;
        }

        public Builder setHint(int hintRes) {
            mHintRes = hintRes;
            return this;
        }

        public Builder setEditText(String text) {
            mEditText = text;
            return this;
        }

        public Builder setEditable(boolean editable) {
            mEditable = editable;
            return this;
        }

        public UserDialog create() {
            final UserDialog dlg = new UserDialog(mContext);
            LayoutInflater inflater = (LayoutInflater)
                    mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            View layout = inflater.inflate(R.layout.dialog_user_layout, null);
            dlg.addContentView(layout, new LayoutParams(
                    LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

            if (mContext.getResources().getString(mPositiveTextRes) != null) {
                Button pButton = layout.findViewById(R.id.positiveButton);
                pButton.setText(mPositiveTextRes);
                if (null != mPositiveListener) {
                    pButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            mPositiveListener.onClick(dlg, DialogInterface.BUTTON_POSITIVE);
                        }
                    });
                }
            } else {
                layout.findViewById(R.id.positiveButton).setVisibility(View.INVISIBLE);
            }

            if (mContext.getResources().getString(mNegativeTextRes) != null) {
                Button nButton = layout.findViewById(R.id.negativeButton);
                nButton.setText(mNegativeTextRes);
                if (null != mNegativeListener) {
                    nButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            mNegativeListener.onClick(dlg, DialogInterface.BUTTON_NEGATIVE);
                        }
                    });
                }
            } else {
                layout.findViewById(R.id.negativeButton).setVisibility(View.INVISIBLE);
            }

            TextView title = layout.findViewById(R.id.user_alert_title);
            title.setText(mTitleRes);
            EditText eText = layout.findViewById(R.id.user_alert_edit_name);
            if (0 != mHintRes) {
                eText.setHint(mHintRes);
            }

            if (null != mEditText) {
                eText.setText(mEditText);
                eText.setSelection(mEditText.length());
            }

            eText.setEnabled(mEditable);

            dlg.setEdit(eText);
            dlg.setView(layout);
            return dlg;
        }
    }

    private void setEdit(EditText edit) {
        mEditor = edit;
    }

    public String getEditContent() {
        return mEditor.getText().toString();
    }

}
