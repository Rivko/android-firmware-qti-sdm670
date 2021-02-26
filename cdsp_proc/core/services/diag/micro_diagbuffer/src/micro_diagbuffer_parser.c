/*============================================================================
  micro_diagbuffer_parser.c

  The parser functions translates any pointer dereferences to local
  address using mapping function set by the client before parsing.

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/micro_diagbuffer_parser.c#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/17   sa     Correctly handle NULL pointer case with client mapping function
06/08/17   sp     Created new file to support microULog parser functionality.
===========================================================================*/

#include "CoreString.h"
#include "micro_diagbuffer_i.h"
#include "micro_diagbuffer_parser.h"

/**
 * <!-- micro_diagbuffer_MsgRemainingBytes -->
 *
 * @brief Add up the part1 and part2 sizes in the parse_data buffer. 
 *
 * @param parse_data: Pointer to a MICRO_DIAGBUFFER_READ_TO_ASCII_DATA struct of the msg data
 *                    and character string being working on
 * @return The number of bytes left to read in the parse_data structure. 
 */
static unsigned int micro_diagbuffer_MsgRemainingBytes(MICRO_DIAGBUFFER_READ_TO_ASCII_DATA* parse_data)
{
  return parse_data->part1_size + parse_data->part2_size;
}

/**
 * <!-- micro_diagbuffer_ReadNextMsgPart -->
 *
 * @brief Read the requested number of bytes from the msg data
 *
 * Because the data can come from 2 different arrays (part1 or part2) we use this
 * function to read the next amount of data from whatever source still has data. 
 *
 * @param dest : Pointer to destination memory
 * @param parse_data : Pointer to a MICRO_DIAGBUFFER_READ_TO_ASCII_DATA struct used to pass
 *                     around the msg data and character string being working on
 * @param bytesToRead : Usually 1, 4 or 8 bytes. 
 *
 * @return TRUE if data was read. 
 */
static uint32 micro_diagbuffer_ReadNextMsgPart( unsigned char* dest,
                                   MICRO_DIAGBUFFER_READ_TO_ASCII_DATA* parse_data,
                                   unsigned int bytesToRead )
{
  if (parse_data->part1_size>=bytesToRead)
  {
    core_memcpy(dest, bytesToRead, parse_data->msgpart1, bytesToRead);
    parse_data->part1_size -= bytesToRead;
    parse_data->msgpart1 += bytesToRead;
    return TRUE;
  }
  if (parse_data->part2_size>=bytesToRead)
  {
    core_memcpy(dest, bytesToRead, parse_data->msgpart2, bytesToRead);
    parse_data->part2_size -= bytesToRead;
    parse_data->msgpart2 += bytesToRead;
    return TRUE;
  }
  return FALSE;
}

/**
 * <!-- micro_diagbuffer_AddInsufficientSpaceMessage -->
 *
 * @brief Indicate there's text in the log message we couldn't add to the string.  
 *
 * @param parse_data : Pointer to a MICRO_DIAGBUFFER_READ_TO_ASCII_DATA struct used to pass
 *                     around the msg data and character string being working on.
 */
static void micro_diagbuffer_AddInsufficientSpaceMessage( MICRO_DIAGBUFFER_READ_TO_ASCII_DATA* parse_data )
{
  //Put an error-like message in the buffer if possible.  
  if (parse_data->outputsz_size > strlen("<...>"))
  {
    SNPRINTF_AND_MOVEPTRS1("<...>");
    return;
  }
  return;
}

/**
 * <!-- micro_diagbuffer_isNotConversionChar -->
 *
 * @brief Used for finding the end of strings like %08x.  
 *
 * When a format letter like x or d is found the format string is done. 
 *
 * @param fmtChar : Character to check. 
 *
 * @return TRUE if the character is not a conversion character 
 */
static uint32 micro_diagbuffer_isNotConversionChar(char fmtChar)
{
  const char convChar[] = "%EGXcdefgimnopstux";
  int high, mid, low;

  low = 0;
  high = sizeof(convChar)-1;
  while (low <= high)
  {
    mid = (low + high) / 2;
    if (fmtChar == convChar[mid])
    {
      return(FALSE);
    }
    else if (fmtChar > convChar[mid])
    {
      low = mid+1;
    }
    else
    {
      high = mid-1;
    }
  }
  return(TRUE);
}

/**
 * <!-- micro_diagbuffer_RealTimeWordArrayParse_vaddr -->
 *
 * @brief Do the RealTimeWordArray to string parse work.  
 *
 * @param parse_data : Pointer to a MICRO_DIAGBUFFER_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param msgFormat : MICRO_ULOG_SUBTYPE_REALTIME_WORDDATA.
 * @param mapping_fptr : mapping function set by client for address translation
 */
static void micro_diagbuffer_RealTimeWordArrayParse_vaddr(
                                         MICRO_DIAGBUFFER_READ_TO_ASCII_DATA* parse_data,
                                         uint32 msgFormat,
                                         vaddr_fptr mapping_fptr)
{
  uint32 columns = 0;
  int wrapData = 0;
  unsigned int value;

  while(micro_diagbuffer_MsgRemainingBytes(parse_data) &&
        (parse_data->outputsz_size > 11))
  {
    if (msgFormat == MICRO_ULOG_SUBTYPE_REALTIME_WORDDATA)
    {
      // WordArray
      wrapData = 1;
      micro_diagbuffer_ReadNextMsgPart((unsigned char*)&value, parse_data, 4);
      if (parse_data->outputsz_size > SNPRINTF_MIN_SIZE){ //ensure there's enough space to write the string	  
        SNPRINTF_AND_MOVEPTRS2("0x%08X ", value);
      }else{
        micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
        return;
      }
    }
    else
    {
      micro_diagbuffer_ReadNextMsgPart((unsigned char*)&value, parse_data, 4);
      if (parse_data->outputsz_size > SNPRINTF_MIN_SIZE){ //ensure there's enough space to write the string   
        SNPRINTF_AND_MOVEPTRS2("0x%08X ", value);
      }else{
        micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
        return;
      }
    }
    columns++;
    if ((columns == 8) && (micro_diagbuffer_MsgRemainingBytes(parse_data)) && (wrapData))
    {
      // Prep the next row of data.
      if (parse_data->outputsz_size > SNPRINTF_MIN_SIZE){ //ensure there's enough space to write the string	  
        SNPRINTF_AND_MOVEPTRS1("\n            ");
      }else{
        micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
        return; 
      }
      columns = 0;
    }
  }

  return;
}

/**
 * <!-- micro_diagbuffer_RealTimePrintfParse_vaddr -->
 *
 * @brief Do the character by character printf parse work.  
 *
 * @param parse_data : Pointer to a MICRO_DIAGBUFFER_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param log : microULog handle.
 * @param mapping_fptr : mapping function set by client for address translation
 */
static void micro_diagbuffer_RealTimePrintfParse_vaddr(
                                                         MICRO_DIAGBUFFER_READ_TO_ASCII_DATA* parse_data,
                                                         micro_ULogHandle log,
                                                         vaddr_fptr mapping_fptr)
{
  char * fmtString;
  #define SUBFMT_MAXSIZE 128
  char subFmtString[SUBFMT_MAXSIZE];
  char * fmtPtr;      //pointer to read individual characters inside the format string
  char * subOutPtr;   //pointer to access individual characters inside the subformat string 
  char is64BitValue = 0;
  uint32 localWord;
  uint64 local64Word;
  char *isRef;        //Indicate if a subformat is a reference data
  boolean is_mapped_localWord = FALSE;

  
  // Make sure the inputs look ok.
  if((parse_data->outputsz_size==0)||  //there's space to put the output string
     (parse_data->part1_size==0)||     //there's data to read (printfs are only ever in the part1 data 
     (parse_data->msgpart1==NULL)||    //the input pointer looks reasonable
     (parse_data->outputsz==NULL)||     //the outputsz pointer looks reasonable
     (log == NULL))
  {        
    return;
  }
  
  // Read in the pointer to the format string
  micro_diagbuffer_ReadNextMsgPart((unsigned char*)&localWord, parse_data, 4);

  if(localWord!=0)
  {
  if (mapping_fptr)
  {
    /* Convert pointer to local address using vaddr mapping function */
    fmtString = (char *)localWord;
    fmtPtr = (char *)mapping_fptr((uint32)fmtString);
  }
  else
  { 
    fmtPtr = (char*)localWord;
  }
  }
  else /* If localWord==0 in any case */
  {
     localWord = (uint32)"<NULL REFERENCE FORMAT STRING>";
     fmtPtr= (char*)localWord;
  }
    
  
  // Parse the string and add in other values.
  while ((*fmtPtr) &&                     //the string pointer is not NULL
         (parse_data->outputsz_size))    //there is space in the outputsz buffer
  {
    if (*fmtPtr == '%')
    {
      // -- We are starting a % sub format string.  Copy and parse it. --
      subOutPtr = subFmtString;

      //start with the assumption that anything we parse will be a normal 32 bit value. 
      is64BitValue=0; //flag this value as a 32 bit value
      do
      {
        *subOutPtr++ = *fmtPtr++;
        //repeat the loop if the last character read is not a conversion char,
        //we haven't hit the end of the string, and we haven't exceeded our 
        //subformat buffer for this copy
      } while(micro_diagbuffer_isNotConversionChar(*fmtPtr)&&(*fmtPtr)&&(((uint32)subOutPtr-(uint32)subFmtString)<(SUBFMT_MAXSIZE-1)));
      *subOutPtr++ = *fmtPtr;
      *subOutPtr = 0;  //null terminate subformat the string

      if (*fmtPtr == '%')
      {
        // There was % following the % instead of an expected conversion character. 
        if (parse_data->outputsz_size > strlen(" ")){
           ADDCHAR_AND_MOVEPTRS(' ');
         }
      }
      else
      {
        if (*fmtPtr == 't')
        {
          // Format specifier for current time stamp. Current time requested 

          local64Word = MICRO_ULOG_GETTIME64();
          core_strlcpy(subFmtString, "0x%016X", SUBFMT_MAXSIZE);
          is64BitValue=1; //flag this value as a 64 bit value
        }
        else
        { 
          // The format char refers to a typical value (%c, %d, %llx) or something that has to be pointed to(%&x, %&llx).
          // First make sure there are bytes enough in this message where there could be a pointer. 
          if (parse_data->part1_size >= 4)
          {
            /* Check whether the 64 bit value is an address or data */ 
            if ((strstr(subFmtString,"llu")!=0)||(strstr(subFmtString,"lld")!=0)||(strstr(subFmtString,"llx")!=0)||(strstr(subFmtString,"llX")!=0))
            {
              //It's a 64 bit value.
              isRef = strchr(subFmtString, '&');
              if (isRef != NULL)
              {
                unsigned int i;

                //read in the pointer
                micro_diagbuffer_ReadNextMsgPart((unsigned char*)&localWord, parse_data, 4);

                // Fix the string by removing the & from the subFmtString
                // isRef points to & in the string, so copy to that location
                // from one character later.
                for (i=0; i<strlen(isRef); i++){
                  isRef[i] = isRef[i+1];
                }

                /* Dereference the pointer to get the data */
                if (localWord != 0) 
                {
                   if (mapping_fptr)
                   {
                     /* Convert pointer to local address before dereference */
                     localWord = mapping_fptr(localWord);
                     is_mapped_localWord = TRUE;
				   }
                   local64Word = *(uint64 *)localWord;
                   is64BitValue=1; //flag this value as a 64 bit value
                }
                else
                {
                  if (parse_data->outputsz_size > strlen("<Format Specified Without Arg>"))
                  {
                    SNPRINTF_AND_MOVEPTRS1("<Format Specified Without Arg>");
                    return;
                  } 
                  else
                  {
                    //no space to add an error message, just return.
                    return;
                  }
                }
              }else
                //read the 64 bit value from the buffer. 
                micro_diagbuffer_ReadNextMsgPart((unsigned char*)&local64Word, parse_data, 8);
                is64BitValue=1; //flag this value as a 64 bit value
            } 
            else 
            {
              // Not a 64 bit value. Do a normal 32 bit read
              is64BitValue=0; //make sure the 64 bit flag is not set.
              // Read in the pointer referred to us by the % format character.
              micro_diagbuffer_ReadNextMsgPart((unsigned char*)&localWord, parse_data, 4);
              /* Check whether the 32 bit value is an address or data */ 
              isRef = strchr(subFmtString, '&');
              if (isRef != NULL)
              {
                unsigned int i;
                /* The 32-bit value is an address. Dereference it to get the data */
                if (localWord != 0) 
                {
                  if (mapping_fptr)
                  {
                    /* Convert pointer to local address before dereference */
                    localWord = mapping_fptr(localWord);
                    is_mapped_localWord = TRUE;
                  }
                  localWord = *(uint32 *)localWord;
                } else {
                  if (parse_data->outputsz_size > strlen("<NULL pointer & reference>"))
                  {
                    SNPRINTF_AND_MOVEPTRS1("<Format Specified Without Arg>");
                    return;
                  } 
                  else
                  {
                    //no space to add an error message, just return.
                    return;
                  }
                }
                // Fix the string by removing the & from the subFmtString
                // isRef points to & in the string, so copy to that location
                // from one character later.
                // memcpy(isRef, isRef+1, strlen(isRef));
                for (i=0; i<strlen(isRef); i++){
                  isRef[i] = isRef[i+1];
                }
              }
            }
          } 
          else
          {
            // Got a % format, but there's no space remaining for pointers. Must be a malformed printf.
            // Put an error message in the buffer if possible.  
            if (parse_data->outputsz_size > strlen("<Format Specified Without Arg>"))
            {
              SNPRINTF_AND_MOVEPTRS1("<Format Specified Without Arg>");
              return;
            }
          }
        }
        if ((*fmtPtr == 's') && (localWord == 0))
        {
          localWord = (uint32)"<NULL REFERENCE>";
        }
        else if (*fmtPtr == 'f') 
        {
          // We must print a floating point value.  Extend it appropriately.
          FloatConverter f;
          f.u.uiData[0] = localWord;
          if ((parse_data->outputsz_size) > SNPRINTF_MIN_SIZE) //ensure there's enough space to write the string
          {
            SNPRINTF_AND_MOVEPTRS2(subFmtString, f.u.fData);
          }
          else 
          {
            micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
            return;           
          }
        }
        else
        {
          //unless the special 'f' or 's' handling above is used, this is where most of the format string printing is done.
          if (parse_data->outputsz_size > SNPRINTF_MIN_SIZE){ //ensure there's enough space to write the smallest string
            //recheck that we really think this is a printable format string
            if (micro_diagbuffer_isNotConversionChar(*fmtPtr)==FALSE)
            {
              if (is64BitValue==0)
              {
                if(*fmtPtr=='s')
                {
                  if ((mapping_fptr) && (!is_mapped_localWord))
                  {
                    localWord = mapping_fptr(localWord);
                  }
                  if ((parse_data->outputsz_size) > strlen((char*)localWord)) //ensure there's enough space to write the string
                  {
                    SNPRINTF_AND_MOVEPTRS2(subFmtString, localWord);
                  } else {
                    micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
                    return;           
                  }
                }
                else
                {
                  if ((parse_data->outputsz_size) > SNPRINTF_MIN_SIZE) //ensure there's enough space to write the string
                  {
                    SNPRINTF_AND_MOVEPTRS2(subFmtString, localWord);
                  } else {
                    micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
                    return;           
                  }
                }
              }
              else 
              {
                //it's a 64 bit value
                if ((parse_data->outputsz_size) > SNPRINTF_MIN_SIZE*2) //ensure there's enough space to write the string
                {
                  SNPRINTF_AND_MOVEPTRS2(subFmtString, local64Word);
                } else {
                  micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
                  return;           
                }
              }
            }
            else
            {
              if (parse_data->outputsz_size > strlen("<bad printf format>")){
                SNPRINTF_AND_MOVEPTRS1("<bad printf format>");
              }
            }
          }
          else 
          {
            micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
            return;
          }
        }
      }
      fmtPtr++;
    }
    else
    {
      // -- Not a % sub format string.  Copy the text to the outputsz buffer. --
      if (parse_data->outputsz_size > 2)
      {
        ADDCHAR_AND_MOVEPTRS(*fmtPtr);
      }
      fmtPtr++;
    }
  }

  return;
}

/**
 * <!-- micro_diagbuffer_FormatParseData_vaddr -->
 * 
 * @brief Add the timestamp to the string and then pass the data to specific parsers.
 * @param parse_data : Pointer to a MICRO_DIAGBUFFER_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param log : microULog handle.
 * @param mapping_fptr : mapping function set by client for address translation
 * @return MICRO_ULOG_SUCCESS if parsing is successful
 */
micro_ULogResult micro_diagbuffer_FormatParseData_vaddr(MICRO_DIAGBUFFER_READ_TO_ASCII_DATA* parse_data,
                                                                micro_ULogHandle log,
                                                                vaddr_fptr mapping_fptr)
{
  // Parse the real-time log and print the results to the output buffer.
  // Input message fomat to this function:
  // <Timestamp> <Format> <Data...>
  uint32 msgFormat;
  uint32 fmt;
  uint64 timestamp;

   if((parse_data == NULL) || (log == NULL))
    return ( MICRO_ULOG_ERR_INVALIDPARAMETER );

  // Check the size looks reasonable and read the format
  if (micro_diagbuffer_MsgRemainingBytes(parse_data) < MIN_MSG_LEN){ //ensure that the message is at least big enough for a FORMAT field
    return(MICRO_ULOG_ERROR);
  }

  // Read the format of this message
  micro_diagbuffer_ReadNextMsgPart((unsigned char*)&fmt, parse_data, 4);

  // Extra step of storing to fmt and then casting to msgFormat is to satisfy compiler
  msgFormat = (uint32)fmt;
  //clear off the length field. 
  msgFormat = msgFormat & ~MICRO_ULOG_LENGTH_MASK;


  // If it's a realtime message, read the timestamp.
  if ((msgFormat > 0) && (msgFormat < MICRO_ULOG_SUBTYPE_RESERVED1)){
    // The timestamp should be the next word in the parse_data
    micro_diagbuffer_ReadNextMsgPart((unsigned char*)&timestamp, parse_data, MICRO_ULOG_TIMESTAMP_SIZE);

    // Add the timestamp to the output string
    if (parse_data->outputsz_size > SNPRINTF_MIN_SIZE*2){ //ensure there's enough space to write the string
      SNPRINTF_AND_MOVEPTRS2("0x%016llX: ", (uint64)timestamp);
    }
    else
    {
      micro_diagbuffer_AddInsufficientSpaceMessage(parse_data);
      return(MICRO_ULOG_ERR_INSUFFICIENT_BUFFER);
    }
  } //end of code to add TS

  switch (msgFormat)
  {
    case MICRO_ULOG_SUBTYPE_REALTIME_WORDDATA:     // An array of uint32 data
      micro_diagbuffer_RealTimeWordArrayParse_vaddr(parse_data, msgFormat, mapping_fptr);
      break;

    case MICRO_ULOG_SUBTYPE_REALTIME_PRINTF:      // A printf style message
      micro_diagbuffer_RealTimePrintfParse_vaddr(parse_data, log, mapping_fptr);
      break;

    default:
      break;
  }

  // Add the linefeed to the end of the message if there's space
  if (parse_data->outputsz_size > strlen("\n"))
  { //ensure there's enough space to write the string
    ADDCHAR_AND_MOVEPTRS('\n')
  }

  return(MICRO_ULOG_SUCCESS);
}

/**
 * <!-- micro_diagbuffer_FormatMsg_vaddr -->
 * 
 * @brief Prepare the message into our convenient parse_data structure.
 *
 * @param log : Pointer to the log the data came from
 * @param message : The binary log data to parse/convert to ascii. 
 * @param outputsz : Pointer to memory to place the ascii decoded log. 
 * @param outputSizeBytes : How much memory is available at outputsz. 
 * @param wordsConsumed : How many bytes of the log message were decoded.
 * @param mapping_fptr : mapping function set by client for address translation
 *
 * @return MICRO_ULOG_SUCCESS if message format is successful
 */

micro_ULogResult micro_diagbuffer_FormatMsg_vaddr( micro_ULogHandle log,
                                  char *  message,
                                  char *  outputsz,
                                  uint32 outputSizeBytes,
                                  uint32 * wordsConsumed,
                                  vaddr_fptr mapping_fptr) 
{
  uint32 msgLength;
  MICRO_DIAGBUFFER_READ_TO_ASCII_DATA parse_data;

  // Format the message as a string, and put it in the outputsz buffer.
  // Return the number of words used.
  if ((outputSizeBytes == 0) ||
      (outputsz == NULL) ||
      (message == NULL) ||
      (log == NULL))
  {
    return(MICRO_ULOG_ERR_INVALIDPARAMETER);
  }

  //NULL terminate the string
  outputsz[0] = 0;

  // Read the length of this message
  core_memcpy(&msgLength, 4, &message[0], 4);
  msgLength = (msgLength & MICRO_ULOG_LENGTH_MASK) >> MICRO_ULOG_LENGTH_SHIFT;

  //set the amount of message words we'll be handling
  *wordsConsumed = MICRO_ULOG_PADCOUNT(msgLength);

  if ((msgLength < MIN_MSG_LEN) || (msgLength > MICRO_ULOG_MAX_MSG_SIZE))
  {
    // If it's too small or too large, we have an invalid formatted message.
    // Throw it away.
    *outputsz = 0;
    return(MICRO_ULOG_ERROR);
  }

  parse_data.msgpart1 = message;
  parse_data.part1_size = msgLength;
  parse_data.msgpart2 = NULL;
  parse_data.part2_size = 0;
  parse_data.outputsz = outputsz;
  parse_data.outputsz_size = outputSizeBytes;
  parse_data.outOffset = 0;
  parse_data.environment = MICRO_DIAGBUFFER_PARSE_READ_DATA;//decoding a stored log, not a data stream.

  micro_diagbuffer_FormatParseData_vaddr(&parse_data, log, mapping_fptr);

  *wordsConsumed = parse_data.outOffset;

  return(MICRO_ULOG_SUCCESS);
}

