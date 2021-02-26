/*===========================================================================
                              arc_runtime.c

DESCRIPTION:

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "arc_runtime.h"
#include "CoreVerify.h"
#include "arc_hal_hwio.h"
#include "arc_sw_override_hwio.h"
#include "HALhwio.h"
#include "rpmh_arc_mol.h"
#include "pwr_utils_lvl.h"
#include "arc_irq_hwio.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define IRQ_TIMER_EXPIRED_BITMASK (0x4)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

typedef struct
{
  arc_runtime_client_t *clients;
  arc_runtime_state_enum_t state;
  uint32 events;
  uint32 id;
} arc_runtime_t;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static arc_runtime_t arc_runtime[RPMH_ARC_NUM];

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/**
 * <!-- get_sequence_level -->
 *
 * @brief Returns the SEQUENCE_OL for the given ARC.
 *
 * @param id - the ARC instance
 *
 * @return the sequence level
 */
static uint32 get_sequence_level (uint32 arc)
{
  uint32 ret = HWIO_INFI (RMm_OPERATIONAL_LEVEL, arc, SEQUENCE_OL);
#if 0
  uint32 timer_expired = HWIO_INFI (RPMH_ARC_RMm_IRQ_STATUS, arc, RM_IRQ_STATUS) & IRQ_TIMER_EXPIRED_BITMASK;

  uint32 vlvl = rpmh_arc_mol_get (arc);
  int mol = pwr_utils_hlvl (arc, vlvl);
  CORE_VERIFY (mol >= 0);

  // Apply HW workaround if applicable, reset the level back to MOL
  if (timer_expired) 
  {
    if (ret < mol)
  {
    if (timer_expired)
    {
        // Set Sequence level to mol
      HWIO_OUTFI (RMm_OPERATIONAL_LEVEL, arc, SEQUENCE_OL, mol);
      ret = mol;
    }
  }
  }
#endif

  return ret;
}

/**
 * <!-- prework_unblock -->
 *
 * @brief Unblocks the ARC sequencer that's presumed to be waiting on prework 
 *
 * @param arc - the arc id
 *
 * @return none
 */
static void prework_unblock (uint32 arc)
{
  HWIO_OUTI  (RPMH_ARC_RMm_IRQ_CLR, arc, IRQ_TIMER_EXPIRED_BITMASK);
  HWIO_OUTFI (RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm, arc, UNBLOCK_SEQ_PRE, 1);
  HWIO_OUTFI (RPMH_ARC_SW_UNBLOCK_SEQ_PRE_RMm, arc, UNBLOCK_SEQ_PRE, 0);
}

/**
 * <!-- postwork_unblock -->
 *
 * @brief Unblocks the ARC sequencer that's presumed to be waiting on postwork
 *
 * @param arc - the arc id
 *
 * @return none
 */
static void postwork_unblock (uint32 arc)
{
  HWIO_OUTFI (RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm, arc, UNBLOCK_SEQ_POST, 1);
  HWIO_OUTFI (RPMH_ARC_SW_UNBLOCK_SEQ_POST_RMm, arc, UNBLOCK_SEQ_POST, 0);
}

/**
 * <!-- aggregation_unblock -->
 *
 * @brief Unblocks the ARC sequencer that's presumed to be waiting on aggregation
 *
 * @param arc - the arc id
 *
 * @return none
 */
static void aggregation_unblock (uint32 arc, uint32 lvl)
{
  HWIO_OUTI  (RPMH_ARC_RMm_IRQ_CLR, arc, IRQ_TIMER_EXPIRED_BITMASK);
  HWIO_OUTFI (RMm_AGGR_UNBLOCK, arc, RM_AGGR_UNBLOCK_DATA, lvl);
}

/**
 * <!-- arc_runtime_init -->
 *
 * @brief Initializes the ARC runtime module
 *
 * @return none
 */
void arc_runtime_init (void)
{
  for (int i = 0; i < RPMH_ARC_NUM; i++)
  {
    arc_runtime[i].id = i;
  }
}

/**
 * <!-- arc_runtime_register -->
 *
 * @brief Register a callback with ARC runtime. The callback will be invoked depending
 *        on what kind of event the client is interested in (look at event_register).
 *
 * @param arc - the arc id
 * @param cb - the callback
 * @param cb_data - any callback data the client wants as part of the callback
 *
 * @return a pointer to the client handle. This is an opaque handle and must not be modified
 *         by the client.
 */
arc_runtime_client_t *
arc_runtime_register (rpmh_arc_enum_t arc, arc_runtime_cb_t cb, uint32 cb_data)
{
  CORE_VERIFY (arc < RPMH_ARC_NUM);
  arc_runtime_client_t *client = arc_runtime[arc].clients;
  arc_runtime_client_t *prev = NULL;

  while (client != NULL)
  {
    prev = client;
    client = client->next;
  }

  client = malloc (sizeof (*client));
  CORE_VERIFY (client != NULL);
  
  if (prev == NULL)
  {
    arc_runtime[arc].clients = client;
  }
  else
  {
    prev->next = client;
  }
  
  client->arc = arc;
  client->cb_data = cb_data;
  client->cb = cb;
  client->events = 0;
  client->blocked = 0;
  
  return client;
}

/**
 * <!-- arc_runtime_event_register -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
int arc_runtime_event_register (arc_runtime_client_t *client, arc_runtime_event_t event)
{
  CORE_VERIFY (client != NULL);
  CORE_VERIFY (client->arc < RPMH_ARC_NUM);
  CORE_VERIFY (event < ARC_RUNTIME_EVENT_NUM);
  
  client->events |= (1 << event);
  arc_runtime[client->arc].events |= (1 << event);

  switch (event)
  {
    case ARC_RUNTIME_EVENT_AGGREGATION:
      HWIO_OUTI (RMm_AGGR_BLOCK_EN, client->arc, 1);
      break;

    case ARC_RUNTIME_EVENT_PREWORK:
      HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, client->arc, 0x10, 0x10);
      break;

    case ARC_RUNTIME_EVENT_POSTWORK:
      HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, client->arc, 0x20, 0x20);
      break;

    default:
      break;
  }

  return ARC_RUNTIME_SUCCESS;
}

/**
 * <!-- arc_runtime_event_deregister -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
int arc_runtime_event_deregister (arc_runtime_client_t *client, arc_runtime_event_t event)
{
  CORE_VERIFY (client != NULL);
  CORE_VERIFY (client->arc < RPMH_ARC_NUM);
  bool other_client = false;
  
  client->events &= ~(1 << event);
  arc_runtime_client_t *c = arc_runtime[client->arc].clients;
  while ((c != NULL) && (! other_client))
  {
    other_client = (c->events & event);
    c = c->next;
  }
  if (! other_client)
  {
    arc_runtime[client->arc].events &= ~(1 << event);

    switch (event)
    {
      case ARC_RUNTIME_EVENT_AGGREGATION:
        HWIO_OUTI (RMm_AGGR_BLOCK_EN, client->arc, 0); 
        break;

      case ARC_RUNTIME_EVENT_PREWORK:
        HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, client->arc, 0x10, 0);
        break;

      case ARC_RUNTIME_EVENT_POSTWORK:
        HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, client->arc, 0x20, 0);
        break;

      default:
        break;
    }
  }

  return ARC_RUNTIME_SUCCESS;
}

/**
 * <!-- dispatch_event -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static uint32 dispatch_event (arc_runtime_t *pa, arc_runtime_event_t e, uint32 ed,
                              bool *con)
{
  arc_runtime_client_t *client = pa->clients;
  uint32 ret = 0;
  bool c = true;
  *con = true;
  
  while (client != NULL)
  {
    if (client->events & (1 << e))
    {
      uint32 tmp = client->cb (client, e, ed, client->cb_data);

      // Max aggregation
      if ((e == ARC_RUNTIME_EVENT_AGGREGATION) && (tmp > ret))
      {
        ret = tmp;
      }

      *con = *con && !client->blocked;
    }
    client = client->next;
  }

  return ret;
}
                              
/**
 * <!-- event_dispatch -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void event_dispatch (arc_runtime_t *pa, arc_runtime_event_t e)
{
  CORE_VERIFY (pa != NULL);
  CORE_VERIFY (e < ARC_RUNTIME_EVENT_NUM);

  switch (pa->state)
  {
    case ARC_RUNTIME_IDLE:
      if (e == ARC_RUNTIME_EVENT_AGGREGATION)
      {
        // Read sequence level
        uint32 ed = get_sequence_level (pa->id);
        bool con = true;
        
        pa->state = ARC_RUNTIME_WAIT_AGG_UNBLOCK;
        uint32 ret = dispatch_event (pa, e, ed, &con);

        // In case the state changes in the callback
        if (con && (pa->state == ARC_RUNTIME_WAIT_AGG_UNBLOCK))
        {
          pa->state = ARC_RUNTIME_IDLE;
          aggregation_unblock (pa->id, ret);
        }
      }
      else if (e == ARC_RUNTIME_EVENT_PREWORK)
      {
        uint32 ed = get_sequence_level (pa->id);
        bool con = true;
        
        pa->state = ARC_RUNTIME_WAIT_PREWORK_UNBLOCK;
        dispatch_event (pa, e, ed, &con);

        if (con && (pa->state == ARC_RUNTIME_WAIT_PREWORK_UNBLOCK))
        {
          pa->state = ARC_RUNTIME_IDLE;
          prework_unblock (pa->id);
        }
      }
      else if (e == ARC_RUNTIME_EVENT_POSTWORK)
      {
        uint32 ed = get_sequence_level (pa->id);
        bool con = true;
        
        pa->state = ARC_RUNTIME_WAIT_POSTWORK_UNBLOCK;
        dispatch_event (pa, e, ed, &con);
        
        if (con && (pa->state == ARC_RUNTIME_WAIT_POSTWORK_UNBLOCK))
        {
          pa->state = ARC_RUNTIME_IDLE;
          postwork_unblock (pa->id);
        }
      }

    default:
      break;
  }
}

/**
 * <!-- arc_runtime_event_dispatch -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void arc_runtime_event_dispatch (uint32 arc, arc_runtime_event_t e)
{
  CORE_VERIFY (arc < RPMH_ARC_NUM);
  
  event_dispatch (&arc_runtime[arc], e);
}

/**
 * <!-- any_blocks -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static bool any_blocks (arc_runtime_t *pa)
{
  bool blocked = false;
  arc_runtime_client_t *pc = pa->clients;
  
  while ((pc != NULL) && (!blocked))
  {
    blocked = blocked || pc->blocked;
    pc = pc->next;
  }

  return blocked;
}

/**
 * <!-- arc_runtime_client_event_dispatch -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void arc_runtime_client_event_dispatch (arc_runtime_client_t *client,
                                        arc_runtime_event_t e, uint32 ed)
{
  CORE_VERIFY (client != NULL);
  CORE_VERIFY (client->arc < RPMH_ARC_NUM);

  arc_runtime_t *pa = &arc_runtime[client->arc];
  
  switch (pa->state)
  {
    case ARC_RUNTIME_WAIT_AGG_UNBLOCK:
      if (e == ARC_RUNTIME_EVENT_AGG_UNBLOCK)
      {
        client->blocked = false;

        if (! any_blocks (pa))
        {
          pa->state = ARC_RUNTIME_IDLE;
          aggregation_unblock (pa->id, ed);
        }
      }  
      break;

    case ARC_RUNTIME_WAIT_PREWORK_UNBLOCK:
      if (e == ARC_RUNTIME_EVENT_PREWORK_UNBLOCK)
      {
        client->blocked = false;
        
        if (! any_blocks (pa))
        {
          pa->state = ARC_RUNTIME_IDLE;
          prework_unblock (pa->id);
        }
      }
      break;
      
    case ARC_RUNTIME_WAIT_POSTWORK_UNBLOCK:
      if (e == ARC_RUNTIME_EVENT_POSTWORK_UNBLOCK)
      {
        client->blocked = false;

        if (! any_blocks (pa))
        {
          pa->state = ARC_RUNTIME_IDLE;
          postwork_unblock (pa->id);
        }
      }   
      break;

    default:
      break;
  }
}
