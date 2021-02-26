'''
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Created on May 5, 2017

@author: amavrin
'''
from __future__ import absolute_import

import jinja2
import os
import datetime

import mpgen_mpdb
import const


# load the template
template_loader = jinja2.FileSystemLoader(searchpath = os.path.join(os.path.dirname(__file__), "templates"))
template_env = jinja2.Environment(loader = template_loader)
template = template_env.get_template("rtic_mp.template.dts")  # this should throw right away if template is not found

def generate_rtic_mp_dts(dts_content_hex):
	ret = template.render(dts_content_hex = dts_content_hex)
	return ret

