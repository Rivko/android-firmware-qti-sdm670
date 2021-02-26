/*==============================================================================
*        @file UIBCManager.java
*
*
*  Copyright (c) 2012 - 2013, 2016 - 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

package com.qualcomm.wfd;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import com.qualcomm.compat.VersionedInputManager;
import com.qualcomm.wfd.WfdEnums.HIDDataType;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.RemoteException;
import android.util.Log;
import android.view.InputDevice;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;

import android.os.SystemProperties;

public class UIBCManager {
    private static final String TAG = "UIBCManager";
    private static final String sDebugEnable = "debug.wfd.uibcevents";
    private Context mContext;
    private int deviceType;
    private VersionedInputManager inputManager;
    private EventReader eventReader;
    public EventDispatcher eventDispatcher;
    private Thread eventReaderThread, eventDispatcherThread;
    private boolean mDebug;
    private EventListener mEventListener;
    private HIDCEventListener mHIDCListener;
    /* constructor */
    public UIBCManager(Context c, int devType, EventListener listener,
            HIDCEventListener hidListener) throws InstantiationException,
            IllegalAccessException, ClassNotFoundException, IllegalArgumentException {
        mContext = c;
        deviceType = devType;
        if (deviceType == WFDDeviceType.SOURCE.getCode()) {
            inputManager = VersionedInputManager.newInstance();
        }
        mDebug = false;
        try {
            mDebug = SystemProperties.getBoolean(sDebugEnable, false);
            if (mDebug) {
                Log.i(TAG, "Enabling UIBC input event debugging");
            }
        } catch (Exception e) {
            Log.e(TAG, "Exception while getting system Property");
            e.printStackTrace();
        }
        if(listener == null || hidListener == null) {
            throw new IllegalArgumentException("listeners cannot be null");
        }
        mEventListener = listener;
        mHIDCListener = hidListener;
        HIDEventCallback(null, 0);//To prevent Progaurd stripping
    }

    /**
     * Function to start UIBC data path. This function is called upon a
     * successful RTSP message exchange to enable UIBC.
     *
     * @return
     */
    public boolean start() {
        if (deviceType == WFDDeviceType.SOURCE.getCode()) {
            Log.d(TAG, "Create eventDispatcher");
            eventDispatcher = new EventDispatcher();
            eventDispatcher.eventQueueStatus = true;
            // start EventDispatcher thread
            eventDispatcherThread = new Thread(eventDispatcher);
            eventDispatcher.running = true;
            eventDispatcherThread.start();
        } else {
            Log.d(TAG, "Create eventReader");
            eventReader = new EventReader(mEventListener);
            eventReader.eventQueueStatus = true;
            Log.d(TAG, "calling WFDNative.startUIBC()");
            // start EventReader thread
            eventReaderThread = new Thread(eventReader);
            eventReader.running = true;
            eventReaderThread.start();
        }
        return true;
    }

    public void addUIBCEvent(InputEvent ev) {
        eventReader.addEvent(ev);
    }

    /**
     * Function to stop UIBC data path This function is called upon a successful
     * RTSP message exchange to disable UIBC.
     *
     * @return
     */
    public boolean stop() {
        if (deviceType == WFDDeviceType.SOURCE.getCode()) {
            eventDispatcher.running = false; // (2) stop EventDispatcher thread
            eventDispatcher.eventQueueStatus = false; // (3) stop eventQueue
                                                      // loop
            try {
                eventDispatcherThread.join(); // (3) wait for EventDispatcher
                                              // thread to finish
            } catch (InterruptedException e) {
                Log.e(TAG, "Error joining event dispatcher thread", e);
            }
        } else {
            eventReader.running = false; // (1) stop EventReader thread
            eventReader.eventQueueStatus = false; // (2) stop eventQueue loop
            try {
                eventReaderThread.join(); // (2) wait for EventReader thread to
                                          // finish
            } catch (InterruptedException e) {
                Log.e(TAG, "Error joining reader thread", e);
            }
           // WFDNative.stopUIBC(); // (4) stop UIBC in UIBCAdaptor
        }
        return true;
    }

    /**
     * EventReader maintains a queue, which stores the UI events generated
     * locally from UI surface.When event is available in queue, an internal
     * thread consumes the event and sends the event to peer device through
     * UIBCAdaptor.
     *
     * @author muhuanc
     */
    class EventReader extends EventQueue implements Runnable {
        public boolean running = true;
        public EventListener mListener;
        public EventReader(EventListener obj) {
            mListener = obj;
        }
        @Override
        public void run() {
            while (running) {
                InputEvent ev = getNextEvent();
                if (ev != null) {
                    if (ev instanceof MotionEvent) {
                        /* send MotionEvent to UIBCAdaptor */
                        if (mDebug) {
                            Log.i(TAG, "Sending motion event " + ev);
                        }
                        mListener.sendMotionEvent((MotionEvent) ev,null);
                    } else if (ev instanceof KeyEvent) {
                        if (mDebug) {
                            Log.i(TAG, "Sending key event " + ev);
                        }
                        mListener.sendKeyEvent((KeyEvent) ev,null);
                    } else {
                        Log.e(TAG, "***** Unknown event received");
                    }
                }
            }

        }
    }

    /**
     * EventDispatcher maintains a queue, which stores events received from peer device.  The events are
     * pushed in by UIBCAdaptor.  When event is available in queue, an internal thread consumes the event
     * and injects the UI event through IWindowManager.
     * @author muhuanc
     *
     */
    class EventDispatcher extends EventQueue implements Runnable {
        public boolean running = true;

        @Override
        public void run() {
            while (running) {
                InputEvent ev = getNextEvent();
                if (ev != null) {
                    boolean injectSuccess = false;
                    if (ev instanceof MotionEvent) {
                        /* inject MotionEvent */
                        MotionEvent me = (MotionEvent) ev;
                        if (mDebug) {
                            Log.e(TAG, "Injecting motion event" + me);
                        }
                        try {
                            if (me.getSource() == InputDevice.SOURCE_TOUCHSCREEN) {
                                injectSuccess = inputManager
                                        .injectPointerEvent(me, false);
                            } else if (me.getSource() == InputDevice.SOURCE_TRACKBALL) {
                                injectSuccess = inputManager
                                        .injectTrackballEvent(me, false);
                            } else {
                                Log.e(TAG, "Unknown motion event");
                            }
                        } catch (RemoteException e) {
                            Log.e(TAG,
                                    "Exception happened when injecting event",
                                    e);
                        } catch (SecurityException e) {
                            if (mContext.checkSelfPermission("android.permission.INJECT_EVENTS")
                                    == PackageManager.PERMISSION_GRANTED) {
                                Log.e(TAG, "Security exception happened when injecting event", e);
                            } else {
                                Log.e(TAG, "Fatal: wfd service does not have INJECT_INPUT permission", e);
                                throw e;
                            }
                        }
                    } else if (ev instanceof KeyEvent) {
                        /* inject key event */
                        KeyEvent ke = (KeyEvent) ev;
                        if (mDebug) {
                            Log.e(TAG, "Injecting key event" + ke);
                        }
                        try {
                            injectSuccess = inputManager.injectKeyEvent(ke,
                                    false);
                        } catch (RemoteException e) {
                            Log.e(TAG,
                                    "Exception happened when injecting key event",
                                    e);
                        }
                    }
                    if (!injectSuccess) {
                        Log.e(TAG,
                                "Inject failed for event type with contents: "
                                        + ev);
                    }
                }
            }
        }
    }

    /**
     * Non-blocking queue for event storage
     * @author muhuanc
     *
     */
    private class EventQueue {
        /* event queue */
        private BlockingQueue<InputEvent> queuedEvents = new LinkedBlockingQueue<InputEvent>();
        private final int timeOut =500;
        protected boolean eventQueueStatus = true;
        public InputEvent getNextEvent() {
            while (eventQueueStatus) {
                InputEvent queuedEvent;
                try {
                    queuedEvent = queuedEvents.poll(timeOut, TimeUnit.MILLISECONDS);
                    if (queuedEvent != null) {
                        // dispatch the event
                        return queuedEvent;
                    } else {
                        return null;
                    }
                } catch (InterruptedException e) {
                    Log.e(TAG, "Interrupted when waiting to read from queue", e);
                    return null;
                }
            }
            return null;
        }

        /*For EventDispatcher, this gets called from native code*/
        public void addEvent(InputEvent ev) {
            try {
                if(ev != null) {
                  queuedEvents.offer(ev, timeOut, TimeUnit.MILLISECONDS );
                }
            } catch (InterruptedException e) {
                Log.e(TAG, "Interrupted when waiting to insert to queue", e);
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Illegal Argument Exception ",e);
            }
        }
    }

    public void HIDEventCallback(byte[] packet, int hidDataType) {
        if (packet == null) {
            Log.e(TAG, "HID payload is null, ignore callback");
        } else {
            if (hidDataType == HIDDataType.HID_REPORT.ordinal()) {
                mHIDCListener.HIDReportRecvd(packet);
            } else if (hidDataType == HIDDataType.HID_REPORT_DESCRIPTOR.ordinal()) {
                mHIDCListener.HIDReportDescRecvd(packet);
            }
        }
    }

    public interface HIDCEventListener {

        void HIDReportDescRecvd(byte[] packet);

        void HIDReportRecvd(byte[] packet);
    }

    public interface EventListener {

        void sendKeyEvent(KeyEvent k, Bundle b);

        void sendMotionEvent(MotionEvent m, Bundle b);
    }
}
