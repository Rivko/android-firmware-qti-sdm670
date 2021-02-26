#ifndef PM_PMIO_SPMI_H
#define PM_PMIO_SPMI_H

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_pmio.h"
#include "DDISpmi.h"


/*=========================================================================
     PMIO Function Implementation
==========================================================================*/

/* =========================================================================
**  Function : in_pmio
** =========================================================================*/
/**
  Reads a PMIC register.
*/
static inline DALResult in_pmio
(
  DalDeviceHandle *_h,
  uint8    pmic,
  uint32   addr,
  uint8   *val,
  uint32   len
)
{
  uint32 bytes_read;
  return DalSpmi_ReadLong(_h, pmic, SPMI_ACCESS_PRIORITY_LOW, addr & 0xFFFF, val, len, &bytes_read);

} /* END in_pmio */

/* =========================================================================
**  Function : out_pmio
** =========================================================================*/
/**
  Writes a PMIC register.
*/

static inline DALResult out_pmio
(
  DalDeviceHandle *_h,
  uint8    pmic,
  uint32   addr,
  uint8    val
)
{
  return DalSpmi_WriteLong(_h, pmic, SPMI_ACCESS_PRIORITY_LOW, addr & 0xFFFF, &val, 1);

} /* END out_pmio */


/* =========================================================================
**  Function : out_pmio_array
** =========================================================================*/
/**
  Writes an array of PMIC registers.
*/

static inline DALResult out_pmio_array
(
  DalDeviceHandle *_h,
  uint8     pmic,
  uint32    addr,
  uint8    *val,
  uint32    len
)
{
  return DalSpmi_WriteLong(_h, pmic, SPMI_ACCESS_PRIORITY_LOW, addr & 0xFFFF, val, len);

} /* END out_pmio_array */


/* =========================================================================
**  Function : in_pmio_field
** =========================================================================*/
/**
  Reads a field from a PMIC register.
*/
static inline DALResult in_pmio_field
(
  DalDeviceHandle *_h,
  uint8    pmic,
  uint32   addr,
  uint8    mask,
  uint8    shift,
  uint8   *val
)
{
  DALResult err = in_pmio(_h, pmic, addr, val, 1);
  *val = (*val & mask) >> shift;
  return err;

} /* END in_pmio_field */


/* =========================================================================
**  Function : out_pmio_field
** =========================================================================*/
/**
  Writes to a PMIC register field.
*/
static inline DALResult out_pmio_field
(
  DalDeviceHandle *_h,
  uint8    pmic,
  uint32   addr,
  uint8    mask,
  uint8    shift,
  uint8    val
)
{
  uint8 reg;
  DALResult err;

  if ((err = in_pmio(_h, pmic, addr, &reg, 1)) != DAL_SUCCESS)
  {
    return err;
  }

  return out_pmio(_h, pmic, addr, (reg & ~mask) | (val << shift));

} /* END out_pmio_field */

#endif /* PM_PMIO_SPMI_H */

