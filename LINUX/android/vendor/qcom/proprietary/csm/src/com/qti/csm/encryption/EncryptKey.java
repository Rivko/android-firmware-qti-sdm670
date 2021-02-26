/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import com.qti.csm.encryption.system.ProcessInfo;
import com.qti.csm.encryption.system.SystemInfo;
import com.qti.csm.utils.UtilsConvert;
import com.qti.csm.utils.UtilsEncrypt;
import com.qti.csm.utils.UtilsFileOperation;

import java.io.File;

public class EncryptKey {

    public final static int SDCARD_INDEX = 1;
    public final static int INTERNAL_INDEX = 0;

    private static String generate16RandomKey(String head) {
        if (head == null)
            head = "";
        String key = head;
        key += UtilsEncrypt.getRandomString(16 - head.length());
        return key;
    }

    public static String loadKeyFromFile(int storage) {

        final String KEY_DIR;
        switch (storage) {
            case INTERNAL_INDEX:
                KEY_DIR = SystemInfo.INTERNAL_ENCRYPT_CONFIG;
                break;
            case SDCARD_INDEX:
                KEY_DIR = SystemInfo.SDCARD_ENCRYPT_CONFIG;
                break;
            default:
                return null;
        }

        File encryptConfigDir = new File(KEY_DIR);

        if (encryptConfigDir.canRead()) {
            File file, keyFile = null;
            File[] files = encryptConfigDir.listFiles();
            for (int i = 0; i < files.length; i++) {
                file = files[i];
                if (file.getName().endsWith(SystemInfo.ENCRYPT_KEY_FILE_POFIX)) {
                    keyFile = file;
                    break;
                }
            }

            if (keyFile != null) {
                String curKeyFileName = keyFile.getName();

                String encryptedPasswordHex = curKeyFileName.substring(0,
                        curKeyFileName.length()
                                - SystemInfo.ENCRYPT_KEY_FILE_POFIX.length());
                byte[] passwordBytes = UtilsEncrypt.decrypt(
                        UtilsConvert.hexToByte(encryptedPasswordHex),
                        SystemInfo.MAGIC);
                String passwordString = new String(passwordBytes);

                byte[] encryptedKeyBytes = UtilsFileOperation
                        .readFileBytes(keyFile.getPath());
                byte[] keyBytes = UtilsEncrypt.decrypt(encryptedKeyBytes,
                        passwordString);
                return new String(keyBytes);
            } else {
                return null;
            }
        }
        return null;
    }

    public static String loadPassword(int storage) {

        switch (storage) {
            case INTERNAL_INDEX:
                if (MainApp.getInstance().mInternalPassword == null) {
                    MainApp.getInstance().mInternalPassword = loadPasswordFromFile(storage);
                }
                return MainApp.getInstance().mInternalPassword;
            case SDCARD_INDEX:
                if (MainApp.getInstance().mSdcardPassword == null) {
                    MainApp.getInstance().mSdcardPassword = loadPasswordFromFile(storage);
                }
                return MainApp.getInstance().mSdcardPassword;
            default:
                return null;
        }
    }

    public static String loadPasswordFromFile(int storage) {

        final String KEY_DIR;
        switch (storage) {
            case INTERNAL_INDEX:
                KEY_DIR = SystemInfo.INTERNAL_ENCRYPT_CONFIG;
                break;
            case SDCARD_INDEX:
                KEY_DIR = SystemInfo.SDCARD_ENCRYPT_CONFIG;
                break;
            default:
                return null;
        }

        File encryptConfigDir = new File(KEY_DIR);

        if (encryptConfigDir.canRead()) {

            File file, keyFile = null;
            File[] files = encryptConfigDir.listFiles();
            for (int i = 0; i < files.length; i++) {
                file = files[i];
                if (file.getName().endsWith(SystemInfo.ENCRYPT_KEY_FILE_POFIX)) {
                    keyFile = file;
                    break;
                }
            }

            if (keyFile != null) {
                String curKeyFileName = keyFile.getName();

                String encryptedPasswordHex = curKeyFileName.substring(0,
                        curKeyFileName.length()
                                - SystemInfo.ENCRYPT_KEY_FILE_POFIX.length());
                byte[] passwordBytes = UtilsEncrypt.decrypt(
                        UtilsConvert.hexToByte(encryptedPasswordHex),
                        SystemInfo.MAGIC);
                return new String(passwordBytes);

            } else {
                return null;
            }
        }
        return null;
    }

    public static boolean writePasswordToFile(String password, int storage) {
        final String KEY_DIR;
        String key = null;
        switch (storage) {
            case INTERNAL_INDEX:
                KEY_DIR = SystemInfo.INTERNAL_ENCRYPT_CONFIG;
                break;
            case SDCARD_INDEX:
                KEY_DIR = SystemInfo.SDCARD_ENCRYPT_CONFIG;
                break;
            default:
                return false;
        }
        byte[] encryptedPasswordBytes = UtilsEncrypt.encrypt(
                password.getBytes(), SystemInfo.MAGIC);
        String encryptedPasswordString = UtilsConvert
                .byteToHex(encryptedPasswordBytes);
        String keyFileName = encryptedPasswordString 
                + SystemInfo.ENCRYPT_KEY_FILE_POFIX;
        String keyFilePath = KEY_DIR + "/" + keyFileName;

        File internalEncryptConfigDir = new File(KEY_DIR);
        internalEncryptConfigDir.mkdirs();

        if (internalEncryptConfigDir.canWrite()) {
            File file, curKeyFile = null;
            File[] files = internalEncryptConfigDir.listFiles();
            for (int i = 0; i < files.length; i++) {
                file = files[i];
                if (file.getName().endsWith(SystemInfo.ENCRYPT_KEY_FILE_POFIX)) {
                    curKeyFile = file;
                    break;
                }
            }

            if (curKeyFile != null) {
                String curKeyFileName = curKeyFile.getName();
                if (curKeyFileName.equals(keyFileName)) {
                    return true;
                }
                String encryptedPasswordHex = curKeyFileName.substring(0,
                        curKeyFileName.length()
                                - SystemInfo.ENCRYPT_KEY_FILE_POFIX.length());
                byte[] passwordBytes = UtilsEncrypt.decrypt(
                        UtilsConvert.hexToByte(encryptedPasswordHex),
                        SystemInfo.MAGIC);
                String passwordString = new String(passwordBytes);
                ProcessInfo.logd("passwordString=" + passwordString);

                if (key == null) {
                    byte[] encryptedKeyBytes = UtilsFileOperation
                            .readFileBytes(curKeyFile.getPath());
                    byte[] keyBytes = UtilsEncrypt.decrypt(encryptedKeyBytes,
                            passwordString);
                    key = new String(keyBytes);
                }
            } else {
                key = generate16RandomKey(SystemInfo.MAGIC);
            }

            byte[] encryptedKeyBytes = UtilsEncrypt.encrypt(key.getBytes(),
                    password);
            UtilsFileOperation.writeFile(keyFilePath, encryptedKeyBytes);
            if (curKeyFile != null)
                UtilsFileOperation.removeFile(curKeyFile);

            if (storage == INTERNAL_INDEX) {
                MainApp.getInstance().mInternalKey = key;
            } else
                MainApp.getInstance().mSdcardKey = key;
        } else
            return false;
        return true;
    }

}
