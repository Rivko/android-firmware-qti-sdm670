# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Feb 21, 2014

@author: hraghav
'''

import os
import re

from meta_build_stager import MetaBuildStager
from sectools.common.utils.c_logging import logger
from sectools.features.isc.imageinfo import DestImagePath, ImagePath
from sectools.features.isc.stager.meta_build_stager import MetaError


class MetaBuildTmpStager(MetaBuildStager):

    # Replacement tags in meta-build paths for images.
    TAG_ATTR = 'ATTR'
    TAG_FILE_TYPE = 'FILE_TYPE'
    TAG_BUILD_PATH = 'BUILD_PATH'
    TAG_VAR = 'VAR'

    # Replacement tag for replacing with the path of the meta-build
    REPL_META_PATH = '$(META_BUILD)'

    def __init__(self, meta_build_path, config_dir_obj, img_config_parser, parsegen_config_parser, sign_id_list=[]):
        self.img_config_parser = img_config_parser
        self.parsegen_config_parser = parsegen_config_parser
        self.chipset = self.img_config_parser.chipset
        MetaBuildStager.__init__(self, meta_build_path, config_dir_obj, parsegen_config_parser, sign_id_list)

    def get_image_info_from_meta(self, meta_info):
        # Cache dicts
        build_paths = dict()
        attrs = dict()
        file_types = dict()

        for sign_id in self.img_config_parser.sign_id_list:
            image = self.img_config_parser.get_config_for_sign_id(sign_id)
            image_path = image.meta_build_location

            if not image_path:
                continue

            try:
                logger.debug('Searching metabuild for ' + sign_id)

                # Replace any tags
                re_match = re.match('\$\((.*?)\)', image_path)
                if re_match:
                    tags = re_match.group(1)
                    tags_dict = {}
                    for tag in tags.split(','):
                        tag = tag.strip().split(':')
                        tags_dict[tag[0]] = tag[1]

                    replacement = None
                    if self.TAG_BUILD_PATH in tags_dict:
                        build = tags_dict[self.TAG_BUILD_PATH]
                        paths_data = build_paths.get(build, None)
                        if paths_data is None:
                            paths_data = meta_info.get_build_path(build)
                            build_paths[build] = paths_data
                        replacement = paths_data

                    elif self.TAG_ATTR in tags_dict:
                        attr = tags_dict[self.TAG_ATTR]
                        file_type = tags_dict[self.TAG_FILE_TYPE]
                        paths_data = attrs.get((attr, file_type), None)
                        if paths_data is None:
                            paths_data = meta_info.get_file_vars(attr=attr, file_types=file_type)
                            attrs[(attr, file_type)] = paths_data
                        if tags_dict[self.TAG_VAR] in paths_data:
                            if not paths_data[tags_dict[self.TAG_VAR]]:
                                raise MetaError(MetaError.UNKNOWN, 'Meta lib did not return image path for sign id: "' + str(sign_id) + '"')
                            replacement = paths_data[tags_dict[self.TAG_VAR]][0]

                    elif self.TAG_FILE_TYPE in tags_dict:
                        file_type = tags_dict[self.TAG_FILE_TYPE]
                        paths_data = file_types.get(file_type, None)
                        if paths_data is None:
                            paths_data = meta_info.get_files(file_types=file_type)
                            file_types[file_type] = paths_data
                        if paths_data:
                            for each_path in paths_data:
                                if each_path.lower().endswith(image.name.lower()):
                                    replacement = each_path
                                    break
                    else:
                        raise MetaError(MetaError.UNKNOWN, 'Unknown image type')

                    if replacement:
                        image_path = image_path.replace(re_match.group(0), replacement)
                    else:
                        raise MetaError(MetaError.NOT_FOUND, 'File not found in meta build: ' + sign_id)

                image_path = image_path.replace(self.REPL_META_PATH, self._meta_build_path)

                image_src_path = ImagePath()
                image_dest_path = DestImagePath()

                image_src_path.image_dir_base = os.path.dirname(image_path)
                image_src_path.image_dir_ext = ''
                image_src_path.image_name = os.path.basename(image_path)
                image_dest_path.image_dir_base = os.path.dirname(image_path)
                image_dest_path.image_dir_ext = ''
                image_dest_path.image_name = os.path.basename(image_path)

            except MetaError as e:
                logger.warning(str(e))
                yield (sign_id, self.chipset, None, None, e.err_code)
                continue

            except Exception as e:
                logger.warning('Metalib error: ' + str(e))
                continue

            yield (sign_id, self.chipset, image_src_path, image_dest_path, MetaError.SUCCESS)

    def get_image_config_parser(self, chipset):
        return self.img_config_parser

    @property
    def mini_build_path(self):
        raise RuntimeError('Old Meta Build contents.xml does not support minimized build.')

    @mini_build_path.setter
    def mini_build_path(self, mini_build_path):
        raise RuntimeError('Old Meta Build contents.xml does not support minimized build.')
