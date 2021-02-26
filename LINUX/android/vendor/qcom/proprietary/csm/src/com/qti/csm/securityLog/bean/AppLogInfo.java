/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog.bean;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class AppLogInfo {
	private static final List<PackageAction> groups = new ArrayList<PackageAction>(0);
	private static final Map<String, List<ActionInfo>> actions = new HashMap<String, List<ActionInfo>>(0);

	public static class ActionInfo {
		private String action;
		private String time;

		public ActionInfo(String action, String time) {
			this.action = action;
			this.time = time;
		}

		public String getAction() {
			return action;
		}

		public void setAction(String action) {
			this.action = action;
		}

		public String getTime() {
			return time;
		}

		public void setTime(String time) {
			this.time = time;
		}
	}

	public static class PackageAction {
		private String name;
		private List<ActionInfo> items = new ArrayList<ActionInfo>();

		public String getName() {
			return name;
		}

		public void setName(String name) {
			this.name = name;
		}

		public List<ActionInfo> getItems() {
			return items;
		}

		public void setItems(List<ActionInfo> items) {
			this.items = items;
		}
	}

	public static void addGroup(PackageAction group) {
		groups.add(group);
		actions.put(group.getName(), group.getItems());
	}
}
