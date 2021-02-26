/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

/**
 * class for Intent data
 */
public class OCRIntentData {

    public static final String ACTION_CAMERA = "qualcomm.qti.intent.action.OCRCamera";
    public static final String ACTION_GALLERY = "qualcomm.qti.intent.action.OCRGallery";

    public static final String KEY_TYPE = "type";
    public static final String VALUE_TYPE_CAMERA = "camera";
    public static final String VALUE_TYPE_CONTACT = "contact";
    public static final String VALUE_TYPE_CALENDAR = "calendar";
    public static final String VALUE_TYPE_GALLERY = "gallery";

    public static final String KEY_LOCALE = "locale";
    public static final String VALUE_LOCALE_USA = "USA";
    public static final String VALUE_LOCALE_CHINA = "CHN";
    public static final String VALUE_LOCALE_KOREA = "KOR";

    public static final String KEY_IMAGE = "image_path";

    public static final String KEY_CONTACT_NAME = "name";
    public static final String KEY_CONTACT_NICKNAME = "nickname";
    public static final String KEY_CONTACT_TITLE = "title";
    public static final String KEY_CONTACT_COMPANY = "company";
    public static final String KEY_CONTACT_PHONE_WORK = "work_phone";
    public static final String KEY_CONTACT_PHONE_MOBILE = "mobile_phone";
    public static final String KEY_CONTACT_PHONE_HOME = "home_phone";
    public static final String KEY_CONTACT_PHONE_FAX = "fax_phone";
    public static final String KEY_CONTACT_ADDRESS = "address";
    public static final String KEY_CONTACT_EMAIL = "email";
    public static final String KEY_CONTACT_WEBSITE = "website";
    public static final String KEY_CONTACT_IM = "im";
    public static final String KEY_CONTACT_RELATION = "relation";
    public static final String KEY_CONTACT_NOTES = "notes";

    public static final String KEY_CALENDAR_EVENT_TITLE = "title";
    public static final String KEY_CALENDAR_EVENT_START_DATE = "start_time_mills";
    public static final String KEY_CALENDAR_EVENT_END_DATE = "end_time_millis";
    public static final String KEY_CALENDAR_EVENT_LOCATION = "location";
    public static final String KEY_CALENDAR_EVENT_DETAILS = "details";
    public static final String KEY_CALENDAR_EVENT_ALL_DAY = "all_day";
    public static final String KEY_CALENDAR_EVENT_REPEAT = "repeat";
}
