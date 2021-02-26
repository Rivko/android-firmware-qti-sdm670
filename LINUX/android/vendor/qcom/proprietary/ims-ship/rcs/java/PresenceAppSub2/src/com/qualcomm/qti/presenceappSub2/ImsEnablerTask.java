/*************************************************************************
 Copyright (c)2012-2013,2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*************************************************************************/

package com.qualcomm.qti.presenceappSub2;



import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;



public class ImsEnablerTask extends AsyncTask<Void, Integer, Object> {

    final String TAG = "Presence_UI2: ImsEnablerTask";

    Context mContext;
    ProgressDialog dialog;

    public ImsEnablerTask() {}

    public ImsEnablerTask(Context appContext) {
        mContext = appContext;
    }

    @Override
        protected void onPreExecute() {
            super.onPreExecute();

            initProgressBar();
        }

    private void initProgressBar() {
        dialog = new ProgressDialog(mContext);
        dialog.setMessage("API Request in progress...");
        dialog.setCancelable(false);

        dialog.show();

    }

    @Override
        protected Object doInBackground(Void... params) {
            Log.d(TAG, "doInBackground(), Thread="+
                    Thread.currentThread().getName());

            if(Looper.myLooper() == null ) {
                Looper.prepare();
            }

            return sendImsEnablerRequest();

        }


    private Object sendImsEnablerRequest() {
        Log.d(TAG, "sendImsEnablerRequest");

        return AppGlobalState.getImsEnablerState();
    }

    @Override
        protected void onProgressUpdate(Integer... values) {

            super.onProgressUpdate(values);

            dialog.incrementProgressBy(values[0]);
        }

    @Override
        protected void onPostExecute(Object obj) {
            super.onPostExecute(obj);
            Log.d(TAG, "onPostExecute(), Thread="+
                    Thread.currentThread().getName());

            dialog.dismiss();

            Toast.makeText(mContext, mContext.getResources().getString(
                    R.string.ims_enabler_toast_text) + AppGlobalState.getImsEnablerState(),
                    Toast.LENGTH_SHORT).show();
        }
}
