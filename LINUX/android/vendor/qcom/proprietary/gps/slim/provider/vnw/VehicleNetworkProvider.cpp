/**
 @file
 @brief Vehicle Network provider implementation file.

 This file defines VNW provider object implementation.
 */

/*
 Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 */

#include <inttypes.h>
#include <slim_os_log_api.h>
#include "VehicleNetworkProvider.h"
#include "CanFrameEvent.h"

//! @addtogroup slim_VNWProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module for VNW provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_VNW

using namespace slim;

const uint32_t VehicleNetworkProvider::s_qThreadId = THREAD_ID_VNW;
VehicleNetworkProvider *VehicleNetworkProvider::s_pzInstance;
Mutex VehicleNetworkProvider::s_zInstanceMutex;
const slimAvailableServiceMaskT VehicleNetworkProvider::s_qSupportedServices =
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_ACCEL)
        | (uint32_t(1) << eSLIM_SERVICE_VEHICLE_GYRO)
        | (uint32_t(1) << eSLIM_SERVICE_VEHICLE_ODOMETRY)
        | (uint32_t(1) << eSLIM_SERVICE_VEHICLE_SPEED)
        | (uint32_t(1) << eSLIM_SERVICE_VEHICLE_DWS)
        | (uint32_t(1) << eSLIM_SERVICE_VEHICLE_GEAR);

void VehicleNetworkProvider::threadMain(void *pData)
{
  SLIM_LOGD("VehicleNetworkProvider::threadMain(0x%"PRIxPTR")",
      (uintptr_t)pData);
  VehicleNetworkProvider *pzVNWProvider =
      reinterpret_cast<VehicleNetworkProvider*>(pData);
  slim_IpcStart(s_qThreadId);
  pzVNWProvider->m_pzTimer = slim_TimerCreate(eTIMER_DEFAULT, s_qThreadId);
  /* Service support is enabled */
  pzVNWProvider->m_pzCanWrapper = CanWrapper::getInstance();
  if (NULL == pzVNWProvider->m_pzCanWrapper) {
    SLIM_LOGE("getInstance failed for CanWrapper!.. Exiting thread \n");
    slim_TaskReadyAck();
    slim_ThreadDetach();
    return;
  }
  SLIM_LOGD("m_pzCanWrapper=0x%"PRIxPTR,
      (uintptr_t)pzVNWProvider->m_pzCanWrapper);
  pzVNWProvider->m_qServiceMask = s_qSupportedServices;
  pzVNWProvider->routeConfigurationChange(s_qSupportedServices);
  pzVNWProvider->runEventLoop();
}

/*!
 * @brief Returns provider instance.
 *
 * Method provides access to provider instance. If necessary, the instance
 * is created and initialized.
 *
 * @return Provider instance.
 * @retval 0 On error.
 */
slim_ServiceProviderInterfaceType *VehicleNetworkProvider::getProvider()
{
  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new VehicleNetworkProvider;
    if (!s_pzInstance->initialize())
    {
      SLIM_LOGE("VNW provider initialization failed;");
      delete s_pzInstance;
      s_pzInstance = 0;
      return 0;
    }
  }
  return s_pzInstance->getSlimInterface();
}

VehicleNetworkProvider::VehicleNetworkProvider()
    : MultiplexingProvider(SLIM_PROVIDER_VNW, false, 0)
    , m_zThread(), m_zMutex(), m_pzTimer(0)
    , m_pzCanWrapper(0), m_earId(0), m_qEnabledServices(0)
    , m_pzVehicleNetworkCfg(0), m_pzAccelProc(0)
    , m_pzGyroProc(0), m_pzOdometryProc(0)
    , m_pzSpeedProc(0), m_pzDwsProc(0)
    , m_pzGearProc(0)
    , m_tLastBusControllerSyncTime(0)
    , m_tLastBusControllerSyncTimestamp(0)
{
  memset(&m_zThread, 0, sizeof(m_zThread));
}

VehicleNetworkProvider::~VehicleNetworkProvider()
{
}

bool VehicleNetworkProvider::initialize()
{
  if (!slim_ThreadCreate(&m_zThread, threadMain, this, "VNWPoll"))
  {
    return false;
  }
  slim_TaskReadyWait(THREAD_ID_VNW);

  return true;
}

void VehicleNetworkProvider::cbOnNewCanFrame(CwFrame* pFrame, void *userData,
    int ifNo)
{
  VehicleNetworkProvider *pzVNWProvider =
      reinterpret_cast<VehicleNetworkProvider*>(userData);

  SLIM_LOGV("New frame received. id:%u, if:%d Time:%d", (unsigned int )pFrame->getId(),
             ifNo, (unsigned int )pFrame->getTimestamp());

  /* create new message data */
  CanFrameEvent* pzNewEvent = new CanFrameEvent(pFrame, ifNo);

  /* send message to event processing loop */
  if (!slim_IpcSendData(THREAD_ID_VNW, eIPC_MSG_NEW_FRAME_EVENT,
                        &pzNewEvent, sizeof(pzNewEvent)))
  {
    SLIM_LOGE("Error sending IPC message to event processing loop");
  }
}

void VehicleNetworkProvider::cbOnNewBufferedCanFrame(CwFrame* pFrame, void *userData,
    int ifNo)
{
  SLIM_LOGV("New Buffer frame received. id:%u, if:%d Time:%d",
             (unsigned int )pFrame->getId(),
             ifNo, (unsigned int )pFrame->getTimestamp());
  /* create new message data */
  CanFrameEvent* pzNewEvent = new CanFrameEvent(pFrame, ifNo);
  pzNewEvent->mIsBuffFrame = true;

  /* send message to event processing loop */
  if (!slim_IpcSendData(THREAD_ID_VNW, eIPC_MSG_NEW_FRAME_EVENT, &pzNewEvent,
      sizeof(pzNewEvent)))
  {
    SLIM_LOGE("Error sending IPC message to event processing loop");
  }
}

void VehicleNetworkProvider::runEventLoop()
{
  SLIM_LOGD("Starting event loop");
  int earIdAccel = 0, earIdGyro = 0, earIdOdo = 0, earIdSpeed = 0, earIdDws = 0, earIdGear = 0;
  int mearIdAccel = 0, mearIdGyro = 0, mearIdOdo = 0, mearIdSpeed =0, mearIdDws =0, mearIdGear = 0;

  //slim_TimerStart(m_pzTimer, 500, 0);
  m_pzVehicleNetworkCfg = new VehicleNetworkConfiguration();
  m_pzVehicleNetworkCfg->Load();

  char pzTStr[2048];
  m_pzVehicleNetworkCfg->ToString(pzTStr, sizeof(pzTStr));
  SLIM_LOGD("Configuration: %s", pzTStr);

  if (0 != m_pzVehicleNetworkCfg->GetAccelConfig())
  {
    m_pzVehicleNetworkCfg->GetAccelConfig()->ToString(pzTStr, sizeof(pzTStr));
    SLIM_LOGD("m_pzAccelConfig: %s", pzTStr);

    if (m_pzVehicleNetworkCfg->GetAccelConfig()->CfgValid())
    {
      m_pzAccelProc = VehicleNetworkMessageProcessor::CreateProcessor(
          this,
          m_pzVehicleNetworkCfg->GetAccelConfig(),
          eSLIM_SERVICE_VEHICLE_ACCEL);
      if (0 != m_pzAccelProc)
      {
        m_pzAccelProc->ToString(pzTStr, sizeof(pzTStr));
        SLIM_LOGD("m_pzAccelProc: %s", pzTStr);

        if (VehicleNetworkConfiguration::eACCEL_CONFIG_CONF1 ==
               m_pzVehicleNetworkCfg->GetAccelConfig()->GetVersion())
        {
          AccelCONF1Config *accelConfig =
               static_cast<AccelCONF1Config*>(m_pzVehicleNetworkCfg->GetAccelConfig());
          /* Register callback to CanWrapper */
          /* Configure listener to get only this frame on all interfaces*/
          earIdAccel = m_pzCanWrapper->registerListener(accelConfig->m_iFrameId,
                                                        CwBase::MASK11,
                                                        cbOnNewCanFrame,
                                                        (void *)(this),
                                                        CwBase::IFACE_ANY);

          mearIdAccel = m_pzCanWrapper->registerBufferedDataListener(accelConfig->m_iFrameId,
                                                                     CwBase::MASK11,
                                                                     cbOnNewBufferedCanFrame,
                                                                     (void *)(this),
                                                                     CwBase::IFACE_ANY);

          m_pzCanWrapper->enableBuffering(CwBase::IFACE_ANY,
                                          accelConfig->m_iFrameId,
                                          CwBase::MASK11);

          if (earIdAccel == 0 || mearIdAccel == 0)
          {
            SLIM_LOGE("Error registering CanWrapper listener for Accel");
          }
        }
      }
      else
      {
        SLIM_LOGD("m_pzAccelProc was NOT created");
      }
    }
  }
  else
  {
    SLIM_LOGD("m_pzAccelConfig was NOT loaded");
  }

  if (0 != m_pzVehicleNetworkCfg->GetGyroConfig())
  {
    m_pzVehicleNetworkCfg->GetGyroConfig()->ToString(pzTStr, sizeof(pzTStr));
    SLIM_LOGD("m_pzGyroConfig: %s", pzTStr);

    if (m_pzVehicleNetworkCfg->GetGyroConfig()->CfgValid())
    {
      m_pzGyroProc = VehicleNetworkMessageProcessor::CreateProcessor(
          this,
          m_pzVehicleNetworkCfg->GetGyroConfig(),
          eSLIM_SERVICE_VEHICLE_GYRO);
      if (0 != m_pzGyroProc)
      {
        m_pzGyroProc->ToString(pzTStr, sizeof(pzTStr));
        SLIM_LOGD("m_pzGyroProc: %s", pzTStr);

        if (VehicleNetworkConfiguration::eGYRO_CONFIG_CONF1 ==
            m_pzVehicleNetworkCfg->GetGyroConfig()->GetVersion())
        {
          GyroCONF1Config *gyroConfig =
                  static_cast<GyroCONF1Config*>(m_pzVehicleNetworkCfg->GetGyroConfig());
          /* Register callback to CanWrapper */
          /* Configure listener to get only this frame on all interfaces*/
          earIdGyro = m_pzCanWrapper->registerListener(gyroConfig->m_iFrameId,
                                                       CwBase::MASK11,
                                                       cbOnNewCanFrame,
                                                       (void *)(this),
                                                       CwBase::IFACE_ANY);

          mearIdGyro = m_pzCanWrapper->registerBufferedDataListener(gyroConfig->m_iFrameId,
                                                                    CwBase::MASK11,
                                                                    cbOnNewBufferedCanFrame,
                                                                    (void *)(this),
                                                                    CwBase::IFACE_ANY);

          m_pzCanWrapper->enableBuffering(CwBase::IFACE_ANY,
                                          gyroConfig->m_iFrameId,
                                          CwBase::MASK11);

          if (earIdGyro == 0 || mearIdGyro == 0)
          {
              SLIM_LOGE("Error registering CanWrapper listener for Gyro");
          }
        }
      }
      else
      {
        SLIM_LOGD("m_pzGyroProc was NOT created");
      }
    }
  }
  else
  {
    SLIM_LOGD("m_pzGyroConfig was NOT loaded");
  }

  if (0 != m_pzVehicleNetworkCfg->GetOdometryConfig())
  {
    if (m_pzVehicleNetworkCfg->GetOdometryConfig()->CfgValid())
    {
      m_pzVehicleNetworkCfg->GetOdometryConfig()->ToString(pzTStr,
          sizeof(pzTStr));
      SLIM_LOGD("m_pzOdometryConfig: %s", pzTStr);
      m_pzOdometryProc = VehicleNetworkMessageProcessor::CreateProcessor(
          this,
          m_pzVehicleNetworkCfg->GetOdometryConfig(),
          eSLIM_SERVICE_VEHICLE_ODOMETRY);
      if (0 != m_pzOdometryProc)
      {
        m_pzOdometryProc->ToString(pzTStr, sizeof(pzTStr));
        SLIM_LOGD("m_pzOdometryProc: %s", pzTStr);

        if (VehicleNetworkConfiguration::eODOMETRY_CONFIG_CONF1 ==
               m_pzVehicleNetworkCfg->GetOdometryConfig()->GetVersion())
        {
          OdometryCONF1Config *odoConfig =
               static_cast<OdometryCONF1Config*>(m_pzVehicleNetworkCfg->GetOdometryConfig());
          /* Register callback to CanWrapper */
          /* Configure listener to get only this frame on all interfaces*/
          earIdOdo = m_pzCanWrapper->registerListener(odoConfig->m_iFrameId,
                                                      CwBase::MASK11,
                                                      cbOnNewCanFrame,
                                                      (void *)(this),
                                                      CwBase::IFACE_ANY);

          mearIdOdo = m_pzCanWrapper->registerBufferedDataListener(odoConfig->m_iFrameId,
                                                                   CwBase::MASK11,
                                                                   cbOnNewBufferedCanFrame,
                                                                   (void *)(this),
                                                                   CwBase::IFACE_ANY);

          m_pzCanWrapper->enableBuffering(CwBase::IFACE_ANY,
                                          odoConfig->m_iFrameId,
                                          CwBase::MASK11);

          if (earIdOdo == 0 || mearIdOdo ==  0)
          {
              SLIM_LOGE("Error registering CanWrapper listener for Odometry");
          }
        }
      }
      else
      {
        SLIM_LOGD("m_pzOdometryProc was NOT created");
      }
    }
  }
  else
  {
    SLIM_LOGD("m_pzOdometryConfig was NOT loaded");
  }

  // slim_TimerStart(m_pzTimer, 500, 0);
  if (0 != m_pzVehicleNetworkCfg->GetSpeedConfig())
  {
    m_pzVehicleNetworkCfg->GetSpeedConfig()->ToString(pzTStr, sizeof(pzTStr));
    SLIM_LOGD("m_pzSpeedConfig: %s", pzTStr);

    if (m_pzVehicleNetworkCfg->GetSpeedConfig()->CfgValid())
    {
      m_pzSpeedProc = VehicleNetworkMessageProcessor::CreateProcessor(
          this,
          m_pzVehicleNetworkCfg->GetSpeedConfig(),
          eSLIM_SERVICE_VEHICLE_SPEED);
      if (0 != m_pzSpeedProc)
      {
        m_pzSpeedProc->ToString(pzTStr, sizeof(pzTStr));
        SLIM_LOGD("m_pzSpeedProc: %s", pzTStr);
        if (VehicleNetworkConfiguration::eSPEED_CONFIG_CONF1 ==
               m_pzVehicleNetworkCfg->GetSpeedConfig()->GetVersion())
        {
          SpeedCONF1Config *speedConfig =
               static_cast<SpeedCONF1Config *>(m_pzVehicleNetworkCfg->GetSpeedConfig());
          /* Register callback to CanWrapper */
          /* Configure listener to get only this frame on all interfaces*/
          earIdSpeed = m_pzCanWrapper->registerListener(speedConfig->m_iFrameId,
                                                        CwBase::MASK11,
                                                        cbOnNewCanFrame,
                                                        (void *)(this),
                                                        CwBase::IFACE_ANY);

          mearIdSpeed = m_pzCanWrapper->registerBufferedDataListener(speedConfig->m_iFrameId,
                                                                     CwBase::MASK11,
                                                                     cbOnNewBufferedCanFrame,
                                                                     (void *)(this),
                                                                     CwBase::IFACE_ANY);

          m_pzCanWrapper->enableBuffering(CwBase::IFACE_ANY,
                                          speedConfig->m_iFrameId,
                                          CwBase::MASK11);

          if (earIdSpeed == 0 || mearIdSpeed ==  0)
          {
              SLIM_LOGE("Error registering CanWrapper listener for Speed");
          }
        }
      }
      else
      {
        SLIM_LOGD("m_pzSpeedProc was NOT created");
      }
    }
  }
  else
  {
    SLIM_LOGD("m_pzSpeedConfig was NOT loaded");
  }

  if (0 != m_pzVehicleNetworkCfg->GetDwsConfig())
  {
    m_pzVehicleNetworkCfg->GetDwsConfig()->ToString(pzTStr, sizeof(pzTStr));
    SLIM_LOGD("m_pzDwsConfig: %s", pzTStr);

    if (m_pzVehicleNetworkCfg->GetDwsConfig()->CfgValid())
    {
      m_pzDwsProc = VehicleNetworkMessageProcessor::CreateProcessor(
          this,
          m_pzVehicleNetworkCfg->GetDwsConfig(),
          eSLIM_SERVICE_VEHICLE_DWS);
      if (0 != m_pzDwsProc)
      {
        m_pzDwsProc->ToString(pzTStr, sizeof(pzTStr));
        SLIM_LOGD("m_pzDwsProc: %s", pzTStr);
        if (VehicleNetworkConfiguration::eDWS_CONFIG_CONF1 ==
               m_pzVehicleNetworkCfg->GetDwsConfig()->GetVersion())
        {
          DwsCONF1Config *dwsConfig =
               static_cast<DwsCONF1Config *>(m_pzVehicleNetworkCfg->GetDwsConfig());
          /* Register callback to CanWrapper */
          /* Configure listener to get only this frame on all interfaces*/
          earIdDws = m_pzCanWrapper->registerListener(dwsConfig->m_iFrameId,
                                                      CwBase::MASK11,
                                                      cbOnNewCanFrame,
                                                      (void *)(this),
                                                      CwBase::IFACE_ANY);

          mearIdDws = m_pzCanWrapper->registerBufferedDataListener(dwsConfig->m_iFrameId,
                                                                   CwBase::MASK11,
                                                                   cbOnNewBufferedCanFrame,
                                                                   (void *)(this),
                                                                   CwBase::IFACE_ANY);

          m_pzCanWrapper->enableBuffering(CwBase::IFACE_ANY,
                                          dwsConfig->m_iFrameId,
                                          CwBase::MASK11);

          if (earIdDws == 0 || mearIdDws ==  0)
          {
              SLIM_LOGE("Error registering CanWrapper listener for Speed");
          }
        }
      }
      else
      {
        SLIM_LOGD("m_pzDwsProc was NOT created");
      }
    }
  }
  else
  {
    SLIM_LOGD("m_pzDwsConfig was NOT loaded");
  }


  if (0 != m_pzVehicleNetworkCfg->GetGearConfig())
  {
    m_pzVehicleNetworkCfg->GetGearConfig()->ToString(pzTStr, sizeof(pzTStr));
    SLIM_LOGD("m_pzGearConfig: %s", pzTStr);

    if (m_pzVehicleNetworkCfg->GetGearConfig()->CfgValid())
    {
      m_pzGearProc = VehicleNetworkMessageProcessor::CreateProcessor(
          this,
          m_pzVehicleNetworkCfg->GetGearConfig(),
          eSLIM_SERVICE_VEHICLE_GEAR);
      if (0 != m_pzGearProc)
      {
        m_pzGearProc->ToString(pzTStr, sizeof(pzTStr));
        SLIM_LOGD("m_pzGearProc: %s", pzTStr);
        if (VehicleNetworkConfiguration::eGEAR_CONFIG_CONF1 ==
               m_pzVehicleNetworkCfg->GetGearConfig()->GetVersion())
        {
          GearCONF1Config *gearConfig =
               static_cast<GearCONF1Config *>(m_pzVehicleNetworkCfg->GetGearConfig());
          /* Register callback to CanWrapper */
          /* Configure listener to get only this frame on all interfaces*/
          earIdGear = m_pzCanWrapper->registerListener(gearConfig->m_iFrameId,
                                                       CwBase::MASK11,
                                                       cbOnNewCanFrame,
                                                       (void *)(this),
                                                       CwBase::IFACE_ANY);

          mearIdGear = m_pzCanWrapper->registerBufferedDataListener(gearConfig->m_iFrameId,
                                                                    CwBase::MASK11,
                                                                    cbOnNewBufferedCanFrame,
                                                                    (void *)(this),
                                                                    CwBase::IFACE_ANY);

          m_pzCanWrapper->enableBuffering(CwBase::IFACE_ANY,
                                          gearConfig->m_iFrameId,
                                          CwBase::MASK11);

          if (earIdGear == 0 || mearIdGear ==  0)
          {
              SLIM_LOGE("Error registering CanWrapper listener for Gear");
          }
        }
      }
      else
      {
        SLIM_LOGD("m_pzGearProc was NOT created");
      }
    }
  }
  else
  {
    SLIM_LOGD("m_pzGearConfig was NOT loaded");
  }

  // slim_TimerStart(m_pzTimer, 500, 0);
  // Notify controller that the task is ready to run.
  slim_TaskReadyAck();
  while (1)
  {
    slim_IpcMsgT* pz_Msg = NULL;
    while (NULL == (pz_Msg = slim_IpcReceive()))
      ;

    SLIM_LOGD("IPC message received. q_MsgId:%" PRIu32 ", q_SrcThreadId:%" PRIu32,
        (uint32_t)pz_Msg->q_MsgId,
        (uint32_t)pz_Msg->q_SrcThreadId);

    switch (pz_Msg->q_MsgId)
    {
    case eIPC_MSG_NEW_FRAME_EVENT:
    {
      SLIM_LOGD("New frame event received");
      CanFrameEvent* pzNewEvent;
      slim_Memscpy(&pzNewEvent, sizeof(pzNewEvent), pz_Msg->p_Data,
          pz_Msg->q_Size);
      MutexLock _l(m_zMutex);
      if ((m_pzAccelProc) && (earIdAccel) && (mearIdAccel))
        m_pzAccelProc->OnNewCanFrameEvent(pzNewEvent);
      if ((m_pzGyroProc) && (earIdGyro) && (mearIdGyro))
        m_pzGyroProc->OnNewCanFrameEvent(pzNewEvent);
      if ((m_pzOdometryProc) && (earIdOdo) && (mearIdOdo))
        m_pzOdometryProc->OnNewCanFrameEvent(pzNewEvent);
      if ((m_pzSpeedProc) && (earIdSpeed) && (mearIdSpeed))
        m_pzSpeedProc->OnNewCanFrameEvent(pzNewEvent);
      if ((m_pzDwsProc) && (earIdDws) && (mearIdDws))
        m_pzDwsProc->OnNewCanFrameEvent(pzNewEvent);
      if ((m_pzGearProc) && (earIdGear) && (mearIdGear))
        m_pzGearProc->OnNewCanFrameEvent(pzNewEvent);
      delete pzNewEvent;
    }
      break;

    case eIPC_MSG_TIMER:
    {
      SLIM_LOGD("Timer signal received");
      os_TimerExpiryType z_TimerMsg;
      slim_Memscpy(&z_TimerMsg, sizeof(z_TimerMsg), pz_Msg->p_Data,
          pz_Msg->q_Size);
      MutexLock _l(m_zMutex);
      if (m_pzAccelProc)
        m_pzAccelProc->OnNewTimerExpiryEvent(&z_TimerMsg);
      if (m_pzGyroProc)
        m_pzGyroProc->OnNewTimerExpiryEvent(&z_TimerMsg);
      if (m_pzOdometryProc)
        m_pzOdometryProc->OnNewTimerExpiryEvent(&z_TimerMsg);
      if (m_pzSpeedProc)
        m_pzSpeedProc->OnNewTimerExpiryEvent(&z_TimerMsg);
      if (m_pzDwsProc)
        m_pzDwsProc->OnNewTimerExpiryEvent(&z_TimerMsg);
      if (m_pzGearProc)
        m_pzGearProc->OnNewTimerExpiryEvent(&z_TimerMsg);
      if (!slim_TimerStart(m_pzTimer, 500, 0))
      {
        SLIM_LOGE("Error starting timer");
      }
    }
      break;
    default:
    {
      MutexLock _l(m_zMutex);
      if (m_pzAccelProc)
        m_pzAccelProc->OnNewIpcMessage(pz_Msg);
      if (m_pzGyroProc)
        m_pzGyroProc->OnNewIpcMessage(pz_Msg);
      if (m_pzOdometryProc)
        m_pzOdometryProc->OnNewIpcMessage(pz_Msg);
      if (m_pzSpeedProc)
        m_pzSpeedProc->OnNewIpcMessage(pz_Msg);
      if (m_pzDwsProc)
        m_pzDwsProc->OnNewIpcMessage(pz_Msg);
      if (m_pzGearProc)
        m_pzGearProc->OnNewIpcMessage(pz_Msg);
    }
      break;
    }

    slim_IpcDelete(pz_Msg);
  }
}

/**
 @brief Method for enabling or disabling vehicle services.

 @param[in] uEnable Flag that indicates if the service shall be enabled or
 disabled.
 @param[in] eService      Service to control.

 @return eSLIM_SUCCESS is operation succeeded.
 */
slimErrorEnumT VehicleNetworkProvider::doUpdateVehicleDataStatus(
bool uEnable, slimServiceEnumT eService)
{
  MutexLock _l(m_zMutex);
  if (uEnable)
  {
    SLIM_MASK_SET(m_qEnabledServices, eService);
    if (eSLIM_SERVICE_VEHICLE_SPEED == eService) {
      uint64_t tProviderTimeMs = slim_TimeTickGetMilliseconds();
      SLIM_LOGV("Calling releaseBufferedData at %"PRId32 "msec from boot up", tProviderTimeMs);
      m_pzCanWrapper->releaseBufferedData(CwBase::IFACE_ANY);
    }
  }
  else
  {
    SLIM_MASK_CLEAR(m_qEnabledServices, eService);
  }
  if (0 == m_qEnabledServices)
  {
    slim_TimerStop(m_pzTimer);
  }
  else
  {
    slim_TimerStart(m_pzTimer, 500, eTIMER_DEFAULT);
  }
  return eSLIM_SUCCESS;
}

/**
 @brief Initiates time offset request.

 Function for making the time request. Successful response enable SLIM to
 calculate the offset between modem time and sensor time.

 @param[in] lTxnId Service transaction id.
 @return eSLIM_SUCCESS if time request is made successfully.
 */
slimErrorEnumT VehicleNetworkProvider::getTimeUpdate(int32_t lTxnId)
{
/*  return MultiplexingProvider::getTimeUpdate(lTxnId); */
  SLIM_LOG_ENTRY();

  SLIM_LOGD("Requesting time update: txn=%"PRId32, lTxnId);

  slimErrorEnumT retVal = eSLIM_ERROR_INTERNAL;

  if ( 0 != m_tLastBusControllerSyncTimestamp &&
       0 != m_tLastBusControllerSyncTime)
  {
    uint64_t t_Time = m_tLastBusControllerSyncTimestamp;
    t_Time += (slim_TimeTickGetMilliseconds() - m_tLastBusControllerSyncTime);

    routeTimeResponse(lTxnId, eSLIM_SUCCESS, t_Time, t_Time);
    SLIM_LOGD("Time update reported:%"PRIu64, t_Time);
    retVal = eSLIM_SUCCESS;
  }

  SLIM_LOGD("retVal= %d", retVal);
  return retVal;
}

