/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Map;
import java.util.HashMap;
import java.util.Random;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.RemoteException;
import android.text.format.Time;
import android.util.Base64;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.BindFidoCryptoButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.SetChallengeButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.BindAuthButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.BindFidoSuiButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.ProcessButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.ShowMessageSuiButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.CancelSuiButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.UnBindFidoCryptoButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.UnBindFidoSuiButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.VerifyUserButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.GetEnrollmentStatusButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.persistence.PersistenceManager;
import com.qualcomm.qti.auth.fidocryptosample.samples.AbstractSampleCase;
import com.qualcomm.qti.auth.fidosuiservice.IFidoSuiController;

public class FidoCryptoClientActivity extends Activity {
  public static final String LOG_TAG = FidoCryptoClientActivity.class
      .getSimpleName();
  public static final String LOG_TAG_PREF = "FIDOSampleApp-";

  public static final int GET_INFO_ITEM_POSITION = 0;
  public static final int REGISTER_ITEM_POSITION = 1;
  public static final int DEREGISTER_ITEM_POSITION = 2;
  public static final int OPEN_SETTINGS_ITEM_POSITION = 3;
  public static final int MALFORMED_REQUEST_ITEM_POSITION = 4;
  public static final int SIGN_ITEM_POSITION = 5;
  public static final int SIGN_TX_ITEM_POSITION = 6;

  private static final String FIDOCRYPTOSERVICE_PACKAGE = "com.qualcomm.qti.auth.fidocryptoservice";
  private static final String FIDOSUISERVICE_CLASS_NAME = "com.qualcomm.qti.auth.fidosuiservice.FidoSuiService";
  private static final String FIDOSUISERVICE_PACKAGE_NAME = "com.qualcomm.qti.auth.fidosuiservice";
  private static final String HEALTHAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.healthauthservice";
  private static final String MOCKAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.mockauthservice";
  private static final String SAMPLEAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.sampleauthenticatorservice";
  private static final String SAMPLEEXTAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.sampleextauthservice";
  private static final String SECSAMPLEAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.securesampleauthservice";
  private static final String SECUREEXTAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.secureextauthservice";

  public FidoCryptoConnection fidoCryptoConnection;
  public FidoSuiConnection fidoSuiConnection;
  public HealthAuthConnection haConnection;
  public MaConnection maConnection;
  public SAConnection saConnection;
  public SEAConnection seaConnection;
  public SecExtConnection secExtConnection;
  public SSAConnection ssaConnection;
  public SSA2Connection ssa2Connection;

  public FidoCryptoListenerImpl iFidoCryptoListener;
  public FidoSuiListenerImpl iFidoSuiListener;
  public IMockAuthListenerImpl iMockAuthListener;
  public SampleAuthenticatorListenerImpl iSampleAuthenticatorListener;
  public SecureSampleAuthListenerImpl iSecureSampleAuthListener;
  public SecureSampleAuth2ListenerImpl iSecureSampleAuth2Listener;
  public SampleExtAuthListenerImpl iSampleExtAuthListener;
  public SecureExtAuthListenerImpl iSecureExtAuthListener;
  public HealthAuthListenerImpl iHealthAuthListener;

  public IFidoSuiController fidoSuiController;

  public String authenticatorName;
  public byte[] encapsulatedResult;
  public byte[] extensionReturned;
  public RadioGroup radioGroup;
  public Spinner spinner;
  public Spinner fidoOpSpinner;
  public FileOutputStream outputStream;
  public PersistenceManager persistenceManager;
  public boolean useExtensions;
  public boolean usePNGContent;
  public boolean isFingerprint;
  public byte[] transactionContent = AbstractSampleCase.TRANSACTION_CONTENT_STR.getBytes();

  public Button verifyUserButton;
  public Button cancelSuiButton;

  public static byte[] finalChallenge;

  public static void newChallenge() {
    Random r = new Random();
    finalChallenge = new byte[3];
    r.nextBytes(finalChallenge);
  }

  static {
    newChallenge();
  }

  public void bindFidoCryptoService() throws ServiceAlreadyBoundException {
    if (this.fidoCryptoConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(FIDOCRYPTOSERVICE_PACKAGE,
        FIDOCRYPTOSERVICE_PACKAGE + ".FidoCryptoService");
    bindService(bindIntent, this.fidoCryptoConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindFidoSuiService() throws ServiceAlreadyBoundException {
    if (this.fidoSuiConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(FIDOSUISERVICE_PACKAGE_NAME,
        FIDOSUISERVICE_CLASS_NAME);
    bindService(bindIntent, this.fidoSuiConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindMAService() throws ServiceAlreadyBoundException {
    if (this.maConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(MOCKAUTHSERVICE_PACKAGE, MOCKAUTHSERVICE_PACKAGE
        + ".MockAuthService");
    bindService(bindIntent, this.maConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindSAService() throws ServiceAlreadyBoundException {
    if (this.saConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SAMPLEAUTHSERVICE_PACKAGE,
        SAMPLEAUTHSERVICE_PACKAGE + ".SampleAuthenticatorService");
    bindService(bindIntent, this.saConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindSSAService() throws ServiceAlreadyBoundException {
    if (this.ssaConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SECSAMPLEAUTHSERVICE_PACKAGE,
        SECSAMPLEAUTHSERVICE_PACKAGE + ".SecureSampleAuthService");
    bindService(bindIntent, this.ssaConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindSSA2Service() throws ServiceAlreadyBoundException {
    if (this.ssa2Connection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SECSAMPLEAUTHSERVICE_PACKAGE,
        SECSAMPLEAUTHSERVICE_PACKAGE + ".SecureSampleAuth2Service");
    bindService(bindIntent, this.ssa2Connection, Context.BIND_AUTO_CREATE);
  }

  public void bindSEAService() throws ServiceAlreadyBoundException {
    if (this.seaConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SAMPLEEXTAUTHSERVICE_PACKAGE,
        SAMPLEEXTAUTHSERVICE_PACKAGE + ".SampleExtAuthService");
    bindService(bindIntent, this.seaConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindSecExtService() throws ServiceAlreadyBoundException {
    if (this.secExtConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SECUREEXTAUTHSERVICE_PACKAGE,
        SECUREEXTAUTHSERVICE_PACKAGE + ".SecureExtAuthService");
    bindService(bindIntent, this.secExtConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindHealthAuthService() throws ServiceAlreadyBoundException {
    if (this.haConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(HEALTHAUTHSERVICE_PACKAGE,
        HEALTHAUTHSERVICE_PACKAGE + ".HealthAuthService");
    bindService(bindIntent, this.haConnection, Context.BIND_AUTO_CREATE);
  }

  @Override
  public void onCreate(Bundle savedInstanceState) {
    Log.d(LOG_TAG, "onCreate+ sample app version 0.1");
    Thread.currentThread().setName(
        FidoCryptoClientActivity.class.getSimpleName());
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);

    fidoCryptoConnection = new FidoCryptoConnection(this);
    fidoSuiConnection = new FidoSuiConnection(this);
    maConnection = new MaConnection(this);
    saConnection = new SAConnection(this);
    ssaConnection = new SSAConnection(this);
    ssa2Connection = new SSA2Connection(this);
    seaConnection = new SEAConnection(this);
    secExtConnection = new SecExtConnection(this);
    haConnection = new HealthAuthConnection(this);

    this.iFidoCryptoListener = new FidoCryptoListenerImpl(this);
    this.iFidoSuiListener = new FidoSuiListenerImpl(this);
    this.iMockAuthListener = new IMockAuthListenerImpl(this);
    this.iSampleAuthenticatorListener = new SampleAuthenticatorListenerImpl(
        this);
    this.iSecureSampleAuthListener = new SecureSampleAuthListenerImpl(this);
    this.iSampleExtAuthListener = new SampleExtAuthListenerImpl(this);
    this.iSecureExtAuthListener = new SecureExtAuthListenerImpl(this);
    this.iHealthAuthListener = new HealthAuthListenerImpl(this);

    Button processButton = (Button) this.findViewById(R.id.processButton);
    ProcessButtonOnClickListener processButtonOnClickListener = new ProcessButtonOnClickListener(
        this);
    processButton.setOnClickListener(processButtonOnClickListener);

    Button bindFidoButton = (Button) this.findViewById(R.id.bindFidoButton);
    BindFidoCryptoButtonOnClickListener bindFidoButtonOnClickListener = new BindFidoCryptoButtonOnClickListener(
        this);
    bindFidoButton.setOnClickListener(bindFidoButtonOnClickListener);

    Button bindFidoSuiButton = (Button) this
        .findViewById(R.id.bindFidoSuiButton);
    BindFidoSuiButtonOnClickListener bindFidoSuiButtonOnClickListener = new BindFidoSuiButtonOnClickListener(
        this);
    bindFidoSuiButton.setOnClickListener(bindFidoSuiButtonOnClickListener);

    Button unbindFidoButton = (Button) this.findViewById(R.id.unbindFidoButton);
    UnBindFidoCryptoButtonOnClickListener unbindFidoCryptoButtonOnClickListener = new UnBindFidoCryptoButtonOnClickListener(
        this);
    unbindFidoButton.setOnClickListener(unbindFidoCryptoButtonOnClickListener);

    Button setChallengeButton = (Button) this.findViewById(R.id.setChallengeButton);
    SetChallengeButtonOnClickListener setChallengeButtonOnClickListener =
      new SetChallengeButtonOnClickListener(this);
    setChallengeButton.setOnClickListener(setChallengeButtonOnClickListener);

    Button unbindFidoSuiButton = (Button) this
        .findViewById(R.id.unbindFidoSuiButton);
    UnBindFidoSuiButtonOnClickListener unbindFidoSuiButtonOnClickListener = new UnBindFidoSuiButtonOnClickListener(
        this);
    unbindFidoSuiButton.setOnClickListener(unbindFidoSuiButtonOnClickListener);

    Button bindAuthButton = (Button) this.findViewById(R.id.bindAuthButton);
    BindAuthButtonOnClickListener bindAuthButtonOnClickListener = new BindAuthButtonOnClickListener(
        this);
    bindAuthButton.setOnClickListener(bindAuthButtonOnClickListener);

    verifyUserButton = (Button) this.findViewById(R.id.verifyUserButton);
    VerifyUserButtonOnClickListener verifyUserButtonOnClickListener = new VerifyUserButtonOnClickListener(
        this);
    verifyUserButton.setOnClickListener(verifyUserButtonOnClickListener);

    Button getEnrollmentStatusButton = (Button) this
        .findViewById(R.id.getEnrollmentStatusButton);
    GetEnrollmentStatusButtonOnClickListener getEnrollmentStatusOnClickListener = new GetEnrollmentStatusButtonOnClickListener(
        this);
    getEnrollmentStatusButton
        .setOnClickListener(getEnrollmentStatusOnClickListener);

    fidoOpSpinner = (Spinner) findViewById(R.id.fido_operation_spinner);
    ArrayAdapter<CharSequence> fidoOpAdapter = ArrayAdapter.createFromResource(
        this, R.array.fido_op_array, android.R.layout.simple_spinner_item);
    fidoOpAdapter
        .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    fidoOpSpinner.setAdapter(fidoOpAdapter);

    spinner = (Spinner) findViewById(R.id.authenticator_spinner);
    ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
        R.array.authenticator_array, android.R.layout.simple_spinner_item);
    adapter
        .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    spinner.setAdapter(adapter);

    Button showMessageSuiButton = (Button) findViewById(R.id.showMessageSuiButton);
    OnClickListener showMessageSuiButtonOnClickListener = new ShowMessageSuiButtonOnClickListener(
        this);
    showMessageSuiButton
        .setOnClickListener(showMessageSuiButtonOnClickListener);

    cancelSuiButton = (Button) findViewById(R.id.cancelSuiButton);
    OnClickListener cancelSuiButtonOnClickListener = new CancelSuiButtonOnClickListener(
        this);
    cancelSuiButton.setOnClickListener(cancelSuiButtonOnClickListener);

    persistenceManager = new PersistenceManager(getBaseContext());
    try {
      outputStream = getApplicationContext().openFileOutput("logfile",
          Context.MODE_PRIVATE);
    } catch (Exception e) {
      e.printStackTrace();
    }
    Log.d(LOG_TAG, "onCreate-");
  }

  public void setText(String string) {
    TextView textView = (TextView) this.findViewById(R.id.messageTextView);
    textView.setText(string);
  }

  public void showResponse(byte[] uafResponse) {
    if (uafResponse != null) {
      StringBuilder stringBuilder = new StringBuilder();
      for (int i = 0; i < 4 && i < uafResponse.length; i++) {
        stringBuilder.append(String.format("0x%02X ", uafResponse[i]));
      }
      this.setText(stringBuilder.toString());
    } else {
      this.setText("N/A");
    }
  }

  public void showExtension(byte[] uafResponse) {
    TextView textView = (TextView) this.findViewById(R.id.extTextView);
    if (uafResponse != null) {
      StringBuilder stringBuilder = new StringBuilder();
      for (int i = 0; i < 4 && i < uafResponse.length; i++) {
        stringBuilder.append(String.format("0x%02X ", uafResponse[i]));
      }
      textView.setText(stringBuilder.toString());
    } else {
      textView.setText("N/A");
    }
  }

  public int processEx(byte[] command, byte[] cookie, Map exParams)
      throws RemoteException {
    try {
      Time now = new Time(Time.getCurrentTimezone());
      now.setToNow();
      String timeString = now.format("\n\n%T: request: ");
      outputStream.write(timeString.getBytes());
      outputStream.write(Base64.encodeToString(command, Base64.DEFAULT)
          .getBytes());
      outputStream.write("\n".getBytes());
    } catch (IOException e) {
    }
    return this.fidoCryptoConnection.processEx(command, AbstractSampleCase.APP_ID_STR, cookie,
        iFidoCryptoListener, exParams);
  }

  public int setChallenge() throws RemoteException {
    return this.fidoCryptoConnection.setChallenge(finalChallenge,
        iFidoCryptoListener);
  }

  @Override
  public void onDestroy() {
    unbindFidoCryptoService();
    unbindFidoSuiService();
    if (maConnection.isBound()) {
      unbindService(maConnection);
      maConnection.unbound();
    }
    if (saConnection.isBound()) {
      unbindService(saConnection);
      saConnection.unbound();
    }
    if (ssaConnection.isBound()) {
      unbindService(ssaConnection);
      ssaConnection.unbound();
    }
    if (ssa2Connection.isBound()) {
      unbindService(ssa2Connection);
      ssa2Connection.unbound();
    }
    if (seaConnection.isBound()) {
      unbindService(seaConnection);
      seaConnection.unbound();
    }
    if (secExtConnection.isBound()) {
      unbindService(secExtConnection);
      secExtConnection.unbound();
    }
    if (haConnection.isBound()) {
      unbindService(haConnection);
      haConnection.unbound();
    }
    try {
      if (outputStream != null) {
        outputStream.close();
      }
    } catch (IOException e) {
      Log.e(LOG_TAG, "", e);
    }
    super.onDestroy();
  }

  public void unbindFidoCryptoService() {
    if (fidoCryptoConnection.isBound()) {
      fidoCryptoConnection.unbind();
      unbindService(fidoCryptoConnection);
    }
  }

  public void unbindFidoSuiService() {
    if (fidoSuiConnection.isBound()) {
      fidoSuiConnection.unbind();
      unbindService(fidoSuiConnection);
    }
  }

  public void showSuiResponse(final int code) {
    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        TextView textView = (TextView) findViewById(R.id.showMessageSuiResultText);
        textView.setText("response code = " + code);
      }
    });
  }

  public void onCheckboxClicked(View view) {
    useExtensions = ((CheckBox) view).isChecked();
  }

  public void onPngCheckboxClicked(View view) {
    usePNGContent = ((CheckBox) view).isChecked();
    if(usePNGContent) {
      File pngFile = new File(AbstractSampleCase.TRANSACTION_CONTENT_PNG);
      int pngSize = (int) pngFile.length();
      transactionContent = new byte[pngSize];
      try {
        BufferedInputStream buf = new BufferedInputStream(new FileInputStream(pngFile));
        buf.read(transactionContent, 0, transactionContent.length);
        buf.close();
      } catch (FileNotFoundException e) {
        Log.e(LOG_TAG, "PNG doesn't exist @ location "+AbstractSampleCase.TRANSACTION_CONTENT_PNG);
        Toast.makeText(view.getContext(), "No PNG File",
              Toast.LENGTH_SHORT).show();
        ((CheckBox) view).setChecked(false);
      } catch (IOException e) {
        Log.d(LOG_TAG, "Could not read PNG");
        Toast.makeText(view.getContext(), "Unable to read PNG",
              Toast.LENGTH_SHORT).show();
        ((CheckBox) view).setChecked(false);
      }
    } else {
      transactionContent = AbstractSampleCase.TRANSACTION_CONTENT_STR.getBytes();
    }
  }

  public void onFPCheckboxClicked(View view) {
    isFingerprint = ((CheckBox) view).isChecked();
  }

  public boolean onKeyDown(int keyCode, KeyEvent event) {
    if((keyCode == KeyEvent.KEYCODE_VOLUME_UP) && isFingerprint
          && this.spinner.getSelectedItemPosition() == 2) {
      Toast.makeText(this.getApplicationContext(), "Fingerprint!",
              Toast.LENGTH_SHORT).show();
      verifyUserButton.callOnClick();
    }
    return true;
  }
}
