#ifndef USB_ERROR_H
#define USB_ERROR_H
/*=======================================================================*/
/**
  @file         UsbError.h

  @brief        USB Error

  @details      USB subsystem error header.

               Copyright (c) 2014 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
 */
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/30/14   ar      Initial creation
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* USB Specific Errors */
/* Refer to Protocol\EFIUsbIo.h for main error codes */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Macros here modeled after those in MdePkg/Include/Library/DebugLib.h */

#ifndef MDEPKG_NDEBUG
  #define DBG_PRINT(Level,Str,...)                       \
          DebugPrint (EFI_D_ERROR,                       \
                      "%a:%d " Str "\n",                 \
                      __FILE__,__LINE__, ##__VA_ARGS__); 
#else
  #define DBG_PRINT(Str,...)
#endif // MDEPKG_NDEBUG 

#define USB_ASSERT_RETURN_STR(Test,Retcode,ErrStr,...)   \
  do {                                                   \
    if (!(Test)) {                                       \
      DBG_PRINT(EFI_D_ERROR,ErrStr, ##__VA_ARGS__);      \
      ASSERT(0);                                         \
      return Retcode;                                    \
    }                                                    \
  } while (0)


#define USB_ASSERT_GOTO_STR(Test,Label,ErrStr,...)       \
  do {                                                   \
    if (!(Test)) {                                       \
      DBG_PRINT(EFI_D_ERROR,ErrStr, ##__VA_ARGS__);      \
      ASSERT(0);                                         \
      goto Label;                                        \
    }                                                    \
  } while (0)


#define USB_ASSERT_GOTO_SETSTS_STR(Test,Label,RetCode,Status,ErrStr,...) \
  do {                                                   \
    if (!(Test)) {                                       \
      DBG_PRINT(EFI_D_ERROR,ErrStr, ##__VA_ARGS__);      \
      ASSERT(0);                                         \
      Status = RetCode;                                  \
      goto Label;                                        \
    }                                                    \
  } while (0)

#define USB_ASSERT(Test)                     USB_ASSERT_RETURN_STR((Test), , "ASSERT: " #Test)
#define USB_ASSERT_RETURN(Test, RetCode)     USB_ASSERT_RETURN_STR((Test), RetCode, "ASSERT: " #Test)
#define USB_ASSERT_GOTO(Test, Label)         USB_ASSERT_GOTO_STR((Test), Label, "ASSERT: " #Test)
#define USB_ASSERT_GOTO_SETSTS(Test, Label, RetCode, Status) \
                                             USB_ASSERT_GOTO_SETSTS_STR((Test), Label, RetCode, Status,"ASSERT: " #Test)

#define USB_ASSERT_EFI_ERROR_RETURN_STR(Status,Retcode,ErrStr,...)   \
  do {                                                   \
    if (EFI_ERROR (Status)) {                            \
      DBG_PRINT(EFI_D_ERROR,ErrStr, ##__VA_ARGS__);      \
      ASSERT(0);                                         \
      return Retcode;                                    \
    }                                                    \
  } while (0)


#define USB_ASSERT_EFI_ERROR_GOTO_STR(Status,Label,ErrStr,...)       \
  do {                                                   \
    if (EFI_ERROR (Status)) {                            \
      DBG_PRINT(EFI_D_ERROR,ErrStr, ##__VA_ARGS__);      \
      ASSERT(0);                                         \
      goto Label;                                        \
    }                                                    \
  } while (0)


#define USB_ASSERT_EFI_ERROR(Status)                     USB_ASSERT_EFI_ERROR_RETURN_STR((Status), , "ASSERT: EFI_ERROR " #Status)
#define USB_ASSERT_EFI_ERROR_RETURN(Status, RetCode)     USB_ASSERT_EFI_ERROR_RETURN_STR((Status), RetCode, "ASSERT: EFI_ERROR " #Status)
#define USB_ASSERT_EFI_ERROR_GOTO(Status, Label)         USB_ASSERT_EFI_ERROR_GOTO_STR((Status), Label, "ASSERT: EFI_ERROR " #Status)

#endif //USB_ERROR_H

