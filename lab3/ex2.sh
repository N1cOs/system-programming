#!/bin/bash

ID_CMD=/usr/bin/id

if [[ $# -ne 1 ]]; then
    echo "Error: enter a username"
    exit 1
fi

USR_ID=$($ID_CMD -u $1 2>/dev/null)
if [[ $? -ne 0 ]]; then
    echo "Error: user $1 does not exist"
    exit 1
fi

echo "Files which user $1 can read:"
for f in $(ls -la | awk '$1 ~ /^-/ {print $9}'); do
    f_uid=$(ls -n $f | awk '{print $3}')
    f_gid=$(ls -n $f | awk '{print $4}')
    rights=(`ls -l $f | cut -d ' ' -f 1 | cut -c 2- | sed 's/./& /g'`)
    
    if [[ $f_uid -eq $USR_ID ]]; then
        if [[ ${rights[0]} == "r" ]]; then
            echo $f
        fi
        continue
    fi

    usrs=($(getent group | nawk -F ":" "\$3 == $f_gid && length(\$4) > 0 {print \$4}" | 
        tr ',' '\n' | xargs -I {} $ID_CMD -u {} 2>/dev/null))
    if $ID_CMD $f_gid 2>/dev/null 1>&2; then
        usrs+=($f_gid)
    fi
    
    for usr in ${usrs[@]}; do
        if [[ $usr -eq $USR_ID ]]; then
            if [[ ${rights[3]} == "r" ]]; then
                echo $f
            fi
            continue 2
        fi
    done

    usrs=$(getent group | nawk -F ":" "\$3 != $f_gid && length(\$4) > 0 {print \$4}" |
        tr ',' '\n' | uniq | xargs -I {} $ID_CMD -u {} 2>/dev/null)
    for usr in $usrs; do
        if [[ $usr -eq $USR_ID && ${rights[6]} == "r" ]]; then
            echo $f
            break
        fi
    done
done