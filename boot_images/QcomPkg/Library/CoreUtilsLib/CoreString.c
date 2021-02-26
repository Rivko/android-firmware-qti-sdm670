/******************************************************************//**
 * @file CoreString.c
 *
 * @brief Srting function for UEFI targets
 *
 * Copyright (C) 2013-2014 by Qualcomm Technologies Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
11/25/14   rli     Replace banned DALSYS_memcpy() with safe core_memcpy() 
02/10/14   rli     '%p' format specifier support
03/25/14   rli     AARCH64 compiler warning fixes
12/11/13   rli     compiler warning fixes
04/30/13   rli     core_snprintf implementation
04/18/13   rli     Initial revision
========================================================================*/


#include "CoreString.h"
#include "DALSys.h"
#include "stdarg.h"
#ifdef TARGET_UEFI

/** 
* Helpers for core_vsnprintf()
*/
#define isdigit(c) ((c>='0') && (c<='9'))
#define char_to_int(c) (c -'0')

/** Parser modes */
typedef enum {
    MODE_0,
    MODE_1,
    MODE_2,
    MODE_3,
} ParserModeEnum;


/*===========================================================================
FUNCTION      num_to_char

DESCRIPTION    Helper that converts integer from 0-15 into hex character.
              
===========================================================================*/
static char num_to_char(unsigned int value, char uppercase)
{
    char ret = '*';
    char lowercase_nums[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    char uppercase_nums[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char *pnums = (uppercase)? uppercase_nums : lowercase_nums;

    if(value < sizeof(lowercase_nums)) ret = pnums[value];
    return ret;
}

/*===========================================================================
FUNCTION      addintvalue

DESCRIPTION    Helper that converts integer(value) to string and adds it to buffer (str).
              
===========================================================================*/
static void addintvalue(char *str, unsigned int count, unsigned int *cur_pos,  unsigned int radix, char prefix0, char uppercase, unsigned int digits, unsigned int size, long long value)
{
 unsigned int width, i, msign=0;
 unsigned long long tmp, uvalue, div=1;


    if (value < 0) 
    {
            uvalue = -value;
            if(radix == 10) msign = 1;
            else
            {
                unsigned long long mask = 0xFFFFFFFFFFFFFFFF;
                uvalue = ~(-value)+1;
                mask = mask >> ((sizeof(mask) - size)*8);
                uvalue &= mask;
            }
    }
    else uvalue = value;

    for(tmp = uvalue/radix, width = 1; tmp > 0; tmp /= radix, width++) div *= radix;
    tmp = uvalue;
    for(i=0; ( *cur_pos < count); (*cur_pos)++, i++)
    {
        if (!i && msign)
        {
            str[*cur_pos] = '-';
        }
        else
        if ( (width + i) < digits )
        {
            str[*cur_pos] =  prefix0 ? '0':' ';
        }
        else 
        {
            unsigned int k = (unsigned int)(tmp/div);
            str[*cur_pos] = num_to_char(k, uppercase);
            tmp -= k*div;
            div /= radix;
            if ( div == 0 )
            {
                break;
            }
        }
    }
}

/*===========================================================================
FUNCTION      addstrvalue

DESCRIPTION    Helper that adds string (svalue) to sized buffer (str).
              
===========================================================================*/
static void addstrvalue(char *str, unsigned int count, unsigned int *cur_pos,  char *svalue)
{
 char *s;
    for( s = svalue; ( *cur_pos < count) && (*s != '\0');  s++)
    {
            str[*cur_pos] = *s;
            (*cur_pos)++;
    }
   (*cur_pos)--;
}

/*===========================================================================
FUNCTION      core_vsnprintf

DESCRIPTION    Writes formatted output to sized buffer.
              
PARAMETERS    str - pointer to a destination buffer

              count - max number of bytes to be used in the buffer

              fmt - format string

              args - variable arguments list

RETURN VALUE  number of characters written
===========================================================================*/

static int core_vsnprintf (char *str, unsigned int count, const char *fmt, va_list args)
{
     unsigned int curpos, ins_pos = 0, digits = 0;
     char *pfmtch = (char *)fmt;
     char prefix0 = 0;
     ParserModeEnum cur_mode = MODE_0;
     char longlong = 0;
     int ivalue;
     char *svalue;
     long long llvalue;

        for(curpos=0; (*pfmtch != '\0') && ( curpos < count); curpos++, pfmtch++)
        {
            str[curpos] = *pfmtch;
            switch (cur_mode)
            {
                case MODE_0:    //Searching for %
                    if(*pfmtch == '%')
                    {
                        prefix0 = 0;
                        digits = 0;
                        longlong = 0;
                        cur_mode = MODE_1;
                        ins_pos = curpos;
                    }
                    break;
                case MODE_1:    //Parsing format flags and width
                    if(isdigit(*pfmtch))
                    {
                        if(*pfmtch == '0') prefix0 = 1;
                        else digits = digits*10 + char_to_int(*pfmtch);
                        break;
                    }
                    else cur_mode = MODE_2;
                    /* No break here */
                case MODE_2:   //Parsing specifier
                    switch(*pfmtch)
                    {
                        case 'l': 
                             if(longlong) cur_mode = MODE_0;
                             else cur_mode = MODE_3;
                             break;
                        case 'X': 
                            curpos = ins_pos;
                            if(longlong) 
                            {
                                llvalue = va_arg (args, long long);
                                addintvalue(str, count, &curpos,  16, prefix0, 1, digits, sizeof(long long), llvalue);
                            }
                            else
                            {
                                ivalue = va_arg (args, int);
                                addintvalue(str, count, &curpos,  16, prefix0, 1, digits, sizeof(int), ivalue);
                            }
                            cur_mode = MODE_0;
                            break;
                        case 'x': 
                            curpos = ins_pos;
                            if(longlong) 
                            {
                                llvalue = va_arg (args, long long);
                                addintvalue(str, count, &curpos,  16, prefix0, 0, digits, sizeof(long long), llvalue);
                            }
                            else
                            {
                                ivalue = va_arg (args, int);
                                addintvalue(str, count, &curpos,  16, prefix0, 0, digits, sizeof(int), ivalue);
                            }
                            cur_mode = MODE_0;
                            break;
                        case 'd': 
                            curpos = ins_pos;
                            if(longlong) 
                            {
                                llvalue = va_arg (args, long long);
                                addintvalue(str, count, &curpos,  10, prefix0, 0, digits, sizeof(long long), llvalue);
                            }
                            else
                            {
                                ivalue = va_arg (args, int);
                                addintvalue(str, count, &curpos,  10, prefix0, 0, digits, sizeof(int), ivalue);
                            }
                            cur_mode = MODE_0;
                            break;
                        case 's': 
                            curpos = ins_pos;
                                svalue = va_arg (args, char *);
                                addstrvalue(str, count, &curpos, svalue);
                            cur_mode = MODE_0;
                            break;
                        case 'p': 
                            curpos = ins_pos;
                            svalue = va_arg (args, char *);
                            addintvalue(str, count, &curpos,  16, 1, 1, 2*sizeof(char *), sizeof(char *), (long long)svalue);
                            cur_mode = MODE_0;
                            break;
                        default: cur_mode = MODE_0;
                    }
                    break;
                case MODE_3:   //Searching for second l in ll
                    switch(*pfmtch)
                    {
                        case 'l': 
                            longlong = 1;
                            cur_mode = MODE_2; 
                            break;
                        default: cur_mode = MODE_0;
                    }
                    break;
            }

        }
        if (curpos < count) str[curpos] = '\0';
        else str[curpos-1] = '\0';
    return curpos;
}

/*===========================================================================
FUNCTION      core_snprintf

DESCRIPTION   Writes formatted output to sized buffer.
              
PARAMETERS    str - pointer to a destination buffer

              count - max number of bytes to be used in the buffer

              fmt - format string

RETURN VALUE  number of characters written
===========================================================================*/
int core_snprintf (char *str, unsigned int  count, const char *fmt, ...)
{
  int ret;
  va_list    ap;
    va_start(ap, fmt);
    ret =  core_vsnprintf(str, count, fmt, ap);
    va_end(ap);
  return ret;
}

/*===========================================================================
FUNCTION      core_strlcpy

DESCRIPTION   Copies the source string into the destination buffer until 
              size is reached, or until a '\0' is encountered.  If valid,
              the destination string will always be NULL deliminated.
              
PARAMETERS    dst - the destination string

              src - the source string

              size - the maximum copy size

RETURN VALUE  pointer to the destination string
===========================================================================*/
char *core_strlcpy(char *dst, const char *src, unsigned int size)
{
  unsigned int sz;

  if( size && dst && src )
  {

      sz = strlen(src) + 1;

      if(sz > size)
      {
        core_memcpy(dst, size-1, src, size-1);
        dst[size-1] = '\0';
      }
      else
      {
        core_memcpy(dst, sz, src, sz);
      }
  } 
  else
  {
      dst = NULL;
  }


  return dst;
}


/*===========================================================================
FUNCTION      core_strchr

DESCRIPTION   Finds the first occurence of the character in the string.
              
PARAMETERS    str - the string

              character - character to be located.

              size - the maximum copy size

RETURN VALUE  pointer to the first occurence of character in the string
===========================================================================*/
char *core_strchr ( char * str, int character )
{
    char *ret = NULL;
    char chr = (char) character;
        if( str )
        {
            for( ret = str; (*ret != '\0') && ((*ret != chr)) ; ret++);
            if ( *ret == '\0' ) ret = NULL;
        }
    return ret;
}

#endif // TARGET_UEFI
