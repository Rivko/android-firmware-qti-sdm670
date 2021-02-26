/******************************************************************************
  @file    SAMService.java
  @brief   SAM service provides SAM funcionalities

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.sam.service;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Message;
import android.os.Handler;
import android.os.RemoteException;
import android.os.AsyncResult;
import android.util.Log;
import android.content.Context;

import java.io.IOException;
import java.util.HashMap;
import java.lang.Byte;
import java.util.List;
import java.util.ArrayList;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


import com.qualcomm.qcrilhook.IQcRilHook;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.OemHookCallback;

import com.qualcomm.qti.sam.interfaces.ISAM;
import com.qualcomm.qti.sam.interfaces.ISAMResponseCallback;
import com.qualcomm.qti.sam.interfaces.ISAMUnsolicitedCallback;
import com.qualcomm.qti.sam.interfaces.CardState;
import com.qualcomm.qti.sam.interfaces.CardATR;
import com.qualcomm.qti.sam.interfaces.SlotMap;
import com.qualcomm.qti.sam.interfaces.SAMErrors;

import com.android.internal.telephony.CommandException;

public class SAMService extends Service {
    private final String LOG_TAG = "SAMService";

    public HashMap<Integer, ISAMResponseCallback> responseCallbackMap;
    public List<ISAMUnsolicitedCallback> unsolCallbackList;
    private Context context;

    private QcRilHook mQcRILHook;
    private Handler mHandler;
    private boolean mIsDestroyed = false;

    private static int mToken = 0;

    private final int SAM_PHONE_ID = 0;
    private final int INT_SIZE = 4;
    private final int SAM_SERVICE_MAX_TOKEN = 65535; //reset after 65535

    private final int MSG_UNSOL_RIL_RESP = 1;
    private final int MSG_CARD_POWER_DOWN = 2;
    private final int MSG_CARD_POWER_UP = 3;
    private final int MSG_SEND_APDU = 4;
    private final int MSG_SWITCH_SLOT = 5;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(LOG_TAG, "onCreate()");
        context = this;

        mHandler = new Handler() {
            public void handleMessage(Message msg) {
                Log.i(LOG_TAG, "Message received: what = " + msg.what);
                synchronized (this) {
                    if (mIsDestroyed) {
                        Log.i(LOG_TAG, "SAM is destroyed; do not handle any messages");
                        return;
                    }
                }

                switch (msg.what) {
                    case MSG_UNSOL_RIL_RESP:
                        AsyncResult ar = (AsyncResult)msg.obj;
                        if (ar.result != null) {
                            try {
                                handleUnsolCardState((Message)ar.result);
                            } catch (RemoteException e) {
                                Log.e(LOG_TAG, "handleMessage, remote exception");
                            }
                        } else {
                            Log.e(LOG_TAG, "Error: empty result, MSG_UNSOL_RIL_RESP");
                        }
                        break;

                    default:
                        Log.e(LOG_TAG, "invalid event");
                        break;

                }
            }
        };

        mQcRILHook = new QcRilHook(getApplicationContext());
        mQcRILHook.register(mHandler, MSG_UNSOL_RIL_RESP, null);
        responseCallbackMap = new HashMap<Integer, ISAMResponseCallback>();
        unsolCallbackList = new ArrayList<ISAMUnsolicitedCallback>();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(LOG_TAG, "onBind()");
        return mBinder;
    }

    @Override
    public void onDestroy() {
        Log.i(LOG_TAG, "onDestroy()");
        stopSelf();
        synchronized (this) {
            mIsDestroyed = true;
        }

        super.onDestroy();
    }

    private void handleUnsolCardState(Message msg) throws RemoteException {
        if (msg == null ||  msg.obj == null) {
            Log.e(LOG_TAG, "Null data received in handleUnsolCardState");
        }

        CardState cardState = new CardState();
        ByteBuffer payload = ByteBuffer.wrap((byte[])msg.obj);
        payload.order(ByteOrder.nativeOrder());

        int rspId = payload.getInt();
        int slotId = msg.arg1;

        if ((slotId == SAM_PHONE_ID) &&
                (rspId == mQcRILHook.QCRILHOOK_UNSOL_CARD_STATE_CHANGED)) {
            Log.i(LOG_TAG, " Unsol: rspId " + rspId + " slotId " + msg.arg1);
            cardState.setSlotId(payload.getInt());
            cardState.setCardState(payload.getInt());

            synchronized (unsolCallbackList) {
                for (ISAMUnsolicitedCallback cb : unsolCallbackList) {
                    cb.cardStateUpdateIndication(cardState);
                }
            }
        }
    }

    private synchronized int getToken() {
        mToken++;
        if(mToken > SAM_SERVICE_MAX_TOKEN) {
            mToken = 0;
        }
        return mToken;
    }

    private class SAMOemHookCallback extends OemHookCallback {
        public Message mMsg;

        public SAMOemHookCallback(Message msg) {
            super(msg);
            mMsg = msg;
        }

        @Override
        public void onOemHookResponse(byte[] response, int phoneId) {
            int token = mMsg.arg1;
            ISAMResponseCallback callback = getCallback(token);
            ByteBuffer byteBuf = ByteBuffer.wrap(response);
            byteBuf.order(ByteOrder.nativeOrder());
            Log.i(LOG_TAG, "what " + mMsg.what + " phoneId " + phoneId + " token " + token);
            switch (mMsg.what)
            {
                case MSG_CARD_POWER_DOWN:
                {
                    int slotId = byteBuf.getInt();
                    int actionStatus = byteBuf.getInt();
                    try {
                        callback.cardPowerDownResponse(slotId, actionStatus);
                    } catch (RemoteException re) {
                        Log.e(LOG_TAG, "cardPowerDownResponse , remote exception");
                    }

                    break;
                }

                case MSG_CARD_POWER_UP:
                {
                    int slotId = byteBuf.getInt();
                    int actionStatus = byteBuf.getInt();
                    Log.i(LOG_TAG, " callback " + callback);
                    try {
                        callback.cardPowerUpResponse(slotId, actionStatus);
                    } catch (RemoteException re) {
                        Log.e(LOG_TAG, "cardPowerDownResponse , remote exception" + re);
                    }

                    break;
                }

                case MSG_SEND_APDU:
                {
                    int slotId = byteBuf.getInt();
                    int actionStatus = byteBuf.getInt();
                    int statusWord1 = byteBuf.getInt();
                    int statusWord2 = byteBuf.getInt();
                    byte[] bytes = new byte[byteBuf.remaining()];
                    byteBuf.get(bytes);
                    String dataResponse = new String(bytes);

                    try {
                        callback.streamApduResponse(slotId, actionStatus, statusWord1, statusWord2, dataResponse);
                    } catch (RemoteException re) {
                        Log.e(LOG_TAG, "cardPowerDownResponse , remote exception");
                    }

                    break;
                }

                case MSG_SWITCH_SLOT:
                {
                    int actionStatus = byteBuf.getInt();
                    try {
                        callback.switchSlotResponse(actionStatus);
                    } catch (RemoteException re) {
                        Log.e(LOG_TAG, "cardPowerDownResponse , remote exception");
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        private ISAMResponseCallback getCallback(int token) {
            ISAMResponseCallback callback = null;
            synchronized(responseCallbackMap) {
                if (responseCallbackMap.containsKey(token)) {
                    callback = responseCallbackMap.get(token);
                } else {
                    Log.e(LOG_TAG, "Token not available");
                }
            }
            return callback;
        }
    }

    private int getSAMError(CommandException.Error errno) {
        int error;
        switch(errno) {
            case SIM_ABSENT:
                error = SAMErrors.SIM_ABSENT;
                break;

            case RADIO_NOT_AVAILABLE:
                error = SAMErrors.RADIO_NOT_AVAILABLE;
                break;

            case GENERIC_FAILURE:
                error = SAMErrors.INTERNAL_FAILURE;
                break;

            case REQUEST_NOT_SUPPORTED:
                error = SAMErrors.REQUEST_NOT_SUPPORTED;
                break;

            case OP_NOT_ALLOWED_DURING_VOICE_CALL:
                error = SAMErrors.BUSY;
                break;

            case INVALID_PARAMETER:
                error = SAMErrors.INVALID_PARAMETER;
                break;

            default:
                error = SAMErrors.INTERNAL_FAILURE;
                break;
        }

        return error;
    }

    private final ISAM.Stub mBinder = new ISAM.Stub() {

        public int registerUnsolicitedResponseCallback(ISAMUnsolicitedCallback callback) {
            int ret = SAMErrors.SUCCESS;

            Log.i(LOG_TAG, "registerUnsolicitedResponseCallback");

            synchronized (unsolCallbackList) {
                unsolCallbackList.add(callback);
            }

            return ret;
        }

        public int deregisterUnsolicitedResponseCallback(ISAMUnsolicitedCallback callback) {
            int ret = SAMErrors.SUCCESS;

            Log.i(LOG_TAG, "deregisterUnsolicitedResponseCallback");

            synchronized (unsolCallbackList) {
                if (unsolCallbackList.contains(callback)) {
                    unsolCallbackList.remove(callback);
                }
            }
            return ret;
        }

        public int getCardState(int slotId, CardState cardState) {
            int ret = SAMErrors.SUCCESS;
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_GET_CARD_STATE;
            byte[] request = new byte[INT_SIZE];
            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);

            Log.i(LOG_TAG, "getCardState");

            reqBuffer.putInt(slotId);

            AsyncResult ar = mQcRILHook.sendQcRilHookMsg(requestId, request, SAM_PHONE_ID);
            if (ar.exception == null){
                if (ar.result != null) {
                    byte[] result = (byte[])ar.result;
                    ByteBuffer byteBuf = ByteBuffer.wrap(result);
                    byteBuf.order(ByteOrder.nativeOrder());
                    cardState.setSlotId(slotId);
                    cardState.setCardState(byteBuf.getInt());
                } else {
                    ret = SAMErrors.INTERNAL_FAILURE;
                    Log.e(LOG_TAG, "no card state result return");
                }
            } else {
                ret = getSAMError(((CommandException)ar.exception).getCommandError());
                Log.e(LOG_TAG, "getCardState returned exception: " + ar.exception);
            }
            return ret;
        }

        public int getCardATR(int slotId, CardATR cardATR) {
            int ret = SAMErrors.SUCCESS;
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_GET_ATR;
            byte[] request = new byte[INT_SIZE];
            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);

            Log.i(LOG_TAG, "getCardATR");

            reqBuffer.putInt(slotId);

            AsyncResult ar = mQcRILHook.sendQcRilHookMsg(requestId, request, SAM_PHONE_ID);
            if (ar.exception == null){
                if (ar.result != null) {
                    byte[] result = (byte[])ar.result;
                    ByteBuffer byteBuf = ByteBuffer.wrap(result);
                    byteBuf.order(ByteOrder.nativeOrder());
                    cardATR.setSlotId(slotId);
                    byte[] bytes = new byte[byteBuf.remaining()];
                    byteBuf.get(bytes);
                    cardATR.setCardATR(new String(bytes));
                    Log.e(LOG_TAG, "card ATR" + cardATR.getCardATR());
                } else {
                    ret = SAMErrors.INTERNAL_FAILURE;;
                    Log.e(LOG_TAG, "no card ATR result return");
                }
            } else {
                ret = getSAMError(((CommandException)ar.exception).getCommandError());
                Log.e(LOG_TAG, "getCardATR returned exception: " + ar.exception);
            }


            return ret;
        }

        public int streamApdu(int slotId, int class_type, int instruction, int param1, int param2, int param3, String dataCmd, ISAMResponseCallback callback) {
            int ret = SAMErrors.SUCCESS;
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_SEND_APDU_CMD;
            byte[] request = new byte[(INT_SIZE * 7) + dataCmd.length()];
            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);
            int token;

            Log.i(LOG_TAG, "streamApdu");

            reqBuffer.putInt(slotId);
            reqBuffer.putInt(0);
            reqBuffer.putInt(class_type);
            reqBuffer.putInt(instruction);
            reqBuffer.putInt(param1);
            reqBuffer.putInt(param2);
            reqBuffer.putInt(param3);
            reqBuffer.put(dataCmd.getBytes());

            token = getToken();
            Message msg = Message.obtain(null, MSG_SEND_APDU, token, 0);
            OemHookCallback oemHookCb = new SAMOemHookCallback(msg);

            synchronized(responseCallbackMap) {
                responseCallbackMap.put(token, callback);
            }

            mQcRILHook.sendQcRilHookMsgAsync(requestId, request, oemHookCb, SAM_PHONE_ID);
            return ret;
        }

        public int switchSlot(SlotMap slotMap, ISAMResponseCallback callback) {
            int ret = SAMErrors.SUCCESS;
            int token;
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_SWITCH_SLOT;

            int[] slotMapInfo = slotMap.getSlotMap();
            byte[] request = new byte[(INT_SIZE * slotMapInfo.length) + INT_SIZE];
            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);

            Log.i(LOG_TAG, "switchSlot");

            reqBuffer.putInt(slotMapInfo.length);
            for (int slot : slotMapInfo) {
                reqBuffer.putInt(slot);
            }

            token = getToken();
            Message msg = Message.obtain(null, MSG_SWITCH_SLOT, token, 0);
            OemHookCallback oemHookCb = new SAMOemHookCallback(msg);

            synchronized(responseCallbackMap) {
                responseCallbackMap.put(token, callback);
            }

            mQcRILHook.sendQcRilHookMsgAsync(requestId, request, oemHookCb, SAM_PHONE_ID);
            return ret;
        }

        public int getSlotMapping(SlotMap slotMap)
        {
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_GET_SLOT_MAPPING;
            byte[] request = new byte[0];
            int ret = SAMErrors.SUCCESS;
            int[] slotList;
            int mapLen = 0;

            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);

            Log.i(LOG_TAG, "getSlotMapping");

            AsyncResult ar = mQcRILHook.sendQcRilHookMsg(requestId, request, SAM_PHONE_ID);
            if (ar.exception == null) {
                if (ar.result != null) {
                    byte[] result = (byte[])ar.result;
                    ByteBuffer byteBuf = ByteBuffer.wrap(result);
                    byteBuf.order(ByteOrder.nativeOrder());
                    mapLen = byteBuf.getInt();
                    if (mapLen == byteBuf.remaining()/INT_SIZE) {
                        slotList = new int[mapLen];
                        for (int i = 0; i < mapLen; i++) {
                            slotList[i] = byteBuf.getInt();
                        }
                        slotMap.setSlotMap(slotList);
                    } else {
                        Log.e(LOG_TAG, "error in get slot map result");
                        ret = SAMErrors.INTERNAL_FAILURE;;
                    }
                } else {
                    Log.e(LOG_TAG, "error in get slot map result");
                    ret = SAMErrors.INTERNAL_FAILURE;;
                    slotMap.setSlotMap(null);
                }
            } else {
                ret = getSAMError(((CommandException)ar.exception).getCommandError());
                Log.e(LOG_TAG, "getSlotMapping returned exception: " + ar.exception);
            }

            return ret;
        }

        public int cardPowerUp(int slotId, ISAMResponseCallback callback) {
            int ret = SAMErrors.SUCCESS;
            int token;
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_CARD_POWER_REQ;
            byte[] request = new byte[INT_SIZE + INT_SIZE];
            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);

            Log.i(LOG_TAG, "cardPowerUp");

            reqBuffer.putInt(slotId);
            reqBuffer.putInt(1);

            token = getToken();
            Message msg = Message.obtain(null, MSG_CARD_POWER_UP, token, 0);
            OemHookCallback oemHookCb = new SAMOemHookCallback(msg);

            Log.i(LOG_TAG, "cardPowerUp" + callback + "token" + token);
            synchronized(responseCallbackMap) {
                responseCallbackMap.put(token, callback);
            }

            try {
            callback.cardPowerUpResponse(slotId, 0);
            } catch (RemoteException re) {
                Log.e(LOG_TAG, "cardPowerDownResponse , remote exception" + re);
            }
            mQcRILHook.sendQcRilHookMsgAsync(requestId, request, oemHookCb, SAM_PHONE_ID);
            return ret;
        }

        public int cardPowerDown(int slotId, ISAMResponseCallback callback) {
            int ret = SAMErrors.SUCCESS;
            int token;
            int requestId = mQcRILHook.QCRIL_EVT_HOOK_CARD_POWER_REQ;
            byte[] request = new byte[INT_SIZE + INT_SIZE];
            ByteBuffer reqBuffer = mQcRILHook.createBufferWithNativeByteOrder(request);

            Log.i(LOG_TAG, "cardPowerDown");

            reqBuffer.putInt(slotId);
            reqBuffer.putInt(0);

            token = getToken();
            Message msg = Message.obtain(null, MSG_CARD_POWER_DOWN, token, 0);
            OemHookCallback oemHookCb = new SAMOemHookCallback(msg);

            synchronized(responseCallbackMap) {
                responseCallbackMap.put(token, callback);
            }

            mQcRILHook.sendQcRilHookMsgAsync(requestId, request, oemHookCb, SAM_PHONE_ID);
            return ret;
        }
    };
}
