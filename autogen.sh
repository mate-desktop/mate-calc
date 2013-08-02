#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="mate-calc"
REQUIRED_AUTOMAKE_VERSION=1.7
REQUIRED_YELP_TOOLS_VERSION=3.1.1

(test -f $srcdir/configure.ac \
  && test -d $srcdir/src) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level mate-calc directory"
    exit 1
}

which mate-autogen || {
    echo "You need to install mate-common from the MATE git"
    exit 1
}

which yelp-build || {
    echo "You need to install yelp-tools" 
    exit 1
}

USE_MATE2_MACROS=1 USE_COMMON_DOC_BUILD=yes . mate-autogen
