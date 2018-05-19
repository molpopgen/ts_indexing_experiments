#!/bin/bash

# Requires google's perftools, which 
# install into funky location on my laptop...
NGENS=300
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=vector_v4.profile ./vector_v4 10000 $NGENS

# Requires golang version of pprof from github.com/google/pprof
~/bin/pprof --pdf vector_v4 vector_v4.profile > vector_v4_profile.pdf

