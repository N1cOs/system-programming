#!/bin/bash

ID_CMD=/usr/bin/id

if [[ $# -ne 1 ]]; then
    echo "Error: enter a filename"
    exit 1
fi

if [[ ! -e $1 ]]; then
    echo "Error: file $1 does not exist"
    exit 1
fi

F_UID=$(ls -n $1 | awk '{print $3}')
F_GID=$(ls -n $1 | awk '{print $4}')
RIGHTS=(`ls -l $1 | cut -d ' ' -f 1 | cut -c 2- | sed 's/./& /g'`)

declare -a CAN_WRT CANT_WRT
FLAG="f"

if [[ "${RIGHTS[1]}" == "w" ]]; then
    CAN_WRT[$F_UID]=$(getent passwd $F_UID | cut -d ':' -f 1)
else
    CANT_WRT[$F_UID]=$FLAG
fi

USRS=$(getent group $F_GID | cut -d ':' -f 4- | tr ',' '\n' | uniq)
if [[ "${RIGHTS[4]}" == "w" ]]; then
    for usr in $USRS; do
        CAN_WRT[$($ID_CMD -u $usr)]=$usr
    done
    
    name=$(getent passwd $F_GID | cut -d ':' -f 1)
    if [[ -n $name && $CANT_WRT[$F_GID] != $FLAG ]]; then
        CAN_WRT[$F_GID]=$name
    fi
else
    for usr in $USRS; do
        CANT_WRT[$($ID_CMD -u $usr)]=$FLAG
    done
fi

if [[ "${RIGHTS[7]}" == "w" ]]; then
    for usr in $(getent group | nawk -F ":" "\$3 ~! /^$F_GID/ && length(\$4) > 0 {print \$4}" | tr ',' '\n' | uniq); do
                gid=$($ID_CMD -u $usr 2>/dev/null)
        if [[ $? -eq 0 && $CANT_WRT[$gid] != $FLAG ]]; then
            CAN_WRT[gid]=$usr
        fi
    done
fi

if [[ ${#CAN_WRT[@]} -gt 0 ]]; then
    echo "Users who can write into $1:"
    for u in ${CAN_WRT[@]}; do
        echo $u
    done
else
    echo "Only root can write into $1"
fi
