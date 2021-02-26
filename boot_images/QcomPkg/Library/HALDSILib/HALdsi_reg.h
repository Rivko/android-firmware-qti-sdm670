#ifndef HAL_DSI_REG_H
#define HAL_DSI_REG_H
/*=============================================================================

  File: HALdsi_reg.h
  

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"
#include "mdsshwio.h"
/* --- DO NOT ADD HEADER FILES HERE! --- */

/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */
#define HAL_DSI_DEVICE_CORE_NUM                         0x02
#define HAL_DSI_VIDEO_CHANNEL_NUM                       0x01
#define HAL_DSI_CMD_CHANNEL_NUM                         0x02
#define LANESWAPNUM                                     8
#define HAL_DSI_MAX_RETURNSIZE_BYTE                     16
#define HAL_DSI_MAX_DMAFIFOSIZE_BYTE                    64
#define HAL_DSI_MAX_DATA_LANE_NUM                       0x04

/* -----------------------------------------------------------------------
** Specific Video mode Traffic Mode
** ----------------------------------------------------------------------- */
#define HAL_DSI_VIDEO_TRAFFIC_MODE_NONBURST_SYNCPULSE   0x00
#define HAL_DSI_VIDEO_TRAFFIC_MODE_NONBURST_SYNCEVENT   0x01
#define HAL_DSI_VIDEO_TRAFFIC_MODE_BURST                0x02

/* Color Format related */
#define HAL_DSI_VIDEO_DST_FOMRAT_RGB565_16BIT           0x00
#define HAL_DSI_VIDEO_DST_FOMRAT_RGB666_18BIT           0x01
#define HAL_DSI_VIDEO_DST_FOMRAT_RGB666_24BIT           0x02
#define HAL_DSI_VIDEO_DST_FOMRAT_RGB888_24BIT           0x03

#define HAL_DSI_CMDMODE_DST_FOMRAT_RGB111_8BIT          0x00
#define HAL_DSI_CMDMODE_DST_FOMRAT_RGB332_16BIT         0x03
#define HAL_DSI_CMDMODE_DST_FOMRAT_RGB444_16BIT         0x04
#define HAL_DSI_CMDMODE_DST_FOMRAT_RGB565_16BIT         0x06
#define HAL_DSI_CMDMODE_DST_FOMRAT_RGB666_18BIT         0x07
#define HAL_DSI_CMDMODE_DST_FOMRAT_RGB888_24BIT         0x08

#define HAL_DSI_DST_FOMRAT_SWAP_RGB                     0x00
#define HAL_DSI_DST_FOMRAT_SWAP_RBG                     0x01
#define HAL_DSI_DST_FOMRAT_SWAP_BGR                     0x02
#define HAL_DSI_DST_FOMRAT_SWAP_BRG                     0x03
#define HAL_DSI_DST_FOMRAT_SWAP_GRB                     0x04
#define HAL_DSI_DST_FOMRAT_SWAP_GBR                     0x05

#define HAL_DSI_B_SEL_NONE                              0x00
#define HAL_DSI_B_SEL_SWAP                              0x01
#define HAL_DSI_G_SEL_NONE                              0x00
#define HAL_DSI_G_SEL_SWAP                              0x01
#define HAL_DSI_R_SEL_NONE                              0x00
#define HAL_DSI_R_SEL_SWAP                              0x01

/*TODO */
#define HAL_DSI_COMMAND_INTERLEAVE_MAX                  0x01

/*Bus Priority */
#define HAL_DSI_CMDMODE_ORDER_DMA_MDP                   0x00
#define HAL_DSI_CMDMODE_ORDER_MDP_DMA                   0x01
#define HAL_DSI_CMDMODE_ORDER_PHYTE_MDP                 0x00
#define HAL_DSI_CMDMODE_ORDER_MDP_PHYTE                 0x01

/* TE Related */
#define HAL_DSI_CMDMODE_TRIGGER_TE_DATALINK             0x00
#define HAL_DSI_CMDMODE_TRIGGER_TE_EXTPIN               0x01

#define HAL_DSI_CMDMODE_TEMODE_VSYNCEDGE                0x00
#define HAL_DSI_CMDMODE_TEMODE_VSYNCWIDTH               0x01
#define HAL_DSI_CMDMODE_TEMODE_VSYNCHSYNCEDGE           0x02
#define HAL_DSI_CMDMODE_TEMODE_VSYNCHSYNCWIDTH          0x03

#define HAL_DSI_CMDMODE_MDP_TRIGGER_NONE                0x00
#define HAL_DSI_CMDMODE_MDP_TRIGGER_TE                  0x02
#define HAL_DSI_CMDMODE_MDP_TRIGGER_SW                  0x04
#define HAL_DSI_CMDMODE_MDP_TRIGGER_SWTE                0x06

#define HAL_DSI_CMDMODE_DMA_TRIGGER_NONE                0x00
#define HAL_DSI_CMDMODE_DMA_TRIGGER_SOFEOF              0x01
#define HAL_DSI_CMDMODE_DMA_TRIGGER_TE                  0x02
#define HAL_DSI_CMDMODE_DMA_TRIGGER_SW                  0x04
#define HAL_DSI_CMDMODE_DMA_TRIGGER_SWSOFEOF            0x05
#define HAL_DSI_CMDMODE_DMA_TRIGGER_SWTE                0x06

#define HAL_DSI_TRIG_CTRL_CMDMODE_DMA_STREAM_SEL_0      0x00
#define HAL_DSI_TRIG_CTRL_CMDMODE_DMA_STREAM_SEL_1      0x01


typedef struct
{
  uint32  iColorDepth;
  uint32  iColorSwap;
  uint32  iColorRSel;
  uint32  iColorGSel;
  uint32  iColorBSel;
  uint32  iBpp;
}HAL_DSI_ColorInfoType;



#endif /* HAL_DSI_REG_H */
