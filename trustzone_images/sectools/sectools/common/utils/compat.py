"""
compat.py

Contains code to allow the codebase to run under either python2
or python3.  Augments future and six.

Deals with imports that differ between python2 and python3,
and handling of bytes and ints wrt byte strings.

"""
from six import int2byte

try:
    # py2
    # noinspection PyCompatibility
    from StringIO import StringIO
except ImportError:
    # py3
    from io import StringIO

try:
    # py2
    # noinspection PyCompatibility
    from cStringIO import StringIO as cStringIO
except ImportError:
    # py3
    from io import BytesIO as cStringIO

try:
    # py2
    import Queue
except ImportError:
    # py3
    # noinspection PyPep8Naming
    import queue as Queue

try:
    # py2
    import ConfigParser
except ImportError:
    # py3
    # noinspection PyPep8Naming
    import configparser as ConfigParser


def ensure_byte(item):
    """
    Ensure item resolves as a single char byte string

    Needed for making code that examines bytes in a string one-by-one
    run correctly in python2 and python3.

    Consider the following code:
     1     x = b'\xde\xad\xbe\xef'
     2     if x[1] == b'\xad':
     3         print("found it")

     This code works perfectly in python2 but fails in python3
     because in python3, indexing a byte string returns an int,
     not a single byte byte string. To make this code run in both
     python2 and python3 we would change line 2:
     2     if ensure_byte(x[1]) == b'\xad':
    """
    if isinstance(item, int):
        return int2byte(item)
    assert isinstance(item, bytes)
    return item


def ensure_int(item):
    """
    Ensures byte indexed from a byte string is resolved as an integer

    python3 does this automatically

    python2 returns a str for the indexed item, which for a byte string
    is the char byte representation of the integer we want.
    """
    if not isinstance(item, int):
        return int(item.encode('hex'), 16)
    return item
