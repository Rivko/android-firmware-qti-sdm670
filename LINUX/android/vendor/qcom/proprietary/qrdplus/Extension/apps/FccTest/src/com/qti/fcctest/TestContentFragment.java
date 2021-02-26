/*
 * Copyright (c) 2013, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qti.fcctest;

import android.app.ListFragment;
import android.app.AlertDialog;

import android.content.Context;
import android.os.Bundle;
import android.os.SystemProperties;

import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.KeyEvent;

import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnKeyListener;

import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.EditText;

import android.widget.AdapterView.OnItemSelectedListener;



import com.qti.fcctest.Utils.SelectionsContent;
import com.qti.fcctest.Utils.ValueContent;

import java.util.ArrayList;
import java.util.HashMap;

public class TestContentFragment extends ListFragment {
    private static final String TAG = "TestContentFragment";

    private View mRootView;
    private LayoutInflater mInflater;

    private ListView mList;
    private MyArrayAdapter mAdapter;

    private int mTestId;
    private SelectionsContent mSelections;
    private OnSelectionChangedListener mSpinnerListener = null;
    private OnEditTextChangedListener  mEditTextChangedListener = null;

    private String[] mTestItemsName;
    private String[] mTestItemsProp;
    private HashMap<String, ArrayList<ValueContent>> mTestItemsList;

    private OnItemSelectedListener mSpinnerValueChangedListener = new OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            int itemId = (Integer) parent.getTag();

            ValueContent value = ValueContent.getSpinnerContentValue((Spinner) parent, position);
            Log.d(TAG, "Value changed, item: " + mTestItemsName[itemId]
                    + ", new value: " + value._value);


            if(itemId == Utils.ID_TEST_ITEM_BANDWIDTH)
            {
                int cb = Integer.parseInt(value._value);
                int pre_cb = Integer.parseInt(Utils.getData(itemId));

                if ( !Utils.isSameCB(cb, pre_cb )){

                    ArrayList<ValueContent> channel = null;
                    ArrayList<ValueContent> rate = null;

                    if (cb == Utils.PHY_SINGLE_CHANNEL_CENTERED){ //HT20

                        channel = getTestItemOptions(
                            R.array.opt_channel_2g_ht20, R.array.opt_channel_value_2g_ht20);
                        rate = getTestItemOptions(
                            R.array.opt_rate_11b, R.array.opt_rate_value_11b);
                        rate.addAll(getTestItemOptions(
                            R.array.opt_rate_ht20, R.array.opt_rate_value_ht20));
                        if (Utils.getBand().equals("5G") ||
                            Utils.getBand().equals("HT80")){

                            channel.addAll(getTestItemOptions(
                                R.array.opt_channel_5g_ht20, R.array.opt_channel_value_5g_ht20));

                            if(Utils.getBand().equals("HT80")){
                                rate.addAll(getTestItemOptions(
                                    R.array.opt_rate_vht20, R.array.opt_rate_value_vht20));
                            }
                        }

                    }
                    else if( cb >= Utils.PHY_DOUBLE_CHANNEL_LOW_PRIMARY &&
                             cb <= Utils.PHY_DOUBLE_CHANNEL_HIGH_PRIMARY){//HT40

                        channel = getTestItemOptions(
                            R.array.opt_channel_2g_ht40, R.array.opt_channel_value_2g_ht40);
                        rate = getTestItemOptions(
                            R.array.opt_rate_ht40, R.array.opt_rate_value_ht40);
                        if (Utils.getBand().equals("5G") ||
                            Utils.getBand().equals("HT80")){

                            channel.addAll(getTestItemOptions(
                                R.array.opt_channel_5g_ht40, R.array.opt_channel_value_5g_ht40));

                            if(Utils.getBand().equals("HT80")){
                                rate.addAll(getTestItemOptions(
                                    R.array.opt_rate_vht40, R.array.opt_rate_value_vht40));
                            }
                        }

                    }
                    else { //HT80
                        channel = getTestItemOptions(
                            R.array.opt_channel_ht80, R.array.opt_channel_value_ht80);
                        rate = getTestItemOptions(
                            R.array.opt_rate_vht80, R.array.opt_rate_value_vht80);
                    }

                    mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_CHANNEL],channel);
                    if (mTestItemsName.length > Utils.ID_TEST_ITEM_RATE) {
                        mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_RATE], rate);
                    }
                    mAdapter.notifyDataSetChanged();

                }
            }
            else if(itemId == Utils.ID_TEST_ITEM_CHANNEL)
            {
                int cb = Integer.parseInt(Utils.getData(Utils.ID_TEST_ITEM_BANDWIDTH));

                if( cb == Utils.PHY_SINGLE_CHANNEL_CENTERED){
                    boolean rateChanged = false;
                    int new_ch = Integer.parseInt(value._value);
                    int pre_ch = Integer.parseInt(Utils.getData(Utils.ID_TEST_ITEM_CHANNEL));
                    if ((new_ch <= 14 && pre_ch > 14) ||
                        (new_ch > 14 && pre_ch <= 14)){
                        rateChanged = true;
                    }

                    if(rateChanged){
                        ArrayList<ValueContent> rate = new ArrayList<ValueContent>();
                        if(new_ch <= 14){
                            rate.addAll(getTestItemOptions(
                                R.array.opt_rate_11b, R.array.opt_rate_value_11b));
                        }
                        rate.addAll(getTestItemOptions(
                           R.array.opt_rate_ht20, R.array.opt_rate_value_ht20));
                        if (mTestItemsName.length > Utils.ID_TEST_ITEM_RATE) {
                            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_RATE], rate);
                        }
                        mAdapter.notifyDataSetChanged();
                    }
                }
            }


           Utils.setData(itemId, value._value);
           updateSelectionsContent(itemId, value);

        }
        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            // Do nothing.
        }
    };

    private OnKeyListener mEditValueSetListener = new OnKeyListener(){
        @Override
        public boolean onKey(View v, int keyCode, KeyEvent event) {


        if(keyCode == KeyEvent.KEYCODE_ENTER &&
           event.getAction() == KeyEvent.ACTION_DOWN){

           EditText text = (EditText)v;
           int editId = (Integer)text.getTag();
           String editValue = text.getText().toString();
           int editValueInt;

           if(editValue.equals(""))
               editValueInt = Utils.getEditValueDefault(editId);
           else
               editValueInt = Integer.parseInt(editValue);

           Log.d(TAG, "Edit Value" + editValueInt + " " + editId);

           if(Utils.checkEditValueValid(editId , editValueInt) == false){

                new AlertDialog.Builder(getActivity())
                .setTitle("Alert!")
                .setMessage("The value range from " + Utils.getEditValueMin(editId) + " to " + Utils.getEditValueMax(editId))
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton("Back", null)
                .show();

                //reset value to default
                editValueInt = Utils.getEditValueDefault(editId);
                text.setText("");
           }
           Utils.setData(editId, editValue);

           // Notify the edittext change.
           if (mEditTextChangedListener != null) {
               mEditTextChangedListener.onEditTextChanged(editId, editValueInt);
           }

        }

        return false;
        }

    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mRootView = super.onCreateView(inflater, container, savedInstanceState);
        mInflater = inflater;

        return mRootView;
    }

    @Override
    public void onActivityCreated(Bundle bundle) {
        mList = getListView();
        mList.setDividerHeight(0);

        super.onActivityCreated(bundle);
    }

    public void onTestModeChanged(int newTestId) {
        mTestId = newTestId;
        if (mSelections == null) {
            mSelections = new SelectionsContent(mTestId);
        } else {
            mSelections.reset(mTestId);
        }

        buildTestItems();
    }

    public void setOnSelectionChangedListener(OnSelectionChangedListener listener) {
        mSpinnerListener = listener;
    }

    public void setOnEditTextChangedListener(OnEditTextChangedListener listener) {
        mEditTextChangedListener = listener;
    }

    public void buildTestItems() {
        if (mTestItemsList == null) {
            mTestItemsList = new HashMap<String, ArrayList<ValueContent>>();
        }

        mTestItemsList.clear();
        Log.e(TAG, "buildTestItems");

        if (mTestId == Utils.ID_TX_TEST) {
            String[] itemsName = getResources().getStringArray(R.array.tx_test_items);
            String[] itemsProps = getResources().getStringArray(R.array.tx_test_items_propertiy);
            ArrayList<ValueContent> channel = null;
            ArrayList<ValueContent> bandwidth = null;
            ArrayList<ValueContent> rate = null;

            mTestItemsName = itemsName;
            mTestItemsProp = itemsProps;

            channel = getTestItemOptions(
                R.array.opt_channel_2g_ht20, R.array.opt_channel_value_2g_ht20);
            bandwidth = getTestItemOptions(
                R.array.opt_bandwidth_ht20_ht40, R.array.opt_bandwidth_value_ht20_ht40);
            rate = getTestItemOptions(R.array.opt_rate_11b, R.array.opt_rate_value_11b);
            rate.addAll(getTestItemOptions(R.array.opt_rate_ht20, R.array.opt_rate_value_ht20));

            if (Utils.getBand().equals("5G")){ //  5G HW
                channel.addAll(getTestItemOptions(
                    R.array.opt_channel_5g_ht20, R.array.opt_channel_value_5g_ht20));

            }
            else if (Utils.getBand().equals("HT80")){ // 11AC HW
                channel.addAll(getTestItemOptions(
                    R.array.opt_channel_5g_ht20, R.array.opt_channel_value_5g_ht20));
                rate.addAll(getTestItemOptions(
                    R.array.opt_rate_vht20, R.array.opt_rate_value_vht20));
                bandwidth.addAll(getTestItemOptions(
                    R.array.opt_bandwidth_ht80, R.array.opt_bandwidth_value_ht80));

            }
            // For tx test, it need this two item.
            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_CHANNEL], channel);
            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_RATE], rate);
            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_BANDWIDTH], bandwidth);


            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_POWER],
                    getTestItemOptions(R.array.opt_power, R.array.opt_power_value));
            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_POWER_MODE],
                    getTestItemOptions(R.array.opt_powermode, R.array.opt_powermode_value));
        } else if (mTestId == Utils.ID_RX_TEST) {
            mTestItemsName = getResources().getStringArray(R.array.rx_test_items);
            mTestItemsProp = getResources().getStringArray(R.array.rx_test_items_property);

            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_BANDWIDTH],
                    getTestItemOptions(R.array.opt_bandwidth_ht20, R.array.opt_bandwidth_value_ht20));

            ArrayList<ValueContent> channel = getTestItemOptions(
                R.array.opt_channel_2g_ht20, R.array.opt_channel_value_2g_ht20);
            if (Utils.getBand().equals("5G") || Utils.getBand().equals("HT80")) {
                channel.addAll(getTestItemOptions(
                    R.array.opt_channel_5g_ht20, R.array.opt_channel_value_5g_ht20));
            }
            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_CHANNEL], channel);
        } else if (mTestId == Utils.ID_SCW_TEST) {
            mTestItemsName = getResources().getStringArray(R.array.scw_test_items);
            mTestItemsProp = getResources().getStringArray(R.array.scw_test_items_property);

            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_BANDWIDTH],
                    getTestItemOptions(R.array.opt_bandwidth_ht20, R.array.opt_bandwidth_value_ht20));

            ArrayList<ValueContent> channel = getTestItemOptions(
                R.array.opt_channel_2g_ht20, R.array.opt_channel_value_2g_ht20);
            if (Utils.getBand().equals("5G") || Utils.getBand().equals("HT80")) {
                channel.addAll(getTestItemOptions(
                    R.array.opt_channel_5g_ht20, R.array.opt_channel_value_5g_ht20));
            }
            mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_CHANNEL], channel);
        }

        // Do not show these items
        /*
        mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_ANTENNA],
                getTestItemOptions(R.array.opt_antenna, 0));
        mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_TYPE],
                getTestItemOptions(R.array.opt_type, 0));
        mTestItemsList.put(mTestItemsName[Utils.ID_TEST_ITEM_PATTERN],
                getTestItemOptions(R.array.opt_pattern, 0));
        */

        // Set the adapter to the list.
        mAdapter = new MyArrayAdapter(this, getActivity(), R.layout.test_item, mTestItemsName);
        setListAdapter(mAdapter);
    }

    private ArrayList<ValueContent> getTestItemOptions(int labelsResId, int valuesResId) {
        ArrayList<ValueContent> valuesList = new ArrayList<ValueContent>();
        String[] labels = getResources().getStringArray(labelsResId);
        String[] values = null;
        if (valuesResId > 0) {
            values = getResources().getStringArray(valuesResId);
        }
        if (values != null && labels.length != values.length) {
            Log.e(TAG, "The label's length is not same as the values length. Please check it.");
            return valuesList;
        }

        for (int i = 0; i < labels.length; i++) {
            ValueContent value = new ValueContent(
                    values != null ? values[i] : labels[i],
                    labels[i]);
            valuesList.add(value);
        }
        return valuesList;
    }

    private void updateSelectionsContent(int itemId, ValueContent curValue) {
        if (curValue == null) return;

        // Update the selection.
        if (mSelections == null) {
            mSelections = new SelectionsContent(mTestId);
        }
        mSelections.setSelection(String.valueOf(itemId), curValue);

        // Notify the selection change.
        if (mSpinnerListener != null) {
            mSpinnerListener.onSelectionChanged(mSelections, itemId);
        }
    }


    private static class MyArrayAdapter extends ArrayAdapter<String> {
        private TestContentFragment fragment;

        public MyArrayAdapter(TestContentFragment fragment, Context context, int resource,
                String[] itemsName) {
            super(context, resource, itemsName);
            this.fragment = fragment;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
         View v = null;
         String prop = fragment.mTestItemsProp[position];

        if( prop.equals("Spinner") ) {
            v = fragment.mInflater.inflate(R.layout.test_item, null);
            Log.d(TAG, "getView " + position + fragment.mTestItemsName[position]);


            TextView testName = (TextView) v.findViewById(R.id.test_item_name);
            testName.setText(fragment.mTestItemsName[position]);


            Spinner testValue = (Spinner) v.findViewById(R.id.test_item_value);
            setSpinnerValues(position, testValue, Utils.getData(position));
            }
        else if( prop.equals("EditPacketLen") ){
            v = fragment.mInflater.inflate(R.layout.test_item_packet_len, null);
            Log.d(TAG, "getView " + position + fragment.mTestItemsName[position]);

            TextView testName = (TextView) v.findViewById(R.id.test_item_editname);
            testName.setText(fragment.mTestItemsName[position]);

            EditText testValue = (EditText)v.findViewById(R.id.test_item_packetlen);
            setEditTextValues(position, testValue, Utils.getData(position));

            }
         else if( prop.equals("EditPacketCnt") ){
            v = fragment.mInflater.inflate(R.layout.test_item_packet_cnt, null);
            Log.d(TAG, "getView " + position + fragment.mTestItemsName[position]);

            TextView testName = (TextView) v.findViewById(R.id.test_item_editname);
            testName.setText(fragment.mTestItemsName[position]);

            EditText testValue = (EditText)v.findViewById(R.id.test_item_packetcnt);
            setEditTextValues(position, testValue, Utils.getData(position));

            }
          else if( prop.equals("EditPacketInterval") ){
            v = fragment.mInflater.inflate(R.layout.test_item_packet_interval, null);
            Log.d(TAG, "getView " + position + fragment.mTestItemsName[position]);

            TextView testName = (TextView) v.findViewById(R.id.test_item_editname);
            testName.setText(fragment.mTestItemsName[position]);

            EditText testValue = (EditText)v.findViewById(R.id.test_item_packetinterval);
            setEditTextValues(position, testValue, Utils.getData(position));

            }
            return v;
        }

        private void setSpinnerValues(int position, Spinner spinnerView, String curValue) {

            spinnerView.setTag(position);

            String itemName = fragment.mTestItemsName[position];
            ArrayList<ValueContent> values = fragment.mTestItemsList.get(itemName);
            if (TextUtils.isEmpty(curValue)) {
                curValue = values.get(0)._value;
            }

            ArrayAdapter<ValueContent> adapter = new ArrayAdapter<ValueContent>(
                    fragment.getActivity(), R.layout.spinner_item, values);
            spinnerView.setAdapter(adapter);
            ValueContent v = ValueContent.setSpinnerContentValue(spinnerView, curValue);
            fragment.updateSelectionsContent(position, v);
            spinnerView.setOnItemSelectedListener(fragment.mSpinnerValueChangedListener);
        }

    private void setEditTextValues(int position, EditText editView, String curValue) {
        if (editView == null) return;

        editView.setTag(position);
        editView.setOnKeyListener(fragment.mEditValueSetListener);
        editView.setText(curValue);
    }


    }

    public interface OnSelectionChangedListener {
        public void onSelectionChanged(SelectionsContent allSelection, int changedItemId);
    }

    public interface OnEditTextChangedListener {
       public void onEditTextChanged(int editId, int editValue);
    }
}
