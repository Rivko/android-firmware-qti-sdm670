/*
 * Copyright (c) 2013, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qti.fcctest;

import android.text.TextUtils;
import android.util.Log;
import android.widget.Spinner;

import java.util.HashMap;


public class Utils {
    // The ID for the tx test and rx test.
    public static final int ID_TX_TEST = 0;
    public static final int ID_RX_TEST = 1;
    public static final int ID_SCW_TEST = 2;

    // This must be match the array list defined.
    public static final int ID_TEST_ITEM_BANDWIDTH = 0;
    public static final int ID_TEST_ITEM_CHANNEL = 1;
    public static final int ID_TEST_ITEM_RATE = 2;
    public static final int ID_TEST_ITEM_POWER = 3;
    public static final int ID_TEST_ITEM_POWER_MODE = 4;
    public static final int ID_TEST_ITEM_PACKETLENGTH = 5;
    public static final int ID_TEST_ITEM_PACKETCNT = 6;
    public static final int ID_TEST_ITEM_PACKETINTERVAL = 7;

    public static final int PHY_SINGLE_CHANNEL_CENTERED = 0;
    public static final int PHY_DOUBLE_CHANNEL_LOW_PRIMARY = 1;
    public static final int PHY_DOUBLE_CHANNEL_HIGH_PRIMARY = 3;
    public static final int PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED = 4;
    public static final int PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH = 10;


    private static String mBand = "";
    private static String[] mData = new String[]{"0", "1", "11B_LONG_1_MBPS", "15", "2", null, null ,null};

    public static boolean isSameCB(int cb1, int cb2)
    {
        if (cb1 == PHY_SINGLE_CHANNEL_CENTERED && cb2 == cb1)
                return true;
        else if ( (cb1 >= PHY_DOUBLE_CHANNEL_LOW_PRIMARY &&
                   cb1 <= PHY_DOUBLE_CHANNEL_HIGH_PRIMARY) &&
                  (cb2 >= PHY_DOUBLE_CHANNEL_LOW_PRIMARY &&
                   cb2 <= PHY_DOUBLE_CHANNEL_HIGH_PRIMARY))
                return true;
        else if ( (cb1 >= PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED &&
                   cb1 <= PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH) &&
                  (cb2 >= PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED &&
                   cb2 <= PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH))
                return true;

        return false;
    }

    public static void setData(int itemId, String val)
    {
        mData[itemId] = val;
    }

    public static String getData(int itemId)
    {
        return mData[itemId];
    }

    public static void setBand(String band){
        mBand = band;
    }

    public static String getBand(){
        return mBand;
    }

    public static boolean checkEditValueValid(int editId, int _val) {

        if( _val <= getEditValueMax(editId) &&
            _val >= getEditValueMin(editId)){
            return true;
        }
        return false;
    }

    public static int getEditValueDefault(int editId){

         switch(editId){
            case ID_TEST_ITEM_PACKETLENGTH:
                return 1000;
            case ID_TEST_ITEM_PACKETCNT:
                return 0;
            case ID_TEST_ITEM_PACKETINTERVAL:
                return 200;

         }
        return 0;
    }

    public static int getEditValueMin(int editId){
         switch(editId){
            case ID_TEST_ITEM_PACKETLENGTH:
                return 1;
            case ID_TEST_ITEM_PACKETCNT:
                return 0;
            case ID_TEST_ITEM_PACKETINTERVAL:
                return 10;

         }
        return 0;
    }

    public static int getEditValueMax(int editId){
         switch(editId){
            case ID_TEST_ITEM_PACKETLENGTH:
                return 4095;
            case ID_TEST_ITEM_PACKETCNT:
                return 65535;
            case ID_TEST_ITEM_PACKETINTERVAL:
                return 100000;
         }
        return 0;
    }


    /**
     * To save the spinner for constants values.
     */
    public static class ValueContent {
        private static final String TAG = "ValueContent";

        public final String _value;
        public final String _label;

        public ValueContent(String value, String label) {
            this._value = value;
            this._label = label;
        }

        public static ValueContent setSpinnerContentValue(Spinner spinner, String value) {
            for (int i = 0, count = spinner.getCount(); i < count; i++) {
                ValueContent sc = (ValueContent) spinner.getItemAtPosition(i);
                if (sc._value.equalsIgnoreCase(value)) {
                    spinner.setSelection(i, true);
                    Log.i(TAG, "Set selection for spinner(" + sc + ") with the value: " + value);
                    return sc;
                }
            }
            return null;
        }

        public static ValueContent getSpinnerContentValue(Spinner spinner, int position) {
            return (ValueContent) spinner.getItemAtPosition(position);
        }

        @Override
        public String toString() {
            return _label;
        }
    }

    public static class SelectionsContent {
        private int _testId;
        private HashMap<String, ValueContent> _values;

        public SelectionsContent(int testId) {
            _testId = testId;
            _values = new HashMap<String, ValueContent>();
        }

        public void reset(int testId) {
            _testId = testId;
            if (_values == null) {
                _values = new HashMap<String, ValueContent>();
            } else {
                _values.clear();
            }
        }

        public void setSelection(String itemId, ValueContent value) {
            if (TextUtils.isEmpty(itemId)) return;

            if (_values == null) {
                _values = new HashMap<String, ValueContent>();
            }
            _values.put(itemId, value);
        }

        public int getTestId() {
            return _testId;
        }

        public HashMap<String, ValueContent> getSelections() {
            return _values;
        }
    }

}
