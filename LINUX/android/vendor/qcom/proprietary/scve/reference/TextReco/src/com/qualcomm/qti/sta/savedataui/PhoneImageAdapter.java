/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.

 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;

import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.R;

/**
 * Class for phone Image adapter to be used by spinner
 */
public class PhoneImageAdapter extends BaseAdapter {

    private enum Phone {
        WORK(FieldType.CONTACT_PHONE_WORK, R.drawable.ic_type_work), MOBILE(
                FieldType.CONTACT_PHONE_MOBILE, R.drawable.ic_type_mobile), HOME(
                FieldType.CONTACT_PHONE_HOME, R.drawable.ic_type_home), FAX(
                FieldType.CONTACT_PHONE_FAX, R.drawable.ic_type_fax);

        private final FieldType type;
        private final int resourceId;

        private Phone(FieldType type, int resourceId) {
            this.type = type;
            this.resourceId = resourceId;
        }
    }

    private final Phone[] phones;
    private final Context context;

    /**
     * Constructor for the calss
     *
     * @param context
     *            application context
     */
    public PhoneImageAdapter(Context context) {
        this.context = context;
        phones = Phone.values();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getCount() {
        return phones.length;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Object getItem(int position) {
        return phones[position];
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long getItemId(int position) {
        return position;
    }

    /**
     * gets the phone type
     *
     * @param position
     *            position in the phones list
     * @return phone field type
     */
    public FieldType getPhoneType(int position) {
        return phones[position].type;
    }
    /**
     * gets the position of the phone type in the list
     *
     * @param type
     *            phone type
     * @return position
     */
    public int getPhonePosition(FieldType type) {
        for(int i = 0; i < phones.length; i++) {
            if(phones[i].type == type) {
                return i;
            }
        }
        return 0;// first one
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View view = convertView;
        if (view == null) {
            LayoutInflater layoutInflater = LayoutInflater.from(context);
            view = layoutInflater.inflate(R.layout.phone_item, parent, false);
        }

        // set image
        ImageView actionImage = (ImageView) view.findViewById(R.id.phone_image);
        actionImage.setImageResource(phones[position].resourceId);
        return view;
    }
}
