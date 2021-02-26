/**
 * @file DwcPlatform.h
 *
 * @ingroup CommonLayer
 *
 * @brief Device mode platform specific header file.
 *
 * This file has any platform specific inclusions, definitions and macros for the
 * Synopsys USB 3.0 controller to allow the remainder of the common layer to be
 * unchanged between the UEFI and Windows driver implementations.
 *
 * @copyright Copyright (C) 2013-2014, 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17   pm      Port from Boot.xf.1.4
 06/12/14   ck      Address pending setup pkt and End Transfer issues 
 04/30/14   ar      Cleanup for NDEBUG build  
 08/23/13   mliss   Initial import
 =============================================================================*/

#ifndef _DWC_PLATFORM_H_
#define _DWC_PLATFORM_H_

/*
 * Platform specific includes
 */
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

/*
 * Define things from DwcCommon.h here to avoid including that file.
 */
typedef struct _DWC_TRB     DWC_TRB;
typedef struct _DWC_URB     DWC_URB;
typedef struct _DWC_DEVICE  DWC_DEVICE;
typedef INT32               DWC_STATUS;


/*
 * Platform specific register access macros
 */
#define DWC_PLAT_READ_REG(addr)                       MmioRead32((addr))
#define DWC_PLAT_WRITE_REG(addr,val)                  MmioWrite32((addr),(val))


#define DWC_POLL_WAIT_INTERVAL                        (10)
#define DWC_PLAT_DELAY(usec)                          (gBS->Stall(usec));
#define DWC_SGLIST_SIZE                               (2)

/*
 * Memory management macros
 */
#define DWC_ALLOC(Size)      \
  AllocatePool (Size)

#define DWC_FREE(Ptr)        \
  {                          \
    FreePool (Ptr);          \
    (Ptr) = NULL;            \
  }


/*
 * Scatter gather list structures defined to match those of Windows
 */
typedef union _DWC_PHYSICAL_ADDRESS {
  struct {
    UINT32                  LowPart;
    UINT32                  HighPart;
  } u;
  UINT64                    QuadPart;
} DWC_PHYSICAL_ADDRESS;

typedef struct _SCATTER_GATHER_ELEMENT {
  DWC_PHYSICAL_ADDRESS      Address;
  UINTN                     Length;
  UINTN                     *Reserved;
} SCATTER_GATHER_ELEMENT;

typedef struct _SCATTER_GATHER_LIST {
  UINTN                     NumberOfElements;
  UINTN                     *Reserved;
  SCATTER_GATHER_ELEMENT    Elements[DWC_SGLIST_SIZE];
} SCATTER_GATHER_LIST;


/*
 * Logging of TRB/data buffer info, possibly implemented
 * on ETW in Windows
 */
#define DWC_LOG_BUF_DUMP_LENGTH               64

/**
 * @brief Wait for register bit
 *
 * Wait for a specific bit in a hardware register to reach a state (set/clear).
 *
 * @param [in] Address      Address of register
 * @param [in] Bit          Mask specifying bit to wait on
 * @param [in] WaitToSet    Value of bit user is waiting for (0/1)
 * @param [in] Timeout      Timeout for waiting (in microseconds)
 *
 * @return DWC_SUCCESS      Bit has arrived at desired value
 * @return DWC_TIMEOUT      Timed out waiting for bit to change
 */
DWC_STATUS
DwcWaitRegBit (
  IN UINT32                 Address,
  IN UINT32                 Bit,
  IN BOOLEAN                WaitToSet,
  IN UINT32                 Timeout
  );


/**
 * @brief Dump a TRB
 *
 * Dump all the TRB fields and the start of the buffer pointed to
 * by the TRB, if any.
 *
 * @param [in] Trb          TRB to dump
 * @param [in] TotalLen     Length of buffer in TRB
 */
VOID
DwcDumpTrb (
  IN DWC_TRB                *Trb,
  IN UINT32                 TotalLen
  );


/**
 * @brief Dump a device structure's data
 *
 * Dump all data within, and pointed to by, a device structure for
 * debugging purposes.
 *
 * @param [in] Dev          Device to dump
 */
VOID
DwcDumpDevice (
  IN DWC_DEVICE             *Dev
  );


/**
 * @brief Log data from an Urb
 *
 * If "info" logging is enabled dump realevant information from the Urb, TRBs
 * from the transfer, and data buffers pointed to by the TRBs.
 *
 * @param [in] Urb          Completed Urb to log
 */
VOID
DwcLogUrbComplete (
  IN DWC_URB                *Urb
  );


/*
 * WPP logging compatibility layer for UEFI
 */

// debug strings to replace WPP_DEFINE_BIT definitions
#define CommonInit          "Init"
#define CommonCommand       "Cmd"
#define CommonEvent         "Evt"
#define CommonTrb           "Trb"
#define CommonDebug         "Dbg"

// macros to replace WPP trace functions
#define _DWCDEBUG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define TraceCrit(flags, msg, ...)   \
  _DWCDEBUG (EFI_D_ERROR, "%a:%a: " msg "\n", __FUNCTION__, flags, ##__VA_ARGS__);

#define TraceErr(flags, msg, ...)    \
  _DWCDEBUG (EFI_D_ERROR, "%a:%a: " msg "\n", __FUNCTION__, flags, ##__VA_ARGS__);

#define TraceWarn(flags, msg, ...)   \
  _DWCDEBUG (EFI_D_WARN, "%a:%a: " msg "\n", __FUNCTION__, flags, ##__VA_ARGS__);

#define TraceInfo(flags, msg, ...)   \
  _DWCDEBUG (EFI_D_INFO, "%a:%a: " msg "\n", __FUNCTION__, flags, ##__VA_ARGS__);

#define TraceVerb(flags, msg, ...)   \
  _DWCDEBUG (EFI_D_VERBOSE, "%a:%a: " msg "\n", __FUNCTION__, flags, ##__VA_ARGS__);


#endif /* _DWC_PLATFORM_H_ */

