/**************************************************************************
 * FILE: devprg_xml.c
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_xml.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-11   wek   Allow adding XML log tags to existing log.
2016-11-04   wek   Create. (20-m)

===========================================================================*/

#include <string.h>
#include <stringl/stringl.h>

#include "devprg_xml.h"
#include "devprg_log.h"

#ifndef DEVPRG_MAX_NUM_ATTRIBUTES
#define DEVPRG_MAX_NUM_ATTRIBUTES 128
#endif
#ifndef DEVPRG_MAX_ATTRIBUTE_SIZE
#define DEVPRG_MAX_ATTRIBUTE_SIZE 4096
#endif

/* holds the offsets to the XML attribute and value as strings.
 * They are NULL terminated. */
struct dp_xml_attribute
{
  const char *name;
  const char *value;
};

struct dp_xml_handle
{
  char in_use;
  char *xml_str;
  char *tag;
  size_t num_attributes;
  struct dp_xml_attribute attributes[DEVPRG_MAX_NUM_ATTRIBUTES];
};

static struct dp_xml_handle xml_handle_inst = {0};

struct devprg_xml_write
{
  char *buffer;
  size_t size;
  size_t offset;
  char  in_use;
};

static struct devprg_xml_write xml_write_instance = {0};

int devprg_xml_init(void)
{
  memset(&xml_write_instance, 0, sizeof(xml_write_instance));
  memset(&xml_handle_inst, 0, sizeof(xml_handle_inst));
  return 0;
}

static char *find_next_attr(char *xml_ptr, char *xml_ptr_end)
{
  for(;xml_ptr < xml_ptr_end; xml_ptr++)
  {
    if (*xml_ptr != ' ' &&
        *xml_ptr != '\n' &&
        *xml_ptr != '\r')
      break;
  }
  return xml_ptr;
}

static char *find_next_char(char to_find, char *xml_ptr, char *xml_ptr_end)
{
  for(;xml_ptr < xml_ptr_end; xml_ptr++)
  {
    if (*xml_ptr == to_find)
      break;
  }
  return xml_ptr;
}


struct dp_xml_handle *devprg_xml_command_open(char *xml_string, size_t len)
{
  size_t offset = 0, last_offset;
  char *xml_str = xml_string;
  char *xml_ptr_end = xml_string + len;
  char *xml_ptr = xml_string;
  int num_attributes = 0;
  struct dp_xml_handle *ret_handle = NULL;
  struct dp_xml_handle *handle = &xml_handle_inst;
  xml_handle_inst.xml_str = xml_string;

  if (handle->in_use != 0)
    DP_LOGE("Too many XML parser opens, make sure close was called %d",
            handle->in_use);

  /* Find the <data> tag. */
  last_offset = len - 6;
  for (offset = 0; offset < last_offset;)
  {
    if (xml_str[offset++] == '<' &&
        xml_str[offset++] == 'd' &&
        xml_str[offset++] == 'a' &&
        xml_str[offset++] == 't' &&
        xml_str[offset++] == 'a' &&
        xml_str[offset++] == '>')
      break;
  }

  /* Scanned the whole string and didn't find data. */
  xml_ptr = &xml_str[offset];
  if (offset >= last_offset)
    goto cleanup;

  /* Offset moved past <data>, after the next < is the tag. */
  while (*xml_ptr != '<')
  {
    xml_ptr++;
    if (xml_ptr >= xml_ptr_end)
      goto cleanup;
  }
  /* found the open '<', the next thing is the tag. */
  xml_ptr++;
  handle->tag = xml_ptr;

  /* Now search for the end of the tag, the white space. */
  while (*xml_ptr != ' ')
  {
    xml_ptr++;
    if (xml_ptr >= xml_ptr_end)
      goto cleanup;
  }
  /* Found the end, add NULL character.*/

  (*xml_ptr) = '\0';
  xml_ptr++;

  num_attributes = 0;
  /* Now loop until we find the closing "/>" */
  xml_ptr = find_next_attr(xml_ptr, xml_ptr_end);
  while (*(xml_ptr) != '/' && *(xml_ptr+1) != '>')
  {
    if (xml_ptr >= xml_ptr_end)
      goto cleanup;

    /* found and attribute */
    handle->attributes[num_attributes].name = xml_ptr;

    /* Just moving to the next equal (=) sign isn't "true" XML parsing
       but it is works since the host cleans the XML. */
    xml_ptr = find_next_char('=', xml_ptr, xml_ptr_end);
    /* Make sure there is enough space for the value which is minimum 3
       characters. */
    if (xml_ptr >= xml_ptr_end - 3)
      goto cleanup;
    /* found the end of the attribute, mark it. */
    *xml_ptr = '\0';
    xml_ptr++;
    /* The value should follow immediately. */
    if (*xml_ptr != '"')
      goto cleanup;

    xml_ptr++;
    handle->attributes[num_attributes].value = xml_ptr;
    xml_ptr = find_next_char('"', xml_ptr, xml_ptr_end);
    if (xml_ptr >= xml_ptr_end)
      goto cleanup;

    /* Found the end of the value, mark that the attribute was found. */
    *xml_ptr = '\0';
    xml_ptr++;
    xml_ptr = find_next_attr(xml_ptr, xml_ptr_end);
    num_attributes++;
    if (num_attributes >= DEVPRG_MAX_NUM_ATTRIBUTES)
    {
      DP_LOGW("Input XML has %d attributes, larger than expected %d, remaining will be ignored",
              num_attributes, DEVPRG_MAX_NUM_ATTRIBUTES);
      break;
    }
  }

  handle->num_attributes = num_attributes;
  handle->in_use = 1;
  ret_handle = handle;
cleanup:

  if (ret_handle == NULL)
    DP_LOGE("Failed to parse xml, error at offset %d", xml_ptr_end - xml_ptr);

  return ret_handle;
}


int devprg_xml_command_close(struct dp_xml_handle *handle)
{
  if (handle->in_use == 1)
  {
    memset (handle, 0, sizeof(struct dp_xml_handle));
    handle->in_use = 0;
    return 0;
  }
  return -1;
}

const char *devprg_xml_get_tag(struct dp_xml_handle *handle)
{
  if (handle->in_use == 1)
  {
    return handle->tag;
  }
  return "";
}

const char *devprg_xml_get_value(struct dp_xml_handle *handle, const char *attr)
{
  if (handle->in_use == 1)
  {
    size_t i;
    for (i = 0; i < handle->num_attributes; i++)
    {
      size_t max_size = DEVPRG_MAX_ATTRIBUTE_SIZE;
      if(strncmp(attr, handle->attributes[i].name, max_size) == 0)
      {
        return handle->attributes[i].value;
      }
    }
  }
  return "";
}



static const char xml_header[] = "<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n<data>\n";
static const char xml_footer[] = " /></data>";

struct devprg_xml_write *devprg_xml_resp_open(void *buffer, size_t size)
{
  static struct devprg_xml_write *xmlw = &xml_write_instance;
  char *buf = (char *)buffer;
  size_t retval;

  if (xmlw->in_use != 0)
  {
    DP_LOGE("NO free handles for xml write, missing a close?", '.');
    return NULL;
  }
  if(buffer == NULL)
  {
    DP_LOGE("NULL BUFFER?", 0);
    return NULL;
  }

  if(size < sizeof(xml_header) + sizeof(xml_footer))
  {
    DP_LOGE("Buffer isn't large enough", 0);
    return NULL;
  }

  xmlw->in_use = 1;
  xmlw->buffer = buf;
  xmlw->size = size;
  xmlw->offset = 0;

  retval = strlcpy(buf, xml_header, size);
  if (retval >= size)
    return NULL; /* Should never hit because of the check above. */

  xmlw->offset = retval;

  return xmlw;
}

int devprg_xml_resp_tag(struct devprg_xml_write *xmlw, const char *tag)
{
  size_t remain;
  char *buf;
  size_t retval;

  if (xmlw->in_use == 0)
    return 0;

  buf = &xmlw->buffer[xmlw->offset];
  remain = xmlw->size - xmlw->offset;
  retval = strlcpy(buf, "<", remain);
  if (retval >= remain)
    return -1;

  buf += retval;
  remain -= retval;
  xmlw->offset += retval;
  retval = strlcpy(buf, tag, remain);
  if (retval >= remain)
    return -1;

  xmlw->offset += retval;

  return 0;
}

int devprg_xml_resp_attr_i(struct devprg_xml_write *xmlw, const char *attr, uint64 val)
{
  size_t remain;
  char *buf;
  size_t retval;

  if (xmlw->in_use == 0)
    return 0;

  buf = &xmlw->buffer[xmlw->offset];
  remain = xmlw->size - xmlw->offset;
  retval = snprintf(buf, remain, " %s=\"%llu\"",
                    attr, (unsigned long long int)val);
  if (retval >= remain)
    return -1;
  xmlw->offset += retval;

  return 0;
}

int devprg_xml_resp_attr_s(struct devprg_xml_write *xmlw, const char *attr, const char *val)
{
  size_t remain;
  char *buf;
  size_t retval;

  if (xmlw->in_use == 0)
    return 0;

  buf = &xmlw->buffer[xmlw->offset];
  remain = xmlw->size - xmlw->offset;
  retval = snprintf(buf, remain, " %s=\"%s\"", attr, val);
  if (retval >= remain)
    return -1;
  xmlw->offset += retval;

  return 0;
}

int devprg_xml_resp_ack(struct devprg_xml_write *xmlw, int ack)
{
  char *ack_str;

  ack_str = (ack == 1) ? "ACK" : "NAK";

  return devprg_xml_resp_attr_s(xmlw, "value", ack_str);

}

int devprg_xml_resp_close(struct devprg_xml_write *xmlw)
{
  size_t remain;
  char *buf;
  size_t retval;

  if (xmlw->in_use == 0)
    return 0;

  xmlw->in_use = 0;

  buf = &xmlw->buffer[xmlw->offset];
  remain = xmlw->size - xmlw->offset;
  retval = strlcpy(buf, xml_footer, remain);
  if (retval >= remain)
    return -1;

  return 0;
}

int devprg_xml_build_response(void *buffer, size_t buf_size,
                              int ack, int rawmode)
{
  char *buf = (char *)buffer;
  size_t retval;
  size_t remain;

  remain = buf_size;
  retval = strlcpy(buf, xml_header, remain);
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;

  retval = strlcpy(buf, "<response value=\"", remain);
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;

  if(ack)
    retval = strlcpy(buf, "ACK", remain);
  else
    retval = strlcpy(buf, "NAK", remain);

  if (retval >= remain)
    return -1;

  remain -= retval;
  buf += retval;

  retval = strlcpy(buf, "\" rawmode=\"", remain);
  if (retval >= remain)
    return -1;

  remain -= retval;
  buf += retval;

  if(rawmode)
    retval = strlcpy(buf, "true\"", remain);
  else
    retval = strlcpy(buf, "false\"", remain);

  if (retval >= remain)
    return -1;

  remain -= retval;
  buf += retval;

  retval = strlcpy(buf, xml_footer, remain);
  if (retval >= remain)
    return -1;

  remain -= retval;
  buf += retval;

  return buf_size - remain;
}

int devprg_xml_add_log_tags(void *buffer, size_t buf_size, size_t log_size)
{
  char *buf = (char *)buffer;
  char *log_loc;
  size_t retval;
  size_t remain;
  const char log_tag[] = "<log value=";
  size_t xml_head_size = sizeof(xml_header)-1 + sizeof(log_tag)-1 + 1; /* +1 the open quote for value. */
  size_t xml_tag_size = xml_head_size + sizeof(xml_footer)-1 + 1; /* +1 the close quote for value. */

  if (buf_size < xml_tag_size)
    return -1;

  /* Crop the log if there isn't enough space. */
  if (buf_size - xml_tag_size < log_size)
    log_size = buf_size - xml_tag_size;

  /* Move the log in the buffer to create space for the header. */
  log_loc = buf + xml_head_size; /* -1 to remove end quote*/
  memmove(log_loc, buf, log_size);

  remain = buf_size;
  retval = strlcpy(buf, xml_header, remain);
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;

  retval = strlcpy(buf, log_tag, remain);
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;
  /* Skip the log string, add quotes around it. */
  *buf = '"';
  buf = buf + 1 + log_size;
  *buf = '"';
  buf++;
  remain = remain - 2 - log_size;

  retval = strlcpy(buf, xml_footer, remain);
  if (retval >= remain)
    return -1;

  remain -= retval;
  buf += retval;

  return buf_size - remain;
}


int devprg_xml_build_log(void *buffer, size_t buf_size,
                         enum devprg_log_lvl lvl, const char* log,
                         va_list list)
{
  char *buf = (char *)buffer;
  size_t retval;
  int result;
  size_t remain;

  remain = buf_size;
  retval = strlcpy(buf, xml_header, remain);
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;

  retval = strlcpy(buf, "<log value=\"", remain);
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;

  switch(lvl)
  {
  case LOG_ERROR:
    retval = strlcpy(buf, "ERROR: ", remain);
    break;
  case LOG_WARN:
    retval = strlcpy(buf, "WARN: ", remain);
    break;
  case LOG_INFO:
    retval = strlcpy(buf, "INFO: ", remain);
    break;
  case LOG_DEBUG:
    retval = strlcpy(buf, "DEBUG: ", remain);
    break;
  default:
    retval = strlcpy(buf, "OTHER: ", remain);
    break;
  }
  if (retval >= remain)
    return -1;
  remain -= retval;
  buf += retval;

  result = vsnprintf(buf, remain, log, list);
  if (result < 0 || (size_t)result >= remain)
    return -1;
  remain -= result;
  buf += result;

  retval = snprintf(buf, remain, "\" %s", xml_footer);
  if (retval >= remain)
    return -1;

  remain -= retval;
  buf += retval;

  return buf_size - remain;
}
