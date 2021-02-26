/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.persistence;

import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;

import android.content.Context;
import android.util.Log;

public class PersistenceManager {

  private static final String KEY_HANDLE_FILE_NAME = "keyHandle.serializedObject";
  private static final String LOG_TAG = PersistenceManager.class.getName();
  private final Context context;

  public PersistenceManager(Context context) {
    this.context = context;
  }

  public void writeKeyHandleToFile(byte[] keyHandle)
      throws UnableToWriteKeyHandleException {
    Log.d(LOG_TAG, "writeKeyHandleToFile(" + Arrays.toString(keyHandle) + ")");
    Log.d(
        LOG_TAG,
        "keyhandle file path: "
            + context.getFileStreamPath(KEY_HANDLE_FILE_NAME));
    if (keyHandle == null) {
      throw new UnableToWriteKeyHandleException();
    }
    try {
      FileOutputStream fileOutputStream = context.openFileOutput(
          KEY_HANDLE_FILE_NAME, Context.MODE_PRIVATE);
      ObjectOutputStream objectOutputStream = new ObjectOutputStream(
          fileOutputStream);
      objectOutputStream.writeObject(keyHandle);
      objectOutputStream.close();
      fileOutputStream.close();
    } catch (IOException e) {
      e.printStackTrace();
      throw new UnableToWriteKeyHandleException();
    }
    Log.d(LOG_TAG, "writeKeyHandleToFile-");
  }

  public byte[] readKeyHandleFromFile() throws UnableToReadKeyHandleException {
    Log.d(LOG_TAG, "readKeyHandleFromFile()");
    Log.d(
        LOG_TAG,
        "keyhandle file path: "
            + context.getFileStreamPath(KEY_HANDLE_FILE_NAME));
    try {
      FileInputStream fileInputStream = context
          .openFileInput(KEY_HANDLE_FILE_NAME);
      ObjectInputStream objectInputStream = new ObjectInputStream(
          fileInputStream);
      byte[] keyHandle = (byte[]) objectInputStream.readObject();
      objectInputStream.close();
      fileInputStream.close();
      Log.d(LOG_TAG, "readKeyHandleFromFile-");
      return keyHandle;
    } catch (IOException | ClassNotFoundException e) {
      e.printStackTrace();
      throw new UnableToReadKeyHandleException();
    }
  }

}
