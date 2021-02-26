"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
from __future__ import absolute_import

import sys
import mpgen_shell
import jinja2
import os
import datetime
import time

import mpgen_mpdb
import const

template_loader = jinja2.FileSystemLoader(searchpath = os.path.join(os.path.dirname(__file__), "templates"))
template_env = jinja2.Environment(loader = template_loader)
template = template_env.get_template("rtic_mp.template.xml")

def generate_rtic_mp_xml(kernel, mpdb):

	ll = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER]
	print("generate_rtic_mp_xml, len(aw) == %d" % len(ll))
	for item in ll:
		item.dump()

	contents = template.render(
		datetime = datetime.datetime.now(),
		time = int(time.time()),
		VERSION = const.VERSION,
		kernel = kernel,
		mpdb = mpdb,
		banner = kernel.get_banner().replace("\n", "\\n").replace("\0", "\\0"),
		ro = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.RD_ONLY],
		wk = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_KNOWN],
		wu = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_UNKNOWN],
		aw = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER],
		av = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_VALUE],
		kc = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.KCONFIG]
	)
	return contents

