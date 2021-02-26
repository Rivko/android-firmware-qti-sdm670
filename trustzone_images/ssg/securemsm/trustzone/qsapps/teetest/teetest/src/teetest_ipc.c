/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stringl.h>
#include "object.h"
#include "ObjectTable.h"
#include "teetest.h"
#include "teetest_ipc.h"
#include "teetest_public.h"
#include "teetest_mmu.h"
#include "teetest_object_table.h"


uintptr_t ipc_addr;
size_t    ipc_size;
uintnt    smc[MAX_SMC_ARGS] = {0};

static int writeBufs(int ndx, const int ndxEnd, Tee_ObjectArg* const ptr_phys_args, const ObjectArg* const args, Tee_ObjectBuf phys_arg); 
static int readBufs(int ndx, const int ndxEnd, const Tee_ObjectArg* const ptr_phys_args, ObjectArg* const args);
static int readOutBufs(int ndx, const int ndxEnd, const Tee_ObjectArg* const ptr_phys_args, ObjectArg* const args); 
static int readObjs(int ndx, const int ndxEnd, const Tee_ObjectArg* const ptr_phys_args, ObjectArg* const args); 
static int writeObjs(int ndx, const int ndxEnd, Tee_ObjectArg* const ptr_phys_args, const ObjectArg* const args);

/*
 *  Transfer buffer pointers and contents into physical memory
 */
static int writeBufs(
  int ndx,
  const int ndxEnd,
  Tee_ObjectArg* const ptr_phys_args,
  const ObjectArg* const args,
  Tee_ObjectBuf phys_arg
)
{
  for (; ndx < ndxEnd; ndx++)
  {
    memset(&(ptr_phys_args[ndx].b), 0, sizeof(ptr_phys_args[ndx].b));
    
    ptr_phys_args[ndx].b.ptr = phys_arg.ptr;
    ptr_phys_args[ndx].b.size = args[ndx].b.size;

    /* Valid to pass NULL ptr for out buffer case */
    if(args[ndx].b.ptr != NULL)
    {
      memscpy (phys_arg.ptr, phys_arg.size, args[ndx].b.ptr, args[ndx].b.size);
    }

    phys_arg.ptr += args[ndx].b.size;
  }
  return Object_OK;
}


/*
 *  Transfer buffer pointers from physical memory
 */
static int readBufs(
  int ndx,
  const int ndxEnd,
  const Tee_ObjectArg* const ptr_phys_args,
  ObjectArg* const args
)
{

  /* transfer the args into the physical buffer */
  for (; ndx < ndxEnd; ndx++)
  {
    args[ndx].b.ptr  = ptr_phys_args[ndx].b.ptr;
    args[ndx].b.size = ptr_phys_args[ndx].b.size;

    /* Map in the memory in to the TeeTest address space */
    tee_mmu_map ((unsigned int)args[ndx].b.ptr, args[ndx].b.size);
  }

  return Object_OK;
}
/*
 *  Transfer the contents of filled output buffers from physical into virtual mem.
 */
static int readOutBufs(
  int ndx, 
  const int ndxEnd, 
  const Tee_ObjectArg* const ptr_phys_args, 
  ObjectArg* const args
)
{
  /* For each arg */
  for (; ndx < ndxEnd; ndx++)
  {
    // out buffer means ptr_phys_args[ndx].b.ptr_l provided by remote
    if(ptr_phys_args[ndx].b.ptr != NULL)
    {
      memscpy(args[ndx].b.ptr, args[ndx].b.size, ptr_phys_args[ndx].b.ptr, ptr_phys_args[ndx].b.size);
    }
    else
    {
      return Object_ERROR;
    }
  }

  return Object_OK;
}

/*
 *  Transfer returned objects from to TEE from physical to virtual mem.
 */
static int readObjs(
  int ndx, 
  const int ndxEnd, 
  const Tee_ObjectArg* const ptr_phys_args, 
  ObjectArg* const args
)
{
  /* For each arg */
  for (; ndx < ndxEnd; ndx++)
  {
    args[ndx].o.context = ptr_phys_args[ndx].o.context;

    /* We should not get nothing here */
    if(ptr_phys_args[ndx].o.invoke == USEROBJECT_INVOKE_NULL)
    {
      return Object_ERROR_REMOTE;
    }
    else
    {
      args[ndx].o.invoke = (ObjectInvoke)tee_ipc_outbound;
    }
  }

  return Object_OK;
}

/*
 * Transfer objects for return to from TEE from virtual to physical mem.
 */
static int writeObjs(
  int ndx, 
  const int ndxEnd, 
  Tee_ObjectArg* const ptr_phys_args, 
  const ObjectArg* const args
)
{
  int ret = Object_OK;

  /* For each arg */
  for (; ndx < ndxEnd; ndx++)
  {
    Object o = args[ndx].o;
    if ( !Object_isNull(o) )
    {
      intptr_t object_handle = tee_object_table_add(o);
      memset(&(ptr_phys_args[ndx].o), 0, sizeof(ptr_phys_args[ndx].o));
      
      if (object_handle < 0)
      {
        // Error allocating an entry in the object table
        ret = Object_ERROR_NOSLOTS;
        ptr_phys_args[ndx].o.invoke = USEROBJECT_INVOKE_NULL;
      }
      else
      {
        ptr_phys_args[ndx].o.invoke = USEROBJECT_INVOKE_OUTBOUND;
        ptr_phys_args[ndx].o.context = (ObjectCxt) object_handle;
      }
    }
  }

  return ret;
}   

/*
 * Assemble an outbound message
 *
 */
int tee_ipc_outbound(
  const ObjectCxt ctx, 
  const ObjectOp op, 
  ObjectArg *args, 
  const ObjectCounts k
)
{
  int    ret = Object_ERROR;
  uint32_t flags;

  Tee_ObjectArg *ptr_phys_args  = (Tee_ObjectArg *) (ipc_addr);
  Tee_ObjectBuf phys_arg = {0};

  phys_arg.ptr = (void *)(ptr_phys_args + ObjectCounts_total(k));
  phys_arg.size = (ipc_addr + ipc_size - (uintptr_t) phys_arg.ptr);

  ret = writeBufs(ObjectCounts_indexBI(k), ObjectCounts_indexOI(k), ptr_phys_args, args, phys_arg);

  if(ret == Object_OK)
  {
    /* Post an outbound object */
    smc[0] = (uintnt)TEE_IPC_OUTBOUND_SMC_ID;
    smc[1] = (uintnt)TEE_IPC_OUTBOUND_SMC_PARAM_ID;
    smc[2] = (uintptr_t)ctx;
    smc[3] = (uintnt)op;
    smc[4] = (uintnt)k;
    smc[5] = (uintptr_t)ipc_addr;

    /* Disable interrupts round an outbound call */
    flags = tee_int_disable();

    /* Outbound SMC */
    tee_ipc_outbound_asm((uintptr_t)&smc);

    /* Restore interrupt flags as per state prior to outbound SMC */
    tee_int_restore(flags);

    /* Result code is in smc[1] */
    ret = (int) smc[1];
  }

  if(ret == Object_OK)
  {
    /* Get arg pointer to buffers out */
    ptr_phys_args = (Tee_ObjectArg *)(ipc_addr);
    readOutBufs(ObjectCounts_indexBO(k), ObjectCounts_indexOI(k), ptr_phys_args, args);
  }

  if(ret == Object_OK)
  {
    /* Get arg pointer to objects out */
    ptr_phys_args = (Tee_ObjectArg *)(ipc_addr);
    ret = readObjs(ObjectCounts_indexOO(k), ObjectCounts_total(k), ptr_phys_args, args);
  }
  return ret;
}

/*
 * Disassemble an inbound message
 *
 */
void tee_ipc_inbound(const Tee_ObjectArg* const ptr_phys_args, const ObjectCounts k)
{
  ObjectArg * args = (ObjectArg *)(ptr_phys_args);
  /* convert the input buffers */
  readBufs(ObjectCounts_indexBI(k), ObjectCounts_indexOI(k), ptr_phys_args, args);
}

/* This function is used by the TEE to return from an INBOUND LOADING SMC
 * When it returns, the return value signals the event, and regs contains
 * the NS register.
 */
void tee_ipc_return_from_inbound_loading(const int ret)
{
  smc[0] = TEE_IPC_RETURN_FROM_INBOUND_SMC_ID;
  smc[1] = ret;
  tee_ipc_return_from_inbound_asm((unsigned int)&smc);
}

/* This function is used by the TEE to return from an INBOUND SMC
 * When it returns, the return value signals the event, and regs contains
 * the NS register.
 */
void tee_ipc_return_from_inbound(Tee_ObjectArg *args, const ObjectCounts k, const int ret)
{
  /* Interrupts are always disabled on returning from the TEE */
  (void)tee_int_disable();

  /* Get arg pointer to objects  */
  Tee_ObjectArg * ptr_phys_args = args;
  writeObjs(ObjectCounts_indexOO(k), ObjectCounts_total(k), ptr_phys_args, (ObjectArg *)args);

  smc[0] = TEE_IPC_RETURN_FROM_INBOUND_SMC_ID;
  smc[1] = ret;
  tee_ipc_return_from_inbound_asm((unsigned int)&smc);
}
