#if ( __QDSP6_ARCH__ >= 4 )

.text

/**************************************************************/
/* Function:    afe_sb_read_avt_sb_time_asm                   */
/**************************************************************/
/* Description: reads the avtimer time and slimbus device time*/
/*              atomically using spinlock.                    */
/* Reg Inputs:  R0 = ptr to afe_dev_port_t structure          */
/*              R1 = ptr to slimbus time in us uint64_t ptr   */
/*              R2 = ptr to av_time in us uint64_t ptr        */
/* Mem Inputs:  None                                          */
/* Reg Outputs: None                                          */
/* Mem Outputs:                                               */
/* SideEffects:                                               */
/* Stack Usage:                                               */
/* Notes:       Callable from C, frame size must > 0          */
/* Cycles:      N/A                                           */
/**************************************************************/

#define StackSizeR16R17 8
#define StackSizeR18R19 8
#define StackSizeR20R21 8
#define StackSizeR22R23 8
#define StackSizeR24R25 8
#define StackSizeR26R27 8
#define StackSizeR30R31 8

#define StackSizeTotal \
        StackSizeR16R17 + \
        StackSizeR18R19 + \
        StackSizeR20R21 + \
        StackSizeR22R23 + \
        StackSizeR24R25 + \
        StackSizeR26R27 + \
        StackSizeR30R31

#define StackOffsetR16R17 StackSizeTotal-StackSizeR16R17
#define StackOffsetR18R19 StackOffsetR16R17-StackSizeR18R19
#define StackOffsetR20R21 StackOffsetR18R19-StackSizeR20R21
#define StackOffsetR22R23 StackOffsetR20R21-StackSizeR22R23
#define StackOffsetR24R25 StackOffsetR22R23-StackSizeR24R25
#define StackOffsetR26R27 StackOffsetR24R25-StackSizeR26R27
#define StackOffsetR30R31 StackOffsetR26R27-StackSizeR30R31

.p2align 2
.globl afe_sb_read_avt_sb_time_asm
.type  afe_sb_read_avt_sb_time_asm, @function

afe_sb_read_avt_sb_time_asm:
   {
      ALLOCFRAME(#StackSizeTotal)
   }
   {
       MEMD(R29+#StackOffsetR30R31) = R31:30
      MEMD(R29+#StackOffsetR24R25) = R25:24
      R25:24 = R1:0						  //store R1:0 (dev_port_ptr,sb_time) in R25:24
   }
   {
      MEMD(R29+#StackOffsetR26R27) = R27:26
      MEMD(R29+#StackOffsetR20R21) = R21:20
   }
   {
     R27:26 = R3:2						         //store R2 (av_time) in R26
	  R20 = ADD(R29,#(StackOffsetR22R23))	   //pointer to local var in R20
   }

.falign
.Lspinlock_loop:
   {
      R27 = memw_locked(R20)			      //read locked from a local variable.
   }

   {
      call qurt_elite_timer_get_time    // *av_time  = qurt_elite_timer_get_time();
   }

   {
      memd(R26) = R1:0                    //store av_time
   }

   {
      R1:0 = R25:24                       //retrieve dev_port_ptr,sb_time
   }

   {
      call afe_sb_get_time_for_avt_afe      //afe_sb_get_time_for_avt_afe(dev_port_ptr, sb_time);
   }

   {
      memw_locked(R20,P0) = R27			 //store conditional to the local variable
   }

   {
      if (!P0) jump .Lspinlock_loop       		//if it wasnot atomic, keep spinning
   }
   {
      R21:20 = MEMD(R29+#StackOffsetR20R21)			//retrieve R20, 21
      R27:26 = MEMD(R29+#StackOffsetR26R27)  		//retrieve R26, 27
   }
   {
      R31:30 = MEMD(R29+#StackOffsetR30R31)  	    //retrieve R30, 31
      R25:24 = MEMD(R29+#StackOffsetR24R25)  	//retrieve R25, R24
   }
#ifndef FEATURE_POPQUEUE_CONFLICT
   {
      DEALLOCFRAME                        		//deallocate frame
      JUMPR R31                           		//return
   }
#else
   {
      DEALLOCFRAME                        		//deallocate frame
   
}
{
      JUMPR R31                           		//return
}
#endif
   .size afe_sb_read_avt_sb_time_asm, .-afe_sb_read_avt_sb_time_asm


#endif //( __QDSP6_ARCH__ >= 3 )
