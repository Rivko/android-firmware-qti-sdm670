/*============================================================================
@file ULogFront.h

Frontend interfaces for ULog Core.  Simplified access to the logging mechanism.

Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef ULOGFRONT_H
#define ULOGFRONT_H

#ifndef TARGET_UEFI
	#define TARGET_UEFI
#endif	

#include "ULog.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_RAW
 * -------------------------------------------------------------------------*/

/**
 * <!-- ULogFront_RawInit -->
 *
 * @brief Create a new log in the RAW format.
 * 
 * Create a new raw log, See ULogCore_LogCreate for details.
 * 
 * @param h : See ULogCore_LogCreate.
 * @param name : See ULogCore_LogCreate.
 * @param logBufSize : See ULogCore_LogCreate.
 * @param logBufMemType : See ULogCore_LogCreate.
 * @param logLockType : See ULogCore_LogCreate.
 *
 * @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RawInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType);

/**
 * <!-- ULogFront_RawLog -->
 *
 * @brief Log data to a raw log.
 *
 * @param h  : A ULog handle.
 * @param dataArray : A pointer to the data to log.
 * @param dataCount : The amount of data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RawLog(ULogHandle h, 
                            const char * dataArray,
                            uint32 dataCount);

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_REALTIME
 * -------------------------------------------------------------------------*/

/** The value for x MUST be a 4 byte "float" variable.  It cannot be immediate data or an 8 byte "double" */
#define ULOG_RT_FLOAT(x) (*((uint32 *)((void *)&x)))

/** Macros for splitting up a 64 bit value */
#define ULOG64_LOWWORD(x) (uint32)x
#define ULOG64_HIGHWORD(x) (uint32)((uint64)x>>32)
#define ULOG64_DATA(x) (uint32)x, (uint32)((uint64)x>>32)


/**
 * <!-- ULogFront_RealTimeInit -->
 *
 * @brief Create a new log in the RealTime format.
 *
 * Create a new RealTime log, See ULogCore_LogCreate for details.
 *
 * @param h : A ULog handle is returned at this pointer. 
 * @param name : The log name (24 chars max, including NULL).
 * @param logBufSize : The size in bytes of the log buffer. A request of size 0 
 *                     will leave the buffer unallocated. 
 * @param logBufMemType : The memory configuration. Use ULOG_MEMORY_LOCAL.
 * @param logLockType : The type of locking required.  ULOG_LOCK_OS or ULOG_LOCK_NONE
 *
 * @return DAL_SUCCESS if the create completed successfully.  The log will have been
 *        enabled and will be ready to use. 
 *        ULOG_ERR_INITINCOMPLETE if ULOG_MEMORY_LOCAL_MANUAL is used.  
 *        ULOG_ERR_INITINCOMPLETE if a buffersize of 0 is used.  
 *        The log can be enabled later after ULogCore_MemoryAssign sets up all the parts.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType);

/**
 * <!-- ULogFront_RealTimeStaticInit -->
 *
 * @brief Create a new log in the RealTime Static format.
 *
 * Create a new RealTime log, See ULogCore_LogCreate for details.
 *
 * @param h : Log handle to be filled in.
 * @param logName : Ascii log name (must be less than 24 chars).
 * @param logMessageSlots : How many slots are needed for messages. Max is 9.
 * @param memoryType : Not used currently. Use ULOG_MEMORY_LOCAL. 
 * @param lockType : ULOG_LOCK_OS or ULOG_LOCK_NONE
 *
 * @return DAL_SUCCESS if the create completed successfully.
 */
ULogResult ULogFront_RealTimeStaticInit( ULogHandle * h,
                                         const char * name,
                                         uint32 logMessageSlots,
                                         uint32 logBufMemType,
                                         ULOG_LOCK_TYPE logLockType);


/**
 * <!-- ULogFront_RealTimePrintf -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function does the most work for ULog logging.  It's been optimized to 
 * write directly to the buffer if possible. 
 *
 * @param h : A ULog handle.
 * @param dataArray : The number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimePrintf( ULogHandle h,
                                     uint32 dataCount,
                                     const char * formatStr,
                                     ... );

/**
 * <!-- ULogFront_RealTimePrintf_Ex -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function takes an additional parameter that is a 
 * bitfield indicating whether each parameter passed in is 
 * either 32/64-bit. This additional parameter allows this call 
 * to log 64-bit data types without the ULOG64 macros
 *  
 * @param h : A ULog handle.
 * @param dataArray : The number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimePrintf_Ex( ULogHandle h,
                                        uint32 dataCount,
                                        uint32 paramBitField,
                                        const char * formatStr,
                                        ... ); 
/**
 * <!-- ULogResult ULogFront_RealTimeStaticPrintf -->
 *
 * Do not use this API.  Use the ULOG_RT_STATIC_PRINTF_n macros.
 */
ULogResult ULogFront_RealTimeStaticPrintf(ULogHandle h, uint32 msgSlot, uint32 dataCount, const char * formatStr, ...);


/**
 * <!-- ULOG_RT_PRINTF_N -->
 *
 * @brief Macros to provide some parameter count safety to the 
 *        ULog_RealTimePrintf_Ex function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly.
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These may be uint32 or 
 *           uint64 types, smaller types will be promoted to
 *           uint32
 * @param sizeof(p1)-sizeof(pN) : The size, in bytes, of the 
 *              parameter to log
 */
#define ULOG_RT_PRINTF_0(h, formatStr)    \
             ULogFront_RealTimePrintf_Ex(h, 0, 0, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)    \
             ULogFront_RealTimePrintf_Ex(h, 1, (uint32)(sizeof(p1)==8), formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1, p2)    \
             ULogFront_RealTimePrintf_Ex(h, 2, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)), formatStr, p1, p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)    \
             ULogFront_RealTimePrintf_Ex(h, 3, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)), formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)    \
             ULogFront_RealTimePrintf_Ex(h, 4, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)), formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimePrintf_Ex(h, 5, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)|((sizeof(p5)==8)<<4)), formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimePrintf_Ex(h, 6, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)|((sizeof(p5)==8)<<4)|((sizeof(p6)==8)<<5)), formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimePrintf_Ex(h, 7, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)|((sizeof(p5)==8)<<4)|((sizeof(p6)==8)<<5)|((sizeof(p7)==8)<<6)), formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimePrintf_Ex(h, 8, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)|((sizeof(p5)==8)<<4)|((sizeof(p6)==8)<<5)|((sizeof(p7)==8)<<6)|((sizeof(p8)==8)<<7)), formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimePrintf_Ex(h, 9, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)|((sizeof(p5)==8)<<4)|((sizeof(p6)==8)<<5)|((sizeof(p7)==8)<<6)|((sizeof(p8)==8)<<7)|((sizeof(p9)==8)<<8)), formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimePrintf_Ex(h, 10, (uint32)((sizeof(p1)==8)|((sizeof(p2)==8)<<1)|((sizeof(p3)==8)<<2)|((sizeof(p4)==8)<<3)|((sizeof(p5)==8)<<4)|((sizeof(p6)==8)<<5)|((sizeof(p7)==8)<<6)|((sizeof(p8)==8)<<7)|((sizeof(p9)==8)<<8)|((sizeof(p10)==8)<<9)), formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)


/**
 * <!-- ULOG_RT_PRINTF_N_CRITICAL -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly. 
 *  
 * For UEFI, these macros will be redefined as a WPP logging 
 * function with the level set to critical 
 *  
 * For NHLOS, the macro will behave the same as the 
 * ULOG_RT_PRINTF_N macros 
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0_CRITICAL(h, formatStr)    \
             ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1_CRITICAL(h, formatStr, p1)    \
             ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2_CRITICAL(h, formatStr, p1, p2)    \
             ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3_CRITICAL(h, formatStr, p1, p2, p3)    \
             ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4_CRITICAL(h, formatStr, p1, p2, p3, p4)    \
             ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5_CRITICAL(h, formatStr, p1, p2, p3, p4, p5)    \
             ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6_CRITICAL(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7_CRITICAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8_CRITICAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9_CRITICAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10_CRITICAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULOG_RT_PRINTF_N_FATAL -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly. 
 *  
 * For UEFI, these macros will be redefined as a WPP logging 
 * function with the level set to fatal 
 *  
 * For NHLOS, the macro will behave the same as the 
 * ULOG_RT_PRINTF_N macros  
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0_FATAL(h, formatStr)    \
             ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1_FATAL(h, formatStr, p1)    \
             ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2_FATAL(h, formatStr, p1, p2)    \
             ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3_FATAL(h, formatStr, p1, p2, p3)    \
             ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4_FATAL(h, formatStr, p1, p2, p3, p4)    \
             ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5_FATAL(h, formatStr, p1, p2, p3, p4, p5)    \
             ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6_FATAL(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7_FATAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8_FATAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9_FATAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10_FATAL(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULOG_RT_PRINTF_N_ERROR -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly. 
 *  
 * For UEFI, these macros will be redefined as a WPP logging 
 * function with the level set to error 
 *  
 * For NHLOS, the macro will behave the same as the 
 * ULOG_RT_PRINTF_N macros 
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0_ERROR(h, formatStr)    \
             ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1_ERROR(h, formatStr, p1)    \
             ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2_ERROR(h, formatStr, p1, p2)    \
             ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3_ERROR(h, formatStr, p1, p2, p3)    \
             ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4_ERROR(h, formatStr, p1, p2, p3, p4)    \
             ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5_ERROR(h, formatStr, p1, p2, p3, p4, p5)    \
             ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6_ERROR(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7_ERROR(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8_ERROR(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9_ERROR(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10_ERROR(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)


/**
 * <!-- ULOG_RT_PRINTF_N_WARNING -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly. 
 *  
 * For UEFI, these macros will be redefined as a WPP logging 
 * function with the level set to warning 
 *  
 * For NHLOS, the macro will behave the same as the 
 * ULOG_RT_PRINTF_N macros 
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0_WARNING(h, formatStr)    \
             ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1_WARNING(h, formatStr, p1)    \
             ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2_WARNING(h, formatStr, p1, p2)    \
             ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3_WARNING(h, formatStr, p1, p2, p3)    \
             ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4_WARNING(h, formatStr, p1, p2, p3, p4)    \
             ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5_WARNING(h, formatStr, p1, p2, p3, p4, p5)    \
             ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6_WARNING(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7_WARNING(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8_WARNING(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9_WARNING(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10_WARNING(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULOG_RT_PRINTF_N_INFORMATION -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly. 
 *  
 * For UEFI, these macros will be redefined as a WPP logging 
 * function with the level set to information 
 *  
 * For NHLOS, the macro will behave the same as the 
 * ULOG_RT_PRINTF_N macros 
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0_INFORMATION(h, formatStr)    \
             ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1_INFORMATION(h, formatStr, p1)    \
             ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2_INFORMATION(h, formatStr, p1, p2)    \
             ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3_INFORMATION(h, formatStr, p1, p2, p3)    \
             ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4_INFORMATION(h, formatStr, p1, p2, p3, p4)    \
             ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5_INFORMATION(h, formatStr, p1, p2, p3, p4, p5)    \
             ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6_INFORMATION(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7_INFORMATION(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8_INFORMATION(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9_INFORMATION(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10_INFORMATION(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)


/**
 * <!-- ULOG_RT_PRINTF_N_VERBOSE -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly. 
 *  
 * For UEFI, these macros will be redefined as a WPP logging 
 * function with the level set to verbose 
 *  
 * For NHLOS, the macro will behave the same as the 
 * ULOG_RT_PRINTF_N macros 
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0_VERBOSE(h, formatStr)    \
             ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1_VERBOSE(h, formatStr, p1)    \
             ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2_VERBOSE(h, formatStr, p1, p2)    \
             ULOG_RT_PRINTF_2(h, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3_VERBOSE(h, formatStr, p1, p2, p3)    \
             ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4_VERBOSE(h, formatStr, p1, p2, p3, p4)    \
             ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5_VERBOSE(h, formatStr, p1, p2, p3, p4, p5)    \
             ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6_VERBOSE(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7_VERBOSE(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8_VERBOSE(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9_VERBOSE(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10_VERBOSE(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)


/**
 * <!-- ULOG_RT_STATIC_PRINTF_N -->
 *
 * @brief Macros to write to Static ULogs. 
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimeStaticPrintf function.  
 *
 * @param h : A handle indicating where to log.
 * @param msgSlot : Which slot of the static log to write to. 
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_STATIC_PRINTF_0(h, msgSlot, formatStr) \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr,0,0,0,0,0,0,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_1(h, msgSlot, formatStr, p1)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1,0,0,0,0,0,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_2(h, msgSlot, formatStr, p1, p2)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2,0,0,0,0,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_3(h, msgSlot, formatStr, p1, p2, p3)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3,0,0,0,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_4(h, msgSlot, formatStr, p1, p2, p3, p4)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4,0,0,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_5(h, msgSlot, formatStr, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4, p5,0,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_6(h, msgSlot, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4, p5, p6,0,0,0,0)
#define ULOG_RT_STATIC_PRINTF_7(h, msgSlot, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4, p5, p6, p7,0,0,0)
#define ULOG_RT_STATIC_PRINTF_8(h, msgSlot, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4, p5, p6, p7, p8,0,0)
#define ULOG_RT_STATIC_PRINTF_9(h, msgSlot, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9,0)
#define ULOG_RT_STATIC_PRINTF_10(h, msgSlot, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimeStaticPrintf(h, msgSlot, 10, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULogFront_RealTimeVprintf -->
 * 
 * @brief Log data in the vprintf format.
 *
 * Log printf data to a RealTime log.  The printf formating is not
 * executed until the log is pulled.  This makes a very performant call,
 * but also means all strings must be present when the log is read.
 *
 * @note This function serves identical purpose to ULogFront_RealTimePrintf,
 *       except it takes a va_list argument instead of being variadic.
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ap : A va_list of the parameters being printed.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeVprintf(ULogHandle h, 
                                     uint32 dataCount, 
                                     const char * formatStr,
                                     va_list ap);

/**
 * <!-- ULogFront_RealTimeData -->
 *
 * @brief Log uint32 parameters.
 *
 * Log uint32 data to a RealTime log.  Data is provided as
 * variatic arguments to this function.
 *  
 * @param h: A ULog handle.
 * @param dataCount: the number of parameters being logged.  Can
 *                  be a maximum of 10.
 * @param ... : The parameters to log.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeData(ULogHandle h, uint32 dataCount, ...);


/**
 * <!-- ULOG_RT_DATA_N -->
 *
 * @brief Macros to provide some parameter count safety to the ULogFront_RealTimeData function.
 * 
 * These macros provide parameter count safety to the
 * ULogFront_RealTimeData function.  They can be used instead of
 * calling the function directly.
 *
 * @param h : A handle indicating where to log.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_DATA_1(h, p1)    \
             ULogFront_RealTimeData(h, 1, p1)
#define ULOG_RT_DATA_2(h, p1, p2)    \
             ULogFront_RealTimeData(h, 2, p1, p2)
#define ULOG_RT_DATA_3(h, p1, p2, p3)    \
             ULogFront_RealTimeData(h, 3, p1, p2, p3)
#define ULOG_RT_DATA_4(h, p1, p2, p3, p4)    \
             ULogFront_RealTimeData(h, 4, p1, p2, p3, p4)
#define ULOG_RT_DATA_5(h, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimeData(h, 5, p1, p2, p3, p4, p5)
#define ULOG_RT_DATA_6(h, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimeData(h, 6, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_DATA_7(h, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimeData(h, 7, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_DATA_8(h, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimeData(h, 8, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_DATA_9(h, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimeData(h, 9, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_DATA_10(h, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimeData(h, 10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULogFront_RealTimeCharArray -->
 *
 * @brief Log byte data to a RealTime log.  
 * 
 * @param h : A ULog handle.
 * @param byteCount : the number of bytes to log.
 * @param byteData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCharArray(ULogHandle h, 
                                       uint32 byteCount, 
                                       char * byteData);


/**
 * <!-- ULogFront_RealTimeString -->
 *
 * @brief Log string data to a RealTime log.
 *
 * This function copies the full string to the log.
 *
 * @param h : A ULog handle.
 * @param string : The string to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeString(ULogHandle h, char * cStr);
 

/**
 * <!-- ULogFront_RealTimeWordArray -->
 *
 * @brief Log word data to a RealTime log.
 *
 * @param h : A ULog handle.
 * @param wordCount : The number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeWordArray(ULogHandle h, 
                                       uint32 wordCount, 
                                       const uint32 * wordData);


/**
 * <!-- ULogFront_RealTimeCsv -->
 *
 * @brief Log word data to a RealTime log. Output will be a CSV compatible format.
 *
 * @param h  : A ULog handle.
 * @param wordCount : the number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCsv(ULogHandle h, 
                                 uint32 wordCount, 
                                 const uint32 * wordData);


/**
 * <!-- ULogFront_RealTimeVector -->
 *
 * @brief Log vector data to a RealTime log.
 *
 * Output will be a vector with the provided format. No newlines will be inserted
 *
 * @param h : A ULog handle.
 * @param formatStr : A format string with a single %x, %d, %i, or %u 
 * @param entryByteCount : 1, 2 or 4 byte size data 
 * @param vectorLength :  How man elements to store. 
 * @param vector : Pointer to the array of elements.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeVector(ULogHandle h, 
                                    const char * formatStr,
                                    unsigned short entryByteCount,
                                    unsigned short vectorLength,
                                    const void * vector);


/**
 * <!-- ULogFront_RealTimeMultipartMsgBegin -->
 *
 * @brief Begin a multipart message.
 *
 * The log will be locked for the duration of the multipart message.  To
 * free the lock, call ULogFront_RealTimeMultipartMsgEnd. The first message of a 
 * MPM is always a ULogFront_RealTimePrintf with %m specifiers for the parts to 
 * add in the subsequent logging before calling ULogFront_RealTimeMultipartMsgEnd.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 *
 * @return TRUE if the mpm was successfully started. False if it wasn't and
 *         the multipart message should not be continued further
 */
DALBOOL ULogFront_RealTimeMultipartMsgBegin (ULogHandle h);


/**
 * <!-- ULogFront_RealTimeMultipartMsgEnd -->
 *
 * @brief End a multipart message
 *
 * The log will be locked for the duration of the multipart message.  
 * This function frees the lock, ends the MPM message and completes the MPM bookkeeping.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 */
void ULogFront_RealTimeMultipartMsgEnd (ULogHandle h);

#ifdef TARGET_UEFI //sync with ULogFront_RealTimePrintf_Ex 
/**
 * <!-- ULogFront_RealTimeVprintf_Ex -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function does the most work for ULog logging.  It's been optimized to 
 * write directly to the buffer if possible. 
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ap : A va_list of the parameters being printed.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
  */
ULogResult ULogFront_RealTimeVprintf_Ex( ULogHandle h,
                                      uint32 dataCount,
                                      uint32 paramBitField,
                                      const char * formatStr,
                                      va_list ap );
#endif //TARGET_UEFI
#ifdef __cplusplus
}
#endif

#endif // ULOGFRONT_H
