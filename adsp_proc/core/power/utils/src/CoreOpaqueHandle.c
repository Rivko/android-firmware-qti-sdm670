/*==============================================================================
@file CoreOpaqueHandle.c

This provides a handle to arbitrary objects that can not be directly
de-referenced.  In order to retrieve the actual pointer, you must
dereference the handle though this table.

The intent for this handle type is when you do not wish to expose raw
pointers directly to clients, typically for security reasons. This
might be used to expose handles across processor boundaries or from
kernel to user space.

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreOpaqueHandle.c#1 $
==============================================================================*/
#include "CoreOpaqueHandle.h"
#include "CoreVerify.h"
#include "CoreString.h"
#include "MemBarrier.h"
#include "DALSys.h"

/* CoreOpaqueHandles are stored in an array of arrays.  The
 * CoreOpaqueHandle itself is essentially an index into the array.
 */
#define NUM_HANDLES 32

/* The handle table is the second level object.  This holds 32 local
 * handles, along with a mask that shows */
typedef struct CoreOpaqueHandleTable
{
  unsigned int  activeMask;    /* Bit mask saying which handles are valid */
  void         *handles[NUM_HANDLES];
} CoreOpaqueHandleTable;

typedef struct CoreOpaqueHandleState
{
  DALSYSSyncHandle        lock;
  DALSYSSyncObj           lockObj;
  unsigned int            numTables;
  CoreOpaqueHandleTable **tables;
} CoreOpaqueHandleState;

static CoreOpaqueHandleState state;

/* Utility Functions */
// From: http://tekpool.wordpress.com/category/bit-count/
static int BitCount(unsigned int u)
{
  unsigned int uCount;

  uCount = u
    - ((u >> 1) & 033333333333)
    - ((u >> 2) & 011111111111);
  return
    ((uCount + (uCount >> 3))
     & 030707070707) % 63;
}

static int First0Bit(int i)
{
  i=~i;
  return BitCount((i&(-i))-1);
}

static void CoreOpaqueHandle_Lock( CoreOpaqueHandleState *state )
{
  DALSYS_SyncEnter(state->lock);
}

static void CoreOpaqueHandle_Unlock( CoreOpaqueHandleState *state )
{
  DALSYS_SyncLeave(state->lock);
}

static void CoreOpaqueHandle_IncreaseHandleTables( 
  CoreOpaqueHandleState *state, 
  unsigned int num_new_tables )
{
  CoreOpaqueHandleTable **old_tables, **new_tables = 0;
  unsigned int i, size;
  unsigned int num_old_tables;

  /* Add a back pointer to the head */
  num_new_tables++;

  CoreOpaqueHandle_Lock( state );

  num_old_tables = state->numTables;
  old_tables     = state->tables;

  /* Check for a race on increasing the tables
   *
   * If so, just bail - we're good*/
  if ( num_new_tables <= num_old_tables ) 
  {
    CoreOpaqueHandle_Unlock( state );
    return;
  }
  
  size = num_new_tables*sizeof(CoreOpaqueHandleTable *);
  CORE_VERIFY( (DALSYS_Malloc( size, (void **)&new_tables )
                == DAL_SUCCESS)
               && (NULL != new_tables) );  /* Klocworks */

  memset( new_tables, 0, size );
  core_memcpy( new_tables, num_old_tables*sizeof(CoreOpaqueHandleTable *), 
	     old_tables, num_old_tables*sizeof(CoreOpaqueHandleTable *) );		  
  
  /* First entry is a back link to the old tables - this means the
   * first table entry can't be used, but it keeps us from leaking
   * memory.  Note that we never actually free the old_table pointer -
   * this is because there's no easy way to be sure that no other
   * thread is still using the old_table without adding some sort of
   * synchronization to the RetrieveHandle() path, and that wants to
   * be fast, but we don't (technically) leak it either since we retain
   * the reference to it.
   *
   * As long as we always double the table count on each allocation,
   * this will use memory equivalent in the limit to using 16-deep
   * handle tables instead of 32.*/
  new_tables[0] = (CoreOpaqueHandleTable *)old_tables;

  for( i = num_old_tables; i < num_new_tables; i++ )
  {
    CORE_DAL_VERIFY( DALSYS_Malloc( sizeof( CoreOpaqueHandleTable ), 
			( void ** )&new_tables[i] ) );
	memset( new_tables[i], 0, sizeof( CoreOpaqueHandleTable ) );
  }
   
   
  state->tables    = new_tables;
  CORE_MEM_BARRIER();           /* state->tables must be comitted
                                 * before the numTables is updated */
  state->numTables = num_new_tables;


  CoreOpaqueHandle_Unlock( state );
}

/* Public Interface */

void CoreOpaqueHandle_Init( void )
{
  if ( NULL == state.lock )
  {
    CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                                      &state.lock, &state.lockObj ) );

    CoreOpaqueHandle_IncreaseHandleTables( &state, 1 );
  }
}

CoreOpaqueHandle CoreOpaqueHandle_AddHandle( void *handle )
{
  unsigned int i, index;
  unsigned int new_handle = 0;

  if ( NULL == handle ) { return NULL; }

  CoreOpaqueHandle_Lock( &state );
  /* Start at 1 since the first entry in tables is not a link to a
   * table, but a back link to previous table arrays. This has the
   * nice side effect of making the NULL Opaque handle never be
   * created */
  for (i = 1; i < state.numTables; i++ )
  {
    if ( 0xffffffff != state.tables[i]->activeMask )
    {
      index = First0Bit( state.tables[i]->activeMask );
      CORE_VERIFY(index < NUM_HANDLES);
      state.tables[i]->activeMask     |= (1 << index);
      state.tables[i]->handles[index]  = handle;

      new_handle = ( i << 5 ) | index;
      break;
    }
  }

  CoreOpaqueHandle_Unlock( &state );

  if ( 0 == new_handle )
  {
    /* double the number of actual table entries - subtract the back link*/
    CoreOpaqueHandle_IncreaseHandleTables( &state, 2*(state.numTables-1));
    return CoreOpaqueHandle_AddHandle( handle );
  }

  return (CoreOpaqueHandle) new_handle;
}

void *CoreOpaqueHandle_RetrieveHandle( CoreOpaqueHandle handle )
{
  unsigned int  index        = (unsigned int)handle;
  unsigned int  table_index  = (index >> 5);
  unsigned int  handle_index = (index & 0x1f);
  void         *native_handle = NULL;

  if ( ( table_index > 0 ) && ( table_index < state.numTables ) &&
       ( state.tables[table_index]->activeMask & (1 << handle_index) ) )
  {
    native_handle = state.tables[table_index]->handles[handle_index];
  }

  return native_handle;
}

void *CoreOpaqueHandle_RemoveHandle( CoreOpaqueHandle handle )
{
  unsigned int  index        = (unsigned int)handle;
  unsigned int  table_index  = (index >> 5);
  unsigned int  handle_index = (index & 0x1f);
  void         *native_handle = NULL;

  if ( ( table_index > 0 ) && ( table_index < state.numTables ) &&
       ( state.tables[table_index]->activeMask & (1 << handle_index) ) )
  {
    state.tables[table_index]->activeMask &= ~(1 << handle_index);
    native_handle = state.tables[table_index]->handles[handle_index];
    state.tables[table_index]->handles[handle_index] = NULL;
  }
  return native_handle;
}

/* Test Functions - not static, but not part of the public API either*/
unsigned int CoreOpaqueHandle_NumTables( void )
{
  return state.numTables;
}
