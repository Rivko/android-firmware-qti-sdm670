/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _NETWORK_PERFORMANCE_TUNER_H
#define _NETWORK_PERFORMANCE_TUNER_H

#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include <utils/SortedVector.h>
#include <utils/String8.h>
#include <utils/StrongPointer.h>
#include <sysutils/SocketClient.h>
#include <stdint.h>

using namespace android;

class NetworkPerformanceTuner;

/**
 * root class for a tuning parameter. The tuning parameter records
 * tuning requests from tuning clients, merges tuning requests in
 * order to calculate an optimal value for tuning parameter, and
 * applies the tuning value when needed.
 * Tuning parameter classes need to implement the following:
 * - init: initializes the parameter. Called by network performance
 *  tuner when it is constructed.
 * - set: add or update a tuning request from a client.
 * - remove: removes a tuning request for a client
 * - apply: merges tuning requests and apply the tuning parameter
 */
class TuningParameter : public RefBase {
public:
	TuningParameter() {}
	TuningParameter(const char *name) : mName(name) {}
	String8 getName() {return mName;}
	virtual int init(NetworkPerformanceTuner *tuner) { return -EINVAL; }
	virtual int set(NetworkPerformanceTuner *tuner,
			String8 clientName, String8 value) { return -EINVAL; }
	virtual int remove(NetworkPerformanceTuner *tuner,
			   String8 clientName) { return -EINVAL; }
	virtual int apply(NetworkPerformanceTuner *tuner) { return -EINVAL; }
	virtual ~TuningParameter() {}
protected:
	String8 mName;
};

/**
 * A generic tuning parameter which maps to a single file
 * in sysfs/procfs with an uint32_t value.
 * Merges tuning requests by calculating the maximum value.
 * Records the initial value by reading the file when initialized.
 */
class IntTuningParameter : public TuningParameter {
public:
	IntTuningParameter(const char *name,
			   const char *path);
	virtual int init(NetworkPerformanceTuner *tuner);
	virtual int set(NetworkPerformanceTuner *tuner,
			String8 clientName, String8 value);
	virtual int remove(NetworkPerformanceTuner *tuner,
			   String8 clientName);
	virtual int apply(NetworkPerformanceTuner *tuner);
protected:
	KeyedVector<String8, uint32_t> mRequests;
	String8 mPath;
	uint32_t mInitialValue;
};

/**
 * A tuning parameter for RPS (receive packet steering)
 * configuration. Supports a single RX queue.
 * The procfs path is dynamic. Use %s where the interface
 * name should be.
 * No merging is done and the value specified in the request
 * is applied as-is for each interface
 * (the client name is used as interface name).
 * Also since RPS value is independent for each interface and does
 * not affect other networks, do not restore default value when
 * client is removed.
 * This is mainly provided for clients that do not have permissions
 * to set tuning parameters (such as WIGIG).
 */
class RPSTuningParameter : public IntTuningParameter
{
public:
	RPSTuningParameter(const char *name, const char *path);
	virtual int init(NetworkPerformanceTuner *tuner);
	virtual int apply(NetworkPerformanceTuner *tuner);
};

/**
 * a generic per-interface tuning parameter with int value
 * The procfs path is dynamic. Use %s where the interface
 * name should be.
 * No merging is done and the value specified in the request
 * is applied as-is for each interface
 * (the client name is used as interface name).
 * Also parameter will not be changed when client removes
 * a request, only when a request is added/updated
 * This is mainly provided for clients that do not have permissions
 * to set tuning parameters (such as WIGIG).
 *
 */
class PerInterfaceTuningParameter : public IntTuningParameter
{
public:
	PerInterfaceTuningParameter(const char *name, const char *path);
	virtual int init(NetworkPerformanceTuner *tuner);
	virtual int apply(NetworkPerformanceTuner *tuner);
};

/**
 * A tuning parameter for socket buffer sizes such as tcp_mem,
 * udp_mem.
 * Tuning value composed of 3 numbers: min, default, max
 * Merges tuning requests by calculating maximum of each of the
 * 3 numbers independently
 * On initialization stores default value by reading from the file.
 */
typedef struct {
	uint32_t minVal, defVal, maxVal;
} MemTuningRequest;

class MemTuningParameter : public TuningParameter {
public:
	MemTuningParameter(const char *name,
			   const char *path);
	virtual int init(NetworkPerformanceTuner *tuner);
	virtual int set(NetworkPerformanceTuner *tuner,
			String8 clientName, String8 value);
	virtual int remove(NetworkPerformanceTuner *tuner,
			   String8 clientName);
	virtual int apply(NetworkPerformanceTuner *tuner);
private:
	KeyedVector<String8, MemTuningRequest> mRequests;
	String8 mPath;
	MemTuningRequest mInitialValue;
};

class NetworkPerformanceTuner : public RefBase {
public:
	static sp<NetworkPerformanceTuner> getInstance();

	int addRequest(const char *paramName, const char *clientName,
			const char *requestedValue);
	int removeRequest(const char *paramName, const char *clientName);
	int runCommand(SocketClient *cli, int argc, const char *const *argv);
	int runCommand(int argc, const char *const *argv,
		       char *resultStr, size_t resultLen);
public:
	// utilities
	int uint32FromFile(const char *fname, uint32_t& value);
	int uint32ToFile(const char *fname, uint32_t value);
	int stringToFile(const char *fname, const char *value);
	int stringFromFile(const char *fname, char *value, size_t length);
private:
	NetworkPerformanceTuner();
	void registerParameter(const char *name,
			       sp<TuningParameter> param);
private:
	static sp<NetworkPerformanceTuner> sInstance;
	KeyedVector<String8, sp<TuningParameter> > mTuningParameters;
	SortedVector<String8> mClientNames;
};

#endif // _NETWORK_PERFORMANCE_TUNER_H

