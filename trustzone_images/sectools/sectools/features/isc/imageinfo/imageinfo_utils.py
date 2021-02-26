# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import json
import os

from sectools.common.utils import c_path


class ImagePath(object):

    def __init__(self, image_path=''):
        self._image_dir_base = os.path.dirname(image_path)
        self._image_dir_ext = ''
        self._image_name = os.path.basename(image_path)

    @property
    def image_dir_base(self):
        return self._image_dir_base

    @image_dir_base.setter
    def image_dir_base(self, image_dir_base):
        image_dir_base = c_path.normalize(image_dir_base)
        self._image_dir_base = image_dir_base

    @property
    def image_dir_ext(self):
        return self._image_dir_ext

    @image_dir_ext.setter
    def image_dir_ext(self, image_dir_ext):
        self._image_dir_ext = image_dir_ext

    @property
    def image_dir(self):
        return c_path.join(self.image_dir_base, self.image_dir_ext)

    @property
    def image_name(self):
        return self._image_name

    @image_name.setter
    def image_name(self, image_name):
        self._image_name = image_name

    @property
    def image_path(self):
        return c_path.join(self.image_dir, self.image_name)

    @property
    def image_name_base(self):
        return os.path.splitext(self.image_name)[0]

    @property
    def image_name_ext(self):
        return os.path.splitext(self.image_name)[1]


class StatusInfo(object):

    NOT_EXECUTED = 'NA'
    SUCCESS = 'T'
    ERROR = 'F'

    def __init__(self):
        self.state = self.NOT_EXECUTED
        self.error = ''


def resolve_enumerated(attributes, *attr_names):
    """ Update enumerated attributes

    Some attributes, such as debug, support enumerated types. Their actual value
    depends on secboot version. So it can only be resolved after the config is
    available.
    """
    for name in attr_names:
        try:
            attr_value = json.loads(getattr(attributes, name))
            assert isinstance(attr_value, dict)
            setattr(attributes, name, str(attr_value[attributes.secboot_version]))
        except ValueError:
            pass
