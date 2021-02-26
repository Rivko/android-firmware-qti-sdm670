/*========================================================================*/
/**
@file asm_macros.h

@brief Assembly macros for callee saving and restoring registers on entry and exit 
of functions.

This file contains ASM data commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2009-2010, 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/asm_macros.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/09/10   rkc      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
12/17/10   sw      (TP) For Rev B of the PDF: (1) Edited most Doxygen comments  
                   for clarity. (2) Line 270, reorganized code and C comments so 
                   Doxygen can parse it correctly; approved by Ravi C.
========================================================================== */


/* *****************************************************************************
   Assembly Code Macros
   *****************************************************************************/

/** @addtogroup macros_c_conv
@{ */

#if ((__QDSP6_ARCH__ >= 3) || (__HEXAGON_ARCH__ >= 3))

/* 
ARG1 added to describe number of registers needed to be saved

   8     R17:R16        --new callee saved registers
   16    R19:R18        --new callee saved registers
   24    R21:R20        --new callee saved registers
   32    R23:R22        --new callee saved registers
   40    R25:R24        --existing callee saved registers
   48    R27:R26        --existing callee saved registers
         R28            --not used for anything in particular
         R29            --STACK register, not typically save, just decremented
   56    R31:R30        --link (ret) pointer : stack frame pointer saved with ALLOCFRAME
*/

/**
  If the Hexagon architecture is version 3 or higher, an offset of 32 bytes is 
  used to save eight registers.

  If the Hexagon architecture is earlier than version 3, a dummy offset of 0 bytes is used.
*/
#define SAVEONENTRY_32_OFFSET   32

/**
  If the Hexagon architecture is version 3 or higher, an offset of 8 bytes is 
  used to save two registers.

  If the Hexagon architecture is earlier than version 3, a dummy offset of 0 bytes is used.
*/
#define SAVEONENTRY_8_OFFSET    8

/**
  If the Hexagon architecture is version 3 or higher, this macro saves registers R16 and 
  R17 on the stack upon entry.

  If the Hexagon architecture is earlier than version 3, this macro does nothing.
*/
#define __saveonentry_8 \
          SP = ADD(SP, #-8);               /* Allocates a stack pointer. */\
          MEMD(SP+#0)  = R17:16;           /* Saves R16:R17.  */

/**
  If the Hexagon architecture is version 3 or higher, this macro saves registers R16 to 
  R23 on the stack upon entry.

  If the Hexagon architecture is earlier than version 3, this macro does nothing.
*/
#define __saveonentry_32 \
        { SP = ADD(SP, #-32);              /* Allocates a stack pointer. */\
          MEMD(SP+#-8) = R23:22;           /* Saves R22:R23.      */\
        }                                                        \
          MEMD(SP+#16) = R21:20;           /* Saves R20:R21.      */\
          MEMD(SP+#8)  = R19:18;           /* Saves R18:R19.      */\
          MEMD(SP+#0)  = R17:16;           /* Saves R16:R17.      */\

/**
  If the Hexagon architecture is version 3 or higher, this macro allocates a stack frame 
  upon entry and saves registers R16 to R23 at an offset in this frame.

  If the Hexagon architecture is earlier than version 3, this macro just allocates 
  the stack.

  @param BYTES    Total number of bytes used in the stack frame.
  @param OFFSET   Relative position of the save-on-entry registers in the 
                  stack frame.
*/
#define __alloconentry(BYTES, OFFSET) \
          ALLOCFRAME(#(BYTES));             /* Allocates a stack; saves the 
                                               stack and frame pointer.*/\
          MEMD(SP+#(0+OFFSET))  = R17:16;   /* Saves R16:R17.      */\
          MEMD(SP+#(8+OFFSET))  = R19:18;   /* Saves R18:R19.      */\
          MEMD(SP+#(16+OFFSET)) = R21:20;   /* Saves R20:R21.      */\
          MEMD(SP+#(24+OFFSET)) = R23:22;   /* Saves R22:R23.      */
          
/**
  If the Hexagon architecture is version 3 or higher, this macro restores registers R16 
  and R17, deallocates the stack upon exit, and returns to the caller.

  If the Hexagon architecture is earlier than version 3, this macro just returns to the 
  caller.
*/
#define __restoreonexit_8 \
        {                                                         \
          R17:16 = MEMD(SP++#8);           /* Restores R16:R17
                                              and deallocates stack. */\
          JUMPR R31;                       /* Returns to the caller. */\
        }

/**
  If the Hexagon architecture is version 3 or higher, this macro restores registers R16 
  to R23, deallocates the stack upon exit, and returns to the caller.

  If the Hexagon architecture is earlier than version 3, this macro just returns to the 
  caller.
*/
#define __restoreonexit_32 \
        { R19:18 = MEMD(SP+#8);            /* Restores R18:R19. */\
          R21:20 = MEMD(SP+#16);           /* Restores R20:R21. */\
        }                                                              \
        { R23:22 = MEMD(SP+#24);           /* Restores R22:R23. */\
          R17:16 = MEMD(SP++#32);          /* Restores R16:R17 and
                                              deallocates the stack. */\
          JUMPR R31;                       /* Returns to the caller.    */\
        }

/**
  If the Hexagon architecture is version 3 or higher, this macro deallocates a stack 
  frame upon exit and restores registers R16 to R23 plus the offset in this 
  frame.

  If the Hexagon architecture is earlier than version 3, this macro just deallocates the 
  stack and returns to the caller.
    
  @param BYTES    Total number of bytes used in the stack frame. Not needed; 
                  added for completeness.
  @param OFFSET   Relative position of the new registers in the stack 
                  frame.
*/
#define __dealloconexit(BYTES, OFFSET) \
        { R17:16 = MEMD(SP+#(0+OFFSET));   /* Restores R16:R17 and
                                              deallocates the stack. */\
          R19:18 = MEMD(SP+#(8+OFFSET));   /* Restores R18:R19. */\
        }                                                             \
        { R21:20 = MEMD(SP+#(16+OFFSET));  /* Restores R20:R21. */\
          R23:22 = MEMD(SP+#(24+OFFSET));  /* Restores R22:R23. */\
        }                                                        \
        DEALLOCFRAME;                      /* Frees the stack and restores
                                              frame/link pointers. */\
        JUMPR R31;                         /* Returns to the caller. */


#else //__QDSP6_ARCH__ >= 3

/* 
ARG1 added to describe the number of registers needed to be saved.
   8     R25:R24        --existing callee saved registers
   16    R27:R26        --existing callee saved registers
         R28            --not used for anything in particular
         R29            --STACK register, not typically save, just decremented
   24    R31:R30        --link (ret) pointer : stack frame pointer saved with ALLOCFRAME
*/

/**
  If the Hexagon architecture is version 3 or higher, an offset of 32 bytes is 
  used to save eight registers.

  If the Hexagon architecture is earlier than version 3, a dummy offset of 0 bytes is used.
*/
#define SAVEONENTRY_32_OFFSET   0

/**
  If the Hexagon architecture is version 3 or higher, an offset of 8 bytes is 
  used to save two registers.

  If the Hexagon architecture is earlier than version 3, a dummy offset of 0 bytes is used.
*/
#define SAVEONENTRY_8_OFFSET    0

/**
  If the Hexagon architecture is version 3 or higher, this macro saves registers R16 and 
  R17 on the stack, deallocates the stack upon exit, and returns to the caller.

  If the Hexagon architecture is earlier than version 3, this macro just returns to the 
  caller.
*/
#define __saveonentry_8

/**
  If the Hexagon architecture is version 3 or higher, this macro saves registers R16 to 
  R23 on the stack, deallocates the stack upon exit, and returns to the caller.

  If the Hexagon architecture is earlier than version 3, this macro just returns to the 
  caller.
*/
#define __saveonentry_32

/**
  If the Hexagon architecture is version 3 or higher, this macro allocates a stack frame 
  upon entry and saves registers R16 to R23 at an offset in this frame.

  If the Hexagon architecture is earlier than version 3, this macro just allocates the 
  stack.

  @param BYTES    Total number of bytes used in the stack frame.
  @param OFFSET   Relative position of the save-on-entry registers in the 
                  stack frame.
*/
#define __alloconentry(BYTES, OFFSET) \
          ALLOCFRAME(#(BYTES));             /* Allocates the stack; saves the 
                                               stack and frame pointer. */
          
/**
  If the Hexagon architecture is version 3 or higher, this macro restores registers R16 
  and R17 and deallocates the stack upon exit.

  If the Hexagon architecture is earlier than version 3, this macro returns to the caller.
*/
#define __restoreonexit_8  JUMPR R31;

/**
  If the Hexagon architecture is version 3 or higher, this macro restores registers R16 
  to R23 and deallocates the stack upon exit.

  If the Hexagon architecture is earlier than version 3, this macro returns to the caller.
*/
#define __restoreonexit_32 JUMPR R31;

/**
  If the Hexagon architecture is version 3 or higher, this macro deallocates a stack 
  frame upon exit and restores registers R16 to R23 plus the offset in this 
  frame. 

  If the Hexagon architecture is earlier than version 3, this macro deallocates the 
  stack and returns to the caller.
  
  @param BYTES    Total number of bytes used in the stack frame. Not needed; 
                  added for completeness.
  @param OFFSET   Relative position of the new registers in the stack 
                  frame.
*/
#define __dealloconexit(BYTES, OFFSET) DEALLOCFRAME; JUMPR R31;     
          /* DEALLOCFRAME frees the stack and restores the frame/link pointers. 
             JUMPR R31 returns to the caller. */

/** @} */ /* end_addtogroup macros_c_conv */

#endif //__QDSP6_ARCH__ >= 3
