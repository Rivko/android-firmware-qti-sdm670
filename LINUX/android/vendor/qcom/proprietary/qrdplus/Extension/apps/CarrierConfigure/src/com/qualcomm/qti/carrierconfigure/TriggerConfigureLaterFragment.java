/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.Activity;
import android.app.Fragment;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.Loader;
import android.content.Loader.OnLoadCompleteListener;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.os.RegionalizationEnvironment;
import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Actions.ActionCallback;
import com.qualcomm.qti.carrierconfigure.Actions.SwitchCarrierTask;
import com.qualcomm.qti.carrierconfigure.Actions.UpdateNVItemsTask;
import com.qualcomm.qti.carrierconfigure.Carrier.CarriersStorage;
import com.qualcomm.qti.carrierconfigure.Carrier.EmptyPathException;
import com.qualcomm.qti.carrierconfigure.Carrier.NullServiceException;
import com.qualcomm.qti.carrierconfigure.Carrier.SwitchData;
import com.qualcomm.qti.carrierconfigure.Utils.CopyAndUnzipOtaFileThread;
import com.qualcomm.qti.carrierconfigure.Utils.MyAlertDialog;
import com.qualcomm.qti.carrierconfigure.Utils.MyNoticeDialog;
import com.qualcomm.qti.carrierconfigure.Utils.WaitDialog;
import com.qualcomm.qti.loadcarrier.ILoadCarrierService;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class TriggerConfigureLaterFragment extends Fragment implements OnClickListener{
    private static final String TAG = "TriggerStartFragment";

    private static final String ARG_CARRIER_LIST = "carrier_list";
    private static final int REQUEST_START_TRIGGER = 1;

    private static final String RE_MNC = "(\\d{2,3})?";
    private static final String SEP_COMMA = ",";

    private static final int RETRY_DELAY = 500;
    private static final int RETRY_COUNT = 10;

    private static final int MSG_START_SWITCH = 0;
    private static final int MSG_SWITCH_ERROR = 1;
    private static final int MSG_COPY_AND_UNZIPFILE = 2;

    private static final int UPDATE_ROW_NV_ITEMS_FINISH = 1;

    private static final int SHOW_ASK_BUTTON_TIMES_INIT = 0;
    private static final int SHOW_ASK_BUTTON_TIMES_MAX = SHOW_ASK_BUTTON_TIMES_INIT + 3;

    private int mMagicValue;
    private ArrayList<Carrier> mCarriers;

    private int mSubMask = 0;
    private String mConfigId = null;
    private TextView message = null;
    public  static String carrierName = null;
    private LinearLayout mCarriersContainer;

    private int mRetryTimes = 0;
    private Carrier mSelectedCarrier;
    private Carrier mSwitchToDefaultCarrier = null;
    private CarriersLoader mLoader = null;
    private CarriersStorage mCarriersStorage = null;
    private OnLoadCompleteListener<CarriersStorage> mLoaderListener =
            new OnLoadCompleteListener<CarriersStorage>() {
        @Override
        public void onLoadComplete(Loader<CarriersStorage> loader, CarriersStorage storage) {
            mCarriersStorage = storage;
        }
    };

    private ILoadCarrierService mLoadCarrierService = null;
    private ICarrierAccessCacheService mAccessCacheService = null;
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            if (Utils.DEBUG) Log.i(TAG, "Service Connected to " + name.getShortClassName());
            if (name.getShortClassName().equals(".LoadCarrierService")) {
                mLoadCarrierService = ILoadCarrierService.Stub.asInterface(service);
            } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = ICarrierAccessCacheService.Stub.asInterface(service);
            }
            if (mLoader != null) {
                mLoader.notifyCarrierLoadServiceChanged(mLoadCarrierService, mAccessCacheService);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            if (name.getShortClassName().equals(".LoadCarrierService")) {
                mLoadCarrierService = null;
            } else if (name.getShortClassName().equals(".CarrierAccessCacheService")) {
                mAccessCacheService = null;
            }
            if (mLoader != null) {
                mLoader.notifyCarrierLoadServiceChanged(mLoadCarrierService, mAccessCacheService);
            }
        }
    };

    public TriggerConfigureLaterFragment() {
    }

    public static TriggerConfigureLaterFragment newInstance(ArrayList<Carrier> carriers) {
        final TriggerConfigureLaterFragment fragment = new TriggerConfigureLaterFragment();

        final Bundle args = new Bundle(1);
        args.putParcelableArrayList(ARG_CARRIER_LIST, carriers);
        fragment.setArguments(args);

        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Try to bind the service.
        if (mServiceConnection != null) {
            if (mLoadCarrierService == null) {
                // Bind the service to get the carriers stored in SD card.
                Intent intent = new Intent(ILoadCarrierService.class.getName());
                intent.setPackage(ILoadCarrierService.class.getPackage().getName());
                getActivity().bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
            if (mAccessCacheService == null) {
                // Bind the service to access cache dir.
                Intent intent = new Intent(ICarrierAccessCacheService.class.getName());
                intent.setPackage(ICarrierAccessCacheService.class.getPackage().getName());
                getActivity().bindService(intent, mServiceConnection,
                        Context.BIND_AUTO_CREATE | Context.BIND_ALLOW_OOM_MANAGEMENT);
            }
        }

        mLoader = new CarriersLoader(getActivity(), mLoadCarrierService, mAccessCacheService);
        mLoader.registerListener(0, mLoaderListener);
        mLoader.startLoading();

        mCarriers = getArguments().getParcelableArrayList(ARG_CARRIER_LIST);
        mMagicValue = mCarriers.hashCode();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle bundle) {
        View rootView = inflater.inflate(R.layout.trigger_configuage_later_fragment, container, false);
        mCarriersContainer = (LinearLayout) rootView.findViewById(R.id.carriers_container_view);
        rootView.findViewById(R.id.btn_no).setOnClickListener(this);
        if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
            buildCarriersView_fileBased(mCarriersContainer);
        } else {
            buildCarriersView(mCarriersContainer);
        }
        return rootView;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_no://do not ask again
                //updateROWNVItems();
                SystemProperties.set("persist.radio.trigger", "false");
                Intent MyIntent1 = new Intent(Intent.ACTION_MAIN);
                MyIntent1.addCategory(Intent.CATEGORY_HOME);
                startActivity(MyIntent1);
                 getActivity().setResult(Activity.RESULT_OK);
                 ConfigurationActivity.onDestroyfinish();
                break;
            default:
                //dusen  ask again duing next
                getActivity().setResult(Activity.RESULT_OK);
                Intent MyIntent = new Intent(Intent.ACTION_MAIN);
                MyIntent.addCategory(Intent.CATEGORY_HOME);
                startActivity(MyIntent);
                ConfigurationActivity.onDestroyfinish();
                break;
        }
    }

    // "*_fileBased" is used to distinguish the carrier switching through preset package
    // and file-based ota.zip with block-based ota.zip.
    private void buildCarriersView_fileBased(LinearLayout parentView) {

            Button btn = new Button(getActivity());
            btn.setText(R.string.configure_now_later);
            btn.setBackgroundResource(R.color.primary_color);
            btn.setAllCaps(false);
            btn.setSingleLine();
            btn.setTextAppearance(getActivity(), android.R.attr.textAppearanceMedium);
            btn.setOnClickListener(this);
            parentView.addView(btn,
                    new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
    }

    private void buildCarriersView(LinearLayout parentView) {
        ArrayList<String> carriers = Carrier.getCurrentCarriers();
        String currentCarrierName = Carrier.getCurrentCarriersName(carriers);

        travelCarriers(parentView, travelCarriers(parentView, currentCarrierName));
    }

    private String travelCarriers(LinearLayout parentView, String fullName) {
        if (fullName == null) return null;
        boolean needScanAgain = !Carrier.getBaseCarrierName(fullName).equals("Default");
        for (int i = 0; i < mCarriers.size(); i++) {
            Carrier carrier = mCarriers.get(i);
            if (!carrier.getBaseCarrierName().equals(Carrier.getSwitchCarrierName(fullName)))
                    continue;
            if (!needScanAgain && mSwitchToDefaultCarrier != null && carrier.mName.equals("Default"))
                    continue;

            // Init the button view.
            Button btn = new Button(getActivity());
            btn.setText(getCarrierDisplay(carrier));
            btn.setTextAppearance(getActivity(), android.R.attr.textAppearanceMedium);
            btn.setTag(carrier);
            btn.setId(mMagicValue + i);
            btn.setOnClickListener(this);
            if (carrier.mName.endsWith("2Default")) {
                if (mCarriers.size() > 1) {
                    btn.setVisibility(View.GONE);
                }
            } else {
                btn.setEnabled(!(carrier.mName.equalsIgnoreCase(Carrier.getCurrentCarriersName(
                        Carrier.getCurrentCarriers()))));
            }

            // Add this button to parent view.
            parentView.addView(btn,
                    new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

            if (!needScanAgain && carrier.getSwitchCarrierName().equals("Default")
                     && !"Default".equals(fullName)) {
                needScanAgain = true;
                mSwitchToDefaultCarrier = carrier;
            }
        }
        if (needScanAgain && mSwitchToDefaultCarrier != null)
            return mSwitchToDefaultCarrier.getSwitchCarrierName();
        else return null;

    }

    private String getCarrierDisplay(Carrier carrier) {
        if (carrier == null) return null;

        if (carrier.mName.endsWith(Utils.CARRIER_TO_DEFAULT_NAME)) {
            return carrier.getSwitchCarrierName();
        }

        String target = carrier.getTarget();
        String brand =  carrier.getBrand();

        String display = TextUtils.isEmpty(brand) ? carrier.getTopCarrierTitle() : brand;
        if (!TextUtils.isEmpty(carrier.getShowSimSpn())
                && (carrier.getShowSimSpn().equalsIgnoreCase("true"))) {
            if (Utils.DEBUG) Log.d(TAG, "Try to display SPN instead of name/brand: " + display);
            int subMask = carrier.getSubMask();
            if (subMask != Utils.INVALID_SUB_MASK) {
                TelephonyManager telephonyManager = (TelephonyManager) getActivity()
                        .getSystemService(Context.TELEPHONY_SERVICE);
                int phoneCount = telephonyManager.getPhoneCount();
                int slotId = Utils.INVALID_SLOT_ID;
                for (int i=0; i<phoneCount; i++) {
                    if ((subMask & (0x0001 << i)) != 0) {
                        slotId = i;
                        break;
                    }
                }
                if (slotId != Utils.INVALID_SLOT_ID) {
                    /*String spn = NewUtils.getSimOperatorNameForSubscription(
                            SubscriptionManager.getSubId(slotId)[0]);*/
                    String spn = telephonyManager.getSimOperatorNameForPhone(
                            SubscriptionManager.getPhoneId(SubscriptionManager.getSubId(slotId)[0]));
                    if (!TextUtils.isEmpty(spn)) {
                        display = spn;
                        if (Utils.DEBUG) Log.d(TAG, "Display SPN: " + display);
                    }
                }
            }
        }
        if (!TextUtils.isEmpty(target)) {
            display = display + " - " + target;
        }
        return display;
    }

}
