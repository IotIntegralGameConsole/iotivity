#!/bin/sh

which git
which unzip
which wget

cat<<EOF
# About to prepare your sources tree before building:
# $(pwd)
# warning: This helper script is not encouraged to use
#  so, Now you can interupt this script by pressing : ctrl+c
#  and use "scons" (which should do similar tasks the supported way).
#  but in some cases it not possible, so this script aims to workaround that.
# Reminder, it's not safe to access online resources at build time,
EOF
sleep 10

echo "# Fetch tinycbor sources if not available locally"
tinycbor_url='https://github.com/01org/tinycbor.git'
tinycbor_rev='v0.2.1'
if [ ! -e 'extlibs/tinycbor/tinycbor' ] ; then
    echo "warning: fetching online sources may not be reproductible" && sleep 10
    git clone -b "${tinycbor_rev}" "$tinycbor_url" extlibs/tinycbor/tinycbor
fi

echo "# Fetch mysql if not available locally"
sqlite_url='http://www.sqlite.org/2015/sqlite-amalgamation-3081101.zip'
sqlite_file="extlibs/sqlite3/sqlite3.c"
sqlite_archive=$(basename -- "$sqlite_url")
if [ ! -e 'extlibs/sqlite3/$sqlite_file' ] ; then
    echo "warning: fetching online sources may not be reproductible" && sleep 10
    cd extlibs/sqlite3 && wget -nc "$sqlite_url" && unzip "${sqlite_archive}" && mv */* .
    cd -
fi
