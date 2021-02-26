# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec
from .elf_v6_persist_encr import ElfV6PersistEncr
from .mbn_v6_persist_encr import MbnV6PersistEncr

class ElfMbnV6PersistEncr(ElfV6PersistEncr, MbnV6PersistEncr, SecParseGenBaseEncDec):
    """ ELF MBN Version 6 For SPU images to sign without decrypting """