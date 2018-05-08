#!/bin/bash

# Requires google's perftools, which 
# install into funky location on my laptop...
NGENS=300
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=avl.profile ./avl_test 10000 $NGENS

# Requires golang version of pprof from github.com/google/pprof
~/bin/pprof --pdf avl_test avl.profile > avl_profile.pdf

