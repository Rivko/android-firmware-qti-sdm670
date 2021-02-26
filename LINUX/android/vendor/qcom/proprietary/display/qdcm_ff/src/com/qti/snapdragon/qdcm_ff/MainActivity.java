/* ====================================================================
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file MainActivity.java
 *
 */

package com.qti.snapdragon.qdcm_ff;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.FormatException;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.nfc.NfcManager;
import android.nfc.Tag;
import android.nfc.tech.Ndef;
import android.os.Handler;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import android.os.AsyncTask;
import android.os.SystemClock;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

public class MainActivity extends Activity {


    private static final String MIME_INTENT_TYPE = "application/com.qti.snapdragon.qdcm_ff";
    private static final String LOG_TAG = "QDCM_FF_Mobile";
    private static final String AP_CONNECTED_INTENT = "com.qti.snapdragon.qdcm_ff.AP_CONNECTED";

    // Local members
    private int mInterval = 500;
    private String mNfcRecordData;
    private static String mSSID;
    private static String mNetworkPass;
    private static String mIpAddress;
    private static String mPort;

    private Handler mUiHandler;
    private Handler mNfcOutOfRangeHandler;

    private WirelessManager mWirelessMgr;
    private NfcAdapter mNfcAdapter;
    private Ndef mNdef;
    private DiagManager mDiagMgr;


    // Create runnable for detecting when NFC Tag is in/out of range
    private final Runnable nfcOutOfRangeRunnable = new Runnable() {
        public void run() {
            try {
                mNdef.getNdefMessage();
            }
            catch (IOException | FormatException e) {
                // Exception means NFC is no longer in range.
                stopWirelessManager();
            }
            mNfcOutOfRangeHandler.postDelayed(this, mInterval);
        }
    };

    // BroadcastReciever to get notification of when Wifi has
    // connected/disconnected to AP in order to enable/disable
    // diag_socket_log application.
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            Log.d(LOG_TAG, "Receiver Intent: " + action);
            if (action.equals(AP_CONNECTED_INTENT)) {
               // enable diag over sockets
               boolean ret = startDiagService();
               if (!ret) {
                   Log.d(LOG_TAG, "Unable to start diag_socket_log app");
               }
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mDiagMgr = new DiagManager();

        // Register intentfilter for when Wifi has been connected to AP
        registerReceiver(mReceiver, new IntentFilter(AP_CONNECTED_INTENT));

        try {
            // Check if it's a NFC enabled phone
            mNfcOutOfRangeHandler = new Handler();
            this.mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
            if (mNfcAdapter == null) {
                // No NFC chip detected
                Log.d(LOG_TAG, "NFC not supported");
                finish();
            }

            if (!mNfcAdapter.isEnabled()) {
                Log.d(LOG_TAG, "NFC not enabled");
                finish();
            }
        } catch (Exception e) {
            Log.d(LOG_TAG, "Unable to check for NFC support: " + e.getMessage());
        } finally {
            handleIntent(getIntent());
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        mNfcOutOfRangeHandler.post(nfcOutOfRangeRunnable);

        if (mNfcAdapter != null) {
           setupForegroundDispatch(this, mNfcAdapter);
        }
    }

    @Override
    protected void onPause() {
        if (mNfcAdapter != null) {
            stopForegroundDispatch(this, mNfcAdapter);
        }

        mNfcOutOfRangeHandler.removeCallbacks(nfcOutOfRangeRunnable);
        super.onPause();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        /**
         * This method is called when a NFC Tag is attached to device
         */
        handleIntent(intent);
    }

    private void handleIntent(Intent intent) {
        try {
            String action = intent.getAction();
            if (NfcAdapter.ACTION_NDEF_DISCOVERED.equals(action)) {
                String type = intent.getType();
                Log.d(LOG_TAG, "NFC Intent: " + type);

                if (MIME_INTENT_TYPE.equals(type)) {
                    Tag tag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
                    mNdef = Ndef.get(tag);
                    mNdef.connect();

                    new NdefReaderTask().execute(tag);
                }
                else {
                    Log.d(LOG_TAG, "Wrong MIME type: " + type);
                }
            }
            else if (NfcAdapter.ACTION_TECH_DISCOVERED.equals(action)) {
                //In this case, we would still use the Tech Discovered Intent
                Tag tag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
                String[] techList = tag.getTechList();
                String scannedTech = Ndef.class.getName();

                for (String tech : techList) {
                    if (scannedTech.equals(tech)) {
                        new NdefReaderTask().execute(tag);
                        break;
                    }
                }
            }
        }
        catch (Exception e) {
            Log.d(LOG_TAG, "handleIntent(): " + e.getMessage());
        }
    }

    /**
     * Launch the diag_socket_log execuatble with NFC tag data
     * and enable QPST over WiFi
     */
    private boolean startDiagService() {
        if (mDiagMgr == null || mIpAddress.isEmpty() || mPort.isEmpty())
            return false;

        boolean ret = mDiagMgr.enableDiagOverSockets(mIpAddress, mPort);
        return ret;
    }

    /**
     * Start Wireless Manager with SSID/Pass and connect to AP/Router
     */
    private void startWirelessManager(String ssid, String networkPass) {
        mWirelessMgr = new WirelessManager(this);
        boolean ret = mWirelessMgr.startWifi(ssid, networkPass);
        if (ret) {
            Log.d(LOG_TAG, "Successfully connected to AP");
        }
    }

    /**
     * Stop Wireless Manager by disconnecting from AP & turnning off Wifi
     */
    private void stopWirelessManager() {
        if (mWirelessMgr != null) {
            boolean ret = mWirelessMgr.stopWifi();
            mWirelessMgr = null;
            Log.d(LOG_TAG, "Successfully disconnected from AP");
        }
    }

    /**
     * Background task (non-blocking UI thread) for reading NFC data.
     */
    private class NdefReaderTask extends AsyncTask<Tag, Void, String> {

        @Override
        protected String doInBackground(Tag... params) {
            Tag tag = params[0];

            Ndef ndef = Ndef.get(tag);
            if (ndef == null) {
                // NDEF is not supported by this Tag
                Log.d(LOG_TAG, "NDEF is not supported by this Tag");
                return null;
            }

            NdefMessage ndefMsg = ndef.getCachedNdefMessage();
            NdefRecord[] records = ndefMsg.getRecords();
            for (NdefRecord ndefRecord : records) {
                if (ndefRecord.getTnf() == NdefRecord.TNF_MIME_MEDIA) {
                    try {
                        return readTag(ndefRecord);
                    }
                    catch (UnsupportedEncodingException e) {
                        Log.d(LOG_TAG, "doInBackground(): " + e.getMessage());
                    }
                }
            }

            return null;
        }

        private String readTag(NdefRecord record) throws UnsupportedEncodingException {
            try {
                byte[] payload = record.getPayload();
                return new String(payload);
            }
            catch (Exception e) {
                Log.d(LOG_TAG, "readTag(): " + e.getMessage());
            }

            return null;
        }

        @Override
        protected void onPostExecute(String result) {
            if (result != null) {
                mNfcRecordData = result;

                try {
                    /**
                     * Split string to get SSID and password
                     * NFC Tag must be defined using the following structure:
                     * <SSID>:<Password>:<PC IP Address>:<Port>
                     */
                    String[] tokens = mNfcRecordData.split("\\:");
                    mSSID = tokens[0];
                    mNetworkPass = tokens[1];
                    mIpAddress = tokens[2];
                    mPort = tokens[3];

                    startWirelessManager(mSSID, mNetworkPass);
                }
                catch (Exception e) {
                    Log.d(LOG_TAG, "onPostExecute(): " + e.getMessage());
                }
            }
        }
    }

    public static void setupForegroundDispatch(final Activity activity, NfcAdapter adapter) {
        final Intent intent = new Intent(activity.getApplicationContext(), activity.getClass());
        intent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
        final PendingIntent pendingIntent =
            PendingIntent.getActivity(activity.getApplicationContext(), 0, intent, 0);

        IntentFilter[] filters = new IntentFilter[1];
        String[][] techList = new String[][]{};

        // Same filter as found in manifest
        filters[0] = new IntentFilter();
        filters[0].addAction(NfcAdapter.ACTION_NDEF_DISCOVERED);
        filters[0].addCategory(Intent.CATEGORY_DEFAULT);

        try {
            filters[0].addDataType(MIME_INTENT_TYPE);
        }
        catch (IntentFilter.MalformedMimeTypeException e) {
            throw new RuntimeException("Check MIME type");
        }

        adapter.enableForegroundDispatch(activity, pendingIntent, filters, techList);
    }

    public static void stopForegroundDispatch(final Activity activity, NfcAdapter adapter) {
        adapter.disableForegroundDispatch(activity);
    }
}

