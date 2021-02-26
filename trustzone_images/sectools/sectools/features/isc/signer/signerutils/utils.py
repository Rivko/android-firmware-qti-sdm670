# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


def get_sw_id(signing_attributes):
    if (hasattr(signing_attributes, "anti_rollback_version") and
            signing_attributes.anti_rollback_version is not None):
        return int("{0:#0{1}x}".format(
            int(signing_attributes.anti_rollback_version, 16), 10) + "{0:0{1}x}"
                   .format(int(signing_attributes.sw_id, 16), 8), 16)
    return int(signing_attributes.sw_id, 16)
