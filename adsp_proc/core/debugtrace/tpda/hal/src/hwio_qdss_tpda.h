#ifndef __HWIO_QDSS_TPDA_H__
#define __HWIO_QDSS_TPDA_H__
/*
===========================================================================
*/
/**
  @file hwio_qdss_tpda.h
  @brief Auto-generated HWIO interface include file.

*/
/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tpda/hal/src/hwio_qdss_tpda.h#1 $

  ===========================================================================
*/


#define QDSS_TPDA_BASE (QDSS_TPDA_BLOCK_BASE)

#define QDSS_TPDA_TPDA_REG_BASE                                    (QDSS_TPDA_BASE)

#define HWIO_QDSS_TPDA_CR_ADDR                                     (QDSS_TPDA_TPDA_REG_BASE      + 0x00000000)
#define HWIO_QDSS_TPDA_CR_RMSK                                     0xffffffff
#define HWIO_QDSS_TPDA_CR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_CR_ADDR, HWIO_QDSS_TPDA_CR_RMSK)
#define HWIO_QDSS_TPDA_CR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_CR_ADDR, m)
#define HWIO_QDSS_TPDA_CR_OUT(v)      \
        out_dword(HWIO_QDSS_TPDA_CR_ADDR,v)
#define HWIO_QDSS_TPDA_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_TPDA_CR_ADDR,m,v,HWIO_QDSS_TPDA_CR_IN)
#define HWIO_QDSS_TPDA_CR_RFU_BMSK                                 0xffc00000
#define HWIO_QDSS_TPDA_CR_RFU_SHFT                                       0x16
#define HWIO_QDSS_TPDA_CR_AVFREQEN_BMSK                              0x200000
#define HWIO_QDSS_TPDA_CR_AVFREQEN_SHFT                                  0x15
#define HWIO_QDSS_TPDA_CR_CMBCHANMODE_BMSK                           0x100000
#define HWIO_QDSS_TPDA_CR_CMBCHANMODE_SHFT                               0x14
#define HWIO_QDSS_TPDA_CR_MID_BMSK                                    0xfe000
#define HWIO_QDSS_TPDA_CR_MID_SHFT                                        0xd
#define HWIO_QDSS_TPDA_CR_ATID_BMSK                                    0x1fc0
#define HWIO_QDSS_TPDA_CR_ATID_SHFT                                       0x6
#define HWIO_QDSS_TPDA_CR_SRIE_BMSK                                      0x20
#define HWIO_QDSS_TPDA_CR_SRIE_SHFT                                       0x5
#define HWIO_QDSS_TPDA_CR_FLRIE_BMSK                                     0x10
#define HWIO_QDSS_TPDA_CR_FLRIE_SHFT                                      0x4
#define HWIO_QDSS_TPDA_CR_FRIE_BMSK                                       0x8
#define HWIO_QDSS_TPDA_CR_FRIE_SHFT                                       0x3
#define HWIO_QDSS_TPDA_CR_FREQTS_BMSK                                     0x4
#define HWIO_QDSS_TPDA_CR_FREQTS_SHFT                                     0x2
#define HWIO_QDSS_TPDA_CR_FREQREQ_BMSK                                    0x2
#define HWIO_QDSS_TPDA_CR_FREQREQ_SHFT                                    0x1
#define HWIO_QDSS_TPDA_CR_FLREQ_BMSK                                      0x1
#define HWIO_QDSS_TPDA_CR_FLREQ_SHFT                                      0x0

#define HWIO_QDSS_TPDA_Pn_CR_ADDR(n)                               (QDSS_TPDA_TPDA_REG_BASE      + 0x00000004 + 0x4 * (n))
#define HWIO_QDSS_TPDA_Pn_CR_RMSK                                  0xffffffff
#define HWIO_QDSS_TPDA_Pn_CR_MAXn                                           0
#define HWIO_QDSS_TPDA_Pn_CR_INI(n)        \
        in_dword_masked(HWIO_QDSS_TPDA_Pn_CR_ADDR(n), HWIO_QDSS_TPDA_Pn_CR_RMSK)
#define HWIO_QDSS_TPDA_Pn_CR_INMI(n,mask)    \
        in_dword_masked(HWIO_QDSS_TPDA_Pn_CR_ADDR(n), mask)
#define HWIO_QDSS_TPDA_Pn_CR_OUTI(n,val)    \
        out_dword(HWIO_QDSS_TPDA_Pn_CR_ADDR(n),val)
#define HWIO_QDSS_TPDA_Pn_CR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QDSS_TPDA_Pn_CR_ADDR(n),mask,val,HWIO_QDSS_TPDA_Pn_CR_INI(n))
#define HWIO_QDSS_TPDA_Pn_CR_RFU_BMSK                              0xfffff800
#define HWIO_QDSS_TPDA_Pn_CR_RFU_SHFT                                     0xb
#define HWIO_QDSS_TPDA_Pn_CR_IMPLDEFESIZE_BMSK                          0x600
#define HWIO_QDSS_TPDA_Pn_CR_IMPLDEFESIZE_SHFT                            0x9
#define HWIO_QDSS_TPDA_Pn_CR_DSBESIZE_BMSK                              0x100
#define HWIO_QDSS_TPDA_Pn_CR_DSBESIZE_SHFT                                0x8
#define HWIO_QDSS_TPDA_Pn_CR_CMBESIZE_BMSK                               0xc0
#define HWIO_QDSS_TPDA_Pn_CR_CMBESIZE_SHFT                                0x6
#define HWIO_QDSS_TPDA_Pn_CR_TCESIZE_BMSK                                0x20
#define HWIO_QDSS_TPDA_Pn_CR_TCESIZE_SHFT                                 0x5
#define HWIO_QDSS_TPDA_Pn_CR_BCESIZE_BMSK                                0x10
#define HWIO_QDSS_TPDA_Pn_CR_BCESIZE_SHFT                                 0x4
#define HWIO_QDSS_TPDA_Pn_CR_HOLD_BMSK                                    0xe
#define HWIO_QDSS_TPDA_Pn_CR_HOLD_SHFT                                    0x1
#define HWIO_QDSS_TPDA_Pn_CR_E_BMSK                                       0x1
#define HWIO_QDSS_TPDA_Pn_CR_E_SHFT                                       0x0

#define HWIO_QDSS_TPDA_FPID_CR_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000084)
#define HWIO_QDSS_TPDA_FPID_CR_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_FPID_CR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_FPID_CR_ADDR, HWIO_QDSS_TPDA_FPID_CR_RMSK)
#define HWIO_QDSS_TPDA_FPID_CR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_FPID_CR_ADDR, m)
#define HWIO_QDSS_TPDA_FPID_CR_OUT(v)      \
        out_dword(HWIO_QDSS_TPDA_FPID_CR_ADDR,v)
#define HWIO_QDSS_TPDA_FPID_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_TPDA_FPID_CR_ADDR,m,v,HWIO_QDSS_TPDA_FPID_CR_IN)
#define HWIO_QDSS_TPDA_FPID_CR_CID_BMSK                            0xffff0000
#define HWIO_QDSS_TPDA_FPID_CR_CID_SHFT                                  0x10
#define HWIO_QDSS_TPDA_FPID_CR_MID_BMSK                                0xffff
#define HWIO_QDSS_TPDA_FPID_CR_MID_SHFT                                   0x0

#define HWIO_QDSS_TPDA_FREQREQ_VAL_ADDR                            (QDSS_TPDA_TPDA_REG_BASE      + 0x00000088)
#define HWIO_QDSS_TPDA_FREQREQ_VAL_RMSK                            0xffffffff
#define HWIO_QDSS_TPDA_FREQREQ_VAL_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_FREQREQ_VAL_ADDR, HWIO_QDSS_TPDA_FREQREQ_VAL_RMSK)
#define HWIO_QDSS_TPDA_FREQREQ_VAL_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_FREQREQ_VAL_ADDR, m)
#define HWIO_QDSS_TPDA_FREQREQ_VAL_OUT(v)      \
        out_dword(HWIO_QDSS_TPDA_FREQREQ_VAL_ADDR,v)
#define HWIO_QDSS_TPDA_FREQREQ_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_TPDA_FREQREQ_VAL_ADDR,m,v,HWIO_QDSS_TPDA_FREQREQ_VAL_IN)
#define HWIO_QDSS_TPDA_FREQREQ_VAL_VAL_BMSK                        0xffffffff
#define HWIO_QDSS_TPDA_FREQREQ_VAL_VAL_SHFT                               0x0

#define HWIO_QDSS_TPDA_SYNCR_ADDR                                  (QDSS_TPDA_TPDA_REG_BASE      + 0x0000008c)
#define HWIO_QDSS_TPDA_SYNCR_RMSK                                  0xffffffff
#define HWIO_QDSS_TPDA_SYNCR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_SYNCR_ADDR, HWIO_QDSS_TPDA_SYNCR_RMSK)
#define HWIO_QDSS_TPDA_SYNCR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_SYNCR_ADDR, m)
#define HWIO_QDSS_TPDA_SYNCR_OUT(v)      \
        out_dword(HWIO_QDSS_TPDA_SYNCR_ADDR,v)
#define HWIO_QDSS_TPDA_SYNCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_TPDA_SYNCR_ADDR,m,v,HWIO_QDSS_TPDA_SYNCR_IN)
#define HWIO_QDSS_TPDA_SYNCR_RFU_BMSK                              0xffffe000
#define HWIO_QDSS_TPDA_SYNCR_RFU_SHFT                                     0xd
#define HWIO_QDSS_TPDA_SYNCR_MODE_BMSK                                 0x1000
#define HWIO_QDSS_TPDA_SYNCR_MODE_SHFT                                    0xc
#define HWIO_QDSS_TPDA_SYNCR_COUNT_BMSK                                 0xfff
#define HWIO_QDSS_TPDA_SYNCR_COUNT_SHFT                                   0x0

#define HWIO_QDSS_TPDA_FLUSH_CR_ADDR                               (QDSS_TPDA_TPDA_REG_BASE      + 0x00000090)
#define HWIO_QDSS_TPDA_FLUSH_CR_RMSK                               0xffffffff
#define HWIO_QDSS_TPDA_FLUSH_CR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_FLUSH_CR_ADDR, HWIO_QDSS_TPDA_FLUSH_CR_RMSK)
#define HWIO_QDSS_TPDA_FLUSH_CR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_FLUSH_CR_ADDR, m)
#define HWIO_QDSS_TPDA_FLUSH_CR_OUT(v)      \
        out_dword(HWIO_QDSS_TPDA_FLUSH_CR_ADDR,v)
#define HWIO_QDSS_TPDA_FLUSH_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_TPDA_FLUSH_CR_ADDR,m,v,HWIO_QDSS_TPDA_FLUSH_CR_IN)
#define HWIO_QDSS_TPDA_FLUSH_CR_REQ_BMSK                           0xffffffff
#define HWIO_QDSS_TPDA_FLUSH_CR_REQ_SHFT                                  0x0

#define HWIO_QDSS_TPDA_FLUSH_SR_ADDR                               (QDSS_TPDA_TPDA_REG_BASE      + 0x00000094)
#define HWIO_QDSS_TPDA_FLUSH_SR_RMSK                               0xffffffff
#define HWIO_QDSS_TPDA_FLUSH_SR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_FLUSH_SR_ADDR, HWIO_QDSS_TPDA_FLUSH_SR_RMSK)
#define HWIO_QDSS_TPDA_FLUSH_SR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_FLUSH_SR_ADDR, m)
#define HWIO_QDSS_TPDA_FLUSH_SR_FLUSHSTAT_BMSK                     0xffffffff
#define HWIO_QDSS_TPDA_FLUSH_SR_FLUSHSTAT_SHFT                            0x0

#define HWIO_QDSS_TPDA_FLUSH_ERR_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000098)
#define HWIO_QDSS_TPDA_FLUSH_ERR_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_FLUSH_ERR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_FLUSH_ERR_ADDR, HWIO_QDSS_TPDA_FLUSH_ERR_RMSK)
#define HWIO_QDSS_TPDA_FLUSH_ERR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_FLUSH_ERR_ADDR, m)
#define HWIO_QDSS_TPDA_FLUSH_ERR_SYNDROME_BMSK                     0xffffffff
#define HWIO_QDSS_TPDA_FLUSH_ERR_SYNDROME_SHFT                            0x0

#define HWIO_QDSS_TPDA_ITCTL_ADDR                                  (QDSS_TPDA_TPDA_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_TPDA_ITCTL_RMSK                                  0xffffffff
#define HWIO_QDSS_TPDA_ITCTL_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_ITCTL_ADDR, HWIO_QDSS_TPDA_ITCTL_RMSK)
#define HWIO_QDSS_TPDA_ITCTL_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_ITCTL_ADDR, m)
#define HWIO_QDSS_TPDA_ITCTL_RFU_BMSK                              0xffffffff
#define HWIO_QDSS_TPDA_ITCTL_RFU_SHFT                                     0x0

#define HWIO_QDSS_TPDA_CLAIMSET_ADDR                               (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_TPDA_CLAIMSET_RMSK                               0xffffffff
#define HWIO_QDSS_TPDA_CLAIMSET_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_CLAIMSET_ADDR, HWIO_QDSS_TPDA_CLAIMSET_RMSK)
#define HWIO_QDSS_TPDA_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_CLAIMSET_ADDR, m)
#define HWIO_QDSS_TPDA_CLAIMSET_RFU_BMSK                           0xffffffff
#define HWIO_QDSS_TPDA_CLAIMSET_RFU_SHFT                                  0x0

#define HWIO_QDSS_TPDA_CLAIMCLR_ADDR                               (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_TPDA_CLAIMCLR_RMSK                               0xffffffff
#define HWIO_QDSS_TPDA_CLAIMCLR_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_CLAIMCLR_ADDR, HWIO_QDSS_TPDA_CLAIMCLR_RMSK)
#define HWIO_QDSS_TPDA_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_CLAIMCLR_ADDR, m)
#define HWIO_QDSS_TPDA_CLAIMCLR_RFU_BMSK                           0xffffffff
#define HWIO_QDSS_TPDA_CLAIMCLR_RFU_SHFT                                  0x0

#define HWIO_QDSS_TPDA_DEVAFF0_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fa8)
#define HWIO_QDSS_TPDA_DEVAFF0_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_DEVAFF0_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVAFF0_ADDR, HWIO_QDSS_TPDA_DEVAFF0_RMSK)
#define HWIO_QDSS_TPDA_DEVAFF0_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVAFF0_ADDR, m)
#define HWIO_QDSS_TPDA_DEVAFF0_AFFINITY_BMSK                       0xffffffff
#define HWIO_QDSS_TPDA_DEVAFF0_AFFINITY_SHFT                              0x0

#define HWIO_QDSS_TPDA_DEVAFF1_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fac)
#define HWIO_QDSS_TPDA_DEVAFF1_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_DEVAFF1_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVAFF1_ADDR, HWIO_QDSS_TPDA_DEVAFF1_RMSK)
#define HWIO_QDSS_TPDA_DEVAFF1_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVAFF1_ADDR, m)
#define HWIO_QDSS_TPDA_DEVAFF1_AFFINITY_BMSK                       0xffffffff
#define HWIO_QDSS_TPDA_DEVAFF1_AFFINITY_SHFT                              0x0

#define HWIO_QDSS_TPDA_LOCKACCESS_ADDR                             (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_TPDA_LOCKACCESS_RMSK                             0xffffffff
#define HWIO_QDSS_TPDA_LOCKACCESS_OUT(v)      \
        out_dword(HWIO_QDSS_TPDA_LOCKACCESS_ADDR,v)
#define HWIO_QDSS_TPDA_LOCKACCESS_LOCKACCESS_BMSK                  0xffffffff
#define HWIO_QDSS_TPDA_LOCKACCESS_LOCKACCESS_SHFT                         0x0

#define HWIO_QDSS_TPDA_LOCKSTATUS_ADDR                             (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_TPDA_LOCKSTATUS_RMSK                             0xffffffff
#define HWIO_QDSS_TPDA_LOCKSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_LOCKSTATUS_ADDR, HWIO_QDSS_TPDA_LOCKSTATUS_RMSK)
#define HWIO_QDSS_TPDA_LOCKSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_LOCKSTATUS_ADDR, m)
#define HWIO_QDSS_TPDA_LOCKSTATUS_RFU_BMSK                         0xfffffff8
#define HWIO_QDSS_TPDA_LOCKSTATUS_RFU_SHFT                                0x3
#define HWIO_QDSS_TPDA_LOCKSTATUS_FIELD_8_BIT_LOCK_BMSK                   0x4
#define HWIO_QDSS_TPDA_LOCKSTATUS_FIELD_8_BIT_LOCK_SHFT                   0x2
#define HWIO_QDSS_TPDA_LOCKSTATUS_ACCESS_BMSK                             0x2
#define HWIO_QDSS_TPDA_LOCKSTATUS_ACCESS_SHFT                             0x1
#define HWIO_QDSS_TPDA_LOCKSTATUS_LOCK_CONTROL_BMSK                       0x1
#define HWIO_QDSS_TPDA_LOCKSTATUS_LOCK_CONTROL_SHFT                       0x0

#define HWIO_QDSS_TPDA_AUTHSTATUS_ADDR                             (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_TPDA_AUTHSTATUS_RMSK                             0xffffffff
#define HWIO_QDSS_TPDA_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_AUTHSTATUS_ADDR, HWIO_QDSS_TPDA_AUTHSTATUS_RMSK)
#define HWIO_QDSS_TPDA_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_AUTHSTATUS_ADDR, m)
#define HWIO_QDSS_TPDA_AUTHSTATUS_RFU_BMSK                         0xffffffff
#define HWIO_QDSS_TPDA_AUTHSTATUS_RFU_SHFT                                0x0

#define HWIO_QDSS_TPDA_DEVARCH_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fbc)
#define HWIO_QDSS_TPDA_DEVARCH_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_DEVARCH_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVARCH_ADDR, HWIO_QDSS_TPDA_DEVARCH_RMSK)
#define HWIO_QDSS_TPDA_DEVARCH_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVARCH_ADDR, m)
#define HWIO_QDSS_TPDA_DEVARCH_ARCHITECT_BMSK                      0xffe00000
#define HWIO_QDSS_TPDA_DEVARCH_ARCHITECT_SHFT                            0x15
#define HWIO_QDSS_TPDA_DEVARCH_PRESENT_BMSK                          0x100000
#define HWIO_QDSS_TPDA_DEVARCH_PRESENT_SHFT                              0x14
#define HWIO_QDSS_TPDA_DEVARCH_REVISION_BMSK                          0xf0000
#define HWIO_QDSS_TPDA_DEVARCH_REVISION_SHFT                             0x10
#define HWIO_QDSS_TPDA_DEVARCH_ARCHID_BMSK                             0xffff
#define HWIO_QDSS_TPDA_DEVARCH_ARCHID_SHFT                                0x0

#define HWIO_QDSS_TPDA_DEVID2_ADDR                                 (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fc0)
#define HWIO_QDSS_TPDA_DEVID2_RMSK                                 0xffffffff
#define HWIO_QDSS_TPDA_DEVID2_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVID2_ADDR, HWIO_QDSS_TPDA_DEVID2_RMSK)
#define HWIO_QDSS_TPDA_DEVID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVID2_ADDR, m)
#define HWIO_QDSS_TPDA_DEVID2_DEVID2_BMSK                          0xffffffff
#define HWIO_QDSS_TPDA_DEVID2_DEVID2_SHFT                                 0x0

#define HWIO_QDSS_TPDA_DEVID1_ADDR                                 (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fc4)
#define HWIO_QDSS_TPDA_DEVID1_RMSK                                 0xffffffff
#define HWIO_QDSS_TPDA_DEVID1_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVID1_ADDR, HWIO_QDSS_TPDA_DEVID1_RMSK)
#define HWIO_QDSS_TPDA_DEVID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVID1_ADDR, m)
#define HWIO_QDSS_TPDA_DEVID1_DEVID1_BMSK                          0xffffffff
#define HWIO_QDSS_TPDA_DEVID1_DEVID1_SHFT                                 0x0

#define HWIO_QDSS_TPDA_DEVID_ADDR                                  (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_TPDA_DEVID_RMSK                                  0xffffffff
#define HWIO_QDSS_TPDA_DEVID_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVID_ADDR, HWIO_QDSS_TPDA_DEVID_RMSK)
#define HWIO_QDSS_TPDA_DEVID_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVID_ADDR, m)
#define HWIO_QDSS_TPDA_DEVID_DEVID_BMSK                            0xffffffff
#define HWIO_QDSS_TPDA_DEVID_DEVID_SHFT                                   0x0

#define HWIO_QDSS_TPDA_DEVTYPE_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_TPDA_DEVTYPE_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_DEVTYPE_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_DEVTYPE_ADDR, HWIO_QDSS_TPDA_DEVTYPE_RMSK)
#define HWIO_QDSS_TPDA_DEVTYPE_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_DEVTYPE_ADDR, m)
#define HWIO_QDSS_TPDA_DEVTYPE_RFU_BMSK                            0xffffff00
#define HWIO_QDSS_TPDA_DEVTYPE_RFU_SHFT                                   0x8
#define HWIO_QDSS_TPDA_DEVTYPE_SUBTYPE_BMSK                              0xf0
#define HWIO_QDSS_TPDA_DEVTYPE_SUBTYPE_SHFT                               0x4
#define HWIO_QDSS_TPDA_DEVTYPE_MAJTYPE_BMSK                               0xf
#define HWIO_QDSS_TPDA_DEVTYPE_MAJTYPE_SHFT                               0x0

#define HWIO_QDSS_TPDA_PERIPHID4_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_TPDA_PERIPHID4_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID4_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID4_ADDR, HWIO_QDSS_TPDA_PERIPHID4_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID4_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID4_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID4_RFU_BMSK                          0xffffff00
#define HWIO_QDSS_TPDA_PERIPHID4_RFU_SHFT                                 0x8
#define HWIO_QDSS_TPDA_PERIPHID4_FIELD_4KB_COUNT_BMSK                    0xf0
#define HWIO_QDSS_TPDA_PERIPHID4_FIELD_4KB_COUNT_SHFT                     0x4
#define HWIO_QDSS_TPDA_PERIPHID4_JEP106_CONTINUATION_BMSK                 0xf
#define HWIO_QDSS_TPDA_PERIPHID4_JEP106_CONTINUATION_SHFT                 0x0

#define HWIO_QDSS_TPDA_PERIPHID5_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fd4)
#define HWIO_QDSS_TPDA_PERIPHID5_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID5_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID5_ADDR, HWIO_QDSS_TPDA_PERIPHID5_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID5_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID5_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID5_RFU_BMSK                          0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID5_RFU_SHFT                                 0x0

#define HWIO_QDSS_TPDA_PERIPHID6_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fd8)
#define HWIO_QDSS_TPDA_PERIPHID6_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID6_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID6_ADDR, HWIO_QDSS_TPDA_PERIPHID6_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID6_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID6_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID6_RFU_BMSK                          0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID6_RFU_SHFT                                 0x0

#define HWIO_QDSS_TPDA_PERIPHID7_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fdc)
#define HWIO_QDSS_TPDA_PERIPHID7_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID7_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID7_ADDR, HWIO_QDSS_TPDA_PERIPHID7_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID7_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID7_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID7_RFU_BMSK                          0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID7_RFU_SHFT                                 0x0

#define HWIO_QDSS_TPDA_PERIPHID0_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_TPDA_PERIPHID0_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID0_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID0_ADDR, HWIO_QDSS_TPDA_PERIPHID0_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID0_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID0_RFU_BMSK                          0xffffff00
#define HWIO_QDSS_TPDA_PERIPHID0_RFU_SHFT                                 0x8
#define HWIO_QDSS_TPDA_PERIPHID0_PARTNUM_BMSK                            0xff
#define HWIO_QDSS_TPDA_PERIPHID0_PARTNUM_SHFT                             0x0

#define HWIO_QDSS_TPDA_PERIPHID1_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_TPDA_PERIPHID1_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID1_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID1_ADDR, HWIO_QDSS_TPDA_PERIPHID1_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID1_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID1_RFU_BMSK                          0xffffff00
#define HWIO_QDSS_TPDA_PERIPHID1_RFU_SHFT                                 0x8
#define HWIO_QDSS_TPDA_PERIPHID1_JEP106_IDENTITY_3_0_BMSK                0xf0
#define HWIO_QDSS_TPDA_PERIPHID1_JEP106_IDENTITY_3_0_SHFT                 0x4
#define HWIO_QDSS_TPDA_PERIPHID1_PARTNUM_BMSK                             0xf
#define HWIO_QDSS_TPDA_PERIPHID1_PARTNUM_SHFT                             0x0

#define HWIO_QDSS_TPDA_PERIPHID2_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_TPDA_PERIPHID2_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID2_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID2_ADDR, HWIO_QDSS_TPDA_PERIPHID2_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID2_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID2_RFU_BMSK                          0xffffff00
#define HWIO_QDSS_TPDA_PERIPHID2_RFU_SHFT                                 0x8
#define HWIO_QDSS_TPDA_PERIPHID2_MAJREV_BMSK                             0xf0
#define HWIO_QDSS_TPDA_PERIPHID2_MAJREV_SHFT                              0x4
#define HWIO_QDSS_TPDA_PERIPHID2_JEDEC_BMSK                               0x8
#define HWIO_QDSS_TPDA_PERIPHID2_JEDEC_SHFT                               0x3
#define HWIO_QDSS_TPDA_PERIPHID2_JEP106_IDENTITY_6_4_BMSK                 0x7
#define HWIO_QDSS_TPDA_PERIPHID2_JEP106_IDENTITY_6_4_SHFT                 0x0

#define HWIO_QDSS_TPDA_PERIPHID3_ADDR                              (QDSS_TPDA_TPDA_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_TPDA_PERIPHID3_RMSK                              0xffffffff
#define HWIO_QDSS_TPDA_PERIPHID3_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID3_ADDR, HWIO_QDSS_TPDA_PERIPHID3_RMSK)
#define HWIO_QDSS_TPDA_PERIPHID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_PERIPHID3_ADDR, m)
#define HWIO_QDSS_TPDA_PERIPHID3_RFU_BMSK                          0xffffff00
#define HWIO_QDSS_TPDA_PERIPHID3_RFU_SHFT                                 0x8
#define HWIO_QDSS_TPDA_PERIPHID3_REV_AND_BMSK                            0xf0
#define HWIO_QDSS_TPDA_PERIPHID3_REV_AND_SHFT                             0x4
#define HWIO_QDSS_TPDA_PERIPHID3_CUSTOMER_MODIFIED_BMSK                   0xf
#define HWIO_QDSS_TPDA_PERIPHID3_CUSTOMER_MODIFIED_SHFT                   0x0

#define HWIO_QDSS_TPDA_COMPID0_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_TPDA_COMPID0_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_COMPID0_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID0_ADDR, HWIO_QDSS_TPDA_COMPID0_RMSK)
#define HWIO_QDSS_TPDA_COMPID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID0_ADDR, m)
#define HWIO_QDSS_TPDA_COMPID0_RFU_BMSK                            0xffffff00
#define HWIO_QDSS_TPDA_COMPID0_RFU_SHFT                                   0x8
#define HWIO_QDSS_TPDA_COMPID0_PREAMBLE_7_0_BMSK                         0xff
#define HWIO_QDSS_TPDA_COMPID0_PREAMBLE_7_0_SHFT                          0x0

#define HWIO_QDSS_TPDA_COMPID1_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_TPDA_COMPID1_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_COMPID1_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID1_ADDR, HWIO_QDSS_TPDA_COMPID1_RMSK)
#define HWIO_QDSS_TPDA_COMPID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID1_ADDR, m)
#define HWIO_QDSS_TPDA_COMPID1_RFU_BMSK                            0xffffff00
#define HWIO_QDSS_TPDA_COMPID1_RFU_SHFT                                   0x8
#define HWIO_QDSS_TPDA_COMPID1_PREAMBLE_15_12_BMSK                       0xf0
#define HWIO_QDSS_TPDA_COMPID1_PREAMBLE_15_12_SHFT                        0x4
#define HWIO_QDSS_TPDA_COMPID1_PREAMBLE_11_8_BMSK                         0xf
#define HWIO_QDSS_TPDA_COMPID1_PREAMBLE_11_8_SHFT                         0x0

#define HWIO_QDSS_TPDA_COMPID2_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_TPDA_COMPID2_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_COMPID2_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID2_ADDR, HWIO_QDSS_TPDA_COMPID2_RMSK)
#define HWIO_QDSS_TPDA_COMPID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID2_ADDR, m)
#define HWIO_QDSS_TPDA_COMPID2_RFU_BMSK                            0xffffff00
#define HWIO_QDSS_TPDA_COMPID2_RFU_SHFT                                   0x8
#define HWIO_QDSS_TPDA_COMPID2_PREAMBLE_23_16_BMSK                       0xff
#define HWIO_QDSS_TPDA_COMPID2_PREAMBLE_23_16_SHFT                        0x0

#define HWIO_QDSS_TPDA_COMPID3_ADDR                                (QDSS_TPDA_TPDA_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_TPDA_COMPID3_RMSK                                0xffffffff
#define HWIO_QDSS_TPDA_COMPID3_IN          \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID3_ADDR, HWIO_QDSS_TPDA_COMPID3_RMSK)
#define HWIO_QDSS_TPDA_COMPID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_TPDA_COMPID3_ADDR, m)
#define HWIO_QDSS_TPDA_COMPID3_RFU_BMSK                            0xffffff00
#define HWIO_QDSS_TPDA_COMPID3_RFU_SHFT                                   0x8
#define HWIO_QDSS_TPDA_COMPID3_PREAMBLE_31_24_BMSK                       0xff
#define HWIO_QDSS_TPDA_COMPID3_PREAMBLE_31_24_SHFT                        0x0


#endif /* __HWIO_QDSS_TPDA_H__ */
