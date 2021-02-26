"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""


		self.section_tbl.dump()

		ss = self.symbol_tbl.all_containing('selinux_enforcing')
		ss.dump()

		ss = self.symbol_tbl.all_containing('linux_banner')
		ss.dump()

		ss = self.symbol_tbl.all_containing('linux_proc_banner')
		ss.dump()

		file_pos = self.get_symbol_file_pos('linux_banner')
		print('file_pos(linux_banner) = {0}'.format(file_pos))
		self.dd_symbol('linux_banner')
		self.dd_section('.head.text')

