# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils import c_path
from sectools.features.isc.advanced_defines import multi_image_string
from sectools.features.isc.cfgparser import ConfigParser
from sectools.features.isc.imageinfo.secimageinfo import MULTI_IMAGE_SIGN_ID
from sectools.features.isc.parsegen.config.parser import ParsegenCfgParser
from .secimage_base_stager import SecimageBaseStager


class ImagePathsStager(SecimageBaseStager):

    def __init__(self, image_path, img_config_parser, parsegen_config, authority,
                 sign_id=None, gen_multi_image=False, multi_image_path=None,
                 crypto_params={}, imageinfo_class=None, verbatim_config=False,
                 disable_serial_binding=False, platform_binding=None):

        assert isinstance(image_path, str)
        assert isinstance(img_config_parser, ConfigParser)
        assert isinstance(parsegen_config, ParsegenCfgParser)
        if sign_id is not None: assert isinstance(sign_id, str)

        # Initialize the BaseStager
        super(ImagePathsStager, self).__init__(authority)

        # Validate that the image path exists
        image_path = c_path.normalize(image_path)
        if not c_path.validate_file(image_path):
            raise RuntimeError('No read access to the image path: ' + image_path)
        # Put the image info object into the list
        imageinfo = self._create_imageinfo(
            img_config_parser, parsegen_config, sign_id, image_path,
            gen_multi_image=False, crypto_params=crypto_params,
            imageinfo_class=imageinfo_class, verbatim_config=verbatim_config,
            disable_serial_binding=disable_serial_binding, platform_binding=platform_binding)
        self._image_info_list.append(imageinfo)

        # Validate that the Multi-Image Sign & Integrity image path exists
        if multi_image_path is not None:
            multi_image_path = c_path.normalize(multi_image_path)
            if not c_path.validate_file(multi_image_path):
                raise RuntimeError('No read access to the ' + multi_image_string() + ' image path: ' + multi_image_path)
            # Set sign id to Multi-Image image's sign id
            sign_id = MULTI_IMAGE_SIGN_ID[authority]
            # Set the Multi-Image Signing & Integrity image's imageinfo
            multi_image_imageinfo = self._create_imageinfo(
                img_config_parser, parsegen_config, sign_id=sign_id,
                image_path=multi_image_path, gen_multi_image=False,
                imageinfo_class=imageinfo_class, platform_binding=platform_binding)
            self._multi_image_imageinfo_dict[multi_image_imageinfo.chipset] = multi_image_imageinfo
        # Create image info object for to-be-created Multi-Image Signing and Integrity image
        elif gen_multi_image:
            # Set sign id to Multi-Image Sign & Integrity image's sign id
            sign_id = MULTI_IMAGE_SIGN_ID[authority]
            # Set the Multi-Image Signing & Integrity image's imageinfo
            multi_image_imageinfo = self._create_imageinfo(
                img_config_parser, parsegen_config, sign_id, None, gen_multi_image,
                imageinfo_class=imageinfo_class, platform_binding=platform_binding)
            self._multi_image_imageinfo_dict[multi_image_imageinfo.chipset] = multi_image_imageinfo

    @property
    def mini_build_path(self):
        raise RuntimeError('Individual Image Signing does not support minimized build.')

    @mini_build_path.setter
    def mini_build_path(self, mini_build_path):
        raise RuntimeError('Individual Image Signing does not support minimized build.')
