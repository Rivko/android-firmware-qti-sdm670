/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Light;

import android.content.Intent;
import android.hardware.light.V2_0.Brightness;
import android.hardware.light.V2_0.Flash;
import android.hardware.light.V2_0.ILight;
import android.hardware.light.V2_0.LightState;
import android.hardware.light.V2_0.Type;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.model.HidlManager;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class LightService extends BaseService {
    private ILight iLight = null;
    private HidlManager hidlManager = null;

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");
        try {
            iLight = ILight.getService();
        } catch (Exception e) {
            LogUtils.loge(e.toString());
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.loge("NoClassDefFoundError occur!");
            t.printStackTrace();
        }

        hidlManager = HidlManager.getInstance();
        if (hidlManager == null) {
            LogUtils.loge("No hidl manager found");
        }
        return super.onStartCommand(intent, flags, startId);
    }


    @Override
    public void register() {

    }

    @Override
    public int stop(TestCase testCase) {
        int type = Integer.valueOf(testCase.getParameter().get("type"));
        try {
            if (type != Type.BACKLIGHT)
                setLight(testCase, false);
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.logi( "No HAL interface, NoClassDefFoundError occur!");
        }

        //disable charger before test light "rgb"
        if (type == Type.NOTIFICATIONS) {
            if (hidlManager != null) {
                hidlManager.chargerEnable(true);
            }
        }
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        int type = Integer.valueOf(testCase.getParameter().get("type"));
        //disable charger before test light "rgb"
        if (type == Type.NOTIFICATIONS) {
            if (hidlManager != null) {
                hidlManager.chargerEnable(false);
            }
        }

        setLight(testCase, true);
        if (iLight != null) {
            updateView(testCase.getName(), this.getResources().getString(R.string.light_on));
        } else {
            updateView(testCase.getName(), this.getResources().getString(R.string.light_hidl_miss));
        }
        return 0;
    }

    private void setLight(TestCase testCase, boolean on) {

        int type = Integer.valueOf(testCase.getParameter().get("type"));
        String color = testCase.getParameter().get("color");
        try {
            LogUtils.logi("LightService service run for:" + Type.toString(type));
            LightState state = new LightState();
            state.flashMode = Flash.NONE;
            state.flashOnMs = 0;
            state.flashOffMs = 0;
            state.brightnessMode = Brightness.USER;

            if (on) {
                if (type == Type.BUTTONS) {   //Button light test
                    state.color = 0xFF020202;
                } else if (type == Type.NOTIFICATIONS) {  // LED light test
                    if ("red".equalsIgnoreCase(color))
                        state.color = 0xFF0000;
                    else if ("blue".equalsIgnoreCase(color))
                        state.color = 0x0000FF;
                    else if ("green".equalsIgnoreCase(color))
                        state.color = 0x00FF00;
                    else
                        LogUtils.loge(" LIGHT: Unknow LED color");

                } else if (type == Type.BACKLIGHT) {
                    state.color = 0xFFFFFF;
                }

            } else {
                state.color = 0x00000000;
            }


            if (iLight != null) {
                LogUtils.logi("LightService set light on for:");
                iLight.setLight(type, state);
            }
        } catch (Exception e) {
            LogUtils.loge("Exception in light service" + e.toString());
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.loge("NoClassDefFoundError occur!");
        }
    }
}
