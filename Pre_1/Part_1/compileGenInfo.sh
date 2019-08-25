#!/bin/bash

# This script compiles each .c source file, runs ls -l  and size on  a.out

for i in *.c;do
	cc -m32 -static "$i" && ls -l a.out && size a.out
done
