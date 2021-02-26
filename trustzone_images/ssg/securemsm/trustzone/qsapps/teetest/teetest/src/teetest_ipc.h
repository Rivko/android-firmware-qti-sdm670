/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

#ifndef __TEETEST_IPC_H__
#define __TEETEST_IPC_H__

#include <stddef.h>
#include "object.h"

#if defined(__aarch64__)  
  typedef ObjectArg Tee_ObjectArg;
  typedef ObjectBuf Tee_ObjectBuf;
#else
  /* 
    Compatibility types for 32 bit TEE
    based on Object64, ObjectBuf64 and ObjectArg64
    from /trustzone/qsee/mink/include/object.h 
  */
  typedef struct {
    void  *ptr;
    void  *ptr_h;
    size_t size;
    size_t size_h;
  } ObjectBuf64Compat;
  
  typedef struct {
    ObjectInvoke invoke;
    ObjectInvoke invoke_h;
    ObjectCxt context;
    ObjectCxt context_h;
  } Object64Compat;
   
  typedef union  {
    ObjectBuf64Compat b;
    Object64Compat  o;
  } ObjectArg64Compat;
   
   typedef ObjectArg64Compat Tee_ObjectArg;
   typedef ObjectBuf64Compat Tee_ObjectBuf;
#endif

#define IPC_BUFFER_LEN_KB 4
#define MAX_SMC_ARGS      8

extern uintptr_t ipc_addr;
extern size_t    ipc_size;
extern uintnt    smc[MAX_SMC_ARGS];

int  tee_ipc_outbound(const ObjectCxt ctx, const ObjectOp op, ObjectArg *args, const ObjectCounts k);
void tee_ipc_inbound(const Tee_ObjectArg* const ptr_phys_args, const ObjectCounts k);
void tee_ipc_return_from_inbound_loading(const int ret);
void tee_ipc_return_from_inbound(Tee_ObjectArg *args, const ObjectCounts k, const int ret);

void tee_ipc_return_from_inbound_asm(unsigned int regs);
void tee_ipc_outbound_asm           (unsigned int regs);

#endif // __TEETEST_IPC_H__
