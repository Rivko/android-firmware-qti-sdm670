#ifndef QURT_ISLAND_H
#define QURT_ISLAND_H

/**
  @file qurt_island.h 
  @brief  Prototypes of power API  
          The APIs allow entering and exiting island mode where the memory
          accesses are limited to local memory.

  EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/
#include <qurt_thread.h>
#include <qurt_memory.h>
#include <qurt_alloc.h>
#include <qurt_error.h>

/** @cond */
enum qurt_island_attr_resource_type {
    QURT_ISLAND_ATTR_INVALID,
    QURT_ISLAND_ATTR_END_OF_LIST = QURT_ISLAND_ATTR_INVALID,
    QURT_ISLAND_ATTR_INT,
    QURT_ISLAND_ATTR_THREAD,
    QURT_ISLAND_ATTR_MEMORY,
    QURT_ISLAND_ATTR_POOL
};

typedef struct qurt_island_attr_resource {
    enum qurt_island_attr_resource_type type;
    union {
        struct {
            qurt_addr_t base_addr;
            qurt_size_t size;
        } memory;
        char poolname[32];
        unsigned int interrupt;
        qurt_thread_t thread_id;
    };
} qurt_island_attr_resource_t;
/** @endcond */

/** QuRT island attributes */
typedef struct qurt_island_attr {
    /** @cond */
    int max_attrs;
    struct qurt_island_attr_resource attrs[1];
    /** @endcond */
} qurt_island_attr_t;

/** Qurt island specification type */
typedef struct {
   int qdi_handle;
} qurt_island_t;

/**@ingroup func_qurt_island_attr_create
  Creates QuRT island attribute.\n
  This function allocates memory for island attributes. The memory can contain
  maximum number of attributes equal to max_attrs. All the attributes are
  initialized as "QURT_ISALND_ATTR_INVALID" type.
 
  @return
  QURT_EOK -- attr is created successfully. \n
  QURT_EMEM -- attr is not created successfully. \n

  @dependencies
  None.
 */
int qurt_island_attr_create (qurt_island_attr_t **attr, int max_attrs);

/**@ingroup func_qurt_island_attr_delete
  Deletes QuRT island attribute.\n
  This function frees memory allocated for QuRT island attributes.
 
  @return
  None. \n

  @dependencies
  None.
 */
void qurt_island_attr_delete (qurt_island_attr_t *attr);

/**@ingroup func_qurt_island_attr_add
  Adds list of resources.\n
  This function adds list of resources to the island attribute. The end of the
  list is indicated by QURT_ISLAND_ATTR_END_OF_LIST. This function doesn't process
  resources beyond the resource with type QURT_ISLAND_ATTR_END_OF_LIST. If the list
  of resources exceed the no. of resources supported by island attribute, it
  will skip the remaining resources.
 
  @datatypes
  #qurt_island_attr_t \n
  #qurt_island_attr_resource_t \n

  @param[in/out] island attribute
  @param[in] interrupt number

  @return
  QURT_EOK -- resource list is successfully added.  \n
  QURT_EINVALID -- undefined resource type or duplicate resources. \n
  QURT_EMEM -- insufficient memory in "attr". \n

  @dependencies
  None.
 */
int qurt_island_attr_add (qurt_island_attr_t *attr, qurt_island_attr_resource_t *resources);

/**@ingroup func_qurt_island_attr_add_interrupt
  Adds interrupt resource.\n
  This function adds interrupt number to the island attribute.
 
  @datatypes
  #qurt_island_attr_t \n

  @param[in/out] island attribute
  @param[in] interrupt number

  @return
  QURT_EOK -- resource type is successfully added.  \n
  QURT_EINVALID -- undefined resource type or duplicate resources. \n
  QURT_EMEM -- failed, not enough memory in "attr". \n

  @dependencies
  None.
 */
int qurt_island_attr_add_interrupt (qurt_island_attr_t *attr, int interrupt);

/**@ingroup func_qurt_island_attr_add_mem
  Adds memory resource.\n
  This function adds memory address range to the island attribute.

  @datatypes
  #qurt_addr_t \n
  #qurt_size_t \n
  #qurt_island_attr_t \n
 
  @param[in/out] island attribute
  @param[in] base_addr    Base address of the memory range
  @param[in] size         Size (in bytes) of the memory range
 
  @return
  QURT_EOK -- resource type is successfully added.  \n
  QURT_EINVALID -- undefined resource type or duplicate resources. \n
  QURT_EMEM -- failed, not enough memory in "attr". \n

  @dependencies
  None.
 */
int qurt_island_attr_add_mem (qurt_island_attr_t *attr, qurt_addr_t base_addr, qurt_size_t size);

/**@ingroup func_qurt_island_attr_add_pool
  Adds memory resource.\n
  This function adds memory address range to the island attribute.

  @datatypes
  #qurt_island_attr_t \n
 
  @param[in/out] island attribute
  @param[in] pool_name   Name of physical pool (configured in customer XML)
 
  @return
  QURT_EOK -- resource type is successfully added.  \n
  QURT_EINVALID -- undefined physical pool. \n

  @dependencies
  None.
 */
int qurt_island_attr_add_pool (qurt_island_attr_t *attr, char * pool_name);

/**@ingroup func_qurt_island_attr_add_thread
  Adds thread resource.\n
  This function adds thread id to the island attribute.

  @datatypes
  #qurt_thread_t \n
  #qurt_island_attr_t \n
 
  @param[in/out] island attribute
  @param[in] thread_id  Thread ID
 
  @return
  QURT_EOK -- resource type is successfully added.  \n
  QURT_EINVALID -- undefined resource type or duplicate resources. \n
  QURT_EMEM -- failed, not enough memory in "attr". \n

  @dependencies
  None.
 */
int qurt_island_attr_add_thread  (qurt_island_attr_t *attr, qurt_thread_t thread_id);

/**@ingroup func_qurt_island_spec_create
  Creates a specification for island mode.\n
  This function is an entry point to QuRT OS to create an island specification.
  A spec is represented by an identifier. QuRT OS validates the attributes /
  resources. Multiple island specs can be created and it will not affect current
  operation. The spec identifier is required to enter island mode.
 
  @datatypes
  #qurt_island_t \n
  #qurt_island_attr_t \n

  @param[in] attr  Attributes that define the island
  @param[out] spec_id ID that identify the island specification, which is defined
              by the attributes.

  @return
  QURT_EOK -- spec is successfully created.  \n
  QURT_EINVALID -- Attributes can't form an island. \n

  @dependencies
  None.
 */
int qurt_island_spec_create (qurt_island_t *spec_id, qurt_island_attr_t *attr);

/**@ingroup func_qurt_island_add_interrupt
  Enter island mode.\n
  This function adds an interrupt to island spec dynamically.

  @datatypes
  #qurt_island_t \n

  @param[in] spec_id ID created for the island specification.
  @param[in] Interrupt number to add to the spec
 
  @return
  QURT_EOK -- Successfully added interrupt to the spec. \n
  QURT_EFAILED -- Not enough slots to add interrupt to the spec

  @dependencies
  None.
 */
int qurt_island_add_interrupt (qurt_island_t spec_id, int int_num);

/**@ingroup func_qurt_island_remove_interrupt
  Enter island mode.\n
  This function removes an interrupt to island spec dynamically.

  @datatypes
  #qurt_island_t \n

  @param[in] spec_id ID created for the island specification.
  @param[in] Interrupt number to be removed from the spec
 
  @return
  QURT_EOK -- Successfully removed the interrupt from spec. \n

  @dependencies
  None.
 */
int qurt_island_remove_interrupt (qurt_island_t spec_id, int int_num);

/**@ingroup func_qurt_island_spec_delete
  Prepares for island mode.\n
  Island specification created in QuRT OS will be deleted. All the resources
  allocated for the specification will be freed.
 
  @datatypes
  #qurt_island_t \n
  #qurt_island_attr_t \n

  @param[in] attr  Attributes that define the island
  @param[out] spec_id ID that identify the island configuration, which is
                      defined by the attributes.

  @return
  QURT_EOK -- island spec is successfully deleted.  \n
  QURT_EINVALID -- spec_id is invalid. \n

  @dependencies
  None.
 */
int qurt_island_spec_delete (qurt_island_t spec_id);

/**@ingroup func_qurt_island_enter
  Enter island mode.\n
  This function triggers the island mode defined by the island mode
  specification. The system should be in single threaded Mode to be able to
  enter island mode.

  @datatypes
  #qurt_island_t \n

  @param[in] spec_id ID created for the island specification.
 
  @return
  QURT_EOK -- Successfully entered island mode. \n
  QURT_ESTM -- System is not in Single Threaded Mode.

  @dependencies
  None.
 */
int qurt_island_enter (qurt_island_t spec_id);

/**@ingroup func_qurt_island_exit
  Exit island mode.\n
  This function brings the system out of island mode. The system should be in
  island mode to be able to bring the system out of island mode. The system
  should also be in Single Threaded Mode to be able to exit island mode.
 
  @return
  QURT_EOK -- Operation was successfully performed. \n
  QURT_ESTM -- System is not in Single Threaded Mode.
  QURT_EFAILED -- The system is not brought out of island mode.

  @dependencies
  None.
 */
int qurt_island_exit (unsigned short stage);

/**@ingroup func_qurt_island_exception_wait
  Registers the island exception handler.
  This function assigns the current thread as the QuRT island exception handler
  and suspends the thread until a exception occurs within island mode.

  When exception occurs in island mode, the thread is awakened with exception
  information assigned to this operations parameters.

  @note1hang If no island exception handler is registered, or if the registered
             handler itself calls exit, then QuRT raises a kernel exception.
             If a thread runs in Supervisor mode, any exceptions are treated as
             kernel exceptions.

  @note2 All the parameters are interpreted based on the cause code. Some of the
         parameters are not valid based on the cause code.

  @param[out]  ip      Pointer to the instruction memory address where the
                       exception occurred.
  @param[out]  sp      Stack pointer.
  @param[out]  badva   Pointer to the virtual data address where the exception
                       occurred due to memory access. Pointer to the interrupt
                       number when exception occured due to the reception of
                       non island interrupt.
  @param[out]  cause   Pointer to the QuRT exception or error code.   

  @return
  Registry status: \n
  - Thread identifier -- Handler successfully registered. \n
  - QURT_EFATAL -- Registration failed.

  @dependencies
  None.
*/
unsigned int qurt_island_exception_wait (unsigned int *ip, unsigned int *sp,
                                         unsigned int *badva, unsigned int *cause);

/**@ingroup func_qurt_island_get_status
  Get island mode status.

  Returns a value indicating whether the QuRT system is executing in island mode.

  @return
  0 - Normal mode. \n
  1 - Island mode. \n

  @dependencies
  None.
*/
unsigned int qurt_island_get_status (void);


/**@ingroup func_qurt_island_get_exit_status
  Get the reason for last island mode exit status.

  @param[in/out] cause_code Return pointer that will have the cause code of the last
                 island exit reason.
                 QURT_EISLANDUSEREXIT: Island exit was due to user call for island exit
                 QURT_ENOISLANDENTRY: The api was called before island has not been exited
                 even once
                 QURT_EISLANDINVALIDINT: Island exit due to an invalid interrupt in island mode.
                          
                 int_num Return pointer that will hold the invalid interrupt number that caused
                 island exit.
                 This will hold the interrupt number that caused the island exit if the cause 
			        code is QURT_EISLANDINVALIDINT.
                 For all other cases, it will be -1.

  @return
  None. \n

  @dependencies
  None.
*/
void qurt_island_get_exit_status(unsigned int *cause_code, int *int_num);

/**@ingroup func_qurt_island_get_specid
  Get the island spec id

  @param[in/out] spec_id Return pointer that will have the spec_id.
                 QURT_EVAL: If the pointer passed is NULL
                 QURT_EFAILED: If the spec id is not initialized.
                          
  @return
  None. \n

  @dependencies
  None.
*/
int qurt_island_get_specid(qurt_island_t *spec_id);

#endif /* QURT_ISLAND_H */

