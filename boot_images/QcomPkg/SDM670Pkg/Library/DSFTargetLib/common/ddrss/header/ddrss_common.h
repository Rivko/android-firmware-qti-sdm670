/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2016, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
================================================================================*/

#ifndef __DDRSS_COMMON_H__
#define __DDRSS_COMMON_H__

#include "ddr_ss_seq_hwiobase.h"
#include "ddr_ss_seq_hwioreg.h"
#include "HAL_SNS_DDR.h"

#define BROADCAST_CH_NUM	         2

/** DDR broadcast read data operation. */
typedef enum
{
  NO_RDATA  = 0x0,         /**< No operation on broadcast read data */
  AND_RDATA = 0x1,         /**< AND each bit of broadcast read data from each channel */ 
  OR_RDATA  = 0x2          /**< OR each bit of broadcast read data from each channel */  
} DDR_BROADCAST_OPCODE;

/*SHKE periodic events enabling*/
typedef enum
{
  SHKE_INTERVAL_EVENT_CTRL_0_EN = 0x0,
  SHKE_ZQSTART_EVENT_CTRL_EN    = 0x1,
  SHKE_ZQLATCH_EVENT_CTRL_EN    = 0x2,
  SHKE_PDT_TRAC_EVENT_CTRL_EN   = 0x3
} SHKE_PERIODIC_EVENT_CTRL;

/***************************************************************************************************
 Freeze IO control in DDRSS
 ***************************************************************************************************/
static inline void DDRSS_Freeze_IO_Ctrl (uint8 enable)
{
   // Assert freeze IO
   // SW_FREEZEIO_SET   = 1 (bit 1)
   // SW_FREEZEIO_RESET = 0 (bit 0)
   // enable = 1, FREEZE_IO_CTRL[1:0] = 0x2
   if (enable == 1) {
      HWIO_OUTX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_FREEZE_IO_CTRL, 0x2);
   }
   // Deassert freeze IO
   // SW_FREEZEIO_SET   = 0 (bit 1)
   // SW_FREEZEIO_RESET = 1 (bit 0)
   // wait for 200ns
   // SW_FREEZEIO_RESET = 0 (bit 0)
   else if (enable == 0) {
      HWIO_OUTX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_FREEZE_IO_CTRL, 0x1);
      seq_wait(200, NS);
      HWIO_OUTX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_FREEZE_IO_CTRL, 0x0);    
   }
}

/***************************************************************************************************
 DDRSS Broadcast 
 ***************************************************************************************************/
static inline uint8 DDRSS_Broadcast_MC_Config(DDR_CHANNEL channel, DDR_BROADCAST_OPCODE opcode)
{
   uint32 val = 0;
   
   // opcode = NO_RDATA  = 0x0 -> DDR_SS_REGS_MC_BROADCAST[31] (OR_RDATA)  = 0
   //                          -> DDR_SS_REGS_MC_BROADCAST[30] (AND_RDATA] = 0
   // opcode = AND_RDATA = 0x1 -> DDR_SS_REGS_MC_BROADCAST[31] (OR_RDATA)  = 0
   //                          -> DDR_SS_REGS_MC_BROADCAST[30] (AND_RDATA] = 1
   // opcode = OR_RDATA  = 0x2 -> DDR_SS_REGS_MC_BROADCAST[31] (OR_RDATA)  = 1
   //                          -> DDR_SS_REGS_MC_BROADCAST[30] (AND_RDATA] = 0
   val = (opcode << 30) | channel;
   HWIO_OUTX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_MC_BROADCAST, val);
   // Add read back value check to make sure broadcast enable write completes 
   // before accessing any broadcast channel registers
   while (HWIO_INX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_MC_BROADCAST) != val);
   
   return BROADCAST_CH_NUM;
}

static inline uint8 DDRSS_Broadcast_MCCC_Config(DDR_CHANNEL channel, DDR_BROADCAST_OPCODE opcode)
{
   uint32 val = 0;
   
   // opcode = NO_RDATA  = 0x0 -> DDR_SS_REGS_MCCC_BROADCAST[31] (OR_RDATA)  = 0
   //                            -> DDR_SS_REGS_MCCC_BROADCAST[30] (AND_RDATA] = 0
   // opcode = AND_RDATA = 0x1 -> DDR_SS_REGS_MCCC_BROADCAST[31] (OR_RDATA)  = 0
   //                            -> DDR_SS_REGS_MCCC_BROADCAST[30] (AND_RDATA] = 1
   // opcode = OR_RDATA  = 0x2 -> DDR_SS_REGS_MCCC_BROADCAST[31] (OR_RDATA)  = 1
   //                            -> DDR_SS_REGS_MCCC_BROADCAST[30] (AND_RDATA] = 0
   val = (opcode << 30) | channel;
   HWIO_OUTX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_MCCC_BROADCAST, val);
   // Add read back value check to make sure broadcast enable write completes 
   // before accessing any broadcast channel registers
   while (HWIO_INX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_MCCC_BROADCAST) != val);
               
   return BROADCAST_CH_NUM;
}

static inline uint8 DDRSS_Broadcast_LLCC_Config(DDR_CHANNEL channel, DDR_BROADCAST_OPCODE opcode)
{
   uint32 val = 0;
   
   val = (opcode << 30) | channel;
   HWIO_OUTX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_LLCC_BROADCAST, val);
   // Add read back value check to make sure broadcast enable write completes 
   // before accessing any broadcast channel registers
   while (HWIO_INX (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_LLCC_BROADCAST) != val);
   
   return BROADCAST_CH_NUM;
}

/***************************************************************************************************
 PHY XO Clock control
 ***************************************************************************************************/
static inline void DDRSS_PHY_XO_Clk_Ctrl(uint8 enable)
{
   HWIO_OUTXF (SEQ_DDR_SS_DPC_REG_DPCC_OFFSET, DPCC_REG_PHY_XO_CBCR, CLK_ENABLE, enable);    
}


#endif /* __DDRSS_COMMON_H__ */
 
