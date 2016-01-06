#!/bin/sh
chmod 755 storeserv datahub transctrl coreproc maintainer eoaservice gnetmon procmon

# The storeserv is a log server, so it must be started at first!
./storeserv &
sleep 2
./datahub &
sleep 1
./transctrl &
sleep 1
./coreproc &
./maintainer &
sleep 1

# start extend modules
#./eoaservice &
#./gnetmon &

# start process monitor
sleep 1
./procmon 10 &
