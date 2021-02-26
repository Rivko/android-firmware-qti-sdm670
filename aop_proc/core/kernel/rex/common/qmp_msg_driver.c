
/*===========================================================================
                              qmp_msg_driver.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.



  @file
    qmp_msg_driver.c

    Contains middleware to implement IPC over qmp protocol.

===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "qmp_msg_driver.h"
#include "rex.h"
#include "CoreVerify.h"
#include "aop_ulog.h"
#include "aop_services.h"
#include "aop_interrupt_table.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define LINK_APPS_DOWN 0x1
#define LINK_APPS_CONN 0x2
#define APPS_LDISC 0x4
#define APPS_RDISC 0x8

#define RX_CB 0x10
#define TX_CB 0x20
#define ABORT 0x40

#define LINK_MPSS_DOWN 0x80
#define LINK_MPSS_CONN 0x100
#define MPSS_LDISC 0x200
#define MPSS_RDISC 0x400

#define LINK_SLPI_DOWN 0x800
#define LINK_SLPI_CONN 0x1000
#define SLPI_LDISC 0x2000
#define SLPI_RDISC 0x4000

#define LINK_ADSP_DOWN 0x8000
#define LINK_ADSP_CONN 0x10000
#define ADSP_LDISC 0x20000
#define ADSP_RDISC 0x40000

#define LINK_CDSP_DOWN 0x80000
#define LINK_CDSP_CONN 0x100000
#define CDSP_LDISC 0x200000
#define CDSP_RDISC 0x400000

#define QMP_AOP_ALIGN8(_size_) (((_size_) + 0x7) & (~0x7))
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct class_2_cb_maptype
{
  cb_func_type cb;
  char* class_string; //cb and class map
}class_cb_maptype;

typedef struct _msg_pool
{
  EDEGES edge;
  char msg[MAX_MSG_LEN]; //json
  const void* ptr; //qmp mailbox pointer
}_msg_pool;

struct map_head
{
  class_cb_maptype* map_begin; //map of class strs and cbs
  int map_size;
  int reqcount;
  int fr_ind; //front index
  int r_ind; //rear index
  _msg_pool* msg_pool;
}_qmp_map_head;

/* glink configuration*/
typedef struct 
{
  glink_handle_type   handle;
  char                name[GLINK_CH_NAME_LEN];
  char			    remote_ss[GLINK_CH_NAME_LEN];
}glink_config;



//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

glink_config glink_ctx[NUM_REMOTES];
glink_link_notify_type  link_cb[NUM_REMOTES];

static rex_crit_sect_type msg_lock;
static glink_open_config_type  config[NUM_REMOTES];
static _msg_pool* alloc_msg_pool;
static rex_tcb_type 	glink_tcb;


//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
msgResult _qmp_map_expand_add(cb_func_type cb,char* class_string);
msgResult _find_space_enQ(const char* test_class_str, size_t size, EDEGES e, const void *mailbox_ptr);
void aop_services_new_message(void);
int get_context_from_priv(const void *priv);
bool _isQfull();
void _expandQ();
void _insertQ(const char* data, size_t size, EDEGES e, const void *mailbox_ptr);
int _deQueue(void);



//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
void glink_event_set(rex_sigs_type sig)
{
  rex_set_sigs(&glink_tcb, sig); 
}

void glink_event_wait_clear(rex_sigs_type sig)
{
  rex_wait(sig);
  rex_clr_sigs(&glink_tcb, sig);
}

/************Callback functions for glink link state change********************/

void glink_link_apps_state_cb()
{
  glink_event_set(LINK_APPS_DOWN);
  glink_err_type result = glink_open(&config[_APPS], &(glink_ctx[_APPS].handle));
  if (result != GLINK_STATUS_SUCCESS)
  {
    return;
  }  
}

void glink_link_mpss_state_cb()
{
  glink_event_set(LINK_MPSS_DOWN);
  glink_err_type result = glink_open(&config[_MPSS], &(glink_ctx[_MPSS].handle));
  if (result != GLINK_STATUS_SUCCESS)
  {
     return;
  }
}

#ifndef SDX24_STUBS
void glink_link_slpi_state_cb()
{
  glink_event_set(LINK_SLPI_DOWN);
  glink_err_type result = glink_open(&config[_SLPI], &(glink_ctx[_SLPI].handle));
  if (result != GLINK_STATUS_SUCCESS)
  {
   return;
  }
}

void glink_link_adsp_state_cb()
{
  glink_event_set(LINK_ADSP_DOWN);
  glink_err_type result = glink_open(&config[_ADSP], &(glink_ctx[_ADSP].handle));
  if (result != GLINK_STATUS_SUCCESS)
  {
   return;
  }
}

void glink_link_cdsp_state_cb()
{
  glink_event_set(LINK_CDSP_DOWN);
  glink_err_type result = glink_open(&config[_CDSP], &(glink_ctx[_CDSP].handle));
  if (result != GLINK_STATUS_SUCCESS)
  {
     return;
  }
}
#endif
/** 
 * <!-- glink_rx_cb() -->
 *
 * @brief callback for rx
 *
 * @param handle        : opaque glink handle
 * @param priv          : priv context of the link
 * @param pkt_priv      : unused
 * @param ptr           : pointer to msg in mailbox
 * @param size          : size of the message
 * @param intent_used   : size of the mailbox 
 * @return void
 */
void glink_rx_cb(glink_handle_type handle,
                    const void        *priv,
                    const void        *pkt_priv,
                    const void        *ptr,
                    size_t            size,
                    size_t            intent_used)
{
  //get which edge message came over
  int i = get_context_from_priv(priv);
  CORE_VERIFY(size <= MAX_MSG_LEN);
  if(size > MAX_MSG_LEN)
  {
     SWEVENT(ERROR_MSG_Q_LEN);
     CORE_VERIFY(0);
  }

  CORE_VERIFY( i < NUM_OF_SUBSYSTEMS);
  //enqueue
  _find_space_enQ((char*)ptr, QMP_AOP_ALIGN8(size), i, ptr);

  //Inform thread to read message queue
  aop_services_new_message();

  //send rx done to remote
  //glink_rx_done(handle, ptr, 1); //Try and call after servicing message
  glink_event_set(RX_CB);
}


/** 
 * <!-- glink_tx_cb() -->
 *
 * @brief callback for tx
 *
 * @param handle    : opaque glink handle
 * @param priv      : priv context of the link
 * @param pkt_priv  : unused
 * @param ptr       : pointer to msg in mailbox
 * @param size      : size of the message
 * 
 * @return void
 */
void glink_tx_cb(glink_handle_type handle,
                    const void        *priv,
                    const void        *pkt_priv,
                    const void        *ptr,
                    size_t            size)
{
    glink_event_set(TX_CB);
}

/** 
 * <!-- get_context_from_priv() -->
 *
 * @brief callback for tx
 *
 * @param priv : private context pointer used for comparison
 *
 * @return ind : index to lookup in subsystem enum
 */
int get_context_from_priv(const void *priv)
{
  uint32 i;
  for (i = 0; i < NUM_REMOTES; i++)
  {
    if (config[i].priv == priv)
	{
	  return i;
	}
  }
  return -1;
}


/** 
 * <!-- glink_state_cb() -->
 *
 * @brief callback for channel state changes
 *
 * @param handle    : opaque glink handle
 * @param priv      : priv context of the link
 * @param event     : event on the channel
 *
 * @return void : void
 */
void glink_state_cb(glink_handle_type handle,
                        const void *priv,
                        glink_channel_event_type  event)
{
  int ss = get_context_from_priv(priv);
  if (ss != -1) 
  {
    switch (ss) 
    {
#ifndef SDX24_STUBS
    case _CDSP:
      if (GLINK_REMOTE_DISCONNECTED == event)
      {
        glink_close((glink_ctx[_CDSP].handle));
      }
     break;
    case _ADSP:
      if (GLINK_REMOTE_DISCONNECTED == event)
      {
        glink_close((glink_ctx[_ADSP].handle));
      }
      break;
    case _SLPI:
      if (GLINK_REMOTE_DISCONNECTED == event)
      {
        glink_close((glink_ctx[_SLPI].handle));
      }
      break;
#endif
    case _MPSS:
      if (GLINK_REMOTE_DISCONNECTED == event)
      {
        glink_close((glink_ctx[_MPSS].handle));
      }
      break;
    }
  }
}


/** 
 * <!-- glink_tx_abort() -->
 *
 * @brief callback for ch abort
 *
 * @param handle    : opaque glink handle
 * @param priv      : priv context of the link
 * @param pkt_priv     : unused
 *
 * @return void : void
 */
void glink_tx_abort(glink_handle_type  handle,
                        const void         *priv,
                        const void         *pkt_priv)
{
  glink_event_set(ABORT); 
}


void _glink_open_ports(void)
{
  int i = 0;
  glink_err_type          result;

  

  
  /* Set glink configuration */
  strlcpy(link_cb[_APPS].remote_ss, "apps", GLINK_MAX_HOST_NAME);
  strlcpy(link_cb[_MPSS].remote_ss, "mpss", GLINK_MAX_HOST_NAME);
#ifndef SDX24_STUBS
  strlcpy(link_cb[_SLPI].remote_ss, "slpi", GLINK_MAX_HOST_NAME);
  strlcpy(link_cb[_CDSP].remote_ss, "cdsp", GLINK_MAX_HOST_NAME);
  strlcpy(link_cb[_ADSP].remote_ss, "adsp", GLINK_MAX_HOST_NAME);
#endif
  link_cb[_APPS].cb = glink_link_apps_state_cb;
  link_cb[_MPSS].cb = glink_link_mpss_state_cb;
#ifndef SDX24_STUBS
  link_cb[_SLPI].cb = glink_link_slpi_state_cb;
  link_cb[_CDSP].cb = glink_link_cdsp_state_cb;
  link_cb[_ADSP].cb = glink_link_adsp_state_cb;
#endif
  for (i = 0; i < NUM_REMOTES; i++) 
  {
      /* Set glink open configuration */
      config[i].name                 = glink_ctx[i].name;
      config[i].remote_ss            = glink_ctx[i].remote_ss;
      config[i].priv                 = &glink_ctx[i];
      config[i].notify_rx            = glink_rx_cb;
      config[i].notify_state         = glink_state_cb;
      config[i].notify_tx_abort      = glink_tx_abort;
      config[i].notify_tx_done       = glink_tx_cb;
      glink_open(&config[i], &(glink_ctx[i].handle));
          /* Register link state callback*/
          glink_register_notify_clients(&link_cb[i]);
      }
    

#if 0
clean_up:
    /* Close the channel */
    result = glink_close(glink_ctx.handle);
    if(GLINK_STATUS_SUCCESS != result){
        return;
    }

    /* wait for local disconnected */
    glink_ut_event_wait_clear(LDISC);
    return;
#endif
}


/** 
 * <!-- _glink_init() -->
 *
 * @brief Initializes glink layer
 *
 *  Setup ports on aop to listen to msgs from apps, mpss and
 *  other ss
 *
 *  
 *  @param void : void
 *
 *  @return void
 */
void _glink_init()
{
  /* setup */
  strlcpy(glink_ctx[_APPS].name,"aop_apps", GLINK_CH_NAME_LEN);
  strlcpy(glink_ctx[_APPS].remote_ss,"apps", GLINK_CH_NAME_LEN);

  strlcpy(glink_ctx[_MPSS].name,"aop_mpss", GLINK_CH_NAME_LEN);
  strlcpy(glink_ctx[_MPSS].remote_ss,"mpss", GLINK_CH_NAME_LEN);

#ifndef SDX24_STUBS
  strlcpy(glink_ctx[_SLPI].name,"aop_slpi", GLINK_CH_NAME_LEN);
  strlcpy(glink_ctx[_SLPI].remote_ss,"slpi", GLINK_CH_NAME_LEN);

  strlcpy(glink_ctx[_ADSP].name,"aop_adsp", GLINK_CH_NAME_LEN);
  strlcpy(glink_ctx[_ADSP].remote_ss,"adsp", GLINK_CH_NAME_LEN);

  strlcpy(glink_ctx[_CDSP].name,"aop_cdsp", GLINK_CH_NAME_LEN);
  strlcpy(glink_ctx[_CDSP].remote_ss,"cdsp", GLINK_CH_NAME_LEN);
#endif
  _glink_open_ports();   
}


/** 
 * <!-- qmp_msg_init() -->
 *
 * @brief Initializes msg driver
 *
 *  Initializes msg driver, and allocates map_head and msg
 *  queues. Opens ports on QMP transport to listen on Q6 edges.
 *  
 *
 *  @param void : void
 *
 *  @return void
 */
void qmp_msg_init(void)
{  
  _glink_init();
  rex_init_crit_sect(&msg_lock);
  int ind = 0;

  //allocate map head
  _qmp_map_head.map_begin = (class_cb_maptype*)malloc(INIT_MAP_SIZE * sizeof(class_cb_maptype));
  CORE_VERIFY(_qmp_map_head.map_begin);
  _qmp_map_head.map_size = INIT_MAP_SIZE;

  //allocate msg string pool
  _qmp_map_head.msg_pool = (_msg_pool*)malloc(NUM_OUTSTANDING_MSG * sizeof(_msg_pool));
  CORE_VERIFY(_qmp_map_head.msg_pool);
  _qmp_map_head.fr_ind = 0;
  _qmp_map_head.r_ind = -1;
  _qmp_map_head.reqcount = 0;

  while(ind < _qmp_map_head.map_size) 
  {
    // clear out the mem
    _qmp_map_head.map_begin[ind].cb = NULL;
    _qmp_map_head.map_begin[ind].class_string = NULL;
    ind++;
  }
}

/** 
 * <!-- qmp_msg_register_cb() -->
 *
 * @brief Register a callback 
 *
 *  Registers a callback of the form void ()(char*) against a
 *  null terminated class value string.
 *  The registered cb will persist until it is unregistered
 * 
 *
 *  @param cb : callback function ptr void (*cb)(char*)
 *  @param class_string : value of the class in the passed
 *                      message
 *
 *  @return status result
 */
msgResult qmp_msg_register_cb(cb_func_type cb, char* class_string)
{
  int ind = 0;
  class_cb_maptype* qmp_cb_map = _qmp_map_head.map_begin;
  // find if the class already exists in the map
  while(ind < _qmp_map_head.map_size && qmp_cb_map[ind].class_string != NULL) 
  {
    if(strcmp(qmp_cb_map[ind].class_string, class_string) == 0)
    {
      qmp_cb_map[ind].cb = cb;
      return MSG_SUCESS;
    }
    ind++;
  }

  //we are still here, must add the new value to the map
  if (ind == _qmp_map_head.map_size)
  {
    //expand map and add
    return _qmp_map_expand_add(cb, class_string);
  }
  else
  {
    // add in the empty slot
    qmp_cb_map[ind].cb = cb;
    qmp_cb_map[ind].class_string = class_string;
    return MSG_SUCESS;
  }
}


/** 
 * <!-- _qmp_map_expand_add() -->
 *
 * @brief Expand the map and add passed params @ end of the map
 *
 *  internal use only
 *
 *  @param cb : callback function ptr void (*cb)(char*)
 *  @param class_string : value of the class in the passed
 *                      message
 *
 *  @return status result
 */
msgResult _qmp_map_expand_add(cb_func_type cb, char* class_string)
{
  _qmp_map_head.map_begin =  (class_cb_maptype*) realloc((void*)_qmp_map_head.map_begin,
                                                         (sizeof(class_cb_maptype)* _qmp_map_head.map_size + 1));
  SWEVENT(QMP_MSG_DRVR_MAP_EXPAND, _qmp_map_head.map_size);
  CORE_VERIFY(_qmp_map_head.map_begin);
  
  // add them at the end
  _qmp_map_head.map_begin[_qmp_map_head.map_size].cb = cb;
  _qmp_map_head.map_begin[_qmp_map_head.map_size].class_string = class_string;
  // update size
  _qmp_map_head.map_size++;
    return MSG_SUCESS;
}

/** 
 * <!-- _find_space_enQ() -->
 *
 * @brief Find a space in the msg pool and insert the msg in the
 *        queue
 *
 *  internal use only
 *
 *  @param class_string : json recieved
 *  @param size:          length of class string
 *
 *  @return status result
 */
msgResult _find_space_enQ(const char* test_class_str, size_t size, EDEGES e, const void* mailbox_ptr)
{
  
  _insertQ(test_class_str, size, e, mailbox_ptr);
  
  return MSG_SUCESS;  
}


/** 
 * <!-- _isQfull() -->
 *
 * @brief checks if the msg queue is full
 *
 *  internal use only
 *
 *  @param void:
 *               
 *
 *  @return bool: True if queue is full
 */
bool _isQfull(void)
{
  return (_qmp_map_head.reqcount >= NUM_OUTSTANDING_MSG);
}

/** 
 * <!-- _insertQ() -->
 *
 * @brief Insert given data at the end of the queue. Ignore
 *        messages if Queue is full.
 *
 *  internal use only
 *
 *  @param data: json string recieved
 *  @param size: length of the data
 *  @return void
 */
void _insertQ(const char* data, size_t size, EDEGES e, const void* mailbox_ptr)
{
  if (_isQfull())
  {
    SWEVENT(MSG_Q_OVERFLOW);
    return;
  }
  if (_qmp_map_head.r_ind == NUM_OUTSTANDING_MSG - 1)
  {
      _qmp_map_head.r_ind = -1; //wrap around
  }
  int write_ind = ++_qmp_map_head.r_ind;
  memcpy(_qmp_map_head.msg_pool[write_ind].msg, data, size);
  _qmp_map_head.msg_pool[write_ind].edge = e;
  _qmp_map_head.msg_pool[write_ind].ptr = mailbox_ptr;
  _qmp_map_head.reqcount++;

}

/** 
 * <!-- _deQueue() -->
 *
 * @brief Remove given data from the front of the queue
 *
 *  internal use only
 *
 *  @param void
 *
 *  @return int: index in the queue
 */
int _deQueue(void)
{
  int data = _qmp_map_head.fr_ind++;
  if (_qmp_map_head.fr_ind == NUM_OUTSTANDING_MSG)
  {
      _qmp_map_head.fr_ind = 0;
  }

  _qmp_map_head.reqcount--;

  return data;
}

/** 
 * <!-- disable_qmp_callbacks() -->
 *
 * @brief Cant lock in isr context in aop need to disable
 *        interrupts while queue is serviced. Enable them back
 *        after being done.
 *
 * @param disable: bool set to true for disabling
 *
 * @return void
 */
static void disable_qmp_callbacks(boolean disable)
{
  if (disable)
  {
    rex_disable_interrupt(APCS_HLOS_IPC0_IRQ);
	rex_disable_interrupt(MSS_IPC0_IRQ);
#ifndef SDX24_STUBS	
    rex_disable_interrupt(SSC_IPC0_IRQ);
    rex_disable_interrupt(COMPUTE_DSP_IPC0_IRQ);
    rex_disable_interrupt(LPASS_IPC0_IRQ);
#endif
  }
  else
  {
    rex_enable_interrupt(APCS_HLOS_IPC0_IRQ);
    rex_enable_interrupt(MSS_IPC0_IRQ);
#ifndef SDX24_STUBS	
    rex_enable_interrupt(SSC_IPC0_IRQ);
    rex_enable_interrupt(COMPUTE_DSP_IPC0_IRQ);
    rex_enable_interrupt(LPASS_IPC0_IRQ);
#endif
  }
}



/** 
 * <!-- service_msg_queue() -->
 *
 * @brief Service the pending msgs in the workaround Queue by
 *        calling the callback functions
 *
 * @param void
 *
 * @return void
 */
void service_msg_queue(void)
{
  while(_qmp_map_head.reqcount)
  {
    int ind = 0;
    bool found = false;
    if (abort_service_now())
    {
      return;
    }

    rex_enter_crit_sect(&msg_lock);
    // disable the qmp isr interrupt
    disable_qmp_callbacks(true);
    int pool_ind = _deQueue();
    char val[MAX_CLASS_VALUE_LEN];
    json_parser_simple(_qmp_map_head.msg_pool[pool_ind].msg, strlen(_qmp_map_head.msg_pool[pool_ind].msg), "class", strlen("class"), val, MAX_CLASS_VALUE_LEN);
    //search for cb in map
    while(ind < _qmp_map_head.map_size && _qmp_map_head.map_begin[ind].class_string != NULL)
    {
      if(strcmp(_qmp_map_head.map_begin[ind].class_string, val) == 0)
      {
        found = true;
        _qmp_map_head.map_begin[ind].cb(_qmp_map_head.msg_pool[pool_ind].msg, _qmp_map_head.msg_pool[pool_ind].edge);
        glink_rx_done(
            glink_ctx[_qmp_map_head.msg_pool[pool_ind].edge].handle,
            _qmp_map_head.msg_pool[pool_ind].ptr, 
            1);
        break;
      }
      ind++;
    }

    //reenable them before leaving crit section
    disable_qmp_callbacks(false);
    rex_leave_crit_sect(&msg_lock);

    if (!found)
    {
      SWEVENT(CLASS_STRING_NOT_FOUND, ind);
    }
  }

}

