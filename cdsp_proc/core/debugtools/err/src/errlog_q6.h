#ifndef ERRLOG_Q6_H
#define ERRLOG_Q6_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  E R R

DESCRIPTION
  This header file contains the definitions of log structure for core dump
 
Copyright (c) 2007 - 2016   by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/errlog_q6.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/07   tbg     Initial revision (pulled code from rexlog.h)

===========================================================================*/


/************************************************************************
 *                        ARCH_COREDUMP_TYPES
 ************************************************************************/

/****************
 *    QDSP6
 ****************/

typedef enum
{
  QDSP6_GP_R0 = 0,
  QDSP6_GP_R1,
  QDSP6_GP_R2,
  QDSP6_GP_R3,
  QDSP6_GP_R4,
  QDSP6_GP_R5,
  QDSP6_GP_R6,
  QDSP6_GP_R7,
  QDSP6_GP_R8,
  QDSP6_GP_R9,
  QDSP6_GP_R10,
  QDSP6_GP_R11,
  QDSP6_GP_R12,
  QDSP6_GP_R13,
  QDSP6_GP_R14,
  QDSP6_GP_R15,
  QDSP6_GP_R16,
  QDSP6_GP_R17,
  QDSP6_GP_R18,
  QDSP6_GP_R19,
  QDSP6_GP_R20,
  QDSP6_GP_R21,
  QDSP6_GP_R22,
  QDSP6_GP_R23,
  QDSP6_GP_R24,
  QDSP6_GP_R25,
  QDSP6_GP_R26,
  QDSP6_GP_R27,
  QDSP6_GP_R28,
  QDSP6_SP,
  QDSP6_FP,
  QDSP6_LR,
  QDSP6_PC,
  QDSP6_USR,
  QDSP6_UGP,
  QDSP6_GP,
  QDSP6_LC0,
  QDSP6_LC1,
  QDSP6_SA0,
  QDSP6_SA1,
  QDSP6_M0,
  QDSP6_M1,
  QDSP6_P,
  QDSP6_SGP,
  QDSP6_SSR,
  QDSP6_IMASK,
  QDSP6_BADVA,
  QDSP6_ELR,
  QDSP6_TID,
  QDSP6_EVB,
  QDSP6_IPEND,
  QDSP6_SYSCFG,
  QDSP6_MODECTL,
  QDSP6_REV,
  QDSP6_TLBHI,
  QDSP6_TLBLO,
  QDSP6_TLBIDX,
  QDSP6_DIAG,
  QDSP6_IAD,
  QDSP6_IEL,
  QDSP6_IAHL,
  QDSP6_PCYCLEHI,
  QDSP6_PCYCLELO,
  SIZEOF_ARCH_COREDUMP_REGISTERS
} arch_coredump_register_type;


typedef struct
{
  uint32 regs[SIZEOF_ARCH_COREDUMP_REGISTERS];
} arch_coredump_array_type;

typedef struct
{
  uint32 gp_regs[29];
  uint32 sp;
  uint32 fp;
  uint32 lr;
  uint32 pc;
  uint32 usr;
  uint32 ugp;
  uint32 gp;
  uint32 lc0;
  uint32 lc1;
  uint32 sa0;
  uint32 sa1;
  uint32 m0;
  uint32 m1;
  uint32 p;
  uint32 sgp;
  uint32 ssr;
  uint32 imask;
  uint32 badva;
  uint32 elr;
  uint32 tid;
  uint32 evb;
  uint32 ipend;
  uint32 syscfg;
  uint32 modectl;
  uint32 rev;
  uint32 tlbhi;
  uint32 tlblo;
  uint32 tlbidx;
  uint32 diag;
  uint32 iad;
  uint32 iel;
  uint32 iahl;
  uint32 pcyclehi;
  uint32 pcyclelo;
} arch_coredump_field_type;

union arch_coredump_union
{
  uint32 array[SIZEOF_ARCH_COREDUMP_REGISTERS];
  arch_coredump_field_type name;
};


#define SIZEOF_ALLOCATED_COREDUMP_REG ( sizeof(uint32) * QDSP6_USR)

#endif /* ERRLOG_Q6_H */
