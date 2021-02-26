# ===============================================================================
#
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.utils import extract_segment
from sectools.features.isc.encryption_service.ssd.encryption_parameters import SSD_MAGIC
from sectools.features.isc.encryption_service.unified.encryption_params_info_hdr import \
    EncryptionParamsInfoHdr_1_0
from sectools.features.isc.encryption_service.unified.encryption_params_info_hdr import \
    EncryptionParamsInfoHdr_2_0
from ..v3.utils import V3Utils


class V3EncDecUtils(V3Utils):

    @classmethod
    def extract_encryption_params(cls, data):
        for magic in [EncryptionParamsInfoHdr_1_0.byte_sequence(),
                      EncryptionParamsInfoHdr_2_0.byte_sequence(), SSD_MAGIC]:
            magic_index = data.find(magic)
            if magic_index != -1:
                offset = magic_index
                size = len(data) - offset
                return extract_segment(data, offset, size)
        return data, ""
