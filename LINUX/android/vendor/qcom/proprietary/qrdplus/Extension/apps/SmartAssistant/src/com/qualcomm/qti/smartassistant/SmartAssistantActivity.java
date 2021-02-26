/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import com.qualcomm.qti.smartassistant.asr.CandidateInfo;
import com.qualcomm.qti.smartassistant.service.Messages;
import com.qualcomm.qti.smartassistant.service.SmartAssistantService;
import com.qualcomm.qti.smartassistant.widget.ContactSelectWindow;
import com.qualcomm.qti.smartassistant.widget.WaveSurfaceView;
import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.utils.SharedPreferenceUtils;
import java.util.List;


public class SmartAssistantActivity extends ServiceActivity {

    private static final String TAG = "SmartAssistantActivity";
    private LinearLayout mWebLayout;
    private WebView mWebView;
    private View mMicLayout;
    private ImageView mMic;
    private RelativeLayout mAsrLayout;
    private TextView mAsrText;
    private ContactSelectWindow mContactSelectWindow;
    private boolean mSwitchStatus = false;
    private WaveSurfaceView mWaveView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        mSwitchStatus = SharedPreferenceUtils.getAssistantEnabled(this);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_smart_assistant);
        initializeUserInterface();
    }

    private void initializeUserInterface() {
        mWebLayout = findViewById(R.id.web_content);
        mMicLayout = findViewById(R.id.home_mic_layout);
        mMic = findViewById(R.id.home_mic);
        mAsrLayout = findViewById(R.id.asr_layout);
        mAsrText = findViewById(R.id.asr_content);
        mAsrLayout.setVisibility(View.INVISIBLE);
        mWebView = new WebView(getApplication());
        configureWebSettings(mWebView);
        mWebLayout.addView(mWebView);
        mWaveView = findViewById(R.id.wave_layout);

        mWebView.setWebViewClient(new WebViewClient() {

            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                super.onPageStarted(view, url, favicon);
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                LogUtils.d(TAG, "onPageFinished "+url);
                mWebView.setVisibility(View.VISIBLE);
                super.onPageFinished(view, url);
            }
        });
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        LogUtils.d(TAG, "onNewIntent By Service"+intent.
                getBooleanExtra(SmartAssistantService.STARTED_BY_ASSISTANT_SERVICE, false));
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_user_main, menu);
        return true;
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        return super.onPrepareOptionsMenu(menu);
    }


    // Handle action bar item clicks.
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.main_action_settings) {
            Intent intent = new Intent(this, AssistantSettingsActivity.class);
            startActivity(intent);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void handleStartVoiceInput() {
        mMicLayout.setVisibility(View.GONE);
        mWaveView.setVisibility(View.VISIBLE);
        mAsrLayout.setVisibility(View.INVISIBLE);
        mWebView.setVisibility(View.INVISIBLE);
//        mWebView.loadUrl("about:blank");
        dismissContactSelectWindow();
    }

    @Override
    protected void handleStopVoiceInput() {
        mMicLayout.setVisibility(View.VISIBLE);
        mWaveView.setVisibility(View.GONE);
    }

    @Override
    protected void handleLoadHtml(String url) {
         mWebView.loadUrl(url);
    }

    @Override
    protected void handleRenderVoiceInputText(String text) {
        if(null != text && text.length() != 0) {
            mAsrLayout.setVisibility(View.VISIBLE);
            mAsrText.setText(text);
        }
    }

    @Override
    protected void handleCallByContacts(List<CandidateInfo> candidateInfos){
        showContactSelectWindow(candidateInfos, "");
    }

    @Override
    protected void handleSendSmsByContacts(List<CandidateInfo> candidateInfos, String message){
        showContactSelectWindow(candidateInfos, message);
    }

    @Override
    protected void executeAfterBindService() {
        if (mSwitchStatus) {
            sendRequest(Messages.START_RECOGNITION_REQUEST, null);
        }
    }

    @Override
    protected void executeAfterUnBindService() {
        if (mSwitchStatus) {
            sendRequest(Messages.STOP_RECOGNITION_REQUEST, null);
        }
    }

    private void showContactSelectWindow(List<CandidateInfo> candidateInfos, String messageContent){
        mContactSelectWindow = new ContactSelectWindow(
                SmartAssistantActivity.this, candidateInfos, messageContent);
        mContactSelectWindow.showWindowOrExecuteCommand(findViewById(R.id.home_mic));
    }

    private void dismissContactSelectWindow(){
        if (mContactSelectWindow != null){
            mContactSelectWindow.dismiss();
        }
    }

    private void configureWebSettings(WebView webView){
        webView.setVerticalScrollBarEnabled(false);
        webView.setHorizontalScrollBarEnabled(false);
        WebSettings localWebSettings = webView.getSettings();
        try {
            localWebSettings.setAllowFileAccess(false);
            localWebSettings.setSupportZoom(false);
            localWebSettings.setBuiltInZoomControls(false);
            localWebSettings.setUseWideViewPort(true);
            localWebSettings.setDomStorageEnabled(true);
            localWebSettings.setLoadWithOverviewMode(true);
            localWebSettings.setCacheMode(WebSettings.LOAD_NO_CACHE);
            localWebSettings.setPluginState(WebSettings.PluginState.ON);
            localWebSettings.setDatabaseEnabled(true);
            String dir = getApplicationContext().getDir("database",
                     Context.MODE_PRIVATE).getPath();
            localWebSettings.setGeolocationDatabasePath(dir);
            localWebSettings.setGeolocationEnabled(true);
            localWebSettings.setJavaScriptEnabled(true);
            localWebSettings.setSavePassword(false);
            String agent = localWebSettings.getUserAgentString();
            localWebSettings.setUserAgentString(agent);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
