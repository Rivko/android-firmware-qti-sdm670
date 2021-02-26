/******************************************************************************
  @file    qmi_mb_common.c
  @brief   The QMI Message Bus common module

  DESCRIPTION
  QMI Message Bus routines.  All publishers/subscribers will be build on top 
  of these routines for initializing, sending and receiving messages.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_mb_fw_init() needs to be called before using the module and before
  adding any transports to this module.
  qmi_mb_init() needs to be called before sending or receiving of any
  service specific messages

  ---------------------------------------------------------------------------
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/qmi/src/qmb/qmi_mb_common.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/13   aep     Initial Creation
===========================================================================*/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include "qmi_mb_common.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/
#define CLIENT_TBL_COUNT (16)
#define CLID_SHIFT        4
#define CLID2IDX(clid) ((clid) & (CLIENT_TBL_COUNT - 1))

#define GET_CLID(clid, svc_id) (CLID2IDX(svc_id) | ((clid) << CLID_SHIFT))
#define CAST_HANDLE_TO_CLID(handle) ((uint32_t)(uintptr_t)(handle))
#define CAST_CLID_TO_HANDLE(clid)   ((qmi_mb_type)(uintptr_t)(clid))
#define USER_HANDLE(handle)         CAST_CLID_TO_HANDLE((handle)->priv.clid)

#define INVALID_CLID   (0)
#define INVALID_HANDLE CAST_CLID_TO_HANDLE(INVALID_CLID)

#define EPUID_TO_EPHANDLE(uid)    ((qmi_mb_ep_type)(uintptr_t)(uid))
#define EPHANDLE_TO_EPUID(handle) (uint32_t)(uintptr_t)(handle)

#define GET_EPID(xid, uid) (((uid) & 0xffff) | ((xid) << 16))
#define GET_UID(epid) ((epid) & 0xffff)
#define GET_XID(epid) ((epid) >> 16)
#define ADDR_EQUALS(addr1, addr2, addr_len) \
  (((addr_len) == 0) || memcmp(addr1, addr2, addr_len) == 0)

#define XPORT_INIT(xp_o, ops_i, xp_data_i, xid_i, user_i) do {  \
  (xp_o)->ops        = ops_i; \
  (xp_o)->xport_data = xp_data_i; \
  (xp_o)->xid        = xid_i; \
  (xp_o)->user       = user_i;  \
  (xp_o)->addr_len   = (ops_i)->addr_len(); \
  (xp_o)->handle     = NULL;  \
  LINK_INIT((xp_o)->link);  \
} while(0)

/*===========================================================================
                      LOCAL DATATYPES
===========================================================================*/
struct qmi_mb_xport_tbl_s
{
  LINK(struct qmi_mb_xport_tbl_s, link);
  qmi_mb_xport_ops_type ops;
  void *xport_data;
  uint32_t xid;
};

struct qmi_mb_handle_store_s
{
  qmi_mb_handle_type *handle;
  LINK(struct qmi_mb_handle_store_s, link);
};

struct qmi_mb_handle_store_list_s
{
  LIST(struct qmi_mb_handle_store_s, list);
};

struct qmi_mb_ep_xport_s
{
  uint32_t xid;
  uint32_t next_uid;
  uint32_t addr_len;
  LINK(struct qmi_mb_ep_xport_s, link);
  LIST(qmi_mb_ep_handle_type, ep_list);
};

/*===========================================================================
                      MODULE GLOBALS
===========================================================================*/

static int qmi_mb_inited = 0;
static qmi_mb_lock_type user_list_lock;

static LIST(struct qmi_mb_xport_tbl_s, g_xports);
static uint32_t next_xid = 1;

static LIST(qmi_mb_handle_type, qmi_mb_tbl)[CLIENT_TBL_COUNT];
static uint32_t next_user_id = 1;

static LIST(struct qmi_mb_ep_xport_s, endpoint_list);
static qmi_mb_lock_type endpoint_list_lock;

/*===========================================================================
                      MODULE INIT FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_mb_fw_init
===========================================================================*/
/*!
@brief

  One time initialization of the QMB stack.

@return
  None  

@note
  This function is NOT re-enterable or thread safe. The only safe place
  to call this is during init.
*/
/*=========================================================================*/
void qmi_mb_fw_init(void)
{
  if(qmi_mb_inited == 0)
  {
    LOCK_INIT(&user_list_lock);
    LOCK_INIT(&endpoint_list_lock);
    qmi_mb_inited = 1;
  }
}

/*===========================================================================
  FUNCTION  qmi_mb_fw_deinit
===========================================================================*/
/*!
@brief

  de-initialization of the QMB stack.

@return
  None  

@note
  This function is NOT re-enterable or thread safe. The only safe place
  to call this is during library deinit.
*/
/*=========================================================================*/
void qmi_mb_fw_deinit(void)
{
  if(qmi_mb_inited)
  {
    LOCK_DEINIT(&endpoint_list_lock);
    LOCK_DEINIT(&user_list_lock);
    qmi_mb_inited = 0;
  }
}

/*===========================================================================
                      USER MANAGEMENT FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_mb_user_lookup
===========================================================================*/
/*!
@brief

  Lookup and return the user structure by taking in the client ID as a key

@return
  
  pointer to the user handle upon success.

@note
  user_list_lock must be held by the caller
*/
/*=========================================================================*/
static qmi_mb_handle_type *qmi_mb_user_lookup(uint32_t clid)
{
  qmi_mb_handle_type *user;
  uint32_t svc_id_hash = clid & (CLIENT_TBL_COUNT - 1);
  LIST_FIND(qmi_mb_tbl[svc_id_hash], user, priv.link, 
              user->priv.clid == clid);
  return user;
}

/*===========================================================================
  FUNCTION  qmi_mb_user_alloc
===========================================================================*/
/*!
@brief

  Allocate a user handle and return its pointer

@return
  
  0 on success, error code on failure.
  pointer to the user handle in out_handle upon success.

@note
  the user handle will be inserted into the client list.
  the pointer MUST be freed using qmi_mb_user_free() only.
*/
/*=========================================================================*/
static int qmi_mb_user_alloc
(
  qmi_idl_service_object_type service_obj,
  qmi_mb_options *options,
  qmi_mb_msg_cb msg_cb,
  void *msg_cb_data,
  qmi_mb_event_cb event_cb,
  void *event_cb_data,
  qmi_mb_handle_type **out_handle
)
{
  qmi_mb_handle_type *handle;
  uint32_t svc_id, maj_vers, max_msg_len;
  struct qmi_mb_xport_tbl_s *xport_tbl;

  *out_handle = NULL;

  if(!qmi_mb_inited)
  {
    return QMI_MB_INTERNAL_ERR;
  }

  if(qmi_idl_get_service_id(service_obj, &svc_id) != QMI_IDL_LIB_NO_ERR)
  {
    return QMI_MB_PARAM_ERR;
  }
  if(qmi_idl_get_idl_version(service_obj, &maj_vers) != QMI_IDL_LIB_NO_ERR)
  {
    return QMI_MB_PARAM_ERR;
  }
  if(qmi_idl_get_max_service_len(service_obj, &max_msg_len) != QMI_IDL_LIB_NO_ERR)
  {
    return QMI_MB_PARAM_ERR;
  }

  handle = CALLOC(1, sizeof(*handle));
  if(!handle)
  {
    return QMI_MB_MEMORY_ERR;
  }

  LINK_INIT(handle->priv.link);
  handle->service_obj = service_obj;
  handle->msg_cb = msg_cb;
  handle->msg_cb_data = msg_cb_data;
  handle->event_cb = event_cb;
  handle->event_cb_data = event_cb_data;
  handle->service_id = svc_id;
  handle->idl_version = maj_vers;
  handle->max_msg_len = max_msg_len;
  handle->release_cb = NULL;
  handle->release_cb_data = NULL;

  /* Setup defaults */
  handle->subgroup = 0;
  handle->has_reg = 0;
  handle->raw = 0;
  handle->rx_enabled = 1;

  /* Setup values passed in the options */
  if(options)
  {
    if((options->options_set & QMI_MB_OPTIONS_SUBGROUP_VALID) != 0)
    {
      handle->subgroup = options->subgroup_id;
    }
    if((options->options_set & QMI_MB_OPTIONS_RAW_FLAG_VALID) != 0)
    {
      handle->raw = 1;
    }
    if((options->options_set & QMI_MB_OPTIONS_DISABLE_RX_VALID) != 0)
    {
      handle->rx_enabled = 0;
    }
  }

  LOCK_INIT(&handle->priv.ref_count_lock);
  LOCK_INIT(&handle->rx_q_lock);
  LOCK_INIT(&handle->xports_lock);

  /* One ref for this call */
  handle->priv.ref_count = 1;

  LOCK(&user_list_lock);
  for(xport_tbl = LIST_HEAD(g_xports); xport_tbl; xport_tbl = xport_tbl->link.next)
  {
    qmi_mb_xport_type *xport = CALLOC(1, sizeof(*xport));
    if(!xport)
    {
      continue;
    }
    XPORT_INIT(xport, &xport_tbl->ops, xport_tbl->xport_data, 
               xport_tbl->xid, handle);
    LIST_ADD(handle->xports, xport, link);
  }

  /* Get a UID for the user */
  handle->priv.clid = INVALID_CLID;
  while(handle->priv.clid == INVALID_CLID)
  {
    handle->priv.clid = GET_CLID(next_user_id, svc_id);
    if(qmi_mb_user_lookup(handle->priv.clid) != NULL)
    {
      handle->priv.clid = INVALID_CLID;
    }
  }
  LIST_ADD(qmi_mb_tbl[CLID2IDX(svc_id)], handle, priv.link);
  UNLOCK(&user_list_lock);
  *out_handle = handle;
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_mb_user_unlink
===========================================================================*/
/*!
@brief

  Unlinks a user handle from the user table

@return
  
  None

@note
  Needs user_list_lock to be held by the caller.
*/
/*=========================================================================*/
static void qmi_mb_user_unlink(qmi_mb_handle_type *handle)
{
  qmi_mb_handle_type *i;
  unsigned int idx;
  if(!handle)
    return;
  idx = CLID2IDX(handle->priv.clid);
  LIST_FIND(qmi_mb_tbl[idx], i, priv.link,
      i->priv.clid == handle->priv.clid);
  if(i)
  {
    LIST_REMOVE(qmi_mb_tbl[idx], handle, priv.link);
  }
}

/*===========================================================================
  FUNCTION  qmi_mb_user_free
===========================================================================*/
/*!
@brief

  Frees a user handle

@return
  
  None

@note
  The user handle will be unlinked if required.
  A user provided release callback will be called.
*/
/*=========================================================================*/
static void qmi_mb_user_free(qmi_mb_handle_type *handle)
{
  qmi_mb_recv_type *ent;
  /* This should fail, but do so just to be sure. */
  LOCK(&user_list_lock);
  qmi_mb_user_unlink(handle);
  UNLOCK(&user_list_lock);

  /* At this point the xports list must be empty */

  LOCK_DEINIT(&handle->priv.ref_count_lock);
  LOCK_DEINIT(&handle->xports_lock);
  LOCK_DEINIT(&handle->rx_q_lock);

  /* Free the RX Q */
  while(NULL != (ent = LIST_HEAD(handle->rx_q)))
  {
    LIST_REMOVE(handle->rx_q, ent, link);
    FREE(ent->c_struct);
    FREE(ent);
  }
  
  if(handle->release_cb)
  {
    handle->release_cb(handle->release_cb_data);
  }
  FREE(handle);
}

/*===========================================================================
  FUNCTION  qmi_mb_user_get_ref_unsafe
===========================================================================*/
/*!
@brief

  Gets a reference on the user.

@return
  QMI_MB_NO_ERR on success error code on failure

@note
  Caller must either:
    a. Hold the user_list_lock or
    b. Must already have a reference for the handle.
*/
/*=========================================================================*/
static qmi_mb_error_type qmi_mb_user_get_ref_unsafe
(
  qmi_mb_handle_type *handle
)
{
  /* This implies that no one shall ever try to acquire user_list_lock
   * while holding ref_count_lock of any user */
  LOCK(&handle->priv.ref_count_lock);
  if(handle->priv.ref_count < 0)
  {
    /* This should never happen, but the check is there
       so I can sleep at night */
    UNLOCK(&handle->priv.ref_count_lock);
    return QMI_MB_INTERNAL_ERR;
  }
  handle->priv.ref_count++;
  UNLOCK(&handle->priv.ref_count_lock);
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_mb_user_get_ref
===========================================================================*/
/*!
@brief

  Gets a reference on the user. If the unlink parameter is set,
  then the user handle will be removed from the global table
  so future calls to qmi_mb_user_get_ref() will fail.

@return
  
  Pointer to the user handle if successful. NULL otherwise.

@note
  The user's reference must be released when one is done with it by
  calling qmi_mb_user_put_ref
*/
/*=========================================================================*/
static qmi_mb_handle_type *qmi_mb_user_get_ref
(
  qmi_mb_type user_handle,
  int unlink
)
{
  uint32_t clid = CAST_HANDLE_TO_CLID(user_handle);
  qmi_mb_handle_type *handle;

  LOCK(&user_list_lock);
  handle = qmi_mb_user_lookup(clid);
  if(!handle)
  {
    UNLOCK(&user_list_lock);
    return NULL;
  }

  if(qmi_mb_user_get_ref_unsafe(handle) != QMI_MB_NO_ERR)
  {
    UNLOCK(&user_list_lock);
    return NULL;
  }

  /* Unlink the client from the global table so future get_ref's fail */
  if(unlink)
    qmi_mb_user_unlink(handle);

  UNLOCK(&user_list_lock);
  return handle;
}

/*===========================================================================
  FUNCTION  qmi_mb_user_put_ref
===========================================================================*/
/*!
@brief

  Releases a reference on the user handle

@return
  
  Current reference count after releasing one's reference.

@note
  The user's structure must NOT be accessed after calling this function.
*/
/*=========================================================================*/
static int qmi_mb_user_put_ref(qmi_mb_handle_type *handle)
{
  int ref = 0;
  if(!handle)
    return 0;
  LOCK(&handle->priv.ref_count_lock);
  ref = --handle->priv.ref_count;
  UNLOCK(&handle->priv.ref_count_lock);
  if(ref == 0)
  {
    qmi_mb_user_free(handle);
  }
  return ref;
}

/*===========================================================================
  FUNCTION  qmi_mb_get_users
===========================================================================*/
/*!
@brief

  Gathers all the users in one list. 
  
@return
  None

@note
  1. A reference is taken for each user and must be released by the caller 
     once done with the list.
  2. Each list element is allocated, and must be released by the user list
  3. Caller must have acquired the users_list_lock
*/
/*=========================================================================*/
static void qmi_mb_get_users
(
  struct qmi_mb_handle_store_list_s *hs
)
{
  int i;
  struct qmi_mb_handle_store_s *handle_store;

  LIST_INIT(hs->list);

  /* Gather all users in a single list after
   * taking a reference. This reference must be
   * released later */
  for(i = 0; i < CLIENT_TBL_COUNT; i++)
  {
    qmi_mb_handle_type *handle;
    for(handle = LIST_HEAD(qmi_mb_tbl[i]); handle; handle = handle->priv.link.next)
    {
      handle_store = CALLOC(1, sizeof(*handle_store));
      if(handle_store)
      {
        LINK_INIT(handle_store->link);
        handle_store->handle = handle;
        if(qmi_mb_user_get_ref_unsafe(handle) != QMI_MB_NO_ERR)
        {
          FREE(handle_store);
          continue;
        }
        LIST_ADD(hs->list, handle_store, link);
      }
    }
  }
}


/*===========================================================================
                      BUFFER MANAGEMENT FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  convert_to_wire
===========================================================================*/
/*!
@brief

  Forms an outbuffer to be sent on the wire.

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type convert_to_wire
(
  qmi_mb_handle_type *user,
  unsigned int msg_id,
  void *c_struct,
  unsigned int c_struct_len,
  void **out_buf,
  unsigned int *out_len
)
{
  uint16_t packed_msg_id = (uint16_t)msg_id;
  uint32_t max_msg_len = c_struct_len; /* If raw */
  uint8_t *buf;
  uint32_t len;
  int enc_rc;

  if(!user->raw)
  {
    uint32_t idl_c_struct_len = 0;

    enc_rc = qmi_idl_get_message_c_struct_len(user->service_obj, QMI_IDL_INDICATION, 
                                                msg_id, &idl_c_struct_len);
    if(enc_rc != QMI_IDL_LIB_NO_ERR || idl_c_struct_len != c_struct_len)
    {
      return QMI_MB_ENCODE_ERR;
    }

    enc_rc = qmi_idl_get_max_message_len(user->service_obj, QMI_IDL_INDICATION, 
                                        msg_id, &max_msg_len);
    if(enc_rc != QMI_IDL_LIB_NO_ERR)
    {
      return QMI_MB_ENCODE_ERR;
    }
  }

  buf = MALLOC(max_msg_len + 2);
  if(!buf)
  {
    return QMI_MB_MEMORY_ERR;
  }

  QMI_MEM_COPY(buf, max_msg_len + 2, &packed_msg_id, 2);

  if(!user->raw)
  {
    enc_rc = qmi_idl_message_encode(
                    user->service_obj,
                    QMI_IDL_INDICATION,
                    msg_id,
                    c_struct,
                    c_struct_len,
                    buf + 2,
                    max_msg_len,
                    &len);
    if(enc_rc != QMI_IDL_LIB_NO_ERR)
    {
      FREE(buf);
      return QMI_MB_ENCODE_ERR;
    }
  }
  else
  {
    QMI_MEM_COPY(buf + 2, max_msg_len, c_struct, max_msg_len);
    len = max_msg_len;
  }

  *out_buf = (void *)buf;
  *out_len = len + 2;
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  convert_from_wire
===========================================================================*/
/*!
@brief

  Forms the buffer requested by the user from the wire packet
  (If the user requires encoding/decoding, the out is a C struct)

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type convert_from_wire
(
  qmi_mb_handle_type *user,
  void *buf,
  unsigned int len,
  unsigned int *out_msg_id,
  void **out_c_struct,
  unsigned int *out_c_struct_len
)
{
  uint16_t msg_id;
  int enc_rc;
  uint32_t c_struct_len;
  void *c_struct = NULL;

  if(len < 2 || !buf || !out_msg_id || !out_c_struct || !out_c_struct_len)
  {
    return QMI_MB_PARAM_ERR;
  }

  QMI_MEM_COPY(&msg_id, sizeof(msg_id), buf, 2);
  len -= 2;

  if(!user->raw)
  {
    enc_rc = qmi_idl_get_message_c_struct_len(user->service_obj, QMI_IDL_INDICATION,
                                              msg_id, &c_struct_len);
    if(enc_rc != QMI_IDL_LIB_NO_ERR)
    {
      return QMI_MB_ENCODE_ERR;
    }
  }
  else
  {
    c_struct_len = len;
  }

  if(c_struct_len > 0)
  {
    c_struct = MALLOC(c_struct_len);
    if(!c_struct)
    {
      return QMI_MB_MEMORY_ERR;
    }

    if(!user->raw)
    {
      enc_rc = qmi_idl_message_decode(
                        user->service_obj,
                        QMI_IDL_INDICATION,
                        (unsigned int)msg_id,
                        ((uint8_t *)buf) + 2,
                        len,
                        c_struct,
                        c_struct_len);
      if(enc_rc != QMI_IDL_LIB_NO_ERR)
      {
        FREE(c_struct);
        return QMI_MB_ENCODE_ERR;
      }
    }
    else
    {
      QMI_MEM_COPY(c_struct, c_struct_len, (uint8_t *)buf + 2, c_struct_len);
    }
  }
  *out_c_struct = c_struct;
  *out_c_struct_len = c_struct_len;
  *out_msg_id = (unsigned int)msg_id;
  return QMI_MB_NO_ERR;
}

/*===========================================================================
                      ENDPOINT MANAGEMENT FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_mb_get_ep
===========================================================================*/
/*!
@brief

  Finds the ep of the xid and address and returns the ep_id. 
  If not found, adds it to endpoint list and returns the EP ID.

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type qmi_mb_get_ep
(
  uint32_t xid,
  void *addr,
  uint32_t addr_len,
  uint32_t *ep_id
)
{
  struct qmi_mb_ep_xport_s *ep_xport;
  qmi_mb_ep_handle_type *ep;

  if(addr_len > MAX_ADDR_LEN)
  {
    return QMI_MB_INVALID_BUS;
  }

  LOCK(&endpoint_list_lock);
  LIST_FIND(endpoint_list, ep_xport, link, ep_xport->xid == xid);
  if(!ep_xport)
  {
    ep_xport = CALLOC(1, sizeof(*ep_xport));
    if(!ep_xport)
    {
      UNLOCK(&endpoint_list_lock);
      return QMI_MB_MEMORY_ERR;
    }
    ep_xport->next_uid = 1;
    LINK_INIT(ep_xport->link);
    LIST_INIT(ep_xport->ep_list);
    ep_xport->xid = xid;
    ep_xport->addr_len = addr_len;
    LIST_ADD(endpoint_list, ep_xport, link);
  }
  else
  {
    /* Xport exists, but the address len is not correct */
    if(ep_xport->addr_len != addr_len)
    {
      UNLOCK(&endpoint_list_lock);
      return QMI_MB_INVALID_BUS;
    }
  }
  LIST_FIND(ep_xport->ep_list, ep, link,
            ADDR_EQUALS(ep->addr, addr, addr_len));
  if(!ep)
  {
    ep = CALLOC(1, sizeof(*ep));
    if(!ep)
    {
      UNLOCK(&endpoint_list_lock);
      return QMI_MB_MEMORY_ERR;
    }
    LINK_INIT(ep->link);
    ep->uid = ep_xport->next_uid++;
    if(addr_len)
      QMI_MEM_COPY(ep->addr, MAX_ADDR_LEN, addr, addr_len);
    LIST_ADD(ep_xport->ep_list, ep, link);
  }
  *ep_id = GET_EPID(xid, ep->uid);
  UNLOCK(&endpoint_list_lock);
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_mb_find_ep
===========================================================================*/
/*!
@brief

  Finds the ep provided by ep ID and if found returns the XID and address
  of the endpoint. 
  out_addr_len as an input is the buffer size of out_addr.
  out_addr_len as an output is the size of the address.

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type qmi_mb_find_ep
(
  uint32_t ep_id,
  uint32_t *out_xid,
  void *out_addr,
  uint32_t *out_addr_len
)
{
  uint32_t uid = ep_id & 0xffff;
  uint32_t xid = (ep_id >> 16) & 0xffff;
  struct qmi_mb_ep_xport_s *ep_xport;
  qmi_mb_ep_handle_type *ep;
  uint32_t max_addr_len = *out_addr_len;

  LOCK(&endpoint_list_lock);
  LIST_FIND(endpoint_list, ep_xport, link, ep_xport->xid == xid);
  if(!ep_xport)
  {
    UNLOCK(&endpoint_list_lock);
    return QMI_MB_INVALID_BUS;
  }
  if(ep_xport->addr_len > max_addr_len)
  {
    UNLOCK(&endpoint_list_lock);
    return QMI_MB_PARAM_ERR;
  }
  LIST_FIND(ep_xport->ep_list, ep, link, ep->uid == uid);
  if(!ep)
  {
    UNLOCK(&endpoint_list_lock);
    return QMI_MB_INVALID_BUS;
  }
  *out_xid = xid;
  *out_addr_len = ep_xport->addr_len;
  if(ep_xport->addr_len)
    QMI_MEM_COPY(out_addr, max_addr_len, ep->addr, ep_xport->addr_len);
  UNLOCK(&endpoint_list_lock);
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_mb_remove_ep
===========================================================================*/
/*!
@brief

  Finds the ep provided by xid and address (and len) and if successful,
  returns the ep_id of the removed endpoint. 
  out_addr_len as an input is the buffer size of out_addr.
  out_addr_len as an output is the size of the address.

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type qmi_mb_remove_ep
(
  uint32_t xid,
  void *addr,
  uint32_t addr_len,
  uint32_t *ep_id
)
{
  qmi_mb_ep_handle_type *ep;
  struct qmi_mb_ep_xport_s *ep_xport;
  qmi_mb_error_type rc = QMI_MB_INVALID_BUS;

  if(addr_len > MAX_ADDR_LEN)
  {
    return QMI_MB_INVALID_BUS;
  }

  LOCK(&endpoint_list_lock);
  LIST_FIND(endpoint_list, ep_xport, link, ep_xport->xid == xid);
  if(!ep_xport || addr_len != ep_xport->addr_len)
  {
    goto bail;
  }
  LIST_FIND(ep_xport->ep_list, ep, link, 
      ADDR_EQUALS(ep->addr, addr, addr_len));
  if(ep)
  {
    *ep_id = ep->uid | (xid << 16);
    LIST_REMOVE(ep_xport->ep_list, ep, link);
    FREE(ep);
    rc = QMI_MB_NO_ERR;
  }
bail:
  UNLOCK(&endpoint_list_lock);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_mb_remove_all_ep
===========================================================================*/
/*!
@brief
  Remove all EPs known over xid.

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static void qmi_mb_remove_all_ep
(
  uint32_t xid
)
{
  qmi_mb_ep_handle_type *ep;
  struct qmi_mb_ep_xport_s *ep_xport;

  LOCK(&endpoint_list_lock);
  LIST_FIND(endpoint_list, ep_xport, link, ep_xport->xid == xid);
  if(!ep_xport)
  {
    UNLOCK(&endpoint_list_lock);
    return;
  }
  LIST_REMOVE(endpoint_list, ep_xport, link);
  while(NULL != (ep = LIST_HEAD(ep_xport->ep_list)))
  {
    LIST_REMOVE(ep_xport->ep_list, ep, link);
    FREE(ep);
  }
  FREE(ep_xport);
  UNLOCK(&endpoint_list_lock);
}

/*===========================================================================
                USER FACING PUBLIC METHODS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_mb_init_internal
===========================================================================*/
static qmi_mb_error_type
qmi_mb_init_internal
(
 qmi_idl_service_object_type service_obj,
 qmi_mb_options *options,
 qmi_mb_msg_cb msg_cb,
 void *msg_cb_data,
 qmi_mb_event_cb event_cb,
 void *event_cb_data,
 qmi_mb_type *user_handle
)
{
  int rc;
  qmi_mb_handle_type *handle;
  qmi_mb_xport_type *xport;
  qmi_mb_type ret_user_handle;

  if(!service_obj || !user_handle)
  {
    return QMI_MB_PARAM_ERR;
  }
  *user_handle = INVALID_HANDLE;

  rc = qmi_mb_user_alloc(service_obj,
                         options,
                         msg_cb,
                         msg_cb_data,
                         event_cb,
                         event_cb_data,
                         &handle);
  if(rc != QMI_MB_NO_ERR)
  {
    return rc;
  }

  ret_user_handle = USER_HANDLE(handle);

  LOCK(&handle->xports_lock);
  xport = LIST_HEAD(handle->xports);
  while(xport)
  {
    /* Get a ref for each xport */
    qmi_mb_user_get_ref_unsafe(handle);

    xport->handle = 
      xport->ops->open(xport->xport_data, xport,
                       handle->service_id,
                       handle->idl_version, 
                       handle->subgroup, 
                       handle->max_msg_len);
    if(!xport->handle)
    {
      (void)qmi_mb_user_put_ref(handle);
      continue;
    }
    xport = xport->link.next;
  }
  UNLOCK(&handle->xports_lock);

  (void)qmi_mb_user_put_ref(handle);
  *user_handle = ret_user_handle;
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_mb_init
===========================================================================*/
qmi_mb_error_type
qmi_mb_init
(
 qmi_idl_service_object_type service_obj,
 qmi_mb_options *options,
 qmi_mb_msg_cb msg_cb,
 void *msg_cb_data,
 qmi_mb_type *user_handle
)
{
  if(!msg_cb)
  {
    return QMI_MB_PARAM_ERR;
  }

  return qmi_mb_init_internal(service_obj, options, msg_cb, msg_cb_data,
                              NULL, NULL, user_handle);
}

/*===========================================================================
  FUNCTION  qmi_mb_create
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_create
(
 qmi_idl_service_object_type service_obj,
 qmi_mb_options *options,
 qmi_mb_event_cb event_cb,
 void *event_cb_data,
 qmi_mb_type *user_handle
)
{
  if(!event_cb)
  {
    return QMI_MB_PARAM_ERR;
  }

  return qmi_mb_init_internal(service_obj, options, NULL, NULL,
                              event_cb, event_cb_data, user_handle);
}

/*===========================================================================
  FUNCTION  qmi_mb_release
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_release
(
 qmi_mb_type         user_handle,
 qmi_mb_release_cb   release_cb,
 void                *release_cb_data
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_xport_type *xport;

  handle = qmi_mb_user_get_ref(user_handle, 1);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }
  handle->release_cb = release_cb;
  handle->release_cb_data = release_cb_data;

  LOCK(&handle->xports_lock);
  while(NULL != (xport = LIST_HEAD(handle->xports)))
  {
    LIST_REMOVE(handle->xports, xport, link);
    /* Call close with lock not held */
    UNLOCK(&handle->xports_lock);
    if(xport->handle)
    {
      xport->ops->close(xport->handle);
    }
    else
    {
      FREE(xport);
    }
    LOCK(&handle->xports_lock);
  }
  UNLOCK(&handle->xports_lock);

  (void)qmi_mb_user_put_ref(handle);
  return QMI_MB_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_mb_join
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_join
(
 qmi_mb_type user_handle
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_xport_type *xport;
  qmi_mb_error_type rc;

  handle = qmi_mb_user_get_ref(user_handle, 0);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }
  if(handle->has_reg)
  {
    rc = QMI_MB_NO_ERR;
    goto bail;
  }
  
  rc = QMI_MB_INTERNAL_ERR;
  LOCK(&handle->xports_lock);
  for(xport = LIST_HEAD(handle->xports); xport; xport = xport->link.next)
  {
    if(xport->handle && xport->ops->reg(xport->handle) == QMI_MB_NO_ERR)
    {
      rc = QMI_MB_NO_ERR;
    }
  }
  handle->has_reg = 1;
  UNLOCK(&handle->xports_lock);
bail:
  (void)qmi_mb_user_put_ref(handle);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_mb_leave
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_leave
(
 qmi_mb_type user_handle
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_xport_type *xport;
  qmi_mb_error_type rc;

  handle = qmi_mb_user_get_ref(user_handle, 0);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }

  if(!handle->has_reg)
  {
    rc = QMI_MB_NO_ERR;
    goto bail;
  }
  
  rc = QMI_MB_INTERNAL_ERR;
  LOCK(&handle->xports_lock);
  for(xport = LIST_HEAD(handle->xports); xport; xport = xport->link.next)
  {
    if(xport->handle && xport->ops->unreg(xport->handle) == QMI_MB_NO_ERR)
    {
      rc = QMI_MB_NO_ERR;
    }
  }
  handle->has_reg = 0;
  UNLOCK(&handle->xports_lock);
bail:
  (void)qmi_mb_user_put_ref(handle);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_mb_publish
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_publish
(
 qmi_mb_type                     user_handle,
 unsigned int                    msg_id,
 void                            *c_struct,
 unsigned int                    c_struct_len
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_xport_type *xport;
  qmi_mb_error_type rc;
  void *buf = NULL;
  unsigned int buf_len = 0;

  handle = qmi_mb_user_get_ref(user_handle, 0);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }

  rc = convert_to_wire(handle, msg_id, c_struct, c_struct_len,
                          &buf, &buf_len);
  if(rc != QMI_MB_NO_ERR)
  {
    (void)qmi_mb_user_put_ref(handle);
    return rc;
  }

  rc = QMI_MB_INTERNAL_ERR;
  LOCK(&handle->xports_lock);
  for(xport = LIST_HEAD(handle->xports); xport; xport = xport->link.next)
  {
    if(xport->handle &&
       xport->ops->send(xport->handle, NULL, buf, buf_len) == QMI_MB_NO_ERR)
    {
      /* Pub on at least 1 succeeded */
      rc = QMI_MB_NO_ERR;
    }
  }
  UNLOCK(&handle->xports_lock);

  FREE(buf);
  (void)qmi_mb_user_put_ref(handle);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_mb_send_msg
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_send_msg
(
 qmi_mb_type                     user_handle,
 qmi_mb_ep_type                  ep_handle,
 unsigned int                    msg_id,
 void                            *c_struct,
 unsigned int                    c_struct_len
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_xport_type *xport;
  qmi_mb_error_type rc;
  uint8_t addr[MAX_ADDR_LEN];
  void *buf;
  unsigned int buf_len;
  uint32_t xid;
  uint32_t addr_len = MAX_ADDR_LEN;

  handle = qmi_mb_user_get_ref(user_handle, 0);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }

  rc = qmi_mb_find_ep(EPHANDLE_TO_EPUID(ep_handle), 
                     &xid, addr, &addr_len);
  if(rc != QMI_MB_NO_ERR)
  {
    goto put_ref_bail;
  }

  rc = convert_to_wire(handle, msg_id, c_struct, c_struct_len,
                          &buf, &buf_len);
  if(rc != QMI_MB_NO_ERR)
  {
    goto put_ref_bail;
  }

  /* Find the xid and addr of the endpoint */
  LOCK(&handle->xports_lock);
  LIST_FIND(handle->xports, xport, link, xport->xid == xid);
  if(!xport || xport->addr_len != addr_len)
  {
    rc = QMI_MB_INTERNAL_ERR;
    goto unlock_bail;
  }
  rc = xport->ops->send(xport->handle, addr, buf, buf_len);
unlock_bail:
  UNLOCK(&handle->xports_lock);
  FREE(buf);
put_ref_bail:
  (void)qmi_mb_user_put_ref(handle);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_mb_recv_msg
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_recv_msg
(
 qmi_mb_type                     user_handle,
 qmi_mb_ep_type                  *ep_handle,
 unsigned int                    *msg_id,
 void                            **c_struct,
 unsigned int                    *c_struct_len
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_recv_type *recv = NULL;
  qmi_mb_error_type rc = QMI_MB_EAGAIN_ERR;

  if(!msg_id || !c_struct || !c_struct_len)
  {
    return QMI_MB_PARAM_ERR;
  }

  handle = qmi_mb_user_get_ref(user_handle, 0);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }

  LOCK(&handle->rx_q_lock);
  recv = LIST_HEAD(handle->rx_q);
  if(recv)
  {
    LIST_REMOVE(handle->rx_q, recv, link);
    LINK_INIT(recv->link);
    /* Added to the free list to ease debugging
     * when users do not call qmi_mb_free_msg */
    LIST_ADD(handle->free_list, recv, link);
    if(ep_handle)
      *ep_handle = recv->sender;
    *msg_id       = recv->msg_id;
    *c_struct     = recv->c_struct;
    *c_struct_len = recv->c_struct_len;
    rc = QMI_MB_NO_ERR;
  }
  UNLOCK(&handle->rx_q_lock);

  (void)qmi_mb_user_put_ref(handle);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_mb_free_msg
===========================================================================*/
QMI_EXPORT qmi_mb_error_type
qmi_mb_free_msg
(
 qmi_mb_type                     user_handle,
 void                            *c_struct,
 unsigned int                    c_struct_len
)
{
  qmi_mb_handle_type *handle;
  qmi_mb_recv_type *recv = NULL;
  qmi_mb_error_type rc = QMI_MB_INVALID_MEM_ERR;

  if(!c_struct || !c_struct_len)
  {
    return QMI_MB_PARAM_ERR;
  }

  handle = qmi_mb_user_get_ref(user_handle, 0);
  if(!handle)
  {
    return QMI_MB_PARAM_ERR;
  }

  LOCK(&handle->rx_q_lock);
  LIST_FIND(handle->free_list, recv, link, recv->c_struct == c_struct);
  if(recv)
  {
    LIST_REMOVE(handle->free_list, recv, link);
    FREE(recv->c_struct);
    FREE(recv);
    rc = QMI_MB_NO_ERR;
  }
  UNLOCK(&handle->rx_q_lock);

  (void)qmi_mb_user_put_ref(handle);
  return rc;
}

/*===========================================================================
                TRANSPORT FACING PUBLIC METHODS
===========================================================================*/

/*=============================================================================
  FUNCTION qmi_mb_xport_start
=============================================================================*/
void qmi_mb_xport_start
(
 qmi_mb_xport_ops_type *ops,
 void *xport_data
 )
{
  struct qmi_mb_xport_tbl_s *xport_tbl;
  struct qmi_mb_handle_store_list_s to_start_list;
  struct qmi_mb_handle_store_s *handle_store;
  uint32_t xid;

  LOCK(&user_list_lock);
  for(xport_tbl = LIST_HEAD(g_xports); xport_tbl; xport_tbl = xport_tbl->link.next)
  {
    if(memcmp(&xport_tbl->ops, ops, sizeof(*ops)) == 0 
        && xport_tbl->xport_data == xport_data)
    {
      UNLOCK(&user_list_lock);
      return;
    }
  }
  xport_tbl = CALLOC(1, sizeof(*xport_tbl));
  if(!xport_tbl)
  {
    UNLOCK(&user_list_lock);
    return;
  }
  xport_tbl->ops = *ops;
  xport_tbl->xport_data = xport_data;
  LINK_INIT(xport_tbl->link);
  xid = xport_tbl->xid = next_xid++;
  LIST_ADD(g_xports, xport_tbl, link);

  qmi_mb_get_users(&to_start_list);

  UNLOCK(&user_list_lock);

  while(NULL != (handle_store = LIST_HEAD(to_start_list.list)))
  {
    qmi_mb_xport_type *xport;
    qmi_mb_handle_type *handle = handle_store->handle;
    LIST_REMOVE(to_start_list.list, handle_store, link);
    FREE(handle_store);

    xport = CALLOC(1, sizeof(*xport));
    if(!xport)
    {
      /* Release ref taken by qmi_mb_get_users */
      (void)qmi_mb_user_put_ref(handle); 
      continue;
    }

    LOCK(&handle->xports_lock);
    XPORT_INIT(xport, &xport_tbl->ops, xport_tbl->xport_data,
               xid, handle);
    LIST_ADD(handle->xports, xport, link);

    /* Get a reference for the xport */
    qmi_mb_user_get_ref_unsafe(handle);

    /* Auto open */
    xport->handle = ops->open(xport->xport_data, xport, handle->service_id,
                              handle->idl_version, handle->subgroup,
                              handle->max_msg_len);
    if(!xport->handle)
    {
      /* If open failed, release the reference taken for the xport */
      (void)qmi_mb_user_put_ref(handle);

      UNLOCK(&handle->xports_lock);
      /* Release ref taken by qmi_mb_get_users */
      (void)qmi_mb_user_put_ref(handle);
      continue;
    }

    /* Auto register */
    if(handle->has_reg)
    {
      (void)ops->reg(xport->handle);
    }
    UNLOCK(&handle->xports_lock);
    /* Release ref taken by qmi_mb_get_users */
    (void)qmi_mb_user_put_ref(handle);
  }
}

/*=============================================================================
  FUNCTION qmi_mb_xport_stop
=============================================================================*/
void qmi_mb_xport_stop
(
 qmi_mb_xport_ops_type *ops,
 void *xport_data
 )
{
  struct qmi_mb_xport_tbl_s *xport_tbl;
  struct qmi_mb_handle_store_list_s to_stop_list;
  struct qmi_mb_handle_store_s *handle_store;
  qmi_mb_xport_type *xport;
  uint32_t xid;


  LOCK(&user_list_lock);
  for(xport_tbl = LIST_HEAD(g_xports); xport_tbl; xport_tbl = xport_tbl->link.next)
  {
    if(memcmp(&xport_tbl->ops, ops, sizeof(*ops)) == 0 && 
        xport_tbl->xport_data == xport_data)
    {
      LIST_REMOVE(g_xports, xport_tbl, link);
      break;
    }
  }
  if(!xport_tbl)
  {
    UNLOCK(&user_list_lock);
    return;
  }
  xid = xport_tbl->xid;

  qmi_mb_get_users(&to_stop_list);

  UNLOCK(&user_list_lock);

  qmi_mb_remove_all_ep(xid);

  /* auto remove xport from each user */
  while(NULL != (handle_store = LIST_HEAD(to_stop_list.list)))
  {
    qmi_mb_handle_type *handle = handle_store->handle;
    LIST_REMOVE(to_stop_list.list, handle_store, link);
    FREE(handle_store);

    LOCK(&handle->xports_lock);
    LIST_FIND(handle->xports, xport, link, xport->xid == xid);
    if(xport)
    {
      LIST_REMOVE(handle->xports, xport, link);
      UNLOCK(&handle->xports_lock);
      if(xport->handle)
      {
        ops->close(xport->handle);
      }
      else
      {
        FREE(xport);
      }
    }
    else
    {
      UNLOCK(&handle->xports_lock);
    }

    /* Release ref taken by qmi_mb_get_users */
    (void)qmi_mb_user_put_ref(handle);
  }

  FREE(xport_tbl);
}

/*=============================================================================
  FUNCTION qmi_mb_xport_subscriber_event
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a subscriber event has occured

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.
@param[in]   event_type         Join or Leave?

@retval      QMI_MB_NO_ERR      Success
*/
/*=========================================================================*/
static qmi_mb_error_type qmi_mb_xport_subscriber_event
(
 void *xport_handle,
 void *addr,
 qmi_mb_event_type event_type
 )
{
  qmi_mb_xport_type *xport = (qmi_mb_xport_type *)xport_handle;
  qmi_mb_handle_type *user;
  uint32_t ep_id = 0;
  qmi_mb_error_type rc;

  if(!xport)
    return QMI_MB_PARAM_ERR;

  if(xport->addr_len > 0 && !addr)
    return QMI_MB_PARAM_ERR;
  
  user = xport->user;
  if(!user)
    return QMI_MB_PARAM_ERR;

  rc = qmi_mb_get_ep(xport->xid, addr, xport->addr_len, &ep_id);
  if(rc != QMI_MB_NO_ERR)
  {
    return rc;
  }

  if(user->event_cb)
  {
    user->event_cb(USER_HANDLE(user), event_type, 
        EPUID_TO_EPHANDLE(ep_id), user->event_cb_data);
  }
  return QMI_MB_NO_ERR;
}


/*=============================================================================
  FUNCTION qmi_mb_xport_join
=============================================================================*/
qmi_mb_error_type qmi_mb_xport_join
(
 void *xport_handle,
 void *addr
 )
{
  return qmi_mb_xport_subscriber_event(xport_handle, addr, QMI_MB_EVENT_JOIN);
}

/*=============================================================================
  FUNCTION qmi_mb_xport_leave
=============================================================================*/
qmi_mb_error_type qmi_mb_xport_leave
(
 void *xport_handle,
 void *addr
 )
{
  return qmi_mb_xport_subscriber_event(xport_handle, addr, QMI_MB_EVENT_LEAVE);
}

/*=============================================================================
  FUNCTION qmi_mb_xport_disconnect
=============================================================================*/
qmi_mb_error_type qmi_mb_xport_disconnect
(
 void *xport_handle,
 void *addr
 )
{
  qmi_mb_xport_type *xport = (qmi_mb_xport_type *)xport_handle;
  qmi_mb_handle_type *user;
  uint32_t ep_id = 0;

  if(!xport || !xport->user)
    return QMI_MB_PARAM_ERR;

  if(xport->addr_len > 0 && !addr)
    return QMI_MB_PARAM_ERR;

  if(qmi_mb_remove_ep(xport->xid, addr, xport->addr_len, &ep_id) != QMI_MB_NO_ERR)
  {
    return QMI_MB_INTERNAL_ERR;
  }

  user = xport->user;

  if(user->event_cb)
  {
    user->event_cb(USER_HANDLE(user), QMI_MB_EVENT_EP_CLOSED, 
                    EPUID_TO_EPHANDLE(ep_id), user->event_cb_data);
  }

  return QMI_MB_NO_ERR;
}

/*=============================================================================
  FUNCTION qmi_mb_xport_recv
=============================================================================*/
qmi_mb_error_type qmi_mb_xport_recv
(
 void *xport_handle,
 void *addr,
 void *buf,
 uint32_t len
 )
{
  qmi_mb_xport_type *xport = (qmi_mb_xport_type *)xport_handle;
  qmi_mb_handle_type *user;
  uint32_t ep_id = 0;
  qmi_mb_error_type rc;
  void *c_struct = NULL;
  unsigned int c_struct_len;
  unsigned int msg_id;

  if(!xport || !buf || !xport->user)
    return QMI_MB_PARAM_ERR;

  if(xport->addr_len > 0 && !addr)
    return QMI_MB_PARAM_ERR;

  user = xport->user;

  rc = qmi_mb_get_ep(xport->xid, addr, xport->addr_len, &ep_id);
  if(rc != QMI_MB_NO_ERR)
  {
    return rc;
  }

  if(!user->rx_enabled)
  {
    return QMI_MB_NO_ERR;
  }

  rc = convert_from_wire(user, buf, len, &msg_id, &c_struct, 
                          &c_struct_len);
  if(rc != QMI_MB_NO_ERR)
  {
    return rc;
  }

  if(user->msg_cb)
  {
    user->msg_cb(USER_HANDLE(user), EPUID_TO_EPHANDLE(ep_id),
                 msg_id, c_struct, c_struct_len, user->msg_cb_data);
    FREE(c_struct);
  }
  else
  {
    qmi_mb_recv_type *ent = CALLOC(1, sizeof(*ent));
    if(!ent)
    {
      FREE(c_struct);
      return QMI_MB_MEMORY_ERR;
    }

    LINK_INIT(ent->link);
    ent->sender = EPUID_TO_EPHANDLE(ep_id);
    ent->msg_id = msg_id;
    ent->c_struct = c_struct;
    ent->c_struct_len = c_struct_len;
    LOCK(&user->rx_q_lock);
    LIST_ADD(user->rx_q, ent, link);
    UNLOCK(&user->rx_q_lock);

    if(user->event_cb)
    {
      user->event_cb(USER_HANDLE(user), QMI_MB_EVENT_MSG, 
                     EPUID_TO_EPHANDLE(ep_id), user->event_cb_data);
    }
  }

  return QMI_MB_NO_ERR;
}

/*=============================================================================
  FUNCTION qmi_mb_xport_closed
=============================================================================*/
void qmi_mb_xport_closed
(
 void *xport_handle
)
{
  qmi_mb_xport_type *xport = (qmi_mb_xport_type *)xport_handle;
  qmi_mb_handle_type *user;
  
  if(!xport || !xport->user)
    return;

  user = xport->user;
  if(!xport->handle)
  {
    /* We have already closed this xport... avoid doing it again */
    return;
  }
  xport->handle = NULL;
  FREE(xport);

  /* Give up the reference taken for this xport */
  (void)qmi_mb_user_put_ref(user);
}


