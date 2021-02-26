# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from sectools.common.utils.c_misc import load_data_from_file
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from . import parsegen_mgr
from .elf_mbn_v3_encdec import ElfMbnV3EncDec
from .elf_mbn_v3_licmngr import ElfMbnV3Licmngr
from .elf_mbn_v5 import ElfMbnV5
from .elf_mbn_v6 import ElfMbnV6
from .elf_mbn_v6_multi_image import ElfMbnV6MultiImage
from .elf_mbn_v6_persist_encr import ElfMbnV6PersistEncr
from .ewm import EwmV3
from .ewm import EwmV5
from .ewm import EwmV6
from .mbn import MbnV3
from .mbn import MbnV5
from .mbn import MbnV6

EwmClasses = {
    SECBOOT_VERSION_3_0: EwmV6,
    SECBOOT_VERSION_2_0: EwmV5,
    SECBOOT_VERSION_1_0: EwmV3
}

MbnClasses = {
    SECBOOT_VERSION_3_0: MbnV6,
    SECBOOT_VERSION_2_0: MbnV5,
    SECBOOT_VERSION_1_0: MbnV3
}


def lookup_parsegen_class(imageinfo):
    """ Use imageinfo elf properties to lookup the correct MBN parser """
    secboot_version = imageinfo.general_properties.secboot_version
    if imageinfo.image_type.file_type == "elf":
        # The imageinfo must have elf_properties attribute
        elf_properties = imageinfo.image_type.elf_properties
        if secboot_version == SECBOOT_VERSION_1_0:
            if (hasattr(elf_properties, "has_license_manager_segment") and
                    elf_properties.has_license_manager_segment):
                return ElfMbnV3Licmngr
            else:
                return ElfMbnV3EncDec
        elif secboot_version == SECBOOT_VERSION_2_0:
            return ElfMbnV5
        elif secboot_version == SECBOOT_VERSION_3_0:
            if (hasattr(elf_properties, "has_multi_image_segment") and
                    elf_properties.has_multi_image_segment):
                return ElfMbnV6MultiImage
            elif (hasattr(elf_properties, "persist_encr") and elf_properties.persist_encr):
                return ElfMbnV6PersistEncr
            else:
                return ElfMbnV6
        else:
            raise RuntimeError(
                "No ELF ParseGen found for Secure Boot version: %s" %
                secboot_version)
    elif imageinfo.image_type.file_type == "ewm":
        return EwmClasses[secboot_version]
    elif imageinfo.image_type.file_type == "mbn":
        return MbnClasses[secboot_version]


# API to query a parser
def get_parser(imageinfo, validating=False, signing=False, prefix_override=None,
               file_exists=True, parsegens=None, sign_attr=False):
    # Get source image path
    image_path = imageinfo.src_image.image_path
    image_name = os.path.basename(image_path)
    image_base, image_ext = os.path.splitext(image_name)

    # Get the parser
    mbn_parsegen_class = lookup_parsegen_class(imageinfo)

    if mbn_parsegen_class is None:
        parser = parsegen_mgr.get_parser(imageinfo.image_type.file_type)
    else:
        parser = parsegen_mgr.get_parser(mbn_parsegen_class.get_plugin_id())

    data = load_data_from_file(image_path) if file_exists else None
    return parser(data, imageinfo=imageinfo,
                  debug_dir=imageinfo.dest_image.debug_dir_parsegen,
                  debug_prefix=(prefix_override if
                                prefix_override is not None else image_base),
                  debug_suffix=image_ext,
                  validating=validating,
                  signing=signing,
                  parsegens=parsegens,
                  sign_attr=sign_attr)
