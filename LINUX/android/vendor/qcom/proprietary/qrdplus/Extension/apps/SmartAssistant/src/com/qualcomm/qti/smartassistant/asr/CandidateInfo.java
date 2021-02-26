/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.asr;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

public class CandidateInfo implements Serializable{

    private final String mContactName;
    private final String mContactId;
    private final List<Phone> mPhones= new ArrayList<>();

    public CandidateInfo(String contactName, String contactId){
        mContactName = contactName;
        mContactId = contactId;
    }

    public String getContactName(){
        return mContactName;
    }

    public String getContactId(){
        return mContactId;
    }

    public void addContactPhoneInfo(Phone phone){
        mPhones.add(phone);
    }

    public Object getItem(int position) {
        if (position == 0) {
            return getContactName();
        } else {
            return mPhones.get(position - 1);
        }
    }

    public int size() {
        return mPhones.size() + 1;
    }

    public List<Phone> getPhones(){
        return mPhones;
    }

    public static class Phone implements Serializable{
        private final String mNumber;
        private final CharSequence mTypeLabel;

        public Phone(String number, CharSequence typeLabel){
            mNumber = number;
            mTypeLabel = typeLabel;
        }

        public String getNumber(){
            return mNumber;
        }

        public CharSequence getTypeLabel(){
            return mTypeLabel;
        }
    }
}
