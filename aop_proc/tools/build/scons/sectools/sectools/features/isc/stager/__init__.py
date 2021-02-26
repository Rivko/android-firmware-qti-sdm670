# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Creates the :class:`..imageinfo.ImageInfo` objects and configures the paths
and ids of these objects based on the params. Takes care of any staging
requirements.
"""

import abc
import os

from sectools.common.utils import c_path
from sectools.common.utils.c_misc import multi_image_string
from sectools.features.isc.imageinfo import ImageInfo, MULTI_IMAGE_SIGN_ID


class BaseStager(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def __init__(self, authority):
        self._output_dir = ''
        self._mini_build_path = ''
        self._image_info_list = []
        self._multi_image_imageinfo_dict = dict()
        self.images_status = {}
        self.authority = authority

    @property
    def image_path_list(self):
        return [image_info.src_image.image_path for image_info in self.image_info_list]

    @property
    def image_info_list(self):
        return self._image_info_list

    @property
    def multi_image_imageinfo_dict(self):
        return self._multi_image_imageinfo_dict

    @property
    def output_dir(self):
        """(str) Output directory to store the output data to."""
        return self._output_dir

    @output_dir.setter
    def output_dir(self, output_dir):
        assert isinstance(output_dir, str)
        output_dir = c_path.normalize(output_dir)
        if not c_path.validate_dir_write(output_dir):
            raise RuntimeError('No write access to output directory: ' + output_dir)

        def update_dest_image(imageinfo):
            imageinfo.dest_image.image_dir_base = output_dir
            imageinfo.dest_image.image_dir_ext = (imageinfo.chipset + '/' + imageinfo.sign_id)

        # Update the output dir of each image in image_info_list
        for image_info in self.image_info_list:
            update_dest_image(image_info)
        # Update the output dir of Multi-Image Signing and Integrity image
        if self._multi_image_imageinfo_dict:
            for target, imageinfo in self._multi_image_imageinfo_dict.iteritems():
                update_dest_image(imageinfo)

        self._output_dir = output_dir

    @property
    def mini_build_path(self):
        return self._mini_build_path

    @mini_build_path.setter
    def mini_build_path(self, mini_build_path):
        assert isinstance(mini_build_path, str)
        mini_build_path = c_path.normalize(mini_build_path)
        if not c_path.validate_dir_write(mini_build_path):
            raise RuntimeError('No write access to minimized build directory: ' + mini_build_path)

        # Update the output dir of each image in image_info_list
        for image_info in self.image_info_list:
            image_info.dest_image.image_dir_base = mini_build_path
        self._mini_build_path = mini_build_path

    def _get_sign_id(self, img_config_parser, image_name, sign_id=None):
        if sign_id is not None:
            # If sign_id is given, check that the sign id exists
            available_sign_ids = img_config_parser.sign_id_list
            if sign_id not in available_sign_ids:
                raise RuntimeError('Unknown sign id provided: ' + sign_id + '\n'
                                   '    Available sign ids: ' + str(available_sign_ids))
        else:
            # Figure out the sign_id if it is not given using the name of the file
            sign_id = img_config_parser.get_sign_id_for_image_name(image_name)
        return sign_id

    def _create_imageinfo(self, img_config_parser, parsegen_config, sign_id, image_path, gen_multi_image, src_image=None, dest_image=None):

            if not gen_multi_image and sign_id not in MULTI_IMAGE_SIGN_ID.values():
                # Validate the sign_id
                sign_id = self._get_sign_id(img_config_parser, os.path.basename(image_path), sign_id)
                # Get the config block for the sign id
                img_config_block = img_config_parser.get_config_for_sign_id(sign_id)
            else:
                try:
                    # Get the config block for the Multi-Image Sign & Integrity image
                    img_config_block = img_config_parser.get_config_for_sign_id(sign_id)
                except:
                    raise RuntimeError(multi_image_string() + " images are not supported for chipset {0}".format(img_config_parser.chipset))

            # Create the image info object
            image_info = ImageInfo(image_path, sign_id, img_config_block, img_config_parser, parsegen_config, gen_multi_image=gen_multi_image, authority=self.authority)
            image_info.dest_image.image_name = image_info.src_image.image_name

            # Set src_image
            if src_image is not None:
                image_info.src_image = src_image
                image_info.image_under_operation = image_info.src_image.image_path

            # Set dest_image
            if dest_image is not None:
                image_info.dest_image = dest_image

            # Check if the dest image name should be overridden
            if img_config_block.output_file_name is not None:
                image_info.dest_image.image_name = img_config_block.output_file_name

            return image_info


#------------------------------------------------------------------------------
# Restrict all import
#------------------------------------------------------------------------------
from image_paths_stager import ImagePathsStager
from meta_build_stager import MetaBuildStager
from meta_build_tmp_stager import MetaBuildTmpStager

__all__ = ['ImagePathsStager',
           'MetaBuildStager',
           'MetaBuildTmpStager',]
