/***************************************************************************
* Copyright (c) Date: Mon Nov 24 16:26:00 CST 2008 Qualcomm Technologies INCORPORATED 
* All Rights Reserved 
* Modified by Qualcomm Technologies INCORPORATED on Mon Nov 24 16:26:00 CST 2008 
****************************************************************************/ 


/* *****************************************************************************
   Assembly Code Macros
   *****************************************************************************/


/* ARG1 added to desxcribe number of registers needed to be saved

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
#define __return \
          JUMP  R31                        /*return back to caller */\

#define __saveonentry_8 \
          SP = ADD(SP, #-8);               /*allocate stack ptr*/\
          MEMD(SP+#0)  = R17:16;           /*save r16,r17      */

#define __saveonentry_32 \
        { SP = ADD(SP, #-32);              /*allocate stack ptr*/\
          MEMD(SP+#-8) = R23:22;           /*save r22,r23      */\
        }                                                        \
          MEMD(SP+#16) = R21:20;           /*save r20,r21      */\
                                                                 \
          MEMD(SP+#8)  = R19:18;           /*save r18,r19      */\
                                                                 \
          MEMD(SP+#0)  = R17:16;           /*save r16,r17      */\


/*
      BYTES : total number of bytes used in stack frame
      OFFSET : relative position of the new SOE registers in the stack frame
 */
#define __alloconentry(BYTES, OFFSET) \
	  ALLOCFRAME(#(BYTES));              /*allocate stack, save stack and frame ptr*/\
                                                                 \
          MEMD(SP+#(0+OFFSET))  = R17:16; /*save r16,r17      */\
                                                                 \
          MEMD(SP+#(8+OFFSET))  = R19:18;    /*save r18,r19      */\
                                                                 \
          MEMD(SP+#(16+OFFSET)) = R21:20;    /*save r20,r21      */\
                                                                 \
          MEMD(SP+#(24+OFFSET)) = R23:22;    /*save r22,r23      */

#define __restoreonexit_8 \
        {                                                         \
          R17:16 = MEMD(SP++#8);           /*restore r16,r17 & dealloc stack */\
          JUMPR R31;                       /*return to caller    */\
        }

#define __restoreonexit_32 \
        { R19:18 = MEMD(SP+#8);            /*restore r18,r19     */\
          R21:20 = MEMD(SP+#16);           /*restore r20,r21     */\
        }                                                          \
        { R23:22 = MEMD(SP+#24);           /*restore r22,r23     */\
          R17:16 = MEMD(SP++#32);          /*restore r16,r17 & dealloc stack */\
          JUMPR R31;                       /*return to caller    */\
        }

/*
      BYTES : total number of bytes used in stack frame, not needed, added for 
              completeness
      OFFSET : relative position of the new SOE registers in the stack frame
 */
#define __dealloconexit(BYTES, OFFSET) \
        { R17:16 = MEMD(SP+#(0+OFFSET));     /*restore r16,r17 & dealloc stack */\
          R19:18 = MEMD(SP+#(8+OFFSET));     /*restore r18,r19     */\
        }                                                          \
        { R21:20 = MEMD(SP+#(16+OFFSET));    /*restore r20,r21     */\
          R23:22 = MEMD(SP+#(24+OFFSET));    /*restore r22,r23     */\
        }                                                        \
	  DEALLOCFRAME;                    /*free stack and restore frame/link ptrs */\
                                                                   \
          JUMPR R31;                       /*return to caller    */
/*******************************************************************************
 *                 Qualcomm Proprietary 2008                                   * 
 *******************************************************************************/
