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

from six import add_metaclass


@add_metaclass(abc.ABCMeta)
class BaseStager(object):

    @abc.abstractmethod
    def __init__(self, authority):
        self._output_dir = ''
        self._image_info_list = []
        self.images_status = {}
        self.authority = authority

    @staticmethod
    def update_dest_image(imageinfo, output_dir):
        imageinfo.dest_image.image_dir_base = output_dir
        imageinfo.dest_image.image_dir_ext = (imageinfo.chipset + '/' + imageinfo.sign_id)

    @property
    def image_path_list(self):
        return [image_info.src_image.image_path for image_info in self.image_info_list]

    @property
    def image_info_list(self):
        return self._image_info_list

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
