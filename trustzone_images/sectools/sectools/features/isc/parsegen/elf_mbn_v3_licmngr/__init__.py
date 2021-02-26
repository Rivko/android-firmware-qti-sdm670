# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec

from .elf_v3_licmngr import ElfV3Licmngr
from ..elf_mbn_v3_encdec.mbn_v3_encdec import MbnV3EncDec


class ElfMbnV3Licmngr(ElfV3Licmngr, MbnV3EncDec, SecParseGenBaseEncDec):
    """ ELF MBN Version 3 License Manager ParseGen """
