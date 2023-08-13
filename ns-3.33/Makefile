# Makefile wrapper for waf
#
.PHONY : all clean build 

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

all:
	./waf

# free free to change this part to suit your requirements
configure:
	./waf configure --enable-mpi --enable-examples --enable-tests

build:
	./waf build

install:
	# ./waf install --destdir=$(ROOT_DIR)/dist
	./waf install

clean:
	./waf clean

distclean:
	./waf distclean
