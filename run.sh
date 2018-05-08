#!/bin/bash

# Requires google's perftools, which 
# install into funky location on my laptop...
NGENS=100
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=vector.profile ./vector 10000 $NGENS
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=vector_v2.profile ./vector_v2 10000 $NGENS
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=map.profile ./map 10000 $NGENS

# Requires golang version of pprof from github.com/google/pprof
~/bin/pprof --pdf vector vector.profile > vector_profile.pdf
~/bin/pprof --pdf vector_v2 vector_v2.profile > vector_v2_profile.pdf
~/bin/pprof --pdf map map.profile > map_profile.pdf
