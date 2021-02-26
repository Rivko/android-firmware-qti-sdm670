/*************************************************************************
 Copyright (c)2012-2013,2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*************************************************************************/

package com.qualcomm.qti.presenceappSub2;

import java.util.ArrayList;

import com.android.ims.internal.uce.common.StatusCode;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;
import android.content.SharedPreferences;

public class AvailabilityFetchTask extends AsyncTask<Void, Integer, Integer> {
    final String TAG = "Presence_UI2: AvailabilityFetchTask";
    static final int AVAILABILITY_FETCH_NOT_SENT = 5;

    int mContactIndex;
    Context mContext;
    ProgressDialog dialog;

    AvailabilityFetchTask me;

    public AvailabilityFetchTask() {
    }

    public AvailabilityFetchTask(Context appContext, int contactIndex) {

        mContext = appContext;
        mContactIndex = contactIndex;
        me = this;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();

        initProgressBar();
    }

    private void initProgressBar() {
        dialog = new ProgressDialog(mContext);
        dialog.setMessage("API Request in progress...");
        dialog.setCancelable(true);

        AppGlobalState.setProgressDialog(dialog);

        dialog.show();

    }

    @Override
    protected Integer doInBackground(Void... params) {
        Log.d(TAG, "doInBackground(), Thread=" + Thread.currentThread().getName());

        if (Looper.myLooper() == null) {
            Looper.prepare();
        }

        ArrayList<Contact> contacts = AppGlobalState.getContacts();
        String phone = contacts.get(mContactIndex).getPhone();
        Log.d(TAG, "phone" + phone);

        String uri = contacts.get(mContactIndex).getContactUri();
        Log.d(TAG, "Contact URI" + uri);

        if (uri.length() == 0)
        {
            Log.d(TAG,
                    "getContactUri is empty, So use getResourceUri for Availability fetch");
            uri = contacts.get(mContactIndex).getResourceUri();
        }

        if (uri.length() > 0) {
            ArrayList<String> contactList = new ArrayList<String>();
            if ((AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE
                || AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
                && uri.equals("<Not Subscribed>"))
            {
      Log.d(TAG, "Mode:"+AppGlobalState.getOperatorMode());
                if(!contacts.get(mContactIndex).getIndividualContactURI().equals(""))
                {
                    uri = contacts.get(mContactIndex).getIndividualContactURI();
                }
                else
                {
                    uri = "tel:"+phone;
                }
                Log.d(TAG, "Not Subscribe uri");
                if(AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE) {
         uri = prepareCompleteUri(phone);
         Log.d(TAG, "For TMO URI:"+uri);
                }
            }
            contactList.add(uri);

            return sendSubscribePollingRequest(contactList);
        } else {
            return AVAILABILITY_FETCH_NOT_SENT;
        }
    }

    private String prepareCompleteUri(String phone) {
        SharedPreferences setting = getSharedPrefHandle(AppGlobalState.IMS_PRESENCE_MY_INFO);
        String uri1Value = setting.getString(
                mContext.getString(R.string.uri1text), "");
        String uri2Value = setting.getString(
                mContext.getString(R.string.uri2text), "");
        phone = "" + (uri1Value != null ? uri1Value : "") + phone;
        phone = phone + (uri2Value != null ? uri2Value : "");
        Log.d(TAG, "For TMO prepareCompleteUri:"+phone);
        return phone;
    }
    private SharedPreferences getSharedPrefHandle(String imsPresencePref) {
        SharedPreferences settings = mContext
                .getSharedPreferences(imsPresencePref, 0);
        return settings;
    }
    private Integer sendSubscribePollingRequest(ArrayList<String> contactList) {
        Log.d(TAG, "aks2 sendSubscribePollingRequest for " + contactList.get(0));

        try {
            if(AppGlobalState.getPresenceService()!=null && AppGlobalState.isCdByOption() == false)
            {
                Log.d(TAG, "aks2 AvailabilityFetchTask : sendSubscribePollingRequest : AppGlobalState.getPresenceSerrviceHandle() "+AppGlobalState.getPresenceSerrviceHandle());
                Log.d(TAG, "AvailabilityFetchTask : sendSubscribePollingRequest : contactList.get(0) "+contactList.get(0));
                RequestInfo requestinfoObject = new RequestInfo();
                requestinfoObject.URI = new String[1];
                requestinfoObject.URI[0] = contactList.get(0);
                requestinfoObject.userData = AppGlobalState.getpUserDataValue();
                AppGlobalState.requestinfo.add(requestinfoObject);
                StatusCode status = AppGlobalState.getPresenceService().getContactCap(AppGlobalState.getPresenceSerrviceHandle(), contactList.get(0), AppGlobalState.getpUserData());
                Log.d(TAG, "AvailabilityFetchTask : sendSubscribePollingRequest : status "+status.getStatusCode());
                return status.getStatusCode();
            }
            else if(AppGlobalState.getCdService() != null && AppGlobalState.isCdByOption())
            {
              StatusCode status = AppGlobalState.getCdService().getContactCap(AppGlobalState.getCdServiceHandle(), contactList.get(0), AppGlobalState.getpUserData());
              Log.d(TAG, "ListAvailabilityFetchTask : QCDService_RequestCdInfo :  status.getStatusCode() "+ status.getStatusCode());
              return status.getStatusCode();
            }
            else
            {
                Log.d(TAG, "AvailabilityFetchTask : sendSubscribePollingRequest : return o");
                return -1;
            }
        } catch (RemoteException e) {
            e.printStackTrace();
            return -2;
        } catch (Exception e) {
            e.printStackTrace();
            return -3;
        }
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
        super.onProgressUpdate(values);
    }

    @Override
    protected void onPostExecute(Integer result) {
        super.onPostExecute(result);
        Log.d(TAG, "onPostExecute(), Thread=" + Thread.currentThread().getName());

        dialog.dismiss();
        if (result != 0) { // failure
            // dialog.dismiss();
        } else { // success
            updateContactSubscriptionFlag();
            Utility.rescheduleSubcribeTimer(mContactIndex,
                    AppGlobalState.getContacts().get(mContactIndex));
        }

        if (result == AVAILABILITY_FETCH_NOT_SENT)
        {
            Toast.makeText(mContext, "Availability fetch not send due to empty URI",
                    Toast.LENGTH_SHORT).show();
        } else {

            Toast.makeText(mContext, "Subscribe Polling Result =" +
                    result, Toast.LENGTH_SHORT).show();
        }
    }

    private void updateContactSubscriptionFlag() {

        ArrayList<Contact> contacts = AppGlobalState.getContacts();

        Contact temp = contacts.get(mContactIndex);
        temp.setSubscriptionOnFlag(false);
    }
}
