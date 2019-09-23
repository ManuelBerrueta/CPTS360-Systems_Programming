#!/bin/sh

echo "===Shell Test\n====="

echo ">INPUT<\n"

input_file="${1:-/dev/stdin}"
cat "$input_file"

echo "Success\n"
echo "GoodBye!\n\n"