/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Fingerprint;

import android.Manifest;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.fingerprint.FingerprintManager;
import android.os.CancellationSignal;
import android.os.HwBinder;
import android.os.AsyncTask;
import android.os.RemoteException;

import java.util.NoSuchElementException;
import java.util.Map;
import java.util.HashMap;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import vendor.qti.hardware.fingerprint.V1_0.IQtiExtendedFingerprint;
import vendor.qti.hardware.fingerprint.V1_0.DigitalTestResult;
import vendor.qti.hardware.fingerprint.V1_0.BiasTestResult;
import vendor.qti.hardware.fingerprint.V1_0.TxTestResult;
import vendor.qti.hardware.fingerprint.V1_0.PsfVerificationResult;
import vendor.qti.hardware.fingerprint.V1_0.PsfCalibrationResult;
import vendor.qti.hardware.fingerprint.V1_0.ContactResistanceTestResult;
import vendor.qti.hardware.fingerprint.V1_0.ImageQualityTestResult;
import vendor.qti.hardware.fingerprint.V1_0.FieldMfgFormat;

public class FingerprintService extends BaseService {

    private static String mSnrMeanRemoval = null;
    private final int TEST_PSF_PHASE_LL = 65;
    private final int TEST_PSF_PHASE_UL = 66;
    private final int SNR_MEAN_LINE_REMOVAL = 75;

    private final int SUCCESS = 0;
    private final int FAILURE = 1;
    private final int ENABLE = 1;
    private final int DISABLE = 0;
    private String CASE_NAME;
    private IQtiExtendedFingerprint mQtiExtendedFingerprint = null;
    private FingerprintManager mFingerprintManager;
    private boolean mHasFingerprintHardware;
    private TestCase mTestCase;
    private HwBinder.DeathRecipient mDeathRecipient = new HwBinder.DeathRecipient() {
        @Override
        public void serviceDied(long cookie) {
            LogUtils.loge("fingerprint HAL died");
            updateView(mTestCase.getName(), "fingerprint HAL died");
            mQtiExtendedFingerprint = null;
        }
    };

    private IQtiExtendedFingerprint getQtiExtendedFingerpintService() {
        if (mQtiExtendedFingerprint == null) {
            try {
                mQtiExtendedFingerprint = IQtiExtendedFingerprint.getService();
                if (mQtiExtendedFingerprint == null) {
                    LogUtils.loge("Unable to get IQtiExtendedFingerprint");
                    updateView(mTestCase.getName(), "Unable to get IQtiExtendedFingerprint");
                    updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                    return null;
                }
            } catch (RemoteException e) {
                LogUtils.loge("RemoteException: in get IQtiExtendedFingerprint" + e.getMessage());
                updateView(mTestCase.getName(), "RemoteException: in get IQtiExtendedFingerprint" + e.getMessage());
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return null;
            } catch (NoSuchElementException e) {
                LogUtils.loge("NoSuchElementException:in get" +
                        "IQtiExtendedFingerprint" + e.getMessage());
                updateView(mTestCase.getName(), "NoSuchElementException:in get" +
                        "IQtiExtendedFingerprint" + e.getMessage());
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return null;
            } catch (Exception e) {
                LogUtils.loge("Exception: in get IQtiExtendedFingerprint" + e.getMessage());
                updateView(mTestCase.getName(), "Exception: in get IQtiExtendedFingerprint" + e.getMessage());
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return null;
            }
            try {
                if (mQtiExtendedFingerprint != null) {
                    mQtiExtendedFingerprint.linkToDeath(mDeathRecipient, 0);
                }
            } catch (Exception e) {
                updateView(mTestCase.getName(), "Unable to link to death " + e.getMessage());
                LogUtils.loge("Unable to link to death " + e.getMessage());
            }
        }
        return mQtiExtendedFingerprint;
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");
        if (this.checkSelfPermission(Manifest.permission.USE_FINGERPRINT) != PackageManager.PERMISSION_GRANTED) {
            return Service.START_NOT_STICKY;
        }
        PackageManager pm = getPackageManager();
        if (pm.hasSystemFeature(PackageManager.FEATURE_FINGERPRINT)) {
            mFingerprintManager = (FingerprintManager)
                    getSystemService(Context.FINGERPRINT_SERVICE);
            mHasFingerprintHardware = mFingerprintManager.isHardwareDetected();
        }
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void register() {
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

    @Override
    public int run(TestCase testCase) {
        LogUtils.logi("SensorService service run");
        String resultStr = getResources().getString(R.string.fprint_in_progress);
        mTestCase = testCase;
        if (mTestCase == null) {
            LogUtils.loge("Null param, please check the framework error");
            return -1;
        }
        updateView(mTestCase.getName(), resultStr);
        String method = mTestCase.getMethod();
        String caseName = mTestCase.getName();
        if (!mHasFingerprintHardware) {
            LogUtils.loge("No fingerprint hardware found");
            updateView(caseName, getResources().getString(R.string.fprint_no_hardware));
            updateResultForCase(caseName, TestCase.STATE_FAIL);
            return -1;
        }
        if (getQtiExtendedFingerpintService() == null) {
            LogUtils.loge("Fingerprint is disabled");
            updateView(caseName, getResources().getString(R.string.fprint_disabled));
            updateResultForCase(caseName, TestCase.STATE_FAIL);

            return -1;
        }

        if (!configureParams()) {
            LogUtils.loge("Failed to set specified paramerters");
            updateView(caseName, getResources().getString(R.string.fprint_param_set_fail));
            updateResultForCase(caseName, TestCase.STATE_FAIL);
            return -1;
        }
        if (method != null) {
            if (method.equalsIgnoreCase("digital_test")) {
                runDigitalTest();
            } else if (method.equalsIgnoreCase("sensor_id")) {
                runSensorId();
            } else if (method.equalsIgnoreCase("tft_test")) {
                runTFTTest();
            } else if (method.equalsIgnoreCase("tx_test")) {
                runTxTest();
            } else if (method.equalsIgnoreCase("bge_calibration")) {
                runBGECalibration();
            } else if (method.equalsIgnoreCase("psf_calibration")) {
                runPSFCalibration();
            } else if (method.equalsIgnoreCase("psf_phase_test")) {
                runPsfPhaseTest();
            } else if (method.equalsIgnoreCase("boost_regulation_test")) {
                runBoostRegulationTest();
            } else if (method.equalsIgnoreCase("contact_resistance")) {
                runContactResistanceTest();
            } else if (method.equalsIgnoreCase("image_quality")) {
                runImageQualityTest();
            } else if (method.equalsIgnoreCase("start_test")) {
                runStartTest();
            } else if (method.equalsIgnoreCase("stop_test")) {
                runStopTest();
            }
        } else {
            LogUtils.loge("no method , or not support method");
        }
        LogUtils.logi("FP service run");
        return 0;
    }

    private boolean configureParams() {
        Map<String, String> data = mTestCase.getParameter();
        if (data != null) {
            for (String key : data.keySet()) {
                String value = data.get(key);
                try {
                    getQtiExtendedFingerpintService().testParamSet(Params.valueOf(key).getParamKey(), value);
                    if (getQtiExtendedFingerpintService().testParamGet(Params.valueOf(key).getParamKey())
                            .equals(value)) {
                        LogUtils.logi("Successfully updated " + key + " to " + value);
                    } else {
                        LogUtils.loge("Failed to set " + key + " to " + value);
                        return false;
                    }
                } catch (Exception e) {
                    LogUtils.loge("Failed to set " + key + " " + e.getMessage());
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    }

    private void runStartTest() {
        try {
            if (getQtiExtendedFingerpintService().testInit() != 0) {
                LogUtils.loge("Test init Failed");
                updateView(mTestCase.getName(), mTestCase.getName() + " " + getResources()
                    .getString(R.string.fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            } else {
                updateView(mTestCase.getName(), mTestCase.getName() + " " + getResources()
                    .getString(R.string.pass));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            }
        } catch (Exception e) {
            LogUtils.loge("Exception in initialize tests framework: " + e.getMessage());
            updateView(mTestCase.getName(), mTestCase.getName() + " " + getResources()
                .getString(R.string.fail) + " " + e.getMessage());
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
        }
    }

    private void runStopTest() {
        try {
            if (getQtiExtendedFingerpintService().testDeinit() != 0) {
                LogUtils.loge("Test init Failed");
                updateView(mTestCase.getName(), mTestCase.getName() + " " + getResources()
                    .getString(R.string.fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            } else {
                updateView(mTestCase.getName(), mTestCase.getName() + " " + getResources()
                    .getString(R.string.pass));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            }
        } catch (Exception e) {
            LogUtils.loge("Exception in initialize tests framework: " + e.getMessage());
            updateView(mTestCase.getName(), mTestCase.getName() + " " + getResources()
                .getString(R.string.fail) + " " + e.getMessage());
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
        }
    }

    private void runDigitalTest() {
        DigitalTestResult digitalTestResult = null;
        String result = "";
        try {
            digitalTestResult = getQtiExtendedFingerpintService().testDigital();
        } catch (Exception e) {
            LogUtils.loge("Exception in Digital Test: " + e.getMessage());
            updateView(mTestCase.getName(), getResources().getString(R.string.fprint_digital_exception) + e.getMessage());
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            return;
        }
        if (null == digitalTestResult) {
            LogUtils.loge("Digital Test QFP Failure NULL returned");
            updateView(mTestCase.getName(), "Digital Test " + getResources().getString(R.string.fprint_null_fail));
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            return;
        }
        LogUtils.logi("chip id:" + digitalTestResult.asicId + " chip sn: "
                + digitalTestResult.asicSerialNo);
        if (SUCCESS == digitalTestResult.status) {
            // test passed
            updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            result += "SUCCESS \n";
        } else {
            // test failed
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            result += "Digital Test QFP Failure " + digitalTestResult.status + "\n";
        }
        //set test results
        result += "Chipd ID: 0x" + Integer.toHexString(digitalTestResult.asicId) + "\n"
                + "Chipd SN: " + String.valueOf(digitalTestResult.asicSerialNo).toString();
        mTestCase.addTestData("CHIP_ID", Integer.toHexString(digitalTestResult.asicId));
        mTestCase.addTestData("CHIP_SN", String.valueOf(digitalTestResult.asicSerialNo).toString());
        updateView(mTestCase.getName(), result);
    }

    private void runTFTTest() {
        new TFTTest().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runTxTest() {
        new TxTest().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runBGECalibration() {
        new BGECalibration().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runPsfPhaseTest() {
        new PSFVerification().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runBoostRegulationTest() {
        new BoostRegulation().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runContactResistanceTest() {
        new ContactResistanceTest().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runImageQualityTest() {
        try {
            mSnrMeanRemoval = getQtiExtendedFingerpintService().testParamGet(SNR_MEAN_LINE_REMOVAL);
        } catch (Exception e) {
            e.printStackTrace();
            LogUtils.loge("SNR_MEAN_LINE_REMOVAL Test Param Get failed " + e.getMessage());
            updateView(mTestCase.getName(), "SNR_MEAN_LINE_REMOVAL " + getResources().getString(R.string.fprint_test_param_get_fail) + e.getMessage());
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
        }
        try {
            getQtiExtendedFingerpintService().testParamSet(SNR_MEAN_LINE_REMOVAL,
                    Integer.toString(DISABLE));
        } catch (Exception e_snr) {
            e_snr.printStackTrace();
            LogUtils.loge("SNR_MEAN_LINE_REMOVAL Test Param Set failed " + e_snr.getMessage());
            updateView(mTestCase.getName(), "SNR_MEAN_LINE_REMOVAL " + getResources().getString(R.string.fprint_test_param_set_fail) + e_snr.getMessage());
            updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
        }
        new ImageQuality().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runPSFCalibration() {
        new PSFCalibration().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private void runSensorId() {
        new MfgFieldVal().executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    private enum Params {
        TEST_TFT_START(1),
        TEST_TFT_END(2),
        TEST_TFT_STEP(3),
        TEST_TFT_MEDIAN_LL(4),
        TEST_TFT_MEDIAN_UL(5),

        TEST_TX_MEDIAN_LL(11),
        TEST_TX_MEDIAN_UL(12),

        TEST_PSF_F_RANGE(48),
        TEST_PSF_F_NUM_IMAGES(49),
        TEST_PSF_TEMPLATE(56),
        TEST_PSF_TEMPLATE_TYPE(78),
        TEST_PSF_CROP(60),
        TEST_PSF_CROP_X(61),
        TEST_PSF_CROP_Y(62),
        TEST_PSF_CROP_W(63),
        TEST_PSF_CROP_H(64),
        TEST_PSF_FREQUENCY(67),

        TEST_PSF_PHASE_LL(65),
        TEST_PSF_PHASE_UL(66),

        TEST_BOOST_CURRENT_LIMIT(70),

        TEST_IQ_LL(17),
        TEST_IQ_UL(18),
        TEST_IQ_SNR_TEMPLATE(55),
        TEST_IQ_TEMPLATE_TYPE(74),
        TEST_IQ_ENABLE_PSF(50),
        TEST_IQ_FREQ_DELTA(51),
        TEST_IQ_FREQ_NP(52),
        TEST_IQ_RGD_DELTA(53),
        TEST_IQ_RGD_NP(54),;

        private final int paramKey;

        Params(int code) {
            this.paramKey = code;
        }

        public int getParamKey() {
            return this.paramKey;
        }
    }

    private class TFTTest extends AsyncTask<Void, Integer, BiasTestResult> {

        String result = "";

        protected void onPostExecute(BiasTestResult tftTestResult) {
            if (null == tftTestResult) {
                LogUtils.loge("TFTTest QFP Failure NULL returned");
                updateView(mTestCase.getName(), "TFTTest " + getResources().getString(R.string.fprint_null_fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return;
            }
            if (SUCCESS == tftTestResult.status) {
                // test passed
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                result += "SUCCESS \n";
            } else {
                // test failed
                result += "TFTTest QFP Failure " + tftTestResult.status + "\n";
                LogUtils.loge("TFTTest QFP Failure " + tftTestResult.status);
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
            // set test results
            result += "Dbias   " + String.valueOf(tftTestResult.bias) + "\n";
            result += "Median  " + String.valueOf(tftTestResult.median) + "\n";
            result += "Stddev  " + String.valueOf(tftTestResult.stddev) + "\n";
            result += "Q5      " + String.valueOf(tftTestResult.q5) + "\n";
            result += "Q95     " + String.valueOf(tftTestResult.q95) + "\n";
            updateView(mTestCase.getName(), result);
            mTestCase.addTestData("DBIAS", String.valueOf(tftTestResult.bias));
            mTestCase.addTestData("MEDIAN", String.valueOf(tftTestResult.median));
            mTestCase.addTestData("STDDEV", String.valueOf(tftTestResult.stddev));
            mTestCase.addTestData("Q5", String.valueOf(tftTestResult.q5));
            mTestCase.addTestData("Q95", String.valueOf(tftTestResult.q95));

        }

        @Override
        protected BiasTestResult doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().testTft();
            } catch (Exception e) {
                LogUtils.loge("Exception in TFT Test: " + e.getMessage());
                return null;
            }
        }
    }

    private class TxTest extends AsyncTask<Void, Integer, TxTestResult> {
        String result = "";

        protected void onPostExecute(TxTestResult txTestResult) {
            if (null == txTestResult) {
                LogUtils.loge("TxTest QFP Failure NULL returned");
                updateView(mTestCase.getName(), "TxTest " +  getResources().getString(R.string.fprint_null_fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return;
            }
            if (SUCCESS == txTestResult.status) {
                // test passed
                result += "SUCCESS \n";
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            } else {
                // test failed
                result += "TxTest QFP Failure " + txTestResult.status + "\n";
                LogUtils.loge("TxTest QFP Failure " + txTestResult.status);
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
            // set test results
            result += "Median  " + String.valueOf(txTestResult.median) + "\n";
            result += "Stddev  " + String.valueOf(txTestResult.stddev) + "\n";
            result += "Q5      " + String.valueOf(txTestResult.q5) + "\n";
            result += "Q95     " + String.valueOf(txTestResult.q95) + "\n";
            updateView(mTestCase.getName(), result);
            mTestCase.addTestData("MEDIAN", String.valueOf(txTestResult.median));
            mTestCase.addTestData("STDDEV", String.valueOf(txTestResult.stddev));
            mTestCase.addTestData("Q5", String.valueOf(txTestResult.q5));
            mTestCase.addTestData("Q95", String.valueOf(txTestResult.q95));

        }

        @Override
        protected TxTestResult doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().testTx();
            } catch (Exception e) {
                LogUtils.loge("Exception in Tx Test: " + e.getMessage());
                return null;
            }
        }
    }

    private class BGECalibration extends AsyncTask<Void, Integer, Integer> {

        protected void onPostExecute(Integer bgeResult) {
            if (SUCCESS == bgeResult) {
                // calibration succeeded
                updateView(mTestCase.getName(), "SUCCESS");
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            } else {
                // calibration failed
                updateView(mTestCase.getName(), "BGECalibration QFP Failure " + bgeResult);
                LogUtils.loge("BGECalibration QFP Failure " + bgeResult);
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
        }

        @Override
        protected Integer doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().bgeCalibration();
            } catch (Exception e) {
                LogUtils.loge("Exception in BGE calibration: " + e.getMessage());
                return FAILURE;
            }
        }
    }

    private class PSFVerification extends AsyncTask<Void, Integer, PsfVerificationResult> {
        @Override
        protected void onPostExecute(PsfVerificationResult psfVerificationResult) {
            String result = "";
            if (null == psfVerificationResult) {
                updateView(mTestCase.getName(), "PSFVerification "  + getResources().getString(R.string.fprint_null_fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return;
            }
            LogUtils.logi(psfVerificationResult.residualPhase + " is the residual phase");
            float residualPhase = ((float) psfVerificationResult.residualPhase / (float) 10000);
            result += "Residual phase " + residualPhase + "\n";
            String residualPhaseLL, residualPhaseUL;
            float rPhaseLL = -0.5f, rPhaseUL = 0.5f;
            try {
                residualPhaseLL = getQtiExtendedFingerpintService().testParamGet(TEST_PSF_PHASE_LL);
                rPhaseLL = Float.valueOf(residualPhaseLL);
            } catch (Exception e) {
                LogUtils.loge("TEST_PSF_PHASE_LL Probably parameter value is not set exception "
                        + e.getMessage());
                result += "TEST_PSF_PHASE_LL " + getResources().getString(R.string.fprint_param_set_fail)
                        + e.getMessage() + "\n";
            }
            try {
                residualPhaseUL = getQtiExtendedFingerpintService().testParamGet(TEST_PSF_PHASE_UL);
                rPhaseUL = Float.valueOf(residualPhaseUL);
            } catch (Exception e) {
                LogUtils.loge("TEST_PSF_PHASE_UL Probably parameter value is not set exception "
                        + e.getMessage());
                result += "TEST_PSF_PHASE_UL " + getResources().getString(R.string.fprint_param_set_fail)
                        + e.getMessage() + "\n";
            }
            if (SUCCESS == psfVerificationResult.status) {
                if (residualPhase < rPhaseLL || residualPhase > rPhaseUL) {
                    LogUtils.loge("PSFVerification Failure: Residual Phase out of Bounds");
                    result += "PSFVerification Failure: Residual Phase" +
                            getResources().getString(R.string.fprint_out_bounds) +  "\n";
                    updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                } else {
                    result += "SUCCESS Residual Phase " + getResources().getString(R.string.fprint_out_bounds) + "\n";
                    updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                }
            } else {
                LogUtils.loge("PSFVerification QFP Failure " + psfVerificationResult.status);
                result += "PSFVerification QFP Failure " + psfVerificationResult.status;
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
            updateView(mTestCase.getName(), result);
            mTestCase.addTestData("PHASE", String.valueOf(residualPhase));

        }

        @Override
        protected PsfVerificationResult doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().runPsfVerification();
            } catch (Exception e) {
                LogUtils.loge("Exception in PSF Verification: " + e.getMessage());
                return null;
            }
        }
    }

    private class BoostRegulation extends AsyncTask<Void, Integer, Integer> {

        protected void onPostExecute(Integer boostResult) {
            if (SUCCESS == boostResult) {
                // calibration succeeded
                updateView(mTestCase.getName(), getResources().getString(R.string.pass));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            } else {
                // calibration failed
                updateView(mTestCase.getName(), getResources().getString(R.string.fail));
                LogUtils.loge("BoostRegulation QFP Failure " + boostResult);
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
        }

        @Override
        protected Integer doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().runBoostRegulation();
            } catch (Exception e) {
                LogUtils.loge("Exception in Boost Regulation Test: " + e.getMessage());
                return FAILURE;
            }
        }
    }

    private class ContactResistanceTest
            extends AsyncTask<Void, Integer, ContactResistanceTestResult> {

        @Override
        protected ContactResistanceTestResult doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().testContactResistance();
            } catch (Exception e) {
                LogUtils.loge("Exception in Contact Resistance Test: " + e.getMessage());
                return null;
            }
        }

        @Override
        protected void onPostExecute(ContactResistanceTestResult result) {
            if (null != result && SUCCESS == result.status) {
                // Contact Resistance test succeeded
                LogUtils.logi("SUCCESS \nResistance in ohm: " + String.format("%.1f", result.rsenseOhm));
                updateView(mTestCase.getName(), "SUCCESS \nResistance in ohm: " + String
                        .format("%.1f", result.rsenseOhm));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                mTestCase.addTestData("RESISTANCE", String.format("%.1f", result.rsenseOhm));

            } else {
                LogUtils.loge("ContactResistanceTest QFP Failure returned: "
                        + (null == result ?
                        "NULL" :
                        result.status));
                updateView(mTestCase.getName(), "ContactResistanceTest QFP Failure returned: "
                        + (null == result ?
                        "NULL" :
                        result.status));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
        }
    }

    private class ImageQuality extends AsyncTask<Void, Integer, ImageQualityTestResult> {

        String result = "";

        protected void onPostExecute(ImageQualityTestResult testImageQualityResult) {
            if (null == testImageQualityResult) {
                LogUtils.loge("Image Quality Test QFP Failure NULL returned");
                updateView(mTestCase.getName(), "Image Quality Test "  + getResources().getString(R.string.fprint_null_fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return;
            }
            if (SUCCESS == testImageQualityResult.status) {
                // test passed
                result += "SUCCESS \n";
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            } else {
                // test failed
                LogUtils.loge("Image Quality Test QFP Failure " + testImageQualityResult.status);
                result += "Image Quality Test QFP Failure " + testImageQualityResult.status + "\n";
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
            // set test results
            result += "SNR:   " +
                    String.valueOf((double) testImageQualityResult.SNR1 / (double) 1000.0) + "\n";
            result += "Delta: " +
                    String.valueOf((double) testImageQualityResult.delta1 / (double) 1000.0) + "\n";
            try {
                getQtiExtendedFingerpintService().testParamSet(SNR_MEAN_LINE_REMOVAL,
                        ((mSnrMeanRemoval != null) ?
                                mSnrMeanRemoval :
                                Integer.toString(ENABLE)));
            } catch (Exception e) {
                LogUtils.loge("SNR_MEAN_LINE_REMOVAL Test Param Set failed " + e.getMessage());
            }
            updateView(mTestCase.getName(), result);
            mTestCase.addTestData("SNR", String.valueOf((double) testImageQualityResult.SNR1 / (double) 1000.0));
            mTestCase.addTestData("DELTA", String.valueOf((double) testImageQualityResult.delta1 / (double) 1000.0));

        }

        @Override
        protected ImageQualityTestResult doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().testImageQuality();
            } catch (Exception e) {
                LogUtils.loge("Exception in Image Quality Test: " + e.getMessage());
                return null;
            }
        }
    }

    private class PSFCalibration extends AsyncTask<Void, Integer, PsfCalibrationResult> {

        String result = "";

        protected void onPostExecute(PsfCalibrationResult psfCalibResult) {
            if (null == psfCalibResult) {
                LogUtils.loge("PSFCalibration QFP Failure NULL returned");
                updateView(mTestCase.getName(), "PSFCalibration "  + getResources().getString(R.string.fprint_null_fail));
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return;
            }
            if (SUCCESS == psfCalibResult.status) {
                // calibration succeeded
                result += "SUCCESS \n";
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            } else {
                // calibration failed
                LogUtils.loge("PSFCalibration QFP Failure " + psfCalibResult.status);
                result += "PSFCalibration QFP Failure " + psfCalibResult.status + "\n";
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
            // set test results
            double phase = (((double) psfCalibResult.phase * 2.0 * Math.PI) / 256.0);
            double delay = ((double) psfCalibResult.initialDelay / Math.pow(2, 14));
            double combinedPhase = (double) psfCalibResult.phaseCosinus / 10000;
            result += "Phase:          " + String.format("%.4f", phase, 4) + "\n";
            result += "Initial Delay:  " + String.format("%.4f", delay, 4) + "\n";
            result += "Combined Phase: " + String.format("%.4f", combinedPhase, 4) + "\n";
            updateView(mTestCase.getName(), result);
            mTestCase.addTestData("PHASE", String.format("%.4f", phase, 4));
            mTestCase.addTestData("INITIAL_DELAY", String.format("%.4f", delay, 4));
            mTestCase.addTestData("PHASE_COS", String.format("%.4f", combinedPhase, 4));

        }

        @Override
        protected PsfCalibrationResult doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().psfCalibraton();
            } catch (Exception e) {
                LogUtils.loge("Exception in PSF Calibration: " + e.getMessage());
                return null;
            }
        }
    }

    private class MfgFieldVal extends AsyncTask<Void, Void, FieldMfgFormat> {
        private short mfgFieldId = 100;
        ;

        @Override
        protected FieldMfgFormat doInBackground(Void... params) {
            try {
                return getQtiExtendedFingerpintService().getMfgValue(mfgFieldId);
            } catch (Exception e) {
                LogUtils.loge("Exception in get MFG value: " + e.getMessage());
                updateView(mTestCase.getName(), getResources().getString(R.string.fprint_fail_get) + " MFG value: " + e.getMessage());
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                return null;
            }
        }

        @Override
        protected void onPostExecute(FieldMfgFormat result) {
            if (result != null && result.status == SUCCESS) {
                LogUtils.logi("SensorID: " + result.mfgValue);
                updateView(mTestCase.getName(), "SensorID: " + result.mfgValue);
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                mTestCase.addTestData("SENSOR_ID", result.mfgValue);

            } else {
                LogUtils.loge("Failed to get Sensor id");
                updateView(mTestCase.getName(), getResources().getString(R.string.fprint_fail_get) + " SensorID");
                updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
            }
        }
    }
}
