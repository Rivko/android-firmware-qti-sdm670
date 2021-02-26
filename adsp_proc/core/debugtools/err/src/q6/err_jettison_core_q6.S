
.global arch_coredump_ptr	
.global jettison_core;

jettison_core:
  allocframe(#8)     // Allocate stack frame
  memw(r29) = r7     // Store orginal r7 onto stack		 
  #ifdef __PIC__
  r7 = add(pc, ##_GLOBAL_OFFSET_TABLE_@PCREL)
  r7 = memw(r7+##arch_coredump_ptr@GOT)  // Get arch_coredump_ptr into r7
  #else
  r7 = ##arch_coredump_ptr   // Get arch_coredump_ptr into r7
  #endif
  r7 = memw(r7)      // Dereference arch_coredump_ptr in r7
  memw(r7) = r0      // Begin storing all registers into coredump struct
  memw(r7+#4) = r1	
  memw(r7+#8) = r2
  memw(r7+#12) = r3
  memw(r7+#16) = r4
  memw(r7+#20) = r5
  memw(r7+#24) = r6
  memw(r7+#32) = r8  // Skip r7 for now
  memw(r7+#36) = r9
  memw(r7+#40) = r10
  memw(r7+#44) = r11
  memw(r7+#48) = r12
  memw(r7+#52) = r13
  memw(r7+#56) = r14
  memw(r7+#60) = r15
  memw(r7+#64) = r16
  memw(r7+#68) = r17
  memw(r7+#72) = r18
  memw(r7+#76) = r19
  memw(r7+#80) = r20
  memw(r7+#84) = r21
  memw(r7+#88) = r22
  memw(r7+#92) = r23
  memw(r7+#96) = r24
  memw(r7+#100) = r25
  memw(r7+#104) = r26
  memw(r7+#108) = r27
  memw(r7+#112) = r28
  memw(r7+#116) = r29
  memw(r7+#120) = r30
  memw(r7+#124) = r31
  r0 = r7             // Move arch_coredump_ptr pointer to r0
  r7 = memw(r29)      // Restore original r7 value	
  r1 = PC             // Store PC into r1 so we can store it to memory
  memw(r0+#128) = r1  // Store PC (now in r1) to coredump
  memw(r0+#28) = r7   // Store original r7 value to coredump
  deallocframe        // Deallocate stack frame
  jumpr r31           // Return from function

.global err_halt_execution; 
err_halt_execution:
  stop(r0)
		