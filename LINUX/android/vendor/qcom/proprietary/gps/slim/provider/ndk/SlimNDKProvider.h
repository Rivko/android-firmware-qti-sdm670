/*
Copyright (c) 2011-2012, 2014-2015, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

2012 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Proprietary and Confidential.
*/
/**
@file
@brief GNSS / Sensor Interface Framework Support

This file defines the interface to accessing sensor data through Native Android
NDK API.

@ingroup slim_NDKProvider
*/

#ifndef __SLIM_NDK_PROVIDER_H__
#define __SLIM_NDK_PROVIDER_H__

#include <stdlib.h>

#include "slim_client_types.h"
#include <android/sensor.h>
#include <utils/Looper.h>
#include <MultiplexingProvider.h>
#include <KalmanFilter.h>
#include "ASensorManager.h"
#include "ASensorEventQueue.h"
#include <sensor/Sensor.h>

//! @addtogroup slim_NDKProvider
//! @{

namespace slim
{
using android::sp;
using android::wp;
using android::Looper;
using android::LooperCallback;
using android::Thread;
/**
@brief SLIM provider implementation that uses Android Sensor API

Provider uses Sensor API which is also used by Android Java component. The
class launches event polling thread that collects sensor data when required.

The Sensor API initialization is performed in worker thread (lazily) to enable
faster startup an do not wait until @a system_server process loads sensor HAL.

Provider supports both batch and non-batch modes operation. In case of batch
mode operation, the responsibility of data batching is delegated to HAL.
However Android HAL API doesn't guarantee accurate batch construction and may
report data in batches of other sizes. When operating in non-batch mode,
the provider expects data coming as it becomes available and constructs
batches with time and size restrictions.

The object locks state when processing sensor events and/or performing state
change operations.

@ingroup slim_NDKProvider
*/
class NDKProvider :
  public Thread,
  public MultiplexingProvider,
  public LooperCallback,
  public virtual android::hardware::hidl_death_recipient
{
private:
  //! @brief Provider messages: SLIM thread
  enum
  {
    ePROVIDER_MSG_ID_REOPEN = eMSG_ID_COMMON_END,
    /**< SSR handling within SLIM context.

    This message triggers request to NDK rebind. The message is present to
    ensure provider status update is handled in correct order.
    */
  };
  //! @brief IPC messages: worker thread
  enum
  {
    eIPC_MSG_ID_REBIND = C_USR_MSG_ID_FIRST,
    /** Message to perform ISensorManager sensor initialization */
  };
  //! @brief Timer IDs: worker thread
  enum
  {
    eIPC_TIMER_ID_REBIND = 0,
    /**< Timer to retry sensor initialization if previous attempt has failed. */
    eIPC_TIMER_ID_BATCH = 1,
    /**< Timer to commit incomplete batch. */
  };
  //! @brief Internal enumeration for batch generation reason
  enum BatchSendReason
  {
    eBATCH_SEND_REASON_NONE = 0,
    eBATCH_SEND_REASON_SIZE = 1,
    eBATCH_SEND_REASON_TIMEOUT = 2,
    eBATCH_SEND_REASON_TIMER = 3,
  };
  /**
  @brief Base class for looper callbacks.

  The class holds a reference to outer class to forward events.
  */
  class LooperHandler : public LooperCallback
  {
  protected:
    sp<NDKProvider> m_pzProvider; /**< Outer class reference */
    LooperHandler
    (
      const sp<NDKProvider> &pzProvider
    ) :
      m_pzProvider(pzProvider)
    {
    }
  };
  /**
  @brief Handler for IPC events.
  */
  class EventHandler : public LooperHandler
  {
  public:
    EventHandler
    (
      const sp<NDKProvider> &pzProvider
    ) : LooperHandler(pzProvider)
    {
    }

    // Forwards call to outer class.
    virtual int handleEvent
    (
      int lFd,
      int lEvents,
      void *pData
    )
    {
      return m_pzProvider->processThreadEvent(lFd, lEvents, pData);
    }
  };
  /**
  @brief Handler for signal events.
  */
  class SignalHandler : public LooperHandler
  {
  public:
    SignalHandler
    (
      const sp<NDKProvider> &pzProvider
    ) : LooperHandler(pzProvider)
    {
    }

    // Forwards call to outer class.
    virtual int handleEvent
    (
      int lFd,
      int lEvents,
      void *pData
    )
    {
      return m_pzProvider->processSignalEvent(lFd, lEvents, pData);
    }
  };
  friend class EventHandler;
  friend class SignalHandler;
  /**
  @brief Inner class for handling single service operation

  Class maintains service buffers and other relevant information.
  @internal
  */
public:
  class ServiceState : public virtual RefBase
  {
    friend class NDKProvider;
  protected:
    sp<NDKProvider>           m_pzNDKProvider;   /**< Owner */
    const slimSensorTypeEnumT m_eSlimSensorType; /**< Managed sensor type */
    const slimServiceEnumT    m_eSlimServiceType;/**< Managed service type */
    const char * const        m_pzName;          /**< Literal name for service */
  public:
    const int32_t          m_lNdkSensorType;  /**< NDK sensor type */
    volatile bool          m_uRunning;        /**< Status */
    bool                   m_uBatchMode;      /**< Service-specific batch mode */
  protected:
    ASensor*               m_pzSensor;
    uint32_t               m_qSampleRateHz;   /**< Current sampling rate in Hz */
    uint32_t               m_qBatchingRateHz; /**< Current batching rate in Hz */
    uint32_t               m_qBatchSize;      /**< Current batch size */
    slimSensorDataStructT  m_zSensorData;     /**< Structure used to batch samples */
    int64_t                m_tLastSampleDelay;/**< Last sample delay */
    bool                   m_uBound;          /**< Flag if bound */
    uint64_t               m_tLastReportTimestampMs; /**< Last report timestamp ms */
    uint64_t               m_tAvgReportIntervalMs; /**< Average report interval ms */
    uint64_t               m_tReportIntervalThresholdMs; /**<Report interval threshold ms */
    /**
    @brief Initializes object

    After initialization a @a bind operation shall be invoked to complete
    startup.

    @param[in] pzNDKProvider Parent container.
    @param[in] eSensorType   SLIM sensor type.
    @param[in] eServiceType  SLIM service type.
    @param[in] lNdkType      Android sensor type.
    @param[in] pzName        Literal name for logging.
    */
    ServiceState
    (
      NDKProvider *pzNDKProvider,
      slimSensorTypeEnumT eSensorType,
      slimServiceEnumT eServiceType,
      int32_t          lNdkType,
      const char *pzName
    );
    virtual ~ServiceState();
    /**
    @brief Adds sample data to buffer

    @param[in] zData        Sample data
    @param[in] zTimestampMs Sample time (using SLIM timer).
    */
    void processRawData
    (
      float zData[3],
      uint64_t tTimestampMs
    );
    /**
    @brief Get batch expiration time

    Batch expiration time is computed when the service is enabled, and there is
    at least one sample available.

    @param[out] tTimeMsOut Expiration time in milliseconds.
    @return true if expiration time is present.
    */
    bool getExpireTime
    (
      uint64_t &tTimeMsOut
    ) const;
    /**
    @brief Attempts to find NDK provider for the given object.

    Method queries ISensorManager provider for existence of sensor provider
    with appropriate NDK sensor type. When multiple found, the one with
    best (maximum) rate reporting capability is selected.

    @return @a true on success; or @a false on failure.
    */
    bool bind
    (
      const ASensor &zSensor
    );
  private:
    /**
    @brief Checks if data should be throttled.

    Function checks if data should be throttled.

    Updated logic: Given T0, t0 & input t1
      --> T1 = alp * (t1 - t0) + (1 - alp) * T0
      --> if T1 < T_THRESHOLD, then THROTTLE

    @return TRUE  if send data should be throttled.
    */
    bool sendDataThrottle();
  public:
    /**
    @brief Test if the object is bound.
    @return true if bound.
    */
    bool isBound() const
    {
      return m_uBound;
    }
    /**
    @brief Reset data buffer

    Method clears output data buffer.
    */
    void resetData();
    /**
    @brief Sends data to SLIM core.

    The method routes collected data to the provider itself for multiplexing and
    dispatching to individual clients using SLIM Core API.

    @param eSendReason Batch generation reason.

    @return eSLIM_SUCCESS if operation is successful.
    */
    slimErrorEnumT sendDataBatch
    (
      BatchSendReason eSendReason
    );
    /**
    @brief Conditional data send.

    Method sends accumulated data only if sufficient amount is collected, or
    time check check reports data is delayed.

    @param[in] uCheckTime Flag if time check is required.

    @return eSLIM_SUCCESS if data is not sent, or operation is successful.
    */
    slimErrorEnumT sendIfReady
    (
      bool uCheckTime
    );
    /**
    @brief Enables or disables data collection.

    When enabling, the actual sampling rate equals @a wReportRateHz multiplied
    by @a wSampleCount.

    @param[in] uRunning      Flag to enable (true) or disable(false)
                             data collection.
    @param[in] wReportRateHz Batch reporting rate in Hz.
    @param[in] wSampleCount  Sample count in one batch.
    @return Operation status; eSLIM_SUCCESS if successful.
    */
    slimErrorEnumT updateStatus
    (
      bool uRunning,
      uint16_t wReportRateHz,
      uint16_t wSampleCount
    );
    /**
    @brief Method for processing raw event data.

    This method is called for processing inbound sensor event data. The
    concrete class shall take a decision if this event is relevant and
    accumulate or discard event's data.

    @param[in] zEvent       Android sensor event container.
    @param[in] tEventTimeMs Event time in SLIM timer units.

    @return true if event is consumed; false if ignored.
    */
    virtual bool processEvent
    (
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
    ) = 0;
  };
  /**
  @brief Accelerometer state
  */
  class AccelState : public ServiceState
  {
  public:
    /**
    @brief Constructs object.

    Constructs object that produces eSLIM_SERVICE_SENSOR_ACCEL service data
    from SENSOR_TYPE_ACCELEROMETER sensor.

    @param[in] pzNDKProvider Parent container.
    */
    AccelState
    (
      NDKProvider *pzNDKProvider
    );

    /**
    @brief Collects sensor data.

    @param[in] e       Android sensor API event.
    @param[in] tEventTimeMs Event time.
    @return true if event type matches @ref SENSOR_TYPE_ACCELEROMETER
    */
    virtual bool processEvent
    (
      const ASensorEvent &zEvent,
      uint64_t tEventTimeMs
    );
  };
  /**
  @brief Gyro state
  */
  class GyroState : public ServiceState
  {
  public:
    /**
    @brief Constructs object.

    Constructs object that produces eSLIM_SERVICE_SENSOR_GYRO service data
    from SENSOR_TYPE_GYROSCOPE sensor.

    @param[in] pzNDKProvider Parent container.
    */
    GyroState
    (
      NDKProvider *pzNDKProvider
    );
    /**
    @brief Collects sensor data.

    @param[in] e       Android sensor API event.
    @param[in] tEventTimeMs Event time.
    @return true if event type matches @ref SENSOR_TYPE_GYROSCOPE
    */
    virtual bool processEvent
    (
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
    );
  };
  /**
  @brief Barometer state
  */
  class BaroState : public ServiceState
  {
  public:
    /**
    @brief Constructs object.

    Constructs object that produces eSLIM_SERVICE_SENSOR_BARO service data
    from SENSOR_TYPE_PRESSURE sensor.

    @param[in] pzNDKProvider Parent container.
    */
    BaroState
    (
      NDKProvider *pzNDKProvider
    );
    /**
    @brief Collects sensor data.

    @param[in] e       Android sensor API event.
    @param[in] tEventTimeMs Event time.
    @return true if event type matches @ref SENSOR_TYPE_PRESSURE
    */
    virtual bool processEvent
    (
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
    );
  };
  /**
  @brief Calibrated magnetometer state
  */
  class MagCalibState : public ServiceState
  {
  public:
    /**
    @brief Constructs object.

    Constructs object that produces eSLIM_SERVICE_SENSOR_MAG_CALIB service data
    from SENSOR_TYPE_MAGNETIC_FIELD sensor.

    @param[in] pzNDKProvider Parent container.
    */
    MagCalibState
    (
      NDKProvider *pzNDKProvider
    );
    /**
    @brief Collects sensor data.

    @param[in] e       Android sensor API event.
    @param[in] tEventTimeMs Event time.
    @return true if event type matches @ref SENSOR_TYPE_MAGNETIC_FIELD
    */
    virtual bool processEvent
    (
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
    );
  };
  /**
  @brief Uncalibrated magnetometer state
  */
  class MagUncalibState : public ServiceState
  {
  public:
    /**
    @brief Constructs object.

    Constructs object that produces eSLIM_SERVICE_SENSOR_MAG_UNCALIB service
    data from SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED sensor.

    @param[in] pzNDKProvider Parent container.
    */
    MagUncalibState
    (
      NDKProvider *pzNDKProvider
    );
    /**
    @brief Collects sensor data.

    @param[in] e       Android sensor API event.
    @param[in] tEventTimeMs Event time.
    @return true if event type matches @ref SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED
    */
    virtual bool processEvent
    (
    const ASensorEvent &zEvent,
    uint64_t tEventTimeMs
    );
  };

  friend class ServiceState;
  typedef android::Vector<sp<ServiceState> > StateVector;

private:
    /*!
   * @brief Kalman Filter for Android Sensor provider
   */
  class KF : public KalmanFilter
  {
  private:
    //! @brief Min Residual diff to check for outlier
    static const uint32_t s_qDivergeThresholdUs;
    //! @brief Minimum uncertainty to initialize filter
    static const uint32_t s_qMinUncToInitUs;
    //! @brief Expect/Meas variance threshold scalar
    static const uint8_t s_uVarThreshold;
    //! @brief Number of outliers before filter reset
    static const uint8_t s_uOutlierLimit;
    //! @brief Minimum time update interval in microseconds.
    static const uint64_t s_tTimeUpdateIntervalUs;

    //! @brief Time limit for minimum process noise in microseconds
    static const double s_dMinTsOffsetQDtUs;
    //! @brief Minimum process noise
    static const double s_dMinTsOffsetQUs2;
    //! @brief Process noise rate between time limits (us^2)/us
    static const double s_dTsOffsetQSfUs2Us;
    //! @brief Measurement noise
    static const double s_dTsOffsetMeasNoiseUs2;
    //! @brief Measurement noise rat between time limits (us^2)/us
    static const double s_dTsOffsetRSfUs2Us;

    /**
     * @brief Function used to calculate process noise
     *
     * @param[in] dDeltaTimeUs elapsed time between updates in microseconds.
     *
     * @return Process noise of clock offset in microseconds.
     */
    virtual double getProceNoise
    (
      double dDeltaTimeUs
    );

    /**
     * @brief Function used to calculate measurement noise
     *
     * @param[in] dDeltaTimeUs elapsed time between updates in microseconds.
     *
     * @return Measurement noise of clock offset in microseconds.
     */
    virtual double getMeasNoise
    (
      double dDeltaTimeUs
    );
  public:

    /**
     * @brief Initializes Kalman filter with provider-specific parameters.
     */
    KF();
  };
public:
  /** @brief Time source for sensor samples */
  enum TimeSource
  {
    eTIME_SOURCE_UNKNOWN,   //!< @brief Time source is unknown and KF is used
    eTIME_SOURCE_BOOTTIME,  //!< @brief CLOCK_BOOTTIME clock is used
    eTIME_SOURCE_MONOTONIC, //!< @brief CLOCK_MONOTONIC clock is used
    eTIME_SOURCE_REALTIME,  //!< @brief CLOCK_REALTIME clock is used
    eTIME_SOURCE_ELAPSED,   //!< @brief CLOCK_BOOTTIME through alarm API is used
  };
  ASensorEventQueue* m_pzEventQueue;

private:
  /** @brief Singleton instance lock */
  static Mutex           s_zInstanceLock;
  /** @brief Singleton instance strong pointer. */
  static sp<NDKProvider> s_pzInstance;
  /*! @brief Timeout in ms between rebind attempts */
  static const uint32_t  s_qNdkRebindTimeoutMs;
  /*! @brief Timeout in microseconds between time diff updates */
  static const uint64_t  s_tTimeDiffUpdateTimeoutUs;
  /*! Android polling loop object */
  sp<Looper>           m_zLooper;
  ALooper*              m_pLooper;
  /*! @brief Event queue */

  /*! @brief Timer for retrying service bind attempts */
  slim_TimerT         *m_pzBinderTimer;
  /*! @brief Timer for generating data batches */
  slim_TimerT         *m_pzBatchTimer;
  /*! @brief Time difference between SLIM and HAL sources */
  int64_t              m_tTimestampDeltaUs;
  /*! @brief Time of computing difference */
  uint64_t             m_tTimestampExpireTimeUs;
  /**
  @brief Get Android Sensor timer time in microseconds.

  @return Time in microseconds from Android's Sensor source.
  */
  uint64_t readSensorTimeUs();

  /**
  @brief Computes time difference in clock ticks.

  Utility function to return the difference in two timestamps in clock ticks.
  Accounts for any timestamp rollovers due to the limited range of milliseconds
  in uint32_t. (Rolls over ~ every 50 days)

  @param[in] tTs1Ms First timestamp in milliseconds. (Comes first in monotonic time)
  @param[in] tTs2Ms Second timestamp in milliseconds. (Comes second in monotonic time)

  @return Time difference in clock ticks
  */
  static uint32_t calcMsTimeDiffTick
  (
    uint64_t tTs1Ms,
    uint64_t tTs2Ms
  );

  /**
  @brief Constructs provider instance.

  Initialization function for NDK provider internal state.
  */
  NDKProvider();
  /**
  @brief Destroys provider.
  */
  ~NDKProvider();

  /**
  @brief Method for completing provider initialization

  Method connects to Android Sensor objects and launches polling thread.

  @retval true on success.
  */
  bool initialize();

  /**
  @brief Query Android Sensor Manager API and update service mask

  Method connects to ISensorManager interface using Binder API and
  queries available sensor services. After that the service mask and
  supported service list are updated.

  @param[in] qAttemptNo Attempt number.
  */
  void bindNDKSensors
  (
    uint32_t qAttemptNo
  );

  // android::Thread
  /**
  @brief Main polling task function.

  Task that polls the sensor for sensor data. This thread will read a sample,
  convert it and batch to send to the SLIM task for multiplexing.

  @return false This function runs only once.
  */
  virtual bool threadLoop();

  // android::LooperCallback
  /**
  @brief Reads events from queue

  Callback method for reading sensor events and dispatching them to proper
  state processors.

  @param[in] lFd        Queue descriptor
  @param[in] lEventMask Available events
  @param[in] pData      User handle (0)

  @return 1 to continue processing.
  */
  virtual int handleEvent
  (
    int lFd,
    int lEventMask,
    void *pData
  );

  // slim::MultiplexingProvider
  virtual slimErrorEnumT doUpdateSensorStatus
  (
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
  );

  virtual void serviceDied
  (
    uint64_t cookie,
    const ::android::wp<::android::hidl::base::V1_0::IBase>& who
  );

  /**
  @brief Handler for messages targeted for provider.

  Function handles messages sent via SLIM core to provider.

  @param[in] qMessageId      Message id.
  @param[in] qMsgPayloadSize Message size.
  @param[in] pMsgPayload     Pointer to the message payload.
  */
  virtual void handleProviderMessage
  (
    uint32_t qMessageId,
    uint32_t qMsgPayloadSize,
    const void *pMsgPayload
  );

  /**
  @brief Internal helper for handling thread signal events.

  @param[in] lFd     File handler.
  @param[in] lEvents Event mask.
  @param[in] pData   Event cookie.
  */
  int processSignalEvent
  (
    int lFd,
    int lEvents,
    void *pData
  );

  /**
  @brief Internal helper for handling thread events.

  @param[in] lFd     File handler.
  @param[in] lEvents Event mask.
  @param[in] pData   Event cookie.
  */
  int processThreadEvent
  (
    int lFd,
    int lEvents,
    void *pData
  );

  /**
  @brief Handler for timer messages

  Timer messages are belonging to the worker thread.

  @param[in] qTimerId Timer identifier.
  @param[in] qParam   Timer parameter.

  @sa slim_Timer
  */
  virtual void handleTimerEvent
  (
    uint32_t qTimerId,
    uint32_t qParam
  );
  /**
  @brief Handler for IPC messages

  IPC messages are belonging to the worker thread.

  @param[in] qMsgId   Message identifier.
  @param[in] qMsgSize Message payload size.
  @param[in] pMsgData Message payload.

  @sa slim_IPC
  */
  virtual void handleIpc
  (
    uint32_t qMsgId,
    uint32_t qMsgSize,
    const void *pMsgData
  );

  /**
   * @brief Initiates time offset request.
   *
   * Function for making the time request. Successful response enable SLIM to
   * calculate the offset between modem time and sensor time.
   *
   * @param[in] lTxnId Service transaction id.
   *
   * @return eSLIM_SUCCESS if time request is made successfully.
   */
  slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  );

public:
  /**
   * @brief Returns SLIM interface
   *
   * The method initializes singleton instance and returns SLIM provider
   * interface.
   *
   * @return Provider object
   */
  static sp<NDKProvider> getInstance();

  /**
   * @brief Set sensor HAL sample time source.
   *
   * @param[in] eTimeSource Sensor HAL time source
   */
  void setTimeSource
  (
    TimeSource eTimeSource
  );

  /*! @brief Mutex for atomic operations */
  mutable Mutex        m_zLock;

  /**
  @brief Sets up batch timer.

  The method iterates through services and for active ones dispatches expired
  ones and computes timeout for others.

  If the timeout is computed, the batch timer is set.
  */
  void processBatchTimeout();

  /**
  @brief Get SLIM core timer time in microseconds.

  @return SLIM core time in microseconds.
  */
  static uint64_t readCoreTimeUs();

  /**
  @brief Compute and provide timestamp delta

  Timestamp delta is a difference between SLIM Core time and sensor time.
  When the Android Sensor HAL time source matches SLIM time source, the
  difference is always 0. Otherwise it is a delta in milliseconds between
  Sensor HAL clock and SLIM Core.

  Due to nature of times, the raw difference between two calls may jump within
  +-10ms (or thread quantum range).

  @return time difference between clocks.
  */
  int64_t getTimestampDeltaUs();

  /**
  @brief Performs reinitialization of ISensorManager connection.

  Unregisters self as a death recipient from manager interface, requests all
  sensors to stop, clears sensor list, updates SLIM core with service change
  notification and enqueues reinitialization.
  */
  void cleanupSensors();

  /*! @brief Kalman Filter */
  KF                   m_zKF;

  /*! @brief Time source */
  TimeSource             m_eTimeSource;
  /*! @brief Estimated delay between HAL and SLIM process in microseconds */
  static const uint64_t  s_tEstimatedSampleDelayUs;

  /*! @brief All supported sensor states */
  StateVector          m_zStateList;
};
} /* namespace slim */

//! @}

#endif /* __SLIM_NDK_PROVIDER_H__ */
