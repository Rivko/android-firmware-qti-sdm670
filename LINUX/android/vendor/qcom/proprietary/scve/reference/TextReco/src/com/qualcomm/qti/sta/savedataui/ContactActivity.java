/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.util.ArrayList;

import android.content.ContentProviderOperation;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds;
import android.provider.ContactsContract.Data;
import android.provider.ContactsContract.RawContacts;
import android.view.ViewGroup;
import android.widget.Toast;

import com.qualcomm.qti.sta.data.CategoryData;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.R;

/**
 * Class for add contact activity
 */
public class ContactActivity extends SaveDataActivity {
    //private static final String LOG_TAG = "ContactActivity";

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_contact_event);

        initializeBaseData(CategoryType.CONTACT);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onSavePressed(ArrayList<FieldData> fieldDataList) {
        String title = "";
        String company = "";
        ArrayList<ContentProviderOperation> operations = new ArrayList<ContentProviderOperation>();
        int rawContactInsertIndex = operations.size();

        // insert a new raw contact
        operations.add(ContentProviderOperation.newInsert(RawContacts.CONTENT_URI)
                .withValue(RawContacts.ACCOUNT_TYPE, null)
                .withValue(RawContacts.ACCOUNT_NAME, null).build());

        for (FieldData fieldData : fieldDataList) {
            switch (fieldData.getType()) {
            case CONTACT_NAME:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.StructuredName.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.StructuredName.DISPLAY_NAME,
                                fieldData.getText()).build());
                break;
            case CONTACT_TITLE:
                title = fieldData.getText();
                break;
            case CONTACT_COMPANY:
                company = fieldData.getText();
                break;

            case CONTACT_PHONE_WORK:
                operations.add(ContentProviderOperation
                        .newInsert(Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Phone.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Phone.NUMBER, fieldData.getText())
                        .withValue(ContactsContract.CommonDataKinds.Phone.TYPE,
                                ContactsContract.CommonDataKinds.Phone.TYPE_WORK).build());
                break;
            case CONTACT_PHONE_MOBILE:
                operations.add(ContentProviderOperation
                        .newInsert(Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Phone.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Phone.NUMBER, fieldData.getText())
                        .withValue(ContactsContract.CommonDataKinds.Phone.TYPE,
                                ContactsContract.CommonDataKinds.Phone.TYPE_MOBILE).build());
                break;
            case CONTACT_PHONE_HOME:
                operations.add(ContentProviderOperation
                        .newInsert(Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Phone.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Phone.NUMBER, fieldData.getText())
                        .withValue(ContactsContract.CommonDataKinds.Phone.TYPE,
                                ContactsContract.CommonDataKinds.Phone.TYPE_HOME).build());
                break;
            case CONTACT_PHONE_FAX:
                operations.add(ContentProviderOperation
                        .newInsert(Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Phone.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Phone.NUMBER, fieldData.getText())
                        .withValue(ContactsContract.CommonDataKinds.Phone.TYPE,
                                ContactsContract.CommonDataKinds.Phone.TYPE_FAX_WORK).build());
                break;
            case CONTACT_ADDRESS:
                break;
            case CONTACT_EMAIL:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Email.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Email.DATA, fieldData.getText()).build());
                break;
            case CONTACT_WEBSITE:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Website.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Website.URL, fieldData.getText()).build());
                break;

            case CONTACT_NICKNAME:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Nickname.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Nickname.NAME, fieldData.getText()).build());
                break;
            case CONTACT_IM:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Im.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Im.DISPLAY_NAME, fieldData.getText()).build());
                break;
            case CONTACT_RELATIONSHIP:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Relation.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Relation.NAME, fieldData.getText()).build());
                break;
            case CONTACT_NOTES:
                operations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(Data.RAW_CONTACT_ID, rawContactInsertIndex)
                        .withValue(Data.MIMETYPE, CommonDataKinds.Note.CONTENT_ITEM_TYPE)
                        .withValue(CommonDataKinds.Note.NOTE, fieldData.getText()).build());
                break;
            default:
                break;
            }
        }
        if (!title.isEmpty() || !company.isEmpty()) {
            operations.add(ContentProviderOperation
                    .newInsert(ContactsContract.Data.CONTENT_URI)
                    .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
                    .withValue(ContactsContract.Data.MIMETYPE,
                            ContactsContract.CommonDataKinds.Organization.CONTENT_ITEM_TYPE)
                    .withValue(ContactsContract.CommonDataKinds.Organization.COMPANY, company)
                    .withValue(ContactsContract.CommonDataKinds.Organization.TYPE,
                            ContactsContract.CommonDataKinds.Organization.TYPE_WORK)
                    .withValue(ContactsContract.CommonDataKinds.Organization.TITLE, title)
                    .withValue(ContactsContract.CommonDataKinds.Organization.TYPE,
                            ContactsContract.CommonDataKinds.Organization.TYPE_WORK).build());
        }
        try {
            getContentResolver().applyBatch(ContactsContract.AUTHORITY, operations);
            Toast.makeText(this, R.string.message_contact_saved, Toast.LENGTH_SHORT).show();
            exiting = true;
            new Handler().postDelayed(exitRunnable, INSTRUCTION_READ_TIME);

        } catch (Exception e) {
            e.printStackTrace();
            Toast.makeText(this, "Exception: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }
    /**
     * {@inheritDoc}
     */
    @Override
    protected void onDonePressed(ArrayList<FieldData> fieldDataList) {
        Intent resultIntent = new Intent();
        for (FieldData fieldData : fieldDataList) {
            FieldType type = fieldData.getType();
            resultIntent.putExtra(type.getIntentKey(), fieldData.getText());
        }
        setResult(RESULT_OK, resultIntent);
        finish();
    }
    /**
     * {@inheritDoc}
     */
    @Override
    protected FieldListHandler createListhandler(ViewGroup primaryView, ViewGroup secondaryView,
            CategoryData categoryData, ArrayList<RecognizedData> ocrDataList) {
        return new ContactFieldList(this, primaryView, secondaryView, categoryData, ocrDataList);
    }
}
