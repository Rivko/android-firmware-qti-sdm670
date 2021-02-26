#ifndef __PMICTESTBSI_H__
#define __PMICTESTBSI_H__

/*! @file PMICTESTBSI.h
 *
 *  PMIC Test BSI
 *
 *  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/14/15   RM      Added Pmic Test BSI Header file

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

typedef enum
{
   EFI_PM_BSI_DLY_0_TAU,
   EFI_PM_BSI_DLY_5_TAU,
   EFI_PM_BSI_DLY_10_TAU,
   EFI_PM_BSI_DLY_15_TAU,
   EFI_PM_BSI_DLY_TAU_INVALID
}EFI_PM_BSI_DLY_TYPE;

typedef enum
{
    EFI_PM_TEST_BSI_BAT_GONE_CFG,
    EFI_PM_TEST_BSI_CFG_TX_DLY,
    EFI_PM_TEST_BSI_FSM_RESET, 
    EFI_PM_TEST_BSI_GET_RX_FIFO_WORD
}EFI_PM_TEST_DRIVER_BSI_TYPE_FUNCTION;

typedef struct _EFI_PM_TEST_BSI_BAT_GONE_CFG_PARAM
{
    UINT8       DeviceIndex;
    boolean     SelectBatt;
} EFI_PM_TEST_BSI_BAT_GONE_CFG_PARAM;

typedef struct _EFI_PM_TEST_BSI_CFG_TX_DLY_PARAM
{
    UINT8                   DeviceIndex;
    EFI_PM_BSI_DLY_TYPE     DlyType;
} EFI_PM_TEST_BSI_CFG_TX_DLY_PARAM;

typedef struct _EFI_PM_TEST_BSI_FSM_RESET_PARAM
{
    UINT8       DeviceIndex;
    boolean     FSMReset;
    boolean     RxFIFOReset;
} EFI_PM_TEST_BSI_FSM_RESET_PARAM;

typedef struct _EFI_PM_TEST_BSI_GET_RX_FIFO_WORD_PARAM
{
    UINT8       DeviceIndex;
    UINT64      Words;
} EFI_PM_TEST_BSI_GET_RX_FIFO_WORD_PARAM;


typedef union _EFI_PM_TEST_BSI_FUNCTION_TYPE
{
    EFI_PM_TEST_BSI_BAT_GONE_CFG_PARAM      BsiBatGoneCFG;
    EFI_PM_TEST_BSI_CFG_TX_DLY_PARAM        BsiCfgTxDly; 
    EFI_PM_TEST_BSI_FSM_RESET_PARAM         BsiFsmReset;
    EFI_PM_TEST_BSI_GET_RX_FIFO_WORD_PARAM  BsiGetRxFifoWord;
}EFI_PM_TEST_BSI_FUNCTION_TYPE;



/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTBSI_H__


