/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief SLIM Daemon header file

@ingroup slim_APSS
*/
#ifndef SLIM_DAEMON_MANAGER_H
#define SLIM_DAEMON_MANAGER_H

#include <qmislim_common_api.h>
#include <slim_provider_conf.h>

#ifdef FEATURE_LOCTECH_SLIM_NDK
#include <SlimNDKProvider.h>
#endif

/* Extra C APIs*/
extern "C"
{
/**
@brief Enable or disable logs

@param u_EnableFlag When set to TRUE logging is enabled.
*/
void slim_LogSetEnabled
(
  boolean u_EnableFlag
);

}

//! @addtogroup slim_APSS
//! @{

namespace slim
{
/* Forward declaration */
class ClientListener;
/**
 * @brief Class for main SLIM service daemon thread.
 *
 * All top-level application logic is implemented by this class. The class
 * handles application configuration, SLIM core operation and external
 * interfaces.
 *
 */
class SlimDaemonManager
{
private:
  /** Command line options used with @a getopt_long */
  enum
  {
    eAPP_CMD_DEFAULT = 0,
    /**< Default command line parsing option */
    eAPP_CMD_LONG_DISABLE_CLIENT,
    /**< Disable client command line option */
    eAPP_CMD_LONG_ENABLE_CLIENT,
    /**< Enable client command line option */
    eAPP_CMD_LONG_DISABLE_PROVIDER,
    /**< Disable provider command line option */
    eAPP_CMD_LONG_ENABLE_PROVIDER,
    /**< Enable provider command line option */
    eAPP_CMD_LONG_FORK,
    /**< Fork child process */
    eAPP_CMD_LONG_PIP,
    /**< PIP launcher option */
    eAPP_CMD_LONG_SAP,
    /**< SAP launcher option */
    eAPP_CMD_SHORT_HELP = 'h',
    /**< Print help command line option */
  };
  /** sap.conf configuration values for SENSOR_PROVIDER */
  enum
  {
    eAPP_CFG_PROVIDER_SENSOR1 = 1,
    /**< Default provider is SSC */
    eAPP_CFG_PROVIDER_NDK = 2,
    /**< Default provider is Native */
  };
  /** sap.conf configuration values for SENSOR_PROVIDER */
  enum NdkProviderTimeSource
  {
    eAPP_CFG_NDK_TIMESRC_UNKNOWN = 0,
    eAPP_CFG_NDK_TIMESRC_BOOTIME = 1,
    eAPP_CFG_NDK_TIMESRC_MONOTONIC = 2,
    eAPP_CFG_NDK_TIMESRC_REALTIME = 3,
    eAPP_CFG_NDK_TIMESRC_ELAPSED = 4,
  };
  enum BatchModeEnum
  {
    eBATCH_MODE_DEFAULT,
    /**< Use default batch settings */
    eBATCH_MODE_ON,
    /**< Force batch mode on */
    eBATCH_MODE_OFF,
    /**< Force batch mode off */
  };
  /** IZAT configuration options */
  enum IZatModeEnum
  {
    eIZAT_MODE_DISABLED, /**< Feature is disabled */
    eIZAT_MODE_BASIC,    /**< Feature is enabled in basic mode */
    eIZAT_MODE_PREMIUM,  /**< Feature is enabled in premium mode */
  };
  /** Provider configuration info */
  struct ProviderInfoCfg
  {
    slim_ProviderEnumType eProvider;
    /**< Provider type */
    const char *pzArgName;
    /**< Command line argument string */
    const char *pzHelp;
    /**< Usage help string*/
  };
  /** Client configuration info */
  struct ClientInfoCfg
  {
    slim_ClientEnumType eClient;
    /**< Provider type */
    const char *pzArgName;
    /**< Command line argument string */
    const char *pzHelp;
    /**< Usage help string*/
  };
  /** IZAT configuration info */
  struct IzatInfoCfg
  {
    IZatModeEnum eMode;
    /**< Provider type */
    const char *pzArgName;
    /**< Command line argument string */
    const char *pzHelp;
    /**< Usage help string*/
  };

  /** Provider mappings configuration */
  static const ProviderInfoCfg c_zProviderConfigs[];
  /** Client mappings configuration */
  static const ClientInfoCfg c_zClientConfigs[];
  /** Izat mappings configuration */
  static const IzatInfoCfg c_zIzatModeConfigs[];

  /**
   * @brief Default sensor provider for IPC-SLIM interface.
   */
  slim_ProviderSettingEnumType m_eProviderSetting;
  /**
   * @brief Service configuration flags.
   */
  uint32_t                     m_qSensorUsage;
  /**
   * @brief Configuration flag for controlling QMI-LOC interface.
   */
  bool                         m_uEnableQmiLocInterface;
  /**
   * @brief QMI-SLIM peer usage mask.
   * @sa slim_QSNodeT
   */
  uint32_t                     m_qQSClientMask;
  /**
   * @brief Provider usage mask.
   * @sa slim_ProviderEnumType
   */
  uint32_t                     m_qProviderMask;
  /**
   * @brief QMI-LOC interface object.
   */
  ClientListener              *m_pzQLClientListener;
  /**
   * @brief Batch mode configuration
   */
  BatchModeEnum               m_eBatchMode;
  /**
   * @breif PIP IZat mode
   */
  IZatModeEnum                m_ePipMode;
  /**
   * @breif SAP IZat mode
   */
  IZatModeEnum                m_eSapMode;
  /**
   * @brief Android HAL sensor time source
   */
#ifdef FEATURE_LOCTECH_SLIM_NDK
  NDKProvider::TimeSource     m_eNdkTimeSource;
#endif
  /**
   * @brief Fork child process and exit parent.
   */
  void forkChild();
  /**
   * @brief Configures process capabilities.
   *
   * Process starts with root privileges and configures capabilities and
   * permissions before execution.
   *
   * @retval true on success
   * @retval false on failure
   */
  bool setupCapabilities();
  /**
   * @brief Prints process capabilities to log.
   */
  void printCapabilities();

  /**
   * @brief Load SAP and GPS configuration files
   */
  void loadConfiguration();
  /**
   * @brief Initializes external client interfaces.
   *
   * The method initializes additional external client interfaces in addition
   * to QMI-SLIM service interface.
   *
   * Currently the following interfaces are supported:
   * - QMI-LOC interface for legacy GNSS builds.
   * - IPC-SLIM interface for AP clients (including PIP).
   * - QMI-SLIM AP/MP/Test interfaces with QMI-SLIM providers.
   *
   * @retval 0 on success
   */
  int initClientInterfaces(void);

  /**
   * @brief Print application usage help.
   * @param[in] pzName Application name.
   */
  void printHelp
  (
    const char *pzName
  );

  /**
   * @brief Convert provider argument into enumeration.
   * @param[in]  pzName       Command line parameter.
   * @param[out] eProviderOut Conversion result
   * @retval true  on success
   * @retval false on failure
   */
  static bool parseProviderArg
  (
    const char* pzName,
    slim_ProviderEnumType& eProviderOut
  );
  /**
   * @brief Convert client argument into enumeration.
   * @param[in]  pzName     Command line parameter.
   * @param[out] eClientOut Conversion result
   * @retval true  on success
   * @retval false on failure
   */
  static bool parseClientArg
  (
    const char *pzName,
    slim_ClientEnumType &eClientOut
  );
  /**
   * @brief Convert IZAT mode argument into enumeration.
   * @param[in]  pzName   Command line parameter.
   * @param[out] eModeOut Conversion result
   * @retval true  on success
   * @retval false on failure
   */
  static bool parseIzatModeArg
  (
    const char *pzName,
    IZatModeEnum &eModeOut
  );
public:
  /**
   * Sets up signal mask and initializes service facilities.
   */
  SlimDaemonManager();
  /**
   * Restores signal mask and terminates services.
   */
  ~SlimDaemonManager();

  /**
   * @brief Handles main application event loop.
   *
   * Main application event loop initializes SLIM facilities, external
   * interfaces and processes SLIM core and application events.
   *
   * The following events are handled:
   * - SLIM task messages
   * - User signals (including Ctrl-C handling)
   * - Timer signals
   * - QMI CSI select results
   */
  void run();
  /**
   * @brief Initialize application object
   *
   * Function performs command-line argument parsing and configuration loading.
   *
   * @param argc Count of command line arguments
   * @param argv Command line arguments as string array
   */
  void initialize
  (
    int argc,
    char *argv[]
  );

};

} /* namespace slim */

//! @}

#endif //SLIM_DAEMON_MANAGER_H
