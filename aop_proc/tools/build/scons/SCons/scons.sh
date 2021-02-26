#!/bin/bash

SCONS_ERR_LEVEL=""

PYTHONCMD="$(which python)"
export SCONS_ROOT="$PWD/../tools/build/scons"
export PYTHONPATH=$PYTHONCMD/..

if [ -z $PYTHONCMD ]; then
	echo "Python is not present in the path"
else
	echo "Python Path: $PYTHONCMD"
fi

#python -c "from os import environ; print environ.get('SCONS_ROOT','..')"
python -c "from os import environ; from os.path import join, abspath; import sys; sys.path = [ join(abspath(environ.get('SCONS_ROOT','..')))] + sys.path; import SCons.Script; SCons.Script.main()" $@

#scons %*


