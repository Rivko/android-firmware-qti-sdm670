/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief C++ interface for QMI-SLIM QMI monitor component.
 *
 * @ingroup slim_ClientLibrary
 */
#ifndef __QMI_MONITOR_H_INCLUDED__
#define __QMI_MONITOR_H_INCLUDED__

#include <slim_api.h>

//! @addtogroup slim_ClientLibrary
//! @{

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#ifndef SLIM_CLIENT_API
/**
@brief Exported or imported API types
*/
#define SLIM_CLIENT_API __attribute__ ((visibility ("default")))
/**
@brief Local API types
*/
#define SLIM_CLIENT_LOCAL __attribute__ ((visibility ("hidden")))
#endif /* SLIM_CLIENT_API */

//! @}

//! @ingroup slim_ClientLibrary
namespace qmislim
{

class MonitorPrivate;
class Monitor;

//! @addtogroup slim_ClientLibrary
//! @{

/**
 * @brief C++ wrapper for QMI-SLIM QMI Monitor interface
 *
 * The object encapsulates plain C interface of QMI-SLIM QMI Monitor API into
 * C++ object.
 *
 * @sa QmiMonitor::ICallback
 * @sa slim_QSQmiMonitorT
 */
class SLIM_CLIENT_API Monitor
{
private:
  MonitorPrivate *m_pzPrivate;
  /**< @brief Private object data */
public:
  /**
   * @brief QMI Monitor callback interface.
   *
   * All interface methods are executed from QCCI thread context unless
   * there is an asynchronous error handling invoked.
   */
  class SLIM_CLIENT_API ICallback
  {
  protected:
    /**
     * @brief Pure virtual destructor for V-table completeness.
     */
    virtual ~ICallback() = 0;
  public:
    /**
     * @brief Notification of new QMI-SLIM service availability.
     */
    virtual void onQmiMonitorServiceAdded() = 0;
    /**
     * @brief Notification of QMI-SLIM service removal.
     */
    virtual void onQmiMonitorServiceRemoved() = 0;
    /**
     * @brief Notification of QMI-SLIM monitor release.
     */
    virtual void onQmiMonitorReleased() = 0;
  };

  /**
   * @brief Constructs monitor object
   */
  Monitor();
  /**
   * @brief Destroys monitor object
   */
  virtual ~Monitor();
  /**
   * @brief Initializes QCCI stack and starts monitoring for service events.
   *
   * @retval true On success.
   * @retval true On error.
   */
  bool Start();
  /**
   * @brief Releases QCCI stack.
   *
   * @retval true On success.
   * @retval true On error.
   */
  bool Stop
  (
    bool uSync
    /**< [in] Flag if the stop operation shall be synchronous or not. */
  );
  /**
   * @brief Sets or clears callback interface for event processing.
   *
   * @retval true On success.
   * @retval true On error.
   */
  bool SetCallback
  (
    ICallback *pzCallback
    /**< [in] New callback interface for handling events or 0 */
  );

}; /* class QmiMonitor */

//! @}

} /* namespace qmislim */

#endif /* __QMI_MONITOR_H_INCLUDED__ */
