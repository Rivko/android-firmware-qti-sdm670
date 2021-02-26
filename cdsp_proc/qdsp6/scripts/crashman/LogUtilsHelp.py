# This file contains the string variables for the somewhat interactive help 
# menu.

# Contains all the options supported by this tool
optionsString = \
"ip\n"\
"Next argument is used as one of the input files. You can specify\n"\
"as many input files as you want, if this option is supported by\n"\
"the command (Usage: ip file_name]\n"\
"\n"\
"op\n"\
"Next argument is used as the output file. It must have a supported\n"\
"extension (currently just ulog). If no extension is provided, ulog\n"\
"will be used by default. Only one output file can be specified in\n"\
"one command. All operations in this script suppport this command.\n"\
"\n"\
"start\n"\
"Will consider only log messages with time greater than or equal\n"\
"to the next argument. The time unit can be specified separately.\n"\
"\n"\
"end\n"\
"All the messages from input files with time stamp greater than\n"\
"this value will be filtered out. This in conjuction with start\n"\
"option can be used to restrict log messages to some particular\n"\
"time window.\n"\
"\n"\
"unit\n"\
"Specifies the time unit for the start and end time specified.\n"\
"Supported time units are sclk, usec (microsecond), msec \n"\
"(millisecond) and sec (second). \n"\
"\n"\
"from\n"\
"Indicates the time unit used by input files. Only one unit\n"\
"can be specified. It can have any one of the values 'unit'\n"\
"option can have. If interested files are using different time\n"\
"units, all can be converted to one unit using convert command\n"\
"(see below). All operation support this option. Default is\n"\
"sclk.\n"\
"\n"\
"to\n"\
"Time unit that will be used to write output. It can be either\n"\
", usec, msec or sec. Default is tick (Based on 19.2 Mhz). Supported by all\n"\
"operations.\n"\
"\n"\
"timediff\n"\
"Prints time difference between two successive message at the end of each\n"\
"message. Next argument is base of time difference. It could be either 'dec'\n"\
"or 'hex'. If any other value is used, this entire option is ignored. Supported\n"\
"operations are merge, window, time, event and cluster.\n"\
"\n"\
"withsrc\n"\
"Indicates the file name from which each output message has\n"\
"been retrieved at the end of message.\n"\
"\n"\
"withlogmarker\n"\
"In the output explicitly adds message when a particular input\n"\
"log starts or ends.\n"\
"\n"\
"If  is used as either input or output time unit, value is assumed\n"\
"to be a HEXADECIMAL number.\n"

# list of operations this tool performs with brief description
briefDescriptionDictionary = {
"merge"   : "Merges all input files into one file",
"convert" : "Converts the input file from one time unit to other",
"window"  : "Searches for all the events happened in a given time window",
"time"    : "Finds out what all events happened at the specified time",
"event"   : "Lists all the log messages containing input event",
"cluster" : "Outputs concurrent events i.e. happened at the same time",
"sort"    : "Sorts the input file in ascending/descending order"
}

# list of operations this tool performs with detailed description
detailDescriptionDictionary = {
"merge" : "Merges log messages from all the input files specified. If two\n"\
          "events from different logs occurred at the same time, it will\n"\
          "give priority to the message from the file specified earlier.\n"\
          "Supported options: ip, op, start, end, unit, from, to, withsrc,\n"\
          "                   withlogmarker\n"\
          "Example: merge [ip file1] [ip file2] [op file3] [to sec] [timediff hex]\n",

"convert" :  "Converts file with time stamp from one unit to a different unit.\n"\
             "Since all other operations requires same time unit for the input\n"\
             "files, this can be used for preprocessing if required. May be\n"\
             "affected by the fact of float inaccuracy. This operation\n"\
             "has different syntax than other just for simplicity.\n"\
             "Example: convert <input_file> <from_unit> <to_unit> <output_file>\n",

"window" :   "Alias of merge operation. Good to use if we are interested in\n"\
             "some time window instead of all messages. It supports the same\n"\
             "options as merge.\n",

"time"   :   "Find out all log messages with the given time stamp from all\n"\
             "specified input files. Time can be specified in sclk, usec,\n"\
             "msec or sec. By default sclk is assumed. Whenever sclk is used,\n"\
             "it assumes the input time as a HEXADECIMAL number. Does not \n"\
             "support 'start' and 'end' options.\n"\
             "Example: time 89ABCD [unit sclk] [ip file1 ip] [file2 op Output] \n"\
             "          time 123.45 [unit sec]  [ip file1] [op Output] [from sclk] [to sec]\n",

"event"  :   "Searches for the log messages that contain the event specified\n"\
             "as argument (case sensitive). \n"\
             "Example: event \"Mode entering\" [start 100ABC] [end 110000] [op Output] [withsrc]\n"\
             "         Outputs all the messages from all input files that contain\n"\
             "         \"Mode entering\" with timestamp in between 0x100ABC and \n"\
             "         0x110000 sclk. Each message in output will contain the\n"\
             "         original source file from which it came.\n",

"cluster" :  "Outputs the concurrent log messages (i.e. happened at the same\n"\
             "time) from input files. We can restrict the number of logs msgs\n"\
             "by specifying start and end time. \n"\
             "Example: cluster [ip file1] [ip file2] [ip file3] [op Output]\n",

"sort" : "Sorts the input file in either ascending or descending order of\n"\
         "timestamps. If no option specified, it sorts the input log in\n"\
         "ascending order. You can use 'dsc' option, if you want output to be\n"\
         "sorted in descending order. It should be noted that in most cases,\n"\
         "if input logs are not in some order, something went wrong with the\n"\
         "system. \n"\
         "Example: sort <input_file> <output_file> [asc|dsc]\n"
}
