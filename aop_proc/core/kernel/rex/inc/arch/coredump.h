#ifndef COREDUMP_H
#define COREDUMP_H

#include <stdint.h>
#include "cortex-m3.h"

#ifdef __cplusplus
#pragma diag_suppress 368 // It's a C structure dammit, it doesn't have a constructor.
extern "C" {
#endif

typedef struct __attribute__((packed, aligned(4)))
{
    uint32_t SystemHandlerCtrlAndState;
    uint32_t ConfigurableFaultStatus;
    uint32_t HardFaultStatus;
    uint32_t DebugFaultStatus;
    uint32_t MemManageAddress;
    uint32_t BusFaultAddress;
    uint32_t AuxFaultStatus;
} m3_fault_info_t;

typedef struct __attribute__((packed, aligned(4)))
{
  uint32_t gp_regs[13]; // r0 - r12

  // The RPM has two possible stack pointers:
  //   1) the "main" stack for the OS, exceptions, and all privileged code
  //   2) the "process" stack for application code
  uint32_t SP_main;
  uint32_t SP_process;
  
  uint32_t LR; // r14 / link register
  uint32_t PC; // r15 / program counter

  // Special registers
  // Program status registers (read collectively)
  uint32_t xPSR;

  // Interrupt mask registers
  uint32_t PRIMASK;
  uint32_t FAULTMASK;
  uint32_t BASEPRI;

  // Control information (which stack and execution privilege)
  uint32_t CONTROL;
} m3_registers_t;

typedef __packed union
{
  uint32_t v;
  __packed struct
  {
      unsigned minor_version : 8;
      unsigned major_version : 8;
      unsigned device_number :12;
      unsigned family_number : 4;
  } parts;
} soc_hw_version_t;

typedef struct __attribute__((packed, aligned(4)))
{
  const uint32_t   cookie;      // Set to 0xbaff1ed, for locating the dump without symbols.

  uint64_t         dumped_at;   // Timestamp of core dump; 0 if no failures, QTimer tick if failed.
  m3_registers_t   registers;
  m3_fault_info_t  fault_detail;

  soc_hw_version_t hw_version;
  uint32_t         chip_reset_en; // Flag to enable pulling PS_HOLD low at the end of abort()
  const void      *ulog_state;
  const void      *npa_state;
  const void      *rpmserver_state;
  const void      *scheduler_state;
  const void      *railway_state;
  const void      *railway_config;
  const void      *cpr_info;
} m3_core_dump_t;

extern m3_core_dump_t aop_core_dump;

void dump_core(void);
void rpm_core_dump_init(void);
void notify_tz(void);

#ifdef __cplusplus
};
#endif

#endif // COREDUMP_H

