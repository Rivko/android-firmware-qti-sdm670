/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2016 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag Java Lib Support

GENERAL DESCRIPTION

Implementation of Diag Java Interface. Users have to implement the
below callback methods in their class in order to receive callbacks
on receiving dci data from diag.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
package com.qualcomm.qti.diagservice;

public interface diag_dci_cbs
{
	public void process_dci_log_stream(byte[] data, int len);
	public void process_dci_event_stream(byte[] data, int len);
	public void process_dci_pkt_rsp(byte[] data, int len, long data_ptr);
	public void process_dci_signal_cb(int signal);
}
