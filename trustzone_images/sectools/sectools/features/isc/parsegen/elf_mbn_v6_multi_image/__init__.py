# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec
from ..elf_mbn_v6.mbn_v6 import MbnV6
from .elf_v6_multi_image import ElfV6MultiImage


class ElfMbnV6MultiImage(ElfV6MultiImage, MbnV6, SecParseGenBaseEncDec):
    """ ELF MBN Version 6 Multi-image ParseGen """
