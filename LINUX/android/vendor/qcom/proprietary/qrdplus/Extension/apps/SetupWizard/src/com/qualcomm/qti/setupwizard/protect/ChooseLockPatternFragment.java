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
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.setupwizard.protect;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.UserHandle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.android.internal.widget.LinearLayoutWithDefaultTouchRecepient;
import com.google.android.collect.Lists;
import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.SetupPageActivity;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class ChooseLockPatternFragment extends Fragment  implements View.OnClickListener {

    private static final String KEY_UI_STAGE = "uiStage";
    private static final String KEY_PATTERN_CHOICE = "chosenPattern";
    private static final String KEY_CURRENT_PATTERN = "currentPattern";

    // how long we wait to clear a wrong pattern
    private static final int WRONG_PATTERN_CLEAR_TIMEOUT_MS = 2000;

    private static final int ID_EMPTY_MESSAGE = -1;

    private FragmentCallbacks callbacks;
    private View view;
    private boolean isRequirePassword;
    private String mCurrentPattern;
    protected TextView mHeaderText;
    protected LockView mLockView;
    protected List<LockView.Cell> mChosenPattern = null;
    private Button mRetry;
    private Stage mUiStage = Stage.Introduction;

    private Runnable mClearPatternRunnable = new Runnable() {
        public void run() {
            mLockView.clearPattern();
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment currentIsPattern
        view = inflater.inflate(R.layout.fragment_choose_lock_pattern, container, false);
        initViews(view);
        if(getArguments()!= null){
            isRequirePassword = getArguments().getBoolean("requirePassword");
        }
        onBackLabel(SetupPageActivity.CURRENT_IS_PATTERN);
        if (savedInstanceState == null) {
            updateStage(Stage.Introduction);
        } else {
            // restore from previous state
            final String patternString = savedInstanceState.getString(KEY_PATTERN_CHOICE);
            if (patternString != null) {
                mChosenPattern = PatternUtils.stringToPattern(patternString);
            }
            if (mCurrentPattern == null) {
                mCurrentPattern = savedInstanceState.getString(KEY_CURRENT_PATTERN);
            }
            updateStage(Stage.values()[savedInstanceState.getInt(KEY_UI_STAGE)]);

        }

        return view;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(String currentPage) {
        if (callbacks != null) {
            callbacks.onFragmentInteraction(currentPage);
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof FragmentCallbacks) {
            callbacks = (FragmentCallbacks) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        callbacks = null;
    }

    /**
     * The patten used during the help screen to show how to draw a pattern.
     */
    private final List<LockView.Cell> mAnimatePattern =
            Collections.unmodifiableList(Lists.newArrayList(
                    LockView.Cell.of(0, 0),
                    LockView.Cell.of(0, 1),
                    LockView.Cell.of(1, 1),
                    LockView.Cell.of(2, 1)
            ));

    /**
     * The pattern listener that responds according to a user choosing a new
     * lock pattern.
     */
    protected LockView.OnPatternListener mChooseNewLockPatternListener =
            new LockView.OnPatternListener() {

                public void onPatternStart() {
                    mLockView.removeCallbacks(mClearPatternRunnable);
                    patternInProgress();
                }
                public void onPatternCleared() {
                    mLockView.removeCallbacks(mClearPatternRunnable);
                }
                @Override
                public void onPatternCellAdded(List<LockView.Cell> pattern) {}

                @Override
                public void onPatternDetected(List<LockView.Cell> pattern) {

                    if (mUiStage == Stage.NeedToConfirm || mUiStage == Stage.ConfirmWrong) {
                        if (mChosenPattern == null) throw new IllegalStateException(
                                "null chosen pattern in stage 'need to confirm");
                        if (mChosenPattern.equals(pattern)) {
                            updateStage(Stage.ChoiceConfirmed);
                        } else {
                            updateStage(Stage.ConfirmWrong);
                        }
                    } else if (mUiStage == Stage.Introduction || mUiStage == Stage.ChoiceTooShort) {
                        if (pattern.size() < PatternUtils.MIN_LOCK_PATTERN_SIZE) {
                            updateStage(Stage.ChoiceTooShort);
                        } else {
                            mChosenPattern = new ArrayList<LockView.Cell>(pattern);
                            updateStage(Stage.FirstChoiceValid);
                        }
                    } else {
                        throw new IllegalStateException("Unexpected stage " + mUiStage + " when "
                                + "entering the pattern.");
                    }
                }

                private void patternInProgress() {
                    mHeaderText.setText(R.string.lockpattern_recording_inprogress);
                }
            };

    /**
     * The states of the left footer button.
     */
    enum RetryButtonMode {
        Cancel(R.string.cancel, true),
        Retry(R.string.lockpattern_retry_button_text, true),
        Gone(ID_EMPTY_MESSAGE, false);

        /**
         * @param text    The displayed text for this mode.
         * @param enabled Whether the button should be enabled.
         */
        RetryButtonMode(int text, boolean enabled) {
            this.text = text;
            this.enabled = enabled;
        }

        final int text;
        final boolean enabled;
    }

    /**
     * The states of the right button.
     */
    enum RightButtonMode {
        Continue(R.string.lockpattern_continue_button_text, true),
        ContinueDisabled(R.string.lockpattern_continue_button_text, false),
        Confirm(R.string.lockpattern_confirm_button_text, true),
        ConfirmDisabled(R.string.lockpattern_confirm_button_text, false),
        Ok(android.R.string.ok, true);

        /**
         * @param text    The displayed text for this mode.
         * @param enabled Whether the button should be enabled.
         */
        RightButtonMode(int text, boolean enabled) {
            this.text = text;
            this.enabled = enabled;
        }
        final int text;
        final boolean enabled;
    }

    /**
     * Keep track internally of where the user is in choosing a pattern.
     */
    protected enum Stage {

        Introduction(
                R.string.lockpattern_recording_intro_header,
                RetryButtonMode.Cancel, RightButtonMode.ContinueDisabled,
                ID_EMPTY_MESSAGE, true),
        HelpScreen(
                R.string.lockpattern_settings_help_how_to_record,
                RetryButtonMode.Gone, RightButtonMode.Ok, ID_EMPTY_MESSAGE, false),
        ChoiceTooShort(
                R.string.lockpattern_recording_incorrect_too_short,
                RetryButtonMode.Retry, RightButtonMode.ContinueDisabled,
                ID_EMPTY_MESSAGE, true),
        FirstChoiceValid(
                R.string.lockpattern_pattern_entered_header,
                RetryButtonMode.Retry, RightButtonMode.Continue, ID_EMPTY_MESSAGE, false),
        NeedToConfirm(
                R.string.lockpattern_need_to_confirm,
                RetryButtonMode.Cancel, RightButtonMode.ConfirmDisabled,
                ID_EMPTY_MESSAGE, true),
        ConfirmWrong(
                R.string.lockpattern_need_to_unlock_wrong,
                RetryButtonMode.Cancel, RightButtonMode.ConfirmDisabled,
                ID_EMPTY_MESSAGE, true),
        ChoiceConfirmed(
                R.string.lockpattern_pattern_confirmed_header,
                RetryButtonMode.Cancel, RightButtonMode.Confirm, ID_EMPTY_MESSAGE, false);

        /**
         * @param headerMessage  The message displayed at the top.
         * @param retryMode       The mode of the left button.
         * @param rightMode      The mode of the right button.
         * @param footerMessage  The footer message.
         * @param patternEnabled Whether the pattern widget is enabled.
         */
        Stage(int headerMessage,
              RetryButtonMode retryMode,
              RightButtonMode rightMode,
              int footerMessage, boolean patternEnabled) {
            this.headerMessage = headerMessage;
            this.retryMode = retryMode;
            this.rightMode = rightMode;
            this.footerMessage = footerMessage;
            this.patternEnabled = patternEnabled;
        }

        final int headerMessage;
        final RetryButtonMode retryMode;
        final RightButtonMode rightMode;
        final int footerMessage;
        final boolean patternEnabled;
    }

    @Override
    public void onResume() {
        super.onResume();
        updateStage(mUiStage);

    }
    private  void initViews(View view){
        mHeaderText = (TextView) view.findViewById(R.id.headerText);
        mLockView = (LockView) view.findViewById(R.id.lockPattern);
        mLockView.setOnPatternListener(mChooseNewLockPatternListener);
        mLockView.setTactileFeedbackEnabled(true);
        mRetry = (Button)view.findViewById(R.id.retry);
        mRetry.setOnClickListener(this);
        final LinearLayoutWithDefaultTouchRecepient topLayout
                = (LinearLayoutWithDefaultTouchRecepient) view.findViewById(
                R.id.topLayout);
        topLayout.setDefaultTouchRecepient(mLockView);
    }

    public void handleRetryButton() {
        if (mUiStage.retryMode == RetryButtonMode.Retry) {
            mChosenPattern = null;
            mLockView.clearPattern();
            updateStage(Stage.Introduction);
        } else if (mUiStage.retryMode == RetryButtonMode.Cancel ) {
        } else {
            throw new IllegalStateException("left footer button pressed, but stage of " +
                    mUiStage + " doesn't make sense");
        }
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    private void saveAndFinish() {
        PatternUtils util = new PatternUtils(getActivity());
        final int userId = UserHandle.myUserId();
        util.saveLockPattern(mChosenPattern, userId);
        util.setCredentialRequiredToDecrypt(isRequirePassword);
        onBackLabel(SetupPageActivity.NOTIFICATION);
    }

    public void handleRightButton() {
        if (mUiStage.rightMode == RightButtonMode.Continue) {
            if (mUiStage != Stage.FirstChoiceValid) {
                throw new IllegalStateException("expected ui stage "
                        + Stage.FirstChoiceValid + " when button is "
                        + RightButtonMode.Continue);
            }
            updateStage(Stage.NeedToConfirm);
            mRetry.setVisibility(View.GONE);
        } else if (mUiStage.rightMode == RightButtonMode.Confirm) {
            if (mUiStage != Stage.ChoiceConfirmed) {
                throw new IllegalStateException("expected ui stage " + Stage.ChoiceConfirmed
                        + " when button is " + RightButtonMode.Confirm);
            }
            saveAndFinish();
        } else if (mUiStage.rightMode == RightButtonMode.Ok) {
            if (mUiStage != Stage.HelpScreen) {
                throw new IllegalStateException("Help screen is only mode with ok button, "
                        + "but stage is " + mUiStage);
            }
            mLockView.clearPattern();
            mLockView.setDisplayMode(LockView.DisplayMode.Correct);
            updateStage(Stage.Introduction);
        }
    }

    public void onClick(View v) {
        if (v == mRetry) {
            handleRetryButton();
        }
    }

    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        outState.putInt(KEY_UI_STAGE, mUiStage.ordinal());
        if (mChosenPattern != null) {
            outState.putString(KEY_PATTERN_CHOICE,
                    patternToString(mChosenPattern));
        }

        if (mCurrentPattern != null) {
            outState.putString(KEY_CURRENT_PATTERN,
                    mCurrentPattern);
        }
    }

    public static String patternToString(List<LockView.Cell> pattern) {
        if (pattern == null) {

            return "";
        }

        final int patternSize = pattern.size();

        byte[] res = new byte[patternSize];
        for (int i = 0; i < patternSize; i++) {
            LockView.Cell cell = pattern.get(i);
            res[i] = (byte) (cell.getRow() * 3 + cell.getColumn() + '1');
        }
        return new String(res);
    }

    /**
     * Updates the messages and buttons appropriate to what stage the user
     * is at in choosing a view.  This doesn't handle clearing out the pattern;
     * the pattern is expected to be in the right state.
     *
     * @param stage
     */
    protected void updateStage(Stage stage) {
        final Stage previousStage = mUiStage;
        mUiStage = stage;

        // header text, footer text, visibility and
        // enabled state all known from the stage
        if (stage == Stage.ChoiceTooShort) {
            mHeaderText.setText(
                    getResources().getString(
                            stage.headerMessage,
                            PatternUtils.MIN_LOCK_PATTERN_SIZE));
        } else {
            mHeaderText.setText(stage.headerMessage);
        }
        // same for whether the pattern is enabled
        if (stage.patternEnabled) {
            mLockView.enableInput();
        } else {
            mLockView.disableInput();
        }

        // the rest of the stuff varies enough that it is easier just to handle
        // on a case by case basis.
        mLockView.setDisplayMode(LockView.DisplayMode.Correct);
        boolean announceAlways = false;

        switch (mUiStage) {
            case Introduction:
                mLockView.clearPattern();
                break;
            case HelpScreen:
                mLockView.setPattern(LockView.DisplayMode.Animate, mAnimatePattern);
                break;
            case ChoiceTooShort:
                mLockView.setDisplayMode(LockView.DisplayMode.Wrong);
                postClearPatternRunnable();
                announceAlways = true;
                break;
            case FirstChoiceValid:
                break;
            case NeedToConfirm:
                mLockView.clearPattern();
                break;
            case ConfirmWrong:
                mLockView.setDisplayMode(LockView.DisplayMode.Wrong);
                postClearPatternRunnable();
                announceAlways = true;
                break;
            case ChoiceConfirmed:
                break;
        }

        // If the stage changed, announce the header for accessibility. This
        // is a no-op when accessibility is disabled.
        if (previousStage != stage || announceAlways) {
            mHeaderText.announceForAccessibility(mHeaderText.getText());
        }
    }

    // clear the wrong pattern unless they have started a new one
    // already
    private void postClearPatternRunnable() {
        mLockView.removeCallbacks(mClearPatternRunnable);
        mLockView.postDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS);
    }
}
