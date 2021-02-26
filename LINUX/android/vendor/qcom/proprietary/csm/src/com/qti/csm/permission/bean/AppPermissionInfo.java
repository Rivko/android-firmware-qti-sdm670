/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission.bean;

import android.content.pm.ApplicationInfo;

public class AppPermissionInfo {

	public int uid;
	public String names[];
	public boolean isAllow;
	public ApplicationInfo appinfo;

	public AppPermissionInfo() {
	}

	public AppPermissionInfo(int uid, String name, boolean isAllow) {
		this.uid = uid;
		this.names = new String[] { name };
		this.isAllow = isAllow;
	}
}
