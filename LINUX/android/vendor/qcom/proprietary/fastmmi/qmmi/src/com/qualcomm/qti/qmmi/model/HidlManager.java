/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import android.os.RemoteException;

import com.qualcomm.qti.qmmi.utils.LogUtils;

import vendor.qti.hardware.factory.V1_0.FactoryResult;
import vendor.qti.hardware.factory.V1_0.IFactory;

public final class HidlManager {

    private volatile static HidlManager mHidlManager = null;
    public static final String MM_AUDIO_FTM_BIN = "mm-audio-ftm";
    public static final String FTM_DAEMON_BIN = "ftmdaemon";
    private IFactory iFactory = null;

    private HidlManager() {
        try {
            iFactory = IFactory.getService();
        } catch (Exception e) {
            LogUtils.loge(e.toString());
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.loge("NoClassDefFoundError occur!");
            t.printStackTrace();
        }
    }

    public static HidlManager getInstance() {
        if (mHidlManager == null) {
            synchronized (HidlManager.class) {
                if (mHidlManager == null) {
                    mHidlManager = new HidlManager();
                }
            }
        }
        return mHidlManager;
    }

    public FactoryResult runApp(String name, String params, boolean isStart) {
        LogUtils.logi("start run: " + name + ", params: " + params + ",istart:" + isStart);
        if (iFactory != null) {
            FactoryResult result = null;
            try {
                result = iFactory.runApp(name, params, isStart);
            } catch (RemoteException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return result;
        } else {
            LogUtils.loge("the factory hidl not exist");
            return null;
        }
    }

    public FactoryResult runAudioApp(String params) {
        FactoryResult r = runApp(MM_AUDIO_FTM_BIN, params, true);

        //stop after finished
        //runApp(MM_AUDIO_FTM_BIN, "", false);
        return r;
    }

    public FactoryResult stopAudioApp() {
        FactoryResult r = runApp(MM_AUDIO_FTM_BIN, "", false);
        return r;
    }

    public FactoryResult runFtmDaemonApp(String params) {
        FactoryResult r = runApp(FTM_DAEMON_BIN, params, true);
        return r;
    }

    public FactoryResult stopFtmDaemonApp() {
        FactoryResult r = runApp(FTM_DAEMON_BIN, "", false);
        return r;
    }

    public boolean chargerEnable(boolean enable) {
        if (iFactory != null) {
            boolean result = false;
            try {
                result = iFactory.chargerEnable(enable);
            } catch (RemoteException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return result;
        } else {
            LogUtils.loge("the factory hidl not exist");
            return false;
        }
    }

    public boolean wifiEnable(boolean enable) {
        if (iFactory != null) {
            boolean result = false;
            try {
                result = iFactory.wifiEnable(enable);
            } catch (RemoteException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return result;
        } else {
            LogUtils.loge("the factory hidl not exist");
            return false;
        }
    }

    public boolean enterShipMode() {
        if (iFactory != null) {
            boolean result = false;
            try {
                result = iFactory.enterShipMode();
            } catch (RemoteException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return result;
        } else {
            LogUtils.loge("the factory hidl not exist");
            return false;
        }
    }

    public FactoryResult getSmbStatus() {
        if (iFactory != null) {
            FactoryResult result = null;
            try {
                result = iFactory.getSmbStatus();
            } catch (RemoteException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            return result;
        } else {
            LogUtils.loge("the factory hidl not exist");
            return null;
        }
    }
}
