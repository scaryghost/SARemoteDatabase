#!/usr/bin/env sh

# This script is an example of how to start the server with minimum configuration.  Run the script 
# with -h or --help to see a complete list of options

dataUrl=share/etc/saremotedb.sqlite
tcpPort=12345
password=server

command="./saremotedatabase --data-url $dataUrl --tcp-port $tcpPort --server-pw $password $*"

echo $command
exec $command
