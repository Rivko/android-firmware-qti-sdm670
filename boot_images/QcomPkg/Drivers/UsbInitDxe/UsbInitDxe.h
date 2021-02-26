#pragma once

#include <Uefi.h>
#include <Guid/EventGroup.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIUsbConfig.h>
#include <Protocol/EFIUsbInitProtocol.h>

#define USB_INIT_DEV_SIGNATURE   SIGNATURE_32 ('u', 's', 'b', 'i')

// Terminate output with a newline, and eliminate the need for double parens.
#define DBG(EfiD, Str, args...) \
  DEBUG ((EfiD, "%a: " Str "\n",__FUNCTION__, ##args));

// For debug stack info.
#define FNC_ENTER_MSG() DBG (EFI_D_INFO, "++")
#define FNC_LEAVE_MSG() DBG (EFI_D_INFO, "--")

// Cannot continue, exit function.
#define ERR_CHK(ErrStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, ErrStr, ##__VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define WRN_CHK(WrnStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_WARN, WrnStr, ##__VA_ARGS__); \
    } \
  } while (0)

typedef struct _DEV_CTXT DEV_CTXT;

#define DEV_CTXT_FROM_THIS(x)    CR(x, DEV_CTXT, UsbInitProtocol, USB_INIT_DEV_SIGNATURE)

/**
* Usbfn software device structure.
*/
struct _DEV_CTXT {
  UINTN                         Signature;
  EFI_USB_INIT_PROTOCOL         UsbInitProtocol;
  QCOM_USB_CONFIG_PROTOCOL     *UsbCfgProtocol;
  EFI_EVENT                     ExitBootServiceEvent;
};
