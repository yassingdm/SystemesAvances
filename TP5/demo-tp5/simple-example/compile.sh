#!/bin/bash

# heptagon compilation
~/bin/heptc-modified -memalloc externc.epi
~/bin/heptc-modified -target lopht -targetnode main -memalloc try.ept
# get rid of "Externc__" prefixes in .gc file
cat try.gc | sed s/Externc__//g >try-renamed.gc

# cleanup after heptagon compilation
#rm -f externc.epci try.epci try.mls try.gc *~ *.obc 

# TODO: check whether the dependencies induced by AT
# annotations are respected

lopht.partitioned -debug -t1042 \
		  -tick 10000 \
		  try-renamed.gc mppa-archi.gc try.analysis \
		  -output-folder gen-t1042-threads \
		  -forcemif \
		  -reservation-overhead 0 \
		  -limit-thread-size \
		  -no-interf-overhead \
		  -no-context-creation \
		  -no-api-sync-overhead \
		  -no-api-cache-overhead \
		  -faster-first \


