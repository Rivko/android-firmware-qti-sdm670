/** 
@file  EFIPmicClkBuff.h
@brief PMIC diff and div clock for UEFI.
*/
/*=============================================================================
Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
06/18/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
02/21/13   al       Add clk div api
10/23/12   al       Updating copyright info 
10/16/12   al       added div clk
09/06/12   al       Created new protocol

=============================================================================*/

#ifndef __EFIPMICCLKBUFF_H__
#define __EFIPMICCLKBUFF_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_pmicClkBuff_constants 
Protocol version.
*/

#define PMIC_CLKBUFF_REVISION 0x0000000000010003
/** @} */ /* end_addtogroup efi_pmicClkBuff_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicClkBuff_protocol */
#define EFI_PMIC_CLKBUFF_PROTOCOL_GUID \
    { 0xdd940b86, 0x98ae, 0x4426, { 0x96, 0xbb, 0x6b, 0xaf, 0x49, 0x84, 0x9c, 0xd7 } }

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC CLK Protocol GUID.
*/
extern EFI_GUID gQcomPmicClkBuffProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_CLKBUFF_PROTOCOL   EFI_QCOM_PMIC_CLKBUFF_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicClkBuff_data_types 
@{ */

/**
  PMIC clock type.
*/
typedef enum{
    EFI_PM_CLK_SLEEP,
    EFI_PM_CLK_XO,
    EFI_PM_CLK_RF_1,
    EFI_PM_CLK_RF_2,
    EFI_PM_CLK_RF_3,
    EFI_PM_CLK_BB_1,
    EFI_PM_CLK_BB_2,
    EFI_PM_CLK_DIFF_1,
    EFI_PM_CLK_DIV_1,
    EFI_PM_CLK_DIV_2,
    EFI_PM_CLK_DIV_3,
    EFI_PM_CLK_DIST,
    EFI_PM_CLK_LN_BB,
    EFI_PM_ALL_CLKS,
    EFI_PM_CLK_TYPE_INVALID  /**< Invalid clock. */
} EFI_PM_CLK_BUFF_TYPE;

/**
* 
  PMIC clock output buffer drive strength.
*/
typedef enum
{
    EFI_PM_CLK_BUFF_OUT_DRV__1X, /**< 1x. */
    EFI_PM_CLK_BUFF_OUT_DRV__2X, /**< 2x.  */
    EFI_PM_CLK_BUFF_OUT_DRV__3X, /**< 3x.  */
    EFI_PM_CLK_BUFF_OUT_DRV__4X, /**< 4x. */
    EFI_PM_CLK_BUFF_OUT_DRV__INVALID  /**< Invalid strength. */
} EFI_PM_CLK_BUFF_OUTPUT_DRIVE_STRENGTH_TYPE;
/** @} */ /* end_addtogroup efi_pmicClkBuff_data_types */

typedef struct
{
    EFI_PM_CLK_BUFF_OUTPUT_DRIVE_STRENGTH_TYPE DrvStrength; 
    BOOLEAN                                    SwEnable;
    BOOLEAN                                    PinCtrled;
    BOOLEAN                                    PullDown;
    UINT32                                     OutDiv;
}EFI_PM_CLK_STATUS_TYPE;

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/* EFI_PM_CLKBUFF_ENABLE */ 
/** @ingroup efi_pmClkBuff_enable
@par Summary
  Enables/disables the clock buffer.

  @param[in]  PmicDeviceIndex   Primary: 0. Secondary: 1.
  @param[in]  ClkBuffType       Clock type. See #EFI_PM_CLK_BUFF_TYPE 
                                for details.
  @param[in]  Enable            If TRUE, enable the clock buffer; if FALSE, 
                                disable the clock buffer.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_CLKBUFF_ENABLE)(
    IN UINT32                PmicDeviceIndex, 
    IN EFI_PM_CLK_BUFF_TYPE  ClkBuffType,
    IN BOOLEAN               Enable
    );

/* EFI_PM_CLKBUFF_SET_OUT_DRV_STRENGTH */ 
/** @ingroup efi_pmClkBuff_set_out_drv_strength
  @par Summary
  Sets the clock buffer output drive strength.

  @param[in]  PmicDeviceIndex   Primary: 0. Secondary: 1.
  @param[in]  ClkBuffType       Clock type. See #EFI_PM_CLK_BUFF_TYPE 
                                for details.
  @param[in]  OutDrvStrength    Buffer output drive strength. See  
                                #EFI_PM_CLK_BUFF_OUTPUT_DRIVE_STRENGTH_TYPE 
                                for details.

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_CLKBUFF_SET_OUT_DRV_STRENGTH)(
    IN UINT32                                      PmicDeviceIndex, 
    IN EFI_PM_CLK_BUFF_TYPE                        ClkBuffType,
    IN EFI_PM_CLK_BUFF_OUTPUT_DRIVE_STRENGTH_TYPE  OutDrvStrength
    );

/* EFI_PM_CLKBUFF_SET_OUT_CLK_DIV */
/** @ingroup efi_pmClkBuff_set_out_clk_div
  @par Summary
  Sets the clock output divider.

  @param[in]  PmicDeviceIndex   Primary: 0. Secondary: 1.
  @param[in]  ClkBuffType       Divider clock type. See #EFI_PM_CLK_BUFF_TYPE 
                                for details.
@param[in]  Divider              Clock output divider. Acceptable values are:
                                - 0 -- No clock
                                - 1 -- XO/1
                                - 2 -- XO/2
                                - 4 -- XO/4
                                - 8 -- XO/8
                                - 16 -- XO/16
                                - 32 -- XO/32
                                - 64 -- XO/64
                                @tablebulletend

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_CLKBUFF_SET_OUT_CLK_DIV)(
    IN UINT32                 PmicDeviceIndex, 
    IN EFI_PM_CLK_BUFF_TYPE   ClkBuffType,
    IN UINT32                 Divider
    );

/* EFI_PM_CLKBUFF_GET_STATUS */ 
/** @ingroup
@par Summary gets the PMIC clk status

@param[in]  PmicDeviceIndex      Pmic Device Index. 
                                 For Primary   :0                 
                                 For Secondary :1
@param[in]  ClkBuffType          Pmic Clock type. Refer EFI_PM_CLK_TYPE
@param[out] ClkStatus            Clock status.  
                                 Refer #EFI_PM_CLK_STATUS_TYPE for more info       

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_CLKBUFF_GET_STATUS)(
    IN UINT32                   PmicDeviceIndex, 
    IN EFI_PM_CLK_BUFF_TYPE     ClkBuffType,
    OUT EFI_PM_CLK_STATUS_TYPE  *ClkStatus
    );

/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicClkBuff_protocol
@par Summary
  Qualcomm PMIC Clock Protocol interface.

@par Parameters
@inputprotoparams{pmic_clk_buff_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_CLKBUFF_PROTOCOL {
    UINT64                               Revision;
    EFI_PM_CLKBUFF_ENABLE                ClkBuffEnable;
    EFI_PM_CLKBUFF_SET_OUT_DRV_STRENGTH  SetOutDrvStrength;
    EFI_PM_CLKBUFF_SET_OUT_CLK_DIV       SetOutClkDiv;
    EFI_PM_CLKBUFF_GET_STATUS            GetStatus;
};

#endif  /* __EFIPMICCLKBUFF_H__ */
