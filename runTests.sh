#!/bin/bash
set -e #Exit on first error

#configuracion
server_ip=localhost
server_port=32500
server_privateport=32501

subserver_ip1=localhost
subserver_ip2=localhost
subserver_ip3=localhost
subserver_port1=32510
subserver_port2=32511
subserver_port3=32512

local_ip=local
local_port=0


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
xterm -geometry +0+0 -T "Servidor" -e "cd $PWD && ./run server $server_port $subserver_ip1 $subserver_port1 $subserver_ip2 $subserver_port2 $subserver_ip3 $subserver_port3 || sleep 5" &

sleep 5

echo -----Ejecutando subservidores-----
xterm -geometry -0-0 -T "Subservidor 1" -e "cd $PWD && ./run subserver $subserver_port1 || sleep 5" &
xterm -geometry -0+0 -T "Subservidor 2" -e "cd $PWD && ./run subserver $subserver_port2 || sleep 5" &
xterm -geometry +0-0 -T "Subservidor 3" -e "cd $PWD && ./run subserver $subserver_port3 || sleep 5" &

sleep 5
