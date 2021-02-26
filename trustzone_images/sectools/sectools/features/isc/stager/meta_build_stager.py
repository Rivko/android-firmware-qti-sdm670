# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import sys

from sectools.common.build.metabuild import MetaBuild
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.features.isc.cfgparser import ConfigDir
from sectools.features.isc.imageinfo import ImagePath
from sectools.features.isc.imageinfo.secimageinfo import DestImagePath
from sectools.features.isc.imageinfo.secimageinfo import MULTI_IMAGE_SIGN_ID
from sectools.features.isc.parsegen.config.parser import ParsegenCfgParser
from sectools.features.isc.secimage.secimage_cfgparser import SecImageConfigParser
from .secimage_base_stager import SecimageBaseStager


class MetaError(RuntimeError):

    SUCCESS = 0
    UNKNOWN = 1
    NOT_FOUND = 2
    NO_ACCESS = 3

    ERR_STR_MAP = \
    {
        SUCCESS: '',
        UNKNOWN: 'Unknown image type',
        NOT_FOUND: 'Image info not found in meta build',
        NO_ACCESS: 'No read access to the image',
    }

    def __init__(self, err_code, *args, **kwargs):
        RuntimeError.__init__(self, *args, **kwargs)
        self.err_code = err_code


class MetaBuildStager(SecimageBaseStager):

    META_LIB_PATH_REL = ['common/tools/meta',
                         'common/build/lib']

    def __init__(self, meta_build_path, config_dir_obj, parsegen_config, authority,
                 sign_id_list=[], gen_multi_image=False, platform_binding=None):
        assert isinstance(meta_build_path, str)
        assert isinstance(config_dir_obj, ConfigDir)
        assert isinstance(parsegen_config, ParsegenCfgParser)

        # Initialize the BaseStager
        super(MetaBuildStager, self).__init__(authority)

        self.config_dir_obj = config_dir_obj

        # Create internal attributes
        self._meta_build_path = meta_build_path

        # Validate that the meta_build path exists
        meta_build_path = c_path.normalize(meta_build_path)
        if not c_path.validate_dir(meta_build_path):
            raise RuntimeError('No read access to the meta build path: ' + meta_build_path)

        # Get the meta lib module from the metabuild
        meta_info = MetaBuild.getMetainfo(meta_build_path)

        # List of sign ids searched
        sign_ids_searched = []

        # Create the image info list based on the meta data
        for sign_id, chipset, image_src_path, image_dest_path, err_code in self.get_image_info_from_meta(meta_info):

            # Filter on the sign id
            if sign_id_list and sign_id not in sign_id_list:
                continue

            # Update internal variables
            sign_ids_searched.append(sign_id)
            self.images_status[sign_id] = {
                'errcode' : err_code,
                'path' : image_src_path.image_path if image_src_path is not None else None,
            }

            # If there is an error, continue
            if err_code != MetaError.SUCCESS:
                continue
            try:
                # Put the image info object into the list
                img_config_parser = self.get_image_config_parser(chipset)
                imageinfo = self._create_imageinfo(
                    img_config_parser, parsegen_config, sign_id, "", gen_multi_image=False,
                    src_image=image_src_path, dest_image=image_dest_path, platform_binding=platform_binding)
                self._image_info_list.append(imageinfo)
            except Exception as e:
                logger.error(str(e))
                continue

            # Create image info object for to-be-created Multi-Image Signing and Integrity image
            if gen_multi_image:
                if chipset in self._multi_image_imageinfo_dict:
                    logger.debug("Multi-image imageinfo already exists for chipset " + chipset)
                    continue
                logger.debug("Create multi-image imageinfo object for chipset " + chipset)
                # Set sign id to Multi-Image Sign & Integrity image's sign id
                sign_id = MULTI_IMAGE_SIGN_ID[authority]
                # Set the Multi-Image Signing & Integrity image's imageinfo
                self._multi_image_imageinfo_dict[chipset] = self._create_imageinfo(
                    img_config_parser, parsegen_config, sign_id, None, gen_multi_image=True,
                    platform_binding=platform_binding)

        if sign_id_list and set(sign_id_list) != set(sign_ids_searched):
            raise RuntimeError('Unknown sign id provided: ' + ', '.join(set(sign_id_list) - set(sign_ids_searched)))

        if not self._image_info_list:
            raise RuntimeError('No images found from the meta build.')

    def get_image_config_parser(self, chipset):
        return SecImageConfigParser(self.config_dir_obj.get_chipset_config_path(chipset))

    @classmethod
    def get_image_info_from_meta(cls, meta_info):
        # Get a list of all files tagged with sign_id
        meta_images_list = meta_info.get_files_detailed('sign_id')

        for image in meta_images_list:
            try:
                logger.debug('Found image from meta_build for signing: ' + image.sign_id)

                source_path = None
                dest_path = None
                for each_path in image.file_path:
                    if source_path is None:
                        if getattr(each_path, 'sign_source', False):
                            source_path = each_path.value
                    if dest_path is None:
                        if getattr(each_path, 'sign_dest', False):
                            dest_path = each_path.value
                    if source_path and dest_path:
                        break

                if source_path is None or dest_path is None:
                    raise RuntimeError('SourcePath, DestPath should not be missing.')

                sign_id = image.sign_id
                chipset = image.chipset
                image_src_path = ImagePath()
                image_dest_path = DestImagePath()

                image_src_path.image_dir_base = image.image_dir_base
                image_src_path.image_dir_ext = source_path
                image_src_path.image_name = image.file_name[0].value
                image_dest_path.image_dir_base = image.image_dir_base
                image_dest_path.image_dir_ext = dest_path
                image_dest_path.image_name = image.file_name[0].value

            except Exception as e:
                logger.error(str(e))
                continue

            yield (sign_id, chipset, image_src_path, image_dest_path, MetaError.SUCCESS)

    @classmethod
    def meta_supports_sign_id(cls, meta_build_path):
        meta_info = MetaBuild.getMetainfo(meta_build_path)
        return len(meta_info.get_file_vars('sign_id')) > 0
