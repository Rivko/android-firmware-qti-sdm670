/*
 * Copyright (c) 2017,2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>

#define LOG_TAG "NetPerfTuner"
#include <android-base/logging.h>
#include <sysutils/SocketClient.h>
#include <ResponseCode.h>
#include "NetworkPerformanceTuner.h"

#define MAX_VALUE_LENGTH	128

sp<NetworkPerformanceTuner> NetworkPerformanceTuner::sInstance;

IntTuningParameter::IntTuningParameter(const char *name,
				       const char *path)
:TuningParameter(name), mPath(path)
{
	// nothing to be done
}

int IntTuningParameter::init(NetworkPerformanceTuner *tuner)
{
	int rc = tuner->uint32FromFile(mPath.string(), mInitialValue);
	if (rc)
		LOG(ERROR) << mName.string() << ": failed to read initial value from "
			<< mPath.string();
	else
		LOG(DEBUG) << mName.string() << ": init from "
			<< mPath.string() << " with value " << mInitialValue;
	return rc;
}

int IntTuningParameter::set(NetworkPerformanceTuner *tuner,
			    String8 clientName, String8 value)
{
	uint32_t valueInt = strtoul(value.string(), NULL, 0);
	ssize_t index = mRequests.add(clientName, valueInt);

	if (index < 0) {
		LOG(ERROR) << "fail to add client " << clientName.string()
			<< " to parameter " << mName.string();
		return -ENOMEM;
	}

	LOG(INFO) << "parameter " << mName.string() << " set client "
		<< clientName.string() << " to value " << valueInt;
	return 0;
}

int IntTuningParameter::remove(NetworkPerformanceTuner *tuner,
			       String8 clientName)
{
	ssize_t index = mRequests.removeItem(clientName);

	if (index < 0) {
		LOG(ERROR) << "fail to remove client " << clientName.string()
			<< " from parameter " << mName.string();
		return -ENOENT;
	}

	return 0;
}

int IntTuningParameter::apply(NetworkPerformanceTuner *tuner)
{
	uint32_t value = mInitialValue;
	int rc;

	for (size_t i = 0; i < mRequests.size(); i++) {
		const uint32_t& rvalue = mRequests.valueAt(i);
		if (rvalue > value) {
			value = rvalue;
		}
	}

	rc = tuner->uint32ToFile(mPath, value);
	if (rc)
		LOG(ERROR) << mName.string() << ": fail to write value "
			<< value << " to " << mPath.string() << " err: " << rc;
	else
		LOG(DEBUG) << mName.string() << ": write " << value
			<< " to " << mPath.string();

	return rc;
}

RPSTuningParameter::RPSTuningParameter(const char *name, const char *path)
:IntTuningParameter(name, path)
{
	// nothing to be done
}

int RPSTuningParameter::init(NetworkPerformanceTuner *tuner)
{
	LOG(DEBUG) << mName.string() << ": init";
	return 0;
}

int RPSTuningParameter::apply(NetworkPerformanceTuner *tuner)
{
	int rc = 0;

	for (size_t i = 0; i < mRequests.size(); i++) {
		const String8& clientName = mRequests.keyAt(i);
		const uint32_t& value = mRequests.valueAt(i);
		String8 fname = String8::format(mPath.string(),
					       clientName.string());
		// rps_cpus expects bitmap as hex number
		String8 hvalue = String8::format("%x", value);
		int tmprc = tuner->stringToFile(fname.string(), hvalue.string());
		if (tmprc) {
			LOG(ERROR) << "fail to set RPS for client " << clientName.string();
			rc = tmprc;
			// continue, attempt other clients
		} else {
			LOG(DEBUG) << "set RPS " << hvalue.string() << " to " << fname.string();
		}
	}

	return rc;
}

PerInterfaceTuningParameter::PerInterfaceTuningParameter(
	const char *name, const char *path)
:IntTuningParameter(name, path)
{
	// nothing to be done
}

int PerInterfaceTuningParameter::init(NetworkPerformanceTuner *tuner)
{
	LOG(DEBUG) << mName.string() << ": init";
	return 0;
}

int PerInterfaceTuningParameter::apply(NetworkPerformanceTuner *tuner)
{
	int rc = 0;

	for (size_t i = 0; i < mRequests.size(); i++) {
		const String8& clientName = mRequests.keyAt(i);
		const uint32_t& value = mRequests.valueAt(i);
		String8 fname = String8::format(mPath.string(),
						clientName.string());
		int tmprc = tuner->uint32ToFile(fname.string(), value);
		if (tmprc) {
			LOG(ERROR) << "fail to set " << mName.string() <<
				" for client " << clientName.string();
			rc = tmprc;
			// continue, attempt other clients
		} else {
			LOG(DEBUG) << "set " << value << " to " << fname.string();
		}
	}

	return rc;
}

MemTuningParameter::MemTuningParameter(const char *name,
				       const char *path)
:TuningParameter(name), mPath(path)
{
	// nothing to do
}

int MemTuningParameter::init(NetworkPerformanceTuner *tuner)
{
	char strval[MAX_VALUE_LENGTH];

	int rc = tuner->stringFromFile(mPath.string(), strval,
				   MAX_VALUE_LENGTH);
	if (rc) {
		LOG(ERROR) << mName.string() << ": fail to read from file " << mPath.string();
		return rc;
	}
	int count = sscanf(strval, "%u %u %u",
			   &mInitialValue.minVal, &mInitialValue.defVal,
			   &mInitialValue.maxVal);
	if (count != 3) {
		LOG(ERROR) << mName.string() << ": fail to parse file " << mPath.string();
		return -EINVAL;
	}

	LOG(DEBUG) << mName.string() << ": init from " << mPath.string() << " with value "
		<< mInitialValue.minVal << "," << mInitialValue.defVal
		<< "," << mInitialValue.maxVal;

	return 0;
}

int MemTuningParameter::set(NetworkPerformanceTuner *tuner,
			    String8 clientName, String8 value)
{
	MemTuningRequest request;

	int count = sscanf(value.string(), "%u,%u,%u",
			   &request.minVal, &request.defVal, &request.maxVal);
	if (count != 3) {
		LOG(ERROR) << mName.string() << ": invalid value " << value.string()
			<< " for client " << clientName.string();
		return -EINVAL;
	}

	ssize_t index = mRequests.add(clientName, request);
	if (index < 0) {
		LOG(ERROR) << "failed to add client " << clientName.string()
			<< " to parameter " << mName.string();
		return -ENOMEM;
	}

	LOG(INFO) << "parameter " <<  mName.string() << " set client "
		<< clientName.string() << " to value " << value.string();
	return 0;
}

int MemTuningParameter::remove(NetworkPerformanceTuner *tuner,
			       String8 clientName)
{
	ssize_t index = mRequests.removeItem(clientName);

	if (index < 0) {
		LOG(ERROR) << "fail to remove client " << clientName.string()
			<< " from parameter " << mName.string();
		return -ENOENT;
	}

	return 0;
}

int MemTuningParameter::apply(NetworkPerformanceTuner *tuner)
{
	MemTuningRequest mergedRequest = mInitialValue;

	for (size_t i = 0; i < mRequests.size(); i++) {
		const MemTuningRequest& rvalue = mRequests.valueAt(i);
		if (rvalue.minVal > mergedRequest.minVal)
			mergedRequest.minVal = rvalue.minVal;
		if (rvalue.defVal > mergedRequest.defVal)
			mergedRequest.defVal = rvalue.defVal;
		if (rvalue.maxVal > mergedRequest.maxVal)
			mergedRequest.maxVal = rvalue.maxVal;
	}

	String8 value = String8::format("%u %u %u", mergedRequest.minVal,
				        mergedRequest.defVal,
				        mergedRequest.maxVal);

	int rc = tuner->stringToFile(mPath, value.string());
	if (rc)
		LOG(ERROR) << mName.string() << ": fail to write value "
			<< value.string() << " to " << mPath.string();
	else
		LOG(DEBUG) << mName.string() << ": write value " << value.string()
			<< " to " << mPath.string();

	return rc;
}

sp<NetworkPerformanceTuner> NetworkPerformanceTuner::getInstance()
{
	if (sInstance == NULL) {
		sInstance = new NetworkPerformanceTuner();
	}

	return sInstance;
}

int NetworkPerformanceTuner::addRequest(const char *paramName, const char *clientName,
			const char *requestedValue)
{
	String8 clientNameStr(clientName);
	ssize_t index = mClientNames.indexOf(clientNameStr);
	if (index < 0)
		index = mClientNames.add(clientNameStr);
	else
		clientNameStr = mClientNames.itemAt(index);

	if (index < 0) {
		LOG(ERROR) << "fail to add new client " << clientName;
		return -ENOMEM;
	}

	index = mTuningParameters.indexOfKey(String8(paramName));
	if (index < 0) {
		LOG(ERROR) << "unknown tuning parameter: " << paramName;
		return -EINVAL;
	}

	sp<TuningParameter>& param = mTuningParameters.editValueAt(index);
	int rc = param->set(this, clientNameStr, String8(requestedValue));
	if (rc)
		return rc;
	return param->apply(this);
}

int NetworkPerformanceTuner::removeRequest(const char *paramName, const char *clientName)
{
	int rc;
	String8 clientNameStr(clientName);
	ssize_t index = mClientNames.indexOf(clientNameStr);

	if (index < 0) {
		LOG(ERROR) << "unknown client " << clientName << "(parameter " << paramName << ")";
		return -ENOENT;
	}

	index = mTuningParameters.indexOfKey(String8(paramName));
	if (index < 0) {
		LOG(ERROR) << "unknown tuning parameter " << paramName;
		return -ENOENT;
	}

	sp<TuningParameter>& param = mTuningParameters.editValueAt(index);
	rc = param->remove(this, clientNameStr);
	if (rc)
		return rc;
	return param->apply(this);
}

int NetworkPerformanceTuner::runCommand(SocketClient *cli, int argc, const char *const *argv)
{
	char resultStr[256];
	int code = runCommand(argc, argv, resultStr, sizeof(resultStr));
	cli->sendMsg(code, resultStr, false);
	return 0;
}

int NetworkPerformanceTuner::runCommand(int argc, const char *const *argv,
					char *resultStr, size_t resultLen)
{
	int rc = 0;

	if (argc < 4) {
		strlcpy(resultStr, "Missing argument", resultLen);
		return ResponseCode::CommandSyntaxError;
	}

	//	0	1	2		3		4
	// perftuner	set	paramName	clientName	value
	// perftuner	unset	paramName	clientName
	if (!strcmp(argv[1], "set") && argc == 5) {
		rc = addRequest(argv[2], argv[3], argv[4]);
	} else if (!strcmp(argv[1], "unset") && argc == 4) {
		rc = removeRequest(argv[2], argv[3]);
	} else {
		strlcpy(resultStr, "Unknown perftuner command", resultLen);
		return ResponseCode::CommandSyntaxError;
	}

	if (!rc) {
		strlcpy(resultStr, "perftuner command succeeded", resultLen);
		return ResponseCode::CommandOkay;
	} else {
		strlcpy(resultStr, "perftuner command failed", resultLen);
		return ResponseCode::OperationFailed;
	}
}

NetworkPerformanceTuner::NetworkPerformanceTuner()
{
	LOG(DEBUG) << "constructing NetworkPerformanceTuner";
	registerParameter("rps", new RPSTuningParameter(
		"rps", "/sys/class/net/%s/queues/rx-0/rps_cpus"));
	registerParameter("mtu", new PerInterfaceTuningParameter(
		"mtu", "/sys/class/net/%s/mtu"));
	registerParameter("gro_flush_timeout", new PerInterfaceTuningParameter(
		"gro_flush_timeout", "/sys/class/net/%s/gro_flush_timeout"));
	registerParameter("rmem_max",
			  new IntTuningParameter("rmem_max", "/proc/sys/net/core/rmem_max"));
	registerParameter("rmem_default",
			  new IntTuningParameter("rmem_default", "/proc/sys/net/core/rmem_default"));
	registerParameter("wmem_max",
			  new IntTuningParameter("wmem_max", "/proc/sys/net/core/wmem_max"));
	registerParameter("wmem_default",
			  new IntTuningParameter("wmem_default", "/proc/sys/net/core/wmem_default"));
	registerParameter("udp_rmem_min",
			  new IntTuningParameter("udp_rmem_min", "/proc/sys/net/ipv4/udp_rmem_min"));
	registerParameter("udp_wmem_min",
			  new IntTuningParameter("udp_wmem_min", "/proc/sys/net/ipv4/udp_wmem_min"));
	registerParameter("tcp_limit_output_bytes",
			  new IntTuningParameter("tcp_limit_output_bytes",
						 "/proc/sys/net/ipv4/tcp_limit_output_bytes"));
	registerParameter("tcp_use_userconfig",
			  new IntTuningParameter("tcp_use_userconfig",
						 "/proc/sys/net/ipv4/tcp_use_userconfig"));
	registerParameter("tcp_delack_seg",
			  new IntTuningParameter("tcp_delack_seg",
						 "/proc/sys/net/ipv4/tcp_delack_seg"));
	registerParameter("tcp_window_scaling",
			  new IntTuningParameter("tcp_window_scaling",
						 "/proc/sys/net/ipv4/tcp_window_scaling"));
	registerParameter("tcp_timestamps",
			  new IntTuningParameter("tcp_timestamps",
						 "/proc/sys/net/ipv4/tcp_timestamps"));
	registerParameter("tcp_sack",
			  new IntTuningParameter("tcp_sack", "/proc/sys/net/ipv4/tcp_sack"));
	registerParameter("tcp_rmem",
			  new MemTuningParameter("tcp_rmem", "/proc/sys/net/ipv4/tcp_rmem"));
	registerParameter("tcp_wmem",
			  new MemTuningParameter("tcp_wmem", "/proc/sys/net/ipv4/tcp_wmem"));
	registerParameter("tcp_mem",
			  new MemTuningParameter("tcp_mem", "/proc/sys/net/ipv4/tcp_mem"));
	registerParameter("udp_mem",
			  new MemTuningParameter("udp_mem", "/proc/sys/net/ipv4/udp_mem"));

	// the next block are variables related to VR functionality
	registerParameter("vr_profile", new PerInterfaceTuningParameter(
		"vr_profile", "/sys/class/net/%s/device/wil6210/vr_profile"));
	registerParameter("tx_queue_len", new PerInterfaceTuningParameter(
		"tx_queue_len", "/sys/class/net/%s/tx_queue_len"));
	registerParameter("ipv4_arp_timeout", new PerInterfaceTuningParameter(
		"ipv4_arp_timeout", "/proc/sys/net/ipv4/neigh/%s/base_reachable_time_ms"));
	registerParameter("ipv6_arp_timeout", new PerInterfaceTuningParameter(
		"ipv6_arp_timeout", "/proc/sys/net/ipv6/neigh/%s/base_reachable_time_ms"));

	LOG(DEBUG) << "NetworkPerformanceTuner is constructed";
}

/**
* register a tuning parameter.
* The NetworkPerformanceTuner takes ownership of the parameter
*/
void NetworkPerformanceTuner::registerParameter(const char *name,
						sp<TuningParameter> param)
{
	if (param->init(this)) {
		return;
	}
	mTuningParameters.add(String8(name), param);
}

int NetworkPerformanceTuner::uint32FromFile(const char *fname, uint32_t& value)
{
	uint32_t tmp;
	FILE *f = fopen(fname, "r");
	if (f == NULL)
		return -errno;
	int res = fscanf(f, "%u", &tmp);
	fclose(f);
	if (res < 1)
		return -EINVAL;
	value = tmp;
	return 0;
}

int NetworkPerformanceTuner::uint32ToFile(const char *fname, uint32_t value)
{
	String8 svalue = String8::format("%u", value);
	FILE *f = fopen(fname, "w");
	if (f == NULL)
		return -errno;
	size_t res = fwrite(svalue.string(), 1, svalue.length(), f);
	fclose(f);
	return (res == svalue.length() ? 0 : -EINVAL);
}

int NetworkPerformanceTuner::stringToFile(const char *fname, const char *value)
{
	String8 svalue(value);
	FILE *f = fopen(fname, "w");
	if (f == NULL)
		return -errno;
	size_t res = fwrite(svalue.string(), 1, svalue.length(), f);
	fclose(f);
	return (res == svalue.length() ? 0 : -EINVAL);
}

int NetworkPerformanceTuner::stringFromFile(const char *fname,
					    char *value, size_t length)
{
	FILE *f = fopen(fname, "r");
	if (f == NULL)
		return -errno;
	char *p = fgets(value, length, f);
	fclose(f);
	return (p == value ? 0 : -EINVAL);
}

