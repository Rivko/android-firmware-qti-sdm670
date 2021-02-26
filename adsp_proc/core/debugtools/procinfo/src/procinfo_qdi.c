/*
#============================================================================
#  Name:
#    procinfo_qdi.c 
#
#  Description:
#    Implements QDI layer for Process Information server that goes to the root image
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contaiprocinfo comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1/debugtools/procinfo/src/procinfo_qdi.c#2 $ 
$DateTime: 2017/09/01 01:33:34 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/18/16   bbhatt  KW fixes.
09/12/16   bbhatt  File created to support Process Information server on User PD.

=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>

#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "tms_utils.h"
#include "tms_utils_msg.h"

#include "procinfo_qdi.h"

struct procinfo_qdi_internal_s
{
  qurt_mutex_t                        mutex;
  qurt_qdi_obj_t                      opener_obj;
  procinfo_qdi_device_p               pd_head;
};

static struct procinfo_qdi_internal_s procinfo_qdi_internal;

int procinfo_qdi_invoke(int client_handle,
                        qurt_qdi_obj_t *pobj,
                        int method,
                        qurt_qdi_arg_t a1,
                        qurt_qdi_arg_t a2,
                        qurt_qdi_arg_t a3,
                        qurt_qdi_arg_t a4,
                        qurt_qdi_arg_t a5,
                        qurt_qdi_arg_t a6,
                        qurt_qdi_arg_t a7,
                        qurt_qdi_arg_t a8,
                        qurt_qdi_arg_t a9);
                        
/* MACROS for locking/unlocking the mutexes */
#define PROCINFO_QDI_MUTEX_INIT()     qurt_pimutex_init(&procinfo_qdi_internal.mutex);
#define PROCINFO_QDI_MUTEX_LOCK()     qurt_pimutex_lock(&procinfo_qdi_internal.mutex);
#define PROCINFO_QDI_MUTEX_UNLOCK()   qurt_pimutex_unlock(&procinfo_qdi_internal.mutex);

/** =====================================================================
 * Function:
 *     procinfo_qdi_release
 *
 * Description: 
 *     Deallocates the specified device handle.
 *
 * Parameters:
 *    obj : Pointer to opener object for the device
 *
 * Returns:
 *    None
 * =====================================================================  */
void procinfo_qdi_release(qurt_qdi_obj_t *obj)
{
  procinfo_qdi_device_p temp, clntobj = (procinfo_qdi_device_p)obj;

  if (NULL == clntobj)
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0, 0, 0);
    return;
  }

  PROCINFO_QDI_MUTEX_LOCK();

  if (NULL == procinfo_qdi_internal.pd_head)
  {
    ERR_FATAL("Release called with empty client list", 0, 0, 0);
    PROCINFO_QDI_MUTEX_UNLOCK();
    return;
  }
  else if (NULL != procinfo_qdi_internal.pd_head && clntobj == procinfo_qdi_internal.pd_head) // single entry in list
  {
    procinfo_qdi_internal.pd_head = clntobj->next;
    free(clntobj);
    PROCINFO_QDI_MUTEX_UNLOCK();
    return;
  }

  for (temp = procinfo_qdi_internal.pd_head; NULL != temp; temp = temp->next)
  {
    if (temp->next == clntobj)
    {
      temp->next = clntobj->next;
      free(clntobj);
      PROCINFO_QDI_MUTEX_UNLOCK();
      return;
    }
  }

  PROCINFO_QDI_MUTEX_UNLOCK();

  /* Should not reach here */
  ERR_FATAL("Release Handle not found in the list", 0, 0, 0);
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_open
 *
 * Description: 
 *     Opens a new qdi driver and obtains a specified handle
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *
 * Returns:
 *     QURT_EOK (0) or QURT_EFATAL(-1)
 * =====================================================================  */
static int procinfo_qdi_open(int client_handle)
{
  int rc = QURT_EFATAL;
  procinfo_qdi_device_p clntobj = calloc(1, sizeof(procinfo_qdi_device_t));
  procinfo_qdi_device_data_p dataobj;
  qurt_sysenv_procname_t pn = { 0 };

  if (NULL == clntobj)
  {
    ERR_FATAL("calloc failed", 0, 0, 0);
  }

  /* QDI_OS_SYSENV is the standard method for requesting this type of information */
  /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
  /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested 
   * -- to fill in a qurt_sysenv_procname_t structure */
  /* &pn is the structure we want filled in */
  if (0 > (rc = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV,
                                       QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)))
  {
    TMS_MSG_HIGH_2("Cannot obtain client info %d %d", pn.asid, rc);
    return rc;
  }

  PROCINFO_QDI_MUTEX_LOCK();
  
  clntobj->qdiobj.invoke    = procinfo_qdi_invoke;
  clntobj->qdiobj.refcnt    = QDI_REFCNT_INIT;
  clntobj->qdiobj.release   = procinfo_qdi_release;

  dataobj = &(clntobj->user_pd_data);

  /* Determine if requesting client_handle is local or remote */
  if (client_handle != QDI_HANDLE_LOCAL_CLIENT)
  {
    dataobj->islocal = FALSE;
  }
  else
  {
    dataobj->islocal = TRUE;
  }

  /* Populate PD name and PID */
  strlcpy(dataobj->internal_name, pn.name, PROCINFO_QDI_INTERNAL_NAME_LEN);
  dataobj->pid = pn.asid;

  /* Insert at the head of the list */
  clntobj->next = procinfo_qdi_internal.pd_head;
  procinfo_qdi_internal.pd_head = clntobj;

  PROCINFO_QDI_MUTEX_UNLOCK();

  return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_close
 *
 * Description: 
 *     Closes the specified driver, releasing any resources associated with the open driver.
 *
 * Parameters:
 *     clntobj      : QDI object which represents the client
 *                    which made this QDI request.
 *
 * Returns:
 *     PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 * =====================================================================  */
static int procinfo_qdi_close(procinfo_qdi_device_p clntobj)
{
  int ret = PROCINFO_QDI_FAILURE;

  if (NULL == clntobj)
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0, 0, 0);
  }
  else
  {
    ret = PROCINFO_QDI_SUCCESS;
  }
  
  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_get_name_internal
 *
 * Description:
 *     Gets the requested name of the process
 *
 * Parameters:
 *     name_type        : Value of name type from enum
 *     dest             : Destination buffer to recieve domain name
 *     dest_len         : Length of user buffer
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results
 * =====================================================================  */
static int procinfo_qdi_get_name_internal(int client_handle, procinfo_qdi_device_p clntobj, uint32_t ntype, char *dest, size_t dest_len)
{
  int ret = PROCINFO_QDI_FAILURE;

  procinfo_qdi_device_data_p dataobj;
  char *src_type = NULL;
  size_t src_len = 0;

  PROCINFO_QDI_MUTEX_LOCK();

  dataobj = &(clntobj->user_pd_data);

  switch (ntype)
  {
  case PROCINFO_OEM_NAME_TYPE:
    {
      src_type = dataobj->oem_name;
      src_len = PROCINFO_QDI_OEM_NAME_LEN;
      break;
    }

  case PROCINFO_INTERNAL_NAME_TYPE:
    {
      src_type = dataobj->internal_name;
      src_len = PROCINFO_QDI_INTERNAL_NAME_LEN;
      break;
    }

  case PROCINFO_DOMAIN_NAME_TYPE:
    {
      src_type = dataobj->domain_name;
      src_len = PROCINFO_QDI_DOMAIN_NAME_LEN;
      break;
    }
  default:
    {
      TMS_MSG_ERROR("Wrong name type");
      ret = PROCINFO_QDI_NOT_INITIALIZED;
      PROCINFO_QDI_MUTEX_UNLOCK();
      return ret;
    }
  }

  if ('\0' == src_type[0])
  {
    TMS_MSG_ERROR("Requested name not set");
    ret = PROCINFO_QDI_NOT_INITIALIZED;
    PROCINFO_QDI_MUTEX_UNLOCK();
    return ret;
  }

  /* Determine if requesting client_handle is local or remote */
  if (TRUE != dataobj->islocal)
  {
    if (0 <= qurt_qdi_copy_to_user(client_handle, dest, src_type, src_len))
    {
      ret = PROCINFO_QDI_SUCCESS;
    }
    else
    {
      TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
      PROCINFO_QDI_MUTEX_UNLOCK();
      return ret;
    }
  }
  else /* treat as local */
  {
    if (0 != strlcpy(dest, src_type, dest_len))
    {
      ret = PROCINFO_QDI_SUCCESS;
    }
    else
    {
      TMS_MSG_ERROR("strlcpy failed");
      PROCINFO_QDI_MUTEX_UNLOCK();
      return ret;
    }
  }

  PROCINFO_QDI_MUTEX_UNLOCK();

  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_get_name_from_pid
 *
 * Description: 
 *     Get name of the process using PID used in error exception handler
 *     NOTE: This function must be called from STM only!
 *
 * Parameters:
 *     name_type        : Value of name type from enum
 *     pid              : ASID of the process
 *     dest             : Destination buffer to recieve domain name
 *     dest_len         : Length of user buffer
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
PROCINFO_RESULT procinfo_get_name_from_pid(uint32_t ntype, uint32_t pid, char *dest, size_t dest_len)
{
  int ret = PROCINFO_QDI_FAILURE;

  if (PROCINFO_NAME_TYPES < ntype || NULL == dest || 0 == dest_len)
  {
    ret = PROCINFO_QDI_INVALID_PARAMETERS;
    TMS_MSG_ERROR("Incorrect name type/NULL or empty dest buffer");
    return ret;
  }

  procinfo_qdi_device_p objptr;
  procinfo_qdi_device_data_p dataobj;
  boolean found_pid = FALSE;

  for (objptr = procinfo_qdi_internal.pd_head; NULL != objptr; objptr = objptr->next)
  {
    dataobj = &(objptr->user_pd_data);
    if (dataobj->pid == pid)
    {
      found_pid = TRUE;
      break;
    }
  }

  if (!found_pid)
  {    
    /* Fallback to get name from qurt API */
    qurt_thread_context_get_pname(qurt_thread_get_id(), dest, dest_len);
    
    ret = PROCINFO_QDI_NOT_INITIALIZED;
    return ret;
  }

  char *src_type = NULL;
  switch (ntype)
  {
  case PROCINFO_OEM_NAME_TYPE:
    {
      src_type = dataobj->oem_name;
      break;
    }

  case PROCINFO_INTERNAL_NAME_TYPE:
    {
      src_type = dataobj->internal_name;
      break;
    }

  case PROCINFO_DOMAIN_NAME_TYPE:
    {
      src_type = dataobj->domain_name;
      break;
    }
  default:
    {
      TMS_MSG_ERROR("Wrong name type");
      ret = PROCINFO_QDI_NOT_INITIALIZED;
      return ret;
    }
  }

  if ('\0' == src_type[0])
  {
    if (PROCINFO_INTERNAL_NAME_TYPE == ntype)
    {
      /* Fallback to get name from qurt API */
      qurt_thread_context_get_pname(qurt_thread_get_id(), dest, dest_len);
      ret = PROCINFO_QDI_NOT_INITIALIZED;
    }
    
    return ret;
  }

  /* Local copy name to dest */ 
  if (0 != strlcpy(dest, src_type, dest_len))
  {
    ret = PROCINFO_QDI_SUCCESS;
  }
  else
  {
    TMS_MSG_ERROR("strlcpy failed");
  }

  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_set_name_internal
 *
 * Description:
 *     Sets the provided name for the process
 *
 * Parameters:
 *     name_type        : Value of name type from enum
 *     src              : Source buffer providing domain name
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results
 * =====================================================================  */
static int procinfo_qdi_set_name_internal(int client_handle, procinfo_qdi_device_p clntobj, uint32_t ntype, const char *src)
{
  int ret = PROCINFO_QDI_FAILURE;
  
  procinfo_qdi_device_data_p dataobj;
  char *dest_type = NULL;
  size_t dest_len = 0;
  
  if (0 == strnlen(src, PROCINFO_QDI_OEM_NAME_LEN))
  {
    TMS_MSG_HIGH("Setting empty name"); 
  }
  
  PROCINFO_QDI_MUTEX_LOCK();

  dataobj = &(clntobj->user_pd_data);

  /* Determine if requesting client_handle is local or remote */
  if (TRUE != dataobj->islocal)
  {
    if (PROCINFO_OEM_NAME_TYPE != ntype)
    {
      TMS_MSG_ERROR("User PD can only access OEM name type");
      ret = PROCINFO_QDI_INVALID_OPERATION;
      PROCINFO_QDI_MUTEX_UNLOCK();
      return ret;
    }
    else
    {
      dest_type = dataobj->oem_name;
      dest_len = PROCINFO_QDI_OEM_NAME_LEN; 
      
      /* Clear destination before setting new name */
      memset(dest_type, 0x0, dest_len);
      
      /* Copy to remote client */
      if (0 <= qurt_qdi_copy_from_user(client_handle, dest_type, src, dest_len))
      {
        /* Explicitly set NULL terminator */
        dest_type[dest_len - 1] = '\0';
        ret = PROCINFO_QDI_SUCCESS;
      }
      else
      {
        TMS_MSG_ERROR("qurt_qdi_copy_from_user failed");
        PROCINFO_QDI_MUTEX_UNLOCK();
        return ret;
      }
    }
  }
  else 
  {
    switch (ntype)
    {
    case PROCINFO_OEM_NAME_TYPE:
      {
        dest_type = dataobj->oem_name;
        dest_len = PROCINFO_QDI_OEM_NAME_LEN;
        break;
      }

    case PROCINFO_INTERNAL_NAME_TYPE:
      {
        dest_type = dataobj->internal_name;
        dest_len = PROCINFO_QDI_INTERNAL_NAME_LEN;
        break;
      }

    case PROCINFO_DOMAIN_NAME_TYPE:
      {
        dest_type = dataobj->domain_name;
        dest_len = PROCINFO_QDI_DOMAIN_NAME_LEN;
        break;
      }
     default:
      {
        TMS_MSG_ERROR("Wrong name type.");
        ret = PROCINFO_QDI_NOT_INITIALIZED;
        PROCINFO_QDI_MUTEX_UNLOCK();
        return ret;
      }
    }
    
    /* Clear destination before setting new name */
    memset(dest_type, 0x0, dest_len);
    
    /* Local copy */
    if (0 != strlcpy(dest_type, src, dest_len))
    {
      ret = PROCINFO_QDI_SUCCESS;
    }
    else
    {
      TMS_MSG_ERROR("strlcpy failed");
      PROCINFO_QDI_MUTEX_UNLOCK();
      return ret;
    }
  }
    
  PROCINFO_QDI_MUTEX_UNLOCK();

  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_op
 *
 * Description:
 *     Performs specified operation for the process
 *
 * Parameters:
 *     optype           : Type of operation enum value (get/set name)
 *     ntype            : Value of name type from enum
 *     buf              : Source/destination buffer for name
 *     buflen           : Optional length of buffer
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results
 * =====================================================================  */
static int procinfo_qdi_op(int client_handle, procinfo_qdi_device_p clntobj, uint32_t optype, uint32_t ntype, char* buf, size_t buflen)
{
  int ret = PROCINFO_QDI_FAILURE;

  if (NULL == clntobj)
  {
    ERR_FATAL("QDI layer Misbehaved, Shouldn't reach here ", 0, 0, 0);
    return PROCINFO_QDI_NOT_INITIALIZED;
  }
  
  if (PROCINFO_OP_TYPES < optype || PROCINFO_NAME_TYPES < ntype || NULL == buf)
  {
    ret = PROCINFO_QDI_INVALID_PARAMETERS;
    TMS_MSG_ERROR("Incorrect name type/NULL buffer");
    return ret;
  }
  
  switch (optype)
  {
    case PROCINFO_OP_GET_NAME:
      {
        if (0 != buflen)
        {
          ret = procinfo_qdi_get_name_internal(client_handle, clntobj, ntype, buf, buflen);
        }
        else
        {
          ret = PROCINFO_QDI_INVALID_PARAMETERS;
          TMS_MSG_ERROR("0 destination length given");
          return ret;
        }
        break;
      }
    case PROCINFO_OP_SET_NAME:
      {
        ret = procinfo_qdi_set_name_internal(client_handle, clntobj, ntype, buf);
        break;
      }
  }
  
  return ret;  
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_invoke
 *
 * Description: 
 *     This gives the canonical form for the arguments to a QDI
 *     driver invocation function.
 * 
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.  
 *
 *     qurt_qdi_obj_t : Points at the qdi_object_t structure
 *                      on which this QDI request is being made.
 *
 *     int method     : The integer QDI method which represents
 *                      the request type.
 *
 *     qurt_qdi_arg_t a1 to a3 :  The first three general purpose arguments
 *                                to the invocation function are passed in
 *                                these slots.
 *
 *     qurt_qdi_arg_t a4 to a9 :   Arguments beyond the first three are
 *                                 passed on the stack.
 * Returns:
 *     -1 for failure and 0 for success
 *     Refer to the enum PROCINFO_RESULT for list of possible results
 * =====================================================================  */
int procinfo_qdi_invoke(int client_handle,
                        qurt_qdi_obj_t *pobj,
                        int method,
                        qurt_qdi_arg_t a1,
                        qurt_qdi_arg_t a2,
                        qurt_qdi_arg_t a3,
                        qurt_qdi_arg_t a4,
                        qurt_qdi_arg_t a5,
                        qurt_qdi_arg_t a6,
                        qurt_qdi_arg_t a7,
                        qurt_qdi_arg_t a8,
                        qurt_qdi_arg_t a9)
{
  int ret = PROCINFO_QDI_FAILURE;

  /* Cannot be used for QDI_OPEN */
  procinfo_qdi_device_p clntobj = (procinfo_qdi_device_p)pobj;

  switch (method)
  {
  case QDI_OPEN:
    {
      ret = procinfo_qdi_open(client_handle);
      break;
    }

  case QDI_CLOSE:
    {
      if (PROCINFO_QDI_SUCCESS != procinfo_qdi_close(clntobj))
      {
        TMS_SHUTDOWN_MSG_HIGH("Close failed for procinfo");
      }
      ret = qurt_qdi_method_default(client_handle, pobj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      break;
    }
    
  case PROCINFO_QDI_OP:
    {
      ret = procinfo_qdi_op(client_handle, clntobj, a1.num, a2.num, a3.ptr, a4.num);
      break;
    }

  default:
    {
      ret = qurt_qdi_method_default(client_handle, pobj, method,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9);
      break;
    }
  }

  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_init
 *
 * Description:
 *     Initialization function. Registers a QDI device with the generic QDI object
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void procinfo_qdi_init(void)
{
  int ret = -1;
  qurt_qdi_obj_t *opener = &procinfo_qdi_internal.opener_obj;

  /* Initialize the mutex here */
  PROCINFO_QDI_MUTEX_INIT();

  PROCINFO_QDI_MUTEX_LOCK();

  procinfo_qdi_internal.pd_head = NULL;

  opener->invoke    = procinfo_qdi_invoke;
  opener->refcnt    = QDI_REFCNT_INIT;
  opener->release   = procinfo_qdi_release;

  /* Register the driver */
  ret = qurt_qdi_devname_register(PROCINFO_DRIVER_NAME, opener);
  if (ret != 0)
  {
    ERR_FATAL("qurt_qdi_devname_register failed %d", ret, 0, 0);
  }

  PROCINFO_QDI_MUTEX_UNLOCK();
} /* procinfo_qdi_init */

