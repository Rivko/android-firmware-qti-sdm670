***********************************************************************
*        Copyright (c) 2016 Qualcomm Technologies, Inc.               *
*        All Rights Reserved.                                         *
*        Confidential and Proprietary - Qualcomm Technologies, Inc.   *
*                                                                     *
*                   CNE Profiles for IMS Configuration                *
***********************************************************************

1. Steps below need to be followed only if modem does not support
   "FR 33251: Carrier specific WQE profile management - 3GPP" and
   if profiles different from the ones in ROW folder need to be used.
   Without the FR, profiles in ROW folder will be used by default.
2. Browse to following location in the build:
    $.../vendor/qcom/proprietary/cne/Profiles/IMS/
2. CNE profiles are categorized per operator
3. Replace the files from operator of interest with the ones
   at $.../vendor/qcom/proprietary/cne/Profiles/IMS/ROW
4. Rebuild. This ensures that profiles in ROW folder are installed
   and used.

***********************************************************************