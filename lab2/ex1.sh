#!/bin/bash

for i in $(echo $PATH | tr ':' '\n' | xargs -I {} find {} -maxdepth 1 -type f -executable 2>/dev/null | sort -ru); do
	echo $i
	for j in $(ldd $i | awk '{print $1}'); do
		echo -e "\t$j"
	done	
done
