from os import environ; from os.path import join, abspath; import sys; sys.path = [ join(abspath(environ.get('SCONS_ROOT','..')))] + sys.path; import SCons.Script; SCons.Script.main()

