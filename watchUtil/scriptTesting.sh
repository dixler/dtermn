#!/bin/zsh
#todo ssh

cwd=/proc/${1}/cwd
lastDir=$(/bin/ls $cwd | sha256sum)

while [ true ]; do
    if [ "$curDir" != "$lastDir"  ]; then
        cwd=/proc/${1}/cwd
        lastDir=$(/bin/ls $cwd | sha256sum)
        clear
        printf "\033c"
        ls --color=yes --format=single-column $cwd
    fi
    cwd=/proc/${1}/cwd
    curDir=$(/bin/ls $cwd | sha256sum)
    sleep 0.1
done
