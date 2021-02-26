/*============================================================================
@file CoreConfig.c

This file provides an abstraction layer for reading a section/key pair
from a configuration area (file, registry, etc.).

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreConfig.c#1 $
============================================================================*/
#include "CoreConfigInternal.h"
#include "CoreVerify.h"

/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/*
 * CoreConfig_ReadUint32
*/
unsigned int CoreConfig_ReadUint32( CoreConfigHandle config, 
                                    char *section, char *key,
                                    unsigned int *value )
{
  CORE_VERIFY_PTR( config );

  return ( config->vtable.ReadUint32( config, section, key, value ) );
}

/*
 * CoreConfig_ReadString
*/
unsigned int CoreConfig_ReadString( CoreConfigHandle config, 
                                    char *section, char *key, 
                                    char *string, unsigned int *size )
{
  CORE_VERIFY_PTR( config );

  return ( config->vtable.ReadString( config, section, key, string, size ) );
}

/*
 * CoreConfig_ReadBool
*/
unsigned int CoreConfig_ReadBool( CoreConfigHandle config, 
                                  char *section, char *key,
                                  unsigned char *value )
{
  unsigned int val = 0;
  unsigned int result;

  CORE_VERIFY_PTR( config );

  result = ( config->vtable.ReadUint32( config, section, key, &val ) );

  /* Convert the integer to a boolean. */
  if ( CORE_CONFIG_SUCCESS == result )
  {
    *value = ( val ) ? 1 : 0;
  }

  return result;
}
