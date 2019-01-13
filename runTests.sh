##*****************************************************************
## File:  runTests.sh
## Authors:   GONZÁLEZ VILLA, DANIEL
##            NAYA FORCANO, ABEL
##            GONZÁLEZ GORRADO, JESÚS ÁNGEL
##            GARCÍA DÍAZ, ÁLVARO
## Date:   Diciembre 2018-Enero 2019
##*****************************************************************

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


#variables
PWD=$(pwd)
UNAME=$(uname)


###################################


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
xterm -geometry +0+0 -T "Servidor" -e "cd $PWD && ./run server $server_port $server_privateport || sleep 5" &

sleep 5

echo -----Ejecutando subservidores-----
xterm -geometry -0-0 -T "Subservidor 1" -e "cd $PWD && ./run subserver 1 $subserver_ip1 $subserver_port1 $server_ip $server_privateport || sleep 5" &
xterm -geometry -0+0 -T "Subservidor 2" -e "cd $PWD && ./run subserver 2 $subserver_ip2 $subserver_port2 $server_ip $server_privateport || sleep 5" &
xterm -geometry +0-0 -T "Subservidor 3" -e "cd $PWD && ./run subserver 3 $subserver_ip3 $subserver_port3 $server_ip $server_privateport || sleep 5" &


if [ $# = 1 ]; then

	sleep 10

	echo -----Ejecutando test 1-----
	./run cliente $server_ip $server_port 1 example .
	
	echo -----Borrar notas-----
	./run cliente $server_ip $server_privateport 1 commands CLEAR
	
	echo -----Ejecutando test 2-----
	./run cliente $server_ip $server_port 1 t_tuplas 10

	echo -----Deteniendo-----
	./run cliente $server_ip $server_privateport 1 commands END

fi