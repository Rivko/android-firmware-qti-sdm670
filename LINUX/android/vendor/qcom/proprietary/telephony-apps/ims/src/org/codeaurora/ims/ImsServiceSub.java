/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import java.io.IOException;
import java.util.Iterator;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.RejectedExecutionException;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import com.android.ims.ImsCallProfile;
import com.android.ims.ImsConfig;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsServiceClass;
import com.android.ims.internal.IImsCallSessionListener;
import com.android.ims.internal.IImsRegistrationListener;
import com.android.ims.internal.IImsEcbm;
import com.android.ims.internal.IImsMultiEndpoint;
import com.android.internal.telephony.CallForwardInfo;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.QtiImsExtBase.QtiImsExtBinder;
import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.vt.ImsVideoGlobals;

import org.codeaurora.ims.internal.IQtiImsExtListener;
import org.codeaurora.ims.utils.QtiImsExtUtils;

import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.Parcelable;
import android.os.RemoteCallbackList;
import android.os.UserHandle;
import android.telephony.ServiceState;
import android.telephony.ims.feature.ImsFeature;
import android.telephony.ims.feature.MMTelFeature;
import android.app.PendingIntent;
import android.content.Context;
import android.location.Address;
import android.location.Geocoder;
import android.os.AsyncResult;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Pair;
import android.widget.Toast;

public class ImsServiceSub extends MMTelFeature
        implements ImsSubController.ImsStackConfigListener {
    private static final String LOG_TAG = "ImsServiceSub";
    protected ImsSenderRxr mCi = null; /* Commands Interface */
    private ImsSubController mSubController;
    // service class --> table
    private Map<Integer, ImsServiceClassTracker> mTrackerTable =
        new HashMap<Integer, ImsServiceClassTracker>();
    // service id --> tracker table
    private Map<Integer, ImsServiceClassTracker> mServiceIdTable =
        new HashMap<Integer, ImsServiceClassTracker>();
    private Handler mHandler;
    private Handler mRegCallbackHandler;
    private HandlerThread mRegHandlerThread;
    private Context mContext;
    private ImsEcbmImpl mImsEcbmImpl;
    private ImsConfigImpl mImsConfigImpl;
    private ImsUtImpl mImsUtImpl;
    private ServiceStatus mServiceStatus[] = null;
    private CommandsInterface mRilCommandsInterface = null;
    private ImsMultiEndpointImpl mImsMultiEndpointImpl = null;
    private ImsServiceStateReceiver mImsServiceStateReceiver =  null;
    private HashSet<Uri> mSelfIndentityUris = null;

    // Determines if this ImsServiceSub is ready for requests.
    private boolean mFeatureIsOpen = false;

    private boolean mIsVopsEnabled = false;
    private boolean mIsSsacVoiceBarred = false;
    private final int SSAC_VOICE_BARRING_ZERO = 0;

    //This is the error that we get from the modem
    //for IKEv2 Authentication failure(24).
    private static final int IKEv2_AUTH_FAILURE = 5;

    // Thread pool for invoking runnable tasks, such as GeocoderTask.
    private static ExecutorService sTaskExecutor = Executors.newCachedThreadPool();

    //Cached list for IQtiImsExtListener listeners registered for vice.
    private CopyOnWriteArrayList<IQtiImsExtListener>
            mQtiImsInterfaceListeners = new CopyOnWriteArrayList<IQtiImsExtListener>();

    //Cached list for IQtiImsExtListener listeners registered for participant status.
    private CopyOnWriteArrayList<IQtiImsExtListener>
            mQtiImsParticipantStatusListeners = new CopyOnWriteArrayList<IQtiImsExtListener>();

    // VOIP, VT_TX, VT_RX, VT, UT
    private static final int SERVICE_TYPE_MAX = 5;
    private final int MAX_FEATURES_SUPPORTED = 3;

    private final int EVENT_CALL_STATE_CHANGE = 1;
    private final int EVENT_IMS_STATE_CHANGED = 2;
    private final int EVENT_IMS_STATE_DONE = 3;
    private final int EVENT_SRV_STATUS_UPDATE = 4;
    private final int EVENT_GET_SRV_STATUS = 5;
    private final int EVENT_SET_SRV_STATUS = 6;
    private final int EVENT_GET_CURRENT_CALLS = 7;
    private final int EVENT_SUPP_SRV_UPDATE = 8;
    private final int EVENT_SET_IMS_STATE = 9;
    private final int EVENT_TTY_STATE_CHANGED = 10;
    private final int EVENT_REFRESH_CONF_INFO = 11;
    //Event that gets triggered for intra RAT HandOver's
    private final int EVENT_HANDOVER_STATE_CHANGED = 12;
    private final int EVENT_CALL_MODIFY = 13;
    private final int EVENT_MWI = 14;
    private final int EVENT_CONFERENCE = 15;
    private final int EVENT_SET_CALL_FORWARD_TIMER = 16;
    private final int EVENT_GET_CALL_FORWARD_TIMER = 17;
    private static final int EVENT_GET_PACKET_COUNT = 18;
    private static final int EVENT_GET_PACKET_ERROR_COUNT = 19;
    private final int EVENT_CALL_DEFLECT = 20;
    private final int EVENT_SRVCC_STATE_CHANGED = 21;
    private final int EVENT_GEOLOCATION_INFO_REQUEST = 22;
    private final int EVENT_GEOLOCATION_RESPONSE = 23;
    private final int EVENT_VOWIFI_CALL_QUALITY_UPDATE = 24;
    private final int EVENT_CALL_TRANSFER = 25;
    private final int EVENT_VOPS_CHANGED = 26;
    private final int EVENT_SSAC_CHANGED = 27;
    private final int EVENT_VOPS_RESPONSE = 28;
    private final int EVENT_SSAC_RESPONSE = 29;
    private final int EVENT_PARTICIPANT_STATUS_INFO = 30;
    private final int EVENT_SET_VOLTE_PREF = 31;
    private final int EVENT_GET_VOLTE_PREF = 32;
    private final int EVENT_GET_HANDOVER_CONFIG = 33;
    private final int EVENT_SET_HANDOVER_CONFIG = 34;
    private final int EVENT_REGISTRATION_BLOCK_STATUS = 35;
    private final int EVENT_CANCEL_MODIFY_CALL = 36;

    static final int CF_REASON_UNCONDITIONAL    = 0;

    // TODO: Change this to an ArrayList/Map of service class trackers.
    private ImsServiceClassTracker mConferenceRequestedbyTracker;

    protected int mPhoneId = SubscriptionManager.INVALID_PHONE_INDEX;
    int[] mEnabledFeatures = {
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN
    };
    int[] mDisabledFeatures = {
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN,
            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN
    };

    private Mwi mMwiResponse;

    // service class based registration listeners. Concurrency safe and automatically cleans up
    // dead listeners upon binder death. When a listener has died, also unregister from service id
    // map.
    private RemoteCallbackList<IImsRegistrationListener> mSrvClassRegListeners =
            new RemoteCallbackList<IImsRegistrationListener>() {
                @Override
                public void onCallbackDied(IImsRegistrationListener callback) {
                    // Clean up references to mSessionIdRegListenerTable as well when listener dies.
                    if (callback == null) {
                        return;
                    }
                    Iterator<Map.Entry<Integer, IImsRegistrationListener>> it;
                    for (it = mSessionIdRegListenerTable.entrySet().iterator(); it.hasNext();) {
                        Map.Entry<Integer, IImsRegistrationListener> entry = it.next();
                        if (entry.getValue() != null
                                && callback.asBinder().equals(entry.getValue().asBinder())) {
                            Log.i(LOG_TAG, "ImsRegistrationListener Died. Removing listener ("
                                    + entry.getValue() + ") from service id " + entry.getKey());
                            it.remove();
                        }
                    }
                }
            };

    // Session Id --> IImsRegistrationListener. Used to keep track of listener that was added by
    // the session id.
    private Map<Integer, IImsRegistrationListener> mSessionIdRegListenerTable =
        new ConcurrentHashMap<Integer, IImsRegistrationListener>();

    private RegistrantList mCapabilitiesChangedRegistrants = new RegistrantList();

    //Constructor
    public ImsServiceSub(Context context, int phoneId, ImsSenderRxr senderRxr,
            ImsSubController subController, CommandsInterface csCommandsInterface) {
        setFeatureState(ImsFeature.STATE_INITIALIZING);
        mPhoneId = phoneId;
        mContext = context;
        mCi = senderRxr;
        mSubController = subController;
        mSubController.registerListener(this);
        Log.i(LOG_TAG, "[phoneId=" + mPhoneId +
                "] Constructor :: Registering with Sub Controller.");
        mHandler = new ImsServiceSubHandler();

        mRegHandlerThread = new HandlerThread(this + "RegHandlerThread");
        mRegHandlerThread.start();
        mRegCallbackHandler = new Handler(mRegHandlerThread.getLooper());
        mCi.registerForImsNetworkStateChanged(mHandler, EVENT_IMS_STATE_CHANGED,
                null);
        // Query for registration state in case we have missed the UNSOL
    //    mCi.getImsRegistrationState(mHandler.obtainMessage(EVENT_IMS_STATE_DONE));
        mCi.registerForSrvStatusUpdate(mHandler, EVENT_SRV_STATUS_UPDATE, null);
        mCi.registerForCallStateChanged(mHandler, EVENT_CALL_STATE_CHANGE, null);
        mCi.registerForRefreshConfInfo(mHandler, EVENT_REFRESH_CONF_INFO, null);
        mCi.registerForHandoverStatusChanged(mHandler, EVENT_HANDOVER_STATE_CHANGED, null);
        mCi.registerForGeolocationRequest(mHandler, EVENT_GEOLOCATION_INFO_REQUEST, null);
        mCi.registerForVopsStatusChanged(mHandler, EVENT_VOPS_CHANGED, null);
        mCi.registerForSsacStatusChanged(mHandler, EVENT_SSAC_CHANGED, null);
        mCi.registerForParticipantStatusInfo(mHandler, EVENT_PARTICIPANT_STATUS_INFO, null);
        mCi.registerForRegistrationBlockStatus(mHandler, EVENT_REGISTRATION_BLOCK_STATUS, null);

        mRilCommandsInterface = csCommandsInterface;
        mRilCommandsInterface.registerForSrvccStateChanged(mHandler,
                    EVENT_SRVCC_STATE_CHANGED, null);

        mCi.registerForVoWiFiCallQualityUpdate(mHandler,
                EVENT_VOWIFI_CALL_QUALITY_UPDATE, null);

        mImsEcbmImpl = new ImsEcbmImpl(this, mCi);

        //Initialize the UT interface associated with the sub.
        mImsUtImpl = new ImsUtImpl(this, mCi, mContext);

        //Initialize the IMS Config interface associated with the sub.
        mImsConfigImpl =  new ImsConfigImpl(this, mCi, mContext);

        //Initialize the MultiEndpoint interface
        mImsMultiEndpointImpl = new ImsMultiEndpointImpl(mCi, mContext, this);

        // For listening to incoming (MT) Hold/Resume UNSOLs.
        mCi.setOnSuppServiceNotification(mHandler, EVENT_SUPP_SRV_UPDATE, null);

        // For listening to MWI UNSOLs.
        mCi.registerForMwi(mHandler, EVENT_MWI, null);
        mMwiResponse = new Mwi();

        // For listening to TTY mode change UNSOL.
        mCi.registerForTtyStatusChanged(mHandler, EVENT_TTY_STATE_CHANGED, null);

        mCi.registerForModifyCall(mHandler, EVENT_CALL_MODIFY, null);
        initServiceStatus();

        //Initialize the ImsServiceStateReceiver associated with this sub.
        mImsServiceStateReceiver = new ImsServiceStateReceiver(this, mContext, mPhoneId);
        mContext.registerReceiver(mImsServiceStateReceiver, new IntentFilter(
                Intent.ACTION_SIM_STATE_CHANGED));
        // Explicitly remove the HD icon in notification bar to handle
        // phone process crash.
        mImsServiceStateReceiver.updateHDIcon(false, false);
        mCi.queryServiceStatus(mHandler.obtainMessage(EVENT_GET_SRV_STATUS));

    }

    public void dispose() {
        setFeatureState(ImsFeature.STATE_NOT_AVAILABLE);
        Log.d(LOG_TAG, "dispose");
        if (mRegHandlerThread != null) {
            mRegHandlerThread.quitSafely();
            mRegHandlerThread = null;
            mRegCallbackHandler = null;
        }
        mImsMultiEndpointImpl = null;
        mContext.unregisterReceiver(mImsServiceStateReceiver);
        mImsServiceStateReceiver = null;
    }

    @Override
    public void onStackConfigChanged(boolean[] activeStacks) {
        try {
            Log.i(LOG_TAG, "onStackConfigChanged :: activeStacks["
                    + mPhoneId + "]=" + activeStacks[mPhoneId]);
            setFeatureState(activeStacks[mPhoneId] == true ?
                ImsFeature.STATE_READY : ImsFeature.STATE_NOT_AVAILABLE);
        } catch (ArrayIndexOutOfBoundsException ex) {
            Log.e(LOG_TAG, "onStackConfigChanged :: Invalid activeStacks length!");
        }
    }

    /* Method to initialize the Service related objects */
    private void initServiceStatus() {
        mServiceStatus = new ServiceStatus[SERVICE_TYPE_MAX];
        for (int i = 0; i < SERVICE_TYPE_MAX; i++) {
            mServiceStatus[i] = new ServiceStatus();
            /*
             * By default, the assumption is the service is enabled on LTE - when RIL and modem
             * changes to update the availability of service on power up this will be removed
             */
            mServiceStatus[i].accessTechStatus = new ServiceStatus.StatusForAccessTech[1];
            mServiceStatus[i].accessTechStatus[0] = new ServiceStatus.StatusForAccessTech();
            mServiceStatus[i].accessTechStatus[0].networkMode = ImsQmiIF.RADIO_TECH_LTE;
            mServiceStatus[i].accessTechStatus[0].status = ImsQmiIF.STATUS_NOT_SUPPORTED;
            mServiceStatus[i].accessTechStatus[0].registered = ImsQmiIF.Registration.NOT_REGISTERED;
            mServiceStatus[i].status = ImsQmiIF.STATUS_NOT_SUPPORTED;
        }
    }

    public void sendConferenceRequest(ImsServiceClassTracker sct) {
        mConferenceRequestedbyTracker = sct;

        // Send the conference request to lower layers.
        mCi.conference(mHandler.obtainMessage(EVENT_CONFERENCE, this));
    }

    private void handleConferenceResponse(AsyncResult ar) {
        mConferenceRequestedbyTracker.handleConferenceResult(ar);
    }

    private void handleSrvccResponse(AsyncResult ar) {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null && ar.exception == null) {
            tracker.calculateOverallSrvccState((int[]) ar.result);
        } else {
            Log.e(this,"Error EVENT_SRVCC_STATE_CHANGED tracker is null or srvcc exception "
                    + ar.exception);
        }
    }

    /**
     * Method to gather the currently available calls from the available trackers.
     */
    private Object getCallsListToClear() {
        Object mmTelCallsList = null;
        ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
        if (tracker == null) {
            Log.e (this, "Call for non-registered service class MMTEL");
        } else {
            // Only IMS calls will go to MMTEL tracker
            mmTelCallsList = tracker.getCallsListToClear();
        }

        return mmTelCallsList;
    }

    /**
     * Creates/updates the tracker object for the service class
     * @param serviceClass a service class specified in {@link ImsServiceClass}
     *      For VoLTE service, it MUST be a {@link ImsServiceClass#MMTEL}.
     * @param incomingCallPendingIntent When an incoming call is received,
     *        the IMS service will call {@link PendingIntent#send(Context, int, Intent)} to
     *        send back the intent to the caller with {@link #INCOMING_CALL_RESULT_CODE}
     *        as the result code and the intent to fill in the call ID; It cannot be null
     * @param listener To listen to IMS registration events; It cannot be null
     * @return Unique identifier
     */
    private int getSessionId(int serviceClass, PendingIntent intent,
            IImsRegistrationListener listener) {
        ImsServiceClassTracker tracker = mTrackerTable.get(serviceClass);
        if (tracker == null) {
            tracker = new ImsServiceClassTracker(serviceClass, intent, mCi, mContext, this);
            tracker.updateFeatureCapabilities(isVideoSupported(), isVoiceSupported());
            mTrackerTable.put(serviceClass, tracker);
            mServiceIdTable.put(tracker.getServiceId(), tracker);
        } else {
            tracker.setIncomingCallIntent(intent);
        }
        int sessionId = tracker.getServiceId();
        //Listener passed here through startSession is dummy and will be removed from AOSP soon.
        //For the first time, addRegistrationListener will register the listener and store it
        //in the table. However it will be called only once by frameworks per subscription.
        //So in the case of 7+5 configuration, after DDS switch, this stored listener need to
        //be registered again.
        IImsRegistrationListener regListener
                = mSessionIdRegListenerTable.get(sessionId);
        if (regListener != null) {
            Log.i(this, "getSessionId :: addRegistrationListener " + regListener);
            addRegistrationListener(regListener);
        }
        notifyFeatureCapabilityChange();
        Log.i(this, "getSessionId :: Returning sessionId = " + sessionId);
        mCi.getImsRegistrationState(mHandler.obtainMessage(EVENT_IMS_STATE_DONE));
        return sessionId;
    }

    /**
     * Notifies the MMTel feature that you would like to start a session. This should always be
     * done before making/receiving IMS calls. The IMS service will register the device to the
     * operator's network with the credentials (from ISIM) periodically in order to receive calls
     * from the operator's network. When the IMS service receives a new call, it will send out an
     * intent with the provided action string. The intent contains a call ID extra
     * {@link IImsCallSession#getCallId} and it can be used to take a call.
     *
     * @param incomingCallIntent When an incoming call is received, the IMS service will call
     * {@link PendingIntent#send} to send back the intent to the caller with
     * {@link #INCOMING_CALL_RESULT_CODE} as the result code and the intent to fill in the call ID;
     * It cannot be null.
     * @param listener To listen to IMS registration events; It cannot be null
     * @return an integer (greater than 0) representing the session id associated with the session
     * that has been started.
     */
    @Override
    public int startSession(PendingIntent incomingCallIntent, IImsRegistrationListener listener) {
        if (isOpened()) {
           Log.e(LOG_TAG, "startSession :: Unable to handle multiple clients..");
           return -1;
        }
        int sessionId = getSessionId(ImsServiceClass.MMTEL, incomingCallIntent, listener);
        mFeatureIsOpen = true;
        Log.d (LOG_TAG, "startSession :: Returning session ID=" + sessionId);
        return sessionId;
    }

    /**
     * Closes the specified service ({@link ImsServiceClass}) not to make/receive calls.
     * All the resources that were allocated to the service are also released.
     *
     * @param sessionId a session id to be closed which is obtained from {@link ImsManager#open}
     */
    @Override
    public void endSession(int sessionId) {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null) {
            //endSession will just unregister the listener so that it can be fetched later
            //to register again after DDS switch.
            IImsRegistrationListener listener
                    = mSessionIdRegListenerTable.get(sessionId);
            if (listener != null) {
                removeRegistrationListener(listener);
            }
            mFeatureIsOpen = false;
            Log.d(LOG_TAG, "endSession :: Removed registration listener and closed feature");
        } else {
            Log.e(LOG_TAG, "endSession :: No ImsServiceClassTracker for MMTEL!");
        }
    }

    /**
     * Checks if the IMS service has successfully registered to the IMS network
     * with the specified service & call type.
     *
     * @param serviceType a service type that is specified in {@link ImsCallProfile}
     *        {@link ImsCallProfile#SERVICE_TYPE_NORMAL}
     *        {@link ImsCallProfile#SERVICE_TYPE_EMERGENCY}
     * @param callType a call type that is specified in {@link ImsCallProfile}
     *        {@link ImsCallProfile#CALL_TYPE_VOICE_N_VIDEO}
     *        {@link ImsCallProfile#CALL_TYPE_VOICE}
     *        {@link ImsCallProfile#CALL_TYPE_VT}
     *        {@link ImsCallProfile#CALL_TYPE_VS}
     * @return true if the specified service id is connected to the IMS network;
     *        false otherwise
     */
    @Override
    public boolean isConnected(int serviceType, int callType) {
        boolean supported = false;
        switch (serviceType) {
            case ImsCallProfile.SERVICE_TYPE_EMERGENCY:
                supported = true;
                // TODO: Check for emergency call type support?
                break;
            case ImsCallProfile.SERVICE_TYPE_NORMAL:
                switch (callType) {
                    case ImsCallProfile.CALL_TYPE_VOICE_N_VIDEO:
                        supported = isVideoSupported() && isVoiceSupported();
                        break;
                    case ImsCallProfile.CALL_TYPE_VT:
                    case ImsCallProfile.CALL_TYPE_VS:
                        supported = isVideoSupported();
                        break;
                    case ImsCallProfile.CALL_TYPE_VOICE:
                        supported = isVoiceSupported();
                }
        }
        Log.d(LOG_TAG, "isConnected :: serviceType=" + serviceType
                + " callType=" + callType + " supported=" + supported);
        return supported;
    }

    /**
     * Checks if the specified IMS service is opened.
     *
     * @return true if the service is opened; false otherwise
     */
    @Override
    public boolean isOpened() {
        return mFeatureIsOpen;
    }

    /**
     * Create a call profile for the call type
     * @param sessionId
     * @param serviceType
     * @param callType
     * @return ImsCallProfile object if successful or null.
     */
    @Override
    public ImsCallProfile createCallProfile(int sessionId, int serviceType, int callType) {
        ImsCallProfile profile = null;
        ImsServiceClassTracker tracker = mServiceIdTable.get(new Integer(sessionId));
        if (tracker == null) {
            Log.e(this, " Invalid service ID - cannot create profile");
        } else {
            //TODO: Check if IMS is registered
            //TODO: Check if callType is supported i.e UNSOL_SRV_STATUS_UPDATE
            profile = new ImsCallProfile(serviceType, callType);
        }
        return profile;
    }

    /**
     * Create a call session
     * @param sessionId
     * @param profile
     * @param listener
     * @return IImsCallSession object or null on failure
     */
    @Override
    public ImsCallSessionImpl createCallSession(int sessionId, ImsCallProfile profile,
            IImsCallSessionListener listener) {
        ImsCallSessionImpl session = null;
        ImsServiceClassTracker tracker = mServiceIdTable.get(new Integer(sessionId));
        if (tracker == null) {
            Log.e(this, "Invalid sessionId - cannot create Call Session " + sessionId);
        } else {
            session = tracker.createCallSession(profile, listener);
        }
        return session;
    }

    @Override
    public void addRegistrationListener(IImsRegistrationListener listener) {
        if (listener == null) {
            Log.e(this, "addRegistrationListener :: null listener");
            return;
        }
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null) {
            int sessionId = tracker.getServiceId();
            IImsRegistrationListener regListener
                    = mSessionIdRegListenerTable.get(sessionId);
            if (regListener == null) {
                Log.i(this, "addRegistrationListener :: add listener to the table " + listener);
                mSessionIdRegListenerTable.put(sessionId, listener);
            }
        }
        if (!mSrvClassRegListeners.register(listener)) {
            Log.w(this, "addRegistrationListener :: Listener (" + listener
                    + ") could not be registered.");
        }
    }

    @Override
    public void removeRegistrationListener(IImsRegistrationListener listener) {
        if (listener == null) {
            Log.e(this, "removeRegistrationListener :: null listener");
            return;
        }
        Log.i(this, "removeRegistrationListener :: listener = " + listener);
        if (!mSrvClassRegListeners.unregister(listener)) {
            Log.w(this, "removeRegistrationListener :: Listener(" + listener
                    + ") could not be unregistered.");
        }
    }

    /** Get a pending call's session
     * @param sessionId
     * @param callId
     * @return IImsCallSession associated with pending call or null on failure
     */
    @Override
    public ImsCallSessionImpl getPendingCallSession(int sessionId, String callId) {
        ImsCallSessionImpl session = null;
        ImsServiceClassTracker tracker = mServiceIdTable.get(new Integer(sessionId));
        if(tracker == null) {
            Log.e(this, "Invalid sessionId - cannot get pending session " + sessionId);
        } else {
            session = tracker.getCallSession(callId);
        }
        return session;
    }

    @Override
    public void setUiTTYMode(int uiTtyMode, Message onComplete) {
        mCi.setUiTTYMode(uiTtyMode, onComplete);
    }

    /**
     * Get the UT interface handle.
     * @return IImsUt interface handle.
     */
    @Override
    public ImsUtImpl getUtInterface() {
        return mImsUtImpl;
    }

    /**
     * Get the Config interface handle.
     * @return IImsConfig interface handle.
     */
    @Override
    public ImsConfigImpl getConfigInterface() {
        return mImsConfigImpl;
    }

    /**
     * Get the ECBM interface handle.
     * @return IImsEcbm interface handle.
     */
    @Override
    public IImsEcbm getEcbmInterface() {
        return mImsEcbmImpl;
    }

    @Override
    public void onFeatureRemoved() {
        final int NOT_USED = 1;
        Log.d(this, "onFeatureRemoved :: Calling endSession to close service");
        endSession(NOT_USED);
    }

    /**
     * Used for turning on IMS when its in OFF state.
     */
    @Override
    public void turnOnIms() {
        mCi.sendImsRegistrationState(ImsQmiIF.Registration.REGISTERED,
                mHandler.obtainMessage(EVENT_SET_IMS_STATE));
    }

    /**
     * Used for turning off IMS when its in ON state. When IMS is OFF, device will behave as
     * CSFB'ed.
     */
    @Override
    public void turnOffIms() {
        mCi.sendImsRegistrationState(ImsQmiIF.Registration.NOT_REGISTERED,
                mHandler.obtainMessage(EVENT_SET_IMS_STATE));
    }

    /**
     * Local utility to start a new thread and then call feature capability call back
     * @param tracker object that contains handle to feature capability call back
     */
    private void notifyFeatureCapabilityChange() {
        final Runnable r = new Runnable() {
            @Override
            public void run() {
                int count = mSrvClassRegListeners.beginBroadcast();
                try {
                    for (int i = 0; i < count; i++) {
                        try {
                            mSrvClassRegListeners.getBroadcastItem(i)
                                    .registrationFeatureCapabilityChanged(ImsServiceClass.MMTEL,
                                            mEnabledFeatures, mDisabledFeatures);
                        } catch (RemoteException e) {
                            Log.w(this, e + " " + "notifyFeatureCapabilityChange() - Skipping " +
                                    "callback.");
                        }
                    }
                } finally {
                    mSrvClassRegListeners.finishBroadcast();
                }
            }
        };
        if (mRegCallbackHandler != null) {
            mRegCallbackHandler.post(r);
        }
    }

    void onIccLoaded() {
        Log.d(this, "onIccLoaded...");
        notifyFeatureCapabilityChange();
    }

    private boolean isSrvTypeValid(int type) {
        // checking UT condition separately
        if (type == ImsQmiIF.CALL_TYPE_UT) {
            return true;
        } else {
            return ((type < ImsQmiIF.CALL_TYPE_VOICE) || (type > ImsQmiIF.CALL_TYPE_VT)) ? false
                    : true;
        }
    }

    private void notifyVoiceMessageCountUpdate(final int msgCount) {
        final Runnable r = new Runnable() {
            @Override
            public void run() {
                int count = mSrvClassRegListeners.beginBroadcast();
                try {
                    for (int i = 0; i < count; i++) {
                        try {
                            mSrvClassRegListeners.getBroadcastItem(i)
                                    .voiceMessageCountUpdate(msgCount);
                        } catch (RemoteException e) {
                            Log.w(this, e + " " + "notifyVoiceMessageCountUpdate() - Skipping " +
                                    "callback.");
                        }
                    }
                } finally {
                    mSrvClassRegListeners.finishBroadcast();
                }
            }
        };
        if (mRegCallbackHandler != null) {
            mRegCallbackHandler.post(r);
        }
    }

    private void resetFeatures() {
        for (int i = 0; i < mEnabledFeatures.length; i++) {
            mEnabledFeatures[i] = ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN;
            mDisabledFeatures[i] = ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN;
        }
    }

    private void handleSrvStatusUpdate(ArrayList<ServiceStatus> updateList) {
        boolean isVideoEnabled = false;
        resetFeatures();
        for (ServiceStatus update : updateList) {
            Log.i(this, "type = " + update.type + " status = " + update.status
                    + " isValid = " + update.isValid);
            if (update != null && update.isValid && isSrvTypeValid(update.type)) {
                ServiceStatus srvSt = null;
                if (update.type == ImsQmiIF.CALL_TYPE_UT) {
                    srvSt = mServiceStatus[SERVICE_TYPE_MAX -1];
                } else {
                    srvSt = mServiceStatus[update.type];
                }
                srvSt.isValid = update.isValid;
                srvSt.type = update.type;
                if (update.userdata != null) {
                    srvSt.userdata = new byte[update.userdata.length];
                    srvSt.userdata = Arrays.copyOf(update.userdata, update.userdata.length);
                }
                if (update.accessTechStatus != null && update.accessTechStatus.length > 0) {
                    srvSt.accessTechStatus = new ServiceStatus.StatusForAccessTech[update.
                            accessTechStatus.length];
                    Log.i(this, "Call Type " + srvSt.type + "has num updates = "
                            + update.accessTechStatus.length);
                    ServiceStatus.StatusForAccessTech[] actSt = srvSt.accessTechStatus;

                    for (int i = 0; i < update.accessTechStatus.length; i++) {
                        ServiceStatus.StatusForAccessTech actUpdate =
                                update.accessTechStatus[i];
                        Log.i(this, "StatusForAccessTech networkMode = "
                                + actUpdate.networkMode
                                + " registered = " + actUpdate.registered
                                + " status = " + actUpdate.status
                                + " restrictCause = " + actUpdate.restrictCause);
                        actSt[i] = new ServiceStatus.StatusForAccessTech();
                        actSt[i].networkMode = actUpdate.networkMode;
                        actSt[i].registered = actUpdate.registered;
                        if (actUpdate.status == ImsQmiIF.STATUS_ENABLED &&
                                actUpdate.restrictCause != CallDetails.CALL_RESTRICT_CAUSE_NONE) {
                            actSt[i].status = ImsQmiIF.STATUS_PARTIALLY_ENABLED;
                        } else {
                            actSt[i].status = actUpdate.status;
                        }
                        srvSt.status = actSt[i].status;
                        actSt[i].restrictCause = actUpdate.restrictCause;
                        int feature = ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN;
                        boolean modeWifi = actSt[i].networkMode == ImsQmiIF.RADIO_TECH_WIFI ||
                                actSt[i].networkMode == ImsQmiIF.RADIO_TECH_IWLAN;
                        boolean modeLte = actSt[i].networkMode == ImsQmiIF.RADIO_TECH_ANY ||
                                actSt[i].networkMode == ImsQmiIF.RADIO_TECH_LTE;
                        if (modeWifi || modeLte) {
                            if (update.type == ImsQmiIF.CALL_TYPE_VOICE) {
                                feature = modeLte ?
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE :
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI;
                            } else if (update.type == ImsQmiIF.CALL_TYPE_UT) {
                                 feature = modeLte ?
                                         ImsConfig.FeatureConstants.FEATURE_TYPE_UT_OVER_LTE:
                                         ImsConfig.FeatureConstants.FEATURE_TYPE_UT_OVER_WIFI;
                            } else { // VT_TX, VT_RX, VT
                                feature = modeLte ?
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE :
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI;
                            }
                            if (actSt[i].status == ImsQmiIF.STATUS_ENABLED ||
                                    actSt[i].status == ImsQmiIF.STATUS_PARTIALLY_ENABLED) {
                                mEnabledFeatures[feature] = feature;
                                mDisabledFeatures[feature] =
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN;
                                if (feature ==
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
                                        feature ==
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
                                    isVideoEnabled = true;
                                }
                                Log.i(this, "enabledFeature = " + feature);
                            } else if (actSt[i].status == ImsQmiIF.STATUS_DISABLED ||
                                    actSt[i].status == ImsQmiIF.STATUS_NOT_SUPPORTED) {
                                // VT is enabled if VT_TX, or VT_RX, or VT is enabled
                                if (!(isVideoEnabled == true && (feature ==
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
                                        feature ==
                                        ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI))) {
                                    mDisabledFeatures[feature] = feature;
                                    mEnabledFeatures[feature] =
                                            ImsConfig.FeatureConstants.FEATURE_TYPE_UNKNOWN;
                                    Log.i(this, "disabledFeature = " + feature);
                                }
                            }
                        }
                    }
                }
            }
        }
        notifyFeatureCapabilityChange();
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null) {
            tracker.updateFeatureCapabilities(isVideoSupported(), isVoiceSupported());
        } else {
            Log.e(this,"handleSrvStatusUpdate tracker is null so not updating global VT capability");
        }

        mImsServiceStateReceiver.updateHDIcon(isVideoSupported(), isVoiceSupported());

        // Notify MultiEndPoint listeners on voice/video capability change.
        // This comes in form of service status change.
        if (mCapabilitiesChangedRegistrants != null) {
            Pair<Boolean, Boolean> result =
                     new Pair<Boolean, Boolean>(isVideoSupported(), isVoiceSupported());
            mCapabilitiesChangedRegistrants.notifyRegistrants(new AsyncResult(null, result, null));
        } else {
            Log.e(this, "handleSrvStatusUpdate :: mCapabilitiesChangedRegistrants null");
        }
    }

    public boolean isVideoSupported() {
        return mEnabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE]
                    == ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE
                || mEnabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI]
                    == ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI;
    }

    public boolean isVoiceSupported() {
        return mEnabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE]
                    == ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE
                || mEnabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI]
                    == ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI;
    }

    public boolean isVideoSupportedOverWifi() {
        return mEnabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI]
                == ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI;
    }

    public boolean isVoiceSupportedOverWifi() {
        return mEnabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI]
                == ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI;
    }

    public void updateLowBatteryStatus() {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null) {
            tracker.updateLowBatteryStatus();
        }
    }

    public void setCallForwardUncondTimer(int startHour, int startMinute, int endHour,
        int endMinute, int action, int condition, int serviceClass, String number,
                IQtiImsExtListener listener) {
        Log.i(this, "setCallForwardUncondTimer");
        mCi.setCallForwardUncondTimer(startHour, startMinute, endHour, endMinute,
                action, condition, serviceClass, number,
                mHandler.obtainMessage(EVENT_SET_CALL_FORWARD_TIMER, listener));
    }

    public void getCallForwardUncondTimer(int reason, int serviceClass,
            IQtiImsExtListener listener) {
        Log.i(this, "getCallForwardUncondTimer reason is"+ reason
                + "serviceClass = " + serviceClass);
        mCi.queryCallForwardStatus(reason, serviceClass, null,
                mHandler.obtainMessage(EVENT_GET_CALL_FORWARD_TIMER, listener));
    }

    public void getPacketCount(IQtiImsExtListener listener) {
        mCi.getPacketCount(mHandler.obtainMessage(EVENT_GET_PACKET_COUNT,
                listener));
    }

    public void getPacketErrorCount(IQtiImsExtListener listener) {
        mCi.getPacketErrorCount(mHandler.obtainMessage(EVENT_GET_PACKET_ERROR_COUNT,
                listener));
    }

    public void sendCallDeflectRequest(String deflectNumber,
                IQtiImsExtListener listener) {
        /* Get the callsession for incoming call */
        List<ImsCallSessionImpl> sessionList =
                getCallSessionByState(DriverCallIms.State.INCOMING);
        if (sessionList.isEmpty()) {
            /* If not incoming call, then get the callsession for waiting call */
            sessionList = getCallSessionByState(DriverCallIms.State.WAITING);
        }

        if (sessionList.isEmpty()) {
            /* No call to deflect, notify error */
            Log.i (this, "sendCallDeflectRequest: no call is available to deflect");
            if (listener != null) {
                try {
                    listener.receiveCallDeflectResponse(mPhoneId,
                            QtiImsExtUtils.QTI_IMS_REQUEST_ERROR);
                } catch (Throwable t) {
                    Log.e(this, "sendCallDeflectRequest exception!");
                }
            } else {
                Log.i (this, "sendCallDeflectRequest: no listener is available");
            }
        } else {
            /* Retrieve the call id to be deflected */
            int nCallId = Integer.parseInt(sessionList.get(0).getCallId());
            Log.i (this, "sendCallDeflectRequest: call ID " + nCallId +
                    "is requested to "+ deflectNumber);

            /* Send the call deflect request to lower layers */
            mCi.deflectCall(nCallId, deflectNumber,
                    mHandler.obtainMessage(EVENT_CALL_DEFLECT, listener));
        }
    }

    public void sendCancelModifyCall(IQtiImsExtListener listener) {
        /* Get the callsession for active call */
        List<ImsCallSessionImpl> sessionList =
                getCallSessionByState(DriverCallIms.State.ACTIVE);

        if (sessionList.isEmpty()) {
            /* No call to cancel, notify error */
            Log.i (this, "sendCancelModifyCall: no call is available to cancel modify call");
            if (listener != null) {
                try {
                    listener.receiveCancelModifyCallResponse(mPhoneId,
                            QtiImsExtUtils.QTI_IMS_REQUEST_ERROR);
                } catch (Throwable t) {
                    Log.e(this, "sendCancelModifyCall exception!");
                }
            } else {
                Log.i (this, "sendCancelModifyCall: no listener is available");
            }
        } else {
            /* Retrieve the call id to be cancelled modify call */
            int nCallId = Integer.parseInt(sessionList.get(0).getCallId());
            Log.i (this, "sendCancelModifyCall: call ID " + nCallId);

            /* Send the cancel modify call request to lower layers */
            mCi.cancelModifyCall(mHandler.obtainMessage(EVENT_CANCEL_MODIFY_CALL,
                    listener), nCallId);
        }
    }

    public void resumePendingCall(int videoState) {
        /* Get the callsession for dialing call */
        List<ImsCallSessionImpl> sessionList =
                getCallSessionByState(DriverCallIms.State.DIALING);

        if (sessionList.isEmpty()) {
            Log.d(this, "resumePendingCall: no call is available");
            return;
        }
        sessionList.get(0).resumePendingCall(videoState);
    }

    private int convertQtiImsTransferTypeToImsQmiType(int type) {
        switch(type) {
            case QtiImsExtUtils.QTI_IMS_BLIND_TRANSFER:
                return ImsQmiIF.BLIND_TRANSFER;
            case QtiImsExtUtils.QTI_IMS_ASSURED_TRANSFER:
                return ImsQmiIF.ASSURED_TRANSFER;
            case QtiImsExtUtils.QTI_IMS_CONSULTATIVE_TRANSFER:
                return ImsQmiIF.CONSULTATIVE_TRANSFER;
        }
        return type;
    }

    public void sendCallTransferRequest(int type, String number,
                IQtiImsExtListener listener) {
        boolean returnError = true;

        switch(type) {
            /* Blind and assured transfer are the same, hence fall through */
            case QtiImsExtUtils.QTI_IMS_BLIND_TRANSFER:
            case QtiImsExtUtils.QTI_IMS_ASSURED_TRANSFER:
                if (number == null) {
                    /* Number cannot be null */
                    Log.e (this, "sendCallTransferRequest: no target number");
                } else {
                    /* Check for an active callsession */
                    List<ImsCallSessionImpl> sessionList = getCallSessionByState(
                            DriverCallIms.State.ACTIVE);
                    if (sessionList.isEmpty()) {
                        /* If no active call, then check for held callsession */
                        sessionList = getCallSessionByState(DriverCallIms.State.HOLDING);
                    }

                    if (sessionList.isEmpty()) {
                        /* No call to transfer, return error */
                        Log.e (this, "sendCallTransferRequest: no call available");
                    } else if (sessionList.get(0).canTransferToNumber()) {
                        /* Send the call transfer request to lower layers */
                        returnError = false;
                        mCi.explicitCallTransfer(
                                Integer.parseInt(sessionList.get(0).getCallId()),
                                convertQtiImsTransferTypeToImsQmiType(type), number, 0,
                                mHandler.obtainMessage(EVENT_CALL_TRANSFER, listener));
                    } else {
                        /* Calls are available, but not allowed to transfer */
                        Log.e (this, "sendCallTransferRequest: transfer not allowed " +
                                sessionList.get(0));
                    }
                }
                break;
            case QtiImsExtUtils.QTI_IMS_CONSULTATIVE_TRANSFER:
                {
                    /* Get the callsession for active call */
                    List<ImsCallSessionImpl> sessionList =
                            getCallSessionByState(DriverCallIms.State.ACTIVE);
                    if (sessionList.isEmpty()) {
                        /* There is no active, return error */
                        Log.e (this, "sendCallTransferRequest: no active call available");
                    } else if (sessionList.get(0).canTransferToId()) {
                        /* check for held call */
                        int nCallId = Integer.parseInt(sessionList.get(0).getCallId());
                        sessionList = getCallSessionByState(DriverCallIms.State.HOLDING);
                        if (sessionList.isEmpty()) {
                            /* There is no held call, return error */
                            Log.e (this, "sendCallTransferRequest: no held call available");
                        } else if (sessionList.get(0).canTransferToId()) {
                            /* Send transfer request */
                            returnError = false;
                            mCi.explicitCallTransfer(
                                    Integer.parseInt(sessionList.get(0).getCallId()),
                                    convertQtiImsTransferTypeToImsQmiType(type), null, nCallId,
                                    mHandler.obtainMessage(EVENT_CALL_TRANSFER, listener));
                        } else {
                            Log.e (this, "sendCallTransferRequest: held call transfer is" +
                                    "not allowed " + sessionList.get(0));
                        }
                    } else {
                        /* no support for conference call, return error */
                        Log.e (this, "sendCallTransferRequest: active call transfer is" +
                                "not allowed " + sessionList.get(0));
                    }
                }
                break;
            default:
                break;
        }

        /* if identified as error case, send error response */
        if (returnError) {
            /* Call transfer failure, notify error */
            Log.e(this, "sendCallTransferRequest: Type " + type + " Failed");
            if (listener != null) {
                try {
                    listener.receiveCallTransferResponse(mPhoneId,
                            QtiImsExtUtils.QTI_IMS_REQUEST_ERROR);
                } catch (Throwable t) {
                    Log.e(this, "sendCallTransferRequest: exception!");
                }
            } else {
                Log.e(this, "sendCallTransferRequest: no listener is available");
            }
        }
    }

    public void getHandoverConfig(IQtiImsExtListener listener) {
        mCi.getHandoverConfig(mHandler.obtainMessage(EVENT_GET_HANDOVER_CONFIG,
                listener));
    }

    public void setHandoverConfig(int hoConfig, IQtiImsExtListener listener) {
        mCi.setHandoverConfig(hoConfig,
                mHandler.obtainMessage(EVENT_SET_HANDOVER_CONFIG, listener));
    }

    /**
     * Check if the exception is due to Radio not being available
     * @param e
     * @return true, if exception is due to RADIO_NOT_AVAILABLE
     */
    private boolean isImsExceptionRadioNotAvailable(Throwable e) {
        return e != null
                && e instanceof RuntimeException
                && ((RuntimeException) e).getMessage().equals(
                        ImsSenderRxr.errorIdToString(ImsQmiIF.E_RADIO_NOT_AVAILABLE));
    }

    /**
     * Check if given call can be transferred or not
     * @param dc, an instance of DriverCallIms
     * @return true, if given call is transferrable
     *         false, otherwise
     */
    private boolean canTransfer(DriverCallIms dc) {
        /**
         * For conference calls, transfer is not supported in any call state
         * For individual calls, transfer is possible only in active and held state
         */
        return (dc != null) && !dc.isMpty &&
                (dc.state == DriverCallIms.State.ACTIVE ||
                dc.state == DriverCallIms.State.HOLDING);
    }

    private void updateCallTransferInfo(ArrayList<DriverCallIms> dcList) {
        /* return if the corresponding config is disabled */
        if (!QtiImsExtUtils.isCallTransferEnabled(mContext)) {
            return;
        }

        int nTransferableCalls = 0;
        /* Get the count of the possible transferable calls */
        for (DriverCallIms dc: dcList) {
            if (canTransfer(dc)) {
                nTransferableCalls++;
            }
        }
        Log.i(this, "Transfer is possible for " + nTransferableCalls + " calls");

        /**
         * If number of transferable calls is
         * 1 - only one call is available to transfer, so can only transfer to number
         * 2 - two calls are available to transfer, so can transfer to both number and id
         * otherwise - not possible for any transfer.
         */
        switch (nTransferableCalls) {
            /* update that call transfer to a number is allowed */
            case 1:
                for (DriverCallIms dc: dcList) {
                    if (canTransfer(dc)) {
                        dc.mEctMask = QtiImsExtUtils.QTI_IMS_BLIND_TRANSFER |
                                QtiImsExtUtils.QTI_IMS_ASSURED_TRANSFER;
                    }
                }
                break;
            /* update that call transfer to a number & id are allowed */
            case 2:
                for (DriverCallIms dc: dcList) {
                    if (canTransfer(dc)) {
                        dc.mEctMask = QtiImsExtUtils.QTI_IMS_BLIND_TRANSFER |
                                QtiImsExtUtils.QTI_IMS_ASSURED_TRANSFER |
                                QtiImsExtUtils.QTI_IMS_CONSULTATIVE_TRANSFER;
                    }
                }
                break;
            /* No call transfer is possible */
            default:
                break;
        } /* end of switch */

        return;
    }

    /**
     * Handle the calls returned as part of UNSOL_CALL_STATE_CHANGED
     * @param ar - the AsyncResult object that contains the call list information
     */
    private void handleCalls(AsyncResult ar) {
        ArrayList<DriverCallIms> callList;
        Log.i(this, ">handleCalls");
        Map<Integer, DriverCallIms> dcList = new HashMap<Integer, DriverCallIms>();

        if (ar.exception == null) {
            callList = (ArrayList<DriverCallIms>) ar.result;
        } else if (isImsExceptionRadioNotAvailable(ar.exception)) {
            // just a dummy empty ArrayList to cause the loop
            // to hang up all the calls
            callList = new ArrayList<DriverCallIms> ();
        } else {
            // Radio probably wasn't ready--try again in a bit
            // But don't keep polling if the channel is closed
            return;
        }

        ArrayList<DriverCallIms> mmTelList = new ArrayList<DriverCallIms> ();

        if (callList != null) {
            for (DriverCallIms dc : callList) {
                Log.i(this, "handleCalls: dc = " + dc);
                mmTelList.add(dc);
            }
        }

        ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
        if (tracker == null) {
            /**
             * TODO: Right not there is no response back from telephony to RIL
             * for these scenarios, Client did not register for service class but a call
             * for the service class was received. Some possible ways are
             *  1) send a reject for an incoming call
             *  2) send a hangup for an active call
             */
            if (mmTelList.size() > 0) {
                Log.e (this, "Call for non-registered service class MMTEL");
            }
        } else {
            /* Check upon the possibility of call transfer support */
            updateCallTransferInfo(mmTelList);
            // service class will be filtered here and only calls with service class MMTEL will go to tracker
            tracker.handleCalls((ArrayList<DriverCallIms>) mmTelList);
        }
    }

    /**
     * Handle intra RAT Handovers as part of UNSOL_RESPONSE_HANDOVER
     * @param ar - the AsyncResult object that contains handover information
     */
    private void handleHandover(AsyncResult ar) {
        Log.i(this, "handleHandover");
        ImsQmiIF.Handover handover = null;
        if (ar.exception == null) {
            handover = (ImsQmiIF.Handover) ar.result;

            ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
            if (tracker != null) {
                tracker.handleHandover(handover);
            }
            else {
                Log.e (this, "Message for non-registered service class");
                return;
            }
        }
        else {
            Log.e(this, "AsyncResult exception in handleHandover- " + ar.exception);
        }

    }

    /**
     * Handle the call state changes for incoming (MT) Hold/Resume as part of
     * the UNSOL_SUPP_SVC_NOTIFICATION message.
     * @param ar - the AsyncResult object that contains the call list information
     */
    private void handleSuppSvc(AsyncResult ar) {
        Log.i(this, "handleSuppSvc");
        ImsQmiIF.SuppSvcNotification supp_svc_unsol = null;
        if (ar.exception == null) {
            supp_svc_unsol = (ImsQmiIF.SuppSvcNotification) ar.result;

            ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
            if (tracker != null) {
                tracker.handleSuppSvcUnsol(supp_svc_unsol);
            }
            else {
                Log.e (this, "Message for non-registered service class");
                return;
            }
        }
        else {
            Log.e(this, "AsyncResult exception in handleSuppSvc.");
        }
    }

    /**
     * Handle the TTY mode changes as part of the UNSOL_TTY_NOTIFICATION message.
     * @param ar - the AsyncResult object that contains new TTY mode.
     */
    private void handleTtyModeChange(AsyncResult ar) {
        Log.i(this, "handleTtyModeChange");
        if (ar != null && ar.result != null && ar.exception == null) {
            int[] mode = (int[])ar.result;
            Log.e(this, "Received EVENT_TTY_STATE_CHANGED mode= " + mode[0]);

            ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
            if (tracker != null) {
                tracker.handleTtyModeChangeUnsol(mode[0]);
            } else {
                Log.e (this, "Message for non-registered service class");
                return;
            }
        } else {
            Log.e(this, "Error EVENT_TTY_STATE_CHANGED AsyncResult ar= " + ar);
        }
     }

    /**
     * Gets a call session with give media id.
     * @param mediaId Media id of the session to be searched.
     * @return Call session with {@code mediaId}
     */
    public List<ImsCallSessionImpl> getCallSessionByState(DriverCallIms.State state) {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        return tracker == null ? Collections.EMPTY_LIST : tracker.getCallSessionByState(state);
    }

    /**
     * Gets a call session with give media id.
     * @param mediaId Media id of the session to be searched.
     * @return Call session with {@code mediaId}
     */
    public ImsCallSessionImpl findSessionByMediaId(int mediaId) {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        return tracker == null ? null : tracker.findSessionByMediaId(mediaId);
    }

    /**
     * @return This instance's Phone ID.
     */
    public int getPhoneId() {
        return mPhoneId;
    }

    /**
     * Registers call list listener.
     * Note: Only {@code ImsServiceClass.MMTEL} is supported.
     * @param listener Listener to registered
     * @see ICallListListener
     */
    public void addListener(ICallListListener listener) {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null) {
            tracker.addListener(listener);
        } else {
            Log.e(this,"ImsServiceClassTracker not found.");
        }
    }

    /**
     * Handles Conference refresh Info notified through UNSOL_REFRESH_CONF_INFO message
     * @param ar - the AsyncResult object that contains the refresh Info information
     */
    public void handleRefreshConfInfo(AsyncResult ar) {
        Log.i(this, "handleRefreshConfInfo");
        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            ImsQmiIF.ConfInfo confInfo = (ImsQmiIF.ConfInfo) ar.result;
            ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
            if (tracker != null) {
                tracker.handleRefreshConfInfo(confInfo);
            } else {
                Log.i(this, "Message for non-registered service class");
                return;
            }
        } else {
            if (ar != null) {
                Log.e(this, "Failed @handleRefreshConfInfo --> " + ar.exception
                        + "with result = " + ar.result);
            } else {
                Log.e(this, "Failed @handleRefreshConfInfo --> AsyncResult is null");
            }
        }
    }

    /**
     * Unregisters call list listener.
     * Note: Only {@code ImsServiceClass.MMTEL} is supported.
     * @param listener Listener to unregistered
     * @see ICallListListener
     */
    public void removeListener(ICallListListener listener) {
        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        if (tracker != null) {
            tracker.removeListener(listener);
        } else {
            Log.e(this,"ImsServiceClassTracker not found.");
        }
    }

    private void handleModifyCallRequest(CallModify cm) {
        Log.i(this, "handleCallModifyRequest(" + cm + ")");

        ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
        tracker.handleModifyCallRequest(cm);
    }

    private void sendBroadcastForDisconnected(ImsReasonInfo imsReasonInfo, int regState){
        Intent intent = new Intent("com.android.imsconnection.DISCONNECTED");
        intent.putExtra("result", (Parcelable)imsReasonInfo);
        intent.putExtra("stateChanged", regState);
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
        Log.i(this,"sendBroadcastForDisconnected");
    }

    //Handler for the events on response from ImsSenderRxr
    private class ImsServiceSubHandler extends Handler {
        ImsServiceSubHandler() {
            super();
        }

        /**
         * Local utility to start a new thread and then call registration call back
         * @param tracker object that contains handle to registration call back
         * @param registered
         */
        private void notifyRegChange(final int registrationState,
                final ImsReasonInfo imsReasonInfo, final int imsRadioTech) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    int count = mSrvClassRegListeners.beginBroadcast();
                    try {
                        for (int i = 0; i < count; i++) {
                            try {
                                switch (registrationState) {
                                    case ImsQmiIF.Registration.REGISTERED:
                                        mSrvClassRegListeners.getBroadcastItem(i)
                                                .registrationConnectedWithRadioTech(imsRadioTech);
                                        break;
                                    case ImsQmiIF.Registration.NOT_REGISTERED:
                                        mSrvClassRegListeners.getBroadcastItem(i)
                                                .registrationDisconnected(imsReasonInfo);
                                        break;
                                    case ImsQmiIF.Registration.REGISTERING:
                                        mSrvClassRegListeners.getBroadcastItem(i)
                                                .registrationProgressingWithRadioTech(imsRadioTech);
                                        break;
                                }
                            } catch (RemoteException e) {
                                Log.w(this, e + " " + "notifyRegChange() - Skipping callback.");
                            }
                        }
                    } finally {
                        mSrvClassRegListeners.finishBroadcast();
                    }
                }
            };
            if (mRegCallbackHandler != null) {
                mRegCallbackHandler.post(r);
            }
        }

        private void notifyRegAssociatedUriChange(final Uri[] self_identity) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    int count = mSrvClassRegListeners.beginBroadcast();
                    try {
                        for (int i = 0; i < count; i++) {
                            try {
                                mSrvClassRegListeners.getBroadcastItem(i)
                                        .registrationAssociatedUriChanged(self_identity);
                            } catch (RemoteException e) {
                                Log.w(this, e + " " + "notifyRegAssociatedUriChange() - Skipping " +
                                        "callback.");
                            }
                        }
                    } finally {
                        mSrvClassRegListeners.finishBroadcast();
                    }
                }
            };
            if (mRegCallbackHandler != null) {
                mRegCallbackHandler.post(r);
            }
        }

        private void handleImsStateChanged(AsyncResult ar) {
            Log.i(this,"handleImsStateChanged");
            int errorCode = ImsReasonInfo.CODE_UNSPECIFIED;
            String errorMessage = null;
            String selfIdentityUrisCombined = null;
            int regState = ImsQmiIF.Registration.NOT_REGISTERED;
            int imsRat = ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN;
            String[] selfIdentityUriStrings = null;
            Uri[] selfIdentityUris = null;
            if (ar != null && ar.exception == null && ar.result instanceof ImsQmiIF.Registration) {
                ImsQmiIF.Registration registration = (ImsQmiIF.Registration) ar.result;

                errorCode = registration.hasErrorCode() ? registration.getErrorCode()
                        : ImsReasonInfo.CODE_UNSPECIFIED;
                errorMessage = registration.hasErrorMessage() ? registration
                        .getErrorMessage() : null;
                selfIdentityUrisCombined = registration.hasPAssociatedUris() ?
                        registration.getPAssociatedUris() : null;
                regState = registration.hasState() ? registration.getState()
                        : ImsQmiIF.Registration.NOT_REGISTERED;
                imsRat = getRilRadioTech(registration);
                if (regState != ImsQmiIF.Registration.NOT_REGISTERED) {
                    mCi.queryServiceStatus(mHandler.obtainMessage(EVENT_GET_SRV_STATUS));
                }
                if (selfIdentityUrisCombined != null) {
                    selfIdentityUris =
                            extractUrisFromPipeSeparatedUriStrings(selfIdentityUrisCombined);
                }
            } else {
                Log.e(this,"handleImsStateChanged error");
            }

            ImsReasonInfo imsReasonInfo = new ImsReasonInfo(
                    ImsReasonInfo.CODE_REGISTRATION_ERROR,
                    errorCode, errorMessage);

            int wfcRegState = regState;
            if (imsRat == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) {
                wfcRegState = ImsQmiIF.Registration.NOT_REGISTERED;
                Log.i(this, "VOLTE ims registered, WFC change to Not  registered");
            }
            sendBroadcastForDisconnected(imsReasonInfo, wfcRegState);

            notifyRegChange(regState, imsReasonInfo, imsRat);

            if (selfIdentityUris != null && selfIdentityUrisHaveChanged(selfIdentityUris)) {
                updateSelfIdentityUriCache(selfIdentityUris);
                notifyRegAssociatedUriChange(selfIdentityUris);
            }
        }

        private int getRilRadioTech(ImsQmiIF.Registration registration) {
            if (!registration.hasRadioTech()) {
                return ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN;
            }

            int imsRat;
            switch (registration.getRadioTech()) {
                case ImsQmiIF.RADIO_TECH_LTE:
                    imsRat = ServiceState.RIL_RADIO_TECHNOLOGY_LTE;
                break;
                case ImsQmiIF.RADIO_TECH_WIFI:
                case ImsQmiIF.RADIO_TECH_IWLAN:
                    imsRat = ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN;
                break;
                default:
                   imsRat = ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN;
            }

            return imsRat;
        }

        private Uri[] extractUrisFromPipeSeparatedUriStrings(String combinedUris) {
            if (combinedUris == null || combinedUris.length() <= 1) return null;
            String[] uriStrings = combinedUris.split("\\|");
            Uri[] uris = new Uri[uriStrings.length];
            for (int i = 0; i < uriStrings.length; i++) {
                uris[i] = Uri.parse(uriStrings[i]);
            }
            return uris;
        }

        private boolean selfIdentityUrisHaveChanged(Uri[] new_uris) {
            if (mSelfIndentityUris == null) return true;
            boolean changed = false;
            if ((new_uris.length != mSelfIndentityUris.size())
                    || (mSelfIndentityUris == null)) {
                changed = true;
            } else {
                for (int i = 0; i < new_uris.length; i++) {
                    if (!mSelfIndentityUris.contains(new_uris[i])) {
                        changed = true;
                        break;
                    }
                }
            }
            return changed;
        }

        private void updateSelfIdentityUriCache(Uri[] new_uris) {
            if (mSelfIndentityUris == null) {
                mSelfIndentityUris = new HashSet<Uri>();
            } else {
                mSelfIndentityUris.clear();
            }
            for (int i = 0; i < new_uris.length; i++) {
                mSelfIndentityUris.add(new_uris[i]);
                Log.i (this, "updateSelfIdentityUriCache :: "
                        + "new self-identity host URI=" + new_uris[i]);
            }
        }

        @Override
        public void handleMessage(Message msg) {
            Log.i (this, "Message received: what = " + msg.what);
            AsyncResult ar;

            switch (msg.what) {
                case EVENT_IMS_STATE_CHANGED:
                    ar = (AsyncResult) msg.obj;
                    if (ar.result == null) {
                        // Backward compatibility with old RIL API
                        mCi.getImsRegistrationState(this.obtainMessage(EVENT_IMS_STATE_DONE));
                    } else {
                        handleImsStateChanged(ar);
                    }
                    break;
                case EVENT_IMS_STATE_DONE:
                    ar = (AsyncResult) msg.obj;
                    handleImsStateChanged(ar);
                    break;
                case EVENT_SET_IMS_STATE:
                    ar = (AsyncResult) msg.obj;
                    if ( ar.exception != null ) {
                        Log.i(this, "Request turn on/off IMS failed");
                    }
                    break;
                case EVENT_SRV_STATUS_UPDATE:
                    Log.i(this, "Received event: EVENT_SRV_STATUS_UPDATE");
                    AsyncResult arResult = (AsyncResult) msg.obj;
                    if (arResult.exception == null && arResult.result != null) {
                        ArrayList<ServiceStatus> responseArray =
                                (ArrayList<ServiceStatus>) arResult.result;
                        handleSrvStatusUpdate(responseArray);
                    } else {
                        Log.e(this, "IMS Service Status Update failed!");
                        initServiceStatus();
                    }
                    break;
                case EVENT_GET_SRV_STATUS:
                    Log.i(this, "Received event: EVENT_GET_STATUS_UPDATE");
                    AsyncResult arResultSrv = (AsyncResult) msg.obj;
                    if (arResultSrv.exception == null && arResultSrv.result != null) {
                        ArrayList<ServiceStatus> responseArray =
                                (ArrayList<ServiceStatus>) arResultSrv.result;
                        handleSrvStatusUpdate(responseArray);
                    } else {
                        Log.e(this, "IMS Service Status Update failed!");
                        initServiceStatus();
                    }
                    break;
                case EVENT_SET_SRV_STATUS:
                    //TODO:
                    break;
                case EVENT_CALL_STATE_CHANGE:
                    ar = (AsyncResult) msg.obj;
                    if (ar.result != null) {
                        handleCalls(ar);
                    } else if (isImsExceptionRadioNotAvailable(ar.exception)) {
                        Log.e(this, "EVENT_CALL_STATE_CHANGE when Radio is Unavailable");
                        ar.exception = null;
                        ar.result = getCallsListToClear();
                        if (ar.result != null) {
                            handleCalls(ar);
                        } else {
                            Log.e(this, "EVENT_CALL_STATE_CHANGE with no calls ignored!");
                        }
                    } else {
                        Log.e(this, "EVENT_CALL_STATE_CHANGE exception " + ar.exception);
                    }
                    break;
                case EVENT_GET_CURRENT_CALLS:
                    ar = (AsyncResult) msg.obj;
                    handleCalls(ar);
                    break;
                case EVENT_SUPP_SRV_UPDATE:
                    ar = (AsyncResult) msg.obj;
                    handleSuppSvc(ar);
                    break;
                case EVENT_TTY_STATE_CHANGED:
                    ar = (AsyncResult) msg.obj;
                    handleTtyModeChange(ar);
                    break;
                case EVENT_REFRESH_CONF_INFO:
                    ar = (AsyncResult) msg.obj;
                    handleRefreshConfInfo(ar);
                    break;
                case EVENT_HANDOVER_STATE_CHANGED:
                    ar = (AsyncResult) msg.obj;
                    handleHandover(ar);
                    break;
                case EVENT_CONFERENCE:
                    ar = (AsyncResult) msg.obj;
                    handleConferenceResponse(ar);
                    break;
                case EVENT_SRVCC_STATE_CHANGED:
                    ar = (AsyncResult) msg.obj;
                    handleSrvccResponse(ar);
                    break;
                case EVENT_CALL_MODIFY:
                    ar = (AsyncResult) msg.obj;
                    if (ar != null && ar.result != null && ar.exception == null) {
                        handleModifyCallRequest((CallModify) ar.result);
                    } else {
                        Log.e(this, "Error EVENT_MODIFY_CALL AsyncResult ar= " + ar);
                    }
                    break;
                case EVENT_MWI:
                    handleMwiNotification(msg);
                    break;
                case EVENT_SET_CALL_FORWARD_TIMER:
                    ar = (AsyncResult) msg.obj;
                    onSetCallForwardTimerDone(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_GET_CALL_FORWARD_TIMER:
                    ar = (AsyncResult) msg.obj;
                    onGetCallForwardTimerDone(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_GET_PACKET_COUNT:
                    ar = (AsyncResult) msg.obj;
                    onGetPacketCountDone(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_GET_PACKET_ERROR_COUNT:
                    ar = (AsyncResult) msg.obj;
                    onGetPacketErrorCountDone(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_CALL_DEFLECT:
                    ar = (AsyncResult) msg.obj;
                    handleCallDeflectResponse(ar);
                    break;
                case EVENT_CANCEL_MODIFY_CALL:
                    ar = (AsyncResult) msg.obj;
                    handleCancelModifyCallResponse(ar);
                    break;
                case EVENT_GEOLOCATION_INFO_REQUEST:
                    ar = (AsyncResult) msg.obj;
                    handleGeolocationRequest(ar);
                    break;
                case EVENT_GEOLOCATION_RESPONSE:
                    ar = (AsyncResult) msg.obj;
                    handleGeolocationResponse(ar);
                    break;
                case EVENT_VOWIFI_CALL_QUALITY_UPDATE:
                    ar = (AsyncResult) msg.obj;
                    handleVoWiFiCallQuality(ar);
                    break;
                case EVENT_CALL_TRANSFER:
                    ar = (AsyncResult) msg.obj;
                    handleCallTransferResponse(ar);
                    break;
                case EVENT_VOPS_CHANGED:
                    ar = (AsyncResult) msg.obj;
                    handleVops(ar);
                    break;
                case EVENT_SSAC_CHANGED:
                    ar = (AsyncResult) msg.obj;
                    handleSsac(ar);
                    break;
                case EVENT_VOPS_RESPONSE:
                    ar = (AsyncResult) msg.obj;
                    handleVopsResponse(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_SSAC_RESPONSE:
                    ar = (AsyncResult) msg.obj;
                    handleSsacResponse(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_PARTICIPANT_STATUS_INFO:
                    ar = (AsyncResult) msg.obj;
                    handleParticipantStatusInfo(ar);
                    break;
                case EVENT_SET_VOLTE_PREF:
                    ar = (AsyncResult) msg.obj;
                    handleUpdateVoltePrefResponse(ar);
                    break;
                case EVENT_GET_VOLTE_PREF:
                    ar = (AsyncResult) msg.obj;
                    handleQueryVoltePrefResponse(ar);
                    break;
                case EVENT_GET_HANDOVER_CONFIG:
                    ar = (AsyncResult) msg.obj;
                    onGetHandoverConfigDone(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_SET_HANDOVER_CONFIG:
                    ar = (AsyncResult) msg.obj;
                    onSetHandoverConfigDone(getImsInterfaceListener(ar), ar);
                    break;
                case EVENT_REGISTRATION_BLOCK_STATUS:
                    ar = (AsyncResult) msg.obj;
                    handleRegistrationBlockStatus(ar);
                    break;
                default:
                    Log.i(this, "Unknown message = " + msg.what);
            }
        }
    }

    private void onGetPacketCountDone(IQtiImsExtListener listener, AsyncResult ar) {
        if (listener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                long result = ar.result == null ? 0 : (long)ar.result;
                listener.onGetPacketCount(mPhoneId, status, result);
            } catch (Throwable t) {
                Log.e(this, "onGetPacketCountDone " + t);
            }
        } else {
            Log.e(this, "onGetPacketCountDone listener is not valid !!!");
        }
    }

    private void onGetPacketErrorCountDone(IQtiImsExtListener listener, AsyncResult ar) {
        if (listener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                long result = ar.result == null ? 0 : (long)ar.result;
                listener.onGetPacketErrorCount(mPhoneId, status, result);
            } catch (Throwable t) {
                Log.e(this, "onGetPacketErrorCountDone " + t);
            }
        } else {
            Log.e(this, "onGetPacketErrorCountDone listener is not valid !!!");
        }
    }

    private int getOperationStatus(boolean status) {
        return status ? ImsConfig.OperationStatusConstants.SUCCESS :
                ImsConfig.OperationStatusConstants.FAILED;
    }

    private void onSetCallForwardTimerDone(IQtiImsExtListener listener, AsyncResult ar) {
        int status = 0;
        if (listener != null) {
            if (ar.result != null) {
                ImsQmiIF.SuppSvcResponse response = (ImsQmiIF.SuppSvcResponse) ar.result;
                if (response.hasFailureCause()) {
                    Log.e(this, "onSetCallForwardTimerDone Failure cause: " +
                            response.hasFailureCause());
                    try {
                        listener.onUTReqFailed(mPhoneId, ImsReasonInfo.CODE_UT_NETWORK_ERROR,
                                response.getFailureCause());
                    } catch (Throwable t) {
                        Log.e(this, "onUTReqFailed exception!" +t);
                    }
                    return;
                }
            }
            if (ar.exception != null) {
                Log.e(this, "set CF Timer error!");
                try {
                    listener.onUTReqFailed(mPhoneId, ImsReasonInfo.CODE_UNSPECIFIED, null);
                } catch (Throwable t) {
                    Log.e(this, "onUTReqFailed exception!" +t);
                }
            } else {
                Log.e(this, "set CF Timer success!");
                try {
                    listener.onSetCallForwardUncondTimer(mPhoneId, status);
                } catch (Throwable t) {
                    Log.e(this, "onSetCallForwardTimerDone exception!" +t);
                }
            }
        }
    }

    private void onGetCallForwardTimerDone(IQtiImsExtListener listener, AsyncResult ar) {
        int startHour = 0;
        int endHour = 0;
        int startMinute = 0;
        int endMinute = 0;
        int reason = 0;
        int status = 0;
        String number = null;
        int serviceClass = 0;

        if (ar.exception != null) {
            Log.e(this, "get CF Timer error!");
            try {
                listener.onUTReqFailed(mPhoneId, ImsReasonInfo.CODE_UNSPECIFIED, null);
            } catch (Throwable t) {
                Log.e(this, "onUTReqFailed exception!" +t);
            }
            return;
        }
        if (ar.result != null) {
            if(ar.result instanceof ImsCallForwardTimerInfo[]) {
                Log.i(this, "onGetCallForwardTimerDone ImsCallForwardTimerInfo instance! ");
                ImsCallForwardTimerInfo cfTimerInfo[] = (ImsCallForwardTimerInfo[]) ar.result;
                for (int i = 0; i < cfTimerInfo.length; i++) {
                   startHour = cfTimerInfo[i].startHour;
                   endHour = cfTimerInfo[i].endHour;
                   startMinute = cfTimerInfo[i].startMinute;
                   endMinute = cfTimerInfo[i].endMinute;
                   reason = cfTimerInfo[i].reason;
                   status = cfTimerInfo[i].status;
                   number = cfTimerInfo[i].number;
                   serviceClass = cfTimerInfo[i].serviceClass;
                }
            } else if (ar.result instanceof CallForwardInfo[]) {
                Log.i(this, "onGetCallForwardTimerDone CallForwardInfo instance! ");
                CallForwardInfo cfInfo[] = (CallForwardInfo[]) ar.result;
                for (int i = 0; i < cfInfo.length; i++) {
                    reason = cfInfo[i].reason;
                    status = cfInfo[i].status;
                    number = cfInfo[i].number;
                    serviceClass = cfInfo[i].serviceClass;
                }
            }
        }

        if (listener !=null) {
            if (reason == CF_REASON_UNCONDITIONAL) {
                try {
                    listener.onGetCallForwardUncondTimer(mPhoneId, startHour, endHour, startMinute,
                            endMinute, reason, status, number, serviceClass);
                } catch (Throwable t) {
                    Log.e(this, "onGetCallForwardTimerDone exception!");
                }
            } else {
                try {
                    listener.onUTReqFailed(mPhoneId, ImsReasonInfo.CODE_UT_NETWORK_ERROR, null);
                } catch (Throwable t) {
                    Log.e(this, "onUTReqFailed exception!"+t);
                }
            }
        }
    }

    private void handleCallDeflectResponse(AsyncResult ar) {
        IQtiImsExtListener listener = getImsInterfaceListener(ar);

        /* Default assume as successful */
        int nStatus = QtiImsExtUtils.QTI_IMS_REQUEST_SUCCESS;
        if (ar != null && ar.exception != null) {
            /* Request failed */
            nStatus = QtiImsExtUtils.QTI_IMS_REQUEST_ERROR;
        }
        Log.i(this, "handleCallDeflectResponse: Result " + nStatus);

        /* If listener is available, notify the result */
        if (listener != null) {
            try {
                listener.receiveCallDeflectResponse(mPhoneId, nStatus);
            } catch (Throwable t) {
                Log.e(this, "handleCallDeflectResponse exception!");
            }
        } else {
            Log.i(this, "handleCallDeflectResponse: no listener is available");
        }
    }

    private void handleCancelModifyCallResponse(AsyncResult ar) {
        IQtiImsExtListener listener = getImsInterfaceListener(ar);

        /* Default assume as successful */
        int nStatus = QtiImsExtUtils.QTI_IMS_REQUEST_SUCCESS;
        if (ar != null && ar.exception != null) {
            /* Request failed */
            nStatus = QtiImsExtUtils.QTI_IMS_REQUEST_ERROR;
            Toast.makeText(mContext, "Cancel upgrade failed", Toast.LENGTH_SHORT).show();
        }
        Log.i(this, "handleCancelModifyCallResponse: Result " + nStatus);

        /* If listener is available, notify the result */
        if (listener != null) {
            try {
                listener.receiveCancelModifyCallResponse(mPhoneId, nStatus);
            } catch (Throwable t) {
                Log.e(this, "handleCancelModifyCallResponse exception!");
            }
        } else {
            Log.i(this, "handleCancelModifyCallResponse: no listener is available");
        }
    }

    private void handleGeolocationResponse(AsyncResult ar) {
        if (ar != null && ar.exception != null) {
            Log.i(this, "handleGeolocationResponse :: Error response!");
        }
        // NOTE: If we need to figure exactly which request's response
        //       failed, the Message's arg1 and arg2 fields will have to
        //       be used in GeoCoderTask. This is not needed as of now.
    }

    private void handleGeolocationRequest(AsyncResult ar) {
        ImsQmiIF.GeoLocationInfo geoInfo = (ImsQmiIF.GeoLocationInfo) ar.result;
        if (geoInfo == null) {
            Log.e(this, "handleGeolocationRequest :: Null AsyncResult!");
            return;
        }
        try {
            sTaskExecutor.execute(
                    new GeocoderTask(geoInfo.getLat(), geoInfo.getLon()));
        } catch (RejectedExecutionException rex) {
            Log.e(this, "handleGeolocationRequest :: Failure to start "
                    + "GeocoderTask thread for [lat=" + geoInfo.getLat()
                    + " | long=" + geoInfo.getLon() + "]!\n");
            rex.printStackTrace();
        }
    }

    private class GeocoderTask implements Runnable {
        private double latitude;
        private double longitude;
        final int MAX_RESULTS = 1; // Check NOTE in run method before changing value.

        public GeocoderTask(double lat, double lon) {
            latitude = lat;
            longitude = lon;
        }

        @Override
        public String toString() {
            return "GeocoderTask [lat=" + latitude + " | long=" + longitude + "]";
        }

        @Override
        public void run() {
            Log.i(this, "Executing " + this);
            Address address = null;
            List<Address> addresses = null;
            if (!Geocoder.isPresent()) {
                Log.e(this, "handleGeolocationRequest :: Geocoder not present!");
            } else {
                Geocoder gc = new Geocoder(mContext);
                try {
                    addresses = gc.getFromLocation(latitude, longitude, MAX_RESULTS);
                } catch (IllegalArgumentException iex) {
                    Log.e(this, this + " :: Invalid latitude or longitude value!");
                } catch (IOException iox) {
                    Log.e(this, this + " :: Error getting Addresses from GeoCoder!");
                }
                if (addresses != null && !addresses.isEmpty()) {
                    // NOTE: Since the current rationale is to only query GeCoder API
                    //       for one Address, the logic below works. A for-loop will
                    //       be required if multiple Address-es need to be parsed.
                    //       This will be required if IMS Service has to select the
                    //       'most appropriate' Address information from multiple objects.
                    address = addresses.get(0);
                } else {
                    Log.e(this, this + " :: Error getting Addresses from GeoCoder!");
                }
            }
            mCi.sendGeolocationInfo(latitude, longitude, address,
                    mHandler.obtainMessage(EVENT_GEOLOCATION_RESPONSE));
        }
    }

    private void handleCallTransferResponse(AsyncResult ar) {
        IQtiImsExtListener listener = getImsInterfaceListener(ar);

        /* Default assume as successful */
        int nStatus = QtiImsExtUtils.QTI_IMS_REQUEST_SUCCESS;
        if (ar != null && ar.exception != null) {
            /* Request failed */
            nStatus = QtiImsExtUtils.QTI_IMS_REQUEST_ERROR;
        }
        Log.i(this, "handleCallTransferResponse: Result " + nStatus);

        /* If listener is available, notify the result */
        if (listener != null) {
            try {
                listener.receiveCallTransferResponse(mPhoneId, nStatus);
            } catch (Throwable t) {
                Log.e(this, "handleCallTransferResponse exception!");
            }
        } else {
            Log.i(this, "handleCallTransferResponse: no listener is available");
        }
    }

    private IQtiImsExtListener getImsInterfaceListener(AsyncResult ar) {
        if (ar != null && ar.userObj instanceof IQtiImsExtListener) {
            return (IQtiImsExtListener)ar.userObj;
        }
        Log.i(this, "getImsConfigListener returns null");
        return null;
    }

    private void handleMwiNotification(Message msg) {
        if ((msg != null) && (msg.obj != null)) {
            AsyncResult arMwiUpdate = (AsyncResult) msg.obj;
            if (arMwiUpdate.exception == null) {
                if (arMwiUpdate.result != null) {
                    ImsQmiIF.Mwi mwiIF = (ImsQmiIF.Mwi) arMwiUpdate.result;
                    int summaryCount = mwiIF.getMwiMsgSummaryCount();
                    mMwiResponse.mwiMsgSummary = new Mwi.MwiMessageSummary[summaryCount];
                    Log.i(this,"handleMwiNotification summaryCount = " + summaryCount);

                    for (int i = 0; i < summaryCount; i++) {
                        mMwiResponse.mwiMsgSummary[i] = new Mwi.MwiMessageSummary();
                        mMwiResponse.mwiMsgSummary[i].mMessageType =
                                mwiIF.getMwiMsgSummary(i).getMessageType();
                        mMwiResponse.mwiMsgSummary[i].mNewMessage =
                                mwiIF.getMwiMsgSummary(i).getNewMessage();
                        mMwiResponse.mwiMsgSummary[i].mOldMessage =
                                mwiIF.getMwiMsgSummary(i).getOldMessage();
                        mMwiResponse.mwiMsgSummary[i].mNewUrgent =
                                mwiIF.getMwiMsgSummary(i).getNewUrgent();
                        mMwiResponse.mwiMsgSummary[i].mOldUrgent =
                                mwiIF.getMwiMsgSummary(i).getOldUrgent();
                        Log.i(this,"Message Summary = "
                                + mMwiResponse.summaryToString(mMwiResponse.mwiMsgSummary[i]));
                    }

                    int detailsCount = mwiIF.getMwiMsgDetailCount();
                    Log.i(this,"handleMwiNotification detailsCount = " + detailsCount);
                    mMwiResponse.mwiMsgDetails = new Mwi.MwiMessageDetails[detailsCount];
                    for (int i = 0; i < detailsCount; i++) {
                        mMwiResponse.mwiMsgDetails[i] = new Mwi.MwiMessageDetails();
                        mMwiResponse.mwiMsgDetails[i].mToAddress =
                                mwiIF.getMwiMsgDetail(i).getToAddress();
                        mMwiResponse.mwiMsgDetails[i].mFromAddress =
                                mwiIF.getMwiMsgDetail(i).getFromAddress();
                        mMwiResponse.mwiMsgDetails[i].mSubject =
                                mwiIF.getMwiMsgDetail(i).getSubject();
                        mMwiResponse.mwiMsgDetails[i].mDate = mwiIF.getMwiMsgDetail(i).getDate();
                        mMwiResponse.mwiMsgDetails[i].mPriority =
                                mwiIF.getMwiMsgDetail(i).getPriority();
                        mMwiResponse.mwiMsgDetails[i].mMessageId =
                                mwiIF.getMwiMsgDetail(i).getMessageId();
                        mMwiResponse.mwiMsgDetails[i].mMessageType =
                                mwiIF.getMwiMsgDetail(i).getMessageType();
                        Log.i(this,"Message Details = "
                                + mMwiResponse.detailsToString(mMwiResponse.mwiMsgDetails[i]));
                    }
                    updateVoiceMail();
                } else {
                    Log.e(this,"handleMwiNotification arMwiUpdate.result null");
                }
            } else {
                Log.e(this,"handleMwiNotification arMwiUpdate exception");
            }
        } else {
            Log.e(this,"handleMwiNotification msg null");
        }
    }

    private void updateVoiceMail() {
        int count = 0;
        for (int i = 0; i < mMwiResponse.mwiMsgSummary.length; i++) {
            if (mMwiResponse.mwiMsgSummary[i].mMessageType == ImsQmiIF.MWI_MSG_VOICE) {
                count = count + mMwiResponse.mwiMsgSummary[i].mNewMessage
                        + mMwiResponse.mwiMsgSummary[i].mNewUrgent;
                break;
            }
        }
        // Holds the Voice mail count
        // .i.e.MwiMessageSummary.NewMessage + MwiMessageSummary.NewUrgent
        Log.i(this,"updateVoiceMail count = " + count);

        notifyVoiceMessageCountUpdate(count);
    }

    public void registerForParticipantStatusInfo(IQtiImsExtListener listener) {
        mQtiImsParticipantStatusListeners.add(listener);
    }

    private void handleVoWiFiCallQuality(AsyncResult ar) {
        if (ar != null && ar.exception == null && ar.result != null) {
            int[] voWifiCallQuality = (int[]) ar.result;
            if (voWifiCallQuality[0] != ImsQmiIF.VOWIFI_QUALITY_NONE) {
                ImsServiceClassTracker tracker = mTrackerTable.get(ImsServiceClass.MMTEL);
                if (tracker != null) {
                    tracker.updateVoWiFiCallQuality(voWifiCallQuality[0]);
                } else {
                    Log.e(this, "Wifi Quality Error -- tracker is null");
                }
            } else {
                Log.e(this, "handleVoWiFiCallQuality received VoWiFIQuality : " +
                        voWifiCallQuality[0]);
            }
        } else {
            Log.e(this, "handleVoWiFiCallQuality response is not valid");
        }
    }

    public void registerForCapabilitiesChanged(Handler h, int what, Object obj) {
        mCapabilitiesChangedRegistrants.addUnique(h, what, obj);
    }

    public void unregisterForCapabilitiesChanged(Handler h) {
        mCapabilitiesChangedRegistrants.remove(h);
    }

    private void handleVops(AsyncResult ar) {
        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            final ImsQmiIF.VopsInfo vops = (ImsQmiIF.VopsInfo) ar.result;

            mIsVopsEnabled = vops.getIsVopsEnabled();
            broadcastVopsSsacIntent();
        } else {
            Log.e(this,"handleVops exception");
        }
    }

    private void handleSsac(AsyncResult ar) {
        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            final ImsQmiIF.SsacInfo ssacInd = (ImsQmiIF.SsacInfo) ar.result;

            Log.i(this, "handleSsac voice = " + ssacInd.getBarringFactorVoice());
            mIsSsacVoiceBarred = (ssacInd.getBarringFactorVoice() == SSAC_VOICE_BARRING_ZERO);
            broadcastVopsSsacIntent();
        } else {
            Log.e(this,"handleSsac exception");
        }
    }

    private void broadcastVopsSsacIntent() {
        Intent intent = new Intent(QtiImsExtUtils.ACTION_VOPS_SSAC_STATUS);
        intent.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
        intent.putExtra(QtiImsExtUtils.EXTRA_VOPS, mIsVopsEnabled);
        intent.putExtra(QtiImsExtUtils.EXTRA_SSAC, mIsSsacVoiceBarred);
        Log.i(this, "broadcastVopsSsacIntent Vops = " + mIsVopsEnabled
                + " , Ssac = " + mIsSsacVoiceBarred);
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    public void queryVopsStatus(IQtiImsExtListener listener) {
        Log.i(this, "queryVopsStatus");
        mCi.queryVopsStatus(mHandler.obtainMessage(EVENT_VOPS_RESPONSE, listener));
    }

    public void querySsacStatus(IQtiImsExtListener listener) {
        Log.i(this, "querySsacStatus");
        mCi.querySsacStatus(mHandler.obtainMessage(EVENT_SSAC_RESPONSE, listener));
    }

    private void handleVopsResponse(IQtiImsExtListener listener, AsyncResult ar) {
        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            final ImsQmiIF.VopsInfo vops = (ImsQmiIF.VopsInfo) ar.result;

            mIsVopsEnabled = vops.getIsVopsEnabled();
            Log.i(this, "Vops Response = " + mIsVopsEnabled);

            if (listener != null) {
                try {
                    listener.notifyVopsStatus(mPhoneId, mIsVopsEnabled);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } else {
            Log.e(this,"handleVopsResponse exception");
        }
    }

    private void handleSsacResponse(IQtiImsExtListener listener, AsyncResult ar) {
        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            final ImsQmiIF.SsacInfo ssacInd = (ImsQmiIF.SsacInfo) ar.result;

            Log.i(this, "handleSsacResponse voice = " + ssacInd.getBarringFactorVoice());
            mIsSsacVoiceBarred = (ssacInd.getBarringFactorVoice() == SSAC_VOICE_BARRING_ZERO);

            if (listener != null) {
                try {
                    listener.notifySsacStatus(mPhoneId, mIsSsacVoiceBarred);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } else {
            Log.e(this,"handleSsacResponse exception");
        }
    }

    /**
     * Handles participant status Info notified through UNSOL_PARTICIPANT_STATUS_INFO message
     * @param listener - Ims extension listener
     * @param ar - the AsyncResult object that contains the participant status Info information
     */
    public void handleParticipantStatusInfo(AsyncResult ar) {
        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            ImsQmiIF.ParticipantStatusInfo participantInfo =
                    (ImsQmiIF.ParticipantStatusInfo) ar.result;
            ImsServiceClassTracker tracker = mTrackerTable.get(new Integer(ImsServiceClass.MMTEL));
            if (mQtiImsParticipantStatusListeners.size() == 0 || tracker == null ||
                    tracker.getCallSession(Integer.toString(participantInfo.getCallId())) == null) {
                Log.i(this, "participant listeners size= "+ mQtiImsParticipantStatusListeners.size()
                        + " isTrackerPresent = " + (tracker != null));
                return;
            }
            Log.i(this, "handleParticipantStatusInfo operation = " + participantInfo.getOperation()
                   + " status = " + participantInfo.getSipStatus() + " participant = "
                   + participantInfo.getParticipantUri() + " ect = " + participantInfo.getIsEct());
            for (IQtiImsExtListener listener : mQtiImsParticipantStatusListeners) {
                try {
                    listener.notifyParticipantStatusInfo(mPhoneId,
                            participantInfo.getOperation(), participantInfo.getSipStatus(),
                            participantInfo.getParticipantUri(), participantInfo.getIsEct());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } else {
            Log.e(this, "ParticipantStatusInfo exception");
        }
    }

    /**
     * Handles registration block status through UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS message
     * @param ar - the AsyncResult object that contains the registration block status information
     */
    public void handleRegistrationBlockStatus(AsyncResult ar) {

        if ((ar != null) && (ar.exception == null) && (ar.result != null)) {
            ImsQmiIF.RegistrationBlockStatus regBlockStatus =
                    (ImsQmiIF.RegistrationBlockStatus) ar.result;
            // We only check and report wlan failures for now. This can be
            // extended to Wwan when we have an usecase for it.
            if(regBlockStatus.hasImsBlockStatusOnWlan()) {
                ImsQmiIF.BlockStatus blockStatus =
                        regBlockStatus.getImsBlockStatusOnWlan();
                //Check if it has block reason details. Block reason type is
                //ignored for now.
                if(isIKEv2Error(blockStatus)) {
                    final Runnable r = new Runnable() {
                        @Override
                        public void run() {
                            Log.i(this, "Permanent IWLAN reg failure (IKEv2 auth failure).");
                            ImsReasonInfo imsReasonInfo = new ImsReasonInfo(
                                    ImsReasonInfo.CODE_EPDG_TUNNEL_ESTABLISH_FAILURE,
                                    ImsReasonInfo.CODE_IKEV2_AUTH_FAILURE,
                                    null);
                            int count = mSrvClassRegListeners.beginBroadcast();
                            try {
                                for (int i = 0; i < count; i++) {
                                    try {
                                        mSrvClassRegListeners.getBroadcastItem(i)
                                                .registrationChangeFailed(
                                                        ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN,
                                                        imsReasonInfo);
                                    } catch (RemoteException e) {
                                        Log.w(this, e + " " + "handleRegistrationBlockStatus() - " +
                                                "Skipping callback.");
                                    }
                                }
                            } finally {
                                mSrvClassRegListeners.finishBroadcast();
                            }
                        }
                    };
                    if (mRegCallbackHandler != null) {
                        mRegCallbackHandler.post(r);
                    }
                }
            }
        } else {
            Log.e(this, "Async result is null or exception is not null.");
        }
    }

    public void updateVoltePreference(int preference, IQtiImsExtListener listener) {
        mCi.updateVoltePref(preference, mHandler.obtainMessage(EVENT_SET_VOLTE_PREF, listener));
    }

    public void queryVoltePreference(IQtiImsExtListener listener) {
        mCi.queryVoltePref(mHandler.obtainMessage(EVENT_GET_VOLTE_PREF, listener));
    }

    private int getQtiImsExtOperationStatus(AsyncResult ar) {
        /* Default assume as successful */
        int status = QtiImsExtUtils.QTI_IMS_REQUEST_SUCCESS;
        if (ar != null && ar.exception != null) {
            /* Request failed */
            status = QtiImsExtUtils.QTI_IMS_REQUEST_ERROR;
        }
        return status;
    }

    private void handleUpdateVoltePrefResponse(AsyncResult ar) {
        IQtiImsExtListener listener = getImsInterfaceListener(ar);
        int result = getQtiImsExtOperationStatus(ar);
        Log.i(this, "handleUpdateVoltePrefResponse: result " + result);

        /* If listener is available, notify the result */
        if (listener != null) {
            try {
                listener.onVoltePreferenceUpdated(mPhoneId, result);
            } catch (Throwable t) {
                Log.e(this, "handleUpdateVoltePrefResponse exception!");
            }
        } else {
            Log.e(this, "handleUpdateVoltePrefResponse: no listener is available");
        }
    }

    private void handleQueryVoltePrefResponse(AsyncResult ar) {
        IQtiImsExtListener listener = getImsInterfaceListener(ar);
        int result = getQtiImsExtOperationStatus(ar);
        int preference = QtiImsExtUtils.QTI_IMS_VOLTE_PREF_UNKNOWN;
        if (result == QtiImsExtUtils.QTI_IMS_REQUEST_SUCCESS) {
            /* Received volte preference from lower layers */
            int[] pref = (int[])ar.result;
            preference = pref[0];
        }
        Log.i(this, "handleQueryVoltePrefResponse: result-" + result + " mode-" + preference);

        /* If listener is available, notify the result */
        if (listener != null) {
            try {
                listener.onVoltePreferenceQueried(mPhoneId, result, preference);
            } catch (Throwable t) {
                Log.e(this, "handleQueryVoltePrefResponse exception!");
            }
        } else {
            Log.e(this, "handleQueryVoltePrefResponse: no listener is available");
        }
    }

    private void onGetHandoverConfigDone(IQtiImsExtListener listener, AsyncResult ar) {
        if (listener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                int result = ar.result == null ?
                             QtiImsExtUtils.QTI_IMS_HO_INVALID :
                             (int)ar.result;
                listener.onGetHandoverConfig(mPhoneId, status, result);
            } catch (Throwable t) {
                Log.e(this, "onGetHandoverConfigDone " + t);
            }
        } else {
            Log.e(this, "onGetHandoverConfigDone listener is not valid !!!");
        }
    }

    private void onSetHandoverConfigDone(IQtiImsExtListener listener, AsyncResult ar) {
        if (listener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                listener.onSetHandoverConfig(mPhoneId, status);
            } catch (Throwable t) {
                Log.e(this, "onSetHandoverConfigDone " + t);
            }
        } else {
            Log.e(this, "onSetHandoverConfigDone listener is not valid !!!");
        }
    }

    private static boolean isIKEv2Error(ImsQmiIF.BlockStatus blockStatus) {
        //Check if it has block reason details. Block reason type is
        //ignored for now.
        if(blockStatus.hasBlockReasonDetails()) {
            ImsQmiIF.BlockReasonDetails blockReasonDetails =
            blockStatus.getBlockReasonDetails();
            //Check if it has registration failure reason type
            //and failure reason.
            return (blockReasonDetails.hasRegFailureReasonType() &&
                    blockReasonDetails.hasRegFailureReason() &&
                    //Check if there was an IKEv2 authentication failure while
                    //establishing tunnel for IWLAN ims registration.
                    (blockReasonDetails.getRegFailureReasonType() ==
                    ImsQmiIF.REG_FAILURE_REASON_TYPE_IWLAN) &&
                    (blockReasonDetails.getRegFailureReason() == IKEv2_AUTH_FAILURE));
        }
        return false;
    }

    /**
     * Get the MultiEndpoint interface handle.
     * @return IImsMultiEndpoint interface handle.
    */
    @Override
    public IImsMultiEndpoint getMultiEndpointInterface() {
        return mImsMultiEndpointImpl;
    }

    /**
     * This method notifes this ServiceSub that it has a new call.
     */
    public void onNewCall() {
        ImsVideoGlobals.getInstance().setServiceSubWithActiveCall(this);
    }

}
