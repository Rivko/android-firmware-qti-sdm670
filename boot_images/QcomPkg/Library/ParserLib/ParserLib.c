/** @file
  Implements the Parser to extract tokens from Config File.
   
  Copyright (c) 2012,2015 Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/05/15   va      Secton support for csv by section file
 09/23/14   yg      Enum csv by section
 03/18/14   vk      Fix gcc warning
 11/01/13   vk      Fix 64 bit compilation
 01/17/13   vk      Fix warning
 11/02/12   yg      Allow all chars in strings
 07/09/12   yg      Make Parser generic and add more capability
 04/24/12   yg      Increased buffer size
 03/15/12   kedara  Initial revision

=============================================================================*/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ParserLib.h>

#define CARRAIGE_RETURN       10
#define LF                    13
#define COMMENT_CHAR          '#'
#define SPACE                 ' '
#define TAB                   '\t'
#define COLON_DELIMITER       ':'
#define COMMA_DELIMITER       ','
#define KEY_VALUE_DELIMITER   '='
#define MAX_CFG_TOKENS        16
#define MAX_LINE_BUFFER_SIZE  256
#define QUOTE_CHAR            '"'
#define SECTION_START_CHAR    '['
#define SECTION_END_CHAR      ']'

typedef struct {
   UINT8*                    OrigBuffer;
   UINT8*                    Buffer;
   UINT8*                    BufferEnd;
   UINT8*                    SectionBuffer;
   UINT8*                    TempParseBuffer;
   UINT8*                    TokenCfgBuffer;
   UINT8**                   TokenPtrs;
   UINT8                     InUse;
   UINT8                     EnumInProgress;
   TokenCallBackFuncType     TokenCbFn;
   KeyValueCallBackFuncType  KeyValCbFn;

   UINT8*                    MemToFree;
   ParseOptionsType          ParseOptions;
} ParserContext;

#define MAX_PARSER_INSTANCES   4
STATIC ParserContext ParserInstances[MAX_PARSER_INSTANCES];

/*   Delimiters:
 *
 *   COMMENT_CHAR    : comment until newline
 *   CARRAIGE_RETURN LF SPACE TAB : space all over the file
 *
 *   COLON_DELIMITER,
 *   COMMA_DELIMITER, 
 *   KEY_VALUE_DELIMITER : Delimiters based on requirement
 */
/**
  Skip spaces and empty lines 

  @param  FileBuffer   Buffer to be parsed.

  @retval EFI_SUCCESS    Initialization success

**/
STATIC UINT8*
SkipSpaceLines (ParserContext*  Pc)
{
  UINT8* buffer = Pc->Buffer;

  while (buffer && *buffer && (buffer < Pc->BufferEnd))
  {
    /* Skip over all the space and new lines */
    if ((*buffer == SPACE) || (*buffer == TAB) ||
        (*buffer == CARRAIGE_RETURN) || (*buffer == LF)) 
    {
      ++buffer;
      continue;
    }
    break;
  }

  Pc->Buffer = buffer;
  if ((buffer == NULL) || (buffer >= Pc->BufferEnd) || (*buffer == 0))
  {
    Pc->Buffer = Pc->BufferEnd;
    return NULL;
  }

  return buffer;
}

/**
Skip over comment line until newline, return next line after space trimming 

  @param   buffer   Buffer to be parsed.

  @retval  UINT8*  updated buffer pointer

**/
STATIC UINT8*
GetNextLine (ParserContext*  Pc)
{
  UINT8* buffer = Pc->Buffer;

  while (buffer && *buffer && (buffer < Pc->BufferEnd))
  {
    if ((*buffer == CARRAIGE_RETURN) || (*buffer == LF))
    {
      Pc->Buffer = buffer;
      buffer = SkipSpaceLines (Pc);
      break;
    }
    ++buffer;
  }

  Pc->Buffer = buffer;
  if ((buffer == NULL) || (buffer >= Pc->BufferEnd) || (*buffer == 0))
  {
    Pc->Buffer = Pc->BufferEnd;
    return NULL;
  }

  return buffer;
}

/**
Get first config line after space, comment trimming 

  @param   buffer   Buffer to be parsed.

  @retval  UINT8*  updated buffer pointer

**/
STATIC UINT8*
GetFirstConfig (ParserContext*  Pc)
{
  UINT8* buffer;

  /* Skip over all the space and new lines */
  buffer = SkipSpaceLines (Pc);

  /* Skip over all comments and embedded space between */
  while (buffer && (buffer < Pc->BufferEnd) && (*buffer == Pc->ParseOptions.CommentChar))
  {
    buffer = GetNextLine (Pc);
  }

  if ((buffer == NULL) || (buffer >= Pc->BufferEnd) || (*buffer == 0))
    return NULL;

  return buffer;
}

/**
Skip over current config line and get next valid config line

  @param   buffer   Buffer to be parsed.

  @retval  UINT8*  updated buffer pointer

**/
STATIC UINT8*
GetNextConfig (ParserContext*  Pc)
{
  if (GetNextLine (Pc) == NULL)
    return NULL;

  return GetFirstConfig (Pc);
}

/* Copy the config line into the destination memory */
STATIC UINT8*
CopyLineBuffer (UINT8              *dst_buffer,
                UINT8              *src_buffer,
                UINT32              buffer_size)
  
{
  UINT8 *temp_ptr = dst_buffer;

  if (src_buffer == NULL || dst_buffer == NULL || buffer_size == 0)
    return 0;

  /* Copy full line into the destination buffer */
  while (buffer_size && *src_buffer)
  {
    *temp_ptr = *src_buffer;
    ++temp_ptr;
    ++src_buffer;
    --buffer_size;

    if (*src_buffer == LF || *src_buffer == CARRAIGE_RETURN)
      break;
  }

  /* If we consumed all the memory, erase a char */
  if (buffer_size == 0)
    --temp_ptr;

  /* NUL termination */
  *temp_ptr = 0;

  return dst_buffer;
}

 /**
 *  Get list of tokens seperating them into the destination buffer
 *  Source Buffer should be a null terminated string
 *  All tokens  will be null terminated strings
 *  an array of pointers to token string is passed.
 *  If dst buffer size is smaller then value gets terminated

  @param  buffer    buffer containing data to be parsed
          token_ptr_array[]  Array of token pointers
          max_tokens        maximum number of tokens expected 
          dst_buffer        buffer to hold seperated tokens
          dst_buffer_size   size of buffer

  @retval  INT32  returns -1 on error
                  returns number of tokens parsed

**/
STATIC INTN
GetTokensFromLine ( ParserContext*     Pc,
                    UINT8*             buffer,
                    UINT8*             token_ptr_array[],
                    UINT32             max_tokens, 
                    UINT8*             dst_buffer,
                    UINT32             dst_buffer_size)
{
  UINT32 token_index = 0;
  BOOLEAN InString = FALSE;
  
  if (buffer == 0 || dst_buffer == 0 || dst_buffer_size == 0 || token_ptr_array == 0)
    return 0;

  /* Point 1st token to start of dst buffer */
  token_ptr_array[token_index] = dst_buffer;
  token_index++;

  /* Copy tokens  */
  while (*buffer)
  {
    if (*buffer == QUOTE_CHAR)
    {
      // Toggle InString flag. User *has* to ensure correct match of Quotes
      InString = !InString;
    }
    else
    {
      /* Allow all chars in strings except Quotes */
      if (InString == TRUE)
      {
        *dst_buffer = *buffer;
        ++dst_buffer;
        --dst_buffer_size;
      }
      else if ((*buffer == COLON_DELIMITER) || (*buffer == COMMA_DELIMITER))
      {
        /* Null terminate current token string */
        *dst_buffer = 0;
        /* assign next token pointer to next token  start after sanity checks */
        /* ensure token allowed */
        if ((token_index < max_tokens) && (dst_buffer_size >= 1))
          token_ptr_array[token_index++] = (dst_buffer+1);

        ++dst_buffer;
        --dst_buffer_size;
      }
      /* Copy char to dst if it is not Space */
      else if ((*buffer != SPACE) && (*buffer != TAB))
      {
        *dst_buffer = *buffer;
        ++dst_buffer;
        --dst_buffer_size;
      }
    }

    ++buffer;
    if (dst_buffer_size == 0)
      break;
  }// end while

  if(dst_buffer_size)
    *dst_buffer = 0;
  else
    *(dst_buffer - 1) = 0;

  return token_index;
}

STATIC VOID
InitializeDataBuffers (ParserContext* Pc)
{
  SetMem (Pc->TokenPtrs, sizeof(UINT8*) * MAX_CFG_TOKENS, 0);
  SetMem (Pc->TempParseBuffer, MAX_LINE_BUFFER_SIZE, 0);
  SetMem (Pc->TokenCfgBuffer, MAX_LINE_BUFFER_SIZE, 0);
}

STATIC INTN
AllocateContext(void)
{
  UINTN i;

  for (i = 0; i < (sizeof(ParserInstances)/sizeof(*ParserInstances)); ++i)
  {
    if (ParserInstances[i].InUse == 0)
    {
      ParserInstances[i].InUse = 1;
      return i;
    }
  }

  return -1;
}

STATIC INTN
FreeContext (INTN Pd)
{
  ParserContext* Pc;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES))
    return -1;

  Pc = &ParserInstances[Pd];

  SetMem (Pc, sizeof(*Pc), 0);
  return 0;
}

STATIC UINT8*
CopySectionString (OUT UINT8*   Dst,
                   IN  UINT8*   Src,
                   IN  UINTN    MaxSize,
                   OUT UINTN*   SectionSizep)
{
  UINT8* DstBak = Dst;

  if ((Dst == NULL) || (Src == NULL) || (MaxSize == 0))
    return NULL;

  while (MaxSize && Src && (*Src != 0) && (*Src != SECTION_END_CHAR))
  {
    *Dst = *Src;
    ++Dst;
    ++Src;
    --MaxSize;
  }

  /* If we ran out of buffer size, make room for nul terminator */
  if (MaxSize == 0)
    --Dst;

  if (SectionSizep)
    *SectionSizep = (UINTN)(Dst - DstBak);

  /* Nul terminate */
  *Dst = 0;
  return DstBak;
}

INTN
ReopenParser (IN  INTN            Pd)
{
  ParserContext* Pc;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES))
    return -1;

  Pc = &ParserInstances[Pd];

  Pc->Buffer = Pc->OrigBuffer;

  return 0;
}

INTN
OpenParser (IN  UINT8*             Buffer,
            IN  UINT32             BufferSize,
            IN  ParseOptionsType*  Popt OPTIONAL)
{
  UINT8 *EndPtr = NULL, *Ptr = NULL;
  INTN Pd = 0;
  ParserContext* Pc = NULL;

  if ((Buffer == NULL) || (BufferSize == 0))
    return -1;

  EndPtr = Buffer + BufferSize;
  if ((UINTN)EndPtr < (UINTN)Buffer)
    return -1;

  Pd = AllocateContext();
  if (Pd < 0)
    return -1;

  Pc = &ParserInstances[Pd];

  Pc->Buffer = Buffer;
  Pc->OrigBuffer = Buffer;
  Pc->BufferEnd = EndPtr;
  Ptr = AllocateZeroPool ((MAX_LINE_BUFFER_SIZE * 3) + 
                (MAX_CFG_TOKENS * sizeof(UINT8*)));
  if (Ptr == NULL)
  {
    FreeContext (Pd);
    return -1;
  }

  Pc->MemToFree = Ptr;
  Pc->SectionBuffer   = Ptr;  Ptr += MAX_LINE_BUFFER_SIZE;
  Pc->TempParseBuffer = Ptr;  Ptr += MAX_LINE_BUFFER_SIZE;
  Pc->TokenCfgBuffer  = Ptr;  Ptr += MAX_LINE_BUFFER_SIZE;
  Pc->TokenPtrs       = (UINT8**)Ptr;

  Pc->TokenCbFn = NULL;
  Pc->KeyValCbFn = NULL;

  if (Popt != NULL)
    CopyMem (&Pc->ParseOptions, Popt, sizeof(*Popt));
  else
  {
    SetMem (&Pc->ParseOptions, sizeof(*Popt), 0);

    /* Set default options */
    Pc->ParseOptions.CommentChar = COMMENT_CHAR;
    Pc->ParseOptions.Options = SUPPRESS_BYTEORDERMARK |
                               ENABLE_COMMA_DELIMITER |
                               ENABLE_SEMICOLON_DELIMITER;
  }

  if (Pc->ParseOptions.Options & SUPPRESS_BYTEORDERMARK)
  {
    /* Suppress byte order mark if we have one at the beginning */
    if ((Buffer[0] == 0xEF) && (Buffer[1] == 0xBB) && (Buffer[2] == 0xBF))
    {
      if (EndPtr > &Buffer[3])
        Pc->Buffer += 3;
      else
        Pc->Buffer = Pc->BufferEnd;
    }
  }

  return Pd;
}

INTN
EnumCommaDelimSectionItems (IN  INTN                         Pd,
                            IN  UINT8*                       Section   OPTIONAL,
                            IN  TokenSectionCallBackFuncType CallBackFunc)
{
  ParserContext* Pc;
  UINT8* Buffer;
  UINT8  SectionMatches = TRUE;
  UINTN  SectionLen, SectionOffset = 0;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES))
    return -1;

  Pc = &ParserInstances[Pd];

  /* Goto First config item */
  Buffer = GetFirstConfig (Pc);
  while (Buffer && *Buffer)
  {
    InitializeDataBuffers(Pc);
    CopyLineBuffer (Pc->TokenCfgBuffer, Pc->Buffer, MAX_LINE_BUFFER_SIZE);

    if (Pc->TokenCfgBuffer[0] == SECTION_START_CHAR)
    {
      ++SectionOffset;
      CopySectionString (Pc->SectionBuffer, Pc->TokenCfgBuffer + 1, MAX_LINE_BUFFER_SIZE, &SectionLen);
      if (Section == NULL)
      {
        SectionMatches = TRUE;
      }
      else
      {
        if (AsciiStriCmp ((CHAR8*)Pc->SectionBuffer, (CHAR8*)Section) == 0)
          SectionMatches = TRUE;
        else
          SectionMatches = FALSE;
      }
      Buffer = GetNextConfig (Pc);
      continue;
    }

    if (SectionMatches)
    {
      INTN TokenCnt;

      TokenCnt = GetTokensFromLine (Pc, Pc->TokenCfgBuffer, Pc->TokenPtrs, MAX_CFG_TOKENS,
                                    Pc->TempParseBuffer, MAX_LINE_BUFFER_SIZE);

      if (TokenCnt > 0 && CallBackFunc)
        CallBackFunc (Pc->SectionBuffer, SectionOffset, Pc->TokenPtrs, TokenCnt);
    }

    Buffer = GetNextConfig (Pc);
  }
  return 0;
}

INTN
EnumCommaDelimItems (IN  INTN                   Pd,
                     IN  TokenCallBackFuncType  CallBackFunc)
{
  ParserContext* Pc;
  UINT8* Buffer;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES))
    return -1;

  Pc = &ParserInstances[Pd];

  /* Goto First config item */
  Buffer = GetFirstConfig (Pc);
  while (Buffer && *Buffer)
  {
    INTN TokenCnt;

    InitializeDataBuffers(Pc);
    CopyLineBuffer (Pc->TokenCfgBuffer, Pc->Buffer, MAX_LINE_BUFFER_SIZE);
    TokenCnt = GetTokensFromLine (Pc, Pc->TokenCfgBuffer, Pc->TokenPtrs, MAX_CFG_TOKENS,
                                  Pc->TempParseBuffer, MAX_LINE_BUFFER_SIZE);

    if (TokenCnt > 0 && CallBackFunc)
      CallBackFunc (Pc->TokenPtrs, TokenCnt);

    Buffer = GetNextConfig (Pc);
  }
  return 0;
}

/* NOTE: this function modifies the source buffer to set nul terminator */
STATIC UINT8*
SkipTrimSpaces (IN OUT UINT8*   Buff,
                IN     UINTN    Size)
{
  UINT8* OutPtr;
  UINTN  TrimSpace = TRUE; // This can be set to false if Quote char is there

  if ((Buff == NULL) || (Size == 0))
    return NULL;

  /* Skip leading spaces */
  while (Buff && Size && *Buff)
  {
    if ((*Buff == SPACE) || (*Buff == TAB))
    {
      ++Buff;
      --Size;
    }
    else
      break;
  }

  OutPtr = Buff;

  /* Return if we are done */
  if ((OutPtr == NULL) || (*OutPtr == 0) || (Size == 0))
    return OutPtr;

  /* If Embedded spaces are needed then the string should start with 
   * quote immediately after leading spaces */
  if (*Buff == QUOTE_CHAR)
  {
    ++Buff;
    --Size;
    OutPtr = Buff;
    TrimSpace = FALSE;
  }

  /* Now consider the string until either quote char or first space char */
  while (Buff && Size && *Buff)
  {
    if (*Buff == QUOTE_CHAR)
    {
      /* We are done */
      *Buff = 0;
      break;
    }

    /* or else first space will terminate it */
    if (((*Buff == SPACE) || (*Buff == TAB)) && (TrimSpace == TRUE))
    {
      *Buff = 0;
      break;
    }

    ++Buff;
    --Size;
  }
  return OutPtr;
}

/*
 * Split Key and Value delimited by = sign
 * */
STATIC VOID
SplitKeyVal (IN  UINT8*    Buff,
             IN  UINTN     Size,
             OUT UINT8**   KeyPtr,
             OUT UINTN*    KeyLen,
             OUT UINT8**   ValPtr,
             OUT UINTN*    ValLen)
{
  UINTN sz;
  /* This routine assumes that the buffer is already nul terminated */

  if ((Buff == NULL) || (Size == 0) || (KeyPtr == NULL) || (ValPtr == NULL))
    return;

  *KeyPtr = NULL;
  *ValPtr = NULL;
  *KeyLen = 0;
  *ValLen = 0;

  /* Check if No key just value, ie line starts with = */
  if (*Buff == KEY_VALUE_DELIMITER)
  {
    *ValPtr = Buff + 1;
    *ValLen = Size - 1;
    return ;
  }

  *KeyPtr = Buff;

  sz = 0;
  while (Buff && Size && *Buff)
  {
    if (*Buff == KEY_VALUE_DELIMITER)
    {
      /* This essentially nul terminates key */
      *Buff = 0;
      *KeyLen = sz;

      /* Holds good even if next char is nul */
      *ValPtr = Buff + 1;
      *ValLen = Size - 1;
      return;
    }
    ++Buff;
    --Size;
    ++sz;
  }
  
  /* If we never saw the delimiter we should set the len atleast */
  *KeyLen = sz;
}

INTN
GetNextKeyValue (IN  INTN     Pd,
                 IN  UINT8**  Sectionp,
                 IN  UINT8**  Keyp,
                 IN  UINT8**  Valp)
{
  ParserContext* Pc;
  UINT8 *Buffer, *Key, *Val;
  UINTN SectionLen, KeyLen, ValLen;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES))
    return -1;

  Pc = &ParserInstances[Pd];

  if (Pc->EnumInProgress)
    Buffer = GetNextConfig (Pc);
  else
  {
    Pc->EnumInProgress = TRUE;
    Pc->SectionBuffer[0] = 0;
    Buffer = GetFirstConfig (Pc);
  }

  while (Buffer && *Buffer)
  {
    CopyLineBuffer (Pc->TempParseBuffer, Pc->Buffer, MAX_LINE_BUFFER_SIZE);
    if (Pc->TempParseBuffer[0] == SECTION_START_CHAR)
    {
      CopySectionString (Pc->SectionBuffer, Pc->TempParseBuffer + 1, MAX_LINE_BUFFER_SIZE, &SectionLen);

      /* ------ Perform any section based processing here ---- */

      /* Skip to next item */
      Buffer = GetNextConfig (Pc);
      continue;
    }

    /* see if key value pairs exist, if yes then split and send */
    SplitKeyVal (Pc->TempParseBuffer, MAX_LINE_BUFFER_SIZE, &Key, &KeyLen, &Val, &ValLen);

    /* Now that we have private copy of Key, Value, trim spaces as needed */
    Key = SkipTrimSpaces (Key, KeyLen);
    Val = SkipTrimSpaces (Val, ValLen);

    *Keyp = Key;
    *Valp = Val;
    *Sectionp = Pc->SectionBuffer;

    return 0;
  }

  return -1;
}

INTN EnumKeyValues  (IN  INTN                      Pd,
                     IN  UINT8*                    SectionReq   OPTIONAL,
                     IN  KeyValueCallBackFuncType  CallBackFunc)
{
  UINT8 *Section = NULL, *Key = NULL, *Val = NULL;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES) || (CallBackFunc == NULL))
    return -1;

  while (GetNextKeyValue (Pd, &Section, &Key, &Val) == 0)
  {
    /* If there is no section filter, or the filter matches then callback */
    if ((SectionReq == NULL) ||
        (AsciiStriCmp ((CHAR8*) SectionReq, (CHAR8*) Section) == 0))
    {
      CallBackFunc (Section, Key, Val);
    }
  }

  return 0;
}

INTN
CloseParser (IN  INTN            Pd)
{
  ParserContext* Pc;

  if ((Pd < 0) || (Pd >= MAX_PARSER_INSTANCES))
    return -1;

  Pc = &ParserInstances[Pd];

  FreePool (Pc->MemToFree);

  FreeContext (Pd);

  return 0;
}

