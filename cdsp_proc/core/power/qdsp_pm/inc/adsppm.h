/*
* Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm.h
@brief: ADSPPM internal data structure and API
        This is the internal data and API for ADSPPM.
        This is not distributed to any clients that uses ADSPPM.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/adsppm.h#1 $
*/

#ifndef ADSPPM_H_
#define ADSPPM_H_

#include "adsppm_types.h"
#include "adsppm_defs.h"
#include "coreUtils.h"
#include "stringl.h"

#include "adsppm_settings.h"

#define ADSPPM_REVISION 001

#define MAX_NUM_OF_ADSPPM_CLIENTS 192
//#define MAX_NUM_OF_ADSPPM_HANDLE_PRE_ALLOCATE MAX_NUM_OF_ADSPPM_CLIENTS
#define MAX_NUM_OF_ADSPPM_CLOCK_ARRAY 8
#define MAX_NUM_OF_ADSPPM_BW_ARRAY    8

#define ADSPPM_REG_PROC_CLOCK_BW_PERCENTAGE 1

#define MAX_ADSPPM_REQ_Q_ELEMENTS 16

//todo: Should be replaced with feature flag
//#define MIPSMGR_AHB_CLOCK_SUPPORT

#define MAX_ADSPPM_DCVS_ROUTES  Adsppm_DCVS_Route_Max

#define MIN_AHB_BW 32767 // Bare minimal AHB BW to keep AHB always ON. Used in AHB BW aggregation

#define CHIP_MIN_VERSION    (0x20000)



/******************************************************************************
 *                     General definitions
 *****************************************************************************/
/**
 * @enum AdsppmApiType - ADSPPM API TYPE
 */
typedef enum
{
     Adsppm_Api_Type_None,              //!< None
     Adsppm_Api_Type_Sync,              //!< SYNC API call
     Adsppm_Api_Type_Async,             //!< ASYNC API call
     Adsppm_Ape_Type_Enum_Max,          //!< enum max
     Adsppm_Api_Type_Force8bits = 0x7F  //!< forcing enum to be 32 bit
} AdsppmApiType;

/**
 * @enum ADSPPMStatusType - Values used for ADSPPM return status
 */
typedef enum
{
    Adsppm_Status_Success,                  //!< no error
    Adsppm_Status_Failed,                   //!< general failure
    Adsppm_Status_NoMemory,                 //!< insufficient Memory
    Adsppm_Status_VersionNotSupport,        //!< version not supported
    Adsppm_Status_BadClass,                 //!< NULL class object
    Adsppm_Status_BadState,                 //!< invalid state
    Adsppm_Status_BadParm,                  //!< invalid parameter
    Adsppm_Status_InvalidFormat,            //!< invalid format
    Adsppm_Status_UnSupported,              //!< API is not supported
    Adsppm_Status_ResourceNotFound,         //!< Resource not found
    Adsppm_Status_BadMemPtr,                //!< bad memory pointer
    Adsppm_Status_BadHandle,                //!< invalid handle
    Adsppm_Status_ResourceInUse,            //!< a resource is in use
    Adsppm_Status_NoBandwidth,              //!< insufficient bandwidth
    Adsppm_Status_NullPointer,              //!< null pointer
    Adsppm_Status_NotInitialized,           //!< not initialized
    Adsppm_Status_ResourceNotRequested,     //!< resource not requested
    Adsppm_Status_CoreResourceNotAvailable, //!< Core Resource not available
    Adsppm_Status_Max,                      //!< Maximum count
    Adsppm_Status_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmStatusType;

typedef AdsppmStatusType Adsppm_Status; //!< ADSPPM status type

/**
 * @enum AdsppmInitStateType - Initialization state of HAL and Manager layers
 */
typedef enum
{
    Adsppm_State_ACMInit = 0x1,
    Adspmm_State_HalIntrInit = 0x2,
    Adsppm_State_HalHwIoInit = 0x4,
    Adsppm_State_HalClkRgmInit = 0x8,
    Adsppm_State_HalBusInit = 0x10,
    Adsppm_State_HalSlpInit = 0x20,
    Adsppm_State_CoreCtxLockInit = 0x80,
    Adsppm_State_CoreRMInit = 0x100,
    Adsppm_State_CoreAMAsyncInit = 0x200,
    Adsppm_State_CoreMIPSInit = 0x400,
    Adsppm_State_CoreBUSInit = 0x800,
    Adsppm_State_CoreAHBMInit = 0x1000,
    Adsppm_State_CorePWRInit = 0x2000,
    Adsppm_State_CoreCLKInit = 0x4000,
    Adsppm_State_CoreSLEEPInit = 0x8000,
    Adsppm_State_CoreTHERMALInit = 0x10000,
    Adsppm_State_CoreMEMPWRInit = 0x20000,
    Adsppm_State_CoreCMInit = 0x40000,
    Adsppm_State_CoreCPMInit = 0x80000,
    Adsppm_State_CoreDCVSInit = 0x100000,
    Adsppm_State_CoreEXTBWInit = 0x200000,
    Adsppm_State_CoreADSPCLKInit = 0x400000,
    Adsppm_State_CoreQCMInit = 0x800000,
    Adsppm_State_CoreBMRegisterEvent = 0x1000000,
    Adsppm_State_CoreSysCacheInit = 0x2000000,
    Adsppm_State_CoreUSLEEPInit = 0x4000000,
    Adsppm_State_Force32bit = 0x7fffffff
} AdsppmInitStateType;

/******************************************************************************
 *                     Callback definitions
 *****************************************************************************/
/**
 * @enum AdsppmCallbackEventIdType - Callback events
 */
typedef enum
{
    Adsppm_Callback_Event_Id_None,                     //!< None
    Adsppm_Callback_Event_Id_Thermal = 0x0002,         //!< Callback event: thermal event
    Adsppm_Callback_Event_Id_Async_Complete = 0x0004,  //!< Callback event: async request complete event
    Adsppm_Callback_Event_Id_Idle,                     //!< Callback event: except for MDP, MMSS is idle
    Adsppm_Callback_Event_Id_Busy,                     //!< Callback event: except for MDP, MMSS is busy
    Adsppm_Callback_Event_Id_Max,                      //!< Maximum count
    Adsppm_Callback_Event_Id_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmCallbackEventIdType;

/**
 * @struct AdsppmCallbackParamType - Callback parameter structure
 */
typedef struct
{
    AdsppmCallbackEventIdType eventId;
    uint32                    clientId;
    uint32                    callbackDataSize; //!< size of callbackData
    void                      *callbackData;    //!< data structure of callbackData to be defined
                                                //!< when async calls are supported
} AdsppmCallbackParamType;

/******************************************************************************
 *                     Thermal definitions
 *****************************************************************************/
 /**
  * @enum AdsppmThermalType - defines temperature condition levels
  */
typedef enum
{
    Adsppm_Thermal_NONE,                     //!< None
    Adsppm_Thermal_LOW,                      //!< Low temperature condition
    Adsppm_Thermal_NORM,                     //!< Normal temperature condition. No thermal mitigation needed
    Adsppm_Thermal_High_L1,                  //!< High temperature condition level 1
    Adsppm_Thermal_High_L2,                  //!< High temperature condition level 2
    Adsppm_Thermal_High_L3,                  //!< High temperature condition level 3
    Adsppm_Thermal_High_L4,                  //!< High temperature condition level 4
    Adsppm_Thermal_High_L5,                  //!< High temperature condition level 5
    Adsppm_Thermal_High_L6,                  //!< High temperature condition level 6
    Adsppm_Thermal_High_L7,                  //!< High temperature condition level 7
    Adsppm_Thermal_High_L8,                  //!< High temperature condition level 8
    Adsppm_Thermal_High_L9,                  //!< High temperature condition level 9
    Adsppm_Thermal_High_L10,                 //!< High temperature condition level 10
    Adsppm_Thermal_High_L11,                 //!< High temperature condition level 11
    Adsppm_Thermal_High_L12,                 //!< High temperature condition level 12
    Adsppm_Thermal_High_L13,                 //!< High temperature condition level 13
    Adsppm_Thermal_High_L14,                 //!< High temperature condition level 14
    Adsppm_Thermal_High_L15,                 //!< High temperature condition level 15
    Adsppm_Thermal_High_L16,                 //!< High temperature condition level 16
    Adsppm_Thermal_Max,                      //!< Maximum count
    Adsppm_Thermal_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmThermalType;

/******************************************************************************
 *                     Bus definitions
 *****************************************************************************/
/**
 * @struct AdsppmBusBWDataIbAbType - Data type holding BW value for type 3 requests (Ab/Ib)
 */
typedef struct
{
    uint64 Ab;         //!< Average BW in bytes per second
    uint64 Ib;         //!< Instantaneous BW in bytes per second
    uint32 latencyNs;  //!< latency in nano sec
} AdsppmBusBWDataIbAbType;

/**
 * @enum Adsppm DCVS Monitored Routes
 */
typedef enum
{
    Adsppm_DCVS_Route_DSP_DDR,
    Adsppm_DCVS_Route_HCP_DDR,
    Adsppm_DCVS_Route_DMA_DDR,
    Adsppm_DCVS_Route_Max
} Adsppm_DCVS_Monitored_Routes;

/**
 * @enum AdsppmBwUsageLpassType - BW usage types for LPASS
 */
typedef enum
{
    Adsppm_BwUsage_None,                      //!< None
    Adsppm_BwUsage_DSP,                       //!< BW usage by ADSP
    Adsppm_BwUsage_DMA,                       //!< BW usage by DMA/BAM
    Adsppm_BwUsage_EXT_CPU,                   //!< BW usage by CPU outside of LPASS
    Adsppm_BwUsage_Enum_Max,                  //!< Maximum count */
    Adsppm_BwUsage_Force32Bbits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmBwUsageLpassType;

/**
 * @struct AdsppmBusBWDataUsageType - Data type holding BW value for type 1 requests (BW, usage)
 */
typedef struct
{
    uint64 bwBytePerSec;              //!< BW value in bytes per second
    uint32 usagePercentage;           //!< utilization percentage
    AdsppmBwUsageLpassType usageType; //!< BW usage type
} AdsppmBusBWDataUsageType;

typedef union
{
    AdsppmBusBWDataIbAbType  busBwAbIb;
    AdsppmBusBWDataUsageType busBwValue;
} AdsppmBusBWDataType;

/**
 * @enum AdsppmBusBWOperationType - Nature of BW operation
 */
typedef enum
{
    AdsppmBusBWOperation_BW = 1,    //!< Normal BW request
    AdsppmBusBWOperation_RegAccess  //!< BW for register access
}AdsppmBusBWOperationType;

/**
 * @struct AdsppmBusBWRequestValueType - Data type specifying BW request parameters
 */
typedef struct
{
    AdsppmBusRouteType       busRoute;    //!< master/slave pair the BW is requested for
    AdsppmBusBWDataType      bwValue;     //!< BW value
    AdsppmBusBWOperationType bwOperation; //!< BW request reason
} AdsppmBusBWRequestValueType;

typedef enum{
    Adsppm_BwReqClass_Generic,
    Adsppm_BwReqClass_Compensated,
    Adsppm_BwReqClass_Force32Bbits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmBwRequestClass;

/**
 * @struct AdsppmBwReqType - adsppm BW value array that contains BW settings for each corresponding mem ID
 */
typedef struct
{
    uint32                      numOfBw; //!< number of BW settings in the array, max is 16 per client
    AdsppmBusBWRequestValueType pBwArray[MAX_NUM_OF_ADSPPM_BW_ARRAY]; //!< the array of BW settings
    AdsppmBwRequestClass        requestClass;
} AdsppmBwReqType;

/******************************************************************************
 *                     Definitions for private info
 *****************************************************************************/

/**
 * @struct AdsppmInfoAggregatedBwType
 * @brief Structure to store aggregated BW ib/ab values
 */
typedef struct
{
    uint64 adspDdrBwAb;
    uint64 adspDdrBwIb;
    uint64 extAhbDdrBwAb;
    uint64 extAhbDdrBwIb;
    // Internal BW: ADSP<->LPASS
    uint64 intAhbBwAb;
    uint64 intAhbBwIb;
} AdsppmInfoAggregatedBwType;

/**
 * @struct AdsppmInfoAhbType
 * @brief AHB information
 */
typedef struct
{
    uint32 ahbeFreqHz;                      //!< AHB-E bus frequency in Hz
} AdsppmInfoAhbType;

/**
 * @struct AdsppmInfoDcvsAdspDdrBwType
 * @brief DCVS ADSP to DDR BW aggregation information
 */
typedef struct
{
    AdsppmBusBWDataIbAbType dcvsVote;
    AdsppmBusBWDataIbAbType clientsFloorVote;
    AdsppmBusBWDataIbAbType clientsFinalVote;
    AdsppmBusBWDataIbAbType finalVoteToNpa;
} AdsppmInfoDcvsAdspDdrBwType;

/**
 * @struct AdsppmInfoDcvsAdspClockType
 * @brief DCVS ADSP clock aggregation information
 */
typedef struct
{
    uint32 dcvsVote;
    uint32 clientsFloorVote;
    uint32 clientsFinalVote;
    uint32 finalVoteToNpa;
} AdsppmInfoDcvsAdspClockType;

// Private information type
typedef enum
{
    ADSPPM_PRIVATE_INFO_AGGREGATED_BW = 0,
    ADSPPM_PRIVATE_INFO_AHB           = 1,
    ADSPPM_PRIVATE_INFO_ADSPDDR_BW    = 2,
    ADSPPM_PRIVATE_INFO_HCPDDR_BW     = 3,
    ADSPPM_PRIVATE_INFO_DMADDR_BW     = 4,
    ADSPPM_PRIVATE_INFO_ADSPCLOCK     = 5,
} AdsppmInfoPrivateTypeType;

typedef struct
{
    // 'type' is an input from the caller and determines the type of private
    // query
    AdsppmInfoPrivateTypeType type;
    union
    {
        // Used when 'type' is ADSPPM_PRIVATE_INFO_AGGREGATED_BW
        AdsppmInfoAggregatedBwType aggregatedBw;
        // Used when 'type' is ADSPPM_PRIVATE_INFO_AHB
        AdsppmInfoAhbType ahb;
        // Used when 'type' is ADSPPM_PRIVATE_INFO_ADSPDDR_BW
        AdsppmInfoDcvsAdspDdrBwType adspDdrBw;
        // Used when 'type' is ADSPPM_PRIVATE_INFO_ADSPCLOCK
        AdsppmInfoDcvsAdspClockType adspClock;
    };
} AdsppmInfoPrivateType;

/******************************************************************************
 *                     MIPS definitions
 *****************************************************************************/
typedef enum
{
    MipsRequestOperation_None = 0,             //!< MipsRequestOperation_None
    MipsRequestOperation_MIPS = 1,             //!< MipsRequestOperation_MIPS
    MipsRequestOperation_BWandMIPS = 2,        //!< MipsRequestOperation_BWandMIPS
    MipsRequestOperation_MAX = 3,              //!< MipsRequestOperation_MAX
    MipsRequestOperation_EnumForce8Bit = 0x7f  //!< MipsRequestOperation_EnumForce8Bit
} AdsppmMipsOperationType;

typedef enum
{
    Adsppm_Q6ClockRequestUsageType_Mips,
    Adsppm_Q6ClockRequestUsageType_Mpps,
    Adsppm_Q6ClockRequestUsageType_Force32Bbits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmQ6ClockRequestUsageType;

/**
 * @struct AdsppmMipsRequestType - MIPS request structure
 */
typedef struct
{
    uint32                  mipsTotal;      //!< Total MIPS value
    uint32                  mipsPerThread;  //!< MIPS value per HW thread
    AdsppmBusPortIdType     codeLocation;   //!< Location where code is executed from
    AdsppmMipsOperationType reqOperation;   //!< Which operations the MIPS request should include
} AdsppmMipsRequestType;

/**
 * @struct AdsppmMppsRequestType - MPPS request structure
 */
typedef struct
{
    uint32 mppsTotal;       //!< Total MPPS value
    uint32 adspFloorClock;  //!< ADSP Floor clock value in MHz
} AdsppmMppsRequestType;

/**
 * @struct AdsppmQ6ClockRequestInfoType - Q6 Clock Request Info structure
 */
typedef struct
{
    // Union to select either MIPS or MPPS request
    union
    {
        AdsppmMipsRequestType mipsRequestData;  //!< MIPS request Data
        AdsppmMppsRequestType mppsRequestData;  //!< MPPS request Data
    } AdsppmQ6ClockRequestType;
    AdsppmQ6ClockRequestUsageType  usageType;   //!< Usage Type - MIPS/MPPS

} AdsppmQ6ClockRequestInfoType;

/**
 * @struct AdsppmMipsDataType - structure for MIPS aggregate data
 */
typedef struct
{
    uint32 mipsTotal;      //!< Total MIPS value
    uint32 mipsPerThread;  //!< MIPS value per HW thread
} AdsppmMipsDataType;

/**
 * @struct AdsppmMIPSToBWAggregateType - structure for mipstoBW aggregate data
 */
typedef struct
{
    AdsppmMipsDataType          mipsData;  //!< MIPS value (total and per thread)
    AdsppmBusBWRequestValueType mipsToBW;  //!< [OUT] returns BW and bus route
} AdsppmMIPSToBWAggregateType;

/**
 * @struct AdsppmMIPSToClockAggregateType - data structure for MIPS to clock aggregate data
 */
typedef struct
{
    AdsppmMipsDataType mipsData;    //!< MIPS value (total and per thread)
    uint32             qDSP6Clock;  //!< [OUT]Q6 Clock in kHz
} AdsppmMIPSToClockAggregateType;

/******************************************************************************
 *                     Register Programming definitions
 *****************************************************************************/
/**
 * @enum AdsppmRegProgMatchType - Register programming match
 */
typedef enum
{
    Adsppm_RegProg_None,                        //!< None
    Adsppm_RegProg_Norm,                        //!< Normal
    Adsppm_RegProg_Fast,                        //!< Fast
    Adsppm_RegProg_Enum_Max,                    //!< Maximum count
    Adsppm_RegProg_EnumForce32Bit = 0x7fffffff  //!< forcing enum to be 8 bit
} AdsppmRegProgMatchType;

/******************************************************************************
 *                     Core Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmFreqMatchType - Frequency clock match
 */
typedef enum
{
    Adsppm_Freq_At_Least,          //!< At least
    Adsppm_Freq_At_Most,           //!< At most
    Adsppm_Freq_Closest,           //!< Closest
    Adsppm_Freq_Exact,             //!< Exact
    Adsppm_Freq_Max,               //!< Maximum count
    Adsppm_Freq_Force8Bits = 0x7F  //!< forcing enum to be 8 bit
} AdsppmFreqMatchType;

/**
 * @enum AdsppmInfoType - Adsppm info type definition
 */
typedef enum
{
    Adsppm_Info_Type_None,               //!< None
    Adsppm_Info_Type_Max_Value,          //!< get MAX value
    Adsppm_Info_Type_Min_Value,          //!< get Min Value
    Adsppm_Info_Type_Current_Value,      //!< Get current Value
    Adsppm_Info_Type_Max,                //!< Maximum count
    Adsppm_Info_Type_Force8Bits = 0x7F   //!< forcing enum to be 32 bit
} AdsppmInfoType;

/**
 * @struct AdsppmClkValType - Clock setting for one clock ID
 */
typedef struct
{
    AdsppmClkIdType      clkId;     //!< Core clock ID(s) defined by enums of AdsppmClkIdXyzType, Xyz is the core name
    uint32               clkFreqHz; //!< Unit of clock Freq is Hz
    AdsppmFreqMatchType  freqMatch; //!< Frequency match: at least, at most, closest, and exact
} AdsppmClkValType;

/**
 * @struct AdsppmInfoClkFreqType - get clock freq in Hz
 */
typedef struct
{
    uint32 clkId;        //!< Input: Core clock ID(s) defined by AdsppmClkIdType
    uint32 clkFreqHz;    //!< Output: Clock frequency in (Hz)
    uint32 forceMeasure; //!< Control to force the clock to be measured,
                         //!< clock freq may be read from a cached table if this is not set to 1
} AdsppmInfoClkFreqType;

/**
 * @struct AdsppmClkRequestType - Clock value array that contains clock frequencies for each corresponding clock ID
 */
typedef struct
{
    uint32            numOfClk; //!< number of clock settings in the array
    AdsppmClkValType  pClkArray[MAX_NUM_OF_ADSPPM_CLOCK_ARRAY]; //!< the array of clock settings
} AdsppmClkRequestType;

/******************************************************************************
 *                     Domain Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmClkDomainSrcIdLpassType - LPASS clock domain source ID
 */
typedef enum
{
    Adsppm_Clk_Domain_Src_Id_Lpass_None,                     //!< None
    Adsppm_Clk_Domain_Src_Id_Primary_PLL,
    Adsppm_Clk_Domain_Src_Id_Secondary_PLL,
    Adsppm_Clk_Domain_Src_Id_Ternery_PLL,
    Adsppm_Clk_Domain_Src_Id_Lpass_Max,                      //!< Maximum count
    Adsppm_Clk_Domain_Src_Id_Lpass_Force32bits = 0x7FFFFFFF  //!< Forces the enumeration to 32 bits
} AdsppmClkDomainSrcIdLpassType;

typedef union
{
    AdsppmClkDomainSrcIdLpassType clkDomainSrcIdLpass; //!< Clock domain source IDs of LPASS
} AdsppmClkDomainSrcIdType;

/**
 * @struct AdsppmClkDomainType - Clock domain setting for one clock ID
 */
typedef struct
{
    AdsppmClkIdType          clkId;        //!< Core clock ID
    uint32                   clkFreqHz;    //!< Unit of clock Freq is Hz
    AdsppmClkDomainSrcIdType clkDomainSrc; //!< Clock domain source ID
} AdsppmClkDomainType;

/**
 * @struct AdsppmClkDomainReqType - Clock domain array that contains clock domain settings for each corresponding clock ID
 */
typedef struct
{
    uint32              numOfClk; //!< Number of clock settings in the array
    AdsppmClkDomainType pClkDomainArray[MAX_NUM_OF_ADSPPM_CLOCK_ARRAY]; //!< Array of clock domain settings
} AdsppmClkDomainReqType;

/******************************************************************************
 *                     Memory Power definitions
 *****************************************************************************/
/**
 * @enum AdsppmMemPowerStateType - Memory power states
 */
typedef enum
{
    Adsppm_Mem_Power_None,                     //!< None
    Adsppm_Mem_Power_Off,                      //!< Memory off
    Adsppm_Mem_Power_Retention,                //!< Memory in retention
    Adsppm_Mem_Power_Active,                   //!< Memory in active state
    Adsppm_Mem_Power_Max,                      //!< Maximum count
    Adsppm_Mem_Power_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmMemPowerStateType;

/**
 * @struct AdsppmMemPowerReqParamType - Memory power state request parameter structure
 */
typedef struct
{
    AdsppmMemIdType         memory;
    AdsppmMemPowerStateType powerState;
} AdsppmMemPowerReqParamType;

/******************************************************************************
 *                     Set Parameter API definitions
 *****************************************************************************/
/**
 * @enum AdsppmParameterIdType - defines parameter IDs for Adsppm_SetParameter()
 */
typedef enum
{
    Adsppm_Param_Id_None = 0,                //!< Invalid parameter
    Adsppm_Param_Id_Resource_Limit,          //!< Set resource limits (min and max). Works globally
    Adsppm_Param_Id_Client_Ocmem_Map,        //!< Set client's OCMEM allocation map
    Adsppm_Param_Id_Memory_Map,              //!< Set memory address map
    Adsppm_Param_Id_Enum_Max,                //!< Enumeration max
    Adsppm_Param_Id_Force32bit = 0x7fffffff
} AdsppmParameterIdType;

/**
 * @struct AdsppmParameterConfigType - data structure for parameter configuration API
 * Based on paramId pParamConfig will take different structure pointers:
 * paramId:
 *      Adsppm_Param_Id_Client_Ocmem_Map - pParamConfig points to AdsppmOcmemMapType structure
 */
typedef struct
{
    AdsppmParameterIdType paramId;      //!< Parameter ID
    void                 *pParamConfig; //!< Pointer to Parameter-specific structure
} AdsppmParameterConfigType;


/******************************************************************************
 *                     Utilities
 *****************************************************************************/
#define ADSPPM_ASSERT(x)  assert(x)

#ifndef MAX
#define MAX(a, b)  (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a, b)  (((a)<(b))?(a):(b))
#endif

/******************************************************************************
 *                     Functions
 *****************************************************************************/
Adsppm_Status ADSPPM_Init(void);

/**
 * @fn ADSPPM_IsInitialized - Check if ADSPPM initialized successfully
 * @return - 1 ADSPPM init was successful
 *           0 ADSPPM init was unsuccessful
 */
uint32 ADSPPM_IsInitialized(void);

/**
 * @fn ADSPPM_EnterSleep
 * @brief This function is called by Sleep driver at the very late stage of entering ADSP PC.
 *
 * The function will ensure that AHB clock is set as low as possible while ADSP is in power collapse.
 * Limitation: the function should only be called from Sleep context after entering STM.
 */
void ADSPPM_EnterSleep(void);

/**
 * @fn ADSPPM_ExitSleep
 * @brief This function is called by SLeep driver early during exit from ADSP power collapse.
 *
 * The function will restore AHB clock to the frequency, which was set before entering power collapse.
 * Limitation: the function should only be called from Sleep context after entering STM.
 */
void ADSPPM_ExitSleep(void);


#endif /* ADSPPM_H_ */

