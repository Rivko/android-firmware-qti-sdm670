/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption.system;

import com.qti.csm.encryption.MainApp;

public class SystemInfo {

    public static final String version = "1.0";
    public static final String TAG = "encr";

    public static final String INTERNAL_STORAGE_REAL = "/data/media/0";
    public static final String SDCARD_REAL = "/mnt/media_rw/sdcard1";

    public static final String INTERNAL_STORAGE = MainApp.getPath();
    public static final String SDCARD_ROOT = MainApp.getPathSd();


    public static final String PROTECTED_PATH = ".secprotect";
    public static final String INTERNAL_PROTECTED_PATH = INTERNAL_STORAGE + "/"
            + PROTECTED_PATH;
    public static final String SDCARD_PROTECTED_PATH = SDCARD_ROOT + "/"
            + PROTECTED_PATH;

    public static final String ENCRYPT_CONFIG_PATH = ".encrypt_config";
    public static final String ENCRYPT_KEY_FILE_POFIX = ".key";
    public static final String ENCRYPT_FILE_SUFFIX = ".encrypted";
    public static final String INTERNAL_ENCRYPT_CONFIG = INTERNAL_PROTECTED_PATH
            + "/" + ENCRYPT_CONFIG_PATH;
    public static final String SDCARD_ENCRYPT_CONFIG = SDCARD_PROTECTED_PATH
            + "/" + ENCRYPT_CONFIG_PATH;
    public static final String ENC_CHECK = "/storage";
    public final static int MODE_ENCRYPT = 1;
    public final static int MODE_DECRYPT = 2;
    public final static int MODE_PROTECT = 3;
    public final static int MODE_DEPROTECT = 4;

    public static final String INTENT_CREATE_PASSWORD = "create";
    public static final String INTENT_CHANGE_PASSWORD = "change";
    public static final String MAGIC = "EncryptOption";
    public static final String PREF_PASSWORD = "password";
    public static final String PREF_ENCRYPTED_KEY = "encrypted_key";
    public static final String INTENT_LOGIN = "login";
}