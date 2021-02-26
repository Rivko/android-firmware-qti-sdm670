#include "oem.h"

/* QSEE defined error fatal reason codes. */
#define TZBSP_ERR_FATAL_NONE                         0
#define TZBSP_ERR_FATAL_NON_SECURE_WDT               1
#define TZBSP_ERR_FATAL_SECURE_WDT                   2
#define TZBSP_ERR_FATAL_AHB_TIMEOUT                  3
#define TZBSP_ERR_FATAL_RPM_WDOG                     4
#define TZBSP_ERR_FATAL_RPM_ERR                      5
#define TZBSP_ERR_FATAL_NOC_ERROR                    6
#define TZBSP_ERR_FATAL_BIMC_ERROR                   7
#define TZBSP_ERR_FATAL_SMEM                         8
#define TZBSP_ERR_FATAL_XPU_VIOLATION                9
#define TZBSP_ERR_FATAL_SMMU_FAULT                   10
#define TZBSP_ERR_FATAL_QSEE_ERR                     11

#define TZBSP_ERR_FATAL_EL3_SP_EL0_SYNCH             12
#define TZBSP_ERR_FATAL_EL3_SP_EL0_IRQ               13
#define TZBSP_ERR_FATAL_EL3_SP_EL0_FIQ               14
#define TZBSP_ERR_FATAL_EL3_SP_EL0_ERR               15
#define TZBSP_ERR_FATAL_EL3_SP_EL3_SYNCH             16
#define TZBSP_ERR_FATAL_EL3_SP_EL3_IRQ               17
#define TZBSP_ERR_FATAL_EL3_SP_EL3_FIQ               18
#define TZBSP_ERR_FATAL_EL3_SP_EL3_ERR               19
#define TZBSP_ERR_FATAL_EL3_LEL64_SYNCH              20
#define TZBSP_ERR_FATAL_EL3_LEL64_IRQ                21
#define TZBSP_ERR_FATAL_EL3_LEL64_FIQ                22
#define TZBSP_ERR_FATAL_EL3_LEL64_ERR                23
#define TZBSP_ERR_FATAL_EL3_LEL32_SYNCH              24
#define TZBSP_ERR_FATAL_EL3_LEL32_IRQ                25
#define TZBSP_ERR_FATAL_EL3_LEL32_FIQ                26
#define TZBSP_ERR_FATAL_EL3_LEL32_ERR                27

#define TZBSP_ERR_FATAL_EL1_SP_EL0_SYNCH             28
#define TZBSP_ERR_FATAL_EL1_SP_EL0_IRQ               29
#define TZBSP_ERR_FATAL_EL1_SP_EL0_FIQ               30
#define TZBSP_ERR_FATAL_EL1_SP_EL0_ERR               31
#define TZBSP_ERR_FATAL_EL1_SP_EL1_SYNCH             32
#define TZBSP_ERR_FATAL_EL1_SP_EL1_IRQ               33
#define TZBSP_ERR_FATAL_EL1_SP_EL1_FIQ               34
#define TZBSP_ERR_FATAL_EL1_SP_EL1_ERR               35
#define TZBSP_ERR_FATAL_EL1_LEL64_SYNCH              36
#define TZBSP_ERR_FATAL_EL1_LEL64_IRQ                37
#define TZBSP_ERR_FATAL_EL1_LEL64_FIQ                38
#define TZBSP_ERR_FATAL_EL1_LEL64_ERR                39
#define TZBSP_ERR_FATAL_EL1_LEL32_SYNCH              40
#define TZBSP_ERR_FATAL_EL1_LEL32_IRQ                41
#define TZBSP_ERR_FATAL_EL1_LEL32_FIQ                42
#define TZBSP_ERR_FATAL_EL1_LEL32_ERR                43

#define TZBSP_ERR_FATAL_RPM_DRIVER_ERR               44
#define TZBSP_ERR_FATAL_RESET_TIMER_EXP              45
#define TZBSP_ERR_FATAL_ICE_ERR                      46
#define TZBSP_ERR_FATAL_LMH_DRIVER_ERR               47
#define TZBSP_ERR_FATAL_ACCESS_CONTROL               48
#define TZBSP_ERR_FATAL_CLOCK                        49
#define TZBSP_ERR_FATAL_GIC_CPU_MAP_INVALID          50
#define TZBSP_ERR_FATAL_SEC_WDT_TIMER_TRIGGER        51
#define TZBSP_ERR_FATAL_FAULT_DETECTED               52

#define TZBSP_ERR_FATAL_MON_PREFETCH_ABT             53
#define TZBSP_ERR_FATAL_MON_DATA_ABT                 54

#define TZBSP_ERR_FATAL_MON_UNDEF                    65

#define TZBSP_ERR_FATAL_UNKNOWN                      0xFFFFFFFF

/* Map QSEE error fatal reason to OEM specific error fatal code */
static tzbsp_reset_reason_pair_t rst_reason_list[] =
{
  /* QSEE Err Fatal reason , OEM Err Fatal code */
  {TZBSP_ERR_FATAL_NONE                  , 0},
  {TZBSP_ERR_FATAL_NON_SECURE_WDT        , 1},
  {TZBSP_ERR_FATAL_SECURE_WDT            , 2},
  {TZBSP_ERR_FATAL_AHB_TIMEOUT           , 3},
  {TZBSP_ERR_FATAL_RPM_WDOG              , 4},
  {TZBSP_ERR_FATAL_RPM_ERR               , 5},
  {TZBSP_ERR_FATAL_NOC_ERROR             , 6},
  {TZBSP_ERR_FATAL_BIMC_ERROR            , 7},
  {TZBSP_ERR_FATAL_SMEM                  , 8},
  {TZBSP_ERR_FATAL_XPU_VIOLATION         , 9},
  {TZBSP_ERR_FATAL_SMMU_FAULT            , 10},
  {TZBSP_ERR_FATAL_QSEE_ERR              , 11},

  {TZBSP_ERR_FATAL_EL3_SP_EL0_SYNCH      , 12},
  {TZBSP_ERR_FATAL_EL3_SP_EL0_IRQ        , 13},
  {TZBSP_ERR_FATAL_EL3_SP_EL0_FIQ        , 14},
  {TZBSP_ERR_FATAL_EL3_SP_EL0_ERR        , 15},
  {TZBSP_ERR_FATAL_EL3_SP_EL3_SYNCH      , 16},
  {TZBSP_ERR_FATAL_EL3_SP_EL3_IRQ        , 17},
  {TZBSP_ERR_FATAL_EL3_SP_EL3_FIQ        , 18},
  {TZBSP_ERR_FATAL_EL3_SP_EL3_ERR        , 19},
  {TZBSP_ERR_FATAL_EL3_LEL64_SYNCH       , 20},
  {TZBSP_ERR_FATAL_EL3_LEL64_IRQ         , 21},
  {TZBSP_ERR_FATAL_EL3_LEL64_FIQ         , 22},
  {TZBSP_ERR_FATAL_EL3_LEL64_ERR         , 23},
  {TZBSP_ERR_FATAL_EL3_LEL32_SYNCH       , 24},
  {TZBSP_ERR_FATAL_EL3_LEL32_IRQ         , 25},
  {TZBSP_ERR_FATAL_EL3_LEL32_FIQ         , 26},
  {TZBSP_ERR_FATAL_EL3_LEL32_ERR         , 27},

  {TZBSP_ERR_FATAL_EL1_SP_EL0_SYNCH      , 28},
  {TZBSP_ERR_FATAL_EL1_SP_EL0_IRQ        , 29},
  {TZBSP_ERR_FATAL_EL1_SP_EL0_FIQ        , 30},
  {TZBSP_ERR_FATAL_EL1_SP_EL0_ERR        , 31},
  {TZBSP_ERR_FATAL_EL1_SP_EL1_SYNCH      , 32},
  {TZBSP_ERR_FATAL_EL1_SP_EL1_IRQ        , 33},
  {TZBSP_ERR_FATAL_EL1_SP_EL1_FIQ        , 34},
  {TZBSP_ERR_FATAL_EL1_SP_EL1_ERR        , 35},
  {TZBSP_ERR_FATAL_EL1_LEL64_SYNCH       , 36},
  {TZBSP_ERR_FATAL_EL1_LEL64_IRQ         , 37},
  {TZBSP_ERR_FATAL_EL1_LEL64_FIQ         , 38},
  {TZBSP_ERR_FATAL_EL1_LEL64_ERR         , 39},
  {TZBSP_ERR_FATAL_EL1_LEL32_SYNCH       , 40},
  {TZBSP_ERR_FATAL_EL1_LEL32_IRQ         , 41},
  {TZBSP_ERR_FATAL_EL1_LEL32_FIQ         , 42},
  {TZBSP_ERR_FATAL_EL1_LEL32_ERR         , 43},

  {TZBSP_ERR_FATAL_RPM_DRIVER_ERR        , 44},
  {TZBSP_ERR_FATAL_RESET_TIMER_EXP       , 45},
  {TZBSP_ERR_FATAL_ICE_ERR               , 46},
  {TZBSP_ERR_FATAL_LMH_DRIVER_ERR        , 47},
  {TZBSP_ERR_FATAL_ACCESS_CONTROL        , 48},
  {TZBSP_ERR_FATAL_CLOCK                 , 49},
  {TZBSP_ERR_FATAL_GIC_CPU_MAP_INVALID   , 50},
  {TZBSP_ERR_FATAL_SEC_WDT_TIMER_TRIGGER , 51},
  {TZBSP_ERR_FATAL_FAULT_DETECTED        , 52},

  {TZBSP_ERR_FATAL_MON_PREFETCH_ABT      , 53},
  {TZBSP_ERR_FATAL_MON_DATA_ABT          , 54},

  {TZBSP_ERR_FATAL_MON_UNDEF             , 65},

  {TZBSP_ERR_FATAL_UNKNOWN               , 0xDEADD00D},
};

tzbsp_cfg_oem_reset_reason_t oem_rst_reason_list =
{
  sizeof(rst_reason_list)/sizeof(tzbsp_reset_reason_pair_t),
  rst_reason_list
};
