#ifndef _ICNE_OBSERVER_DEFS_H_
#define _ICNE_OBSERVER_DEFS_H_
/**
 * @file ICneObserverDefs.h
 *
 * @brief
 * Connectivity Engine Observer CO Definitions Header file
 *
 * This file contains the definitions of constants, data structures and
 * interfaces for the Connectivity Engine Observer.
 *
 *
 *                   Copyright 2010-2013, 2015-2016 Qualcomm Technologies, Inc.
 *                   All Rights Reserved.
 *                   Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <netinet/in.h>
#include <vector>
#include <time.h>
#include <android/multinetwork.h>
#ifndef IFHWADDRLEN
#define IFHWADDRLEN 6
#endif

#ifndef MAXSSIDLEN
#define MAXSSIDLEN 32
#endif

#ifndef MAXDNSADDRS
#define MAXDNSADDRS 2
#endif

/**
 * @addtogroup coi_ds
 * @{
 * Various Error codes returned by Connectivity Engine Observers APIs
 */
typedef enum
{
  CO_RET_SUCCESS             =  0, /**< -- Success */
  CO_RET_FAILED              = -1, /**< -- Generic error */
  CO_RET_SERVICE_UNAVAILABLE = -2, /**< -- CNO service unavailable */
  CO_RET_NOT_ALLOWED         = -3, /**< -- Operation not allowed */
  CO_RET_NET_INACTIVE        = -4, /**< -- Network is inactive */
  CO_RET_INVALID_ARGS        = -5, /**< -- Invalid argument */
  CO_RET_PERM_DENIED         = -6, /**< -- Permission denied */
  CO_RET_NO_NOTIFIER         = -7, /**< -- No notifier is registered */
}
CORetType;
/**
 * @} end group coi_ds
 */

/**
 * @addtogroup cno_ds
 * @{
 */

/**
 * Possible Connectivity Engine Network Observer Signal types
 */
typedef enum
{
  SIGNAL_NET_ALL = 0,                   /**< -- All Signals */
  SIGNAL_NET_MIN = SIGNAL_NET_ALL,
  SIGNAL_NET_CONFIG_CHANGE,             /**< -- onNetConfigChange signal */
  SIGNAL_NETWORK_REQUEST_CHANGE,        /**< -- onNetworkRequestChange signal */
  SIGNAL_NET_MAX = SIGNAL_NETWORK_REQUEST_CHANGE
} CnoSignalType;

/**
 * Possible Connectivity Engine Network Observer network types
 */
typedef enum
{
  NET_WWAN = 0, /**< -- RAT type WWAN */
  NET_WLAN = 1,     /**< -- RAT type WLAN */
  NET_WWAN_MMS = 2,
  NET_WWAN_SUPL = 3,
  /*NET_WWAN_DUN,*/
  /*NET_WWAN_FOTA,*/
  NET_WWAN_IMS = 4,
  /*NET_WWAN_CBS,*/
  /*NET_WWAN_IA,*/
  NET_WWAN_RCS = 5,
  /*NET_WWAN_XCAP,*/
  NET_WWAN_EIMS = 6,
  NET_WWAN_EMERGENCY = 7,

  NET_WLAN_SOFTAP = 100,
  NET_WLAN_P2P = 101,

  NET_ANY = 253,
  NET_NONE = 254,
  NET_UNKNOWN = 255
} CnoNetType;

/**
 * Possible Connectivity Engine Network Observer network subtypes
 */
typedef enum
{
  NET_SUBTYPE_UNKNOWN = 0, /**< -- Unknown */
  NET_SUBTYPE_GPRS = 1,        /**< -- Sub type GPRS */
  NET_SUBTYPE_EDGE = 2,        /**< -- Sub type EDGE */
  NET_SUBTYPE_UMTS = 3,        /**< -- Sub type UMTS */
  NET_SUBTYPE_CDMA = 4,        /**< -- Sub type CDMA IS-95 */
  NET_SUBTYPE_EVDO_0 = 5,      /**< -- Sub type EVDO Rev 0 */
  NET_SUBTYPE_EVDO_A = 6,      /**< -- Sub type EVDO Rev A */
  NET_SUBTYPE_1xRTT = 7,       /**< -- Sub type 1x RTT */
  NET_SUBTYPE_HSDPA = 8,       /**< -- Sub type HSDPA */
  NET_SUBTYPE_HSUPA = 9,       /**< -- Sub type HSUPA */
  NET_SUBTYPE_HSPA = 10,        /**< -- Sub type HSPA */
  NET_SUBTYPE_IDEN = 11,        /**< -- Sub type IDEN */
  NET_SUBTYPE_EVDO_B = 12,      /**< -- Sub type EVDO Rev B */
  NET_SUBTYPE_LTE = 13,         /**< -- Sub type LTE */
  NET_SUBTYPE_EHRPD = 14,
  NET_SUBTYPE_HSPAP = 15,       /**< -- Sub type HSPAP */
  NET_SUBTYPE_LTE_CA = 19,

  NET_SUBTYPE_WLAN_B = 100, /**< -- Sub type 802.11 B */
  NET_SUBTYPE_WLAN_G = 101       /**< -- Sub type 802.11 G */
} CnoNetSubtype;

typedef enum
{
  SIM_SLOT_UNSPECIFIED = 0,
  SIM_SLOT_FIRST_IDX,
  SIM_SLOT_SECOND_IDX,
  SIM_SLOT_THIRD_IDX,
  SIM_SLOT_MAX_IDX = SIM_SLOT_THIRD_IDX
} CnoSimSlotIdxType;

typedef struct CnoNetworkRequestStruct{
  CnoNetType net;
  CnoSimSlotIdxType slot;

  CnoNetworkRequestStruct(){
    net = NET_UNKNOWN;
    slot = SIM_SLOT_UNSPECIFIED;
  }

  bool operator<(const struct CnoNetworkRequestStruct& other) const {
       return ((net < other.net) || (net == other.net && slot < other.slot));
  }
  bool operator==(const struct CnoNetworkRequestStruct& other) {
       return ((other.net == net) && (other.slot == slot));
  }
  bool operator!=(const struct CnoNetworkRequestStruct& other) {
       return ((other.net != net) || (other.slot != slot));
  }
} CnoNetworkRequestStruct;

/**
 * Connectivity Engine Network Observer network subtype structure
 */
typedef struct
{
  CnoNetSubtype fl; /**< fwd link subtype */
  CnoNetSubtype rl; /**< rev link subtype */
} CnoNetSubTypeStruct;

typedef enum
{
  ERROR_INVALID,
  ERROR_SUBINFO_NOT_READY
} CnoErrorType;

/**
 * Connectivity Engine Network Observer network type struct
 */
typedef struct
{
  CnoNetType type;          /**< -- network type */
  CnoNetSubTypeStruct subtype; /**< -- network subtype */
  CnoSimSlotIdxType slot;
  CnoErrorType errorCause;
} CnoNetTypeStruct;

/**
 * Connectivity Engine Network Observer Network Configuration request response
 * type structure
 */
typedef struct
{
  CnoNetTypeStruct net;    /**< -- network type & subtype */
  in_addr ip4;             /**< -- network ipv4 address */
  in6_addr ip6;            /**< -- network ipv6 address */
  unsigned int mtu;        /**< -- mtu size in bytes */
  bool isDefault;          /**< -- true if network is default, false if not*/
  net_handle_t netHdl;             /**< -- network Handle */
  bool isConnected;        /**< -- true if network is connected, false otherwise */
} CnoNetConfigType;

/**
 * Connectivity Engine Network Observer Any Service response type
 * type structure
 */
typedef struct {
  int status;             /**< -- current state of the service */
  int error;              /**< -- current error with service */
} CnoServiceResponseType;

/**
 * Connectivity Engine Network Observer Any Service Status Indication type
 */
typedef enum
{
  SERVICE_STARTED,  /**< -- service started */
  SERVICE_STOPPED,  /**< -- service stopped */
} CnoServiceStatusType;

/**
 * Connectivity Engine Network Observer Any Service Error Indication type
 */
typedef enum
{
  CNO_SVC_NO_ERROR,           /**< -- No error detected */
  CNO_SVC_NET_INACTIVE,       /**< -- No network available */
  CNO_SVC_NET_NOT_SUPPORTED,  /**< -- Network not supported */
  CNO_SVC_BUSY,               /**< -- Service is already running */
  CNO_SVC_DISABLED,           /**< -- Service is disabled */
  CNO_SVC_GENERIC_ERR,        /**< -- Error while trying to start a service */
} CnoServiceErrorType;

/**
* Supplemental wlan network configuration information.
*/
typedef struct
{
  char bssid[IFHWADDRLEN]; /**< -- mac address of access point */
  char ssid[MAXSSIDLEN]; /**< -- SSID of access point */
  in_addr ip4dns[MAXDNSADDRS]; /**< -- IPv4 addresses of DNS server */
  in6_addr ip6dns[MAXDNSADDRS]; /**< -- IPv6 addresses of DNS server */
} CnoWlanSuppNetConfigType;


/**
 * @} end group cno_ds
 */

/**
 * @addtogroup cfo_ds
 * @{
 */

/**
 * Possible Connectivity Engine Feature Observer Signal types
 */
typedef enum
{
  SIGNAL_FEATURE_ALL = 0,                   /**< -- All Signals */
  SIGNAL_FEATURE_MIN = SIGNAL_FEATURE_ALL,
  SIGNAL_IWLAN_USER_PREF_CHANGE,        /**< -- OnIwlanUserPrefChange signal */
  SIGNAL_FEATURE_STATUS_CHANGE,         /**< -- OnFeatureStatusChange signal */
  SIGNAL_FEATURE_MAX = SIGNAL_FEATURE_STATUS_CHANGE
} CfoSignalType;


/**
* Connectivity Engine Feature Observer feature status type
*/
typedef enum
{
  CFO_STATUS_INACTIVE = 1,
  CFO_STATUS_MIN = CFO_STATUS_INACTIVE,
  CFO_STATUS_ACTIVE = 2 ,
  CFO_STATUS_MAX = CFO_STATUS_ACTIVE,
} CfoFeatureStatusType;

/**
* Connectivity Engine feature name type
*/
typedef enum
{
  CFO_FEATURE_WQE = 1, /* Wifi Quality Estimation */
  CFO_FEATURE_MIN = CFO_FEATURE_WQE,
  CFO_FEATURE_MAX = CFO_FEATURE_WQE
} CfoFeatureNameType;

/**
* Connectivity Engine Network Observer
* Inter-working WLAN (IWLAN) user preference type
*/
typedef enum
{
  CFO_IWLAN_PREF_UNSET = 0,
  CFO_IWLAN_PREF_MIN = CFO_IWLAN_PREF_UNSET,
  CFO_IWLAN_PREF_DISABLED,
  CFO_IWLAN_PREF_ENABLED,
  CFO_IWLAN_PREF_MAX = CFO_IWLAN_PREF_ENABLED
} CfoIwlanUserPrefType;

/**
* Connectivity Engine Network Observer
* IWLAN user preference acknowledgement type
*
* Indicates whether setting user preference
* was successful
*/
typedef enum
{
  CFO_IWLAN_ACK_FAIL = 0,
  CFO_IWLAN_ACK_MIN = CFO_IWLAN_ACK_FAIL,
  CFO_IWLAN_ACK_SUCCESS,
  CFO_IWLAN_ACK_MAX = CFO_IWLAN_ACK_SUCCESS
} CfoIwlanUserPrefAckType;

/**
 * @} end group cfo_ds
 */
#endif /* _ICNE_OBSERVER_DEFS_H_ */
