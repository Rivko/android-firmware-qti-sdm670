//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef _CDMPAYLOAD_H_
#define _CDMPAYLOAD_H_
/* ===========================================================================
*//** @file cdmpayload.h
This header file defines the payload structure that CDM h/w supports.

*//*======================================================================= */


/*----------------------------------------------------------------------
   CDM Command ID
----------------------------------------------------------------------*/
typedef enum {
    CDM_DMI_CMD              = 0x1,
    CDM_RESERVED_CMD         = 0x2,
    CDM_REG_CONT_CMD         = 0x3,
    CDM_REG_RANDOM_CMD       = 0x4,
    CDM_BUF_INDIRECT_CMD     = 0x5,
    CDM_GEN_IRQ_CMD          = 0x6,
    CDM_WAIT_EVENT_CMD       = 0x7,
    CDM_CHANGE_BASE_CMD      = 0x8,
    CDM_WAIT_COMP_EVENT_CMD  = 0xc,
    CDM_CLEAR_COMP_EVENT_CMD = 0xd,
} CDM_CMD_ID;

/*----------------------------------------------------------------------
   Payload Data Structures
----------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)

typedef struct {
  unsigned int    u32_1;
} _cdm_32;

typedef struct {
  unsigned int    u32_1;
  unsigned int    u32_2;
} _cdm_64;

typedef struct {
  unsigned int    u32_1;
  unsigned int    u32_2;
  unsigned int    u32_3;
} _cdm_96;


//GEN_IRQ Command payload
typedef struct {
  unsigned                       RESERVED  :  24; /* 23:00 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                      USER_DATA  :  32; /* 31:00 */
} _cdm_gen_irq;

typedef union {
  _cdm_gen_irq    bitfields;
  _cdm_64         data;
} CDM_GEN_IRQ;

//WAIT_FOR_EVENT
typedef struct {
  unsigned                           MASK  :   8; /* 07:00 */
  unsigned                             ID  :   8; /* 15:08 */
  unsigned                       RESERVED  :   7; /* 22:16 */
  unsigned                             IW  :   1; /* 23:23 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                         OFFSET  :  24; /* 23:00 */
  unsigned                         UNUSED  :   8; /* 31:24 */
  unsigned                      USER_DATA  :  32; /* 32:00 */
} _cdm_wait_for_event;

typedef union {
  _cdm_wait_for_event bitfields;
  _cdm_96             data;
} CDM_WAIT_FOR_EVENT;

//WAIT_COMP_EVENT
typedef struct {
    unsigned                     RESERVED  :   8; /* 07:00 */
    unsigned                           ID  :   8; /* 15:08 */
    unsigned                    RESERVED2  :   8; /* 23:16 */
    unsigned                      COMMAND  :   8; /* 31:24 */
    unsigned                     MASK_LOW  :  32; /* 32:00 */
    unsigned                    MASK_HIGH  :  32; /* 32:00 */
} _cdm_wait_comp_event;

typedef union {
  _cdm_wait_comp_event    bitfields;
  _cdm_96                 data;
} CDM_WAIT_COMP_EVENT;

//CHANGE_BASE Command payload
typedef struct {
  unsigned                           BASE  :  24; /* 23:00 */
  unsigned                        COMMAND  :   8; /* 31:24 */
} _cdm_change_base;

typedef union {
  _cdm_change_base    bitfields;
  _cdm_32             data;
} CDM_CHANGE_BASE;
//CLEAR COMP EVENT
typedef struct {
    unsigned                     RESERVED : 8; /* 07:00 */
    unsigned                           ID : 8; /* 15:08 */
    unsigned                    RESERVED2 : 8; /* 23:16 */
    unsigned                      COMMAND : 8; /* 31:24 */
    unsigned                     MASK_LOW : 32; /* 32:00 */
    unsigned                    MASK_HIGH : 32; /* 32:00 */
} _cdm_clear_comp_event;

typedef union {
    _cdm_clear_comp_event bitfields;
    _cdm_96             data;
} CDM_CLEAR_COMP_EVENT;

//UPDATE_USERDATA Command payload
typedef struct {
  unsigned                            SEL  :   3; /* 02:00 */
  unsigned                       RESERVED  :  21; /* 23:03 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                      USER_DATA  :  32; /* 32:00 */
} _cdm_update_userdata;

typedef union {
  _cdm_update_userdata   bitfields;
  _cdm_64                data;
} CDM_UPDATE_USERDATA;

//DMI Command payload
typedef struct {
  unsigned                            LEN  :  16; /* 15:00 */
  unsigned                       RESERVED  :   8; /* 23:16 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                            SRC  :  32; /* 32:00 */
  unsigned                         OFFSET  :  24; /* 23:00 */
  unsigned                        DMI_SEL  :   8; /* 31:24 */
} _cdm_dmi;

typedef union {
  _cdm_dmi        bitfields;
  _cdm_96         data;
} CDM_DMI;

//BUFFER_INDIRECT Command payload
typedef struct {
  unsigned                            LEN  :  16; /* 15:00 */
  unsigned                       RESERVED  :   8; /* 23:16 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                            SRC  :  32; /* 32:00 */
} _cdm_buffer_indirect;

typedef union {
  _cdm_buffer_indirect   bitfields;
  _cdm_64                data;
} CDM_BUFFER_INDIRECT;


//REG_RANDOM Command payload
typedef struct {
  unsigned                          COUNT  :  16; /* 15:00 */
  unsigned                       RESERVED  :   8; /* 23:16 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                         OFFSET  :  24; /* 23:00 */
  unsigned                        UNUSED   :   8; /* 31:24 */
  unsigned                           DATA  :  32; /* 32:00 */
} _cdm_reg_random;

typedef union {
  _cdm_reg_random bitfields;
  _cdm_96         data;
} CDM_REG_RANDOM;

//REG_CONT Command payload
typedef struct {
  unsigned                          COUNT  :  16; /* 15:00 */
  unsigned                       RESERVED  :   8; /* 23:16 */
  unsigned                        COMMAND  :   8; /* 31:24 */
  unsigned                         OFFSET  :  24; /* 23:00 */
  unsigned                         UNUSED  :   8; /* 31:24 */
  unsigned                           DATA  :  32; /* 32:00 */
} _cdm_reg_cont;

typedef union {
  _cdm_reg_cont   bitfields;
  _cdm_96         data;
} CDM_REG_CONT;

//BASE LENGTH payload
typedef struct {
  unsigned                           BASE  :  32; /* 32:00 */
  unsigned                       RESERVED  :  12; /* 32:20 */
  unsigned                            LEN  :  20; /* 19:00 */
} _cdm_base_length;

typedef union {
  _cdm_base_length    bitfields;
  _cdm_64             data;
} CDM_BASE_LENGHT;

#pragma pack(pop)

#endif /*_CDMPAYLOAD_H_*/
