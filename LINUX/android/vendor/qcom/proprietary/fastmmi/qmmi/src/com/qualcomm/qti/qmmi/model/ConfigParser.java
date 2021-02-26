/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import android.util.Xml;

import com.qualcomm.qti.qmmi.bean.PlatformInfo;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public final class ConfigParser {

    public static final String FILE_PCBA_DEFAULT_XML = "pcba_default.xml";
    public static final String FILE_ASSEMBLE_DEFAULT_XML = "assembly_default.xml";
    public static final String FILE_UI_DEFAULT_XML = "ui_default.xml";

    public static final String TYPE_UI = "ui";
    public static final String TYPE_PCBA = "pcba";
    public static final String TYPE_ASSEMBLY = "assembly";

    public enum TestMode {
        UI, PCBA, ASSEMBLY
    }

    /**
     * Read from file path
     */
    public static TestMode parseCaseConfigFile(String filePathAbs, List<TestCase> testCaseList, Map<String, TestCase> testCaseMap) {
        LogUtils.logi("start to parse config file:" + filePathAbs);
        try {
            XmlPullParser xmlPullParser = XmlPullParserFactory.newInstance().newPullParser();
            FileInputStream fis = new FileInputStream(filePathAbs);
            //set input
            xmlPullParser.setInput(fis, "utf-8");
            return parseCaseConfigFile(xmlPullParser, testCaseList, testCaseMap);
        } catch (FileNotFoundException e) {

        } catch (XmlPullParserException e) {
            e.printStackTrace();
        }

        return TestMode.UI;
    }

    /**
     * Read from resource
     */
    public static TestMode parseCaseConfigFile(XmlPullParser xmlPullParser, List<TestCase> testCaseList, Map<String, TestCase> testCaseMap) {
        TestCase curCase = null;
        TestMode mode = null;
        TestCase.Limit limit = null;
        try {
            int mEventType = xmlPullParser.getEventType();
            while (mEventType != XmlPullParser.END_DOCUMENT) {

                if (mEventType == XmlPullParser.START_TAG) {
                    String startTag = xmlPullParser.getName();
                    if (startTag.equals("FactoryTest")) {
                        String type = xmlPullParser
                                .getAttributeValue(null, "type");

                        if (type != null){
                            if (type.equalsIgnoreCase(TYPE_PCBA))
                                mode = TestMode.PCBA;
                            else if (type.equalsIgnoreCase(TYPE_ASSEMBLY))
                                mode = TestMode.ASSEMBLY;
                            else if (type.equalsIgnoreCase(TYPE_UI))
                                mode = TestMode.UI;
                        }

                    } else if (startTag.equals("TestCase")) {
                        curCase = new TestCase();
                        curCase.setGroup(xmlPullParser
                                .getAttributeValue(null, "group"));
                        curCase.setName(xmlPullParser
                                .getAttributeValue(null, "name"));
                        curCase.setMethod(xmlPullParser
                                .getAttributeValue(null, "method"));

                        if (xmlPullParser
                                .getAttributeValue(null, "auto").equalsIgnoreCase("true"))
                            curCase.setAuto(true);

                        if (xmlPullParser
                                .getAttributeValue(null, "enable").equalsIgnoreCase("true"))
                            curCase.setEnable(true);

                    } else if (startTag.equals("string")) {
                        String stringName = xmlPullParser.getAttributeValue(null, "name");
                        String stringValue = xmlPullParser.nextText();
                        curCase.getParameter().put(stringName, stringValue);
                    } else if (startTag.equals("limit")) {
                        String limitName = xmlPullParser.getAttributeValue(null, "name");
                        limit = new TestCase.Limit(limitName, Integer.valueOf(xmlPullParser.getAttributeValue(null, "min").trim()), Integer.valueOf(xmlPullParser.getAttributeValue(null, "max").trim()));
                        curCase.getLimits().put(limitName, limit);
                    }
                } else if (mEventType == XmlPullParser.END_TAG) {
                    String endTag = xmlPullParser.getName();
                    if (curCase != null && endTag.equals("TestCase")) {
                        /*let check the activity and service*/
                        Class<?> activity = Utils.getActivityClass(curCase.getGroup());
                        if (activity != null) {
                            curCase.setActivityClass(activity);
                        } else {
                            LogUtils.loge("Can not find activity for testcase: " + curCase.getName());
                        }

                        Class<?> service = Utils.getServiceClass(curCase.getGroup());
                        if (service != null) {
                            curCase.setServiceClass(service);
                        } else {
                            LogUtils.loge("Can not find service for testcase: " + curCase.getName());
                        }

                        /*save it*/
                        if (testCaseMap != null)
                            testCaseMap.put(curCase.getName(), curCase);

                        if (testCaseList != null)
                            testCaseList.add(curCase);
                    }
                }
                mEventType = xmlPullParser.next();
            }
        } catch (Exception e) {
            LogUtils.loge("startParseCaseConfig:" + e);
        }

        return mode;
    }

    public static void parsePlatformConfigFile(XmlPullParser xmlPullParser, Map<String, PlatformInfo> platformInfoMap) {
        LogUtils.logi("startParsePlatformFile");
        PlatformInfo curPlatformInfo = null;
        Map<String, String> variables = new HashMap<>();

        try {
            int mEventType = xmlPullParser.getEventType();
            while (mEventType != XmlPullParser.END_DOCUMENT) {
                if (mEventType == XmlPullParser.START_TAG) {
                    String startTag = xmlPullParser.getName();
                    if (startTag.equals("soc")) {
                        curPlatformInfo = new PlatformInfo();
                        curPlatformInfo.setName(xmlPullParser
                                .getAttributeValue(null, "name"));
                        curPlatformInfo.setSubTypeId(xmlPullParser
                                .getAttributeValue(null, "subtype_id"));
                    } else if (startTag.equals("variable")) {
                        String vName = xmlPullParser.getAttributeValue(null, "name").trim();
                        String vValue = xmlPullParser.nextText().trim();
                        variables.put(vName, vValue);
                    } else if (startTag.equals("ui_testcases")) {
                        parseCases(xmlPullParser.nextText(), variables, curPlatformInfo.getUiCases());
                    } else if (startTag.equals("pcba_testcases")) {
                        parseCases(xmlPullParser.nextText(), variables, curPlatformInfo.getPcbaCases());
                    } else if (startTag.equalsIgnoreCase("assemble_testcases")) {
                        parseCases(xmlPullParser.nextText(), variables, curPlatformInfo.getAssembleCases());
                    }
                } else if (mEventType == XmlPullParser.END_TAG) {
                    String endTag = xmlPullParser.getName();
                    if (curPlatformInfo != null && endTag.equals("soc")) {
                        platformInfoMap.put(curPlatformInfo.getName(), curPlatformInfo);
                        LogUtils.logi("Parsed Platform: " + curPlatformInfo.getName() + " UI:" + curPlatformInfo.getUiCases().toString() + ",PCBA:" + curPlatformInfo.getPcbaCases().toString());
                    }
                }
                mEventType = xmlPullParser.next();
            }
        } catch (Exception e) {
            LogUtils.loge("error:" + e);
        }
    }

    public static void parseCases(final String in, Map<String, String> variables, List<String> out) {
        String[] strs = in.split(",");
        for (String substr : strs) {
            LogUtils.logi("..." + substr + "," + substr.indexOf("$"));
            if (substr.indexOf("$") >= 0) {
                /**find the key variables*/
                int iStart = substr.indexOf("{");
                int iEnd = substr.indexOf("}");
                if (iStart < 0 || iEnd < 0) {
                    LogUtils.loge("Not valid format, skipped");
                    continue;
                }

                String key = substr.substring(iStart + 1, iEnd).trim();
                String testCases = variables.get(key);
                LogUtils.logi("find variable: " + key + "," + testCases);
                if (testCases != null) {
                    String[] cases = testCases.split(",");
                    for (String c : cases) {
                        out.add(c.trim());
                    }
                }

            } else {
                out.add(substr.trim());
            }
        }
    }

    /*
    *  Write config file
    * **/
    public static void generateConfig(String filePath, List<TestCase> data, TestMode mode) {

        if (data == null || filePath == null) {
            return;
        }

        LogUtils.logi( "writeResultFile,filename:" + filePath);
        try {

            FileOutputStream os = new FileOutputStream(filePath);
            XmlSerializer serializer = Xml.newSerializer();
            serializer.setOutput(os, "utf-8");
            serializer.setFeature("http://xmlpull.org/v1/doc/features.html#indent-output", true);
            serializer.startDocument("utf-8", true);

            serializer.startTag("", "FactoryTest");
            if (mode == TestMode.PCBA)
                serializer.attribute(null, "type", TYPE_PCBA);
            else if (mode == TestMode.ASSEMBLY)
                serializer.attribute(null, "type", TYPE_ASSEMBLY);
            else
                serializer.attribute(null, "type", TYPE_UI);

            for (int i = 0; i < data.size(); i++) {
                TestCase testCase = data.get(i);

                serializer.startTag("", "TestCase");
                LogUtils.logi("writeResultFile, TestCase , name:" + testCase.getName());
                serializer.attribute(null, "name", testCase.getName());
                serializer.attribute(null, "auto", testCase.getAuto() ? "true" : "false");
                serializer.attribute(null, "enable", testCase.getEnable() ? "true" : "false");
                serializer.attribute(null, "group", testCase.getGroup());
                if (testCase.getMethod() != null) {
                    serializer.attribute(null, "method", testCase.getMethod());
                }

                if (testCase.getParameter().size() > 0) {
                    Iterator<Map.Entry<String, String>> paramIter = testCase.getParameter().entrySet().iterator();
                    serializer.startTag("", "parameters");
                    while (paramIter.hasNext()) {
                        Map.Entry<String, String> entry = paramIter.next();
                        LogUtils.logi("write ressult file, data ,key:" + entry.getKey() + ",param,value:" + entry.getValue());
                        serializer.startTag("", "string");
                        serializer.attribute(null, "name", entry.getKey());
                        serializer.text(entry.getValue());
                        serializer.endTag("", "string");
                    }
                    serializer.endTag("", "parameters");
                }

                if (testCase.getLimits().size() > 0) {
                    Iterator<Map.Entry<String, TestCase.Limit>> limitIter = testCase.getLimits().entrySet().iterator();
                    serializer.startTag("", "limits");
                    while (limitIter.hasNext()) {
                        Map.Entry<String, TestCase.Limit> entry = limitIter.next();
                        LogUtils.logi("write ressult file, data ,key:" + entry.getKey() + ",param,value:" + entry.getValue());
                        TestCase.Limit limit = entry.getValue();
                        serializer.startTag("", "limit");
                        serializer.attribute(null, "name", limit.name);
                        serializer.attribute(null, "min", String.valueOf(limit.min));
                        serializer.attribute(null, "max", String.valueOf(limit.max));
                        serializer.endTag("", "limit");
                    }
                    serializer.endTag("", "limits");
                }

                serializer.endTag("", "TestCase");
            }

            serializer.endTag("", "FactoryTest");
            serializer.endDocument();
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
