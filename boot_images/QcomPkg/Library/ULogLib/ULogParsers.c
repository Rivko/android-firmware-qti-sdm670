/*============================================================================
@file ULogParsers.c

This module implements the parsing of ULOG data from different ULOG interfaces. 

Copyright (C) 2009-2015 by Qualcomm Technologies Inc.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "ULogBuildFeatures.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DEFEATURE_ULOG_MSG_FORMAT
#include "DALSys.h"
#include "ULog.h"
#include "ULogCoreInternal.h"
#include "CoreString.h"
#include "CoreTime.h"


/** A minimum realtime message has a len/fmt word + a 4 or 8 byte timestamp*/
#ifdef ULOG_64BIT_TIME
  #define MIN_REALTIME_MSG_LEN 12
#else
  #define MIN_REALTIME_MSG_LEN 8
#endif

/** For sprintfs of 32 bit numbers, what is a safe minimum number of available bytes needed */
#define SNPRINTF_MIN_SIZE 14
#define SNPRINTF_MIN_SIZE_64 28


/**
 * Macros for adding characters to strings. These operations are done a lot,
 * so these help make the code a little more compact and hopefully easier to read. 
 * To avoid lots of strcats and copies we mostly do snprintf at the end of the
 * existing strings. 
 */
//no argument "...." string only type sprintf string
#define SNPRINTF_AND_MOVEPTRS1(x) {unsigned int bytesOut; bytesOut = core_snprintf(&(parse_data->outputsz[parse_data->outOffset]), parse_data->outputsz_size, x); parse_data->outOffset += bytesOut;  parse_data->outputsz_size -= bytesOut;}
//one argument  "..%x.",value type string
#define SNPRINTF_AND_MOVEPTRS2(x, y) {unsigned int bytesOut; bytesOut = core_snprintf(&(parse_data->outputsz[parse_data->outOffset]), parse_data->outputsz_size, x, y); parse_data->outOffset += bytesOut; parse_data->outputsz_size -= bytesOut;}
//add a single char to the string.
#define ADDCHAR_AND_MOVEPTRS(x) {parse_data->outputsz[parse_data->outOffset]=x; parse_data->outOffset+=1; parse_data->outputsz[parse_data->outOffset]=0; parse_data->outputsz_size--;}


/**
 * <!-- ULogCore_MsgFormat -->
 *
 * @brief Format an individual message retrieved with the ULogCore_Read function.
 *
 * @param h : A ULog handle to the log we are formatting.
 * @param msg : The unformatted binary msg data
 * @param msgString : The memory to be filled in with the output string.
 * @param msgStringSize : The size of the msgString memory.
 * @param msgConsumed : Number of bytes consumed from the msg data buffer.
 *
 * @return DAL_SUCCESS if the format was successful. Errors are
 *        defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_MsgFormat( ULogHandle h,
                               char *  msg,
                               char *  msgString,
                               uint32 msgStringSize,
                               uint32 * msgConsumed )
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  return(ULogParsers_FormatMsg(log, msg, msgString, msgStringSize, msgConsumed));
}


/**
 * <!-- isNotConversionChar -->
 *
 * @brief Used for finding the end of strings like %08x.  
 *
 * When a format letter like x or d is found the format string is done. 
 *
 * @param fmtChar : Character to check. 
 *
 * @return TRUE if the character is not a conversion character 
 */
static uint32 isNotConversionChar(char fmtChar)
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
 * <!-- Add_InsufficientSpaceMessage -->
 *
 * @brief Indicate there's text in the log message we couldn't add to the string.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct used to pass
 *                     around the msg data and character string being working on.
 */
static void Add_InsufficientSpaceMessage( ULOG_READ_TO_ASCII_DATA* parse_data )
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
 * <!-- UlogReadNextMsgPart -->
 *
 * @brief Read the requested number of bytes from the msg data
 *
 * Because the data can come from 2 different arrays (part1 or part2) we use this
 * function to read the next amount of data from whatever source still has data. 
 *
 * @param dest : Pointer to destination memory
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct used to pass
 *                     around the msg data and character string being working on
 * @param bytesToRead : Usually 1, 4 or 8 bytes. 
 *
 * @return TRUE if data was read. 
 */
static uint32 UlogReadNextMsgPart( unsigned char* dest,
                                   ULOG_READ_TO_ASCII_DATA* parse_data,
                                   unsigned int bytesToRead )
{
  if (parse_data->part1_size>=bytesToRead)
  {
    core_memcpy( dest, bytesToRead, parse_data->msgpart1, bytesToRead );
    parse_data->part1_size -= bytesToRead;
    parse_data->msgpart1 += bytesToRead;
    return TRUE;
  }
  if (parse_data->part2_size>=bytesToRead)
  {
    core_memcpy( dest, bytesToRead, parse_data->msgpart2, bytesToRead );
    parse_data->part2_size -= bytesToRead;
    parse_data->msgpart2 += bytesToRead;
    return TRUE;
  }
  return FALSE;
}


/**
 * <!-- UlogSnoopNextMsgPart -->
 *
 * @brief Read the next value out of the arrays, but leave the data in the queue.
 *
 * @param dest : Pointer to destination memory
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                    and character string being working on
 * @param bytesToRead : Usually 4 bytes
 *
 * @return TRUE if data was read. 
 */
static uint32 UlogSnoopNextMsgPart( unsigned char* dest,
                                    ULOG_READ_TO_ASCII_DATA* parse_data,
                                    unsigned int bytesToRead )
{
  if (parse_data->part1_size>=bytesToRead)
  {
    core_memcpy(dest,bytesToRead, parse_data->msgpart1, bytesToRead);
    return TRUE;
  }
  if (parse_data->part2_size>=bytesToRead)
  {
    core_memcpy(dest,bytesToRead, parse_data->msgpart2, bytesToRead);
    return TRUE;
  }
  return FALSE;
}


/**
 * <!-- UlogMsgRemainingBytes -->
 *
 * @brief Add up the part1 and part2 sizes in the parse_data buffer. 
 *
 * @param parse_data: Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                    and character string being working on
 * @return The number of bytes left to read in the parse_data structure. 
 */
static unsigned int UlogMsgRemainingBytes(ULOG_READ_TO_ASCII_DATA* parse_data)
{
  return parse_data->part1_size + parse_data->part2_size;
}


/**
 * <!-- HandleNextMultipartMsg -->
 *
 * @brief Process a multipart message, somewhat recursively. 
 * 
 * Multipart Messages(abreviated MPMs) always take the form of a 
 * ULogFront_RealTimePrintf with one or more %m format specifiers in the
 * format string, followed by the ULogFront_RealTime* log messages that
 * will fill in the %m's. 
 *
 * All parse functions work on a ULOG_READ_TO_ASCII_DATA structure. 
 * This function creates another one of these structures to isolate
 * individual MPM parts (single ULOG messages) and parse/add them together.
 * The function uses mpm_parse_data to point to the "next" part of the 
 * composite MPM message.  Called again, it moves the next to the front 
 * and sets up a new "next". 
 *
 * MPM data is always in part1 of the ULOG_READ_TO_ASCII_DATA so that makes the 
 * pointer moves possible in this MPM parsing.
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on
 * @param mpm_parse_data : NULL at the start of a MPM, the next/remaining msg 
 *                         data on subsequent calls. 
 */
static void HandleNextMultipartMsg( ULOG_READ_TO_ASCII_DATA* parse_data,
                                    ULOG_READ_TO_ASCII_DATA* mpm_parse_data )
{
  uint32 padding = 0;
  uint32 length;

  // Used in the recursive calls to keep track of the total MPM and break out
  // individual msgs to parse.
  ULOG_READ_TO_ASCII_DATA new_parse_data;  

  if ( mpm_parse_data == NULL )
  {
     //It's the start of a new MPM. The format and the TS words, something like:
    //[00600007] [1f7c1337 00000000]  have already been consumed.
     //The next bytes in the parse_data are the first msg (typically a printf)
     
    //Make sure there's at least one msg remaining to parse.
    if ( UlogMsgRemainingBytes( parse_data ) < MIN_REALTIME_MSG_LEN )
    {
      return;
    }

    // Read the length of the next message to prepare to arrange the parse data structures
    UlogSnoopNextMsgPart((unsigned char*)&length, parse_data, 4);    
    length = (length & ULOG_LENGTH_MASK) >> ULOG_LENGTH_SHIFT;

    //Start filling in a new structure to point to the message following the next one
    new_parse_data.msgpart1 = parse_data->msgpart1;  
    new_parse_data.msgpart2 = 0;
    new_parse_data.part1_size = parse_data->part1_size;
    new_parse_data.part2_size = 0;
    new_parse_data.outputsz = parse_data->outputsz;
    new_parse_data.outputsz_size = parse_data->outputsz_size;  //the total size for the MPM
    new_parse_data.outOffset = parse_data->outOffset;
    new_parse_data.environment = ULOG_PARSE_READ_DATA;        //decoding a stored log, not a data stream.

    //Now that we've backed up the key info for this MPM, we repurpose the
    //parse_data structure to point to only the next ULog msg. Typically
    //this message is a ULogFront_RealTimePrintf with one or more "%m" in
    //the format string.
    //Set the length in parse_data to ONLY the length of the next msg.
    parse_data->part1_size = length;                
    parse_data->part2_size = 0;
   
    // Advance the mpm parse data we filled in earlier to the end of the next
    // message.  msgs only start on word boundaries.
    if ((length&0x3)!=0)
    {
      padding = (length & 0x3);  //messages always start on a word boundary.  Calculate if padding is needed.
    }
    length = length + padding;  // Find the start of the next message.
    
    //Set the length in new_parse_data parse_data for all of the remaining data
    //after the msg we're about to parse.
    new_parse_data.msgpart1 += length;
    new_parse_data.part1_size -= length;

    //parse_data points to the first part of the MPM (almost always a ULOG_REALTIME_SUBTYPE_PRINTF msg)
    //and new_parse_data is the log msg to fill the first %m msg afterwards.
    ULogParsers_FormatParseData(parse_data, &new_parse_data);
  }
  else
  {  
    // A MPM has begun and we're now at a %m in a printf.
     // mpm_parse_data was previously setup to point to the next msg to process
     // move forward to parse the next part of the message and update pointers

    //make sure there's remaining msgs to process.
    if (UlogMsgRemainingBytes(mpm_parse_data) < MIN_REALTIME_MSG_LEN)
    {
      if (parse_data->outputsz_size > strlen("<Mutipart Err>"))
      {
        SNPRINTF_AND_MOVEPTRS1("<Mutipart Err>");
      }
      return;
    }

    //read the length in the next msg
    UlogSnoopNextMsgPart((unsigned char*)&length, mpm_parse_data, 4);    
    length = (length & ULOG_LENGTH_MASK) >> ULOG_LENGTH_SHIFT;

    //create a parse data structure to setup the read of the next msg
    new_parse_data.msgpart1 = mpm_parse_data->msgpart1;  //next msg pointer that we recorded earlier
    new_parse_data.msgpart2 = NULL;
    new_parse_data.part1_size = length;
    new_parse_data.part2_size = 0;
    //pass the current string info for the next parse to add to.
    new_parse_data.outputsz = parse_data->outputsz; 
    new_parse_data.outputsz_size = parse_data->outputsz_size; 
    new_parse_data.outOffset = parse_data->outOffset;  
    new_parse_data.environment = ULOG_PARSE_READ_DATA;        //decoding a stored log, not a data stream.

    // Update the total MPM data structure now that we're about to process
    // another msg from it. Advance the data pointer to the end of the
    // msg we're about to process.
    if ( (length & 0x3) != 0 )
    {
      padding = 4 - (length & 0x3);  //messages always start on a word boundary.  Calculate if padding is needed.
    }
    length = length + padding;  // Find the start of the next message.

    //advance the pointer to the next start
    mpm_parse_data->msgpart1 = mpm_parse_data->msgpart1 + length; 
    //reduce the remaining size by the amount we're about to process.
    mpm_parse_data->part1_size -= length;
    ULogParsers_FormatParseData(&new_parse_data, mpm_parse_data);

    //after the return, update the string values of the original parse_data
    parse_data->outOffset = new_parse_data.outOffset;
    parse_data->outputsz_size = new_parse_data.outputsz_size;
  }

  return;
}


/**
 * <!-- ULFE_RealTimePrintfParse -->
 *
 * @brief Do the character by character printf parse work.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 */
static void ULFE_RealTimePrintfParse( ULOG_READ_TO_ASCII_DATA* parse_data,
                                       ULOG_READ_TO_ASCII_DATA* mpm )
{
  char * fmtString;
  #define SUBFMT_MAXSIZE 128
  char subFmtString[SUBFMT_MAXSIZE];
  char * fmtPtr;      //pointer to read individual characters inside the format string
  char * subOutPtr;   //pointer to access individual characters inside the subformat string
  char is64BitValue = 0;
  uint32 localWord = 0;
  uint64 local64Word = 0;
  char *isRef;        //Indicate if a subformat is a reference data

  
  // Make sure the inputs look ok.
  if((parse_data->outputsz_size==0)||  //there's space to put the output string
       (parse_data->part1_size == 0) ||     //there's data to read (printfs are only ever in the part1 data
     (parse_data->msgpart1==NULL)||    //the input pointer looks reasonable
     (parse_data->outputsz==NULL))     //the outputsz pointer looks reasonable
  {        
    return;
  }
  
  // Read in the pointer to the format string
#ifndef ULOG_64BIT_PROC
  UlogReadNextMsgPart((unsigned char*)&localWord, parse_data, 4);
  fmtString = (char *)localWord;
#else
  UlogReadNextMsgPart( (unsigned char *)&local64Word, parse_data, 8 );
  fmtString = (char *)local64Word;
#endif
  if (fmtString == 0)
  {
    fmtString = "<NULL REFERENCE FORMAT STRING>";
  }

  // Initialize the pointer used to move around format the string.
  fmtPtr = fmtString;
  
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
#ifndef ULOG_64BIT_PROC
      }
      while ( isNotConversionChar( *fmtPtr ) && (*fmtPtr) && (((uint32)subOutPtr - (uint32)subFmtString) < (SUBFMT_MAXSIZE - 1)) );
#else
      } while ( isNotConversionChar( *fmtPtr ) && (*fmtPtr) && (((uint64)subOutPtr - (uint64)subFmtString) < (SUBFMT_MAXSIZE - 1)) );
#endif
      *subOutPtr++ = *fmtPtr;
      *subOutPtr = 0;  //null terminate subformat the string

      if (*fmtPtr == '%')
      {
        // There was % following the % instead of an expected conversion character.
        if ( parse_data->outputsz_size > strlen( " " ) )
        {
           ADDCHAR_AND_MOVEPTRS(' ');
         }
      }
      else
      {
        if (*fmtPtr == 't')
        {
          // Format specifier for current time stamp. Current time requested
#ifdef ULOG_64BIT_TIME
          local64Word = CoreTimetick_Get64();
          core_strlcpy(subFmtString, "0x%016X", SUBFMT_MAXSIZE);
          is64BitValue=1; //flag this value as a 64 bit value
#else
          localWord = CoreTimetick_Get();
          core_strlcpy(subFmtString, "0x%08X", SUBFMT_MAXSIZE);
#endif
        }
        else if (*fmtPtr == 'm')
        {

          if ( parse_data->environment == ULOG_PARSE_DATA_STREAM )
          {
            if (parse_data->outputsz_size > strlen("<mpm>"))
            {
              SNPRINTF_AND_MOVEPTRS1("<mpm>");
              subFmtString[0] = 0;
            }
          } 
          else if (mpm == NULL)
          {
            if (parse_data->outputsz_size > strlen("<Mutipart Err>"))
            {
              SNPRINTF_AND_MOVEPTRS1("<Mutipart Err>");
              subFmtString[0] = 0;
            }
          }
          else if (parse_data->environment == ULOG_PARSE_READ_DATA)
          {
            HandleNextMultipartMsg(parse_data, mpm);
            subFmtString[0] = 0;
          }
        }
        else
        { 
          // The format char refers to something that has to be pointed to.  Make sure there are bytes
          // enough in this message where there could be a pointer.
          if (parse_data->part1_size >= 4)
          {
            /* Check whether the 64 bit value is an address or data */ 
#ifdef ULOG_64BIT_PROC
            if ( (strstr( subFmtString, "llu" ) != 0) || (strstr( subFmtString, "lld" ) != 0) || (strstr( subFmtString, "llx" ) != 0) || (strstr( subFmtString, "llX" ) != 0) || (strstr( subFmtString, "s" ) != 0) || (strstr( subFmtString, "p" ) != 0) )
#else
            if ((strstr(subFmtString,"llu")!=0)||(strstr(subFmtString,"lld")!=0)||(strstr(subFmtString,"llx")!=0)||(strstr(subFmtString,"llX")!=0))
#endif
            {
              //It's a 64 bit value.
              isRef = strchr(subFmtString, '&');
              if (isRef != NULL)
              {
                unsigned int i;

                //read in the pointer
#ifdef ULOG_64BIT_PROC
                  UlogReadNextMsgPart( (unsigned char *)&local64Word, parse_data, 8 );
#else
                  UlogReadNextMsgPart( (unsigned char *)&localWord, parse_data, 4 );
#endif
                // Fix the string by removing the & from the subFmtString
                // isRef points to & in the string, so copy to that location
                // from one character later.
                  for ( i = 0; i < strlen( isRef ); i++ )
                  {
                  isRef[i] = isRef[i+1];
                }

                /* Dereference the pointer to get the data */
#ifndef ULOG_64BIT_PROC
                  if ( localWord != 0 )
                {
                    local64Word = *(uint64 *)localWord;
                   is64BitValue=1; //flag this value as a 64 bit value
                }
#else
                  if ( local64Word != 0 )
                  {
                    local64Word = *(uint64 *)local64Word;
                    is64BitValue = 1; //flag this value as a 64 bit value
                  }
#endif
                else
                {
                    if ( parse_data->outputsz_size > strlen( "<NULL pointer & reference>" ) )
                  {
                      SNPRINTF_AND_MOVEPTRS1( "<NULL pointer & reference>" );
                    return;
                  } 
                  else
                  {
                    //no space to add an error message, just return.
                    return;
                  }
                }
                }
                else
                {
                  //read the 64 bit value from the buffer.
                UlogReadNextMsgPart((unsigned char*)&local64Word, parse_data, 8);
                is64BitValue=1; //flag this value as a 64 bit value
            } 
              }
            else 
            {
              // Not a 64 bit value. Do a normal 32 bit read
              is64BitValue=0; //make sure the 64 bit flag is not set.
                                  // Read in the pointer referred to us by the % format character.

              /* Check whether the 32 bit value is an address or data */ 
              isRef = strchr(subFmtString, '&');
              if (isRef != NULL)
              {
                unsigned int i;
                  /* The 32-bit value is an address. Dereference it to get the data */
#ifdef ULOG_64BIT_PROC

                  UlogReadNextMsgPart( (unsigned char *)&local64Word, parse_data, 8 );

                  if ( local64Word != 0 )
                  {
                    local64Word = *(uint64 *)local64Word;
                  }
#else
                  UlogReadNextMsgPart( (unsigned char *)&localWord, parse_data, 4 );
                  if ( localWord != 0 )
                {
                    localWord = *(uint32 *)localWord;
                  }
#endif
                  else
                  {
                  if (parse_data->outputsz_size > strlen("<NULL pointer & reference>"))
                  {
                      SNPRINTF_AND_MOVEPTRS1( "<NULL pointer & reference>" );
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
                  for ( i = 0; i < strlen( isRef ); i++ )
                  {
                  isRef[i] = isRef[i+1];
                  }
                }
                else
                {
                  UlogReadNextMsgPart( (unsigned char *)&localWord, parse_data, 4 );
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
#ifdef ULOG_64BIT_PROC
        if ( (*fmtPtr == 's') && (local64Word == 0) )
        {
          local64Word = (uint64)"<NULL REFERENCE>";
        }
#else
        if ((*fmtPtr == 's') && (localWord == 0))
        {
          localWord = (uint32)"<NULL REFERENCE>";
        }
#endif
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
            Add_InsufficientSpaceMessage(parse_data);
            return;           
          }
        }
        else
        {
          //unless the special 'f' or 's' handling above is used, this is where most of the format string printing is done.
#ifndef ULOG_64BIT_PROC
          if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the smallest string
          {
#else
          if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE_64 ) //ensure there's enough space to write the smallest string
          {
#endif
            //recheck that we really think this is a printable format string
            if (isNotConversionChar(*fmtPtr)==FALSE)
            {
              if (is64BitValue==0)
              {
                if(*fmtPtr=='s')
                {
#ifdef ULOG_64BIT_PROC
                  local64Word = (uint64)"<ERROR: String Logged as 32-bit param>";
                  if ( (parse_data->outputsz_size) > strlen( (char *)local64Word ) ) //ensure there's enough space to write the string
                  {
                    SNPRINTF_AND_MOVEPTRS2( subFmtString, local64Word );
                  }
                  else
                  {
                    Add_InsufficientSpaceMessage(parse_data);
                    return;           
                  }
                }
#else
                  if ( (parse_data->outputsz_size) > strlen( (char *)localWord ) ) //ensure there's enough space to write the string

{
  SNPRINTF_AND_MOVEPTRS2( subFmtString, localWord );
}
else
{
  Add_InsufficientSpaceMessage( parse_data );
  return;
}
}
#endif

                else
                {
                  if ((parse_data->outputsz_size) > SNPRINTF_MIN_SIZE) //ensure there's enough space to write the string
                  {
                    SNPRINTF_AND_MOVEPTRS2(subFmtString, localWord);
                  }
                  else
                  {
                    Add_InsufficientSpaceMessage(parse_data);
                    return;           
                  }
                }
              }
              else 
              {
                //it's a 64 bit value
                if ( *fmtPtr == 's' )
                {
#ifdef ULOG_64BIT_PROC
                  if ( (parse_data->outputsz_size) > strlen( (char *)local64Word ) ) //ensure there's enough space to write the string
#else
                  local64Word = (uint64)"<ERROR: String Logged as 64-bit param>";
                  if ( (parse_data->outputsz_size) > strlen( (char *)(UINTN)local64Word ) )
#endif
                  {
                    SNPRINTF_AND_MOVEPTRS2( subFmtString, local64Word );
                  }
                  else
                  {
                    Add_InsufficientSpaceMessage( parse_data );
                    return;
                  }
                }
                else
                {
                  if ( (parse_data->outputsz_size) > SNPRINTF_MIN_SIZE_64 ) //ensure there's enough space to write the string
                {
                  SNPRINTF_AND_MOVEPTRS2(subFmtString, local64Word);
                  }
                  else
                  {
                  Add_InsufficientSpaceMessage(parse_data);
                  return;           
                  }
                }
              }
            }
            else
            {
              if ( parse_data->outputsz_size > strlen( "<bad printf format>" ) )
              {
                SNPRINTF_AND_MOVEPTRS1("<bad printf format>");
              }
            }
          }
          else 
          {
            Add_InsufficientSpaceMessage(parse_data);
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
 * <!-- ULFE_RealTimeCharArrayParse -->
 *
 * @brief Do the RealTimeCharArray to string parse work.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 */
static void ULFE_RealTimeCharArrayParse( ULOG_READ_TO_ASCII_DATA* parse_data,
                                         ULOG_READ_TO_ASCII_DATA* mpm )
{
  uint32 columns = 0;
  unsigned char value;

  while(UlogReadNextMsgPart((unsigned char*)&value, parse_data, 1))
  {
    if (columns == 24)
    {
      // Prep the next row of data.
      if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
      {
        SNPRINTF_AND_MOVEPTRS1("\n            ");
      }
      else
      {
        Add_InsufficientSpaceMessage(parse_data);
        return;
      }
      columns = 0;
    }

    if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
    {
      SNPRINTF_AND_MOVEPTRS2("%02X ", value);
    }
    else
    {
      Add_InsufficientSpaceMessage(parse_data);
      return;
    }
    columns++;

    // Put an extra space inbetween groups of four bytes
    if ((columns & 0x03) == 0)
    {
      if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
      {
        SNPRINTF_AND_MOVEPTRS1(" ");
      }
      else 
      {
        Add_InsufficientSpaceMessage(parse_data);
        return;
      }
    }
  }

  return;
}


/**
 * <!-- ULFE_RealTimeStringParse -->
 *
 * @brief Do the RealTimeString to string parse work.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 */
static void ULFE_RealTimeStringParse( ULOG_READ_TO_ASCII_DATA* parse_data,
                                      ULOG_READ_TO_ASCII_DATA* mpm )
{
  char value;

  while(UlogReadNextMsgPart((unsigned char*)&value, parse_data, 1))
  {
    if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
    {
      SNPRINTF_AND_MOVEPTRS2("%c", value);
    }
    else
    {
      Add_InsufficientSpaceMessage(parse_data);
      return;
    }
  }

  return;
}


/**
 * <!-- ULFE_RealTimeWordArrayParse -->
 *
 * @brief Do the RealTimeWordArray to string parse work.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param msgFormat : ULOG_REALTIME_SUBTYPE_WORDDATA or ULOG_REALTIME_SUBTYPE_CSVDATA
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 */
static void ULFE_RealTimeWordArrayParse( ULOG_READ_TO_ASCII_DATA* parse_data,
                                         uint32 msgFormat,
                                         ULOG_READ_TO_ASCII_DATA* mpm )
{
  uint32 columns = 0;
  int wrapData = 0;
  unsigned int value;

  while(UlogMsgRemainingBytes(parse_data) &&
        (parse_data->outputsz_size > 11))
  {
    if (msgFormat == ULOG_REALTIME_SUBTYPE_WORDDATA)
    {
      // WordArray
      wrapData = 1;
      UlogReadNextMsgPart((unsigned char*)&value, parse_data, 4);
      if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
      {
        SNPRINTF_AND_MOVEPTRS2("0x%08X ", value);
      }
      else
      {
        Add_InsufficientSpaceMessage(parse_data);
        return;
      }
    }
    else
    {
      // CSV
      UlogReadNextMsgPart((unsigned char*)&value, parse_data, 4);
      if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
      {
        SNPRINTF_AND_MOVEPTRS2("0x%08X ", value);
      }
      else
      {
        Add_InsufficientSpaceMessage(parse_data);
        return;
      }
    }
    columns++;
    if ((columns == 8) && (UlogMsgRemainingBytes(parse_data)) && (wrapData))
    {
      // Prep the next row of data.
      if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
      {
        SNPRINTF_AND_MOVEPTRS1("\n            ");
      }
      else
      {
        Add_InsufficientSpaceMessage(parse_data);
        return; 
      }
      columns = 0;
    }
  }

  return;
}


/**
 * <!-- ULFE_RealTimeVectorParse -->
 *
 * @brief Do the RealTimeVector to string parse work.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 */
static void ULFE_RealTimeVectorParse( ULOG_READ_TO_ASCII_DATA* parse_data,
                                      ULOG_READ_TO_ASCII_DATA* mpm )
{
  char  localChar;
  uint16 localShort;
  uint32 localWord;
#ifdef ULOG_64BIT_PROC
  uint64 local64Word;
#endif
  const char * fmtString;
  uint16 entryByteCount;
  //uint16 vectorLength;

  if (UlogMsgRemainingBytes(parse_data) > 8)  //must have space for a fmtstring and size info
  {
#ifdef ULOG_64BIT_PROC
    UlogReadNextMsgPart( (unsigned char *)&local64Word, parse_data, 8 );
    fmtString = (const char *)local64Word;
#else
    UlogReadNextMsgPart((unsigned char*)&localWord, parse_data, 4);
    fmtString = (const char *)localWord;
#endif

    UlogReadNextMsgPart((unsigned char*)&localWord, parse_data, 4);
    entryByteCount = localWord >> 16;
    //vectorLength = localWord & 0x0000FFFF;

    while((UlogMsgRemainingBytes(parse_data)) &&
          (parse_data->outputsz_size))
    {
      switch (entryByteCount)
      {
        case 1:
          UlogReadNextMsgPart((unsigned char*)&localChar, parse_data, entryByteCount);
          localWord = localChar;
          break;
        case 2:
          UlogReadNextMsgPart((unsigned char*)&localShort, parse_data, entryByteCount);
          localWord = localShort;
          break;
        case 4:
          UlogReadNextMsgPart((unsigned char*)&localWord, parse_data, entryByteCount);
          break;

        default:
          // This is an error.  Exit.
          break;
      }
      //ensure there's enough space to write the string	the string + NULL + the number
      if ( parse_data->outputsz_size > strlen( fmtString ) + 1 + SNPRINTF_MIN_SIZE )
      {
        SNPRINTF_AND_MOVEPTRS2(fmtString, localWord);
      }
      else
      {
        Add_InsufficientSpaceMessage(parse_data);
        return;
      }
    }
  }

  return;
}


/**
 * <!-- ULFE_RawParse -->
 *
 * @brief Do the raw message to string parse work.  
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 * @param datawidth : 1, 2, or 4 byte sized data.
 */
static void ULFE_RawParse( ULOG_READ_TO_ASCII_DATA* parse_data,
                           ULOG_READ_TO_ASCII_DATA* mpm,
                           unsigned char datawidth )
{
  unsigned char  localChar;
  uint16 localShort;
  uint32 localWord;

  if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
  {
    SNPRINTF_AND_MOVEPTRS1("- ");
  }
  else
  {
    Add_InsufficientSpaceMessage(parse_data);
    return;
  }

  while((UlogMsgRemainingBytes(parse_data)) &&
          (parse_data->outputsz_size))
  {
    switch (datawidth)
    {
      case 1:
        UlogReadNextMsgPart((unsigned char*)&localChar, parse_data, datawidth);
        //localWord = localChar;
        if ( parse_data->outputsz_size > 5 ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("%02X, ", localChar);
        }
        else
        {
          Add_InsufficientSpaceMessage(parse_data);
          return;
        }
        break;
      case 2:
        UlogReadNextMsgPart((unsigned char*)&localShort, parse_data, datawidth);
        //localWord = localShort;
        if ( parse_data->outputsz_size > 7 ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("%04X, ", localShort);
        }
        else
        {
          Add_InsufficientSpaceMessage(parse_data);
          return;
        }
        break;
      case 4:
        UlogReadNextMsgPart((unsigned char*)&localWord, parse_data, datawidth);
        if ( parse_data->outputsz_size > 12 ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("%8X, ", (unsigned int)localWord);
        }
        else
        {
          Add_InsufficientSpaceMessage(parse_data);
          return;
        }
        break;

      default:
        // This is an error.  Exit.
        break;
    }
  }
  return;
}


/**
 * <!-- ULogParsers_FormatParseData -->
 * 
 * @brief Add the timestamp to the string and then pass the data to specific parsers.
 *
 * If mpm is not NULL it means we're in a MPM and the timestamp should not be added.
 *
 * @param parse_data : Pointer to a ULOG_READ_TO_ASCII_DATA struct of the msg data
 *                     and character string being working on.
 * @param mpm : While parsing a MPM this additional data is passed around to 
 *              hold MPM data.
 */
ULogResult ULogParsers_FormatParseData( ULOG_READ_TO_ASCII_DATA* parse_data,
                                        ULOG_READ_TO_ASCII_DATA* mpm )
{
  // Parse the real-time log and print the results to the output buffer.
  // Input message fomat to this function:
  // <Timestamp> <Format> <Data...>
  uint32 msgFormat;
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#else
  uint32 timestamp;
#endif
      
  // Check the size looks reasonable and read the format
  if ( UlogMsgRemainingBytes( parse_data ) < MIN_RAW_MSG_LEN ) //ensure that the message is at least big enough for a FORMAT field
  {
    return(DAL_ERROR);
  }

  // Read the format of this message
  UlogReadNextMsgPart((unsigned char*)&msgFormat, parse_data, 4);

  //clear off the length field.
  msgFormat = msgFormat & ULOG_FORMAT_MASK;

  // If it's a realtime message, read the timestamp.
  if ( (msgFormat > 0) && (msgFormat < ULOG_SUBTYPE_RESERVED1) )
  {
    // The timestamp should be the next word in the parse_data
    UlogReadNextMsgPart((unsigned char*)&timestamp, parse_data, ULOG_TIMESTAMP_SIZE);

    // Add the timestamp to the output string if we're not in a MPM
    if (mpm == NULL)
    {
      if (msgFormat == ULOG_REALTIME_SUBTYPE_CSVDATA)
      {
#ifdef ULOG_64BIT_TIME
        if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE * 2 ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("%llu: ", (uint64)timestamp);
#else
        if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("%u: ", (unsigned int)timestamp);
#endif
        }
        else
        {
          Add_InsufficientSpaceMessage(parse_data);
          return(DAL_ERROR);
        }
      }
      else
      {
#ifdef ULOG_64BIT_TIME
        if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE * 2 ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("0x%016llX: ", (uint64)timestamp);
#else
        if ( parse_data->outputsz_size > SNPRINTF_MIN_SIZE ) //ensure there's enough space to write the string
        {
          SNPRINTF_AND_MOVEPTRS2("0x%08X: ", (unsigned int)timestamp);
#endif
        }
        else
        {
          Add_InsufficientSpaceMessage(parse_data);
          return(DAL_ERROR);
        }
      }
    } //end if(mpm == NULL)
  } //end of code to add TS


  switch (msgFormat)
  {
    case ULOG_REALTIME_SUBTYPE_WORDDATA:    // An array of uint32 data
    case ULOG_REALTIME_SUBTYPE_CSVDATA:     // An array of word data output as CSV
      ULFE_RealTimeWordArrayParse(parse_data, msgFormat, mpm);
      break;

    case ULOG_REALTIME_SUBTYPE_PRINTF:      // A printf style message
      ULFE_RealTimePrintfParse(parse_data, mpm);
      break;

    case ULOG_REALTIME_SUBTYPE_BYTEDATA:    // An array of byte data
      ULFE_RealTimeCharArrayParse(parse_data, mpm);
      break;

    case ULOG_REALTIME_SUBTYPE_STRINGDATA:  // An array of char data
      ULFE_RealTimeStringParse(parse_data, mpm);
      break;

    case ULOG_REALTIME_SUBTYPE_VECTOR:      // An vector of data
      ULFE_RealTimeVectorParse(parse_data, mpm);
      break;

    case ULOG_REALTIME_SUBTYPE_MULTIPART:   // A multipart message
      HandleNextMultipartMsg(parse_data, NULL);  
      break;

    case ULOG_SUBTYPE_RAW8:
    case ULOG_REALTIME_SUBTYPE_RESERVED_FOR_RAW:   // An Raw 1 byte format message
      ULFE_RawParse(parse_data, mpm, 1);
      break;

    case ULOG_SUBTYPE_RAW16:
      ULFE_RawParse(parse_data, mpm, 2); // An Raw 2 byte format message
      break;

    case ULOG_SUBTYPE_RAW32:
      ULFE_RawParse(parse_data, mpm, 4); // An Raw 4 byte format message
      break;

    default:
      break;
  }

  if (mpm == NULL)
  {
    // Add the linefeed to the end of the message if there's space
    if ( parse_data->outputsz_size > strlen( "\n" ) ) //ensure there's enough space to write the string
    {
      ADDCHAR_AND_MOVEPTRS('\n')
    } 
  }

  return(DAL_SUCCESS);
}


/**
 * <!-- ULogParsers_FormatMsg -->
 * 
 * @brief Prepare the message into our convenient parse_data structure and call ULogParsers_FormatParseData. 
 *
 * @param log : Pointer to the log the data came from
 * @param message : The binary log data to parse/convert to ascii. 
 * @param outputsz : Pointer to memory to place the ascii decoded log. 
 * @param outputSizeBytes : How much memory is available at outputsz. 
 * @param wordsConsumed : How many bytes of the log message were decoded. 
 */
ULogResult ULogParsers_FormatMsg( ULOG_TYPE * log,
                                  char *  message,
                                  char *  outputsz,
                                  uint32 outputSizeBytes,
                                  uint32 * wordsConsumed) 
{
  uint32 msgLength;
  ULOG_READ_TO_ASCII_DATA parse_data;

  // Format the message as a string, and put it in the outputsz buffer.
  // Return the number of words used.
  if ((outputSizeBytes == 0) ||
      (outputsz == NULL) ||
      (message == NULL))
  {
    return(DAL_ERROR);
  }

  //NULL terminate the string
  outputsz[0] = 0;

  // Read the length of this message
  core_memcpy( &msgLength, 4, &message[0], 4 );
  msgLength = (msgLength & ULOG_LENGTH_MASK) >> ULOG_LENGTH_SHIFT;

  //set the amount of message words we'll be handling
  *wordsConsumed = ULOGINTERNAL_PADCOUNT(msgLength);

  if ((msgLength < MIN_RAW_MSG_LEN) || (msgLength > ULOG_MAX_MSG_SIZE))
  {
    // If it's too small or too large, we have an invalid formatted message.
    // Throw it away.
    *outputsz = 0;
    return(DAL_ERROR);
  }

  parse_data.msgpart1 = message;
  parse_data.part1_size = msgLength;
  parse_data.msgpart2=NULL;
  parse_data.part2_size = 0;
  parse_data.outputsz = outputsz;
  parse_data.outputsz_size = outputSizeBytes;
  parse_data.outOffset = 0;
  parse_data.environment = ULOG_PARSE_READ_DATA;  //decoding a stored log, not a data stream.
  ULogParsers_FormatParseData(&parse_data, NULL);

  return(DAL_SUCCESS);
}

#endif // DEFEATURE_ULOG_MSG_FORMAT

#ifdef __cplusplus
}
#endif
