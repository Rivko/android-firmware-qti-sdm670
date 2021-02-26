#ifndef CTI_TOP_H
#define CTI_TOP_H
/*==============================================================================
  FILE:         cti_top.h
  
  OVERVIEW:     This file provides the externs and declarations needed for
                target specific code

  DEPENDENCIES: None

                Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/products/cti/inc/cti_top.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "DALStdDef.h"
#include "DDIChipInfo.h"

/* ==================================================================
                            Macros
   ================================================================== */

#define CTI_BUSYWAIT_DELAY                      20000000 //about 20 seconds
#define CTI_WORKLOOP_PRIORITY                   (3)
#define CTI_WORKLOOP_MAX_EVENTS                 (10)
#define CTI_WORKLOOP_STACK_SIZE                 0x00001000

//Timer values, for clock votes.
#define PRODUCTS_QDSS_CLOCK_VOTE_TIMER_VALUE    1000 //leave clock on for 1 second
#define PRODUCTS_QDSS_CLOCK_TIMER_SIGNAL        0x1
#define TIMER_BOOL_NON_DEFERABLE                0


//////////////////Base Address Values///////////////////
#define SMEM_START_SDM670       0x14680000
#define SMEM_START_SDM845       0x14680000
#define SMEM_START_8998         0x14680000
#define SMEM_START_8996         0x14680000
#define SMEM_START_8994         0x14680000
#define SMEM_START_8992         0x14680000

#define SMEM_START_SDX24        0x14680000
#define SMEM_START_SDX50        0x08600000
#define SMEM_START_SDX20        0x08600000
#define SMEM_START_9X65         0x08600000
#define SMEM_START_9X55         0x08600000

#define SDM_SHARED_IMEM_OFFSET  0x3F000
#define SDX_SHARED_IMEM_OFFSET  0x0

//MSM/SDM base addresses
#define CTI_IMEM_BASE_SDM670    SMEM_START_SDM670 + SDM_SHARED_IMEM_OFFSET       //0x146BF000
#define CTI_IMEM_BASE_SDM845    SMEM_START_SDM845 + SDM_SHARED_IMEM_OFFSET       //0x146BF000
#define CTI_IMEM_BASE_8997      SMEM_START_8997   + SDM_SHARED_IMEM_OFFSET       //0x146BF000
#define CTI_IMEM_BASE_8998      SMEM_START_8998   + SDM_SHARED_IMEM_OFFSET       //0x146BF000
#define CTI_IMEM_BASE_8996      SMEM_START_8996   + SDM_SHARED_IMEM_OFFSET       //0x066BF000
#define CTI_IMEM_BASE_8994      SMEM_START_8994   + SDM_SHARED_IMEM_OFFSET       //0xFE87F000
#define CTI_IMEM_BASE_8992      SMEM_START_8992   + SDM_SHARED_IMEM_OFFSET       //0xFE80F000
//SDX/MDM base addresses    
#define CTI_IMEM_BASE_9X55      SMEM_START_9X55   + SDX_SHARED_IMEM_OFFSET       //0x08600000
#define CTI_IMEM_BASE_9X65      SMEM_START_9X65   + SDX_SHARED_IMEM_OFFSET       //0x08600000
#define CTI_IMEM_BASE_SDX20     SMEM_START_SDX20  + SDX_SHARED_IMEM_OFFSET       //0x08600000
#define CTI_IMEM_BASE_SDX24     SMEM_START_SDX24  + SDX_SHARED_IMEM_OFFSET       //0x08600000
#define CTI_IMEM_BASE_SDX50     SMEM_START_SDX50  + SDX_SHARED_IMEM_OFFSET       //0x08600000
    
#define IMEM_BASE_DEFAULT       CTI_IMEM_BASE_SDM845

#define CTI_IMEM_OFFSET_1       0x938
#define CTI_IMEM_OFFSET_2       0x93C
#define CTI_IMEM_OFFSET_3       0x940
//#define COOKIE_LOCATION       CTI_IMEM_3


//////////////////////////Opcodes//////////////////////////////
#define COOKIE_BUSYWAIT                     0x00444248
#define COOKIE_BUSYWAIT_DEVOTE_QDSS_CLOCK   0x0044424F
#define COOKIE_BKRPT                        0x00444249
#define COOKIE_CRASH                        0x0044424A
#define COOKIE_TRACE                        0x0044424B
#define COOKIE_ETM_COMPARATOR               0x0044424C
#define COOKIE_DISABLE_SLEEP                0x0044424D
#define COOKIE_READ_REG                     0x0044424E

#define GOOD_COOKIE_OPCODE_MASK             (COOKIE_BUSYWAIT&(0xFFFFFFF0))


#define MPSS_SELECT_VALUE                   0x1
#define ADSP_SELECT_VALUE                   0x2
#define CDSP_SELECT_VALUE                   0x3
#define SLPI_SELECT_VALUE                   0x4
#define SSCM3_SELECT_VALUE                  0x5
#define SHRM_SELECT_VALUE                   0x6
                
#define PROCESSOR_ALL_FLAG                  0x1

////////////////////////Various shifts and masks/////////////////
#define PROCESSOR_ALL_FLAG_SHIFT            31
#define PROCESSOR_ALL_FLAG_MASK             80000000
#define PROCESSOR_VALUE_SELECT_MASK         0xFF000000
#define PROCESSOR_VALUE_SELECT_SHIFT        24
#define COOKIE_ACTION_MASK                  0x00FFFFFF


///////////////////////Various flags/////////////////////////////

//NOTE: Image specific defines are in lower image specific cti.h

///Flag to enable island clocks on CTI interrupt. For SSC Island only (Defined as 1 in slpi/cti.h)
#define ENABLE_ISLAND_CLOCKS_FOR_JTAG       0 

#define COOKIE_SELECT_ISLAND_MODE           COOKIE_BUSYWAIT

///Need to get correct clock votes here. Disable until that's done.
#define SDM845_CLOCKVOTE_ENABLE_FLAG        FALSE


/* ==================================================================
                            Structs
   ================================================================== */
typedef struct 
{
  DALSYSWorkLoopHandle    handle;
  DALSYSEventHandle       event;
  
  // Event for trigger infinite loop in the WorkLoop
  DALSYSEventHandle         start_event;
  uint32                    cookie;
  uint32                    all_processor_flag;
  uint32                    processor_select;
  uint32                    cookie_action;
  unsigned int              reg1;
  unsigned int              reg2;
  unsigned int              reg3;
  
} cti_target_config;

/* ==================================================================
                            Extern Vars
   ================================================================== */

extern npa_client_handle    npa_handle;
extern DalDeviceHandle      *hDALIntCtl;

extern uint32               IMEM_Base; 
extern uint32               CTI_imem_1; 
extern uint32               CTI_imem_2; 
extern uint32               CTI_imem_3;
extern uint32               Cookie_Location;

//Island Variables
extern cti_target_config    cti_cfg;               
extern boolean              QDSS_ClockVote_Feature_Enabled;  
extern boolean              NPA_QDSS_Increase_Request_Issued;
extern boolean              NPA_QDSS_Decrease_Request_Issued;

/* ==================================================================
                        Function Declarations
   ================================================================== */

void                    cti_irq_init( void );
void                    cti_init_workloop( void );
void                    cti_read_registers(void);


void                    ProductsTimerFunc( void );

//Island Functions
extern unsigned int     cti_read_imem(void);                         
extern DALResult        cti_workloop(DALSYSEventHandle hEvent, void *pCtx); 
extern void             islandcti_enableDebug(boolean enable);                   
extern void             CTI_Wakeup_Handler(uint32 param);                        

#endif //CTI_TOP_H