/*=============================================================================

                                    debug_me_asm.S

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

.section .text
.global debug_me
.type	debug_me, @function

debug_me:
    //P:0846D884|7F00C000                   debug_me: nop;
    nop    
    //P:0846D888|7F00C000                             nop;
    nop
    //P:0846D88C|78004021 78004042 28002833             R1=#0x1; R2=#0x2; R0=#0x0; R3=#0x3;
    {
        r1 = #0;
        r2 = add(r0, r1);
        r3 = sub(r0, r1);
        r4 = #4;
    }
    //P:0846D898|00845B627800C480                     R0=##0x846D8A4;    
    jump ##debug_me_continue
   .size debug_me, .-debug_me