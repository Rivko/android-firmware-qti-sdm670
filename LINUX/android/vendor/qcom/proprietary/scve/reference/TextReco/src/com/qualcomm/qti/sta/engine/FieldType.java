/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.engine;

import java.util.ArrayList;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.OCRIntentData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils.CategoryLevel;
import com.qualcomm.qti.sta.data.Utils.CategoryType;

/**
 * Enum for the fields to be used for contact and event
 */
public enum FieldType {
    NONE, UNKNOWN,

    // contact enums
    CONTACT_NAME(CategoryType.CONTACT, CategoryLevel.PRIMARY, 0, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_NAME, R.string.contact_name_field_caption),
    CONTACT_TITLE(CategoryType.CONTACT, CategoryLevel.PRIMARY, 1, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_TITLE, R.string.contact_title_field_caption),
    CONTACT_COMPANY(CategoryType.CONTACT, CategoryLevel.PRIMARY, 2, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_COMPANY, R.string.contact_company_field_caption),

    CONTACT_PHONE_WORK(CategoryType.CONTACT, CategoryLevel.SECONDARY, 0, JniConstants.DATA_TYPE_PHONENUMBER, OCRIntentData.KEY_CONTACT_PHONE_WORK, R.string.contact_phone_work_field_caption, R.string.contact_phone_work_field_name),
    CONTACT_PHONE_MOBILE(CategoryType.CONTACT, CategoryLevel.SECONDARY, 1, JniConstants.DATA_TYPE_PHONENUMBER, OCRIntentData.KEY_CONTACT_PHONE_MOBILE, R.string.contact_phone_mobile_field_caption, R.string.contact_phone_mobile_field_name),
    CONTACT_PHONE_HOME(CategoryType.CONTACT, CategoryLevel.SECONDARY, 2, JniConstants.DATA_TYPE_PHONENUMBER, OCRIntentData.KEY_CONTACT_PHONE_HOME, R.string.contact_phone_home_field_caption, R.string.contact_phone_home_field_name),
    CONTACT_PHONE_FAX(CategoryType.CONTACT, CategoryLevel.SECONDARY, 3, JniConstants.DATA_TYPE_PHONENUMBER, OCRIntentData.KEY_CONTACT_PHONE_FAX, R.string.contact_phone_fax_field_caption, R.string.contact_phone_fax_field_name),
    CONTACT_ADDRESS(CategoryType.CONTACT, CategoryLevel.SECONDARY, 4, JniConstants.DATA_TYPE_ADDRESS, OCRIntentData.KEY_CONTACT_ADDRESS, R.string.contact_address_field_caption, R.string.contact_address_field_name),
    CONTACT_EMAIL(CategoryType.CONTACT, CategoryLevel.SECONDARY, 5, JniConstants.DATA_TYPE_EMAIL, OCRIntentData.KEY_CONTACT_EMAIL, R.string.contact_email_field_caption, R.string.contact_email_field_name),
    CONTACT_WEBSITE(CategoryType.CONTACT, CategoryLevel.SECONDARY, 6, JniConstants.DATA_TYPE_URL, OCRIntentData.KEY_CONTACT_WEBSITE, R.string.contact_website_field_caption, R.string.contact_website_field_name),

    CONTACT_NICKNAME(CategoryType.CONTACT, CategoryLevel.NONE, 0, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_NICKNAME, R.string.contact_nickname_field_caption, R.string.contact_nickname_field_name),
    CONTACT_IM(CategoryType.CONTACT, CategoryLevel.NONE, 1, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_IM, R.string.contact_im_field_caption, 0),
    CONTACT_RELATIONSHIP(CategoryType.CONTACT, CategoryLevel.NONE, 2, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_RELATION, R.string.contact_relationship_field_caption, R.string.contact_relationship_field_name),
    CONTACT_NOTES(CategoryType.CONTACT, CategoryLevel.NONE, 3, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CONTACT_NOTES, R.string.contact_notes_field_caption, R.string.contact_notes_field_name),

    // event types
    EVENT_TITLE(CategoryType.EVENT, CategoryLevel.PRIMARY, 0, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CALENDAR_EVENT_TITLE, R.string.event_title_field_caption),
    EVENT_START_DATE(CategoryType.EVENT, CategoryLevel.PRIMARY, 1, JniConstants.DATA_TYPE_DATETIME, OCRIntentData.KEY_CALENDAR_EVENT_START_DATE, R.string.event_from_date_field_caption),
    EVENT_END_DATE(CategoryType.EVENT, CategoryLevel.PRIMARY, 2, JniConstants.DATA_TYPE_DATETIME, OCRIntentData.KEY_CALENDAR_EVENT_END_DATE, R.string.event_to_date_field_caption),
    EVENT_ALL_DAY(CategoryType.EVENT, CategoryLevel.PRIMARY, 3, JniConstants.DATA_TYPE_NONE, OCRIntentData.KEY_CALENDAR_EVENT_ALL_DAY, R.string.event_all_day_field_caption),
    EVENT_REPEAT(CategoryType.EVENT, CategoryLevel.PRIMARY, 4, JniConstants.DATA_TYPE_NONE, OCRIntentData.KEY_CALENDAR_EVENT_REPEAT, R.string.event_repeat_field_caption),

    EVENT_LOCATION(CategoryType.EVENT, CategoryLevel.NONE, 0, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CALENDAR_EVENT_LOCATION, R.string.event_location_field_caption, R.string.event_location_field_name),
    EVENT_DETAILS(CategoryType.EVENT, CategoryLevel.NONE, 1, JniConstants.DATA_TYPE_GENERAL, OCRIntentData.KEY_CALENDAR_EVENT_DETAILS, R.string.event_details_field_caption, R.string.event_details_field_name);

    private final CategoryType dataCategory;
    private final CategoryLevel categoryType;
    private final int titleId;
    private final int descriptionId;
    private final int nativeType;// native scve type
    private final String intentKey;

    private final int priority;

    private FieldType() {
        this(CategoryType.NONE, CategoryLevel.NONE, -1, 0, null, JniConstants.DATA_TYPE_NONE, 0);
    }

    private FieldType(CategoryType category, CategoryLevel categoryType, int priority,
            int nativeType, String intentName, int titleId) {
        this(category, categoryType, priority, nativeType, intentName, titleId, 0);
    }

    private FieldType(CategoryType category, CategoryLevel categoryType, int priority,
            int nativeType, String intentName, int titleId, int descriptionId) {
        dataCategory = category;
        this.categoryType = categoryType;
        this.priority = priority;

        this.nativeType = nativeType;
        this.intentKey = intentName;
        this.titleId = titleId;

        this.descriptionId = descriptionId;
    }

    /**
     * Gets the description ID
     *
     * @return description resource id
     */
    public int getNameId() {
        return descriptionId;
    }

    /**
     * get the field name resource id
     *
     * @return name resource id
     */
    public int getCaptionId() {
        return titleId;
    }

    /**
     * Gets the category type
     *
     * @return category type
     */
    public CategoryLevel getCategoryType() {
        return categoryType;
    }

    /**
     * gets the display priority
     *
     * @return display priority
     */
    public int getPriority() {
        return priority;
    }

    /**
     * gets the native OCR type
     *
     * @return native OCR type
     */
    public int getNativeType() {
        return nativeType;
    }

    /**
     * Gets the intent name
     *
     * @return intent name
     */
    public String getIntentKey() {
        return intentKey;
    }

    /**
     * gets the list of all field types based on category, category type
     *
     * @param fieldsTypes
     *            out parameter for field types
     * @param dataCategory
     *            data category
     * @param categoryType
     *            category type
     */
    public static void getFieldTypes(ArrayList<FieldType> fieldsTypes, CategoryType dataCategory,
            CategoryLevel categoryType) {
        if (fieldsTypes != null) {
            for (FieldType fieldType : FieldType.values()) {
                if (dataCategory == fieldType.dataCategory
                        && categoryType == fieldType.categoryType) {
                    fieldsTypes.add(fieldType);
                }
            }
        }
    }

    /**
     * Converts native field type to Event field type
     *
     * @param type
     *            native type
     * @return event field type
     */
    public static FieldType nativeTypeToEventFieldType(int type) {
        FieldType fieldType = FieldType.UNKNOWN;
        switch (type) {
        case JniConstants.DATA_TYPE_ADDRESS:
            fieldType = FieldType.EVENT_LOCATION;
            break;
        case JniConstants.DATA_TYPE_DATETIME:
            fieldType = FieldType.EVENT_START_DATE;
            break;
        default: // general text
            break;
        }
        return fieldType;
    }

    /**
     * Converts native field type to contact field type
     *
     * @param type
     *            native type
     * @return contact field type
     */
    public static FieldType nativeTypeToContactFieldType(int type) {
        FieldType fieldType = FieldType.UNKNOWN;
        switch (type) {
        case JniConstants.DATA_TYPE_ADDRESS:
            fieldType = FieldType.CONTACT_ADDRESS;
            break;
        case JniConstants.DATA_TYPE_EMAIL:
            fieldType = FieldType.CONTACT_EMAIL;
            break;
        case JniConstants.DATA_TYPE_PHONENUMBER:
            fieldType = FieldType.CONTACT_PHONE_WORK;
            break;
        case JniConstants.DATA_TYPE_URL:
            fieldType = FieldType.CONTACT_WEBSITE;
            break;
        default: // General text
            break;

        }
        return fieldType;
    }

    /**
     * gets the action text id from the native type
     *
     * @param type
     *            native type
     * @return text id
     */
    public static int getActionImageIdFromNativeType(int type) {
        int textId = 0;
        switch (type) {
        case JniConstants.DATA_TYPE_ADDRESS:
            textId = R.drawable.ic_action_location;
            break;
        case JniConstants.DATA_TYPE_EMAIL:
            textId = R.drawable.ic_action_email;
            break;
        case JniConstants.DATA_TYPE_PHONENUMBER:
            textId = R.drawable.ic_action_call;
            break;
        case JniConstants.DATA_TYPE_URL:
            textId = R.drawable.ic_action_web;
            break;
        case JniConstants.DATA_TYPE_DATETIME:
            textId = R.drawable.ic_action_date;
            break;
        default:
            textId = R.drawable.ic_action_translate;
            break;
        }
        return textId;
    }

    /**
     * checks if the mandatory fields for contact are present
     *
     * @param dataList
     *            data list of parsed out texts
     * @return true if mandatory fields are present, false otherwise
     */
    public static boolean areContactFieldsPresent(ArrayList<RecognizedData> dataList) {
        int fields = JniConstants.DATA_TYPE_PHONENUMBER | JniConstants.DATA_TYPE_EMAIL| JniConstants.DATA_TYPE_URL | JniConstants.DATA_TYPE_ADDRESS;
        return areFieldsPresent(fields, dataList);
    }
    /**
     * checks if the mandatory fields for event are present
     *
     * @param dataList
     *            data list of parsed out texts
     * @return true if mandatory fields are present, false otherwise
     */
    public static boolean areEventFieldsPresent(ArrayList<RecognizedData> dataList) {
        int fields = JniConstants.DATA_TYPE_DATETIME;
        return areFieldsPresent(fields, dataList);
    }
    private static boolean areFieldsPresent(int fields, ArrayList<RecognizedData> dataList) {
        for(RecognizedData data: dataList) {
            if( (data.nativeType&fields) == data.nativeType) {
                return true;
            }
        }
        return false;
    }

    /**
     * check if the native type is general text
     *
     * @param nativeType
     *            native type
     * @return true if general text, false otherwise
     */
    public static boolean isGeneralText(int nativeType) {
        return (nativeType == JniConstants.DATA_TYPE_GENERAL);
    }

    /**
     * check if the native type is phone text
     *
     * @param nativeType
     *            native type
     * @return true if phone text, false otherwise
     */
    public static boolean isPhoneText(int nativeType) {
        return (nativeType == JniConstants.DATA_TYPE_PHONENUMBER);
    }

    /**
     * check if the native type is URL text
     *
     * @param nativeType
     *            native type
     * @return true if URL text, false otherwise
     */
    public static boolean isURLText(int nativeType) {
        return (nativeType == JniConstants.DATA_TYPE_URL);
    }

    /**
     * check if the native type is EMAIL text
     *
     * @param nativeType
     *            native type
     * @return true if EMAIL text, false otherwise
     */
    public static boolean isEmailText(int nativeType) {
        return (nativeType == JniConstants.DATA_TYPE_EMAIL);
    }

    /**
     * check if the native type is Address text
     *
     * @param nativeType
     *            native type
     * @return true if Address text, false otherwise
     */
    public static boolean isAddressText(int nativeType) {
        return (nativeType == JniConstants.DATA_TYPE_ADDRESS);
    }

    /**
     * check if the native type is date text
     *
     * @param nativeType
     *            native type
     * @return true if date text, false otherwise
     */
    public static boolean isDateText(int nativeType) {
        return (nativeType == JniConstants.DATA_TYPE_DATETIME);
    }
    /**
     * check if field type is phone
     *
     * @param fieldType
     *           field type
     * @return true if it is phone, false otherwise
     */
    public static boolean isPhone(FieldType fieldType) {
        return (fieldType == FieldType.CONTACT_PHONE_FAX
                || fieldType == FieldType.CONTACT_PHONE_HOME
                || fieldType == FieldType.CONTACT_PHONE_MOBILE || fieldType == FieldType.CONTACT_PHONE_WORK);
    }
}
