/*===========================================================================
  @file
  qmi_client_helper.c

  @brief
  QCCI client library wrapper for performing port binding management.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/14   hm      Initial module.

===========================================================================*/

#include <ctype.h>
#include "qmi_port_defs.h"
#include "qmi_platform.h"
#include "qmi_client_helper.h"
#include "wireless_data_service_v01.h"
#include "data_filter_service_v01.h"
#include "quality_of_service_v01.h"
#include "ds_util.h"

typedef qmi_client_qmux_instance_type (*qmi_get_qmux_inst_func_type)
(
  const char *cid,
  qmi_client_qmux_instance_type *instance_id
);

static qmi_client_qmux_instance_type qmi_client_helper_get_msm_instance
(
  const char *cid,
  qmi_client_qmux_instance_type *instance_id
);

static qmi_client_qmux_instance_type qmi_client_helper_get_mdm_instance
(
  const char *cid,
  qmi_client_qmux_instance_type *instance_id
);

typedef struct
{
  qmi_client_qmux_instance_type  inst;
  qmi_get_qmux_inst_func_type    func;
} qmi_client_helper_instance_type;

typedef enum
{
  QMI_CLIENT_HELPER_RMNET_INSTANCE,
  QMI_CLIENT_HELPER_RMNET_USB_INSTANCE,
  QMI_CLIENT_HELPER_SMUX_INSTANCE,
  QMI_CLIENT_HELPER_MHI_INSTANCE,
  QMI_CLIENT_HELPER_MSM_INSTANCE,
  QMI_CLIENT_HELPER_MDM_INSTANCE,
} qmi_client_helper_instance_e;

/* This should be in the same order as enum qmi_client_helper_instance_e */
static const qmi_client_helper_instance_type qmux_inst_arr[] =
{
  { QMI_CLIENT_QMUX_RMNET_INSTANCE_0,      NULL }, /* RMNET_INSTANCE */
  { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,  NULL }, /* RMNET_USB_INSTANCE */
  { QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0, NULL }, /* SMUX_INSTANCE */
  { QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0,  NULL }, /* MHI_INSTANCE */
  { QMI_CLIENT_QMUX_RMNET_INSTANCE_0,      qmi_client_helper_get_msm_instance }, /* MSM_INSTANCE */
  { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,  qmi_client_helper_get_mdm_instance }, /* MDM_INSTANCE */
};

/* QMUX xports that QCCI uses - SMD, HSIC, SMUX, MHI */
#define MAX_QMUX_XPORTS (4)
typedef qmi_client_qmux_instance_type  qcci_xports_type[MAX_QMUX_XPORTS+1];

/* The following defines are tightly coupled with the ordering of disable_qmux_xports[] */
#define DISABLE_XPORTS_USB_SMUX_MHI (disable_qmux_xports[0])
#define DISABLE_XPORTS_SMD_SMUX_MHI (disable_qmux_xports[1])
#define DISABLE_XPORTS_SMUX_MHI     (disable_qmux_xports[2])
#define DISABLE_XPORTS_USB_MHI      (disable_qmux_xports[3])
#define DISABLE_XPORTS_SMD_MHI      (disable_qmux_xports[4])
#define DISABLE_XPORTS_SMD_USB_SMUX (disable_qmux_xports[5])
#define DISABLE_XPORTS_ALL          (disable_qmux_xports[6])

static const qcci_xports_type disable_qmux_xports[] =
{
  /* Disable USB, SMUX, MHI */
  {QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,  QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0, QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0, -1},
  /* Disable SMD, SMUX, MHI */
  {QMI_CLIENT_QMUX_RMNET_INSTANCE_0,      QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0, QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0, -1},
  /* Disable SMUX, MHI */
  {QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0, QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0,  -1},
  /* Disable USB, MHI */
  {QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,  QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0,  -1},
  /* Disable SMD, MHI */
  {QMI_CLIENT_QMUX_RMNET_INSTANCE_0,      QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0,  -1},
  /* Disable SMD, USB, SMUX */
  {QMI_CLIENT_QMUX_RMNET_INSTANCE_0,      QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,  QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0, -1},

  /* Disable all: SMD, USB, SMUX, MHI */
  {QMI_CLIENT_QMUX_RMNET_INSTANCE_0,      QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,  QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0, QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0, -1},
};

#define RMNET_INSTANCE       (&qmux_inst_arr[QMI_CLIENT_HELPER_RMNET_INSTANCE])
#define RMNET_USB_INSTANCE   (&qmux_inst_arr[QMI_CLIENT_HELPER_RMNET_USB_INSTANCE])
#define SMUX_INSTANCE        (&qmux_inst_arr[QMI_CLIENT_HELPER_SMUX_INSTANCE])
#define MHI_INSTANCE         (&qmux_inst_arr[QMI_CLIENT_HELPER_MHI_INSTANCE])
#define MSM_INSTANCE         (&qmux_inst_arr[QMI_CLIENT_HELPER_MSM_INSTANCE])
#define MDM_INSTANCE         (&qmux_inst_arr[QMI_CLIENT_HELPER_MDM_INSTANCE])
#define INVALID_INSTANCE     (NULL)

typedef struct
{
  ds_target_t target;
  const qmi_client_helper_instance_type *qmux_inst;
  const qmi_client_helper_instance_type *qmux_inst2; /* For dual-modem targets */
  const qmi_client_qmux_instance_type   *disable_xports;
} qmi_client_xport_reg_s;

static const qmi_client_xport_reg_s xport_reg_arr[] =
{
  /* Generic target types */
  { DS_TARGET_MSM,              MSM_INSTANCE,          INVALID_INSTANCE,    DISABLE_XPORTS_USB_SMUX_MHI },
  { DS_TARGET_MDM,              MDM_INSTANCE,          INVALID_INSTANCE,    DISABLE_XPORTS_SMD_SMUX_MHI },
  { DS_TARGET_APQ,              INVALID_INSTANCE,      INVALID_INSTANCE,    DISABLE_XPORTS_ALL          },

  { DS_TARGET_SVLTE1,           RMNET_USB_INSTANCE,    RMNET_INSTANCE,      DISABLE_XPORTS_SMUX_MHI     },
  { DS_TARGET_SVLTE2,           RMNET_USB_INSTANCE,    RMNET_INSTANCE,      DISABLE_XPORTS_SMUX_MHI     },
  { DS_TARGET_CSFB,             RMNET_USB_INSTANCE,    INVALID_INSTANCE,    DISABLE_XPORTS_SMD_SMUX_MHI },
  { DS_TARGET_SGLTE,            SMUX_INSTANCE,         RMNET_INSTANCE,      DISABLE_XPORTS_USB_MHI      },
  { DS_TARGET_SGLTE2,           SMUX_INSTANCE,         RMNET_INSTANCE,      DISABLE_XPORTS_USB_MHI      },
  { DS_TARGET_DSDA,             SMUX_INSTANCE,         RMNET_USB_INSTANCE,  DISABLE_XPORTS_SMD_MHI      },
  { DS_TARGET_DSDA2,            RMNET_USB_INSTANCE,    RMNET_USB_INSTANCE,  DISABLE_XPORTS_SMD_SMUX_MHI },
  { DS_TARGET_DSDA3,            SMUX_INSTANCE,         RMNET_INSTANCE,      DISABLE_XPORTS_USB_MHI      },
  { DS_TARGET_FUSION_8084_9X45, MHI_INSTANCE,          INVALID_INSTANCE,    DISABLE_XPORTS_SMD_USB_SMUX },
  { DS_TARGET_FUSION_8096_9X55, MHI_INSTANCE,          INVALID_INSTANCE,    DISABLE_XPORTS_SMD_USB_SMUX },
  { DS_TARGET_FUSION4_5_PCIE,   MHI_INSTANCE,          INVALID_INSTANCE,    DISABLE_XPORTS_SMD_USB_SMUX },
  { DS_TARGET_LE_MDM9X35,       RMNET_INSTANCE,        INVALID_INSTANCE,    DISABLE_XPORTS_USB_SMUX_MHI },
  { DS_TARGET_LE_MDM9X25,       RMNET_INSTANCE,        INVALID_INSTANCE,    DISABLE_XPORTS_USB_SMUX_MHI },
  { DS_TARGET_LE_MDM9X15,       RMNET_INSTANCE,        INVALID_INSTANCE,    DISABLE_XPORTS_USB_SMUX_MHI },
  { DS_TARGET_LE_LEGACY,        RMNET_INSTANCE,        INVALID_INSTANCE,    DISABLE_XPORTS_USB_SMUX_MHI },
};

#ifndef ARR_SIZE
  #define ARR_SIZE(x) (sizeof(x)/sizeof(x[0]))
#endif


/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION: qmi_client_helper_get_msm_instance
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Returns the QMUX instance to use for given MSM connection id

  @param[in]   None.
  @retval      None.
*/
static qmi_client_qmux_instance_type qmi_client_helper_get_msm_instance
(
  const char *cid,
  qmi_client_qmux_instance_type *instance_id
)
{
  qmi_client_qmux_instance_type  qmux_inst = QMI_CLIENT_QMUX_RMNET_INSTANCE_0;
  unsigned int i;

  static struct
  {
    const char *cid;
    qmi_client_qmux_instance_type inst_id;
  } msm_instance_id_map[] =
  {
    { QMI_PORT_RMNET_0,     QMI_CLIENT_QMUX_RMNET_INSTANCE_0 },
    { QMI_PORT_RMNET_1,     QMI_CLIENT_QMUX_RMNET_INSTANCE_1 },
    { QMI_PORT_RMNET_2,     QMI_CLIENT_QMUX_RMNET_INSTANCE_2 },
    { QMI_PORT_RMNET_3,     QMI_CLIENT_QMUX_RMNET_INSTANCE_3 },
    { QMI_PORT_RMNET_4,     QMI_CLIENT_QMUX_RMNET_INSTANCE_4 },
    { QMI_PORT_RMNET_5,     QMI_CLIENT_QMUX_RMNET_INSTANCE_5 },
    { QMI_PORT_RMNET_6,     QMI_CLIENT_QMUX_RMNET_INSTANCE_6 },
    { QMI_PORT_RMNET_7,     QMI_CLIENT_QMUX_RMNET_INSTANCE_7 },
    { QMI_PORT_RMNET_8,     QMI_CLIENT_QMUX_RMNET_INSTANCE_8 },
    { QMI_PORT_REV_RMNET_0, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_0 },
    { QMI_PORT_REV_RMNET_1, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_1 },
    { QMI_PORT_REV_RMNET_2, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_2 },
    { QMI_PORT_REV_RMNET_3, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_3 },
    { QMI_PORT_REV_RMNET_4, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_4 },
    { QMI_PORT_REV_RMNET_5, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_5 },
    { QMI_PORT_REV_RMNET_6, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_6 },
    { QMI_PORT_REV_RMNET_7, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_7 },
    { QMI_PORT_REV_RMNET_8, QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_8 },
  };

  if (!cid)
  {
    goto bail;
  }

  for (i = 0; i < ARR_SIZE(msm_instance_id_map); i++)
  {
    if (0 == strcmp(msm_instance_id_map[i].cid, cid))
    {
      qmux_inst = msm_instance_id_map[i].inst_id;
      break;
    }
  }

bail:
  if (instance_id)
  {
    *instance_id = qmux_inst;
  }

  return qmux_inst;
}

/*===========================================================================
  FUNCTION: qmi_helper_get_mdm_instance
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Returns the QMUX instance to use for given MDM connection id

  @param[in]   None.
  @retval      None.
*/
static qmi_client_qmux_instance_type qmi_client_helper_get_mdm_instance
(
  const char *cid,
  qmi_client_qmux_instance_type *instance_id
)
{
  qmi_client_qmux_instance_type  qmux_inst = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0;
  unsigned int i;

  static struct
  {
    const char *cid;
    qmi_client_qmux_instance_type inst_id;
  } mdm_instance_id_map[] =
  {
    { QMI_PORT_RMNET_USB_0,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0 },
    { QMI_PORT_RMNET_USB_1,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_1 },
    { QMI_PORT_RMNET_USB_2,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_2 },
    { QMI_PORT_RMNET_USB_3,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_3 },
    { QMI_PORT_RMNET_USB_4,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_4 },
    { QMI_PORT_RMNET_USB_5,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_5 },
    { QMI_PORT_RMNET_USB_6,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_6 },
    { QMI_PORT_RMNET_USB_7,     QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_7 },
    { QMI_PORT_REV_RMNET_USB_0, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_0 },
    { QMI_PORT_REV_RMNET_USB_1, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_1 },
    { QMI_PORT_REV_RMNET_USB_2, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_2 },
    { QMI_PORT_REV_RMNET_USB_3, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_3 },
    { QMI_PORT_REV_RMNET_USB_4, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_4 },
    { QMI_PORT_REV_RMNET_USB_5, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_5 },
    { QMI_PORT_REV_RMNET_USB_6, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_6 },
    { QMI_PORT_REV_RMNET_USB_7, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_7 },
    { QMI_PORT_REV_RMNET_USB_8, QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_8 },
  };

  if (!cid)
  {
    goto bail;
  }

  for (i = 0; i < ARR_SIZE(mdm_instance_id_map); i++)
  {
    if (0 == strcmp(mdm_instance_id_map[i].cid, cid))
    {
      qmux_inst = mdm_instance_id_map[i].inst_id;
      break;
    }
  }

bail:
  if (instance_id)
  {
    *instance_id = qmux_inst;
  }

  return qmux_inst;
}

/*===========================================================================
  FUNCTION: qmi_client_helper_get_xport_name
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Returns the name of the given QMUX xport

  @param[in]   None.
  @retval      None.
*/
static const char *qmi_client_helper_get_xport_name
(
  qmi_client_qmux_instance_type  xport
)
{
  const char *xport_name = "Unknown";
  unsigned int i;

  static struct
  {
    qmi_client_qmux_instance_type xport;
    const char *name;
  } xport_name_map[] =
  {
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_0,          "SMD0"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_1,          "SMD1"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_2,          "SMD2"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_3,          "SMD3"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_4,          "SMD4"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_5,          "SMD5"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_6,          "SMD6"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_7,          "SMD7"   },
    { QMI_CLIENT_QMUX_RMNET_INSTANCE_8,          "SMD8"   },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_0,      "R_SMD0" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_1,      "R_SMD1" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_2,      "R_SMD2" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_3,      "R_SMD3" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_4,      "R_SMD4" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_5,      "R_SMD5" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_6,      "R_SMD6" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_7,      "R_SMD7" },
    { QMI_CLIENT_QMUX_REV_RMNET_INSTANCE_8,      "R_SMD8" },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,      "USB0"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_1,      "USB1"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_2,      "USB2"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_3,      "USB3"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_4,      "USB4"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_5,      "USB5"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_6,      "USB6"   },
    { QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_7,      "USB7"   },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_0,  "R_USB0" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_1,  "R_USB1" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_2,  "R_USB2" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_3,  "R_USB3" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_4,  "R_USB4" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_5,  "R_USB5" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_6,  "R_USB6" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_7,  "R_USB7" },
    { QMI_CLIENT_QMUX_REV_RMNET_USB_INSTANCE_8,  "R_USB8" },
    { QMI_CLIENT_QMUX_RMNET_SMUX_INSTANCE_0,     "SMUX0"  },
    { QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0,      "MHI0"   },
    { QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_1,      "MHI1"   },
  };

  for (i = 0; i < ARR_SIZE(xport_name_map); i++)
  {
    if (xport_name_map[i].xport == xport)
    {
      xport_name = xport_name_map[i].name;
      break;
    }
  }

  return xport_name;
}

/*===========================================================================
  FUNCTION: qmi_client_helper_unreg_xports
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Unregister unnecessary QMUX transports with QCCI library based on target.

  @param[in]   None.
  @retval      None.
*/
static void qmi_client_helper_unreg_xports
(
  const qmi_client_qmux_instance_type  *disable_xports
)
{
  const qmi_client_qmux_instance_type  *xport = NULL;

  if (!disable_xports)
  {
    return;
  }

  for (xport = disable_xports; -1 != (int)(*xport); ++xport)
  {
    QMI_DEBUG_MSG_1("qmi_client_helper: Unreg QMUX transport %s",
                     qmi_client_helper_get_xport_name(*xport));
    qmi_cci_qmux_xport_unregister(*xport);
  }
}

/*===========================================================================
  FUNCTION: qmi_client_helper_reg_xport
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Register correct QMUX transports with QCCI library based on target.

  @param[in]   None.
  @retval      None.
*/
static void qmi_client_helper_reg_xport
(
  const char                     *cid,
  qmi_client_qmux_instance_type  *instance_id
)
{
  unsigned int i;
  ds_target_t target = ds_get_target();

  QMI_DEBUG_MSG_1("qmi_client_helper_reg_xport(): Target %s",
                  ds_get_target_str(target));

  for (i = 0; i < ARR_SIZE(xport_reg_arr); i++)
  {
    if (xport_reg_arr[i].target == target)
    {
      if (INVALID_INSTANCE != xport_reg_arr[i].qmux_inst)
      {
        qmi_client_qmux_instance_type  qmux_inst = -1;

        if (xport_reg_arr[i].qmux_inst->func)
        {
          qmux_inst = xport_reg_arr[i].qmux_inst->func(cid, instance_id);
        }
        else
        {
          qmux_inst = xport_reg_arr[i].qmux_inst->inst;
        }

        if (-1 != (int) qmux_inst)
        {
          QMI_DEBUG_MSG_2("qmi_client_helper: Reg QMUX transport %s for target %s",
                          qmi_client_helper_get_xport_name(qmux_inst),
                          ds_get_target_str(target));
          qmi_cci_qmux_xport_register(qmux_inst);
        }
      }

      if (INVALID_INSTANCE != xport_reg_arr[i].qmux_inst2)
      {
        qmi_client_qmux_instance_type  qmux_inst = -1;

        if (xport_reg_arr[i].qmux_inst2->func)
        {
          qmux_inst = xport_reg_arr[i].qmux_inst2->func(cid, instance_id);
        }
        else
        {
          qmux_inst = xport_reg_arr[i].qmux_inst2->inst;
        }

        if (-1 != (int) qmux_inst)
        {
          QMI_DEBUG_MSG_2("qmi_client_helper: Reg QMUX transport %s for target %s",
                          qmi_client_helper_get_xport_name(qmux_inst),
                          ds_get_target_str(target));
          qmi_cci_qmux_xport_register(qmux_inst);
        }
      }

      qmi_client_helper_unreg_xports(xport_reg_arr[i].disable_xports);
      break;
    }
  }

  if (i >= ARR_SIZE(xport_reg_arr))
  {
    QMI_DEBUG_MSG_2("qmi_client_helper: Using default QMUX transport %s for target %s",
                    qmi_client_helper_get_xport_name(QMI_CLIENT_QMUX_RMNET_INSTANCE_0),
                    ds_get_target_str(target));
    qmi_cci_qmux_xport_register(QMI_CLIENT_QMUX_RMNET_INSTANCE_0);
    qmi_client_helper_unreg_xports(DISABLE_XPORTS_USB_SMUX_MHI);
  }
}

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION: qmi_client_wds_init_instance
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Helper blocking function to lookup and initialize a connection with
  WDS service over a specific connection.

  @details
  Data services QMI clients need to communicate with WDS service over
  a specific connection. These connection string names are different over
  different transports. In newer architectures, binding to a mux-id is
  required. This helper routine performs all of these background activites
  so clients can easily use QMI services for multi-port scenarios.
  Other parameter definitions are present in qmi_client.h header file.

  @param[in]   dev_str            Connection ID string("rmnet_data1" etc)
  @param[in]   service_obj        Service object
  @param[in]   instance_id        Instance ID of the service.
                                  Use QMI_CLIENT_INSTANCE_ANY if there is
                                  no preference on the instance ID.
  @param[in]   ind_cb             Indication callback function
  @param[in]   ind_cb_data        Indication callback user-data
  @param[in]   os_params          OS-specific parameters. It can be a pointer
                                  to event object, or signal mask and TCB
  @param[in]   timeout            Time-out in milliseconds. 0 = no timeout
  @param[out]  user_handle        Handle used by the infrastructure to
                                  identify different clients.

  @retval  QMI_NO_ERR             Success
           QMI_INTERNAL_ERR       Invalid input parameters
           QMI_TIMEOUT_ERR        No service of the required instance_id
                                  was found in the time provided by timeout.

  @notes
  See qmi_client.h:qmi_client_init_instance() for other usage details of
  the API including blocking behavior, qmi_instance_id and timeout usage.
*/
qmi_client_error_type qmi_client_wds_init_instance
(
 const char                 *dev_str,
 qmi_idl_service_object_type service_obj,
 qmi_service_instance        instance_id,
 qmi_client_ind_cb           ind_cb,
 void                        *ind_cb_data,
 qmi_client_os_params        *os_params,
 uint32_t                    timeout,
 qmi_client_type             *user_handle
)
{
  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_connection_id_type conn_id;
  qmi_client_error_type rc;
  wds_bind_mux_data_port_req_msg_v01 req;
  wds_bind_mux_data_port_resp_msg_v01 resp;

  QMI_DEBUG_MSG_0("qmi_client_wds_init_instance(): Entry()");

  /* Register correct QMUX transport with QCCI based on target */
  qmi_client_helper_reg_xport(dev_str, &instance_id);

  /* Create client using QCCI libary - this is a blocking call */
  rc = qmi_client_init_instance(service_obj,
                                instance_id,
                                ind_cb,
                                ind_cb_data,
                                os_params,
                                timeout,
                                user_handle);
  if (QMI_NO_ERR != rc)
  {
    QMI_ERR_MSG_1("qmi_client_init_instance() error %d", rc);
    return rc;
  }

  /* Get EPID, MUX-ID information */
  if (QMI_CONN_ID_INVALID ==
      (conn_id = QMI_PLATFORM_DEV_NAME_TO_CONN_ID_EX(dev_str,
                                                     &ep_type,
                                                     &epid,
                                                     &mux_id)))
  {
    QMI_ERR_MSG_1("Dev to conn_id translation failed: dev %s", dev_str);
    return QMI_INTERNAL_ERR;
  }

  /* Bind to a epid/mux-id if mux-id-binding is supported */
  if (0 < mux_id)
  {
    memset (&req, 0, sizeof(req));
    memset (&resp, 0, sizeof(resp));

    req.ep_id_valid = (epid != -1);
    req.ep_id.ep_type = (data_ep_type_enum_v01) ep_type;
    req.ep_id.iface_id = (uint32_t) epid;

    req.mux_id_valid = (mux_id != -1);
    req.mux_id = (uint8_t) mux_id;

#ifdef FEATURE_QMI_IWLAN
    req.reversed_valid = req.reversed =
      (0 == strncmp(dev_str,
                    QMI_PLATFORM_REV_RMNET_DATA_PREFIX,
                    strlen(QMI_PLATFORM_REV_RMNET_DATA_PREFIX)));
#endif

    /* Send bind_req */
    rc = qmi_client_send_msg_sync(*user_handle,
                                  QMI_WDS_BIND_MUX_DATA_PORT_REQ_V01,
                                  (void *)&req,
                                  sizeof(wds_bind_mux_data_port_req_msg_v01),
                                  (void*)&resp,
                                  sizeof(wds_bind_mux_data_port_resp_msg_v01),
                                  timeout);
  }

  return rc;
}

/*===========================================================================
  FUNCTION: qmi_client_dfs_init_instance
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Helper blocking function to lookup and initialize a connection with
  DFS service over a specific connection.

  @details
  Data services QMI clients need to communicate with DFS service over
  a specific connection. These connection string names are different over
  different transports. In newer architectures, binding to a mux-id is
  required. This helper routine performs all of these background activites
  so clients can easily use QMI services for multi-port scenarios.
  Other parameter definitions are present in qmi_client.h header file.

  @param[in]   dev_str            Connection ID string("rmnet_data1" etc)
  @param[in]   service_obj        Service object
  @param[in]   instance_id        Instance ID of the service.
                                  Use QMI_CLIENT_INSTANCE_ANY if there is
                                  no preference on the instance ID.
  @param[in]   ind_cb             Indication callback function
  @param[in]   ind_cb_data        Indication callback user-data
  @param[in]   os_params          OS-specific parameters. It can be a pointer
                                  to event object, or signal mask and TCB
  @param[in]   timeout            Time-out in milliseconds. 0 = no timeout
  @param[out]  user_handle        Handle used by the infrastructure to
                                  identify different clients.

  @retval  QMI_NO_ERR             Success
           QMI_INTERNAL_ERR       Invalid input parameters
           QMI_TIMEOUT_ERR        No service of the required instance_id
                                  was found in the time provided by timeout.

  @notes
  See qmi_client.h:qmi_client_init_instance() for other usage details of
  the API including blocking behavior, qmi_instance_id and timeout usage.
*/
qmi_client_error_type qmi_client_dfs_init_instance
(
 const char                 *dev_str,
 qmi_idl_service_object_type service_obj,
 qmi_service_instance        instance_id,
 qmi_client_ind_cb           ind_cb,
 void                        *ind_cb_data,
 qmi_client_os_params        *os_params,
 uint32_t                    timeout,
 qmi_client_type             *user_handle
)
{

  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_connection_id_type conn_id;
  qmi_client_error_type rc;
  dfs_bind_client_req_msg_v01 req;
  dfs_bind_client_resp_msg_v01 resp;

  QMI_DEBUG_MSG_0("qmi_client_dfs_init_instance(): Entry()");

  /* Register correct QMUX transport with QCCI based on target */
  qmi_client_helper_reg_xport(dev_str, &instance_id);

  /* Create client using QCCI libary - this is a blocking call */
  rc = qmi_client_init_instance(service_obj,
                                instance_id,
                                ind_cb,
                                ind_cb_data,
                                os_params,
                                timeout,
                                user_handle);
  if (QMI_NO_ERR != rc)
  {
    QMI_ERR_MSG_1("qmi_client_init_instance() error %d", rc);
    return rc;
  }

  /* Get EPID, MUX-ID information */
  if (QMI_CONN_ID_INVALID ==
      (conn_id = QMI_PLATFORM_DEV_NAME_TO_CONN_ID_EX(dev_str,
                                                     &ep_type,
                                                     &epid,
                                                     &mux_id)))
  {
    QMI_ERR_MSG_1("Dev to conn_id translation failed: dev %s", dev_str);
    return QMI_INTERNAL_ERR;
  }

  /* Bind to a epid/mux-id if mux-id-binding is supported */
  if (0 < mux_id)
  {
    memset (&req, 0, sizeof(req));
    memset (&resp, 0, sizeof(resp));

    req.ep_id_valid = (epid != -1);
    req.ep_id.ep_type = (data_ep_type_enum_v01) ep_type;
    req.ep_id.iface_id = (uint32_t) epid;

    req.mux_id_valid = (mux_id != -1);
    req.mux_id = (uint8_t) mux_id;

    /* Send bind_req */
    rc = qmi_client_send_msg_sync(*user_handle,
                                  QMI_DFS_BIND_CLIENT_REQ_V01,
                                  (void *)&req,
                                  sizeof(req),
                                  (void*)&resp,
                                  sizeof(resp),
                                  timeout);
  }

  return rc;
}

/*===========================================================================
  FUNCTION: qmi_client_qos_init_instance
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Helper blocking function to lookup and initialize a connection with
  QOS service over a specific connection.

  @details
  Data services QMI clients need to communicate with QOS service over
  a specific connection. These connection string names are different over
  different transports. In newer architectures, binding to a mux-id is
  required. This helper routine performs all of these background activites
  so clients can easily use QMI services for multi-port scenarios.
  Other parameter definitions are present in qmi_client.h header file.

  @param[in]   dev_str            Connection ID string("rmnet_data1" etc)
  @param[in]   service_obj        Service object
  @param[in]   instance_id        Instance ID of the service.
                                  Use QMI_CLIENT_INSTANCE_ANY if there is
                                  no preference on the instance ID.
  @param[in]   ind_cb             Indication callback function
  @param[in]   ind_cb_data        Indication callback user-data
  @param[in]   os_params          OS-specific parameters. It can be a pointer
                                  to event object, or signal mask and TCB
  @param[in]   timeout            Time-out in milliseconds. 0 = no timeout
  @param[out]  user_handle        Handle used by the infrastructure to
                                  identify different clients.

  @retval  QMI_NO_ERR             Success
           QMI_INTERNAL_ERR       Invalid input parameters
           QMI_TIMEOUT_ERR        No service of the required instance_id
                                  was found in the time provided by timeout.

  @notes
  See qmi_client.h:qmi_client_init_instance() for other usage details of
  the API including blocking behavior, qmi_instance_id and timeout usage.
*/
qmi_client_error_type qmi_client_qos_init_instance
(
 const char                 *dev_str,
 qmi_idl_service_object_type service_obj,
 qmi_service_instance        instance_id,
 qmi_client_ind_cb           ind_cb,
 void                        *ind_cb_data,
 qmi_client_os_params        *os_params,
 uint32_t                    timeout,
 qmi_client_type             *user_handle
)
{
  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_connection_id_type conn_id;
  qmi_client_error_type rc;
  qos_bind_data_port_req_msg_v01 req;
  qos_bind_data_port_resp_msg_v01 resp;

  QMI_DEBUG_MSG_0("qmi_client_qos_init_instance(): Entry()");

  /* Register correct QMUX transport with QCCI based on target */
  qmi_client_helper_reg_xport(dev_str, &instance_id);

  /* Create client using QCCI libary - this is a blocking call */
  rc = qmi_client_init_instance(service_obj,
                                instance_id,
                                ind_cb,
                                ind_cb_data,
                                os_params,
                                timeout,
                                user_handle);
  if (QMI_NO_ERR != rc)
  {
    QMI_ERR_MSG_1("qmi_client_init_instance() error %d", rc);
    return rc;
  }

  /* Get EPID, MUX-ID information */
  if (QMI_CONN_ID_INVALID ==
      (conn_id = QMI_PLATFORM_DEV_NAME_TO_CONN_ID_EX(dev_str,
                                                     &ep_type,
                                                     &epid,
                                                     &mux_id)))
  {
    QMI_ERR_MSG_1("Dev to conn_id translation failed: dev %s", dev_str);
    return QMI_INTERNAL_ERR;
  }

  /* Bind to a epid/mux-id if mux-id-binding is supported */
  if (0 < mux_id)
  {
    memset (&req, 0, sizeof(req));
    memset (&resp, 0, sizeof(resp));

    req.ep_id_valid = (epid != -1);
    req.ep_id.ep_type = (data_ep_type_enum_v01) ep_type;
    req.ep_id.iface_id = (uint32_t) epid;

    req.mux_id_valid = (mux_id != -1);
    req.mux_id = (uint8_t) mux_id;

    /* Send bind_req */
    rc = qmi_client_send_msg_sync(*user_handle,
                                  QMI_QOS_BIND_DATA_PORT_REQ_V01,
                                  (void *)&req,
                                  sizeof(req),
                                  (void*)&resp,
                                  sizeof(resp),
                                  timeout);
  }

  return rc;
}


/*===========================================================================
  FUNCTION: qmi_client_ott_init_instance
===========================================================================*/
/**
  @ingroup qmi_client_helper

  @brief
  Helper blocking function to lookup and initialize a connection with
  OTT service over a specific connection.

  @details
  Data services QMI clients need to communicate with OTT service over
  a specific connection. These connection string names are different over
  different transports. In newer architectures, binding to a mux-id is
  required. This helper routine performs all of these background activites
  so clients can easily use QMI services for multi-port scenarios.
  Other parameter definitions are present in qmi_client.h header file.

  @param[in]   dev_str            Connection ID string("rmnet_data1" etc)
  @param[in]   service_obj        Service object
  @param[in]   instance_id        Instance ID of the service.
                                  Use QMI_CLIENT_INSTANCE_ANY if there is
                                  no preference on the instance ID.
  @param[in]   ind_cb             Indication callback function
  @param[in]   ind_cb_data        Indication callback user-data
  @param[in]   os_params          OS-specific parameters. It can be a pointer
                                  to event object, or signal mask and TCB
  @param[in]   timeout            Time-out in milliseconds. 0 = no timeout
  @param[out]  user_handle        Handle used by the infrastructure to
                                  identify different clients.

  @retval  QMI_NO_ERR             Success
           QMI_INTERNAL_ERR       Invalid input parameters
           QMI_TIMEOUT_ERR        No service of the required instance_id
                                  was found in the time provided by timeout.

  @notes
  See qmi_client.h:qmi_client_init_instance() for other usage details of
  the API including blocking behavior, qmi_instance_id and timeout usage.
*/
qmi_client_error_type qmi_client_ott_init_instance
(
 const char                 *dev_str,
 qmi_idl_service_object_type service_obj,
 qmi_service_instance        instance_id,
 qmi_client_ind_cb           ind_cb,
 void                        *ind_cb_data,
 qmi_client_os_params        *os_params,
 uint32_t                    timeout,
 qmi_client_type             *user_handle
)
{
  int ep_type = -1;
  int epid = -1;
  int mux_id = -1;
  qmi_client_error_type rc;

  QMI_DEBUG_MSG_0("qmi_client_ott_init_instance(): Entry()");

  /* Register correct QMUX transport with QCCI based on target */
  qmi_client_helper_reg_xport(dev_str, &instance_id);

  /* Create client using QCCI libary - this is a blocking call */
  rc = qmi_client_init_instance(service_obj,
                                instance_id,
                                ind_cb,
                                ind_cb_data,
                                os_params,
                                timeout,
                                user_handle);
  if (QMI_NO_ERR != rc)
  {
    QMI_ERR_MSG_1("qmi_client_init_instance() error %d", rc);
  }

  return rc;
}
