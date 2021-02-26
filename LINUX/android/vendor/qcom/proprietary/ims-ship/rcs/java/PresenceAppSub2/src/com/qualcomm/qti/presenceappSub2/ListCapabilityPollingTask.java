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
import android.os.Handler;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;

public class ListCapabilityPollingTask extends AsyncTask<Void, Integer, Integer> {
    final private static String TAG = "Presence_UI2: ListSubscribeSimpleTask";

    public static String prefix = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            + "<resource-lists xmlns=\"urn:ietf:params:xml:ns:resource-lists\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            + "<list name=\"dummy-rfc5367\">";
    public static String suffix = "</list>" + "</resource-lists>";
    Context mContext;
    ProgressDialog dialog;

    public ListCapabilityPollingTask() {
    }

    public ListCapabilityPollingTask(Context appContext) {

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

        AppGlobalState.setProgressDialog(dialog);

        dialog.show();

    }

    @Override
    protected Integer doInBackground(Void... params) {
        Log.d(TAG, "doInBackground(), Thread=" + Thread.currentThread().getName());

        if (Looper.myLooper() == null) {
            Looper.prepare();
        }

        ArrayList<String> uriList = getSelectedContactsList();

        if (uriList.size() == 0) {
            Log.d(TAG, "None of the contact selected for list subscription");
            return 0;
        }

        return sendListSubscribePollingRequest(uriList);
    }

    private ArrayList<String> getSelectedContactsList() {
        ArrayList<Contact> contacts = AppGlobalState.getContacts();
        ArrayList<String> uriList = new ArrayList<String>();
        for (Contact c : contacts) {
            if (c.isMultiSelected()) {
                String phone = c.getPhone();
                if (phone!= null && AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
                {
                    uriList.add(prepareCompleteUri(phone));
                }
                else if(phone !=null)
                {
                    uriList.add(prepareCompleteUriForAtt(c));
                }
            }
        }
        return uriList;
    }


   private String checkPhoneFormat(String number) {
      if(number.startsWith("+")){
         return number;
      } else {
         number = "+"+number;
         return number;
      }
   }
    private String prepareCompleteUri(String phone) {
        // Modified to enable sip uri. need to update back to tell uri.
        phone = "tel:" + checkPhoneFormat(phone);
        return phone;
    }

    private String prepareCompleteUriForAtt(Contact contact) {
        // Modified to enable sip uri. need to update back to tell uri.
        String phone = contact.getContactUri();

        if((phone == null) || (phone != null && phone.length() == 0))
        {
            phone = contact.getResourceUri();

        }
        if(phone != null && phone.length() > 0)
        {
            if(phone.equals("<Not Subscribed>"))
            {
                if(!contact.getIndividualContactURI().equals(""))
                {
                    phone = contact.getIndividualContactURI();
                }
                else
                {
                    phone = "tel:" + checkPhoneFormat(contact.getPhone());
                }
            }
        }
        else
        {
            if(!contact.getIndividualContactURI().equals(""))
            {
                phone = contact.getIndividualContactURI();
            }
            else
            {
                phone = "tel:" + checkPhoneFormat(contact.getPhone());
            }
        }

        return phone;
    }

    private Integer sendListSubscribePollingRequest(ArrayList<String> contactList) {
        Log.d(TAG, "sendListSubscribePollingRequest for " + contactList);

        String [] stringContactList = null;

        int standardXmlLength = 1;
        if(Settings.maxSubscriptionListEntries > 0)
        {
            standardXmlLength = Settings.maxSubscriptionListEntries;
        }
        else
        {
            standardXmlLength = 100;
        }
        Log.d(TAG, "standardXmlLength =  " + standardXmlLength);
        int noOfIterations = contactList.size() / standardXmlLength;

        Log.d(TAG, "noOfIterations =  " + noOfIterations);
        int contactPosition = 0;
        for (int count = 0; count <= noOfIterations; count++) {
            Log.d(TAG, "count =  " + count);
            if (count == noOfIterations)
            {
                standardXmlLength = contactList.size() % standardXmlLength;
                Log.d(TAG, "standardXmlLength =  " + standardXmlLength);
            }
            stringContactList = new String[standardXmlLength];

            for (int i = 0; i < standardXmlLength; i++) {

                Log.d(TAG, "contactPosition =  " + contactPosition);
                stringContactList[i] = contactList.get(contactPosition);
                Log.d(TAG, "stringContactList[i] =  " + stringContactList[i]);
                contactPosition++;
            }
            if (count != noOfIterations) {

                Log.d(TAG, "Before QPresService_GetContactListCap Inside");
                try {
                if(AppGlobalState.getPresenceService() != null && AppGlobalState.isCdByOption() == false)
                {
                    Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : AppGlobalState.getPresenceSerrviceHandle() "+AppGlobalState.getPresenceSerrviceHandle());
                    Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : stringContactList "+stringContactList.toString());
                    RequestInfo requestinfoObject = new RequestInfo();
                    requestinfoObject.URI = new String[stringContactList.length];
                    for(int i =0; i < stringContactList.length; i++ )
                    {
                        requestinfoObject.URI[i] = stringContactList[i];
                    }
                    requestinfoObject.userData = AppGlobalState.getpUserDataValue();
                    AppGlobalState.requestinfo.add(requestinfoObject);
                    AppGlobalState.listCapPollingRequestinfo.add(requestinfoObject);
                    Log.d("PRESENCE_UI", "ListCapabilityPollingTask : AppGlobalState.getpUserDataValue() "+AppGlobalState.getpUserDataValue());
                    if(AppGlobalState.listCapPollingRequestinfo.size() == 1)
                    {
                        StatusCode status = AppGlobalState.getPresenceService().getContactListCap(AppGlobalState.getPresenceSerrviceHandle(), stringContactList, AppGlobalState.getpUserData());
                        Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : status.getStatusCode() "+status.getStatusCode());
                    }
                    else
                    {
                        Log.d(TAG,"ListCapabilityPollingTask added in queue");
                    }
                }
                else if(AppGlobalState.getCdService() != null && AppGlobalState.isCdByOption())
                {
                    StatusCode status = AppGlobalState.getCdService().getContactListCap(AppGlobalState.getCdServiceHandle(), stringContactList, AppGlobalState.getpUserData());
                    Log.d(TAG, "ListAvailabilityFetchTask : QCDService_RequestGivenContactsCDInfo :  status.getStatusCode() "+ status.getStatusCode());
                    return status.getStatusCode();
                }
                else
                {
                    Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : AppGlobalState.getPresenceService =  NULL");
                    return -1;
                }
            } catch (RemoteException e) {
                e.printStackTrace();
                return -2;
            } catch (Exception e) {
                e.printStackTrace();
                return -3;
            }
                Log.d(TAG, "After QPresService_GetContactListCap");
            }

        }
        if(stringContactList != null)
        {
            Log.d(TAG, "stringContactList " + stringContactList.length);
            if (stringContactList.length <= 0)
            {
                return 0;
            }
            else
            {
                Log.d(TAG, "Before RETURN QPresService_GetContactListCap");
                try {
                if(AppGlobalState.getPresenceService() != null && AppGlobalState.isCdByOption() == false)
                {
                    Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : AppGlobalState.getPresenceSerrviceHandle() "+AppGlobalState.getPresenceSerrviceHandle());
                    Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : stringContactList "+stringContactList.toString());
                    RequestInfo requestinfoObject = new RequestInfo();
                    requestinfoObject.URI = new String[stringContactList.length];
                    for(int i =0; i < stringContactList.length; i++ )
                    {
                        requestinfoObject.URI[i] = stringContactList[i];
                    }
                    requestinfoObject.userData = AppGlobalState.getpUserDataValue();
                    AppGlobalState.requestinfo.add(requestinfoObject);
                    AppGlobalState.listCapPollingRequestinfo.add(requestinfoObject);
                    //As userdata gets post incremented in getPUserData() value, so printing it will change the value, so commenting
                    //Log.d("PRESENCE_UI", "ListCapabilityPollingTask : AppGlobalState.getpUserData() " + AppGlobalState.getpUserData());
                    Log.d("PRESENCE_UI", "ListCapabilityPollingTask : AppGlobalState.getpUserDataValue() "+AppGlobalState.getpUserDataValue());
                    if(AppGlobalState.listCapPollingRequestinfo.size() == 1)
                    {
                        StatusCode status = AppGlobalState.getPresenceService().getContactListCap(AppGlobalState.getPresenceSerrviceHandle(), stringContactList, AppGlobalState.getpUserData());
                        Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : status.getStatusCode() "+status.getStatusCode());
                        return status.getStatusCode();
                    }
                    else
                    {
                        Log.d(TAG,"ListCapabilityPollingTask added in queue");
                        return 0;
                    }
                }
                else if(AppGlobalState.getCdService() != null && AppGlobalState.isCdByOption())
                {
                    StatusCode status = AppGlobalState.getCdService().getContactListCap(AppGlobalState.getCdServiceHandle(), stringContactList, AppGlobalState.getpUserData());
                    Log.d(TAG, "ListAvailabilityFetchTask : QCDService_RequestGivenContactsCDInfo :  status.getStatusCode() "+ status.getStatusCode());
                    return status.getStatusCode();
                }
                else
                {
                    Log.d(TAG, "ListCapabilityPollingTask : sendListSubscribePollingRequest : AppGlobalState.getPresenceService =  NULL");
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
        }
        else
        {
        return -4;
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

        if (getSelectedContactsList().size() == 0) {
            Toast.makeText(mContext, "None of the contact selected for list subscription.",
                    Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(mContext, "List Subscribe Polling Result =" +
                    result, Toast.LENGTH_SHORT).show();
        }
    }

    public static int subscriptionExpireTime = 30;
    public static Handler listCapPollinghandler = new Handler();
    public static long lastListCapSend = System.currentTimeMillis();
    private static Runnable listCapPollingRunnable = new Runnable() {

        public void run() {
            Log.d(TAG,"inside listCapPollingRunnable size "+AppGlobalState.listCapPollingRequestinfo.size());
            if(AppGlobalState.listCapPollingRequestinfo.size() > 1 && AppGlobalState.getPresenceService() != null)
            {
                try
                {
                    AppGlobalState.listCapPollingRequestinfo.remove(0);
                    sendListCapPollingFromQueue();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            else
            {
                if(AppGlobalState.listCapPollingRequestinfo.size() == 1)
                {
                    if(AppGlobalState.listCapPollingRequestinfo.get(0).requestID == 0)
                    {
                        Log.d(TAG,"inside listCapPollingRunnable size 1 and request id 0");
                        sendListCapPollingFromQueue();
                    }
                    else
                    {
                        Log.d(TAG,"inside listCapPollingRunnable size 1 and request id not 0");
                        AppGlobalState.listCapPollingRequestinfo.remove(0);
                    }
                }
            }
        }
    };

    public static int sendListCapPollingFromQueue()
    {
        try
        {
            Log.d(TAG,"inside sendListCapPollingFromQueue size "+AppGlobalState.listCapPollingRequestinfo.size());
            if (AppGlobalState.listCapPollingRequestinfo.size() > 0 && AppGlobalState.getPresenceService() != null)
            {
                StatusCode status = AppGlobalState.getPresenceService()
                        .getContactListCap(AppGlobalState.getPresenceSerrviceHandle(),
                                AppGlobalState.listCapPollingRequestinfo.get(0).URI,
                                AppGlobalState.listCapPollingRequestinfo.get(0).userData);
                Log.d(TAG,
                        "ListCapabilityPollingTask : sendListSubscribePollingRequest : status.getStatusCode() "
                                + status.getStatusCode());
                return status.getStatusCode();
            }
            else
            {
                Log.d(TAG,"sendListCapPollingFromQueue -0");
                return 0;

            }
        } catch (RemoteException e) {
            e.printStackTrace();
            Log.d(TAG,"sendListCapPollingFromQueue -2");
            return -2;
        } catch (Exception e) {
            e.printStackTrace();
            Log.d(TAG,"sendListCapPollingFromQueue -3");
            return -3;
        }
    }
}