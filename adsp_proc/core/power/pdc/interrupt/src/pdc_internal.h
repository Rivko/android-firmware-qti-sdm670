#ifndef _PDC_INTERNAL_H_
#define _PDC_INTERNAL_H_
/*==============================================================================
  FILE:         pdc_internal.h

  OVERVIEW:     This file contains private types & data for the PDC dirver

  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/pdc_internal.h#2 $
$DateTime: 2017/12/13 21:59:53 $
================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "pdc_types.h"

/*============================================================================
 *                            TYPES & DEFINITIONS
 *===========================================================================*/
/* Definitions for default (invalid) states */
#define PDC_GPIO_INVALID      (uint32)-1  /* All indexes are invalid */
#define PDC_GPIO_IDX_INVALID  (uint16)-1  /* Specific index is invalid */
#define PDC_MUX_OPEN          (uint16)0   /* Unconnected MUX input */
#define PDC_INT_INVALID       (uint32)-1  /* Invalid interrupt vector */
 
/* Enumeration of the pin types */
typedef enum
{
  PDC_IRQ  = 0, /* Interrupt type */
  PDC_GPIO      /* GPIO type */
}pdc_pin_type;
 
 /* Table data structure that contains which GPIO inputs are valid to the subsystem
  * and which mux they are mapped to */
typedef struct
{
  uint16  gpio_num;     /* Subsystem GPIO option */

  union
  {
    uint32 number;      /* Configured MUX index for this GPIO */
    struct 
    {
      uint32 main :16;
      uint32 alt  :16;
    };
  }mux;
  
}pdc_gpio_inputs;

/* Table data structure that contains the GPIO configuraiton and the associated
 * local master interrupt vector number */
typedef struct
{
  pdc_trigger_config  trig_config;          /* PDC GPIO configuraiton */
  pdc_gpio_inputs     *gpio_tbl_ptr;        /* Back pointer to the GPIO input data */
  const uint32        subsystem_interrupt;  /* Subsystem interrupt vector for this mux */
}pdc_gpio_mapping;

/* Table data structure that contains the interrupt configuration and the associated
 * local master interrupt vector number */
typedef struct
{
  pdc_trigger_config  trig_config;          /* PDC interrupt configuration */
  uint32              subsystem_interrupt;  /* Subsystem interrupt vector for this PDC int */
}pdc_interrupt_mapping;

/* Type that simply combines the two required GPIO tables into one structure for easier access */
typedef struct
{
  pdc_gpio_inputs   *gpio_inputs;   /* GPIO input table */
  pdc_gpio_mapping  *mux_map;       /* GPIO configuraiton table */
}pdc_gpio_config;

/* Complete interrupt table/mapping data */
typedef struct
{
  pdc_interrupt_mapping *map;
  uint32                numInt;
}pdc_interrupts;

/* Complete interrupt table/mapping data */
typedef struct
{
  pdc_gpio_config map;
  uint32          numGpio; /* Number of GPIO inputs to multiplexors */
  uint32          numMux;  /* Number of multiplexors */
}pdc_gpios;

/*============================================================================
 *                                FUNCTIONS
 *===========================================================================*/
/** 
 * pdc_initialize 
 *  
 * @brief Initializes the main PDC driver
 *
 * @return none
 */
void pdc_initialize(void);

/** 
 * pdc_getTriggerConfig 
 *  
 * @brief Gets the trigger configuration for an interrupt or GPIO
 *  
 * @param pinType:  GPIO or interrupt pin type 
 * @param index:    GPIO or interrupt mapping table index
 *  
 * @return Pointer to the trigger configuraiton
 */
pdc_trigger_config *pdc_getTriggerConfig(pdc_pin_type pinType, uint32 index);

#ifdef __cplusplus
}
#endif

#endif /* _PDC_INTERNAL_H_ */

