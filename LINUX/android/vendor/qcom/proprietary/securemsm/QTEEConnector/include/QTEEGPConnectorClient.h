/********************************************************************
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#ifndef CLIENT_INCLUDE_QTEEGPCONNECTORCLIENT_H_
#define CLIENT_INCLUDE_QTEEGPCONNECTORCLIENT_H_

#include <utils/Log.h>
#include <vendor/qti/hardware/qteeconnector/1.0/IGPApp.h>
#include <vendor/qti/hardware/qteeconnector/1.0/IGPAppConnector.h>
#include <mutex>
#include <string>
#include <vector>
#include "QSEEComAPI.h"

namespace QTEE {

using ::vendor::qti::hardware::qteeconnector::V1_0::IGPApp;
using ::vendor::qti::hardware::qteeconnector::V1_0::IGPAppConnector;
using ::vendor::qti::hardware::qteeconnector::V1_0::QTEECom_ion_fd_info;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_death_recipient;
using android::sp;
using android::wp;

/**
 * @brief Helper class for connection via IGPApp/IGPAppConnector
 *
 * This class wraps the IGPAppConnector/IGPApp hwbinder interface used to load and to communicate
 * with
 * GP applications. It should be transparent to the GPTEE environment.
 */
class QTEEGPConnectorClient {
  /**
   * @brief Notifier if the service dies
   *
   * If the service dies, the notifier tries to re-establish the connection.
   */
  struct QTEEGPDeathNotifier : hidl_death_recipient {
    QTEEGPDeathNotifier(QTEEGPConnectorClient& QTEEGPc);
    virtual void serviceDied(uint64_t cookie, android::wp<IBase> const& who);
    QTEEGPConnectorClient& mQTEEGPc;
  };

 public:
  QTEEGPConnectorClient() = delete;
  /**
   * @brief create a QTEEGPConnectorClient
   *
   * The constructor tries to establish a connection to the service.
   *
   * @note The size of the shared memory can be overridden by the daemon, if the
   * QSEE Application is configured to require a larger memory than here requested.
   *
   * @note The requested size MUST take into account alignment and padding of all the
   * command and response buffers which are going to be used in subsequent calls to
   * openSession, invokeCommand or closeSession.
   *
   * @param[in] path path in the HLOS file system where the QSEE App is located
   * @param[in] name name of the QSEE application to be loaded
   * @param[in] requestedSize size of the shared memory associated with the QSEE Application
   */
  QTEEGPConnectorClient(std::string const& path, std::string const& name, uint32_t requestedSize);

  /**
   * @brief Destructor
   *
   * If the app is still loaded, the client attempts to unload the application.
   */
  virtual ~QTEEGPConnectorClient();

  /**
   * @brief load the GP application
   *
   * @return true on success, false otherwise
   */
  bool load();

  /**
   * @brief load the GP application
   *
   * @return true on success, false otherwise
   */
  void unload();

  /**
   * @brief check whether the loaded app is a 64bit application
   *
   * @return true if the loaded app is 64bit, false otherwise
   */
  bool isApp64() const { return mIsApp64; }

  /**
   * @brief add dependencies for this GP application
   *
   * @param[in] appPath the path for the dependency
   * @param[in] appName the name of the dependency
   */
  void add_dep_uuid(std::string const& appPath, std::string const& appName);

  /**
   * @brief add dependencies for this GP application
   *
   * @note it is assumed the dependencies are stored in the same HLOS path
   *
   * @param[in] appName the name of the dependency
   */
  void add_dep_uuid(std::string const& appName);

  /**
   * @brief invoke a command in the GP application

   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   * @param[in] info a description of the memory references shared with the trusted application
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t invokeCommand(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen,
                                  struct QSEECom_ion_fd_info const* info);

  /**
   * @brief open a session to a GP application

   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   * @param[in] info a description of the memory references shared with the trusted application
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t openSession(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen,
                                struct QSEECom_ion_fd_info const* info);

  /**
   * @brief close a session with a GP application

   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t closeSession(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen);

  /**
   * @brief request a cancellation
   * @param[in] sessionId the session id to be cancelled
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t requestCancellation(uint32_t sessionId);

 private:
  /**
   * @brief attempt to recover the connection to the service
   *
   * @return true if recovery succeeded, false otherwise
   */
  bool recover();
  /**
   * @brief load the applications dependencies
   *
   * @return true on success, false otherwise
   */
  bool loadDependencies();

  /**
   * @brief load the application
   *
   * @return true on success, false otherwise
   */
  bool doLoad();

  sp<IGPAppConnector> mGPAppConnector;  ///< a strongpointer to the IGPAppConnector interface
  sp<IGPApp> mGPApp;  ///< a strongpointer representing the application at the service side
  sp<QTEEGPDeathNotifier> mDeathNotifier;  ///< the death notifier

  std::string const mPath;        ///< the path of the GP application
  std::string const mName;        ///< the name of the GP application
  uint32_t const mRequestedSize;  ///< the originally requested buffer size
  bool mIsApp64;                  ///< whether the application is 64bit
  bool mLoaded;                   ///< whether the application is loaded
  std::mutex mutable mQTEEmutex;           ///< a mutex for all calls to the service
};
};  // namespace QTEE

#endif  // CLIENT_INCLUDE_QTEEGPCONNECTORCLIENT_H_
