#ifndef ATE_H__
#define ATE_H__

#include "mmdefs.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"

/******************************************************************************
*  APR Test Environment
******************************************************************************/

int32_t ate_init(void);
int32_t ate_deinit(void);

/* General Test Cases */
void ate_get_version(void);
void ate_allocate_free(uint32_t size);

/* Query Test Cases */
void ate_query_by_dns(void);
void ate_query_by_handle(void);

/* Register Test Cases */
void ate_register2_by_name(char_t *dns_name, uint32_t dns_name_size);
void ate_register2_by_addr(void);

/* Send Test Cases */
void ate_alloc_send(char_t *dns_name, uint32_t dns_name_size, uint32_t a, uint32_t b);
void ate_alloc_send_addr(uint16_t addr, uint32_t a, uint32_t b);

#endif // ATE_H__
