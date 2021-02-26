"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
from __future__ import absolute_import

# Populates mpdb
# Assumes catalog is loaded
# Assumes kernel is loaded

import sys
import mpgen_catalog
import mpgen_kernel
import mpgen_mpdb
from mpgen_mpdb import MPDB

class Populate():
	def __init__(self):
		return
	def populate(self, catalog, kernel):
		return

# Populating the read-only sections could be part of the catalog, but
# in the case where new sections are added extracting RO sections from the
# compiled kernel ensures any new sections are included in the MP's.
def populate_ro_sections(mpdb, kernel):
	a = iter(kernel.section_tbl)
	for section in a:
		if (('READONLY' in section.flags)
				and ('debug' not in section.name)
				and ('+' not in section.name)) :
			section.dump()
			sha256 = kernel.get_section_sha256(section.name)
			if ('CODE' in section.flags) :
				mpdb.append(mpgen_mpdb.MP_WR_ONCE_UNKNOWN(section.name,
														  section.VMA,
														  section.size,
														  0,
														  sha256))
			else :
				mpdb.append(mpgen_mpdb.MP_RD_ONLY(section.name,
												  section.VMA,
												  section.size,
												  0,
												  sha256))

	return

def get_mpdb(catalog, kernel, config):
	mpdb = MPDB(catalog)
	a = iter(catalog)
	for x in a:
		try:
			# x.dump()
			if x.mp_type is mpgen_mpdb.RD_ONLY :
				if x.section_or_symbol is mpgen_mpdb.SYMBOL :
					symbol = kernel.get_symbol(x.name)
					if (symbol is None) : continue
					if (symbol.section == "*ABS*"):
						print("Symbol %s is absolute value" % symbol.name)
						sha256 = "0"
					else:
						sha256 = kernel.get_symbal_sha256(x.name)
					mpdb.append(mpgen_mpdb.MP_RD_ONLY(symbol.name,
													  symbol.value,
													  symbol.size,
													  x.attrs,
													  sha256))
				else :
					if (x.name == 'populate_all_ro_sections') :
						populate_ro_sections(mpdb, kernel)
					else :
						section = kernel.get_section(x.name)
						if (('READONLY' in section.flags) and ('debug' not in section.name)) :
							section.dump()
							sha256 = kernel.get_section_sha256(section.name)
							mpdb.append(mpgen_mpdb.MP_RD_ONLY(section.name,
															  section.VMA,
															  section.size,
															  x.attrs,
															  sha256))
			if x.mp_type is mpgen_mpdb.WR_ONCE_KNOWN :
				symbol = kernel.get_symbol(x.name)
				if (symbol is None) : continue
				sha256 = kernel.get_symbal_sha256(x.name)
				mpdb.append(mpgen_mpdb.MP_WR_ONCE_KNOWN(
												symbol.name,
												symbol.value,
												symbol.size,
												x.attrs,
												sha256))

			if x.mp_type is mpgen_mpdb.WR_ONCE_UNKNOWN :
				symbol = kernel.get_symbol(x.name)
				if (symbol is None) : continue
				sha256 = kernel.get_symbal_sha256(x.name)
				mpdb.append(mpgen_mpdb.MP_WR_ONCE_UNKNOWN(
												symbol.name,
												symbol.value,
												symbol.size,
												x.attrs,
												sha256))

			if x.mp_type is mpgen_mpdb.WR_AUTH_WRITER :
				symbol = kernel.get_symbol(x.name)
				if (symbol is None) : continue
				writers = kernel.superset(x.writer)
				if (writers is None):
					# TODO emit warning & mpgen message
					catalog.mp_warning("AU:`%s` - no writers" % (x.name))
					print("WARNING: mpgen authorized writer not found: %s" % x.writer)
					continue
				mpdb.append(mpgen_mpdb.MP_WR_AUTH_WRITERS(
												symbol.name,
												symbol.value,
												symbol.size,
												x.attrs,
												writers))

			if x.mp_type is mpgen_mpdb.KCONFIG :
				parameter = config.get_parameter(x.name)
				if (parameter is None) : continue
				mpdb.append(mpgen_mpdb.MP_KCONFIG(x.name, parameter.value))

		except mpgen_kernel.ExceptionSymbolNotFound as e:  # symbol not found is just a warning, report and continue
			catalog.mp_warning("No sym. `%s`" % (e))
			print >> sys.stderr, "WARNING: could not find symbol: `%s`" % (e)

	return mpdb

