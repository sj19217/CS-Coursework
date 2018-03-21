"""
Utility functions.
"""

import logging

def get_size_of_type(type_: str):
    """Returns the size in bytes of the given type"""
    if type_ in ("char", "uchar"):
        return 1
    elif type_ in ("short", "ushort"):
        return 2
    elif type_ in ("int", "uint", "float"):
        return 4
    else:
        logging.error("Unknown type {}".format(type_))