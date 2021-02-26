#ifndef __VEHICLE_NETWORK_CONFIGURATION_H_INCLUDED__
#define __VEHICLE_NETWORK_CONFIGURATION_H_INCLUDED__
/**
@file
@brief Vehicle Network configuration interface file.

This file declares classes describing configuration of vehicle network.
*/

/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <inttypes.h>

namespace slim
{
/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/
class SensorConfig;

/**
@brief Vehicle network configuration description class.

VehicleNetworkConfiguration contains data and functionality to read
and parse configuration of the vehicle network so it can be used by
processors handling data coming from the CAN bus.
*/
class VehicleNetworkConfiguration
{
public:
  /**
  @brief Supported configurations of CAN frames processing

  This enumerations lists constants to denote possible configurations
  that are supported by corresponding classes for configuration storage
  (inherited from @sa SensorConfig) and message processor classes
  (inherited from @sa VehicleNetworkMessageProcessor).

  XXX_DEFAULT configurations use messages processors to generate data
  internally, populating resulting data fields with constants.

  XXX_CONF1 configurations use messages processors that receive CAN bus
  frames and decode data as configuration string describes. Refer to
  @sa VehicleNetworkCONF1AccelMP, @sa VehicleNetworkCONF1GyroMP,
  @sa VehicleNetworkCONF1OdometryMP classes,   @sa VehicleNetworkCONF1SpeedMP,
  @sa VehicleNetworkCONF1DwsMP and @sa VehicleNetworkCONF1GearMP,
  classes' VehicleNetworkCONF1OdometryMP methods for details of CAN frames parsing.

  */
  enum
  {
    eACCEL_CONFIG_DEFAULT = 0,    /**< Accelerometer configuration #0 */
    eGYRO_CONFIG_DEFAULT = 0,     /**< Gyro configuration #0 */
    eODOMETRY_CONFIG_DEFAULT = 0, /**< Odometry configuration #0 */
    eSPEED_CONFIG_DEFAULT = 0,    /**< Speed configuration #0 */
    eDWS_CONFIG_DEFAULT = 0,     /**< Differential Wheel Speed configuration #0 */
    eGEAR_CONFIG_DEFAULT = 0, /**< Gear configuration #0 */

    eACCEL_CONFIG_CONF1 = 1,      /**< Accelerometer configuration #1 */
    eGYRO_CONFIG_CONF1 = 1,       /**< Gyro configuration #1 */
    eODOMETRY_CONFIG_CONF1 = 1,   /**< Odometry configuration #1 */
    eSPEED_CONFIG_CONF1 = 1,      /**< Speed configuration #1 */
    eDWS_CONFIG_CONF1 = 1,       /**< Differential Wheel Speed configuration #1 */
    eGEAR_CONFIG_CONF1 = 1,   /**< Gear configuration #1 */

    eCFG_VERSION_MAX = 1,         /**< Max allowed configuration version */

  };

  /**
  @brief Constructs object

  Function creates the object and initialises internal fields.
  */
  VehicleNetworkConfiguration();

  /**
  @brief Destroys the object.
  */
  virtual ~VehicleNetworkConfiguration();

  /**
  @brief Load configuration

  Function reads the configuration defined in /vendor/etc/sap.conf.
  The configuration has to be defined in the following format:

  VN_<SNS>_CFG=<V>:<cfg>

  , where
   <SNS> is sensor name: 'ACCEL', 'GYRO' ,'ODOMETRY', 'SPEED' , 'DWS' or 'GEAR',
   <V> is configuration version. Currently can be 0 or 1. 0 is for timer based
       constant data and 1 is for CONF1 configuration. Refer to corresponding
       configuration classes for details.
   <cfg> is a string of comma separated values that describe configuration
         files. The format of the string is specific for each configuration
         class.

   Function tried to read configuration and initialize internal configuration
   classes. After that call functions GetAccelConfig(),  GetGyroConfig(),
   GetOdometryConfig(), GetSpeedConfig(),  GetDwsConfig() and GetGearConfig()
   return non-null pointers for successfully read
   configuration .
  */
  void Load();

  /**
  @brief Get configuration of accelerometer on CAN bus

  Function returns pointer to the object describing configuration of the
  accelerometer connected to the CAN bus.

  @return pointer to the object of a type inherited from SensorConfig
  */
  SensorConfig* GetAccelConfig() const;

  /**
  @brief Get configuration of the gyroscope on CAN bus

  Function returns pointer to the object describing configuration of the
  gyroscope connected to the CAN bus.

  @return pointer to the object of a type inherited from SensorConfig
  */
  SensorConfig* GetGyroConfig() const;

  /**
  @brief Get configuration of odometry sensor(s) on CAN bus

  Function returns pointer to the object describing configuration of the
  odometry sensor(s) connected to the CAN bus.

  @return pointer to the object of a type inherited from SensorConfig
  */
  SensorConfig* GetOdometryConfig() const;

  /**
  @brief Get configuration of speed sensor(s) on CAN bus

  Function returns pointer to the object describing configuration of the
  speed sensor(s) connected to the CAN bus.

  @return pointer to the object of a type inherited from SensorConfig
  */
  SensorConfig* GetSpeedConfig() const;

  /**
  @brief Get configuration of DWS sensor(s) on CAN bus

  Function returns pointer to the object describing configuration of the
  DWS sensor(s) connected to the CAN bus.

  @return pointer to the object of a type inherited from SensorConfig
  */
  SensorConfig* GetDwsConfig() const;

  /**
  @brief Get configuration of Gear sensor(s) on CAN bus

  Function returns pointer to the object describing configuration of the
  Gear sensor(s) connected to the CAN bus.

  @return pointer to the object of a type inherited from SensorConfig
  */
  SensorConfig* GetGearConfig() const;

  /**
  @brief Get a ratio of AP and CAN bus microcontroller clock

  Function returns a value specified in configuration file or 0
  if value is not set or cannot be parsed.

  @return ratio of AP and CAN bus microcontroller clock
  */
  double GetCanApClockRatio() const;

  /**
  @brief Get textual representation of this configuration

  This method populates the string passed as a parameter with a human readable
  textual representation of this configuration object.

  @param[in/out] pStr pointer to a string buffer
  @param[in] sLen length of the buffer
  */
  virtual void ToString(char* pStr, size_t sLen) const;

private:
  /**
  @brief Load configuration of type eACCEL_CONFIG_DEFAULT (0)

  This method parses configuration string and extracts configuration
  assuming the configuration version is eACCEL_CONFIG_DEFAULT (0).

  @return pointer to the object of AccelConfig class
  */
  SensorConfig* LoadAccelerometerConfig(const char* pStr);

  /**
  @brief Load configuration of type eGYRO_CONFIG_DEFAULT (0)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eGYRO_CONFIG_DEFAULT (0).

  @return pointer to the object of GyroConfig class
  */
  SensorConfig* LoadGyroConfig(const char* pStr);

  /**
  @brief Load configuration of type eODOMETRY_CONFIG_DEFAULT (0)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eODOMETRY_CONFIG_DEFAULT (0).

  @return pointer to the object of OdometryConfig class
  */
  SensorConfig* LoadOdometryConfig(const char* pStr);

  /**
  @brief Load configuration of type eSPEED_CONFIG_DEFAULT (0)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eSPEED_CONFIG_DEFAULT (0).

  @return pointer to the object of SpeedConfig class
  */
  SensorConfig* LoadSpeedConfig(const char* pStr);

  /**
  @brief Load configuration of type eDWS_CONFIG_DEFAULT (0)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eDWS_CONFIG_DEFAULT (0).

  @return pointer to the object of DwsConfig class
  */
  SensorConfig* LoadDwsConfig(const char* pStr);

  /**
  @brief Load configuration of type eGEAR_CONFIG_DEFAULT (0)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eGEAR_CONFIG_DEFAULT (0).

  @return pointer to the object of GearConfig class
  */
  SensorConfig* LoadGearConfig(const char* pStr);

  /**
  @brief Load configuration of type eACCEL_CONFIG_CONF1 (1)

  This method parses configuration string and extracts configuration
  assuming the configuration version is eACCEL_CONFIG_CONF1 (1).

  @return pointer to the object of AccelCONF1Config class
  */
  SensorConfig* LoadAccelerometerCONF1Config(const char* pStr);

  /**
  @brief Load configuration of type eGYRO_CONFIG_CONF1 (1)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eGYRO_CONFIG_CONF1 (1).

  @return pointer to the object of GyroCONF1Config class
  */
  SensorConfig* LoadGyroCONF1Config(const char* pStr);

  /**
  @brief Load configuration of type eODOMETRY_CONFIG_CONF1 (1)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eODOMETRY_CONFIG_CONF1 (1).

  @return pointer to the object of OdometryCONF1Config class
  */
  SensorConfig* LoadOdometryCONF1Config(const char* pStr);

  /**
  @brief Load configuration of type eSPEED_CONFIG_CONF1 (1)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eSPEED_CONFIG_CONF1 (1).

  @return pointer to the object of SpeedCONF1Config class
  */
  SensorConfig* LoadSpeedCONF1Config(const char* pStr);

  /**
  @brief Load configuration of type eDWS_CONFIG_CONF1 (1)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eDWS_CONFIG_CONF1 (1).

  @return pointer to the object of DwsCONF1Config class
  */
  SensorConfig* LoadDwsCONF1Config(const char* pStr);

  /**
  @brief Load configuration of type eGEAR_CONFIG_CONF1 (1)

  This methods parses configuration string and extracts configuration
  assuming the configuration version is eGEAR_CONFIG_CONF1 (1).

  @return pointer to the object of GearCONF1Config class
  */
  SensorConfig* LoadGearCONF1Config(const char* pStr);

  /**
  @brief Get version configuration

  This methods parses configuration string and extracts a number
  showing version of configuration string. It is a number in the beginning of
  a configuration string, before first colon ':' character.

  @return a number determining version of configuration
  */
  int32_t GetCfgVersion(const char* pCfgStr) const;

  SensorConfig*         m_pzAccelConfig;
  SensorConfig*         m_pzGyroConfig;
  SensorConfig*         m_pzOdometryConfig;
  SensorConfig*         m_pzSpeedConfig;
  SensorConfig*         m_pzDwsConfig;
  SensorConfig*         m_pzGearConfig;

  double                m_dCanApClockRatio;

  static const char* const VN_CFG_FILE_NAME;
};

/* -------------------------------------------------------------------------*/
/**
@brief Base class for sensor configurations.

SensorConfig stores basic configuration items and provides functionality to
parse configuration string. All classes describing real configuration should
be inherited from this class so methods of @sa VehicleNetworkConfiguration class
can operate with them.
*/
class SensorConfig
{
public:
  /**
  @brief Constructs object

  Function creates the object and initialises internal fields.

  @param[in] qCfgVersion configuration version
  */
  SensorConfig(uint32_t qCfgVersion);

  /**
  @brief Destroys the object.
  */
  virtual ~SensorConfig();

  /**
  @brief Load configuration

  This method stores configuration string into internal field. The method must
  be overwritten in inherited class to implement functionality to parse
  configuration string to initialise necessary values.

  @param[in] pStr configuration string
  */
  virtual void Load(const char* pStr);

  /**
  @brief Get configuration version

  This method returns configuration version that the object was constructed for.

  @return configuration version
  */
  uint32_t GetVersion() const;

  /**
  @brief Verify configuration


  @return true if configuration fields have acceptable values
  */
  virtual bool CfgValid() const;

  /**
  @brief Get textual representation of this configuration

  This method populates the string passed as a parameter with a human readable
  textual representation of this configuration object. Inherited class must
  override this method to add own fields to the output.

  @param[in/out] pStr pointer to a string buffer
  @param[in] sLen length of the buffer
  */
  virtual void ToString(char* pStr, size_t sLen) const;

protected:
  /**
  @brief Stored configuration string
  */
  char*                 m_pCfgStr;

  /**
  @brief Configuration version
  */
  uint32_t              m_qCfgVersion;

  /**
  @brief Get float field value and advance read pointer

  This method reads float value from the buffer pointed by @sa pFBegin and
  advances buffer pointer to a character it stopped reading at. Refer to
  strtod() function documentation for full details.
  If text cannot be converted to a float value or pFBegin is null or points to
  the end of string (null character) function puts default value to the output.
  If @sa pfX is null functions returns doing nothing.

  @param[in/out] pfX pointer to a float variable to store result to
  @param[in/out] pFBegin pointer to a next field beginning in the string buffer
  @param[in] fDefault default value to be returned in case conversion fails
  */
  static void GetNextFloat(float* pfX, char** pFBegin, float fDefault);

  /**
  @brief Get integer field value and advance read pointer

  This method reads integer value from the buffer pointed by @sa pFBegin and
  advances buffer pointer to a character it stopped reading at. Refer to
  strtol() function documentation for full details.
  If text cannot be converted to an integer value or pFBegin is null or
  points to the end of string (null character) function puts default value
  to the output. If @sa piX is null functions returns doing nothing.

  @param[in/out] piX pointer to an integer variable to store result to
  @param[in/out] pFBegin pointer to a next field beginning in the string buffer
  @param[in] iDefault default value to be returned in case conversion fails
  */
  static void GetNextInt(int* piX, char** pFBegin, int iDefault);
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing default configuration of accelerometer sensor.

       Refer to @sa SensorConfig for description of overloaded methods.
       This class is defined for reference and debugging purposes and shall
       not be used in real application.
       This class contains one specific field @sa m_fPeriod that is not used
       and is added for debugging purposes.
*/
class AccelConfig
    : public SensorConfig
{
public:
  AccelConfig();
  virtual void Load(const char* pStr);
  virtual bool CfgValid() const;
  virtual void ToString(char* pStr, size_t sLen) const;

  float                 m_fPeriod;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing default configuration of gyroscope sensor.

       Refer to @sa SensorConfig for description of overloaded methods.
       This class is defined for reference and debugging purposes and shall
       not be used in real application.
       This class contains one specific field @sa m_fPeriod that is not used
       and is added for debugging purposes.
*/
class GyroConfig
    : public SensorConfig
{
public:
  GyroConfig();
  virtual void Load(const char* pStr);
  virtual bool CfgValid() const;
  virtual void ToString(char* pStr, size_t sLen) const;

  float                 m_fPeriod;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing default configuration of odometry sensor.

       Refer to @sa SensorConfig for description of overloaded methods.
       This class is defined for reference and debugging purposes and shall
       not be used in real application.
       This class contains one specific field @sa m_fAverageSpeed and
       @sa m_fPeriod that are not used and are added for debugging purposes.
*/
class OdometryConfig
    : public SensorConfig
{
public:
  OdometryConfig();
  virtual void Load(const char* pStr);
  virtual bool CfgValid() const;
  virtual void ToString(char* pStr, size_t sLen) const;

  float                 m_fAverageSpeed;
  float                 m_fPeriod;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing default configuration of Speed sensor.

       Refer to @sa SensorConfig for description of overloaded methods.
       This class is defined for reference and debugging purposes and shall
       not be used in real application.
       This class contains one specific field @sa m_fPeriod that are not used
       and are added for debugging purposes.
*/
class SpeedConfig
    : public SensorConfig
{
public:
  SpeedConfig();
  virtual void Load(const char* pStr);
  virtual bool CfgValid() const;
  virtual void ToString(char* pStr, size_t sLen) const;

  float                 m_fPeriod;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing default configuration of DWS sensor.

       Refer to @sa SensorConfig for description of overloaded methods.
       This class is defined for reference and debugging purposes and shall
       not be used in real application.
       This class contains one specific field @sa m_fPeriod that are not used
       and are added for debugging purposes.
*/
class DwsConfig
    : public SensorConfig
{
public:
  DwsConfig();
  virtual void Load(const char* pStr);
  virtual bool CfgValid() const;
  virtual void ToString(char* pStr, size_t sLen) const;

  float                 m_fPeriod;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing default configuration of Gear sensor.

       Refer to @sa SensorConfig for description of overloaded methods.
       This class is defined for reference and debugging purposes and shall
       not be used in real application.
       This class contains one specific field @sa m_fPeriod that are not used
       and are added for debugging purposes.
*/
class GearConfig
    : public SensorConfig
{
public:
  GearConfig();
  virtual void Load(const char* pStr);
  virtual bool CfgValid() const;
  virtual void ToString(char* pStr, size_t sLen) const;

  float                 m_fPeriod;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing reference configuration of accelerometer sensor
       connected to CAN bus.

       Refer to @sa SensorConfig for description of overloaded methods.

       This class is defined for reference and can be used to debug e2e
       operation with CAN bus simulator.

       This configuration assumes that CAN frame with the ID stored to
       @sa m_iFrameId carries three values for X, Y and Z axes. Each value
       is a product of scaling coefficient and signed integer value of given
       length read from given offset. Scaling factor, field offset and field
       length are set for each axis separately.
*/
class AccelCONF1Config
    : public SensorConfig
{
public:
  AccelCONF1Config();
  void Load(const char* pStr);
  bool CfgValid() const;
  void ToString(char* pStr, size_t sLen) const;

  int                    m_iFrameId;
  int                    m_iOffsetX;
  int                    m_iLenX;
  float                  m_fScalingX;
  int                    m_iOffsetY;
  int                    m_iLenY;
  float                  m_fScalingY;
  int                    m_iOffsetZ;
  int                    m_iLenZ;
  float                  m_fScalingZ;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing reference configuration of gyroscope sensor
       connected to CAN bus.

       Refer to @sa SensorConfig for description of overloaded methods.

       This class is defined for reference and can be used to debug e2e
       operation with CAN bus simulator.

       This configuration assumes that CAN frame with the ID stored to
       @sa m_iFrameId carries three values for X, Y and Z axes. Each value
       is a product of scaling coefficient and signed integer value of given
       length read from given offset. Scaling factor, field offset and field
       length are set for each axis separately.
*/
class GyroCONF1Config
    : public SensorConfig
{
public:
  GyroCONF1Config();
  void Load(const char* pStr);
  bool CfgValid() const;
  void ToString(char* pStr, size_t sLen) const;

  int                    m_iFrameId;
  int                    m_iOffsetX;
  int                    m_iLenX;
  float                  m_fScalingX;
  int                    m_iOffsetY;
  int                    m_iLenY;
  float                  m_fScalingY;
  int                    m_iOffsetZ;
  int                    m_iLenZ;
  float                  m_fScalingZ;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing reference configuration of odometry sensor
       connected to CAN bus.

       Refer to @sa SensorConfig for description of overloaded methods.

       This class is defined for reference and can be used to debug e2e
       operation with CAN bus simulator.

       This configuration assumes that CAN frame with the ID stored to
       @sa m_iFrameId carries values for reverse movement indication,
       specification of source of travelled distance information and travelled
       distance value itself.

       Indication of reversed movement is a 1 bit boolean value. Its location
       in the frame is specified by @sa m_iReverseBitOffset value.

       Source of travelled distance information can specify either left wheel,
       right wheel or average between them.

       Travelled distance value is a unsigned integer value.
*/
class OdometryCONF1Config
    : public SensorConfig
{
public:
  OdometryCONF1Config();
  void Load(const char* pStr);
  bool CfgValid() const;
  void ToString(char* pStr, size_t sLen) const;

  enum
  {
    eODO_CFG1_SRC_LEFT   = 1,
    eODO_CFG1_SRC_RIGHT  = 2,
    eODO_CFG1_SRC_LR_AVR = 4
  };

  int                    m_iFrameId;
  int                    m_iReverseBitOffset;
  int                    m_iSourceOffset;
  int                    m_iSourceLen;
  int                    m_iDistanceOffset;
  int                    m_iDistanceLen;
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing reference configuration of Speed sensor
       connected to CAN bus.

       Refer to @sa SensorConfig for description of overloaded methods.

       This class is defined for reference and can be used to debug e2e
       operation with CAN bus simulator.

       This configuration assumes that CAN frame with the ID stored to
       @sa m_iFrameId carries speed and unit information as follows.
       VN_SPEED_CFG=1:CAN_SPEED_PID, unitOffset, unitLength, numberofSpeedConfig, speedUnit,
       unitValue, scale, offset, speedOffsetLowBits, speedLengthLowBits, speedOffsetUpBits, speedLengthUpBits

      Where
      1.  unitOffset - indicates the bit position where speed unit information is available in bit stream.
      2.  unitLength - indicates number of bits to read from offset positon.
      3.  numberofSpeedConfig - indicates the number of speed configurations this message wants to convey.
          Based on this parameter the following parameters will be repeated that many times.
      4. speedUnit - can be mph - 1 , kmph -2 , m/s -3. In some cases we know the speed unit beforehand
          and in some cases gets known in message itself. When it comes in message field 1-2-5 will be used to
          determine it.
      5. unitValue - value corresponding to speedUnit.
      6. scale - scale factor to convert bytes to the physical speed unit
      7. offset - offset to convert bytes to the physical speed unit
      8. speedOffsetLowBits - indicates the bit position where lower bits of speed information is available in bit stream.
      9. speedLengthLowBits  - indicates number of bits to read from offset positon.
      10. speedOffsetUpBits - indicates the bit position where upper bits of speed information is available in bit stream.
      11. speedLengthUpBits  - indicates number of bits to read from offset positon.

     speedUnit = In case unitLength is greater than zero, get 'unitLength' bits from 'unitOffset' position
     in CAN speed PID message. Match this against 'unitValue' to get speedUnit.

     speedLowBits = Get 'speedLengthLowBits' bits from 'speedOffsetLowBits' position in CAN speed PID
     message.
     speedUpBits = Get 'speedLengthUpBits' bits from 'speedOffsetUpBits' position in CAN speed PID
     message.

     speedBytes = (speedUpBits << speedLengthLowBits) | speedLowBits
     speedPhysicalUnit =  (speedBytes - offset) * scale


*/
class SpeedCONF1Config
    : public SensorConfig
{
public:
  SpeedCONF1Config();
  void Load(const char* pStr);
  bool CfgValid() const;
  void ToString(char* pStr, size_t sLen) const;

  enum
  {
    eSPEED_UNIT_MPH   = 1,  /**< Miles per hour */
    eSPEED_UNIT_KMPH  = 2,  /**< kilometers per hour */
    eSPEED_UNIT_MPS = 3 /**< Meters per second */
  };
  struct speedParams{
  uint8_t speedUnit;
  uint8_t unitValue;
  float scale;
  float offset;
  uint8_t speedOffsetLowBits;
  uint8_t speedLengthLowBits;
  uint8_t speedOffsetUpBits;
  uint8_t speedLengthUpBits;
  };

  int                    m_iFrameId;
  uint8_t                m_unitOffset;
  uint8_t                m_unitLength;
  uint8_t                m_numberofSpeedConfig;
  speedParams    m_speedConfig[3];
};

/* -------------------------------------------------------------------------*/
/**
@brief Class describing reference configuration of DWS sensor
       connected to CAN bus.

       Refer to @sa SensorConfig for description of overloaded methods.

       This class is defined for reference and can be used to debug e2e
       operation with CAN bus simulator.

       This configuration assumes that CAN frame with the ID stored to
       @sa m_iFrameId carries differential wheel speeds and unit information as follows.
       VN_DWS_CFG=1:CAN_DWS_PID, unitOffset, unitLength, numberofSpeedConfig ,speedUnit,
       unitValue, scale, offset, speedOffsetLowBitsFL, speedLengthLowBitsFL, speedOffsetUpBitsFL,
       speedLengthUpBitsFL, speedOffsetLowBitsFR, speedLengthLowBitsFR, speedOffsetUpBitsFR,
       speedLengthUpBitsFR, speedOffsetLowBitsRL, speedLengthLowBitsRL, speedOffsetUpBitsRL,
       speedLengthUpBitsRL, speedOffsetLowBitsRR, speedLengthLowBitsRR, speedOffsetUpBitsRR,
       speedLengthUpBitsRR

      Where
      1.  unitOffset - indicates the bit position where speed unit information is available in bit stream.
      2.  unitLength - indicates number of bits to read from offset positon.
      3.  numberofSpeedConfig - indicates the number of speed configurations this message wants to convey.
          Based on this parameter the following parameters will be repeated that many times.
      4. speedUnit - can be mph - 1 , kmph -2 , m/s -3. In some cases we know the speed unit beforehand
          and in some cases gets known in message itself. When it comes in message field 1-2-5 will be used to
          determine it.
      5. unitValue - value corresponding to speedUnit.
      6. scale - scale factor to convert bytes to the physical speed unit
      7. offset - offset to convert bytes to the physical speed unit
      8. speedOffsetLowBitsFL - indicates the bit position where lower bits  of DWS information for front left
         wheel is available in bit stream.
      9. speedLengthLowBitsFL  - indicates number of bits to read from offset positon.
      10. speedOffsetUpBitsFL - indicates the bit position where upper bits of DWS information for front left
          wheel is available in bit stream.
      11. speedLengthUpBitsFL  - indicates number of bits to read from offset positon.
      12. speedOffsetLowBitsFR - indicates the bit position where lower bits of DWS information for front right
          wheel is available in bit stream.
      13. speedLengthLowBitsFR  - indicates number of bits to read from offset positon.
      14. speedOffsetUpBitsFR - indicates the bit position where upper bits of DWS information for front right
          wheel is available in bit stream.
      15. speedLengthUpBitsFR  - indicates number of bits to read from offset positon.
      16. speedOffsetLowBitsRL - indicates the bit position where lower bits of DWS information for rear left
         wheel is available in bit stream.
      17. speedLengthLowBitsRL  - indicates number of bits to read from offset positon.
      18. speedOffsetUpBitsRL - indicates the bit position where upper bits of DWS information for rear left
         wheel is available in bit stream.
      19. speedLengthUpBitsRL  - indicates number of bits to read from offset positon.
      20. speedOffsetLowBitsRR - indicates the bit position where lower bits of DWS information for rear right
         wheel is available in bit stream.
      21. speedLengthLowBitsRR  - indicates number of bits to read from offset positon.
      22. speedOffsetUpBitsRR - indicates the bit position where upper bits of DWS information for rear right
         wheel is available in bit stream.
      23. speedLengthUpBitsRR  - indicates number of bits to read from offset positon.


     speedUnit = In case unitLength is greater than zero, get 'unitLength' bits from 'unitOffset' position in CAN
     DWS PID message. Match this against 'unitValue' to get speed unit.


     speedLowBitsFL = Get 'speedLengthLowBitsFL' bits from 'speedOffsetLowBitsFL' position in CAN DWS PID
     message.

     speedUpBitsFL = Get 'speedLengthUpBitsFL' bits from 'speedOffsetUpBitsFL' position in CAN DWS PID
     message.

     speedBytesFL = (speedUpBitsFL << speedLengthLowBitsFL) | speedLowBitsFL

     dwsFLPhysicalUnit =  (speedBytesFL - offset) * scale


*/
class DwsCONF1Config
    : public SensorConfig
{
public:
  DwsCONF1Config();
  void Load(const char* pStr);
  bool CfgValid() const;
  void ToString(char* pStr, size_t sLen) const;

  enum
  {
    eSPEED_UNIT_MPH   = 1,  /**< Miles per hour */
    eSPEED_UNIT_KMPH  = 2,  /**< kilometers per hour */
    eSPEED_UNIT_MPS = 3 /**< Meters per second */
  };
  struct dwsParams{
  uint8_t speedUnit;
  uint8_t unitValue;
  float scale;
  float offset;
  uint8_t speedOffsetLowBitsFL; uint8_t speedLengthLowBitsFL;
  uint8_t speedOffsetUpBitsFL; uint8_t speedLengthUpBitsFL;
  uint8_t speedOffsetLowBitsFR; uint8_t speedLengthLowBitsFR;
  uint8_t speedOffsetUpBitsFR; uint8_t speedLengthUpBitsFR;
  uint8_t speedOffsetLowBitsRL; uint8_t speedLengthLowBitsRL;
  uint8_t speedOffsetUpBitsRL; uint8_t speedLengthUpBitsRL;
  uint8_t speedOffsetLowBitsRR; uint8_t speedLengthLowBitsRR;
  uint8_t speedOffsetUpBitsRR; uint8_t speedLengthUpBitsRR;
  };

  int                 m_iFrameId;
  uint8_t             m_unitOffset;
  uint8_t             m_unitLength;
  uint8_t             m_numberofSpeedConfig;
  dwsParams    m_dwsConfig[3];
};


/* -------------------------------------------------------------------------*/
/**
@brief Class describing reference configuration of Gear sensor
       connected to CAN bus.

       Refer to @sa SensorConfig for description of overloaded methods.

       This class is defined for reference and can be used to debug e2e
       operation with CAN bus simulator.

       This configuration assumes that CAN frame with the ID stored to
       @sa m_iFrameId carries gear information as follows.
        VN_GEAR_CFG=1:CAN_GEAR_PID, gearOffset, gearLength, parkValue, reverseValue, neutralValue,
        numberDriveGear, driveValue0, driveValue1, driveValue2, driveValue3, driveValue4, driveValue5,
        driveValue6, driveValue7, driveValue8, driveValue9

        Where
        1. gearOffset - indicates the bit position where gear information is available in bit stream.
        2. gearLength - indicates number of bits to read from offset positon.
        3. parkValue - indicates the gear park state.
        4. reverseValue - indicates the gear reverse state.
        5. neutralValue - indicates the gear neutral state.
        6. numberDriveGear - indicates number of driver gear. Maximum of 10 drive gear supported.
        7. driveValue0 -9 - indicates the value for gear drive state.

*/
class GearCONF1Config
    : public SensorConfig
{
public:
  GearCONF1Config();
  void Load(const char* pStr);
  bool CfgValid() const;
  void ToString(char* pStr, size_t sLen) const;


  int              m_iFrameId;
  uint8_t          m_gearOffset;
  uint8_t          m_gearLength;
  int8_t            m_parkValue;
  int8_t            m_reverseValue;
  int8_t            m_neutralValue;
  uint8_t          m_numberDriveGear;
  int8_t           m_driveValue[10];
};

}

#endif
