/*============================================================================
  @file sns_hidl_hal_batch.cpp

  @brief
  Test the QTI Hybrid approach at the HAL layer using HIDL.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ==========================================================================*/

#include <iostream>
#include <stdio.h>
#include "android/hardware/sensors/1.0/ISensors.h"
#include "hardware/sensors.h"
#include "convert.h"
#include "thread"
#include <vector>
#include <algorithm>
#include <utils/SystemClock.h>
#include <signal.h>
#include <limits>

using namespace android;
using namespace std;
using namespace android::hardware::sensors::V1_0;
using namespace android::hardware::sensors::V1_0::implementation;

#define PRINT_SENSOR_SAMPLES 1

/* Copied from sensors_hal.h */
#define QTI_SENSOR_TYPE_BASE 33171000

#define NSEC_PER_MS 1000000
#define NUM_POLL_EVENTS 100
#define FAKE_SIGNAL 0xFFFFF

vector <int32_t> enabledClients;

static const char *type_to_string(int type)
{
  return (type == SENSOR_TYPE_ACCELEROMETER) ? SENSOR_STRING_TYPE_ACCELEROMETER :
    (type == SENSOR_TYPE_GEOMAGNETIC_FIELD) ? SENSOR_STRING_TYPE_MAGNETIC_FIELD :
    (type == SENSOR_TYPE_ORIENTATION) ? SENSOR_STRING_TYPE_ORIENTATION :
    (type == SENSOR_TYPE_GYROSCOPE) ? SENSOR_STRING_TYPE_GYROSCOPE :
    (type == SENSOR_TYPE_LIGHT) ? SENSOR_STRING_TYPE_LIGHT :
    (type == SENSOR_TYPE_PRESSURE) ? SENSOR_STRING_TYPE_PRESSURE :
    (type == SENSOR_TYPE_TEMPERATURE) ? SENSOR_STRING_TYPE_TEMPERATURE :
    (type == SENSOR_TYPE_PROXIMITY) ? SENSOR_STRING_TYPE_PROXIMITY :
    (type == SENSOR_TYPE_GRAVITY) ? SENSOR_STRING_TYPE_GRAVITY :
    (type == SENSOR_TYPE_LINEAR_ACCELERATION) ? SENSOR_STRING_TYPE_LINEAR_ACCELERATION :
    (type == SENSOR_TYPE_ROTATION_VECTOR) ? SENSOR_STRING_TYPE_ROTATION_VECTOR :
    (type == SENSOR_TYPE_RELATIVE_HUMIDITY) ? SENSOR_STRING_TYPE_RELATIVE_HUMIDITY :
    (type == SENSOR_TYPE_AMBIENT_TEMPERATURE) ? SENSOR_STRING_TYPE_AMBIENT_TEMPERATURE :
    (type == SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED) ? SENSOR_STRING_TYPE_MAGNETIC_FIELD_UNCALIBRATED :
    (type == SENSOR_TYPE_GAME_ROTATION_VECTOR) ? SENSOR_STRING_TYPE_GAME_ROTATION_VECTOR :
    (type == SENSOR_TYPE_GYROSCOPE_UNCALIBRATED) ? SENSOR_STRING_TYPE_GYROSCOPE_UNCALIBRATED :
    (type == SENSOR_TYPE_SIGNIFICANT_MOTION) ? SENSOR_STRING_TYPE_SIGNIFICANT_MOTION :
    (type == SENSOR_TYPE_STEP_DETECTOR) ? SENSOR_STRING_TYPE_STEP_DETECTOR :
    (type == SENSOR_TYPE_STEP_COUNTER) ? SENSOR_STRING_TYPE_STEP_COUNTER :
    (type == SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR) ? SENSOR_STRING_TYPE_GEOMAGNETIC_ROTATION_VECTOR :
    (type == SENSOR_TYPE_HEART_RATE) ? SENSOR_STRING_TYPE_HEART_RATE :
    (type == SENSOR_TYPE_TILT_DETECTOR) ? SENSOR_STRING_TYPE_TILT_DETECTOR :
    (type == SENSOR_TYPE_WAKE_GESTURE) ? SENSOR_STRING_TYPE_WAKE_GESTURE :
    (type == SENSOR_TYPE_GLANCE_GESTURE) ? SENSOR_STRING_TYPE_GLANCE_GESTURE :
    (type == SENSOR_TYPE_PICK_UP_GESTURE) ? SENSOR_STRING_TYPE_PICK_UP_GESTURE :
#ifdef SENSORS_DEVICE_API_VERSION_1_1
    (type == SENSOR_TYPE_META_DATA) ? "Meta Data (FLUSH_CMPLT)" :
#endif
    (type == QTI_SENSOR_TYPE_BASE) ? "com.qti.sensor.basic_gestures" :
    (type == QTI_SENSOR_TYPE_BASE + 1) ? "com.qti.sensor.tap" :
    (type == QTI_SENSOR_TYPE_BASE + 2) ? "com.qti.sensor.facing" :
    (type == QTI_SENSOR_TYPE_BASE + 3) ? "com.qti.sensor.tilt" :
    (type == QTI_SENSOR_TYPE_BASE + 4) ? "com.qti.sensor.fns" :
    (type == QTI_SENSOR_TYPE_BASE + 5) ? "com.qti.sensor.bte" :
    (type == QTI_SENSOR_TYPE_BASE + 6) ? "com.qti.sensor.amd" :
    (type == QTI_SENSOR_TYPE_BASE + 7) ? "com.qti.sensor.rmd" :
    (type == QTI_SENSOR_TYPE_BASE + 8) ? "com.qti.sensor.vmd" :
    (type == QTI_SENSOR_TYPE_BASE + 9) ? "com.qti.sensor.pedometer" :
    (type == QTI_SENSOR_TYPE_BASE + 10) ? "com.qti.sensor.pam" :
    (type == QTI_SENSOR_TYPE_BASE + 11) ? "com.qti.sensor.motion_accel" :
    (type == QTI_SENSOR_TYPE_BASE + 12) ? "com.qti.sensor.cmc" :
    (type == QTI_SENSOR_TYPE_BASE + 13) ? "com.qti.sensor.rgb" :
    (type == QTI_SENSOR_TYPE_BASE + 14) ? "com.qti.sensor.ir_gesture" :
    (type == QTI_SENSOR_TYPE_BASE + 15) ? "com.qti.sensor.sar" :
    (type == QTI_SENSOR_TYPE_BASE + 16) ? "com.qti.sensor.hall_effect" :
    (type == QTI_SENSOR_TYPE_BASE + 17) ? "com.qti.sensor.fast_amd" :
    (type == QTI_SENSOR_TYPE_BASE + 18) ? "com.qti.sensor.uv" :
    (type == QTI_SENSOR_TYPE_BASE + 19) ? "com.qti.sensor.thermopile" :
    (type == QTI_SENSOR_TYPE_BASE + 20) ? "com.qti.sensor.cct" :
    "Unknown";
}

static const char *sensor_to_stringType(struct sensor_t const *sensor)
{
  return *(sensor->stringType) != 0 ? sensor->stringType : type_to_string(sensor->type);
}

class snsHidlBufTest {
  private:
    sp<ISensors> sensors;
    vector<sensor_t> mSnsList;
    vector<int>::iterator iter;

    void addSensorToList(const  android::hardware::sensors::V1_0::SensorInfo elm) {
      sensor_t sensor;
      android::hardware::sensors::V1_0::implementation::convertToSensor(elm,&sensor);
      mSnsList.push_back(sensor);
    }
    void printSensor(struct sensor_t const *sensor) {
        fprintf(stderr, "[Type: %2d] %s (%s)\n", sensor->type, sensor_to_stringType(sensor), (sensor->flags & SENSOR_FLAG_WAKE_UP) != 0 ? "wakeup" : "non-wakeup");
        fprintf(stderr, "\tName:%s Vendor:%s Version:%d Handle:%d\n", sensor->name, sensor->vendor, sensor->version, sensor->handle);
        fprintf(stderr, "\tmaxRange: %f resolution: %f power: %f mA\n", sensor->maxRange, sensor->resolution, sensor->power);
        fprintf(stderr, "\tminDelay: %d us maxDelay: %ld us\n", sensor->minDelay, sensor->maxDelay);
        fprintf(stderr, "\tfifoReservedEventCount: %d fifoMaxEventCount: %d\n", sensor->fifoReservedEventCount, sensor->fifoMaxEventCount);
        fprintf(stderr, "\trequiredPermission: %s\n", sensor->requiredPermission);
    }
    int32_t getHandleFrmType(int type) {
      for (auto it = mSnsList.begin(); it != mSnsList.end(); it++) {
        if (it->type == type) return it->handle;
      }
      return -1;
    }

  public:
    snsHidlBufTest() {
      sensors = ISensors::getService();
    }
    void getSensorList() {
      struct timespec before_rt;
      struct timespec after_rt;
      uint64_t before_ns;
      uint64_t after_ns;
      uint64_t time_taken_ns;

      /* Step1 : get the sensor list and publish it */
      clock_gettime( CLOCK_BOOTTIME, &before_rt );
      sensors->getSensorsList(
        [&](const auto list){
          int i = 0;
          for (auto it = list.begin(); it != list.end(); it++) {
            addSensorToList(list[i++]);
          }
          cout <<"number of sensors:" <<list.size() << endl;
        });
      clock_gettime( CLOCK_BOOTTIME, &after_rt );
      before_ns = (uint64_t)before_rt.tv_sec * 1000000000LL + (uint64_t)before_rt.tv_nsec;
      after_ns = (uint64_t)after_rt.tv_sec * 1000000000LL + (uint64_t)after_rt.tv_nsec;
      time_taken_ns = (after_ns-before_ns);
      cout << "getSensorsList took:" << time_taken_ns/(1000)<< "Usec" << endl;
    }
    void printSensorattr() {
      int i = 0;
      for (auto it = mSnsList.begin(); it != mSnsList.end(); it++) {
        printSensor(&mSnsList[i++]);
      }
    }
    void printSensorattr(int type) {
      int i = 0;
      for (auto it = mSnsList.begin(); it != mSnsList.end(); it++,i++) {
        if (it->type == type)
          printSensor(&mSnsList[i]);
      }
    }
    void printSensorType() {
      int i = 0;
      for (auto it = mSnsList.begin(); it != mSnsList.end(); it++) {
        cout << "<type:" << it->type << ">" << sensor_to_stringType(&mSnsList[i++]) << endl;
      }
    }
    Result enable(int type,int en) {
      int32_t sensor_handle = getHandleFrmType(type);
      bool enable = false;
      if (en) {
        enable = true;
        enabledClients.push_back(sensor_handle);
      } else {
        cout << "active sensors " << enabledClients.size() << endl;
        iter = find(enabledClients.begin(), enabledClients.end(), sensor_handle);
        if (iter != enabledClients.end())
          enabledClients.erase(iter);
      }
      cout << "active sensors " << enabledClients.size() << endl;
      return sensors->activate(sensor_handle,enable);
    }
    /* Check the enabled clients and call disable */
    void resetSensors() {
      Result Rs;
      sp<ISensors> RstSensors;
      RstSensors = ISensors::getService();
      cout << "active sensors in resetSensors:" << enabledClients.size() << endl;
      for (size_t cnt = 0; cnt < enabledClients.size(); cnt++) {
        cout << "disable sensor:" << enabledClients[cnt] << endl;
        Rs = RstSensors->activate(enabledClients[cnt], 0);
        cout << "reset sensor done" << endl;
        if (Result::OK != Rs) {
          cout << "disable failed !!" << toString(Rs) << endl;
        }
      }
    }
    Result batch(int type,
                 int64_t sampling_period_ns,
                 int64_t max_report_latency_ns) {
      int32_t sensor_handle = getHandleFrmType(type);
      return sensors->batch(sensor_handle, sampling_period_ns, max_report_latency_ns);
    }
    void poll(vector<sensors_event_t> &eventlist, int cnt) {
      sensors->poll(cnt, [&](auto Rs, auto &src, auto dynamicSensorsAdded) {
        if (Rs == Result::OK) {
          for (size_t i = 0; i < src.size(); i++) {
            sensors_event_t sample;
            memset(&sample,0,sizeof(sample));
            convertToSensorEvent(src[i], &sample);
            eventlist.push_back(sample);
          }
        }
      });
    }
    Result flush(int type) {
      int32_t sensor_handle = getHandleFrmType(type);
      return sensors->flush(sensor_handle);
    }
    bool checkSensorType(int Type) {
      int i = 0;
      bool rc = false;

      for (auto it = mSnsList.begin(); it != mSnsList.end(); it++) {
        if (mSnsList[i++].type == Type) {
          rc = true;
          break;
        }
      }
      if (!rc) {
        cout<<"Entered sensor type (" << Type << ") does not match" << endl;
      }
      return rc;
    }
};

void pollTask(snsHidlBufTest *TestHandle)
{
  int64_t now_ns;
  std::vector<sensors_event_t> eventlist;

  cout << endl;
  cout << "starting the Task" << endl;
  if (!TestHandle) {
    cout << "Test Handle NULL" << endl;
    return ;
  }
  eventlist.resize(NUM_POLL_EVENTS);

  while (1) {
    eventlist.clear();
    TestHandle->poll(eventlist,NUM_POLL_EVENTS);
    now_ns = android::elapsedRealtimeNano();
    for (auto it = eventlist.begin(); it != eventlist.end(); it++) {
#ifdef PRINT_SENSOR_SAMPLES
      if (it->type == SENSOR_TYPE_META_DATA) {
        printf("%ld.%06ld, %s/%s, %ld.%06ld %d latency: %ld.%06ld\n",
                now_ns/NSEC_PER_MS, now_ns%NSEC_PER_MS, "META_DATA",
                (it->meta_data.what == META_DATA_FLUSH_COMPLETE) ?"FLUSH_COMPLETE":"Unk",
                it->timestamp/NSEC_PER_MS, it->timestamp%NSEC_PER_MS,
                it->meta_data.sensor, (now_ns - it->timestamp) / NSEC_PER_MS,
                (now_ns - it->timestamp) % NSEC_PER_MS);
      } else {
        printf("recTs:%ld.%06ld, %s, sampleTs:%ld.%06ld, %f, %f, %f, %f, %ld, latency(ms): %ld.%06ld\n",
                now_ns/NSEC_PER_MS, now_ns%NSEC_PER_MS,
                type_to_string( it->type ),
                it->timestamp/NSEC_PER_MS, it->timestamp%NSEC_PER_MS,
                it->data[0], it->data[1], it->data[2], it->data[3], it->u64.step_counter,
                (now_ns - it->timestamp) / NSEC_PER_MS,
                (now_ns - it->timestamp) % NSEC_PER_MS);
      }
#endif
    }
  }
}

void my_handler(int s, void * v)
{
  cout << "Caught signal " << s <<endl;
  snsHidlBufTest *TestHandle = (snsHidlBufTest *)v;
  if (s == SIGINT) {
    cout << "start reset sensors" << endl;
    TestHandle->resetSensors();
    cout << "start reset sensors done. Exited hidl buf test." << endl;
    exit(0);
  }
}

int main()
{
  int64_t sampling_rate_nsecs;
  int64_t report_rate_nsecs;
  int type = -1;
  char ipt;
  Result Rs;

  cout<<"starting hidl bufftest" << endl;
  snsHidlBufTest *TestHandle = new snsHidlBufTest;

  /* Step1 : get the list */
  TestHandle->getSensorList();

  /* Step2 :get the attributes */
  TestHandle->printSensorattr();
  TestHandle->printSensorType();

  /* Start the thread for poll */
  thread t1(pollTask, TestHandle);

  /* Register signal handler */
  signal(SIGINT, (void (*)(int))my_handler);
  my_handler(FAKE_SIGNAL, TestHandle);

  sleep(1);

  do {
    do {
      cout << "please select the sensor type>>:";
        cin >> type;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        fflush(stdin);
    } while (TestHandle->checkSensorType(type) == false);

    cout << "options <a(attributes), e(enable), b(batch), f(flush), d(dactivate)>:" ;
    cin >> ipt;
    cin.ignore(1, '\n');
    fflush(stdin);

    switch (ipt) {
      case 'a': /* Attributes */
        TestHandle->printSensorattr(type);
        break;

      case 'b': /* Batch */
        cout << "please enter input sample rate in ms>>:";
        cin >> sampling_rate_nsecs;

        cout << "please enter input timeout in ms>>:";
        cin >> report_rate_nsecs;
        fflush(stdin);

        /* Step3: set batch params */
        sampling_rate_nsecs *= NSEC_PER_MS;
        report_rate_nsecs *= NSEC_PER_MS;
        cout << "sns_hidl_test:batch" << endl;
        Rs = TestHandle->batch(type, sampling_rate_nsecs, report_rate_nsecs);
        if (Result::OK != Rs) {
          cout << "batch failed:" << toString(Rs) << endl;
          return -1;
        }
        break;

      /* Step4: enable request */
      case 'e': /* Enable */
        Rs = TestHandle->enable(type, 1);
        if (Result::OK != Rs) {
          cout << "enable failed:" << toString(Rs) << endl;
          return -1;
        }
        break;

      case 'd': /* Disable */
        Rs = TestHandle->enable(type, 0);
        if (Result::OK != Rs) {
          cout << "disable failed:" << toString(Rs) << endl;
          return -1;
        }
        break;

      case 'f': /* Flush */
        cout << "sns_hidl_test:flush" << endl;
        Rs = TestHandle->flush(type);
        if (Result::OK != Rs) {
          cout << "flush failed:" << toString(Rs) << endl;
          return -1;
        }
        break;

      default:
        cout << "default case" << ipt << endl;
        ipt = {};
        break;
    }
    cout << "type x to exit or type c to continue:";
    cin >> ipt;
    cin.ignore(1, '\n');
    fflush(stdin);
  } while (ipt != 'x');

  TestHandle->resetSensors();
  delete TestHandle;
  cout << "End hidl buff test" << endl;
  return 0;
}
