#ifndef _USB_UTILITY_H_
#define _USB_UTILITY_H_

//
// Miscellaneous Definitions
//
// Terminate output with a newline, and eliminate the need for double parens.
#define DBG(EfiD, Str, args...) \
  DEBUG ((EfiD, "%a: " Str "\n",__FUNCTION__, ##args));

// For debug stack info.
#define FNC_ENTER_MSG() DBG (EFI_D_INFO, "++")
#define FNC_LEAVE_MSG() DBG (EFI_D_INFO, "--")

// Converts endpoint index and direction to address.
#define ENDPOINT_ADDR(EndpointIndex, Tx) \
  ((EndpointIndex) | ((Tx) ? 0x80 : 0x00))

#define ERR_CHK(ErrStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, ErrStr, __VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define WRN_CHK(WrnStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, WrnStr, __VA_ARGS__); \
    } \
  } while (0)

// Check parameters.
#define ARG_CHK(Expr) \
  do { \
    if (!(Expr)) { \
      Status = EFI_INVALID_PARAMETER; \
      ERR_CHK ("invalid parameter"); \
    } \
  } while (0)

#endif
