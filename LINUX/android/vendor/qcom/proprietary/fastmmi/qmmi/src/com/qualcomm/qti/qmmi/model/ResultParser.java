/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import android.system.Os;
import android.util.Xml;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlSerializer;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public final class ResultParser {
    public static final String FILE_RESULT_UI_XML = "ui_default_res.xml";
    public static final String FILE_RESULT_PCBA_XML = "pcba_default_res.xml";
    public static final String FILE_RESULT_ASSEMBLY_XML = "assembly_default_res.xml";
    public static final String KEY_PASS = "pass";
    public static final String KEY_FAIL = "fail";
    public static final String KEY_UNKNOW = "unknow";

    public static void readResultFile(String filePathAbs, Map<String, TestCase> testCaseMap) {
        if (testCaseMap == null || filePathAbs == null) {
            return;
        }
        try {
            FileInputStream fis = new FileInputStream(filePathAbs);
            XmlPullParser xmlPullParser = Xml.newPullParser();
            xmlPullParser.setInput(fis, "UTF-8");

            TestCase curCase = null;
            int mEventType = xmlPullParser.getEventType();
            while (mEventType != XmlPullParser.END_DOCUMENT) {
                if (mEventType == XmlPullParser.START_TAG) {
                    String startTag = xmlPullParser.getName();
                    if (startTag.equals("TestCase")) {
                        String caseName = xmlPullParser.getAttributeValue(null, "name");
                        if (caseName != null) {
                            //check the test case existence
                            curCase = testCaseMap.get(caseName);
                            if (curCase != null) {
                                curCase.setDuration(Float.parseFloat(xmlPullParser.getAttributeValue(null, "duration_in_sec")));
                                curCase.setEndTime(new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").parse(xmlPullParser.getAttributeValue(null, "end_time")));
                                curCase.setLimitCheck(Boolean.parseBoolean(xmlPullParser.getAttributeValue(null, "limit_checked")));

                                int result = TestCase.STATE_NONE;
                                String resultString = xmlPullParser.getAttributeValue(null, "result");
                                if (resultString.equals(KEY_PASS)) {
                                    result = TestCase.STATE_PASS;
                                } else if (resultString.equals(KEY_FAIL)) {
                                    result = TestCase.STATE_FAIL;
                                } else if (resultString.equals(KEY_UNKNOW)) {
                                    result = TestCase.STATE_UNKNOW;
                                }
                                curCase.setResult(result);
                                curCase.setStartTime(new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").parse(xmlPullParser.getAttributeValue(null, "start_time")));
                                curCase.setToken(xmlPullParser.getAttributeValue(null, "token"));
                            }
                        }

                    } else if (startTag.equals("string")) {
                        if (curCase != null) {
                            String stringName = xmlPullParser.getAttributeValue(null, "name");
                            String stringValue = xmlPullParser.nextText();
                            curCase.getParameter().put(stringName, stringValue);
                        }
                    } else if (startTag.equals("Data")) {
                        if (curCase != null) {
                            String stringName = xmlPullParser.getAttributeValue(null, "name");
                            String stringValue = xmlPullParser.nextText();
                            curCase.addTestData(stringName, stringValue);
                        }
                    }
                } else if (mEventType == XmlPullParser.END_TAG) {
                    String endTag = xmlPullParser.getName();
                    if (curCase != null && endTag.equals("TestCase")) {
                        LogUtils.logi("End of readResultFile TestCase: " + curCase.getName());
                    }
                }
                mEventType = xmlPullParser.next();
            }
            //close the istream
            fis.close();
        } catch (Exception e) {
            LogUtils.loge("readResultFile:" + e);
        }
        return;
    }

    public static void saveResultToFile(String filePath, List<TestCase> data) {
        if (data == null || filePath == null) {
            return;
        }

        LogUtils.logi("writeResultFile,filename:" + filePath);
        FileOutputStream os = null;
        try {

            os = new FileOutputStream(filePath);
            XmlSerializer serializer = Xml.newSerializer();
            serializer.setOutput(os, "utf-8");
            serializer.setFeature("http://xmlpull.org/v1/doc/features.html#indent-output", true);
            serializer.startDocument("utf-8", true);

            serializer.startTag("", "Result");

            for (int i = 0; i < data.size(); i++) {
                TestCase testCase = data.get(i);
                //skip the test not tested
                if (testCase.getResult() != TestCase.STATE_NONE) {
                    serializer.startTag("", "TestCase");
                    LogUtils.logi("writeResultFile, TestCase , name:" + testCase.getName());
                    serializer.attribute(null, "name", testCase.getName());
                    serializer.attribute(null, "duration_in_sec", String.valueOf(testCase.getDuration()));
                    serializer.attribute(null, "end_time", new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").format(testCase.getEndTime()));
                    serializer.attribute(null, "limit_checked", String.valueOf(testCase.getLimitCheck()));
                    if (testCase.getResult() == TestCase.STATE_PASS)
                        serializer.attribute(null, "result", KEY_PASS);
                    else if (testCase.getResult() == TestCase.STATE_FAIL)
                        serializer.attribute(null, "result", KEY_FAIL);
                    else
                        serializer.attribute(null, "result", KEY_UNKNOW);

                    serializer.attribute(null, "start_time", new SimpleDateFormat("yyyy-MM-dd hh:mm:ss").format(testCase.getStartTime()));
                    serializer.attribute(null, "token", testCase.getToken() == null ? "11111" : testCase.getToken());

                    LogUtils.logi("writeResultFile, TestCase , data size:" + testCase.getTestData().size());
                    if (testCase.getTestData().size() > 0) {
                        Iterator<Map.Entry<String, String>> it = testCase.getTestData().entrySet().iterator();
                        serializer.startTag("", "DataSet");
                        while (it.hasNext()) {
                            Map.Entry<String, String> entry = it.next();
                            LogUtils.logi("write ressult file, data ,key:" + entry.getKey() + ",param,value:" + entry.getValue());
                            serializer.startTag("", "Data");
                            serializer.attribute(null, "name", entry.getKey());
                            serializer.text(entry.getValue() == null ? "null" : entry.getValue());
                            serializer.endTag("", "Data");
                        }
                        serializer.endTag("", "DataSet");
                    }
                    serializer.endTag("", "TestCase");
                }

            }
            serializer.endTag("", "Result");
            serializer.endDocument();
            serializer.flush();
        } catch (Exception e) {
            e.printStackTrace();
        } finally{
            if (os != null){
                try {
                    os.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
    }
}
