# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec
from .elf_v6 import ElfV6
from .mbn_v6 import MbnV6


class ElfMbnV6(ElfV6, MbnV6, SecParseGenBaseEncDec):
    """ ELF MBN Version 6 Meta Data ParseGen """
