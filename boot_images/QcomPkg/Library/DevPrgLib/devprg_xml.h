/**************************************************************************
 * FILE: devprg_xml.h
 *
 * XML handler.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_xml.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-11   wek   Allow adding XML log tags to existing log.
2016-11-04   wek   Create. (20-m)

===========================================================================*/

#ifndef __DEVPRG_XML_H__
#define __DEVPRG_XML_H__

#include "comdef.h"
#include "devprg_log.h"
#include <stdarg.h>
#include <stddef.h>

struct dp_xml_handle;

/* Intialize data structures used by xml handlers. */
int devprg_xml_init(void);

/** Parse the XML and extract all the attributes.
 *
 * Parse the XML, this parser is an in place parser, it will modify the
 * xml_string to add null termination to the tag, attribute and values.
 *
 * param  xml_string  XML string as received.
 * param  len         Lenght of the XML string.
 *
 * return Handle to the XML to retrieve attributes and tag.
 */
struct dp_xml_handle *devprg_xml_command_open(char *xml_string, size_t len);

/** Close an open XML handle.
 *
 * param handle  Handle returned by devprg_xml_command_open.
 *
 * return   0 on success, -1 on error (handle was not open or double close).
 */
int devprg_xml_command_close(struct dp_xml_handle *handle);

/** Get the XML Tag for the command.
 *
 * param handle  Handle returned by devprg_xml_command_open.
 *
 * return  Pointer to a constant string for the xml TAG, e.g. program, read,
 *         ufs, emmc.
 */
const char *devprg_xml_get_tag(struct dp_xml_handle *handle);

/** Get the value for a given attribute in the XML.
 *
 * param handle  Handle returned by devprg_xml_command_open.
 *
 * return  Pointer to a constant string containing the value for a given
 *          attribute.
 */
const char *devprg_xml_get_value(struct dp_xml_handle *handle, const char *attribute);

/** Generate a generic response packet.
 *
 * param buffer    Location where to place the XML output.
 * param buf_size  Size of the buffer.
 * param ack       Is the response "ACK"(1) or "NAK"(0) back to the host.
 * param rawmode   Are we switching to raw mode.
 *
 * return -1 on error, the length of data copied to buffer on success.
 */
int devprg_xml_build_response(void *buffer, size_t buf_size,
                              int ack, int rawmode);


/** Add the XML tags for a log to an existing log.
 *
 * This function will prepend and append the XML tags so it can be sent to
 * host.
 *
 * param buffer    Location where to place the XML output.
 * param buf_size  Size of the buffer.
 * param log_size  Size of the log data already in the buffer.
 *
 * return -1 on error, the length of data copied to buffer on success.
 */
int devprg_xml_add_log_tags(void *buffer, size_t buf_size, size_t log_size);

/** Generate a Log message to send to the host.
 *
 * param buffer    Location where to place the XML output.
 * param buf_size  Size of the buffer.
 * param log       Log string to print.
 * param list      Variable list arguments, equivalent to vprintf function.
 *
 * return -1 on error, the length of data copied to buffer on success.
 */
int devprg_xml_build_log(void *buffer, size_t buf_size,
                         enum devprg_log_lvl lvl, const char* log,
                         va_list list);


/** Start populating the buffer with an xml to send a response to the host.
 *
 * param buffer   Where to place the XML data.
 * param size     size in bytes of the buffer.
 *
 * return   0 on success, -1 on error.
 */
struct devprg_xml_write *devprg_xml_resp_open(void *buffer, size_t size);

/** Add a xml tag.
 *
 * param xmlw   handle received from devprg_xml_resp_open()
 * param tag    tag value, say "response" will generate <response in the xml
 *
 * return   0 on success, -1 on error.
 */
int devprg_xml_resp_tag(struct devprg_xml_write *xmlw, const char *tag);

/** Add a integer attribute.
 *
 * param xmlw   handle received from devprg_xml_resp_open()
 * param attr   Attribute name.
 * param val    Attribute Value
 *
 * return   0 on success, -1 on error.
 */
int devprg_xml_resp_attr_i(struct devprg_xml_write *xmlw, const char *attr, uint64 val);

/** Add a string attribute.
 *
 * param xmlw   handle received from devprg_xml_resp_open()
 * param attr   Attribute name.
 * param val    Attribute Value
 *
 * return   0 on success, -1 on error.
 */
int devprg_xml_resp_attr_s(struct devprg_xml_write *xmlw, const char *attr, const char *val);

/** Add basic ACK/NACK response
 *
 * Adds the value="ACK" to the xml.
 *
 * param xmlw   handle received from devprg_xml_resp_open()
 * param ack    1 for ACK, NACK otherwise.
 *
 * return   0 on success, -1 on error.
 */
int devprg_xml_resp_ack(struct devprg_xml_write *xmlw, int ack);

/** Write the tail end of the XML and call the XML complete
 *
 * param xmlw   handle received from devprg_xml_resp_open()
 *
 * return   0 on success, -1 on error.
 */
int devprg_xml_resp_close(struct devprg_xml_write *xmlw);


#endif /* __DEVPRG_XML_H__ */
