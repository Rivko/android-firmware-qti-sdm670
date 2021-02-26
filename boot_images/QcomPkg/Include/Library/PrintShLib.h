/** @file PrintShLib.h
   
  Contains Print Shared Library Interface.

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/03/16   bh      Initial Version

=============================================================================*/
#ifndef __PRINTSH_LIB__H__
#define __PRINTSH_LIB__H__

#include "Uefi.h"
#include <Library/PrintLib.h>

/* Typedefs for Print Shared Library Interface */
typedef UINTN   (*PSH_ASCII_VS) (
  OUT CHAR8         *StartOfBuffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR8   *FormatString,
  IN  VA_LIST       Marker
  );

typedef UINTN   (*PSH_UNICODE_VS) (
  OUT CHAR16        *StartOfBuffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *FormatString,
  IN  VA_LIST       Marker
  );

typedef UINTN  (*PSH_ASCII_S) (
  OUT CHAR8         *StartOfBuffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR8   *FormatString,
  ...
  );

typedef UINTN (*PSH_UNICODE_VS_A_F) (
  OUT CHAR16       *StartOfBuffer,
  IN  UINTN        BufferSize,
  IN  CONST CHAR8  *FormatString,
  IN  VA_LIST      Marker
  );

typedef UINTN (*PSH_UNICODE_S) (
  OUT CHAR16        *StartOfBuffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *FormatString,
  ...
  );

typedef UINTN (*PSH_UNICODE_STRING) (
  IN OUT CHAR16  *Buffer,
  IN UINTN       Flags,
  IN INT64       Value,
  IN UINTN       Width
  );

typedef UINTN (*PSH_SPRINT_L) (
  IN  CONST CHAR16   *FormatString,
  IN  VA_LIST       Marker
  );

/* PrintLib Shared Library Interface */
typedef struct {
  PSH_ASCII_S        AsciiSPrint;
  PSH_ASCII_VS       AsciiVSPrint;
  PSH_UNICODE_VS     UnicodeVSPrint;
  PSH_UNICODE_S      UnicodeSPrint;
  PSH_UNICODE_VS_A_F UnicodeVSPrintAsciiFormat;
  PSH_UNICODE_STRING UnicodeValueToString;
  PSH_SPRINT_L       SPrintLength;
}PrintShLibType;

/* This function installs the PrintShared Library into system.
 * Only SEC should be calling this function only once during system Init. */
EFI_STATUS InitPrintLibHost (VOID);

#endif /* __PRINTSH_LIB__H__ */

