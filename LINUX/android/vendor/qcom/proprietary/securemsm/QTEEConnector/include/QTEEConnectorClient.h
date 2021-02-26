/********************************************************************
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#ifndef CLIENT_INCLUDE_QTEECONNECTORCLIENT_H_
#define CLIENT_INCLUDE_QTEECONNECTORCLIENT_H_

#include <utils/Log.h>
#include <vendor/qti/hardware/qteeconnector/1.0/IApp.h>
#include <vendor/qti/hardware/qteeconnector/1.0/IAppConnector.h>
#include <mutex>
#include <string>
#include "QSEEComAPI.h"

namespace QTEE {

using ::vendor::qti::hardware::qteeconnector::V1_0::IApp;
using ::vendor::qti::hardware::qteeconnector::V1_0::IAppConnector;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_death_recipient;
using android::sp;
using android::wp;

/**
 * @brief Helper class for connection via IApp/IAppConnector
 *
 * This class wraps the IAppConnector/IApp hwbinder interface used for connections
 * via the QSEEComAPI to match the earlier used QSEEConnectorClient.h
 */
class QTEEConnectorClient {
  /**
   * @brief Notifier if the service dies
   *
   * If the service dies, the notifier tries to re-establish the connection.
   */
  struct QTEEDeathNotifier : hidl_death_recipient {
    QTEEDeathNotifier(QTEEConnectorClient& QTEEc);
    virtual void serviceDied(uint64_t cookie, android::wp<IBase> const& who);
    QTEEConnectorClient& mQTEEc;
  };

 public:
  QTEEConnectorClient() = delete;
  /**
   * @brief create a QTEEConnectorClient
   *
   * The constructor tries to establish a connection to the service.
   *
   * @note The size of the shared memory can be overridden by the daemon, if the
   * QSEE Application is configured to require a larger memory than here requested.
   *
   * @note The requested size MUST take into account alignment and padding of all the
   * command and response buffers which are going to be used in subsequent calls to
   * sendCommand and sendModifiedCommand.
   *
   * @param[in] path path in the HLOS file system where the QSEE App is located
   * @param[in] name name of the QSEE application to be loaded
   * @param[in] requestedSize size of the shared memory associated with the QSEE Application
   */
  QTEEConnectorClient(std::string const& path, std::string const& name, uint32_t requestedSize);

  /**
   * @brief Destructor
   *
   * If the app is still loaded, the client attempts to unload the application.
   */
  virtual ~QTEEConnectorClient();

  /**
   * @brief load the trusted application
   *
   * @return true on success, false otherwise
   */
  bool load();
  /**
   * @brief unload the trusted application
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
   * @brief send a command to the trusted application
   *
   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t sendCommand(void const* req, uint32_t reqLen, void* rsp, uint32_t rspLen);
  /**
   * @brief send a modified command to the trusted application
   *
   * @param[in] req the request buffer
   * @param[in] reqLen length of the request buffer
   * @param[out] rsp the response buffer
   * @param[in] rspLen length of the response buffer
   * @param[in] info a description of the memory references shared with the trusted application
   *
   * @return android::OK on success, errorcode otherwise
   */
  android::status_t sendModifiedCommand(void const* req, uint32_t reqLen, void* rsp,
                                        uint32_t rspLen, struct QSEECom_ion_fd_info const* info);

 private:
  /**
   * @brief attempt to recover the connection to the service
   *
   * @return true if recovery succeeded, false otherwise
   */
  bool recover();

  /**
   * @brief load the application
   *
   * @return true on success, false otherwise
   */
  bool doLoad();

  sp<IAppConnector> mAppConnector;  ///< a strongpointer to the IAppConnector interface
  sp<IApp> mApp;  ///< a strongpointer representing the application at the service side
  sp<QTEEDeathNotifier> mDeathNotifier;  ///< the death notifier

  std::string const mPath;        ///< the path of the trusted application
  std::string const mName;        ///< the name of the trusted application
  uint32_t const mRequestedSize;  ///< the originally requested buffer size
  bool mIsApp64;                  ///< whether the application is 64bit
  bool mLoaded;                   ///< whether the application is loaded
  std::mutex mutable mQTEEmutex;  ///< a mutex for all calls to the service
};
};  // namespace QTEE

#endif  // CLIENT_INCLUDE_QTEECONNECTORCLIENT_H_
