/*============================================================================

FILE:      DalHWIOLogMsg.c

DESCRIPTION:
  This file implements the HWIO access message logging portion of the
  HWIO DAL.

PUBLIC CLASSES:
  __inp_extern
  __inpw_extern
  __inpdw_extern
  __outp_extern
  __outpw_extern
  __outpdw_extern

============================================================================
        Copyright (c) 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/hwio/src/DalHWIOLogMsg.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "msg.h"



/*=========================================================================
      Implementation
==========================================================================*/


/*==========================================================================

  FUNCTION      __inp_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

uint8 __inp_extern
(
  uint32 nAddr
)
{
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "HWIO IN BYTE: 0x%08x", nAddr);

  return (*((volatile uint8 *)(nAddr)));

} /* END __inp_extern */


/*==========================================================================

  FUNCTION      __inpw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

uint16 __inpw_extern
(
  uint32 nAddr
)
{
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "HWIO IN WORD: 0x%08x", nAddr);

  return (*((volatile uint16 *)(nAddr)));

} /* END __inpw_extern */


/*==========================================================================

  FUNCTION      __inpdw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

uint32 __inpdw_extern
(
  uint32 nAddr
)
{
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "HWIO IN DWORD: 0x%08x", nAddr);

  return (*((volatile uint32 *)(nAddr)));

} /* END __inpdw_extern */


/*==========================================================================

  FUNCTION      __outp_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

void __outp_extern
(
  uint32 nAddr,
  uint8  nData
)
{
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "HWIO OUT BYTE: 0x%08x = %d", nAddr, nData);

  *((volatile uint8 *)(nAddr)) = nData;

} /* END __outp_extern */


/*==========================================================================

  FUNCTION      __outpw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

void __outpw_extern
(
  uint32 nAddr,
  uint16 nData
)
{
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "HWIO OUT WORD: 0x%08x = %d", nAddr, nData);

  *((volatile uint16 *)(nAddr)) = nData;

} /* END __outpw_extern */


/*==========================================================================

  FUNCTION      __outpdw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

void __outpdw_extern
(
  uint32 nAddr,
  uint32 nData
)
{
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "HWIO OUT DWORD: 0x%08x = %d", nAddr, nData);

  *((volatile uint32 *)(nAddr)) = nData;

} /* END __outpdw_extern */

