#ifndef QURT_QDI_DRIVER_H
#define QURT_QDI_DRIVER_H

/**
  @file qurt_qdi_driver.h
  @brief  Definitions, macros, and prototypes used when writing a
  QDI driver

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/


#include "qurt_qdi.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_imacros.h"

/*
||  This gives the canonical form for the arguments to a QDI
||   driver invocation function.  The arguments are as follows:
||
||   int client_handle    (R0) QDI handle which represents the client
||                             which made this QDI request.  If the
||                             client is remote, this will be a
||                             variable handle; if the client is local
||                             (same thread and process), this will be
||                             set to QDI_HANDLE_LOCAL_CLIENT.
||
||   qurt_qdi_obj_t *obj  (R1) Points at the qdi_object_t structure
||                             on which this QDI request is being made.
||                             The qdi_object_t structure is usually
||                             the first element of a larger structure
||                             which contains state associated with the
||                             object; since it's usually the first
||                             element, the object pointers can be freely
||                             interchanged through casts.
||
||   int method           (R2) The integer QDI method which represents
||                             the request type.
||
||   qurt_qdi_arg_t arg1  (R3) The first three general purpose arguments
||   qurt_qdi_arg_t arg2  (R4)  to the invocation function are passed in
||   qurt_qdi_arg_t arg3  (R5)  these slots.
||
||   qurt_qdi_arg_t arg4  (SP+0)  Arguments beyond the first three are
||   qurt_qdi_arg_t arg5  (SP+4)  passed on the stack.
||   qurt_qdi_arg_t arg6  (SP+8)
||   qurt_qdi_arg_t arg7  (SP+12)
||   qurt_qdi_arg_t arg8  (SP+16)
||   qurt_qdi_arg_t arg9  (SP+20)
||
||  Note that the canonical form of the invocation function takes a
||   total of 12 arguments, but not all of them are used.  In general,
||   the QDI infrastructure will only pass those arguments provided by
||   the caller; if the invocation function accesses additional
||   arguments beyond those provided by the caller, the values will not
||   be useful.
*/

#define QDI_INVOKE_ARGS \
   int, struct qdiobj *, int, \
   qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t, \
   qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t, \
   qurt_qdi_arg_t, qurt_qdi_arg_t, qurt_qdi_arg_t

struct qdiobj;

typedef union {
  void *ptr;
  int num;
} qurt_qdi_arg_t;

typedef int (*qurt_qdi_pfn_invoke_t)(QDI_INVOKE_ARGS);
typedef void (*qurt_qdi_pfn_release_t)(struct qdiobj *);

typedef struct qdiobj {
   qurt_qdi_pfn_invoke_t invoke;
   int refcnt;
   qurt_qdi_pfn_release_t release;
} qurt_qdi_obj_t;

extern int qurt_qdi_obj_ref_inc(qurt_qdi_obj_t *);
extern int qurt_qdi_obj_ref_dec(qurt_qdi_obj_t *);

/**@ingroup driver_support_functions
  Processes a method that is unrecognized or unsupported in the driver invocation function.
  All arguments passed to the current invocation function (Section 3.4.1) must be forwarded
  to this function.

  @note1hang Invocation functions must process all unrecognized or unsupported methods
             by calling this function.

  @return

  @dependencies
  None.
*/
extern int qurt_qdi_method_default(QDI_INVOKE_ARGS);

/**@ingroup driver_support_functions
  Allocates a new device handle for use with the specified driver object.
   
  @param client_handle Obtained from the current invocation function (Section 3.4.1).
  @param obj           Pointer

  @return
  Non-negative integer value that is the new handle -- Success. \n
  Negative return value -- Error.

  @dependencies
  None.
*/
static __inline int qurt_qdi_handle_create_from_obj_t(int client_handle, qurt_qdi_obj_t *obj)
{
   return qurt_qdi_handle_invoke(client_handle,
                                 QDI_CLIENT_HANDLE_HANDLE_CREATE_FROM_OBJ_T,
                                 obj);
}

/**@ingroup driver_support_functions
  Deallocates the specified device handle.

  @param client_handle     Obtained from the current invocation function (Section 3.4.1).
  @param handle_to_release 

  @return 
  0 -- Success. \n
  Negative value -- Error. 

  @dependencies
  None.
*/
static __inline int qurt_qdi_handle_release(int client_handle, int handle_to_release)
{
   return qurt_qdi_handle_invoke(client_handle,
                                 QDI_CLIENT_HANDLE_HANDLE_RELEASE,
                                 handle_to_release);
}

static __inline qurt_qdi_obj_t *
qurt_qdi_objref_get_from_handle(int client_handle, int object_handle)
{
   qurt_qdi_obj_t *ret;

   ret = NULL;

   qurt_qdi_handle_invoke(client_handle,
                          QDI_CLIENT_HANDLE_OBJREF_GET,
                          object_handle,
                          &ret);

   return ret;
}

static __inline qurt_qdi_obj_t *
qurt_qdi_objref_get_from_pointer(qurt_qdi_obj_t *objptr)
{
   if (qurt_qdi_obj_ref_inc(objptr) < 0)
      return NULL;
   return objptr;
}

static __inline void
qurt_qdi_objref_release(qurt_qdi_obj_t *objptr)
{
   if (qurt_qdi_obj_ref_dec(objptr) == 1)
      (*objptr->release)(objptr);
}

void qurt_qdi_safe_to_kill(void);      /* Mark the current thread as safe to be killed with no notification */
void qurt_qdi_unsafe_to_kill(void);    /* Mark the current thread as unsafe to be killed with no notification */
int qurt_qdi_marked_to_kill(void);     /* Poll the current thread for a kill notification (nonzero if thread is being killed) */

/**@ingroup driver_support_functions
  Copies the contents of a user memory buffer into the current driver.

  @note1hang User buffer addresses are valid only for the duration of the current driver
  invocation.

  @param client_handle   Obtained from the current invocation function (Section 3.4.1).
  @param dest            Base address of the driver buffer address.
  @param src             Base address of the user buffer address.
  @param len             Number of bytes to copy.
  
  @return
  Negative error result -- privilege or security violation, the copy operation 
                has crossed a privilege boundary.
  
  @dependencies
  None.
*/
static __inline int qurt_qdi_copy_from_user(int client_handle, void *dest, const void *src, unsigned len)
{
   return qurt_qdi_handle_invoke(client_handle,
                                 QDI_CLIENT_HANDLE_COPY_FROM_USER,
                                 dest, src, len);
}

/**@ingroup driver_support_functions
  Copies the contents of a user memory buffer into the current driver.

  @note1hang User buffer addresses are valid only for the duration of the current driver
  invocation.

  @param client_handle   Obtained from the current invocation function (Section 3.4.1).
  @param dest            Base address of the driver buffer address.
  @param src             Base address of the user buffer address.
  @param len             Number of bytes to copy. NOTE: This is the destination buffer length
  
  @return
  Negative error result -- privilege or security violation, the copy operation 
                has crossed a privilege boundary.
  
  @dependencies
  None.
*/
int qurt_qdi_copy_string_from_user(int client_handle, char *dest, const char *src, unsigned len);

/**@ingroup driver_support_functions
  Copies the contents of a driver memory buffer out to user memory.

  @note1hang User buffer addresses are valid only for the duration of the current driver
             invocation.

  @param client_handle Obtained from the current invocation function (Section 3.4.1).
  @param dest          Base address of the user buffer address.
  @param src           Base address of the driver buffer address.
  @param len           Number of bytes to copy.

  @return
  Negative error -- Privilege or security violation, copy operation has crossed a 
                    privilege boundary

  @dependencies
  None.
*/
static __inline int qurt_qdi_copy_to_user(int client_handle, void *dest, const void *src, unsigned len)
{
   return qurt_qdi_handle_invoke(client_handle,
                                 QDI_CLIENT_HANDLE_COPY_TO_USER,
                                 dest, src, len);
}

/**@ingroup driver_support_functions
  Prepares for the direct manipulation of a possibly-untrusted buffer provided by a QDI
  client.
  This function is used to permit a trusted driver to safely access memory which has been
  provided by a possibly-untrusted client. A driver calls this function to obtain a safe buffer
  pointer for accessing the memory.
  This function performs the following security checks: \n
  -- It verifies that the entire buffer is accessible to the client
  -- It ensures that the pointer remains valid for the remainder of the QDI driver
      operation. \n

  @note1hang  User buffer addresses are valid only for the duration of the current driver
              invocation.

  @param client_handle Obtained from the current invocation function (Section 3.4.1).
  @param buf           Pointer to the base address of the client buffer address.
  @param len           Buffer length (in bytes).
  @param perms         Bitmask value which specifies the read or write access to be performed on the
                       client buffer: \n
                       -- QDI_PERM_R -- Read access \n
                       -- QDI_PERM_W -- Write access \n
                       -- QDI_PERM_RW -- Read/write access \n
  @param obuf          Returns a pointer to the buffer address that the driver must use to access the buffer.

  @return
  Negative error result -- Operation crosses a privilege boundary, indicating a privilege or security violation. \n
  Nonzero result -- User passed a buffer which does not fulfill the requested read/write access permission.
                    In this case the QDI driver call must be terminated cleanly, with an appropriate error code 
                    returned to the client. \n
  Zero function result -- Lock operation was successful. In this case the
                    QDI driver must use the pointer at *obuf to perform its memory accesses, and not the
                    pointer passed in as buf -- even if the user process changes the mapping of memory at buf,
                    the mapping of memory at *obuf will remain valid until the driver invocation completes.

  @dependencies
  None.
*/
static __inline int qurt_qdi_buffer_lock(int client_handle, void *buf, unsigned len,
                                         unsigned perms, void **obuf)
{
   return qurt_qdi_handle_invoke(client_handle,
                                 QDI_CLIENT_HANDLE_BUFFER_LOCK,
                                 buf, len, perms, obuf);
}


/**@ingroup driver_support_fuctions
  Allocates memory area in QDI heap which is read/write accessible to both the driver and
  the client. \n
  @note1hang The QDI heap has only a limited amount of memory available, and only the
  device driver can free the allocated memory.

  @param client_handle Obtained from the current invocation function (Section 3.4.1).
  @param size

  @return
  Non-zero value pointing to the allocated memory area -- Success. \n
  Zero -- Error.

  @dependencies
  None.
*/
static __inline void *qurt_qdi_user_malloc(int client_handle, unsigned size)
{
   void *ret;

   if (qurt_qdi_handle_invoke(client_handle,
                              QDI_CLIENT_HANDLE_USER_MALLOC,
                              size, &ret) < 0)
      return NULL;
   else
      return ret;
}

/**@ingroup driver_support_functions
  Deallocates memory area in the QDI heap.

  @param client_handle Obtained from the current invocation function (Section 3.4.1).
  @param ptr Pointer.

  @return

  @dependencies
  None.
*/
static __inline void qurt_qdi_user_free(int client_handle, void *ptr)
{
   qurt_qdi_handle_invoke(client_handle,
                          QDI_CLIENT_HANDLE_USER_FREE,
                          ptr);
}

/**@ingroup driver_support_functions
  Detaches a client (a process), indicating that the client does not
  participate in the qurt_wait() mechanism.  Note that this behavior
  is "opt-in" and is irrevocable.  Once a client is detached, it can
  not be "un-detached".

  @param client_handle The handle of the client to be detached.

  @return
  Zero return value for success.  Detachable clients always return success.
  Non-zero return value indicates that client_handle did not refer to a
    detachable user client.

  @dependencies
  None.
*/
static __inline int qurt_qdi_client_detach(int client_handle)
{
   return qurt_qdi_handle_invoke(client_handle, QDI_CLIENT_HANDLE_DETACH);
}

/**@ingroup driver_support_functions
  Creates a new signal group for use in a device driver.
  A QDI signal group contains up to 32 signals, which can be operated on either
  individually (using the qurt_qdi_signal_* functions) or as a group (using the
  qurt_qdi_signal_group_* functions). \n
  @note1hang A driver implementation is responsible for using the proper signal group
             handle in any given situation. \n
  For more information on signals see the Hexagon QuRT RTOS User Guide.

  @param client_handle -- Obtained from the current invocation function (Section 3.4.1).
  @param p_signal_group_handle_local -- Returns a handle which is intended for use by code
                      residing in the same context and process as the created signal group
                      (for example, the device driver implementation that allocated the 
                      signal group).
  @param p_signal_group_handle_remote -- Returns a handle which is intended for use by code
                      residing in a different context and process than the created signal group 
                      (for example, the user-mode client of an OS driver).

  @return

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_group_create(int client_handle,
                                                 int *p_signal_group_handle_local,
                                                 int *p_signal_group_handle_remote)
{
   return qurt_qdi_handle_invoke(client_handle,
                                 QDI_CLIENT_HANDLE_SIGNAL_GROUP_CREATE,
                                 p_signal_group_handle_local,
                                 p_signal_group_handle_remote);
}

/**@ingroup driver_support_functions
  Suspends the current thread until any of the signals are set in the specified signal group
  If a signal is set in a signal group object, and a thread is waiting on the signal group object,
  then the thread is awakened. If the awakened thread has higher priority than the current
  thread, a context switch may occur.

  @param signal_group_handle   Handle of the signal group.

  @return

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_group_wait(int signal_group_handle)
{
   return qurt_qdi_handle_invoke(signal_group_handle,
                                 QDI_SIGNAL_GROUP_WAIT);
}

/**@ingroup driver_support_functions
  Returns a value indicating if any of the signals are set in the specified signal group.

  @param signal_group_handle Handle of the signal group.

  @return
  1 -- Indicates whether any of the signals are set in the signal group.\n
  0 -- Indicates that none of the signals are set.

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_group_poll(int signal_group_handle)
{
   return qurt_qdi_handle_invoke(signal_group_handle,
                                 QDI_SIGNAL_GROUP_POLL);
}


/**@ingroup driver_support_functions
  Creates a new signal in the specified signal group.
  For more information on signals see the Hexagon QuRT RTOS User Guide.

  @note1hang A driver implementation is responsible for using the proper signal handle in
             any given situation.

  @param signal_group_handle   Handle of an existing signal group (Section 3.7.10).
  @param p_signal_handle_local Returns a handle which is intended for use by code residing in
                               the same context and process as the created signal (for example,
                               the device driver implementation that allocated the signal).
  @param p_signal_handle_remote Returns a handle which is intended for use by code residing in
                               a different context and process than the created signal (for 
                               example, the user-mode client of an OS driver).

  @return 
  Non-zero value -- No more signals can be created in the specified
                    signal group. 

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_create(int signal_group_handle,
                                           int *p_signal_handle_local,
                                           int *p_signal_handle_remote)
{
   return qurt_qdi_handle_invoke(signal_group_handle,
                                 QDI_SIGNAL_GROUP_SIGNAL_CREATE,
                                 p_signal_handle_local,
                                 p_signal_handle_remote);
}

/**@ingroup driver_support_functions
  Sets the signal in the specified signal object.

  @param signal_handle Handle of the signal.

  @return
  1 -- Signal is set \n
  0 -- Signal is clear.

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_set(int signal_handle)
{
   return qurt_qdi_handle_invoke(signal_handle,
                                 QDI_SIGNAL_SET);
}

/**@ingroup driver_support_functions
  Clears the signal in the specified signal object.

  @param signal_handle   Handle of the signal.
  
  @return 
  1 -- Signal is set. \n
  0 -- Signal is clear.

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_clear(int signal_handle)
{
   return qurt_qdi_handle_invoke(signal_handle,
                                 QDI_SIGNAL_CLEAR);
}

/**@ingroup driver_support functions
  Suspends the current thread until the specified signal is set.
  If a signal is set in a signal object, and a thread is waiting on the signal object, then the
  thread is awakened. If the awakened thread has higher priority than the current thread, a
  context switch may occur.

  @param signal_handle Handle of the signal.

  @return

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_wait(int signal_handle)
{
   return qurt_qdi_handle_invoke(signal_handle,
                                 QDI_SIGNAL_WAIT);
}


/**@ingroup driver_support_functions
  Returns a value indicating if the specified signal is set.

  @param signal_handle Handle of the signal.

  @return
  1 -- Signal is set. \n
  0 -- Signal is not set.

  @dependencies
  None.
*/
static __inline int qurt_qdi_signal_poll(int signal_handle)
{
   return qurt_qdi_handle_invoke(signal_handle,
                                 QDI_SIGNAL_POLL);
}

/**@ingroup driver_support_functions
  Registers a QDI device with the generic QDI object in the 
  current QDI context.
  This function registers an exact name or a directory prefix with a QDI opener object.
  Future invocations of qurt_qdi_open() in the context of the caller will invoke the
  opener object if a match is detected.
  Directory prefix names are specified by ending the name with a forward slash character.
  Example of exact name:
  qurt_qdi_devname_register(/dev/foobar, foobar_opener);
  Example of directory prefix:
  qurt_qdi_devname_register(/pipedev/, pipedev_opener);
  Given the two registrations shown above, the only qurt_qdi_open() requests that would
  be directed to the foobar_opener object would be requests for the exact name
  "/dev/foobar", Any request beginning with "/pipedev/" would be directed to the
  pipedev_opener object.
  The pipedev invocation function would presumably examine the name argument to
  determine exactly how to handle the request. The name is passed to the invocation
  function in the a1.ptr argument (Section 3.4.1).

  @param  name   Device name or device name prefix.
  @param  opener Pointer to opener object for the device.
 
  @return
  0 -- Device was successfully registered. \n
  Negative error code -- Device was not registered.
 */
static __inline int qurt_qdi_devname_register(const char *name,
                                              qurt_qdi_obj_t *opener)
{
   return qurt_qdi_handle_invoke(QDI_HANDLE_GENERIC,
                                 QDI_DEVNAME_REGISTER,
                                 name,
                                 opener);
}

// Macros for backward compatibility with deprecated APIs
//  (These will go away soon)

#define qurt_qdi_register_devname(name, opener) \
        qurt_qdi_devname_register(name, (void *)opener)
#define qurt_qdi_new_handle_from_obj_t(handle, obj) \
        qurt_qdi_handle_create_from_obj_t(handle, obj)
#define qurt_qdi_release_handle(client_handle, handle) \
        qurt_qdi_handle_release(client_handle, handle)
#define qurt_qdi_lock_buffer(handle, buf, len, perms, obuf) \
        qurt_qdi_buffer_lock(handle, buf, len, perms, obuf)
#define qurt_qdi_usermalloc(handle, size) \
        qurt_qdi_user_malloc(handle, size)
#define qurt_qdi_userfree(handle, ptr) \
        qurt_qdi_user_free(handle, ptr)

#endif
