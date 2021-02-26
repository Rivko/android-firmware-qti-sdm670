/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

//import org.apache.http.protocol.HTTP;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.DateTimeData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.resultui.ActionEditText.ActionEditTextListener;
import com.qualcomm.qti.sta.settings.PreferenceData;

/**
 * class for handling floating action list
 */
public class ActionListHandler implements View.OnCreateContextMenuListener, ActionEditTextListener {
    //private static final String LOG_TAG = "ActionListHandler";

    public static int MENU_COPY_ID = 1;
    public static int MENU_EDIT_ID = 2;

    /**
     * Interface for notifications of ocr action list events
     */
    public interface ListEventListener {
        /**
         * Notification when date is clicked
         *
         * @param dataListIndex
         *            data list index
         * @param dateIndex
         *            date index
         */
        public void onDateClicked(int dataListIndex, int dateIndex);

        /**
         * Notification when text is clicked
         *
         * @param dataListIndex
         *            data list index
         * @param text
         *            text which is clicked
         */
        public void onTextClicked(int dataListIndex, String text);

        /**
         * Notification when an action item enters the edit text mode
         */
        public void onEditTextModeStart();

        /**
         * Notification when an action item exits the edit text mode
         */
        public void onEditTextModeOver(int parentIndex, String newText);
    }

    private class ActionItem {
        private final int parentIndex;
        private final RecognizedData data;
        private final int dateIndex;
        private View view;

        private ActionItem(int index, RecognizedData data) {
            this(index, data, -1);
        }

        private ActionItem(int index, RecognizedData data, int dateIndex) {
            this.parentIndex = index;
            this.data = data;
            this.dateIndex = dateIndex;
        }
    }

    private final Activity activity;
    private final LayoutInflater layoutInflater;
    private final ViewGroup layoutView;
    private ActionItem selectedActionItem = null;
    private boolean editOn = false;

    private final DateTextHandler dateTimeUtils;
    private final String appLocale;
    private final String enLocale;

    private final ListEventListener listEventListener;
    private ArrayList<ActionItem> actionItemList = new ArrayList<ActionItem>();

    private class ActionClickListener implements OnClickListener {
        private final ActionItem actionItem;

        private ActionClickListener(ActionItem actionItem) {
            this.actionItem = actionItem;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onClick(View v) {
            if (editOn) {
                return;
            }

            int nativeType = actionItem.data.nativeType;
            String text = actionItem.data.getText();

            Intent clickedIntent = null;
            if (FieldType.isPhoneText(nativeType)) {
                clickedIntent = new Intent(Intent.ACTION_VIEW);
                clickedIntent.setData(Uri.parse("tel:" + text));

            } else if (FieldType.isURLText(nativeType)) {
                String urlText = text;
                if (!text.startsWith("http://")) {
                    urlText = "http://" + text;
                }
                Uri urlPage = Uri.parse(urlText);
                clickedIntent = new Intent(Intent.ACTION_VIEW, urlPage);

            } else if (FieldType.isEmailText(nativeType)) {
                clickedIntent = new Intent(Intent.ACTION_SEND);
                //clickedIntent.setType(HTTP.PLAIN_TEXT_TYPE);
                clickedIntent.setType("text/plain");
                clickedIntent.putExtra(Intent.EXTRA_EMAIL, new String[] { text });

            } else if (FieldType.isAddressText(nativeType)) {
                Uri location = Uri.parse("geo:0,0?q=" + text);
                clickedIntent = new Intent(Intent.ACTION_VIEW, location);

            } else if (listEventListener != null) {
                if (FieldType.isDateText(nativeType) ) {
                    text = ((DateTimeData) actionItem.data).getDateAt(actionItem.dateIndex);
                    listEventListener.onDateClicked(actionItem.parentIndex, actionItem.dateIndex);
                } else {
                    listEventListener.onTextClicked(actionItem.parentIndex, text);
                }

                return;
            }

            if (clickedIntent != null) {
                try {
                    activity.startActivity(clickedIntent);
                } catch (ActivityNotFoundException e) {
                    String message = activity.getResources().getString(
                            R.string.message_no_matching_app);
                    Toast.makeText(activity, message + " " + text, Toast.LENGTH_LONG).show();
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * Constructor for the class
     *
     * @param activity
     *            parent activity
     * @param view
     *            parent view for the list items
     * @param rawDataList
     *            STA raw data list
     * @param listener
     *            OCR action list listener
     */
    public ActionListHandler(Activity activity, ViewGroup view,
            ArrayList<RecognizedData> rawDataList, ListEventListener listener) {
        this.activity = activity;
        this.listEventListener = listener;
        layoutView = view;
        layoutInflater = LayoutInflater.from(activity);

        dateTimeUtils = new DateTextHandler(activity.getResources());
        appLocale = PreferenceData.getOCRLocale(activity);
        enLocale = activity.getResources().getString(R.string.setting_sta_locale_usa);

        for (int i = 0; i < rawDataList.size(); i++) {
            RecognizedData data = rawDataList.get(i);
            if (FieldType.isDateText(data.nativeType)) {
                int datesCount = ((DateTimeData) data).getDateCount();
                for (int j = 0; j < datesCount; j++) {
                    actionItemList.add(new ActionItem(i, data, j));
                }
            } else {
            actionItemList.add(new ActionItem(i, data));
        }
        }

        sortDataList();
        constructList();
    }

    private void sortDataList() {
        class DataListComparator implements Comparator<ActionItem> {
            /**
             * {@inheritDoc}
             */
            @Override
            public int compare(ActionItem lhs, ActionItem rhs) {
                if (lhs.data.id < rhs.data.id) {
                    return 1;// swap
                } else if (lhs.data.id > rhs.data.id) {
                    return -1;
                }
                if (FieldType.isGeneralText(lhs.data.nativeType)
                        && !FieldType.isGeneralText(rhs.data.nativeType)) {
                    return 1;// swap
                } else if (!FieldType.isGeneralText(lhs.data.nativeType)
                        && FieldType.isGeneralText(rhs.data.nativeType)) {
                    return -1;
                }
                return 0;
            }
        }
        Collections.sort(actionItemList, new DataListComparator());
    }

    private void constructList() {
        for (ActionItem actionItem : actionItemList) {
            RecognizedData data = actionItem.data;
            int imageId = FieldType.getActionImageIdFromNativeType(actionItem.data.nativeType);

            String actionTitle;
            String actionText;
            if (FieldType.isDateText(data.nativeType)) {
                String dateTime = ((DateTimeData) data).getDateAt(actionItem.dateIndex);
                actionText = dateTime;
                actionTitle = dateTimeUtils.getActionTitle((DateTimeData) data, dateTime);

            } else if (FieldType.isGeneralText(data.nativeType)) {
                actionText = data.getText();
                actionTitle = actionText.replace('\n', ' ');

            } else if (FieldType.isPhoneText(data.nativeType)) {
                actionText = data.getText();
                actionTitle = (appLocale.equals(enLocale)) ? Utils.toUsaFormattedPhone(actionText)
                        : actionText;

            } else {
                actionText = data.getText();
                actionTitle = actionText;
            }
            addAction(actionTitle, actionText, imageId, actionItem);
        }
    }

    private void addAction(String actionTitle, String actionText, int imageId, ActionItem actionItem) {
        View view = layoutInflater.inflate(R.layout.action_item, layoutView, false);
        // set image
        ImageView actionImage = (ImageView) view.findViewById(R.id.action_image);
        actionImage.setImageResource(imageId);

        // set text
        TextView actionTextView = (TextView) view.findViewById(R.id.action_text);
        actionTextView.setText(actionTitle);
        if (FieldType.isAddressText(actionItem.data.nativeType)) {
            actionTextView.setSingleLine(false);
        }

        // add click listener
        view.setOnClickListener(new ActionClickListener(actionItem));
        view.setOnCreateContextMenuListener(this);
        layoutView.addView(view);
        actionItem.view = view;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onCreateContextMenu(ContextMenu menu, View view, ContextMenuInfo menuInfo) {
        if (!editOn) {
            selectedActionItem = getActionItem(view);

            if (selectedActionItem.data != null) {
                String copyMenuCaption = activity.getResources().getString(R.string.menu_item_copy);
                menu.add(Menu.NONE, MENU_COPY_ID, 0, copyMenuCaption);

                if (!FieldType.isDateText(selectedActionItem.data.nativeType)) {
                    String editMenuCaption = activity.getResources().getString(
                            R.string.menu_item_edit);
                    menu.add(Menu.NONE, MENU_EDIT_ID, 1, editMenuCaption);
                }
            }
        }
    }

    /**
     * get the text which is ready for copy
     *
     * @param view
     *            action item view
     * @return action item
     */
    private ActionItem getActionItem(View view) {
        int viewAt = -1;
        for (int i = 0; i < layoutView.getChildCount(); i++) {
            View childView = layoutView.getChildAt(i);
            if (view == childView) {
                viewAt = i;
            }
        }
        return (viewAt > -1) ? actionItemList.get(viewAt) : null;
    }

    /**
     * Make text editable
     */
    public void makeSelectedActionEditable() {
        final TextView textView = (TextView) selectedActionItem.view.findViewById(R.id.action_text);
        final ActionEditText editText = (ActionEditText) selectedActionItem.view
                .findViewById(R.id.action_text_edit);

        enableEditMode(editText, textView, true);
        editText.setOnEditorActionListener(new OnEditorActionListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
                int result = actionId & EditorInfo.IME_MASK_ACTION;
                if (result == EditorInfo.IME_ACTION_DONE) {
                    enableEditMode(editText, textView, false);
                }
                return false;
            }
        });

        InputMethodManager keyboard = (InputMethodManager) activity
                .getSystemService(Context.INPUT_METHOD_SERVICE);
        keyboard.showSoftInput(editText, 0);
    }

    /**
     * Resets/clear the selected action mode
     */
    public void clearSelectedActionItem() {
        selectedActionItem = null;
        editOn = false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onKeyboardRemoved(ActionEditText editText, View textView) {
        enableEditMode(editText, (TextView) textView, false);
    }

    /**
     * Clears the edit text information if any
     */
    public void clearEditModeIfAny() {
        if (editOn) {
            TextView textView = (TextView) selectedActionItem.view.findViewById(R.id.action_text);
            ActionEditText editText = (ActionEditText) selectedActionItem.view
                    .findViewById(R.id.action_text_edit);
            enableEditMode(editText, textView, false);
        }
    }

    /**
     * Checks the edit text mode
     *
     * @return true if selected text is in edit text mode, false otherwise
     */
    public boolean isInEditTextMode() {
        return editOn;
    }

    /**
     * gets the selected action data
     *
     * @return data
     */
    public RecognizedData getSelectedActionData() {
        return selectedActionItem.data;
    }

    private void enableEditMode(ActionEditText editText, TextView textView, boolean enable) {
        if(editOn == enable) {
            return;
        }

        if (enable) {
            RecognizedData data = selectedActionItem.data;
            if (FieldType.isGeneralText(data.nativeType)) {
                String dataText = data.getText();
                if (dataText.contains("\n")) { // is multiline
                    if (listEventListener != null) {
                        listEventListener.onTextClicked(selectedActionItem.parentIndex, dataText);
                    }
                    clearSelectedActionItem();
                    return;
                }
            }
            textView.setVisibility(View.INVISIBLE);

            editText.setInformation(textView, this);
            editText.setVisibility(View.VISIBLE);

            String text = textView.getText().toString();
            editText.setText(text);
            editText.setSelection(text.length());
            editOn = true;

            if (listEventListener != null) {
                listEventListener.onEditTextModeStart();
            }

        } else {
            editText.setVisibility(View.INVISIBLE);
            String edittedText = editText.getText().toString();
            textView.setText(edittedText);
            textView.setVisibility(View.VISIBLE);

            int parentIndex = selectedActionItem.parentIndex;
            clearSelectedActionItem();

            if (listEventListener != null) {
                listEventListener.onEditTextModeOver(parentIndex, edittedText);
            }
        }

    }

    /**
     * Get the number of elements
     *
     * @return number of elements
     */
    public int getElementsCount() {
        return actionItemList.size();
    }

    /**
     * Gets the parent (parser data index) index of the action item
     *
     * @param textIndex
     *            index of the action item text
     * @return parent index
     */
    public int getParentIndex(int textIndex) {
        if (textIndex < 0 || textIndex >= actionItemList.size()) {
            return -1;
        }
        ActionItem actionItem = actionItemList.get(textIndex);
        return actionItem.parentIndex;
    }

    /**
     * Updated text in the action list menu
     *
     * @param textIndex
     *            index in the action list menu
     * @param newText
     *            new text to be updated
     */
    public void updateText(int textIndex, String newText) {
        if (textIndex < 0 || textIndex >= actionItemList.size()) {
            return;
        }
        ActionItem actionItem = actionItemList.get(textIndex);
        TextView textView = (TextView) actionItem.view.findViewById(R.id.action_text);
        textView.setText(newText);
    }
}
