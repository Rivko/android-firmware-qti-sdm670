/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2016 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag Java Lib Support

GENERAL DESCRIPTION

Implementation of Diag Java Interface

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
package com.qualcomm.qti.diagservice;

import android.util.Log;

public class diag_dci {
	public static final int DIAG_DCI_NO_ERROR = 1001;  /* No error */
	public static final int DIAG_DCI_NO_REG = 1002;        /* Could not register */
	public static final int DIAG_DCI_NO_MEM = 1003;     /* Failed memory allocation */
	public static final int DIAG_DCI_NOT_SUPPORTED = 1004;      /*  client is not supported */
	public static final int DIAG_DCI_HUGE_PACKET = 1005;        /* Request/Response Packet too huge */
	public static final int DIAG_DCI_SEND_DATA_FAIL = 1006; /* writing to kernel or remote peripheral
								fails */
	public static final int DIAG_DCI_ERR_DEREG = 1007;      /* Error while de-registering */
	public static final int DIAG_DCI_PARAM_FAIL = 1008;     /* Incorrect Parameter */
	public static final int DIAG_DCI_DUP_CLIENT = 1009;
	diag_dci_cbs reg_obj = null;

	static
	{
		try {
			System.loadLibrary("diagjni");
		}
		catch (UnsatisfiedLinkError ule) {
			 Log.e("LibDiagWrapper", "WARNING: Could not load libdiagjni");
		}
	}

	/* Triggers the client log packet callback */
	public void diag_dci_log_cb(byte[] data, int len)
	{
		reg_obj.process_dci_log_stream(data,len);
	}

	/* Triggers the client event callback */
	public void diag_dci_event_cb(byte[] data, int len)
	{
		reg_obj.process_dci_event_stream(data,len);
	}

	/* Triggers the client pkt/rsp callback */
	public void diag_dci_pkt_rsp_cb(byte[] data , int len, long data_ptr)
	{
		reg_obj.process_dci_pkt_rsp(data, len, data_ptr);
	}

	/* Triggers the client signal callback */
	public void diag_dci_signal_cb_handler(int signal)
	{
		reg_obj.process_dci_signal_cb(signal);
	}

	/* Register the object on which callback methods needs to be called */
	public void register_dci_callback(diag_dci_cbs cb_obj)
	{
		reg_obj = cb_obj;
	}

	/* Native C calls */
	/* Initialization function required for DCI functions.

	Input parameters:
	a) Bit mask which holds peripheral information,
	b) an integer to specify which processor to talk to (Local or Remote in the case of Fusion Devices)
	c) For future needs (not implemented as of now)

	Return Value:
	Returns the client_id if successfully registered else it returns -1
	*/

	public final static native int diag_register_dci_client(long jarg1, int jarg2, int jarg3);

	/* This API provides information about the peripherals that support
	DCI in a given processor.

	Input Parameters:
	a) processor id

	Return Value:
	Returns the list of peripherals that support DCI on success , else returns 0 */
	public final static native int diag_get_dci_support_list_proc(int jarg1);

	/* Version handshaking function. This will get the dci version of diag lsm

	Return Value:
	Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/

	public final static native int diag_dci_get_version();

	/* Version handshaking function. This will register the version the client is using

	Input Parameters:
	a)client_id
	b)version number

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error

	*/
	public final static native int diag_dci_set_version(int jarg1, int jarg2);

	/* Sends command request over DCI.

	Input parameters are:
	a) client ID generate earlier
	b) request buffer
	c) request buffer length
	d) client data to receive in callback along with response

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_send_dci_async_req(int jarg1, byte[] jarg2, int jarg3, long jarg4);

	/* Closes DCI connection for this client. The client needs to pass
	the client id generated earlier

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_release_dci_client(int jarg1);

	/* Used to set up log streaming to the client. This will send an array of log codes, which are
	desired by client.

	Input parameters are:
	1. Client ID
	2. Boolean value telling to set or disable logs specified later
	3. Array of log codes
	4. Number of log codes specified in argument 3

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_log_stream_config(int jarg1, int jarg2, int[] jarg3, int jarg4);
	/* Initialization function required for DCI streaming.
	Callback for logs,events will be triggered to the client
        if client has implemented the methods in diag_dci_cbs interface and
        registered their class object using register_dci_callback.

	Input parameters are:
	1. Object of the calling function class which stored the callbacks object.
	2. Client ID

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_register_dci_stream_proc(Object param,int jarg1);

	/* Used to set up event streaming to the client. This will send an array of event ids, which are
	desired by client.

	Input parameters are:
	1. Client ID
	2. Boolean value telling to set or disable event specified later
	3. Array of event id
	4. Number of event ids specified in argument 3

	Return Value:
	Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_event_stream_config(int jarg1, int jarg2, int[] jarg3, int jarg4);
	/* Used to query DCI statistics on a specific processor for logs & events
	
	Input Parameters:
	1. Client ID
	2. Processor

	Return Value:
        Returns Object to diag_dci_health_stats class on success , else gives error
	*/
	public final static native Object diag_get_health_stats_proc(int jarg1, int jarg2);

	/* Queries a given Log Code to check if it is enabled or not.

	Input parameters are:
	1. Client ID
	2. Log Code that needs to be checked

	Return Value:
        Returns 1 on success , else gives zero
	*/
	public final static native int diag_get_log_status(int jarg1, long jarg2);

	/* Queries a given Event ID to check if it is enabled or not.

	Input parameters are:
	1. Client ID
	2. Event ID that needs to be checked

	Return Value:
        Returns 1 on success , else gives zero
	*/
	public final static native int diag_get_event_status(int jarg1, long jarg2);

	/* Disables all the Log Codes for a given client.

	Input parameters are:
	1.Client ID

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_disable_all_logs(int jarg1);

	/* Disables all the Event ID for a given client.
	
	Input parameters are:
	1.Client ID

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_disable_all_events(int jarg1);

	/* Votes for real time or non real time mode. The Input paramters are:
	
	1. Client ID
	2. The desired mode - MODE_REALTIME or MODE_NONREALTIME

	Return Value:
        Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_dci_vote_real_time(int jarg1, int jarg2);

	/* Gets the current mode (Real time or Non Real Time ) Diag is in.
	
	Input parameters are:
	1. Proc Id

	Return Value:
        Returns mode

	*/
	public final static native int diag_dci_get_real_time_status_proc(int jarg1);

	/* Registers a signal to be fired on receiving DCI data from
	the peripherals. Callback for signal will be triggered to the client
	if client has implemented the methods in diag_dci_cbs interface and
	registered their class object using register_dci_callback

	Input parameters are:
	1. Client ID
	2. Signal Type

	Return Value:
	Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_register_dci_signal_data(int jarg1, int jarg2);

	/* Disables the signal that fires on receiving DCI data from
	the peripherals.

	Input parameters are:
	1. Client ID
	2. Signal Type

	Return Value:
	Returns DIAG_DCI_NO_ERROR on success , else gives error
	*/
	public final static native int diag_deregister_dci_signal_data(int jarg1);
}

