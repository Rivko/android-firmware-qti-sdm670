# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.versions.v6.parsegen_mbn import ParseGenMbnV6
from sectools.common.parsegen.mbn.versions.v6.utils import V6Utils
from sectools.common.utils.c_logging import logger
from sectools.features.isc.advanced_defines import multi_image_string
from ..elf_mbn_v3_base.mbn_v3_base import copy_header
from ..elf_mbn_v5.elf_v5 import ElfV5


class ElfV6(ElfV5):

    utils = V6Utils

    supports_multi_image = True

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV6, **kwargs):
        super(ElfV6, self).__init__(
            data, parsegen_mbn_class=parsegen_mbn_class, **kwargs)

    def get_multi_image_entry_data(self, authority):
        logger.debug("Getting data to hash for " + authority + " " +
                     multi_image_string() + " entry")
        try:
            if self.is_signed():
                return self.data_to_sign
            else:
                header = self._mbn_parsegen.header
                # Copy the MBN header so that masking doesn't alter original header
                header = copy_header(header)
                # Mask metadata and sig/cert sizes of other authority
                header = self.utils.mask_header_values(header, self.authority)
                return header.pack() + self._mbn_parsegen.code
        except:
            return None
