/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2013 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.setupwizard;

import android.app.admin.DevicePolicyManager;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.UserHandle;
import android.support.v13.app.FragmentStatePagerAdapter;
import android.support.v4.view.ViewPager;

import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.DialogInterface;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.android.internal.widget.LockPatternUtils;
import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.data.AbstractSetupData;
import com.qualcomm.qti.setupwizard.data.Page;
import com.qualcomm.qti.setupwizard.data.PageList;
import com.qualcomm.qti.setupwizard.data.SetupDataCallbacks;
import com.qualcomm.qti.setupwizard.protect.ChooseLockPasswordFragment;
import com.qualcomm.qti.setupwizard.protect.ChooseLockPatternFragment;
import com.qualcomm.qti.setupwizard.protect.NotificationFragment;
import com.qualcomm.qti.setupwizard.protect.ProtectFragment;
import com.qualcomm.qti.setupwizard.protect.SecureStartUpFragment;
import com.qualcomm.qti.setupwizard.sim.NoSimCardFragment;
import com.qualcomm.qti.setupwizard.sim.SimSettingFragment;
import com.qualcomm.qti.setupwizard.ui.CustomViewPager;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;

public class SetupPageActivity extends Activity implements SetupDataCallbacks,
        FragmentCallbacks {

    private static final String TAG = "SetupPageActivity";
    public static final String PROTECT = "protect";
    public static final String CHOOSE_PATTERN = "choosePattern";
    public static final String CHOOSE_PIN = "choosePin";
    public static final String CHOOSE_PASSWORD = "choosePassword";
    public static final String CURRENT_IS_PIN = "currentIsPin";
    public static final String CURRENT_IS_PASSWORD = "currentIsPassword";
    public static final String CURRENT_IS_PATTERN = "currentIsPattern";
    public static final String NOTIFICATION = "notification";
    public static final int RESULT_CODE = 10;
    public static final int CACHE_PAGES = 4;
    public static final int NO_SIM = 0;
    public static final int SIM_SINGLE = 1;
    public static final int SIM_DOUBLE = 2;

    private PageList mPageList;
    private CustomViewPager mViewPager;
    private CMPagerAdapter mPagerAdapter;

    private RelativeLayout mSetupHead;
    private TextView mSetupTitle;
    private LinearLayout mNextButton;
    private ImageButton mBackButton;
    private TextView mNextText;
    private ProgressBar mProgressBar;
    private AbstractSetupData mSetupData;
    private Window mRootView;

    private LockPatternUtils mLockPatternUtils;
    private String mCurrentPage;
    private boolean mRequirePassword;
    private boolean mWifiState;

    private FragmentManager mFragmentManager;
    private Bundle mBundle;

    private SimSettingFragment mSimSettingFragment;
    private NoSimCardFragment mNoSimCardFragment;
    private ProtectFragment mProtectFragment;
    private SecureStartUpFragment mSecureStartUpFragment;
    private NotificationFragment mNotificationFragment;
    private ChooseLockPatternFragment mChooseLockPatternFragment;
    private ChooseLockPasswordFragment mChooseLockPasswordFragment;

    private int mSelectWifiProgress;
    private int mSimCardsSettingProgress;
    private int mSetScreenLockProgress;
    private int mSecureStartUpProgress;
    private int mSetUpProgress;
    private int mConfirmProgress;
    private int mNotificationProgress;
    private boolean mIsSaved = false;

    private final Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.setup_page);
        initProgressResource();
        if (mSetupData == null) {
            mSetupData = new AbstractSetupData(this);
        }
        mRootView = getWindow();
        if (mRootView != null) {
            int flag = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_IMMERSIVE
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
            mRootView.getDecorView().setSystemUiVisibility(flag);
            mRootView.setStatusBarColor(Color.TRANSPARENT);
        }
        mFragmentManager = getFragmentManager();
        mLockPatternUtils = new LockPatternUtils(this);
        mSetupHead = (RelativeLayout) findViewById(R.id.setup_head);
        mSetupTitle = (TextView) mSetupHead.findViewById(R.id.setup_title);
        mBackButton = (ImageButton) findViewById(R.id.back_btn);
        mNextButton = (LinearLayout) findViewById(R.id.next_btn);
        mNextText = (TextView) findViewById(R.id.next_text);
        mProgressBar = (ProgressBar) findViewById(R.id.progress_bar);
        mProgressBar.setProgress(mSelectWifiProgress);
        mSetupData.registerListener(this);
        mPagerAdapter = new CMPagerAdapter(mFragmentManager);
        mViewPager = (CustomViewPager) findViewById(R.id.pager);
        mViewPager.setCanScroll(false);
        mViewPager.setOffscreenPageLimit(CACHE_PAGES);
        mViewPager.setPageTransformer(true, new DepthPageTransformer());
        mViewPager.setAdapter(mPagerAdapter);
        mViewPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                if (position < mPageList.size()) {
                    onPageLoaded(mPageList.get(position));
                }
            }
        });
        mNextButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                doNext();
            }
        });
        mBackButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                doBack();
            }
        });
        onPageTreeChanged();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mIsSaved = true;
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mSetupData.unregisterListener(this);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // TODO Auto-generated method stub
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            doBack();
        }
        return super.onKeyUp(keyCode, event);
    }

    private void initProgressResource() {
        int index = 0;
        if (getResources().getBoolean(R.bool.config_hide_avg)) {
            index = 1;
        }

        mSelectWifiProgress = getResources().getIntArray(R.array.select_wifi_progress)[index];
        mSimCardsSettingProgress = getResources().getIntArray(R.array.sim_cards_setting_progress)[index];
        mSetScreenLockProgress = getResources().getIntArray(R.array.set_screen_lock_progress)[index];
        mSecureStartUpProgress = getResources().getIntArray(R.array.secure_start_up_progress)[index];
        mSetUpProgress = getResources().getIntArray(R.array.set_up_progress)[index];
        mConfirmProgress = getResources().getIntArray(R.array.confirm_progress)[index];
        mNotificationProgress = getResources().getIntArray(R.array.notification_progress)[index];
    }

    protected void doBack() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                final int currentItem = mViewPager.getCurrentItem();
                final Page currentPage = mPageList.get(currentItem);
                if (currentPage.getId() == R.string.protect_page) {
                    if (CHOOSE_PATTERN.equals(mCurrentPage)
                            || CHOOSE_PIN.equals(mCurrentPage)
                            || CHOOSE_PASSWORD.equals(mCurrentPage)
                            || NOTIFICATION.equals(mCurrentPage)
                            || CURRENT_IS_PATTERN.equals(mCurrentPage)
                            || CURRENT_IS_PASSWORD.equals(mCurrentPage)
                            || CURRENT_IS_PIN.equals(mCurrentPage)) {
                        initProtectFragment();
                    } else {
                        backView();
                    }
                } else if (currentItem > 0) {
                    mViewPager.setCurrentItem(currentItem - 1);
                    if (currentPage.getId() == R.string.more_protection_page) {
                        initProtectFragment();
                    } else if(currentPage.getId() == R.string.sim_page) {
                        onChangeWifiNextButtonText();
                    }
                } else if (currentItem == 0) {
                    finish();
                }
            }
        });
    }

    protected void doNext() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                final int currentItem = mViewPager.getCurrentItem();
                final Page currentPage = mPageList.get(currentItem);
                if (currentPage.getId() == R.string.wifi_page) {
                    skipView();
                } else if (currentPage.getId() == R.string.protect_page) {
                    if ((PROTECT.equals(mCurrentPage))
                            && !mLockPatternUtils.isSecure(UserHandle.myUserId())) {
                        showSkipProtectDialog();
                    } else if (CHOOSE_PIN.equals(mCurrentPage)) {
                        initPasswordFragment(
                                DevicePolicyManager.PASSWORD_QUALITY_NUMERIC,
                                mRequirePassword);
                    } else if (CHOOSE_PASSWORD.equals(mCurrentPage)) {
                        initPasswordFragment(
                                DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC,
                                mRequirePassword);
                    } else if (CHOOSE_PATTERN.equals(mCurrentPage)) {
                        initPasswordFragment(
                                DevicePolicyManager.PASSWORD_QUALITY_SOMETHING,
                                mRequirePassword);
                    } else if (CURRENT_IS_PATTERN.equals(mCurrentPage)) {
                        setPatternNext();
                    } else if (CURRENT_IS_PIN.equals(mCurrentPage)
                            || CURRENT_IS_PASSWORD.equals(mCurrentPage)) {
                        setPasswordNext();
                    } else if (NOTIFICATION.equals(mCurrentPage)) {
                        if (getResources().getBoolean(R.bool.config_hide_avg)) {
                            setResult(RESULT_CODE);
                            finish();
                        } else {
                           mViewPager.setCurrentItem(currentItem + 1);
                        }
                    } else {
                        if (getResources().getBoolean(R.bool.config_hide_avg)) {
                            setResult(RESULT_CODE);
                            finish();
                        } else {
                           mViewPager.setCurrentItem(currentItem + 1);
                        }
                    }
                } else if (currentPage.getId() == R.string.more_protection_page) {
                    setResult(RESULT_CODE);
                    finish();
                } else {
                    mViewPager.setCurrentItem(currentItem + 1);
                    if (currentPage.getId() == R.string.sim_page) {
                        initProtectFragment();
                    }
                }
            }
        });
    }

    public void skipView() {
        if (getActiveSimCount() == SIM_SINGLE) {
            mViewPager.setCurrentItem(mViewPager.getCurrentItem() + 2);
            initProtectFragment();
        } else {
            mViewPager.setCurrentItem(mViewPager.getCurrentItem() + 1);
            if (getActiveSimCount() == SIM_DOUBLE) {
                initSimSettingFragment();
            } else {
                initNoSimFragment();
            }
        }
    }

    public void backView() {
        if (getActiveSimCount() == SIM_SINGLE) {
            mViewPager.setCurrentItem(mViewPager.getCurrentItem() - 2);
        } else {
            mViewPager.setCurrentItem(mViewPager.getCurrentItem() - 1);
            if (getActiveSimCount() == SIM_DOUBLE) {
                initSimSettingFragment();
            } else {
                initNoSimFragment();
            }
        }
    }

    public int getActiveSimCount() {
        SubscriptionManager mSubscriptionManager =
                SubscriptionManager.from(this);
        TelephonyManager tm =
                (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        int simCount = tm.getSimCount();
        int activeSimCount;
        activeSimCount = simCount;
        for (int i = 0; i < simCount; ++i) {
            final SubscriptionInfo sir = mSubscriptionManager
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            if (sir == null) {
                activeSimCount --;
            }
        }
        return activeSimCount;
    }

    @Override
    public void skipToNextPage() {
        mViewPager.setCurrentItem(mViewPager.getCurrentItem() + 1);
    }

    public void initSimSettingFragment() {
        if (mIsSaved) return;
        mSimSettingFragment = new SimSettingFragment();
        mFragmentManager.beginTransaction()
                .replace(R.id.base_sim, mSimSettingFragment)
                .commit();
    }

    public void initNoSimFragment() {
        if (mIsSaved) return;
        mSetupTitle.setText(R.string.setup_no_sim_title);
        mNextText.setText(R.string.setup_skip);
        mNoSimCardFragment = new NoSimCardFragment();
        mFragmentManager.beginTransaction()
                .replace(R.id.base_sim, mNoSimCardFragment)
                .commit();
    }

    public void initProtectFragment() {
        if (mIsSaved) return;
        mSetupTitle.setText(R.string.setup_protect_title);
        mNextText.setText(R.string.setup_skip);
        mProgressBar.setProgress(mSetScreenLockProgress);
        mProtectFragment = new ProtectFragment();
        mFragmentManager.beginTransaction()
                .replace(R.id.base_protect, mProtectFragment)
                .commit();
    }

    public void initSecureStartUpFragment() {
        if (mIsSaved) return;
        mSetupTitle.setText(R.string.secure_start_up_title);
        mNextText.setText(R.string.setup_next);
        mProgressBar.setProgress(mSecureStartUpProgress);
        mSecureStartUpFragment = new SecureStartUpFragment();
        mSecureStartUpFragment.setArguments(mBundle);
        mFragmentManager.beginTransaction()
                .replace(R.id.base_protect, mSecureStartUpFragment)
                .commit();
    }

    public void initNotificationFragment() {
        if (mIsSaved) return;
        mSetupTitle.setText(R.string.notification_title);
        if (getResources().getBoolean(R.bool.config_hide_avg)) {
            mNextText.setText(R.string.setup_start);
        } else {
           mNextText.setText(R.string.setup_next);
        }
        mProgressBar.setProgress(mNotificationProgress);
        if (mNotificationFragment == null) {
            mNotificationFragment = new NotificationFragment();
        }
        mFragmentManager.beginTransaction()
                .replace(R.id.base_protect, mNotificationFragment)
                .commit();
    }

    public void initPasswordFragment(int quality, boolean isRequirePassword) {
        if (mIsSaved) return;
        Bundle bundle = new Bundle();
        bundle.putInt("quality", quality);
        bundle.putBoolean("requirePassword", isRequirePassword);
        mProgressBar.setProgress(mSetUpProgress);
        if (quality == DevicePolicyManager.PASSWORD_QUALITY_SOMETHING) {
            mSetupTitle.setText(R.string.choose_pattern_title);
            mNextText.setText(R.string.setup_continue);
            mChooseLockPatternFragment = new ChooseLockPatternFragment();
            mChooseLockPatternFragment.setArguments(bundle);
            mFragmentManager.beginTransaction()
                    .replace(R.id.base_protect, mChooseLockPatternFragment)
                    .commit();
        } else {
            if (quality == DevicePolicyManager.PASSWORD_QUALITY_NUMERIC) {
                mSetupTitle.setText(R.string.choose_pin_title);
                mNextText.setText(R.string.setup_continue);
            } else if (quality == DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC) {
                mSetupTitle.setText(R.string.choose_password_title);
                mNextText.setText(R.string.setup_continue);
            }
            mChooseLockPasswordFragment = new ChooseLockPasswordFragment();
            mChooseLockPasswordFragment.setArguments(bundle);
            mFragmentManager.beginTransaction()
                    .replace(R.id.base_protect, mChooseLockPasswordFragment)
                    .commit();
        }
    }

    public void setPatternNext() {
        mProgressBar.setProgress(mConfirmProgress);
        if (mChooseLockPatternFragment == null) {
            mChooseLockPatternFragment = new ChooseLockPatternFragment();
        }
        mChooseLockPatternFragment.handleRightButton();
    }

    public void setPasswordNext() {
        mProgressBar.setProgress(mConfirmProgress);
        if (mChooseLockPasswordFragment == null) {
            mChooseLockPasswordFragment = new ChooseLockPasswordFragment();
        }
        mChooseLockPasswordFragment.handleNext();
    }

    protected void showSkipProtectDialog() {
        AlertDialog mDialog = new AlertDialog.Builder(this)
                .setMessage(R.string.skip_protect_dialog_msg)
                .setNeutralButton(R.string.dont_skip, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialogInterface.cancel();
                    }
                })
                .setPositiveButton(R.string.skip_anyway, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        if (getResources().getBoolean(R.bool.config_hide_avg)) {
                            setResult(RESULT_CODE);
                            finish();
                        } else {
                           mViewPager.setCurrentItem(mViewPager.getCurrentItem() + 1);
                        }
                    }
                }).create();
        mDialog.show();
    }

    @Override
    public void onPageLoaded(Page page) {
        mSetupHead.setBackgroundResource(page.getSetupBGResId());
        mSetupTitle.setText(page.getSetupTitleResId());
        mNextText.setText(page.getNextButtonResId());
        if (page.getId() == R.string.wifi_page) {
            mProgressBar.setProgress(mSelectWifiProgress);
        } else if (page.getId() == R.string.sim_page) {
            mProgressBar.setProgress(mSimCardsSettingProgress);
        } else if (page.getId() == R.string.protect_page) {
            mProgressBar.setProgress(mSetScreenLockProgress);
        } else if (page.getId() == R.string.more_protection_page) {
            mProgressBar.setProgress(80);
        }

        if (page.isRequired()) {
            if (recalculateCutOffPage()) {
                mPagerAdapter.notifyDataSetChanged();
            }
        }
        updateNextPreviousState();
    }

    @Override
    public void onPageTreeChanged() {
        mPageList = mSetupData.getPageList();
        recalculateCutOffPage();
        mPagerAdapter.notifyDataSetChanged();
        updateNextPreviousState();
    }

    @Override
    public void onPageFinished(final Page page) {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if (page == null) {
                    doNext();
                }
                onPageTreeChanged();
            }
        });
    }

    private void updateNextPreviousState() {
        final int position = mViewPager.getCurrentItem();
        mNextButton.setEnabled(position != mPagerAdapter.getCutOffPage());
    }

    private boolean recalculateCutOffPage() {
        // Cut off the pager adapter at first required page that isn't completed
        int cutOffPage = mPageList.size();
        for (int i = 0; i < mPageList.size(); i++) {
            Page page = mPageList.get(i);
            if (page.isRequired() && !page.isCompleted()) {
                cutOffPage = i;
                break;
            }
        }

        if (mPagerAdapter.getCutOffPage() != cutOffPage) {
            mPagerAdapter.setCutOffPage(cutOffPage);
            return true;
        }

        return false;
    }

    @Override
    public Page getPage(String key) {
        return mSetupData.findPage(key);
    }

    @Override
    public Page getPage(int key) {
        return mSetupData.findPage(key);
    }

    @Override
    public void onFragmentInteraction(String currentPage) {
        mCurrentPage = currentPage;
        if (NOTIFICATION.equals(mCurrentPage)) {
            initNotificationFragment();
        } else if (CHOOSE_PATTERN.equals(mCurrentPage)
                || CHOOSE_PIN.equals(mCurrentPage)
                || CHOOSE_PASSWORD.equals(mCurrentPage)) {
            mBundle = new Bundle();
            if (CHOOSE_PATTERN.equals(mCurrentPage)) {
                mBundle.putInt("quality", DevicePolicyManager.PASSWORD_QUALITY_SOMETHING);
            } else if (CHOOSE_PIN.equals(mCurrentPage)) {
                mBundle.putInt("quality", DevicePolicyManager.PASSWORD_QUALITY_NUMERIC);
            } else if (CHOOSE_PASSWORD.equals(mCurrentPage)) {
                mBundle.putInt("quality", DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC);
            }
            initSecureStartUpFragment();
        }
    }

    @Override
    public void isWifiConnected(boolean isConnected) {
        mWifiState = isConnected;
        final Page currentPage = mPageList.get(mViewPager.getCurrentItem());
        if (currentPage.getId() == R.string.wifi_page) {
            onChangeWifiNextButtonText();
        }
    }

    @Override
    public void onFragmentInteraction(boolean isRequirePassword) {
        mRequirePassword = isRequirePassword;
    }

    public void onChangeWifiNextButtonText() {
        if (mWifiState) {
            mNextText.setText(R.string.setup_next);
        } else {
            mNextText.setText(R.string.setup_skip);
        }
        mNextText.postInvalidate();
    }

    @Override
    public void updateSetupRes(int simCount) {
        if (simCount == NO_SIM) {
            mSetupHead.setBackgroundResource(R.drawable.ic_no_sim_head);
            mSetupTitle.setText(R.string.setup_no_sim_title);
            mNextText.setText(R.string.setup_skip);
        } else {
            mSetupHead.setBackgroundResource(R.drawable.ic_sim_head);
            mSetupTitle.setText(R.string.setup_sim_title);
            mNextText.setText(R.string.setup_next);
        }
        mSetupHead.postInvalidate();
    }

    @Override
    public void updateSetupRes(boolean available) {
        final Page currentPage = mPageList.get(mViewPager.getCurrentItem());
        if (currentPage.getId() == R.string.wifi_page) {
            if (available) {
                mSetupHead.setBackgroundResource(R.drawable.ic_wifi_head);
            } else {
                mSetupHead.setBackgroundResource(R.drawable.ic_no_wifi_head);
            }
            mSetupHead.postInvalidate();
        }
    }

    private class CMPagerAdapter extends FragmentStatePagerAdapter {

        private int mCutOffPage;

        private CMPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int i) {
            return mPageList.get(i).createFragment();
        }

        @Override
        public int getItemPosition(Object object) {
            return POSITION_NONE;
        }

        @Override
        public int getCount() {
            if (mPageList == null)
                return 0;
            return Math.min(mCutOffPage, mPageList.size());
        }

        public void setCutOffPage(int cutOffPage) {
            if (cutOffPage < 0) {
                cutOffPage = Integer.MAX_VALUE;
            }
            mCutOffPage = cutOffPage;
        }

        public int getCutOffPage() {
            return mCutOffPage;
        }
    }

    public static class DepthPageTransformer implements ViewPager.PageTransformer {
        private static float MIN_SCALE = 0.5f;

        public void transformPage(View view, float position) {
            int pageWidth = view.getWidth();

            if (position < -1) {
                view.setAlpha(0);

            } else if (position <= 0) { // [-1,0]
                // Use the default slide transition when moving to the left page
                view.setAlpha(1);
                view.setTranslationX(0);
                view.setScaleX(1);
                view.setScaleY(1);

            } else if (position <= 1) { // (0,1]
                // Fade the page out.
                view.setAlpha(1 - position);

                // Counteract the default slide transition
                view.setTranslationX(pageWidth * -position);

                // Scale the page down (between MIN_SCALE and 1)
                float scaleFactor = MIN_SCALE
                        + (1 - MIN_SCALE) * (1 - Math.abs(position));
                view.setScaleX(scaleFactor);
                view.setScaleY(scaleFactor);

            } else { // (1,+Infinity]
                // This page is way off-screen to the right.
                view.setAlpha(0);
            }
        }
    }
}
