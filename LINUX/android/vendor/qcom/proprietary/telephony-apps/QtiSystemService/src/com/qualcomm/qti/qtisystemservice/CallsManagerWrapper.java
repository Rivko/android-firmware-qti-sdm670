/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import android.content.Context;
import android.content.pm.PackageManager;
import android.telecom.ParcelableCall;
import android.os.DeadObjectException;
import android.os.IBinder;
import android.os.RemoteException;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.Collections;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class CallsManagerWrapper implements IBinder.DeathRecipient {
    private final Set<ICallStateListener> mListeners = Collections.newSetFromMap(
            new ConcurrentHashMap<ICallStateListener, Boolean>(8, 0.9f, 1));
    private static CallsManagerWrapper mInstance;
    private Object mCallsManager = null;
    private Context mContext;
    private ReflectionBuilder.ReflectionMethod mRmForToParcelableCall;

    private CallsManagerWrapper(Context context){
        try {
            mContext = context.createPackageContext("com.android.server.telecom",
                    Context.CONTEXT_INCLUDE_CODE | Context.CONTEXT_IGNORE_SECURITY);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        ReflectionBuilder.ReflectionMethod rMethodForTelecomSys = ReflectionBuilder.buildMethod(
                mContext.getClassLoader(), "com.android.server.telecom.TelecomSystem",
                "getInstance");
        if (rMethodForTelecomSys != null) {
            Object telecomSystem = rMethodForTelecomSys.call(null);
            if (telecomSystem != null) {
                mCallsManager = rMethodForTelecomSys.call(telecomSystem, "getCallsManager");
                if (mCallsManager !=  null) {
                    try {
                        Class<?> classForListener = Class.forName(
                                "com.android.server.telecom.CallsManagerListenerBase",
                                false, mContext.getClassLoader());
                        Object proxyObj = Proxy.newProxyInstance(mContext.getClassLoader(),
                                classForListener.getInterfaces(),
                                new OnCallSateChanged(classForListener.newInstance()));
                        Method addListener = mCallsManager.getClass()
                                .getDeclaredMethod("addListener", classForListener.getInterfaces());
                        addListener.invoke(mCallsManager, proxyObj);

                        Class<?> classForCall = Class.forName(
                                "com.android.server.telecom.Call",
                                false, mContext.getClassLoader());

                        Class<?> classForAcount = Class.forName(
                                "com.android.server.telecom.PhoneAccountRegistrar",
                                false, mContext.getClassLoader());
                        mRmForToParcelableCall = ReflectionBuilder.buildMethod(
                                mContext.getClassLoader(),
                                "com.android.server.telecom.ParcelableCallUtils$Converter",
                                "toParcelableCall", new Class[]{classForCall, boolean.class,
                                classForAcount});
                    } catch (ClassNotFoundException e) {
                        e.printStackTrace();
                    } catch (NoSuchMethodException e) {
                        e.printStackTrace();
                    } catch (InstantiationException e) {
                        e.printStackTrace();
                    } catch (IllegalAccessException e) {
                        e.printStackTrace();
                    } catch (InvocationTargetException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    @Override
    public void binderDied() {
        Iterator<ICallStateListener> it = mListeners.iterator();
        while (it.hasNext()) {
            ICallStateListener listener = it.next();
            if (!listener.asBinder().isBinderAlive()) {
                it.remove();
                listener.asBinder().unlinkToDeath(this, 0);
            }
        }
    }

    public static CallsManagerWrapper getInstance(Context context) {
        if (mInstance == null) {
            mInstance = new CallsManagerWrapper(context);
        }
        return  mInstance;
    }

    public boolean hasAnyActiveCall() {
        try {
            return mCallsManager !=null
                    && mCallsManager.getClass()
                    .getDeclaredMethod("getActiveCall")
                    .invoke(mCallsManager) != null;
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return false;
    }

    public boolean hasAnyHeldCall () {
        try {
            return mCallsManager !=null
                    && mCallsManager.getClass()
                    .getDeclaredMethod("getHeldCall")
                    .invoke(mCallsManager) != null;
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return false;
    }

    public ParcelableCall getForegroundCall() {
        Object obj = invokeOnObject(mCallsManager, "getForegroundCall");
        if (obj != null && mRmForToParcelableCall != null) {
            return (ParcelableCall) mRmForToParcelableCall.callWithoutReceiver(obj,
                    true, invokeOnObject(mCallsManager, "getPhoneAccountRegistrar"));
        }
        return null;
    }

    public void registerListener(ICallStateListener listener) {
        for (ICallStateListener it : mListeners) {
            if (it.asBinder() == listener.asBinder()) {
                return;
            }
        }
        if (mListeners.add(listener)) {
            try {
                listener.asBinder().linkToDeath(this, 0);
            } catch (RemoteException e) {
                mListeners.remove(listener);
            }
        }

    }

    public void unregisterListener(ICallStateListener listener) {
        if (listener != null) {
            for (ICallStateListener it : mListeners) {
                if (it.asBinder() == listener.asBinder() && mListeners.remove(listener)) {
                    listener.asBinder().unlinkToDeath(this, 0);
                    break;
                }
            }
        }
    }

    private Object invokeOnObject(Object o, String name, Object ...args) {
        try {
            if (o != null) {
                Method method = o.getClass().getDeclaredMethod(name);
                if (!method.isAccessible()) {
                    method.setAccessible(true);
                }
                return method.invoke(o, args);
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return null;
    }

    private final class OnCallSateChanged implements InvocationHandler {
        private Object mObj;

        public OnCallSateChanged (Object o) {
            mObj = o;
        }

        @Override
        public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {

            String name = method.getName();
            if (name.equals("onCallStateChanged")) {
                ParcelableCall obj= null;
                if (mRmForToParcelableCall != null) {
                    obj = (ParcelableCall) mRmForToParcelableCall.callWithoutReceiver(args[0],
                            true, invokeOnObject(mCallsManager, "getPhoneAccountRegistrar"));
                }
                Iterator<ICallStateListener> it = mListeners.iterator();
                while (it.hasNext()) {
                    ICallStateListener listener = it.next();
                    try {
                        listener.onCallStateChanged(obj, (int) args[2], (int) args[1]);
                        QLog.d(this, "onCallStateChanged new state = " + args[1]
                                + " old state = " + args[2]);
                    } catch (DeadObjectException e) {
                        QLog.d(this, "Object has died");
                    } catch (RemoteException e) {
                        QLog.d(this, "RemoteException");
                    }
                }
            }
            return method.invoke(mObj, args);
        }
    }

}
