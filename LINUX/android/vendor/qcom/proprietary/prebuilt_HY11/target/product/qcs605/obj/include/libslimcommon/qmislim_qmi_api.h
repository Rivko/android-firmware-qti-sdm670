#ifndef QMISLIM_QMI_API_H
#define QMISLIM_QMI_API_H

/**
  @file
  @brief QMI encoders/decoders and common types
  @ingroup slim_QSQmiEndDec
*/
/*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/common/inc/qmislim_qmi_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "qmislim_common_api.h"
#include "sensor_location_interface_manager_v01.h"
#include "qmi_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*!
  @brief Type for handling all supported QMI-SLIM indication messages.

  This type is used for reserving space when handling QMI indication message
  decoding.

  @ingroup slim_QS
 */
typedef union slim_QSQmiIndUnion
{
  qmiSlimServiceUpdateIndMsgT_v01       z_ServiceUpdateInd;
  /**< @brief Service status update indication message */
  qmiSlimSensorDataIndMsgT_v01          z_SensorDataInd;
  /**< @brief Sensor service data indication message */
  qmiSlimPedometerDataIndMsgT_v01       z_PedometerDataInd;
  /**< @brief Pedometer service data indication message */
  qmiSlimQmdDataIndMsgT_v01             z_QmdDataInd;
  /**< @brief QMD service data indication message */
  qmiSlimVehicleSensorDataIndMsgT_v01   z_VehicleSensorDataInd;
  /**< @brief Vehicle sensor service data indication message */
  qmiSlimVehicleOdometryDataIndMsgT_v01 z_VehicleOdometryDataInd;
  /**< @brief Vehicle odometry service data indication message */
  qmiSlimVehicleMotionDataIndMsgT_v01   z_VehicleMotionDataInd;
  /**< @brief Vehicle motion service data indication message */
  qmiSlimServiceErrorIndMsgT_v01        z_DataErrorInd;
  /**< @brief Service error indication message */
} slim_QSQmiIndUnionT;

/*!
  @brief Type for handling all supported QMI-SLIM response messages

  This type is used for reserving space when handling QMI response message
  handling.

  @ingroup slim_QS
 */
typedef union slim_QSQmiRespUnion
{
  qmiSlimGenRespMsgT_v01                z_RegisterResp;
  /**< @brief Response to register client request. */
  qmiSlimGenRespMsgT_v01                z_UnregisterResp;
  /**< @brief Response to unregister client request. */
  qmiSlimGetTimeUpdateRespMsgT_v01      z_GetTimeUpdateResp;
  /**< @brief Response to get time update information request. */
  qmiSlimGenRespMsgT_v01                z_EnableSensorDataResp;
  /**< @brief Response to enable sensor service request. */
  qmiSlimGenRespMsgT_v01                z_EnablePedometerDataResp;
  /**< @brief Response to enable pedometer service request. */
  qmiSlimGenRespMsgT_v01                z_EnableQmdDataResp;
  /**< @brief Response to enable QMD service request. */
  qmiSlimGenRespMsgT_v01                z_EnableVehicleDataResp;
  /**< @brief Response to enable vehicle service request. */
  qmiSlimGenRespMsgT_v01                z_DisableServiceResp;
  /**< @brief Response to disable service request. */
} slim_QSQmiRespUnionT;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Translates SLIM error code into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeError
(
  slimErrorEnumT          e_Error,
  /**< [in] SLIM data type */
  qmiSlimErrorEnumT_v01  *pe_ErrorOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM service number into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeService
(
  slimServiceEnumT         e_Service,
  /**< [in] SLIM data type */
  qmiSlimServiceEnumT_v01 *pe_QmiServiceOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM service provider mask into QMI-SLIM type

  When mapping SLIM service provider mask, the
  @ref eSLIM_SERVICE_PROVIDER_DEFAULT value is mapped to non-present value, i.e.
  this flag indicates the QMI-SLIM service provider mask is not used.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeServiceProviderFlags
(
  slimServiceProviderMaskT         q_Flags,
  /**< [in] SLIM data type */
  uint8_t                         *pu_FlagsOutValid,
  /**< [out] QMI-SLIM data type flag */
  qmiSlimServiceProviderMaskT_v01 *pq_FlagsOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM service provider type into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeServiceProvider
(
  slimServiceProviderEnumT         e_ServiceProvider,
  /**< [in] SLIM data type */
  qmiSlimServiceProviderEnumT_v01 *pe_ServiceProviderOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM time source into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeTimeSource
(
  slimTimeSourceEnumT         e_TimeSource,
  /**< [in] SLIM data type */
  qmiSlimTimeSourceEnumT_v01 *pe_TimeSourceOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM flags into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeFlags
(
  uint32_t                    q_Flags,
  /**< [in] SLIM data type */
  qmiSlimFlagsMaskT_v01      *pt_FlagsOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM pedometer report type into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodePedometerReportType
(
  slimInjectPedometerReportEnumT         e_ReportType,
  /**< [in] SLIM data type */
  qmiSlimInjectPedometerReportEnumT_v01 *pe_ReportTypeOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM QMD motion state into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeQmdMotionState
(
  slimQmdMotionStateEnumT        e_State,
  /**< [in] SLIM data type */
  qmiSlimQmdMotionStateEnumT_v01*pe_StateOut
  /**< [out] QMI-SLIM data type */
);
/**
  @brief Translates SLIM vehicle odometry mask into QMI-SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeVehicleOdometryMask
(
  qmiSlimVehicleOdometryMaskT_v01 t_Mask,
  /**< [in] SLIM data type */
  uint32_t                       *pq_MaskOut
  /**< [out] QMI-SLIM data type */
);

/**
  @brief Translates SLIM indication message into QMI-SLIM message.

  This conversion is used for data indication messages only (messages without
  errors).

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeSlimInd
(
  uint32                         q_SlimMsgId,
  /**< [in] SLIM message type */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service */
  const slim_QSSlimMessageDataT *pz_SlimMsgIn,
  /**< [in] SLIM message payload */
  slim_QSQmiIndUnionT           *pz_QmiMsgOut,
  /**< [out] QMI-SLIM message payload */
  uint32                        *pq_QmiMsgIdOut,
  /**< [out] QMI-SLIM message identifier */
  uint32                        *pq_QmiMsgSizeOut
  /**< [out] QMI-SLIM message payload size in bytes */
);
/**
  @brief Generates common QMI-SLIM data header from SLIM input.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeDataHeader
(
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service */
  slimServiceProviderEnumT       e_SlimServiceProvider,
  /**< [in] SLIM service provider */
  slimTimeSourceEnumT            e_SlimTimeSource,
  /**< [in] SLIM time source */
  uint64_t                       t_SlimTimeMs,
  /**< [in] SLIM time */
  uint32_t                       q_SlimFlags,
  /**< [in] SLIM flags */
  qmiSlimServiceDataHeaderT_v01 *pz_DataOut
  /**< [out] QMI-SLIM message header */
);
/**
  @brief Encodes SLIM sensor data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeSensorData
(
  const slimSensorDataStructT   *pz_DataIn,
  /**< [in] SLIM message data */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service type */
  qmiSlimSensorDataIndMsgT_v01  *pz_DataOut
  /**< [out] QMI-SLIM message data */
);
/**
  @brief Updates time source field if the client is not on the same processor.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.
 */
boolean slim_QSFixupEncodedTimeSource
(
  slimMessageIdEnumT             e_SlimMsgId,
  /**< [in] SLIM message type */
  slim_QSQmiIndUnionT           *pz_QmiMsgOut
  /**< [out] QMI-SLIM message payload */
);

/**
  @brief Encodes SLIM sensor data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodePedometerData
(
  const slimPedometerDataStructT*pz_DataIn,
  /**< [in] SLIM message data */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service type */
  qmiSlimPedometerDataIndMsgT_v01*pz_DataOut
  /**< [out] QMI-SLIM message data */
);
/**
  @brief Encodes SLIM QMD data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeQmdData
(
  const slimQmdDataStructT      *pz_DataIn,
  /**< [in] SLIM message data */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service type */
  qmiSlimQmdDataIndMsgT_v01     *pz_DataOut
  /**< [out] QMI-SLIM message data */
);
/**
  @brief Encodes SLIM vehicle odometry data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeVehicleOdometryData
(
  const slimVehicleOdometryDataStructT *pz_DataIn,
  /**< [in] SLIM message data */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service type */
  qmiSlimVehicleOdometryDataIndMsgT_v01 *pz_DataOut
  /**< [out] QMI-SLIM message data */
);
/**
  @brief Encodes SLIM vehicle sensor data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeVehicleSensorData
(
  const slimVehicleSensorDataStructT *pz_DataIn,
  /**< [in] SLIM message data */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service type */
  qmiSlimVehicleSensorDataIndMsgT_v01 *pz_DataOut
  /**< [out] QMI-SLIM message data */
);
/**
  @brief Encodes SLIM vehicle motion data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeVehicleMotionData
(
  const slimVehicleMotionDataStructT *pz_DataIn,
  /**< [in] SLIM message data */
  slimServiceEnumT               e_SlimService,
  /**< [in] SLIM service type */
  qmiSlimVehicleMotionDataIndMsgT_v01 *pz_DataOut
  /**< [out] QMI-SLIM message data */
);
/**
  @brief Translates QMI-SLIM error code into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeError
(
  qmiSlimErrorEnumT_v01   e_Error,
  /**< [in] QMI-SLIM data type */
  slimErrorEnumT         *pe_ErrorOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM service number into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeService
(
  qmiSlimServiceEnumT_v01  e_Service,
  /**< [in] QMI-SLIM data type */
  slimServiceEnumT        *pe_ServiceOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM service provider mask into SLIM type

  When QMI-SLIM field is not present, the result is mapped into
  @ref eSLIM_SERVICE_PROVIDER_DEFAULT value.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeServiceProviderFlags
(
  uint8_t                          u_FlagsValid,
  /**< [in] Flag if QMI field is present */
  qmiSlimServiceProviderMaskT_v01  q_Flags,
  /**< [in] QMI-SLIM data type */
  slimServiceProviderMaskT        *pq_FlagsOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM service provider type into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeServiceProvider
(
  qmiSlimServiceProviderEnumT_v01  e_ServiceProvider,
  /**< [in] QMI-SLIM data type */
  slimServiceProviderEnumT        *pe_ServiceProviderOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM time source into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeTimeSource
(
  qmiSlimTimeSourceEnumT_v01  e_TimeSource,
  /**< [in] QMI-SLIM data type */
  slimTimeSourceEnumT        *pe_TimeSourceOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM flags into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeFlags
(
  qmiSlimFlagsMaskT_v01       t_Flags,
  /**< [in] QMI-SLIM data type */
  uint32_t                   *pq_FlagsOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM pedometer report type into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodePedometerReportType
(
  qmiSlimInjectPedometerReportEnumT_v01 e_ReportType,
  /**< [in] QMI-SLIM data type */
  slimInjectPedometerReportEnumT       *pe_ReportTypeOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM QMD motion state into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeQmdMotionState
(
  qmiSlimQmdMotionStateEnumT_v01 e_State,
  /**< [in] QMI-SLIM data type */
  slimQmdMotionStateEnumT       *pe_StateOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM vehicle odometry mask into SLIM type

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSEncodeVehicleOdometryMask
(
  uint32_t                         q_Mask,
  /**< [in] QMI-SLIM data type */
  qmiSlimVehicleOdometryMaskT_v01 *pt_MaskOut
  /**< [out] SLIM data type */
);
/**
  @brief Translates QMI-SLIM indication message into SLIM message.

  This conversion is used for data indication messages only (messages without
  errors).

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeSlimInd
(
  uint32                         q_QmiMsgId,
  /**< [in] QMI message type */
  const slim_QSQmiIndUnionT     *pz_QmiMsgIn,
  /**< [in] QMI message data */
  uint32                         q_QmiMsgSize,
  /**< [in] QMI message size in bytes */
  slimServiceEnumT              *pe_SlimServiceOut,
  /**< [out] SLIM service type */
  slimErrorEnumT                *pe_SlimErrorOut,
  /**< [out] SLIM status code */
  slimMessageIdEnumT            *pe_SlimMsgIdOut,
  /**< [out] SLIM indication message type */
  slim_QSSlimMessageDataT       *pz_SlimMsgOut,
  /**< [out] SLIM message payload */
  uint32                        *pq_SlimMsgSizeOut
  /**< [out] SLIM message size in bytes */
);
/**
  @brief Parses common QMI-SLIM data header into SLIM format.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeDataHeader
(
  const qmiSlimServiceDataHeaderT_v01 *pz_DataIn,
  /**< [in] QMI-SLIM data header */
  slimServiceEnumT                    *pe_SlimServiceOut,
  /**< [out] SLIM service */
  slimServiceProviderEnumT            *pe_SlimServiceProviderOut,
  /**< [out] SLIM service provider */
  slimTimeSourceEnumT                 *pe_SlimTimeSourceOut,
  /**< [out] SLIM time source */
  uint64_t                            *pt_SlimTimeMsOut,
  /**< [out] SLIM time */
  uint32_t                            *pq_SlimFlagsOut
  /**< [out] SLIM flags */
);
/**
  @brief Decodes SLIM sensor data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeSensorData
(
  const qmiSlimSensorDataIndMsgT_v01  *pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimSensorDataStructT               *pz_DataOut,
  /**< [out] SLIM message data */
  slimServiceEnumT                    *pe_SlimServiceOut
  /**< [out] SLIM service type */
);
/**
  @brief Encodes SLIM pedometer data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodePedometerData
(
  const qmiSlimPedometerDataIndMsgT_v01*pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimPedometerDataStructT             *pz_DataOut,
  /**< [out] SLIM message data */
  slimServiceEnumT                     *pe_SlimServiceOut
  /**< [out] SLIM service type */
);
/**
  @brief Decodes SLIM QMD data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeQmdData
(
  const qmiSlimQmdDataIndMsgT_v01     *pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimQmdDataStructT                  *pz_DataOut,
  /**< [out] SLIM message data */
  slimServiceEnumT                    *pe_SlimServiceOut
  /**< [out] SLIM service type */
);
/**
  @brief Decodes SLIM vehicle odometry data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeVehicleOdometryData
(
  const qmiSlimVehicleOdometryDataIndMsgT_v01 *pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimVehicleOdometryDataStructT              *pz_DataOut,
  /**< [out] SLIM message data */
  slimServiceEnumT                            *pe_SlimServiceOut
  /**< [out] SLIM service type */
);
/**
  @brief Decodes SLIM vehicle sensor data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeVehicleSensorData
(
  const qmiSlimVehicleSensorDataIndMsgT_v01 *pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimVehicleSensorDataStructT              *pz_DataOut,
  /**< [out] SLIM message data */
  slimServiceEnumT                          *pe_SlimServiceOut
  /**< [out] SLIM service type */
);
/**
  @brief Decodes SLIM vehicle motion data message.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeVehicleMotionData
(
  const qmiSlimVehicleMotionDataIndMsgT_v01 *pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimVehicleMotionDataStructT              *pz_DataOut,
  /**< [out] SLIM message data */
  slimServiceEnumT                          *pe_SlimServiceOut
  /**< [out] SLIM service type */
);
/**
  @brief Decodes SLIM service data message with error.

  @retval TRUE On successful conversion.
  @retval FALSE On conversion error.

  @ingroup slim_QSEncDec
 */
boolean slim_QSDecodeServiceError
(
  const qmiSlimServiceErrorIndMsgT_v01     *pz_DataIn,
  /**< [in] QMI-SLIM message */
  slimServiceEnumT                         *pe_SlimServiceOut,
  /**< [out] SLIM service type */
  slimErrorEnumT                           *pe_SlimErrorOut,
  /**< [out] SLIM error code */
  slimMessageIdEnumT                       *pe_SlimMsgIdOut
  /**< [out] SLIM indication message type */
);

/**
  @brief Method returns allocated QMI service instance for given node type

  The method provides allocated QMI service instance Id for a given node
  type.

  @return QMI service instance id
  @ingroup slim_QSEncDec
 */
qmi_service_instance   slim_QSNodeType2Instance
(
  slim_QSNodeT e_Node
  /**< [in] QMI-SLIM node type */
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_QMI_API_H */
