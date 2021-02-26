# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base import SecParseGenBase

from .mbn_v3_base import MbnV3Base
from .elf_v3_base import ElfV3Base


class ElfMbnV3Base(ElfV3Base, MbnV3Base, SecParseGenBase):
    """ ELF MBN Version 3 Base ParseGen """
