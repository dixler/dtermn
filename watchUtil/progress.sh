#!/bin/zsh

lastDir=$(/bin/progress | sha256sum)

while [ true ]; do
    if [ "$curDir" != "$lastDir"  ]; then
        lastDir=$(/bin/progress | /bin/sha256sum)
        clear
        printf "\033c"
        /bin/progress
    fi
    curDir=$(/bin/progress | /bin/sha256sum)
    sleep 0.1
done
