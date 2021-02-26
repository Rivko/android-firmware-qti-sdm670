;============================================================================
;
;             ProcAsmLib
;
; GENERAL DESCRIPTION
;   Generic processor control routines in assembly
;
; Copyright 2012 - 2013, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved 
;============================================================================

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  -----   ----------------------------------------------------------
; 09/14/15   bh      Add reg read functions
; 10/22/13   vk      Add WriteCNTKCTL
; 03/25/13   nk      Add ETM clock enable 
; 03/13/13   yg      Rename API
; 03/03/13   yg      Add SMP register access functions.
; 05/01/12   yg      Created new file
;============================================================================


;============================================================================
;                             MODULE EXPORTS
;============================================================================
   EXPORT  ReadPMonCtlReg
   EXPORT  WritePMonCtlReg
   EXPORT  ReadUserEnReg
   EXPORT  WriteUserEnReg
   EXPORT  ReadCntEnSetReg
   EXPORT  WriteCntEnSetReg
   EXPORT  ReadCntEnClrReg
   EXPORT  WriteCntEnClrReg
   EXPORT  ReadPerfCntSelReg
   EXPORT  WritePerfCntSelReg
   EXPORT  ReadCycleCntReg
   EXPORT  WriteCycleCntReg
   EXPORT  ReadEvtSelReg
   EXPORT  WriteEvtSelReg

   EXPORT  ReadTPIDRURWReg
   EXPORT  WriteTPIDRURWReg
   EXPORT  ReadTPIDRUROReg
   EXPORT  WriteTPIDRUROReg

   EXPORT  ReadACTLR
   EXPORT  WriteACTLR
      
   EXPORT  ReadCPMR
   EXPORT  WriteCPMR
   EXPORT  WriteETMCR
   EXPORT  WriteCNTKCTL
   EXPORT  ReadABit
   EXPORT  ReadDACRReg
      
;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================

  AREA    ProcAsmLib, CODE, READONLY

;============================================================================
;
;  UINT32 ReadPMonCtlReg (void);
;
;============================================================================
ReadPMonCtlReg
  MRC p15, 0, R0, c9, c12, 0 ; Read PMNC Register
  BX  LR
  
;============================================================================
;
;  void WritePMonCtlReg(UINT32 Val);
;
;============================================================================
WritePMonCtlReg
  MCR p15, 0, R0, c9, c12, 0 ; Write PMNC Register
  BX LR

;============================================================================
;
;  UINT32 ReadUserEnReg(void);
;
;============================================================================
ReadUserEnReg
  MRC p15, 0, R0, c9, c14, 0 ; Read USEREN Register
  BX  LR
  
;============================================================================
;
;  void WriteUserEnReg(UINT32 Val);
;
;============================================================================
WriteUserEnReg
  MCR p15, 0, R0, c9, c14, 0 ; Write USEREN Register
  BX LR

;============================================================================
;
;  UINT32 ReadCntEnSetReg(void);
;
;============================================================================
ReadCntEnSetReg
  MRC p15, 0, R0, c9, c12, 1 ; Read CNTENS Register
  BX LR

;============================================================================
;
;  void WriteCntEnSetReg(UINT32 Val);
;
;============================================================================
WriteCntEnSetReg
  MCR p15, 0, R0, c9, c12, 1 ; Write CNTENS Register
  BX LR

;============================================================================
;
;  UINT32 ReadCntEnClrReg(void);
;
;============================================================================
ReadCntEnClrReg
  MRC p15, 0, R0, c9, c12, 2 ; Read CNTENC Register
  BX LR

;============================================================================
;
;  void WriteCntEnClrReg(UINT32 Val);
;
;============================================================================
WriteCntEnClrReg
  MCR p15, 0, R0, c9, c12, 2 ; Write CNTENC Register
  BX LR

;============================================================================
;
;  UINT32 ReadPerfCntSelReg(void);
;
;============================================================================
ReadPerfCntSelReg
  MRC p15, 0, R0, c9, c12, 5; Read PMNXSEL Register
  BX LR

;============================================================================
;
;  void WritePerfCntSelReg(UINT32 Val);
;
;============================================================================
WritePerfCntSelReg
  MCR p15, 0, R0, c9, c12, 5; Write PMNXSEL Register
  BX LR

;============================================================================
;
;  UINT32 ReadCycleCntReg(void);
;
;============================================================================
ReadCycleCntReg
  MRC p15, 0, R0, c9, c13, 0 ; Read CCNT Register
  BX LR

;============================================================================
;
;  void WriteCycleCntReg(UINT32 Val);
;
;============================================================================
WriteCycleCntReg
  MCR p15, 0, R0, c9, c13, 0 ; Write CCNT Register
  BX LR

;============================================================================
;
;  UINT32 ReadEvtSelReg(void);
;
;============================================================================
ReadEvtSelReg
  MRC p15, 0, R0, c9, c13, 1 ; Read EVTSEL Register
  BX LR

;============================================================================
;
;  void WriteEvtSelReg(UINT32 Val);
;
;============================================================================
WriteEvtSelReg
  MCR p15, 0, R0, c9, c13, 1 ; Write EVTSEL Register
  BX LR

;============================================================================
;
;  UINT32 ReadTPIDRURWReg(void);
;
;============================================================================
ReadTPIDRURWReg
  MRC p15, 0, R0, c13, c0, 2 ; Read TPIDRURW Register
  BX LR

;============================================================================
;
;  void WriteTPIDRURWReg(UINT32 Val);
;
;============================================================================
WriteTPIDRURWReg
  MCR p15, 0, R0, c13, c0, 2 ; Write TPIDRURW Register
  BX LR

;============================================================================
;
;  UINT32 ReadTPIDRUROReg(void);
;
;============================================================================
ReadTPIDRUROReg
  MRC p15, 0, R0, c13, c0, 3 ; Read TPIDRURO Register
  BX LR

;============================================================================
;
;  void WriteTPIDRUROReg(UINT32 Val);
;
;============================================================================
WriteTPIDRUROReg
  MCR p15, 0, R0, c13, c0, 3 ; Write TPIDRURO Register
  BX LR

;============================================================================
;
;  UINT32 ReadACTLR(VOID);
;
;============================================================================
ReadACTLR
  MRC p15, 0, R0, c1, c0, 1 ; Read ACTLR Register
  BX LR

;============================================================================
;
;  VOID WriteACTLR(UINT32 Val);
;
;============================================================================
WriteACTLR
  MCR p15, 0, R0, c1, c0, 1 ; Write ACTLR Register
  BX LR

;===========================================================================
;
;  UINT32 ReadCPMR(VOID);
;
;============================================================================
ReadCPMR
   MRC p15, 7, R0, c15, c0, 5 ; Read CPMR Register
   BX LR

;============================================================================
;
;  VOID WriteCPMR(UINT32 Val)
;
;============================================================================
WriteCPMR
   MCR p15, 7, R0, c15, c0, 5 ; Write 1 to ETMCLKEN bit
   BX LR

;============================================================================
;
;  VOID WriteETMCR(UINT32 Val);
;
;============================================================================
WriteETMCR
   MCR p14, 1, R0, c0, c0, 0 ; Write ETMCR Register
   BX LR

;============================================================================
;
;  VOID WriteCNTKCTL(UINT32 Val);
;
;============================================================================
WriteCNTKCTL
   MCR p15, 0, R0, c14, c1, 0 ; Write CNTKCTL Register
   BX LR

;============================================================================
;
;  UINT32 ReadABit(VOID);
;
;============================================================================
ReadABit
   MRS R0, CPSR
   MOV R1, #1
   AND R0, R1, R0, LSR #8
   BX LR

;============================================================================
;
;  UINT32 ReadDACRReg(VOID);
;
;============================================================================
ReadDACRReg
   MRC p15, 0, R0, c3, c0, 0
   BX LR

;===========================================================================

   END
