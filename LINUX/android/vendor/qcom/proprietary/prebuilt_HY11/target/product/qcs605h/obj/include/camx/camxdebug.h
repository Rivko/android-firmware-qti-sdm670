////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxdebug.h
/// @brief Debug related defines
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXDEBUG_H
#define CAMXDEBUG_H

// Debug options globally controlled by the DEBUG symbol
#if DEBUG

#define CAMX_ASSERTS_ENABLED           1
#define CAMX_USE_MEMSPY                0
#define SETTINGS_DUMP_ENABLE           1
#define CAMX_DETECT_WRITE_TO_FREED_MEM 1 // see below, requires memspy to be set to 1

/// @todo (CAMX-560): Add setting to enable/disable Freed-Write detection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAMX_DETECT_WRITE_TO_FREED_MEM: Detect writes to freed allocations feature
//
// Useful for debugging writes to stale pointers.
//
// NOTE: This works by setting freed buffers to read-only in the page table instead of freeing them. There are a few
//       implications:
//
//       1) Catching a write to a freed portion of memory will manifest as an access violation.
//       2) This feature intentionally causes memory leaks by not freeing the allocations.
//       3) This feature allocates more memory since each allocation must be aligned to page(s).
//
//       This could be improved by adding windows support (linux only at the moment) and a garbage collector for
//       the leaked memory.
//
//       Requires USE_MEMSPY to be enabled.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // DEBUG

// NOWHINE GR017 : Using built in so we don't need any includes
typedef unsigned int CamxAssert;
static const CamxAssert CamxAssertConditional    = (1 << 0);    ///< A typical conditional assert, e.g. CAMX_ASSERT(val != 4);
static const CamxAssert CamxAssertAlways         = (1 << 1);    ///< CAMX_ASSERT_ALWAYS()
static const CamxAssert CamxAssertNotImplemented = (1 << 2);    ///< CAMX_NOT_IMPLEMENTED()
static const CamxAssert CamxAssertNotTested      = (1 << 3);    ///< CAMX_NOT_TESTED()


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Global for debug log settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CamxAssertTypeToString
///
/// @brief  Converts an AssertType into a human readable string
///
/// @param  assertType AssertType to convert to a string
///
/// @return Stringized AssertType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOWHINE GR017 : Using built in so we don't need any includes
const char* CamxAssertTypeToString(
    CamxAssert assertType);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CamxUpdateAssertMask
///
/// @brief  Updates assert behavior
///
/// @param  assertMask Mask of AssertTypes to throw asserts for
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CamxUpdateAssertMask(
    CamxAssert assertMask);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CamxFireAssert
///
/// @brief  Runtime asserts if enabled for this assert type
///
/// @param  assertType   Assert type
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CamxFireAssert(
    CamxAssert assertType);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CamxLogAssert
///
/// @brief Logs the asserts with the optional message
///
/// @param  assertType   Assert type
/// @param  pFileName    file name that triggered the assert
/// @oaram  line         line number that triggered the assert
/// @oaram  pFunction    function that triggered the assert
/// @oaram  pCondition   condition that triggered assert
/// @oaram  ...          arg list
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOWHINE GR017 : Using built in so we don't need any includes
void CamxLogAssert(unsigned int assertType,
    const char* pFileName, unsigned int line, const char* pFunction, const char* pCondition, ...); // NOWHINE GR017 <-

#if (CAMX_ASSERTS_ENABLED)
// Common function for logging asserts. Use the macros below instead
#define CAMX_ASSERT_EX(t, c, ...) CamxLogAssert((t), __FILE__, __LINE__, __FUNCTION__, (c), ##__VA_ARGS__)
#else // CAMX_ASSERTS_ENABLED
#define CAMX_ASSERT_EX(type, fmt, ...)      void(0)
#endif // CAMX_ASSERTS_ENABLED

// NOWHINE CP049: We're defining CAMX_STATIC_ASSERT here
#define CAMX_STATIC_ASSERT(condition)                    static_assert(condition, #condition)

// NOWHINE CP049: We're defining CAMX_STATIC_ASSERT here
#define CAMX_STATIC_ASSERT_MESSAGE(condition, message)   static_assert(condition, message)

#define CAMX_ASSERT(condition) do                                                                  \
                               {                                                                   \
                                   if (!(condition))                                               \
                                   {                                                               \
                                       CAMX_ASSERT_EX(CamxAssertConditional, #condition);          \
                                   }                                                               \
                               } while ((void)0, 0)                                                \

#define CAMX_ASSERT_MESSAGE(condition, fmt, ...) do                                                \
                                                 {                                                 \
                                                     if (!(condition))                             \
                                                     {                                             \
                                                        CAMX_ASSERT_EX(CamxAssertConditional,      \
                                                                       (fmt), ##__VA_ARGS__);      \
                                                      }                                            \
                                                 } while ((void)0, 0)                              \


#define CAMX_ASSERT_ALWAYS()                    CAMX_ASSERT_EX(CamxAssertAlways,          "ALWAYS")
#define CAMX_ASSERT_ALWAYS_MESSAGE(fmt, ...)    CAMX_ASSERT_EX(CamxAssertAlways,          fmt, ##__VA_ARGS__)
#define CAMX_NOT_IMPLEMENTED()                  CAMX_ASSERT_EX(CamxAssertNotImplemented,  "NOT IMPLEMENTED")
#define CAMX_NOT_TESTED()                       CAMX_ASSERT_EX(CamxAssertNotTested,       "NOT TESTED")

#endif // CAMXDEBUG_H
