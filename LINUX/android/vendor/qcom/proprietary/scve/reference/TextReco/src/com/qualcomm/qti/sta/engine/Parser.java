/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.engine;

import android.util.Log;

import com.qualcomm.qti.sta.engine.JniManager.STAListener;

/**
 * class for sta parser
 */
public class Parser {
    private static final String LOG_TAG = "Parser";

    enum State {
        NOT_INITIALIZED, STARTING, STARTED, RECONFIGURING_HANDLER, RESTARTING_HANDLER, STOPPING
    }

    private int mode = JniConstants.PARSER_MODE_SNAPSHOT;
    private int scveMode = -1;
    private String locale = "";
    private int patterns = -1;
    private State state = State.NOT_INITIALIZED;

    /**
     * initializes the parser
     *
     * @param jniHandler
     *            JNI library handler to post the messages
     * @param staListener
     *            STA Listener
     * @param textRecordMode
     *            text record mode (snapshot or preview)
     * @param locale
     *            locale for the parser
     * @param dataPatterns
     *            data patterns supported
     */
    public void initialize(LibraryHandler jniHandler, STAListener staListener,
            int scveMode, int textRecordMode, String locale, int dataPatterns) {

        switch (state) {
        case NOT_INITIALIZED:
        case STOPPING:
            update(scveMode, textRecordMode, locale, dataPatterns);
            state = State.STARTING;
            Log.d(LOG_TAG, "Parser: Starting");
            jniHandler.init(scveMode, mode, locale, patterns);
            break;

        case STARTED:
        case RECONFIGURING_HANDLER:
        case STARTING:
            if(this.scveMode != scveMode) {
                update(scveMode, textRecordMode, locale, dataPatterns);
                Log.d(LOG_TAG, "Restarting Engine");
                state = State.STARTING;
                jniHandler.restartEngine(scveMode, textRecordMode, locale, dataPatterns);

            } else if (mode != textRecordMode) {
                // restart
                update(scveMode, textRecordMode, locale, dataPatterns);
                Log.d(LOG_TAG, "Restarting handler");
                state = State.RESTARTING_HANDLER;
                jniHandler.restartHandler(textRecordMode, locale, dataPatterns);

            } else if (needsReconfiguration(locale, dataPatterns)) {
                // reconfigure
                update(scveMode, textRecordMode, locale, dataPatterns);
                Log.d(LOG_TAG, "Parser: Reconfiguring");
                state = State.RECONFIGURING_HANDLER;
                jniHandler.reconfigureHandler(locale, dataPatterns);

            } else {
                Log.d(LOG_TAG, "Parser: Already running");
                if (staListener != null) {
                    staListener.onSTAReady();
                }
            }
            break;
        case RESTARTING_HANDLER:
            Log.d(LOG_TAG, "Parser: updated parser data only");
            update(scveMode, textRecordMode, locale, dataPatterns);
            break;

        default:
            break;
        }
    }

    private void update(int scveMode, int mode, String locale, int patterns) {
        this.mode = mode;
        this.locale = locale;
        this.patterns = patterns;
        this.scveMode = scveMode;
    }

    private boolean needsReconfiguration(String locale, int patterns) {
        return (!this.locale.equals(locale) || this.patterns != patterns);
    }

    /**
     * gets the state of the parser
     *
     * @param state
     *            parser state
     */
    void setState(State state) {
        this.state = state;
    }

    /**
     * gets the mode of the parser
     *
     * @return mode
     */
    /*package*/int getMode() {
        return mode;
    }

    /**
     * checks the  parser state
     *
     * @param state
     *            state to be checked
     * @return true if same, false otherwise
     */
    boolean isState(State state) {
        return (this.state == state);
    }
}
