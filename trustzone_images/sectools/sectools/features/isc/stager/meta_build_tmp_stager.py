# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import re
import uuid

from .meta_build_stager import MetaBuildStager
from sectools.common.utils.c_logging import logger
from sectools.features.isc.imageinfo import ImagePath
from sectools.features.isc.imageinfo.secimageinfo import DestImagePath
from sectools.features.isc.stager.meta_build_stager import MetaError

# Keys for meta build product info dictionary.
PRODUCT_NAME = "product_name"
PRODUCT_META_TYPE = "meta_type"

# Replacement tags in meta-build locations for image config.
TAG_VAR = "VAR"
TAG_ATTR = "ATTR"
TAG_FLAVOR = "FLAVOR"
TAG_FILE_TYPE = "FILE_TYPE"
TAG_META_TYPE = "META_TYPE"
TAG_BUILD_PATH = "BUILD_PATH"
TAG_PRODUCT_NAME_PREFIX = "PRODUCT_NAME_PREFIX"


def config_flavor_not_supported(product_meta_type, config_meta_type, config_flavor):
    """ Determine if a flavored location is not supported by the meta build.

    arg:product_meta_type - meta build contents.xml product info "meta_type"
    arg:config_meta_type - secimage meta build location config variable META_TYPE
    arg:config_flavor - secimage meta build location config variable FLAVOR

    Both product_meta_type and config_meta_type are used to determine whether a
    meta build is flavored. If they mismatch, the meta build is not flavored. If they
    both are None, the meta build is considered not flavored.

    If config_flavor is not defined or is None, bool(config_flavor) = False. Otherwise,
    it is True.

    So the return value of this function is determined by the truth table below:

              |     is config_flavor defined
    ==============================================
              |   |     NOT      |      YES      |
       is     +---+--------------+---------------+
      meta    |NOT|    False     |      True     |
      build   +------------------+---------------+
    flavored  |YES|    True      |     False     |
    ==============================================
    """
    is_meta_build_flavored = False if not product_meta_type else (
            product_meta_type == config_meta_type)
    return bool(config_flavor) ^ is_meta_build_flavored


class MetaBuildTmpStager(MetaBuildStager):

    # Replacement tag for replacing with the path of the meta-build
    REPL_META_PATH = '$(META_BUILD)'

    def __init__(self, meta_build_path, config_dir_obj, img_config_parser, parsegen_config_parser, authority,
                 sign_id_list=[], gen_multi_image=False, platform_binding=None):
        self.img_config_parser = img_config_parser
        self.parsegen_config_parser = parsegen_config_parser
        self.chipset = self.img_config_parser.chipset
        super(MetaBuildTmpStager, self).__init__(meta_build_path, config_dir_obj, parsegen_config_parser, authority,
                                                 sign_id_list, gen_multi_image, platform_binding=platform_binding)

    def resolve_image_path(self, meta_info, image, sign_id):
        """ Resolve a meta build location template to an actual image file path.

        Multiple meta build locations are supported. Try them in the order
        configured in the secimage XML file. Return the first image file path
        that is resolved and exists. If none works, a MetaError.NOT_FOUND
        exception is raised.
        """
        logger.debug('Searching metabuild for ' + sign_id)

        product_info = meta_info.get_product_info()
        product_name = product_info.get(PRODUCT_NAME)
        product_meta_type = product_info.get(PRODUCT_META_TYPE)

        for image_path in convert_multi_meta_build_locations_to_list(image.meta_build_location):
            # Initialize cache dictionaries.
            build_paths = dict()
            attrs = dict()
            file_types = dict()

            # Replace any tags
            re_match = re.match('\$\((.*?)\)', image_path)
            if re_match:
                tags = re_match.group(1)
                # noinspection PyTypeChecker
                tags_dict = dict([tuple(tag.strip().split(':')) for tag in tags.split(',')])
                flavor = tags_dict.get(TAG_FLAVOR)
                if product_name and tags_dict.get(TAG_PRODUCT_NAME_PREFIX):
                    if not product_name.startswith(tags_dict[TAG_PRODUCT_NAME_PREFIX]):
                        continue
                if config_flavor_not_supported(product_meta_type, tags_dict.get(TAG_META_TYPE), flavor):
                    continue
                replacement = None
                if TAG_BUILD_PATH in tags_dict:
                    build = tags_dict[TAG_BUILD_PATH]
                    paths_data = build_paths.get(build, None)
                    if paths_data is None:
                        paths_data = meta_info.get_build_path(build)
                        build_paths[build] = paths_data
                    replacement = paths_data

                # Extract images from meta_lib based on attribute(attr) tag.
                elif TAG_ATTR in tags_dict:
                    attr = tags_dict[TAG_ATTR]
                    paths_data = attrs.get(attr, None)
                    if paths_data is None:
                        # Removed file_type when calling get_file_vars(). By default, it returns files of all types.
                        paths_data = meta_info.get_file_vars(attr, flavor=flavor)
                        attrs[attr] = paths_data
                    if tags_dict[TAG_VAR] in paths_data:
                        if not paths_data[tags_dict[TAG_VAR]]:
                            raise MetaError(MetaError.UNKNOWN,
                                            'Meta lib did not return image path for sign id: "' + str(sign_id) + '"')
                        # If ATTR:minimized, VAR:true, then meta_lib returns many paths when minimized = true.
                        if len(paths_data[tags_dict[TAG_VAR]]) > 1:
                            # Iterate through the returned paths until the matching criteria is met.
                            for each_path in paths_data[tags_dict[TAG_VAR]]:
                                if each_path.lower().endswith(image.name.lower()):
                                    # The matching criteria is that file path ends with image name.
                                    replacement = each_path
                                    break
                        else:
                            # If only 1 path is returned, then use it as replacement.
                            replacement = paths_data[tags_dict[TAG_VAR]][0]

                # To make file_type as optional and to keep backward compatible file_type is still kept as same as previous
                elif TAG_FILE_TYPE in tags_dict:
                    file_type = tags_dict[TAG_FILE_TYPE]
                    paths_data = file_types.get(file_type, None)
                    if paths_data is None:
                        paths_data = meta_info.get_files(file_types=file_type, flavor=flavor)
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

            image_path = image_path.replace(self.REPL_META_PATH, self._meta_build_path)
            if os.path.exists(image_path):
                return image_path

        # If all possible image paths are exhausted and none of them exists, then
        # raise MetaError.NOT_FOUND exception.
        raise MetaError(MetaError.NOT_FOUND, 'File not found in meta build: ' + sign_id)

    def get_image_info_from_meta(self, meta_info):
        """ Get the image info from the meta build for all applicable sign ID.

        The image info includes:
          sign_id, chipset, image_src_path, image_dest_path and meta status.

        If an image is found or resolved, the meta status is SUCCESS. If not found,
        then meta status is NOT_FOUND. Otherwise, status is UNKNOWN. If unknown,
        a warning is logged, and the search continues.

        All the images defined in the secimage XML file image list is examined.
        Those that do not have meta build location tag is skipped.
        """
        for sign_id in self.img_config_parser.sign_id_list:
            image = self.img_config_parser.get_config_for_sign_id(sign_id)

            if not image.meta_build_location:
                continue

            try:
                image_path = self.resolve_image_path(meta_info, image, sign_id)
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


def convert_multi_meta_build_locations_to_list(definition):
    """ Convert the meta_build_location definition to a list of locations.

    Multiple meta build locations are supported. The XML parser presents them
    as a single string, separated by whitespaces. It needs to be broken into
    individual locations before they can be used.

    Each location definition could be composed of a $(...) template, a simple raw
    text file path, or a combination of both. This function is able to find these
    patterns and return a list of them in order.

    Specifically,
      1. it replaces $(...) patterns with raw text placeholders
      2. it splits the entire string along the whitespaces
      3. it replaces the placeholders with the original definition
    """
    def split_raw_text_only(text):
        locations_ = []
        for line in [x.strip() for x in text.splitlines()]:
            locations_.extend(line.split())
        return locations_

    replacement = []
    definition = definition.strip()
    if not definition:
        return []
    if definition.find("$(") < 0:
        return split_raw_text_only(definition)

    matching_pattern = re.compile(r"(\$\(.*?\))")
    match = matching_pattern.search(definition)
    while match:
        temp_string = "<%s>" % str(uuid.uuid4())
        definition = definition.replace(match.group(1), temp_string)
        replacement.append((temp_string, match.group(1)))
        match = matching_pattern.search(definition)
    locations = []
    for item in split_raw_text_only(definition):
        if item.find("<") < 0:
            locations.append(item)
            continue
        temp_string, original = replacement[0]
        if temp_string not in item:
            raise RuntimeError("%s not in %s" % (temp_string, item))
        locations.append(item.replace(temp_string, original))
        del replacement[0]
    return locations
