/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2014, The Linux Foundation. All Rights Reserved.
 */

package com.qualcomm.qti.extsettings;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.TextView;
import com.qualcomm.qti.extsettings.R;
import com.qualcomm.qti.extsettings.hotspot.Res;

public class WifiApHelpActivity extends Activity {

    private static WifiConfiguration wifiApConfig = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.wifi_ap_help);
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        wifiApConfig = wifiManager.getWifiApConfiguration();
        ((TextView) findViewById(R.id.content_view_settings_desc)).
                setText(Res.getSettingsString(this, Res.STRING_WIFIAP_SETTINGS_DESC));
        ((TextView) findViewById(R.id.content_view_settings_indication)).
                setText(Res.getSettingsString(this, Res.STRING_WIFIAP_SETTINGS_INDICATION));
        ((TextView) findViewById(R.id.content_view_settings_indication_step1)).
                setText(Res.getSettingsString(this, Res.STRING_WIFIAP_SETTINGS_INDICATION_STEP1));
        ((TextView) findViewById(R.id.wifiap_help_select_ssid)).
                setText(getWifiApSsid());

    }

    private String getIndicateText() {
        if (wifiApConfig == null) {
            return null;
        }

        if (wifiApConfig.preSharedKey == null) {
            return getString(R.string.wifiap_settings_indication_step3_ssid,
                    wifiApConfig.SSID);
        }
        return getString(R.string.wifiap_settings_indication_step3_all,
                wifiApConfig.SSID, wifiApConfig.preSharedKey);

    }

    private String getWifiApSsid() {
        if (wifiApConfig == null) {
            return null;
        }
        return Res.getSettingsString(this, Res.STRING_WIFIAP_SETTINGS_INDICATION_STEP2);
    }

    @Override
    protected void onResume() {
        super.onResume();

        ((TextView) findViewById(R.id.wifiap_help_ssid_pwd)).setText(getIndicateText());
        ((TextView) findViewById(R.id.wifiap_help_select_ssid)).setText(getWifiApSsid());
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

}
