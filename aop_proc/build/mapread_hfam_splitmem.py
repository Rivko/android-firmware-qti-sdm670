# vim: set ts=4 sw=4 noet smarttab ai :

import os, re, sys, subprocess, csv
from itertools import islice, izip, count


# dictionary of dictionaries representing image sizes
# first dictionary is keyed on reference build, e.g. 30, 31, 32
# second dictionary is keyed on "driver" e.g. sleep, kernel, etc.
library_sizes = { }

library_map = {
	'glink_api.*'						: 'mproc',
	'glink_os_rex.o'					: 'mproc',
	'xport_qmp*'						: 'mproc',
	'startup.o'							: 'aop',
	'main.o'							: 'aop',
	'retarget.o'						: 'aop',
	'exceptions.o'                      : 'aop',
	'time_service.o'                    : 'aop',
	'coredump.o'                        : 'aop',
	'stack_protect.o'                   : 'aop',
	'aop_interrupt_table.o'				: 'aop',
	'bcm_init_vals'						: 'bcm',
	'rex_arm.lib'						: 'rex',
	'pmic_framework.lib'				: 'pmic',
	'pmic_target_rpm.lib'				: 'pmic',
	'pmic_npa.lib'						: 'pmic',
	'pmic_[^\s]+.lib'					: 'pmic',
	'vrm.lib'							: 'vrm',
	'arc.lib'							: 'arc',
	'arc_init_vals'						: 'arc',
	'cmd_db.lib'						: 'cmd_db',
	'cpr.lib'							: 'cpr',
	'cpr_init_vals'						: 'cpr',
	'ddr_mgr_arc.lib'					: 'ddr_mgr',
	'ddraux_init_vals'					: 'ddr_mgr',
	'pdc[^\s]+.lib'						: 'pdc',
	'pdc_global_init_vals'				: 'pdc',
	'rinit.lib'							: 'rinit',
	'rpmh.lib'							: 'rpmh',
	'sleep.lib'							: 'sleep',
	'ulog.lib'							: 'ulog',
	'pwr_utils.lib'						: 'pwr',
	'icb.*'								: 'buses',
	'Spmi.lib'    						: 'spmi',
	'Clock.lib'							: 'clock',
	'clock_init_vals'					: 'clock',
	'aop_fsm_arc.lib'					: 'aop',
	'rsc.lib'							: 'rsc',
	'libc.a'							: 'libc',
	'oem_version.o'						: 'aop',
	'qc_version.o'						: 'aop',
	'datetime.o'						: 'aop',
	'exception_isr.o'					: 'aop',
	'oem_uuid.o'						: 'aop',
	'crt1.o'							: 'aop',
	'aop_mpu*'							: 'mpu',
	'unpa*'								: 'unpa',
	'qdss*'								: 'qdss',
    'ChipInfo.lib'                      : 'chip_info',
    'PM_CRAM_RECLAIM_POOL'              : 'PM_CRAM_RECLAIM',
    'DDR_ULOG'                          : 'ulog',
    'PM_DRAM_RECLAIM_POOL'              : 'PM_DRAM_RECLAIM',
    'cprv'                              : 'cprv',
    'bcm_v'                             : 'bcm_v',
    'bcm_db_headers'                    : 'bcm_db_headers',
    'arc_v'                             : 'arc_v',
    'arc_db_headers'                    : 'arc_db_headers',
    'arc_aux'                           : 'arc_aux',
    'PMIC_DATA'                         : 'PMIC_DATA',
    'bcm_aux'                           : 'bcm_aux',
    'cprwa_init_vals'                   : 'cpr',
    'vrm_tcs_no_pmi_init_vals'          : 'vrm',
    'vrm_tcs_init_vals'                 : 'vrm',
    'vrm_init_vals'                     : 'vrm',
}

section_map = {
	'RINIT_DATA'						: 'RINIT DATA',
	'CODE_RAM'							: 'CODE RAM',
	'ARM_LIB_HEAP'						: 'LIB HEAP',
	'ARM_LIB_STACK'						: 'LIB STACK',
	'DATA_RAM'							: 'DATA RAM',
	'AOP_ULOG'							: 'ULOG',
    'DDR_ULOG'							: 'ULOG',
    'PMIC_DATA'                         : 'PMIC_DDR',
	'TASK_STACKS'						: 'TASK STACKS',
    'PM_DRAM_RECLAIM_POOL'              : 'DRAM_RECLAIM',
    'PM_CRAM_RECLAIM_POOL'              : 'CRAM_RECLAIM',
    'AOPSS_DATA'                        : 'AOPSS_DATA',
    'DRAM_SLEEP_CODE_POOL'              : 'DRAM_SLEEP_CODE_POOL',
}


current_section = ""
found_end_regex = False

def get_lib_line_data(m):
	
	base_addr = int(m.group('base_addr') , base=16)
	size = int(m.group('size') , base=16)
	lib = m.group('lib').strip()
	if '(' in lib: # #bracket libnames not valid
		split_libname = re.split('\(|\/', lib) # hack to get obj file name 
		lib = split_libname[len(split_libname)-2]
	return lib, base_addr, size
	


def get_sec_line_data(m):
	size = int(m.group('size'), base=16)
	base_addr = int(m.group('base_addr'), base=16)
	section = m.group('section').strip()
	return section , base_addr, size


map_line_types_dict = {
	#dictionary of regexs for the type of lines expected and their corresponding parser functions
	'lib' : {'lib_regex': re.compile(r'(?P<subsec>[_.a-zA-Z0-9]+)\s*(?P<base_addr>0x[0-9a-fA-F]+)\s+(?P<size>0x[0-9a-fA-F]+)\s+(?P<path>[^\s]*[\/|\\])(?P<lib>[^\s]*\.o)'),
	              'lib_parser' : get_lib_line_data},
	'sections' :     {'section_regex' : re.compile(r'(?P<section>[_0-9a-zA-Z]*)\s+(?P<base_addr>0x[0-9a-fA-F]+)\s+(?P<size>0x[0-9a-fA-F]+)\s*\#\s*[^\s*]*\s*(?P<offset_addr>0x[0-9a-fA-F]+)\,\s+[LMA: ]+(?P<lma_addr>0x[0-9a-fA-F]+)'),
				  'section_parser' : get_sec_line_data},
	'end'  :    {'end_regex' : re.compile(r'\.ARM\.attributes'),
	             'end_parser' : None},
}


def get_module_pretty_name(refbuild, module):
	# figure out if we should attribute this to a renamed module, or to it's own name
	for pattern in library_map:
		m = pattern.match(module)
		if m:
			return library_map[pattern]

	print >> sys.stderr, "*** WARNING: no pretty renaming for refbuild %s module %s" % (refbuild, module)
	return module

	
def get_section_pretty_name(refbuild, section):
	for pattern in section_map:
		m = pattern.match(section)
		if m:
			return section_map[pattern]

	print >> sys.stderr, "*** WARNING: no pretty renaming for refbuild %s section %s" % (refbuild, section)
	return section
	
def _debug_print():
  for refbuild in library_sizes:
		 print refbuild
		 for sections in library_sizes[refbuild]:
			 print sections + ': \n'
			 for libs in library_sizes[refbuild][sections]:
				 print "\t" + libs + " : " + str(library_sizes[refbuild][sections][libs])

def dump_csv():
		 
	# Create a local CSV file
	#_debug_print()
	header_list = []
	data_list	= []
	for refbuild in library_sizes:
		
		for libs in library_sizes[refbuild]:
			agg_lib_size = 0
			for sections in library_sizes[refbuild][libs]:
				header_list.append('%s:%s' % (libs, sections))
				#print sections
				if(sections != 'RINIT DATA'):
					agg_lib_size += library_sizes[refbuild][libs][sections]
				data_list.append('%s' % library_sizes[refbuild][libs][sections])
			header_list.append('%s: Total' % libs)
			data_list.append('%s' % agg_lib_size) 
	
	print "Writing the csv file"
	with open('memsizes.csv', "wb") as opfile:
		wr = csv.writer(opfile, dialect = 'excel')
		wr.writerow(header_list)
		wr.writerow(data_list)
	

def get_map_lines(build_root, map_file):
	global found_end_regex
	refbuild = os.path.basename(build_root)
	library_sizes[refbuild] = {}
	current_section = '!!ERROR!!' #most certainly will be overwritten
	
	fname = os.path.join(build_root, map_file)
	if os.path.exists(fname):
		f = open(fname, 'r')
		first_line = f.readline()
		if  "LLVM" not in first_line:
			print >> sys.err, "LLVM Linker map expected"
			f.close()
			sys.exit()
		else:
			# proceed till end_regex
			while found_end_regex is False:
				line = f.readline()
				m = map_line_types_dict['end']['end_regex'].match(line)
				if m:
					found_end_regex = True
					break
				m = map_line_types_dict['sections']['section_regex'].match(line)
				if m:
					# line matches a section line
					section, base_addr, size = map_line_types_dict['sections']['section_parser'](m)
					#print section, base_addr, size
					
					
					if section == 'RINIT_DATA':
                        # exception parsing for RINIT
						parse_rinit_dump(f, base_addr, size)
						continue
					current_section = get_section_pretty_name(refbuild, section)
				
				m = map_line_types_dict['lib']['lib_regex'].match(line)
				if m:
					#line match a lib file.
					lib, base_addr, size = map_line_types_dict['lib']['lib_parser'](m)
					lib = get_module_pretty_name(refbuild, lib)
					#add lib data to with with current_section
					if lib not in library_sizes[refbuild]:
						library_sizes[refbuild][lib] = {}
						library_sizes[refbuild][lib][current_section] = size
					else:
						if current_section not in library_sizes[refbuild][lib]:
							library_sizes[refbuild][lib][current_section] = 0
						library_sizes[refbuild][lib][current_section] += size
						
	else:
		print  "Map file not found in location: " + fname
		sys.exit()
		
		return None

	


def parse_rinit_dump(f, start_addr, size):
		# parsing rinit section with an exception.
		dict_size = []
		global found_end_regex
		still_inside_rinit_section = True
		while found_end_regex is False and still_inside_rinit_section:
			line = f.readline()
			m = map_line_types_dict['end']['end_regex'].match(line)
			if m:
				found_end_regex = True
				break
			
			if line == '\n':
				still_inside_rinit_section = False
				break
			
			m = re.compile(r'\s*(?P<base>0x[0-9a-fA-F]+)\s+(?P<module>[a-zA-z]+)').match(line)
			if m:
				#Capture symbols and their base addresses.
				base_addr = int(m.group('base'), base=16)
				module = m.group('module').strip()
				dict_size.append([module, base_addr, 0])
		#get the end address 
		end_add = start_addr + size
		for i in reversed(range(len(dict_size))):
			dict_size[i][2] =  end_add - (dict_size[i][1])  
			end_add = dict_size[i][1]
		
			current_section = get_section_pretty_name(refbuild, 'RINIT_DATA')
			
			lib = get_module_pretty_name(refbuild, dict_size[i][0])
			
			if lib not in library_sizes[refbuild]:
				
				library_sizes[refbuild][lib] = {}
				library_sizes[refbuild][lib][current_section] = dict_size[i][2]
			else:
				if current_section not in library_sizes[refbuild][lib]:
					library_sizes[refbuild][lib][current_section] = 0
				library_sizes[refbuild][lib][current_section] += dict_size[i][2]
		return
	
if __name__ == '__main__':
	# regenerate the pretty library renaming map with actual compiled regexs
	library_map_compiled = {}
	section_map_compiled = {}
	for regex in library_map:
		library_map_compiled[re.compile(regex)] = library_map[regex]
	library_map = library_map_compiled

	for regex in section_map:
		section_map_compiled[re.compile(regex)] = section_map[regex]
	section_map = section_map_compiled
	
	if len(sys.argv) > 1:
		builds = sys.argv[1]
		
		
	else:
		if "aop_proc" in os.getcwd():
			#we are executing in build path proceed to find map file
			m = re.match(r'(.*\\)aop_proc', os.getcwd())
			builds = m.group(1)
			
			
		else:
			# path to the build should've been provided
			print 'Usage:', sys.argv[0], ' <build path>'
			sys.exit(1)

	
	refbuild = os.path.basename(builds)
	
	
	map_file_locations = [
		  r'aop_proc/core/bsp/aop/build/'+os.environ.get('MSM_ID', '845')+'/AOP_' + os.environ.get('BUILD_ID', 'AAAAANAAO') + '.map',
	]
	for map_file in map_file_locations:
		map_lines = get_map_lines(builds, map_file)
			

	dump_csv()
	print 'Created memsizes.csv in local dir'
