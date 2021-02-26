import re
import json
import argparse

class build_log_timing_parser(object):
    def __init__(self, log_file=None, output_file=None):

        if log_file is None or not isinstance(log_file, basestring):
            raise ValueError("log_file must be a basestring and not None.")

        if log_file is None or not isinstance(output_file, basestring):
            raise ValueError(" output_file be a basestring and not None.")

        self._output_file = output_file
        self._timing_report = {}
        self._timing_report['Images'] = {}

        self._current_component_stack = []


        with open(log_file, 'r') as fh:
            for line in fh:
                if "Elapsed time for" in line:
                    if "Image" in line:
                        image_name = re.search(r'Elapsed time for Analysis of Image (.*?) was', line).group(1)
                        self._timing_report['Images'][image_name] = {}
                        self._timing_report['Images'][image_name]['Components'] = [{x[0]: x[1]} for x in self._current_component_stack]
                        self._timing_report['Images'][image_name]['Total Time Elapsed'] = line.split()[-1][1:-1]
                        self.clear_current_components()
                    elif "Component" in line:
                        self._current_component_stack.append((re.search(r'Elapsed time for Analysis of Component (.*?) was', line).group(1), line.split()[-1][1:-1]))

    #TODO:Fix this
    def __repr__(self):
        return build_log_timing_parser()

    def __str__(self):
        return self.timing_report

    def clear_current_components(self):
        del self._current_component_stack[:]

    def generate_report(self):
        with open(self._output_file, 'w') as outfile:
            outfile.write(self.timing_report)

    @property
    def timing_report(self):
        return json.dumps(self._timing_report, sort_keys=True, indent=4)

    @timing_report.setter
    def timing_report(self, report):
        self._timing_report = report



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="This program will generate a timing report based on the outcome of a \
    build_log.txt file. \nBoth a path to a valid build_log.txt and a destination JSON output file are required.")

    parser.add_argument("-b", "--buildlog", dest="build_log_file",
                        help="The build log file you wish to produce a timing report for.", metavar="FILE", required=True)

    parser.add_argument("-o", "--outputfile", dest="output_file",
                        help="The destination file for the JSON output.", metavar="FILE", required=True)


    args = parser.parse_args()

    myparser = build_log_timing_parser(args.build_log_file, args.output_file)

    myparser.generate_report()

    exit(0)
