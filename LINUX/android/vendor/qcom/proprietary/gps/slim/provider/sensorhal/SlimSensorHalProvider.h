/*
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/**
@file
@brief Sensor HAL provider interface file.

This file declares Sensor HAL provider object.

@ingroup slim_SensorHALProvider
*/
#ifndef __SENSOR_HAL_H_INCLUDED__
#define __SENSOR_HAL_H_INCLUDED__

#include <MultiplexingProvider.h>
#include <slim_api.h>
#include <stdio.h>
#include <list>
namespace slim
{

/**
@brief Sensor HAL provider class.

SensorHALProvider class implements methods specific for Sensor on HAL
connection.
 */

class SensorHalProvider :
  public MultiplexingProvider
{
  /*! Timers enumeration */
  enum
  {
    eTIMER_DEFAULT, /**< Default timer */
  };

  /*! Enumeration of messages to be sent to the provider message processing
   * loop */
  enum
  {

    eIPC_MSG_TIMER = M_IPC_MSG_TIMER,
    /**< Timer message ID */

    eIPC_MSG_NEW_FRAME_EVENT = C_USR_MSG_ID_FIRST + 1,
    /**< New frame message ID */
  };

  SensorHalProvider *m_pzSensorHalProvider;
  static const uint32_t s_qThreadId;
  static Mutex s_zInstanceMutex;
  static SensorHalProvider *s_pzInstance;
  static const slimAvailableServiceMaskT s_qSupportedServices;
  slimSensorDataStructT m_zSensorData;
  slimSensorDataStructT m_zAccSensorData;
  slimSensorDataStructT m_zGyroSensorData;
  int  mTempuS;
  bool mTempServiceStatus;
  slim_ThreadT m_zThread;
  Mutex m_zMutex;
  slim_TimerT *m_pzTimer;

  /*! Sensor HAL provider main thread function */
  static void threadMain
  (
    void *pData
  );

  /**
  @brief Class constructor

  Creates and initializes new instance of the class.

  */
  SensorHalProvider();

  /**
  @brief Class destructor

  Destroys current instance

  */
  ~SensorHalProvider();
  bool initialize();
  void runEventLoop();

  /**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in] lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
  virtual slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  );

private:

   #define BATCHSIZE_100HZ (10)
  struct HalLibInfo
  {
    uint8_t sensorType;
    uint8_t sensorHalLibPathStrSet;
    char    sensorHalLibPath[256];
  };
  static  HalLibInfo m_zLibInfo;
  enum BufferTxStatus
  {
    COMPLETED_BUFFER_DATA,
    KERNEL_BUFFER_DATA_INIT,
    KERNEL_BUFFER_DATA_IN_PROGRESS,
    SLIM_BUFFER_DATA_IN_PROGRESS,
  };

  slim_ThreadT m_zBuffThread;
  pthread_mutex_t mHalBuffMutex;
  pthread_cond_t mHalBuffCond;
  slimSensorDataStructT m_zAccBuffSensorData;
  slimSensorDataStructT m_zGyroBuffSensorData;
  BufferTxStatus mBufferTxStatus;
  std::list<slimSensorSampleStructT> mSecAccBufferList;
  std::list<slimSensorSampleStructT> mSecGyroBufferList;
  pthread_mutex_t mHalTempMutex;
  uint16_t mAccReportRateHz;
  uint16_t mAccSampleCount;
  uint16_t mGyroReportRateHz;
  uint16_t mGyroSampleCount;
  uint16_t mAcc100HzBatchCount;
  uint16_t mGyro100HzBatchCount;
  bool mAccServiceStatus;
  bool mGyroServiceStatus;
  bool m_bIsAccelPPSTimeNotReached;
  bool m_bIsGyroPPSTimeNotReached;
  slim_ThreadT m_zTempSensorThread;
  bool m_Enable;
  /**
  * @brief Initiates the buffere data thread
  *
  * @return true on sucess.
  */
  bool memsBufferDataInit();

  /**
  * @brief Deinitiates the buffer data thread and closes open files
  */
  void memsBufferDataDeInit();

  /**
  * @brief Handler for Buffered data.
  *
  * Function for polling buffered data from sysfs interface
  * and send it for formatting and routing through slim core.
  *
  * @param[in] pData .
  *
  * @return void.
  */
  static void bufferedDataHandler(void *pData);

  /**
  * @brief baching and for formatting of buffered data.
  *
  * Function for converting buffered data to SLIM service sepecific format
  * Read queue until next EV_SYN , this will fill only newly received data
  * if some fields of sample are not received will maintain older data
  *
  * @param[in/out] zEventBuffer - input previous data, output new data,
  * if some fields are not received will hold previous data.
  * @param[in] fd - pointer to file descritor
  * @param[in] eSensorType - sensorType
  *
  * @return void.
  */
  bool getBufferedSample
  (
    slimServiceEnumT eSensorType,
    FILE* fd,
    slimSensorSampleStructT *zEventBuffer
  );

  /**
  * @brief set buffere data state.
  *
  * @param[in] tate.
  *
  * @return void.
  */
  void setBufferState(BufferTxStatus state);

  /**
  * Scans the input device and stores the fhandle
  * for reading the buffer data from kernel
  *
  * @param[in] sensor buffer name.
  * @param[in] pointer to file descriptor.
  *
  * return val: SUCCESS -  0
  *               FAIL - -1
  */
  int snsBufDevOpen(char *s_name, FILE** fd);

  /* file descriptor for Accel event file bmi160_accbuf */
  FILE* mfdBuffAccel;
  /* file descriptor for Gyro event file bmi160_gyrobuf */
  FILE* mfdBuffGyro;
  /* file descriptor for Accel buffer file read_acc_boot_sample */
  FILE* mfdBuffAccelE;
  /* file descriptor for Gyro buffer file read_gyro_boot_sample */
  FILE* mfdBuffGyroE;

  /**
  @brief Converts sensor number to sensor name.

  @param[in] type - sensor number
  @return character pointer (name of sensor)
  */
  char const* getSensorName(int type);

  /**
  @brief Converts sensor type from sensor number to slim sensor enum number

  @param[in] sensorType - sensor number
  @return slimSensorTypeEnumT enum value
  */
  slimSensorTypeEnumT SensorTypeToSlimSensorType(int sensorType);

  /**
  @brief Converts slim sensor number to corresponding service

  @param[in] slimSensorTypeEnumT  slimSensorType- sensor enum value
  @return slimServiceEnumT service corresponding to the sensor number
  */
  slimServiceEnumT SlimSensorTypeToService(slimSensorTypeEnumT slimSensorType);

 /**
  @brief Method for enabling or disabling sensor service.

  Generic function to start/stop a sensor based on provided sampling rate,
  batching rate, mounted state, and sensor information using Sensor1 Buffering
  API.

  @todo The sampling frequency is a mere suggestion to the sensor1 daemon.
  Sensor 1 will stream at the requested minimum sampling frequency requested
  by all AP clients combined. So we do see cases where buffering API does not
  help and we get single sensor data for every indication. In that case should
  SLIM AP do the batching?

  @param[in] uEnable       Flag that indicates if the service shall be enabled
                           or disabled.
  @param[in] wReportRateHz Requested reporting rate in Hz.
  @param[in] wSampleCount  Requested sample count in one batch.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateSensorStatus
  (
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
  );

  /**
   * @brief clean the buffers used for Batching in case of Disable sensor data.
   *
   * @return void.
   */
  virtual void cleanBatchingBuffers();

public:
  /*!
   * @brief Returns provider instance.
   *
   * Method provides access to provider instance. If necessary, the instance
   * is created and initialized.
   *
   * @return Provider instance.
   * @retval 0 On error.
   */
  static slim_ServiceProviderInterfaceType *getProvider();
  void bufferDataprocessTask();
  void tempSensorDataPollTask();
  bool tempSensorDataInit();
  static void tempSensorDataHandler(void *pData);
  int readTempASM330(void);
  int readTempBMI(void);
};
}

#endif /* __SENSOR_HAL_PROVIDER_H_INCLUDED__ */
