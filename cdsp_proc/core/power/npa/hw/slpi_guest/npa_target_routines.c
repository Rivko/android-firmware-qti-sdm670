/*============================================================================
@file npa_target_routines.c

NPA target initialization for SLPI User PD

Copyright (c) 2010 - 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/hw/slpi_guest/npa_target_routines.c#1 $
============================================================================*/

#ifdef NPA_USES_QDI
/* Protocol init routine, defined in npa_remote_qdi_protocol.c */
extern void npa_qdi_remote_init(void);
#endif

void npa_target_init( void )
{
#ifdef NPA_USES_QDI
  npa_qdi_remote_init();
#endif
}
