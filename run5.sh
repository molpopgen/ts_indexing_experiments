#!/bin/bash

for i in vector vector_v2 vector_v3 map avl_test
do
	/usr/bin/time -f "%e %M" ./$i 1000000 5
done
