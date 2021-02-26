#ifndef QMISLIM_COMMON_API_H
#define QMISLIM_COMMON_API_H
/**
  @file
  @brief Defines types and methods common to QMI-SLIM component.
  @ingroup slim_QS

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/common/inc/qmislim_common_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qmislim_target.h"
#include "slim_client_types.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Doxygen Module Declarations
 * -------------------------------------------------------------------------*/

/**
  @addtogroup slim
  @{
  @defgroup slim_QS QMI-SLIM
  @brief QMI-SLIM general
  @}

  @defgroup slim_QS_IDL QMI IDL
  @brief QMI-SLIM IDL
  @ingroup slim_QS

  @addtogroup slim_qmi_consts
  @ingroup slim_QS_IDL
  @addtogroup slim_qmi_msg_ids
  @ingroup slim_QS_IDL
  @addtogroup slim_qmi_enums
  @ingroup slim_QS_IDL
  @addtogroup slim_qmi_messages
  @ingroup slim_QS_IDL
  @addtogroup slim_qmi_aggregates
  @ingroup slim_QS_IDL
  @addtogroup slim_qmi_accessor
  @ingroup slim_QS_IDL
  @addtogroup slim_qmi_version
  @ingroup slim_QS_IDL
  @addtogroup wms_qmi_accessor
  @ingroup slim_QS_IDL

  @defgroup slim_QSAlloc Allocators
  @ingroup slim_QS
  @brief API for allocating and releasing objects.

  @defgroup slim_QSQmiEndDec QMI Encoders and Decoders
  @ingroup slim_QS
  @brief QMI-SLIM message converters

  @defgroup slim_QSQmiClient QMI Client
  @brief QMI Client API for QMI-SLIM component
  @ingroup slim_QS

  @defgroup slim_QSQmiClientCb QMI Client Callback
  @brief QMI Client Callback Interface
  @ingroup slim_QSQmiClient

  @defgroup slim_QSQmiClientCbImpl QMI Client Callback Implementations
  @ingroup slim_QSQmiClient slim_QSQmiClientCb
  @brief Implementation of QMI Client callback interfaces

  @defgroup slim_QSQmiMonitor QMI Monitor
  @brief QMI Monitor
  @ingroup slim_QS

  @defgroup slim_QSQmiMonitorCb QMI Monitor Callback
  @ingroup slim_QSQmiMonitor

  @defgroup slim_QSQmiSvc QMI Service object
  @brief QMI-SLIM QMI Service object and implementation.
  @ingroup slim_QS

  @defgroup slim_QSQmiSvcCb QMI Service object callback
  @brief QMI Service object callback
  @ingroup slim_QSQmiSvc

  @defgroup slim_QSQmiSvcCbFactory QMI Service callback factory object
  @brief QMI Service callback factory object
  @ingroup slim_QSQmiSvc

  @defgroup slim_QSTask QMI-SLIM Tasks
  @ingroup slim_QS
  @brief QMI-SLIM Task Implementations

  QMI-SLIM Tasks integrate QMI-SLIM facilities into application using platform
  APIs.

  @defgroup slim_QSQmiMonitorTask QMI Monitor Task
  @ingroup slim_QSTask
  @brief QMI Monitor Task
  @sa slim_QSQmiMonitorTaskT

  @defgroup slim_QSQmiSvcTask QMI Service Task
  @ingroup slim_QSTask
  @brief QMI Service Task.

  QMI Service Task enables access of SLIM Client API using QMI-SLIM QMI
  interface.

  @sa slim_QSQmiSvcT

  @defgroup slim_QSQmiProviderTask QMI-SLIM Provider Task
  @ingroup slim_QSTask
  @brief Provider task

  Provider task is executed in the context of SLIM Core task, and handles a
  subset of messages that are forwarded to SLIM Core.

  @defgroup slim_QSQmi QMI integration
  @brief QMI-SLIM QMI integration
  @ingroup slim_QS

  QMI integration includes operations required for generic QCSI/QCCI framework
  usage with QMI-SLIM.

  @defgroup slim_QSQmiEndDec QMI Encoders and Decoders
  @ingroup slim_QS
  @brief QMI-SLIM message converters

  @defgroup slim_QSProvider QMI-SLIM Service Provider
  @brief QMI-SLIM SLIM Service Provider API and implementation
  @ingroup slim_QS

  @defgroup slim_QSProviderCore QMI-SLIM Service Provider Registry
  @ingroup slim_QS

  @defgroup slim_QSLog QMI-SLIM DM Logging Module
  @brief QMI-SLIM DM logging API and implementation
  @ingroup slim_QS
 */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
  @brief Marks variable as used to suppress unused variable warning.
  @ingroup slim_QS
 */
#define SLIM_QS_UNUSED(var) ((void)(var))

/**
  @brief Macro for structure member offset definition

  Member offset is an offset of the member variable inside the structure.
  Practically it is equal to address of that member if the object address is
  NULL.

  @param[in] type   Structure type
  @param[in] member Member name
  @ingroup slim_QS
 */
#define SLIM_QS_P2M_OFFSET(type,member) ((size_t)&(((type*)0)->member))

/**
  @defgroup slim_QS_LIST Linked lists
  @brief Linked lists API used by QMI-SLIM

  Linked list implementation is cloned from QMI framework sources. The
  differences from GNSS containers are:
  - List entry variable can be placed anywhere in the object; not necessary as
    a first entry.
  - The object can be participate in multiple different lists.
  - Compiler fully inlines list algorithms.
  - There is a strong type check by compiler.

  The implementation provides generic and robust implementation for dual linked
  list facility. The following macros are available:
  - @ref SLIM_QS_LIST - Declare list member variable.
  - @ref SLIM_QS_LIST_INIT - Initialize list variable.
  - @ref SLIM_QS_LIST_DEINIT - Deinitialize list variable.
  - @ref SLIM_QS_LIST_HEAD - Get list head.
  - @ref SLIM_QS_LIST_TAIL - Get list tail.
  - @ref SLIM_QS_LIST_CNT - Get list size.
  - @ref SLIM_QS_LIST_ADD - Add element to the tail.
  - @ref SLIM_QS_LIST_REMOVE - Remove element from the list.
  - @ref SLIM_QS_LIST_FIND - Find entry inside list by given criteria.
  - @ref SLIM_QS_LINK - Declare link entry member variable.
  - @ref SLIM_QS_LINK_INIT - Initialize list entry variable.
  - @ref SLIM_QS_LINK_NEXT - Get next linked entry.

  @sa qmi_common.h

  @ingroup slim_QS
  @{
 */

/**
  @brief Declares linked-list field
  @param[in] type Type of the lined elements
  @param[in] name Name of the entry field
 */
#define SLIM_QS_LIST(type, name) \
  struct { \
    type *head; \
    type *tail; \
    unsigned int count; \
  } name

/**
  @brief Declares field for supporting linked list
  @param[in] type Type of the lined elements.
  @param[in] link Name of the field.
 */
#define SLIM_QS_LINK(type, link) \
  struct { \
    type *prev; \
    type *next; \
  } link
/**
  Initializes linked list.

  @param list Linked list field (by reference)
 */
#define SLIM_QS_LIST_INIT(list) do {  \
  (list).head = (list).tail = NULL; \
  (list).count = 0; \
} while(0)

/**
  @brief Deinitializes linked list
  @param list Linked list field
 */
#define SLIM_QS_LIST_DEINIT(list) SLIM_QS_LIST_INIT(list)
/**
  @brief Initializes linked list entry
  @param link Entry reference
 */
#define SLIM_QS_LINK_INIT(link) (link).next = (link).prev = NULL
/**
  @brief Returns the first list element
  @param[in] list List
  @return The first element or NULL if the list is empty
 */
#define SLIM_QS_LIST_HEAD(list) (list.head)
/**
  @brief Returns the last list element
  @param[in] list List
  @return The last element or NULL if the list is empty
 */
#define SLIM_QS_LIST_TAIL(list) (list.tail)
/**
  @brief Returns next entry from the list
  @param[in] link Current entry link field
  @returns The next entry or NULL if the entry is the last one
 */
#define SLIM_QS_LINK_NEXT(link) (link.next)
/**
  @brief Returns number of linked elements
  @param[in] list List
  @return List size
 */
#define SLIM_QS_LIST_CNT(list) (list.count)
/**
  @brief Removes entry from list
  @param[in] list List
  @param[in] node Node to remove
  @param[in]  link Link field name
 */
#define SLIM_QS_LIST_REMOVE(list, node, link) \
  do { \
    if((node)->link.prev) \
      (node)->link.prev->link.next = (node)->link.next; \
    else /* node at the front of the list */ \
      list.head = (node)->link.next; \
    if((node)->link.next) \
      (node)->link.next->link.prev = (node)->link.prev; \
    else /* node at the end of the list */ \
      list.tail = (node)->link.prev; \
    list.count--; \
  } while(0)
/**
  @brief Removes entry from list
  @param[in] list List
  @param[in] node Node to remove
  @param[in]  link Link field name
 */
#define SLIM_QS_LIST_ADD(list, node, link) \
  do { \
    if(!list.tail) \
    { \
      /* first node on the list */ \
      list.tail = list.head = node; \
    } \
    else \
    { \
      (node)->link.prev = list.tail; \
      list.tail->link.next = node; \
      list.tail = node; \
    } \
    list.count++; \
  } while(0)
/**
  @brief Finds entry in the list

  @param[in]  list List to search
  @param[out] iter Iterator variable
  @param[in]  link Link field name
  @param[in]  test Test expression that uses iterator variable
 */
#define SLIM_QS_LIST_FIND(list, iter, link, test) do {  \
  iter = (list).head; \
  while(iter) { \
    if(test) {  \
      break;  \
    } \
    iter = (iter)->link.next;  \
  } \
} while(0)
/**
 * @}
 */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Type for enumerating supported QMI SLIM service instances.

  QMI-SLIM is built with assumption, there will be one client/service
  pair per processor. In practice, same chip may host multiple different
  service pairs, but they must be uniquely identified within this
  enumeration.

  @ingroup slim_QSAlloc
 */
typedef enum slim_QSNodeEnum
{
  eSLIM_QS_NODE_UNKNOWN, /*!< Unknown node */
  eSLIM_QS_NODE_MP,      /*!< Modem node */
  eSLIM_QS_NODE_AP,      /*!< Application processor node */
  eSLIM_QS_NODE_TEST,    /*!< Test node */
} slim_QSNodeT;

/**
  @brief Type for holding SLIM data objects

  The type is for reserving sufficient space to handle any of supported SLIM
  data messages.

  @ingroup slim_QSAlloc
 */
typedef union slim_QSSlimMessageDataS
{
  slimSensorDataStructT                     z_SensorData;
  /**< SLIM sensor data payload */
  slimQmdDataStructT                        z_QmdData;
  /**< SLIM QMD data payload */
  slimPedometerDataStructT                  z_PedometerData;
  /**< SLIM pedometer data payload */
  slimVehicleSensorDataStructT              z_VehicleSensorData;
  /**< SLIM vehicle sensor data payload */
  slimVehicleOdometryDataStructT            z_VehicleOdometryData;
  /**< SLIM vehicle odometry data payload */
  slimVehicleMotionDataStructT              z_VehicleMotionData;
  /**< SLIM vehicle motion data payload */

  slimServiceStatusEventStructT             z_ServiceStatus;
  /**< SLIM service status change payload */
} slim_QSSlimMessageDataT;

/* Forward typedefs for QMI-SLIM objects */
typedef struct slim_QSProviderS            slim_QSProviderT;
typedef struct slim_QSProviderTxS          slim_QSProviderTxT;
typedef struct slim_QSQmiClientS           slim_QSQmiClientT;
typedef struct slim_QSQmiClientTxS         slim_QSQmiClientTxT;
typedef struct slim_QSQmiClientCb_VtS      slim_QSQmiClientCb_VtT;
typedef struct slim_QSQmiSvcS              slim_QSQmiSvcT;
typedef struct slim_QSQmiSvcCltS           slim_QSQmiSvcCltT;
typedef struct slim_QSQmiSvcTxS            slim_QSQmiSvcTxT;
typedef struct slim_QSQmiSvcCb_VtS         slim_QSQmiSvcCb_VtT;
typedef struct slim_QSQmiSvcCbFactory_VtS  slim_QSQmiSvcCbFactory_VtT;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
  @brief Initializes allocators for QMI-SLIM SLIM provider stack

  QMI SLIM facilities require explicit initialization before use.

  Initialization can be called multiple times, but each call must
  be followed by corresponding call to @a slim_QmiSlimDeinit()

  @sa slim_QSProviderAllocator_Deinit
  @ingroup slim_QSAlloc
 */
int slim_QSProviderAllocator_Init
(
  void
);

/*!
  @brief Cleans up allocators for QMI-SLIM SLIM provider stack

  This call requests release of QMI SLIM SLIM provider stack resources. In case
  if there are any open client handles, they become invalid and an
  attempt to use them lead to unpredictable result.

  @sa slim_QSProviderAllocator_Init
  @ingroup slim_QSAlloc
 */
int slim_QSProviderAllocator_Deinit
(
  void
);

/*!
  @brief Initializes allocators for QMI-SLIM QMI service stack

  QMI SLIM facilities require explicit initialization before use.

  Initialization can be called multiple times, but each call must
  be followed by corresponding call to @a slim_QmiSlimDeinit()

  @sa slim_QSServiceAllocator_Deinit
  @ingroup slim_QSAlloc
 */
void slim_QSServiceAllocator_Init
(
  void
);

/*!
  @brief Cleans up allocators for QMI-SLIM QMI service stack

  This call requests release of QMI SLIM QMI service stack resources. In case
  if there are any open client handles, they become invalid and an
  attempt to use them lead to unpredictable result.

  @sa slim_QSServiceAllocator_Init
  @ingroup slim_QSAlloc
 */
void slim_QSServiceAllocator_Deinit
(
  void
);

/**
  @brief Allocates QMI-SLIM provider object
  @return Allocated object pointer or NULL

  @ingroup slim_QSAlloc
 */
slim_QSProviderT *slim_QSProvider_Alloc(void);
/**
  @brief Releases object
  @param[in] pz_Object Object pointer to release

  @ingroup slim_QSAlloc
 */
void slim_QSProvider_Free(slim_QSProviderT *pz_Object);
/**
  @brief Allocates QMI-SLIM provider transaction object
  @return Allocated object pointer or NULL

  @ingroup slim_QSAlloc
 */
slim_QSProviderTxT *slim_QSProviderTx_Alloc(void);
/**
  @brief Releases object
  @param[in] pz_Object Object pointer to release

  @ingroup slim_QSAlloc
 */
void slim_QSProviderTx_Free(slim_QSProviderTxT *pz_Object);

/* QMI-SLIM QMI Service */
/**
  @brief Allocates QMI-SLIM QMI service connection object
  @return Allocated object pointer or NULL

  @ingroup slim_QSAlloc
 */
slim_QSQmiSvcCltT *slim_QSQmiSvcClt_Alloc(void);
/**
  @brief Releases QMI-SLIM QMI service connection object
  @param[in] pz_Object Object pointer to release

  @ingroup slim_QSAlloc
 */
void slim_QSQmiSvcClt_Free(slim_QSQmiSvcCltT *pz_Object);

/**
  @brief Provides time suitable for timers usage

  The time value must be monotonic and tickable when system is in sleep/suspend
  mode if such mode is applicable.

  @return Time value in milliseconds. The relative point is unspecified.

  @ingroup slim_QS
 */
uint64 slim_QSTimeGetMs
(
  void
);

/**
  @brief Computes structure offset from a member pointer

  Function performs a NULL check and returns pointer to structure computed
  from pointer to member.

  @param[in] p_Member Pointer to member. Can be NULL.
  @param[in] q_Offset Member in bytes

  @return Computed pointer or NULL if @a p_Member is NULL.
  @ingroup slim_QS
 */
SLIM_INLINE void *slim_QSMember2Container
(
  void *p_Member,
  size_t q_Offset
)
{
  if (NULL == p_Member)
  {
      return NULL;
  }
  return (char*)p_Member - q_Offset;
}

/**
  @brief Converts object pointer to member pointer.

  @param[in] p_Container Pointer to object or NULL
  @param[in] q_Offset    Member offset in bytes

  @return Computed pointer or NULL if @a p_Container is NULL.
  @ingroup slim_QS
 */
SLIM_INLINE void *slim_QSContainer2Member
(
  void *p_Container,
  size_t q_Offset
)
{
  if (NULL == p_Container)
  {
      return NULL;
  }
  return (char*)p_Container + q_Offset;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_COMMON_API_H */

