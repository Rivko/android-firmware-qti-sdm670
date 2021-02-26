# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.core.plugin import SecPluginMgr


# API to query a parser
def get_parser(imageinfo):

    # Import within this method
    import os
    from sectools.common.utils.c_misc import load_data_from_file

    # Get source image path
    image_path = imageinfo.src_image.image_path
    image_name = os.path.basename(image_path)
    image_base, image_ext = os.path.splitext(image_name)

    # Get the parser
    parser = parsegen_mgr.get_parser(imageinfo.image_type.file_type)
    return parser(load_data_from_file(image_path), imageinfo=imageinfo,
                  debug_dir=imageinfo.dest_image.debug_dir_parsegen,
                  debug_prefix=image_base,
                  debug_suffix=image_ext)


class ParsegenMgr(SecPluginMgr):

    def get_parser(self, file_type):
        parsegens = self.get_map()
        if file_type not in parsegens:
            raise RuntimeError('Parser unavailable for file type: ' + str(file_type) + '\n'
                               '    ' + 'Available file types are: ' + str(parsegens.keys()))
        return parsegens[file_type]

parsegen_mgr = ParsegenMgr()


from sectools.features.isc.parsegen import bin
from sectools.features.isc.parsegen import elf
from sectools.features.isc.parsegen import ewm
from sectools.features.isc.parsegen import mbn
from sectools.features.isc.parsegen import aost_lic
from sectools.features.isc.parsegen import aost_bin_lic
from sectools.features.isc.parsegen import pmbl
from sectools.features.isc.parsegen import hash_to_sign
from sectools.features.isc.parsegen import bin_to_sign