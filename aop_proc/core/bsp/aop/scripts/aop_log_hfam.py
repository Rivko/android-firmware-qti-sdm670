from struct import unpack
import argparse
from math import log
import sys, re, os


# ============================================================================
# Globals
# ============================================================================
table_dict = {}
prev_timestamp = 0

# ----------------------------------------------------------------------------
# Pattern definitions for parsing a tracer_event_tbl.h file
# ----------------------------------------------------------------------------
struct_decl_pattern = re.compile('tracer_event_item[\s]+([\w]+)\[\]')
struct_line_pattern = re.compile('[\s]*{[\s]*(?P<event_id>[a-zA-Z0-9_]+)([\s]*=[\s]*(?P<id>[\d]*))*[\s]*,[\s]*"(?P<format>[\s]*[\S ]*)"[\s]*}')
dump_line_pattern = re.compile('-[\s]+(0x[\da-fA-F]{2},[\s]*){28}')


# logger object to dump a log file in local dir to this script has same content
# as seen on console
class Logger(object):
	def __init__(self):
		self.terminal = sys.stdout
		self.log = open("aop_ulog_parsed_dump.txt", "w")
	
	def write( self, message):
		self.terminal.write(message)
		self.log.write(message)
		
	def flush(self):
		pass
	
# scan tracer_event_tbl.h file for event 
def parse_table_header(fname):

	inside_struct = False #only store values inside the struct, ignore rest
	counter = -1 #maintain ids by counting through each event
	id = 0
	try:
		if os.path.exists(fname):
			print "*** Parsing table header ***"
			with open(fname, 'r') as f:
				header_lines = f.readlines()
			#return dump_lines
	except:
		print "Error reading from the " + "table_file"
		raise
	
	for header_line in header_lines:
		if not inside_struct:
			# keep matching till we find struct decl
			m = struct_decl_pattern.match(header_line)
			if m:
				# means we found the struct
				inside_struct = True
		else:
			# match with line pattern
			m = struct_line_pattern.match(header_line)
			if m:
				
				#print m.group('format')
				if not m.group('id'):
					# we didn't find the id from header use counter
					
					counter = int(counter) + 1
					id = counter
				else:
					id = int(m.group('id'))
					counter = id
				
				event_str = str(m.group('event_id'))
				format = str(m.group('format'))
				if id not in table_dict:
					table_dict[id] = [event_str, format]
	#debug
	#for k, v in table_dict.items():
	#	 print (k, v[0], v[1])
	
	
				
		


# either the format str has 4,3,2,1 or none place holders
# Try and handle typeerrors as we go
# if there isn't a data place holder in the format str return the str itself
# however if the format str doesn't specify data format specifiers and 
# clients do have non-zero logged data, concat and return
def parse_dump_line(format_str, data_arr):
	try:
		pretty_str = format_str%(data_arr[0], data_arr[1], data_arr[2], data_arr[3])
	except TypeError:
		try:
			pretty_str = format_str%(data_arr[0], data_arr[1], data_arr[2])
		except TypeError:
			try:
				pretty_str = format_str%(data_arr[0], data_arr[1])
			except TypeError:
				try:
					pretty_str = format_str%(data_arr[0])
				except TypeError:
					if all([ d == 0 for d in data ]):
						pretty_str = format_str
					else:
						pretty_str = format_str + " " + str(data_arr)
						
	return pretty_str
#
# The parsing core
#

if __name__ == '__main__':
	#global cmb_log, log_idx
	# Get command line arguments
	parser = argparse.ArgumentParser()
	parser.add_argument('-f', '--file', help='path to ulog file', metavar='FILE', dest='filename', required=True)
	parser.add_argument("-p", "--pretty", action="store_true", dest="print_pretty_time", default=False, help="parse timestamp")
	parser.add_argument('-tbl', '--tblheader', help='path to tracer_event_tbl.h', metavar='FILE', dest='tbl_h', required=False)
	parser.add_argument("-r", "--raw", action="store_false", dest="parse_header", default=True, help="skip parsing tracer_event_tbl.h. Dump raw.")
	parser.add_argument("-d", "--dump-file", action="store_true", dest="dump_to_file", default=False, help="Dump this script's result to text file")
	args = parser.parse_args()
	
	
	
	
	
	if args.dump_to_file:
		sys.stdout = Logger()
	
	if args.parse_header:
		if not args.tbl_h:
			# user didn't provide build path. Find if we are in build root.
			if "aop_proc" in os.getcwd():
				#we are executing in aop tree. Look for tbl header in known locations
				
				#get the build root
				m = re.match(r'(.*)aop_proc', os.getcwd())
				print "\tBuild root: " + m.group(1)
				tbl_path = os.path.join(m.group(1), 'aop_proc/core/api/debugtrace/tracer_event_tbl.h')
				print "\tSearch for tbl_header: " + tbl_path
				parse_table_header(tbl_path)
			else:
				# we are not in build root. Build root must be passed as arguments
				print "Error: Not in AOP build tree please provide a path to tracer_event_tbl.h. Will dump raw data for now."
			
		
		else:
			tbl_path = args.tbl_h
			if os.path.exists(tbl_path):
					# must be unzipped build, can read map file directly
				parse_table_header(tbl_path)
			else:
				print "***Error: Couldn't find tbl_header at " + tbl_path

	
	try:
		f = open(args.filename)
		loglines = f.readlines()
		f.close()
	except:
		print 'Error loading log data from file:', sys.exc_info()[0]
		raise
	#import pdb; pdb.set_trace()
	if args.print_pretty_time:
		print "Time (s)" + "\t" + "delta" + "\t\t"  + "Data"
	else:
		print "TimeStamp" + "\t\t" + "delta" + "\t\t\t"  + "Data"
	for line in loglines:
		if dump_line_pattern.match(line): #only try and parse memory dump files
			bytestring = ''.join(map(lambda x: chr(int(x, 16)), line[2:line.rfind(',')].split(', ')))
			message = list(unpack('<%iL' % (len(bytestring)/4), bytestring))
			timestamp = (message[1] << 32) | message[0]
			id = message[2]
			data = message[3:]
			delta = timestamp - prev_timestamp
			prev_timestamp = timestamp
			
			if args.print_pretty_time:
				timestamp = '%f' % (timestamp * (1 / 19200000.0))
				delta = '%f' % (delta * (1 / 19200000.0))
			else:
				timestamp = '0x%0.16x' % (timestamp)
				delta = '0x%0.16x' % (delta)
			
			
			
			#find id in the table_dict
			if id in table_dict:
				pretty_log = parse_dump_line(str(table_dict[id][1]), data)
				#print timestamp + "\t" + str(table_dict[id][0]) + "\t\t"   + pretty_log
				print timestamp + "\t" + delta + "\t" + pretty_log
			else:
				print timestamp + "\t" + delta + "\t" + str(id) + "\t"   + str(data) + '*'
			
	
	print "\n*Note: Raw dump lines"
	