/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.util.ArrayList;
import java.util.HashMap;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Spinner;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.CategoryData;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;

/**
 * Class for handling contact field list
 */
public class ContactFieldList extends FieldListHandler {
    //private static final String LOG_TAG = "ContactFieldList";

    private static class PhoneItemListener implements OnItemSelectedListener {
        private final FieldData field;
        private final PhoneImageAdapter phoneAdapter;

        private PhoneItemListener(FieldData field, PhoneImageAdapter phoneAdapter) {
            this.field = field;
            this.phoneAdapter = phoneAdapter;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            FieldType phoneType = phoneAdapter.getPhoneType(position);
            field.setType(phoneType);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onNothingSelected(AdapterView<?> parent) {
        }
    };

    /**
     * Constructor for the class
     *
     * @param context
     *            Application context
     * @param primaryView
     *            primary list view container
     * @param secondaryView
     *            secondary lost view container
     * @param categoryData
     *            previous category data
     * @param ocrDataList
     *            ocr data list
     */
    public ContactFieldList(Context context, ViewGroup primaryView, ViewGroup secondaryView,
            CategoryData categoryData, ArrayList<RecognizedData> ocrDataList) {
        super(context, primaryView, secondaryView);
        initializeCategoryTypeData(CategoryType.CONTACT);

        loadContactFields(categoryData, ocrDataList);
        constructFieldsList();
    }

    private void loadContactFields(CategoryData categoryData, ArrayList<RecognizedData> ocrDataList) {
        fieldOptions.add(noneText);
        int contactMobileCount = 0;

        // load previous data
        if (categoryData != null) {
            for (FieldData fieldData : categoryData.data) {
                if (FieldType.isPhone(fieldData.getType())) {
                    contactMobileCount++;
                }
                insertKnownField(fieldData);
            }
        }
        // look into sta data
        int categoryId = 0;
        if (categoryData != null) {
            categoryId = categoryData.dataId;
        }
        for (RecognizedData data : ocrDataList) {
            if (data.id > categoryId) {// load new data
                FieldType fieldType = FieldType.nativeTypeToContactFieldType(data.nativeType);

                if (fieldType == FieldType.CONTACT_PHONE_WORK) {
                    contactMobileCount++;
                    switch (contactMobileCount) {
                    case 1:
                        break;
                    case 2:
                        fieldType = FieldType.CONTACT_PHONE_MOBILE;
                        break;
                    case 3:
                        fieldType = FieldType.CONTACT_PHONE_HOME;
                        break;
                    case 4:
                        fieldType = FieldType.CONTACT_PHONE_FAX;
                        break;
                    default:
                        fieldType = FieldType.NONE;
                        break;
                    }
                }

                if (fieldType == FieldType.UNKNOWN) { // general text
                    insertUnknownField(data.getText());
                } else if (fieldType != FieldType.NONE) {
                    FieldData fieldData = new FieldData(fieldType, data.getText());
                    insertKnownField(fieldData);
                }
            }
            fieldOptions.add(data.getText());
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected FieldRow handleCategoryField(FieldData fieldData,
            HashMap<FieldType, FieldRow> fieldsHashMap, ViewGroup viewGroup) {
        FieldRow fieldRow = null;
        if (FieldType.isPhone(fieldData.getType())) {
            fieldRow = new FieldRow(fieldData);
            fieldRow.view = layoutInflater.inflate(R.layout.phone_field_item, viewGroup, false);
            setCaptionInRow(fieldRow.view, fieldRow.data);
            setPhoneSpinnerInRow(fieldRow.view, fieldRow.data);
            setTextInRow(fieldRow.view, fieldRow.data);
            setDeleteButtonInRow(fieldRow.view, fieldRow.data, fieldsHashMap, viewGroup);
        }
        return fieldRow;
    }

    private void setPhoneSpinnerInRow(View rowView, FieldData data) {// set spinner
        final PhoneImageAdapter phoneAdapter = new PhoneImageAdapter(context);
        Spinner spinnerView = (Spinner) rowView.findViewById(R.id.phone_spinner);
        spinnerView.setAdapter(phoneAdapter);
        spinnerView.setSelection(phoneAdapter.getPhonePosition(data.getType()));
        spinnerView.setOnItemSelectedListener(new PhoneItemListener(data, phoneAdapter));
    }

}
