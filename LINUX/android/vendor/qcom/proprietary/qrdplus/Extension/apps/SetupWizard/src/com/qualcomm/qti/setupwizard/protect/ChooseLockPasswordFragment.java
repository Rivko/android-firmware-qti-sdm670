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
 * Copyright (C) 2010 The Android Open Source Project
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

import android.app.admin.DevicePolicyManager;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.UserHandle;
import android.app.Fragment;
import android.text.Editable;
import android.text.InputType;
import android.text.Selection;
import android.text.Spannable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;

import com.android.internal.widget.LockPatternUtils;
import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.SetupPageActivity;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;

public class ChooseLockPasswordFragment extends Fragment
         implements TextWatcher, TextView.OnEditorActionListener {

    private FragmentCallbacks mCallbacks;
    private TextView mEditPassword;
    private TextView mHeaderText;
    private String mChosenPassword;
    private String mFirstPin;
    private Stage mUiStage = Stage.Introduction;
    private boolean mIsAlphaMode;
    private int mPasswordMinLength = LockPatternUtils.MIN_LOCK_PASSWORD_SIZE;
    private int mPasswordMaxLength = 16;
    private int mPasswordMinLetters = 0;
    private int mPasswordMinUpperCase = 0;
    private int mPasswordMinLowerCase = 0;
    private int mPasswordMinSymbols = 0;
    private int mPasswordMinNumeric = 0;
    private int mPasswordMinNonLetter = 0;
    protected LockPatternUtils mUtils;
    private int mRequestedQuality = DevicePolicyManager.PASSWORD_QUALITY_NUMERIC;
    private boolean mPasswordRequired;
    private View mView;

    public void initData(int quality, boolean isRequirePassword) {
        InputMethodManager imm =
            (InputMethodManager) getActivity().getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.toggleSoftInput(0, InputMethodManager.HIDE_NOT_ALWAYS);
        mPasswordRequired = isRequirePassword;
        if (quality == DevicePolicyManager.PASSWORD_QUALITY_NUMERIC) {
            onBackLabel(SetupPageActivity.CURRENT_IS_PIN);
        } else if (quality == DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC) {
            onBackLabel(SetupPageActivity.CURRENT_IS_PASSWORD);
        }
        mRequestedQuality = quality;
        mIsAlphaMode = DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC == mRequestedQuality
                || DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC == mRequestedQuality
                || DevicePolicyManager.PASSWORD_QUALITY_COMPLEX == mRequestedQuality;
        // Re-attach to the exiting worker if there is one.

        int currentType = mEditPassword.getInputType();
        mEditPassword.setInputType(mIsAlphaMode ? currentType
                : (InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_VARIATION_PASSWORD));

    }

    private void initViews(View view) {
        mEditPassword = (EditText) view.findViewById(R.id.pin_password);
        mEditPassword.requestFocus();
        mEditPassword.addTextChangedListener(this);
        mEditPassword.setOnEditorActionListener(this);
        mHeaderText = (TextView) view.findViewById(R.id.password_header);

    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {

    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {

    }

    @Override
    public void afterTextChanged(Editable s) {
        // Changing the text while error displayed resets to NeedToConfirm state
        if (mUiStage == Stage.ConfirmWrong) {
            mUiStage = Stage.NeedToConfirm;
        }
        updateUi();
    }

    /**
     * Update the hint based on current Stage and length of password entry
     */
    private void updateUi() {
        String password = mEditPassword.getText().toString();
        final int length = password.length();
        if (mUiStage == Stage.Introduction) {
            if (length < mPasswordMinLength) {
                String msg = getString(mIsAlphaMode ? R.string.lockpassword_password_too_short
                        : R.string.lockpassword_pin_too_short, mPasswordMinLength);
                if (msg == null) {
                    return;
                }
                mHeaderText.setText(msg);
            } else {
                String error = validatePassword(password);
                if (error != null) {
                    mHeaderText.setText(error);
                }
            }
        } else {
            mHeaderText.setText(mIsAlphaMode ? mUiStage.alphaHint : mUiStage.numericHint);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        updateStage(mUiStage);
    }

    /**
     * Validates PIN and returns a message to display if PIN fails test.
     *
     * @param password the raw password the user typed in
     * @return error message to show to user or null if password is OK
     */
    private String validatePassword(String password) {
        if (password.length() < mPasswordMinLength) {
            return getString(mIsAlphaMode ?
                    R.string.lockpassword_password_too_short
                    : R.string.lockpassword_pin_too_short, mPasswordMinLength);
        }
        if (password.length() > mPasswordMaxLength) {
            return getString(mIsAlphaMode ?
                    R.string.lockpassword_password_too_long
                    : R.string.lockpassword_pin_too_long, mPasswordMaxLength + 1);
        }
        int letters = 0;
        int numbers = 0;
        int lowercase = 0;
        int symbols = 0;
        int uppercase = 0;
        int nonletter = 0;
        for (int i = 0; i < password.length(); i++) {
            char c = password.charAt(i);
            // allow non control Latin-1 characters only
            if (c < 32 || c > 127) {
                return getString(R.string.lockpassword_illegal_character);
            }
            if (c >= '0' && c <= '9') {
                numbers++;
                nonletter++;
            } else if (c >= 'A' && c <= 'Z') {
                letters++;
                uppercase++;
            } else if (c >= 'a' && c <= 'z') {
                letters++;
                lowercase++;
            } else {
                symbols++;
                nonletter++;
            }
        }
        if (DevicePolicyManager.PASSWORD_QUALITY_NUMERIC == mRequestedQuality
                || DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX == mRequestedQuality) {
            if (letters > 0 || symbols > 0) {
                // This shouldn't be possible unless user finds some way to bring up
                // soft keyboard
                return getString(R.string.lockpassword_pin_contains_non_digits);
            }
            final int sequence = LockPatternUtils.maxLengthSequence(password);
            if (DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX == mRequestedQuality
                    && sequence > LockPatternUtils.MAX_ALLOWED_SEQUENCE) {
                return getString(R.string.lockpassword_pin_no_sequential_digits);
            }
        } else if (DevicePolicyManager.PASSWORD_QUALITY_COMPLEX == mRequestedQuality) {
            if (letters < mPasswordMinLetters) {
                return String.format(getResources().getQuantityString(
                        R.plurals.lockpassword_password_requires_letters,
                        mPasswordMinLetters), mPasswordMinLetters);
            } else if (numbers < mPasswordMinNumeric) {
                return String.format(getResources().getQuantityString(
                        R.plurals.lockpassword_password_requires_numeric,
                        mPasswordMinNumeric), mPasswordMinNumeric);
            } else if (lowercase < mPasswordMinLowerCase) {
                return String.format(getResources().getQuantityString(
                        R.plurals.lockpassword_password_requires_lowercase,
                        mPasswordMinLowerCase), mPasswordMinLowerCase);
            } else if (uppercase < mPasswordMinUpperCase) {
                return String.format(getResources().getQuantityString(
                        R.plurals.lockpassword_password_requires_uppercase,
                        mPasswordMinUpperCase), mPasswordMinUpperCase);
            } else if (symbols < mPasswordMinSymbols) {
                return String.format(getResources().getQuantityString(
                        R.plurals.lockpassword_password_requires_symbols,
                        mPasswordMinSymbols), mPasswordMinSymbols);
            } else if (nonletter < mPasswordMinNonLetter) {
                return String.format(getResources().getQuantityString(
                        R.plurals.lockpassword_password_requires_nonletter,
                        mPasswordMinNonLetter), mPasswordMinNonLetter);
            }
        } else {
            final boolean alphabetic = DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC
                    == mRequestedQuality;
            final boolean alphanumeric = DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC
                    == mRequestedQuality;
            if ((alphabetic || alphanumeric) && letters == 0) {
                return getString(R.string.lockpassword_password_requires_alpha);
            }
            if (alphanumeric && numbers == 0) {
                return getString(R.string.lockpassword_password_requires_digit);
            }
        }

        return null;
    }

    public void handleNext() {
        mChosenPassword = mEditPassword.getText().toString();
        if (TextUtils.isEmpty(mChosenPassword)) {
            return;
        }
        String errorMsg = null;
        if (mUiStage == Stage.Introduction) {
            errorMsg = validatePassword(mChosenPassword);
            if (errorMsg == null) {
                mFirstPin = mChosenPassword;
                mEditPassword.setText("");
                updateStage(Stage.NeedToConfirm);
            }
        } else if (mUiStage == Stage.NeedToConfirm) {
            if (mFirstPin.equals(mChosenPassword)) {
                saveAndFinish();
            } else {
                CharSequence tmp = mEditPassword.getText();
                if (tmp != null) {
                    Selection.setSelection((Spannable) tmp, 0, tmp.length());
                }
                updateStage(Stage.ConfirmWrong);
            }
        }
        if (errorMsg != null) {
            showError(errorMsg, mUiStage);
        }
    }


    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == MSG_SHOW_ERROR) {
                updateStage((Stage) msg.obj);
            }
        }
    };
    private static final long ERROR_MESSAGE_TIMEOUT = 3000;
    private static final int MSG_SHOW_ERROR = 1;

    private void showError(String msg, final Stage next) {
        mHeaderText.setText(msg);
        mHeaderText.announceForAccessibility(mHeaderText.getText());
        Message mesg = mHandler.obtainMessage(MSG_SHOW_ERROR, next);
        mHandler.removeMessages(MSG_SHOW_ERROR);
        mHandler.sendMessageDelayed(mesg, ERROR_MESSAGE_TIMEOUT);
    }

    protected void updateStage(Stage stage) {
        final Stage previousStage = mUiStage;
        mUiStage = stage;
        updateUi();

        // If the stage changed, announce the header for accessibility. This
        // is a no-op when accessibility is disabled.
        if (previousStage != stage) {
            mHeaderText.announceForAccessibility(mHeaderText.getText());
        }
    }

    @Override
    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        // Check if this was the result of hitting the enter or "done" key
        if (actionId == EditorInfo.IME_NULL
                || actionId == EditorInfo.IME_ACTION_DONE
                || actionId == EditorInfo.IME_ACTION_NEXT) {
            handleNext();
            return true;
        }
        return false;
    }

    protected enum Stage {

        Introduction(R.string.lockpassword_choose_your_password_header,
                R.string.lockpassword_choose_your_pin_header),

        NeedToConfirm(R.string.lockpassword_confirm_your_password_header,
                R.string.lockpassword_confirm_your_pin_header),

        ConfirmWrong(R.string.lockpassword_confirm_passwords_dont_match,
                R.string.lockpassword_confirm_pins_dont_match
        );

        Stage(int hintInAlpha, int hintInNumeric) {
            this.alphaHint = hintInAlpha;
            this.numericHint = hintInNumeric;
        }

        public final int alphaHint;
        public final int numericHint;
    }

    private void saveAndFinish() {
        final int userId = UserHandle.myUserId();
        mUtils.setCredentialRequiredToDecrypt(mPasswordRequired);
        mUtils.saveLockPassword(mChosenPassword, null, mRequestedQuality, userId);
        onBackLabel(SetupPageActivity.NOTIFICATION);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.fragment_choose_lock_password, container, false);
        initViews(mView);
        mUtils = new LockPatternUtils(getActivity());
        if(getArguments()!=null){
            initData(getArguments().getInt("quality"),getArguments().getBoolean("requirePassword"));
        }

        return mView;
    }


    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(String currentPage) {
        if (mCallbacks != null) {
            mCallbacks.onFragmentInteraction(currentPage);
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof FragmentCallbacks) {
            mCallbacks = (FragmentCallbacks) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mCallbacks = null;
    }
}
