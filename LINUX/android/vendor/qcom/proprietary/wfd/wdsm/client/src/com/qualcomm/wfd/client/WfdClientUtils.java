/*
 * Copyright (c) 2012 - 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.preference.PreferenceManager;
import android.util.Log;
import android.util.Xml;
import android.widget.Toast;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.service.IWfdActionListener;
import com.qualcomm.wfd.service.IWfdSession;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Iterator;


public class WfdClientUtils {

    public static final String TAG = "Client.WfdOperationUtil";
    public static ProgressDialog wfdOperationProgressDialog = null;
    public static WfdMode wfdMode = WfdMode.INVALID;
    public static final int PLAY_CALLBACK = 0;
    public static final int PAUSE_CALLBACK = 1;
    public static final int STANDBY_CALLBACK = 2;
    public static final int UIBC_ACTION_COMPLETED = 3;
    public static final int TEARDOWN_CALLBACK = 4;
    public static final int INVALID_WFD_DEVICE = 5;
    public static final int SET_WFD_FINISHED = 6;
    public static final int SERVICE_BOUND = 7;
    public static final int START_SESSION_ON_UI = 8;
    public static final int CLEAR_UI = 10;
    public static final int UIBC_ENABLED = 1;
    public static final int UIBC_DISABLED = 0;
    public static final int AUDIO_ONLY_NOTIFICATION = 11;
    public static final int RTP_TRANSPORT_NEGOTIATED = 12;
    public static final int RTP_TRANSPORT_NEGOTIATED_FAIL = 13;
    public static final int REMOVE_WFD_SESSION = 14;
    public static final int TCP_PLAYBACK_CONTROL_DONE = 15;
    public static final String WFD_SESSION_BINDER_KEY = "wfd_session";
    public static final String SURFACE_TEARDOWN_KEY = "wfd_surface_teardown";

    public static enum WfdOperation {
        PLAY, PAUSE, STANDBY, RESUME, START_UIBC, STOP_UIBC, SELECT_TCP, SELECT_UDP, FLUSH, SET_DECODER_LATENCY, TEARDOWN
    }

    public enum WfdMode {
        PLAYING, PAUSED, STANDBY, INVALID
    }

    public static void startWfdOperationTask (final Context inContext, final WfdOperation inWfdOperation) {
        startWfdOperationTask(inContext,inWfdOperation,null);
    }

    public static void startWfdOperationTask (final Context inContext, final WfdOperation inWfdOperation, final IWfdSession wfdSession) {
        if (inWfdOperation == WfdOperation.PLAY) {
            if (wfdMode == WfdMode.PAUSED) {
                Log.d(TAG, "startWfdOperationTask: Play button clicked in wfdMode PAUSED");
                WfdOperationTask task = new WfdOperationTask(inContext, "Playing WFD Session", WfdOperation.PLAY);
                task.execute();
            } else if (wfdMode == WfdMode.STANDBY) {
                Log.d(TAG, "startWfdOperationTask: Play button clicked in wfdMode STANDBY");
                WfdOperationTask task = new WfdOperationTask(inContext, "Playing WFD Session", WfdOperation.RESUME);
                task.execute();
            } else {
                Log.d(TAG, "startWfdOperationTask: Play button clicked in an incorrect wfdMode- " + wfdMode.ordinal());
            }
        } else if (inWfdOperation == WfdOperation.PAUSE) {
            if (wfdMode == WfdMode.PLAYING) {
                Log.d(TAG, "startWfdOperationTask: Pause button clicked in wfdMode PLAYING");
                WfdOperationTask task = new WfdOperationTask(inContext, "Pausing WFD Session", WfdOperation.PAUSE);
                task.execute();
            } else if (wfdMode == WfdMode.STANDBY) {
                Log.d(TAG, "startWfdOperationTask: Pause button clicked in wfdMode STANDBY");
                AlertDialog.Builder builder = new AlertDialog.Builder(inContext);
                builder.setTitle("Cannot perform pause when in the standby state")
                       .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Log.d(TAG, "startWfdOperationTask: Can't pause in standby- User clicked OK");
                                return;
                    }
                });
                AlertDialog alert = builder.create();
                alert.show();
            } else {
                Log.d(TAG, "startWfdOperationTask: Pause button clicked in an incorrect wfdMode- " + wfdMode.ordinal());
            }
        } else if (inWfdOperation == WfdOperation.STANDBY) {
            if (wfdMode == WfdMode.PLAYING || wfdMode == WfdMode.PAUSED ) {
               if (wfdMode == WfdMode.PLAYING) {
                  Log.d(TAG, "startWfdOperationTask: Standby button clicked in wfdMode PLAYING");
		} else {
		  Log.d(TAG, "startWfdOperationTask: Standby button clicked in wfdMode PAUSED");
	        }
                WfdOperationTask task = new WfdOperationTask(inContext, "Moving WFD Session to Standby", WfdOperation.STANDBY);
                task.execute();
            } else {
                Log.d(TAG, "startWfdOperationTask: Standby button clicked in an incorrect wfdMode- " + wfdMode.ordinal());
            }
        } else if (inWfdOperation == WfdOperation.RESUME) {
            /* mode = paused check is added for DS scenario where after standby, it moves to mirroring
               and that moves the state to pause.*/
            if (wfdMode == WfdMode.STANDBY || wfdMode == WfdMode.PAUSED) {
                Log.d(TAG, "Resume button clicked in wfdMode STANDBY");
                WfdOperationTask task = new WfdOperationTask(inContext, "Playing WFD Session", WfdOperation.RESUME);
                task.execute();
            } else {
                Log.d(TAG, "startWfdOperationTask: Resume button clicked in an incorrect wfdMode- " + wfdMode.ordinal());
            }
        } else if (inWfdOperation == WfdOperation.TEARDOWN) {
                Log.d(TAG, "Teardown button clicked");
                WfdOperationTask task = new WfdOperationTask(inContext, "Tearing down session", WfdOperation.TEARDOWN);
                task.execute();
        } else if (inWfdOperation == WfdOperation.START_UIBC || inWfdOperation == WfdOperation.STOP_UIBC) {
            boolean proceedUIBC = true;
            if (wfdMode != WfdMode.PLAYING) {
                AlertDialog.Builder builder = new AlertDialog.Builder(inContext, AlertDialog.THEME_HOLO_DARK);
                builder.setTitle("UIBC requested in invalid WFD state")
                       .setMessage("Do you really want to proceed with UIBC?")
                       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                           public void onClick(DialogInterface dialog, int id) {
                               Log.d(TAG, "startWfdOperationTask: UIBC dialog user clicked Yes");
                               WfdOperationTask task = new WfdOperationTask(inContext, (inWfdOperation == WfdOperation.START_UIBC)
                                        ?"Starting UIBC":"Stopping UIBC", inWfdOperation);
                               task.execute();
                           }
                       })
                       .setNegativeButton("No", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(TAG, "startWfdOperationTask: UIBC dialog user clicked No" );
                            return;
                        }
                    });
                AlertDialog alert = builder.create();
                alert.show();
            } else {
                WfdOperationTask task = new WfdOperationTask(inContext, (inWfdOperation == WfdOperation.START_UIBC)
                        ?"Starting UIBC":"Stopping UIBC", inWfdOperation);
                task.execute();
            }
        } else if (inWfdOperation == WfdOperation.SELECT_TCP || inWfdOperation == WfdOperation.SELECT_UDP) {
            if (wfdMode != WfdMode.PLAYING) {
                AlertDialog.Builder builder = new AlertDialog.Builder(inContext, AlertDialog.THEME_HOLO_DARK);
                builder.setTitle("Invalid WFD state for transport change")
                       .setMessage("Transport Change is allowed only while Playing. OK?")
                       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                           public void onClick(DialogInterface dialog, int id) {
                               Log.d(TAG, "startWfdOperationTask: Transport change dialog user clicked Yes");
                           }
                       });

                AlertDialog alert = builder.create();
                alert.show();
            } else {
                WfdOperationTask task = new WfdOperationTask(inContext, (inWfdOperation == WfdOperation.SELECT_TCP)
                        ?"Starting TCP":"Starting UDP", inWfdOperation);
                task.execute();
            }
        } else if (inWfdOperation == WfdOperation.SET_DECODER_LATENCY) {
            if (wfdMode == WfdMode.PLAYING){
                WfdOperationTask task = new WfdOperationTask(inContext, "Starting SET_DECODER_LATENCY", inWfdOperation);
                task.execute();
            }
        }else if (inWfdOperation == WfdOperation.FLUSH) {
            if (wfdMode != WfdMode.PLAYING) {
                AlertDialog.Builder builder = new AlertDialog.Builder(inContext, AlertDialog.THEME_HOLO_DARK);
                builder.setTitle("Invalid WFD state for Flush")
                       .setMessage("Flush is allowed only while Playing. OK?")
                       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                           public void onClick(DialogInterface dialog, int id) {
                               Log.d(TAG, "startWfdOperationTask: Flush dialog user clicked Yes");
                           }
                       });
                AlertDialog alert = builder.create();
                alert.show();
            } else {
                WfdOperationTask task = new WfdOperationTask(inContext, "Flushing WFD session ", WfdOperation.FLUSH);
                task.execute();
            }
        }
    }

    public static class WfdOperationTask extends AsyncTask<Void, Void, Void> {

        private final IWfdSession mWFDSession;
        private Context context;
        private String dialogText;
        private WfdOperation controlOperation;
        private SharedPreferences mSharedPrefs;
        int ret = -1;

        public WfdOperationTask(Context inContext, String inDialogText, WfdOperation inControlOperation) {
            this(inContext,inDialogText,inControlOperation,null);
        }
        public WfdOperationTask(Context inContext, String inDialogText, WfdOperation inControlOperation,
                                IWfdSession wfdSession) {
            this.context = inContext;
            this.dialogText = inDialogText;
            this.controlOperation = inControlOperation;
            this.mSharedPrefs = PreferenceManager.getDefaultSharedPreferences(inContext);
            if(wfdSession == null) {
                try {
                    wfdSession = IWfdSession.Stub.asInterface(ServiceUtil.getInstance().getManagedSession());
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            this.mWFDSession = wfdSession;
        }

        @Override
        protected Void doInBackground(Void... params) {
            Log.d(TAG, "Control Operation- do in background started");
            switch (this.controlOperation) {
            case PLAY:
                try {
                    Log.d(TAG, "Control Operation- play()");
                        ret = mWFDSession.play();
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling play(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling play()", e);
                }
                break;
            case PAUSE:
                try {
                    Log.d(TAG, "Control Operation- pause()");
                        ret = mWFDSession.pause();
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling pause(): " + ret);
                    } else {
                        Log.d(TAG, "Called pause() successfully");
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "Remote exception when calling pause()", e);
                }
                Log.d(TAG, "Called pause() successfully- outside try block");
                break;
            case STANDBY:
                try {
                    Log.d(TAG, "Control Operation- standby()");
                        ret = mWFDSession.standby();
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling standby():" + ret);
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "Remote exception when calling standby()", e);
                }
                break;
            case RESUME:
                try {
                    Log.d(TAG, "Control Operation- resume()");
                        ret = mWFDSession.play();
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling resume()/play(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling resume()/play()", e);
                }
                break;
            case START_UIBC:
                try {
                    Log.d(TAG, "Control Operation- startUibcSession()");
                        ret = mWFDSession.startUibcSession();
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling startUibcSession(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling startUibcSession()", e);
                }
                break;
            case STOP_UIBC:
                try {
                    Log.d(TAG, "Control Operation- stopUibcSession()");
                        ret = mWFDSession.stopUibcSession();
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling stopUibcSession(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling stopUibcSession()", e);
                }
                break;
            case SELECT_TCP:
                try {
                    Log.d(TAG, "Control Operation- setRtpTransport()");
                        ret = mWFDSession.setRtpTransport(WfdEnums.RtpTransportType.TCP.ordinal(),
                                    Integer.parseInt(mSharedPrefs.getString("tcp_buffering_delay_value", "5")), 0);
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling setRtpTransport(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling setRtpTransport()", e);
                }
                break;
            case SELECT_UDP:
                try {
                    Log.d(TAG, "Control Operation- setRtpTransport()");
                        ret = mWFDSession.setRtpTransport(WfdEnums.RtpTransportType.UDP.ordinal(),
                                    Integer.parseInt(mSharedPrefs.getString("udp_decoder_latency_value", "5")), 0);
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling setRtpTransport(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling setRtpTransport()", e);
                }
                break;
            case SET_DECODER_LATENCY:
                try {
                    Log.d(TAG, "Control Operation- setDecoderLatency()");
                    ret = mWFDSession.setDecoderLatency(Integer.parseInt(mSharedPrefs.getString("udp_decoder_latency_value", "5")));
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling setDecoderLatency(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling setDecoderLatency()", e);
                }
                break;
            case FLUSH:
                try {
                    Log.d(TAG, "Control Operation- flush()");
                        ret = mWFDSession.tcpPlaybackControl(WfdEnums.ControlCmdType.FLUSH.ordinal(), 0);
                    if (ret < 0) {
                        Log.e(TAG, "Error when calling flush(): " + ret);
                    }
                } catch (RemoteException e) {
                        Log.e(TAG, "Remote exception when calling flush()", e);
                }
                break;
            case TEARDOWN:
                try {
                    Log.e(TAG, "Control Operation- teardown()");
                    ret = mWFDSession.teardown();
                } catch (RemoteException e) {
                    Log.e(TAG, "Remote exception", e);
                }
                break;
            }
            return null;
        }

        @Override
        protected void onPreExecute() {
            Log.d(TAG, "Control Operation- onPreExecute");
            wfdOperationProgressDialog = ProgressDialog.show(context,
                    dialogText, "Press back to cancel", true, true,
                    new  DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface dialog) {
                            Log.d(TAG, "onCancel called- before cancel()");
                            //cancel(true);
                            Log.d(TAG, "onCancel called- after cancel()");
                        }
            });
            Log.d(TAG, "class name: " +
                    context.getClass().getName().substring(context.getClass().getName().lastIndexOf(".")+1));
        };

        @Override
        protected void onPostExecute(Void aVoid) {
            if(ret < 0) {
                if(wfdOperationProgressDialog != null) {
                    wfdOperationProgressDialog.dismiss();
                    Toast.makeText(context,dialogText + " failed ! ",Toast.LENGTH_LONG).show();
                }
            }
        }
    }

    public static IWfdActionListener createActionListener(Handler inHandler) {
        Log.d(TAG,"Creating action listener for " + inHandler.getClass());
        return new WfdActionListenerImpl(inHandler);
    }

    private static class WfdActionListenerImpl extends IWfdActionListener.Stub {
        Handler mHandler;

        WfdActionListenerImpl(Handler handler) {
            super();
            mHandler = handler;
        }

        @Override
        public void onStateUpdate(int newState, int sessionId) throws RemoteException {
            WfdEnums.SessionState state = WfdEnums.SessionState.values()[newState];
            switch (state) {
                case INITIALIZED:
                    Log.d(TAG, "WfdEnums.SessionState==INITIALIZED");
                    if (sessionId > 0) {
                        Log.d(TAG, "WfdEnums.SessionState==INITIALIZED, sessionId > 0");
                        Message messageTeardown = mHandler.obtainMessage(TEARDOWN_CALLBACK);
                        mHandler.sendMessage(messageTeardown);
                    }
                    break;
                case INVALID:
                    Log.d(TAG, "WfdEnums.SessionState==INVALID");
                    wfdMode = WfdMode.INVALID;
                    break;
                case IDLE:
                    Log.d(TAG, "WfdEnums.SessionState==IDLE");
                    break;
                case PLAY:
                    Log.d(TAG, "WfdEnums.SessionState==PLAY");
                    wfdMode = WfdMode.PLAYING;
                    Message messagePlay = mHandler.obtainMessage(PLAY_CALLBACK);
                    mHandler.sendMessage(messagePlay);
                    break;
                case PAUSE:
                    Log.d(TAG, "WfdEnums.SessionState==PAUSE");
                    wfdMode = WfdMode.PAUSED;
                    Message messagePause = mHandler.obtainMessage(PAUSE_CALLBACK);
                    mHandler.sendMessage(messagePause);
                    break;
                case STANDBY:
                    Log.d(TAG, "WfdEnums.SessionState = STANDBY");
                    wfdMode = WfdMode.STANDBY;
                    Message messageStandby = mHandler.obtainMessage(STANDBY_CALLBACK);
                    mHandler.sendMessage(messageStandby);
                    break;
                case ESTABLISHED:
                    Log.d(TAG, "WfdEnums.SessionState==ESTABLISHED");
                    Message messageEstablishedCallback = mHandler.obtainMessage(START_SESSION_ON_UI);
                    mHandler.sendMessage(messageEstablishedCallback);
                    break;
                case TEARDOWN:
                    Log.d(TAG, "WfdEnums.SessionState==TEARDOWN");
                    Message messageTeardown = mHandler.obtainMessage(TEARDOWN_CALLBACK);
                    mHandler.sendMessage(messageTeardown);
                    break;
            }
        }

        @Override
        public void notifyEvent(int event, int sessionId) throws RemoteException {
            if (event == WfdEnums.WfdEvent.UIBC_ENABLED.ordinal()) {
                Log.d(TAG, "notifyEvent- UIBC enabled callback");
                Message messageUibcAction = mHandler.obtainMessage(UIBC_ACTION_COMPLETED);
                messageUibcAction.arg1 = UIBC_ENABLED;
                mHandler.sendMessage(messageUibcAction);
            } else if (event == WfdEnums.WfdEvent.UIBC_DISABLED.ordinal()) {
                Log.d(TAG, "notifyEvent- UIBC disabled callback");
                Message messageUibcAction = mHandler.obtainMessage(UIBC_ACTION_COMPLETED);
                messageUibcAction.arg1 = UIBC_DISABLED;
                mHandler.sendMessage(messageUibcAction);
            }else if (event == WfdEnums.WfdEvent.START_SESSION_FAIL.ordinal()) {
                Log.d(TAG, "notifyEvent- START_SESSION_FAIL");
                Message messageFailSession = mHandler.obtainMessage(CLEAR_UI);
                mHandler.sendMessage(messageFailSession);
            }else if (event == WfdEnums.WfdEvent.AUDIO_ONLY_SESSION.ordinal()) {
                Log.d(TAG, "notifyEvent- AUDIO_ONLY_SESSION");
                Message audioOnlySession = mHandler.obtainMessage(AUDIO_ONLY_NOTIFICATION);
                mHandler.sendMessage(audioOnlySession);
            }else if (event == WfdEnums.WfdEvent.RTP_TRANSPORT_NEGOTIATED_FAIL.ordinal()) {
                Log.d(TAG, "notifyEvent - Negotiation Failed");
                Message rtpNegotiation = mHandler.obtainMessage(RTP_TRANSPORT_NEGOTIATED_FAIL);
                mHandler.sendMessage(rtpNegotiation);
            }else if (event == WfdEnums.WfdEvent.RTP_TRANSPORT_NEGOTIATED.ordinal()) {
                Log.d(TAG, "notifyEvent - Negotiation Success");
                Message rtpNegotiation = mHandler.obtainMessage(RTP_TRANSPORT_NEGOTIATED);
                mHandler.sendMessage(rtpNegotiation);
            }else if (event == WfdEnums.WfdEvent.TCP_PLAYBACK_CONTROL.ordinal()) {
                Log.d(TAG, "Playback Control - Success");
                Message rtpNegotiation = mHandler.obtainMessage(TCP_PLAYBACK_CONTROL_DONE);
                mHandler.sendMessage(rtpNegotiation);
            }else {
                Log.d(TAG, "notifyEvent- unrecognized event: " + event);
            }
        }

        @Override
        public void notify(Bundle b, int sessionId) throws RemoteException {
            if(b != null) {
                Log.d(TAG, "Notify from WFDService");
                String event = b.getString("event");
                if("setDecoderLatency".equalsIgnoreCase(event)) {
                    if(WfdClientUtils.wfdOperationProgressDialog != null) {
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                    }
                    Log.d(TAG, "clientProgressDialog dismissed");
                }
                else if ("RTCPRRMessage".equalsIgnoreCase(event)) {
                    int rtcpMsgLen = b.getInt("length");

                    if (rtcpMsgLen > 0) {
                        Log.d(TAG, "Recevied RTCPRRMessage with length " + rtcpMsgLen);

                        String mesg = b.getString("mesg");

                        if (null != mesg) {
                            int msgLen = mesg.length();
                            Log.e(TAG, "rtcp rr notify len=" + msgLen);
                            Log.e(TAG, "rtcp rr notify xml '" + mesg + "'");

                            HashMap<String, String> hashMap = new HashMap<String, String>();
                            fetchRtcpRRInfoFromXml(mesg, hashMap);
                        }
                        else {
                            Log.e(TAG, "RTCPRRMessage - failed to get rtcp rr string");
                        }
                    }
                    else {
                        Log.e(TAG, "RTCPRRMessage - invalid msg length zero!");
                    }
                }
            }
        }

        private void fetchRtcpRRInfoFromXml(
            String inputXml, HashMap<String, String> hashMap) {

            // References: http://developer.android.com/training/basics/network-ops/xml.html
            class RtcpRRXmlParser {

                public void parse(
                    InputStream in, HashMap<String, String> hashMap)
                        throws XmlPullParserException, IOException {

                    try {
                        XmlPullParser parser = Xml.newPullParser();
                        parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false);
                        parser.setInput(in, null);
                        parser.nextTag();
                        readFeed(parser, hashMap);
                    } finally {
                        in.close();
                    }
                }

                private void readFeed(
                    XmlPullParser parser, HashMap<String, String> hashMap)
                        throws XmlPullParserException, IOException {

                    int eventType = parser.getEventType();

                    String tagname = "";
                    while (eventType != XmlPullParser.END_DOCUMENT) {

                        switch (eventType) {
                        case XmlPullParser.START_TAG:
                            tagname = parser.getName();
                            //Log.e(TAG, "Xml start tag '" + tagname + "'");
                            break;

                        case XmlPullParser.TEXT:
                            String text = parser.getText();
                            //Log.e(TAG, "xml text '" + text + "' for tag '" + tagname + "'");

                            if (tagname.equals("fl") ||     // fraction lost
                                tagname.equals("cnpl") ||   // cumumulative number of packets lost
                                tagname.equals("ehsn") ||   // extended highest sequence number received
                                tagname.equals("iaji")) {   // interarrival jitter
                                hashMap.put(tagname, text);
                            }
                            break;

                        case XmlPullParser.END_TAG:
                            //Log.e(TAG, "Xml end tag '" + tagname + "'");
                            tagname = "";
                            break;

                        default:
                            Log.e(TAG, "xml parser default");
                            break;
                        }

                        eventType = parser.next();
                    }
                }
            }

            try {
                RtcpRRXmlParser xmlParser = new RtcpRRXmlParser();
                InputStream stream = new ByteArrayInputStream(
                    inputXml.getBytes(/* Charset */));
                xmlParser.parse(stream, hashMap);

                Iterator it = hashMap.entrySet().iterator();

                Log.e(TAG, "rtcp RR hashmap size = " + hashMap.size());

                // print out the RTCP RR information as an example
                while (it.hasNext()) {
                    HashMap.Entry entry = (HashMap.Entry)it.next();
                    Log.e(TAG, "rtcp RR '" + entry.getKey() + "' -> '" + entry.getValue());
                }

            } catch (XmlPullParserException e) {
                Log.e(TAG, "XmlPullParserException");
            } catch (Exception e) {
                Log.e(TAG, "General Exception");
            }
        }
    }

    public static String msgCodeToString(Message msg) {
        if(msg == null) {
            return null;
        }
        StringBuilder sb = new StringBuilder();
        switch(msg.what) {
            case PLAY_CALLBACK:
                sb.append("PLAY");
                break;
            case PAUSE_CALLBACK:
                sb.append("PAUSE");
                break;
            case UIBC_ACTION_COMPLETED:
                sb.append("UIBC: ");
                if (WfdClientUtils.UIBC_DISABLED == msg.arg1) {
                    sb.append("Disabled");
                } else if (WfdClientUtils.UIBC_ENABLED == msg.arg1) {
                    sb.append("Enabled");
                }
                break;
            case START_SESSION_ON_UI:
                sb.append("Start Session on UI");
                break;
            case CLEAR_UI:
                sb.append("CLEAR UI");
                break;
            case TEARDOWN_CALLBACK:
                sb.append("TEAR DOWN");
                break;
            case AUDIO_ONLY_NOTIFICATION:
                sb.append("AUDIO_ONLY_NOTIFICATION");
                break;
            default:
                sb.append("Unknown callback");
        }
        return sb.toString();
    }
}
