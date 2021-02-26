# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Apr 19, 2013

@author: hraghav

This module provides very basic classes that would be needed by other modules.
'''

import abc
from datetime import timedelta
import optparse
import os
import sys
import time


class CoreErrorCode(object):
    """ Class to provide enums that can be used to specify error codes """

    SUCCESS = 0
    GENERIC_FAILURE = 1
    INVALID_PARAMS = 2
    INVALID_CONFIG = 3
    INVALID_PATH = 4
    KEY_INT = 5


class CoreError(Exception):
    """
    Exception class to keep the code clean.
    Uses the CoreErrorCode enums to specify the error.
    """

    def __init__(self, errno, errorString):
        """
        Sets the error number and the srror string

        Parameters:
        1. errno (int): Integer chosen from one of CoreErrorCode values.
        2. errorString (str): String that clearly defines the error.
        """
        self.value = errno
        self.stringData = errorString

    def __str__(self):
        """ Returns the string representation of the CoreError object """
        return '({0}): {1}'.format(repr(self.value), self.stringData)


class DynamicToolStatus(object):
    """
    Class that holds information about the current status of the tool.
    Ideally there should only be one global instance of this class at runtime.
    The default instance is created in this module at the end of this class.
    """

    def __init__(self):
        """
        Initializes multiple attributes that specify the state of the program.

        Attributes:
        1. toolDir (str): Directory from where the tool was run.
        2. runningPython (bool): True - Running python as .py
                                 False - Running executable as .exe
        3. status (dict): Dictionary to allow addition of other attributes.
        """

        self.toolDir = ''
        self.status = dict()
        self._runningPython = True

    @property
    def runningPython(self):
        """ Returns if running in Python mode """
        return self._runningPython

    @runningPython.setter
    def runningPython(self, boolean):
        """ Sets runningPython variable and based on that sets the toolDir """
        if boolean is False:
            self._runningPython = False
            self.toolDir = os.path.dirname(sys.path[0])
        elif boolean is True:
            self._runningPython = True
            self.toolDir = sys.path[0]
        else:
            raise ValueError('boolean argument must be True or False')

""" Global instance variable of DynamicToolStatus """
dynamicToolStatus = DynamicToolStatus()
dynamicToolStatus.runningPython = True if sys.argv[0].endswith('.py') else False


class CoreTimer(object):
    """ Class that provides a way to get time elapsed between two points. """

    def __init__(self):
        """ Initializes the start time. """
        self.startTime = time.time()

    def start(self):
        """
        Sets the start time as current time and returns an ascii
        representation of the current time
        """
        self.startTime = time.time()
        return time.asctime()

    def get_start_time(self):
        return time.asctime(time.localtime(self.startTime))

    def elapsed(self):
        """
        Return:
        1. endTime - The current time.
        2. elapsedTime - The time that has elapsed from the last start.
        """
        return (time.asctime(), str(timedelta(seconds=(time.time() -
                                                       self.startTime))))

""" Global instance variable of to keep a time of the process run """
processTimer = CoreTimer()


class CoreOptionParser(optparse.OptionParser, object):
    """Provides basic functionality to parse and validate the command line
    arguments. Each tool's command line parser should derive from this abstract
    class and implement the abstract interface. The command line arguments are
    parsed as soon as the object is created and are available via
    :meth:`parsed_args`.

    The following command line arguments are added by default:

        -h, --help     show this help message
        -v, --verbose  enable more logging
        -d, --debug    enable debugging

    The help and the verbose args are handled by the parser automatically.
    The help message for the debug option is suppressed.
    """

    __metaclass__ = abc.ABCMeta

    def __init__(self, argv=None):
        # Initialize the base parser
        self.c_args = argv if argv is not None else sys.argv
        optparse.OptionParser.__init__(self, add_help_option=False,
                                       description=self.c_description,
                                       version=self.c_version,
                                       epilog=self.c_epilog)

        # Allow overrides to be specified
        self.overrides = self.init_overrides
        self.spec_overrides = self.init_spec_overrides

        # Add all the options and parse
        self._c_add_base_options()
        self.c_add_options()
        self._parsed_args = self.parse_args(self.c_args)[0]

        # Help option is provided. Extend the help message and print the help
        # message.
        if self.parsed_args.help or (self.help_on_no_args and len(self.c_args) == 1):
            extended_epilog = self.c_epilog_from_params
            if extended_epilog:
                sys.stdout.write(extended_epilog)
                sys.stdout.flush()
            else:
                self.print_help()
            sys.exit(0)

        # Validate the args
        self.c_validate()

        # Update the overrides
        self.c_update_overrides()

        # Update the verbosity.
        self.c_update_verbosity(self.parsed_args.verbose if self.parsed_args.quiet == 0 else
                                ((self.parsed_args.quiet + 1) * -1))

    @property
    def help_on_no_args(self):
        return True

    @property
    def parsed_args(self):
        """(namespace) Returns the parsed arguments from the command line"""
        return self._parsed_args

    @property
    def c_prog(self):
        """(str) Returns the name of the program. By default this is the name
        of the python file being executed.
        """
        return os.path.basename(self.c_args[0])

    @abc.abstractproperty
    def c_description(self):
        """(abstractproperty) (str) Returns the description of the program."""
        pass

    @abc.abstractproperty
    def c_version(self):
        """(abstractproperty) (str) Returns the version of the program."""
        pass

    @property
    def c_epilog(self):
        """(str) Returns the epilog for the program."""
        return ''

    @property
    def c_epilog_from_params(self):
        """(str) Returns additional epilog message, if any, generated based on
        the parsed command line arguments. The method can assume that the
        command line arguments are parsed at this point and can be accessed by
        :meth:`parsed_args`.
        """
        return ''

    @abc.abstractmethod
    def c_validate(self):
        """(abstractmethod) Validates the command line arguments that have been
        parsed. If there is any errors in the arguments, raises a RuntimeError.

        :raises: RuntimeError
        """
        pass

    @abc.abstractmethod
    def c_add_options(self):
        """(abstractmethod) Adds the command line arguments to the parser.
        The arguments should be added to "self" as :class:`CoreOptionParser`
        derives from OptionParser.

        ::

            self.add_option('-s', '--source', help='source directory')

        """
        pass

    @property
    def init_overrides(self):
        """(str) Returns a newly initialized overrides variable."""
        return None

    @property
    def init_spec_overrides(self):
        """(str) Returns a newly initialized spec_overrides variable."""
        return None

    @classmethod
    def mask_private_args(cls, args, priv_prefix):
        retval = []

        i = 0
        while (i < len(args)):
            arg = args[i]
            if arg.startswith('--' + priv_prefix):
                if '=' not in arg:
                    delim = ' '
                    i += 1
                else:
                    delim = '='
                    arg = arg.split(delim)[0]
                arg = arg + delim + '****'
            retval.append(arg)
            i += 1
        return retval

    def format_epilog(self, formatter):
        """This method is implemented to override the OptionParser's formatting
        of the epilog"""
        return self.epilog

    def c_update_overrides(self):
        """Updates the overrides object based on the overrides parsed from the
        command line."""
        pass

    def c_update_verbosity(self, level):
        """Updates the global logger's verbosity

        :param int level: Level to increase the verbosity to. Accepted values
            are - 0: INFO, 1: DEBUG, 2+: DEBUG2
        :raises: RuntimeError if the given level value is not supported.
        """
        from c_logging import logger
        verbosity_map = {
                            -3: logger.ERROR,
                            -2: logger.WARN,
                            -1: logger.NOTE,
                            0 : logger.INFO,
                            1 : logger.DEBUG,
                            2 : logger.DEBUG2
                        }
        if level < -3:
            logger.verbosity = logger.CRITICAL
        elif level > 2:
            logger.verbosity = logger.DEBUG3
        elif level in verbosity_map:
            logger.verbosity = verbosity_map[level]
        else:
            raise RuntimeError('Given verbosity level: "' + str(level) + '" is invalid')

    def _c_add_base_options(self):
        """Adds the basic/mandatory command line arguments to the parser."""
        self.add_option('-h', '--help', action='store_true', default=False,
                        help='show this help message')
        self.add_option('-v', '--verbose', action='count', default=0,
                        help='enable more logging.')
        self.add_option('-z', '--quiet', action='count', default=0,
                        help='enable less logging. This takes precedence over -v. '
                        'Errors & Warnings: Use -z, Errors only: Use -zz')
        self.add_option('-d', '--debug', action='store_true', default=False,
                        help=optparse.SUPPRESS_HELP)


class ValPolicy:
    """ Class that provides a way to select a policy for a validation and
    execute it. """

    IGNORE   = 1
    WARN     = 2
    ERROR    = 3

    def __init__(self, policy):
        """Selects the policy for validation"""
        if policy not in [self.IGNORE, self.WARN, self.ERROR]:
            raise RuntimeError('Invalid Validation Policy selected.')
        self.policy = policy

    def is_ignore(self):
        """Returns True if selected policy is to ignore validation"""
        return self.policy == self.IGNORE

    def run(self, error):
        """Runs the policy and prints error message per the policy."""
        if self.policy == self.WARN:
            from sectools.common.utils.c_logging import logger
            logger.warning(error)
        elif self.policy == self.ERROR:
            raise RuntimeError(error)
