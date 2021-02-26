#include "Uefi.h"
#include <Library/BdsTargetLib.h>

EFI_STATUS OverrideAblLoadAddr(UINTN *Addr)
{
  if (Addr == NULL)
    return EFI_INVALID_PARAMETER;

  *Addr = 0x9FA00000;
  return EFI_SUCCESS;
}
