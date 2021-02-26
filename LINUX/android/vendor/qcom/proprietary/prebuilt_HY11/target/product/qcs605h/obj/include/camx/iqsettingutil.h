// NOWHINE NC009 <- Shared file with system team so uses non-CamX file naming
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  iqsettingutil.h
/// @brief Calculate IQ module Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IQSETTINGUTIL_H
#define IQSETTINGUTIL_H

struct ISPIQTriggerData;

// Bayer pattern
static const UINT8  RGGB_PATTERN      = 0;
static const UINT8  GRBG_PATTERN      = 1;
static const UINT8  BGGR_PATTERN      = 2;
static const UINT8  GBRG_PATTERN      = 3;
static const UINT8  YCBYCR422_PATTERN = 4;
static const UINT8  YCRYCB422_PATTERN = 5;
static const UINT8  CBYCRY422_PATTERN = 6;
static const UINT8  CRYCBY422_PATTERN = 7;
static const UINT8  Y_PATTERN         = 8;

// Max Values
static const UINT32 NMAX10            = 1024;
static const UINT32 MIN_BIT           = 0;
static const UINT32 MAX_BIT           = 1;
static const INT32  MIN_INT8          = -128;
static const INT32  MAX_INT8          = 127;
static const INT32  MIN_INT9          = -256;
static const INT32  MAX_INT9          = 255;
static const INT32  MIN_INT10         = -512;
static const INT32  MAX_INT10         = 511;
static const INT32  MIN_INT11         = -1024;
static const INT32  MAX_INT11         = 1023;
static const INT32  MIN_INT12         = -2048;
static const INT32  MAX_INT12         = 2047;
static const UINT32 MIN_UINT8         = 0;
static const UINT32 MAX_UINT8         = 255;
static const UINT32 MAX_UINT11        = 2048;
static const UINT32 MAX_UINT12        = 4095;
static const UINT32 MAX_UINT14        = 16383;
static const UINT32 PIPELINE_BITWIDTH = 14;
static const FLOAT  FLOATMAX12        = 4096.0f;
static const FLOAT  FLOATMAX8         = 256.0f;

// Interpolation Level
static const INT32  InitialLevel = 1;

// FD MAX Face Number
static const UINT   MAX_FACE_NUM = 5;

// Q Numbers
static const UINT8  QNumber_4U  = 4;
static const UINT8  QNumber_5U  = 5;
static const UINT8  QNumber_6U  = 6;
static const UINT8  QNumber_7U  = 7;
static const UINT8  QNumber_8U  = 8;
static const UINT8  QNumber_9U  = 9;
static const UINT8  QNumber_10U = 10;
static const UINT8  QNumber_11U = 11;
static const UINT8  QNumber_12U = 12;
static const UINT8  QNumber_13U = 13;
static const UINT8  QNumber_14U = 14;
static const UINT8  QNumber_15U = 15;
static const UINT8  QNumber_16U = 16;
static const UINT8  QNumber_17U = 17;
static const UINT8  QNumber_18U = 18;
static const UINT8  QNumber_19U = 19;
static const UINT8  QNumber_20U = 20;
static const UINT8  QNumber_21U = 21;
static const UINT8  QNumber_22U = 22;
static const UINT8  QNumber_23U = 23;
static const UINT8  QNumber_24U = 24;
static const UINT8  QNumber_25U = 25;
static const UINT8  QNumber_26U = 26;
static const UINT8  QNumber_27U = 27;
static const UINT8  QNumber_28U = 28;
static const UINT8  QNumber_29U = 29;
static const UINT8  QNumber_30U = 30;
static const UINT8  QNumber_31U = 31;
static const UINT8  QNumber_32U = 32;
static const UINT   MaxLUTBank  = 2;

static const UINT32 CHROMATIX_BIT_WIDTH = 12;

// ADRC Const
static const FLOAT MIN_NORM_OUTPUT_GAIN = 0.0f;
static const FLOAT MAX_NORM_OUTPUT_GAIN = 1.0f;

/// @brief Input data for any initialization input/output data
// NOWHINE NC004c: Share code with system team
struct IQLibraryData
{
    VOID* pCustomticData;  ///< OEM library data
};

// Maxmium Region Per Node
static const UINT MaxNumRegion = 20;

// Maxmium Child Nodes for one Parent Node
static const UINT MaxNumChildNode = 3;

// Maxmium Number of the ratios values
static const UINT MaxInterpolationItem = MaxNumChildNode - 1;

/// @brief Different sensor type
enum SensorType
{
    MONO_Y = 0,   ///< Mono Sensor
    BAYER_RGGB,   ///< Bayer Pattern
    BAYER_RCCB,   ///< RCCB format
    BAYER_PDAF,   ///< PDAF sensor
    BAYER_HDR,    ///< HDR sensor
};


// Trigger Interpolation Output
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct InterpolationOutput
{
    UINT32  startIndex;              ///<  Index of start node
    UINT32  endIndex;                ///<  Index of end node
    FLOAT   interpolationRatio;      ///<  Interpolation Ratio
};

/// @brief Trigger Region
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct TriggerRegion
{
    FLOAT start;                     ///< Region start value
    FLOAT end;                       ///< Region end value
};

// Define the chromtix intepoloation tree
/// @brief Node structure of the chromatix tree
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct TuningNode
{
    BOOL         isValid;                                  ///< Is the tuning Node valid
    TuningNode*  pChildNode[MaxNumChildNode];              ///< Child Node Pointer Array
    UINT         numChild;                                 ///< Number of Child Node
    FLOAT        interpolationValue[MaxInterpolationItem]; ///< interpolation Value
    VOID*        pNodeData;                                ///< Pointer to current Node Data
    VOID*        pData;                                    ///< Pointer to the tuning data
    UINT         level;                                    ///< Which level in the tree that this Node is at, starting with 1
};

typedef UINT (*SearchChildNode)(
    TuningNode* pParentNode,
    VOID*       pTriggerData,
    TuningNode* pChildNode);

// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct NodeOperation
{
    SearchChildNode  pSearchChildNode;  ///< Searching function for child node
    UINT             numChildPerNode;   ///< Define the number of change each node can have
};

// Interpolation Function pototype
typedef BOOL (*DoInterpolation)(
    VOID* pData1,
    VOID* pData2,
    FLOAT ratio,
    VOID* pOutput);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IQSettingUtils
///
/// @brief Utility class for IQ setting calculation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
class IQSettingUtils
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DataConversion
    ///
    /// @brief  Convert the Data from float to UINT16
    ///
    /// @param  input Input float value
    /// @param  min   Minimum value
    /// @param  max   Maximum value
    ///
    /// @return The converted UINT16 value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT16 DataConversion(
        FLOAT input,
        UINT32 min,
        UINT32 max)
    {
        INT    result1 = IQSettingUtils::RoundFLOAT(input);
        UINT32 result2 = IQSettingUtils::AbsoluteINT(result1);
        UINT16 result3 = static_cast<UINT16>(IQSettingUtils::ClampUINT32(result2, min, max));

        return result3;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Memcpy
    ///
    /// @brief  Wraps the standard memcpy function; copies "numBytes" from source to destination
    ///
    /// @param  pDst       pointer to destination of copy
    /// @param  pSrc       pointer to source of copy
    /// @param  numBytes   size of data pointed at by pSrc
    ///
    /// @return Dest Pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline VOID* Memcpy(
        VOID*       pDst,
        const VOID* pSrc,
        SIZE_T      numBytes)
    {
        VOID* pData = NULL;

        if ((NULL != pDst) && (NULL != pSrc))
        {
            pData = memcpy(pDst, pSrc, numBytes);
        }

        return pData;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Memset
    ///
    /// @brief  Wraps the standard memset function; sets "numBytes" at the destination pointer to the specified pattern
    ///
    /// @param  pDst        pointer to destination of copy
    /// @param  setVal      character valueto set
    /// @param  numBytes    number of characters (bytes) to copy from pSrc to pDst
    ///
    /// @return Destination pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline VOID* Memset(
        VOID*    pDst,
        INT      setVal,
        SIZE_T   numBytes)
    {
        if (NULL == pDst)
        {
            return NULL;
        }
        return memset(pDst, setVal, numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Memcmp
    ///
    /// @brief  Function to wrap the standard memcmp function; compares the first n bytes of memory between the two sources
    ///
    /// @param  pSrc1      pointer to first source
    /// @param  pSrc2      pointer to second source
    /// @param  numBytes   number of characters (bytes) to compare between pSrc1 and pSrc2
    ///
    /// @return An integer less than, equal to, or greater than zero if the first n bytes of s1 is found, respectively,
    ///         to be the same in relation to s2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT Memcmp(
        const VOID*  pSrc1,
        const VOID*  pSrc2,
        SIZE_T       numBytes)
    {
        return memcmp(pSrc1, pSrc2, numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SquareRootFLOAT
    ///
    /// @brief  Wraps the standard sqrt function
    ///
    /// @param  val    Input value whose Square root need to be found
    ///
    /// @return Float  Square Root value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT SquareRootFLOAT(
        FLOAT val)
    {
        return static_cast<FLOAT>(sqrt(val));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SquareRootDOUBLE
    ///
    /// @brief  Wraps the standard sqrt function
    ///
    /// @param  val    Input value whose Square root need to be found
    ///
    /// @return DOUBLE  Square Root value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline DOUBLE SquareRootDOUBLE(
        DOUBLE val)
    {
        return sqrt(val);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CeilingDOUBLE
    ///
    /// @brief  Find the Ceil value of double
    ///
    /// @param  input  Input double value
    ///
    /// @return Ceil value of the input double value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline DOUBLE CeilingDOUBLE(
        DOUBLE input)
    {
        return ceil(input);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampUINT16
    ///
    /// @brief  Clamp UINT16 to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, below which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return UINT16 Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT16 ClampUINT16(
        UINT16 input1,
        UINT16 min,
        UINT16 max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampINT16
    ///
    /// @brief  Clamp INT16 to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, bellow which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return INT16 Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT16 ClampINT16(
        INT16 input1,
        INT16 min,
        INT16 max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampUINT32
    ///
    /// @brief  Clamp UINT32 to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, below which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return UINT32 Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT32 ClampUINT32(
        UINT32 input1,
        UINT32 min,
        UINT32 max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampDOUBLE
    ///
    /// @brief  Clamp DOUBLE to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, below which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return DOUBLE Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline DOUBLE ClampDOUBLE(
        DOUBLE input1,
        DOUBLE min,
        DOUBLE max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampFLOAT
    ///
    /// @brief  Clamp FLOAT to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, below which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return FLOAT Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT ClampFLOAT(
        FLOAT input1,
        FLOAT min,
        FLOAT max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampINT32
    ///
    /// @brief  Clamp INT32 to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, bellow which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return UINT32 Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT32 ClampINT32(
        INT32 input1,
        INT32 min,
        INT32 max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FloatToQNumber
    ///
    /// @brief  Converts Float value to Q Format
    ///
    /// @param  value  Float Value , for which Needs to converted to Q format
    /// @param  exp    Exponent bits of Q are used for that reg
    ///
    /// @return INT32 The converted value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT32 FloatToQNumber(
        FLOAT value,
        INT   exp)
    {
        return  RoundFLOAT((value * static_cast<FLOAT>(1 << exp)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// QNumberToFloat
    ///
    /// @brief  Converts Q value to Float Format
    ///
    /// @param  value  Q Value , for which Needs to converted to Float
    /// @param  exp    Exponent bits of Q are used for that reg
    ///
    /// @return FLOAT The converted value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT QNumberToFloat(
        INT value,
        INT exp)
    {
        return (static_cast<FLOAT>(value) / static_cast<FLOAT>(1 << (exp)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MAXINT64BITFIELD
    ///
    /// @brief  Maximum Integer with given bit field
    ///
    /// @param  input  Bits for the maximum integer
    ///
    /// @return INT64  Returns the max value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT64 MAXINT64BITFIELD(
        INT input)
    {
        return ((input < 1) ? (0) : (((1ull) << ((input))) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RoundFLOAT
    ///
    /// @brief  Round the FLOAT to Nearest INT
    ///
    /// @param  input input value
    ///
    /// @return Round to the nearest INT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT RoundFLOAT(
        FLOAT input)
    {
        return static_cast<INT>(roundf(input));
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RoundDOUBLE
    ///
    /// @brief  Round the DOUBLE to Nearest INT64
    ///
    /// @param  input input value
    ///
    /// @return Round to the nearest int64 value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT64 RoundDOUBLE(
        DOUBLE input)
    {
        return static_cast<INT64>(llroundl(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AbsoluteINT
    ///
    /// @brief  Find the absolute value of a INT
    ///
    /// @param  input input value
    ///
    /// @return Absolute value of the input INT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT32 AbsoluteINT(
        INT input)
    {
        return abs(input);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AbsoluteFLOAT
    ///
    /// @brief  Find the absolute value of a FLOAT
    ///
    /// @param  input input value
    ///
    /// @return Absolute value of the input FLOAT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT AbsoluteFLOAT(
        FLOAT input)
    {
        return fabsf(input);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AbsoluteDOUBLE
    ///
    /// @brief  Find the absolute value of a DOUBLE
    ///
    /// @param  input input value
    ///
    /// @return Absolute value of the input DOUBLE value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline DOUBLE AbsoluteDOUBLE(
        DOUBLE input)
    {
        return fabs(input);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxFLOAT
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline FLOAT MaxFLOAT(
        FLOAT value1,
        FLOAT value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxDouble
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline DOUBLE MaxDouble(
        DOUBLE value1,
        DOUBLE value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxINT8
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT8 MaxINT8(
        INT8 value1,
        INT8 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxINT16
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT16 MaxINT16(
        INT16 value1,
        INT16 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxINT32
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT32 MaxINT32(
        INT32 value1,
        INT32 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxINT64
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT64 MaxINT64(
        INT64 value1,
        INT64 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT8
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT8 MaxUINT8(
        UINT8 value1,
        UINT8 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT16
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT16 MaxUINT16(
        UINT16 value1,
        UINT16 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT32
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT32 MaxUINT32(
        UINT32 value1,
        UINT32 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT64
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT64 MaxUINT64(
        UINT64 value1,
        UINT64 value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinFLOAT
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline FLOAT MinFLOAT(
        FLOAT value1,
        FLOAT value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinDouble
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline DOUBLE MinDouble(
        DOUBLE value1,
        DOUBLE value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinINT8
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT8 MinINT8(
        INT8 value1,
        INT8 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinINT16
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT16 MinINT16(
        INT16 value1,
        INT16 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinINT32
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT32 MinINT32(
        INT32 value1,
        INT32 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinINT64
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT64 MinINT64(
        INT64 value1,
        INT64 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT8
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT8 MinUINT8(
        UINT8 value1,
        UINT8 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT16
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT16 MinUINT16(
        UINT16 value1,
        UINT16 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT32
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT32 MinUINT32(
        UINT32 value1,
        UINT32 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT64
    ///
    /// @brief  Returns the larger of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline UINT64 MinUINT64(
        UINT64 value1,
        UINT64 value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SignOfINT32
    ///
    /// @brief  Returns the sign of the input value.
    ///
    /// @param  value value whose sign need to check
    ///
    /// @return The sign of the value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT SignOfINT32(
        INT32 value)
    {
        return ((value < 0) ? (-1) : (1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SignOfINT64
    ///
    /// @brief  Returns the sign of the input value.
    ///
    /// @param  value value whose sign need to check
    ///
    /// @return The sign of the value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline INT SignOfINT64(
        INT64 value)
    {
        return ((value < 0) ? (-1) : (1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SignOfFLOAT
    ///
    /// @brief  Returns the sign of the input value.
    ///
    /// @param  value value whose sign need to check
    ///
    /// @return The sign of the value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT SignOfFLOAT(
        FLOAT value)
    {
        return ((value < 0) ? (-1) : (1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FEqual
    ///
    /// @brief  Compares the floating point numbers
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return BOOL TRUE if equal FALSE if not equal
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline BOOL FEqual(
        FLOAT value1,
        FLOAT value2)
    {
        return ((fabs(value1 - value2) < 1e-9) ? (TRUE) : (FALSE));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CalculateInterpolationRatio
    ///
    /// @brief  Calculate the interpolation Ratio within the region
    ///
    /// @param  value  The input value within the region
    /// @param  start  The start value of the region
    /// @param  end    The end value of the region
    ///
    /// @return The interpolation ratio value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  __inline FLOAT CalculateInterpolationRatio(
        DOUBLE value,
        DOUBLE start,
        DOUBLE end)
    {
        FLOAT ratio;

        if (value < start)
        {
            ratio = 0.0f;
        }
        else if (value >= end)
        {
            ratio = 1.0f;
        }
        else
        {
            ratio = static_cast<FLOAT>((value  - start) / (end - start));
        }

        return ratio;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InterpolationFloatBilinear
    ///
    /// @brief  Perform Bilinear Interpolation to two float value
    ///
    /// @param  inputData1  Input parameter one, float value
    /// @param  inputData2  Input parameter two, float value
    /// @param  ratioData   Interpolation Ratio
    ///
    /// @return The interpolation result, float value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT InterpolationFloatBilinear(
        FLOAT             inputData1,
        FLOAT             inputData2,
        FLOAT             ratioData)
    {
        FLOAT  result        = 0.0f;
        DOUBLE output        = 0.0f;
        FLOAT  outputInteger = 0.0f;

        // To get only integer value. Type cast from float to int will be round down.
        INT32 input1d = static_cast<INT32>(inputData1);
        INT32 input2d = static_cast<INT32>(inputData2);

        // To get only decimal value and make big value by multiplying 1000000
        DOUBLE input1 = static_cast<DOUBLE>(inputData1 - input1d) * 1000000.0f;
        DOUBLE input2 = static_cast<DOUBLE>(inputData2 - input2d) * 1000000.0f;
        DOUBLE ratio  = static_cast<DOUBLE>(ratioData);

        // Calculate for decimal value and make proper decimal value by deviding 1000000
        output = input1 + ratio * (input2 - input1);
        result = static_cast<FLOAT>(output) / 1000000.0f;

        // Calculate for integer value
        outputInteger = input1d + ratio * static_cast<FLOAT>(input2d - input1d);

        // SUM interger and decimal
        result += outputInteger;

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InterpolationFloatNearestNeighbour
    ///
    /// @brief  Perform Interpolation to nearest float value
    ///
    /// @param  inputData1  Input parameter one, float value
    /// @param  inputData2  Input parameter two, float value
    /// @param  ratioData   Interpolation Ratio
    ///
    /// @return The interpolation result, float value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT InterpolationFloatNearestNeighbour(
        FLOAT             inputData1,
        FLOAT             inputData2,
        FLOAT             ratioData)
    {
        FLOAT  result = 0.0f;
        DOUBLE output = 0;

        output = ((ratioData + 0.500f) >= 1.0f) ? inputData2 : inputData1;
        result = static_cast<FLOAT>(output);

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GettriggerHDRAEC
    ///
    /// @brief  Get Trigger HDR value
    ///
    /// @param  aec_hdr_control      HDR control method
    /// @param  exposureTimeTrigger  exposure time trigger
    /// @param  aecSensitivity       aec sensitivity
    ///
    /// @return The trigger value, float value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT GettriggerHDRAEC(
        ispglobalelements::tuning_control_aec_hdr_type aec_hdr_control,
        FLOAT                                          exposureTimeTrigger,
        FLOAT                                          aecSensitivity)
    {
        FLOAT   triggerHDRAEC = 0.0f;  ///< HDRAEC trigger

        switch (aec_hdr_control)
        {
            case ispglobalelements::tuning_control_aec_hdr_type::control_exp_time_ratio:
                triggerHDRAEC = exposureTimeTrigger;
                break;
            case ispglobalelements::tuning_control_aec_hdr_type::control_aec_exp_sensitivity_ratio:
                triggerHDRAEC = aecSensitivity;
                break;
            default:
                /// @todo (CAMX-1812) Need to add logging for Common library
                break;
        }

        return triggerHDRAEC;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GettriggerAEC
    ///
    /// @brief  Get Trigger HDR value
    ///
    /// @param  aec_exp_control  AEC control method
    /// @param  luxIndex         lux index
    /// @param  realIndex        real gain value
    ///
    /// @return The trigger value , float value
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline FLOAT GettriggerAEC(
        ispglobalelements::tuning_control_aec_type aec_exp_control,
        FLOAT                                      luxIndex,
        FLOAT                                      realGain)
    {
        FLOAT   triggerAEC = 0.0f;  ///< AEC trigger

        switch (aec_exp_control)
        {
            case ispglobalelements::tuning_control_aec_type::control_lux_idx:
                triggerAEC = luxIndex;
                break;
            case ispglobalelements::tuning_control_aec_type::control_gain:
                triggerAEC = realGain;
                break;
            default:
                /// @todo (CAMX-1812) Need to add logging for Common library
                break;
        }

        return triggerAEC;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyTriggerRegionHDRAEC
    ///
    /// @brief  Copy Trigger Region for HDRAEC Node
    ///
    /// @param  aec_hdr_control  HDR control method
    /// @param  hdr_aec_trigger  Input data
    /// @param  triggerRegion    Pointer to trigger region to be filled
    ///
    /// @return NONE
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline VOID CopyTriggerRegionHDRAEC(
        ispglobalelements::tuning_control_aec_hdr_type aec_hdr_control,
        ispglobalelements::trigger_pt_type_aec_hdr*    hdr_aec_trigger,
        TriggerRegion*                                 pTriggerRegion)
    {
        if (NULL != pTriggerRegion)
        {
            switch (aec_hdr_control)
            {
                case ispglobalelements::tuning_control_aec_hdr_type::control_exp_time_ratio:
                    pTriggerRegion->start = hdr_aec_trigger->exp_time_start;
                    pTriggerRegion->end   = hdr_aec_trigger->exp_time_end;
                    break;
                case ispglobalelements::tuning_control_aec_hdr_type::control_aec_exp_sensitivity_ratio:
                    pTriggerRegion->start = hdr_aec_trigger->aec_sensitivity_start;
                    pTriggerRegion->end   = hdr_aec_trigger->aec_sensitivity_end;
                    break;
                default:
                    /// @todo (CAMX-1812) Need to add logging for Common library
                    break;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyTriggerRegionAEC
    ///
    /// @brief Copy Trigger Region for AEC Node
    ///
    /// @param  aec_control      HDR control method
    /// @param  aec_trigger      Input data
    /// @param  triggerRegion    Pointer to trigger region to be filled
    ///
    /// @return NONE
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline VOID CopyTriggerRegionAEC(
        ispglobalelements::tuning_control_aec_type aec_control,
        globalelements::trigger_pt_type_aec*       aec_trigger,
        TriggerRegion*                             pTriggerRegion)
    {
        if (NULL != pTriggerRegion)
        {
            switch (aec_control)
            {
                case ispglobalelements::tuning_control_aec_type::control_lux_idx:
                    pTriggerRegion->start = aec_trigger->lux_idx_start;
                    pTriggerRegion->end   = aec_trigger->lux_idx_end;
                    break;
                case ispglobalelements::tuning_control_aec_type::control_gain:
                    pTriggerRegion->start = aec_trigger->gain_start;
                    pTriggerRegion->end   = aec_trigger->gain_end;
                    break;
                default:
                    /// @todo (CAMX-1812) Need to add logging for Common library
                    break;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyTriggerRegion
    ///
    /// @brief  Copy Trigger Region value from chromatix
    ///
    /// @param  pNodeTrigger     Pointer to the trigger region in chromatix
    /// @param  triggerRegion    Pointer to target trigger region
    ///
    /// @return NONE
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline VOID CopyTriggerRegion(
        ispglobalelements::trigger_pt_type* pNodeTrigger,
        TriggerRegion*                      pTriggerRegion)
    {
        pTriggerRegion->start = pNodeTrigger->start;
        pTriggerRegion->end   = pNodeTrigger->end;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetIndexPtTrigger
    ///
    /// @brief  Find out which region the input value is at.
    ///         If the input value is located between two regions, calculate the interpolation value.
    ///
    /// @param  pTriggerSet  The array of the trigger region
    /// @param  numRegion    Total number of region
    /// @param  triggerValue The input trigger value
    /// @param  pOutput0     The output value of trigger region index and interpolation ratio
    ///
    /// @return None
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetIndexPtTrigger(
        TriggerRegion*       pTriggerSet,
        UINT32               numRegion,
        FLOAT                triggerValue,
        InterpolationOutput* pOutput0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InterpolateTuningData
    ///
    /// @brief  Walk across the tree and perform interpolation to the tuning data
    ///
    /// @param  pNode            Pointer to the tip node
    /// @param  numOfNode        Number of the no leaf node
    /// @param  numOfLevel       Number of level of the tree
    /// @param  pDoInterpolation Function pointer to perform node level interpolation
    ///
    /// @return True if success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL InterpolateTuningData(
        TuningNode*     pNode,
        UINT            numOfNode,
        UINT            numOfLevel,
        DoInterpolation pDoInterpolation);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupInterpolationTree
    ///
    /// @brief  Setup Interpolation Tree
    ///         The tree is formed by an arry of nodes, in the following order:
    ///         level 1 ---------------------------> node[0]
    ///         level 2 ------------------>node[1]             node[2]
    ///         level 3 ------------>node[3]     node[4] node[5]      node[6]
    ///
    /// @param  pTipNode        Pointer to the first node of the tree
    /// @param  numOfLevel      Number of levels of the tree
    /// @param  pOperationTable Pointer to the level based node search function table
    /// @param  pTriggerList    Pointer to the trigger value list
    ///
    /// @return True if success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL SetupInterpolationTree(
        TuningNode*    pTipNode,
        UINT           numOfLevel,
        NodeOperation* pOperationTable,
        VOID*          pTriggerList);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GainCurveSampling
    ///
    /// @brief  Gain curve sampling
    ///
    /// @param  pKneeInput  Pointer to the Knee input.
    /// @param  pKneeOutput Pointer to the Knee output
    /// @param  pCoef       Pointer to the Coefficient
    /// @param  pInput      Pointer to the Input curve
    /// @param  pOutput     Pointer to the Output curve
    /// @param  lutSize     ADRC LUT Size
    /// @param  rate        rate to generate output curve.
    /// @param  gain        DRC gain
    /// @param  qFactor     qFactor
    ///
    /// @return BOOL  Return True if succeed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL GainCurveSampling(
        FLOAT*       pKneeInput,
        FLOAT*       pKneeOutput,
        FLOAT*       pCoef,
        const FLOAT* pInput,
        FLOAT*       pOutput,
        UINT32       lutSize,
        FLOAT        rate,
        FLOAT        gain,
        UINT32       qFactor);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ModuleInitialize
    ///
    /// @brief  Function to Initiate the IQ Setting Module
    ///
    /// @param  pData Pointer to the IQ Library Data
    ///
    /// @return BOOL  Return True if succeed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL ModuleInitialize(
        IQLibraryData* pData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ModuleUninitialize
    ///
    /// @brief  Function to Uninitialize the IQ Setting Module
    ///
    /// @param  pData Pointer to the IQ Library Data
    ///
    /// @return BOOL  Return True if succeed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL ModuleUninitialize(
        IQLibraryData* pData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DumpTriggerCondition
    ///
    /// @brief  Dump Trigger Condition
    ///
    /// @param  pTriggerData Pointer to the Trigger Data
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID DumpTriggerCondition(
        ISPIQTriggerData* pTriggerData);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MAXINT32BITFIELD
    ///
    /// @brief  Maximum Integer with given bit field
    ///
    /// @param  input  Bits for the maximum integer
    ///
    /// @return INT32  Returns the max value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT32 MAXINT32BITFIELD(
        INT32 input)
    {
        return (((input) < (1)) ? 0 : ((1ll) << ((input)-1)) - 1);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MININT32BITFIELD
    ///
    /// @brief  Minimum Integer with given bit field
    ///
    /// @param  input  Bits for the maximum integer
    ///
    /// @return INT32  Returns the min value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT32 MININT32BITFIELD(
        INT32 input)
    {
        return ((input) < (1)) ? 0 : (-(((1ll) << ((input)-1))));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MINUINTBITFIELD
    ///
    /// @brief  Minmum Unsigned Integer with given bit field
    ///
    /// @param  input  Bits for the maximum integer
    ///
    /// @return UINT32 Returns the min value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT32 MINUINTBITFIELD(
        INT input)
    {
        return input * 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MAXUINTBITFIELD
    ///
    /// @brief  Maximum Unsigned Integer with given bit field
    ///
    /// @param  input  Bits for the maximum integer
    ///
    /// @return UINT32 Returns the maxmium value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT32 MAXUINTBITFIELD(
        INT input)
    {
        return ((((input) < (1)) ? (0) : (((1ull) << ((input))) - 1)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Ceiling
    ///
    /// @brief  This function finds the integer ceiling of a FLOAT precision operand
    ///
    /// @param  input value whose ceiling needs to be calculated
    ///
    /// @return Integer ceiling
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT32 Ceiling(
        FLOAT input)
    {
        return static_cast<UINT32>(ceil(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Floor
    ///
    /// @brief  This function finds the integer floor of a FLOAT precision operand
    ///
    /// @param  input value whose floor needs to be calculated
    ///
    /// @return Integer ceiling
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline UINT32 Floor(
        FLOAT input)
    {
        return static_cast<UINT32>(floor(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InitializeNode
    ///
    /// @brief  Initialize an interpoloatin Node
    ///
    /// @param  pNode       Pointer to the interpolation node
    /// @param  pDataBuffer Pointer to the tuning data buffer
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID InitializeNode(
        TuningNode* pNode,
        VOID*       pDataBuffer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddNodeToInterpolationTree
    ///
    /// @brief  Add a subnode to the interpoloation Tree
    ///
    /// @param  pParentNode      Pointer to the parent node
    /// @param  pChildNode       Pointer to the child node
    /// @param  pData            Pointer to the payload data of the child node
    /// @param  pTuningData      Pointer to the tunning data of the child node, it could be NULL
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID AddNodeToInterpolationTree(
        TuningNode* pParentNode,
        TuningNode* pChildNode,
        VOID*       pData,
        VOID*       pTuningData);
};

#endif // IQSETTINGUTIL_H
