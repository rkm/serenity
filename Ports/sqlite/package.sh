#!/bin/bash ../.port_include.sh

port=sqlite
version=3310100

files="https://sqlite.org/2020/sqlite-autoconf-${version}.tar.gz sqlite-autoconf-${version}.tar.gz"
auth_type="sha1"
# sha1: 0c30f5b22152a8166aa3bebb0f4bc1f3e9cc508b

workdir="sqlite-autoconf-${version}"

export CFLAGS="-Os -DSQLITE_THREADSAFE=0 -DHAVE_UTIME=1"
useconfigure=true
configopts="--disable-threadsafe"
#makeopts="-ltime"

