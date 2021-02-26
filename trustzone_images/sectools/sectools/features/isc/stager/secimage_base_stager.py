# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from sectools.common.utils import c_path
from sectools.features.isc.advanced_defines import multi_image_string
from sectools.features.isc.imageinfo.secimageinfo import ImageInfo
from sectools.features.isc.imageinfo.secimageinfo import MULTI_IMAGE_SIGN_ID
from sectools.features.isc.imageinfo.secimageinfo import QTI_MULTI_IMAGE_SIGN_ID
from .base_stager import BaseStager


class SecimageBaseStager(BaseStager):

    def __init__(self, authority):
        super(SecimageBaseStager, self).__init__(authority)
        self._mini_build_path = ''
        self._multi_image_imageinfo_dict = dict()

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

        # Update the output dir of each image in image_info_list
        for image_info in self.image_info_list:
            self.update_dest_image(image_info, output_dir)
        # Update the output dir of Multi-Image Signing and Integrity image
        if self._multi_image_imageinfo_dict:
            for target, imageinfo in self._multi_image_imageinfo_dict.items():
                self.update_dest_image(imageinfo, output_dir)

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

    def _create_imageinfo(self, img_config_parser, parsegen_config, sign_id, image_path, gen_multi_image=False,
                          src_image=None, dest_image=None, crypto_params={}, imageinfo_class=ImageInfo,
                          verbatim_config=False, disable_serial_binding=False, platform_binding=None):
        """
        The same dest_image object should not be used by two or more image_info
        objects. It results in destination image path cross contamination.
        For example, image_info objects A and B use the same dest_image.
        If A.dest_image is updated with a new output_dir, then B.dest_image
        is changed as well, because they are the same object and share the same
        attributes.
        """
        if not gen_multi_image and sign_id not in MULTI_IMAGE_SIGN_ID.values():
            # Validate the sign_id
            sign_id = self._get_sign_id(img_config_parser, os.path.basename(image_path), sign_id)
            # Get the config block for the sign id
            img_config_block = img_config_parser.get_config_for_sign_id(sign_id)
        else:
            try:
                # Get the config block for the Multi-Image Sign & Integrity image
                img_config_block = img_config_parser.get_config_for_sign_id(sign_id)
            except RuntimeError:
                prefix = "QTI " if sign_id == QTI_MULTI_IMAGE_SIGN_ID else ""
                raise RuntimeError(prefix + multi_image_string() +
                                   " image is not supported for chipset {0}"
                                   .format(img_config_parser.chipset))

        # Create the image info object
        image_info = imageinfo_class(image_path, sign_id, img_config_block, img_config_parser, parsegen_config,
                                     gen_multi_image=gen_multi_image, authority=self.authority,
                                     crypto_params=crypto_params, verbatim_config=verbatim_config,
                                     disable_serial_binding=disable_serial_binding, platform_binding=platform_binding)
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
