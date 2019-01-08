#!/bin/bash
set -e #Exit on first error

#configuracion
server_ip=localhost
server_port=32000
server_privateport=32001

subserver_ip=(localhost localhost localhost)
subserver_port=(32010 32011 32012)

local_ip=local
local_port=0


#variables
PWD=$(pwd)
UNAME=$(uname)

#paths
binaries_dir=./build$UNAME/bin
cliente=$binaries_dir/cliente/cliente
server=$binaries_dir/server/server
subserver=$binaries_dir/subserver/subserver


###################################33


# prueba cliente local (sin servidor) con test supersencillo
#echo -----Compilando cliente-----
#gmake cliente
#
#echo -----Ejecutando cliente-----
#$cliente $local_ip $local_port 1 myexample .


##########################################

#prueba cliente+server+subserver
echo -----Compilando todo-----
gmake

echo -----Ejecutando servidor-----
xterm -geometry +0+0 -T "Servidor" -e $PWD/$server $server_port $server_privateport &

sleep 5

echo -----Ejecutando subservidores-----
xterm -geometry -0-0 -T "Subservidor 1" -e $PWD/$subserver 1 $subserver_ip[0] $subserver_port[0] $server_ip $server_privateport &
xterm -geometry -0+0 -T "Subservidor 2" -e $PWD/$subserver 2 $subserver_ip[1] $subserver_port[1] $server_ip $server_privateport &
xterm -geometry +0-0 -T "Subservidor 3" -e $PWD/$subserver 3 $subserver_ip[2] $subserver_port[2] $server_ip $server_privateport &

sleep 5

# tests
$cliente $server_ip $server_port  1 myexample .