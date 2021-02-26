/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.Utils.CategoryLevel;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.settings.PreferenceData;

/**
 * Field list handler for showing and updating contact or event field list
 */
public abstract class FieldListHandler {
    private static final String LOG_TAG = "FieldListHandler";

    private static class FieldClickListener implements OnItemClickListener {

        private final FieldData field;
        private final FieldTextView fieldTextView;
        private final String noneText;

        private FieldClickListener(FieldData field, FieldTextView fieldTextView, String noneText) {
            this.field = field;
            this.fieldTextView = fieldTextView;
            this.noneText = noneText;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            String selectedText = (String) parent.getItemAtPosition(position);
            if (selectedText.equals(noneText)) {
                fieldTextView.setText("");
                selectedText = "";
            }
            field.setValue(selectedText);
        }
    }

    /**
     * class for representing field row data
     */
    protected static class FieldRow {
        protected final FieldData data;
        protected View view = null;

        /**
         * Constructor for the class
         *
         * @param type
         *            field type
         * @param value
         *            field value
         */
        protected FieldRow(FieldType type, String value) {
            this(new FieldData(type, value));
        }

        /**
         * Constructor for the class
         *
         * @param data
         *            Field data
         */
        protected FieldRow(FieldData data) {
            this.data = data;
        }
    }

    protected HashMap<FieldType, FieldRow> primaryFieldMap = new HashMap<FieldType, FieldRow>();
    protected ArrayList<FieldType> sortedPrimaryFieldTypes = new ArrayList<FieldType>();
    protected final ViewGroup primaryView;

    private HashMap<FieldType, FieldRow> secondaryFieldMap = new HashMap<FieldType, FieldRow>();
    private ArrayList<FieldType> sortedSecondaryFieldTypes = new ArrayList<FieldType>();
    private final ViewGroup secondaryView;

    private HashMap<FieldType, FieldRow> extraFieldMap = new HashMap<FieldType, FieldRow>();
    private ArrayList<FieldType> sortedExtraFieldTypes = new ArrayList<FieldType>();

    protected ArrayList<String> fieldOptions = new ArrayList<String>();
    private ArrayAdapter<String> fieldOptionAdapter = null;
    protected final LayoutInflater layoutInflater;

    protected final Context context;
    protected final String noneText;
    private final String appLocale;
    private final String enLocale;

    /**
     * Constructor for the class
     *
     * @param context
     *            Application context
     * @param primaryView
     *            primary list view container
     * @param secondaryView
     *            secondary lost view container
     */
    protected FieldListHandler(Context context, ViewGroup primaryView, ViewGroup secondaryView) {
        this.context = context;
        this.primaryView = primaryView;
        this.secondaryView = secondaryView;

        noneText = context.getResources().getString(R.string.dropdown_none_option);
        appLocale = PreferenceData.getOCRLocale(context);
        enLocale = context.getResources().getString(R.string.setting_sta_locale_usa);

        layoutInflater = LayoutInflater.from(context);
    }

    /**
     * Method to construct the fields
     */
    protected void constructFieldsList() {
        addEmptyPrimaryFields();

        fieldOptionAdapter = new ArrayAdapter<String>(context, R.layout.field_dropdown_item,
                fieldOptions);
        for (FieldType fieldType : sortedPrimaryFieldTypes) {
            FieldRow fieldRow = primaryFieldMap.get(fieldType);
            if (fieldRow != null) {
                attachViewToList(fieldRow.view, fieldRow.data, primaryView, -1);
            }
        }

        int secondaryIndex = 0;
        for (FieldType fieldType : sortedSecondaryFieldTypes) {
            FieldRow fieldRow = secondaryFieldMap.get(fieldType);
            if (fieldRow != null) {
                attachViewToList(fieldRow.view, fieldRow.data, secondaryView, secondaryIndex);
                secondaryIndex++;
            }
        }

        for (FieldType fieldType : extraFieldMap.keySet()) {
            FieldRow fieldRow = extraFieldMap.get(fieldType);
            if (fieldRow != null) {
                attachViewToList(fieldRow.view, fieldRow.data, secondaryView, secondaryIndex);
                secondaryIndex++;
            }
        }
    }

    private void attachViewToList(View view, final FieldData data, ViewGroup parentView, int index) {
        FieldTextView fieldTextView = (FieldTextView) view.findViewById(R.id.field_value);
        if (fieldTextView != null) {
            fieldTextView.setAdapter(fieldOptionAdapter);
        }

        if (index >= 0) {
            parentView.addView(view, index);
        } else {
            parentView.addView(view);
        }
    }

    private void addEmptyPrimaryFields() {
        for (FieldType fieldType : sortedPrimaryFieldTypes) {
            FieldRow fieldRow = primaryFieldMap.get(fieldType);
            if (fieldRow == null) {
                FieldData fieldData = new FieldData(fieldType, "");
                addNewField(primaryFieldMap, fieldData, primaryView);
            }
        }
    }

    /**
     * Method to insert the unknown field
     *
     * @param text
     *            text to be considered for fields
     */
    protected void insertUnknownField(String text) {
        for (FieldType fieldTypeKey : sortedPrimaryFieldTypes) {
            if (primaryFieldMap.get(fieldTypeKey) == null
                    && FieldType.isGeneralText(fieldTypeKey.getNativeType())) {
                FieldData fieldData = new FieldData(fieldTypeKey, text);
                addNewField(primaryFieldMap, fieldData, primaryView);
                return;
            }
        }
        for (FieldType fieldTypeKey : sortedSecondaryFieldTypes) {
            if (secondaryFieldMap.get(fieldTypeKey) == null
                    && FieldType.isGeneralText(fieldTypeKey.getNativeType())) {
                FieldData fieldData = new FieldData(fieldTypeKey, text);
                addNewField(secondaryFieldMap, fieldData, secondaryView);
                return;
            }
        }
    }

    /**
     * insert the text for known field
     *
     * @param fieldType
     *            field type
     * @param text
     *            text to be considered for field
     */
    protected void insertKnownField(FieldData fieldData) {
        for (FieldType fieldTypeKey : sortedPrimaryFieldTypes) {
            if (fieldData.getType() == fieldTypeKey && primaryFieldMap.get(fieldTypeKey) == null) {
                addNewField(primaryFieldMap, fieldData, primaryView);
                return;
            }
        }
        for (FieldType fieldTypeKey : sortedSecondaryFieldTypes) {
            if (fieldData.getType() == fieldTypeKey && secondaryFieldMap.get(fieldTypeKey) == null) {
                addNewField(secondaryFieldMap, fieldData, secondaryView);
                return;
            }
        }
        for (FieldType fieldTypeKey : sortedExtraFieldTypes) {
            if (fieldData.getType() == fieldTypeKey && extraFieldMap.get(fieldTypeKey) == null) {
                addNewField(extraFieldMap, fieldData, secondaryView);
                return;
            }
        }
    }

    /**
     * initialize the category type data
     *
     * @param categoryType
     *            category type
     */
    protected void initializeCategoryTypeData(CategoryType categoryType) {
        ArrayList<FieldType> fieldTypesArray = new ArrayList<FieldType>();
        FieldType.getFieldTypes(fieldTypesArray, categoryType, CategoryLevel.PRIMARY);
        for (FieldType fieldType : fieldTypesArray) {
            primaryFieldMap.put(fieldType, null);
            sortedPrimaryFieldTypes.add(fieldType);
        }
        fieldTypesArray.clear();

        FieldType.getFieldTypes(fieldTypesArray, categoryType, CategoryLevel.SECONDARY);
        for (FieldType fieldType : fieldTypesArray) {
            secondaryFieldMap.put(fieldType, null);
            sortedSecondaryFieldTypes.add(fieldType);
        }
        fieldTypesArray.clear();

        FieldType.getFieldTypes(fieldTypesArray, categoryType, CategoryLevel.NONE);
        for (FieldType fieldType : fieldTypesArray) {
            extraFieldMap.put(fieldType, null);
            sortedExtraFieldTypes.add(fieldType);
        }

        sortArrayBasedOnPriority(sortedPrimaryFieldTypes);
        sortArrayBasedOnPriority(sortedSecondaryFieldTypes);
        sortArrayBasedOnPriority(sortedExtraFieldTypes);
    }

    private void sortArrayBasedOnPriority(ArrayList<FieldType> fieldTypeArray) {
        Collections.sort(fieldTypeArray, new Comparator<FieldType>() {
            /**
             * {@inheritDoc}
             */
            @Override
            public int compare(FieldType lhs, FieldType rhs) {
                if (lhs.getPriority() < rhs.getPriority()) {
                    return -1;
                } else if (lhs.getPriority() > rhs.getPriority()) {
                    return 1;
                }
                return 0;
            }
        });
    }

    private FieldRow addNewField(final HashMap<FieldType, FieldRow> fieldsHashMap,
            final FieldData fieldData, ViewGroup viewGroup) {
        FieldRow fieldRow = handleCategoryField(fieldData, fieldsHashMap, viewGroup);
        if (fieldRow == null) {
            fieldRow = new FieldRow(fieldData);
            fieldRow.view = layoutInflater.inflate(R.layout.field_item, viewGroup, false);
            setCaptionInRow(fieldRow.view, fieldRow.data);
            setTextInRow(fieldRow.view, fieldRow.data);
            setDeleteButtonInRow(fieldRow.view, fieldRow.data, fieldsHashMap, viewGroup);
        }
        fieldsHashMap.put(fieldData.getType(), fieldRow);
        return fieldRow;
    }

    /**
     * Adds the delete button to the row
     *
     * @param rowView
     *            view to which delete button needs to be added
     * @param data
     *            data associated with row
     * @param fieldsHashMap
     *            parent hash map
     * @param viewGroup
     *            view group to which row needs to be added
     */
    protected void setDeleteButtonInRow(final View rowView, final FieldData data,
            final HashMap<FieldType, FieldRow> fieldsHashMap, final ViewGroup viewGroup) {
        View deleteOption = rowView.findViewById(R.id.delete_field);
        if (fieldsHashMap == primaryFieldMap) {
            deleteOption.setVisibility(View.GONE);
        } else {
            deleteOption.setOnClickListener(new OnClickListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void onClick(View v) {
                    fieldsHashMap.put(data.getType(), null);
                    viewGroup.removeView(rowView);
                }
            });
        }
    }

    /**
     * Sets the caption for the field row
     *
     * @param rowView
     *            field view
     * @param data
     *            field data
     */
    protected void setCaptionInRow(View rowView, FieldData data) {// set caption
        TextView captionView = (TextView) rowView.findViewById(R.id.field_caption);
        String caption = context.getResources().getString(data.getType().getCaptionId()) + ":";
        captionView.setText(caption);
    }

    /**
     * Sets the text in the field row
     *
     * @param rowView
     *            field view
     * @param data
     *            field data
     */
    protected void setTextInRow(View rowView, FieldData data) {
        String text = data.getText();
        if (text != null) {// set text
            if (FieldType.isPhone(data.getType()) && appLocale.equals(enLocale)) {
                text = Utils.toUsaFormattedPhone(text);
            }
            FieldTextView fieldTextView = (FieldTextView) rowView.findViewById(R.id.field_value);
            fieldTextView.setThreshold(1);
            fieldTextView.setText(text);
            fieldTextView.setOnItemClickListener(new FieldClickListener(data, fieldTextView,
                    noneText));
        }
    }

    /**
     * gets the array list of FieldTypes which are not there in the list
     *
     * @return array list of the FieldTypes
     */
    public ArrayList<FieldType> getAvailableFields() {
        ArrayList<FieldType> fieldNames = new ArrayList<FieldType>();
        for (FieldType fieldType : sortedPrimaryFieldTypes) {
            FieldRow fieldRow = primaryFieldMap.get(fieldType);
            if (fieldRow == null) {
                fieldNames.add(fieldType);
            }
        }

        for (FieldType fieldType : sortedSecondaryFieldTypes) {
            FieldRow fieldRow = secondaryFieldMap.get(fieldType);
            if (fieldRow == null) {
                fieldNames.add(fieldType);
            }
        }

        for (FieldType fieldType : extraFieldMap.keySet()) {
            FieldRow fieldRow = extraFieldMap.get(fieldType);
            if (fieldRow == null) {
                fieldNames.add(fieldType);
            }
        }
        return fieldNames;
    }

    /**
     * Adds a field to the list
     *
     * @param fieldType
     *            Field Type which needs to be added
     */
    public void addField(FieldType fieldType) {
        if (primaryFieldMap.containsKey(fieldType)) {
            attachViewToPrimarySecondary(fieldType, primaryFieldMap, sortedPrimaryFieldTypes,
                    primaryView);

        } else if (secondaryFieldMap.containsKey(fieldType)) {
            attachViewToPrimarySecondary(fieldType, secondaryFieldMap, sortedSecondaryFieldTypes,
                    secondaryView);

        } else {
            attachExtraListField(fieldType);
        }
    }

    private void attachExtraListField(FieldType fieldType) {
        FieldData fieldData = new FieldData(fieldType, "");
        FieldRow fieldRow = addNewField(extraFieldMap, fieldData, secondaryView);
        int position = sortedExtraFieldTypes.indexOf(fieldType);
        int checkTill = position;
        for (int i = 0; i < checkTill; i++) {
            FieldType type = sortedExtraFieldTypes.get(i);
            if (extraFieldMap.get(type) == null) {
                position--;
            }
        }

        int secondaryFields = 0;
        for (FieldType secondaryFieldType : sortedSecondaryFieldTypes) {
            FieldRow secondaryFieldRow = secondaryFieldMap.get(secondaryFieldType);
            if (secondaryFieldRow != null) {
                secondaryFields++;
            }
        }

        attachViewToList(fieldRow.view, fieldRow.data, secondaryView, secondaryFields + position);
    }

    private void attachViewToPrimarySecondary(FieldType fieldType,
            HashMap<FieldType, FieldRow> fieldMap, ArrayList<FieldType> sortedFieldTypes,
            ViewGroup viewGroup) {
        FieldData fieldData = new FieldData(fieldType, "");
        FieldRow fieldRow = addNewField(fieldMap, fieldData, viewGroup);
        int position = sortedFieldTypes.indexOf(fieldType);
        int checkTill = position;
        for (int i = 0; i < checkTill; i++) {
            FieldType type = sortedFieldTypes.get(i);
            if (fieldMap.get(type) == null) {
                position--;
            }
        }
        attachViewToList(fieldRow.view, fieldRow.data, viewGroup, position);
    }

    /**
     * Gets the filled fields from the list
     *
     * @return filled fields in array list
     */
    public ArrayList<FieldData> getFilledFields() {
        ArrayList<FieldData> filledFields = new ArrayList<FieldData>();
        fillFieldsFrom(filledFields, sortedPrimaryFieldTypes, primaryFieldMap);
        fillFieldsFrom(filledFields, sortedSecondaryFieldTypes, secondaryFieldMap);
        fillFieldsFrom(filledFields, sortedExtraFieldTypes, extraFieldMap);
        return filledFields;
    }

    private static boolean isNullOrEmpty(String str) {
        return (str == null || str.isEmpty());
    }

    private void fillFieldsFrom(ArrayList<FieldData> outArray,
            ArrayList<FieldType> sortedFieldTypes, HashMap<FieldType, FieldRow> fieldMap) {
        for (FieldType fieldType : sortedFieldTypes) {
            FieldRow fieldRow = fieldMap.get(fieldType);
            if (fieldRow != null) {
                String dataText = fieldRow.data.getText();
                String viewText = null;
                if (fieldRow.view != null) {
                    FieldTextView textView = (FieldTextView) fieldRow.view
                            .findViewById(R.id.field_value);
                    if (textView != null) {
                        viewText = textView.getText().toString();
                    }
                }

                if (!isNullOrEmpty(viewText)) {
                    if (isNullOrEmpty(dataText) || !viewText.equals(dataText)) {
                        Log.d(LOG_TAG, "Text changed: " + dataText + "=>" + viewText);
                        dataText = viewText;
                        fieldRow.data.setValue(dataText);
                    }
                }

                if (dataText != null && !dataText.isEmpty()) {
                    outArray.add(fieldRow.data);
                }
            }
        }
    }

    /**
     * Clear the already added views
     */
    public void clearViews() {
        for (FieldType fieldType : sortedPrimaryFieldTypes) {
            FieldRow fieldRow = primaryFieldMap.get(fieldType);
            if (fieldRow != null && fieldRow.view != null) {
                primaryView.removeView(fieldRow.view);
            }
        }

        for (FieldType fieldType : sortedSecondaryFieldTypes) {
            FieldRow fieldRow = secondaryFieldMap.get(fieldType);
            if (fieldRow != null && fieldRow.view != null) {
                secondaryView.removeView(fieldRow.view);
            }
        }

        for (FieldType fieldType : extraFieldMap.keySet()) {
            FieldRow fieldRow = extraFieldMap.get(fieldType);
            if (fieldRow != null && fieldRow.view != null) {
                secondaryView.removeView(fieldRow.view);
            }
        }
    }

    /**
     * handles the category specific fields
     *
     * @param fieldType
     *            field type
     * @param text
     *            field text
     * @param fieldsHashMap
     *            hash map container
     * @param viewGroup
     *            view group to show the field
     * @return field row if handled null otherwise
     */
    protected abstract FieldRow handleCategoryField(FieldData fieldData,
            HashMap<FieldType, FieldRow> fieldsHashMap, ViewGroup viewGroup);
}
