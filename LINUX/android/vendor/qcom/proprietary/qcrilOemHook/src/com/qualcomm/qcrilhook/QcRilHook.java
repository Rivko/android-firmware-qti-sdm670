/******************************************************************************
 * @file    QcRilOemHook.java
 * @brief   Provides the library functions to Android Applications to add
 *          Qualcomm OEM Header to Qualcomm RIL OEM Hook Messages and send
 *          them using the RIL OEM Hook interface. This class is not thread
 *          safe and clients should not access APIs of this class after calling
 *          the dispose() function.
 *
 * ---------------------------------------------------------------------------
 *  Copyright (C) 2009 Qualcomm Technologies, Inc.
 *  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 *******************************************************************************/

package com.qualcomm.qcrilhook;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.BufferUnderflowException;
import java.io.UnsupportedEncodingException;
import java.util.Arrays;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.util.Log;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.uicc.IccUtils;
import com.qualcomm.qcrilhook.IOemHookCallback;
import com.qualcomm.qcrilmsgtunnel.IQcrilMsgTunnel;

public class QcRilHook implements IQcRilHook {
    private final static String LOG_TAG = "QC_RIL_OEM_HOOK";
    private final static boolean DBG = true;
    private final static boolean VDBG = false;

    private final String mOemIdentifier = "QOEMHOOK";

    private final static int RESPONSE_BUFFER_SIZE = 2048;
    private final static int MAX_REQUEST_BUFFER_SIZE = 1024;

    private final static int INT_SIZE = 4;

    private final static int BYTE_SIZE = 1;
    private static final int AVOIDANCE_BUFF_LEN = 164;

    private static final int DEFAULT_PHONE = 0;

    // Header consists of OEM Identifier String, Request Id(int) and Request
    // Size (int)
    private final int mHeaderSize = mOemIdentifier.length() + 2 * INT_SIZE;

    /*
     * According to C.S0016-C_v2.0 3.3.6
     * SPC Value range is from 0 to 999,999
     */
    private final static int MAX_SPC_LEN = 6;

    private final static int MAX_PDC_ID_LEN = 124;

    private IQcrilMsgTunnel mService = null;

    private static RegistrantList mRegistrants;

    private Context mContext;
    private boolean mBound = false;
    private QcRilHookCallback mQcrilHookCb = null;

    private final String ENCODING = "ISO-8859-1";
    public static final String ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW =
            "com.qualcomm.intent.action.ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW";

    public static final String QCRIL_MSG_TUNNEL_SERVICE_NAME =
            "com.qualcomm.qcrilmsgtunnel.QcrilMsgTunnelService";

    public static final String QCRIL_MSG_TUNNEL_PACKAGE_NAME = "com.qualcomm.qcrilmsgtunnel";

    @Deprecated
    public QcRilHook(Context context) {
        this(context, null);
    }

    /**
     * Constructor for QcRilHook, the context passed in cannot be null
     */
    public QcRilHook(Context context, QcRilHookCallback cb) {
        super();
        mQcrilHookCb = cb;
        mRegistrants = new RegistrantList();

        mContext = context;

        if (mContext == null) {
            throw new IllegalArgumentException("Context is null");
        }

        Intent intent = new Intent();
        intent.setClassName(QCRIL_MSG_TUNNEL_PACKAGE_NAME, QCRIL_MSG_TUNNEL_SERVICE_NAME);

        logd("Starting QcrilMsgTunnel Service");
        mContext.startService(intent);

        boolean status = mContext.bindService(intent, mQcrilMsgTunnelConnection,
                Context.BIND_AUTO_CREATE);
        logd("Attempt to bind service returned with: " + status);

        try {
            IntentFilter filter = new IntentFilter();
            filter.addAction(ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW);
            mContext.registerReceiver(mIntentReceiver, filter);
            logd("Registering for intent ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW");
        } catch (Exception e) {
            Log.e(LOG_TAG, "Uncaught Exception while while registering " +
                    "ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW intent. Reason: " + e);
        }
    }

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {

        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW)) {
                int response_id = 0;
                logd("Received Broadcast Intent ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW");

                /* Get Intent payload */
                byte[] payload = intent.getByteArrayExtra("payload");
                int instanceId = intent.getIntExtra(QmiOemHookConstants.INSTANCE_ID, 0);

                if (payload != null) {
                    /* Parse QCRILHOOK Unsol Response Header and get response_id */
                    if (payload.length < mHeaderSize) {
                        Log.e(LOG_TAG, "UNSOL_RESPONSE_OEM_HOOK_RAW incomplete header");
                        Log.e(LOG_TAG, "Expected " + mHeaderSize + " bytes. Received "
                                + payload.length + " bytes.");
                        return;
                    } else {
                        ByteBuffer response = createBufferWithNativeByteOrder(payload);
                        byte[] oem_id_bytes = new byte[mOemIdentifier.length()];
                        response.get(oem_id_bytes);
                        String oem_id_str = new String(oem_id_bytes);
                        logd("Oem ID in QCRILHOOK UNSOL RESP is " + oem_id_str);
                        if (!oem_id_str.equals(mOemIdentifier)) {
                            Log.w(LOG_TAG, "Incorrect Oem ID in QCRILHOOK UNSOL RESP. Expected "
                                    + mOemIdentifier + ". Received " + oem_id_str);
                            return;
                        }

                        int remainingSize = payload.length - mOemIdentifier.length();
                        if (remainingSize > 0) {
                            byte[] remainingPayload = new byte[remainingSize];

                            response.get(remainingPayload);
                            Message msg = Message.obtain();
                            msg.obj = remainingPayload;
                            msg.arg1 = instanceId;
                            /*
                             * Send the remaining payload in AsyncResult.result
                             * to appropriate handlers
                             */
                            //AsyncResult ar = new AsyncResult(null, remainingPayload, null);
                            AsyncResult ar = new AsyncResult(null, msg, null);
                            QcRilHook.notifyRegistrants(ar);
                        }
                    }
                }
            } else {
                Log.w(LOG_TAG, "Received Unknown Intent: action = " + action);
            }
        }
    };

    /**
     * Checks if QcRilHook has already been disposed
     *
     * @return boolean true if disposed while false if not disposed
     */
    public boolean isDisposed() {
        return mContext == null;
    }

    /**
     * Throws an IllegalStateException if QcRilHook has already been disposed
     *
     */
    private void validateInternalState() {
        if (isDisposed()) {
            throw new IllegalStateException("QcRilHook is in disposed state");
        }
    }

    public void dispose() {
        if (mContext != null) {
            if (mBound) {
                logv("dispose(): Unbinding service");
                mContext.unbindService(mQcrilMsgTunnelConnection);
                mQcrilHookCb.onQcRilHookDisconnected();
                mBound = false;
            }
            logv("dispose(): Unregistering receiver");
            mContext.unregisterReceiver(mIntentReceiver);

            mContext = null;
            mRegistrants = null;
            mQcrilHookCb = null;
        }
    }

    /**
     * Creates a ByteBuffer using the byte array passed as a parameter to this
     * method and sets the Byte Order to the native endian format.
     *
     * @param request - byte array used to populate the ByteBuffer
     * @return ByteBuffer that wraps the byte array
     */
    public static ByteBuffer createBufferWithNativeByteOrder(byte[] bytes) {
        ByteBuffer buf = ByteBuffer.wrap(bytes);
        buf.order(ByteOrder.nativeOrder());
        return buf;
    }

    /**
     * Adds the QC RIL Header to the request which consists of the OEM
     * Identifier String, Request ID, Request Size to the ByteBuffer passed to
     * this method
     *
     * @param buf - ByteBuffer to pack the request
     * @param requestId - Type of OEM request
     * @param requestSize - Size of the Request
     * @return None.
     */
    private void addQcRilHookHeader(ByteBuffer buf, int requestId, int requestSize) {
        // Add OEM identifier String
        buf.put(mOemIdentifier.getBytes());

        // Add Request ID
        buf.putInt(requestId);

        // Add Request payload length
        buf.putInt(requestSize);
    }

    /**
     * Main method that shall send the request and wait for response.
     *
     * @param requestId
     * @param request
     * @return Return Code and Response Bytes (if any) in AsyncResult Object
     */
    private AsyncResult sendRilOemHookMsg(int requestId, byte[] request) {
        return sendRilOemHookMsg(requestId, request, DEFAULT_PHONE);
    }

    private AsyncResult sendRilOemHookMsg(int requestId, byte[] request, int phoneId) {
        AsyncResult ar;
        int retVal;
        byte[] response = new byte[RESPONSE_BUFFER_SIZE];

        logv("sendRilOemHookMsg: Outgoing Data is " + IccUtils.bytesToHexString(request));

        try {
            retVal = mService.sendOemRilRequestRaw(request, response, phoneId);
            logd("sendOemRilRequestRaw returns value = " + retVal);
            if (retVal >= 0) {
                byte[] validResponseBytes = null;
                // retVal contains the actual number of bytes in the response
                if (retVal > 0) {
                    validResponseBytes = new byte[retVal];
                    System.arraycopy(response, 0, validResponseBytes, 0, retVal);
                }
                ar = new AsyncResult(retVal, validResponseBytes, null);
            } else {
                /**
                 * RIL Error was converted to negative value by
                 * sendOemRilRequestRaw method Convert it back to the positive
                 * value and create a exception to return to the caller.
                 */
                byte[] validResponseBytes = new byte[response.length];
                System.arraycopy(response, 0, validResponseBytes, 0, response.length);
                CommandException ex = CommandException.fromRilErrno(-1 * retVal);
                ar = new AsyncResult(request, validResponseBytes, ex);
            }
        } catch (RemoteException e) {
            Log.e(LOG_TAG, "sendOemRilRequestRaw RequestID = " + requestId
                    + " exception, unable to send RIL request from this application", e);

            ar = new AsyncResult(requestId, null, e);
        } catch (NullPointerException e) {
            Log.e(LOG_TAG, "NullPointerException caught at sendOemRilRequestRaw." +
                    "RequestID = " + requestId + ". Return Error");
            ar = new AsyncResult(requestId, null, e);
        }

        return ar;
    }

    /**
     * Main method that shall send the request without waiting for response.
     *
     * @param requestId
     * @param request
     * @param oemHookCb
     * @param phoneId
     */
    private void sendRilOemHookMsgAsync(int requestId, byte[] request, IOemHookCallback oemHookCb,
            int phoneId) throws NullPointerException {
        logv("sendRilOemHookMsgAsync: Outgoing Data is "
                + IccUtils.bytesToHexString(request));
        boolean requestFailed = false;
        try {
            mService.sendOemRilRequestRawAsync(request, oemHookCb, phoneId);
        } catch (RemoteException e) {
            Log.e(LOG_TAG, "sendOemRilRequestRawAsync RequestID = " + requestId
                    + " exception, unable to send RIL request from this application", e);
            requestFailed = true;
        } catch (NullPointerException e) {
            Log.e(LOG_TAG, "NullPointerException caught at sendOemRilRequestRawAsync." +
                    "RequestID = " + requestId);
            requestFailed = true;
        }
        if (requestFailed) {
            try {
                oemHookCb.onOemHookException(phoneId);
            } catch (RemoteException re) {
                Log.e(LOG_TAG, "RemoteException at OemHookException callback. " + re);
            }
        }
    }

    //========================================================================
    // FUNCTION: getLpluslSupportStatus
    //
    // DESCRIPTION: queries RIL for getLpluslSupportStatus
    //
    //
    // RETURN: boolean - ture if modem support L plus L feature
    //========================================================================
    public boolean getLpluslSupportStatus() {
        boolean status = false;
        AsyncResult ar = sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ);
        if ((ar.exception == null) && (ar.result != null)) {
            byte[] response = (byte[]) ar.result;
            status = (ByteBuffer.wrap(response).get() & 1) == 1;
        }

        logd("getLpluslSupportStatus: " + status + " exception: " + ar.exception);
        return status;
    }

    //========================================================================
    // FUNCTION: qcRilGetConfig
    //
    // DESCRIPTION: queries RIL for current configuration
    //
    // PARAM: phoneId   phoneId id: 0 or 1 or 2...
    // PARAM: mbnType   0:SW, 1:HW
    //
    // RETURN: string - the current configuration or null
    //========================================================================
    public String qcRilGetConfig(int phoneId, int mbnType) {
        validateInternalState();
        String result = null;
        byte[] payload = new byte[mHeaderSize + INT_SIZE + BYTE_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(payload);

        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_GET_CONFIG, INT_SIZE + BYTE_SIZE);
        reqBuffer.put((byte)phoneId);
        reqBuffer.putInt(mbnType);

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_CONFIG, payload);

        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_CONFIG failed w/ "
                           + ar.exception);
            return result;
        }

        if (ar.result == null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_CONFIG failed w/ null result");
            return result;
        }

        // Command executed successfully
        try {
            result = new String((byte[])ar.result, ENCODING);
        } catch (UnsupportedEncodingException e) {
            logd("unsupport " + ENCODING);
            return null;
        }
        logv("QCRIL_EVT_HOOK_GET_CONFIG returned w/ " + result);

        return result;
    }

    //========================================================================
    // FUNCTION: qcRilGetConfig
    //
    // DESCRIPTION: queries RIL for current configuration for phoneId 0
    //
    // RETURN: string - the current configuration or null
    //========================================================================
    public String qcRilGetConfig() {
        return qcRilGetConfig(0);
    }

    //========================================================================
    // FUNCTION: qcRilGetConfig
    //
    // DESCRIPTION: queries RIL for current configuration for phoneId and
    //              mbnType:0
    //
    // RETURN: string - the current configuration or null
    //========================================================================
    public String qcRilGetConfig(int phoneId) {
        return qcRilGetConfig(phoneId, 0);
    }

    //========================================================================
    // FUNCTION: qcRilSetConfig
    //
    // DESCRIPTION: send a selected configuration to RIL, with SUBs specified
    //
    // PARAM: file      file name including whole path
    // PARAM: config    config id specified by user
    // PARAM: subMask   sub mask (sub0->1, sub1->2, sub2->4, sub0&sub1->3)
    //
    // RETURN: boolean - success or fail
    //========================================================================
    public boolean qcRilSetConfig(String file, String config, int subMask) {
        return qcRilSetConfig(file, config, subMask, 0);
    }

    //========================================================================
    // FUNCTION: qcRilSetConfig
    //
    // DESCRIPTION: send a selected configuration to RIL, with SUBs specified
    //
    // PARAM: file      file name including whole path
    // PARAM: config    config id specified by user
    // PARAM: subMask   sub mask (sub0->1, sub1->2, sub2->4, sub0&sub1->3)
    // PARAM: mbnType   0: SW, 1: HW
    //
    // RETURN: boolean - success or fail
    //========================================================================

    public boolean qcRilSetConfig(String file, String config, int subMask, int mbnType) {
        validateInternalState();
        if (!config.isEmpty() && config.length() <= MAX_PDC_ID_LEN
                                && !file.isEmpty()) {
            byte[] payload = new byte[mHeaderSize + 3 * BYTE_SIZE + INT_SIZE
                    + file.length() + config.length()];
            ByteBuffer buf = createBufferWithNativeByteOrder(payload);
            /*
             * Build the payload buffer
             * Format: Header | subMask | file_name | config_id
             */
            addQcRilHookHeader(buf, QCRIL_EVT_HOOK_SET_CONFIG,
                            3 * BYTE_SIZE + INT_SIZE + file.length() + config.length());
            buf.put((byte)subMask);
            buf.putInt(mbnType);
            buf.put(file.getBytes());
            buf.put((byte)0);
            try {
                buf.put(config.getBytes(ENCODING));
            } catch (UnsupportedEncodingException e) {
                logd("unsupport " + ENCODING);
                return false;
            }
            buf.put((byte)0);

            AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_SET_CONFIG, payload);
            if (ar.exception != null) {
                Log.e(LOG_TAG, "QCRIL_EVT_HOOK_SET_CONFIG failed w/ "
                                + ar.exception);
                return false;
            }
        } else {
            Log.e(LOG_TAG, "set with incorrect config id: " + config);
            return false;
        }

        return true;
    }

    //========================================================================
    // FUNCTION: qcRilSetConfig
    //
    // DESCRIPTION: send a selected configuration to RIL, with config id the
    // same as file name. It is applicable with SUB0 in default.
    //
    // PARAM: file      file name including whole path
    //
    // RETURN: boolean - succes or fail
    //========================================================================
    public boolean qcRilSetConfig(String file) {
        return qcRilSetConfig(file, file, 1);
    }

    //========================================================================
    // FUNCTION: qcRilSetConfig
    //
    // DESCRIPTION: send a selected configuration to RIL, with SUBs specified
    // Config_id is the same as file name
    //
    // PARAM: file      file name including whole path
    // PARAM: subMask   sub mask (sub0->1, sub1->2, sub2->4, sub0&sub1->3)
    //
    // RETURN: boolean - success or fail
    //========================================================================
    public boolean qcRilSetConfig(String file, int subMask) {
        return qcRilSetConfig(file, file, subMask);
    }

    //========================================================================
    // FUNCTION: qcRilGetQcVersionOfFile
    //
    // DESCRIPTION: Get the QC version of a given MBN file
    //
    // PARAM: file      file name including whole path
    //
    // RETURN: null if fails, QC version if succeeds
    //========================================================================
    public byte[] qcRilGetQcVersionOfFile(String file) {
        validateInternalState();
        if (file.isEmpty())
            return null;

        byte[] payload = new byte[mHeaderSize + file.getBytes().length];
        ByteBuffer buf = createBufferWithNativeByteOrder(payload);
        addQcRilHookHeader(buf, QCRIL_EVT_HOOK_GET_QC_VERSION_OF_FILE, file.getBytes().length);
        buf.put(file.getBytes());

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_QC_VERSION_OF_FILE, payload);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_QC_VERSION_OF_FILE failed w/ "
                                    + ar.exception);
            return null;
        }
        if (ar.result == null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_QC_VERSION_OF_FILE failed w/ null result");
            return null;
        }

        // Command executed successfully
        logv("QCRIL_EVT_HOOK_GET_QC_VERSION_OF_FILE returned w/ " + (byte[])ar.result);
        return (byte[])ar.result;
    }

    //========================================================================
    // FUNCTION: qcRilGetOemVersionOfFile
    //
    // DESCRIPTION: Get the OEM version of a given MBN file
    //
    // PARAM: file      file name including whole path
    //
    // RETURN: null if fails, OEM version if succeeds
    //========================================================================
    public byte[] qcRilGetOemVersionOfFile(String file) {
        validateInternalState();
        if (file.isEmpty())
            return null;

        byte[] payload = new byte[mHeaderSize + file.getBytes().length];
        ByteBuffer buf = createBufferWithNativeByteOrder(payload);
        addQcRilHookHeader(buf, QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_FILE, file.getBytes().length);
        buf.put(file.getBytes());

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_FILE, payload);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_FILE failed w/ "
                                    + ar.exception);
            return null;
        }
        if (ar.result == null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_FILE failed w/ null result");
            return null;
        }

        // Command executed successfully
        logv("QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_FILE returned w/ " + (byte[])ar.result);
        return (byte[])ar.result;
    }

    //========================================================================
    // FUNCTION: qcRilGetQcVersionOfID
    //
    // DESCRIPTION: Get the QC version of the given config id
    //
    // PARAM: null
    //
    // RETURN: null if fails, QC version if succeeds
    //========================================================================
    public byte[] qcRilGetQcVersionOfID(String configId) {
        validateInternalState();
        if (configId.isEmpty() || configId.length() > MAX_PDC_ID_LEN) {
            Log.w(LOG_TAG, "invalid config id");
            return null;
        }

        byte[] payload = new byte[mHeaderSize + configId.length()];
        ByteBuffer buf = createBufferWithNativeByteOrder(payload);
        addQcRilHookHeader(buf, QCRIL_EVT_HOOK_GET_QC_VERSION_OF_ID,
                                                configId.length());
        try {
            buf.put(configId.getBytes(ENCODING));
        } catch (UnsupportedEncodingException e) {
            logd("unsupport " + ENCODING);
            return null;
        }

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_QC_VERSION_OF_ID, payload);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_QC_VERSION_OF_ID failed w/ "
                                    + ar.exception);
            return null;
        }
        if (ar.result == null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_QC_VERSION_OF_ID failed w/ null result");
            return null;
        }

        // Command executed successfully
        logv("QCRIL_EVT_HOOK_GET_QC_VERSION_OF_ID returned w/ " + (byte[])ar.result);
        return (byte[])ar.result;
    }

    //========================================================================
    // FUNCTION: qcRilGetOemVersionOfID
    //
    // DESCRIPTION: Get the OEM version of the given config_id
    //
    // PARAM: null
    //
    // RETURN: null if fails, OEM version if succeeds
    //========================================================================
    public byte[] qcRilGetOemVersionOfID(String config_id) {
        validateInternalState();
        if (config_id.isEmpty() || config_id.length() > MAX_PDC_ID_LEN) {
            Log.w(LOG_TAG, "invalid config_id");
            return null;
        }

        byte[] payload = new byte[mHeaderSize + config_id.length()];
        ByteBuffer buf = createBufferWithNativeByteOrder(payload);
        addQcRilHookHeader(buf, QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_ID,
                                                config_id.length());
        try {
            buf.put(config_id.getBytes(ENCODING));
        } catch (UnsupportedEncodingException e) {
            logd("unsupport " + ENCODING);
            return null;
        }

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_ID, payload);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_ID failed w/ "
                                    + ar.exception);
            return null;
        }
        if (ar.result == null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_ID failed w/ null result");
            return null;
        }

        // Command executed successfully
        logv("QCRIL_EVT_HOOK_GET_OEM_VERSION_OF_ID returned w/ " + (byte[])ar.result);
        return (byte[])ar.result;
    }

    //========================================================================
    // FUNCTION: qcRilActivateConfig
    //
    // DESCRIPTION: Activate the configuration for the specific phoneID
    //
    // PARAM: phoneId        phoneId that needs to be activated
    //
    // RETURN: true if activate successfully, otherwise false
    //========================================================================
    public boolean qcRilActivateConfig(int phoneId) {
        return qcRilActivateConfig(phoneId, 0);
    }

    //========================================================================
    // FUNCTION: qcRilActivateConfig
    //
    // DESCRIPTION: Activate the configuration for the specific phoneID
    //
    // PARAM: phoneId        phoneId that needs to be activated
    // PARAM: mbnType        0:SW, 1: HW
    //
    // RETURN: true if activate successfully, otherwise false
    //========================================================================
    public boolean qcRilActivateConfig(int phoneId, int mbnType) {
        validateInternalState();
        byte[] payload = new byte[mHeaderSize + INT_SIZE + BYTE_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(payload);
        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_ACT_CONFIGS, INT_SIZE + BYTE_SIZE);
        reqBuffer.put((byte)phoneId);
        reqBuffer.putInt(mbnType);

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_ACT_CONFIGS, payload);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_ACT_CONFIGS failed w/ "
                                                + ar.exception);
            return false;
        }
        return true;
    }

    //========================================================================
    // FUNCTION: qcRilValidateConfig
    //
    // DESCRIPTION: Validate the current configuration in modem, compared with
    // the given golden/reference MBN
    //
    // PARAM: configId     config Id of reference MBN
    //        phoneId          phoneId that needs to be verified
    //
    // RETURN: true if send commands to modem successfully, otherwise false
    //========================================================================
    public boolean qcRilValidateConfig(String configId, int phoneId) {
        validateInternalState();
        if (configId.isEmpty() || configId.length() > MAX_PDC_ID_LEN) {
            Log.w(LOG_TAG, "invalid config id");
            return false;
        }
        byte[] payload = new byte[mHeaderSize + 2 * BYTE_SIZE + configId.length()];
        ByteBuffer buf = createBufferWithNativeByteOrder(payload);
        addQcRilHookHeader(buf, QCRIL_EVT_HOOK_VALIDATE_CONFIG,
                            2 * BYTE_SIZE + configId.length());
        buf.put((byte)phoneId);
        try {
            buf.put(configId.getBytes(ENCODING));
        } catch (UnsupportedEncodingException e) {
            logd("unsupport " + ENCODING);
            return false;
        }
        buf.put((byte)0);

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_META_INFO, payload);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_VALIDATE_CONFIG failed w/ "
                                                + ar.exception);
            return false;
        }
        return true;
    }

    //========================================================================
    // FUNCTION: qcRilGetAvailableConfigs <device>
    //
    // DESCRIPTION: get a list of available configurations from RIL
    //
    // RETURN: list of strings - available configurations or null
    //========================================================================
    @Deprecated
    public String[] qcRilGetAvailableConfigs(String device) {
        Log.w(LOG_TAG, "qcRilGetAvailableConfigs is deprecated");
        return null;
    }

    //========================================================================
    // FUNCTION: qcRilGetAllConfigs
    //
    // DESCRIPTION: get all configurations maintained by modem
    //
    // RETURN: true on SUCCESS and false on FAILURE
    //========================================================================
    public boolean qcRilGetAllConfigs() {
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_GET_AVAILABLE_CONFIGS);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_AVAILABLE_CONFIGS failed w/ "
                    + ar.exception);
            return false;
        }
        return true;
    }

    //========================================================================
    // FUNCTION: qcRilCleanupConfigs
    //
    // DESCRIPTION: Clean up all configurations loaded in EFS
    //
    // RETURN: boolean - success or fail
    //========================================================================
    public boolean qcRilCleanupConfigs() {
        boolean retval = false;
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_DELETE_ALL_CONFIGS);

        if (ar.exception == null) {
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL_EVT_HOOK_DELETE_ALL_CONFIGS failed w/ "
                            + ar.exception);
        }

        return retval;
    }
    public boolean qcRilDeactivateConfigs() {
        return qcRilDeactivateConfigs(0);
    }

    //========================================================================
    // FUNCTION: qcRilDeactivateConfigs
    //
    // DESCRIPTION: Deactivate the current config for all SUBs for an mbnType
    //
    // PARAM: mbnType : 0 - SW, 1- HW
    //
    // RETURN: boolean - success or fail
    //========================================================================
    public boolean qcRilDeactivateConfigs(int mbnType) {
        validateInternalState();
        boolean retval = false;
        byte[] payload = new byte[mHeaderSize + INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(payload);

        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_DEACT_CONFIGS, INT_SIZE);
        reqBuffer.putInt(mbnType);

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_DEACT_CONFIGS, payload);

        if (ar.exception == null) {
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL_EVT_HOOK_DEACT_CONFIGS failed w/ "
                            + ar.exception);
        }

        return retval;
    }

    public boolean qcRilSelectConfig(String config, int subMask) {
        return qcRilSelectConfig(config, subMask, 0);
    }

    //========================================================================
    // FUNCTION: qcRilSelectConfig
    //
    // DESCRIPTION: Select the config based on configId, mask and mbnType
    //
    // PARAM: config : configId of the MBN
    // PARAM: subMask   sub mask (sub0->1, sub1->2, sub2->4, sub0&sub1->3)
    // PARAM: mbnType : 0 - SW, 1- HW
    //
    // RETURN: boolean - success or fail
    //========================================================================
    public boolean qcRilSelectConfig(String config, int subMask, int mbnType) {
        validateInternalState();
        byte[] payload = null;
        if (!config.isEmpty() && config.length() <= MAX_PDC_ID_LEN) {
            try {
                payload = new byte[mHeaderSize + BYTE_SIZE + INT_SIZE +
                        config.getBytes(ENCODING).length];
                ByteBuffer buf = createBufferWithNativeByteOrder(payload);
                addQcRilHookHeader(buf, QCRIL_EVT_HOOK_SEL_CONFIG, BYTE_SIZE +
                                                INT_SIZE + config.getBytes(ENCODING).length);
                buf.put((byte)subMask);
                buf.putInt(mbnType);
                buf.put(config.getBytes(ENCODING));
            } catch (UnsupportedEncodingException e) {
                logd("unsupport " + ENCODING);
                return false;
            }

            AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_SEL_CONFIG, payload);
            if (ar.exception != null) {
                Log.e(LOG_TAG, "QCRIL_EVT_HOOK_SEL_CONFIG failed w/ "
                                + ar.exception);
                return false;
            }
        } else {
            Log.e(LOG_TAG, "select with incorrect config id: " + config);
            return false;
        }
        return true;
    }

    public String qcRilGetMetaInfoForConfig(String config) {
        return qcRilGetMetaInfoForConfig(config, 0);
    }


    public String qcRilGetMetaInfoForConfig(String config, int mbnType) {
        validateInternalState();
        String result = null;
        byte[] payload = null;

        if (!config.isEmpty() && config.length() <= MAX_PDC_ID_LEN) {
            try {
                payload = new byte[mHeaderSize + INT_SIZE + config.getBytes(ENCODING).length];
                ByteBuffer buf = createBufferWithNativeByteOrder(payload);
                addQcRilHookHeader(buf, QCRIL_EVT_HOOK_GET_META_INFO,
                                                INT_SIZE + config.getBytes(ENCODING).length);
                buf.putInt(mbnType);
                buf.put(config.getBytes(ENCODING));
            } catch (UnsupportedEncodingException e) {
                logd("unsupport " + ENCODING);
                return null;
            }

            AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_GET_META_INFO, payload);

            if (ar.exception != null) {
                Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_META_INFO failed w/ "
                                + ar.exception);
                return result;
            }

            if (ar.result == null) {
                Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_META_INFO failed w/ null result");
                return result;
            }

            // Command executed successfully
            try {
                result = new String((byte[])ar.result, ENCODING);
            } catch (UnsupportedEncodingException e) {
                logd("unsupport " + ENCODING);
                return null;
            }
            logv("QCRIL_EVT_HOOK_GET_META_INFO returned w/ " + result);
        } else {
            Log.e(LOG_TAG, "get meta info with incorrect config id: " + config);
        }
        return result;
    }

    //========================================================================
    // FUNCTION: qcRilGoDormant
    //
    // DESCRIPTION:
    //
    // RETURN:
    //========================================================================
    public boolean qcRilGoDormant(String interfaceName) {
        boolean retval = false;
        AsyncResult result = sendQcRilHookMsg(QCRILHOOK_GO_DORMANT, interfaceName);

        if (result.exception == null) {
            // Command executed successfully
            retval = true;
        } else {
            Log.w(LOG_TAG, "Go Dormant Command returned Exception: " + result.exception);
        }

        return retval;
    }

    public boolean qcRilSetCdmaSubSrcWithSpc(int cdmaSubscription, String spc) {
        validateInternalState();
        boolean retval = false;

        logv("qcRilSetCdmaSubSrcWithSpc: Set Cdma Subscription to "+ cdmaSubscription);

        if (!spc.isEmpty() && spc.length() <= MAX_SPC_LEN)
        {
            byte[] payload = new byte[BYTE_SIZE + spc.length()];
            /*
             *  Build the payload buffer
             *  Format : | cdmaSubscription(1) | SPC(1-6)
             */
            ByteBuffer buf = createBufferWithNativeByteOrder(payload);
            buf.put((byte)cdmaSubscription);
            buf.put(spc.getBytes());

            AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_SET_CDMA_SUB_SRC_WITH_SPC, payload);

            if (ar.exception == null) {
                if (ar.result != null) {
                    byte[] result = (byte[])ar.result;
                    ByteBuffer byteBuf = ByteBuffer.wrap(result);
                    byte succeed = byteBuf.get();
                    logv("QCRIL Set Cdma Subscription Source Command "
                            + ((succeed == 1) ? "Succeed." : "Failed."));
                    if (succeed == 1) {
                        retval = true;
                    } else {
                        retval = false;
                    }
                }
            } else {
                Log.e(LOG_TAG, "QCRIL Set Cdma Subscription Source Command returned Exception: " +
                        ar.exception);
            }
        } else {
            Log.e(LOG_TAG, "QCRIL Set Cdma Subscription Source Command incorrect SPC: " + spc);
        }
        return retval;
    }

    public byte[] qcRilSendProtocolBufferMessage(byte[] protocolBuffer, int phoneId) {
        byte[] returnValue = null;
        logv("qcRilSendProtoBufMessage: protocolBuffer" + protocolBuffer.toString());

        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_PROTOBUF_MSG, protocolBuffer, phoneId);

        if (ar.exception != null){
            Log.e(LOG_TAG, "qcRilSendProtoBufMessage: Exception " + ar.exception);
            return returnValue;
        }
        if (ar.result == null) {
            Log.e(LOG_TAG, "QCRIL_EVT_HOOK_PROTOBUF_MSG returned null");
            return returnValue;
        }
        return (byte[])ar.result;
    }

    public boolean qcRilSetTuneAway(boolean tuneAway) {
        boolean returnValue = false;
        logd("qcRilSetTuneAway: tuneAway Value to be set to " + tuneAway);

        byte payload = (byte)0;
        if (tuneAway) {
           payload = (byte)1;
        }
        logv("qcRilSetTuneAway: tuneAway payload " + payload);

        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_SET_TUNEAWAY, payload);

        if (ar.exception == null){
            returnValue = true;
        } else {
            Log.e(LOG_TAG, "qcRilSetTuneAway: Exception " + ar.exception);
        }
        return returnValue;
    }

    public boolean qcRilGetTuneAway() {
        boolean tuneAway = false;

        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_GET_TUNEAWAY);

        if (ar.exception == null) {
            if (ar.result != null) {
                byte[] response = (byte[])ar.result;
                ByteBuffer byteBuffer = ByteBuffer.wrap(response);
                byte tuneAwayValue = byteBuffer.get();
                logd("qcRilGetTuneAway: tuneAwayValue " + tuneAwayValue);

                if (tuneAwayValue == 1) {
                    tuneAway = true;
                }
             } else {
                Log.e(LOG_TAG, "qcRilGetTuneAway: Null Response");
             }
        } else{
           Log.e(LOG_TAG, "qcRilGetTuneAway: Exception " + ar.exception);
        }
        return tuneAway;
    }

    public boolean qcRilSetPrioritySubscription(int priorityIndex) {
        boolean returnValue = false;
        logv("qcRilSetPrioritySubscription: PrioritySubscription to be set to"
                + priorityIndex);

        byte payload = (byte)priorityIndex;
        logv("qcRilSetPrioritySubscription: PrioritySubscription payload " + payload);

        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_SET_PAGING_PRIORITY, payload);

        if (ar.exception == null){
            returnValue = true;
        } else {
            Log.e(LOG_TAG, "qcRilSetPrioritySubscription: Exception " + ar.exception);
        }
        return returnValue;
    }

    public int qcRilGetCsgId(){
        int csgId = 0xFFFFFFFF;
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_GET_CSG_ID);
        if (ar.exception == null) {
            if (ar.result != null) {
                byte[] response = (byte[]) ar.result;
                csgId = ByteBuffer.wrap(response).get();
                logd("qcRilGetCsgId: csg Id " + csgId);
            } else {
                Log.e(LOG_TAG, "qcRilGetCsgId: Null Response");
            }
        } else {
            Log.e(LOG_TAG, "qcRilGetCsgId: Exception " + ar.exception);
        }
        return csgId;
    }

    public int qcRilGetPrioritySubscription() {
        int subscriptionIndex = 0;
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_GET_PAGING_PRIORITY);
        if (ar.exception == null) {
            if (ar.result != null) {
                byte[] response = (byte[]) ar.result;
                subscriptionIndex = ByteBuffer.wrap(response).get();
                logv("qcRilGetPrioritySubscription: subscriptionIndex "
                        + subscriptionIndex);
            } else {
                Log.e(LOG_TAG, "qcRilGetPrioritySubscription: Null Response");
            }
        } else {
            Log.e(LOG_TAG, "qcRilGetPrioritySubscription: Exception " + ar.exception);
        }
        return subscriptionIndex;
    }

    public boolean qcRilInformShutDown(int phoneId) {
        logd("QCRIL Inform shutdown for phoneId " + phoneId);
        OemHookCallback oemHookCb = new OemHookCallback(null) {
            @Override
            public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
                logd("QCRIL Inform shutdown DONE!");
            }
        };
        sendQcRilHookMsgAsync(QCRIL_EVT_HOOK_INFORM_SHUTDOWN, null, oemHookCb, phoneId);

        return true;
    }

    public boolean qcRilCdmaAvoidCurNwk() {
        boolean retval = false;
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_CDMA_AVOID_CUR_NWK);

        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL Avoid the current cdma network Command returned Exception: "
                    + ar.exception);
        }
        return retval;
    }

    public boolean qcRilSetFieldTestMode(int phoneId, byte ratType, int enable) {
        validateInternalState();
        boolean retval = false;

        byte[] request = new byte[mHeaderSize + 8];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_ENABLE_ENGINEER_MODE, 0);
        reqBuffer.putInt(ratType);
        reqBuffer.putInt(enable);
        logd("enable = " + enable + "ratType =" + ratType);
        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_ENABLE_ENGINEER_MODE,
                request, phoneId);
        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL enable engineer mode cmd returned exception: "
                    + ar.exception);
        }
        return retval;
    }

    public boolean qcRilCdmaClearAvoidanceList() {
        boolean retval = false;
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_CDMA_CLEAR_AVOIDANCE_LIST);

        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL Clear the cdma avoidance list Command returned Exception: "
                    + ar.exception);
        }
        return retval;
    }

    public byte[] qcRilCdmaGetAvoidanceList() {
        byte[] retval = null;
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_CDMA_GET_AVOIDANCE_LIST);

        if (ar.exception == null) {
            if (ar.result != null) {
                /*
                 * Avoidance list result buffer
                 * Format : Num of Valid Elements(4) | SID(4) | NID(4) | MNC(4) | MCC(4) | ...
                 */
                byte[] result = (byte[])ar.result;
                if (result.length == AVOIDANCE_BUFF_LEN) {
                    // The size of the avoidance list result buffer
                    // must be 164(4 + 16 * 10)
                    retval = result;
                } else {
                    Log.e(LOG_TAG, "QCRIL Get unexpected cdma avoidance list buffer length: "
                            + result.length);
                }
            } else {
                Log.e(LOG_TAG, "QCRIL Get cdma avoidance list command returned a null result.");
            }

        } else {
            Log.e(LOG_TAG, "QCRIL Get the cdma avoidance list Command returned Exception: "
                    + ar.exception);
        }

        return retval;
    }

    public boolean qcRilPerformIncrManualScan(int phoneId) {
        validateInternalState();
        boolean retval = false;
        byte[] request = new byte[mHeaderSize];

        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_PERFORM_INCREMENTAL_NW_SCAN, phoneId);

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_PERFORM_INCREMENTAL_NW_SCAN,
                request, phoneId);

        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL perform incr manual scan returned exception "
                    + ar.exception);
        }
        return retval;
    }

    public boolean qcRilAbortNetworkScan(int phoneId) {
        validateInternalState();
        boolean retval = false;
        byte[] request = new byte[mHeaderSize];

        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_ABORT_NW_SCAN, phoneId);

        AsyncResult ar = sendRilOemHookMsg(QCRIL_EVT_HOOK_ABORT_NW_SCAN,
                request, phoneId);

        if (ar.exception == null) {
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL cancel ongoing nw scan returned exception "
                    + ar.exception);
        }
        return retval;
    }

    public boolean qcrilSetBuiltInPLMNList(byte[] payload, int phoneId) {
        validateInternalState();
        boolean retval = false;
        if (payload == null) {
            Log.e(LOG_TAG, "payload is null");
            return false;
        }

        byte[] request = new byte[mHeaderSize + payload.length];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, QCRIL_EVT_HOOK_SET_BUILTIN_PLMN_LIST,
                payload.length);
        reqBuffer.put(payload);

        AsyncResult ar =  sendRilOemHookMsg(QCRIL_EVT_HOOK_SET_BUILTIN_PLMN_LIST,
                request, phoneId);
        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL set builtin PLMN list returned exception: "
                    + ar.exception);
        }
        return retval;
    }

    /**
     * FuncName: qcRilSetPreferredNetworkAcqOrder
     *
     * Description: Set preferred network acq order.
     *
     * @param acqOrder       acq order
     * @param phoneId            phoneId
     *
     * @return  true Or false
     */
    public boolean qcRilSetPreferredNetworkAcqOrder(int acqOrder, int phoneId) {
        validateInternalState();
        boolean retval = false;
        int requestId = QCRIL_EVT_HOOK_SET_PREFERRED_NETWORK_ACQ_ORDER;
        byte[] request = new byte[mHeaderSize + INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);


        logd("acq order: " + acqOrder);
        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        reqBuffer.putInt(acqOrder);

        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);
        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL set acq order cmd returned exception: " + ar.exception);
        }

        return retval;
    }

    /**
     * FuncName: qcRilGetPreferredNetworkAcqOrder
     *
     * Description: Get preferred network acq order.
     *
     * @param phoneId            phoneId
     *
     * @return acq order
     */
    public byte qcRilGetPreferredNetworkAcqOrder(int phoneId) {
        validateInternalState();
        byte acq_order = 0;
        int requestId = QCRIL_EVT_HOOK_GET_PREFERRED_NETWORK_ACQ_ORDER;
        byte[] request = new byte[mHeaderSize];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);

        if (ar.exception == null){
            if (ar.result != null) {
                byte[] result = (byte[])ar.result;
                ByteBuffer byteBuf = ByteBuffer.wrap(result);
                acq_order = byteBuf.get();
                logd("acq order is " + acq_order);
            } else {
                Log.e(LOG_TAG, "no acq order result return");
            }
        } else {
            Log.e(LOG_TAG, "QCRIL set acq order cmd returned exception: " + ar.exception);
        }

        return acq_order;
    }

    /**
     * FuncName: qcRilSetLteTuneaway
     *
     * Description: Enable/Disable LTE tune away on given phoneId.
     *
     * @param enable       Enable/Disable LTE tune away
     * @param phoneId            phoneId
     *
     * @return  true Or false
     */
    public boolean qcRilSetLteTuneaway(boolean enable, int phoneId) {
        validateInternalState();
        boolean retval = false;
        byte tuneaway = enable ? (byte)1 : 0;
        int requestId = QCRIL_EVT_HOOK_SET_LTE_TUNE_AWAY;
        byte[] request = new byte[mHeaderSize + BYTE_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);


        logd("qcRilSetLteTuneaway enable :" +  enable);
        addQcRilHookHeader(reqBuffer, requestId, BYTE_SIZE);
        reqBuffer.put(tuneaway);

        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);
        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL set lte tune away returned exception: " + ar.exception);
        }

        return retval;
    }

    /**
     * FuncName: qcRilSendDataEnableStatus
     *
     * Description: Send data enable status
     *
     * @param enable (1-enabled, 0-disabled)
     *
     * @return
     */

    public void qcRilSendDataEnableStatus(int enable, int phoneId) {
        validateInternalState();
        int requestId = QCRIL_EVT_HOOK_SET_IS_DATA_ENABLED;
        OemHookCallback oemHookCb = new OemHookCallback(null) {
            @Override
            public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
                logd("QCRIL send data enable status DONE!");
            }
        };
        byte[] request = new byte[mHeaderSize + INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);
        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        reqBuffer.putInt(enable);
        sendRilOemHookMsgAsync(requestId, request, oemHookCb, phoneId);
    }

    /**
     * FuncName: qcRilSendDataRoamingEnableStatus
     *
     * Description: Send data roaming enable status
     *
     * @param enable (1-enabled, 0-disabled)
     *
     * @return
     */

    public void qcRilSendDataRoamingEnableStatus(int enable, int phoneId) {
        validateInternalState();
        int requestId = QCRIL_EVT_HOOK_SET_IS_DATA_ROAMING_ENABLED;
        OemHookCallback oemHookCb = new OemHookCallback(null) {
            @Override
            public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
                logd("QCRIL send data roaming enable status DONE!");
            }
        };
        byte[] request = new byte[mHeaderSize + INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);
        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        reqBuffer.putInt(enable);
        sendRilOemHookMsgAsync(requestId, request, oemHookCb, phoneId);
    }

    /**
     * FuncName: qcRilSendApnInfo
     *
     * Description: Send type and apn info
     *
     * Format of oemhook message:
     * int32   apn_type_len;
     * char    apn_type[apn_type_len]; //null terminated string
     * int32   apn_name_len;
     * char    apn_name[apn_name_len]; //null terminated string
     * int32   is_apn_valid; //indicates whether apn is valid or has been removed
     *
     * @param type  type of apn (internet, mms, supl etc)
     * @param apn   apn as mentioned in ApnSettings
     *
     * @return
     */

    public void qcRilSendApnInfo(String type, String apn, int isValid, int phoneId) {
        validateInternalState();
        int requestId = QCRIL_EVT_HOOK_SET_APN_INFO;
        OemHookCallback oemHookCb = new OemHookCallback(null) {
            @Override
            public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
                logd("QCRIL send apn info DONE!");
            }
        };
        int payloadSize = 3 * INT_SIZE + type.getBytes().length +
                apn.getBytes().length + 2 * BYTE_SIZE;
        if (payloadSize > MAX_REQUEST_BUFFER_SIZE) {
            Log.e(LOG_TAG, "APN sent is larger than maximum buffer. Bail out");
            return;
        }
        byte[] request = new byte[mHeaderSize + payloadSize];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);
        addQcRilHookHeader(reqBuffer, requestId, payloadSize);
        reqBuffer.putInt(type.getBytes().length + 1);
        reqBuffer.put(type.getBytes());
        reqBuffer.put((byte)0);
        reqBuffer.putInt(apn.getBytes().length + 1);
        reqBuffer.put(apn.getBytes());
        reqBuffer.put((byte)0);
        reqBuffer.putInt(isValid);
        sendRilOemHookMsgAsync(requestId, request, oemHookCb, phoneId);
    }

     public boolean qcRilSendDDSInfo(int dds, int reason, int phoneId) {
        validateInternalState();
        boolean retval = false;
        int requestId = QCRIL_EVT_HOOK_SET_DATA_SUBSCRIPTION;
        byte[] request = new byte[mHeaderSize + 2 * INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        logd("dds phoneId: " + dds + " reason: " + reason);
        addQcRilHookHeader(reqBuffer, requestId, 2 * INT_SIZE);
        reqBuffer.putInt(dds);
        reqBuffer.putInt(reason);

        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);
        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL send dds sub info returned exception: " + ar.exception);
        }
        return retval;
    }

     public boolean setLocalCallHold(int phoneId, boolean enable) {
        validateInternalState();
        boolean retval = false;
        byte lchStatus = enable ? (byte)1 : 0;
        int requestId = QCRIL_EVT_SET_LOCAL_CALL_HOLD;
        byte[] request = new byte[mHeaderSize + BYTE_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        logd("setLocalCallHold: " + phoneId + " " + enable);
        addQcRilHookHeader(reqBuffer, requestId, BYTE_SIZE);
        reqBuffer.put(lchStatus);

        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);
        if (ar.exception == null){
            logd("setLocalCallHold success");
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL setLocalCallHold returned exception: " + ar.exception);
        }
        return retval;
    }

    /**
     * FuncName: qcRilSetPreferredNetworkBandPref
     *
     * Description: Set preferred network band pref.
     *
     * @param bandPref       band pref
     * @param phoneId            phoneId
     *
     * @return  true Or false
     */
    public boolean qcRilSetPreferredNetworkBandPref(int bandPref, int phoneId) {
        validateInternalState();
        boolean retval = false;
        int requestId = QCRIL_EVT_HOOK_SET_PREFERRED_NETWORK_BAND_PREF;
        byte[] request = new byte[mHeaderSize + INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);


        logd("band pref: " + bandPref);
        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        reqBuffer.putInt(bandPref);

        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);
        if (ar.exception == null){
            retval = true;
        } else {
            Log.e(LOG_TAG, "QCRIL set band pref cmd returned exception: " + ar.exception);
        }

        return retval;
    }

    /**
     * FuncName: qcRilGetPreferredNetworkBandPref
     *
     * Description: Get preferred network band pref.
     *
     * @param bandType       specify band type(LTE,TDS...)
     * @param phoneId            phoneId
     *
     * @return band pref
     */
    public byte qcRilGetPreferredNetworkBandPref(int bandType, int phoneId) {
        validateInternalState();
        byte band_pref = 0;
        int requestId = QCRIL_EVT_HOOK_GET_PREFERRED_NETWORK_BAND_PREF;
        byte[] request = new byte[mHeaderSize];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        reqBuffer.putInt(bandType);

        AsyncResult ar = sendRilOemHookMsg(requestId, request, phoneId);

        if (ar.exception == null){
            if (ar.result != null) {
                byte[] result = (byte[])ar.result;
                ByteBuffer byteBuf = ByteBuffer.wrap(result);
                band_pref = byteBuf.get();
                logd("band pref is " + band_pref);
            } else {
                Log.e(LOG_TAG, "no band pref result return");
            }
        } else {
            Log.e(LOG_TAG, "QCRIL get band perf cmd returned exception: " + ar.exception);
        }

        return band_pref;
    }

    /**
     * FuncName: qcRilGetSlotStatus
     *
     * Description: Get Slot status info.
     *
     * @param NULL
     *
     * @return null if fails, RIL_SlotsStatus if succeeds
     */
    public byte[] qcRilGetSlotStatus() {
        AsyncResult ar = sendQcRilHookMsg(QCRIL_EVT_HOOK_GET_SLOTS_STATUS_REQ);
        if (ar.exception != null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_SLOTS_STATUS_REQ failed w/ "
                                    + ar.exception);
            return null;
        }
        if (ar.result == null) {
            Log.w(LOG_TAG, "QCRIL_EVT_HOOK_GET_SLOTS_STATUS_REQ failed w/ null result");
            return null;
        }

        // Command executed successfully
        logv("QCRIL_EVT_HOOK_GET_SLOTS_STATUS_REQ returned w/ " + (byte[])ar.result);
        return (byte[])ar.result;
    }

    public AsyncResult sendQcRilHookMsg(int requestId) {
        validateInternalState();
        byte[] request = new byte[mHeaderSize];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, 0);

        return sendRilOemHookMsg(requestId, request);
    }

    public AsyncResult sendQcRilHookMsg(int requestId, byte payload) {
        return sendQcRilHookMsg(requestId, payload, DEFAULT_PHONE);
    }
    public AsyncResult sendQcRilHookMsg(int requestId, byte payload, int phoneId) {
        validateInternalState();
        byte[] request = new byte[mHeaderSize + BYTE_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, BYTE_SIZE);
        reqBuffer.put(payload);

        return sendRilOemHookMsg(requestId, request, phoneId);
    }

    public AsyncResult sendQcRilHookMsg(int requestId, byte[] payload) {
        return sendQcRilHookMsg(requestId, payload, DEFAULT_PHONE);
    }
    public AsyncResult sendQcRilHookMsg(int requestId, byte[] payload, int phoneId) {
        validateInternalState();
        byte[] request = new byte[mHeaderSize + payload.length];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, payload.length);
        reqBuffer.put(payload);

        return sendRilOemHookMsg(requestId, request, phoneId);
    }

    public AsyncResult sendQcRilHookMsg(int requestId, int payload) {
        validateInternalState();
        byte[] request = new byte[mHeaderSize + INT_SIZE];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, INT_SIZE);
        reqBuffer.putInt(payload);

        return sendRilOemHookMsg(requestId, request);
    }

    public AsyncResult sendQcRilHookMsg(int requestId, String payload) {
        validateInternalState();
        byte[] request = new byte[mHeaderSize + payload.length()];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, payload.length());
        reqBuffer.put(payload.getBytes());

        return sendRilOemHookMsg(requestId, request);
    }

    public void sendQcRilHookMsgAsync(int requestId, byte[] payload, OemHookCallback oemHookCb) {
        sendQcRilHookMsgAsync(requestId, payload, oemHookCb, DEFAULT_PHONE);
    }

    public void sendQcRilHookMsgAsync(int requestId, byte[] payload, OemHookCallback oemHookCb,
            int phoneId) {
        validateInternalState();
        int payloadLength = 0;
        if (payload != null) {
            payloadLength = payload.length;
        }

        byte[] request = new byte[mHeaderSize + payloadLength];
        ByteBuffer reqBuffer = createBufferWithNativeByteOrder(request);

        addQcRilHookHeader(reqBuffer, requestId, payloadLength);
        if (payload != null) {
            reqBuffer.put(payload);
        }
        sendRilOemHookMsgAsync(requestId, request, oemHookCb, phoneId);
    }

    public static void register(Handler h, int what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        synchronized (mRegistrants) {
            mRegistrants.add(r);
        }
    }

    public static void unregister(Handler h) {
        synchronized (mRegistrants) {
            mRegistrants.remove(h);
        }
    }

    public void registerForFieldTestData(Handler h, int what, Object obj) {
        // TODO Auto-generated method stub

    }

    public void unregisterForFieldTestData(Handler h) {
        // TODO Auto-generated method stub

    }

    public void registerForExtendedDbmIntl(Handler h, int what, Object obj) {
        // TODO Auto-generated method stub

    }

    public void unregisterForExtendedDbmIntl(Handler h) {
        // TODO Auto-generated method stub

    }

    protected void finalize() {
        logv("is destroyed");
    }

    public static void notifyRegistrants(AsyncResult ar) {
        if( mRegistrants != null) {
            mRegistrants.notifyRegistrants(ar);
        } else {
            Log.e(LOG_TAG, "QcRilOemHook notifyRegistrants Failed");
        }
    }

    private ServiceConnection mQcrilMsgTunnelConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = IQcrilMsgTunnel.Stub.asInterface(service);
            if (mService == null) {
                Log.e(LOG_TAG, "QcrilMsgTunnelService Connect Failed (onServiceConnected)");
            } else {
                logd("QcrilMsgTunnelService Connected Successfully (onServiceConnected)");
            }
            mBound = true;
            if (mQcrilHookCb != null) {
                logd("Calling onQcRilHookReady callback");
                mQcrilHookCb.onQcRilHookReady();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            logd("The connection to the service got disconnected unexpectedly!");
            mService = null;
            mBound = false;
            if (mQcrilHookCb != null) {
                logd("Calling onQcRilHookDisconnected callback");
                mQcrilHookCb.onQcRilHookDisconnected();
            }
        }
    };

    private void logd(String str) {
        if (DBG) {
            Log.d(LOG_TAG, str);
        }
    }

    private void logv(String str) {
        if (VDBG) {
            Log.v(LOG_TAG, str);
        }
    }
}
