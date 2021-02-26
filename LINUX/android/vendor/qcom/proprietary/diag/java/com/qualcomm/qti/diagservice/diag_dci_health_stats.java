/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2016 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag Java Lib Support

GENERAL DESCRIPTION

Implementation of Diag Java Interface. diag_dci_health_stats class is
implemented to return the diag dci health stats to users.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

package com.qualcomm.qti.diagservice;

public class diag_dci_health_stats {

	public int ret_val;
	public int dropped_logs;
	public int dropped_events;
	public int received_logs;
	public int received_events;
	public int reset_status;

	public diag_dci_health_stats(int ret_val, int dropped_logs,
				int dropped_events, int received_logs,
				int received_events, int reset_status)
	{
		this.ret_val = ret_val;
		this.dropped_logs = dropped_logs;
		this.dropped_events = dropped_events;
		this.received_logs = received_logs;
		this.received_events = received_events;
		this.reset_status = reset_status;
	}
};
