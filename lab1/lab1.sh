#! /bin/bash

FILE=""
TERMINAL_WIDTH=$(tput cols)
ERR_PATH="$HOME/lab1_err"

print_info()
{
	echo "Enter 1 to find out current working directory"
 	echo "Enter 2 to change directory"
 	echo "Enter 3 to find out current date"
 	echo "Enter 4 to get the file content"
 	echo "Enter 5 to copying the file"
	echo "Enter 6 to create directory"
	echo "Enter 7 for getting rights list of the current directory"
 	echo "Enter 8 to exit"
}	

get_file_name()
{
	FILE=""
	while [ -z $FILE ]; do
		read -r FILE
		if [ -z $FILE ]; then
			echo "Enter non-blank name"
		fi
	done
}

print_hl()
{

	printf "%${TERMINAL_WIDTH}s\n" | tr " " "-"
}


while :; do
	print_info
	read OPTION || break
	echo ""
	case $OPTION in
		"1")
			echo "Your working directory is `pwd`"
			;;
		"2")
			echo "Enter destination directory"
			get_file_name
			cd $FILE 2>&1 | tee -a $ERR_PATH
			;;
		"3")
			echo "Current date is $(date +"%a %b %e %R %Z %G")"
			;;
		"4")
			echo "Enter file name"
			get_file_name
			print_hl
			cat $FILE 2>/dev/null || cat $FILE 2>&1 | tee -a $ERR_PATH
			print_hl
			;;
		"5")
			echo "Enter a source filename"
			get_file_name
			SOURCE_FILE=$FILE
			echo "Enter a destination filename"
			get_file_name
			cp $SOURCE_FILE $FILE 2>&1 | tee -a $ERR_PATH
			;;
		"6")
			echo "Enter a directory name"
			get_file_name
			mkdir "${FILE}" 2>&1 | tee -a $ERR_PATH
			;;
		"7")
			DIR_INFO=$(ls -ld)
			USER=`echo "$DIR_INFO" | cut -d ' ' -f 3`						
			GROUP=`echo "$DIR_INFO" | cut -d ' ' -f 4`	
			RIGHTS_INFO=`echo "$DIR_INFO" | cut -d ' ' -f 1`

			i=1
			while [ $i -lt 10 ]; do 
					rights+=(${RIGHTS_INFO:$i:1}) 
					i=$((i + 1))
			done

			if [ "${rights[0]}" = "r" ]; then
				can_read+=($USER)
			fi	   
			
			if [ "${rights[1]}" = "w" ]; then
				can_write+=($USER)
			fi
			
			if [ "${rights[3]}" = "r" ]; then
				can_read+=($(getent group $GROUP | cut -d ':' -f 4 | tr ',' ' '))
				
				id $GROUP > /dev/null 2>&1
				if [ $? -eq 0 ]; then
					can_read+=($GROUP)
				fi
			fi

			if [ "${rights[4]}" = "w" ]; then
				can_write+=($(getent group $GROUP | cut -d ':' -f 4 | tr ',' ' '))
				
				id $GROUP > /dev/null 2>&1
				if [ $? -eq 0 ]; then
					can_write+=($GROUP)
				fi
			fi

			if [ "${rights[6]}" = "r" ]; then
					can_read+=($(getent group | awk -F ':' "\$1 !~ /^($USER|$GROUP)/ && length(\$4) > 0 {gsub(\",\", \" \" ,\$4); print \$4}"))
			fi

			if [ "${rights[7]}" = "w" ]; then
					can_write+=($(getent group | awk -F ':' "\$1 !~ /^($USER|$GROUP)/ && length(\$4) > 0 {gsub(\",\", \" \" ,\$4); print \$4}"))
			fi

			print_hl
			echo "Users who can read the current directory:"
			print_hl
			echo "${can_read[@]}" | tr ' ' '\n' | sort -u | awk '{cmd = "id -u " $0; cmd | getline r; if (r >= 1000) print $0}'
			print_hl
			echo "Users who can write into the current directory:"
			print_hl
			echo "${can_write[@]}" | tr ' ' '\n' | sort -u | awk '{cmd = "id -u " $0; cmd | getline r; if (r >= 1000) print $0}'
			print_hl

			unset can_read
			unset can_write
			;;
		"8")
			break;
			;;
		*)
			echo "Invalid input"
			;;
	esac
	echo ""
done