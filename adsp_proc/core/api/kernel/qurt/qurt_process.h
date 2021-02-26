#ifndef QURT_PROCESS_H
#define QURT_PROCESS_H
/**
  @file qurt_process.h
  @brief Prototypes of QuRT process control APIs

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

#define QURT_PROCESS_ATTR_NAME_MAXLEN QURT_MAX_NAME_LEN

#define QURT_PROCESS_NON_SYSTEM_CRITICAL        (1u << 1)     /* Bit 1 (0x00000002) */

/** @addtogroup process_types
@{ */
/** QuRT process type. */
typedef struct _qurt_process_attr {
    /** @cond */
    char name[QURT_PROCESS_ATTR_NAME_MAXLEN]; /* Process name. */
    int flags;
    /** @endcond */
} qurt_process_attr_t; 
/** @} */ /* end_addtogroup process_types */
/*=============================================================================
FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_process_create
  Creates a process with the specified attributes, and start the process.

  The process executes the code in the specified executable ELF file.

  @datatypes
  #qurt_process_attr_t

  @param[out] attr Accepts an initialized process attribute structure, which specifies
                   the attributes of the created process.

  @return
  None.

  @dependencies
  None.
*/
int qurt_process_create (qurt_process_attr_t *attr);

/**@ingroup func_qurt_process_get_id
  Returns the process identifier for the current thread. 

  @return
  None.

  @dependencies
  Process identifier for the current thread..
*/
int qurt_process_get_id (void);

/**@ingroup func_qurt_process_attr_init
  Initializes the structure that is used to set the process attributes when a thread is created.

  After an attribute structure is initialized, the individual attributes in the structure can 
  be explicitly set using the process attribute operations.

  Table @xref{tbl:processAttrDefaults} lists the default attribute values set by the initialize 
  operation.

  @inputov{table_process_attribute_defaults}

  @datatypes
  #qurt_process_attr_t

  @param[out] attr Pointer to the structure to initialize.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_process_attr_init (qurt_process_attr_t *attr)
{
    attr->name[0] = 0;
    attr->flags = 0;
}

/**@ingroup func_qurt_process_attr_set_executable
  Sets the process name in the specified process attribute structure.

  Process names are used to identify process objects that are already 
  loaded in memory as part of the QuRT system.

  @note1hang Process objects are incorporated into the QuRT system at build time.

  @datatypes
  #qurt_process_attr_t

  @param[in] attr Pointer to the process attribute structure.
  @param[in] name Pointer to the process name.
 
  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_process_attr_set_executable (qurt_process_attr_t *attr, char *name)
{
    strlcpy (attr->name, name, QURT_PROCESS_ATTR_NAME_MAXLEN);
}

/**@ingroup func_qurt_process_attr_set_flags
Sets the process properties in the specified process attribute structure.
Process properties are represented as defined symbols that map into bits 
0 through 31 of the 32-bit flag value. Multiple properties are specified by OR'ing 
together the individual property symbols.

@datatypes
#qurt_process_attr_t

@param[in] attr  Pointer to the process attribute structure.
@param[in] flags QURT_PROCESS_SUSPEND_ON_STARTUP suspends the process after creating it.

@return

@dependencies
None.
*/
static inline void qurt_process_attr_set_flags (qurt_process_attr_t *attr, int flags)
{
    attr->flags = flags;
}

/**@ingroup func_qurt_process_cmdline_get
Gets the command line string associated with the current process.
The Hexagon simulator command line arguments are retrieved using 
this function as long as the call is made
in the process of the QuRT installation, and with the 
requirement that the program is running in a simulation environment.

If the function modifies the provided buffer, it zero-terminates
the string. It is possible that the function will not modify the
provided buffer, so the caller must set buf[0] to a NULL
byte before making the call. If the command line is longer than the provided
buffer, a truncated command line is returned.

@param[in] buf     Pointer to a character buffer that must be filled in.
@param[in] buf_siz  Size (in bytes) of the buffer pointed to by buf.

@return
None.

@dependencies
None.
*/
void qurt_process_cmdline_get(char *buf, unsigned buf_siz);
#endif
