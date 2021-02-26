////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxutils.h
/// @brief Various static inline utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXUTILS_H
#define CAMXUTILS_H

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "camxincs.h"
#include "camxmem.h"

#ifdef BitScanForward       // NOWHINE PR002 <- Win32 definition
#undef BitScanForward
#endif // BitScanForward
#ifdef BitScanReverse       // NOWHINE PR002 <- Win32 definition
#undef BitScanReverse
#endif // BitScanReverse


CAMX_NAMESPACE_BEGIN

const UINT64 MicroMetersPerMilliMeter   = 1000ULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Utils
///
/// @brief General utility class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Utils
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Result Code Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CamxResultToErrno
    ///
    /// @brief  Converts a CamxResult result code to an errno result code
    ///
    /// @param  result The CamxResult result code to convert
    ///
    /// @return The errno code corresponding to the CamxResult result code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOWHINE GR017: Converting to int type for top-level API
    static CAMX_INLINE int CamxResultToErrno(
        CamxResult result)
    {
        // NOWHINE GR017: Converting to int type for top-level API
        int errnoResult = 0;

        switch (result)
        {
            case CamxResultSuccess:
                errnoResult = 0;
                break;
            case CamxResultEFailed:
                errnoResult = -ENODEV;
                break;
            case CamxResultEUnsupported:
                errnoResult = -EPERM;
                break;
            case CamxResultEInvalidState:
                errnoResult = -EPERM;
                break;
            case CamxResultEInvalidArg:
                errnoResult = -EINVAL;
                break;
            case CamxResultEInvalidPointer:
                errnoResult = -EFAULT;
                break;
            case CamxResultENoSuch:
                errnoResult = -EINVAL;
                break;
            case CamxResultEOutOfBounds:
                errnoResult = -EINVAL;
                break;
            case CamxResultENoMemory:
                errnoResult = -ENOMEM;
                break;
            case CamxResultETimeout:
                errnoResult = -ETIME;
                break;
            case CamxResultENoMore:
                errnoResult = -ENODATA;
                break;
            case CamxResultENeedMore:
                errnoResult = -EPERM;
                break;
            case CamxResultEExists:
                errnoResult = -EEXIST;
                break;
            case CamxResultEPrivLevel:
                errnoResult = -EACCES;
                break;
            case CamxResultEResource:
                errnoResult = -EPERM;
                break;
            case CamxResultEUnableToLoad:
                errnoResult = -1;
                break;
            case CamxResultEInProgress:
                errnoResult = -EINPROGRESS;
                break;
            case CamxResultETryAgain:
                errnoResult = -EAGAIN;
                break;
            case CamxResultEBusy:
                errnoResult = -EBUSY;
                break;
            case CamxResultEReentered:
                errnoResult = -EPERM;
                break;
            case CamxResultEReadOnly:
                errnoResult = -EPERM;
                break;
            case CamxResultEOverflow:
                errnoResult = -EOVERFLOW;
                break;
            case CamxResultEOutOfDomain:
                errnoResult = -EDOM;
                break;
            case CamxResultEInterrupted:
                errnoResult = -EINTR;
                break;
            case CamxResultEWouldBlock:
                errnoResult = -EPERM;
                break;
            case CamxResultETooManyUsers:
#if defined (_WIN32)
                errnoResult = -68;
#else // WIN32
                errnoResult = -EUSERS;
#endif // WIN32
                break;
            case CamxResultENotImplemented:
                errnoResult = -ENOSYS;
                break;
            default:
                break;
        }

        return errnoResult;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Math Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetInterpolationRatio
    ///
    /// @brief  Calculate ratio of a FLOAT value within certain range
    ///
    /// @param  input      Input value to be interpolated. Its value should be between startValue and endValue.
    /// @param  startValue Start value
    /// @param  endValue   End value
    ///
    /// @return Interpolation Ratio (0.0f to 1.0f)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE FLOAT GetInterpolationRatio(
        FLOAT input,
        FLOAT startValue,
        FLOAT endValue)
    {
        FLOAT output = 0.0f;

        if ((endValue != startValue) &&
            (input    >= startValue) &&
            (input    <= endValue))
        {
            output = 1.0f - (input - startValue)/(endValue - startValue);
        }
        else
        {
            output = 0.0f;
            CAMX_ASSERT_ALWAYS();
        }

        return output;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsUINT32Odd
    ///
    /// @brief  Check if UINT32 is odd number
    ///
    /// @param  input input value to be checked for odd
    ///
    /// @return UINT32 True if input is odd number
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 IsUINT32Odd(
        UINT32 input)
    {
        return ((input & 0x0001) ? TRUE : FALSE);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsUINT32Even
    ///
    /// @brief  Check if UINT32 is Even number
    ///
    /// @param  input input value to be checked for Even
    ///
    /// @return UINT32 True if input is Even number
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 IsUINT32Even(
        UINT32 input)
    {
        return ((input & 0x0001) ? FALSE : TRUE);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// OddFloorUINT32
    ///
    /// @brief  Floor UINT32 to Odd number
    ///
    /// @param  input input value to be rounded down
    ///
    /// @return UINT32 Rounded down value of input
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 OddFloorUINT32(
        UINT32 input)
    {
        UINT32 result;

        if (0 == input)
        {
            result = 0;
        }
        else if (0 != (input & 0x00000001))
        {
            result = input;
        }
        else
        {
            result = input - 0x00000001;
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// EvenFloorUINT32
    ///
    /// @brief  Floor UINT32 to Even number
    ///
    /// @param  input input value to be rounded down
    ///
    /// @return UINT32 Rounded down value of input
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 EvenFloorUINT32(
        UINT32 input)
    {
        return (input & 0xFFFFFFFE);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// OddCeilingUINT32
    ///
    /// @brief  Ceil UINT32 to Odd number
    ///
    /// @param  input input value to be rounded up
    ///
    /// @return UINT32 Rounded up value of input
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 OddCeilingUINT32(
        UINT32 input)
    {
        return (input | 0x00000001);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// EvenCeilingUINT32
    ///
    /// @brief  Ceil UINT32 to Even number
    ///
    /// @param  input input value to be rounded up
    ///
    /// @return UINT32 Rounded up value of input
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 EvenCeilingUINT32(
        UINT32 input)
    {
        UINT32 result;

        if (0xFFFFFFFF == input)
        {
            result = input;
        }
        else if (0 != (input & 0x00000001))
        {
            result = input + 0x00000001;
        }
        else
        {
            result = input;
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DivideAndCeil
    ///
    /// @brief  calculates the ceil after dividing a UINT32
    ///
    /// @param  val     UINT32 to be divided and ceiled
    /// @param  divisor UINT32 to use as the divisor in the operation
    ///
    /// @return UINT32  ceil of the quotient
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 DivideAndCeil(
        UINT32 val,
        UINT32 divisor)
    {
        CAMX_ASSERT(divisor != 0);
        return (val + (divisor - 1)) / divisor;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FloorUINT32
    ///
    /// @brief  Floor UINT32 to multiple of base floor value
    ///
    /// @param  floor the base number to which value should be floored
    /// @param  input input value to be floored
    ///
    /// @return UINT32 Floored input value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 FloorUINT32(
        UINT32 floor,
        UINT32 input)
    {
        return (input & (0xFFFFFFFF - (floor - 1)));
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
    static CAMX_INLINE UINT32 ClampUINT32(
        UINT32 input1,
        UINT32 min,
        UINT32 max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    }

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
    static CAMX_INLINE INT32 ClampINT32(
        INT32 input1,
        INT32 min,
        INT32 max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ClampFLOAT
    ///
    /// @brief  Clamp FLOAT to the range of maximum and minimum
    ///
    /// @param  input1 input value to be clamped
    /// @param  min    Minimum value, bellow which the input will be clamped
    /// @param  max    Maximum value, beyond which the input will be clamped
    ///
    /// @return UINT32 Input if it is range of max and minimum, else clamped value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE FLOAT ClampFLOAT(
        FLOAT input1,
        FLOAT min,
        FLOAT max)
    {
        return (((input1) <= (min)) ? (min) : (((input1) >= (max)) ? (max) : (input1)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RoundFLOAT
    ///
    /// @brief  Round the FLOAT to Nearest INT
    ///
    /// @param  input input value
    ///
    /// @return Round to the nearest int value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT RoundFLOAT(
        FLOAT input)
    {
        return static_cast<INT>(roundf(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Power
    ///
    /// @brief  Utility to return the base raised to the power of exponent
    ///
    /// @param  base     Value to be used a base
    /// @param  exponent Value to be which the base should be raised to
    ///
    /// @return base to the power of exponent
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE DOUBLE Power(
        DOUBLE base,
        DOUBLE exponent)
    {
        DOUBLE result = 0.0;

        if (0.0 == exponent)
        {
            result = 1.0;
        }
        else if (0.0 != base)
        {
            result = pow(base, exponent);
        }
        else
        {
            result = 0.0;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AbsoluteINT32
    ///
    /// @brief  Find the absolute value of a INT32
    ///
    /// @param  input input value
    ///
    /// @return Absolute value of the input INT32 value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 AbsoluteINT32(
        INT32 input)
    {
        return static_cast<UINT32>(abs(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RoundDOUBLE
    ///
    /// @brief  Round the DOUBLE to Nearest INT64
    ///
    /// @param  input input value
    ///
    /// @return Round to the nearest int64 value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT64 RoundDOUBLE(
        DOUBLE input)
    {
        return static_cast<INT64>(llroundl(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AbsoluteFLOAT
    ///
    /// @brief  Find the absolute value of a FLOAT
    ///
    /// @param  input input value
    ///
    /// @return Absolute value of the input FLOAT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE DOUBLE AbsoluteFLOAT(
        FLOAT input)
    {
        return fabs(input);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SqrtF
    ///
    /// @brief  Find the square root value of a FLOAT
    ///
    /// @param  input input value
    ///
    /// @return Square root value of the input FLOAT value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE FLOAT SqrtF(
        FLOAT input)
    {
        return static_cast<FLOAT>(sqrt(input));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sqrt
    ///
    /// @brief  Find the square root value of a DOUBLE
    ///
    /// @param  input input value
    ///
    /// @return Square root value of the input DOUBLE value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE DOUBLE Sqrt(
        DOUBLE input)
    {
        return sqrt(input);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LinearInterpolationINT
    ///
    /// @brief  Perform Linear Interpolation for INT value.
    ///
    /// @param  input1 Interpolation range end value
    /// @param  input2 Interpolation range start value
    /// @param  ratio  ratio value
    ///
    /// @return Pointer to the Matrix
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT LinearInterpolationINT(
        INT   input1,
        INT   input2,
        FLOAT ratio)
    {
        return RoundFLOAT(input2 + (ratio * (input1 - input2)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LinearInterpolationFLOAT
    ///
    /// @brief  Perform Linear Interpolation, Input is Float
    ///
    /// @param  input1 Interpolation range end value
    /// @param  input2 Interpolation range start value
    /// @param  ratio  ratio value
    ///
    /// @return Pointer to the Matrix
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE FLOAT LinearInterpolationFLOAT(
        FLOAT input1,
        FLOAT input2,
        FLOAT ratio)
    {
        return (input2 + (ratio * (input1 - input2)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetUnityMatrix
    ///
    /// @brief  Set the Input Matrix to the Unity Matrix
    ///
    /// @param  pIn        pointer to the input Matrix
    /// @param  matrixSize inputMatrix is of size x size
    ///
    /// @return Pointer to the unity  matrix
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE FLOAT* SetUnityMatrix(
        FLOAT* pIn,
        UINT   matrixSize)
    {
        UINT countI = 0;
        UINT countJ = 0;

        if ((NULL != pIn) && (matrixSize > 0))
        {
            for (countI = 0; countI < matrixSize; countI++)
            {
                for (countJ = 0; countJ < matrixSize; countJ++)
                {
                    if (countI == countJ)
                    {
                        pIn[countJ + countI * matrixSize] = 1.0f;
                    }
                    else
                    {
                        pIn[countJ + countI * matrixSize] = 0.0f;
                    }
                }
            }
        }

        return pIn;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MatrixMultiply
    ///
    /// @brief  Multiply two matrices
    ///
    /// @param  pIn1        Pointer to the input Matrix1
    /// @param  pIn2        Pointer to the input Matrix2
    /// @param  pOut        Pointer to the output Matrix
    /// @param  matrixSize  Matrix is of size x size
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID MatrixMultiply(
        FLOAT (*pIn1)[3],
        FLOAT (*pIn2)[3],
        FLOAT (*pOut)[3],
        UINT  matrixSize)
    {
        UINT countI = 0;
        UINT countJ = 0;
        UINT countK = 0;
        if ((NULL != pIn1) && (NULL != pIn2) && (NULL != pOut) && (matrixSize > 0))
        {
            for (countI = 0; countI < matrixSize; countI++)
            {
                for (countJ = 0; countJ < matrixSize; countJ++)
                {
                    pOut[countI][countJ] = 0;
                    for (countK = 0; countK < matrixSize; countK++)
                    {
                        pOut[countI][countJ] += (pIn1[countI][countK] * pIn2[countK][countJ]);
                    }
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MatrixInverse3x3
    ///
    /// @brief  Compute Inverse of a matrix
    ///
    /// @param  pIn        Pointer to the input Matrix
    /// @param  pOut       Pointer to the output Matrix
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL MatrixInverse3x3(
        FLOAT* pIn,
        FLOAT* pOut)
    {
        BOOL result = TRUE;
        FLOAT det   = 0.0f;

        if ((NULL != pIn) && (NULL != pOut))
        {
            det = pIn[0] * (pIn[4] * pIn[8] - pIn[5] * pIn[7]) +
                  pIn[1] * (pIn[5] * pIn[6] - pIn[3] * pIn[8]) +
                  pIn[2] * (pIn[3] * pIn[7] - pIn[4] * pIn[6]);
            if (FEqual(det, 0.0f))
            {
                result = FALSE;
            }
            else
            {
                pOut[0] = (pIn[4] * pIn[8] - pIn[5] * pIn[7]) / det;
                pOut[1] = (pIn[2] * pIn[7] - pIn[1] * pIn[8]) / det;
                pOut[2] = (pIn[1] * pIn[5] - pIn[2] * pIn[4]) / det;
                pOut[3] = (pIn[5] * pIn[6] - pIn[3] * pIn[8]) / det;
                pOut[4] = (pIn[0] * pIn[8] - pIn[2] * pIn[6]) / det;
                pOut[5] = (pIn[2] * pIn[3] - pIn[0] * pIn[5]) / det;
                pOut[6] = (pIn[3] * pIn[7] - pIn[4] * pIn[6]) / det;
                pOut[7] = (pIn[1] * pIn[6] - pIn[0] * pIn[7]) / det;
                pOut[8] = (pIn[0] * pIn[4] - pIn[1] * pIn[3]) / det;
            }
        }
        else
        {
            result = FALSE;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsPowerOfTwo
    ///
    /// @brief  Checks if a value is a power of 2
    ///
    /// @param  val    value to check
    ///
    /// @return TRUE if the number is a power of 2, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL IsPowerOfTwo(
        SIZE_T val)
    {
        return ((val == 0) ? FALSE : (((val & (val - 1)) == 0) ? TRUE : FALSE));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlignFloor
    ///
    /// @brief  This function is used to align down any SIZE_T value to a specific power of 2
    ///
    /// @param  operand     value to be aligned
    /// @param  alignment   desired alignment
    ///
    /// @return Value aligned as specified
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE SIZE_T ByteAlignFloor(
        SIZE_T operand,
        UINT   alignment)
    {
        // This function only works for a power-of-2 alignment.
        CAMX_ASSERT(IsPowerOfTwo(alignment));

        return ((operand) & ~(static_cast<SIZE_T>(alignment) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlignFloor64
    ///
    /// @brief  This function is used to align down any UINT64 value to a specific power of 2
    ///
    /// @param  operand      value to be aligned
    /// @param  alignment    desired alignment
    ///
    /// @return Value aligned as specified
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT64 ByteAlignFloor64(
        UINT64 operand,
        UINT   alignment)
    {
        // This function only works for a power-of-2 alignment
        CAMX_ASSERT(IsPowerOfTwo(alignment));

        return ((operand) & ~(static_cast<UINT64>(alignment) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlignFloorPtr
    ///
    /// @brief  This function is used to align down a pointer to a specific power of 2 BYTE boundary
    ///
    /// @param  pMem        pointer to be aligned
    /// @param  alignBytes  desired alignment in bytes
    ///
    /// @return Pointer aligned down as specified
    ///
    /// @remark Use The ByteAlignFloor() function for non-pointer alignment
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID* ByteAlignFloorPtr(
        VOID*  pMem,
        UINT   alignBytes)
    {
        // This function only works for a power-of-2 alignment
        CAMX_ASSERT(IsPowerOfTwo(alignBytes));

        return reinterpret_cast<VOID*>((reinterpret_cast<SIZE_T>(pMem)) & ~(static_cast<SIZE_T>(alignBytes) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlign
    ///
    /// @brief  This function is used to align up any SIZE_T value to a specific power of 2
    ///
    /// @param  operand     value to be aligned
    /// @param  alignment   desired alignment
    ///
    /// @return Value aligned as specified
    ///
    /// @remark Use The ByteAlignPtr() function for pointer calculations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE SIZE_T ByteAlign(
        SIZE_T operand,
        UINT   alignment)
    {
        // This function only works for a power-of-2 alignment.
        CAMX_ASSERT(IsPowerOfTwo(alignment));

        return ((operand + (alignment - 1)) & ~(static_cast<SIZE_T>(alignment) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlign64
    ///
    /// @brief  This function is used to align up any UINT64 value to a specific power of 2
    ///
    /// @param  operand     value to be aligned
    /// @param  alignment   desired alignment
    ///
    /// @return Value aligned as specified
    ///
    /// @remark Use The ByteAlignPtr() function for pointer calculations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT64 ByteAlign64(
        UINT64 operand,
        UINT   alignment)
    {
        // This function only works for a power-of-2 alignment
        CAMX_ASSERT(IsPowerOfTwo(alignment));

        return ((operand + (alignment - 1)) & ~(static_cast<UINT64>(alignment) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlign32
    ///
    /// @brief  This function is used to align up any UINT32 value to a specific power of 2
    ///
    /// @param  operand     value to be aligned
    /// @param  alignment   desired alignment
    ///
    /// @return Value aligned as specified
    ///
    /// @remark Use The ByteAlignPtr() function for pointer calculations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 ByteAlign32(
        UINT32 operand,
        UINT   alignment)
    {
        // This function only works for a power-of-2 alignment
        CAMX_ASSERT(IsPowerOfTwo(alignment));

        return ((operand + (alignment - 1)) & ~(static_cast<UINT32>(alignment) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ByteAlignPtr
    ///
    /// @brief  This function is used to align a pointer to a specific power of 2 BYTE boundary
    ///
    /// @param  pMem        pointer to be aligned
    /// @param  alignBytes  desired alignment in bytes
    ///
    /// @return Pointer aligned as specified
    ///
    /// @remark Use The ByteAlign() function for non-pointer alignment
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID* ByteAlignPtr(
        VOID*  pMem,
        SIZE_T alignBytes)
    {
        // This function only works for a power-of-2 alignment
        CAMX_ASSERT(IsPowerOfTwo(alignBytes));

        return reinterpret_cast<VOID*>((reinterpret_cast<SIZE_T>(pMem) + (alignBytes - 1)) &
                                       ~(static_cast<SIZE_T>(alignBytes) - 1));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AlignGeneric32
    ///
    /// @brief  This function is used to align up any UINT32 value to any alignment value
    ///
    /// @param  operand     value to be aligned
    /// @param  alignment   desired alignment
    ///
    /// @return Value aligned as specified
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 AlignGeneric32(
        UINT32 operand,
        UINT   alignment)
    {
        UINT remainder = (operand % alignment);

        return (0 == remainder) ? operand : operand - remainder + alignment;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AlignNearGeneric32
    ///
    /// @brief  This function is used to align to near any UINT32 value to any alignment value
    ///
    /// @param  operand                  value to be aligned
    /// @param  midNextAlignemnt         at mid point align to next aligned value
    /// @param  alignment                desired alignment
    ///
    /// @return Value aligned as specified
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 AlignNearGeneric32(
        UINT32 operand,
        BOOL   midNextAlignemnt,
        UINT   alignment)
    {
        UINT remainder     = (operand % alignment);
        UINT32 returnvalue = operand - remainder;
        if ((remainder > (alignment / 2)) || ((remainder == (alignment / 2)) &&
            midNextAlignemnt))
        {
            returnvalue = operand - remainder + alignment;
        }
        return returnvalue;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Ceiling
    ///
    /// @brief  This function finds the integer ceiling of a FLOAT precision operand
    ///
    /// @param  operand value whose ceiling needs to be calculated
    ///
    /// @return Integer ceiling
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 Ceiling(
        FLOAT operand)
    {
        return static_cast<UINT32>(ceil(operand));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinBitsUINT32
    ///
    /// @brief  This function finds the minimum number of bits required to represent the operand value.
    ///
    /// @param  operand     UINT32 number
    ///
    /// @return Minimum number of bits required to represent the operand value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 MinBitsUINT32(
        UINT32 operand)
    {
        UINT32 value = 0;

        if (0 == operand)
        {
            return 1; // Zero needs at least one bit to be represented
        }

        while (1 <= operand)
        {
            value++;
            operand >>= 1;
        }

        return value;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitScanForward
    ///
    /// @brief  Search the mask data from least significant bit (LSB) to most significant bit (MSB) for a set bit (1)
    ///
    /// @param  number  The 32-bit value to search.
    /// @param  pIndex  Loaded with the bit position of the first set bit (1) found, set to 0 if no bits are set in number
    ///
    /// @return TRUE if found, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL BitScanForward(
        UINT32  number,
        UINT32* pIndex)
    {
        UINT32  mask = number;
        BOOL    result;

        if (mask != 0)
        {
            // get first 1 bit's position (from LSB) in mask
#if defined (_WIN32)
            // NOWHINE GR017: The native type uses unsigned long
            _BitScanForward(reinterpret_cast<unsigned long*>(pIndex), mask);
#elif defined(__GNUC__)
            *pIndex = __builtin_ctz(mask);      // Count Trailing Zeros
#else
            CAMX_STATIC_ASSERT2(FALSE, "Unsupported Target");
#endif // defined (_WIN32)/defined(__GNUC__)
            result = TRUE;
        }
        else
        {
            result = FALSE;
            /// @note The contents of pIndex when there are no set bits in mask are undefined according to the underlying
            ///      behavior of the instructions used to implement the intrinsic.  Explicitly reset the value to 0 and avoid
            ///      relying on compiler/platform dependent behavior.
            *pIndex = 0;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitScanForwardArray
    ///
    /// @brief  Search the mask data from least significant bit (LSB) to most significant bit (MSB) for a set bit (1)
    ///
    /// @param  pNumber     Ptr to the array to bitscan
    /// @param  arraySize   Number of array indexes (1 based)
    /// @param  pIndex      Loaded with the bit position of the first set bit (1) found, set to 0 if no bits are set in number
    ///
    /// @return TRUE if found, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL BitScanForwardArray(
        const UINT* pNumber,
        UINT        arraySize,
        UINT32*     pIndex)
    {
        BOOL result = FALSE;

        CAMX_ASSERT(arraySize != 0);

        for (UINT i = 0; i < arraySize; i++)
        {
            CAMX_ASSERT((pNumber != NULL) && (pIndex != NULL));

            if (TRUE == BitScanForward(pNumber[i], pIndex))
            {
                result = TRUE;
                // BitScan is zero based however we do need to multiply by the array index
                // to get bit > 31
                (*pIndex) += (i * 32);
                break;
            }
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitScanReverse
    ///
    /// @brief  Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1)
    ///
    /// @param  number      The 32-bit value to search
    /// @param  pIndex      Loaded with the bit position of the first set bit (1) found, set to 0 if no bits are set in number
    ///
    /// @return TRUE if found, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL BitScanReverse(
        UINT32  number,
        UINT32* pIndex)
    {
        UINT32  mask = number;
        BOOL    result;

        if (mask != 0)
        {
            // get first 1 bit's position (from MSB) in mask
#if defined (_WIN32)
            // NOWHINE GR017: The native type uses unsigned long
            _BitScanReverse(reinterpret_cast<unsigned long*>(pIndex), mask);
#elif defined(__GNUC__)
            *pIndex = 31 - __builtin_clz(mask);      // Count Leading Zeros
#else
            CAMX_STATIC_ASSERT2(FALSE, "Unsupported Target");
#endif // defined (_WIN32)/defined(__GNUC__)
            result = TRUE;
        }
        else
        {
            result = FALSE;
            /// @note The contents of pIndex when there are no set bits in mask are undefined according to the underlying
            ///      behavior of the instructions used to implement the intrinsic.  Explicitly reset the value to 0 and avoid
            ///      relying on compiler/platform dependent behavior.
            *pIndex = 0;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitScanReverseArray
    ///
    /// @brief  Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1)
    ///
    /// @param  pNumber     Ptr to the array to bitscan
    /// @param  arraySize   Number of array indexes (1 based)
    /// @param  pIndex      Loaded with the bit position of the first set bit (1) found, set to 0 if no bits are set in number
    ///
    /// @return TRUE if found, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL BitScanReverseArray(
        const UINT* pNumber,
        UINT        arraySize,
        UINT32*     pIndex)
    {
        BOOL result = FALSE;

        CAMX_ASSERT(arraySize != 0);

        for (INT i = (arraySize - 1); i >= 0; i--)
        {
            CAMX_ASSERT((pNumber != NULL) && (pIndex != NULL));

            if (TRUE == BitScanReverse(pNumber[i], pIndex))
            {
                result = TRUE;
                // BitScan is zero based however we do need to multiply by the array index
                // to get bit > 31
                (*pIndex) += (i * 32);
                break;
            }
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitReset
    ///
    /// @brief  Reset a particular bit to 0
    ///
    /// @param  number  Number whose bit needs to be set
    /// @param  bit     Bit number to set
    ///
    /// @return Resulting number
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 BitReset(
        UINT32  number,
        UINT32  bit)
    {
        static const UINT32 One = 1;     /// @note Forces type of literal constant without relying on suffix

        CAMX_ASSERT(bit < (sizeof(UINT32) * 8));

        return (number & ~(One << bit));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitSet
    ///
    /// @brief  Sets a particular bit to 1
    ///
    /// @param  number  Number whose bit needs to be set
    /// @param  bit     Bit number to set
    ///
    /// @return Resulting number
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 BitSet(
        UINT32  number,
        UINT32  bit)
    {
        static const UINT32 One = 1;     /// @note Forces type of literal constant without relying on suffix

        CAMX_ASSERT(bit < (sizeof(UINT32) * 8));

        return (number | (One << bit));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitMaskReset
    ///
    /// @brief  Resets all bits in number that are set in mask
    ///
    /// @param  number  Number whose bits need to be reset
    /// @param  mask    Bit mask
    ///
    /// @return Resulting number
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 BitMaskReset(
        UINT32  number,
        UINT32  mask)
    {
        return (number & ~mask);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// BitMaskSet
    ///
    /// @brief  Sets all bits in number that are set in mask
    ///
    /// @param  number  Number whose bits need to be set
    /// @param  mask    Bit mask
    ///
    /// @return Resulting number
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 BitMaskSet(
        UINT32  number,
        UINT32  mask)
    {
        return (number | mask);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsBitMaskReset
    ///
    /// @brief  Tests if all bits set in mask are reset in number
    ///
    /// @param  number  Number to test
    /// @param  mask    Bit mask
    ///
    /// @return TRUE if test passes
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL IsBitMaskReset(
        UINT32  number,
        UINT32  mask)
    {
        return ((number & mask) == 0) ? TRUE : FALSE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsBitMaskSet
    ///
    /// @brief  Tests if all bits set in mask are set in number
    ///
    /// @param  number  Number to test
    /// @param  mask    Bit mask
    ///
    /// @return TRUE if test passes
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL IsBitMaskSet(
        UINT32  number,
        UINT32  mask)
    {
        return ((number & mask) == mask) ? TRUE : FALSE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsBitReset
    ///
    /// @brief  Tests if a bit is reset in number
    ///
    /// @param  number  Number to test
    /// @param  bit     Bit number to test
    ///
    /// @return TRUE if test passes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL IsBitReset(
        UINT32  number,
        UINT32  bit)
    {
        static const UINT32 One = 1;     /// @note Forces type of literal constant without relying on suffix

        CAMX_ASSERT(bit < (sizeof(UINT32) * 8));

        return ((number & (One << bit)) == 0) ? TRUE : FALSE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsBitSet
    ///
    /// @brief  Tests if a bit is set in number
    ///
    /// @param  number  Number to test
    /// @param  bit     Bit number to test
    ///
    /// @return TRUE if test passes
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE BOOL IsBitSet(
        UINT32  number,
        UINT32  bit)
    {
        static const UINT32 One = 1;     /// @note Forces type of literal constant without relying on suffix

        CAMX_ASSERT(bit < (sizeof(UINT32) * 8));

        return ((number & (One << bit)) != 0) ? TRUE : FALSE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FLOATToSFixed32
    ///
    /// @brief  Convert a floating point number to a signed fixed point number with the given fractional bits
    ///
    /// @param  floatVal            Number to convert
    /// @param  numFractionalBits   Number of fractional bits
    /// @param  round               Whether to enable rounding
    ///
    /// @return Signed 32-bit fixed point number representation of the input FLOAT
    ///
    /// @note   This function does not saturate on overflow
    ///         This function does not attempt to handle NaN
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT32 CAMX_INLINE FLOATToSFixed32(
        FLOAT   floatVal,
        UINT32  numFractionalBits,
        BOOL    round)
    {
        if (0 != numFractionalBits)
        {
            // Convert to integer scale
            floatVal *= (1u << numFractionalBits);
        }

        if (TRUE == round)
        {
            // Round before conversion
            floatVal += (0 < floatVal) ? 0.5f : -0.5f;
        }

        // Convert to fixed point
        return static_cast<INT32>(floatVal);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SFixed32ToFLOAT
    ///
    /// @brief  Convert a signed fixed point number to a floating point number with the given fractional bits
    ///
    /// @param  fixedPointNum       Number to convert
    /// @param  numFractionalBits   Number of fractional bits
    ///
    /// @return Floating point number representation of the input signed integer
    ///
    /// @note   This function does not saturate on overflow
    ///         This function does not attempt to handle NaN
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static FLOAT CAMX_INLINE SFixed32ToFLOAT(
        INT32   fixedPointNum,
        UINT32  numFractionalBits)
    {
        UINT32  integerValue;
        UINT32  fractionalBits;
        FLOAT   fractionalValue;
        UINT32  factor = (1u << numFractionalBits);

        // Calculate the integer portion of the result
        integerValue = (fixedPointNum >> numFractionalBits);
        // Get the bits representing the fractional portion
        fractionalBits = fixedPointNum & AllOnes32(numFractionalBits);
        // Calculate the fractional portion of the result
        fractionalValue = static_cast<FLOAT>(fractionalBits) / factor;

        return (integerValue + fractionalValue);
    }

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
    static FLOAT CAMX_INLINE MaxFLOAT(
        FLOAT value1,
        FLOAT value2)
    {
        return ((value1 > value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinFLOAT
    ///
    /// @brief  Returns the smaller of the two input values.
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return The smaller of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static FLOAT CAMX_INLINE MinFLOAT(
        FLOAT value1,
        FLOAT value2)
    {
        return ((value1 < value2) ? value1 : value2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT32
    ///
    /// @brief  Returns the larger of two UINT32 values
    ///
    /// @param  input1 The first number to compare
    /// @param  input2 The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT32 CAMX_INLINE MaxUINT32(
        UINT32 input1,
        UINT32 input2)
    {
        return ((input1 > input2) ? input1 : input2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT16
    ///
    /// @brief  Returns the larger of two UINT16 values
    ///
    /// @param  input1 The first number to compare
    /// @param  input2 The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT16 CAMX_INLINE MaxUINT16(
        UINT16 input1,
        UINT16 input2)
    {
        return ((input1 > input2) ? input1 : input2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT16
    ///
    /// @brief  Returns the smaller of two UINT16 values
    ///
    /// @param  input1 The first number to compare
    /// @param  input2 The second number to compare
    ///
    /// @return The smaller of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT16 CAMX_INLINE MinUINT16(
        UINT16 input1,
        UINT16 input2)
    {
        return ((input1 < input2) ? input1 : input2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FEqualCoarse
    ///
    /// @brief  Compares the floating point numbers with reduced precision
    ///
    /// @param  value1  The first number to compare
    /// @param  value2  The second number to compare
    ///
    /// @return BOOL TRUE if equal FALSE if not equal
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  CAMX_INLINE BOOL FEqualCoarse(
        FLOAT value1, FLOAT value2)
    {
        return ((fabs(value1 - value2) < 1e-2) ? (TRUE) : (FALSE));
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT32
    ///
    /// @brief  Returns the smaller of two UINT32 values
    ///
    /// @param  input1 The first number to compare
    /// @param  input2 The second number to compare
    ///
    /// @return The smaller of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT32 CAMX_INLINE MinUINT32(
        UINT32 input1,
        UINT32 input2)
    {
        return ((input1 < input2) ? input1 : input2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MinUINT64
    ///
    /// @brief  Returns the smaller of two UINT64 values
    ///
    /// @param  input1 The first number to compare
    /// @param  input2 The second number to compare
    ///
    /// @return The smaller of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT64 CAMX_INLINE MinUINT64(
        UINT64 input1,
        UINT64 input2)
    {
        return ((input1 < input2) ? input1 : input2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MaxUINT64
    ///
    /// @brief  Returns the larger of two UINT64 values
    ///
    /// @param  input1 The first number to compare
    /// @param  input2 The second number to compare
    ///
    /// @return The larger of the two input values
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT64 CAMX_INLINE MaxUINT64(
        UINT64 input1,
        UINT64 input2)
    {
        return ((input1 > input2) ? input1 : input2);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TwosComplementValidBits
    ///
    /// @brief  Get the 2's complement of the number with respect to valid bits.
    ///
    /// @param  value     value to be adjusted
    /// @param  validBits number of destination bits
    ///
    /// @return INT32     Adjusted value of valid bits
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE INT32 TwosComplementValidBits(
        INT32 value,
        UINT8 validBits)
    {
        if (value < 0)
        {
            value = -(value);
            value &= ((1 << validBits) - 1);
            value = (1 << validBits) - value;
        }
        else
        {
            value &= ((1 << validBits) - 1);
        }
        return value;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Memory Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// VoidPtrInc
    ///
    /// @brief  Utility function to add the specified byte offset to a void pointer
    ///
    /// @param  pVoid       Pointer to be incremented
    /// @param  numBytes    Number of bytes to increase the pointer
    ///
    /// @return New pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID* VoidPtrInc(
        VOID*    pVoid,
        SIZE_T   numBytes)
    {
        return (static_cast<BYTE*>(pVoid) + numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ConstVoidPtrInc
    ///
    /// @brief  Utility function to add the specified byte offset to a void pointer (and deal with const-ness)
    ///
    /// @param  pVoid       Pointer to be incremented
    /// @param  numBytes    Number of bytes to increase the pointer
    ///
    /// @return New pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE const VOID* ConstVoidPtrInc(
        const VOID*  pVoid,
        SIZE_T       numBytes)
    {
        return (static_cast<const BYTE*>(pVoid) + numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// VoidPtrDec
    ///
    /// @brief  Utility function to subtract the specified byte offset from a void pointer
    ///
    /// @param  pVoid       Pointer to be decremented
    /// @param  numBytes    Number of bytes to decrease the pointer
    ///
    /// @return New pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID* VoidPtrDec(
        VOID*    pVoid,
        SIZE_T   numBytes)
    {
        return (static_cast<BYTE*>(pVoid) - numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ConstVoidPtrDec
    ///
    /// @brief  Utility function to subtract the specified byte offset from a void pointer (and deal with const-ness)
    ///
    /// @param  pVoid       Pointer to be decremented
    /// @param  numBytes    Number of bytes to decrease the pointer
    ///
    /// @return New pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE const VOID* ConstVoidPtrDec(
        const VOID*  pVoid,
        SIZE_T       numBytes)
    {
        return (static_cast<const BYTE*>(pVoid) - numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// VoidPtrToUINT64
    ///
    /// @brief  Utility function to store a void pointer in a UINT64 value, independent of actual pointer size
    ///
    /// @param  pVoid       Pointer to be stored
    ///
    /// @return UINT64 representation of the pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT64 VoidPtrToUINT64(
        VOID* pVoid)
    {
        return static_cast<UINT64>(reinterpret_cast<SIZE_T>(pVoid));
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
    static CAMX_INLINE INT Memcmp(
        const VOID*  pSrc1,
        const VOID*  pSrc2,
        SIZE_T       numBytes)
    {
        CAMX_ASSERT(pSrc1 != NULL);
        CAMX_ASSERT(pSrc2 != NULL);

        return memcmp(pSrc1, pSrc2, numBytes);
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
    static CAMX_INLINE VOID* Memcpy(
        VOID*       pDst,
        const VOID* pSrc,
        SIZE_T      numBytes)
    {
        CAMX_ASSERT(pDst != NULL);
        CAMX_ASSERT(pSrc != NULL);
        CAMX_ASSERT(static_cast<INT32>(numBytes) > 0);

        return memcpy(pDst, pSrc, numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Memmove
    ///
    /// @brief  Wraps the standard Memmove function; moves "numBytes" from source to destination
    ///
    /// @param  pDst       pointer to destination of move
    /// @param  pSrc       pointer to source of move
    /// @param  numBytes   size of data pointed at by pSrc
    ///
    /// @return Dest Pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID* Memmove(
        VOID*       pDst,
        const VOID* pSrc,
        SIZE_T      numBytes)
    {
        CAMX_ASSERT(pDst != NULL);
        CAMX_ASSERT(pSrc != NULL);

        return memmove(pDst, pSrc, numBytes);
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
    static CAMX_INLINE VOID* Memset(
        VOID*    pDst,
        INT      setVal,
        SIZE_T   numBytes)
    {
        CAMX_ASSERT(pDst != NULL);
        return memset(pDst, setVal, numBytes);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AllOnes32
    ///
    /// @brief  Returns a bitmask with all 'numberOfBits' LSB bits set
    ///
    /// @param  numberOfBits  Number of LSB bits to set in the bit-mask
    ///
    /// @return 32 bit bitmask of 1's with all 'numberOfBits' LSB bits set
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 AllOnes32(
        UINT32 numberOfBits)
    {
        CAMX_ASSERT(numberOfBits <= 32);

        return (1 << numberOfBits) - 1;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AllOnes32LeftShift
    ///
    /// @brief  Left shift the result of AllOnes32
    ///
    /// @param  numberOfBits    Number of LSB bits to set in the mask before shifting the mask to the left
    /// @param  leftShiftBits   The number of bits to shift left
    ///
    /// @return the result of AllOnes32 left-shifted by leftShiftBits
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT32 AllOnes32LeftShift(
        UINT32 numberOfBits,
        UINT32 leftShiftBits)
    {
        CAMX_ASSERT(numberOfBits <= 32);
        CAMX_ASSERT(leftShiftBits <= 32);
        return ((1 << numberOfBits) - 1) << leftShiftBits;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AllOnes64
    ///
    /// @brief  Returns a bitmask with all 'numberOfBits' LSB bits set
    ///
    /// @param  numberOfBits  Number of LSB bits to set in the bit-mask
    ///
    /// @return 64 bit bitmask of 1's with all 'numberOfBits' LSB bits set
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT64 AllOnes64(
        UINT32 numberOfBits)
    {
        CAMX_ASSERT(numberOfBits <= 64);

        return (1ULL << numberOfBits) - 1ULL;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AllOnes64LeftShift
    ///
    /// @brief  Left shift the result of AllOnes64
    ///
    /// @param  numberOfBits    Number of LSB bits to set in the mask before shifting the mask to the left
    /// @param  leftShiftBits   The number of bits to shift left
    ///
    /// @return the result of AllOnes64 left-shifted by leftShiftBits
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE UINT64 AllOnes64LeftShift(
        UINT32 numberOfBits,
        UINT32 leftShiftBits)
    {
        CAMX_ASSERT(numberOfBits <= 32);
        CAMX_ASSERT(leftShiftBits <= 32);
        return ((1ULL << numberOfBits) - 1ULL) << leftShiftBits;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Srand
    ///
    /// @brief  Initialize pseudo-random number generator. To generate different sequences of results by calling rand(), a
    ///         different seed should be used, such as time value.
    ///
    /// @param  seed    Used to initialize the random number generator to allow distinctive sequences of results from rand().
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID Srand(
        UINT seed)
    {
        return srand(seed);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Rand
    ///
    /// @brief  Generate a random floating point number between 0 and 1.
    ///
    /// @return FLOAT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE FLOAT Rand()
    {
        return ((FLOAT) rand()) / RAND_MAX;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Qsort
    ///
    /// @brief  Wraps the standard qsort function; sorts "num" elements of "base" using function "compar"
    ///
    /// @param  base   Pointer to the first object of the array to be sorted, converted to a void*.
    /// @param  num    Number of elements in the array pointed to by base.
    /// @param  size   Size in bytes of each element in the array.
    /// @param  compare Pointer to a function that compares two elements.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE VOID Qsort(
        VOID*  pBase,
        SIZE_T num,
        SIZE_T size,
        INT    (*pCompare)(const VOID*, const VOID*))
    {
        CAMX_ASSERT(NULL != pBase);
        CAMX_ASSERT(NULL != pCompare);

        qsort(pBase, num, size, pCompare);
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static  CAMX_INLINE BOOL FEqual(
        FLOAT value1, FLOAT value2)
    {
        return ((fabs(value1 - value2) < 1e-9) ? (TRUE) : (FALSE));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CalculateShiftBits
    ///
    /// @brief  Calculate the shift bits required to produce output of specified depth.
    ///
    /// @param  pixels          Number of pixels to calculate shift bits for
    /// @param  inputDepth      Bit depth of input
    /// @param  outputDepth     Bit depth of output
    ///
    /// @return shift bits required to get output of specified bit depth
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT32 CalculateShiftBits(
        UINT32  pixels,
        UINT32  inputDepth,
        UINT32  outputDepth)
    {
        UINT32  shiftBits           = 0;
        UINT32  numberOfBits        = 0;
        INT32   bithDepthDifference = outputDepth - inputDepth;

        numberOfBits = MinBitsUINT32(pixels);
        if (static_cast<INT32>(numberOfBits) > bithDepthDifference)
        {
            shiftBits = numberOfBits - bithDepthDifference;
        }
        else
        {
            shiftBits = 0;
        }

        return shiftBits;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FloatToQNumber
    ///
    /// @brief  Converts Float value to Q Format
    ///
    /// @param  value   Float Value , for which Needs to converted to Q format
    /// @param  QNumber constant value to be used for conversion of float to Q
    ///
    /// @return INT32 The converted value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT32 FloatToQNumber(
        FLOAT value,
        INT QNumber)
    {
        return  RoundFLOAT((value * static_cast<FLOAT>(QNumber)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SignExtendQnumber
    ///
    /// @brief  Sign extends Q number
    ///
    /// @param  value     int Value , Q number value which needs sign extension
    /// @param  validBits valid bits in value
    ///
    /// @return INT  sign extended Qnumber
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT SignExtendQnumber(
        INT value,
        INT validBits)
    {
        INT ret = value;
        if (value & (1 << (validBits - 1)))
        {
            ret = (((INT) ~ 0) ^ ((1 << validBits) - 1)) | value;
        }
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DoubleToQNumber
    ///
    /// @brief  Converts Double value to Q Format
    ///
    /// @param  value   Double Value , for which Needs to converted to Q format
    /// @param  QNumber constant value to be used for conversion of double to Q
    ///
    /// @return INT32 The converted value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static __inline INT32 DoubleToQNumber(
        DOUBLE value,
        INT QNumber)
    {
        return static_cast<INT32>(RoundDOUBLE((value * static_cast<DOUBLE>(QNumber))));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FloatToHalf
    ///
    /// @brief  Converts Float value to half float
    ///
    /// @param  f      Float Value to be converted to half float
    ///
    /// @return UINT16 The converted value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT16 FloatToHalf(
        FLOAT f)
    {
        // For IEEE-754 float representation, the sign is in bit 31, biased exponent is in bits 23-30 and mantissa is in bits
        // 0-22. Using hex representation, extract sign, biased exponent and mantissa.
        UINT32 hex_32             = *(reinterpret_cast<UINT32*>(&f));
        UINT32 sign_32            = hex_32 & (0x80000000);
        UINT32 biased_exponent_32 = hex_32 & (0x7F800000);
        UINT32 mantissa_32        = hex_32 & (0x007FFFFF);

        // special case: 0 or denorm
        if (biased_exponent_32 == 0)
        {
            return 0;
        }

        // The exponent is stored in the range 1 .. 254 (0 and 255 have special meanings), and is biased by subtracting 127
        // to get an exponent value in the range -126 .. +127.
        // remove exp bias, adjust mantissa
        INT32 exponent_32 = (static_cast<INT32>(hex_32 >> 23)) - 127;
        mantissa_32 = ((mantissa_32 >> (23 - 10 - 1)) + 1) >> 1;    // shift with rounding to yield 10 MSBs

        if (mantissa_32 & 0x00000400)
        {
            mantissa_32 >>= 1;  // rounding resulted in overflow, so adjust mantissa and exponent
            exponent_32++;
        }

        // Can only support 4 bits for exponent with half
        CAMX_ASSERT(exponent_32 <= 15 && exponent_32 >= -14);

        // compose
        UINT16 sign_16            = static_cast<UINT16>(sign_32 >> 16);
        UINT16 biased_exponent_16 = static_cast<UINT16>((exponent_32 + 15) << 10);
        UINT16 mantissa_16        = static_cast<UINT16>(mantissa_32);

        return (sign_16 | biased_exponent_16 | mantissa_16);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MatchAspectRatio
    ///
    /// @brief  Matches output dimensions aspect ratio to reference dimensions aspect ratio
    ///
    /// @param  pReferenceDimension     Pointer to reference dimension
    /// @param  pUpdateDimension        Pointer to dimension that needs to be updated
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID MatchAspectRatio(
        const CamxDimension* pReferenceDimension,
        CamxDimension*       pUpdateDimension)
    {
        FLOAT referenceAspectRatio = 0.0f;
        FLOAT currentAspectRatio   = 0.0f;

        CAMX_ASSERT(pReferenceDimension != NULL);
        CAMX_ASSERT(pUpdateDimension != NULL);

        referenceAspectRatio = static_cast<FLOAT>(pReferenceDimension->width) / static_cast<FLOAT>(pReferenceDimension->height);
        currentAspectRatio   = static_cast<FLOAT>(pUpdateDimension->width) / static_cast<FLOAT>(pUpdateDimension->height);

        if (currentAspectRatio < referenceAspectRatio)
        {
            pUpdateDimension->height = static_cast<UINT32>(static_cast<FLOAT>(pUpdateDimension->width) / referenceAspectRatio);
        }
        else if (currentAspectRatio > referenceAspectRatio)
        {
            pUpdateDimension->width = static_cast<UINT32>(static_cast<FLOAT>(pUpdateDimension->height) * referenceAspectRatio);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CalculateLCM
    ///
    /// @brief  Calculate the Least Common Multiple of the two input data
    ///
    /// @param  value0  First value to compare for LCM
    /// @param  value1  Second value to compare for LCM
    ///
    /// @return INT32 The Least Common Multiple
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT32 CalculateLCM(
        INT32 value0,
        INT32 value1)
    {
        INT32 leastCommonMultiple = 0;
        BOOL  foundLCM            = FALSE;

        value0 = Utils::AbsoluteINT32(value0);
        value1 = Utils::AbsoluteINT32(value1);

        if ((0 != value0) && (0 != value1))
        {

            if (value0 != value1)
            {
                leastCommonMultiple = Utils::MaxUINT32(value0, value1);

                while (FALSE == foundLCM)
                {
                    if ((0 == leastCommonMultiple % value0) && (0 == leastCommonMultiple % value1))
                    {
                        foundLCM = TRUE;
                    }
                    leastCommonMultiple++;
                }
            }
            else
            {
                leastCommonMultiple = value0;
            }
        }
        else if (0 == value0)
        {
            leastCommonMultiple = value1;
        }
        else
        {
            leastCommonMultiple = value0;
        }

        return leastCommonMultiple;
    }

private:
    Utils(const Utils&) = delete;               ///< Disallow the copy constructor
    Utils& operator=(const Utils&) = delete;    ///< Disallow assignment operator
    Utils() = delete;                           ///< Disallow constructor, this class contains only static functions
    ~Utils() = delete;                          ///< Disallow destructor, this class contains only static functions
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable CountDownLatch class implementation
///
/// A synchronization util tool that allow one thread to wait for all others threads to complete an activity.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CountDownLatch final
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of CountDownLatch
    ///
    /// @param  pResourceName pointer to description of resource being conditioned
    /// @param  value         initialize the count down value
    ///
    /// @return Pointer to newly created object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static CAMX_INLINE CountDownLatch* Create(
        const CHAR* pResourceName,
        UINT        value)
    {
        CountDownLatch* pCountDownLatch = NULL;

        pCountDownLatch = CAMX_NEW CountDownLatch();

        if (NULL != pCountDownLatch)
        {
            if (CamxResultSuccess != pCountDownLatch->Initialize(pResourceName, value))
            {
                CAMX_DELETE pCountDownLatch;
                pCountDownLatch = NULL;
            }
        }

        return pCountDownLatch;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of CountDownLatch
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID Destroy()
    {
        if (NULL != m_pMutex)
        {
            m_pMutex->Destroy();
            m_pMutex = NULL;
        }
        if (NULL != m_pCondition)
        {
            m_pCondition->Destroy();
            m_pCondition = NULL;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Await
    ///
    /// @brief  Indefinitely wait on count down untils it reaches zero.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID Await()
    {
        CAMX_LOG_WARN(CamxLogGroupUtils, "Await() should be used for debugging only. Use TimedAwait() instead.");
        (void) TimedAwait(InfiniteWaitTime);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TimedAwait
    ///
    /// @brief  Wait with timeout on count down reach zero
    ///
    /// @param  timeoutMilliseconds Timeout in milliseconds
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE CamxResult TimedAwait(
        UINT timeoutMilliseconds)
    {
        CamxResult result = CamxResultSuccess;
        m_pMutex->Lock();
        if (0 != m_counter)
        {
            result = m_pCondition->TimedWait(m_pMutex->GetNativeHandle(), timeoutMilliseconds);
        }
        m_pMutex->Unlock();
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CountDown
    ///
    /// @brief  decrement counter when it reach zero, signal the wait condition
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID CountDown()
    {
        m_pMutex->Lock();
        if (m_counter > 0)
        {
            m_counter--;
        }
        if (0 == m_counter)
        {
            m_pCondition->Signal();
        }
        m_pMutex->Unlock();
    }

private:

    /// @brief Constructor for CountDownLatch object.
    CountDownLatch() = default;

    /// @brief Destructor for CountDownLatch object.
    ~CountDownLatch() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Count down latch
    ///
    /// @param  pResourceName pointer to description of resource being conditioned
    /// @param  value         counter value
    ///
    /// @return Success if successful, a failure code otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE CamxResult Initialize(
        const CHAR* pResourceName,
        UINT        value)
    {
        CamxResult result = CamxResultSuccess;

        m_pMutex     = Mutex::Create(pResourceName);
        m_pCondition = Condition::Create(pResourceName);

        if ((NULL != m_pMutex) && (NULL != m_pCondition))
        {
            m_counter = value;
        }
        else
        {
            result = CamxResultENoMemory;
        }

        return result;
    }

    static const UINT InfiniteWaitTime = 0xFFFFFFFF;               ///< Static value for infinite wait

    CountDownLatch(const CountDownLatch&) = delete;                ///< Disallow the copy constructor
    CountDownLatch& operator=(const CountDownLatch&) = delete;     ///< Disallow assignment operator

    Mutex*       m_pMutex;                                         ///< Mutex lock
    Condition*   m_pCondition;                                     ///< Condition
    UINT         m_counter;                                        ///< Counter
};


CAMX_NAMESPACE_END

#endif // CAMXUTILS_H
