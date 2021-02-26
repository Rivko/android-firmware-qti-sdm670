/*==============================================================================
@file CoreIni.c

The functions in this file should be accessed by first
calling CoreIni_ConfigCreate() to get a config object, and
then passing that object to the CoreConfig subsystem.  When
the operations are complete, CoreIni_ConfigDestory() must
be called.

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreIni.c#1 $
==============================================================================*/
#include "CoreIni.h"
#include "CoreFile.h"
#include "CoreConfig.h"
#include "CoreConfigInternal.h"
#include "CoreString.h"
#include "CoreVerify.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CoreString.h"

/* ==================================================================
                  FORWARD DECLARATIONS
   ================================================================== */              

static unsigned int CoreIni_ReadString( CoreConfigHandle config, 
                                        char *section, char *key, 
                                        char *output, 
                                        unsigned int *outputSize );

static unsigned int CoreIni_ReadUint32( CoreConfigHandle config, char *section, 
                                        char *key, unsigned int *output );

/* ==================================================================
                     TYPE DEFINITIONS
   ================================================================== */              

/**
 * CoreIniConfig 
 *  
 * @brief This structure is used to prepare the proper data to 
 * pass to the CoreConfig subsystem. 
 */
typedef struct CoreIniConfig
{
  CoreConfig base;
  int        fileHandle;
} CoreIniConfig;


/* ==================================================================
                      GLOBAL DATA
   ================================================================== */              
           
/**
 * iniConfig 
 *  
 * @brief This structure contains the function pointers needed 
 *        by CoreConfig, for calling the functions in this file.
*/
CoreConfig iniConfig =
{
  {
    CoreIni_ReadUint32,
    CoreIni_ReadString
  }
};


/* ==================================================================
              INTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/**
 * CoreIni_ReadString
 *
 * @brief Reads the value for the key associated with the 
 *        specified category name. The value is returned as a
 *        string, and it's up to the user to convert it as they
 *        wish.
 *  
 * @param filename:   Complete path to the ini file to read.
 * @param category:   Name of the category to read the key from.
 * @param key:        Key name to get the associated data for. 
 * @param output:     Output buffer to write the value 
 *                    associated with the specific key.
 * @param outputSize: Size of the output buffer to write the 
 *                    value.
 * 
 * @return CORE_CONFIG_SUCCESS if the key was read successfully. 
 *         CORE_CONFIG_ERROR_SECTION_NOT_FOUND if the section
 *         was not found.
 *         CORE_CONFIG_ERROR_KEY_NOT_FOUND if the key was not
 *         found.
 *         CORE_CONFIG_ERROR_INSUF_SIZE if the output buffer was
 *         not big enough for the data in the file.
 */ 
static unsigned int CoreIni_ReadString( CoreConfigHandle config, 
                                        char *section, char *key, 
                                        char *output, 
                                        unsigned int *outputSize )
{
  char keyString[128];
  char *keyData = NULL;
  unsigned char result;
  int  retVal = CORE_CONFIG_SUCCESS;
  char nextLine[128];
  CoreIniConfig *iniConfig = ( CoreIniConfig *)config;
  size_t ret;

  core_snprintf( keyString, sizeof(keyString), "[%s]", section );

  /* Search for the [section] in the file */
  do
  {
    result = FALSE;
    while ( CoreFile_GetNextLine( iniConfig->fileHandle, 
                                  nextLine, 128 ) == TRUE )
    {
      if ( strncmp( keyString, nextLine, strlen( keyString ) ) == 0 )
      {
        /* We found the section. */
        result = TRUE;
        break;
      }
    }
    if ( result == FALSE )
    {
      /* We did not find the section.  Error. */
      retVal = CORE_CONFIG_ERROR_SECTION_NOT_FOUND;
      break;
    }

    result = FALSE;

    memset(keyString, '\0', sizeof(keyString));
    core_snprintf( keyString, sizeof(keyString), "%s=", key );
  
    /* Search for the key= */
    while ( CoreFile_GetNextLine( iniConfig->fileHandle, 
                                  nextLine, 128 ) == TRUE )
    {
      if ( strncmp( keyString, nextLine, strlen( keyString ) ) == 0 )
      {
        /* We found the key. */
        result = TRUE;
        break;
      }
      else if ( nextLine[0] == '[' )
      {
        /* We're at the next section.  Error. */
        break;
      }
    }

    if ( result == FALSE )
    {
      /* We did not find the key.  Error. */
      retVal = CORE_CONFIG_ERROR_KEY_NOT_FOUND;
      break;
    }

    /* Set the output.  Make sure the key string fits into the destination
     * buffer, with room for the end of string character.  If it doesn't,
     * return an error. */
    keyData = &nextLine[strlen( keyString )];

    ret = core_strlcpy( output, keyData, *outputSize );

    if ( ret >= *outputSize )
    {
      /* too long */
      retVal = CORE_CONFIG_ERROR_INSUF_SIZE;
    }

    /* Tell the user how many bytes were actually written/needed */    
    *outputSize = ret;

    /*
     * We are done - section and key is processed 
     * Assumption: Only the first occurrence of the section and the key is 
     *             processed. 
     */
    break;

  } while ( 0 );

  /*
   * Move file pointer back to the begining so that next 
   * search can start at the top of the file. 
   */
  CoreFile_Rewind( iniConfig->fileHandle );

  return( retVal );
}

/**
 * CoreIni_ReadUint32
 *
 * @brief Reads the value for the key associated with the 
 *        specified category name, and returns it as a uint32.
 *  
 * @param filename:   Complete path to the ini file to read.
 * @param category:   Name of the category to read the key from.
 * @param key:        Key name to get the associated data for. 
 * @param output:     Output buffer to write the value 
 *                    associated with the specific key.
 * 
 * @return CORE_CONFIG_SUCCESS if the key was read successfully. 
 *         CORE_CONFIG_ERROR_SECTION_NOT_FOUND if the section
 *         was not found.
 *         CORE_CONFIG_ERROR_KEY_NOT_FOUND if the key was not
 *         found.
 *         CORE_CONFIG_ERROR_INSUF_SIZE if the output buffer was
 *         not big enough for the data in the file.
 */ 
static unsigned int CoreIni_ReadUint32( CoreConfigHandle config, char *section, 
                                        char *key, unsigned int *output )
{
  char value_string[10] = {0};
  unsigned char retVal = CORE_CONFIG_SUCCESS;
  unsigned int string_size = 10;

  if ( CORE_CONFIG_SUCCESS == ( retVal = CoreIni_ReadString( config, section, 
                                                             key, 
                                                             value_string, 
                                                             &string_size ) ) )
  {
    /* Convert the string that was read to an integer. */
    *output = atoi( value_string );
  }

  return retVal;
}


/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/*
 * CoreIni_ConfigCreate
 */
CoreConfigHandle CoreIni_ConfigCreate( const char *filename )
{
  CoreIniConfig *iniObj = ( CoreIniConfig * )malloc( sizeof( CoreIniConfig ) );

  CORE_VERIFY_PTR( iniObj );

  /* Try to open the file.  If it does not open, return a NULL handle. */
  if ( FALSE == CoreFile_Open( filename, CORE_FILE_READ_ONLY, 
                               &( iniObj->fileHandle ) ) )
  {
    free(iniObj);
    return( NULL );
  }

  /* Fill in the vtable part of the iniObj */
  iniObj->base = iniConfig;

  /* Klocworks - Even though &iniObj->base is the address we want to return,
   * Klocworks thinks that the pointer to iniObj malloced above is lost.
   * So typecase iniObj and return it. 
   */ 
  return( (CoreConfigHandle) iniObj );
}

/*
 * CoreIni_ConfigDestroy
 */
unsigned int CoreIni_ConfigDestroy( CoreConfigHandle config )
{
  CoreIniConfig *iniConfig = ( CoreIniConfig * )config;

  CoreFile_Close( iniConfig->fileHandle );

  free( iniConfig );

  return CORE_CONFIG_SUCCESS;
}
