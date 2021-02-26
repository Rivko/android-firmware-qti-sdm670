/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.networksetting;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.Parcel;
import android.os.Parcelable;
import android.os.RemoteException;
import android.preference.Preference;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.BidiFormatter;
import android.text.TextDirectionHeuristics;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.telephony.OperatorInfo;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;


/**
 * "Networks" preference in "Mobile network" settings UI for the Phone app.
 * It's used to manually search and choose mobile network. Enabled only when
 * autoSelect preference is turned off.
 */
public class NetworkSelectListPreference extends Preference
        implements DialogInterface.OnCancelListener {

    private static final String LOG_TAG = "networkSelect";
    private static final boolean DBG = true;

    private static final int EVENT_NETWORK_SCAN_COMPLETED = 100;

    //dialog ids
    private static final int DIALOG_NETWORK_SELECTION = 100;
    private static final int DIALOG_NETWORK_LIST_LOAD = 200;

    private static final int EVENT_AUTO_SELECT_DONE = 1;
    private static final int EVENT_NETWORK_SELECTION_DONE = 2;
    private static final int EVENT_QUERY_AVAILABLE_NETWORKS = 3;
    private static final int EVENT_NETWORK_DATA_MANAGER_DONE = 4;
    private static final int EVENT_INCREMENTAL_MANUAL_SCAN_RESULTS = 5;

    // error statuses that will be retured in the callback.
    private static final int QUERY_EXCEPTION = -1;
    private static final int QUERY_OK = 0;
    private static final int QUERY_PARTIAL = 1;
    private static final int QUERY_ABORT = 2;
    private static final int QUERY_REJ_IN_RLF = 3;

    private int mPhoneId = SubscriptionManager.INVALID_PHONE_INDEX;
    private OperatorInfo mOperatorInfo;
    private Dialog mDialog;
    private Dialog mQuitDialog;
    private int mSubId;
    private NetworkOperators mNetworkOperators;

    private NetworkSettingDataManager mDataManager = null;

    private NetworkListAdapter listAdapter;
    private ArrayList<OperatorInfo> operatorsList;
    private enum State{
        IDLE,
        SCAN
    }
    private State mState = State.IDLE;
    private HashMap<String, String> mRatMap;

    private ProgressDialog mProgressDialog;
    public NetworkSelectListPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public NetworkSelectListPreference(Context context, AttributeSet attrs, int defStyleAttr,
            int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    @Override
    protected void onClick() {
        logd("onclick of networkSelect");
        if (isDataDisableRequired()) {
            mDataManager = new NetworkSettingDataManager(getContext());
            Message onCompleteMsg = mHandler.obtainMessage(EVENT_NETWORK_DATA_MANAGER_DONE);
            mDataManager.updateDataState(false, onCompleteMsg);
        } else {
            logd("loading network");
            loadNetworksList();
        }
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar;
            switch (msg.what) {
                case EVENT_NETWORK_SCAN_COMPLETED:
                    networksListLoaded((List<OperatorInfo>) msg.obj, msg.arg1);
                    break;

                case EVENT_NETWORK_SELECTION_DONE:
                    if (DBG) logd("hideProgressPanel");
                    try {
                        dismissProgressBar();
                    } catch (IllegalArgumentException e) {
                    }
                    setEnabled(true);

                    ar = (AsyncResult) msg.obj;
                    if (ar.exception != null) {
                        if (DBG) logd("manual network selection: failed! "+ mOperatorInfo);
                        mNetworkOperators.displayNetworkSelectionFailed(ar.exception);
                    } else {
                        if (DBG) {
                            logd("manual network selection: succeeded!"
                                    + getNetworkTitle(mOperatorInfo));
                        }
                        mNetworkOperators.displayNetworkSelectionSucceeded();
                    }
                    mNetworkOperators.getNetworkSelectionMode();
                    break;

                case EVENT_NETWORK_DATA_MANAGER_DONE:
                    logd("EVENT_NETWORK_DATA_MANAGER_DONE: " + msg.arg1);
                    if (msg.arg1 == 1) {
                        loadNetworksList();
                    } else {
                        mNetworkOperators.getNetworkSelectionMode();
                    }
                    break;

                case EVENT_INCREMENTAL_MANUAL_SCAN_RESULTS:

                    /*try {
                        //mNetworkQueryService.unregisterCallback(mCallback);
                    } catch (RemoteException e) {
                        logd("EVENT_INCREMENTAL_MANUAL_SCAN_RESULTS: exception " + e);
                    }*/
                    logd("onNetworksListLoadResult : " + msg.arg1);
                    onNetworksListLoadResult((String[])msg.obj, msg.arg1);
                    break;
            }

            return;
        }
    };

    INetworkQueryService mNetworkQueryService = null;
    /**
     * This implementation of INetworkQueryServiceCallback is used to receive
     * callback notifications from the network query service.
     */
    private final INetworkQueryServiceCallback mCallback =
            new INetworkQueryServiceCallback.Stub() {

        /** place the message on the looper queue upon query completion. */
        public void onQueryComplete(List<OperatorInfo> networkInfoArray, int status) {
            if (DBG) logd("notifying message loop of query completion.");
            Message msg = mHandler.obtainMessage(EVENT_NETWORK_SCAN_COMPLETED,
                    status, 0, networkInfoArray);
            msg.sendToTarget();
        }
        public void  onIncrementalManualScanResult(String[] scanInfo, int result){
            logd("onIncrementalManualScanResult result: "+result);
            Message msg = mHandler.obtainMessage(EVENT_INCREMENTAL_MANUAL_SCAN_RESULTS,
                    result, 0, scanInfo);
            msg.sendToTarget();
        }
    };

    @Override
    //implemented for DialogInterface.OnCancelListener
    public void onCancel(DialogInterface dialog) {
        logd("onCancel " + mState);
        if (mState == State.SCAN) {
            showQuitDialog();
        }
    }

    /**
     * Return normalized carrier name given network info.
     *
     * @param ni is network information in OperatorInfo type.
     */
    public String getNormalizedCarrierName(OperatorInfo ni) {
        if (ni != null) {
            return ni.getOperatorAlphaLong() + " (" + ni.getOperatorNumeric() + ")";
        }
        return null;
    }

    // This method is provided besides initialize() because bind to network query service
    // may be binded after initialize(). In that case this method needs to be called explicitly
    // to set mNetworkQueryService. Otherwise mNetworkQueryService will remain null.
    public void setNetworkQueryService(INetworkQueryService queryService) {
        mNetworkQueryService = queryService;
    }

    // This initialize method needs to be called for this preference to work properly.
    protected void initialize(int subId, INetworkQueryService queryService,
            NetworkOperators networkOperators, ProgressDialog progressDialog) {
        mSubId = subId;
        mNetworkQueryService = queryService;
        mNetworkOperators = networkOperators;
        operatorsList = new ArrayList<OperatorInfo>();
        mRatMap = new HashMap<String, String>();
        initRatMap();
        // This preference should share the same progressDialog with networkOperators category.
        mProgressDialog = progressDialog;

        if (SubscriptionManager.from(getContext()).isActiveSubId(mSubId)) {
            mPhoneId = SubscriptionManager.getPhoneId(mSubId);
        }

        TelephonyManager telephonyManager = (TelephonyManager)
                getContext().getSystemService(Context.TELEPHONY_SERVICE);

        setSummary(telephonyManager.getNetworkOperatorName(mSubId));

    }

    @Override
    protected void onPrepareForRemoval() {
        destroy();
        super.onPrepareForRemoval();
    }

    private void showResultsDialog(){
        if (NetworkOperators.isParentWindowFinished(getContext())) {
            return;
        }

        mDialog = new Dialog(getContext());
        mDialog.setCanceledOnTouchOutside(false);
        ListView listView = new ListView(getContext());
        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle(getContext().getResources().getString(R.string.available_networks));
        listAdapter = new NetworkListAdapter
                (getContext(), android.R.layout.simple_list_item_1, operatorsList);
        listView.setAdapter(listAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                mOperatorInfo = listAdapter.getItem(position);
                if (DBG) logd("selected network: " + getNetworkTitle(mOperatorInfo));
                Message msg = mHandler.obtainMessage(EVENT_NETWORK_SELECTION_DONE);
                Phone phone = PhoneFactory.getPhone(mPhoneId);
                if (phone != null) {
                    phone.selectNetworkManually(mOperatorInfo, true, msg);
                    displayNetworkSelectionInProgress();
                } else {
                    loge("Error selecting network. phone is null.");
                }
                if(mDialog!=null && mDialog.isShowing()){
                        mDialog.dismiss();
                }
            }
        });
        builder.setView(listView);
        builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                mNetworkOperators.getNetworkSelectionMode();
            }
        });
        mDialog = builder.create();
        mDialog.show();
    }

    private void showQuitDialog() {
        mQuitDialog = new Dialog(getContext());
        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle(R.string.quit_mention).setMessage(R.string.dialog_message);
        builder.setPositiveButton(android.R.string.ok,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        // Request that the service stop the query with this callback object.
                        try {
                            if (mNetworkQueryService != null) {
                                mState = State.IDLE;
                                mNetworkQueryService.stopNetworkQuery(mCallback, mSubId);
                            }
                            // If cancelled, we query NetworkSelectMode and update states of
                            // AutoSelect button.
                            mNetworkOperators.getNetworkSelectionMode();
                        } catch (RemoteException e) {
                            loge("exception from stopNetworkQuery " + e);
                        }
                        if (mDataManager != null && isDataDisableRequired()) {
                            mDataManager.updateDataState(true, null);
                        }
                    }
                });
        builder.setNeutralButton(android.R.string.cancel,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog,int whichButton) {
                        showProgressBar(DIALOG_NETWORK_LIST_LOAD);
                        dialog.dismiss();
                    }
                });
        mQuitDialog = builder.create();
        mQuitDialog.show();
    }

    private void destroy() {
        try {
            dismissProgressBar();
        } catch (IllegalArgumentException e) {
            loge("onDestroy: exception from dismissProgressBar " + e);
        }

        try {
            if (mNetworkQueryService != null) {
                // used to un-register callback
                mNetworkQueryService.unregisterCallback(mCallback);
            }
        } catch (RemoteException e) {
            loge("onDestroy: exception from unregisterCallback " + e);
        }

        if (mDataManager != null && isDataDisableRequired()) {
            mDataManager.updateDataState(true, null);
        }
    }

    private void displayEmptyNetworkList() {
        String status = getContext().getResources().getString(R.string.empty_networks_list);

        mNetworkOperators.postTransientNotification(status);
    }

    private void displayNetworkSelectionInProgress() {
        showProgressBar(DIALOG_NETWORK_SELECTION);
    }

    private void displayNetworkQueryFailed(int error) {
        mState = State.IDLE;
        String status = getContext().getResources().getString(R.string.network_query_error);

        try {
            dismissProgressBar();
        } catch (IllegalArgumentException e1) {
            // do nothing
        }

        mNetworkOperators.postTransientNotification(status);
    }

    private void loadNetworksList() {
        if (DBG) logd("load networks list...");
        clearNetworkList();
        showProgressBar(DIALOG_NETWORK_LIST_LOAD);

        // delegate query request to the service.
        try {
            if (mNetworkQueryService != null) {
                mState = State.SCAN;
                mNetworkQueryService.startNetworkQuery(mCallback, mSubId);
            } else {
                displayNetworkQueryFailed(NetworkQueryService.QUERY_EXCEPTION);
            }
        } catch (RemoteException e) {
            loge("loadNetworksList: exception from startNetworkQuery " + e);
            displayNetworkQueryFailed(NetworkQueryService.QUERY_EXCEPTION);
        }
    }

    /**
     * networksListLoaded has been rewritten to take an array of
     * OperatorInfo objects and a status field, instead of an
     * AsyncResult.  Otherwise, the functionality which takes the
     * OperatorInfo array and creates a list of preferences from it,
     * remains unchanged.
     */
    private void networksListLoaded(List<OperatorInfo> result, int status) {
        if (DBG) logd("networks list loaded " + status);

        // used to un-register callback
        try {
            if (status == NetworkQueryService.QUERY_OK && mNetworkQueryService != null) {
                mNetworkQueryService.unregisterCallback(mCallback);
            }
        } catch (RemoteException e) {
            loge("networksListLoaded: exception from unregisterCallback " + e);
        }

        // update the state of the preferences.
        if (DBG) logd("hideProgressPanel");

        // Always try to dismiss the dialog because activity may
        // be moved to background after dialog is shown.
        try {
            dismissProgressBar();
        } catch (IllegalArgumentException e) {
            // It's not a error in following scenario, we just ignore it.
            // "Load list" dialog will not show, if NetworkQueryService is
            // connected after this activity is moved to background.
            loge("Fail to dismiss network load list dialog " + e);
        }

        try {
            if (mQuitDialog != null && mQuitDialog.isShowing()) {
                mQuitDialog.dismiss();
            }
        } catch (Exception e) {
            loge("Fail to dismiss quit dialog" + e);
        }
        setEnabled(true);

        if (status != NetworkQueryService.QUERY_OK && status != QUERY_PARTIAL) {
            if (DBG) logd("error while querying available networks");
            displayNetworkQueryFailed(status);
        } else {
            if (result != null) {
                for (int i = 0; i < result.size(); i++) {
                    operatorsList.add(result.get(i));
                }

                if(mDialog != null && mDialog.isShowing()){
                    listAdapter.notifyDataSetChanged();
                }
                else{
                    showResultsDialog();
                }
            } else {
                displayEmptyNetworkList();
            }
        }
    }

    private void onNetworksListLoadResult(final String[] result, final int status) {
        logd("result: " + status + " length: " + result.length);
        switch (status) {
            case QUERY_REJ_IN_RLF:
                // Network scan did not complete due to a radio link
                // failure recovery in progress
                mState = State.IDLE;
                clearNetworkList();
                mNetworkOperators.postTransientNotification(getContext().getResources()
                        .getString(R.string.network_query_error));
            case QUERY_ABORT:
                // Network searching time out
                // show the nw info if it bundled with this query
            case QUERY_OK:
                mState = State.IDLE;
                updateUIState();
                if (mDataManager != null) {
                    mDataManager.updateDataState(true, null);
                }
            case QUERY_PARTIAL:
                if (result != null) {
                    if (result.length >= 4 && (result.length % 4 == 0)) {
                        //if receive the whole results with QUERY_OK at one time, will
                        //split with every four to show on UI
                        logd("QUERY_PARTIAL result.length: " + result.length);
                        List<OperatorInfo> operators = new ArrayList<OperatorInfo>();
                        for (int i = 0; i < result.length / 4; i++) {
                            int j = 4 * i;
                            operators.add(new OperatorInfo(result[0 + j], result[1 + j],
                                    result[2 + j], result[3 + j]));
                        }
                        networksListLoaded(operators,status);
                    } else {
                        logd("result.length is: " + result.length);
                    }
                }
                break;
            default:
                mState = State.IDLE;
                mNetworkOperators.postTransientNotification(getContext().getResources()
                        .getString(R.string.network_query_error));
                clearNetworkList();
                updateUIState();
                if (mDataManager != null) {
                    mDataManager.updateDataState(true, null);
                }
        }
    }

    private String getLocalString(String originalString) {
        String[] origNames = getContext().getResources().
                getStringArray(R.array.original_carrier_names);
        String[] localNames = getContext().getResources().
                getStringArray(R.array.locale_carrier_names);
        for (int i = 0; i < origNames.length; i++) {
            if (origNames[i].equalsIgnoreCase(originalString)) {
                return getContext().getResources().getString(
                        getContext().getResources().getIdentifier(localNames[i],
                        "string", getContext().getPackageName()));
            }
        }
        return originalString;
    }

    private String getNetworkTitle(OperatorInfo ni) {
        String title;
        String numericOnly = "";
        String radioTech = "";
        String operatorNumeric = ni.getOperatorNumeric();

        /* operatorNumeric format: PLMN+RAT or PLMN */
        if (null != operatorNumeric) {
            String values[] = operatorNumeric.split("\\+");
            numericOnly = values[0];
            if (values.length > 1)
                radioTech = values[1];
        }

        if (!TextUtils.isEmpty(ni.getOperatorAlphaLong())) {
            title = getLocalString(ni.getOperatorAlphaLong());
        } else if (!TextUtils.isEmpty(ni.getOperatorAlphaShort())) {
            title = getLocalString(ni.getOperatorAlphaShort());
        } else
            title = numericOnly;

        if (!TextUtils.isEmpty(radioTech))
            title += " " + mRatMap.get(radioTech);

        if (ni.getState() == OperatorInfo.State.FORBIDDEN) {
            title += getContext().getResources().getString(R.string.network_forbidden);
        }

        return title;
    }

    private void initRatMap() {
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN), "Unknown");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_GPRS), "2G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_EDGE), "2G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_UMTS), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_IS95A), "2G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_IS95B), "2G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT), "2G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_0), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_A), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_HSDPA), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_HSUPA), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_HSPA), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_B), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_LTE), "4G");
        //mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_LTE_CA), "4G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_HSPAP), "3G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_GSM), "2G");
        mRatMap.put(String.valueOf(ServiceState.RIL_RADIO_TECHNOLOGY_TD_SCDMA), "3G");
    }

    private void dismissProgressBar() {
        if (mProgressDialog != null && mProgressDialog.isShowing()
                && !NetworkOperators.isParentWindowFinished(getContext())) {
            mProgressDialog.dismiss();
        }
    }

    private void showProgressBar(int id) {
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(getContext());
        } else {
            // Dismiss progress bar if it's showing now.
            dismissProgressBar();
        }

        if ((id == DIALOG_NETWORK_SELECTION) || (id == DIALOG_NETWORK_LIST_LOAD)) {
            switch (id) {
                case DIALOG_NETWORK_SELECTION:
                    final String networkSelectMsg = getContext().getResources()
                            .getString(R.string.register_on_network,
                                    getNetworkTitle(mOperatorInfo));
                    mProgressDialog.setMessage(networkSelectMsg);
                    mProgressDialog.setCanceledOnTouchOutside(false);
                    mProgressDialog.setCancelable(false);
                    mProgressDialog.setIndeterminate(true);
                    break;
                case DIALOG_NETWORK_LIST_LOAD:
                    mProgressDialog.setMessage(
                            getContext().getResources().getString(
                            R.string.load_networks_progress));
                    mProgressDialog.setCanceledOnTouchOutside(false);
                    mProgressDialog.setCancelable(true);
                    mProgressDialog.setIndeterminate(false);
                    mProgressDialog.setOnCancelListener(this);
                    break;
                default:
            }
            mProgressDialog.show();
        }
    }


    @Override
    protected Parcelable onSaveInstanceState() {
        final Parcelable superState = super.onSaveInstanceState();
        if (isPersistent()) {
            // No need to save instance state since it's persistent
            return superState;
        }
        final SavedState myState = new SavedState(superState);
        myState.mOperatorInfoList = operatorsList;
        return myState;
    }

    @Override
    protected void onRestoreInstanceState(Parcelable state) {
        if (state == null || !state.getClass().equals(SavedState.class)) {
            // Didn't save state for us in onSaveInstanceState
            super.onRestoreInstanceState(state);
            return;
        }

        SavedState myState = (SavedState) state;

        if (operatorsList == null && myState.mOperatorInfoList != null) {
            operatorsList = myState.mOperatorInfoList;
        }

        super.onRestoreInstanceState(myState.getSuperState());
    }

    /**
     *  We save entries, entryValues and operatorInfoList into bundle.
     *  At onCreate of fragment, dialog will be restored if it was open. In this case,
     *  we need to restore entries, entryValues and operatorInfoList. Without those information,
     *  onPreferenceChange will fail if user select network from the dialog.
     */
     //TODO to save the results state. This need to be aligned to Qti model
    private static class SavedState extends BaseSavedState {
        ArrayList<OperatorInfo> mOperatorInfoList;

        SavedState(Parcel source) {
            super(source);
            final ClassLoader boot = Object.class.getClassLoader();
            mOperatorInfoList.addAll(source.readParcelableList(mOperatorInfoList, boot));
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            super.writeToParcel(dest, flags);
            dest.writeParcelableList(mOperatorInfoList, flags);
        }

        SavedState(Parcelable superState) {
            super(superState);
        }

        public static final Parcelable.Creator<SavedState> CREATOR =
                new Parcelable.Creator<SavedState>() {
                    public SavedState createFromParcel(Parcel in) {
                        return new SavedState(in);
                    }

                    public SavedState[] newArray(int size) {
                        return new SavedState[size];
                    }
                };
    }

    private boolean isDataDisableRequired() {
        boolean isRequired = getContext().getResources().getBoolean(
                         R.bool.config_disable_data_manual_plmn);
        Phone phone = PhoneFactory.getPhone(mPhoneId);
        if((TelephonyManager.getDefault().getMultiSimConfiguration()
                == TelephonyManager.MultiSimVariants.DSDA) &&
               (SubscriptionManager.getDefaultDataSubscriptionId() != phone.getSubId())){
                isRequired = false;
        }
        logd("isDataDisableRequired : " + isRequired);
        return isRequired;
    }

    private void clearNetworkList() {
        if (listAdapter != null && operatorsList != null) {
            operatorsList.clear();
            listAdapter.notifyDataSetChanged();
        }
    }

    private void updateUIState() {
        switch (mState) {
            case IDLE:
                dismissProgressBar();
                break;
            case SCAN:
                displayNetworkSelectionInProgress();
                break;
        }
    }

    private void logd(String msg) {
        Log.d(LOG_TAG, "[NetworksList] " + msg);
    }


    private void loge(String msg) {
        Log.e(LOG_TAG, "[NetworksList] " + msg);
    }

    public class NetworkListAdapter extends ArrayAdapter<OperatorInfo> {
        ArrayList<OperatorInfo> operatorList = new ArrayList<>();
        Context mContext;

        public NetworkListAdapter(Context context,
                int textViewResourceId,  ArrayList<OperatorInfo> objects) {
            super(context, textViewResourceId, objects);
            operatorList = objects;
            mContext = context;
        }

        @Override
        public int getCount() {
            return super.getCount();
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            LinearLayout.LayoutParams llp =
                    new LinearLayout.LayoutParams(
                    LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
            llp.setMargins(20, 40, 20, 40);
            TextView tv = new TextView(mContext);
            OperatorInfo operator = operatorList.get(position);
            tv.setText(getNetworkTitle(operator));
            tv.setLayoutParams(llp);
            LinearLayout layout = new LinearLayout(mContext);
            layout.addView(tv);
            convertView = layout;
            return convertView;
        }
    }
}
