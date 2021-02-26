'''
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Created on Apr 28, 2017

@author: amavrin
'''
from __future__ import absolute_import

import jinja2
import os
import datetime

import mpgen_mpdb
import const
import mpgen_catalog
import mpgen_kernel
import calendar
import time
# import pkgutil

# load the template

template_loader = jinja2.FileSystemLoader(searchpath = os.path.join(os.path.dirname(__file__), "templates"))
template_env = jinja2.Environment(loader = template_loader)
template = template_env.get_template("rtic_mp.template.c")  # this should throw right away if template is not found
template_header = template_env.get_template("rtic_mp_header.template.h")  # this should throw right away if template is not found
template_dts = template_env.get_template("rtic_dts_payload.template.c")  # this should throw right away if template is not found

def generate_rtic_dts(kernel, mpdb):
	kernel_base = int(kernel.get_symbol(".head.text").value, 16)
	# Note: it is possible that someone mixes the build environments and when generating the DTB, MP structure in kernel is not available
	mp_offset = "0x0"
	sha256 = "0"
	mp_va_addr = "0x0"
	mp_size = "0x0"
	try:
		mp_offset = "0x%x" % (int(kernel.get_symbol("rtic_mp").value, 16) - kernel_base)
		sha256 = kernel.get_symbal_sha256("rtic_mp")
		mp_va_addr = "0x%x" % int(kernel.get_symbol("rtic_mp").value, 16)
		mp_size = "0x%x" % int(kernel.get_symbol("rtic_mp").size, 16)
	except mpgen_kernel.ExceptionSymbolNotFound as e:
		print "WARNING! Symbol `%s` is missing. It seems that kernel missing an MP. Generating empty RTIC DTS." % (e)

	ret = template_dts.render(datetime = datetime, VERSION = const.VERSION, kernel = kernel, mp_va_addr = mp_va_addr, mp_offset = mp_offset, mp_size = mp_size, sha256 = sha256)  # pylint: disable=maybe-no-member
	return ret

def generate_rtic_mp(kernel, mpdb):
	# template = jinja2.Template(pkgutil.get_data('templates', 'rtic_mp.template.c'))  # using package as we are going to freeze these resource files

	# when using vmlinux.o values for below vars may be undefined, therefore use default value of 0x0
	kernel_offset_le = "0"
	try: kernel_offset_le = "%s%s" % (kernel.get_symbol('_kernel_offset_le_hi32').value, kernel.get_symbol('_kernel_offset_le_lo32').value) or "0"
	except Exception:
		pass
	kernel_size_le = "0"
	try: kernel_size_le = "%s%s" % (kernel.get_symbol('_kernel_size_le_hi32').value, kernel.get_symbol('_kernel_size_le_lo32').value) or "0"
	except Exception:
		pass

	# symbols __entry_task and __per_cpu_offset may be missing, default it to 0x0
	linux_entry_task_va = "0"
	try: linux_entry_task_va = "%s" % (kernel.get_symbol('__entry_task').value)
	except Exception:
		pass
	linux_per_cpu_offset_va = "0"
	try: linux_per_cpu_offset_va = "%s" % (kernel.get_symbol('__per_cpu_offset').value)
	except Exception:
		pass

	ll = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER]
	print("generate_rtic_mp, len(aw) == %d" % len(ll))
	for item in ll:
		item.dump()
	banner = kernel.get_banner().replace("\n", "\\n").replace("\0", "\\0")

	# to make it easier to read the template - generate a list of RO features
	# ro = filter(lambda mp: mp.mp_type is mpgen_mpdb.RD_ONLY, mpdb)
	ro = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.RD_ONLY]
	ro = sorted(ro, key = lambda mp: int(mp.vaddr, 16), reverse = False)
	wk = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_KNOWN]
	wk = sorted(wk, key = lambda mp: int(mp.vaddr, 16), reverse = False)
	wu = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_ONCE_UNKNOWN]
	wu = sorted(wu, key = lambda mp: int(mp.vaddr, 16), reverse = False)
	aw = [mp for mp in mpdb if mp.mp_type is mpgen_mpdb.WR_AUTH_WRITER]
	aw = sorted(aw, key = lambda mp: int(mp.vaddr, 16), reverse = False)

	# filter(lambda mp: mp.mp_type is mpgen_mpdb.RD_ONLY, mpdb) # pylint complains that list comprehension is more efficient

	# 64bit time stamp from the beginning of epoch
	created_tmstmp = "0x%x" % (calendar.timegm(time.gmtime()))

	ret = template.render(created_tmstmp = created_tmstmp, datetime = datetime, VERSION = const.VERSION, kernel = kernel, mpdb = mpdb, banner = banner, ro = ro, wk = wk, wu = wu, aw = aw, len = len, kernel_size_le = kernel_size_le, kernel_offset_le = kernel_offset_le, linux_entry_task_va = linux_entry_task_va, linux_per_cpu_offset_va = linux_per_cpu_offset_va)  # pylint: disable=maybe-no-member
	return ret

def generate_rtic_mp_header():
	"""
	Generates RTIC MP header that to be included by the TA and QHEE
	"""

	ret = template_header.render(datetime = datetime, VERSION = const.VERSION)  # pylint: disable=maybe-no-member
	return ret

