#!/bin/bash

if [[ $1 =~ ^\+?[0-9]+$ ]]; then
	ps -e -o user | sed '1d' | sort | uniq -c | awk "\$1 > $1 {print \$2}" | sort
else
	echo "First argument should be a positive number"
fi
