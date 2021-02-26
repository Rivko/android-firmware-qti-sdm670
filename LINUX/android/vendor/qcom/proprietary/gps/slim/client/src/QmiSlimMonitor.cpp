/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief C++ implementation for QMI-SLIM QMI monitor component.
 *
 * @ingroup slim_ClientLibrary
 */
#include <inttypes.h>
#include <limits.h>

#include <qmislim_qmi_monitor_api.h>
#include <QmiSlimMonitor.h>

namespace qmislim
{

//! @addtogroup slim_ClientLibrary
//! @{

/*!
 * @brief Private implementation for QMI-SLIM monitor class
 *
 * Class hides all implementation details from the user of QMI-SLIM QMI monitor.
 * @sa qmislim::Monitor
 */
class SLIM_CLIENT_LOCAL MonitorPrivate
{
  friend class Monitor;

  static const slim_QSQmiMonitorCb_VtT c_zMonitorCbVt;

  Monitor *const m_pzMonitor;
  const slim_QSQmiMonitorCb_VtT *const m_pzMonitorCbVt;
  slim_QSQmiMonitorT z_QmiMonitor;
  Monitor::ICallback *m_pzCallback;

  static slim_QSQmiMonitorCb_ServiceAddedFn   ServiceAdded;
  static slim_QSQmiMonitorCb_ServiceRemovedFn ServiceRemoved;
  static slim_QSQmiMonitorCb_HandleReleasedFn HandleReleased;

  static MonitorPrivate *QmiClientCbI2O
  (
     slim_QSQmiMonitorCbT *p_Iface
  );
  static Monitor::ICallback *GetCallback
  (
      slim_QSQmiMonitorCbT *p_Iface
  );

  MonitorPrivate(Monitor *pzMonitor);
  ~MonitorPrivate();
};

//! @}

const slim_QSQmiMonitorCb_VtT MonitorPrivate::c_zMonitorCbVt =
{
    ServiceAdded,
    ServiceRemoved,
    HandleReleased,
};

MonitorPrivate *MonitorPrivate::QmiClientCbI2O
(
    slim_QSQmiMonitorCbT *p_Iface
)
{
   return reinterpret_cast<MonitorPrivate*>(
       slim_QSMember2Container(
          (void*)p_Iface,
          SLIM_QS_P2M_OFFSET(MonitorPrivate, m_pzMonitorCbVt)));
}

Monitor::ICallback *MonitorPrivate::GetCallback
(
    slim_QSQmiMonitorCbT *pzInterface
)
{
  MonitorPrivate *pzObject = QmiClientCbI2O(pzInterface);
  if (0 != pzObject)
  {
    return pzObject->m_pzCallback;
  }
  return 0;
}

MonitorPrivate::MonitorPrivate(Monitor *pzMonitor) :
    m_pzMonitor(pzMonitor),
    m_pzMonitorCbVt(&c_zMonitorCbVt),
    m_pzCallback(0)
{
}
MonitorPrivate::~MonitorPrivate()
{
}

void MonitorPrivate::ServiceRemoved
(
  slim_QSQmiMonitorCbT *pz_Iface
)
{
  Monitor::ICallback *pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiMonitorServiceRemoved();
  }
}
void MonitorPrivate::ServiceAdded
(
  slim_QSQmiMonitorCbT *pz_Iface
)
{
  Monitor::ICallback *pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiMonitorServiceAdded();
  }
}
void MonitorPrivate::HandleReleased
(
  slim_QSQmiMonitorCbT *pz_Iface
)
{
  Monitor::ICallback *pzCallback = GetCallback(pz_Iface);
  if (0 != pzCallback)
  {
    pzCallback->onQmiMonitorReleased();
  }
}
Monitor::ICallback::~ICallback()
{
}

/**
 * @brief Constructs monitor object
 */
Monitor::Monitor() : m_pzPrivate(0)
{
  m_pzPrivate = new MonitorPrivate(this);
  if (!slim_QSQmiMonitor_Init(&m_pzPrivate->z_QmiMonitor))
  {
    delete m_pzPrivate;
    m_pzPrivate = 0;
  }
}

/**
 * @brief Destroys monitor object
 */
Monitor::~Monitor()
{
  if (0 != m_pzPrivate)
  {
    slim_QSQmiMonitor_Deinit(&m_pzPrivate->z_QmiMonitor);
    delete m_pzPrivate;
  }
}

/**
 * @brief Initializes QCCI stack and starts monitoring for service events.
 *
 * @retval true On success.
 * @retval true On error.
 */
bool Monitor::Start()
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiMonitor_Start(&m_pzPrivate->z_QmiMonitor) != FALSE;
  }
  return false;
}

/**
 * @brief Releases QCCI stack.
 *
 * @retval true On success.
 * @retval true On error.
 */
bool Monitor::Stop
(
  bool uSync
  /**< [in] Flag if the stop operation shall be synchronous or not. */
)
{
  if (0 != m_pzPrivate)
  {
    return slim_QSQmiMonitor_Stop(
        &m_pzPrivate->z_QmiMonitor, uSync ? TRUE : FALSE) != FALSE;
  }
  return false;
}

/**
 * @brief Sets or clears callback interface for event processing.
 *
 * @retval true On success.
 * @retval true On error.
 */
bool Monitor::SetCallback
(
  ICallback *pzCallback
  /**< [in] New callback interface for handling events or 0 */
)
{
  if (0 != m_pzPrivate)
  {
    m_pzPrivate->m_pzCallback = pzCallback;
    return slim_QSQmiMonitor_SetCallback(
        &m_pzPrivate->z_QmiMonitor,
        0 == pzCallback ? 0 : &m_pzPrivate->m_pzMonitorCbVt) != FALSE;
  }
  return false;
}

} /* namespace qmislim */
