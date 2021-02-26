/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 */

package com.qti.csm.backup.mms;

import java.io.Serializable;

public class QTIMMSObject implements Serializable {

    private String id;
    private String msg_box;
    private String msg_type;
    private String read;
    private String date;
    private String locked;
    private String msg_size;
    private byte[] mmData;


    public byte[] getMmData() {
        return mmData;
    }
    public void setMmData(byte[] mmData) {
        this.mmData = mmData;
    }
    public String getId() {
        return id;
    }
    public void setId(String id) {
        this.id = id;
    }
    public String getMsg_box() {
        return msg_box;
    }
    public void setMsg_box(String msg_box) {
        this.msg_box = msg_box;
    }
    public String getMsg_type() {
        return msg_type;
    }
    public void setMsg_type(String msg_type) {
        this.msg_type = msg_type;
    }
    public String getRead() {
        return read;
    }
    public void setRead(String read) {
        this.read = read;
    }
    public String getDate() {
        return date;
    }
    public void setDate(String date) {
        this.date = date;
    }
    public String getLocked() {
        return locked;
    }
    public void setLocked(String locked) {
        this.locked = locked;
    }
    public String getMsg_size() {
        return msg_size;
    }
    public void setMsg_size(String msg_size) {
        this.msg_size = msg_size;
    }
}
