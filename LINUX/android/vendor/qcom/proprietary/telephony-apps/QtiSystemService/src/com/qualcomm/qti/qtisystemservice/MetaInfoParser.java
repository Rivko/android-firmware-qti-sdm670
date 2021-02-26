/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.json.JSONException;
import org.json.JSONObject;

public class MetaInfoParser {
    private static MetaInfoParser mInstance;
    private static final String FILENAME_META_VERSION = "/firmware/verinfo/ver_info.txt";

    private String mMetaInfo;

    private MetaInfoParser() {
        String metaId = "";
        try {
            String meta = readFile(FILENAME_META_VERSION);
            try {
                JSONObject metaInfo = new JSONObject(meta).getJSONObject("Metabuild_Info");;
                metaId = metaInfo.getString("Meta_Build_ID");
            } catch (JSONException e) {
                metaId = readLine(FILENAME_META_VERSION);
            }
        } catch (IOException ioe) {
            // Fail quietly, as the file may not exist on some devices.
        } catch (NumberFormatException nfe) {
            // Fail quietly, returning empty string should be sufficient
        }
        mMetaInfo = metaId;
    }

    public static MetaInfoParser getInstance() {
        if (mInstance == null) {
            mInstance = new MetaInfoParser();
        }
        return mInstance;
    }

    private String readFile(String filePath) throws IOException {
        InputStreamReader inputStreamReader = new InputStreamReader(
                new FileInputStream(filePath), "UTF-8");
        StringBuilder stringBuilder = new StringBuilder();
        BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
        String line;
        while((line = bufferedReader.readLine()) != null) {
            stringBuilder.append(line);
            stringBuilder.append('\n');
        }
        bufferedReader.close();
        return stringBuilder.toString();
    }

    private String readLine(String filename) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(filename),
                256);
        try {
            return reader.readLine();
        } finally {
            reader.close();
        }
    }

    public String getMetaInfo() {
        return mMetaInfo;
    }
}
