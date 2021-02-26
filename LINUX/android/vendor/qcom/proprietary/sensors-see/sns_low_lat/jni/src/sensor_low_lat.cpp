/*============================================================================
@file sensor_low_lat.cpp

@brief
Java Native code to use the Sensors (SNS) Low Latency stream.

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

// Prevent AEEStdDef.h (fastRPC header) from redefining uint64 and int64 types
#define _UINT64_DEFINED
#define _INT64_DEFINED

#include <jni.h>
#include <pthread.h>
#include "rpcmem.h"
#include "log.h"
#include "log_codes.h"
#include "sns_low_lat_buffer.h"
#include "fixed_point.h"
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <utils/Log.h>
#include <common_log.h>
#include <stdbool.h>
#include <time.h>
#include "sns_low_lat_stream.h"
#include "sensor_low_lat.h"
#include <cutils/properties.h>
#include <utils/Condition.h>
#include <string>
#include <vector>
#include <iostream>
#include <cinttypes>
#include <unistd.h>
#include "ssc_connection.h"
#include "ssc_utils.h"
#include "sns_std_sensor.pb.h"
#include "sns_std_type.pb.h"


#undef LOG_TAG
#define LOG_TAG "sensor_low_lat"

#define SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE  120
#define SENSOR_LOW_LAT_DEFAULT_OFFSET    0
#define SENSOR_LOW_LAT_SLEEP_US          5000

#ifndef LOG_SNS_HDRLL_DATA_CLIENT_C
    #define LOG_SNS_HDRLL_DATA_CLIENT_C  0x19B9
#endif /* LOG_SNS_HDRLL_DATA_CLIENT_C */

#define SNS_LOG_HDRLL_DATA_NUM_AXES      3
#define SAMPLE_PERIOD_US                 1250 // 800 hz

#define SNS_TIMESTAMP_DSPS               0
#define SNS_TIMESTAMP_APPS               1
#define SNS_TIMESTAMP_MODEM              2

#define USEC_PER_SEC                     1000000L
#define NSEC_PER_SEC                     1000000000L

#define SEC_TO_USEC(sec)                 ((sec)*USEC_PER_SEC)
#define SEC_TO_NSEC(sec)                 ((sec)*NSEC_PER_SEC)
#define NSEC_TO_USEC(nsec)               ((nsec)/1000L)

#define USEC_TO_NSEC(usec)               ((usec)*1000L)
#define USEC_TO_SEC(usec)                ((usec)/USEC_PER_SEC)

#if defined(__aarch64__)
    #define TARGET_ARM64
#elif defined(__arm__)
    #define TARGET_ARM
#else
    #error "target cpu architecture not supported"
#endif

typedef int32_t q16_t;

#define SNS_LOG_HDRLL_DATA_CLIENT_VERSION   1
typedef PACK(struct) {
    log_hdr_type log_type;       /**< Type, as defined in log_codes.h */
    uint32_t     version;        /**< Version of the log packet */
    uint32_t     sensor_id_1;    /**< The sensor ID for the first set of data */
    uint64_t     timestamp_1;    /**< Timestamp for the first set of data (in SSC ticks) */
    q16_t        data_1[3];      /**< First set of data in Q16 */
    uint32_t     sensor_id_2;    /**< The sensor ID for the second set of data */
    uint64_t     timestamp_2;    /**< Timestamp for the second set of data (in SSC ticks) */
    q16_t        data_2[3];      /**< second set of data in Q16 */
    uint8_t      stream_id;      /**< Stream that this data belongs to */
    uint8_t      log_timestamp_type; /**< Source of the timestamp */
    uint64_t     log_timestamp;  /**< Timestamp that this log packet was generated */
} sns_log_hdrll_data_client;

typedef struct sns_low_lat_client
{
    struct sns_low_lat_client *next;
    uint8 *low_lat_buf;
    int sns_handle;
} sns_low_lat_client;


using namespace android;

Condition _cv;
Mutex _mutex;

// PEND: This class will allow for better abstraction to deal with concurrent JNI clients
///*============================================================================
// * Class sensor_low_lat
// *===========================================================================*/
//class sensor_low_lat
//{
//};

/*============================================================================
 Variable Definitions
 ===========================================================================*/


/*============================================================================
 Static Variable Definitions
 ===========================================================================*/

// PEND: These static variables should, ideally, be encompassed in a C++ object
static uint8* output_buf = 0;
static uint32 circ_buf_iter = 0;
static pthread_t worker_thread;
static boolean active = false;
sns_low_lat_client *low_lat_clients;
static int mutex_counter = 0;

const uint64_t NSEC_PER_SEC_VAL = 1000000000ull;

uint64_t qtimer_ticks_to_ns(uint64_t ticks);
uint64_t qtimer_get_freq();
uint64_t qtimer_get_time_ns();
uint64_t qtimer_get_ticks();


/*============================================================================
 Static Method Definitions
 ===========================================================================*/

/**
 * @brief convert the qtimer tick value of nanoseconds
 * @param ticks
 * @return uint64_t qtimer time in nanoseconds
 */
uint64_t qtimer_ticks_to_ns(uint64_t ticks)
{
    return (uint64_t)((double)ticks * ((double)NSEC_PER_SEC_VAL / (double)qtimer_get_freq()));
}

/**
 * @brief get QTimer frequency in Hz
 * @return uint64_t
 */
uint64_t qtimer_get_freq()
{
  #if defined(TARGET_ARM64)
    uint64_t val = 0;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
  #else
    uint32_t val = 0;
    asm volatile("mrc p15, 0, %[val], c14, c0, 0" : [val] "=r" (val));
    return val;
  #endif
}

/**
 * @brief get time in nanoseconds since boot-up
 * @return uint64_t nanoseconds
 */
uint64_t qtimer_get_time_ns()
{
    return qtimer_ticks_to_ns(qtimer_get_ticks());
}

/**
  * @brief reads the current QTimer count value
  * @return uint64_t QTimer tick-count
  */
uint64_t qtimer_get_ticks()
{
  #if defined(TARGET_ARM64)
    unsigned long long val = 0;
    asm volatile("mrs %0, cntvct_el0" : "=r" (val));
    return val;
  #else
    uint64_t val;
    unsigned long lsb = 0, msb = 0;
    asm volatile("mrrc p15, 1, %[lsb], %[msb], c14"
                 : [lsb] "=r" (lsb), [msb] "=r" (msb));
    val = ((uint64_t)msb << 32) | lsb;
    return val;
  #endif
}

static void sensor_low_lat_log_samples(sns_low_lat_sample* sample_1, sns_low_lat_sample* sample_2)
{
    int a;
    struct timespec time;
    sns_log_hdrll_data_client* log_pkt;

    // Don't bother logging anything if neither sample is present
    if ((NULL == sample_1) && (NULL == sample_2)) {
        return;
    }

    log_pkt = (sns_log_hdrll_data_client*) log_alloc(LOG_SNS_HDRLL_DATA_CLIENT_C, sizeof(*log_pkt));

    if (NULL != log_pkt) {
        log_pkt->version = SNS_LOG_HDRLL_DATA_CLIENT_VERSION;

        // Log the first sample, if it's available
        if (NULL != sample_1) {
            if (SNS_LOW_LAT_ACCEL == sample_1->sensor_type) {
                log_pkt->sensor_id_1 = 1;
            }
            else {
                log_pkt->sensor_id_1 = 3;
            }
            // Convert the timestamp from QTimer ticks to SSC ticks
            log_pkt->timestamp_1 = (sample_1->timestamp << 4) / 9375;
            for (a = 0; a < SNS_LOG_HDRLL_DATA_NUM_AXES; a++)
            {
              log_pkt->data_1[a] = FX_FLTTOFIX_Q16(sample_1->sample[a]);
            }
        }

        // Log the second sample in the expected pair, if it's available
        if (NULL != sample_2) {
            if (SNS_LOW_LAT_ACCEL == sample_2->sensor_type) {
                log_pkt->sensor_id_2 = 1;
            }
            else {
                log_pkt->sensor_id_2 = 3;
            }
            // Convert the timestamp from QTimer ticks to SSC ticks
           log_pkt->timestamp_2 = (sample_2->timestamp << 4) / 9375;
           for (a = 0; a < SNS_LOG_HDRLL_DATA_NUM_AXES; a++)
           {
               log_pkt->data_2[a] = FX_FLTTOFIX_Q16(sample_2->sample[a]);
           }
        }

        // Currently the client does not know the stream ID so we can't publish it.
        //log_pkt->stream_id = stream_id;
        log_pkt->log_timestamp_type = SNS_TIMESTAMP_APPS;
        clock_gettime( CLOCK_REALTIME, &time );
        log_pkt->log_timestamp = SEC_TO_USEC((uint64_t)time.tv_sec) + NSEC_TO_USEC((uint64_t)time.tv_nsec);

        log_commit(log_pkt);
    }
}

static void *sensor_low_lat_polling(void* arg)
{
    int error_code;
    uint32 most_recent_idx;
    uint32 iter_stop_idx;
    sns_low_lat_sample* sample_1 = NULL;
    sns_low_lat_sample* sample_2 = NULL;

    sns_low_lat_circ_buf* circ_buf = (sns_low_lat_circ_buf*) output_buf;

    // Constantly poll the stream for new samples
    while (active)
    {
        error_code = sns_low_lat_stream_poll((unsigned long int)arg);

        // Log the data if the poll was successful
        if (0 == error_code) {
            most_recent_idx = circ_buf->header.info.most_recent_index;
            iter_stop_idx = most_recent_idx + 1;
            if (iter_stop_idx >= circ_buf->header.info.max_samples) {
                iter_stop_idx = 0;
            }

            // Iterate through the circular buffer starting with where we last left off
            do
            {
                // Retrieve the first sample
                sample_1 = &circ_buf->samples[circ_buf_iter].data;
                // Increment the iterator for the next sample
                circ_buf_iter++;
                if (circ_buf_iter >= circ_buf->header.info.max_samples) {
                    circ_buf_iter = 0;
                }

                // Break out of this loop if the iter hits the stopping point
                if (circ_buf_iter == iter_stop_idx) {
                    break;
                }

                // Retrieve the second sample
                sample_2 = &circ_buf->samples[circ_buf_iter].data;

                // Increment the iterator for the next sample
                circ_buf_iter++;
                if (circ_buf_iter >= circ_buf->header.info.max_samples) {
                    circ_buf_iter = 0;
                }

                // Log the samples and nullify the pointers so they can be reused safely
                sensor_low_lat_log_samples(sample_1, sample_2);
                sample_1 = NULL;
                sample_2 = NULL;
             } while (circ_buf_iter != iter_stop_idx);

             // Log any unlogged samples
             sensor_low_lat_log_samples(sample_1, sample_2);
             sample_1 = NULL;
             sample_2 = NULL;
        }
        else {
            LOGE("%s: error polling low lat stream %d", __FUNCTION__, error_code);
        }
    }
    pthread_exit(NULL);
}


/*============================================================================
 SSC Channel Interactions for getting max rates supported by driver
 ===========================================================================*/

std::vector<float> _supported_rates;

static void
_cb(const uint8_t *data, size_t size)
{
  sns_client_event_msg pb_event_msg;
  pb_event_msg.ParseFromArray(data, size);
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    ALOGE("event[%d] msg_id=%d, ts=%llu", i, pb_event.msg_id(),
    (unsigned long long) pb_event.timestamp());
    if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == pb_event.msg_id())
    {
    sns_std_attr_event attr_event;
    attr_event.ParseFromString(pb_event.payload());
    for (int i = 0; i < attr_event.attributes_size(); i++) {
      ALOGE("attribute ID :%d", attr_event.attributes(i).attr_id());
      if(attr_event.attributes(i).attr_id() == SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES) {
        const sns_std_attr_value& attr_value = attr_event.attributes(i).value();
        ALOGE("recevied SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES ,number of supported rates %d", (int)attr_value.values_size());
        for (int i = 0; i< attr_value.values_size(); i++) {
          if (attr_value.values(i).has_flt()) {
            _supported_rates.push_back(attr_value.values(i).flt());
          }
        }
        _cv.signal();
        }
      }
    }
  }
}

void send_attr_request(ssc_connection *conn, sensor_uid const *suid)
{
    sns_client_request_msg pb_req_msg;
    sns_std_attr_req pb_attr_req;
    std::string pb_attr_req_encoded;
    std::string pb_req_msg_encoded;

    /* populate the pb_attr_req message */
    pb_attr_req.set_register_updates(false);
    pb_attr_req.SerializeToString(&pb_attr_req_encoded);
    /* populate the client request message */
    pb_req_msg.set_msg_id(SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    pb_req_msg.mutable_request()->set_payload(pb_attr_req_encoded);
    pb_req_msg.mutable_suid()->set_suid_high(suid->high);
    pb_req_msg.mutable_suid()->set_suid_low(suid->low);
    pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(pb_req_msg_encoded);
}

void get_supportedrates(sensor_uid suid) {
  static ssc_connection *connection;
  /*clear data before asking*/
  ALOGE("clear older ones");
  if (!_supported_rates.empty())
    _supported_rates.clear();
  connection = new ssc_connection(_cb);
  send_attr_request(connection, &suid);
  _mutex.lock();
  if (!_cv.waitRelative(_mutex, 3*NSEC_PER_SEC_VAL)) {
    ALOGE("failed to get the attributes for suid_low: %lld suid_high: %lld",
        (long long int)suid.high, (long long int)suid.low);
  }
  _mutex.unlock();
  delete connection;
}

/*============================================================================
 JNI Method Definitions
 ===========================================================================*/

/**
 * Initializes the stream state with the framework.
 *
 * @param bufferSize The desired size of the circular buffer for the output samples.
 * @throws
 */
/*
 * Class:     com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    maxrate
 * Signature: ([JII)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_getmaxrate
(JNIEnv *env, jobject obj, jlongArray suids)
{
    int       fd;
    int       error_code;
    jclass    mClass = env->FindClass("com/qualcomm/qti/usta/core/SensorLowLatencyJNI");
    jboolean  sensor_suid_is_copy;
    jlong*    sensor_suid = env->GetLongArrayElements( suids, &sensor_suid_is_copy);
    char      err_msg[SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE];
    sensor_uid sns_suid;
    jfloat max_rate;
    jint   max_supported_rate = -1;
    sns_suid.low = 0;
    sns_suid.high = 0;

    if (NULL == mClass) {
      // PEND: This prevents concurrent JNI clients. This limitation should be removed there's a need to support concurrent clients.
      jclass state_exception;

      LOGE("%s: can't find the object's class", __FUNCTION__);
      state_exception = env->FindClass("java/lang/IllegalStateException");
      if (state_exception != NULL) {
          env->ThrowNew(state_exception, "can't find the object's class");
      }
      return -1;
    }

    if (sensor_suid_is_copy == JNI_TRUE ) {
        sns_suid.low = sensor_suid[0];
        sns_suid.high= sensor_suid[1];
    }

    ALOGE("%s: suid_low 0x%llx\n", __FUNCTION__, (unsigned long long)sns_suid.low);
    ALOGE("%s: suid_high 0x%llx\n", __FUNCTION__, (unsigned long long)sns_suid.high);
    get_supportedrates(sns_suid);
    if (!_supported_rates.empty())
      max_supported_rate = _supported_rates.at(_supported_rates.size()-1);
    else
      ALOGE("empty supported SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES");

    env->ReleaseLongArrayElements(suids, sensor_suid, 0);
    ALOGE("max_supported rate %d", max_supported_rate);
    return max_supported_rate;
}

/**
 * Initializes the stream state with the framework.
 *
 * @param bufferSize The desired size of the circular buffer for the output samples.
 * @throws
 */
/*
 * Class:     com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    getchannel
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_getchannel
    (JNIEnv *env, jobject obj, jint buffer_size)
{
    int       fd;
    int       error_code;
    int       stream_handle = 0;
    jclass    mClass = env->FindClass("com/qualcomm/qti/usta/core/SensorLowLatencyJNI");
    jfieldID  field_id;
    char      err_msg[SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE];
    sns_low_lat_client *low_lat_client, *low_lat_client_temp;

    // Validate input
    if (buffer_size <= 0) {
        jclass input_exception;

        LOGE("%s: invalid buffer size %d", __FUNCTION__, buffer_size);
        input_exception = env->FindClass("java/lang/IllegalArgumentException");
        if (input_exception != NULL) {
          snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Invalid buffer_size %d", buffer_size);
          env->ThrowNew(input_exception, err_msg);
        }
        return -1;
    }

    if (NULL == mClass) {
        // PEND: This prevents concurrent JNI clients. This limitation should be removed there's a need to support concurrent clients.
        jclass state_exception;

        LOGE("%s: can't find the object's class", __FUNCTION__);
        state_exception = env->FindClass("java/lang/IllegalStateException");
        if (state_exception != NULL) {
            env->ThrowNew(state_exception, "can't find the object's class");
        }
        return -1;
    }

    low_lat_client_temp = (sns_low_lat_client *)malloc(sizeof(sns_low_lat_client));
    if (!low_lat_client_temp) {
        LOGE("%s: malloc failed.", __FUNCTION__);
        return -1;
    }
    memset(low_lat_client_temp, 0, sizeof(sns_low_lat_client));

    if (!low_lat_clients) {
        low_lat_clients = low_lat_client_temp;
        low_lat_client = low_lat_clients;
    }
    else {
        low_lat_client = low_lat_clients;
        while (low_lat_client->next != NULL)
        {
            low_lat_client = low_lat_client->next;
        }
        low_lat_client->next = low_lat_client_temp;
        low_lat_client = low_lat_client->next;
    }

    if (mutex_counter == 0) {
        rpcmem_init();
    }
    mutex_counter++;

    // Allocate the shared memory buffer and get the FD for that buffer
    output_buf = (uint8*)rpcmem_alloc(RPCMEM_DEFAULT_HEAP, RPCMEM_DEFAULT_FLAGS, buffer_size);
    if (NULL == output_buf) {
        jclass null_ptr_exception;

        LOGE("%s: rpcmem_alloc failed.", __FUNCTION__);
        null_ptr_exception = env->FindClass("java/lang/NullPointerException");
        if (null_ptr_exception != NULL) {
            env->ThrowNew(null_ptr_exception, "rpcmem_alloc failed");
        }
        return -1;
    }

    low_lat_client->low_lat_buf = output_buf;
    fd = rpcmem_to_fd(output_buf);

    error_code = sns_low_lat_stream_init(fd, buffer_size, SENSOR_LOW_LAT_DEFAULT_OFFSET, &stream_handle);
    if (0 != error_code) {
        jclass state_exception;

        rpcmem_free(output_buf);
        output_buf = NULL;

        LOGE("%s: sns_low_lat_stream_init failed! error_code %d", __FUNCTION__, error_code);
        state_exception = env->FindClass("java/lang/IllegalStateException");
        if (state_exception != NULL) {
            snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Init failed! error_code %d", error_code);
           env->ThrowNew(state_exception, err_msg);
        }
        return -1;
    }

    low_lat_client->sns_handle = stream_handle;
    low_lat_client->next = NULL;
    return stream_handle;
}

/*
 * Class:     com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    start
 * Signature: ([JIII)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_start
    (JNIEnv *env, jobject obj, jlongArray suids, jint sample_period_us, jint flags, jint stream_handle)
{
    int       error_code;
    jclass    mClass = env->FindClass( "com/qualcomm/qti/usta/core/SensorLowLatencyJNI");
    jfieldID  field_id;
    jsize     suids_len = env->GetArrayLength( suids);
    jboolean  sensor_suid_is_copy;
    jlong*    sensor_suid = env->GetLongArrayElements( suids, &sensor_suid_is_copy);
    jint      sensor_handle = 0;
    char      err_msg[SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE];
    int64 timestamp_offset;
    struct timespec ts;
    sns_std_suid_t sns_suid;
    char prop_val[PROPERTY_VALUE_MAX] = {0};
    sns_low_lat_client *low_lat_client = low_lat_clients;
    sns_suid.suid_low = 0;
    sns_suid.suid_high = 0;

    clock_gettime(CLOCK_BOOTTIME, &ts);

    // Validate input
    if (suids_len <= 0) {
        jclass input_exception;

        LOGE("%s: invalid suids_len %d", __FUNCTION__, suids_len);
        input_exception = env->FindClass("java/lang/IllegalArgumentException");
        if (input_exception != NULL) {
            snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Invalid suids_len %d", suids_len);
            env->ThrowNew( input_exception, err_msg);
        }
        return;
    }
    if (sensor_suid_is_copy == JNI_TRUE ) {
        sns_suid.suid_low = sensor_suid[0];
        sns_suid.suid_high= sensor_suid[1];
    }
    LOGE("%s: suid_low 0x%llx\n", __FUNCTION__, (unsigned long long)sns_suid.suid_low);
    LOGE("%s: suid_high 0x%llx\n", __FUNCTION__, (unsigned long long)sns_suid.suid_high);

    if (sample_period_us <= 0) {
        jclass input_exception;

        LOGE("%s: invalid sample_period_us", __FUNCTION__, sample_period_us);
        input_exception = env->FindClass( "java/lang/IllegalArgumentException");
        if (input_exception != NULL) {
            snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Invalid sample_period_us %d", sample_period_us);
            env->ThrowNew( input_exception, err_msg);
        }
        return;
    }

    LOGE("%s: stream handle 0x%x\n", __FUNCTION__, (int)stream_handle);

    while(low_lat_client != NULL)
    {
        if (low_lat_client->sns_handle == stream_handle) {
            break;
        }
        low_lat_client = low_lat_client->next;
    }
    if (low_lat_client == NULL)
       return;

    output_buf = low_lat_client->low_lat_buf;
    if (NULL == output_buf) {
        jclass state_exception;

        LOGE("%s: no available low-lat stream to start!", __FUNCTION__);
        state_exception = env->FindClass("java/lang/IllegalStateException");
        if (state_exception != NULL) {
            env->ThrowNew(state_exception, "There's no available low lat stream to start, try calling init first");
        }
        return;
    }

    // Start the stream
    timestamp_offset = SEC_TO_NSEC(ts.tv_sec) + ts.tv_nsec - (int64_t)qtimer_get_time_ns();
    LOGE("%s: timestamp offset 0x%llx\n", __FUNCTION__, (unsigned long long)timestamp_offset);
    error_code = sns_low_lat_stream_config((int)stream_handle, (int64)timestamp_offset, &sns_suid, (unsigned int)sample_period_us, (unsigned int)flags, (int)sensor_handle);
    if (0 != error_code) {
        jclass state_exception;

        LOGE("%s: sns_low_lat_stream_config failed! error_code %d", __FUNCTION__, error_code);
        state_exception = env->FindClass("java/lang/IllegalStateException");
        if (state_exception != NULL) {
            snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Start failed! error_code %d", error_code);
            env->ThrowNew(state_exception, err_msg);
        }
        return;
    }

#if 0
    //Spawn a worker thread for polling the shared memory
    active = true;
    circ_buf_iter = 0;
    error_code = pthread_create(&worker_thread, NULL, sensor_low_lat_polling, (void *)(intptr_t)stream_handle);
    if (0 != error_code) {
        jclass state_exception;

        active = false;
        LOGE("%s: pthread_create failed! error_code %d", __FUNCTION__, error_code);
        state_exception = env->FindClass("java/lang/IllegalStateException");
        if (state_exception != NULL) {
            snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Unable to create a worker thread! error_code %d", error_code);
            env->ThrowNew(state_exception, err_msg);
        }
        return;
    }
#endif
    // TODO: MAKE SURE THIS IS CALLED BEFORE ALL OF THE RETURNS SO MEMORY ISN'T LEAKED
    // Release the sensor_suid array so that the Java garbage collector knows
    // it's okay to free the sensor_suid array
    env->ReleaseLongArrayElements(suids, sensor_suid, 0);
}

/*
 * Class:     com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    stop
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_stop
    (JNIEnv *env, jobject obj, jint stream_handle)
{
    int       error_code;
    char      err_msg[SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE];

    // Signal the worker thread to exit and wait a little bit
    usleep(SENSOR_LOW_LAT_SLEEP_US);

    // Stop the stream
    error_code = sns_low_lat_stream_stop((int)stream_handle);
    if (0 != error_code) {
        jclass state_exception;

        LOGE("%s: sns_low_lat_stream_stop failed! error_code %d", __FUNCTION__, error_code);
        state_exception = env->FindClass("java/lang/IllegalStateException");
        if (state_exception != NULL) {
            snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Stop failed! error_code %d", error_code);
            env->ThrowNew(state_exception, err_msg);
        }
        return;
    }
}

/*
 * Class:     com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    deinit
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_closechannel
    (JNIEnv *env, jobject obj, jint stream_handle)
{
    int       error_code;
    jclass    mClass = env->FindClass("com/qualcomm/qti/usta/core/SensorLowLatencyJNI");
    jfieldID  field_id;
    char      err_msg[SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE];
    sns_low_lat_client *low_lat_client = low_lat_clients, *low_lat_client_prev = NULL;

    error_code = sns_low_lat_stream_deinit((int)stream_handle);
    if (0 != error_code) {
        jclass state_exception;

        LOGE("%s: sns_low_lat_stream_deinit failed! error_code %d", __FUNCTION__, error_code);
        state_exception = env->FindClass( "java/lang/IllegalStateException");
       if (state_exception != NULL) {
           snprintf(err_msg, SENSOR_LOW_LAT_ERR_MSG_MAX_SIZE, "Deinit failed! error_code %d", error_code);
           env->ThrowNew(state_exception, err_msg);
       }
       return ;
    }

    while(low_lat_client != NULL)
    {
        if (low_lat_client->sns_handle == stream_handle)
            break;

        if (low_lat_client->next != NULL) {
            low_lat_client_prev = low_lat_client;
            low_lat_client = low_lat_client->next;
        }
    }
    if (low_lat_client == NULL)
       return;

    output_buf = low_lat_client->low_lat_buf;

    if (low_lat_client->next == NULL) { //Last and only one node
        if (low_lat_client_prev != NULL) {
            low_lat_client_prev->next = NULL;
        } else { //only one node
            if (low_lat_client == low_lat_clients)
                low_lat_clients = NULL;
            }
    } else { // middle
        if (low_lat_client_prev != NULL) { //middle
            if (low_lat_client->next == NULL) {
                low_lat_client_prev->next = NULL;
            } else {
                low_lat_client_prev->next = low_lat_client->next;
            }
        } else { // first node
            low_lat_clients = low_lat_client->next;
        }
    }

    if (NULL != output_buf) {
        rpcmem_free(output_buf);
        low_lat_client->next = NULL;
        free (low_lat_client);
        output_buf = NULL;
    }

    mutex_counter--;
    if (mutex_counter == 0) {
        rpcmem_deinit();
    }
}
